#include "ISystemLayInterface.h"
/******************************************************************************
����˵�����̲߳���
******************************************************************************/

/********************************************************************************************
  Function		: CGSThread    
  DateTime		: 2010/6/9 17:28	
  Description	: CGSThread���б����ĳ�ʼ��
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			: NULL
********************************************************************************************/
CGSThread::CGSThread()
{
	//��ʼ��������ʶ
	m_bExit = FALSE;
	m_bExiting = FALSE;
	m_bPause = FALSE;
	m_bRunningState = FALSE;
	m_bJoin=FALSE;
	m_bDetached=FALSE;
	m_bMutexLock =	FALSE;
	m_bthreadfinish = FALSE;

	m_GSCond = NULL;
	m_GSCondPause = NULL;
	m_GSMutexPause=NULL;
	m_GSMutexUnit = NULL;
	m_GSMutexthreadfinish = NULL;
	

	if(!m_GSCond)
	{
		m_GSCond = new CGSCond;
	}
	if (!m_GSCondPause)
	{
		m_GSCondPause = new	CGSCond;
	}
	if(!m_GSMutexPause)
	{
		m_GSMutexPause = new CGSMutex;
	}
	if (!m_GSMutexUnit)
	{
		m_GSMutexUnit = new CGSMutex;
	}
	if (!m_GSMutexthreadfinish)
	{
		m_GSMutexthreadfinish = new CGSMutex;
	}

	m_fnUser = NULL;
	m_pFnUserParam = NULL;

#ifdef	_WIN32	
	m_GSThread = NULL;
#endif

}

//��������
/********************************************************************************************
  Function		: ~CGSThread   
  DateTime		: 2010/6/9 17:29	
  Description	: �ͷ�CGSThread���е���Դ
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			: NULL
********************************************************************************************/
CGSThread::~CGSThread()
{
	Stop();
	if (m_GSThread)
	{
		m_GSMutexUnit->Lock();
		UnInitData();
		m_GSMutexUnit->Unlock();
	}	
	if(m_GSCond)
	{
		delete m_GSCond;
		m_GSCond = NULL;
	}
	if (m_GSCondPause)
	{
		delete	m_GSCondPause;
		m_GSCondPause = NULL;
	}
	if(m_GSMutexPause)
	{
		delete m_GSMutexPause;
		m_GSMutexPause = NULL;
	}
	if (m_GSMutexUnit)
	{
		delete	m_GSMutexUnit;
		m_GSMutexUnit = NULL;
	}
	if (m_GSMutexthreadfinish)
	{
		delete m_GSMutexthreadfinish;
		m_GSMutexthreadfinish = NULL;
	}
	
}

/********************************************************************************************
  Function		: UnInitData    
  DateTime		: 2010/6/9 17:30	
  Description	: widows:�ͷ��߳̾��
				  linux: �������
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			: NULL
********************************************************************************************/
void CGSThread::UnInitData()
{
	
#ifdef _WIN32
	if(m_GSThread)
	{
		CloseHandle(m_GSThread);
		m_GSThread = NULL;
	}
#elif _LINUX
	Detach();
#endif
	m_bRunningState = FALSE;
}

//CGSThread�̹߳���
/********************************************************************************************
  Function		: CGSThreadProcFunc    
  DateTime		: 2010/6/9 17:33	
  Description	: ִ���߳�
  Input			: HANDLE param���̺߳����Ĳ���
  Output		: NULL
  Return		: windows:����0��linux:�����̺߳����Ĳ���ָ��
  Note			: NULL
********************************************************************************************/
#ifdef _WIN32
//DWORD WINAPI CGSThreadProcFunc(HANDLE param)
unsigned   _stdcall CGSThreadProcFunc(HANDLE param)
#elif _LINUX
void *CGSThreadProcFunc(HANDLE param)
#endif 
{	
	CGSThread * pGSThread = (CGSThread *)param;

#ifdef _WIN32
	DWORD dwRet = 0;
	if(! pGSThread)
	{
		return dwRet;
	}
#endif
	//�����߳�����״̬

	if(pGSThread->m_fnUser)
	{
		//ִ���û��̺߳���
		pGSThread->m_fnUser(pGSThread,pGSThread->m_pFnUserParam);
	}
	//�����źţ�����join�ȴ�
	//pGSThread->m_GSCond->Signal();

	pGSThread->m_GSMutexthreadfinish->Lock();
	pGSThread->m_bthreadfinish = TRUE;
	pGSThread->m_bRunningState = FALSE;	
	pGSThread->m_GSMutexthreadfinish->Unlock();

	pGSThread->m_bExit = TRUE;
	//�ͷ���Դ
	pGSThread->UnInitData();
	if (pGSThread->m_bMutexLock)
	{
		pGSThread->m_GSMutexUnit->Unlock();
		pGSThread->m_bMutexLock = FALSE;
	}
	//�����źţ�����join�ȴ�
	pGSThread->m_GSCond->Signal();

#ifdef _WIN32	
	return dwRet;
#elif _LINUX
	return pGSThread;
#endif 
}


//CGSThread�߳̿�ʼ
/********************************************************************************************
  Function		: Start    
  DateTime		: 2010/6/9 17:36	
  Description	: �����߳�
  Input			: GSThreadCallbackFunction fnOnEvent���û��̺߳�����void *pUserParam���û��̲߳���
  Output		: NULL
  Return		: �ɹ�����TRUE��ʧ�ܷ���FALSE
  Note			: NULL
********************************************************************************************/
BOOL CGSThread::Start(GSThreadCallbackFunction fnOnEvent/* =NULL */, void *pUserParam/* =NULL */)
{
	m_fnUser = fnOnEvent;
	m_pFnUserParam = pUserParam;
	m_bRunningState=FALSE;
	if (m_bMutexLock)
	{
		return	FALSE;
	}
	m_GSMutexUnit->Lock();
	m_bMutexLock = TRUE;

#ifdef WINCE
	m_GSThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)CGSThreadProcFunc,HANDLE(this),0,NULL);
	if (m_GSThread)
	{
		m_GSMutexthreadfinish->Lock();
		if (m_bthreadfinish)
		{
			m_bRunningState=FALSE;
			//�ͷ���Դ
			UnInitData();
		}
		else
		{
			m_bRunningState=TRUE;
		}
		m_GSMutexthreadfinish->Unlock();
	}
#elif _WIN32
	//�����߳�
	//m_GSThread = CreateThread(NULL,0,CGSThreadProcFunc,HANDLE(this),0,NULL);

	//�����߳�
	m_GSThread = (HANDLE)_beginthreadex(NULL,0,CGSThreadProcFunc,HANDLE(this),0,NULL);
	
	if (m_GSThread)
	{
		m_GSMutexthreadfinish->Lock();
		if (m_bthreadfinish)
		{
			m_bRunningState=FALSE;
			//�ͷ���Դ
			UnInitData();
		}
		else
		{
			m_bRunningState=TRUE;
		}
		m_GSMutexthreadfinish->Unlock();
	}
#elif _LINUX
	//�����߳�
	if(pthread_create(&m_GSThread,0,CGSThreadProcFunc,HANDLE(this))==0)
	{
		m_GSMutexthreadfinish->Lock();
		if (m_bthreadfinish)
		{
			m_bRunningState=FALSE;
			//�ͷ���Դ
			UnInitData();
		}
		else
		{
			m_bRunningState=TRUE;
		}
		m_GSMutexthreadfinish->Unlock();
	}
#endif 
	else
	{
		m_GSMutexUnit->Unlock();
		m_bMutexLock = FALSE;
	}

	return m_bRunningState;
}


//ֹͣ�߳�
/********************************************************************************************
  Function		: Stop    
  DateTime		: 2010/6/9 17:38	
  Description	: ����ֹͣ�̱߳�ʶ
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			: NULL
********************************************************************************************/
BOOL CGSThread::Stop()
{
	//�ж��߳��Ƿ�������
	if(m_bRunningState)
	{
		//����ֹͣ�̱߳�ʶ
		m_bExiting = TRUE;
	}
	return m_bExiting;

}

/********************************************************************************************
  Function		: IsRunning    
  DateTime		: 2010/6/9 17:40	
  Description	: �����߳�����״̬
  Input			: NULL
  Output		: NULL
  Return		: �߳��������з���TRUE,���򷵻�FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::IsRunning()
{
	return m_bRunningState;
}

//�̹߳���
/********************************************************************************************
  Function		: Suspend   
  DateTime		: 2010/6/9 17:42	
  Description	: �����߳�����״̬
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::Suspend()
{
	//�ж��߳�����״̬
	if(m_bRunningState)
	{
		//�����߳�����״̬
		m_GSMutexPause->Lock();
		m_bPause = TRUE;
		m_GSMutexPause->Unlock();
	}

	return m_bPause;

}

//�߳�˯��
/********************************************************************************************
  Function		: Resume    
  DateTime		: 2010/6/9 17:43	
  Description	: �����źŻ����̣߳������߳�����״̬
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::Resume()
{
	if(m_bRunningState)
	{
		//�ж��߳�����״̬
		if(m_bPause)
		{
			//�����߳�
			m_GSMutexPause->Lock();
			m_GSCondPause->Signal();
			m_bPause = FALSE;
			m_GSMutexPause->Unlock();
		}
	}
	//�����߳�����״̬
	return m_bPause;
}

//�ȴ��߳̽���
/********************************************************************************************
  Function		: Join   
  DateTime		: 2010/6/9 17:44	
  Description	: �ȴ��߳������˳�
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::Join()
{
	//�ж��߳��Ƿ������У��Ƿ���join��
	if(m_bRunningState&&!m_bJoin)
	{
		//�ж��̵߳�����״̬
		if(m_bPause)
		{
			//�����߳�
			m_GSMutexPause->Lock();
			m_GSCondPause->Signal();
			m_bPause = FALSE;
			m_GSMutexPause->Unlock();;
		}
		//�����߳��˳���ʶ
		m_bExiting = TRUE;

#ifdef _WIN32
		//�ȴ��߳��˳�
		if(0 == m_GSCond->Wait())
		{
			//�����߳��˳���־λ��join��־λ
			m_bExit=TRUE;
			m_bJoin=TRUE;
		}
#elif _LINUX
		//�ȴ��߳��˳�
		if (pthread_join(m_GSThread,NULL)==0)
		{
			m_bJoin=TRUE;
		}		
#endif
	}
	//�����߳�Join��־λ
	return m_bJoin;

}


/********************************************************************************************
  Function		: Join  
  DateTime		: 2010/6/9 17:47	
  Description	: �ȴ��߳�һ��ʱ�䣬�ж��Ƿ��˳� 
  Input			: INT mseconds:�ȴ�ʱ�䣬����
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::Join(INT mseconds)
{
	//�ж��߳��Ƿ������У��Ƿ���join��
	if(m_bRunningState&&!m_bJoin)
	{
		//�ж��̵߳�����״̬
		if(m_bPause)
		{
			//�����߳�
			m_GSMutexPause->Lock();
			m_GSCondPause->Signal();
			m_bPause = FALSE;
			m_GSMutexPause->Unlock();;
		}
		//�����߳��˳���ʶ
		m_bExiting = TRUE;

#ifdef _WIN32
		//�ȴ��߳��˳�
		if(0 == m_GSCond->WaitTimeout(mseconds))
		{
			//�����߳��˳���־λ��join��־λ
			m_bExit=TRUE;
			m_bJoin=TRUE;
		}

#elif _LINUX
		//�ȴ��߳��˳�
		if (pthread_join(m_GSThread,NULL)==0)
		{
			m_bExit=TRUE;
			m_bJoin=TRUE;
		}		
#endif
	}
	//�����߳�Join��־λ
	return m_bJoin;

}

/********************************************************************************************
  Function		: Cancel   
  DateTime		: 2010/6/9 17:51	
  Description	: ǿ���˳��߳�
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
void CGSThread::Cancel()
{
	//�ж��߳�����״̬
	if(m_bRunningState)
	{
		//�����߳��˳�״̬
		m_bExiting = TRUE;
		
		//�ж��߳�����״̬
		if(m_bPause)
		{
			//�����߳�
			m_GSMutexPause->Lock();
			m_GSCondPause->Signal();
			m_bPause = FALSE;
			m_GSMutexPause->Unlock();;
		}
		//�ж��߳��˳�״̬
		if(!m_bExit)
		{
			//ǿ����ֹ�߳�
			Kill();
		}
	}

}

/********************************************************************************************
  Function		: TestExit   
  DateTime		: 2010/6/9 17:57	
  Description	: ����̵߳��˳�״̬��ִ���̵߳�join���Ѻ�����
  Input			: NULL
  Output		: NULL
  Return		: ����TRUE���߳����˳�������FALSE,�߳�δ�˳�
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSThread::TestExit()
{

	//�ж��߳��˳���ʶ
	if(m_bExiting==TRUE)
	{
		m_bExit = TRUE;
	}
	//�ж��̵߳�����״̬
	else if(m_bPause==TRUE)
	{
		//�߳�����
		m_GSMutexPause->Lock();
		if (m_bPause)
		{
			m_GSMutexPause->Unlock();
			m_GSCondPause->Wait();			
		}
		else
		{
			m_GSMutexPause->Unlock();
		}
		
	}
	
	//�ͷ���Դ��Ϊ�߳�ִ����Ϻ��ͷţ����̴߳������Ľ�β�ͷţ�hf��2010.11.01�޸�
	//�ж��̵߳��˳�״̬
	//if (m_bExit==TRUE)
	//{
	//	//�ͷ��߳̾��
	//	UnInitData();
	//	return TRUE;
	//}

	return m_bExit;
}


/********************************************************************************************
  Function		: Kill    
  DateTime		: 2010/6/9 18:06	
  Description	: ǿ����ֹ�߳�
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
void CGSThread::Kill()
{
	//�ж��߳�����״̬���˳�״̬
	if(m_bRunningState || (! m_bExit))
	{
#ifdef _WIN32

		//��ֹ�߳�
		DWORD dwExitCode;
		GetExitCodeThread(m_GSThread,&dwExitCode);
		TerminateThread(m_GSThread,dwExitCode);
#elif _LINUX
		//ǿ���˳��߳�
		pthread_cancel(m_GSThread);
#endif
		//�ͷ��߳̾��
		UnInitData();
		if (m_bMutexLock)
		{
			m_GSMutexUnit->Unlock();
			m_bMutexLock = FALSE;
		}
	}
}

/********************************************************************************************
  Function		:  Detach   
  DateTime		: 2010/6/9 18:06	
  Description	:  �����߳�
  Input			:  NULL
  Output		:  NULL
  Return		:  NULL
  Note			:				// ��ע
********************************************************************************************/
void CGSThread::Detach()
{
#ifdef _LINUX
	//�ж��̵߳ķ����ʶ��Join��ʶ
	if (!m_bDetached&&!m_bJoin)
	{
		//�����߳�
		if (pthread_detach(m_GSThread)==0)
		{
			m_bDetached=TRUE;
		}		
	}
#endif

}
/********************************************************************************************
Function		:  GetThreadHandle   
DateTime		: 2010/9/6 09:06	
Description		:  ��ȡ�߳̾��
Input			:  NULL
Output			:  GSThread:�߳̾����ʧ�ܷ���NULL
Return			:  NULL
Note			:				// ��ע
********************************************************************************************/
GSThread CGSThread::GetThreadHandle()
{

	if (m_GSThread)
	{
		return	m_GSThread;
	}
	return	NULL;
}
