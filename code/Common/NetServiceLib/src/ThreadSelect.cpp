#include "ThreadSelect.h"

#if OPERATING_SYSTEM

using namespace NetServiceLib;

//�߳�ִ�к��� ������ѯlinux��epoll�¼�
/********************************************************************************
Function:		SelectEvent
Description:	�ȴ�Select�¼��������������¼������̳߳������б����̳߳ش���
Input:  		enumEvent ����Ĳ���������EPOLLEVENTWAIT��pObject ָ��CCommunicationManager����
Output:      	   
Return:  		��ȷΪERROR_BASE_SUCCESS�����������������       
Note:					
********************************************************************************/
INT CThreadDealNetEvent::SelectEvent(enumThreadEventType enumEvent, void* pObject)
{
	timeval timeout; 
	int rt;
	fd_set fd;
	//fd_set efd;
	SOCKETHANDLE	sockMax;	// ����sockMaxֵ

	int iFlag = 0;

	m_GSMutexExit.Lock();
	if ( !m_bIsExitSelectEvent )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();
	while ( m_bIsExitSelectEvent )
	{
		// 1������socket��FD
		//2��select
		//3���ж��Ƿ�ʱ���Ƿ��쳣�����յ�����
		//4����ʱ����ѭ��
		//5���쳣:�����close socket ��ô��Ҫ��������ͬ���������쳣��
		//6�����յ����ݣ���FD�Ƴ���SOCKET�������̳߳�������
		//7���ظ�ѭ��

		iFlag = 0;
		sockMax = -1;

		FD_ZERO(&fd);
		//FD_ZERO(&efd);
		timeout.tv_sec=1;	//1��
		timeout.tv_usec=0;

		m_GSMutexVerChnl.Lock();

		// 1������socket��FD
		for (VectorChannelPoint::size_type i=0; i<m_vectorSocketChannel.size(); i++)
		{
			if ( m_vectorSocketChannel[i]->GetChannelStatus() != CHANNEL_NORMAL )
			{
				continue;
			}
			else if ( m_vectorSocketChannel[i]->GetChannelType() == LISTEN_CHANNEL
				&& m_vectorSocketChannel[i]->GetNetProtocolType() == NET_PROTOCOL_TCP )
			{
				// TCP����ͨ��������
				continue;
			}
			else if ( m_vectorSocketChannel[i]->GetIOCPNoUse())
			{
				// TRUE ���ڶ�����
				continue;
			}

			SOCKETHANDLE sockValue = m_vectorSocketChannel[i]->GetCbaseSocketPoint()->GetSocket();

			if ( sockValue <= 0 )
			{
				assert(0);
				continue;
			}

			sockMax = sockValue > sockMax ? sockValue : sockMax;
			FD_SET(sockValue,&fd);//��
			iFlag++;
		}

		m_GSMutexVerChnl.Unlock();

		if ( iFlag == 0)
		{
			m_GSCond.WaitTimeout(10);
			continue;
		}

		//2��select
		rt = select(sockMax+1,&fd,NULL,NULL,&timeout);

		//3���ж��Ƿ�ʱ���Ƿ��쳣�����յ�����
		if ( rt > 0 )
		{
			// ����Ӧ
			// ����ǶԷ��ر� �������߱��� ���ض� ������������
			m_GSMutexVerChnl.Lock();

			// 1������ĸ�socket����Ӧ
			for (VectorChannelPoint::size_type i=0; i<m_vectorSocketChannel.size(); i++)
			{
				if ( m_vectorSocketChannel[i]->GetChannelStatus() != CHANNEL_NORMAL )
				{
					continue;
				}
				else if ( m_vectorSocketChannel[i]->GetChannelType() == LISTEN_CHANNEL
					&& m_vectorSocketChannel[i]->GetNetProtocolType() == NET_PROTOCOL_TCP )
				{
					// TCP����ͨ��������
					continue;
				}
				else if ( m_vectorSocketChannel[i]->GetIOCPNoUse())
				{
					// TRUE ���ڶ�����
					continue;
				}

				SOCKETHANDLE sockValue = m_vectorSocketChannel[i]->GetCbaseSocketPoint()->GetSocket();

				if ( sockValue <= 0 )
				{
					assert(0);
					continue;
				}

				if ( FD_ISSET( sockValue,&fd) )
				{
					// ����
					// ����Ϊ���ڶ�����
					m_vectorSocketChannel[i]->SetIOCPNoUse( TRUE );
					//����������̳߳أ����̳߳ض�д
					pstruThreadPoolTask	 pTask = (pstruThreadPoolTask) malloc(sizeof(struThreadTask));//

					if ( NULL == pTask )
					{
						assert(0);
						continue;
					}

					memset(pTask,0x0,sizeof(struThreadTask));
					pTask->pFunction = &CCommunicationManager::ThreadAcceptData;
					pTask->enumEvent = ACCEPTDATA;
					pTask->pObject = pObject;	//pObjectָ��CCommunicationManager
					pTask->pObject2 = m_vectorSocketChannel[i];
					m_clsThreadPool.AssignTask(pTask);
					if ( NET_PROTOCOL_UDP == m_vectorSocketChannel[i]->GetNetProtocolType()  )
					{
						// UDP��ʽ���������̣߳�����Ч�ʸ߶���
						m_clsThreadPool.WakeUpThread();
					}

					// ����Ƿ��Ѿ�����
					rt--;
					if ( 0 == rt )
					{
						// �Ѿ�����
						break;
					}

				}

				
			}

			m_GSMutexVerChnl.Unlock();
		}
		else if ( 0 == rt )
		{
			// ��ʱ ����ѭ��
			continue;
		}
		else
		{
			// �쳣 �����ҵ���� �쳣���账�� �ٴ�ѭ��ʱ�쳣��socket�Ĳ������FDSET
			// ����Ǳ����ر� rt == -1
			continue;
			
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

	//1����������
	//2����һ�ν��վ�û�յ����ݣ�˵���ǶԷ��ص�socket������ͨ���жϱ�־ ������ѭ��
	//3���յ����ݣ��ϱ�֪ͨ
	//4�����ݽ�����ɣ���socket������У��̷߳���

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
					DealDisnChannel( pclsSocketChannel );

				}				

			}

			// ���ٶ�����, ��������ɶ˿ڵı�־ ����Ҳ�õ�selectģ�����ˡ����˵�,�����������岻ƥ��
			pclsSocketChannel->SetIOCPNoUse( FALSE );
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

	return ERROR_BASE_SUCCESS;

}


// �����¼���Ӧ �̳���CCommunicationManager
void CThreadDealNetEvent::OnEventModel( CSocketChannel* pclsSocketChannel )
{
	m_GSCond.Signal();

	return;
}

#endif 