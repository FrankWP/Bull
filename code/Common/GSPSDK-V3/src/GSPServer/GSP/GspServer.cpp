#include "GspServer.h"
#include "Log.h"
#include "../Server.h"
#include "GspSrvSession.h"

using namespace GSP;


static void _FreeAsyncTaskPoolMember( CGspSrvSession *pSession )
{
	pSession->DeleteBefore();
	delete pSession;
}

CGspServer::CGspServer(void)
:CIProServer(ePRO_GSP)
{
	m_pListenSocket = NULL;
	m_iTCPPort = GSP_SERVER_DEFAULT_TCP_LISTEN_PORT;
	m_strTcpBindIP = "";

	m_bUDP = 1;
	m_iUDPPort = GSP_SERVER_DEFAULT_UDP_LISTEN_PORT;
	m_strUdpBindIP = "";
	m_csTaskDestorySession.SetFreedTaskDataFunction((FuncPtrFree)_FreeAsyncTaskPoolMember);
	m_stConfig.bEnableUDP = m_bUDP;
	m_stConfig.iKeepaliveTimeouts = 30;
	m_pAsyncTcpEvtCtx = NULL;
	
	
}

CGspServer::~CGspServer(void)
{
	if( m_pListenSocket )
	{
		m_pListenSocket->Release();
		m_pListenSocket=NULL;
	}
	if( m_pAsyncTcpEvtCtx )
	{
		m_pAsyncTcpEvtCtx->Unint();
		m_pAsyncTcpEvtCtx->UnrefObject();
		m_pAsyncTcpEvtCtx = NULL;
	}
	
}

EnumErrno CGspServer::Init( CServer *pServer )
{
	m_pServer = pServer;
	 CConfigFile		&csCfg = m_pServer->GetConfig();

	 if( ! m_csTaskDestorySession.Init(this,
		 (FuncPtrObjThreadPoolEvent)&CGspServer::OnAsyncDelSessionTaskPoolEvent,
		 2, TRUE) )
	 {
		 MY_LOG_ERROR(g_pLog, _GSTX("��ʼ�� GSPЭ�������ʧ��. ��ʼ�������̳߳�ʧ��.\n"));
		 GS_ASSERT(0);
		 return eERRNO_SYS_EBUSY;
	 }

	
	m_iTCPPort = csCfg.GetProperty("GSPNetwork","TCPPort", 
									GSP_SERVER_DEFAULT_TCP_LISTEN_PORT );
	m_strTcpBindIP = csCfg.GetProperty("GSPNetwork", "TCPBindIP", "" );

	m_bUDP = csCfg.GetProperty("GSPNetwork", "EnableUDP", 1 );
	m_iUDPPort = csCfg.GetProperty("GSPNetwork", "UDPPort",
						GSP_SERVER_DEFAULT_UDP_LISTEN_PORT );
	m_strUdpBindIP = csCfg.GetProperty("GSPNetwork", "UDPBindIP", "" );

	m_stConfig.iKeepaliveTimeouts = csCfg.GetProperty("GSPNetwork", "KeepaliveTimeout", 30 );


	if( m_bUDP )
	{
		//��������ʹ�� RTP UDP ����
		
	}
	m_stConfig.bEnableUDP = m_bUDP;

	MY_LOG_NOTICE(g_pLog, _GSTX("��ʼ�� GSPЭ������� ���. TCP ����:(%s:%d),UDP ����(%s, %s:%d)\n"),
		m_strTcpBindIP.c_str(), m_iTCPPort,m_bUDP ? "Enable" : "Disable",
		m_strUdpBindIP.c_str(), m_iUDPPort );


	return eERRNO_SUCCESS;
}

void CGspServer::Unint(void)
{
	if( m_pListenSocket )
	{
		m_pListenSocket->Disconnect();
	}
	m_csTaskDestorySession.Uninit(TRUE);
	
}

EnumErrno CGspServer::Start(void)
{
	GS_ASSERT_RET_VAL(m_pServer, eERRNO_SYS_ENEXIST);
	GS_ASSERT(m_pListenSocket==NULL);

	MY_LOG_DEBUG(g_pLog, _GSTX("��ʼ ����GSPЭ�������...\n"));

	

	m_pAsyncTcpEvtCtx = m_pAsyncTcpEvtCtx->Create();

	if( m_pAsyncTcpEvtCtx )
	{
		if( m_pAsyncTcpEvtCtx->Init() )
		{
			MY_LOG_WARN(g_pLog, _GSTX("����GSPЭ��������쳣����������TCP����ģʽʧ��.\n"));
			GS_ASSERT(0);
			m_pAsyncTcpEvtCtx->UnrefObject();
			m_pAsyncTcpEvtCtx = NULL;
		}
	}
	else
	{
		MY_LOG_WARN(g_pLog, _GSTX("����GSPЭ��������쳣����������TCP����ģʽʧ��.\n"));
		GS_ASSERT(0);
	}


	m_pListenSocket = CTCPServerSocket::Create();
	if( !m_pListenSocket )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("����GSPЭ�������ʧ�ܣ�����Socket ����ʧ��.\n"));
		GS_ASSERT(0);
		return eERRNO_SYS_ENMEM;
	}
	CNetError csEerror = m_pListenSocket->Open(m_iTCPPort, 
		m_strTcpBindIP.length()>1 ? m_strTcpBindIP.c_str(): NULL);
	if( csEerror.m_eErrno )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("����GSPЭ�������ʧ�ܣ��󶨶˿�ʧ��:%s.\n"),
			csEerror.m_strError.c_str() );
		GS_ASSERT(0);
		m_pListenSocket->Release();
		m_pListenSocket = NULL;
		return eERRNO_SYS_ENMEM;
	}
	m_iTCPPort = m_pListenSocket->LocalPort();
	m_iListenPort = m_iTCPPort;
	m_strListenIP = m_strTcpBindIP;

	m_pListenSocket->SetListener(this, (FuncPtrSocketEvent)&CGspServer::OnTcpListenSocketEvent);
	csEerror.m_eErrno = m_pListenSocket->AsyncAccept(TRUE);

	if( csEerror.m_eErrno )
	{
		MY_LOG_FATAL(g_pLog, _GSTX("����GSPЭ�������ʧ�ܣ����������߳�ʧ�� Errno:0x%x.\n"),
									csEerror.m_eErrno );
		GS_ASSERT(0);
		m_pListenSocket->Release();
		m_pListenSocket = NULL;
		return eERRNO_SYS_ENMEM;
	}



	MY_LOG_DEBUG(g_pLog, _GSTX("����GSPЭ������� �ɹ�!! \n"));

	
	return csEerror.m_eErrno;



}

void CGspServer::Stop(void)
{
	if( m_pListenSocket )
	{
		m_pListenSocket->AsyncAccept(FALSE);
		m_pListenSocket->Release();
		m_pListenSocket = NULL;
	}
}




void* CGspServer::OnTcpListenSocketEvent(CISocket *pSocket,
										EnumSocketEvent iEvt,void *pParam, void *pParamExt)
{
	if( iEvt==eEVT_SOCKET_ACCEPT )
	{
		CISocket *pCliSocket = (CISocket*)pParam;
		MY_LOG_DEBUG(g_pLog, _GSTX("GSPЭ������� ���ܵ�����(%s).\n"),
						pCliSocket->GetDescri().c_str());

		if( CMemoryPool::IsNoMemory() )
		{
			MY_LOG_DEBUG(g_pLog, _GSTX("GSPЭ������� �ܾ�����(%s). �ڴ�ȱ��.\n"),
				pCliSocket->GetDescri().c_str());
			pCliSocket->Release();
			return (void*) TRUE;
		}

		CGspSrvSession *pSession = new CGspSrvSession(this);

		if( !pSession )
		{
			MY_LOG_ERROR(g_pLog, _GSTX("GSPЭ������� �ܾ�(%s)����. �����Ự����ʧ��.\n"),
				pCliSocket->GetDescri().c_str());	
			pCliSocket->Release();
		}
		if(eERRNO_SUCCESS != pSession->Init(pCliSocket) )
		{
			MY_LOG_ERROR(g_pLog, _GSTX("GSPЭ������� �ܾ�(%s)����. �Ự�����ʼ��ʧ��.\n"),
				pCliSocket->GetDescri().c_str());	
			pCliSocket->Release();
			pSession->DeleteBefore();
			delete pSession;
		}
		else
		{
			m_pServer->OnProServerEvent(this, eEVT_PROSRV_ACCEPT,(CISrvSession*)  pSession);
			pSession->Start();
		}
	}
	else if( iEvt==eEVT_SOCKET_ERR )
	{
		CNetError *pError = (CNetError*)pParam;
		MY_LOG_ERROR(g_pLog, _GSTX("GSPЭ������� (%s) Socket ����. %s (Sys:%d,Errno:0x%x)\n"),
				pSocket->GetDescri().c_str(),
				pError->m_strError.c_str(), pError->m_iSysErrno,
				pError->m_eErrno);
		return (void*)FALSE;
	}
	else
	{
		GS_ASSERT(0);
	}
	return (void*)TRUE;
}

void CGspServer::AsyncDestroySession( CGspSrvSession *pSession )
{
	
	if( m_csTaskDestorySession.Task(pSession) == CGSPThreadPool::RSUCCESS)
	{
		m_pServer->OnProServerEvent(this, eEVT_PROSRV_SESSION_RELEASE,(CISrvSession*) pSession);
	}
// 		pSession->DeleteBefore();
// 		m_pServer->OnProServerEvent(this, eEVT_PROSRV_SESSION_RELEASE,(CISrvSession*)  pSession);
// 		delete pSession;
	

}

void CGspServer::OnAsyncDelSessionTaskPoolEvent( CObjThreadPool *pTkPool, void *pData )
{
	CGspSrvSession *pSession = (CGspSrvSession *)pData;	
	pSession->DeleteBefore();
	delete pSession;
}

CAsyncTcpSrvSocket *CGspServer::CreateAsyncTcpSrvSocket(void)
{
	if( m_pAsyncTcpEvtCtx )
	{
		return m_pAsyncTcpEvtCtx->Create(NULL, 0);
	}
	return NULL;
}