#include "SocketChanel.h"

using namespace NetServiceLib;

CSocketChannel::CSocketChannel(void)
{
	memset(m_szLocalIP,0x0,16);
	memset(m_szRemoteHost,0x0,256);
	m_unLocalPort = 0;
	m_unRemotePort = 0;
	m_dwRemoteIP = 0;
	m_uiLastActiveTime = DoGetTickCount();

	m_pBaseSocket = NULL;
	m_pListenSocketChannel = NULL;

	m_pIOData_Recv = NULL;
	m_pIOData_Recv = (LPPER_IO_OPERATION_DATA) malloc(sizeof(PER_IO_OPERATION_DATA));
	assert(m_pIOData_Recv);
	memset(m_pIOData_Recv, 0x0, sizeof(PER_IO_OPERATION_DATA) );
#if OPERATING_SYSTEM

#elif _WIN32
	//m_pIOCP_HdlData = (LPPER_HANDLE_DATA) GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));	
	m_pIOCP_HdlData = (LPPER_HANDLE_DATA) malloc(sizeof(PER_HANDLE_DATA));	
	assert(m_pIOCP_HdlData);
	//��ʼ��
	InitIOCPHandleData();
#else
	m_pstruEpollEvent = ( epoll_event* ) malloc( sizeof(epoll_event));
	assert(m_pstruEpollEvent);
	//��ʼ��
	InitEpollEvent();
#endif

	//Ĭ��������״̬
	m_enumChannelStatus = CHANNEL_NORMAL;

	m_uiRefCount = 0;	

	m_bIOCPNoUse = FALSE;

	m_uiCloseTick = 0;
	
	m_pUserData = NULL;

	m_unSocketSendBuf = 0;

	m_unSocketRcvBuf = 0;

	m_clsLogPtr = NULL;

	m_uiChnIng = 0;
}

CSocketChannel::~CSocketChannel(void)
{
	m_csCallBackGSMutex.Lock();
	m_GSMutex.Lock();

	if ( CHANNEL_CLOSE != m_enumChannelStatus )
	{
		assert(0);
	}

	if ( m_pIOData_Recv )
	{
		free(m_pIOData_Recv);
		m_pIOData_Recv = NULL;
	}
	
#if OPERATING_SYSTEM

#elif _WIN32
	if ( m_pIOCP_HdlData )
	{
		free(m_pIOCP_HdlData);
		m_pIOCP_HdlData = NULL;
	}
	
	
#else //_LINUX
	if ( m_pstruEpollEvent )
	{
		free(m_pstruEpollEvent);
		m_pstruEpollEvent  = NULL;
	}
	
#endif

	if ( SERVER == m_enumServerType && NET_PROTOCOL_UDP == m_enumNetProType && COMM_CHANNEL == m_enumChannelType )
	{
		//����ͨ������Ҫdelete
		m_pBaseSocket = NULL;
	}	
	else
	{
		if ( NULL != m_pBaseSocket )
		{
			delete m_pBaseSocket;
			m_pBaseSocket = NULL;
		}
	}

	m_GSMutex.Unlock();
	m_csCallBackGSMutex.Unlock();
	

}
INT	CSocketChannel::GetLocalIPPort(char* pszIP, UINT16& unPort)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (strlen(m_szLocalIP) > 0)
	{
		strcpy(pszIP, m_szLocalIP);
	}
	else
	{
		pszIP = NULL;
	}
	
	unPort = m_unLocalPort;
	
	return ERROR_BASE_SUCCESS;
}
INT	CSocketChannel::GetReMoteIPPort(char* pszIP, UINT16& unPort)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	strcpy(pszIP, m_szRemoteHost);
	unPort = m_unRemotePort;

	return ERROR_BASE_SUCCESS;
}
/*************************************************
  Function:       SendData
  Description:    
  Input:  
  Output:         
  Return:         
  Note:			  �������ݣ��������ʧ�ܣ�ͨ��Ӧ������¼���������Ƿ��������Ƿ�֪ͨ�ϲ��	
*************************************************/
INT	CSocketChannel::SendData(void* pData, UINT unDataLen)
{
	CTRLPRINTF_D(m_clsLogPtr,"ͨ��%p׼����������0, localport=%u, RemotePort=%u, unDataLen=%u \n",this, m_unLocalPort, m_unRemotePort,unDataLen);
	if (m_pBaseSocket == NULL || pData == NULL || unDataLen > 8192)		// socketĬ�ϵĻ�������С����8192 �Ҿ���û��Ҫ����Ĭ��ֵ
	{
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��1, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
		return ERROR_NET_PARAM_WRONG;
	}

	if ( CHANNEL_NORMAL != m_enumChannelStatus && CHANNEL_TIMEOUT != m_enumChannelStatus )//��������״̬��ͨ������������ �����жϡ��رա���ȴ���ɶ˿�
	{ //��ʱҲ���Է�����
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��2, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
		return ERROR_NET_CHANNEL_STUTAS_CLOSE;
	}

	CGSAutoMutex	AutoMutex(&m_GSMutex);
	

	if ( CHANNEL_NORMAL != m_enumChannelStatus && CHANNEL_TIMEOUT != m_enumChannelStatus )//��������״̬��ͨ������������ �����жϡ��رա���ȴ���ɶ˿�
	{ //��ʱҲ���Է�����
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��3, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
		return ERROR_NET_CHANNEL_STUTAS_CLOSE;
	}
	

	if ( NULL == m_pBaseSocket )
	{
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��4, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
		return ERROR_NET_INVALID_CHANNEL_POINT;//��Ч��ͨ��ָ��
	}

	if ( NULL == m_pBaseSocket->GetSocket())
	{
		//socket�Ѿ����رջ���δ����
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��5, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
		return ERROR_NET_SOCKET_NOT_EXIST;
	}
	
	INT nLen = 0;

	if ( NET_PROTOCOL_TCP == m_enumNetProType)
	{
		INT nSendLen = 0;
		INT nTotalLen = unDataLen;
		while ( nLen < nTotalLen )
		{
			nSendLen = m_pBaseSocket->SendData( ((char*)pData + nLen), nTotalLen - nLen );			
			if (nSendLen <= 0)
			{	//����ֻҪ��ʧ�� ����
				break;
			}
			else
			{
				//δ������� ��������
				nLen += nSendLen;
				continue;
			}
		}
	}
	else
	{
		nLen = m_pBaseSocket->SendDataTo(pData, unDataLen, m_szRemoteHost, m_unRemotePort);
	}
	
	//���۷��ͳɹ���ʧ�ܶ���Ϊ��ͨ���ǻ��
	m_uiLastActiveTime = DoGetTickCount();
	
	//���������

	if (nLen == unDataLen)
	{
		CTRLPRINTF_D(m_clsLogPtr,"ͨ��%p�ɹ���������*, localport=%u, RemotePort=%u, unDataLen=%u \n",this, m_unLocalPort, m_unRemotePort,unDataLen);
		return ERROR_BASE_SUCCESS;
	}
	else
	{
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p��������ʧ��6, localport=%u, RemotePort=%u, nLen=%d,  unDataLen=%u \n",this, m_unLocalPort, m_unRemotePort, nLen, unDataLen);
		return ERROR_NET_UNKNOWN;
	}

	

}
/********************************************************************************
  Function:       CloseChannel
  Description:    �ر�ͨ��
  Input:  
  Output:         
  Return:         
  Note:				�����˳���־���ر�socket����Ӧ�̼߳�⵽�˳���־���ͷ�����Դ
********************************************************************************/
INT	CSocketChannel::CloseChannel()
{

	if ( m_pBaseSocket )
	{
		if ( m_pBaseSocket->GetSocket() == NULL )
		{
			return ERROR_BASE_SUCCESS;
		}
	}

	if (m_enumChannelStatus == CHANNEL_CLOSE)
	{
		return ERROR_BASE_SUCCESS;
	}

	CTRLPRINTF(m_clsLogPtr,"ͨ��%p������ر�, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
	//SetChannelStatus( CHANNEL_CLOSE );

#ifdef OPERATING_SYSTEM
#elif _LINUX
	SubRefCount();
#endif

	CGSAutoMutex	CallBackGSMutex(&m_csCallBackGSMutex);

	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	m_uiCloseTick = DoGetTickCount();

	m_enumChannelStatus = CHANNEL_CLOSE;

	//�������˵�UDPͨѶͨ�����ܹر�socket. ֻ��Ҫ����ͨ����־λ�˳����ɣ��̻߳��ͷ���Դ��
	if ( SERVER == m_enumServerType && NET_PROTOCOL_UDP == m_enumNetProType && COMM_CHANNEL == m_enumChannelType )
	{
		return ERROR_BASE_SUCCESS;
	}	
	//�ر�sokcet
    UINT16 iRet = 	CloseHandle();

	

	return iRet;

}
/********************************************************************************
Function:       CloseChannel
Description:    �ر�ͨ��
Input:  
Output:         
Return:         
Note:			��CloseChannel��ͬ�ĵط��Ǵ�CloseChannelEx���Ը�ͨ���ӻص���
********************************************************************************/
INT	CSocketChannel::CloseChannelEx()
{

	if ( m_pBaseSocket )
	{
		if ( m_pBaseSocket->GetSocket() == NULL )
		{
			return ERROR_BASE_SUCCESS;
		}
	}

	if (m_enumChannelStatus == CHANNEL_CLOSE)
	{
		return ERROR_BASE_SUCCESS;
	}

	CTRLPRINTF(m_clsLogPtr,"ͨ��%p������ر�, localport=%u, RemotePort=%u \n",this, m_unLocalPort, m_unRemotePort);
	//SetChannelStatus( CHANNEL_CLOSE );

#ifdef OPERATING_SYSTEM
#elif _LINUX
	SubRefCount();
#endif

	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	m_uiCloseTick = DoGetTickCount();

	m_enumChannelStatus = CHANNEL_CLOSE;

	//�������˵�UDPͨѶͨ�����ܹر�socket. ֻ��Ҫ����ͨ����־λ�˳����ɣ��̻߳��ͷ���Դ��
	if ( SERVER == m_enumServerType && NET_PROTOCOL_UDP == m_enumNetProType && COMM_CHANNEL == m_enumChannelType )
	{
		return ERROR_BASE_SUCCESS;
	}	
	//�ر�sokcet
	UINT16 iRet = 	CloseHandle();



	return iRet;

}

/********************************************************************************
Function:       GetListenChannel
Description:    ��ȡͨ���ļ���ͨ����TCP��UDP�����ԡ�
Input:  
Output:         
Return:         ����У������ؼ���ͨ����ָ��ISocketChannel*�����û�н�����NULL��
Note:			  ����ÿ��ͨ�����м���ͨ��������ͻ���ͨ����û�С�
********************************************************************************/
ISocketChannel*	CSocketChannel::GetListenChannel()
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( NULL == m_pListenSocketChannel)
	{
		return NULL;
	}

	return m_pListenSocketChannel;
}
/********************************************************************************
Function:       GetNetProType
Description:    ��ȡ����Э������
Input:  
Output:         
Return:         ���enumNetProtocolType
Note:				
********************************************************************************/
enumNetProtocolType	CSocketChannel::GetNetProType()
{
	return m_enumNetProType;
}
void CSocketChannel::SetLastActiveTime()
{ 
	m_GSMutex.Lock();
	m_uiLastActiveTime = DoGetTickCount();
	m_GSMutex.Unlock();
}
INT	CSocketChannel::SetLocalIPPort(const char* pszIP, UINT16 unPort)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (unPort < 0)
	{
		return ERROR_NET_PARAM_WRONG;
	}
	if ( pszIP != NULL )
	{
		memset(m_szLocalIP, 0x0, 16);
		strcpy(m_szLocalIP, pszIP);
	}
	
	m_unLocalPort = unPort;

	return ERROR_BASE_SUCCESS;
}
INT	CSocketChannel::SetReMoteIPPort(const char* pszIP, UINT16 unPort)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (pszIP == NULL || unPort <0)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	if (strlen(pszIP) >= 256)
	{
		assert(0);
	}

	memset(m_szRemoteHost, 0x0, 256);
	strcpy(m_szRemoteHost, pszIP);
	m_unRemotePort = unPort;

	return ERROR_BASE_SUCCESS;

}
/*************************************************
  Function:       SetCbaseSocketPoint
  Description:    ����ÿ��ͨ����CBaseSocket����ָ��
  Input:  
  Output:         
  Return:         
  Note:				
*************************************************/
INT CSocketChannel::SetCbaseSocketPoint(CBaseSocket* pBaseSocket)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (pBaseSocket == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_pBaseSocket = pBaseSocket;

	return ERROR_BASE_SUCCESS;
}

INT	CSocketChannel::SetListenSocketChannel(CSocketChannel*	pSocketChannel)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (pSocketChannel == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_pListenSocketChannel = pSocketChannel;

	return ERROR_BASE_SUCCESS;
}

/*************************************************
  Function:       RecvData
  Description:    ������
  Input:	
  Output:         
  Return:         
  Note:				
*************************************************/
INT	CSocketChannel::RecvData()
{	
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);
	if ( m_pBaseSocket->GetSocket() <= 0 )
	{
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p�Ѿ��ر�,���ٽ������� \n",this);
#if OPERATING_SYSTEM

#elif _WIN32

		m_bIOCPNoUse = FALSE; //IOCP����ʹ�ø�ͨ�� 

#endif
		return -1;
	}
#if _WIN32

	m_bIOCPNoUse = TRUE; //IOCP��Ҫʹ�ø�ͨ�� 

#endif
	
	//ֻ�н����̵߳��ô˺�������˲�����
	memset(&(m_pIOData_Recv->Overlapped), 0x0,sizeof(OVERLAPPED));	
	//memset(&(m_pIOData_Recv->Buffer),0x0,DATA_BUFSIZE);				 //������  ��0  �ȽϺ�ʱ ��ʱ��Ҫ
	m_pIOData_Recv->DataBuf.len = DATA_BUFSIZE;
	m_pIOData_Recv->DataBuf.buf = m_pIOData_Recv->Buffer;
	m_pIOData_Recv->OptionType = RECV_POSTED;
	memset(&(m_pIOData_Recv->struSockAddrFrom), 0x0, sizeof(sockaddr_in));
	m_pIOData_Recv->iAddrFromLen = sizeof(sockaddr_in);

	int iRet = m_pBaseSocket->RecvData((void*)m_pIOData_Recv);

#ifdef OPERATING_SYSTEM

	return iRet;

#elif _LINUX

	return iRet;

#else	// ��ɶ˿�

	if (  1 != iRet  )
	{
		CTRLPRINTF(m_clsLogPtr,"ͨ��%p,��������ʧ��\n",this);

		m_bIOCPNoUse = FALSE; //IOCP����ʹ�ø�ͨ�� 
		// ��������ʧ�� ˵�������ϲ��Լ������رգ������Ҫ�����ϲ��ж�
		if (m_enumChannelStatus != CHANNEL_CLOSE)
		{
			m_enumChannelStatus = CHANNEL_DISCNN;
		}
		else
		{
			CTRLPRINTF(m_clsLogPtr,"ͨ��%p,�Ѿ��ǹر�״̬\n",this);
		}
		
	}

	return iRet;

#endif
}

#if OPERATING_SYSTEM

#elif _LINUX
void CSocketChannel::InitEpollEvent()
{
	m_pstruEpollEvent->events = EPOLLIN | EPOLLET;
	m_pstruEpollEvent->data.ptr = this;
}

#endif

INT	CSocketChannel::CloseHandle()
{
	if (m_pBaseSocket == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	return m_pBaseSocket->CloseSocket();
}

//�˹�������������TCP�ͻ��˵���	
INT	CSocketChannel::ReConnect()
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( m_enumNetProType == NET_PROTOCOL_TCP && m_enumServerType == CLIENT && m_pBaseSocket && m_enumChannelStatus == CHANNEL_DISCNN) //Ӧ������һ��ͨ���Ƿ��жϵ�״̬�жϲ�����
	{
		m_pBaseSocket->CloseSocket();
		m_pBaseSocket->CreateSocket( m_szLocalIP, m_unLocalPort );
		INT iRet = m_pBaseSocket->Connect( m_szRemoteHost, m_unRemotePort );
		if (iRet == ERROR_BASE_SUCCESS )
		{
			SetChannelStatus( CHANNEL_ADD_IOCPOREPOLL );//�����ɹ��� ��Ҫ���¼�����ɶ˿ڻ�epoll
		}

		return iRet;
	}
	else
	{
		return ERROR_NET_PARAM_WRONG;
	}
}

void CSocketChannel::SetChannelStatus(enumChannelStatus enumStatus)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( CHANNEL_CLOSE == m_enumChannelStatus )
	{
		//���ڹر�״̬��ͨ�� ����Ҫ�����ó�ʱ������״̬�������޷��ر�
		return;
	}

	m_enumChannelStatus = enumStatus;
}
// ����socket���ͻ�������С
INT	CSocketChannel::SetSendBuf(INT iBufSize)
{
	if ( iBufSize <=0 )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	if ( NULL == m_pBaseSocket )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( 0 != m_pBaseSocket->SetSendBuf( iBufSize ) )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_unSocketSendBuf = iBufSize;

	return ERROR_BASE_SUCCESS;

}
// ����socket���ջ�������С
INT	CSocketChannel::SetRcvBuf(INT iBufSize)
{
	if ( iBufSize <=0 )
	{
		return ERROR_NET_PARAM_WRONG;
	}
	if ( NULL == m_pBaseSocket )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( 0 != m_pBaseSocket->SetRcvBuf( iBufSize ) )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_unSocketRcvBuf = iBufSize;

	return ERROR_BASE_SUCCESS;
}

//��������ģʽ�������������
INT CSocketChannel::SetNetBlockMode( BOOL bMode)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( NULL == m_pBaseSocket )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	// ����ͨ����������������ģʽ�������һ��ǿ��Ϊ������ʽ
	if ( LISTEN_CHANNEL == m_enumChannelType )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_pBaseSocket->SetBlockModeEx( bMode );

	return ERROR_BASE_SUCCESS;

}

INT	CSocketChannel::SendDataEx(void* pData, UINT unDataLen)
{
	if (m_pBaseSocket == NULL || pData == NULL || unDataLen > 8192)		// socketĬ�ϵĻ�������С����8192 �Ҿ���û��Ҫ����Ĭ��ֵ
	{
		return -1;
	}

	if ( CHANNEL_NORMAL != m_enumChannelStatus && CHANNEL_TIMEOUT != m_enumChannelStatus )//��������״̬��ͨ������������ �����жϡ��رա���ȴ���ɶ˿�
	{ //��ʱҲ���Է�����
		return -2;
	}


	if ( NULL == m_pBaseSocket )
	{
		return -3;//��Ч��ͨ��ָ��
	}

	if ( NULL == m_pBaseSocket->GetSocket())
	{
		//socket�Ѿ����رջ���δ����
		return -4;
	}


	INT nLen = 0;

	m_GSMutex.Lock();

	if ( NET_PROTOCOL_TCP == m_enumNetProType)
	{
		
		nLen = m_pBaseSocket->SendDataEx( (char*)pData , unDataLen );
	}
	else
	{
		nLen = m_pBaseSocket->SendDataTo(pData, unDataLen, m_szRemoteHost, m_unRemotePort);
	}

	//m_GSWRMutex.UnlockWrite();
	m_GSMutex.Unlock();

	//���۷��ͳɹ���ʧ�ܶ���Ϊ��ͨ���ǻ��
	SetLastActiveTime();

	//���������

	return nLen;


}

INT	CSocketChannel::GetSocketHandle(void)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);
	return m_pBaseSocket->GetSocket();
}



