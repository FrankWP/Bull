
#include "ISystemLayInterface.h"

/******************************************************************************
����˵������ͨ��
******************************************************************************/

/********************************************************************************************
  Function		: CGSMutex   
  DateTime		: 2010/6/9 19:01	
  Description	: ��ʼ����
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
CGSMutex::CGSMutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_GSMutex);
#elif _LINUX
	pthread_mutex_init( &m_GSMutex,NULL);
#endif

}


/********************************************************************************************
  Function		: ~CGSMutex   
  DateTime		: 2010/6/9 19:04	
  Description	: ɾ����
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
CGSMutex::~CGSMutex()
{
	//Unlock();

#ifdef _WIN32
	DeleteCriticalSection(&m_GSMutex);
#elif _LINUX
	pthread_mutex_destroy(&m_GSMutex);
#endif

}


/********************************************************************************************
  Function		: Lock   
  DateTime		: 2010/6/9 19:08	
  Description	: ����
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSMutex::Lock()
{
	BOOL bRet=TRUE;

#ifdef _WIN32
	EnterCriticalSection(&m_GSMutex);
#elif _LINUX
	if(pthread_mutex_lock(&m_GSMutex)!=0)
	{
		bRet=FALSE;
	};
#endif
	return bRet;
}

//
/********************************************************************************************
  Function		: TryLock   
  DateTime		: 2010/6/9 19:10	
  Description	: �������ļ���
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			:				// ��ע
********************************************************************************************/
BOOL CGSMutex::TryLock()
{
	BOOL bRet=TRUE;

#ifdef _WIN32
	bRet=TryEnterCriticalSection(&m_GSMutex);
#elif _LINUX
	if (pthread_mutex_trylock(&m_GSMutex)!=0)
	{
		bRet=FALSE;
	}

#endif
	return bRet;
}


/********************************************************************************************
  Function		: Unlock    
  DateTime		: 2010/6/9 19:11	
  Description	: ����
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
void CGSMutex::Unlock()
{
#ifdef _WIN32
		LeaveCriticalSection(&m_GSMutex);
#elif _LINUX
		pthread_mutex_unlock(&m_GSMutex);	
#endif

}


