#include "CommunicationManager.h"

using namespace NetServiceLib;

CCommunicationManager::CCommunicationManager(void)
{
	m_bIsExitLinsten = true;
	m_bIsExitActiveTest = true;
	m_bIsExitAcceptData = true;
	m_bIsExitActiveTest = true;
	m_bIsExitChannelStatus = true;
	m_bIsExitAcceptUpNotice = true;
	m_bIsSleep = false;
#if OPERATING_SYSTEM
	m_bIsExitSelectEvent = true;
#endif
#if _LINUX
	m_bIsExitEpollEventWait = true;
#endif
}

CCommunicationManager::~CCommunicationManager(void)
{

}
/********************************************************************************
  Function:       Init
  Description:    ��ʼ��
  Input:  
  Output:         
  Return:         
  Note:				
********************************************************************************/
INT	CCommunicationManager::Init()
{
	CNetInterfaceCommData::Init();

	return ERROR_BASE_SUCCESS;
}

/********************************************************************************
  Function:       ChannelActiveTest
  Description:    ͨ������
  Input:  
  Output:         
  Return:         
  Note:				
********************************************************************************/
INT	CCommunicationManager::ChannelActiveTest(enumThreadEventType enumEvent, void* pObject)
{
	if (enumEvent != ACTIVETEST)
	{
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Lock();
	if ( !m_bIsExitActiveTest )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();
	
	while (m_bIsExitActiveTest)//ʲôʱ���˳���ѭ��
	{
		if (m_unActiveTime > 0)	// �����غ����޹�������޸� Ĭ�ϲ�����ͨ����ʱ��� �����û������˸�ֵ
		{
			WAITFOREVENT_SLEEP( ACTIVE_TEST_TIME );

			TestActiveTime();
		}
		else
		{
			
			MSLEEP(10);
		}
		
	}

	return ERROR_BASE_SUCCESS;
}

/********************************************************************************
  Function:       WaitForEvent
  Description:    �̵߳ȴ�ʱ�� ���������� Ҳ�������˳�
  Input:  
  Output:         
  Return:         
  Note:				bool IsExitFlag ������ֵ�ﲻ��Ŀ��
********************************************************************************/
INT	CCommunicationManager::WaitForEvent( DWORD dwSleep, DWORD dwSpace )
{
	if (dwSleep <1)
	{
		dwSleep=1;
	}
	if (dwSpace<1)
	{
		dwSpace=1;
	}

	for (DWORD i=0; i<dwSleep; i+=dwSpace)
	{		
		// ��ʱ:��ֹ��ͣ������ռ��CPUʱ��

		if ( m_bIsExitActiveTest == false )
		{
			return -1;
		}

		MSLEEP(dwSpace);		
	}

	return ERROR_BASE_SUCCESS;
}


//�ͷ����и���ͨ��
INT	CCommunicationManager::FreeAllExtraChannel(CSocketChannel* pclsSocketChannel)
{
	if ( NULL == pclsSocketChannel )
	{
		return ERROR_NET_PARAM_WRONG;
	}
	
	m_GSMutexVerChnl.Lock();

	VectorChannelPoint::size_type i=0;
	bool bExitFlag = true;
	while ( bExitFlag )
	{
		bExitFlag = false;	//�Ҳ���ƥ��ľ��˳�

		//Ҫ��֤����ɾ���Բ���
		for ( i=0; i< m_vectorSocketChannel.size(); i++)
		{
			if ( m_vectorSocketChannel[i]->GetListenChannel() == pclsSocketChannel)
			{
				vector<CSocketChannel*>::iterator pIter;
				pIter = find(m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), m_vectorSocketChannel[i]);
				delete m_vectorSocketChannel[i];
				m_vectorSocketChannel.erase(pIter);
				bExitFlag = true;	//��ƥ��� �����whileѭ��
				break;
			}
			
		}

	}
	
	m_GSMutexVerChnl.Unlock();

	return ERROR_BASE_SUCCESS;
}

//�ͷ�ȫ��ͨ��
INT	CCommunicationManager::FreeAllChannel()
{
	m_GSMutexVerChnl.Lock();

	VectorChannelPoint::size_type i=0;
	for ( i=0; i< m_vectorSocketChannel.size(); i++)
	{
		m_vectorSocketChannel[i]->CloseChannel();

	}

	m_GSMutexVerChnl.Unlock();

	m_GSMutexVerFaultChnl.Lock();

	for ( i=0; i< m_vectorFaultChannel.size(); i++)
	{
		m_vectorFaultChannel[i]->CloseChannel();

	}

	m_GSMutexVerFaultChnl.Unlock();

	return ERROR_BASE_SUCCESS;
}

//��������
INT	CCommunicationManager::ReConnectChannel( CSocketChannel* pclsSocketChannel )
{
	if (m_bReConnect)
	{
		char	szIP[256] = {0};
		UINT16	unPort = 0;
		pclsSocketChannel->GetReMoteIPPort( szIP, unPort );
		CBaseSocket* pBaseSocket = pclsSocketChannel->GetCbaseSocketPoint();
		pBaseSocket->CloseSocket();
		char szLocalIP[256] = {0};
		UINT16 unLocalPort = 0;
		pclsSocketChannel->GetLocalIPPort( szLocalIP, unLocalPort );
		pBaseSocket->CreateSocket( szLocalIP, unLocalPort );
		return pBaseSocket->Connect( szIP, unPort );
	}

	return ERROR_NET_PARAM_WRONG;
}


// �������ͨ������
void CCommunicationManager::CheckNormalChannelDeque()
{
	m_GSMutexVerChnl.Lock();
	for (VectorChannelPoint::size_type i=0; i<m_vectorSocketChannel.size(); i++)
	{
		if (m_vectorSocketChannel[i]->GetChannelStatus() == CHANNEL_NORMAL)
		{
			continue;
		}

		else if ( m_vectorSocketChannel[i]->GetChannelStatus() == CHANNEL_CLOSE)
		{
			//�ر�

			CSocketChannel* pclsSocketChannel = m_vectorSocketChannel[i];

			//m_GSMutexVerChnl.Unlock();

			SaveToFaultVector( pclsSocketChannel );
			//DeleteSocketChannel(pclsSocketChannel);
			vector<CSocketChannel*>::iterator pIter;
			pIter = find(m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), pclsSocketChannel);

			if (pIter != m_vectorSocketChannel.end())
			{
				m_vectorSocketChannel.erase(pIter);
			}
			else
			{
				assert(0);
			}


			//m_GSMutexVerChnl.Lock();

			i=-1;	//���¿�ʼ���
			continue;
			//break;

		}
		else if ( m_vectorSocketChannel[i]->GetChannelStatus() == CHANNEL_DISCNN)
		{
			CSocketChannel* pclsSocketChannel = m_vectorSocketChannel[i];

			//m_GSMutexVerChnl.Unlock();

			pclsSocketChannel->m_csCallBackGSMutex.Lock();
			if ( m_vectorSocketChannel[i]->GetChannelStatus() == CHANNEL_DISCNN)
			{
				pclsSocketChannel->AddRefCount();
				pclsSocketChannel->m_uiChnIng = 2;

				char szRemoteIP[16]={0};
				UINT16 uiRemotePort = 0;
				char szLocalPort[16]={0};
				UINT16 uiLocalPort = 0;

				memset(szRemoteIP,0,16);
				uiRemotePort = 0;
				pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
				memset(szLocalPort,0,16);
				uiLocalPort = 0;
				pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);

				CTRLPRINTF(m_clsLogPtr, "֪ͨ�ϲ�ͨ��%p�ж�, szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n ",\
					pclsSocketChannel,szRemoteIP,uiRemotePort,uiLocalPort);

				//֪ͨ�ϲ㣬Զ�������Ͽ�����������Ƿ������˼�������������ͨ�����ϲ�Ӧֹͣ�������ݣ���CloseChannel��
				OnEventCallBack( pclsSocketChannel, NET_REMOTE_DISCNN, NULL, 0);
				CTRLPRINTF(m_clsLogPtr, "֪ͨ�ϲ�ͨ��%p�ж�,֪ͨ���, szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n ",\
					pclsSocketChannel,szRemoteIP,uiRemotePort,uiLocalPort);


				pclsSocketChannel->m_uiChnIng = 0;
				pclsSocketChannel->SubRefCount();

			}
			pclsSocketChannel->m_csCallBackGSMutex.Unlock();			


			//��������ǿͻ��ˣ�����⽫���������������в���
			if ( CLIENT == pclsSocketChannel->GetServerType() && m_bReConnect )
			{
				ChannelReconnect(pclsSocketChannel);

			}

			//m_GSMutexVerChnl.Lock();

			i=-1;	//���¿�ʼ���

			if ( pclsSocketChannel->GetChannelStatus() == CHANNEL_DISCNN)
			{
				//m_GSMutexVerChnl.Unlock();

				SaveToFaultVector( pclsSocketChannel );
				//DeleteSocketChannel(pclsSocketChannel);
				vector<CSocketChannel*>::iterator pIter;
				pIter = find(m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), pclsSocketChannel);

				if (pIter != m_vectorSocketChannel.end())
				{
					m_vectorSocketChannel.erase(pIter);
				}
				else
				{
					assert(0);
				}


				//m_GSMutexVerChnl.Lock();

				continue;

			}

		}
		else
		{
			//��Ӧ�ó��ִ�״̬
		}

	}

	m_GSMutexVerChnl.Unlock();
}

// ������ͨ������
void CCommunicationManager::CheckFaultChannelDeque()
{
	m_GSMutexVerFaultChnl.Lock();
	for (VectorChannelPoint::size_type i=0; i<m_vectorFaultChannel.size(); i++)
	{
		if ( m_vectorFaultChannel[i]->GetChannelStatus() == CHANNEL_CLOSE )
		{

			if ( m_vectorFaultChannel[i]->GetCloseTick() == 0)
			{
				continue;
			}

			// ����60��Ͳ���Ҫ������������ ֱ��ִ������Ĵ��� �ͷ�ͨ��
			UINT64 iTick = DoGetTickCount() - m_vectorFaultChannel[i]->GetCloseTick(); 

			if ( iTick < 30*1000 )	// 10��
			{
				if (m_vectorFaultChannel[i]->GetRefCount() > 0 )
				{
					// ֱ�����ü�������0��ɾ��

					continue;
				}

				if (m_vectorFaultChannel[i]->GetIOCPNoUse())
				{
					// Ҫ�ȵ���ɶ˿ڲ�ʹ���˲�ɾ��
					continue;
				}

			}
			else
			{
				if ( m_vectorFaultChannel[i]->GetRefCount() != 0 || m_vectorFaultChannel[i]->m_uiChnIng != 0)
				{
					char szRemoteIP[16]={0};
					UINT16 uiRemotePort = 0;
					char szLocalPort[16]={0};
					UINT16 uiLocalPort = 0;

					memset(szRemoteIP,0,16);
					uiRemotePort = 0;
					m_vectorFaultChannel[i]->GetReMoteIPPort(szRemoteIP,uiRemotePort);
					memset(szLocalPort,0,16);
					uiLocalPort = 0;
					m_vectorFaultChannel[i]->GetLocalIPPort(szLocalPort,uiLocalPort);
					CTRLPRINTF(m_clsLogPtr, "ͨ��%p�����ü�����ص���־δ�ܹ��� m_uiRefCount:%u, m_bIOCPNoUse:%d,m_uiChnIng:%u, szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",m_vectorFaultChannel[i],
						m_vectorFaultChannel[i]->GetRefCount(), m_vectorFaultChannel[i]->GetIOCPNoUse(),m_vectorFaultChannel[i]->m_uiChnIng, szRemoteIP, uiRemotePort, uiLocalPort );
					assert(0);
				}

			}

			CSocketChannel* pclsSocketChannel = m_vectorFaultChannel[i];

			//m_GSMutexVerFaultChnl.Unlock();

			//DeleteoFromFaultVector( pclsSocketChannel );

			VectorChannelPoint::iterator pIter;
			pIter = find(m_vectorFaultChannel.begin(), m_vectorFaultChannel.end(), pclsSocketChannel);
			if (pIter != m_vectorFaultChannel.end())
			{
				m_vectorFaultChannel.erase(pIter);

			}
			else
			{
				assert(0);
			}

			CTRLPRINTF(m_clsLogPtr, "�ͷ�ͨ��%p�ڴ�\n ",pclsSocketChannel);

			delete pclsSocketChannel;

			//m_GSMutexVerFaultChnl.Lock();

			i=-1;	//���¿�ʼ���
			continue;
			//break;
		}
		else if ( m_vectorFaultChannel[i]->GetChannelStatus() == CHANNEL_ADD_IOCPOREPOLL)
		{
			//����epoll or iocp
			CSocketChannel* pclsSocketChannel = m_vectorFaultChannel[i];
			pclsSocketChannel->SetChannelStatus( CHANNEL_NORMAL );				

			//m_GSMutexVerFaultChnl.Unlock();

			//DeleteoFromFaultVector( pclsSocketChannel );
			VectorChannelPoint::iterator pIter;
			pIter = find(m_vectorFaultChannel.begin(), m_vectorFaultChannel.end(), pclsSocketChannel);

			if (pIter != m_vectorFaultChannel.end())
			{
				m_vectorFaultChannel.erase(pIter);

			}
			else
			{
				assert(0);
			}

			SaveSocketChannel( pclsSocketChannel );

			OnEventModel( pclsSocketChannel );


			//m_GSMutexVerFaultChnl.Lock();

			i=-1;	//���¿�ʼ���
			continue;
			//break;

		}			
		else
		{
			//����״̬������
		}

	}

	m_GSMutexVerFaultChnl.Unlock();
}

//ͨ������
void CCommunicationManager::ChannelReconnect(CSocketChannel*	pclsSocketChannel)
{
	//����
	if( ERROR_BASE_SUCCESS != ReConnectChannel( pclsSocketChannel ))
	{
		//����ʧ��
		OnEventCallBack( pclsSocketChannel, NET_RECONNECT_FAIL, NULL, 0 );

	}
	else
	{

		//�����ɹ�
		pclsSocketChannel->SetChannelStatus( CHANNEL_NORMAL );

		OnEventCallBack( pclsSocketChannel, NET_RECONNECT_SUCCESS, NULL, 0 );

		OnEventModel( pclsSocketChannel );

	}
}

//�Ͽ�����
/********************************************************************************
Function:		DealDisnChannel
Description:	�Ͽ�����
Input:  		pclsSocketChannel
Output:      	   
Return:  		       
Note:					
Author:        	CHC
Date:				2010/08/11
********************************************************************************/
void CCommunicationManager::DealDisnChannel( CSocketChannel*	pclsSocketChannel)
{
	if (pclsSocketChannel == NULL || pclsSocketChannel->GetChannelStatus() == CHANNEL_CLOSE)
	{
		return;
	}

	pclsSocketChannel->SetChannelStatus( CHANNEL_DISCNN );

}

/********************************************************************************
Function:		ThreadChannelStatus
Description:	����ͨ����״̬�����жϡ��رա��ȵ�
Input:  		
Output:      	   
Return:  		       
Note:					
********************************************************************************/
INT	CCommunicationManager::ThreadChannelStatus(enumThreadEventType enumEvent, void* pObject)

{
	if (enumEvent != CHANNELSTATUS )
	{

		return ERROR_NET_PARAM_WRONG;
	}

	UINT64 uiTick = DoGetTickCount();

	m_GSMutexExit.Lock();
	if ( !m_bIsExitChannelStatus )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();
	while ( m_bIsExitChannelStatus )
	{
		int iRet = m_GSCond.WaitTimeout(10);
		if (iRet == 0)
		{
			break;
		}

		// �������ͨ��״̬����
		CheckNormalChannelDeque();

		CheckFaultChannelDeque();

		// �����־

		if ( DoGetTickCount() - uiTick > 20 *1000 )
		{
			// ÿ30�������1�������״̬			
#ifdef _DEBUG

			CTRLPRINTF(m_clsLogPtr,"------------��ǰ�����״̬��������------------\n");
			m_GSMutexVerChnl.Lock();
			m_GSMutexVerFaultChnl.Lock();
			INT16 iNum = m_vectorSocketChannel.size() + m_vectorFaultChannel.size();
			CTRLPRINTF(m_clsLogPtr,"��ǰ�������ͨ����Ŀ�� %d \n", iNum);
			char szRemoteIP[16]={0};
			UINT16 uiRemotePort = 0;
			char szLocalPort[16]={0};
			UINT16 uiLocalPort = 0;
			for (VectorChannelPoint::size_type i=0; i<m_vectorSocketChannel.size(); i++)
			{
				memset(szRemoteIP,0,16);
				uiRemotePort = 0;
				m_vectorSocketChannel[i]->GetReMoteIPPort(szRemoteIP,uiRemotePort);
				memset(szLocalPort,0,16);
				uiLocalPort = 0;
				m_vectorSocketChannel[i]->GetLocalIPPort(szLocalPort,uiLocalPort);
				CTRLPRINTF( m_clsLogPtr,"ͨ������N szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u, ChannelStatus��%d,m_uiRefCount:%u, m_bIOCPNoUse:%d,m_uiChnIng:%u \n",\
					szRemoteIP, uiRemotePort,uiLocalPort,m_vectorSocketChannel[i]->GetChannelStatus(),m_vectorSocketChannel[i]->GetRefCount(), m_vectorSocketChannel[i]->GetIOCPNoUse(),m_vectorSocketChannel[i]->m_uiChnIng );
			}

			for (VectorChannelPoint::size_type i=0; i<m_vectorFaultChannel.size(); i++)
			{
				memset(szRemoteIP,0,16);
				uiRemotePort = 0;
				m_vectorFaultChannel[i]->GetReMoteIPPort(szRemoteIP,uiRemotePort);
				memset(szLocalPort,0,16);
				uiLocalPort = 0;
				m_vectorFaultChannel[i]->GetLocalIPPort(szLocalPort,uiLocalPort);
				CTRLPRINTF( m_clsLogPtr,"ͨ������F szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u, ChannelStatus��%d,m_uiRefCount:%u, m_bIOCPNoUse:%d,m_uiChnIng:%u \n",\
					szRemoteIP, uiRemotePort,uiLocalPort,m_vectorFaultChannel[i]->GetChannelStatus(),m_vectorFaultChannel[i]->GetRefCount(), m_vectorFaultChannel[i]->GetIOCPNoUse(),m_vectorFaultChannel[i]->m_uiChnIng );
			}

			

			m_GSMutexVerFaultChnl.Unlock();
			m_GSMutexVerChnl.Unlock();
#endif
			
			uiTick = DoGetTickCount();

		}	




	}

	return ERROR_BASE_SUCCESS;


}
// UDP,���ݸ�ͨ��������ͨ����������������� ʧ�ܷ���NULL
CSocketChannel*	CCommunicationManager::CreateUdpChannel( CSocketChannel* pParentSocketChannel, LPPER_IO_OPERATION_DATA PerIoData )
{
	if ( NULL == pParentSocketChannel )
	{
		return NULL;
	}

	//������ͨ��
	CSocketChannel* pclsNewSocketChannel = NULL;
	pclsNewSocketChannel = new CSocketChannel();
	if ( pclsNewSocketChannel )
	{
		pclsNewSocketChannel->SetCbaseSocketPoint(pParentSocketChannel->GetCbaseSocketPoint());
		pclsNewSocketChannel->SetListenSocketChannel(pParentSocketChannel);
		pclsNewSocketChannel->SetChannelType( COMM_CHANNEL );
		pclsNewSocketChannel->SetNetProtocolType( NET_PROTOCOL_UDP );
		pclsNewSocketChannel->SetServerType( SERVER );		

		char szIP[16] = { 0 };		
		UINT16 unPort = 0;
		pParentSocketChannel->GetLocalIPPort( szIP, unPort );
		pclsNewSocketChannel->SetLocalIPPort( szIP, unPort );

		memset( szIP, 0x0 , 16 );
		strcpy(szIP, inet_ntoa(PerIoData->struSockAddrFrom.sin_addr));
		pclsNewSocketChannel->SetReMoteIPPort( szIP, unPort );
		pclsNewSocketChannel->SetDWORDRemoteIP( PerIoData->struSockAddrFrom.sin_addr.s_addr );

		pclsNewSocketChannel->SetLogInstancePtr(m_clsLogPtr);	

		//�յ����ݣ�����ͨ���ʱ��
		pclsNewSocketChannel->SetLastActiveTime();

		
	}
	else
	{
		// �����ڴ�ʧ��
		assert(0);
		return NULL;
	}

	return pclsNewSocketChannel;
}
/********************************************************************************
Function:		Listen
Description:	������
Input:  		enumEvent ����Ĳ���������LISTEN��pObject ����ָ��CSocketChannel����,�������ĸ�ͨ������
Output:      	   
Return:  		��ȷΪERROR_BASE_SUCCESS�����������������       
Note:			
********************************************************************************/
INT  CCommunicationManager::Listen(enumThreadEventType enumEvent, void* pObject)
{
	if (enumEvent != LISTEN || pObject == NULL)
	{
		//1 д����־ ���������� �����Ǵ������
		//2 ����
		return ERROR_NET_PARAM_WRONG;
	}

	CSocketChannel* pclsSocketChannel = (CSocketChannel*)pObject;

	CBaseSocket* pclsCBaseSocket = pclsSocketChannel->GetCbaseSocketPoint();
	if ( NULL == pclsCBaseSocket)
	{
		return ERROR_NET_UNKNOWN;
	}
	INT iRet = pclsCBaseSocket->Listen();

#ifdef _WIN32
	if (iRet == SOCKET_ERROR)
	{
		INT iErr = GetLastError();
		CTRLPRINTF(m_clsLogPtr,"listen fail %d \n", iErr);
	}
#endif

	sockaddr_in ClientAddr;
	INT nLen = sizeof(ClientAddr);	
	memset(&ClientAddr, 0x0, nLen);
	SOCKETHANDLE ClientAccept;

	m_GSMutexExit.Lock();
	if ( !m_bIsExitLinsten )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();

	pclsSocketChannel->AddRefCount();
	while (m_bIsExitLinsten == true)//Ҫ�������ѭ��ʲôʱ���˳�
	{		
		ClientAccept = pclsCBaseSocket->AcceptConnect((sockaddr*)&ClientAddr, &nLen);

		if ( ClientAccept <= 0 )
		{
			if( pclsSocketChannel->GetChannelStatus() == CHANNEL_CLOSE )
			{	
				CTRLPRINTF(m_clsLogPtr, "�����:�رռ���ͨ�� %p\n", pclsSocketChannel);
				break;	
			}
#ifdef _LINUX
			MSLEEP(1);
#endif
			continue;//δ�ر� �������ռ���
		}

		StruSocketInfoPtr pStruSocketInfo = new STRUSOCKETINFO();
		if (pStruSocketInfo == NULL)
		{
			CTRLPRINTF(m_clsLogPtr, "�����:�����ڴ�ʧ��\n");	
#ifdef _WIN32
			closesocket( ClientAccept );//����д�ƻ��˷�װ��
#endif
#ifdef _LINUX
			close( ClientAccept );//����д�ƻ��˷�װ��
#endif
			memset(&ClientAddr, 0x0, nLen);
			ClientAccept = NULL;
			continue;
		}

		pStruSocketInfo->iSocket = ClientAccept;
		pStruSocketInfo->pListenChannel = pclsSocketChannel;
		pStruSocketInfo->ClientAddr = ClientAddr;
		pStruSocketInfo->nLen = nLen;

		m_GSMutexSocketInfoList.Lock();
		pclsSocketChannel->AddRefCount();
		m_SocketInfoList.push(pStruSocketInfo);
		m_GSMutexSocketInfoList.Unlock();
		if (m_bIsSleep)
		{
			m_GSCondAcceptUpNotice.Signal();
		}

	

	}

	pclsSocketChannel->SubRefCount();

	return ERROR_BASE_SUCCESS;
}

// ֪ͨ�ϲ������ӵ���
INT	CCommunicationManager::NoticeUpNewConnect(enumThreadEventType enumEvent, void* pObject)
{
	if (enumEvent != ACCEPTNOTICE )
	{
		//1 д����־ ���������� �����Ǵ������
		//2 ����
		return ERROR_NET_PARAM_WRONG;
	}


	m_GSMutexExit.Lock();
	if ( !m_bIsExitAcceptUpNotice )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();

	SocketInfoList	listSocketInfo;
	SOCKETHANDLE ClientAccept;
	sockaddr_in ClientAddr;
	INT nLen = sizeof(ClientAddr);	
	memset(&ClientAddr, 0x0, nLen);

	CTRLPRINTF(m_clsLogPtr, "֪ͨ�ϲ������ӵ����߳̿�ʼ����\n");

	while (m_bIsExitAcceptUpNotice)//Ҫ�������ѭ��ʲôʱ���˳�
	{

		// ���������һ������
		while(!listSocketInfo.empty())
		{
			listSocketInfo.pop();
		}

		// ����ȡ���ݵ��ֲ�����
		m_GSMutexSocketInfoList.Lock();
		while(!m_SocketInfoList.empty())
		{
			StruSocketInfoPtr	pData = m_SocketInfoList.front();
			listSocketInfo.push(pData);
			m_SocketInfoList.pop();
		}
		m_GSMutexSocketInfoList.Unlock();

		if (listSocketInfo.empty())
		{
			m_bIsSleep = true;
			int iRet = m_GSCondAcceptUpNotice.WaitTimeout(100);
			m_bIsSleep = false;
			if (iRet == 0 && !m_bIsExitAcceptUpNotice )
			{
				break;
			}
			else
			{
				continue;
			}
		}

		

		// ֪ͨ�ϲ�
		while(!listSocketInfo.empty())
		{
			if (!m_bIsExitAcceptUpNotice)
			{
				return ERROR_BASE_SUCCESS;
			}
			StruSocketInfoPtr	pData = listSocketInfo.front();
			listSocketInfo.pop();

			ClientAccept = pData->iSocket;//sokcet
			CSocketChannel* pclsSocketChannel = pData->pListenChannel;//����ͨ��
			ClientAddr = pData->ClientAddr;
			nLen = pData->nLen;

			if ( ClientAccept <= 0 )
			{
				if( pclsSocketChannel->GetChannelStatus() == CHANNEL_CLOSE )
				{	
					CTRLPRINTF(m_clsLogPtr, "�����:�رռ���ͨ��\n");
				}
#ifdef _LINUX
				MSLEEP(1);
#endif
				CTRLPRINTF(m_clsLogPtr, "�����:�����socket %d\n",ClientAccept);
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;//�����socket ����������һ��
			}



			//��Ҫ�ж��ǲ����Ѵﵽ������������ﵽ�͹رո����ӽ�����ͨ��
			if (IfMaxChannelCount())
			{
				//�Ѿ��ﵽ���������
#ifdef _WIN32
				closesocket( ClientAccept );//����д�ƻ��˷�װ��
#endif
#ifdef _LINUX
				close( ClientAccept );//����д�ƻ��˷�װ��
#endif
				memset(&ClientAddr, 0x0, nLen);
				ClientAccept = NULL;
				CTRLPRINTF(m_clsLogPtr, "�����:�ﵽ���ͨ����\n");
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;
			}	


			//������ͨ��
			CSocketChannel* pclsNewSocketChannel = new CSocketChannel();
			//����Զ��IP�˿�
			CBaseSocket* pclsNewCBaseSocket = NULL;
#ifdef _WIN32

			pclsNewCBaseSocket = new CWinTcpSocket();

#endif

#ifdef _LINUX
			pclsNewCBaseSocket = new CLinuxTcpSocket();

#endif

			if ( NULL == pclsNewSocketChannel || NULL == pclsNewCBaseSocket)
			{
				//�����ڴ�ʧ��
				CTRLPRINTF(m_clsLogPtr,"CThreadDealNetEvent::Listen �����ڴ�ʧ�� \n");
#ifdef _WIN32
				closesocket( ClientAccept );//����д�ƻ��˷�װ��
#endif
#ifdef _LINUX
				close( ClientAccept );//����д�ƻ��˷�װ��
#endif
				memset(&ClientAddr, 0x0, nLen);
				ClientAccept = NULL;
				CTRLPRINTF(m_clsLogPtr, "�����: �����ڴ�ʧ��\n");
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;
			}

			//����sokcet ��CBaseSocket�ࡢ����ͨ��
			pclsNewCBaseSocket->SetSocket(ClientAccept);
			pclsNewCBaseSocket->SetBlockMode( m_bBlockMode );
			pclsNewCBaseSocket->SetSockOption();
			pclsNewSocketChannel->SetCbaseSocketPoint(pclsNewCBaseSocket);
			pclsNewSocketChannel->SetListenSocketChannel(pclsSocketChannel);
			pclsNewSocketChannel->SetChannelType( COMM_CHANNEL );
			pclsNewSocketChannel->SetNetProtocolType( NET_PROTOCOL_TCP );
			pclsNewSocketChannel->SetServerType( SERVER );
			pclsNewSocketChannel->SetDWORDRemoteIP( ClientAddr.sin_addr.s_addr );

			//����ͨ���ı���IP��Զ��IP
			pclsNewSocketChannel->SetReMoteIPPort(inet_ntoa(ClientAddr.sin_addr), (UINT16) ntohs(ClientAddr.sin_port));
			sockaddr_in struLocalAddr;
#ifdef _WIN32
			INT32   iLocalAddrLen = sizeof(sockaddr_in);
#endif
#ifdef _LINUX
			socklen_t   iLocalAddrLen = sizeof(sockaddr_in);
#endif

			getsockname( ClientAccept, (sockaddr*)&struLocalAddr, &iLocalAddrLen );
			pclsNewSocketChannel->SetLocalIPPort(inet_ntoa(struLocalAddr.sin_addr), (UINT16) ntohs(struLocalAddr.sin_port));

			pclsNewSocketChannel->SetLogInstancePtr(m_clsLogPtr);

			//6���ص�֪ͨ�û�
			pclsSocketChannel->m_csCallBackGSMutex.Lock();
			if ( pclsSocketChannel->GetChannelStatus() == CHANNEL_CLOSE  )
			{
				pclsNewSocketChannel->CloseChannelEx();
				delete pclsNewSocketChannel;
				pclsNewSocketChannel = NULL;
				pclsSocketChannel->m_csCallBackGSMutex.Unlock();
				CTRLPRINTF(m_clsLogPtr, "�����: ֪ͨ�ϲ�֮ǰ���ر�\n");
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;
			}

			char szRemoteIP[16]={0};
			UINT16 uiRemotePort = 0;
			char szLocalPort[16]={0};
			UINT16 uiLocalPort = 0;

			memset(szRemoteIP,0,16);
			uiRemotePort = 0;
			pclsNewSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
			memset(szLocalPort,0,16);
			uiLocalPort = 0;
			pclsNewSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);

			CTRLPRINTF(m_clsLogPtr, "ͨ��%p�����ɹ�, new socket %d, szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n ",\
				pclsNewSocketChannel,ClientAccept,szRemoteIP,uiRemotePort,uiLocalPort);
			ISocketChannel *pSocketChnl = (ISocketChannel*)pclsNewSocketChannel;
			CTRLPRINTF(m_clsLogPtr, "�ص�֪ͨ�ϲ����ӽ���,new socket %d\n ",ClientAccept);
			OnEventCallBack( pSocketChnl,NET_ACCEPT, NULL, 0 );
			CTRLPRINTF(m_clsLogPtr, "�ص�����\n ");

			if (  NULL == pclsNewSocketChannel->GetCbaseSocketPoint()->GetSocket())
			{
				CTRLPRINTF(m_clsLogPtr, " �ϲ���NET_ACCEPT�ص��йر��� new socket %d \n ", ClientAccept );
				//pclsNewSocketChannel->CloseChannel();
				delete pclsNewSocketChannel;
				pclsNewSocketChannel = NULL;
				pclsSocketChannel->m_csCallBackGSMutex.Unlock();
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;
			}
			pclsSocketChannel->m_csCallBackGSMutex.Unlock();

			CTRLPRINTF(m_clsLogPtr, "ͨ��%p NET_ACCEPT�ص����, new socket %d\n ",pclsNewSocketChannel,ClientAccept);

			memset(&ClientAddr, 0x0, nLen);
			ClientAccept = NULL;		

			pclsNewSocketChannel->m_csCallBackGSMutex.Lock();
			if ( pclsNewSocketChannel->GetChannelStatus() == CHANNEL_CLOSE )
			{
				CTRLPRINTF(m_clsLogPtr, "ͨ��%p ���쳣�ر� \n ",pclsNewSocketChannel);

				pclsNewSocketChannel->m_csCallBackGSMutex.Unlock();
				delete pclsNewSocketChannel;
				pclsNewSocketChannel = NULL;
				pclsSocketChannel->SubRefCount();
				delete pData;
				continue;

			}
			//3������ͨ��
			SaveSocketChannel(pclsNewSocketChannel);  

			// ��Ӧ�¼�
			OnEventModel( pclsNewSocketChannel );

			pclsNewSocketChannel->m_csCallBackGSMutex.Unlock();

			pclsSocketChannel->SubRefCount();
			delete pData;

		}//end while
		
	}


	return ERROR_BASE_SUCCESS;
}