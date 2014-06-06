
#include "ThreadIOCP.h"

#if OPERATING_SYSTEM
#elif _WIN32

using namespace NetServiceLib;

CThreadDealNetEvent::CThreadDealNetEvent(void)
{
	
}

CThreadDealNetEvent::~CThreadDealNetEvent(void) 
{
	
}

/*************************************************
  Function:       ThreadAcceptData
  Description:    �������ݣ����ص�֪ͨ�ϲ㡣
  Input:		  enumEvent ����Ĳ���������ACCEPTDATA��pObjectû�õ�������Ϊnull.
  Output:         
  Return:         
  Note:			1���ж������Ƿ�Ͽ���2���ж��Ƿ���ն���RECV_POSTED 3���ص�֪ͨ�û��� 4��������һ�ν���	
*************************************************/
INT CThreadDealNetEvent::ThreadAcceptData(enumThreadEventType enumEvent, void* pObject)
{
	//pObject������null
	if (enumEvent != ACCEPTDATA)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	// �ص�IO
	LPOVERLAPPED lpOverlapped = NULL;	
	// �����ֽ���
	DWORD BytesTransferred = 0;
	// �׽��ֽṹ��
	LPPER_HANDLE_DATA PerHandleData = NULL;
	// IO�ṹ��
	LPPER_IO_OPERATION_DATA PerIoData = NULL;  

	m_GSMutexExit.Lock();
	if ( !m_bIsExitAcceptData )
	{
		m_GSMutexExit.Unlock();
		return ERROR_NET_PARAM_WRONG;
	}
	m_GSMutexExit.Unlock();

	DWORD	dtTick = DoGetTickCount();
	char szRemoteIP[16]={0};
	UINT16 uiRemotePort = 0;
	char szLocalPort[16]={0};
	UINT16 uiLocalPort = 0;

	CTRLPRINTF( m_clsLogPtr," ��ɶ˿ڿ�ʼ�������� \n");

	while( m_bIsExitAcceptData )	//ʲôʱ���˳�ѭ��
	{

		//������
		PerHandleData = NULL;
		PerIoData = NULL;
		lpOverlapped = NULL;
		BytesTransferred = 0;

		BOOL bRet = GetQueuedCompletionStatusEx(&BytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&lpOverlapped, (LPPER_IO_OPERATION_DATA*)&PerIoData);
		

		if ( bRet > 0 && BytesTransferred > 0 )
		{
			if(PerIoData->OptionType == RECV_POSTED && NULL != PerHandleData )
			{

				//�յ�����
				CSocketChannel*  pclsSocketChannel = (CSocketChannel*)PerHandleData->pUserData;

				if (LISTEN_CHANNEL == pclsSocketChannel->GetChannelType() && NET_PROTOCOL_UDP == pclsSocketChannel->GetNetProtocolType())
				{
					//Ҳ����������㿴��
					//�����UDP��ʽSERVERͨ�� �ж��ǲ����µĿͻ��˷���������

					UINT16  unPort = 0;				
					unPort = (UINT16)ntohs( PerIoData->struSockAddrFrom.sin_port);
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

								OnEventCallBack( pSocketChnl, NET_READ, PerIoData->Buffer, BytesTransferred );
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
						//3���ص�֪ͨ�û���
						if (pclsNewSocketChannel->GetChannelStatus() != CHANNEL_CLOSE)
						{
							pclsNewSocketChannel->AddRefCount();
							ISocketChannel *pSocketChnl = (ISocketChannel*)pclsNewSocketChannel;
							OnEventCallBack( pSocketChnl,NET_READ, PerIoData->Buffer, BytesTransferred );
							pclsNewSocketChannel->SubRefCount();
						}
						else
						{						
							CTRLPRINTF( m_clsLogPtr," ͨ��%p���յ����ݵ�ʱ���ر� \n", pclsSocketChannel);
							pclsSocketChannel->SetIOCPNoUse( FALSE );
						}

					}

					//4��������һ�ν���	 UDP��ʽ�ջ���������,�����µ�ͨ��ȥ��
					pclsSocketChannel->RecvData();


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
							int i=0;
							//if (DoGetTickCount() - dtTick > 10*1000)
							{
								memset(szRemoteIP,0,16);
								uiRemotePort = 0;								
								pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);								
								memset(szLocalPort,0,16);
								uiLocalPort = 0;
								pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
								CTRLPRINTF_D( m_clsLogPtr,"�����:ͨ��%p�ص����ݰ�,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",pclsSocketChannel, szRemoteIP, uiRemotePort,uiLocalPort );
								dtTick = DoGetTickCount();
								i=1;

							}
							OnEventCallBack( pSocketChnl, NET_READ, PerIoData->Buffer, BytesTransferred );
							//if (i==1)
							{
								CTRLPRINTF_D( m_clsLogPtr,"�����:ͨ��%p�ص����ݰ����,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",pclsSocketChannel, szRemoteIP, uiRemotePort,uiLocalPort );
							}
							pclsSocketChannel->m_uiChnIng = 0;
							//4��������һ�ν���	
							int iRet = pclsSocketChannel->RecvData();
							if (  1 != iRet  )
							{
								memset(szRemoteIP,0,16);
								uiRemotePort = 0;
								pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
								memset(szLocalPort,0,16);
								uiLocalPort = 0;
								pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
								CTRLPRINTF( m_clsLogPtr,"��������ʧ��,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",szRemoteIP, uiRemotePort,uiLocalPort );

							}
							else 
							{
								memset(szRemoteIP,0,16);
								uiRemotePort = 0;
								pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
								memset(szLocalPort,0,16);
								uiLocalPort = 0;
								pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
								//CTRLPRINTF( m_clsLogPtr,"�ɹ��������¼�Ͷ�ݵ���ɶ˿� szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",szRemoteIP, uiRemotePort,uiLocalPort );
							}

							pclsSocketChannel->SubRefCount();
						}
						else
						{
							memset(szRemoteIP,0,16);
							uiRemotePort = 0;
							pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
							memset(szLocalPort,0,16);
							uiLocalPort = 0;
							pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
							CTRLPRINTF( m_clsLogPtr," ͨ�����յ����ݵ�ʱ���ر�,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",szRemoteIP, uiRemotePort,uiLocalPort );
							pclsSocketChannel->SetIOCPNoUse( FALSE );

						}
						pclsSocketChannel->m_csCallBackGSMutex.Unlock();
						
					}
					else
					{						
						
						memset(szRemoteIP,0,16);
						uiRemotePort = 0;
						pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
						memset(szLocalPort,0,16);
						uiLocalPort = 0;
						pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
						CTRLPRINTF( m_clsLogPtr," ͨ�����յ����ݵ�ʱ���ر� ,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",szRemoteIP, uiRemotePort,uiLocalPort );
						pclsSocketChannel->SetIOCPNoUse( FALSE );
					}
					
				}



			}//end if(PerIoData->OptionType == RECV_POSTED)
			else if (PerIoData->OptionType == SEND_POSTED)
			{
				//��������  ����û��Ͷ������������Բ�Ӧ���ܵ�������
				assert(0);
				continue;
			}
		}
		else if (FALSE == bRet && 0 == BytesTransferred)
		{
			//����TCP�ͻ��˻���UDP�������ˣ������Ͽ�������.
			//�Է�ֱ�ӵ���رհ�ťҲ�ǽ���
	
			if ( NULL == PerHandleData )
			{
				//��ɶ˿��յ�PostQueuedCompletionStatus��Ϣ�˳�
				INT32 iRet = GetLastError();
				
				CTRLPRINTF( m_clsLogPtr,"��ɶ˿��յ��˳���Ϣ FALSE == bRet\n");


				return ERROR_BASE_SUCCESS;
			}

			INT32 iError = GetLastError();
			
			CSocketChannel*  pclsSocketChannel = (CSocketChannel*)PerHandleData->pUserData;

			if ( !IfExistSocketChannel( pclsSocketChannel) )
			{
				
				CTRLPRINTF( m_clsLogPtr," ͨ��%p��ͨ�������в����� FALSE == bRet \n", pclsSocketChannel);
				continue;
			}


			if (ERROR_NETNAME_DELETED == iError)
			{
				//�����ж�
				memset(szRemoteIP,0,16);
				uiRemotePort = 0;
				pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
				memset(szLocalPort,0,16);
				uiLocalPort = 0;
				pclsSocketChannel->GetLocalIPPort(szLocalPort,uiLocalPort);
				CTRLPRINTF( m_clsLogPtr," ͨ��%p�жϣ�ָ�������������ٿ��� ,error = %d  ,szRemoteIP:%s, uiRemotePort:%u, uiLocalPort:%u \n",pclsSocketChannel, ERROR_NETNAME_DELETED, szRemoteIP, uiRemotePort, uiLocalPort );
				DealDisnChannel(pclsSocketChannel);

				
			}
			
			// ��ɶ˿ڲ�ʹ�ø�ͨ��
			pclsSocketChannel->SetIOCPNoUse( FALSE );

			CTRLPRINTF( m_clsLogPtr," ��ɶ˿ڲ�ʹ�ø�ͨ��%p FALSE == bRet \n", pclsSocketChannel);

			continue;
		}//end if(FALSE == bRet)

		//UDP��ʽ ����Զ������Ͽ�����߲������κη�Ӧ��ֻ�������ڷ��������Ƿ�ɹ����ж�

	    else if ( TRUE == bRet && 0 == BytesTransferred)
		{
			//TCP ֻҪ�ǶԶ������Ͽ������ˡ����۶Է��Ƿ������˻��ǿͻ���

			if ( NULL == PerHandleData )
			{
				//��ɶ˿��յ�PostQueuedCompletionStatus��Ϣ�˳�
				CTRLPRINTF( m_clsLogPtr,"��ɶ˿��յ��˳���Ϣ\n");
				return ERROR_BASE_SUCCESS;
			}

			CSocketChannel*  pclsSocketChannel = (CSocketChannel*)PerHandleData->pUserData;

			if ( !IfExistSocketChannel( pclsSocketChannel) )
			{
				CTRLPRINTF( m_clsLogPtr," ͨ��%p��ͨ�������в�����\n", pclsSocketChannel);
				continue;
			}

			DealDisnChannel(pclsSocketChannel);

			// ��ɶ˿ڲ�ʹ�ø�ͨ��
			pclsSocketChannel->SetIOCPNoUse( FALSE );

			CTRLPRINTF( m_clsLogPtr," ��ɶ˿ڲ�ʹ�ø�ͨ��%p\n", pclsSocketChannel);
			
			continue;
		}//end if ( TRUE == bRet && 0 == BytesTransferred)

		else if  ( FALSE == bRet && BytesTransferred > 0 )
		{
			//ͨ���ر�˲���յ�������,�����п��ܵ�,

			INT32 iError = GetLastError();
			CTRLPRINTF( m_clsLogPtr," BytesTransferred=%u, iError = %d \n", BytesTransferred, iError);

			if ( NULL == PerHandleData )
			{
				//��ɶ˿��յ�PostQueuedCompletionStatus��Ϣ�˳�
				CTRLPRINTF( m_clsLogPtr,"��ɶ˿��յ��˳���Ϣ && BytesTransferred > 0\n");
				return ERROR_BASE_SUCCESS;
			}

			CSocketChannel*  pclsSocketChannel = (CSocketChannel*)PerHandleData->pUserData;

			if ( !IfExistSocketChannel( pclsSocketChannel) )
			{
				CTRLPRINTF( m_clsLogPtr," ͨ��%p��ͨ�������в����� && BytesTransferred > 0\n", pclsSocketChannel);
				continue;
			}

			DealDisnChannel(pclsSocketChannel);

			// ��ɶ˿ڲ�ʹ�ø�ͨ��
			pclsSocketChannel->SetIOCPNoUse( FALSE );

			CTRLPRINTF( m_clsLogPtr," ��ɶ˿ڲ�ʹ�ø�ͨ��%p && BytesTransferred > 0\n", pclsSocketChannel);

			continue;
		}
		else
		{
			INT32 iError = GetLastError();
			CTRLPRINTF( m_clsLogPtr," BytesTransferred=%u, bRet=%d, iError = %d \n", BytesTransferred, bRet, iError);

			if ( NULL == PerHandleData )
			{
				//��ɶ˿��յ�PostQueuedCompletionStatus��Ϣ�˳�
				CTRLPRINTF( m_clsLogPtr,"��ɶ˿��յ��˳���Ϣ \n");
				return ERROR_BASE_SUCCESS;
			}

			CSocketChannel*  pclsSocketChannel = (CSocketChannel*)PerHandleData->pUserData;

			if ( !IfExistSocketChannel( pclsSocketChannel) )
			{
				CTRLPRINTF( m_clsLogPtr," ͨ��%p��ͨ�������в����� else \n", pclsSocketChannel);
				continue;
			}

			DealDisnChannel(pclsSocketChannel);

			// ��ɶ˿ڲ�ʹ�ø�ͨ��
			pclsSocketChannel->SetIOCPNoUse( FALSE );

			CTRLPRINTF( m_clsLogPtr," ��ɶ˿ڲ�ʹ�ø�ͨ��%p \n", pclsSocketChannel);
		}

		

		

		
	}

	CTRLPRINTF( m_clsLogPtr, "��ɶ˿��յ��˳���Ϣ\n");

	return ERROR_BASE_SUCCESS;
}

void CThreadDealNetEvent::OnEventModel( CSocketChannel* pclsSocketChannel )
{
	if ( NULL == pclsSocketChannel)
	{
		assert(0);
		return;
	}
	HANDLE hReturn = AddToIoCompletionPort((HANDLE)pclsSocketChannel->GetCbaseSocketPoint()->GetSocket(), (DWORD)pclsSocketChannel->GetIOCPHandleData());

	if (!hReturn)
	{
		char szRemoteIP[16]={0};
		UINT16 uiRemotePort = 0;

		pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
		CTRLPRINTF( m_clsLogPtr,"�����:ͨ��%p������ɶ˿�ʧ��,szRemoteIP:%s, uiRemotePort:%u \n",pclsSocketChannel, szRemoteIP, uiRemotePort );
		closesocket(pclsSocketChannel->GetCbaseSocketPoint()->GetSocket());
		DealDisnChannel(pclsSocketChannel);
	}

	//�������
	int iRet = pclsSocketChannel->RecvData();
	if (  1 != iRet  )
	{
		char szRemoteIP[16]={0};
		UINT16 uiRemotePort = 0;

		pclsSocketChannel->GetReMoteIPPort(szRemoteIP,uiRemotePort);
		CTRLPRINTF( m_clsLogPtr,"�����:ͨ��%p��������ʧ��,szRemoteIP:%s, uiRemotePort:%u \n",pclsSocketChannel, szRemoteIP, uiRemotePort );
		DealDisnChannel(pclsSocketChannel);

	}
}

// �����˳�
INT	CThreadDealNetEvent::ExitAcceptData(INT32 iThreadCount)
{
	if (iThreadCount < 0 || iThreadCount > 1000)
	{
		return 0;
	}
#if OPERATING_SYSTEM
#elif _WIN32
	UINT64 uiTick = DoGetTickCount();
	//UINT16 iTatolIocpThreadCount = GetNumberOfProcessors() * 2 + 2;
	while( iThreadCount != m_clsThreadPool.m_ExitThreadCount)
	{
		PostQueuedCompletionStatusEx();//����ɶ˿��߳��˳�
		if (DoGetTickCount() - uiTick > 20000)
		{
			CTRLPRINTF( m_clsLogPtr,"��ɶ˿��߳��޷�ȫ���˳������˳�%u���߳� \n",m_clsThreadPool.m_ExitThreadCount );
			break;
		}
		MSLEEP(20);
	}
	
#else
#endif

	return 0;
}

#endif //_win32

