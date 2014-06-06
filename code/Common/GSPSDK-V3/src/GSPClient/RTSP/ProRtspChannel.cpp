#include "ProRtspChannel.h"
#include "../ClientChannel.h"
#include "../ClientSection.h"
#include "RTSP/RTSPAnalyer.h"
#include "Log.h"
#include "MyMemory.h"


using namespace GSP;
using namespace GSP::RTSP;

#define H_LOG  m_pParent->m_pLog


namespace GSP
{

	namespace RTSP
	{

#define TIMER_ID_KEEPALIVE 10
#define TIMER_ID_SEND_KEEPALIVE 11
#define TIMER_ID_ASYNC_WAKEUP 12




	/*
	********************************************************************
	��ע��
	����    :    CSyncWaiter
	����    :    zouyx
	����ʱ��:    2011/8/2 9:45
	������  :    �첽���͵ĵȴ���
	*********************************************************************
	*/

	class CRtspSyncWaiter : public CRefObject
	{

	public :		
		CGSCondEx m_csCond;
		CGSMutex m_csMutex;
		INT32 m_iWaitCSeqResponse; //�ȴ��Ļظ�
		EnumRTSPComandMask m_eWaitCommandID; //�ȴ�������ID
		UINT32 m_iTimeouts;
		UINT16 m_bWaitSignal;
		UINT64 m_iStartTm;
		CRtspChannel *m_pWaiter;
		BOOL m_bAsync;
		StruGSPCmdCtrl m_stCtrl;
		EnumErrno m_eErrno;
		BOOL m_bOpen;
		BOOL m_bWakeup;
		BOOL m_bDelay;

		INT32 m_iWakeupParentCSeq; //�ȴ��Ļظ�
		EnumRTSPComandMask m_iWakeupParentCmdID; //�ȴ�������ID

		static CRtspSyncWaiter *Create(CRtspChannel *pWaiter,
										EnumRTSPComandMask eWaitCommandID,
										INT32 iWaitCSeq, 
										BOOL bAsync,
										UINT32 iTimeouts )
		{
			CGspSyncWaiter *p= new CGspSyncWaiter();
			if( p )
			{
				
				p->m_pWaiter = pWaiter;
				p->m_eWaitCommandID = eWaitCommandID;
				p->m_iWaitCSeqResponse = iWaitCSeq;
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
		CRtspSyncWaiter(void)
			:CRefObject()
			,m_csCond()
			,m_csMutex()
		{
		
			m_iStartTm = DoGetTickCount()+40;
			m_iTimeouts = 2000;
			m_bWaitSignal = FALSE;	
			m_iWaitCSeqResponse = -1;
			m_eErrno = eERRNO_ENONE;
			bzero( &m_stCtrl, sizeof(m_stCtrl) );
			m_bOpen =  FALSE;
			m_bWakeup = FALSE;
			m_bDelay = TRUE;
			m_iWakeupParentCSeq=-1;
			m_iWakeupParentCmdID = eRTSP_CMD_NONE;
		}

		virtual ~CRtspSyncWaiter(void)
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

} //end namespace RTSP

static void _FreeAsyncWaiter( CRtspSyncWaiter *p )
{
	SAFE_DESTROY_REFOBJECT(&p);
}

static void _FreeWaitSendQueueMember( CIPacket *p )
{
	SAFE_DESTROY_REFOBJECT(&p);
}

static void _FreeTcpRcvTaskMember( CRefBuffer *p )
{
	SAFE_DESTROY_REFOBJECT(&p);
}

CRtspChannel::CRtspChannel(CClientChannel *pParent)
:CIProChannel()
,m_pParent(pParent)
,m_iAutoID(pParent->m_iAutoID)
{
	m_csAsyncCmdList.SetFreeCallback((FuncPtrFree)_FreeAsyncWaiter);
	m_csRtspTcpWaitSendQueue.SetFreeCallback((FuncPtrFree)_FreeWaitSendQueueMember);
	m_csRtspTcpRcvTask.SetFreedTaskDataFunction((FuncPtrFree)_FreeTcpRcvTaskMember);


	m_pRtspTcpSocket = NULL;

	
	m_bRtspTcpSending = FALSE;    //��׼TCP �Ƿ����ڷ���
	m_bRtspWaitSendFinish = FALSE;


	m_pRtspTcpDecoder = NULL; //TCP ��Э�����



	m_iMaxKeepaliveTimeouts = 0;

	m_iKeepalivePlugs = 0;
	
}

CRtspChannel::~CRtspChannel(void)
{
	m_bRtspWaitSendFinish = FALSE;
	m_csSendKeepaliveTimer.Stop();
	m_csAsyncWaiterTimer.Stop();
	m_csKeepaliveTestTimer.Stop();
	m_csRtspTcpRcvTask.Uninit();
	if(m_pRtspTcpDecoder )
	{
		delete m_pRtspTcpDecoder;
		m_pRtspTcpDecoder = NULL;
	}
	if( m_pRtspTcpSocket )
	{
		m_pRtspTcpSocket->Release();
		m_pRtspTcpSocket = NULL;
	}
}

void CRtspChannel::DestoryBefore(void)
{
	m_csSendKeepaliveTimer.Stop();
	m_csAsyncWaiterTimer.Stop();
	m_csKeepaliveTestTimer.Stop();
	m_csRtspTcpRcvTask.Uninit();
	INT iTrys = 1000;
	while( m_bRtspTcpSending && m_bRtspWaitSendFinish  && iTrys-->0)
	{
		//�ȴ��������
		MSLEEP(10);
	}
	GS_ASSERT(iTrys>0);

	if( m_pRtspTcpSocket )
	{
		m_pRtspTcpSocket->Disconnect();
	}	
	

}

EnumErrno CRtspChannel::FlowCtrl( BOOL bStart )
{
	//TODO...
	return eERRNO_SYS_EFUNC;
}

EnumErrno CRtspChannel::Open(const CUri &csUri, BOOL bAsync, INT iTimeouts)
{

	CTCPClientSocket *pSocket;
	EnumErrno eErrno = eERRNO_SUCCESS;

	MY_LOG_NOTICE(H_LOG, _GSTX("CRtspChannel(%u) Open ��URI(%s)\n"),
		m_iAutoID, csUri.GetURI());

	m_pRtspTcpDecoder == new CRtspTcpDecoder();

	if( !m_pRtspTcpSocket )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ����Э���������ʧ��.\n"), m_iAutoID );
		return eERRNO_SYS_ENMEM;
	}

	if( !m_csRtspTcpRcvTask.Init(this, (FuncPtrObjThreadPoolEvent)&CRtspChannel::OnRtspTcpRcvTaskPoolEvent, 
		1, FALSE) )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��ʼ���̳߳�ʧ��.\n"), m_iAutoID );
		return eERRNO_SYS_ETKP;
	}

	m_csSendKeepaliveTimer.Init(this, (FuncPtrTimerCallback)&CRtspChannel::OnTimerEvent,
								TIMER_ID_SEND_KEEPALIVE, m_iMaxKeepaliveTimeouts*1000,FALSE);
	if( !m_csSendKeepaliveTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��A.\n"), m_iAutoID );
		return eERRNO_SYS_EBUSY;
	}

	m_csAsyncWaiterTimer.Init(this, (FuncPtrTimerCallback)&CRtspChannel::OnTimerEvent,
		TIMER_ID_ASYNC_WAKEUP, 10,FALSE);
	if( !m_csAsyncWaiterTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��B.\n"), m_iAutoID );
		return eERRNO_SYS_EBUSY;
	}

	m_csKeepaliveTestTimer.Init(this, (FuncPtrTimerCallback)&CRtspChannel::OnTimerEvent,
		TIMER_ID_KEEPALIVE, 1000,FALSE);
	if( !m_csAsyncWaiterTimer.IsReady())
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��ʱ����ʼ��ʧ��C.\n"), m_iAutoID );
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
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ����Socket ����ʧ��.\n") , m_iAutoID );
		return eERRNO_SYS_ENMEM;
	}
	eErrno = pSocket->Connect(csUri.GetPort(),csUri.GetHost());
	if( eERRNO_SUCCESS != eErrno )
	{
		//GS_ASSERT(0);
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ���ӷ�����ʧ��.\n"), m_iAutoID );
							
		pSocket->Release();
		return eERRNO_NET_EDISCNN;
	}
	pSocket->SetListener(this, (FuncPtrSocketEvent)&CRtspChannel::OnTcpSocketEvent);
	m_pRtspTcpSocket = pSocket;
	eErrno = pSocket->AsyncRcv(TRUE);

	if( eERRNO_SUCCESS != eErrno )
	{		
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ����������¼�ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);
		m_pRtspTcpSocket->Disconnect();
		return eERRNO_NET_EREVT;
	}


	CRtspIPacket *pRequest = NULL;
	INT32 iCSeq = 0;

	eErrno = ParserRequest(eRTSP_CMD_OPTIONS, &pRequest, iCSeq );
	if( eERRNO_SUCCESS != eErrno )
	{
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��������ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);	
		m_pRtspTcpSocket->Disconnect();
		return eERRNO_SYS_ENMEM;
	}
	
	
	 CRtspSyncWaiter *pAsync = CRtspSyncWaiter::Create(this,eRTSP_CMD_OPTIONS, iCSeq, bAsync, iTimeouts);
	 if( !pAsync )
	 {
		 GS_ASSERT(0);
		 MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��A.\n"), m_iAutoID );
		 pRequest->UnrefObject();
		 m_pRtspTcpSocket->Disconnect();
		 return eERRNO_SYS_ENMEM;
	 }
	 pAsync->RefObject();
	 m_csAsyncWRMutex.LockWrite();
	 eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	 m_csAsyncWRMutex.UnlockWrite();

	 if( eERRNO_SUCCESS!= eErrno )
	 {
		 GS_ASSERT(0);
		 MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );
		 pAsync->UnrefObject();
		 pAsync->UnrefObject();
		 pRequest->UnrefObject();
		 m_pRtspTcpSocket->Disconnect();
		 return eERRNO_SYS_ENMEM;
	 }

	 eErrno = SendRtspCmdPacket(pRequest);
	 pRequest->UnrefObject();

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
				 MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. %s.\n"),
					 m_iAutoID,GetError(eErrno));	
				  m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
			 }
		 }
	 } 
	 else
	 {
		 MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ���緢������ʧ��.\n"),m_iAutoID);
		 eErrno = eERRNO_NET_EWEVT;
		 m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
	 }

	 if( eErrno == eERRNO_SUCCESS )
	 {
		 m_csAsyncWaiterTimer.Start(); //���������
	 }
	 else
	 {
		m_pRtspTcpSocket->Disconnect();
		m_csAsyncWRMutex.LockWrite();
		m_csAsyncCmdList.Erase(pAsync);
		m_csAsyncWRMutex.UnlockWrite();

	 }
	 pAsync->DelayFalse();
	 pAsync->UnrefObject();	
	 return eErrno;
}

EnumErrno CRtspChannel::Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts)
{
EnumErrno eErrno = eERRNO_SUCCESS;
	if( !m_pParent->IsOpened() )
	{
		GS_ASSERT(0);
		return eERRNO_SYS_ESTATUS;
	}


	CRtspIPacket *pRequest = NULL;
	INT32 iCSeq = 0;

	eErrno = ParserCtrl(stCtrl, &pRequest, iCSeq );
	if( eERRNO_SUCCESS != eErrno )
	{
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ����ʧ��. ��������ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);		
		return eERRNO_SYS_ENMEM;
	}


	CRtspSyncWaiter *pAsync = CRtspSyncWaiter::Create(this,eRTSP_CMD_PLAY, iCSeq, bAsync, iTimeouts);
	if( !pAsync )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u)  ����ʧ��. �����ڴ�ʧ��A.\n"),m_iAutoID);		
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
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ����ʧ��. �����ڴ�ʧ��B.\n"),m_iAutoID);
		pAsync->UnrefObject();
		pAsync->UnrefObject();
		pRequest->UnrefObject();
		return eERRNO_SYS_ENMEM;
	}

	eErrno = SendRtspCmdPacket(pRequest);
	pRequest->UnrefObject();

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
				MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ���� (%d. %s) ʧ��. %s.\n"),
					m_iAutoID,stCtrl.iCtrlID, GSPCtrlName(stCtrl.iCtrlID), GetError(eErrno));	
				m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
			}
		}
	} 
	else
	{
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ���� (%d. %s) ʧ��. ���緢������ʧ��.\n"),
			m_iAutoID,stCtrl.iCtrlID, GSPCtrlName(stCtrl.iCtrlID));	
		eErrno = eERRNO_NET_EWEVT;
		m_pParent->m_eStatus = CIClientChannel::ST_ASSERT;
	}

	if( eErrno != eERRNO_SUCCESS )
	{
		if( m_pRtspTcpSocket )
		{
			m_pRtspTcpSocket->Disconnect();
		}
		m_csAsyncWRMutex.LockWrite();
		m_csAsyncCmdList.Erase(pAsync);
		m_csAsyncWRMutex.UnlockWrite();

	}
	pAsync->DelayFalse();
	pAsync->UnrefObject();	
	return eErrno;
	
}



void *CRtspChannel::OnRtspTcpSocketEvent(	CISocket *pSocket, 
									   EnumSocketEvent iEvt,
									   void *pParam, void *pParamExt )
{
	switch(iEvt)
	{
	case  eEVT_SOCKET_ASEND :
		{               
			return HandleRtspTcpSocketWriteEvent( (CIPacket*) pParam );
		}
		break;
	case eEVT_SOCKET_ARCV :
		{
			// INT64 i = DoGetTickCount();           
			return (void*) HandleRtspTcpSocketReadEvent( (CRefBuffer*) pParam );

			//             MY_PRINTF( "***%d, %lld, %lld, (%s), Thd:%d\n", pSocket->GetOSSocket(), (DoGetTickCount()-i), i
			//                 ,m_pClient->m_csURI.GetKey().c_str(), CURRENT_THREAD_ID );
		}
		break;


	case eEVT_SOCKET_ERR :
		{
			m_bRtspWaitSendFinish = FALSE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(H_LOG,  _GSTX("CRtspChannel(%u) �������. Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );

		}
		//�Զ˹ر�
	default :
		{
			m_bRtspWaitSendFinish = FALSE;
			//����
			MY_LOG_DEBUG(H_LOG, _GSTX("CRtspChannel(%u) Remote Disconnect. Evt:0x%x\n"), 
									m_iAutoID, iEvt );
			m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; 
}

CIPacket *CRtspChannel::HandleRtspTcpSocketWriteEvent( CIPacket *pPacket )
{   
	m_pParent->m_stChannelInfo.iSendToNet += pPacket->TotalSize();
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT && !m_bRtspWaitSendFinish )
	{
		//�Ѿ��ر�
		m_csWRMutex.LockReader();
		m_bRtspTcpSending = FALSE;
		m_csWRMutex.UnlockReader();
		return NULL;
	}

	//����TCP д�¼�
	if( m_csRtspTcpWaitSendQueue.IsEmpty() )
	{
		// ���Ͷ���Ϊ��
		m_csWRMutex.LockReader();
		m_bRtspTcpSending = FALSE;
		m_csWRMutex.UnlockReader();
		return NULL;
	}

	//������һ��
	m_csWRMutex.LockReader();
	void *pData = NULL;
	m_csRtspTcpWaitSendQueue.RemoveFront(&pData);
	m_csWRMutex.UnlockReader();
	GS_ASSERT(NULL!=pData); //����Ӧ�ó���????
	return (CIPacket*)pData;
}

BOOL CRtspChannel::HandleRtspTcpSocketReadEvent( CRefBuffer *pBuffer )
{
	m_pParent->m_stChannelInfo.iRcvFromNet += pBuffer->GetDataSize();
	m_iKeepalivePlugs = 0;
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	// TCP ����Socket ����ص� 
	pBuffer->RefObject();
	if( CGSPThreadPool::RSUCCESS== m_csRtspTcpRcvTask.Task(pBuffer, FALSE) )
	{
		return TRUE;
	}
	else
	{
		pBuffer->UnrefObject();
	}
	return FALSE;
}

void CRtspChannel::OnRtspTcpRcvTaskPoolEvent( CObjThreadPool *pTkPool, void *pData )
{
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		return;
	}

	//TCP �����̳߳� �ص�
	CRefBuffer *pBuffer = (CRefBuffer*)pData;

	EnumErrno eErrno;
	BOOL bRet = TRUE;
	eErrno = m_pRtspTcpDecoder->Decode(pBuffer);
	pBuffer->UnrefObject();

	if( eERRNO_SUCCESS != eErrno )
	{
		//ʧ��
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ����Э�����!!!\n"), m_iAutoID );
		GS_ASSERT(0);	
		m_pParent->OnProChannelSignalEvent(CClientChannel::eSIG_ASSERT);
		return;
	}

	//�ɹ�
	CRtspIPacket *pPacket;
	while( m_pParent->m_eStatus!=CIClientChannel::ST_ASSERT  &&
		NULL != (pPacket = m_pRtspTcpDecoder->Pop()) )
	{
		HandleRtspTcpPacket(pPacket);
		pPacket->UnrefObject();
	}
}

//RTSP ������

void CRtspChannel::HandleRtspTcpPacket(CRtspIPacket *pPacket)
{
	//�����
	CRtspHeader &csHeader = pPacket->GetHeader();
	if( !csHeader.GetExistParser(eRTSP_H_TYPE_Response) ||
		!csHeader.GetExistParser(eRTSP_H_TYPE_CSeq) )
	{
		//����Ч����
		GS_ASSERT(0);	
		return eERRNO_SYS_EINVALID;
	}

	INT32 iCSeq = GSStrUtil::ToNumber<long>(csHeader.m_csCSeq.m_strCSeq);

	CRtspSyncWaiter *pWaiter = GetSyncWaiter(iCSeq);

	if( !pWaiter )
	{
		//�ǵȴ�����
		GS_ASSERT(0);
		MY_LOG_WARN(H_LOG, _GSTX("CRtspChannel(%u) �յ�δ֪����Seq: %d !!!\n"), m_iAutoID, iCSeq );
		return;
	}


	switch( pWaiter->m_eWaitCommandID )
	{

	case eRTSP_CMD_PLAY :
		{
			pWaiter->Wakeup( RtspResponseStatusToLocalErrno(csHeader.m_csResponse.m_eResNo) );
		}
	break;
		case eRTSP_CMD_OPTIONS:
		{				
			HandleOptionsResponse(pWaiter, &csHeader);
			
		}
		break;
	case eRTSP_CMD_DESCRIBE :
		{

			HandleDescribeResponse(pWaiter, &csHeader);	
		}
		break;
	case eRTSP_CMD_SETUP :
		{
			HandleSetupResponse(pWaiter, &csHeader);	
		}
		break;
	default :
		{
			MY_LOG_ERROR( H_LOG, _GSTX("CRtspChannel(%u) ����ʶ����(%d,%s).\n"),
					m_iAutoID,pWaiter->m_eWaitCommandID,
					RtspGetResponseStatusName(pWaiter->m_eWaitCommandID) );				
		}
		break;
	} //end switch
}

void CRtspChannel::HandleOptionsResponse(CRtspSyncWaiter *pAsync, CRtspHeader *pHeader)
{
	//����Options ����ظ�
	// �����̷��� Options -> Describe

	m_iCurSetupChn = 0;	

	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		pAsync->Wakeup(eERRNO_CLI_EASSERT);
		return;
	}

	if( pHeader->m_csResponse.m_eResNo != eRTSP_OK )
	{
		pAsync->Wakeup(RtspResponseStatusToLocalErrno(pHeader->m_csResponse.m_eResNo) );
		return;
	}


	CRtspIPacket *pRequest = NULL;
	INT32 iCSeq = 0;

	EnumErrno eErrno = ParserRequest(eRTSP_CMD_DESCRIBE, &pRequest, iCSeq );
	if( eERRNO_SUCCESS != eErrno )
	{
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��������ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);		
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}


	CRtspSyncWaiter *pNewAsync = CRtspSyncWaiter::Create(this,eRTSP_CMD_DESCRIBE, iCSeq, 
														TRUE, pAsync->m_iTimeouts);
	if( !pAsync )
	{
		
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��A.\n"), m_iAutoID );
		GS_ASSERT(0);
		pRequest->UnrefObject();
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}

	pNewAsync->m_iWakeupParentCmdID = pAsync->m_eWaitCommandID;  //���û��Ѽ���
	pNewAsync->m_iWakeupParentCSeq = pAsync->m_iWaitCSeqResponse;

	pAsync->RefObject();
	m_csAsyncWRMutex.LockWrite();
	pAsync->m_iStartTm = DoGetTickCount();
	eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	m_csAsyncWRMutex.UnlockWrite();

	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pAsync->UnrefObject();
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();	
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return eERRNO_SYS_ENMEM;
	}

	m_csAsyncWRMutex.LockWrite();
	eErrno = m_csAsyncCmdList.AddTail(pNewAsync);	 
	m_csAsyncWRMutex.UnlockWrite();
	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();
		WakeupAsync(pAsync->m_eWaitCommandID, pAsync->m_iWaitCSeqResponse, eERRNO_SYS_ENMEM);
		return;
	}

	eErrno = SendRtspCmdPacket(pRequest);
	pRequest->UnrefObject();

	if( eErrno == eERRNO_SUCCESS )
	{
		m_csAsyncWaiterTimer.Start(); //���������
	}
	else
	{
		WakeupAsync(eRTSP_CMD_DESCRIBE, iCSeq, eERRNO_SYS_ENMEM);
	}
}


void CRtspChannel::HandleDescribeResponse(CRtspSyncWaiter *pAsync, CRtspHeader *pHeader, 
										  const CGSString &strContent )
{
	if( !pHeader->LineParser(eRTSP_H_TYPE_Accept)  ||
		pHeader->m_csAccept.m_eContentType != eCONT_TYPE_SDP || strContent.length()<1 )
	{
		GS_ASSERT(0);
		pAsync->Wakeup(eERRNO_SYS_EPRO);
		return;
	}
	CSDPFormater csSdp;

	if( csSdp.ParserFrom(strContent) )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u)  Parser sdp protocol '%s' fail.\n"), m_iAutoID,
								strContent.c_str() );
		pAsync->Wakeup(eERRNO_SYS_EPRO);
		return;
	}
	csSdp.SerialToInfo(m_pParent->m_csMediaInfo);
	

	// �� Describe->steup

	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		pAsync->Wakeup(eERRNO_CLI_EASSERT);
		return;
	}

	if( pHeader->m_csResponse.m_eResNo != eRTSP_OK )
	{
		pAsync->Wakeup(RtspResponseStatusToLocalErrno(csHeader->m_csResponse.m_eResNo) );
		return;
	}


	CRtspIPacket *pRequest = NULL;
	INT32 iCSeq = 0;

	EnumErrno eErrno = ParserRequest(eRTSP_CMD_SETUP, &pRequest, iCSeq );
	if( eERRNO_SUCCESS != eErrno )
	{
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��������ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);		
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}


	CRtspSyncWaiter *pNewAsync = CRtspSyncWaiter::Create(this,eRTSP_CMD_SETUP, iCSeq, 
		TRUE, pAsync->m_iTimeouts);
	if( !pAsync )
	{

		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��A.\n"), m_iAutoID );
		GS_ASSERT(0);
		pRequest->UnrefObject();
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}

	pNewAsync->m_iWakeupParentCmdID = pAsync->m_eWaitCommandID;  //���û��Ѽ���
	pNewAsync->m_iWakeupParentCSeq = pAsync->m_iWaitCSeqResponse;

	pAsync->RefObject();
	m_csAsyncWRMutex.LockWrite();
	pAsync->m_iStartTm = DoGetTickCount();
	eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	m_csAsyncWRMutex.UnlockWrite();

	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pAsync->UnrefObject();
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();	
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return eERRNO_SYS_ENMEM;
	}

	m_csAsyncWRMutex.LockWrite();
	eErrno = m_csAsyncCmdList.AddTail(pNewAsync);	 
	m_csAsyncWRMutex.UnlockWrite();
	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();
		WakeupAsync(pAsync->m_eWaitCommandID, pAsync->m_iWaitCSeqResponse, eERRNO_SYS_ENMEM);
		return;
	}

	eErrno = SendRtspCmdPacket(pRequest);
	pRequest->UnrefObject();

	if( eErrno == eERRNO_SUCCESS )
	{
		m_csAsyncWaiterTimer.Start(); //���������
	}
	else
	{
		WakeupAsync(eRTSP_CMD_DESCRIBE, iCSeq, eERRNO_SYS_ENMEM);
	}

}


void  CRtspChannel::HandleSetupResponse(CRtspSyncWaiter *pAsync, CRtspHeader *pHeader)
{
	if( m_pParent->m_eStatus==CIClientChannel::ST_ASSERT )
	{
		//�Ѿ��ر�
		pAsync->Wakeup(eERRNO_CLI_EASSERT);
		return;
	}

	if( pHeader->m_csResponse.m_eResNo != eRTSP_OK )
	{
		pAsync->Wakeup(RtspResponseStatusToLocalErrno(csHeader->m_csResponse.m_eResNo) );
		return;
	}

	if( m_iCurSetupChn==m_pParent->m_csMediaInfo.GetChannelNums() )
	{
		pAsync->Wakeup(eERRNO_SUCCESS);
		//���
		return;
	}

	//����һͨ��
	CRtspIPacket *pRequest = NULL;
	INT32 iCSeq = 0;

	EnumErrno eErrno = ParserRequest(eRTSP_CMD_SETUP, &pRequest, iCSeq );
	if( eERRNO_SUCCESS != eErrno )
	{
		MY_LOG_FATAL(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. ��������ʧ��.\n"), m_iAutoID );
		GS_ASSERT(0);		
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}


	CRtspSyncWaiter *pNewAsync = CRtspSyncWaiter::Create(this,eRTSP_CMD_SETUP, iCSeq, 
		TRUE, pAsync->m_iTimeouts);
	if( !pAsync )
	{

		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��A.\n"), m_iAutoID );
		GS_ASSERT(0);
		pRequest->UnrefObject();
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return;
	}

	pNewAsync->m_iWakeupParentCmdID = pAsync->m_eWaitCommandID;  //���û��Ѽ���
	pNewAsync->m_iWakeupParentCSeq = pAsync->m_iWaitCSeqResponse;

	pAsync->RefObject();
	m_csAsyncWRMutex.LockWrite();
	pAsync->m_iStartTm = DoGetTickCount();
	eErrno = m_csAsyncCmdList.AddTail(pAsync);	 
	m_csAsyncWRMutex.UnlockWrite();

	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pAsync->UnrefObject();
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();	
		pAsync->Wakeup(eERRNO_SYS_ENMEM);
		return eERRNO_SYS_ENMEM;
	}

	m_csAsyncWRMutex.LockWrite();
	eErrno = m_csAsyncCmdList.AddTail(pNewAsync);	 
	m_csAsyncWRMutex.UnlockWrite();
	if( eERRNO_SUCCESS!= eErrno )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(H_LOG, _GSTX("CRtspChannel(%u) ��ʧ��. �����ڴ�ʧ��B.\n"), m_iAutoID );	
		pNewAsync->UnrefObject();
		pRequest->UnrefObject();
		WakeupAsync(pAsync->m_eWaitCommandID, pAsync->m_iWaitCSeqResponse, eERRNO_SYS_ENMEM);
		return;
	}

	eErrno = SendRtspCmdPacket(pRequest);
	pRequest->UnrefObject();

	if( eErrno == eERRNO_SUCCESS )
	{
		m_csAsyncWaiterTimer.Start(); //���������
	}
	else
	{
		WakeupAsync(eRTSP_CMD_DESCRIBE, iCSeq, eERRNO_SYS_ENMEM);
	}

}
















void CRtspChannel::WakeupAsync(INT32 iCommandID, INT32 iCmdTag, EnumErrno eErrno )
{
	m_csAsyncWRMutex.LockWrite();

	CList::CIterator<CGspSyncWaiter* > csIt;
	CGspSyncWaiter *p;
	CGspSyncWaiter *pFind = NULL;
	for(csIt = m_csAsyncCmdList.First<CGspSyncWaiter* >();
			csIt.IsOk(); csIt.Next() )
	{
		p = csIt.Data();
		if( p->m_iWaitTagResponse = iCmdTag  &&
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

void CRtspChannel::OnTimerEvent( CWatchTimer *pTimer )
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

void CRtspChannel::SendKeepalive( BOOL bResponse )
{
	StruGSPCmdKeepalive stKeepalive;
	bzero(&stKeepalive, sizeof(stKeepalive));
	stKeepalive.iMagic = GSP_MAGIC;
	stKeepalive.iArgs = bResponse;			
	SendCommand(GSP_CMD_ID_KEEPAVLIE, &stKeepalive, sizeof(stKeepalive));	
}

void CRtspChannel::OnAsyncWaiterEvent(CGspSyncWaiter *pWaiter)
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



