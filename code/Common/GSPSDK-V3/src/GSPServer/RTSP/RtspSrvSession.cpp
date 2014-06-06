#include "RtspSrvSession.h"
#include "RTSP/RTSPAnalyer.h"
#include "RtspServer.h"
#include "Log.h"
#include "../RefSource.h"
#include "MediaInfo.h"
#include "../Server.h"
#include "GSPMemory.h"
#include "../IBaseSource.h"
#include "StrFormater.h"
#include "RTSP/RTSPProDebug.h"



using namespace GSP;
using namespace GSP::RTSP;

#define TIMER_ID_KEEPALIVE 1
#define TIMER_ID_SEND_KEEPALIVE 2 //����Keepalive
#define TIMER_ID_SEND_RTCP 3 //����RTCP

#define WRITE_FLOWOUT_SIZE  300  //д�������


static void _FreeWaitSendQueueMember( CRtspProPacket *p )
{
	SAFE_DESTROY_REFOBJECT(&p);
}

static INT ParserSoureKeyStreamID( CUri &csUri, INT iDefStreamID )
{
	const char *p;
	INT iRet = iDefStreamID;
	const CGSString &strKey=csUri.GetKey();

	if( (p=strstr(strKey.c_str(), "streamid="))  )
	{
		p += 9;
		iRet = atoi(p);	
		p = strKey.c_str();
		CGSString strNewKey;
		CStrFormater::GetWordSep(strNewKey,"/",&p);
		GS_ASSERT(!strNewKey.empty() );		
		csUri.SetKey(strNewKey.c_str());
	}
	return iRet;
}


CRtspSrvSession::CRtspSrvSession(CRtspServer *pProServer)
:CISrvSession(pProServer)
,m_pRtspServer(pProServer)
{
	m_pRefSource = NULL;
	m_pRtspTcpDecoder = NULL;
	
	m_csRtspTcpWaitSendQueue.SetFreeCallback((FuncPtrFree)_FreeWaitSendQueueMember);
	m_bRtspTcpSending = FALSE;
	m_bRtspTcpWaitSendFinish = FALSE;
	m_iWaitSendSize = 0;

	//�����µ�SessionID
	GSStrUtil::Format(m_strSessionID, "%ld", (long) m_iAutoID );

	m_eTransport = eTRANSPORT_RTP_UDP; //֧��
	m_eHttpStatus = eRTSPSTATE_INIT;
	m_eSignal = eSIG_NONE;
	bzero(&m_stPlayStatus, sizeof(m_stPlayStatus));  //��ǰ����״̬
	m_eTransModel = GSP_TRAN_RTPLAY; //����ģʽ
	m_pRtpSender = NULL;
	m_pRtspTcpSocket = NULL;
	m_iKeepalivePlugs = 0;
	m_bStopStream = FALSE;
	m_bFirstFrame = TRUE;
	m_fSpeed = 1.0;
	m_eOutRtpPlayloadType = (EnumRTPPayloadType)127;
	m_iSNSeq = 1;
	m_iFileBegin = 0;
	m_iFileEnd = 10000;

	m_eInputStreamPkt = eSTREAM_PKG_NONE;
	m_eOutputStreamPkt = eSTREAM_PKG_NONE;
	m_pPkgCvt = NULL;

	m_iSendKeySeq = 0;

	m_bCtrlIng = FALSE;

	m_bPlayEnd = FALSE;

	m_strUserAgent.clear();

	m_bSteup = FALSE;
	m_eCurAllowMask = (eRTSP_CMD_OPTIONS|eRTSP_CMD_DESCRIBE|eRTSP_CMD_TEARDOWN|eRTSP_CMD_SETUP|eRTSP_CMD_PLAY|eRTSP_CMD_PAUSE);

	

	MY_LOG_DEBUG(g_pLog, _GSTX("RtspSession(%u) Create.\n"), m_iAutoID);
}

CRtspSrvSession::~CRtspSrvSession(void)
{
	GS_ASSERT(m_pRtspTcpDecoder==NULL);
	GS_ASSERT(m_pRtspTcpSocket==NULL);		
	m_csRtspTcpWaitSendQueue.Clear();

	if( m_pRtpSender )
	{
		m_pRtpSender->Stop();
		delete m_pRtpSender;
		m_pRtpSender = NULL;

	}
	if( m_pPkgCvt )
	{
		delete  m_pPkgCvt;
		m_pPkgCvt = NULL;
	}

	MY_LOG_DEBUG(g_pLog, _GSTX("RtspSession(%u) Destory.\n"), m_iAutoID);
}


EnumErrno CRtspSrvSession::Init( CISocket *pRtspTcpSocket )
{
	CGSWRMutexAutoWrite wlocker(&m_csWRMutex);

	GS_ASSERT(NULL==m_pRtspTcpDecoder);

	m_pRtspTcpDecoder = new CRtspTcpDecoder(TRUE);
	m_strClientIPInfo = pRtspTcpSocket->GetDescri();

	MY_LOG_NOTICE(g_pLog, _GSTX("GspSession(%u) Init Of Client '%s'.\n"),
		m_iAutoID, m_strClientIPInfo.c_str() );

	GS_SNPRINTF(m_stClientInfo.szRemoteIP, GSP_STRING_IP_LEN, "%s", pRtspTcpSocket->RemoteIP());
	m_stClientInfo.iReomtePort = pRtspTcpSocket->RemotePort();

	if( !m_pRtspTcpDecoder )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u)  �ܾ� '%s' ����. �½� CRtspTcpDecoder ʧ��!!\n"),
			m_iAutoID, m_strClientIPInfo.c_str() );
		GS_ASSERT(0);
		return eERRNO_SYS_ENMEM;
	}

	m_csKeepaliveTimer.Init(this,
		(FuncPtrTimerCallback)&CRtspSrvSession::OnTimerEvent
		,TIMER_ID_KEEPALIVE ,
		1000L*(m_pRtspServer->m_stConfig.iKeepaliveTimeouts/3+1), 
		FALSE); 

	m_iKeepalivePlugs = 0;
	m_pRtspTcpSocket = pRtspTcpSocket;
	m_csKeepaliveTimer.Start(); //������ʱ��
	m_eHttpStatus =  eRTSPSTATE_WAIT_REQUEST;	
	return eERRNO_SUCCESS; 
}

void CRtspSrvSession::Start(void)
{
	m_csWRMutex.LockWrite();

	if( m_pRtspTcpSocket )
	{
		m_pRtspTcpSocket->SetListener( this,(FuncPtrSocketEvent) &CRtspSrvSession::OnRtspTcpSocketEvent);
	}
	if(NULL ==m_pRtspTcpSocket  || eERRNO_SUCCESS != m_pRtspTcpSocket->AsyncRcv(TRUE) )
	{
		m_eHttpStatus =  eRTSPSTATE_ASSERT;	
		m_bRtspTcpWaitSendFinish = FALSE;
		m_csWRMutex.UnlockWrite();
		//����ʧ��
		StopAllAction();
		m_pRtspServer->AsyncDestroySession(this);

	}
	else
	{
		m_csWRMutex.UnlockWrite();
	}
}

void CRtspSrvSession::OnRtpContentKeepalive(void)
{	
	m_iKeepalivePlugs = 0;
}

void *CRtspSrvSession::OnRtspTcpSocketEvent(	CISocket *pSocket, 
									   EnumSocketEvent iEvt,
									   void *pParam, void *pParamExt )
{
	switch(iEvt)
	{
	case  eEVT_SOCKET_ASEND :
		{               
			return (void*) HandleRtspTcpSocketWriteEvent( (StruAsyncSendEvent *) pParam );
		}
		break;
	case eEVT_SOCKET_ARCV :
		{
			// INT64 i = DoGetTickCount();           
			return (void*) HandleRtspTcpSocketReadEvent( (CGSPBuffer*) pParam );

			//             MY_PRINTF( "***%d, %lld, %lld, (%s), Thd:%d\n", pSocket->GetOSSocket(), (DoGetTickCount()-i), i
			//                 ,m_pClient->m_csURI.GetKey().c_str(), CURRENT_THREAD_ID );
		}
		break;

	case eEVT_SOCKET_ERR :
		{
			m_bRtspTcpWaitSendFinish = FALSE;
			m_bStopStream = TRUE;
			CNetError *pError = (CNetError *) pParam;
			MY_LOG_ERROR(g_pLog,  _GSTX("RtspSession(%u) Disconnect Errno:%d SysErrno:%d %s.\n"),
				m_iAutoID,pError->m_eErrno, pError->m_iSysErrno, pError->m_strError.c_str() );
			Signal(eSIG_REMOTE_DISCONNECT);

		}
	break;
		//�Զ˹ر�
	default :
		{
			m_bRtspTcpWaitSendFinish = FALSE;
			m_bStopStream = TRUE;
			//����
			MY_LOG_DEBUG(g_pLog, _GSTX("RtspSession(%u) Remote Disconnect. Evt:0x%x\n"), m_iAutoID, iEvt );
			Signal(eSIG_REMOTE_DISCONNECT);
		}
		break;
	}
	return NULL; //TODO
}

BOOL CRtspSrvSession::HandleRtspTcpSocketWriteEvent( StruAsyncSendEvent *pEvt )
{   
	m_stClientInfo.iSend += pEvt->iSends;
	if( m_eHttpStatus==eRTSPSTATE_ASSERT && !m_bRtspTcpWaitSendFinish )
	{
		//�Ѿ��ر�
		m_csWRMutex.LockWrite();
		m_bRtspTcpSending = FALSE;
		m_csWRMutex.UnlockWrite();
		return FALSE;
	}

	//����TCP д�¼�
	if( m_csRtspTcpWaitSendQueue.IsEmpty() )
	{
		// ���Ͷ���Ϊ��
		m_csWRMutex.LockWrite();
		m_bRtspTcpSending = FALSE;
		m_csWRMutex.UnlockWrite();
		return FALSE;
	}

	
	//������һ��
	m_csWRMutex.LockReader();  
	std::vector<CProPacket *> vSends;
	void *pData = NULL;
	UINT32 iSeq = m_iSendKeySeq++;
	do 
	{
		pData = NULL;
		m_csRtspTcpWaitSendQueue.RemoveFront(&pData);
		CRtspProPacket *p = (CRtspProPacket*)pData;	
		if( p  )
		{			
			vSends.push_back(p);
		}
	} while( pData );
	m_iWaitSendSize = 0;

	m_csWRMutex.UnlockReader();
	if( !vSends.empty() )
	{			
		m_pRtspTcpSocket->AsyncSend((void*)iSeq, vSends);
		for(UINT i = 0; i<vSends.size(); i++ )
		{
			vSends[i]->UnrefObject();
		}
	}
	return NULL;
}


BOOL CRtspSrvSession::HandleRtspTcpSocketReadEvent( CGSPBuffer *pBuffer )
{
	m_iKeepalivePlugs = 0;
	m_stClientInfo.iRecv += pBuffer->m_iDataSize; 
	if( m_eHttpStatus==eRTSPSTATE_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	EnumErrno eErrno;
	BOOL bRet = TRUE;
	eErrno = m_pRtspTcpDecoder->Decode(pBuffer);

	if( eERRNO_SUCCESS != eErrno )
	{
		//ʧ��
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) ����Э�����!!!"), m_iAutoID );
		GS_ASSERT(0);	
		Signal(eSIG_ASSERT);
		return FALSE;
	}

	if( m_eHttpStatus==eRTSPSTATE_ASSERT )
	{
		//�Ѿ��ر�
		return FALSE;
	}
	
	//�ɹ�
	CRtspProPacket *pPacket;

	while( m_eHttpStatus != eRTSPSTATE_ASSERT  &&
		NULL != (pPacket = m_pRtspTcpDecoder->Pop()) )
	{
		HandleRtspTcpCommand(pPacket);
		pPacket->UnrefObject();
	}


	return FALSE;
}


void CRtspSrvSession::HandleRtspTcpCommand( CRtspProPacket *pPacket )
{
	//�����
	CRtspHeader &csHeader = pPacket->GetHeader();
	if( !csHeader.GetExistParser(eRTSP_H_TYPE_Request) )
	{
		//����Ч����
		GS_ASSERT(0);
		CloseOfBadRequest();
		return;
	}
	if( m_strUserAgent.empty() )
	{
		//��ȡ�ͻ�������
		if( csHeader.GetExistParser(eRTSP_H_TYPE_UserAgent) )
		{
			m_strUserAgent = csHeader.m_csUserAgent.m_strAgent;
		}
		else
		{
			GS_ASSERT(0);
		}
	}
	EnumRTSPComandMask eCmd = CIRtspLineParser::GetRtspCommandMask(csHeader.m_csRequest.m_strMethod.c_str() );
	switch( (eCmd&m_eCurAllowMask))
	{
	case eRTSP_CMD_OPTIONS :
		{
			HandleOptions(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_DESCRIBE :
		{
			HandleDescribe(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_TEARDOWN :
		{
			HandleTeardown(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_SETUP :
		{
			HandleSetup(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_PLAY :
		{

			HandlePlay(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_PAUSE :
		{

			HandlePause(&csHeader, pPacket);

		}
		break;
	case eRTSP_CMD_GET_PARAMETER :
		{
			HandleGetParameter(&csHeader, pPacket);
		}
		break;
	case eRTSP_CMD_SET_PARAMETER :
		{
			HandleSetParameter(&csHeader, pPacket);
		}
		break;
	default :
		{
			HandleInvalidCmmand(&csHeader, pPacket);
		}
		break;
	}	
}


void CRtspSrvSession::Signal(EnumSignal eSig )
{
	m_csWRMutex.LockWrite();
	if( m_eHttpStatus==eRTSPSTATE_ASSERT || m_eSignal != eSIG_NONE )
	{
		//�Ѿ������쳣
		if( eSig == eSIG_REMOTE_DISCONNECT )
		{
			m_bRtspTcpWaitSendFinish = FALSE;
		}
		m_csWRMutex.UnlockWrite();
		return;
	}

	if( eSig == eSIG_REMOTE_CLOSE  )
	{
		m_bRtspTcpWaitSendFinish = TRUE;
	}
	m_eHttpStatus=eRTSPSTATE_ASSERT;

	GS_ASSERT(m_eSignal == eSIG_NONE );

	m_eSignal = eSig;
	
	m_csWRMutex.UnlockWrite();

	if( eSig == eSIG_REMOTE_CLOSE )
	{
		if( m_pRefSource )
		{
			m_pRefSource->Stop(); //TODO
		}
		//�ȴ���������		
		INT iTrys = 1000;
		while(!m_bStopStream && m_bRtspTcpSending && m_bRtspTcpWaitSendFinish && iTrys-- > 0 )
		{
			//�ȴ����ݷ������
			MSLEEP(10);
		}
		
		GS_ASSERT(iTrys>0);
	}
	m_bRtspTcpWaitSendFinish = FALSE;
	StopAllAction();
	m_pRtspServer->AsyncDestroySession(this);

	
}


void CRtspSrvSession::StopAllAction(void)
{

	m_csKeepaliveTimer.Stop();
	if( m_pRtspTcpSocket )
	{
		m_pRtspTcpSocket->Disconnect();
	}	

	if( m_pRefSource )
	{
		m_pRefSource->Stop();
	}	
	if( m_pRtpSender )
	{
		m_pRtpSender->Stop();
	}
}

void CRtspSrvSession::OnTimerEvent( CWatchTimer *pTimer )
{
	switch(pTimer->GetID() )
	{
	case TIMER_ID_KEEPALIVE :
		{
			//���      
			m_iKeepalivePlugs++;
			if( m_iKeepalivePlugs > 4 )
			{							
			    Signal(eSIG_ASSERT);
			}
		}
		break;
	}
}

EnumErrno CRtspSrvSession::OnSourceEvent(CRefSource *pRefSource,
						CRefSource::EnumRefSourceEvent eEvt, void *pParam)
{
	switch(eEvt)
	{
	case CRefSource::eEVT_STREAM_FRAME :
		{
			//����֡
			//����֡
			while( m_bCtrlIng  )
			{
				if( m_eTransModel == GSP_TRAN_RTPLAY )
				{
					return eERRNO_SRC_EUNUSED;
				}
				MSLEEP(5);
			}
			return SendStreamFrame( (CFrameCache*) pParam);
		}
	break;
	case CRefSource::eEVT_PLAY_END :
		{
			if( m_eTransModel!=GSP_TRAN_RTPLAY && !m_bPlayEnd )
			{
				m_bPlayEnd = TRUE;
				// TODO ...
				MSLEEP(20);
			}
		}
		break;

	case CRefSource::eEVT_PLAY_STATUS :
		{
			//����״̬ TODO...
			::memcpy(&m_stPlayStatus, pParam, sizeof(m_stPlayStatus));
		//	SendCommand(GSP_CMD_ID_RET_STATUS, &m_stPlayStatus, sizeof(m_stPlayStatus));
		}
		break;
	case CRefSource::eEVT_SOURCE_RELEASE :
		{
			//����Դ���ͷ�	TODO...
 			m_bStopStream = TRUE;
			m_csWRMutex.LockWrite();
			if( m_pRtpSender )
			{
				m_pRtpSender->Stop();
			}
			if( m_pRefSource)
			{
				delete m_pRefSource;
				m_pRefSource = NULL;
			}
			m_csWRMutex.UnlockWrite();
			Signal(eSIG_RELEASE);
		}
		break;
	case CRefSource::eEVT_SOURCE_ASSERT :
		{
			//����Դ�쳣 TODO...
			m_bStopStream = TRUE;
			Signal(eSIG_ASSERT);
		}
		break;
	default :
		;
	}
	return eERRNO_SUCCESS;
}


EnumErrno CRtspSrvSession::SendStreamFrame( CFrameCache* pFrame )
{
	//������		
	if( !m_pRtpSender  || !m_pPkgCvt)
	{
		return eERRNO_SRC_EUNUSED;
	}


	EnumErrno eRet = m_pPkgCvt->Conver(pFrame, TRUE);
	if( eRet )
	{
		return eRet;
	}

	CFrameCache *pNewFrame;



	while(!m_bStopStream && (pNewFrame = m_pPkgCvt->Get())  )
	{		

		if( pNewFrame->m_stFrameInfo.eMediaType != GS_MEDIA_TYPE_VIDEO &&
			(m_eOutputStreamPkt == eSTREAM_PKG_Standard || 
			(m_eTransModel == GSP_TRAN_REPLAY && (m_fSpeed<0.99 || m_fSpeed>1.01 ) ) ) )
		{
			//������Ƶ
			pNewFrame->UnrefObject();
			continue;
		}

		m_pRtpSender->Send(pNewFrame);

		if( pNewFrame->m_stFrameInfo.eMediaType == GS_MEDIA_TYPE_SYSHEADER )
		{
			if( m_bFirstFrame )
			{
				MSLEEP(1);
				m_pRtpSender->Send(pNewFrame);
				MSLEEP(1);
				m_pRtpSender->Send(pNewFrame);
				m_bFirstFrame = FALSE;
			}			
		}
		pNewFrame->UnrefObject();
	}
	return eERRNO_SUCCESS;	

}


void CRtspSrvSession::DeleteBefore(void)
{
	m_eHttpStatus = eRTSPSTATE_ASSERT;
	m_eSignal = eSIG_RELEASE; //ֹͣ������Ϣ

	StopAllAction();



	m_csWRMutex.LockWrite();
	
	CRefSource *pTemp = m_pRefSource;
	m_pRefSource = NULL;
	m_csWRMutex.UnlockWrite();

	if( pTemp )
	{
		delete pTemp;
	}

	if( m_pRtspTcpSocket )
	{
		m_pRtspTcpSocket->Release();
		m_pRtspTcpSocket = NULL;
	}
	
	if( m_pRtspTcpDecoder )
	{
		delete m_pRtspTcpDecoder;
		m_pRtspTcpDecoder = NULL;
	}

	if( m_pRtpSender )
	{
		delete  m_pRtpSender;
		m_pRtpSender = NULL;
	}
	
}


void CRtspSrvSession::SendRtspCmdPacket( CRtspProPacket *pPacket )
{
	m_csWRMutex.LockWrite();
	if( !m_pRtspTcpSocket || m_eHttpStatus == eRTSPSTATE_ASSERT )
	{
		m_csWRMutex.UnlockWrite();
		return;
	}
	//�ӵ�д�ȴ�����
EnumErrno eErrno;
	if( m_bRtspTcpSending )
	{

		pPacket->RefObject();
		eErrno = m_csRtspTcpWaitSendQueue.AddTail(pPacket);
		if( eERRNO_SUCCESS != eErrno )
		{			
			
			pPacket->UnrefObject();
			MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) �쳣. �����ڴ�ʧ��.\n"), m_iAutoID);
			GS_ASSERT(0);
			m_csWRMutex.UnlockWrite();
			Signal(eSIG_ASSERT);
			return;
		}
		m_iWaitSendSize += pPacket->GetContentLenght();
	}
	else
	{
		//û���ڷ��ͣ� ��������
		m_bRtspTcpSending = TRUE;
		std::vector<CProPacket *> vSends;
		vSends.push_back(pPacket);
		UINT32 iSeq = m_iSendKeySeq++;		
		eErrno = m_pRtspTcpSocket->AsyncSend((void*)iSeq, vSends);		
		if( eERRNO_SUCCESS != eErrno )
		{			
			
			m_bRtspTcpSending = FALSE;
			if( eErrno != eERRNO_NET_ECLOSED && eErrno!= eERRNO_NET_EDISCNN)
			{
				//GS_ASSERT(0);		
				MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) �쳣. �����첽����ʧ��.\n"), m_iAutoID);
			}	
			m_csWRMutex.UnlockWrite();
			Signal(eSIG_ASSERT);	
			return;			
		}

	}
	m_csWRMutex.UnlockWrite();
}


void CRtspSrvSession::CloseOfBadRequest(void)
{
	CRtspHeader stHeader;
	stHeader.LineParser(eRTSP_H_TYPE_CSeq);
	ResponseError(&stHeader,eRTSP_BAD_REQUEST );
	Signal(eSIG_REMOTE_CLOSE);
}


void CRtspSrvSession::HandleOptions( CRtspHeader *pHeader,   CRtspProPacket *pRcvPacket  )
{
	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq) ||
		!csHeader.LineParser(eRTSP_H_TYPE_Allow) )
	{
		GS_ASSERT(0);
		return;
	}
	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";

	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;
	csHeader.m_csAllow.m_eAllowMask = eRTSP_CMD_OPTIONS;
	csHeader.m_csAllow.m_eAllowMask |= eRTSP_CMD_DESCRIBE;
	csHeader.m_csAllow.m_eAllowMask |= eRTSP_CMD_SETUP;
	csHeader.m_csAllow.m_eAllowMask |= eRTSP_CMD_TEARDOWN;
	csHeader.m_csAllow.m_eAllowMask |= eRTSP_CMD_PLAY;
	csHeader.m_csAllow.m_eAllowMask |= eRTSP_CMD_PAUSE;

	CRtspProPacket *pProPacket = pProPacket->Create(csHeader);
	if( !pProPacket )
	{
		GS_ASSERT(0);
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) CRtspIPacket::Create fail.\n"), m_iAutoID );
		Signal(eSIG_ASSERT);
		return;
	}
	if( !pProPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pProPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_INTERNAL_SERVER_ERROR);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );
	SendRtspCmdPacket(pProPacket);
	pProPacket->UnrefObject();
}


void CRtspSrvSession::HandleDescribe( CRtspHeader *pHeader,  CRtspProPacket *pRcvPacket )
{
	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq) ||		
		!csHeader.LineParser(eRTSP_H_TYPE_ContenLength) ||
		!csHeader.LineParser(eRTSP_H_TYPE_ContenType) ||
		!csHeader.LineParser(eRTSP_H_TYPE_Server)  )
	{
		GS_ASSERT(0);
		return;
	}


	CUri csUri;
	if( !csUri.Analyse(pHeader->m_csRequest.m_strUrl.c_str() ) )
	{
		GS_ASSERT(0);
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u)  Rtsp Describe Url '%s' is invalid.\n"), 
			m_iAutoID,
			pHeader->m_csRequest.m_strUrl.c_str() );
		ResponseError(pHeader, eRTSP_HEADER_FIELD_NOT_VALID );
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	GS_ASSERT(m_pRefSource==NULL);

	if( m_pRefSource )
	{
		m_pRefSource->Stop();
		delete m_pRefSource;
		m_pRefSource = NULL;
	}

	CGSString strReqSdp;

	if( pHeader->m_csContentType.m_eContentType == eCONT_TYPE_SDP  )
	{
		strReqSdp = pRcvPacket->GetStrContent();
	}

	EnumErrno eRet = BuildSource(csUri, pHeader, strReqSdp);
	if( eRet )
	{
		GS_ASSERT(0);
		return;
	}


	CGSString strSdp = m_csSdp.Serial();

	

	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";

	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;

	csHeader.m_csServer.m_strServer = "Server: GSPS V1.0 of RTSP";

	csHeader.m_csContenLength.m_iLength = strSdp.length();
	csHeader.m_csContentType.m_eContentType = eCONT_TYPE_SDP;


	CRtspProPacket *pProPacket = pProPacket->Create(csHeader);
	if( !pProPacket )
	{		
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_INTERNAL_SERVER_ERROR );
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}
	pProPacket->SetContent(strSdp);

	if( !pProPacket->Packet() )
	{		
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u)  CRtspProFrame::Packet fail.\n"), m_iAutoID  );
		GS_ASSERT(0);
		pProPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_INTERNAL_SERVER_ERROR );
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

#ifdef RTSP_DEBUG_STEP
	CGSString strDebug = csHeader.Serial();
	strDebug+= pProPacket->GetStrContent();
	RTSP_CMD_DEBUG_PRINTF( "Response",strDebug.c_str() );
#endif
	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pProPacket);
	pProPacket->UnrefObject();	
}





void CRtspSrvSession::HandleTeardown(CRtspHeader *pHeader,  CRtspProPacket *pRcvPacket  )
{
	if( pHeader->m_csSession.m_strSession != m_strSessionID  )
	{
		//�Ƿ�����
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Rtsp %s Teardown invalid session id: %s!=%s\n"),
			m_iAutoID,
			m_strClientIPInfo.c_str(),
			pHeader->m_csSession.m_strSession.c_str(), m_strSessionID.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_SESSION_NOT_FOUND );
		return;
	}

	CUri csUri;

	if( !csUri.Analyse(pHeader->m_csRequest.m_strUrl.c_str() ) )
	{
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Rtsp %s Teardown invalid url '%s'\n"),
			m_iAutoID,
			m_strClientIPInfo.c_str(),pHeader->m_csRequest.m_strUrl.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_HEADER_FIELD_NOT_VALID);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	int iStreamId = -1;
	iStreamId = ParserSoureKeyStreamID(csUri, iStreamId);
	CGSString strKey = csUri.GetKey();

	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_Session) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq) )
	{
		GS_ASSERT(0);
		return;
	}

	m_pRefSource->Stop();
	if( m_pRtpSender)
	{
		m_pRtpSender->Stop();		
	}

	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";
	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;
	csHeader.m_csSession.m_strSession = m_strSessionID;

	CRtspProPacket *pProPacket = pProPacket->Create(csHeader);
	if( !pProPacket )
	{
		
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_INTERNAL_SERVER_ERROR);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}
	if( !pProPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pProPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_INTERNAL_SERVER_ERROR);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );
	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pProPacket);
	pProPacket->UnrefObject();

}



void CRtspSrvSession::HandleSetup( CRtspHeader *pHeader, CRtspProPacket *pRcvPacket   )
{
	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq) ||	
		!csHeader.LineParser(eRTSP_H_TYPE_Session) ||	
		!csHeader.LineParser(eRTSP_H_TYPE_Transport) )

	{
		GS_ASSERT(0);
		return;
	}


	CUri csUri;

	if( !csUri.Analyse(pHeader->m_csRequest.m_strUrl.c_str() ) )
	{
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Rtsp %s Teardown invalid url '%s'\n"),m_iAutoID,
			pHeader->m_csRequest.m_strUrl.c_str() ,pHeader->m_csRequest.m_strUrl.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_HEADER_FIELD_NOT_VALID);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	int iStreamId = 0;
	iStreamId = ParserSoureKeyStreamID(csUri, iStreamId);
	CGSString strKey = csUri.GetKey();

	if(iStreamId<0 || iStreamId>=GSP_MAX_MEDIA_CHANNELS )
	{


		MY_LOG_WARN(g_pLog, 
			_GSTX("RtspSession(%u)  Request: URI(%s) Setup streamid %d flowout.\n"),
				m_iAutoID,
				csUri.GetURI(), iStreamId );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_BAD_REQUEST);
		return;

	}

	if( m_pRefSource )
	{
		if( 0 != strKey.compare(m_pRefSource->Source()->GetKey()) && strKey.find("streamid=")<0 )
		{


			MY_LOG_WARN(g_pLog, 
				_GSTX("RtspSession(%u)  Request: URI(%s) Setup Souce Key %s!=[%s] Illegal .\n"),
				m_iAutoID,csUri.GetURI(),
				m_pRefSource->Source()->GetKey(), strKey.c_str() );
			GS_ASSERT(0);
			ResponseError(pHeader, eRTSP_ILLEGAL_CONFERENCE_IDENTIFIER);
			Signal(eSIG_REMOTE_CLOSE);
			return;
		}
	}
	else 
	{
		CGSString strReqSdp;

		if( csHeader.LineParser(eRTSP_H_TYPE_ContenType) )
		{
			strReqSdp = pRcvPacket->GetStrContent();
		}
		if( BuildSource(csUri, pHeader, strReqSdp) )
		{
			GS_ASSERT(0);
			return;
		}
	}


	if( m_bSteup ) {
		MY_LOG_WARN(g_pLog, _GSTX("RtspSession(%u)  Request: URI(%s : %d) Session %s had setup.\n"),
			m_iAutoID,csUri.GetURI(),iStreamId, m_strSessionID.c_str() );
		ResponseError(pHeader,eRTSP_METHOD_NOT_VALID_IN_THIS_STATE);
		return;
	}

	if( !m_pRtpSender)
	{
		m_pRtpSender->Create(m_pRtspServer->m_stConfig.m_iRtpUdpPortBegin,
			              m_pRtspServer->m_stConfig.m_iRtpUdpPortEnd, 
						  NULL, RTSP_IS_GXX_USERAGENT(m_strUserAgent.c_str()), FALSE );
		if( !m_pRtpSender == NULL )
		{
			MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u)  Request: URI(%s : %d) Session %s Create RTP Socket fail.\n"),
				m_iAutoID,csUri.GetURI(),iStreamId, m_strSessionID.c_str() );
			ResponseError(pHeader,eRTSP_LOW_ON_STORAGE_SPACE);
			return;
		}
	}

	CRtspTransportParser::StruTransPortField *pTransPortFiled = NULL;
	for( UINT i=0; i<pHeader->m_csTransPort.m_vFields.size(); i++  )
	{
		if( pHeader->m_csTransPort.m_vFields[i].eTrType == eTRANSPORT_RTP_UDP 			
			/*|| pHeader->m_csTransPort.m_vFields[i].eTrType == eRTSP_TRANSPORT_RTP_TCP*/)
		{
			m_eTransport = pHeader->m_csTransPort.m_vFields[i].eTrType;
			pTransPortFiled = &pHeader->m_csTransPort.m_vFields[i];
			break;
		}
	}

	if( !pTransPortFiled || pTransPortFiled->stCliPort.vPort[RTP_PORT_IDX]<=0 )
	{
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Request: URI(%s) not support rtp protocol:%d.\n"),
			m_iAutoID,csUri.GetURI(), m_eTransport );
		ResponseError(pHeader, eRTSP_UNSUPPORTED_TRANSPORT);
		return;
		//��֧�ֵĴ���
	}

	m_strRemoteRtpIp = m_pRtspTcpSocket->RemoteIP();
	m_pRtpSender->SetRemoteAddress(m_pRtspTcpSocket->RemoteIP(),
		pTransPortFiled->stCliPort.vPort[RTP_PORT_IDX],
		pTransPortFiled->stCliPort.vPort[RTCP_PORT_IDX]  );


	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";

	csHeader.m_csSession.m_strSession = m_strSessionID;

	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;

	pTransPortFiled->stSrvPort.vPort[RTP_PORT_IDX] = m_pRtpSender->GetRtpSocket()->LocalPort();
	if( m_pRtpSender->GetRtcpSocket())
	{
		pTransPortFiled->stSrvPort.vPort[RTCP_PORT_IDX] = m_pRtpSender->GetRtcpSocket()->LocalPort();
	}
	else
	{
		pTransPortFiled->stSrvPort.vPort[RTCP_PORT_IDX]  = 0;
	}
	pTransPortFiled->strSrvAddr = COSSocket::GuessLocalIp(m_pRtspTcpSocket->RemoteIP(), "0");	
	pTransPortFiled->strDestAddr = m_pRtspTcpSocket->RemoteIP();
	csHeader.m_csTransPort.m_vFields.push_back(*pTransPortFiled);


	CRtspProPacket *pProPacket = pProPacket->Create(csHeader);
	if( !pProPacket )
	{
		
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Request CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;
		
	}
	if( !pProPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Request CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pProPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	m_bSteup = TRUE;
	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );
	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pProPacket);
	pProPacket->UnrefObject();

}

#if 0
void CRtspSrvSession::HandlePlay( CRtspHeader *pHeader, const CGSString &strContent )
{
	if( pHeader->m_csSession.m_strSession != m_strSessionID  )
	{
		//�Ƿ�����
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Play invalid session id: %s!=%s\n"),
			m_iAutoID,
			pHeader->m_csSession.m_strSession.c_str(), m_strSessionID.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_SESSION_NOT_FOUND );
		return;
	}

	CUri csUri;

	if( !csUri.Analyse(pHeader->m_csRequest.m_strUrl.c_str() ) )
	{
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Play invalid url '%s'\n"),
			m_iAutoID, pHeader->m_csRequest.m_strUrl.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_HEADER_FIELD_NOT_VALID);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	int iStreamId = 0;
	iStreamId = ParserSoureKeyStreamID(csUri,iStreamId);
	CGSString strKey = csUri.GetKey();

	if(iStreamId<0 || iStreamId>=GSP_MAX_MEDIA_CHANNELS ||
		!m_pContent->IsExistStream(iStreamId) )
	{


		MY_LOG_WARN(g_pLog, 
			_GSTX("RtspSession(%u) Play  streamid %d invalid.\n"),
			m_iAutoID, iStreamId );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_BAD_REQUEST);
		return;

	}
	StruGSPCmdCtrl stCtrl;
	bzero(&stCtrl, sizeof(stCtrl));
	stCtrl.iCtrlID = GSP_CTRL_PLAY;
	stCtrl.iSubChn = iStreamId;
	


	if( m_pContent->Play(iStreamId) )
	{
		MY_LOG_WARN(g_pLog, 
			_GSTX("RtspSession(%u) Play  streamid %d rtp content play fail.\n"),
			m_iAutoID, iStreamId );
		ResponseError(pHeader, eRTSP_METHOD_NOT_VALID_IN_THIS_STATE);
		GS_ASSERT(0);
		return;
	}
	
	if( eERRNO_SUCCESS != m_pRefSource->Ctrl(stCtrl) )
	{
		MY_LOG_WARN(g_pLog, 
			_GSTX("RtspSession(%u) Play streamid %d Ctrl source %s  fail.\n"),
			m_iAutoID, iStreamId, m_pRefSource->Source()->GetKey() );
		ResponseError(pHeader, eRTSP_METHOD_NOT_VALID_IN_THIS_STATE);
		GS_ASSERT(0);
		return;
	}

	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq)  ||
		!csHeader.LineParser(eRTSP_H_TYPE_Session) )
	{
		GS_ASSERT(0);
		return;
	}

	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";

	csHeader.m_csSession.m_strSession = m_strSessionID;
	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;

	CRtspIPacket *pPacket = CRtspIPacket::Create(csHeader);
	if( !pPacket )
	{

		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Play CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;

	}
	if( !pPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Play CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}
	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );
	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pPacket);
	m_csRTCPKeepalvie.Stop();
	pPacket->UnrefObject();	
}


void CRtspSrvSession::HandlePause(CRtspHeader *pHeader, const CGSString &strContent)
{
	if( pHeader->m_csSession.m_strSession != m_strSessionID  )
	{
		//�Ƿ�����
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Pause invalid session id: %s!=%s\n"),
			m_iAutoID,
			pHeader->m_csSession.m_strSession.c_str(), m_strSessionID.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_SESSION_NOT_FOUND );
		return;
	}

	CUri csUri;

	if( !csUri.Analyse(pHeader->m_csRequest.m_strUrl.c_str() ) )
	{
		MY_LOG_ERROR(g_pLog, _GSTX("RtspSession(%u) Pause invalid url '%s'\n"),
			m_iAutoID, pHeader->m_csRequest.m_strUrl.c_str() );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_HEADER_FIELD_NOT_VALID);	
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	int iStreamId = 0;

	iStreamId = ParserSoureKeyStreamID(csUri,iStreamId);
	CGSString strKey = csUri.GetKey();

	if(iStreamId<0 || iStreamId>=GSP_MAX_MEDIA_CHANNELS ||
		!m_pContent->IsExistStream(iStreamId) )
	{


		MY_LOG_WARN(g_pLog, _GSTX("RtspSession(%u) Pause  streamid %d invalid.\n"),
			m_iAutoID, iStreamId );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_BAD_REQUEST);
		return;

	}
	StruGSPCmdCtrl stCtrl;
	bzero(&stCtrl, sizeof(stCtrl));
	stCtrl.iCtrlID = GSP_CTRL_PAUSE;
	stCtrl.iSubChn = iStreamId;


	if( m_pContent->Pause(iStreamId) )
	{
		MY_LOG_WARN(g_pLog, _GSTX("RtspSession(%u) Pause streamid %d Rtp Content pause fail.\n"),
			m_iAutoID, iStreamId );
		ResponseError(pHeader, eRTSP_METHOD_NOT_VALID_IN_THIS_STATE);
		GS_ASSERT(0);
		return;
	}

	if( eERRNO_SUCCESS != m_pRefSource->Ctrl(stCtrl) )
	{
		MY_LOG_WARN(g_pLog, 
			_GSTX("RtspSession(%u) Play streamid %d Ctrl source %s  fail.\n"),
			m_iAutoID, iStreamId, m_pRefSource->Source()->GetKey() );
		ResponseError(pHeader, eRTSP_METHOD_NOT_VALID_IN_THIS_STATE);
		GS_ASSERT(0);
		return;
	}


	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq)  ||
		!csHeader.LineParser(eRTSP_H_TYPE_Session) )
	{
		GS_ASSERT(0);
		return;
	}

	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eRTSP_OK;
	csHeader.m_csResponse.m_strError = "OK";

	csHeader.m_csSession.m_strSession = m_strSessionID;
	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;




	CRtspIPacket *pPacket = CRtspIPacket::Create(csHeader);
	if( !pPacket )
	{

		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Pause CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;

	}
	if( !pPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) Pause CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );

	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pPacket);
	m_csRTCPKeepalvie.Start();
	pPacket->UnrefObject();	
}


void CRtspSrvSession::HandleGetParameter( CRtspHeader *pHeader, const CGSString &strContent )
{
	//TODO...
	ResponseError(pHeader, eRTSP_METHOD_NOT_ALLOWED);

}

void CRtspSrvSession::HandleSetParameter(CRtspHeader *pHeader, const CGSString &strContent)
{
	//TODO...
	ResponseError(pHeader, eRTSP_METHOD_NOT_ALLOWED);
}


void CRtspSrvSession::HandleInvalidCmmand(CRtspHeader *pHeader, const CGSString &strContent)
{
	ResponseError(pHeader, eRTSP_METHOD_NOT_ALLOWED);
}

void CRtspSrvSession::ResponseError(const CRtspHeader *pHeader, EnumResponseStatus eErrno )
{
	CRtspHeader csHeader;
	if( !csHeader.LineParser(eRTSP_H_TYPE_Response) ||
		!csHeader.LineParser(eRTSP_H_TYPE_CSeq)  )
	{
		GS_ASSERT(0);
		return;
	}

	if( pHeader->GetExistParser(eRTSP_H_TYPE_Session) )
	{
		csHeader.LineParser(eRTSP_H_TYPE_Session);
		csHeader.m_csSession.m_strSession = m_strSessionID;
	}

	csHeader.m_csResponse.m_strVersion = RTSP_VERSION;
	csHeader.m_csResponse.m_eResNo = eErrno;
	csHeader.m_csResponse.m_strError = RtspGetResponseStatusName(eErrno);

	csHeader.m_csCSeq.m_strCSeq = pHeader->m_csCSeq.m_strCSeq;


	CRtspIPacket *pPacket = CRtspIPacket::Create(csHeader);
	if( !pPacket )
	{

		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) ResponseError CRtspIPacket::Create fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;

	}
	if( !pPacket->Packet() )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("RtspSession(%u) ResponseError CRtspIPacket::Packet fail.\n"), m_iAutoID );
		GS_ASSERT(0);
		pPacket->UnrefObject();
		ResponseError(pHeader, eRTSP_LOW_ON_STORAGE_SPACE);
		Signal(eSIG_REMOTE_CLOSE);
		return;
	}

	RTSP_CMD_DEBUG_PRINTF( "Response",csHeader.Serial().c_str() );

	m_eHttpStatus = eHTTPSTATE_SEND_DATA;
	SendRtspCmdPacket(pPacket);
	m_csRTCPKeepalvie.Start();
	pPacket->UnrefObject();	


}



EnumErrno CRtspSrvSession::BuildSource(const CUri &csUri, 
									   const CRtspHeader *pHeader, 
									   const CGSString &strContent  )
{
EnumErrno eErrno;

	GS_ASSERT_RET_VAL(m_pRefSource==NULL, eERRNO_SYS_ESTATUS);

	m_eTransModel  = GSP_TRAN_RTPLAY;




	const StruUriAttr *pAttr = csUri.GetAttr("TrMode");
	if( pAttr )
	{
		if( 0==strcasecmp(pAttr->szValue,"PlayBack") )
		{
			m_eTransModel  = GSP_TRAN_REPLAY;
		}
		else if(  0==strcasecmp(pAttr->szValue,"Download") )
		{
			m_eTransModel  = GSP_TRAN_DOWNLOAD;
		}
		else if(0==strcasecmp(pAttr->szValue,"Realtime") )
		{
			m_eTransModel  = GSP_TRAN_RTPLAY;
		}
		else
		{
			GS_ASSERT(0);
		}
	}

	CMediaInfo csMdInfo;
	CIRtspLineParser *pLiner = pHeader->GetExistParser(eRTSP_H_TYPE_ContenType);
	if( NULL != pLiner && pHeader->m_csContentType.m_eContentType == eCONT_TYPE_SDP )
	{
		//�������ý����Ϣ
		CSDPFormater csFtm;
		eErrno = csFtm.ParserFrom(strContent);
		if( eERRNO_SUCCESS == eErrno)
		{
			eErrno = csFtm.SerialToInfo(csMdInfo);
		}
		GS_ASSERT(eErrno==eERRNO_SUCCESS );
	}

	eErrno =  m_pRtspServer->m_pServer->RequestSource( csUri, csMdInfo, m_eTransModel, &m_pRefSource);

	if(eErrno == eERRNO_SUCCESS)
	{
		m_pRefSource->SetListener(this,(CRefSource::FuncPtrRefSourceEvent) &CRtspSrvSession::OnSourceEvent);
		m_pRefSource->Start();
	}
	else
	{
		EnumResponseStatus iErr = eRTSP_BAD_REQUEST;
		switch(eErrno  )
		{		
		case eERRNO_SYS_EPERMIT :
			//û��Ȩ��
			iErr = eRTSP_UNAUTHORIZED;
			break;
		case eERRNO_SRC_ECODER :
			iErr = eRTSP_UNSUPPORTED_MEDIA_TYPE;
			break;
		case eERRNO_SRC_ENXIST :
			//�����ڵ�URI
			iErr = eRTSP_NOT_FOUND;
			break;
		default :
			break;
		}
		ResponseError(pHeader, iErr);  
		Signal(eSIG_REMOTE_CLOSE);		
	}
	return eErrno;	
}

CGSString CRtspSrvSession::GetSDPDescription( CUri &csUri )
{
	CSDPFormater csFormater;
	CGSString strRet;

	GS_ASSERT(m_pRefSource);
	const CIMediaInfo &csInfo = m_pRefSource->Source()->MediaInfo();
	csFormater.FormatFrom(csInfo, m_pRtspTcpSocket->LocalIP(), csUri, 0);
	strRet = csFormater.Serial();
	return strRet;
}

#endif