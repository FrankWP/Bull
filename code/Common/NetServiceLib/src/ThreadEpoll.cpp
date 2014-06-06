#include "ThreadEpoll.h"
#if OPERATING_SYSTEM

#elif  defined(_LINUX)



using namespace NetServiceLib;

//�߳�ִ�к��� ������ѯlinux��epoll�¼�
/********************************************************************************
  Function:		EpollWaitEvent
  Description:	�ȴ�socket��epoll�¼��������������¼������̳߳������б����̳߳ش���
  Input:  		enumEvent ����Ĳ���������EPOLLEVENTWAIT��pObject ָ��CCommunicationManager����
  Output:      	   
  Return:  		��ȷΪERROR_BASE_SUCCESS�����������������       
  Note:					
********************************************************************************/
INT CThreadDealNetEvent::EpollWaitEvent(enumThreadEventType enumEvent, void* pObject)
{
	if ( enumEvent != EPOLLEVENTWAIT || pObject == NULL )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	struct		epoll_event events[10];			//�ش���������¼�������
	int			nfds;							//epoll_wait�ķ���ֵ
	int			iIndex;							//��������
	CSocketChannel*  pclsSocketChannel = NULL;

	m_GSMutexExit.Lock();
	if ( !m_bIsExitEpollEventWait )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();

	while( m_bIsExitEpollEventWait )	
	{
		nfds = EpollWait( events, 10, 1 );			

		if (nfds == -1) 
		{
			printf("nfds == -1, %d, %s\n", errno, strerror(errno));
			if( errno == EINTR ) //���жϷ��� 
			{
				continue;
			}
			//������˳�����Ϊʲô
			//exit(-1);			
		}
		else if( nfds == 0 ) 
		{
			//��ʱ
			continue;
		}	

		if ( nfds < -1 )
		{
			printf("epoll error, %d, %s\n", errno, strerror(errno));
		}

		for (iIndex = 0; iIndex < nfds; ++iIndex) 
		{			
			if( !events[iIndex].data.ptr )  
			{
				//���ܳ��ָ�����
				assert(0);
				continue;
			}

			pclsSocketChannel = (CSocketChannel*)events[iIndex].data.ptr;


			if( events[iIndex].events & EPOLLIN ) 
			{ //��ȡ����
				//�޹��ڴ˼����� �Ҳ�֪����Ϊʲô �����Ȳ���
				pclsSocketChannel->DelEpollEvent( ~EPOLLIN );
				EpollCtrl( EPOLL_CTL_MOD,pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), pclsSocketChannel->GetEpollEvent());

				//����������̳߳أ����̳߳ض�д
				pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

				if ( NULL == pTask )
				{
					assert(0);
					continue;
				}

				pclsSocketChannel->AddRefCount();

				memset(pTask,0x0,sizeof(struThreadTask));
				pTask->pFunction = &CCommunicationManager::ThreadAcceptData;
				pTask->enumEvent = ACCEPTDATA;
				pTask->pObject = pObject;	//pObjectָ��CCommunicationManager
				pTask->pObject2 = pclsSocketChannel;
				m_clsThreadPool.AssignTask(pTask);
				if ( NET_PROTOCOL_UDP == pclsSocketChannel->GetNetProtocolType()  )
				{
					// UDP��ʽ���������̣߳�����Ч�ʸ߶���
					m_clsThreadPool.WakeUpThread();
				}
				
			}

		}
	}

	return ERROR_BASE_SUCCESS;
}
/********************************************************************************
Function:		ThreadAcceptData
Description:	�������ݣ����ص�֪ͨ�ϲ�.
Input:  		enumEvent ����Ĳ���������ACCEPTDATA��pObject ����ָ��CSocketChannel����
Output:      	   
Return:  		��ȷΪERROR_BASE_SUCCESS�����������������       
Note:			�����ݽ��պ����ŷ��ء����������ִ�к�EpollWaitEvent����������ϵ
********************************************************************************/
INT CThreadDealNetEvent::ThreadAcceptData(enumThreadEventType enumEvent, void* pObject)
{
	if ( enumEvent != ACCEPTDATA || pObject == NULL )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	//������
	CSocketChannel* pclsSocketChannel = (CSocketChannel*)pObject;;
	INT		nDataLen = 0;
	UINT64	dwTick = DoGetTickCount();
	BOOL	bFirstCircle = TRUE;

	m_GSMutexExit.Lock();
	if ( !m_bIsExitAcceptData )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();
 
	while( m_bIsExitAcceptData )	//ʲôʱ���˳�ѭ��,ֱ��������
	{
		//������
		nDataLen = pclsSocketChannel->RecvData();

		if ( nDataLen <= 0 )
		{
			if ( bFirstCircle )
			{
				//��һ��������û�յ� ˵���������ӶϿ�
				//�����udp�ķ������˶Ͽ� �ǲ������и���ͨ����֪ͨ�ϲ�˵ͨѶ�жϣ���
				if (pclsSocketChannel->GetChannelStatus() != CHANNEL_CLOSE )
				{
					//���������Ͽ� ֪ͨ�ϲ�

					//��EPOLL��ȥ��
					EpollCtrl( EPOLL_CTL_DEL, pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), pclsSocketChannel->GetEpollEvent());

					DealDisnChannel( pclsSocketChannel );


					
				}
				else
				{
					//�ͷ�ͨ����Դ

					//��EPOLL��ȥ��
					EpollCtrl( EPOLL_CTL_DEL, pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), pclsSocketChannel->GetEpollEvent());

				}

				pclsSocketChannel->SubRefCount();

				return ERROR_BASE_SUCCESS; //ֱ���˳��ú���, ��break�������ǲ�һ����
				
			}

			pclsSocketChannel->SubRefCount();			

			break;
		}

		bFirstCircle = FALSE;

		LPPER_IO_OPERATION_DATA  PerIoData = pclsSocketChannel->GetIORecvData();	

		//�յ�����
		if (LISTEN_CHANNEL == pclsSocketChannel->GetChannelType() && NET_PROTOCOL_UDP == pclsSocketChannel->GetNetProtocolType())
		{
			//Ҳ����������㿴��
			//�����UDP��ʽSERVERͨ�� �ж��ǲ����µĿͻ��˷���������

			UINT16  unPort = 0;				
			unPort = ntohs( PerIoData->struSockAddrFrom.sin_port);
			//���Ҹ�ͨ���Ƿ��Ѿ�����
			CSocketChannel*  pclsNewSocketChannel = GetExistSocketChannel( PerIoData->struSockAddrFrom.sin_addr.s_addr, unPort );
			if ( NULL == pclsNewSocketChannel)
			{
				// �ж��Ƿ�ﵽ���ͨ����
				if ( !IfMaxChannelCount())
				{
					//ͨ�������� ����
					pclsNewSocketChannel = CreateUdpChannel(pclsSocketChannel, PerIoData);

					if ( pclsNewSocketChannel  )
					{		
						//3���ص�֪ͨ�û���
						ISocketChannel *pSocketChnl = (ISocketChannel*)pclsNewSocketChannel;
						//�޹�Ҫ���2�λص�����֪ͨ�ϲ��н��� �ٷ�����
						OnEventCallBack( pSocketChnl, NET_ACCEPT, NULL, 0 );

						//���浽����ͨ������ 
						SaveSocketChannel(pclsNewSocketChannel);  

						OnEventCallBack( pSocketChnl, NET_READ, PerIoData->Buffer, nDataLen );
					}
					
				}
				

			}//end if ( NULL == pclsNewSocketChannel)
			else
			{
				//�յ����ݣ�����ͨ���ʱ��

				if (m_unActiveTime > 0)
				{
					pclsNewSocketChannel->SetLastActiveTime();
					pclsNewSocketChannel->SetChannelStatus( CHANNEL_NORMAL );
				}

				//3���ص�֪ͨ�û���
				if (pclsNewSocketChannel->GetChannelStatus() != CHANNEL_CLOSE)
				{
					pclsNewSocketChannel->AddRefCount();
					pclsSocketChannel->m_uiChnIng = 1;
					ISocketChannel *pSocketChnl = (ISocketChannel*)pclsNewSocketChannel;
					OnEventCallBack( pSocketChnl,NET_READ, PerIoData->Buffer, nDataLen );
					pclsSocketChannel->m_uiChnIng = 0;
					pclsNewSocketChannel->SubRefCount();
				}

			}
		}
		else
		{
			//�յ����ݣ�����ͨ���ʱ��

			if (m_unActiveTime > 0)
			{
				pclsSocketChannel->SetLastActiveTime();
				pclsSocketChannel->SetChannelStatus( CHANNEL_NORMAL );
			}

			//3���ص�֪ͨ�û���
			if (pclsSocketChannel->GetChannelStatus() != CHANNEL_CLOSE)
			{
				pclsSocketChannel->m_csCallBackGSMutex.Lock();
				if ( pclsSocketChannel->GetChannelStatus() != CHANNEL_CLOSE )
				{
					pclsSocketChannel->AddRefCount();
					pclsSocketChannel->m_uiChnIng = 1;
					ISocketChannel *pSocketChnl = (ISocketChannel*)pclsSocketChannel;
					OnEventCallBack( pSocketChnl, NET_READ, PerIoData->Buffer, nDataLen );
					pclsSocketChannel->m_uiChnIng = 0;
					pclsSocketChannel->SubRefCount();
				}
				else
				{
					CTRLPRINTF( m_clsLogPtr," ͨ��%p���յ����ݵ�ʱ���ر� \n", pclsSocketChannel);

				}
				pclsSocketChannel->m_csCallBackGSMutex.Unlock();
				
			}
			else
			{						
				CTRLPRINTF( m_clsLogPtr," ͨ��%p���յ����ݵ�ʱ���ر� \n", pclsSocketChannel);
			}

		}

	}//end while

	if ( NULL != pclsSocketChannel)
	{
		if (  CHANNEL_NORMAL == pclsSocketChannel->GetChannelStatus()  )
		{
			pclsSocketChannel->AddEpollEvent( EPOLLIN );
			EpollCtrl( EPOLL_CTL_MOD, pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), pclsSocketChannel->GetEpollEvent());

		}
		

	}
	
	return ERROR_BASE_SUCCESS;

}

// �����¼���Ӧ �̳���CCommunicationManager
void CThreadDealNetEvent::OnEventModel( CSocketChannel* pclsSocketChannel )
{
	EpollCtrl( EPOLL_CTL_ADD, pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), pclsSocketChannel->GetEpollEvent());

	pclsSocketChannel->AddRefCount();
}


#endif //_LINUX


