#include "NetService.h"

#include "BaseSocket.h"

#if _LINUX
#include "LinuxSocket.h"
#include <signal.h>
#endif

using namespace NetServiceLib;




#define		MAX_CNETSERVICE_COUNT		3 //CNetService���󴴽�������

#define		MAX_MEMORYOBJECT_COUNT		10 //�ڴ�ص������ɵĶ������

INetService* CreateNetService()
{
	return new CNetService();
}

//��ľ�̬��Ա��ʼ��
UINT16		CNetService::s_unRefCount = 0;

CNetService::CNetService(void)
{
	m_bIsInit = false;	
	m_bIsExit = FALSE;
	m_SocketChannelMemoryPtr = NULL;
}

CNetService::~CNetService(void)
{
	m_clsClientChannel.CloseChannel();
	m_clsServerChannel.CloseChannel();
	if ( !m_bIsExit)
	{
		StopNetService();
	}
	
}
/*************************************************
  Function:		InitNetService    
  Description:  ��ʼ������  
  Input:		��
  Output:         
  Return:         
  Note:		//1����������  //2�������߳�  //3�����ü�����1	4��������ɶ˿ڻ�poll 5������cpu��Ŀ���߳�׼����������	
*************************************************/
INT CNetService::InitNetService()
{
	return InitNetLib();


}
INT CNetService::InitSimpleNetService()
{
	return InitNetLib(2);
}

INT	CNetService::InitNetLib(UINT iThreadNum)
{
	if(m_bIsInit)
	{
		//�Ѿ���ʼ������ֹ�ظ���ʼ��
		return ERROR_NET_REPEAT_INIT;

	}

	CGSAutoMutex		GSAutoMutex(&m_GSMutex);


	if(m_bIsInit)
	{
		//�Ѿ���ʼ������ֹ�ظ���ʼ��
		return ERROR_NET_REPEAT_INIT;

	}

	if (g_clsLogPtr == NULL)
	{
		m_clsThreadDealNetEvent.SetLogPath( GetApplicationPath().c_str() );

		m_clsThreadDealNetEvent.m_clsThreadPool.SetLogInstancePtr( m_clsThreadDealNetEvent.GetLogInstancePtr());
	}
	else
	{
		m_clsThreadDealNetEvent.SetLogInstancePtr(g_clsLogPtr);
		m_clsThreadDealNetEvent.m_clsThreadPool.SetLogInstancePtr(g_clsLogPtr);
	}

	m_bIsExit = FALSE;

	

	if ( s_unRefCount >= MAX_CNETSERVICE_COUNT )
	{
		return ERROR_NET_MAX_CNETSERVICE_COUNT;
	}	

	INT16 iThreadCount = 0;

	m_clsThreadDealNetEvent.Init();
#if OPERATING_SYSTEM
	INT iRet = m_clsThreadDealNetEvent.m_clsThreadPool.Initialize(5);//��iThreadCount���߳�

	if ( ERROR_BASE_SUCCESS != iRet)
	{
		return iRet;
	}

	CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(),"����ϵͳΪOPERATING_SYSTEM \n");


#elif _WIN32
	if (iThreadNum > 0)
	{
		iThreadCount = iThreadNum; //һ�����ڿͻ���
	}
	else
	{
		iThreadCount = (INT16)m_clsThreadDealNetEvent.GetNumberOfProcessors() * 2 + 2;
	}

	m_iIocpThreadCount = iThreadCount;
	
	INT iRet = m_clsThreadDealNetEvent.m_clsThreadPool.Initialize(iThreadCount);//��iThreadCount���߳�ȥ��������

	if ( ERROR_BASE_SUCCESS != iRet)
	{
		return iRet;
	}
	CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(),"����ϵͳΪ_WIN32 \n");
#else
	INT iRet = m_clsThreadDealNetEvent.m_clsThreadPool.Initialize(10);//��10���߳�ȥ��������

	if ( ERROR_BASE_SUCCESS != iRet)
	{
		return iRet;
	}
	CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(),"����ϵͳΪ_LINUX \n");
#endif

#if _LINUX

	signal(SIGPIPE, &CNetService::ReceiveSignal);
#endif





#if _WIN32
	if ( 0 == s_unRefCount )
	{
		//��һ������
		WSADATA wsaData;
		INT iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != NO_ERROR)
		{
			return ERROR_NET_WSASTARTUP_FAIL;
		}		
	}

#endif

#if OPERATING_SYSTEM

	iThreadCount = 1; // ����Ҫһ���߳�ȥ���select�¼�
	for (INT i=0; i<iThreadCount; i++)
	{
		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

		if ( NULL == pTask )
		{

			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::SelectEvent;
		pTask->enumEvent = EPOLLEVENTWAIT;
		pTask->pObject = (CCommunicationManager *)&m_clsThreadDealNetEvent;
		pTask->pObject2 = (CCommunicationManager *)&m_clsThreadDealNetEvent;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
	}

#elif _WIN32

	m_clsThreadDealNetEvent.CreateIoCompletionPortEx();//������ɶ˿�	


	//����cpu*2+2��һ���Ľ������������Ա�ʹ��ͬ�����߳���Ŀȥ��������

	for (INT i=0; i<iThreadCount; i++)
	{
		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

		if ( NULL == pTask )
		{

			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::ThreadAcceptData;
		pTask->enumEvent = ACCEPTDATA;
		pTask->pObject = (CCommunicationManager *)&m_clsThreadDealNetEvent;
		pTask->pObject2 = NULL;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
		m_clsThreadDealNetEvent.m_clsThreadPool.WakeUpThread();
	}

#else //_LINUX
	//����epoll
	m_clsThreadDealNetEvent.CreateEpoll();
	iThreadCount = 1; //EPOLLֻ��Ҫһ���߳�ȥִ��
	for (INT i=0; i<iThreadCount; i++)
	{
		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

		if ( NULL == pTask )
		{

			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::EpollWaitEvent;
		pTask->enumEvent = EPOLLEVENTWAIT;
		pTask->pObject = (CCommunicationManager *)&m_clsThreadDealNetEvent;
		pTask->pObject2 = (CCommunicationManager *)&m_clsThreadDealNetEvent;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
	}
#endif

	s_unRefCount++;  //���ü�����1

	//ͨ�����ϴ�������
	pstruThreadPoolTask	 pTask2 = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

	if ( NULL == pTask2 )
	{

		return ERROR_NET_UNKNOWN;
	}

	memset(pTask2,0x0,sizeof(struThreadTask));
	pTask2->pFunction = &CCommunicationManager::ThreadChannelStatus;
	pTask2->enumEvent = CHANNELSTATUS;
	pTask2->pObject = (CCommunicationManager *)&m_clsThreadDealNetEvent;
	pTask2->pObject2 = NULL;
	m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask2);
	m_clsThreadDealNetEvent.m_clsThreadPool.WakeUpThread();

	//֪ͨ�ϲ������ӵ���
	pstruThreadPoolTask	 pTask3 = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

	if ( NULL == pTask2 )
	{

		return ERROR_NET_UNKNOWN;
	}

	memset(pTask3,0x0,sizeof(struThreadTask));
	pTask3->pFunction = &CCommunicationManager::NoticeUpNewConnect;
	pTask3->enumEvent = ACCEPTNOTICE;
	pTask3->pObject = (CCommunicationManager *)&m_clsThreadDealNetEvent;
	pTask3->pObject2 = NULL;
	m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask3);
	m_clsThreadDealNetEvent.m_clsThreadPool.WakeUpThread();

	m_bIsInit = true;

	return ERROR_BASE_SUCCESS;
}
/*************************************************
  Function:      AddServerChannel 
  Description:   ����ͨѶͨ����
  Input:  
  Output:         
  Return:         
  Note:			����	
*************************************************/
INT CNetService::AddServerChannel(const char* pszBindIP, UINT16 unPort, 
					 enumNetProtocolType eProtocolType, ISocketChannel** pSocketChnl)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if ( !m_bIsInit )
	{
		return ERROR_NET_UNKNOWN;
	}

	// Ϊ������Э��ջ��gsp������ôд
	/*if (!g_clsLogPtr)
	{
		m_clsThreadDealNetEvent.SetLogPath( GetApplicationPath().c_str() );

		m_clsThreadDealNetEvent.m_clsThreadPool.SetLogInstancePtr( m_clsThreadDealNetEvent.GetLogInstancePtr());
	}*/	


	if (true == m_clsThreadDealNetEvent.IfMaxChannelCount())
	{
		return ERROR_NET_MAX_CHANNEL_COUNT;//�Ѵﵽ���������
	}
	
	if (unPort < 0) //�󶨵�IPΪnullʱ ��ʾ�󶨱�������IP
	{
		return ERROR_NET_PARAM_WRONG;
	}

	//����socketchannel
	CSocketChannel *pclsSocketChnl = new CSocketChannel();//��ָ�뱣�浽�����У�ֱ��ͨ����Ҫɾ��ʱ���ͷ�.��~CNetInterfaceCommData(void)���ͷ�
	
	if ( NULL == pclsSocketChnl )
	{
		return ERROR_NET_UNKNOWN;
	}
	
	//����ͨ������
	pclsSocketChnl->SetLocalIPPort(pszBindIP, unPort);
	pclsSocketChnl->SetServerType( SERVER );
	pclsSocketChnl->SetNetProtocolType( eProtocolType );
	pclsSocketChnl->SetChannelType( LISTEN_CHANNEL );//������UDP����tcp ����AddServerChannel add��ͨ������Ϊ�Ǽ���ͨ��

	
	//����basesocket
	CBaseSocket* pBaseSocket = NULL;//�������ں�pclsSocketChnlһ����
#if _WIN32
	if (eProtocolType == NET_PROTOCOL_TCP)
	{
		pBaseSocket = new CWinTcpSocket();
		
	}
	else
	{
		pBaseSocket = new CWinUdpSocket();
	}
#endif
#if _LINUX
	if (eProtocolType == NET_PROTOCOL_TCP)
	{
		pBaseSocket = new CLinuxTcpSocket();
	}
	else
	{
		pBaseSocket = new CLinuxUdpSocket();

	}
#endif

	if ( NULL == pBaseSocket )
	{
		if ( pclsSocketChnl )
		{
			pclsSocketChnl->CloseChannel();
			delete pclsSocketChnl;
			pclsSocketChnl = NULL;
		}
		return ERROR_NET_UNKNOWN;
	}
	 
	pBaseSocket->SetBlockMode( m_clsThreadDealNetEvent.GetBlockMode() );
	m_clsServerChannel.SetLocalIPPort(pszBindIP, unPort);
	//����ͨ��
	if (m_clsServerChannel.CreateChannel(pBaseSocket) != ERROR_BASE_SUCCESS)
	{
		//���� ͨ��ʧ��
		pclsSocketChnl->CloseChannel();
		delete pclsSocketChnl;
		pclsSocketChnl = NULL;

		delete pBaseSocket;
		pBaseSocket = NULL;

		return ERROR_NET_CREATE_CHANNEL_FAIL;
	}


	//����CBaseSocket
	pclsSocketChnl->SetCbaseSocketPoint(pBaseSocket);

	//��socketchannel����vector����
    m_clsThreadDealNetEvent.SaveSocketChannel(pclsSocketChnl);

	pclsSocketChnl->SetLogInstancePtr(m_clsThreadDealNetEvent.GetLogInstancePtr());

#if OPERATING_SYSTEM

	if (eProtocolType == NET_PROTOCOL_TCP)
	{
		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//���������ʱ������˳�ʱ�����̳߳ص�Uninitialize()���ͷ�

		if ( NULL == pTask )
		{
			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::Listen;
		pTask->enumEvent = LISTEN;
		pTask->pObject = (CCommunicationManager*)&m_clsThreadDealNetEvent;
		pTask->pObject2 = pclsSocketChnl;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
		m_clsThreadDealNetEvent.m_clsThreadPool.WakeUpThread();

	}
	

#elif _WIN32
	
	if (eProtocolType == NET_PROTOCOL_UDP)
	{
		//������ɶ˿�
		m_clsThreadDealNetEvent.AddToIoCompletionPort((HANDLE)pBaseSocket->GetSocket(), (DWORD)pclsSocketChnl->GetIOCPHandleData());
		//�������
		pclsSocketChnl->RecvData();
	}
	else
	{
		//m_clsThreadDealNetEvent.m_clsThreadPool.AddThread(1);//����һ���߳�ȥ������洴�������񣬼�����

		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//���������ʱ������˳�ʱ�����̳߳ص�Uninitialize()���ͷ�

		if ( NULL == pTask )
		{
			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::Listen;
		pTask->enumEvent = LISTEN;
		pTask->pObject = (CCommunicationManager*)&m_clsThreadDealNetEvent;
		pTask->pObject2 = pclsSocketChnl;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
		m_clsThreadDealNetEvent.m_clsThreadPool.WakeUpThread();
	}
	

#else	//linux
	
	if (eProtocolType == NET_PROTOCOL_UDP)
	{
		//����EPOLL
		m_clsThreadDealNetEvent.EpollCtrl( EPOLL_CTL_ADD,pBaseSocket->GetSocket(),pclsSocketChnl->GetEpollEvent());

	}
	else
	{

		pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//���������ʱ������˳�ʱ�����̳߳ص�Uninitialize()���ͷ�

		if ( NULL == pTask )
		{
			return ERROR_NET_UNKNOWN;
		}

		memset(pTask,0x0,sizeof(struThreadTask));
		pTask->pFunction = &CCommunicationManager::Listen;
		pTask->enumEvent = LISTEN;
		pTask->pObject = (CCommunicationManager*)&m_clsThreadDealNetEvent;
		pTask->pObject2 = pclsSocketChnl;
		m_clsThreadDealNetEvent.m_clsThreadPool.AssignTask(pTask);
	}
#endif

	//ת��
	*pSocketChnl = (ISocketChannel*)pclsSocketChnl;

	return ERROR_BASE_SUCCESS;
}
/********************************************************************************
  Function:		AddClientChannel
  Description:	����һ���ͻ���
  Input:  		pszhost ������ַ	unDesPort Ŀ�Ķ˿�	pszBindIP ����IP��ַ ������0 ��ʾ����������IP	localport ���ض˿� ����Ϊ0 ϵͳ�Զ�����
  Output:		pSocketChnl   �ɹ�����ͨ��ָ�� 
  Return:  		       
  Note:					
  Author:        	CHC
  Date:				2010/08/31
********************************************************************************/
INT CNetService::AddClientChannel( const char *pszhost, UINT16 unDesPort,const char *pszBindIP, 
									UINT16 localport,enumNetProtocolType eProtocolType,  ISocketChannel** pSocketChnl)
{
	if ( m_bIsExit )
	{
		return ERROR_NET_UNKNOWN;
	}

	if ( !m_bIsInit )
	{
		return ERROR_NET_UNKNOWN;
	}

	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	// Ϊ������Э��ջ��gsp������ôд
	/*if (!g_clsLogPtr)
	{
	m_clsThreadDealNetEvent.SetLogPath( GetApplicationPath().c_str() );

	m_clsThreadDealNetEvent.m_clsThreadPool.SetLogInstancePtr( m_clsThreadDealNetEvent.GetLogInstancePtr());
	}*/

	if (true == m_clsThreadDealNetEvent.IfMaxChannelCount())
	{
		return ERROR_NET_MAX_CHANNEL_COUNT;//�Ѵﵽ���������
	}

	if (pszhost == NULL)		
	{
		return ERROR_NET_PARAM_WRONG;
	}

	//����socketchannel
	CSocketChannel *pclsSocketChnl = new CSocketChannel();//��ָ�뱣�浽�����У�ֱ��ͨ����Ҫɾ��ʱ���ͷţ���������˳��������ϲ�����ر�ͨ��
	if ( NULL == pclsSocketChnl )
	{
		// û���뵽�ڴ�
		return ERROR_NET_UNKNOWN;
	}
	pclsSocketChnl->SetLocalIPPort(pszBindIP, localport);
	pclsSocketChnl->SetReMoteIPPort(pszhost, unDesPort);
	pclsSocketChnl->SetNetProtocolType( eProtocolType );
	pclsSocketChnl->SetServerType( CLIENT );
	pclsSocketChnl->SetChannelType( COMM_CHANNEL );//�ͻ��˽����Ķ���ͨѶͨ��

	//����basesocket
	CBaseSocket* pBaseSocket = NULL;//�������ں�pclsSocketChnlһ����
#if _WIN32
	if (eProtocolType == NET_PROTOCOL_TCP)
	{
		pBaseSocket = new CWinTcpSocket();
	}
	else
	{
		pBaseSocket = new CWinUdpSocket();

	}
#endif

#if _LINUX
	if (eProtocolType == NET_PROTOCOL_TCP)
	{
		pBaseSocket = new CLinuxTcpSocket();
	}
	else
	{
		pBaseSocket = new CLinuxUdpSocket();

	}
#endif

	if ( NULL == pBaseSocket )
	{
		// û���뵽�ڴ�
		return ERROR_NET_UNKNOWN;
	}

	pBaseSocket->SetBlockMode( m_clsThreadDealNetEvent.GetBlockMode() );
	m_clsClientChannel.SetLocalIPPort(pszBindIP, localport);
	m_clsClientChannel.SetReMoteIPPort(pszhost,unDesPort);


	//����ͨ��
	if (m_clsClientChannel.CreateChannel(pBaseSocket) != ERROR_BASE_SUCCESS)
	{
		//���� ͨ��ʧ��
		pclsSocketChnl->CloseChannel();
		delete pclsSocketChnl;
		pclsSocketChnl = NULL;

		delete pBaseSocket;
		pBaseSocket = NULL;	

		char szRemoteIP[16]={0};
		UINT16 uiRemotePort = 0;
		char szLocalIP[16]={0};
		UINT16 uiLocalPort = 0;
		m_clsClientChannel.GetLocalIPPort(szLocalIP,uiLocalPort);
		m_clsClientChannel.GetReMoteIPPort(szRemoteIP,uiRemotePort);
		CTRLPRINTF( g_clsLogPtr,"����ͨ��ʧ�ܴ���ͨ������,pszhost:%s, unDesPort:%u, pszBindIP:%s, localport:%u \n",pszhost, unDesPort,pszBindIP,localport );
		CTRLPRINTF( g_clsLogPtr,"����ͨ��ʧ�ܿͻ���ͨ������,szRemoteIP:%s, uiRemotePort:%u, szLocalIP:%s, uiLocalPort:%u \n",szRemoteIP, uiRemotePort,szLocalIP,uiLocalPort );
		return ERROR_NET_CREATE_CHANNEL_FAIL;
	}

	sockaddr_in struLocalAddr;
#if _WIN32
	INT32   iLocalAddrLen = sizeof(sockaddr_in);
#endif
#if _LINUX
	socklen_t   iLocalAddrLen = sizeof(sockaddr_in);
#endif
	getsockname( pBaseSocket->GetSocket(), (sockaddr*)&struLocalAddr, &iLocalAddrLen );
	pclsSocketChnl->SetLocalIPPort(inet_ntoa(struLocalAddr.sin_addr), (UINT16) ntohs(struLocalAddr.sin_port));

	//����CBaseSocket
	pclsSocketChnl->SetCbaseSocketPoint(pBaseSocket);

	//��socketchannel����vector����
	m_clsThreadDealNetEvent.SaveSocketChannel(pclsSocketChnl);

	pclsSocketChnl->SetLogInstancePtr(m_clsThreadDealNetEvent.GetLogInstancePtr());

	CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(), "ͨ��%p�����ɹ�, new socket %d, szRemoteIP:%s, \
		uiRemotePort:%u, szLocalIP:%s, uiLocalPort:%u\n ",pclsSocketChnl,pBaseSocket->GetSocket(),pszhost,unDesPort,
		inet_ntoa(struLocalAddr.sin_addr), (UINT16) ntohs(struLocalAddr.sin_port));

#if OPERATING_SYSTEM

	// select ʲô������Ҫ��

#elif _WIN32

	//������ɶ˿�
	HANDLE hHandle = m_clsThreadDealNetEvent.AddToIoCompletionPort((HANDLE)pBaseSocket->GetSocket(), (DWORD)pclsSocketChnl->GetIOCPHandleData());
	if (hHandle == NULL)
	{
		pclsSocketChnl->CloseChannel();
		delete pclsSocketChnl;
		pclsSocketChnl = NULL;

		delete pBaseSocket;
		pBaseSocket = NULL;	
		CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(), "ͨ��%p������ɶ˿�ʧ��, socket %d\n ",pclsSocketChnl,pBaseSocket->GetSocket());
		return ERROR_NET_CREATE_CHANNEL_FAIL;
	}
	//Ͷ����һ�ν�����������
	pclsSocketChnl->RecvData();

#else
	//linux

	//����epoll
	m_clsThreadDealNetEvent.EpollCtrl( EPOLL_CTL_ADD,pBaseSocket->GetSocket(),pclsSocketChnl->GetEpollEvent());

	pclsSocketChnl->AddRefCount();

#endif
	//ת��
	*pSocketChnl = (ISocketChannel*)pclsSocketChnl;


	return ERROR_BASE_SUCCESS;
}
/********************************************************************************
  Function:       StopNetService
  Description:    
  Input:  
  Output:         
  Return:         
  Note:			  ��Ҫ���̣�1��ֹͣ�߳� 2���ر�����socket 3���ͷ�����ͨ�� 4���ͷ��̺߳����� 5�����ü�����һ
							6���������ü����ж��Ƿ��ͷ�������ԴWSACleanup
********************************************************************************/
INT CNetService::StopNetService()//���ô˺���ֹͣ������񣬽�������ûص�֪ͨ�ϲ�
{
	
	UINT64	uiTick = DoGetTickCount();

	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if ( m_bIsExit )
	{
		return ERROR_BASE_SUCCESS;
	}

	m_bIsExit = TRUE;

	m_clsThreadDealNetEvent.SetExitChannelStatus();	////�����˳�ͨ��״̬�����̱߳�־

	m_clsThreadDealNetEvent.FreeAllChannel();//�ر�ͨ��	

#if OPERATING_SYSTEM
#elif _WIN32
	m_clsThreadDealNetEvent.ExitAcceptData(m_iIocpThreadCount);//�˳��������ݵ�whileѭ��
#else
#endif


	m_clsThreadDealNetEvent.m_clsThreadPool.SetIsExitWorkerThreadProc();//�������̶߳��˳�ѭ������ʵ������ִ��������߳�Ҫ�ȵ���һ�����������˳�

	//һ���������߳�ִ�е���������ֹͣ����ʵ���ǽ�������whileѭ�� //ֻ�������־ û���ж�ѭ���Ƿ��������������������׵�
	m_clsThreadDealNetEvent.SetExitListen();//�˳����� 
	m_clsThreadDealNetEvent.SetExitAcceptData();//�˳���������
	m_clsThreadDealNetEvent.SetExitActiveTest();//�˳����ӻ���
	m_clsThreadDealNetEvent.SetExitAcceptUpNotice();//�˳��˳�֪ͨ�ϲ������ӵ����־
	

#if OPERATING_SYSTEM
	m_clsThreadDealNetEvent.SetExitSelectEvent();
#elif _WIN32
	//m_clsThreadDealNetEvent.ExitAcceptData();//�˳��������ݵ�whileѭ��
	//m_clsThreadDealNetEvent.PostQueuedCompletionStatusEx();//�˳���ɶ˿�
#else
	m_clsThreadDealNetEvent.SetExitEpollEventWait();
#endif

	//ֹͣȫ���߳�  �˵��߳̾��Բ���һ���򵥵��� 
	//1������������У���ɾ������δִ�е����񣬱����߳̽���ִ��
	//2��ֹͣ�����̵߳�whileѭ��
	//3���ͷ��߳���Դ
	//4��������������
	m_clsThreadDealNetEvent.m_clsThreadPool.Uninitialize();

	if ( m_SocketChannelMemoryPtr )
	{
		delete m_SocketChannelMemoryPtr;
		m_SocketChannelMemoryPtr = NULL;
	}

	//��δ��ʼ����־
	m_bIsInit = false;

	//���ü�����һ
	s_unRefCount--;
	if (s_unRefCount == 0)
	{
#if _WIN32
		WSACleanup();
#endif

	}

	CTRLPRINTF(m_clsThreadDealNetEvent.GetLogInstancePtr(),"������˳���ʱ %u ����\n", DoGetTickCount() - uiTick);

	return ERROR_BASE_SUCCESS;
}
/********************************************************************************
  Function:      CloseChannel 
  Description:   �ر�ͨ�� 
  Input:  
  Output:         
  Return:         
  Note:			
********************************************************************************/
INT CNetService::CloseChannel(ISocketChannel* pSocketChnl)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if ( pSocketChnl == NULL )
	{
		return ERROR_NET_PARAM_WRONG;
	}
	CSocketChannel*  pclsSocketChannel = dynamic_cast<CSocketChannel*>(pSocketChnl);
	if ( NULL == pclsSocketChannel )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	//������Ӽ��ָ��ĺϷ��ԣ�����ָ������Ƿ񻹴���
	enumChannelType			enumCnvType;
	try
	{
		enumCnvType = pclsSocketChannel->GetChannelType();
		if ( COMM_CHANNEL != enumCnvType && LISTEN_CHANNEL != enumCnvType )
		{
			return ERROR_NET_INVALID_CHANNEL_POINT;
		}
		
	}
	catch ( ...)
	{
		return ERROR_NET_CHANNEL_NOT_EXIST;
	}

	pclsSocketChannel->CloseChannel();

	return ERROR_BASE_SUCCESS;
}
INT CNetService::SetReConnect(bool bReConnect)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	m_clsThreadDealNetEvent.SetReConnect(bReConnect);
	
	return ERROR_BASE_SUCCESS;

}
INT CNetService::SetMsgBuffFlag(bool bMsgBuffFlag)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	m_clsThreadDealNetEvent.SetMsgBufFlag(bMsgBuffFlag);

	return ERROR_BASE_SUCCESS;

}
INT CNetService::SetActiveTime( UINT16 unTime )
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);
	if (unTime <= 0)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_clsThreadDealNetEvent.SetActiveTime(unTime);

	return ERROR_BASE_SUCCESS;

}

INT CNetService::SetMaxChannel( UINT16 unCount )
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if (unCount <= 0)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_clsThreadDealNetEvent.SetMaxChannel( unCount );

	return ERROR_BASE_SUCCESS;

}

INT CNetService::SetOnEventCallBack(void* pUserData, pOnEventCallBack OnEventCallBack)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);
	
	//pUserData�ǿ���Ϊnull�� �ϲ������ò���Ҫʱ
	if ( NULL == OnEventCallBack )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	return m_clsThreadDealNetEvent.SetOnEventCallBack(pUserData, OnEventCallBack);
}

INT	CNetService::GetAllChannelNum()
{

	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	//������������ɲ��Ժ�����д����ʾһЩ����������
	printf("��ǰ�������ͨ����Ŀ�� %d \n", m_clsThreadDealNetEvent.GetAllChannelNum());
	printf("��ǰ������������߳���Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetAllThreadCount());
	printf("��ǰ�������æµ�߳���Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetBusyThreadCount());
	printf("��ǰ������п����߳���Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetIdleThreadCount());
	printf("��ǰ�������������Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetTaskCount());
	printf("��ǰ������д�ɾ�����߳���Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetWaitDelThreadNum());
	printf("��ǰ�������ɾ�������е��߳���Ŀ�� %d \n", m_clsThreadDealNetEvent.m_clsThreadPool.GetVecWaitDelThread());
	return	m_clsThreadDealNetEvent.GetAllChannelNum();
}

INT CNetService::SetNetBlockMode( BOOL bMode)
{
	// �������ϲ��������������������ģʽ��û�����壬��Ϊselect��epollģ��Ҫ������Ƿ�������
	/*CGSAutoMutex		GSAutoMutex(&m_GSMutex);
	
	m_clsThreadDealNetEvent.SetBlockMode( bMode );*/

	return ERROR_BASE_SUCCESS;
}



INT CNetService::SetSendBuf(INT iBufSize)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if ( iBufSize <=0 )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	if ( ERROR_BASE_SUCCESS != m_clsThreadDealNetEvent.SetSendBuf( iBufSize ) )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	return ERROR_BASE_SUCCESS;
	
}

INT CNetService::SetRcvBuf(INT iBufSize)
{
	CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	if ( iBufSize <=0 )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	if ( ERROR_BASE_SUCCESS != m_clsThreadDealNetEvent.SetRcvBuf( iBufSize ) )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	return ERROR_BASE_SUCCESS;
}

INT	CNetService::InitLog(const char *czPathName)
{
	if ( NULL == czPathName  )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	//ͬ����initserviceʱ������־
	/*CGSAutoMutex		GSAutoMutex(&m_GSMutex);

	m_clsThreadDealNetEvent.SetLogPath( czPathName );

	m_clsThreadDealNetEvent.m_clsThreadPool.SetLogInstancePtr( m_clsThreadDealNetEvent.GetLogInstancePtr());*/

	return ERROR_BASE_SUCCESS;
}

#if _LINUX

void CNetService::ReceiveSignal( INT32 iSignalNum )
{
	// ����Ҫ���κδ���
	
	return;
}

#endif






