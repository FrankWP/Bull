#include "ThreadPoolModule.h"

using namespace NetServiceLib;

#ifdef _WIN32
unsigned   _stdcall ThreadProcFunc(HANDLE param)
#elif _LINUX
void *ThreadProcFunc(HANDLE param)
#endif 
{	
	CNetThread * pNetThread = (CNetThread *)param;
	if ( pNetThread->m_fnUser )
	{
		pNetThread->m_fnUser(pNetThread, pNetThread->m_pFnUserParam);
	}

#ifdef _WIN32	
	return 0;
#elif _LINUX
	return pNetThread;
#endif 
}
CNetThread::CNetThread():m_fnUser(NULL), m_hThread(0), m_pFnUserParam(NULL)
{

};

CNetThread::~CNetThread()
{
	if ( m_hThread )
	{
#ifdef _WIN32
		CloseHandle(m_hThread);
#elif _LINUX
		//�����߳�
		if (pthread_detach(m_hThread)!=0)
		{
			assert(0);
		}	
#endif
	}
}

BOOL CNetThread::Start(ThreadCallbackFunction fnOnEvent, void *pUserParam)
{
	if ( !fnOnEvent )
	{
		assert(0);
	}
	m_fnUser = fnOnEvent;
	m_pFnUserParam = pUserParam;
#ifdef WINCE
	m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadProcFunc,(LPVOID)this,0,NULL); //(NetThread)_beginthreadex(NULL,0,ThreadProcFunc,HANDLE(this),0,NULL);

#elif _WIN32
	m_hThread = (NetThread)_beginthreadex(NULL,0,ThreadProcFunc,HANDLE(this),0,NULL);
#elif _LINUX
	if(pthread_create(&m_hThread,0,ThreadProcFunc,HANDLE(this))!=0)
	{
		assert(0);

	}
#endif

	return TRUE;
}

CThreadPoolModule::CThreadPoolModule(void)
{
	m_uiCurThreadCount = 0;
	m_uiMaxThreadCount = THREADPOOL_MAX_THREAD_COUNT;
	m_DequeTask.clear();
	m_VectorThread.clear();
	m_VecBusyThread.clear();
	m_VecIdleThread.clear();
	m_bIsExitWorkerThreadProc = true;
	m_iIdleThreadMaxNum = 4;
	m_iIdleThreadMinNum = 1;
	m_iWaitDelThreadNum = 0;
	m_unExitTick = 0;
	m_bIsExitManangeThread = FALSE;
	m_clsLogPtr = NULL;
	m_ExitThreadCount=0;
}

CThreadPoolModule::~CThreadPoolModule(void)
{
}
INT CThreadPoolModule::Initialize( UINT uiThreads)
{
	//���������ֹ�ظ�����
	if (m_uiCurThreadCount > 0)
	{
		//�Ѿ���ʼ��
		return ERROR_NET_REPEAT_INIT;
	}
	if ( uiThreads < 0 )
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_uiMaxThreadCount = uiThreads > m_uiMaxThreadCount ? uiThreads : m_uiMaxThreadCount;
	if (m_uiMaxThreadCount - uiThreads <= 2)
	{
		// ����û���߳���ִ���������������� ��ͨ���ͷ�
		m_uiMaxThreadCount += 10;
	}


//#ifdef OPERATING_SYSTEM
//#else
	if(AddThread(uiThreads) != ERROR_BASE_SUCCESS)
	{
		//�����߳�ʧ��
		return ERROR_NET_PARAM_WRONG;
	}
//#endif

	// �����߳� 
	m_clsManagerThread.Start(CThreadPoolModule::ManagerThreadProc, (void *)this);
	//_beginthreadex(NULL,0,ManagerThreadProcEx,HANDLE(this),0,NULL);


	return ERROR_BASE_SUCCESS;

}

//�����߳�
INT CThreadPoolModule::AddThread(UINT16 usThreads)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutexDequeThread);

	if (m_uiCurThreadCount + usThreads > m_uiMaxThreadCount)
	{
		return ERROR_NET_OVERFLOW_MAX_THREADS;
	}
	if ( usThreads > 0 )
	{
		//�����µ��߳�
		return AllocNewThreads(usThreads);
	}

	return ERROR_NET_PARAM_WRONG;

}
//�����߳�
INT CThreadPoolModule::SubThread(UINT16 usThreads)
{
	//return ERROR_NET_PARAM_WRONG;
	CGSAutoMutex	GSAutoMutex(&m_GSMutexDequeThread);

	m_iWaitDelThreadNum += usThreads;

	return 0;	

}
//�Ӷ����Ƴ��߳�
INT	CThreadPoolModule::DeleteFromDeque( CNetThread* pclsThread )
{  
	if ( NULL == pclsThread )
	{
		return ERROR_NET_THREAD_NOT_EXIST;
	}

	vector<CNetThread*>::iterator pIter;
	pIter = find(m_VectorThread.begin(), m_VectorThread.end(), pclsThread);
	if (pIter != m_VectorThread.end())
	{
		m_VectorThread.erase(pIter);
		return ERROR_BASE_SUCCESS;
	}


	return ERROR_NET_THREAD_NOT_EXIST;
}
/*************************************************
Function:      Uninitialize 
Description:   �ͷ��̳߳���Դ
Input:  
Output:         
Return:         
Note:		//1��ֹͣ�����߳�,Ҫ�����̵߳�״̬
//2���ͷ��߳���Դ
//3���ͷ���������е�����		
*************************************************/
void CThreadPoolModule::Uninitialize()
{
	//�ͷ�������Դ
	m_GSMutexDequeTask.Lock();
	CTRLPRINTF(m_clsLogPtr,"�����δִ��������%d \n", m_DequeTask.size());
	for (deque<pstruThreadPoolTask>::size_type i=0; i< m_DequeTask.size(); i++)
	{
		//����ͷ�
		free(m_DequeTask[i]);
	}
	m_DequeTask.clear();

	m_GSMutexDequeTask.Unlock();

	StruSysTime		struTime;
	DoGetLocalTime(&struTime);
	CTRLPRINTF(m_clsLogPtr,"������̳߳ؿ�ʼ�˳�  %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
	
	// �����߳�Ҳʹ�ô˱���
	m_unExitTick = DoGetTickCount();

	while ( !m_bIsExitManangeThread )
	{
		MSLEEP(10);
		if (  DoGetTickCount() - m_unExitTick > 20*1000 )
		{
			CTRLPRINTF(m_clsLogPtr," NetServiceLib �����߳��˳��쳣,����m_VecBusyThread=%d, m_VecIdleThread=%d,\
								   m_VecWaitDelThread=%d, m_VectorThread=%d \n ",m_VecBusyThread.size(),m_VecIdleThread.size(),m_VecWaitDelThread.size(),m_VectorThread.size());
			assert(0);
			break;
		}
	}
	
	CTRLPRINTF(m_clsLogPtr,"������̳߳��˳���ʱ %u ����\n", DoGetTickCount() - m_unExitTick);


	m_GSMutexDequeThread.Lock();
	m_VectorThread.clear();
	m_GSMutexDequeThread.Unlock();

	return;
}
bool CThreadPoolModule::AssignTask(pstruThreadPoolTask pTask)
{
	if (pTask == NULL)
	{
		return false;
	}

	if ( !m_bIsExitWorkerThreadProc )
	{
		return false;
	}

	//��������������
	m_GSMutexDequeTask.Lock();
	m_DequeTask.push_back(pTask);
	m_GSMutexDequeTask.Unlock();

	return true;
}

// 
/********************************************************************************
  Function:		ManagerThreadProc
  Description:	�̳߳ع����߳�
  Input:  		
  Output:      	   
  Return:  		       
  Note:			��̬����		
  Author:        	CHC
  Date:				2010/09/01
********************************************************************************/
void CThreadPoolModule::ManagerThreadProc(CNetThread* pclsThread, void * pInfo)
{
	if ( !pInfo || !pclsThread ) return;	


	CThreadPoolModule *pThisThreadPool = (CThreadPoolModule*) pInfo;
	pThisThreadPool->ManagerThreadProc(pclsThread);

}

/********************************************************************************
  Function:		ManagerThreadProc
  Description:	�̳߳ع����߳�ִ�к���
  Input:  		
  Output:      	   
  Return:  		       
  Note:			�ڴ˺����ж��̵߳��߳̽��й��� ������ ���� �ͷ�	���̳߳�ǿ��ǿ��Ϳ��˺������߼���	
  Author:        	CHC
  Date:				2010/09/01
********************************************************************************/
void CThreadPoolModule::ManagerThreadProc(CNetThread* pclsThread)
{
	
	//UINT64 uiTick = DoGetTickCount();
	while ( 1 )
	{

		m_GSMutexDequeTask.Lock();
		m_GSMutexDequeThread.Lock();
		if ( m_DequeTask.size() > 0 && m_VecIdleThread.size())
		{
#if _WIN32
			int nCount = m_VecIdleThread.size();	//�෢�����ź���Ҳ�޷���
			while ( nCount-- )
			{
				m_GSCond.Signal();
			}
#else
			
			m_GSCond.Signal();

#endif
			
			
		}

		m_GSMutexDequeThread.Unlock();
		m_GSMutexDequeTask.Unlock();

		// ��������Ҫɾ���߳�
		FreeThread();

		if ( m_bIsExitWorkerThreadProc )
		{
			// �жϿ����߳���Ŀ
			//INT iIdleNum =  m_VecIdleThread.size();
			//if (iIdleNum > m_iIdleThreadMaxNum)
			//{
			//	//�����߳�
			//	INT iSubNum = iIdleNum - ((m_iIdleThreadMaxNum - m_iIdleThreadMinNum)/2 + m_iIdleThreadMinNum);

			//	SubThread(iSubNum);	

			//	UINT64	unTick = DoGetTickCount();

			//	while ( m_iWaitDelThreadNum )
			//	{
			//		m_GSCond.Signal();
			//		MSLEEP(10);

			//		if ( DoGetTickCount() - unTick > 10*1000 )
			//		{
			//			CTRLPRINTF(m_clsLogPtr," NetServiceLib �߼��쳣֮�����߳� \n ");
			//			assert(0);

			//			break;
			//		}
			//	}


			//}
			//else if (iIdleNum < m_iIdleThreadMinNum)
			//{
			//	//�����߳�
			//	AddThread(m_iIdleThreadMinNum - iIdleNum);


			//}

		}
		else
		{
			// �˳�	1���ȴ�æµ�߳�Ϊ0	2���ͷ�ȫ�������߳�
			if ( m_VecBusyThread.size() == 0 )
			{
				if ( m_VecIdleThread.size() > 0 )
				{
					SubThread( m_VecIdleThread.size() );

					UINT64	unTick = DoGetTickCount();

					while ( m_iWaitDelThreadNum )
					{
						m_GSCond.BroadcastSignal();

						MSLEEP(10);

						if ( DoGetTickCount() - unTick > 10*1000 )
						{
							CTRLPRINTF(m_clsLogPtr," NetServiceLib �߼��쳣,����m_VecBusyThread=%d, m_VecIdleThread=%d,\
								m_VecWaitDelThread=%d, m_VectorThread=%d \n ",m_VecBusyThread.size(),m_VecIdleThread.size(),m_VecWaitDelThread.size(),m_VectorThread.size());
							assert(0);

							break;
						}
					}
				}
						
			}
			

		}
		

		// �ж�������

		if ( m_bIsExitWorkerThreadProc )
		{

			if ( m_uiCurThreadCount < m_uiMaxThreadCount )
			{
				deque<pstruThreadPoolTask>::size_type iSize = m_DequeTask.size();
				if ( iSize > 0 && m_VecIdleThread.size() == 0)
				{
					if (iSize < m_uiMaxThreadCount - m_uiCurThreadCount )
					{
						//�����߳�
						AddThread(iSize);
						CTRLPRINTF(m_clsLogPtr," �����߳����� %d\n ",iSize);
					}
					else
					{
						//�����߳�
						AddThread(m_iIdleThreadMinNum - m_uiCurThreadCount);
						CTRLPRINTF(m_clsLogPtr," �����߳����� %d\n ",m_iIdleThreadMinNum - m_uiCurThreadCount);
					}
				}

			}

		}

		/*if (DoGetTickCount() - uiTick > 20000)
		{
			CTRLPRINTF(m_clsLogPtr,"��ǰ������������߳���Ŀ�� %d \n", GetAllThreadCount());
			CTRLPRINTF(m_clsLogPtr,"��ǰ�������æµ�߳���Ŀ�� %d \n", GetBusyThreadCount());
			CTRLPRINTF(m_clsLogPtr,"��ǰ������п����߳���Ŀ�� %d \n", GetIdleThreadCount());
			CTRLPRINTF(m_clsLogPtr,"��ǰ�������������Ŀ�� %d \n", GetTaskCount());
			CTRLPRINTF(m_clsLogPtr,"��ǰ������д�ɾ�����߳���Ŀ�� %d \n",GetWaitDelThreadNum());
			CTRLPRINTF(m_clsLogPtr,"��ǰ�������ɾ�������е��߳���Ŀ�� %d \n",GetVecWaitDelThread());
			uiTick = DoGetTickCount();

		}
		*/
		MSLEEP(10);
		

		if ( !m_bIsExitWorkerThreadProc )
		{
			// �˳�	�ȴ������߳��˵��� �����߳��˳�

			if ( m_VectorThread.size() == 0 )
			{
				break;
			}
			else if( m_unExitTick > 0 )
			{
				if ( DoGetTickCount() - m_unExitTick > 30*1000 )
				{
					CTRLPRINTF(m_clsLogPtr," �� %d ���߳�δ�������˳�  \n ",  m_VecBusyThread.size());
					assert(0);
					break;
				}
			}
			else
			{
				// ����Ҫ��ʲô
			}
		}

	}//end while

	m_bIsExitManangeThread = TRUE;

}


//��̬����
void CThreadPoolModule::WorkerThreadProc( CNetThread* pclsThread, void * pInfo)
{
	if ( NULL == pInfo || NULL == pclsThread ) 
	{
		return;	
	}


	CThreadPoolModule *pThisThreadPool = (CThreadPoolModule*) pInfo;
	pThisThreadPool->WorkerThreadProc(pclsThread);


}
void CThreadPoolModule::WorkerThreadProc(CNetThread* pclsThread)
{
	
	pstruThreadPoolTask	pQueueTask = NULL;

	while (1)
	{

		m_GSMutexDequeThread.Lock();
		if (m_iWaitDelThreadNum > 0)
		{		
			--m_iWaitDelThreadNum;
			m_GSMutexDequeThread.Unlock();
			break;

		}

		m_GSMutexDequeThread.Unlock();

		m_GSMutexDequeTask.Lock();

		if (m_DequeTask.empty() == false)
		{
			pQueueTask = m_DequeTask.front();
			m_DequeTask.pop_front();

			m_GSMutexDequeTask.Unlock();
		}
		else
		{
			//������������񷵻�
			m_GSMutexDequeTask.Unlock();
			
			if (m_bIsExitWorkerThreadProc)
			{	// ���˳������ȴ�����

				m_GSCond.Wait();
			}

			continue;
		}

		if ( NULL == pQueueTask)
		{
			//�����񣬵�û��ȡ�ɹ����������������ִ����
			assert(0);
			return;
		}


		//�Ƶ�æµ����
		MoveToBusyVec(pclsThread);

		//ִ������
		if ( NULL != pQueueTask->pFunction && NULL != pQueueTask->pObject )
		{		

			
			((CCommunicationManager*)(pQueueTask->pObject)->*(pQueueTask->pFunction))(pQueueTask->enumEvent, pQueueTask->pObject2);

			StruSysTime		struTime;
			DoGetLocalTime(&struTime);
#if OPERATING_SYSTEM

#elif _WIN32
			if ( ACCEPTDATA == pQueueTask->enumEvent )
			{
				++m_ExitThreadCount;
				CTRLPRINTF(m_clsLogPtr,"��ɶ˿��߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}
#endif
			if ( LISTEN == pQueueTask->enumEvent )
			{
				CTRLPRINTF(m_clsLogPtr,"�����߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}
			else if ( EPOLLEVENTWAIT == pQueueTask->enumEvent )
			{
				CTRLPRINTF(m_clsLogPtr,"epoll�¼��߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}
			else if ( CHANNELSTATUS == pQueueTask->enumEvent )
			{
				CTRLPRINTF(m_clsLogPtr,"ͨ��״̬�߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}
			else if ( ACTIVETEST == pQueueTask->enumEvent )
			{
				CTRLPRINTF(m_clsLogPtr,"ͨ�������߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}
			else if (ACCEPTNOTICE == pQueueTask->enumEvent)
			{
				CTRLPRINTF(m_clsLogPtr,"֪ͨ�ϲ������ӵ����߳��˳� %d:%d:%d \n", struTime.wHour, struTime.wMinute, struTime.wSecond);
			}

		}
		
		//������������ �ͷŸ�������ڴ档
		if ( NULL != pQueueTask )
		{
			free(pQueueTask);
			pQueueTask = NULL;
		}

		MoveToIdleVec(pclsThread);



	}

	m_GSMutexDequeThread.Lock();

	m_VecWaitDelThread.push_back(pclsThread);

	m_GSMutexDequeThread.Unlock();


}

INT CThreadPoolModule::AllocNewThreads(UINT uiThreadCount)
{
	UINT ixThreadData = m_uiCurThreadCount;
	while ( ixThreadData <  (m_uiCurThreadCount + uiThreadCount) )
	{
		CNetThread* pclsGSThread = new CNetThread();
		if ( NULL == pclsGSThread )
		{
			assert(0);
			return ERROR_NET_PARAM_WRONG;
		}
		pclsGSThread->Start(CThreadPoolModule::WorkerThreadProc, (void *)this);

		m_VectorThread.push_back(pclsGSThread);
		//ͬʱҲ������ж���
		m_VecIdleThread.push_back(pclsGSThread);

		//_beginthreadex(NULL,0,WorkerThreadProcEx,HANDLE(this),0,NULL);

		ixThreadData++; 
		// �ﵽ����߳���ʱ�˳�
		if ( m_uiMaxThreadCount == ixThreadData )
		{
			break;
		}

	}
	// ���浱ǰ���߳���
	m_uiCurThreadCount = ixThreadData;

	return ERROR_BASE_SUCCESS;
}


INT	CThreadPoolModule::MoveToBusyVec(CNetThread* pclsThread)
{
	m_GSMutexDequeThread.Lock();

	m_VecBusyThread.push_back(pclsThread);

	vector<CNetThread*>::iterator pIter;
	pIter = find(m_VecIdleThread.begin(), m_VecIdleThread.end(), pclsThread);
	if (pIter != m_VecIdleThread.end())
	{
		m_VecIdleThread.erase(pIter);

	}

	m_GSMutexDequeThread.Unlock();

	return ERROR_BASE_SUCCESS;
}

INT	CThreadPoolModule::MoveToIdleVec(CNetThread* pclsThread)
{
	m_GSMutexDequeThread.Lock();

	m_VecIdleThread.push_back(pclsThread);

	vector<CNetThread*>::iterator pIter;
	pIter = find(m_VecBusyThread.begin(), m_VecBusyThread.end(), pclsThread);
	if (pIter != m_VecBusyThread.end())
	{
		m_VecBusyThread.erase(pIter);

	}
	m_GSMutexDequeThread.Unlock();

	return ERROR_BASE_SUCCESS;
}



/********************************************************************************
  Function:		FreeThread
  Description:	�ͷ��߳�
  Input:  		
  Output:      	   
  Return:  		       
  Note:					
  Author:        	CHC
  Date:				2010/09/01
********************************************************************************/
INT	CThreadPoolModule::FreeThread()
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutexDequeThread);

	for (vector<CNetThread*>::size_type i=0; i<m_VecWaitDelThread.size(); ++i)
	{
		DeleteFromDeque( m_VecWaitDelThread[i] );

		vector<CNetThread*>::iterator pIter;
		pIter = find(m_VecIdleThread.begin(), m_VecIdleThread.end(), m_VecWaitDelThread[i]);
		if (pIter != m_VecIdleThread.end())
		{
			m_VecIdleThread.erase(pIter);

		}

		delete m_VecWaitDelThread[i]; 
	}

	m_VecWaitDelThread.clear();

	m_uiCurThreadCount = m_VectorThread.size();

	return 0;

}
INT16 CThreadPoolModule::GetIdleThreadCount()
{ 
	CGSAutoMutex	AutoMutex(&m_GSMutexDequeThread);
	return m_VecIdleThread.size();
}

