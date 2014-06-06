#include "GspSrvSession.h"
#include "GspServer.h"
#include "Log.h"
#include "../RefSource.h"
#include "MediaInfo.h"
#include "../Server.h"
#include "../IBaseSource.h"
#include "GSP/GSPAnalyer.h"



using namespace GSP;
using namespace GSP::RTP;

#define TIMER_ID_KEEPALIVE 1
#define TIMER_ID_SEND_KEEPALIVE 2 //����Keepalive

#define WRITE_FLOWOUT_SIZE  MBYTES*3  ////�������֡�� ���2 ��




static void _FreeWaitSendQueueMember( CGspProFrame *p )
{
	SAFE_DESTROY_REFOBJECT(&p);
}


CGspSrvSession::CGspSrvSession(CGspServer *pProServer)
:CISrvSession(pProServer)
,m_pGspServer(pProServer)
{
	m_iGspVersion = -1;
	
	m_pTcpDecoder = NULL;
	m_csTcpWaitSendQueue.SetFreeCallback((FuncPtrFree)_FreeWaitSendQueueMember);
	m_csStreamWaitSendQueue.SetFreeCallback((FuncPtrFree)_FreeWaitSendQueueMember);

	m_eStatus = eST_NONE;
	m_pTcpSocket = NULL;
	m_pRefSource = NULL;
	bzero(&m_stPlayStatus, sizeof(m_stPlayStatus));
	bzero(&m_stProCmdHeader, sizeof(m_stProCmdHeader));
	bzero(&m_vProStreamHeader[0], sizeof(m_vProStreamHeader));

	m_stProCmdHeader.iSubChn = 1;
	m_stProCmdHeader.iVersion = GSP_VERSION;
	m_stProCmdHeader.iCRC = CRC_TYPE_NONE;
	m_stProCmdHeader.iDataType =  GSP_PACKET_TYPE_CMD;

	for(UINT i = 0; i<ARRARY_SIZE(m_vProStreamHeader); i++ )
	{
		m_vProStreamHeader[i].iSubChn = i;
		m_vProStreamHeader[i].iVersion = GSP_VERSION;
		m_vProStreamHeader[i].iCRC = CRC_TYPE_NONE;
		m_vProStreamHeader[i].iDataType =  GSP_PACKET_TYPE_STREAM;
	}


	m_iTagSequence = 1; //���������
	m_bWaitSendFinish = FALSE; //�ȴ��������
	m_bTcpSending = FALSE;

	m_bStopStream = FALSE;

	m_eSignal = eSIG_NONE;

	m_bCtrlIng = FALSE;

	m_stClientInfo.iLostFrames = 0;

	m_bPlayEnd = FALSE;

	m_iWaitSendSize = 0;

	m_eGspStreamTransMode = -1;


	m_pAsyncSrvSocket = NULL;

	m_pRtpUdpSender = NULL;
	m_pTcpStreamSender = NULL;
	m_bStreamTcpSending = FALSE;
	m_bFirstFrame = TRUE;

	m_pSysFrame = NULL;

	m_bFlowCtrl = FALSE;

	m_bPlayCtrlIng = FALSE;

	m_eCtrlIngCmd = GSP_CTRL_PAUSE;


	MY_LOG_INFO(g_pLog, _GSTX("GspSession(%u) Create.\n"), m_iAutoID);
}

CGspSrvSession::~CGspSrvSession(void)
{
	GS_ASSERT(m_pTcpSocket==NULL);
	GS_ASSERT(m_pTcpDecoder==NULL);	
	m_bCtrlIng = FALSE;
	m_csTcpWaitSendQueue.Clear();
	MY_LOG_INFO(g_pLog, _GSTX("GspSession(%u) Destory.\n"), m_iAutoID);

	SAFE_DESTROY_REFOBJECT(&m_pSysFrame);
}


EnumErrno CGspSrvSession::Init( CISocket *pSocket )
{
	CGSWRMutexAutoWrite wlocker(&m_csWRMutex);

	GS_ASSERT(NULL==m_pTcpDecoder);
	m_bCtrlIng = FALSE;
	m_bPlayCtrlIng = FALSE;
	m_bFlowCtrl = FALSE;
	m_eCtrlIngCmd = GSP_CTRL_PAUSE;
	m_pTcpDecoder = new CGspTcpDecoder();
	m_strClientIPInfo = pSocket->GetDescri();

	MY_LOG_NOTICE(g_pLog, _GSTX("GspSession(%u) Init Of Client '%s'.\n"),
		m_iAutoID, m_strClientIPInfo.c_str() );

	GS_SNPRINTF(m_stClientInfo.szRemoteIP, GSP_STRING_IP_LEN, "%s", pSocket->RemoteIP());
	m_stClientInfo.iReomtePort = pSocket->RemotePort();

	if( !m_pTcpDecoder )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("GspSession(%u)  �ܾ� '%s' ����. �½� CGspTcpDecoder ʧ��!!\n"),
			m_iAutoID, m_strClientIPInfo.c_str() );
		GS_ASSERT(0);
		return eERRNO_SYS_ENMEM;
	}

	m_csKeepaliveTimer.Init(this,
		(FuncPtrTimerCallback)&CGspSrvSession::OnTimerEvent
		,TIMER_ID_KEEPALIVE ,
		1000L*(m_pGspServer->m_stConfig.iKeepaliveTimeouts/3+1), 
		FALSE); //

	m_csSendKeepaliveTimer.Init(this,
			(FuncPtrTimerCallback)&CGspSrvSession::OnTimerEvent
			,TIMER_ID_SEND_KEEPALIVE ,
			1000L*(m_pGspServer->m_stConfig.iKeepaliveTimeouts/3+1),
			FALSE); //



	m_pTcpSocket = pSocket;
	m_csKeepaliveTimer.Start(); //������ʱ��

	m_eStatus = eST_WAIT_REQUEST;
	return eERRNO_SUCCESS; 
}

void CGspSrvSession::Start(void)
{
	m_csWRMutex.LockWrite();

	if( m_pTcpSocket )
	{
		m_pTcpSocket->SetListener( this,(FuncPtrSocketEvent) &CGspSrvSession::OnTcpSocketEvent);
	}
	if(NULL ==m_pTcpSocket  || eERRNO_SUCCESS != m_pTcpSocket->AsyncRcv(TRUE) )
	{
		m_eStatus = eST_ASSERT;
		m_bWaitSendFinish = FALSE;
		m_csWRMutex.UnlockWrite();
		//����ʧ��
		StopAllAction();
		m_pGspServer->AsyncDestroySession(this);

	}
	else
	{
		m_csWRMutex.UnlockWrite();
	}
}

// Stream TCP ����Socket ����ص� 
void *CGspSrvSession::OnStreamTcpSocketEvent(	CISocket *pSocket, 
							 EnumSocketEvent iEvt,
							 void *pParam, void *pParamExt )
{
	switch(iEvt)
	{
	case  eEVT_SOCKET_ASEND :
		{     
			StruAsyncSendEvent *pEvt =  (StruAsyncSendEvent *) pParam;
			m_stClientInfo.iSend += pEvt->iSends;
			if( m_eStatus==eST_ASSERT )
			{
				//�Ѿ��ر�
				m_csWRMutex.LockWrite();
				m_bStreamTcpSending = FALSE;
				m_csWRMutex.UnlockWrite();
				return NULL;
			}

			//����TCP д�¼�
			if( m_csStreamWaitSendQueue.IsEmpty() )
			{
				// ���Ͷ���Ϊ��
				m_csWRMutex.LockWrite();
				m_bStreamTcpSending = FALSE;
				m_iWaitSendStreamSize = 0;
				m_csWRMutex.UnlockWrite();
				return NULL;
			}

			//������һ��
			m_csWRMutex.LockReader();  
			std::vector<CProFrame *> vSends;
			void *pData = NULL;
			UINT32 iSeq = m_iSendKeySeq++;
			do 
			{
				pData = NULL;
				m_csStreamWaitSendQueue.RemoveFront(&pData);
				CGspProFrame *p = (CGspProFrame*)pData;	
				if( p  )
				{			
					vSends.push_back(p);
				}
			} while( pData );
			m_iWaitSendStreamSize = 0;

			m_csWRMutex.UnlockReader();
			if( !vSends.empty() )
			{			
				if( m_pTcpStreamSender->AsyncSend((void*)iSeq, vSends) )
				{
					m_bStreamTcpSending = FALSE;
					MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) ��TCPͨ�� ��������ʧ�ܣ� �Ͽ�����.\n"),
						m_iAutoID);
					Signal(eSIG_ASSERT);
				}
				for(UINT i = 0; i<vSends.size(); i++ )
				{
					vSends[i]->UnrefObject();
				}
			}
			return NULL;
		}
		break;
	case eEVT_SOCKET_ARCV :
		{
			//�������յ�����
			MY_LOG_WARN(g_pLog,  _GSTX("GspSession(%u) ��TCPͨ���յ�����.\n"),
				m_iAutoID );
		}
		break;

	case eEVT_SOCKET_ERR :
		{
			m_bWaitSendFinish = FALSE;
			m_bStopStream = TRUE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) ��TCPͨ�� Disconnect Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );
			Signal(eSIG_REMOTE_DISCONNECT);

		}
		break;
		//�Զ˹ر�
	default :
		{
			m_bWaitSendFinish = FALSE;
			m_bStopStream = TRUE;
			//����
			MY_LOG_DEBUG(g_pLog, _GSTX("GspSession(%u) ��TCPͨ�� Remote Disconnect. Evt:0x%x\n"), m_iAutoID, iEvt );
			Signal(eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; 
}

//Async  TCP Server ����Socket ����ص� 
void *CGspSrvSession::OnATcpSrvSocketEvent(	CISocket *pSocket, 
						   EnumSocketEvent iEvt,
						   void *pParam, void *pParamExt )
{

	if( iEvt==eEVT_SOCKET_ACCEPT )
	{
		CISocket *pCliSocket = (CISocket*)pParam;
		MY_LOG_DEBUG(g_pLog,  _GSTX("GspSession(%u) ������TCP����:%s.\n"),
			m_iAutoID, pCliSocket->GetDescri().c_str());

		if( m_pTcpStreamSender )
		{
			MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) �Ѿ�������TCP����:%s.\n"),
				m_iAutoID, m_pTcpStreamSender->GetDescri().c_str());
			pCliSocket->Release();
			(void*)  FALSE;
		}
		m_pTcpStreamSender = pCliSocket;
		m_pAsyncSrvSocket->AsyncAccept(FALSE);
		m_pAsyncSrvSocket->Disconnect();
		m_pTcpStreamSender->SetListener(this, (FuncPtrSocketEvent)&CGspSrvSession::OnStreamTcpSocketEvent );
		m_pTcpStreamSender->AsyncRcv(TRUE);
		return FALSE;
	}
	return (void*) TRUE;
}


void *CGspSrvSession::OnTcpSocketEvent(	CISocket *pSocket, 
									   EnumSocketEvent iEvt,
									   void *pParam, void *pParamExt )
{
	switch(iEvt)
	{
	case  eEVT_SOCKET_ASEND :
		{               
			return HandleTcpSocketWriteEvent( (StruAsyncSendEvent *) pParam );
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
			m_bStopStream = TRUE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) Disconnect Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );
			Signal(eSIG_REMOTE_DISCONNECT);

		}
	break;
		//�Զ˹ر�
	default :
		{
			m_bWaitSendFinish = FALSE;
			m_bStopStream = TRUE;
			//����
			MY_LOG_DEBUG(g_pLog, _GSTX("GspSession(%u) Remote Disconnect. Evt:0x%x\n"), m_iAutoID, iEvt );
			Signal(eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; //TODO
}

void *CGspSrvSession::HandleTcpSocketWriteEvent( const StruAsyncSendEvent *pEvt )
{ 
	m_stClientInfo.iSend += pEvt->iSends;
	if( m_eStatus==eST_ASSERT && !m_bWaitSendFinish )
	{
		//�Ѿ��ر�
		m_csWRMutex.LockWrite();
		m_bTcpSending = FALSE;
		m_csWRMutex.UnlockWrite();
		return NULL;
	}

	//����TCP д�¼�
	if( m_csTcpWaitSendQueue.IsEmpty() )
	{
		// ���Ͷ���Ϊ��
		m_csWRMutex.LockWrite();
		m_bTcpSending = FALSE;
		m_iWaitSendSize = 0;
		m_csWRMutex.UnlockWrite();
		return NULL;
	}

	//������һ��
	m_csWRMutex.LockReader();  
	std::vector<CProFrame *> vSends;
	void *pData = NULL;
	UINT32 iSeq = m_iSendKeySeq++;
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
	m_iWaitSendSize = 0;

	m_csWRMutex.UnlockReader();
	if( !vSends.empty() )
	{			
		if( m_pTcpSocket->AsyncSend((void*)iSeq, vSends) )
		{
			m_bTcpSending = FALSE;
			MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) TCPͨ����������ʧ�ܣ� �Ͽ�����.\n"),
				m_iAutoID);
			Signal(eSIG_ASSERT);
		}
		for(UINT i = 0; i<vSends.size(); i++ )
		{
			vSends[i]->UnrefObject();
		}
	}
	return NULL;
}


BOOL CGspSrvSession::HandleTcpSocketReadEvent(CGSPBuffer *pBuffer )
{
	m_iKeepalivePlugs = 0;
	m_stClientInfo.iRecv += pBuffer->m_iDataSize; 
	if( m_eStatus==eST_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	// TCP ����Socket ����ص� 


	EnumErrno eErrno;
	BOOL bRet = TRUE;
	eErrno = CGspSrvSession::m_pTcpDecoder->Decode(pBuffer);


	if( eERRNO_SUCCESS != eErrno )
	{
		//ʧ��
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) ����Э�����!!!"), m_iAutoID );
		GS_ASSERT(0);	
		Signal(eSIG_ASSERT);
		return FALSE;
	}

	if( m_eStatus==eST_ASSERT )
	{
		//�Ѿ��ر�
		return  FALSE;
	}

	//�ɹ�

	CGspCommand stCmd;

	EnumErrno eRet;
	CGspProFrame *pProFrame;
	while( m_eStatus!=eST_ASSERT &&
		NULL != (pProFrame = m_pTcpDecoder->Pop()) )
	{
		//���ð汾��
		if( m_iGspVersion  == -1 )
		{
			m_iGspVersion = pProFrame->m_iGspVersion;		
			if( m_iGspVersion < (INT) m_stProCmdHeader.iVersion )
			{
				m_stProCmdHeader.iVersion = m_iGspVersion;		
				for(UINT i = 0; i<ARRARY_SIZE(m_vProStreamHeader); i++ )
				{			
					m_vProStreamHeader[i].iVersion = m_iGspVersion;				
				}			
			}
		}

		if( !pProFrame->IsCommand()  )
		{
			//������??? Ϊʲô���յ���
			GS_ASSERT(0);
			pProFrame->UnrefObject();
			continue;
		}
		//��ȡ�������ڴ�
		eRet = stCmd.Parser(pProFrame);		
		pProFrame->UnrefObject();
		pProFrame = NULL;		
		if( eRet )
		{
			MY_LOG_FATAL(g_pLog, _GSTX("GspSession(%u) ��������ʧ��!!!\n"), m_iAutoID );
			GS_ASSERT(0);				
			Signal(eSIG_ASSERT);
			
			return  FALSE;
		}
		HandleCommand( &stCmd );			
	}
	return TRUE;
	
}


void CGspSrvSession::HandleCommand( CGspCommand *pCommand )
{
	//�����
	StruGSPCommand &stCmd = pCommand->GetCommand();
	MY_LOG_INFO(g_pLog,  _GSTX("GspSession(%u) ��ʼ��������(%d, %s)...\n"), m_iAutoID, 
		stCmd.iCmdID, GSPCommandName((EnumGSPCommandID)stCmd.iCmdID) );


	if( m_eStatus == eST_WAIT_START)
	{
		//��Ϊ����
		m_eStatus = eST_PLAYING;           
	}


	switch( stCmd.iCmdID )
	{

	case GSP_CMD_ID_KEEPAVLIE :
		{
			//����Keepalive
			if(  m_eStatus == eST_PLAYING )
			{                 
				HandleKeepalive( pCommand );
			}                
		}
		break;
	case GSP_CMD_ID_RESEND :
		{
			//�ش�����
			HandleRequestResend(pCommand );
		}
		break;
	case GSP_CMD_ID_CTRL :
		{
			// ����
			HandleCtrl( pCommand );
		}
		break;
	case GSP_CMD_ID_REQUEST_STATUS :
		{
			//��ȡ״̬
			HandleRequestStatus( pCommand );
		}
		break;
	case GSP_CMD_ID_REQUEST :
		{
			//����������               
			HandleRequest( pCommand );
		}
		break;
	case GSP_UDP_SET_SETUP :
		{
			//����ʹ��RTP 
			HandleUdpSetupRequest( pCommand );
		}
	break;
	case GSP_RESET_TRANS_ON_TCP :
	{
		//�ָ�ʹ��TCP ����		
		StruGSPCmdReturn stRet;
		bzero( &stRet, sizeof(stRet));
		stRet.iErrno = GSP_PRO_RET_SUCCES;
		SendCommand(GSP_RESET_TRANS_ON_TCP_RESPONSE, &stRet, sizeof(stRet), stCmd.iTag );


	}
	break;
	case GSP_CMD_ID_CLOSE :
		{
			//�Ͽ�
			HandleRequestClose(pCommand);
		}
		break;
	case GSP_CMD_ID_ASSERT_AND_CLOSE :
		{
			//�Զ��쳣
			HandleRemoteAssert(pCommand);
		}
		break;
	default :	
		{
			//������ʶ������
			HandleUnknownCommand(pCommand);
		}
		break;

	} //end switch
}

void CGspSrvSession::HandleKeepalive( CGspCommand *pCommand   )
{
	//����Keepalive ����
	StruGSPCmdKeepalive *pKeepalive;
	pKeepalive = &pCommand->CastSubCommand<StruGSPCmdKeepalive>(*pCommand);
	if( pKeepalive->iMagic != GSP_MAGIC )
	{   
		//���ӱ��ر�
		//�Ƿ�����
		Signal(eSIG_ASSERT);
		return;
	}

	if(pKeepalive->iArgs == KEEPALIVE_ARGS_NONE  )
	{
		//����Ҫ�ظ�
		return ;
	}
	StruGSPCmdKeepalive stKeepalive;
	bzero(&stKeepalive, sizeof(stKeepalive));
	stKeepalive.iMagic = GSP_MAGIC;
	stKeepalive.iArgs = 0;

	SendCommand(GSP_CMD_ID_KEEPAVLIE, &stKeepalive, sizeof(stKeepalive));
}

void CGspSrvSession::HandleRequestResend(CGspCommand *pCommand  )
{
	//�����ش����� 
	//TODO ....
	GS_ASSERT(0);
	MY_DEBUG(_GSTX("����û��ʵ��\n"));
}



void CGspSrvSession::HandleCtrl( CGspCommand *pCommand  )
{
	//�����������
	StruGSPCmdCtrl *pCtrl = &pCommand->CastSubCommand<StruGSPCmdCtrl>(*pCommand);
	EnumErrno eErrno = eERRNO_SYS_EINVALID;
	StruGSPCmdReturn stRet;
	bzero( &stRet, sizeof(stRet));

	if( pCtrl->iCtrlID == GSP_CTRL_STOP )
	{
		//�ȷ���
		m_bStopStream = TRUE;		
		stRet.iErrno = GSP_PRO_RET_SUCCES;
		SendCommand( GSP_CMD_ID_RET_CTRL, &stRet, sizeof(stRet) , pCommand->GetCommand().iTag);
	}
	else if( pCtrl->iCtrlID ==  GSP_CTRL_FLOWCTRL )
	{
		//����
		if( pCtrl->iArgs1 )
		{
			m_bFlowCtrl++;
		}
		else
		{
			m_bFlowCtrl = 0;
		}		
		stRet.iErrno = GSP_PRO_RET_SUCCES;
		SendCommand( GSP_CMD_ID_RET_CTRL, &stRet, sizeof(stRet) , pCommand->GetCommand().iTag);		
		MY_LOG_DEBUG(g_pLog, _GSTX("GspSession(%u) **FlowCtrl: %d.\n"), m_iAutoID,
						m_bFlowCtrl );
		return;
	}

	m_bCtrlIng = TRUE;	

	m_eCtrlIngCmd =  pCtrl->iCtrlID;

	if( pCtrl->iCtrlID == GSP_CTRL_PLAY )
	{
		m_bPlayCtrlIng = TRUE;
	}
	else
	{
		m_bPlayCtrlIng = FALSE;
	}


	if( m_pRefSource )
	{
		eErrno = m_pRefSource->Ctrl(*pCtrl);
	}	
	
	stRet.iErrno = ErrnoLocal2Gsp( eErrno );
	if( eErrno == eERRNO_SUCCESS  )
	{
		if( pCtrl->iCtrlID == GSP_CTRL_STOP )
		{
			m_bStopStream = TRUE;			
		} 
		else
		{
			m_bStopStream = FALSE;
			m_eStatus = eST_PLAYING;
		}

		if( pCtrl->iCtrlID == GSP_CTRL_PAUSE )
		{			
			m_csSendKeepaliveTimer.Start();
			
		}
		else
		{
			m_csSendKeepaliveTimer.Stop();
		}
		
	} 
	if( pCtrl->iCtrlID != GSP_CTRL_STOP )
	{

		SendCommand( GSP_CMD_ID_RET_CTRL, &stRet, sizeof(stRet) ,pCommand->GetCommand().iTag);
	}

	if( m_bCtrlIng )
	{
		m_bCtrlIng = FALSE;
	}

	if( m_bPlayCtrlIng )
	{
		m_bPlayCtrlIng = FALSE;
	}

}


void CGspSrvSession::HandleRequestStatus( CGspCommand *pCommand )
{
	//����״̬��������
	StruPlayStatus stRet;
	bzero( &stRet, sizeof(stRet));
	::memcpy( &stRet, &m_stPlayStatus, sizeof( stRet));
	SendCommand( GSP_CMD_ID_RET_CTRL, &stRet, sizeof(stRet) , pCommand->GetCommand().iTag );
}

void CGspSrvSession::HandleUdpSetupRequest(  CGspCommand *pCommand )
{
//���� RTP UDP �������� GSP_UDP_SET_SETUP
	StruUdpSetupRespone stResponse;
	bzero(&stResponse, sizeof(stResponse) );

	stResponse.iErrno = GSP_PRO_RET_EINVALID;
	SendCommand(GSP_UDP_SET_SETUP_RESPONSE, 
		&stResponse, 
		sizeof(stResponse),
		pCommand->GetCommand().iTag );
	return;
	
}

void CGspSrvSession::HandleRequest( CGspCommand *pCommand )
{
//���� URI ��������
	StruGSPCmdRequest *pRequest = &pCommand->CastSubCommand<StruGSPCmdRequest>(*pCommand);
	StruMediaInfoTable *pTable;
	EnumErrno eErrno;
	
	UINT32 iTag = pCommand->GetCommand().iTag;
	UINT iSize;

	

	if( pRequest->iMagic != GSP_MAGIC ||
		pCommand->m_iGspVersion!=GSP_VERSION )
	{
		//�Ƿ���      
		GS_ASSERT(0);
		SendRequestResponse(GSP_PRO_RET_EPRO, iTag );
		MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) ���󱻾ܾ�. �ͻ���GSP�汾�Ŷ�: %d (��ǰ�汾:%d).\n"),
			m_iAutoID, pCommand->m_iGspVersion, GSP_VERSION);    
		return ;                    
	}
	pTable = GSPCMDREQUEST_STREAMATTRI(pRequest);
	iSize  = GSPCMDREQUEST_BASE_SIZE+GetMediaInfoTableSize(pTable);

	if( pCommand->CommandPlayloadSize() < iSize )
	{
		//�Ƿ���      
		GS_ASSERT(0);
		SendRequestResponse(GSP_PRO_RET_EPRO, iTag );
		MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) ���󱻾ܾ�. �Ƿ����ݰ�����.\n"),
			m_iAutoID );    
		return ; 
	}

	if( m_eStatus != eST_WAIT_REQUEST || m_pRefSource )
	{   
		//�Ѿ������
//		GS_ASSERT(0);
		
		SendRequestResponse(GSP_PRO_RET_EEXIST, iTag);
		return;                    
	}

	//
	m_eGspStreamTransMode = pRequest->iTransMode;
	if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_RTP_UDP )
	{
		//ʹ��RTP-UDP
		m_pRtpUdpSender = m_pRtpUdpSender->Create(NULL, TRUE,FALSE );
		if( !m_pRtpUdpSender )
		{
			SendRequestResponse(GSP_PRO_RET_EBUSY, iTag);
			return;      
		}
		m_pRtpUdpSender->SetRemoteAddress((char*) pRequest->czClientIP, 
			pRequest->iClientPort, 
			pRequest->iClientPort+1);
	}
	else if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_TCP )
	{
		//ʹ�ö���TCP
		m_pAsyncSrvSocket = m_pGspServer->CreateAsyncTcpSrvSocket();
		if( m_pAsyncSrvSocket )
		{
			
				m_pAsyncSrvSocket->SetListener(this, 
							(FuncPtrSocketEvent)&CGspSrvSession::OnATcpSrvSocketEvent );
				if( m_pAsyncSrvSocket->AsyncAccept(TRUE) )
				{
					GS_ASSERT(0);
					SendRequestResponse(GSP_PRO_RET_EBUSY, iTag);
					return;    
				}
		} 
		else 
		{
			GS_ASSERT(0);
			SendRequestResponse(GSP_PRO_RET_EBUSY, iTag);
			return;      
		}
	}
	else if( m_eGspStreamTransMode!=GSP_STREAM_TRANS_MODE_MULTI_TCP )
	{
		SendRequestResponse(GSP_PRO_RET_EVER, iTag);
		return;  
	}



	CUri csUri;
	if( pRequest->iURIBufLen>0 && pRequest->iURIBufLen<MAX_URI_SIZE )
	{  
		//����URI �� ��ֹ����
		char czURI[MAX_URI_SIZE];

		GS_SNPRINTF( czURI,pRequest->iURIBufLen, (char*) pRequest->szURI);
		czURI[MAX_URI_SIZE-1] = '\0';
		m_strURI = czURI;
		//Ȩ�޼��
		if( !csUri.Analyse( (const char *) czURI ) )
		{

			//�Ƿ� URI
			MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u) Request: URI(%s) is invalid.\n"),
				m_iAutoID, czURI );             
			SendRequestResponse(GSP_PRO_RET_EINVALID, iTag );
			return ; 
		}		
	} 
	else
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("GspSession(%u)  Request: URI len invalid: %d.\n"),
			m_iAutoID, pRequest->iURIBufLen  );
		SendRequestResponse(GSP_PRO_RET_EINVALID, iTag );
		return ; 
	}

	GS_SNPRINTF(m_stClientInfo.szSourceKey, GSP_MAX_URI_KEY_LEN, "%s", csUri.GetKey() );

	MY_LOG_DEBUG(g_pLog, _GSTX("GspSession(%u) Request URI: (%s).\n"),m_iAutoID,  m_strURI.c_str());

	CMediaInfo csMdInfo;

	CGspMediaFormat::StructToInfo(pTable, csMdInfo);

	m_eTransModel = pRequest->iTransType;

	eErrno =  m_pGspServer->m_pServer->RequestSource(csUri, csMdInfo,m_eTransModel, &m_pRefSource);




	if( eErrno != eERRNO_SUCCESS )
	{
		INT32 iErr = ErrnoLocal2Gsp(eErrno);
		SendRequestResponse(iErr, iTag);      
		return;
	}
	m_eStatus = eST_WAIT_START;
	UINT32 iAbilites = 0;
	iAbilites = m_pRefSource->GetCtrlAbilities();
	csMdInfo.Set( m_pRefSource->Source()->MediaInfo() );
	m_pRefSource->SetTransMode(m_eTransModel);
	m_pRefSource->SetListener(this,(CRefSource::FuncPtrRefSourceEvent) &CGspSrvSession::OnSourceEvent);
	m_pRefSource->Start();
	SendRequestResponse(GSP_PRO_RET_SUCCES, iTag, iAbilites, &csMdInfo);
}

void CGspSrvSession::HandleRequestClose( CGspCommand *pCommand )
{
	StruGSPCmdReturn stRet;
	bzero( &stRet, sizeof(stRet));
	stRet.iErrno = GSP_PRO_RET_SUCCES;
	SendCommand( GSP_CMD_ID_RET_CLOSE, &stRet, sizeof(stRet), pCommand->GetCommand().iTag );   
	Signal(eSIG_REMOTE_CLOSE);
}

void CGspSrvSession::HandleRemoteAssert( CGspCommand *pCommand  )
{
	Signal(eSIG_REMOTE_DISCONNECT);
}

void CGspSrvSession::HandleUnknownCommand( CGspCommand *pCommand   )
{
	StruGSPCmdReturn stRet;
	bzero( &stRet, sizeof(stRet));
	stRet.iErrno =  pCommand->GetCommand().iCmdID;
	SendCommand( GSP_CMD_ID_RET_UNKNOWN_COMMAND, &stRet, sizeof(stRet), pCommand->GetCommand().iTag );  
}


void CGspSrvSession::SendCommand(EnumGSPCommandID eCommandID, const void *pCommandPlayload, UINT iSize, 
								 UINT32 iTag )
{
	GS_ASSERT( iSize<(GSP_PACKET_SIZE-GSP_PACKET_HEADER_LEN) ); //��ǰ����ְ�

	//��������
	if( iTag==INVALID_COMMAND_TAG )
	{
		iTag = (UINT32)AtomicInterInc(m_iTagSequence);
	}

	CGspCommand csCmd;	
	

	if( eERRNO_SUCCESS!= csCmd.AddCommandPlayload(pCommandPlayload, iSize) )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
		Signal(eSIG_ASSERT);
		return;
	}
	StruGSPCommand &stCmd = csCmd.GetCommand();
	stCmd.iCmdID = (INT)eCommandID;
	stCmd.iTag = iTag;

	CGspTcpEncoder csEnocder;
	m_csWRMutex.LockWrite();

	if( !m_pTcpSocket || m_eStatus == eST_ASSERT )
	{

		m_csWRMutex.UnlockWrite();
		return;
	}

	m_stProCmdHeader.iSeq ++;
	m_stProCmdHeader.iSSeq = 0;

	StruBaseBuf stBuf;
	stBuf.iSize = csCmd.GetWholeDataSize();
	stBuf.pBuffer = csCmd.GetWholeData();

	

	CGspProFrame *pProFrame =  csEnocder.Encode(&stBuf,1, m_stProCmdHeader );
	if( pProFrame==NULL )
	{
		m_csWRMutex.UnlockWrite();
		GS_ASSERT(0);		
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
		Signal(eSIG_ASSERT);
		return;
	}

	


	//�ӵ�д�ȴ�����
	EnumErrno eErrno = m_csTcpWaitSendQueue.AddTail(pProFrame);
	if( eERRNO_SUCCESS != eErrno )
	{			
		GS_ASSERT(0);
		pProFrame->UnrefObject();
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
		m_csWRMutex.UnlockWrite();
		Signal(eSIG_ASSERT);
		return;
	}
	m_iWaitSendSize += pProFrame->GetTotalSize();

	

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
		m_iWaitSendSize = 0;
	
		if( !vSends.empty() )
		{			
			UINT32 iSeq = m_iSendKeySeq++;
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
					MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����첽����ʧ��.\n"), m_iAutoID);
				}
				m_csWRMutex.UnlockWrite();
				Signal(eSIG_ASSERT);	
				return;			
			}
		}
		
	}
	m_csWRMutex.UnlockWrite();
}

void CGspSrvSession::SendRequestResponse( UINT16 iErrno,UINT32 iTag, UINT32 iAbilities,
						 const CIMediaInfo  *pMediaInfo)
{



	StruGSPCmdRetRequest stCmd;
	bzero( &stCmd, sizeof(stCmd));
	stCmd.iMagic = GSP_MAGIC;
	stCmd.iAbilities = iAbilities;
	stCmd.iErrno = iErrno;
	stCmd.iTransMode = m_eGspStreamTransMode; 
	stCmd.iKeepaliveTimeout = m_pGspServer->m_stConfig.iKeepaliveTimeouts;

	if( iErrno == GSP_PRO_RET_SUCCES )
	{
		CGSString strLocalIp = m_pTcpSocket->LocalIP();
		INT iPort = 0;
		if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_TCP )
		{
			iPort = m_pAsyncSrvSocket->LocalPort();
		}
		else if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_RTP_UDP )
		{
			iPort = m_pRtpUdpSender->GetRtpSocket()->LocalPort();
			stCmd.iRtpSSRC = m_iAutoID;
			stCmd.iRtpPlayloadType = eRTP_PT_C3MVIDEO;
			m_pRtpUdpSender->SetSSRC(m_iAutoID);
			m_pRtpUdpSender->SetPlayloadType(eRTP_PT_C3MVIDEO, 0);
		}
		GS_SNPRINTF((char*)stCmd.czServerIP,65, strLocalIp.c_str());
		stCmd.iServerPort = iPort;
	}


	if( pMediaInfo )
	{
		unsigned char *pBuf, *p;

		pBuf = (unsigned char *)CMemoryPool::Malloc(1400);
		if( !pBuf )
		{
			GS_ASSERT(0);
			MY_LOG_FATAL(g_pLog, _GSTX("GspSession(%u) �����ڴ�ʧ��!!\n"), m_iAutoID);
			Signal(eSIG_ASSERT);
			return;
		}
		p = pBuf;	
		INT iSize = sizeof(stCmd)-sizeof(StruMediaInfoTable);
		::memcpy( p, &stCmd, iSize);
		p += iSize;
		
		StruMediaInfoTable *pTable = (StruMediaInfoTable *)p;
		bzero(pTable, sizeof(*pTable));

		if( !CGspMediaFormat::InfoToStruct(*pMediaInfo,pTable, 1400-iSize ) )
		{
			GS_ASSERT(0);
		}		
		iSize = GetMediaInfoTableSize(pTable)+sizeof(stCmd)-sizeof(StruMediaInfoTable);
		SendCommand(GSP_CMD_ID_RET_REQUEST,pBuf, iSize, iTag);
		CMemoryPool::Free(pBuf);
		
	}
	else
	{
		SendCommand(GSP_CMD_ID_RET_REQUEST, &stCmd,sizeof(stCmd), iTag); 
	}

	if( iErrno == GSP_PRO_RET_SUCCES )
	{

		m_eStatus =  eST_WAIT_START; 
	}
}

void CGspSrvSession::Signal(EnumSignal eSig )
{
	m_csWRMutex.LockWrite();
	if( m_eStatus==eST_ASSERT || m_eSignal != eSIG_NONE )
	{
		//�Ѿ������쳣
		if( eSig == eSIG_REMOTE_DISCONNECT )
		{
			m_bWaitSendFinish = FALSE;
		}
		m_csWRMutex.UnlockWrite();
		return;
	}

	if( eSig == eSIG_REMOTE_CLOSE  )
	{
		m_bWaitSendFinish = TRUE;
	}
	m_eStatus = eST_ASSERT;

	GS_ASSERT(m_eSignal == eSIG_NONE );

	m_eSignal = eSig;
	
	m_csWRMutex.UnlockWrite();

	if( eSig == eSIG_REMOTE_CLOSE )
	{
		if( m_pRefSource )
		{
			m_pRefSource->Stop();
		}
		//�ȴ���������		
		INT iTrys = 1000;
		while(!m_bStopStream && m_bTcpSending && m_bWaitSendFinish && iTrys-- > 0 )
		{
			//�ȴ����ݷ������
			MSLEEP(10);
		}
		
		GS_ASSERT(iTrys>0);
	}
	m_bWaitSendFinish = FALSE;
	StopAllAction();
	m_pGspServer->AsyncDestroySession(this);

// 	switch( eSig )
// 	{
// 	case eSIG_ASSERT :
// 	case eSIG_RELEASE :	
// 	case eSIG_REMOTE_DISCONNECT :	
// 	break;
// 	default :
// 		;
// 	}
	
}

void CGspSrvSession::StopAllAction(void)
{
	
	m_csKeepaliveTimer.Stop();
	m_csSendKeepaliveTimer.Stop();
	
	if( m_pTcpSocket )
	{
		m_pTcpSocket->Disconnect();
	}	
	if( m_pRefSource )
	{
		m_pRefSource->Stop();
	}	

	if( m_pAsyncSrvSocket )
	{
		m_pAsyncSrvSocket->Disconnect();
	}
	if( m_pRtpUdpSender )
	{
		m_pRtpUdpSender->Stop();
	}
	if( m_pTcpStreamSender )
	{
		m_pTcpStreamSender->Disconnect();
	}
}

void CGspSrvSession::OnTimerEvent( CWatchTimer *pTimer )
{
	switch(pTimer->GetID() )
	{
	case TIMER_ID_KEEPALIVE :
		{
			//���      
			m_iKeepalivePlugs++;
			if( m_iKeepalivePlugs > 3 )
			{				

				MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. Keepalive ��ʱ.\n"), m_iAutoID);
			    Signal(eSIG_ASSERT);
			}
		}
		break;
	case TIMER_ID_SEND_KEEPALIVE :
		{
			//����Keepalive
			if( m_eStatus != eST_ASSERT )
			{
				StruGSPCmdKeepalive stKeepalive;
				bzero(&stKeepalive, sizeof(stKeepalive));
				stKeepalive.iMagic = GSP_MAGIC;
				if( m_iKeepalivePlugs>2)
				{
					stKeepalive.iArgs = KEEPALIVE_ARGS_REQUEST;
				}
				else
				{
					stKeepalive.iArgs = 0;
				}
				SendCommand(GSP_CMD_ID_KEEPAVLIE, &stKeepalive, sizeof(stKeepalive));
			}
		}
		break;
	}
}

EnumErrno CGspSrvSession::OnSourceEvent(CRefSource *pRefSource,
						CRefSource::EnumRefSourceEvent eEvt, void *pParam)
{
	switch(eEvt)
	{
	case CRefSource::eEVT_PLAY_END :
		{
			if( m_eTransModel!=GSP_TRAN_RTPLAY && !m_bPlayEnd )
			{
				m_bPlayEnd = TRUE;
				EnumGSPCommandID iCmd = GSP_CMD_ID_COMPLETE;			
				StruGSPCmdReturn stCmd;
				bzero(&stCmd, sizeof(stCmd));
				stCmd.iErrno = GSP_PRO_RET_EEND;
				SendCommand(iCmd,&stCmd, sizeof(stCmd));
				MSLEEP(40);
				int i = 0;
				while( m_csTcpWaitSendQueue.Size() >0 && !m_bStopStream  && i++<150 )
				{
					MSLEEP(20);
				}
			}
		}
	break;
	case CRefSource::eEVT_STREAM_FRAME :
		{
			//����֡	
			int iTrys = 200;
			while( m_bPlayCtrlIng && !m_bStopStream && iTrys-- )
			{
				if( m_eTransModel == GSP_TRAN_RTPLAY )
				{
					return eERRNO_SRC_EUNUSED;
				}
				if( m_eCtrlIngCmd == GSP_CTRL_PAUSE )
				{
					break;
				}
				MSLEEP(5);
			}
			return SendStreamFrame( (CFrameCache*) pParam);
		}
	break;
	case CRefSource::eEVT_PLAY_STATUS :
		{
			//����״̬
			
			::memcpy(&m_stPlayStatus, pParam, sizeof(m_stPlayStatus));
			int iTrys = 200;
			while( m_bPlayCtrlIng && !m_bStopStream  && iTrys-- )
			{
				if( m_eTransModel == GSP_TRAN_RTPLAY )
				{
					return eERRNO_SRC_EUNUSED;
				}
				if( m_eCtrlIngCmd == GSP_CTRL_PAUSE )
				{
					break;
				}
				MSLEEP(5);

				
			}
			SendCommand(GSP_CMD_ID_RET_STATUS, &m_stPlayStatus, sizeof(m_stPlayStatus));
		}
		break;
	case CRefSource::eEVT_SOURCE_RELEASE :
		{
			//����Դ���ͷ�	
			EnumGSPCommandID iCmd = GSP_CMD_ID_ASSERT_AND_CLOSE;
			if( m_eTransModel!=GSP_TRAN_RTPLAY )
			{
				if( m_bPlayEnd )
				{					
					iCmd =  GSP_CMD_ID_CLOSE; //������
				}
				else
				{
					m_bStopStream = TRUE;
				}
			}
			StruGSPCmdReturn stCmd;
			bzero(&stCmd, sizeof(stCmd));
			stCmd.iErrno = GSP_PRO_RET_EEND;
			SendCommand(iCmd,&stCmd, sizeof(stCmd));
			if( iCmd ==  GSP_CMD_ID_CLOSE )
			{
				MSLEEP(10);
			}			
			Signal(iCmd ==  GSP_CMD_ID_CLOSE ? eSIG_RELEASE: eSIG_REMOTE_CLOSE);
			
		}
		break;
	case CRefSource::eEVT_SOURCE_ASSERT :
		{
			//����Դ�쳣
			m_bStopStream = TRUE;
			StruGSPCmdReturn stCmd;
			bzero(&stCmd, sizeof(stCmd));
			stCmd.iErrno = GSP_PRO_RET_ESTREAM_ASSERT;
			SendCommand(GSP_CMD_ID_ASSERT_AND_CLOSE,&stCmd, sizeof(stCmd));
		}
		break;
	default :
		;
	}
	return eERRNO_SUCCESS;
}


EnumErrno CGspSrvSession::SendStreamFrame( CFrameCache* pFrame )
{
	
// 	if( pFrame->m_bSysHeader )
// 	{
// 		MY_LOG_WARN(g_pLog, "Start Send Sys header.\n");
// 	}
	


	if( m_bStopStream )
	{
		m_stClientInfo.iLostFrames++;
		return eERRNO_SRC_EUNUSED; //����Ҫ��
	}

	if( m_eStatus==eST_ASSERT )
	{
		m_stClientInfo.iLostFrames++;
		return eERRNO_NET_ECLOSED;	
	}
	UINT iChnNo = pFrame->m_stFrameInfo.iChnNo;
	if( iChnNo>=GSP_MAX_MEDIA_CHANNELS)
	{
		GS_ASSERT(0);
		return eERRNO_NET_ECLOSED;
	}

	//����
	if( m_eTransModel != GSP_TRAN_RTPLAY )
	{
		int iTrys = m_bFlowCtrl*2;
		while( m_bFlowCtrl && iTrys-- )
		{		


			if( m_bStopStream )
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_SRC_EUNUSED; //����Ҫ��
			}
			MSLEEP(5);
			if( m_eStatus==eST_ASSERT)
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_NET_EDISCNN;
			}
			if( m_bCtrlIng )
			{
				break;
			}
			
		}
	}


	if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_MULTI_TCP )
	{
		return SendMultiTcpStreamFrame(pFrame, iChnNo);
	}
	else if ( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_RTP_UDP )
	{
		return SendRtpUdpStreamFrame(pFrame, iChnNo);
	}
	else if( m_eGspStreamTransMode == GSP_STREAM_TRANS_MODE_TCP )
	{
		return  SendTcpStreamFrame(pFrame, iChnNo);
	}
	return eERRNO_SRC_EUNUSED;

}

EnumErrno CGspSrvSession::SendRtpUdpStreamFrame(CFrameCache* pFrame, int iChnNo)
{
	//������		
	if( !m_pRtpUdpSender)
	{
		return eERRNO_SRC_EUNUSED;
	}

	m_pRtpUdpSender->Send(pFrame);

	if( pFrame->m_stFrameInfo.eMediaType == GS_MEDIA_TYPE_SYSHEADER )
	{
		if( m_bFirstFrame )
		{
			MSLEEP(1);
			m_pRtpUdpSender->Send(pFrame);
			MSLEEP(1);
			m_pRtpUdpSender->Send(pFrame);
			m_bFirstFrame = FALSE;
		}			
	}
	return eERRNO_SUCCESS;	
}

EnumErrno CGspSrvSession::SendTcpStreamFrame(CFrameCache* pFrame, int iChnNo)
{

	CGspTcpEncoder csEncoder;


	if( m_pTcpStreamSender == NULL )
	{
		//GS_ASSERT(0);
		if( m_bFirstFrame && pFrame->m_stFrameInfo.eMediaType == GS_MEDIA_TYPE_SYSHEADER )
		{
			SAFE_DESTROY_REFOBJECT(&m_pSysFrame);
			m_pSysFrame = pFrame;
			m_pSysFrame->RefObject();
		}
		return eERRNO_SRC_EUNUSED;
	}

	if( m_bFirstFrame )
	{
		m_bFirstFrame = FALSE;

		if( m_pSysFrame )
		{
			CFrameCache* ptemp = m_pSysFrame;
			m_pSysFrame = NULL;
			SendTcpStreamFrame(ptemp, iChnNo);
			ptemp->UnrefObject();
		}
	}

	if( m_iWaitSendStreamSize >= WRITE_FLOWOUT_SIZE && m_bStreamTcpSending )
	{
		if( m_eTransModel == GSP_TRAN_RTPLAY )
		{

			m_stClientInfo.iLostFrames++;
			//	printf( "lose %d  %lld\n", m_stClientInfo.iLostFrames, (long long) m_iWaitSendSize);
			//ʵʱ���� ��֡			
			return eERRNO_SYS_EFLOWOUT;
		}

		while( m_iWaitSendStreamSize >= (WRITE_FLOWOUT_SIZE>>1) )
		{
			if( m_bStopStream )
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_SRC_EUNUSED; //����Ҫ��
			}
			MSLEEP(10);
			if( m_eStatus==eST_ASSERT)
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_NET_EDISCNN;
			}
			if( m_bCtrlIng )
			{
				break;
			}
		}
	}



	m_vProStreamHeader[iChnNo].iSeq++;
	m_vProStreamHeader[iChnNo].iExtraVal = (pFrame->m_stFrameInfo.bKey || pFrame->m_stFrameInfo.bSysHeader) ? 1 : 0;

	CGspProFrame *pProFrame =  csEncoder.Encode(pFrame, m_vProStreamHeader[iChnNo]);

	if( pProFrame == NULL )
	{		
		m_stClientInfo.iLostFrames++;	
		return eERRNO_SYS_EFLOWOUT;
	}

	m_csWRMutex.LockWrite();
	if( !m_pTcpStreamSender || m_eStatus == eST_ASSERT )
	{
		m_csWRMutex.UnlockWrite();
		m_stClientInfo.iLostFrames++;
		pProFrame->UnrefObject();
		return eERRNO_NET_ECLOSED;
	}
	EnumErrno eErrno = m_csStreamWaitSendQueue.AddTail(pProFrame);
	if( eERRNO_SUCCESS != eErrno )
	{			
		GS_ASSERT(0);		
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
		m_csWRMutex.UnlockWrite();
		pProFrame->UnrefObject();
		Signal(eSIG_ASSERT);
		return eERRNO_SYS_ENMEM;
	}	
	m_iWaitSendStreamSize += pProFrame->GetTotalSize();



	if( !m_bStreamTcpSending )
	{
		//û���ڷ��ͣ� ��������
		std::vector<CProFrame *> vSends;
		void *pData = NULL;		
		do 
		{
			pData = NULL;
			m_csStreamWaitSendQueue.RemoveFront(&pData);
			CGspProFrame *p = (CGspProFrame*)pData;	
			if( p  )
			{		

				vSends.push_back(p);
			}
		} while( pData );
		m_iWaitSendStreamSize = 0;

		if( !vSends.empty() )
		{			
			m_bStreamTcpSending = TRUE;
			UINT32 iKey = m_iSendKeySeq++;
			eErrno = m_pTcpStreamSender->AsyncSend((void*)iKey, vSends);
			for(UINT i = 0; i<vSends.size(); i++ )
			{
				vSends[i]->UnrefObject();
			}

			if( eERRNO_SUCCESS != eErrno )
			{	
				m_bStreamTcpSending = FALSE;
				if( eErrno != eERRNO_NET_ECLOSED &&
					eErrno != eERRNO_NET_EDISCNN )
				{

					//GS_ASSERT(0);			
					MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����첽����ʧ��.\n"), m_iAutoID);
				}

				m_csWRMutex.UnlockWrite();
				Signal(eSIG_ASSERT);	
				return eERRNO_NET_EWEVT;			
			}
		}
	}
	m_csWRMutex.UnlockWrite();	
	return eERRNO_SUCCESS;
}

EnumErrno CGspSrvSession::SendMultiTcpStreamFrame(CFrameCache* pFrame, int iChnNo)
{

	CGspTcpEncoder csEncoder;

	if(  m_iWaitSendSize >= WRITE_FLOWOUT_SIZE && m_bTcpSending )
	{
		if( m_eTransModel == GSP_TRAN_RTPLAY )
		{
			
			m_stClientInfo.iLostFrames++;
		//	printf( "lose %d  %lld\n", m_stClientInfo.iLostFrames, (long long) m_iWaitSendSize);
			//ʵʱ���� ��֡			
			return eERRNO_SYS_EFLOWOUT;
		}

		while(  m_iWaitSendSize >= (WRITE_FLOWOUT_SIZE>>1) )
		{
			if( m_bStopStream )
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_SRC_EUNUSED; //����Ҫ��
			}
			MSLEEP(10);
			if( m_eStatus==eST_ASSERT)
			{
				m_stClientInfo.iLostFrames++;
				return eERRNO_NET_EDISCNN;
			}
			if( m_bCtrlIng )
			{
				break;
			}
		}
	}



	m_vProStreamHeader[iChnNo].iSeq++;
	m_vProStreamHeader[iChnNo].iExtraVal = (pFrame->m_stFrameInfo.bKey || pFrame->m_stFrameInfo.bSysHeader) ? 1 : 0;

	CGspProFrame *pProFrame =  csEncoder.Encode(pFrame, m_vProStreamHeader[iChnNo]);

	if( pProFrame == NULL )
	{		
		m_stClientInfo.iLostFrames++;	
		return eERRNO_SYS_EFLOWOUT;
	}	

	m_csWRMutex.LockWrite();
	if( !m_pTcpSocket || m_eStatus == eST_ASSERT )
	{
		m_csWRMutex.UnlockWrite();
		m_stClientInfo.iLostFrames++;
		pProFrame->UnrefObject();
		return eERRNO_NET_ECLOSED;
	}

	EnumErrno eErrno = m_csTcpWaitSendQueue.AddTail(pProFrame);
	if( eERRNO_SUCCESS != eErrno )
	{			
		GS_ASSERT(0);		
		MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
		m_csWRMutex.UnlockWrite();
		pProFrame->UnrefObject();
		Signal(eSIG_ASSERT);
		return eERRNO_SYS_ENMEM;
	}	
	m_iWaitSendSize += pProFrame->GetTotalSize();



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
		m_iWaitSendSize = 0;
	
		if( !vSends.empty() )
		{			
			m_bTcpSending = TRUE;
			UINT32 iKey = m_iSendKeySeq++;
			eErrno = m_pTcpSocket->AsyncSend((void*)iKey, vSends);
			for(UINT i = 0; i<vSends.size(); i++ )
			{
				vSends[i]->UnrefObject();
			}

			if( eERRNO_SUCCESS != eErrno )
			{	
				m_bTcpSending = FALSE;
				if( eErrno != eERRNO_NET_ECLOSED &&
					eErrno != eERRNO_NET_EDISCNN )
				{

					//GS_ASSERT(0);			
					MY_LOG_ERROR(g_pLog, _GSTX("GspSession(%u) �쳣. �����첽����ʧ��.\n"), m_iAutoID);
				}

				m_csWRMutex.UnlockWrite();
				Signal(eSIG_ASSERT);	
				return eERRNO_NET_EWEVT;			
			}
		}
	}
	m_csWRMutex.UnlockWrite();
	return eERRNO_SUCCESS;

}

void CGspSrvSession::DeleteBefore(void)
{
	
	m_bFlowCtrl = FALSE;
	m_bCtrlIng = FALSE;
	m_eStatus = eST_ASSERT;
	m_eSignal = eSIG_RELEASE; //ֹͣ������Ϣ
	m_bPlayCtrlIng = FALSE;
	m_eCtrlIngCmd = GSP_CTRL_PAUSE;
	StopAllAction();

	m_csWRMutex.LockWrite();
	
	CRefSource *pTemp = m_pRefSource;
	m_pRefSource = NULL;
	m_csWRMutex.UnlockWrite();

	if( pTemp )
	{
		pTemp->Release();
	}

	if( m_pTcpSocket )
	{
		m_pTcpSocket->Release();
		m_pTcpSocket = NULL;
	}
	
	if( m_pTcpDecoder )
	{
		delete m_pTcpDecoder;
		m_pTcpDecoder = NULL;
	}

	if( m_pAsyncSrvSocket )
	{
		m_pAsyncSrvSocket->Release();
		m_pAsyncSrvSocket = NULL;

	}
	if( m_pRtpUdpSender )
	{
		delete m_pRtpUdpSender;
		m_pRtpUdpSender = NULL;
	}

	if( m_pTcpStreamSender )
	{
		m_pTcpStreamSender->Release();
		m_pTcpStreamSender = NULL;
	}


	
	if( m_stClientInfo.iLostFrames > 0 )
	{
		MY_LOG_WARN(g_pLog, _GSTX("CGspSrvSession(%u) %s �ۼƶ�֡ (%lld).\n")
			,m_iAutoID,m_strURI.c_str(),  (long long ) m_stClientInfo.iLostFrames);
		m_stClientInfo.iLostFrames = 0;
	}

	
}


