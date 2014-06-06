/******************************************************************************
����˵�����ź���
******************************************************************************/

#include "ISystemLayInterface.h"

/********************************************************************************************
  Function		: CGSSem   
  DateTime		: 2010/6/10 9:48	
  Description	: �����ź���
  Input			: const char *czKey ���ź��������ƣ�BOOL bProcess���Ƿ��ǽ��̼���ź���
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
#ifdef WINCE
HANDLE OpenSemaphore(DWORD dwState,bool bFlag,WCHAR* wcKey)
{
	return  CreateSemaphore(NULL,GS_INIT_SEM_COUNT,GS_MAX_SEM_COUNT,wcKey);
}
#endif

CGSSem::CGSSem( const char *czKey,BOOL bProcess)
{
	m_bIsValid=FALSE;
	m_GSSem=NULL;


#ifdef _WIN32

	WCHAR * wcKey = (WCHAR *)(czKey);

	if(NULL == czKey)
	{
		return;
	}

	//�ж��ź����Ƿ��Ѵ��ڣ������ڣ�����ź���
	if( (m_GSSem=OpenSemaphore(SEMAPHORE_ALL_ACCESS,FALSE,wcKey))==NULL)
	{
		//���û���������̴�������źŵƣ��򴴽��ź�������ִ�гɹ��������ź�������ľ�������ʾ����
		m_GSSem = CreateSemaphore(NULL,GS_INIT_SEM_COUNT,GS_MAX_SEM_COUNT,wcKey);
	} 

	if (m_GSSem)
	{
		m_bIsValid=TRUE;
	}

#elif _LINUX

	char *wcKey=(char *)(czKey);
	if (NULL==czKey)
	{
		return;
	}

	//�ж��ź����Ƿ��Ѵ��ڣ������ڣ�����ź���
	if ((m_GSSem=sem_open(wcKey, 0))==SEM_FAILED)
	{
		//�����źŵƣ��ɹ�ʱ����ָ���ź�����ָ�룬����ʱΪSEM_FAILED
		if ((m_GSSem=sem_open(wcKey,O_RDWR | O_CREAT | O_EXCL,0644, 1))==SEM_FAILED)
		{
			m_bIsValid=FALSE;
			return;
		}
	}
	m_bIsValid=TRUE;

#endif

}

//��������
/********************************************************************************************
  Function		: ~CGSSem   
  DateTime		: 2010/6/10 10:05	
  Description	: �ͷ��ź�����ָ��
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
CGSSem::~CGSSem()
{

	if (m_GSSem)
	{
#ifdef _WIN32
		CloseHandle(m_GSSem);
		m_GSSem=NULL;
#elif _LINUX
		sem_close(m_GSSem);
		m_GSSem=NULL;
#endif

	}
	m_bIsValid=FALSE;

}


/********************************************************************************************
  Function		: IsValid   
  DateTime		: 2010/6/10 10:06	
  Description	: �����ź�������Ч��ʶ
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSSem::IsValid()
{
	return m_bIsValid;
}

//�����ź���
/********************************************************************************************
  Function		: Signal    
  DateTime		: 2010/6/10 10:07	
  Description	: ����һ���ź�������
  Input			:  NULL
  Output		:         	// �����������˵����
  Return		: TRUE��ʾ�ɹ���FALSE��ʾʧ��
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSSem::Signal()
{
	BOOL bRet=TRUE;
#ifdef _WIN32

	//����һ���ź�������,TRUE��ʾ�ɹ���FALSE��ʾʧ�ܡ�
	bRet=ReleaseSemaphore(m_GSSem, 1, NULL);

#elif _LINUX

	//����ָ�����ź�����ֵ��1��Ȼ�������ڵȴ����źŵ�ֵ��Ϊ�����������̡߳����ɹ��򷵻�0�����򷵻�-1��
	if(sem_post(m_GSSem)!=0)
	{
		bRet=FALSE;
	}

#endif
	return bRet;
}


/********************************************************************************************
  Function		: Wait    
  DateTime		: 2010/6/10 10:28	
  Description	: �ȴ���ȡ�ź���
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE��ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSSem::Wait()
{

	BOOL bRet=TRUE;

#ifdef _WIN32

	bRet=WaitForSingleObject(m_GSSem, INFINITE) == WAIT_OBJECT_0 ? TRUE :FALSE;

#elif _LINUX

	if(sem_wait(m_GSSem)!=0)
	{
		bRet=FALSE;
	}

#endif

	return bRet;
}

/********************************************************************************************
Function		: Wait    
DateTime		: 2010/6/10 10:28	
Description		: �ȴ�һ��ʱ���ȡ�ź���
Input			: UINT mSeconds���ȴ���ʱ�䣬����
Output			: NULL
Return			: �ɹ�����TRUE��ʧ�ܷ���FALSE
Note			:				// ��ע
********************************************************************************************/
BOOL CGSSem::Wait(UINT mSeconds)
{
	BOOL bRet=TRUE;

#ifdef _WIN32

	DWORD dwWaitTime=(DWORD)mSeconds;
	bRet= WaitForSingleObject(m_GSSem, dwWaitTime) == WAIT_TIMEOUT  ? FALSE :TRUE;

#elif _LINUX

	struct timeval struTimeVal;
	struct timespec struTimeSpec;
	gettimeofday(&struTimeVal, NULL);
	struTimeSpec.tv_sec  = mSeconds/1000;
	struTimeSpec.tv_nsec =1000L *(struTimeVal.tv_usec+(mSeconds-struTimeSpec.tv_sec*1000)*1000L);
	struTimeSpec.tv_sec += struTimeVal.tv_sec;

	if(sem_timedwait(m_GSSem,  &struTimeSpec)!=0)
	{
		bRet=FALSE;
	}

#endif

	return bRet;
}
