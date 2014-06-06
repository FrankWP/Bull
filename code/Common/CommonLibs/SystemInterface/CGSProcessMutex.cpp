
#include "ISystemLayInterface.h"

/******************************************************************************
����˵�������̼����
******************************************************************************/

/********************************************************************************************
Function		: CGSProcessMutex   
DateTime		: 2010/6/9 19:01	
Description		: ����������
Input			: NULL
Output			: NULL
Return			: NULL
Note			:				// ��ע
********************************************************************************************/
CGSProcessMutex::CGSProcessMutex(const char *czKey)
{
#ifdef _WIN32
	
	m_GSProcessMutex=NULL;
	if (czKey==NULL)
	{
		return;
	}
	LPCWSTR lpMutexName=(WCHAR *)(czKey);
	m_GSProcessMutex=CreateMutex(NULL, FALSE, lpMutexName);
	
#elif _LINUX

	m_GSProcessMutex=0;
	if (czKey==NULL)
	{
		return;
	}
	m_GSProcessMutex=open(czKey,O_RDWR|O_CREAT,S_IWRITE);
	if(m_GSProcessMutex<0)
	{
		return;
	}

#endif

}

/********************************************************************************************
Function		: ~CGSProcessMutex   
DateTime		: 2010/6/9 19:04	
Description		: ɾ����
Input			: NULL
Output			: NULL
Return			: NULL
Note			:				// ��ע
********************************************************************************************/
CGSProcessMutex::~CGSProcessMutex()
{

#ifdef _WIN32
	if (m_GSProcessMutex)
	{
		CloseHandle(m_GSProcessMutex);
		m_GSProcessMutex=NULL;
	}	
#elif _LINUX
	close(m_GSProcessMutex);
#endif

}

/********************************************************************************************
Function		: LockProcess   
DateTime		: 2010/6/9 19:08	
Description		: ����
Input			: NULL
Output			: NULL
Return			: �ɹ�����TRUE,ʧ�ܷ���FALSE
Note			:				// ��ע
********************************************************************************************/
BOOL CGSProcessMutex::LockProcess()
{

#ifdef _WIN32
	if (m_GSProcessMutex)
	{
		return	WaitForSingleObject(m_GSProcessMutex, INFINITE)==WAIT_OBJECT_0 ? TRUE :FALSE;
	}
	else
	{
		return FALSE;
	}
#elif _LINUX

	if(lockf(m_GSProcessMutex,F_LOCK,0)<0) //����ʹ��F_LOCK��������Ѿ���������������ǰһ�������ͷ���
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#endif


}

/********************************************************************************************
Function		: TryLockProcess   
DateTime		: 2010/6/9 19:10	
Description		: �������ļ���
Input			: NULL
Output			: NULL
Return			: �ɹ�����TRUE,ʧ�ܷ���FALSE
Note			:				// ��ע
********************************************************************************************/
BOOL CGSProcessMutex::TryLockProcess()
{

#ifdef _WIN32
	if (m_GSProcessMutex)
	{
		return	WaitForSingleObject(m_GSProcessMutex, 0)==WAIT_OBJECT_0 ? TRUE :FALSE;
	}
	else
	{
		return FALSE;
	}
#elif _LINUX

	if(lockf(m_GSProcessMutex,F_TLOCK,0)<0) //����ʹ��F_TLOCK�����ļ��Ѿ�������ʱ�����ý��̲�����������ֱ�Ӵ�ӡ������Ϣ������
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#endif

}

/********************************************************************************************
Function		: UnlockProcess    
DateTime		: 2010/6/9 19:11	
Description		: ����
Input			: NULL
Output			: NULL
Return			: NULL
Note			:				// ��ע
********************************************************************************************/
void CGSProcessMutex::UnlockProcess()
{

#ifdef _WIN32
	if (m_GSProcessMutex)
	{
		ReleaseMutex(m_GSProcessMutex);
	}	
#elif _LINUX
	lockf(m_GSProcessMutex,F_ULOCK,0);
#endif

}
