#include "ProGspChannel.h"
#include "../ClientChannel.h"
#include "../ClientSection.h"
#include "GSP/GSPAnalyer.h"
#include "Log.h"
#include "GSPMemory.h"


using namespace GSP;
using namespace GSP::RTP;

#define H_LOG  m_pParent->m_pLog


namespace GSP
{
#define TIMER_ID_KEEPALIVE 10
#define TIMER_ID_SEND_KEEPALIVE 11
#define TIMER_ID_ASYNC_WAKEUP 12

#ifdef _DEBUG
	class _CTester
	{
	public :
		GSAtomicInter &m_iTester; 
		_CTester( GSAtomicInter &iTester)
			: m_iTester(iTester)
		{
			GS_ASSERT(1==AtomicInterInc(m_iTester) );

		}
		~_CTester(void)
		{
			AtomicInterDec(m_iTester);
		}
	};
#endif


	/*
	********************************************************************
	��ע��
	����    :    CSyncWaiter
	����    :    zouyx
	����ʱ��:    2011/8/2 9:45
	������  :    �첽���͵ĵȴ���
	*********************************************************************
	*/

	class CGspSyncWaiter : public CRefObject
	{

	public :		
		CGSCondEx m_csCond;  //ͬ���ź�
		CGSMutex m_csMutex;  //ͬ����
		INT32 m_iWaitTagResponse; //�ȴ��Ļظ��������
		EnumGSPCommandID m_eWaitCommandID; //�ȴ�������ID
		UINT32 m_iTimeouts; //��ʱʱ�䣬 ����
		UINT16 m_bWaitSignal; //�Ƿ���ʹ���ڵȴ��ź�
		UINT64 m_iStartTm; //��ʱ��ʼ��ʱʱ��
		CGspChannel *m_pWaiter; // �ȴ��Ķ���
		BOOL m_bAsync; //�Ƿ��첽
		StruGSPCmdCtrl m_stCtrl; //Ctrl ����Ļ���
		EnumErrno m_eErrno; //����Ĵ����
		BOOL m_bOpen; //Open ���ؽ���Ļ���
		BOOL m_bWakeup; //�Ƿ��Ѿ�����
		BOOL m_bDelay; //�Ƿ���ʱ����

		INT32 m_iWakeupParentTag; //�ȴ��Ļظ�
		EnumGSPCommandID m_iWakeupParentCmdID; //�ȴ�������ID


		/*
		 *********************************************
		 Function : Create
		 DateTime : 2012/4/24 9:48
		 Description :  ����CGspSyncWaiter ����
		 Input : pWaiter �ȴ���Channel ����
		 Input : eWaitCommandID �ȴ�������ID
		 Input : iCmdWaitTag �ȴ��Ļظ��������
		 Input : bAsync �Ƿ��첽ͬ��
		 Input : iTimeouts ��ʱʱ��
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		static CGspSyncWaiter *Create(CGspChannel *pWaiter,
										EnumGSPCommandID eWaitCommandID,
										INT32 iCmdWaitTag, 
										BOOL bAsync,
										UINT32 iTimeouts )
		{
			CGspSyncWaiter *p= new CGspSyncWaiter();
			if( p )
			{
				
				p->m_pWaiter = pWaiter;
				p->m_eWaitCommandID = eWaitCommandID;
				p->m_iWaitTagResponse = iCmdWaitTag;
				p->m_iTimeouts = iTimeouts;
				p->m_bAsync = bAsync;
			}
			return p;
		}


		EnumErrno WaitResult( INT iTimeouts )
		{
			CGSAutoMutex locker(&m_csMutex);
			m_iStartTm = DoGetTickCount();
			if( !m_bAsync )
			{		
				m_bWaitSignal = TRUE;
				if( iTimeouts>0 && !m_bWakeup )
				{					
					m_csCond.WaitTimeout(&m_csMutex, iTimeouts);
				}
				m_bWakeup = TRUE; //�����ٻ���
			}
			m_bWaitSignal = FALSE;

			return m_eErrno;
		}

		EnumErrno Result(void)
		{
			return m_eErrno;
		}

		void Wakeup( EnumErrno eErrno )
		{   
			m_csMutex.Lock();
			if( m_bWakeup )
			{
				m_csMutex.Unlock();
				return;
			}
			
			if(m_eErrno==eERRNO_ENONE)
			{
				m_eErrno = eErrno;
			}

			if( m_bWaitSignal )
			{					
				m_csCond.Signal();	
				m_csMutex.Unlock();
			} 
			else if(  m_bAsync && !m_bDelay )
			{				
				m_bWakeup = TRUE;
				m_csMutex.Unlock();
				m_pWaiter->OnAsyncWaiterEvent( this);					
			}
			else
			{
				m_csMutex.Unlock();
			}
		}

		
		BOOL TestTimeout(void)
		{
			CGSAutoMutex locker(&m_csMutex);			
			UINT64 iCur = DoGetTickCount();

			if( !m_bAsync || m_bDelay )
			{
				//ͬ�������ʱ
				return FALSE;
			}


			if( m_iStartTm>iCur )
			{
				//�¼��ı���
				m_iStartTm = iCur;
				return FALSE;
			}
			if( (m_iStartTm+m_iTimeouts) > iCur )
			{
				return FALSE;
			}

			return TRUE;

		}

		void DelayFalse(void)
		{
			m_iStartTm = DoGetTickCount()+40;
			m_bDelay = FALSE;
		}



	private :
		CGspSyncWaiter(void)
			:CRefObject()
			,m_csCond()
			,m_csMutex()
		{
		
			m_iStartTm = DoGetTickCount()+40;
			m_iTimeouts = 2000;
			m_bWaitSignal = FALSE;	
			m_iWaitTagResponse = -1;
			m_eErrno = eERRNO_ENONE;
			bzero( &m_stCtrl, sizeof(m_stCtrl) );
			m_bOpen =  FALSE;
			m_bWakeup = FALSE;
			m_bDelay = TRUE;
			m_iWakeupParentTag=-1;
			m_iWakeupParentCmdID = GSP_CMD_ID_NONE;
		}

		virtual ~CGspSyncWaiter(void)
		{   
			m_eErrno = eERRNO_SYS_ETIMEOUT;
			m_csMutex.Lock();
			m_bAsync = TRUE;
			m_bWakeup = TRUE;
			m_csCond.Signal();	
			m_csMutex.Unlock();			
			while(m_bWaitSignal )
			{
				MSLEEP(10);
			}
			m_csMutex.Lock();
			m_csMutex.Unlock();
		}

	};


} //end namespace GSP



static void _FreeAsyncWaiter( CGspSyncWaiter *p )
{
	// �ͷ� m_csAsyncCmdList ��������
	
	SAFE_DESTROY_REFOBJECT(&p);
}

static void _FreeWaitSendQueueMember( CGspProFrame *p )
{

	// �ͷ� m_csTcpWaitSendQueue ��������
	SAFE_DESTROY_REFOBJECT(&p);
}

static void _FreeCommandTaskMember( CGspProFrame *p )
{
	//�ͷ� m_csCommandTask �����������
	SAFE_DESTROY_REFOBJECT(&p);
}

CGspChannel::CGspChannel(CClientChannel *pParent)
:CIProChannel()
,m_pParent(pParent)
,m_iAutoID(pParent->m_iAutoID)
{
	m_csAsyncCmdList.SetFreeCallback((FuncPtrFree)_FreeAsyncWaiter);
	m_csTcpWaitSendQueue.SetFreeCallback((FuncPtrFree)_FreeWaitSendQueueMember);
	m_csCommandTask.SetFreedTaskDataFunction((FuncPtrFree)_FreeCommandTaskMember);
	m_csCommandTask.SetMaxWaitTask(100);

	m_pTcpSocket = NULL;

	m_stProCmdHeader.iSubChn = 1;
	m_stProCmdHeader.iVersion = GSP_VERSION;
	m_stProCmdHeader.iCRC = CRC_TYPE_NONE;
	m_stProCmdHeader.iDataType =  GSP_PACKET_TYPE_CMD;
	m_iCmdTagSequence = 0;
	m_bTcpSending = FALSE;    //��׼TCP �Ƿ����ڷ���
	m_bWaitSendFinish = FALSE;


	m_pTcpDecoder = NULL; //TCP ��Э�����



	m_iMaxKeepaliveTimeouts = 1;

	m_iKeepalivePlugs = 0;

	


	m_iGspVersion = -1;

	m_iSkTest = 0;

	m_eGspStreamTranMode = pParent->m_pSection->GetGspStreamTransMode();

	m_pTcpStreamReader = NULL;
	m_pRtpUdpReader = NULL;
	m_pStreamTcpDecoder = NULL;

	
}

CGspChannel::~CGspChannel(void)
{


	WakeupAllAsyncWaiter( eERRNO_SYS_ETIMEOUT);

	m_bWaitSendFinish = FALSE;
	m_csSendKeepaliveTimer.Stop();
	m_csAsyncWaiterTimer.Stop();
	m_csKeepaliveTestTimer.Stop();
	m_csCommandTask.Uninit();

	if( m_pTcpSocket )
	{
		m_pTcpSocket->Release();
		m_pTcpSocket = NULL;
	}

	if( m_pTcpStreamReader )
	{
		m_pTcpStreamReader->Release();
		m_pTcpStreamReader = NULL;
	}

	if( m_pRtpUdpReader )
	{
		m_pRtpUdpReader->Stop();
		delete m_pRtpUdpReader;
		m_pRtpUdpReader = NULL;
	}

	if(m_pTcpDecoder )
	{
		delete m_pTcpDecoder;
		m_pTcpDecoder = NULL;
	}

	if(m_pStreamTcpDecoder )
	{
		delete m_pStreamTcpDecoder;
		m_pStreamTcpDecoder = NULL;
	}
	
	
//	OutputDebugString( "Destory Channel...\n");

}

void CGspChannel::DestoryBefore(void)
{

	WakeupAllAsyncWaiter( eERRNO_SYS_ETIMEOUT);
	
	m_csCommandTask.Uninit();


	m_csSendKeepaliveTimer.Stop();
	m_csAsyncWaiterTimer.Stop();
	m_csKeepaliveTestTimer.Stop();
	


	INT iTrys = 1000;
	while( m_bTcpSending && m_bWaitSendFinish  && iTrys-->0)
	{
		//�ȴ��������
		MSLEEP(10);
	}
	GS_ASSERT(iTrys>0);

	if( m_pTcpSocket )
	{
		m_pTcpSocket->Disconnect();
	}	

	if( m_pTcpStreamReader )
	{
		m_pTcpStreamReader->Disconnect();
	}

	if( m_pRtpUdpReader )
	{
		m_pRtpUdpReader->Stop();
	}


}


void CGspChannel::WakeupAllAsyncWaiter(EnumErrno eErrno)
{
	m_csAsyncWRMutex.LockWrite();
	CList::CIterator< CGspSyncWaiter*>  csIt;
	for( csIt = m_csAsyncCmdList.First< CGspSyncWaiter*>(); csIt.IsOk(); csIt.Next() )
	{
		csIt.Data()->Wakeup(eErrno);
	}		 
	m_csAsyncWRMutex.UnlockWrite();
}

EnumErrno CGspChannel::FlowCtrl( BOOL bStart )
{
	StruGSPCmdCtrl stCtrl;
	bzero(&stCtrl, sizeof(stCtrl));
	stCtrl.iArgs1 = bStart;
	stCtrl.iCtrlID = GSP_CTRL_FLOWCTRL;
	return SendCommand(GSP_CMD_ID_CTRL,&stCtrl, sizeof(stCtrl));
}

EnumErrno CGspChannel::Open(const CUri &csUri, BOOL bAsync, INT iTimeouts)
{

	CTCPClientSocket *pSocket;
	EnumErrno eErrno = eERRNO_SUCCESS;

	

	MY_LOG_NOTICE(H_LOG, _GSTX("CGspChannel(%u) Open ��URI(%s)\n"),
		m_iAutoID, csUri.GetURI());

	m_pTcpDecoder = new CGspTcpDecoder();

	if( !m_pTcpDecoder )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ����Э���������ʧ��.\n"), m_iAutoID );
		return eERRNO_SYS_ENMEM;
	}

	if( m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_TCP )
	{
		m_pStreamTcpDecoder = new CGspTcpDecoder();
		if( !m_pStreamTcpDecoder )
		{

		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ����TCP��Э���������ʧ��.\n"), m_iAutoID );
		return eERRNO_SYS_ENMEM;
		}
		m_pTcpStreamReader = m_pTcpStreamReader->Create();
		if( !m_pTcpStreamReader )
		{
			GS_ASSERT(0);
			MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ������Э��TCP Socket����ʧ��.\n"), m_iAutoID );
			return eERRNO_SYS_ENMEM;
		}
	}
	else if(  m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_RTP_UDP  )
	{
		m_pRtpUdpReader = m_pRtpUdpReader->Create(NULL, TRUE, FALSE);
		if( !m_pRtpUdpReader )
		{
			GS_ASSERT(0);
			MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ����RTP UDP Reader����ʧ��.\n"), m_iAutoID );
			return eERRNO_SYS_ENMEM;
		}
	}

	if( !m_csCommandTask.Init(this, 
			(FuncPtrObjThreadPoolEvent)&CGspChannel::OnCommandTaskPoolEvent, 
		1, FALSE) )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ��ʼ���̳߳�ʧ��.\n"), m_iAutoID );
		return eERRNO_SYS_ETKP;
	}

	m_csSendKeepaliveTimer.Init(this, (FuncPtrTimerCallback)&CGspChannel::OnTimerEvent,
								TIMER_ID_SEND_KEEPALIVE, m_iMaxKeepaliveTimeouts*1000,FALSE);
	if( !m_csSendKeepaliveTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��A.\n"), m_iAutoID );
		return eERRNO_SYS_EBUSY;
	}

	m_csAsyncWaiterTimer.Init(this, (FuncPtrTimerCallback)&CGspChannel::OnTimerEvent,
		TIMER_ID_ASYNC_WAKEUP, 10,FALSE);
	if( !m_csAsyncWaiterTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��B.\n"), m_iAutoID );
		return eERRNO_SYS_EBUSY;
	}

	m_csKeepaliveTestTimer.Init(this, (FuncPtrTimerCallback)&CGspChannel::OnTimerEvent,
		TIMER_ID_KEEPALIVE, 1000,FALSE);
	if( !m_csAsyncWaiterTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��C.\n"), m_iAutoID );
		return eERRNO_SYS_EBUSY;
	}


	if( iTimeouts<10 )
	{
		iTimeouts = 10;
	}

	pSocket = CTCPClientSocket::Create();
	if( !pSocket )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ����Socket ����ʧ��.\n") , m_iAutoID );
		return eERRNO_SYS_ENMEM;
	}
	eErrno = pSocket->Connect(csUri.GetPort(),csUri.GetHost());
	if( eERRNO_SUCCESS != eErrno )
	{
		//GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ���ӷ�����ʧ��.\n"), m_iAutoID );
							
		pSocket->Release();
		return eERRNO_NET_EDISCNN;
	}
	pSocket->SetListener(this, (FuncPtrSocketEvent)&CGspChannel::OnTcpSocketEvent);
	m_pTcpSocket = pSocket;
	eErrno = pSocket->AsyncRcv(TRUE);

	if( eERRNO_SUCCESS != eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ����������¼�ʧ��.\n"), m_iAutoID );
		return eERRNO_NET_EREVT;
	}
	
	 StruGSPCmdRequest *pRequest;
	 pRequest = (StruGSPCmdRequest *)CMemoryPool::Malloc(1400);
	 bzero(pRequest, 1400);

	 pRequest->iMagic = GSP_MAGIC;
	 pRequest->iTransType = m_pParent->m_eTranModel;
	 pRequest->iTransMode = m_eGspStreamTranMode;
	 GS_SNPRINTF((char*)pRequest->czClientIP, 64, m_pTcpSocket->LocalIP() );
	 if( m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_TCP )
	 {
		pRequest->iClientPort = m_pTcpStreamReader->LocalPort();
	 }
	 else if( m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_RTP_UDP )
	 {
		pRequest->iClientPort = m_pRtpUdpReader->GetRtpSocket()->LocalPort();
	 }
	 int iSize = strlen(csUri.GetURI());
	 pRequest->iURIBufLen = iSize+9;
	 pRequest->iURIBufLen &= (~3); //4 �ֽڶ���
	 GS_SNPRINTF((char*)pRequest->szURI, pRequest->iURIBufLen-1, "%s",csUri.GetURI()  );

	 StruMediaInfoTable *pTable;
	 pTable = GSPCMDREQUEST_STREAMATTRI(pRequest);   
	 bzero(pTable, sizeof(*pTable));

	 iSize = GSPCMDREQUEST_BASE_SIZE+pRequest->iURIBufLen+4;
	 
	 if( !CGspMediaFormat::InfoToStruct(m_pParent->m_csMediaInfo,pTable, 1400-iSize ) )
	 {
		 GS_ASSERT(0);
		 MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ת��ý����Ϣʧ��.\n"), m_iAutoID );
		 CMemoryPool::Free(pRequest);
		 return eERRNO_NET_EREVT;
	 }	
	 INT32 iTag = AtomicInterInc(m_iCmdTagSequence);

	 CGspSyncWaiter *pAsync = CGspSyncWaiter::Create(this,GSP_CMD_ID_RET_REQUEST, iTag, bAsync, iTimeouts);
	 if( !pAsync )
	 {
		 GS_ASSERT(0);
		 MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. �����ڴ�ʧ��A.\n"), m_iAutoID );
		 CMemoryPool::Free(pRequest);
		 return eERRNO_SYS_ENMEM;
	 }
	 pAsync->RefObject();
	 m_csAsyncWRMutex.LockWrite();
	 eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	 m_csAsyncWRMutex.UnlockWrite();

	 if( eERRNO_SUCCESS!= eErrno )
	 {
		 GS_ASSERT(0);
		 MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );
		 CMemoryPool::Free(pRequest);
		 pAsync->UnrefObject();
		 pAsync->UnrefObject();
		 return eERRNO_SYS_ENMEM;
	 }

		  
	 iSize = GSPCMDREQUEST_BASE_SIZE+pRequest->iURIBufLen+GetMediaInfoTableSize(pTable);
	 eErrno = SendCommand(GSP_CMD_ID_REQUEST, pRequest,iSize, iTag );
	 CMemoryPool::Free(pRequest);

	 if( eErrno == eERRNO_SUCCESS )
	 {

		 eErrno = pAsync->WaitResult(iTimeouts);	 
		 if( eErrno == eERRNO_ENONE )
		 {
			 //û�д���
			 if( bAsync)
			 {
				 eErrno = eERRNO_SUCCESS; //�첽����
				 m_pParent->m_eStatus = CIClientChannel::ST_WOPEN;
				 m_csAsyncWaiterTimer.Start();
			 }
			 else
			 {
				 eErrno = eERRNO_SYS_ETIMEOUT;
			 }

		 }
		 else
		 {
			 //�ȵ����
			 if(  !bAsync  )
			 {
				 //�첽֪ͨ
				 m_csAsyncWRMutex.LockWrite();
				 m_csAsyncCmdList.Erase(pAsync);
				 m_csAsyncWRMutex.UnlockWrite();
			 }

			 if( eErrno == eERRNO_SUCCESS )
			 {
				 //�ɹ�
				 m_pParent->m_eStatus = CIClientChannel::ST_READY;	
			 }
			 else
			 {
				 //ʧ��
				 MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. %s.\n"),
					 m_iAutoID,GetError(eErrno));	
				  m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
			 }
		 }
	 } 
	 else
	 {
		 MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ��ʧ��. ���緢������ʧ��.\n"),m_iAutoID);
		 eErrno = eERRNO_NET_EWEVT;
		 m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
	 }

	 if( eErrno == eERRNO_SUCCESS )
	 {
		 m_csAsyncWaiterTimer.Start(); //���������
	 }
	 else
	 {
		m_pTcpSocket->Disconnect();
		m_csAsyncWRMutex.LockWrite();
		m_csAsyncCmdList.Erase(pAsync);
		m_csAsyncWRMutex.UnlockWrite();

	 }
	 pAsync->DelayFalse();
	 pAsync->UnrefObject();	
	 return eErrno;
}

EnumErrno CGspChannel::Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts)
{
	

EnumErrno eErrno = eERRNO_SUCCESS;
	if( !m_pParent->IsOpened() )
	{
	//	GS_ASSERT(0);
		return eERRNO_SYS_ESTATUS;
	}
	INT32 iTag = AtomicInterInc(m_iCmdTagSequence);

	CGspSyncWaiter *pAsync = CGspSyncWaiter::Create(this,GSP_CMD_ID_RET_CTRL, iTag, bAsync, iTimeouts);
	if( !pAsync )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u)  ����ʧ��. �����ڴ�ʧ��A.\n"),m_iAutoID);		
		return eERRNO_SYS_ENMEM;
	}

	::memcpy( &pAsync->m_stCtrl, &stCtrl,sizeof(stCtrl) );
	pAsync->RefObject();
	m_csAsyncWRMutex.LockWrite();
	eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	m_csAsyncWRMutex.UnlockWrite();

	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ����ʧ��. �����ڴ�ʧ��B.\n"),m_iAutoID);
		pAsync->UnrefObject();
		pAsync->UnrefObject();
		return eERRNO_SYS_ENMEM;
	}

	eErrno = SendCommand(GSP_CMD_ID_CTRL, &stCtrl,sizeof(stCtrl), iTag );	

	if( eErrno == eERRNO_SUCCESS )
	{
		//�ȴ����
		eErrno = pAsync->WaitResult(iTimeouts);	 
		if( eErrno == eERRNO_ENONE )
		{
			//û�д���
			if( bAsync)
			{
				eErrno = eERRNO_SUCCESS; //�첽����		
				m_csAsyncWaiterTimer.Start();
			}
			else
			{
				eErrno = eERRNO_SYS_ETIMEOUT;
			}

		}
		else
		{
			//�ȵ����
			if(  !bAsync  )
			{
				//�첽֪ͨ
				m_csAsyncWRMutex.LockWrite();
				m_csAsyncCmdList.Erase(pAsync);
				m_csAsyncWRMutex.UnlockWrite();
			}

			if( eErrno != eERRNO_SUCCESS )
			{
				//ʧ��
				MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ���� (%d. %s) ʧ��. %s.\n"),
					m_iAutoID,stCtrl.iCtrlID, GSPCtrlName(stCtrl.iCtrlID), GetError(eErrno));	
				m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
			}
		}
	} 
	else
	{
		MY_LOG_ERROR(H_LOG, _GSTX("CGspChannel(%u) ���� (%d. %s) ʧ��. ���緢������ʧ��.\n"),
			m_iAutoID,stCtrl.iCtrlID, GSPCtrlName(stCtrl.iCtrlID));	
		eErrno = eERRNO_NET_EWEVT;
		m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
	}

	if( eErrno != eERRNO_SUCCESS )
	{
		if( m_pTcpSocket )
		{
			m_pTcpSocket->Disconnect();
		}
		m_csAsyncWRMutex.LockWrite();
		m_csAsyncCmdList.Erase(pAsync);
		m_csAsyncWRMutex.UnlockWrite();

	}
	pAsync->DelayFalse();
	pAsync->UnrefObject();	
	return eErrno;
	
}

EnumErrno CGspChannel::SendCommand(EnumGSPCommandID eCommandID,
						const void *pCommandPlayload,
						UINT iSize, 
						UINT32 iTag)
{
	
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		return eERRNO_SYS_ESTATUS;
	}

	GS_ASSERT( iSize<(GSP_PACKET_SIZE-GSP_PACKET_HEADER_LEN) ); //��ǰ����ְ�

	//��������
	if( iTag==INVALID_COMMAND_TAG )
	{
		iTag = (UINT32)AtomicInterInc(m_iCmdTagSequence);
	}

	CGspCommand csCmd;

	StruGSPCmdRequest *pRequest = (StruGSPCmdRequest *)pCommandPlayload;

	if( eERRNO_SUCCESS!= csCmd.AddCommandPlayload(pCommandPlayload, iSize) )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("GspChannel(%u) ��������(%d,%s)ʧ��. �����ڴ�ʧ��A.\n"),
			m_iAutoID,eCommandID, GSPCommandName(eCommandID));
		return eERRNO_SYS_ENMEM;
	}

	StruGSPCommand &stCmd = csCmd.GetCommand();
	stCmd.iCmdID = (INT)eCommandID;
	stCmd.iTag = iTag;

	pRequest = (StruGSPCmdRequest *)csCmd.CommandPlayload();

	CGspTcpEncoder csEnocder;

	CGSWRMutexAutoWrite wlocker(&m_csWRMutex);
	
	m_stProCmdHeader.iSeq ++;
	m_stProCmdHeader.iSSeq = 0;

	StruBaseBuf vBuf[1];
	vBuf[0].iSize = csCmd.GetWholeDataSize();
	vBuf[0].pBuffer = csCmd.GetWholeData();

	
	CGspProFrame *pProFrame =  csEnocder.Encode(vBuf,1, m_stProCmdHeader );

	if( pProFrame == NULL )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("GspSession(%u) ��������(%d,%s)ʧ��. �����ڴ�ʧ��B.\n"),
			m_iAutoID,eCommandID, GSPCommandName(eCommandID));	
		return eERRNO_SYS_ENMEM;
	}
	
	
	if( !m_pTcpSocket )
	{		
		
		return eERRNO_NET_ECLOSED;
	}
	//�ӵ�д�ȴ�����
	EnumErrno	eErrno = m_csTcpWaitSendQueue.AddTail(pProFrame);
	if( eERRNO_SUCCESS != eErrno )
	{			
		GS_ASSERT(0);
		
		MY_LOG_ERROR(H_LOG, _GSTX("GspSession(%u) ��������(%d,%s)ʧ��. �����ڴ�ʧ��C.\n"),
			m_iAutoID,eCommandID, GSPCommandName(eCommandID));	
		pProFrame->UnrefObject();
		return eERRNO_SYS_ENMEM;
	}

	if( !m_bTcpSending )
	{
		//û���ڷ��ͣ� ��������
		std::vector<CProFrame *> vSends;
		void *pData = NULL;

		do 
		{
			pData = NULL;
			m_csTcpWaitSendQueue.RemoveFront(&pData);
			CGspProFrame *p = (CGspProFrame*)pData;	
			if( p  )
			{			
				vSends.push_back(p);
			}
		} while( pData );

		if( !vSends.empty() )
		{	
			UINT32 iSeq = AtomicInterInc(m_iSendKeySeq);	
			m_bTcpSending = TRUE;
			eErrno = m_pTcpSocket->AsyncSend((void*)iSeq, vSends);
			for(UINT i = 0; i<vSends.size(); i++ )
			{
				vSends[i]->UnrefObject();
			}
			if( eERRNO_SUCCESS != eErrno )
			{			
				m_bTcpSending = FALSE;
				if( eErrno != eERRNO_NET_ECLOSED && eErrno!= eERRNO_NET_EDISCNN)
				{
					//GS_ASSERT(0);		
					MY_LOG_ERROR(H_LOG, _GSTX("GspSession(%u) ��������(%d,%s)ʧ��. �����첽����ʧ��.\n"),
						m_iAutoID,eCommandID, GSPCommandName(eCommandID));
				}		
				return eErrno;			
			}
		}
	}
	return eErrno;
}

void *CGspChannel::OnStreamTcpSocketEvent(	CISocket *pSocket, 
							 EnumSocketEvent iEvt,
							 void *pParam, void *pParamExt )
{
	//ֻ��������
	switch(iEvt)
	{

	case eEVT_SOCKET_ARCV :
		{
			// INT64 i = DoGetTickCount();           
			return (void*) HandleStreamTcpSocketReadEvent( (CGSPBuffer*) pParam );

			//             MY_PRINTF( "***%d, %lld, %lld, (%s), Thd:%d\n", pSocket->GetOSSocket(), (DoGetTickCount()-i), i
			//                 ,m_pClient->m_csURI.GetKey().c_str(), CURRENT_THREAD_ID );
		}
		break;
	case  eEVT_SOCKET_ASEND :
		{               
			GS_ASSERT(0);
		}
		break;
	//�Զ˹ر�
	case eEVT_SOCKET_ERR :
		{
			m_bWaitSendFinish = FALSE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(H_LOG,  _GSTX("CGspChannel(%u) ��TCP�������. Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );

		}		
	
	default :
		{
			m_bWaitSendFinish = FALSE;
			//����
			MY_LOG_DEBUG(H_LOG, _GSTX("CGspChannel(%u) ��TCP Remote Disconnect. Evt:0x%x\n"), 
				m_iAutoID, iEvt );
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; 

}

void CGspChannel::OnRtpReaderEvent( EnumRtpNetEvent eEvt, void *pEvtArgs )
{

	m_iKeepalivePlugs = 0;

	if(  m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		return;
	} 
	if( eEvt == eEVT_RTPNET_STREAM_FRAME )
	{
		CFrameCache *pFrame = (CFrameCache*)pEvtArgs;
		bzero( &pFrame->m_stFrameInfo, sizeof(StruFrameInfo));
		HandleRtpStreamFrame(pFrame);
		//printf( "@");

		//	if( m_eRtpPT == eRTP_PT_PS )

	}
	else
	{
		MY_LOG_DEBUG(H_LOG, _GSTX("CSipChannel(%u) Rtp Rcv Event:%d.\n"), 
			m_iAutoID, (int) eEvt );
	}	
}

void *CGspChannel::OnTcpSocketEvent(	CISocket *pSocket, 
									   EnumSocketEvent iEvt,
									   void *pParam, void *pParamExt )
{
#ifdef _DEBUG
	_CTester _tester(m_iSkTest);
#endif

	switch(iEvt)
	{
	case  eEVT_SOCKET_ASEND :
		{               
			return HandleTcpSocketWriteEvent( (StruAsyncSendEvent*) pParam );
		}
		break;
	case eEVT_SOCKET_ARCV :
		{
			// INT64 i = DoGetTickCount();           
			return (void*) HandleTcpSocketReadEvent( (CGSPBuffer*) pParam );

			//             MY_PRINTF( "***%d, %lld, %lld, (%s), Thd:%d\n", pSocket->GetOSSocket(), (DoGetTickCount()-i), i
			//                 ,m_pClient->m_csURI.GetKey().c_str(), CURRENT_THREAD_ID );
		}
		break;


	case eEVT_SOCKET_ERR :
		{
			m_bWaitSendFinish = FALSE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(H_LOG,  _GSTX("CGspChannel(%u) �������. Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );

		}
		//�Զ˹ر�
	default :
		{
			m_bWaitSendFinish = FALSE;
			//����
			MY_LOG_DEBUG(H_LOG, _GSTX("CGspChannel(%u) Remote Disconnect. Evt:0x%x\n"), 
									m_iAutoID, iEvt );
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; 
}


void *CGspChannel::HandleTcpSocketWriteEvent( const StruAsyncSendEvent *pEvt )
{   
	m_pParent->m_stChannelInfo.iSendToNet += pEvt->iSends;
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT && !m_bWaitSendFinish )
	{
		//�Ѿ��ر�
		m_csWRMutex.LockReader();
		m_bTcpSending = FALSE;
		m_csWRMutex.UnlockReader();
		return NULL;
	}

	//����TCP д�¼�
	if( m_csTcpWaitSendQueue.IsEmpty() )
	{
		// ���Ͷ���Ϊ��
		m_csWRMutex.LockReader();
		if( m_csTcpWaitSendQueue.IsEmpty() )
		{

			m_bTcpSending = FALSE;
			m_csWRMutex.UnlockReader();
			return NULL;
		}
		m_csWRMutex.UnlockReader();
	}

	//������һ��
	std::vector<CProFrame *> vSends;
	void *pData = NULL;
	UINT32 iSeq = AtomicInterInc(m_iSendKeySeq);
	m_csWRMutex.LockReader();
	do 
	{
		pData = NULL;
		m_csTcpWaitSendQueue.RemoveFront(&pData);
		CGspProFrame *p = (CGspProFrame*)pData;	
		if( p  )
		{			
			vSends.push_back(p);
		}
	} while( pData );
	m_csWRMutex.UnlockReader();

	
	if( !vSends.empty() )
	{			
		m_pTcpSocket->AsyncSend((void*)iSeq, vSends);
		for(UINT i = 0; i<vSends.size(); i++ )
		{
			vSends[i]->UnrefObject();
		}
	}
	return NULL;
}

void CGspChannel::HandleRtpStreamFrame(CFrameCache *pFrame)
{
	const BYTE *p = pFrame->GetBuffer().GetData();
	INT iSize = pFrame->GetBuffer().GetDataSize();
	StruGSFrameHeader stHeader;
	if( iSize< sizeof(stHeader) )
	{
		GS_ASSERT(0);
		return ;
	}
	memcpy( &stHeader, p, sizeof(stHeader));
	if( stHeader.iMagic != GS_FRAME_HEADER_MAGIC )
	{
		GS_ASSERT(0);
		return;
	}
	pFrame->m_stFrameInfo.bKey = stHeader.bKey;
	pFrame->m_stFrameInfo.eMediaType = (EnumGSMediaType) stHeader.eMediaType;
	pFrame->m_stFrameInfo.iTimestamp  = stHeader.iTimeStamp;
	pFrame->m_stFrameInfo.bSysHeader = pFrame->m_stFrameInfo.eMediaType == GS_MEDIA_TYPE_SYSHEADER;
	pFrame->m_stFrameInfo.iChnNo = GetMediaChannel((EnumGSMediaType)pFrame->m_stFrameInfo.eMediaType);
	m_pParent->HandleStream(pFrame, TRUE);
}


BOOL CGspChannel::HandleStreamTcpSocketReadEvent( CGSPBuffer  *pBuffer )
{
	m_pParent->m_stChannelInfo.iRcvFromNet += pBuffer->m_iDataSize;
	m_iKeepalivePlugs = 0;
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	//TCP �����̳߳� �ص�


	EnumErrno eErrno;
	BOOL bRet = TRUE;


	eErrno = m_pStreamTcpDecoder->Decode(pBuffer);



	if( eERRNO_SUCCESS != eErrno )
	{
		//ʧ��
		MY_LOG_ERROR(g_pLog, _GSTX("CGspChannel(%u) TCP������Э�����!!!"), m_iAutoID );
		GS_ASSERT(0);	
		m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
		return FALSE;
	}

	//�ɹ�
	CGspProFrame *pProFrame;	
	INT iLost = 0;
	while( m_pParent->m_eStatus!=CIClientChannel::ST_ASSERT  &&
		NULL != (pProFrame = m_pStreamTcpDecoder->Pop()) )
	{
		if( m_iGspVersion==-1 )
		{
			m_iGspVersion = pProFrame->m_iGspVersion;
			if( m_iGspVersion< (INT) m_stProCmdHeader.iVersion )
			{
				m_stProCmdHeader.iVersion = m_iGspVersion;
			}
		}

		if( !pProFrame->IsCommand() )
		{
			//�����Ƿ�֡
			UINT iChn = pProFrame->m_iGspSubChn;
			if( iChn>=GSP_MAX_MEDIA_CHANNELS )
			{
				//�Ƿ�			
				m_pParent->m_stChannelInfo.iLostNetFrames++;
				MY_LOG_FATAL(g_pLog, _GSTX("CGspChannel(%u) �Ƿ�ͨ����:%d!!!"), m_iAutoID 
					,iChn);
				GS_ASSERT(0);	
				pProFrame->UnrefObject();
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
				return FALSE;
			}

			iLost = CClientChannel::CountSeqStep(pProFrame->m_iGspSeq,
				m_pParent->m_VSeq[iChn]);			
			if( iLost>1 )
			{
				m_pParent->m_stChannelInfo.iLostNetFrames += iLost-1;
			}
			m_pParent->m_VSeq[iChn] = pProFrame->m_iGspSeq;

			CFrameCache *pFrame = pFrame->Create(pProFrame);
			if( pFrame )
			{
				pFrame->m_stFrameInfo.iChnNo = iChn;
				pFrame->m_stFrameInfo.bKey = pProFrame->m_iGspExtraVal;
				pFrame->m_stFrameInfo.eMediaType = m_pParent->m_vMediaType[iChn];
				pFrame->m_stFrameInfo.bSysHeader = pFrame->m_stFrameInfo.eMediaType==GS_MEDIA_TYPE_SYSHEADER;
				m_pParent->HandleStream(pFrame, TRUE);	
				pFrame->UnrefObject();
			}
			else
			{
				m_pParent->m_stChannelInfo.iLostNetFrames++;
				MY_LOG_FATAL(g_pLog, _GSTX("CGspChannel(%u) �����ڴ�ʧ��!!!"), m_iAutoID );
				GS_ASSERT(0);					
				pProFrame->UnrefObject();
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
				return FALSE;

			}
			pProFrame->UnrefObject();
			continue;
		}

		//���� �����ܻ��յ����� 
		GS_ASSERT(0); 
		pProFrame->UnrefObject();
	}

	return TRUE;

}

BOOL CGspChannel::HandleTcpSocketReadEvent( CGSPBuffer  *pBuffer )
{

	m_pParent->m_stChannelInfo.iRcvFromNet += pBuffer->m_iDataSize;
	m_iKeepalivePlugs = 0;
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	
	//TCP �����̳߳� �ص�


	EnumErrno eErrno;
	BOOL bRet = TRUE;


 	eErrno = m_pTcpDecoder->Decode(pBuffer);

	

	if( eERRNO_SUCCESS != eErrno )
	{
		//ʧ��
		MY_LOG_ERROR(g_pLog, _GSTX("CGspChannel(%u) ����Э�����!!!"), m_iAutoID );
		GS_ASSERT(0);	
		m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
		return FALSE;
	}

	//�ɹ�
	CGspProFrame *pProFrame;	
	INT iLost = 0;
	while( m_pParent->m_eStatus!=CIClientChannel::ST_ASSERT  &&
		NULL != (pProFrame = m_pTcpDecoder->Pop()) )
	{
		if( m_iGspVersion==-1 )
		{
			m_iGspVersion = pProFrame->m_iGspVersion;
			if( m_iGspVersion< (INT) m_stProCmdHeader.iVersion )
			{
				m_stProCmdHeader.iVersion = m_iGspVersion;
			}
		}

		if( !pProFrame->IsCommand() )
		{
			if( m_eGspStreamTranMode != GSP_STREAM_TRANS_MODE_MULTI_TCP )
			{
				GS_ASSERT(0);
				pProFrame->UnrefObject();
				continue;
			}

			//�����Ƿ�֡
			UINT iChn = pProFrame->m_iGspSubChn;
			if( iChn>=GSP_MAX_MEDIA_CHANNELS )
			{
				//�Ƿ�			
				m_pParent->m_stChannelInfo.iLostNetFrames++;
				MY_LOG_FATAL(g_pLog, _GSTX("CGspChannel(%u) �Ƿ�ͨ����:%d!!!"), m_iAutoID 
					,iChn);
				GS_ASSERT(0);	
				pProFrame->UnrefObject();
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
				return FALSE;
			}

			iLost = CClientChannel::CountSeqStep(pProFrame->m_iGspSeq,
									m_pParent->m_VSeq[iChn]);			
			if( iLost>1 )
			{
				m_pParent->m_stChannelInfo.iLostNetFrames += iLost-1;
			}
			m_pParent->m_VSeq[iChn] = pProFrame->m_iGspSeq;

			CFrameCache *pFrame = pFrame->Create(pProFrame);
			if( pFrame )
			{
				pFrame->m_stFrameInfo.iChnNo = iChn;
				pFrame->m_stFrameInfo.bKey = pProFrame->m_iGspExtraVal;
				pFrame->m_stFrameInfo.eMediaType = m_pParent->m_vMediaType[iChn];
				pFrame->m_stFrameInfo.bSysHeader = pFrame->m_stFrameInfo.eMediaType==GS_MEDIA_TYPE_SYSHEADER;
				m_pParent->HandleStream(pFrame, TRUE);	
				pFrame->UnrefObject();
			}
			else
			{
				m_pParent->m_stChannelInfo.iLostNetFrames++;
				MY_LOG_FATAL(g_pLog, _GSTX("CGspChannel(%u) �����ڴ�ʧ��!!!"), m_iAutoID );
				GS_ASSERT(0);					
				pProFrame->UnrefObject();
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
				return FALSE;

			}
			pProFrame->UnrefObject();
			continue;
		}

		//����
		if( m_pParent->m_eTranModel != GSP_TRAN_RTPLAY  )
		{
			//��ʵʱ��,����״̬����һ����
			CGspCommand csGspCmd;
			eErrno = csGspCmd.Parser(pProFrame);		
			GS_ASSERT(eErrno==eERRNO_SUCCESS);
			if(eErrno==eERRNO_SUCCESS &&
				csGspCmd.GetCommand().iCmdID == GSP_CMD_ID_RET_STATUS )
			{				
				StruPlayStatus *pStatus = &csGspCmd.CastSubCommand<StruPlayStatus>(csGspCmd);				
				m_pParent->HandlePlayStatus(*pStatus);					
				pProFrame->UnrefObject();	
				continue;
			}			
		}

		//ʹ���̳߳��첽����
		if(m_csCommandTask.RSUCCESS != m_csCommandTask.Task(pProFrame))
		{
			pProFrame->UnrefObject();	
			if( m_pParent->m_eStatus!=CIClientChannel::ST_ASSERT && m_csCommandTask.IsEnable() )
			{
				//�ж��Ƿ��Ѿ��ر�
				GS_ASSERT(0);				
				MY_LOG_FATAL(g_pLog, _GSTX("GspSession(%u) ���������̳߳�ʧ��!!!\n"), 
					m_iAutoID );
				m_pTcpSocket->Disconnect();
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);			
			}
			return FALSE;
		}
	}

	return TRUE;
}

void CGspChannel::OnCommandTaskPoolEvent( CObjThreadPool *pTkPool, void *pData )
{
	CGspProFrame *pProFrame = (CGspProFrame*)pData;
	GS_ASSERT(pProFrame);
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		pProFrame->UnrefObject();
		return;
	}

	//�ɹ�
	CGspCommand csGspCmd;
	EnumErrno eErrno = csGspCmd.Parser(pProFrame);
	if( eErrno != eERRNO_SUCCESS )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(g_pLog, _GSTX("GspSession(%u) �����ڴ�ʧ��!!!\n"), m_iAutoID );
		m_pTcpSocket->Disconnect();
		m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
		pTkPool->Disable(FALSE);
		pProFrame->UnrefObject();
		return;
	}
	HandleCommand( &csGspCmd );			
	pProFrame->UnrefObject();
}

//�����
void CGspChannel::HandleCommand( CGspCommand *pCommand )
{
	//�����
	StruGSPCommand *pCmd = &pCommand->GetCommand();
	MY_LOG_INFO(g_pLog,  _GSTX("CGspChannel(%u) ��ʼ��������(%d, %s)...\n"), m_iAutoID, 
		pCmd->iCmdID, GSPCommandName((EnumGSPCommandID)pCmd->iCmdID) );

	switch( pCmd->iCmdID )
	{

	case GSP_CMD_ID_KEEPAVLIE :
		{
			//���� KEEPALIVE        
			HandleKeepalive(pCommand );

		}
		break;
	case GSP_CMD_ID_RET_REQUEST :
		{
			//����������ظ�����         
			HandleRequestResponse( pCommand );
			
		}
		break;
	case GSP_CMD_ID_RET_CTRL :
		{
			//������ƻظ�  
			StruGSPCmdReturn *pRetCmd;
			pRetCmd = (StruGSPCmdReturn*)pCmd->cPlayload; 
			WakeupAsync(pCmd->iCmdID, pCmd->iTag,ErrnoGsp2Local(pRetCmd->iErrno ) );
			
		}
		break;
	case GSP_UDP_SET_SETUP_RESPONSE :
		{
			//RTP ��������
			//HandleRequestRtpResponse( pCommand );
			GS_ASSERT(0);


		}
	break;

	case GSP_RESET_TRANS_ON_TCP_RESPONSE :
		{
				//�ָ�TCP ����
			StruGSPCmdReturn *pRetCmd;
			pRetCmd = (StruGSPCmdReturn*)pCmd->cPlayload; 			
			if( pRetCmd->iErrno != GSP_PRO_RET_SUCCES )
			{
				MY_LOG_ERROR(H_LOG,  _GSTX("CGspChannel(%u) ���ر� �ָ�TCP ����ʧ��: %s.\n"), 
								m_iAutoID, GSPError(pRetCmd->iErrno) );
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
			}
			WakeupAsync(GSP_CMD_ID_RET_REQUEST, -1, eERRNO_SUCCESS );
			
		}
	break;

	case GSP_CMD_ID_COMPLETE :
		{
			//�������	
			StruGSPCmdReturn stRet;
			bzero( &stRet, sizeof( stRet));
			stRet.iErrno = GSP_PRO_RET_SUCCES;		
			m_bWaitSendFinish = TRUE;
			SendCommand(GSP_CMD_ID_RET_COMPLETE, &stRet, sizeof(stRet),pCmd->iTag );
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_STREAM_FINISH);
		}
		break;
	case GSP_CMD_ID_CLOSE :
		{
			//�Ͽ�����		
			StruGSPCmdReturn stRet;
			bzero( &stRet, sizeof( stRet));
			stRet.iErrno = GSP_PRO_RET_SUCCES;		
			m_bWaitSendFinish = TRUE;
			SendCommand(GSP_CMD_ID_RET_CLOSE, &stRet, sizeof(stRet),pCmd->iTag );		
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_REMOTE_CLOSE);
          
		}
		break;

	case GSP_CMD_ID_ASSERT_AND_CLOSE :
		{			
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_REMOTE_ASSERT);
		}
		break;
	case GSP_CMD_ID_RET_STATUS :
		{
			StruPlayStatus *pStatus;         
			pStatus = (StruPlayStatus*)pCmd->cPlayload;
			m_pParent->HandlePlayStatus(*pStatus);			
		}
		break;

	case GSP_CMD_ID_RET_UNKNOWN_COMMAND :
		{
			StruGSPCmdReturn *pCmdRet = (StruGSPCmdReturn*)pCmd->cPlayload;
			
			MY_LOG_ERROR( H_LOG, _GSTX("CGspChannel(%u) ����������ʶ����(%d,%s).\n"),
							m_iAutoID,pCmdRet->iErrno,
							GSPCommandName((EnumGSPCommandID)pCmdRet->iErrno) );	
			if( (EnumGSPCommandID)pCmdRet->iErrno == GSP_UDP_SET_SETUP_RESPONSE )
			{				
				WakeupAsync(GSP_CMD_ID_RET_REQUEST, -1, eERRNO_SUCCESS );
				
			}
			else
			{
				WakeupAsync(pCmdRet->iErrno, -1, eERRNO_SYS_EPRO );			
			}
		}
		break;
	default :
		{
			MY_LOG_ERROR( H_LOG, _GSTX("CGspChannel(%u) ����ʶ����(%d,%s).\n"),
				m_iAutoID,pCmd->iCmdID,
				GSPCommandName((EnumGSPCommandID)pCmd->iCmdID));	
			StruGSPCmdReturn stRet;
			bzero( &stRet, sizeof(stRet));
			stRet.iErrno = pCmd->iCmdID;		
			SendCommand( GSP_CMD_ID_RET_UNKNOWN_COMMAND, &stRet, sizeof(stRet), pCmd->iTag ); 
			
		}
		break;

	} //end switch
}

void CGspChannel::HandleKeepalive(  CGspCommand *pCommand  )
{
	//����Keepalive ����
	StruGSPCmdKeepalive *pKeepalive = &pCommand->CastSubCommand<StruGSPCmdKeepalive>(*pCommand);	
	if( pKeepalive->iMagic != GSP_MAGIC )
	{   
		//���ӱ��ر�
		//�Ƿ�����
		m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
		return;
	}

	if(pKeepalive->iArgs == KEEPALIVE_ARGS_NONE  )
	{
		//����Ҫ�ظ�
		return ;
	}
	SendKeepalive(FALSE);
}

void CGspChannel::HandleRequestResponse(  CGspCommand *pCommand )
{
	StruGSPCmdRetRequest *pRetCmd = &pCommand->CastSubCommand<StruGSPCmdRetRequest>(*pCommand);	
	EnumErrno eRet = ErrnoGsp2Local(pRetCmd->iErrno);
	if( pRetCmd->iTransMode != m_eGspStreamTranMode )
	{
		GS_ASSERT(0);
		 eRet = eERRNO_SYS_EPRO; 
	}
	
	if( eRet == eERRNO_SUCCESS )
	{
		  
		//�ɹ�

		//����Keepalive 

		StruGSPCmdKeepalive stKeepalive;
		bzero(&stKeepalive, sizeof(stKeepalive));
		stKeepalive.iMagic = GSP_MAGIC;
		stKeepalive.iArgs = 0;
		SendCommand(GSP_CMD_ID_KEEPAVLIE, &stKeepalive, sizeof(stKeepalive));


		CGspMediaFormat::StructToInfo( &pRetCmd->stStreamAttri, m_pParent->m_csMediaInfo );
		m_pParent->RefreshMediaInfo();
		m_iMaxKeepaliveTimeouts = pRetCmd->iKeepaliveTimeout;
		m_pParent->m_iCtrlAbilities = pRetCmd->iAbilities|GSP_CTRL_STOP;

		m_csSendKeepaliveTimer.AlterTimer((m_iMaxKeepaliveTimeouts/4)*1000+500);
		//	m_csSendKeepaliveTimer.AlterTimer(1000);

		if( m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_RTP_UDP )
		{
			m_pRtpUdpReader->BindChannelMediaType(0, GS_MEDIA_TYPE_VIDEO, pRetCmd->iRtpPlayloadType );
			m_pRtpUdpReader->SetEventListener(this, (FuncPtrRtpNetEvent)&CGspChannel::OnRtpReaderEvent);
			if(  m_pRtpUdpReader->Start() )
			{
				GS_ASSERT(0);
				eRet = eERRNO_NET_EBIN;
				MY_LOG_DEBUG(H_LOG,  _GSTX("CGspChannel(%u) ����URI(%s) ʧ��.���� RTPReader ʧ��.\n "),
					m_iAutoID, m_pParent->m_strURI.c_str());
			}
		}
		else if( m_eGspStreamTranMode == GSP_STREAM_TRANS_MODE_TCP  )
		{
			m_pTcpStreamReader->SetListener(this, (FuncPtrSocketEvent)&CGspChannel::OnStreamTcpSocketEvent);
			pRetCmd->czServerIP[65]='\0';
			eRet = m_pTcpStreamReader->Connect(pRetCmd->iServerPort, (char*)pRetCmd->czServerIP);
			if(  eRet )
			{
				GS_ASSERT(0);
				eRet = eERRNO_NET_EDISCNN;
				MY_LOG_DEBUG(H_LOG,  _GSTX("CGspChannel(%u) ����URI(%s) ʧ��.������TCP������: %s:%d ʧ��.\n "),
					m_iAutoID, m_pParent->m_strURI.c_str(),
					(char*)pRetCmd->czServerIP, pRetCmd->iServerPort );
			}
			else 
			{
				eRet = m_pTcpStreamReader->AsyncRcv(TRUE);
				if(  eRet )
				{
					GS_ASSERT(0);
					eRet = eERRNO_NET_EBIN;
					MY_LOG_DEBUG(H_LOG,  _GSTX("CGspChannel(%u) ����URI(%s) ʧ��.������TCP������: %s:%d SOCKET �������¼�ʧ��.\n "),
						m_iAutoID,m_pParent->m_strURI.c_str(),
						(char*)pRetCmd->czServerIP, pRetCmd->iServerPort );
					m_pTcpStreamReader->Disconnect();
				}
			}
		}
	
		if( eRet == eERRNO_SUCCESS ) 
		{
			m_csSendKeepaliveTimer.Start();

			if( m_pParent->m_eStatus != CIClientChannel::ST_ASSERT )
			{
				m_pParent->m_eStatus = CIClientChannel::ST_READY;
			}		

			MY_LOG_DEBUG(H_LOG,  _GSTX("CGspChannel(%u) ����URI(%s) �ɹ�.\n "),
				m_iAutoID, m_pParent->m_strURI.c_str());
			MY_LOG_INFO(H_LOG, "=*===========*=\n%s\n=*===========*=\n",m_pParent->m_csMediaInfo.Serial2String().c_str());
		}
	} 

	WakeupAsync(GSP_CMD_ID_RET_REQUEST, pCommand->GetCommand().iTag,
					eRet );
	
	
}

void CGspChannel::WakeupAsync(INT32 iCommandID, INT32 iCmdTag, EnumErrno eErrno )
{
	m_csAsyncWRMutex.LockWrite();

	CList::CIterator<CGspSyncWaiter* > csIt;
	CGspSyncWaiter *p;
	CGspSyncWaiter *pFind = NULL;
	for(csIt = m_csAsyncCmdList.First<CGspSyncWaiter* >();
			csIt.IsOk(); csIt.Next() )
	{
		p = csIt.Data();
		if( (p->m_iWaitTagResponse == iCmdTag || iCmdTag==-1 )  &&
			p->m_eWaitCommandID == iCommandID )
		{
			pFind =  p;
			m_csAsyncCmdList.Remove(csIt);
			break;
		}
	}
	m_csAsyncWRMutex.UnlockWrite();

	if(!pFind )
	{		
		return;
	}
	pFind->Wakeup(eErrno);
	if( pFind->m_iWakeupParentCmdID != GSP_CMD_ID_NONE )
	{
		//���Ѹ���
		WakeupAsync(pFind->m_iWakeupParentCmdID, pFind->m_iWakeupParentTag, eErrno );
	}	
	pFind->UnrefObject();
}

void CGspChannel::OnTimerEvent( CWatchTimer *pTimer )
{
	switch(pTimer->GetID() )
	{
	case TIMER_ID_KEEPALIVE :
		{
			//���      
			m_iKeepalivePlugs++;
			if( m_iKeepalivePlugs > m_iMaxKeepaliveTimeouts )
			{							
				m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
			} 
			else if( m_iKeepalivePlugs > m_iMaxKeepaliveTimeouts>>2  
					&& 0==m_iMaxKeepaliveTimeouts%5 )
			{
				//����Զ˷���Keepalive 
				SendKeepalive(TRUE);			
			}

		}
		break;
	case TIMER_ID_SEND_KEEPALIVE :
		{
			//����Keepalive
			if( m_pParent->m_eStatus != CIClientChannel::ST_ASSERT )
			{
				SendKeepalive(FALSE);			
			}
		}
		break;
	case TIMER_ID_ASYNC_WAKEUP :
		{
			//����ͬ��
			m_csAsyncWRMutex.LockReader();
			CList::CIterator<CGspSyncWaiter* > csIt;
			CGspSyncWaiter *p;
			CGspSyncWaiter *pFind = NULL;
			for(csIt = m_csAsyncCmdList.First<CGspSyncWaiter* >();
				csIt.IsOk(); csIt.Next() )
			{
				p = csIt.Data();
				if( p->TestTimeout() )
				{
					pFind =  p;
					m_csAsyncCmdList.Remove(csIt);
					break;
				}
			}
			m_csAsyncWRMutex.UnlockReader();

			if(!pFind )
			{		
				return;
			}
			pFind->Wakeup(eERRNO_SYS_ETIMEOUT);
			if( pFind->m_iWakeupParentCmdID != GSP_CMD_ID_NONE )
			{
				//���Ѹ���
				WakeupAsync(pFind->m_iWakeupParentCmdID, pFind->m_iWakeupParentTag, eERRNO_SYS_ETIMEOUT );
			}	
			pFind->UnrefObject();

		}
	}
}

void CGspChannel::SendKeepalive( BOOL bResponse )
{
	StruGSPCmdKeepalive stKeepalive;
	bzero(&stKeepalive, sizeof(stKeepalive));
	stKeepalive.iMagic = GSP_MAGIC;
	stKeepalive.iArgs = bResponse;			
	SendCommand(GSP_CMD_ID_KEEPAVLIE, &stKeepalive, sizeof(stKeepalive));	
}

void CGspChannel::OnAsyncWaiterEvent(CGspSyncWaiter *pWaiter)
{
	if( pWaiter->m_eWaitCommandID == GSP_CMD_ID_RET_REQUEST )
	{
		//�������󷵻�
		BOOL bRet = pWaiter->m_eErrno==eERRNO_SUCCESS;
		m_pParent->SendEvent(GSP_EVT_CLI_RETREQUEST,
						(void*)bRet	, sizeof(BOOL));		
	}
	else if( pWaiter->m_eWaitCommandID == GSP_CMD_ID_RET_CTRL )
	{
		if( 0== (pWaiter->m_stCtrl.iCtrlID&GSP_CTRL_FLOWCTRL ) )
		{ 
			//��������Ϊ�ڲ�ʹ��
			if( pWaiter->m_eErrno == eERRNO_SUCCESS )
			{
				//�ɹ�
				m_pParent->SendEvent(GSP_EVT_CLI_CTRL_OK,
					(void*)&pWaiter->m_stCtrl, sizeof(pWaiter->m_stCtrl));	
			}
			else
			{
				m_pParent->SendEvent(GSP_EVT_CLI_CTRL_FAIL,
					(void*)&pWaiter->m_stCtrl, sizeof(pWaiter->m_stCtrl));	
			}
		}
	}
}

INT CGspChannel::GetMediaChannel(EnumGSMediaType eType)
{
	for( int i = 0; i<GSP_MAX_MEDIA_CHANNELS; i++ )
	{
		if( m_pParent->m_vMediaType[i] == eType )
		{
			return i;
		}
	}
	return 0;
}



