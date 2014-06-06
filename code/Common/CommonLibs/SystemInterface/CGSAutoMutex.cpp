#include "ISystemLayInterface.h"

/******************************************************************************
����˵�����Զ���
******************************************************************************/

/********************************************************************************************
  Function		: CGSAutoMutex����   
  DateTime		: 2010/6/10 9:32	
  Description	: ����
  Input			: CGSMutex *locker����ͨ����ָ��
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
CGSAutoMutex::CGSAutoMutex(CGSMutex *locker): m_locker( locker )
{

	if( m_locker ) 
	{
		m_locker->Lock();
	}

}

//
/********************************************************************************************
  Function		: ~CGSAutoMutex   
  DateTime		: 2010/6/10 9:36	
  Description	: ����
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:				// ��ע
********************************************************************************************/
CGSAutoMutex::~CGSAutoMutex()
{

	if( m_locker )
	{
		m_locker->Unlock();
	}

}

