// DBAccessModule.cpp : ���� DLL Ӧ�ó���ĵ���������
//
#include "IDBAccessModule.h"
#include "../InnerRealize/CDBConnectionPool.h"

using	namespace	DBAccessModule;

/**************************************************************************************************
Function: StartDBAccessModule    
DateTime: 2010/5/23 10:51	
Description: �������ݿ�ģ��
Input:       NULL
Output:      NULL
Return:      TRUE:�ɹ�  FALSE:ʧ��
Note:		   �ṩ�������ݿ�ģ����񣬸ú������ṩ�ⲿ������ʹ��
��			 ֻ�ṩ�ײ���������������ӿ�
 **************************************************************************************************/
BOOL	DBAccessModule::StartDBAccessModule()
{
//#ifdef _WIN32
#if  0
	// ADO���ݿ⻷����ʼ��
	::CoInitialize(NULL);
#else
// linux
#endif
	// �������ӳ�
	return	CDBConnectionPool::Instance() == NULL ? FALSE : TRUE;

}

/**************************************************************************************************
Function:  GetConnectionPoolInstance
DateTime: 2010/5/23 10:15	
Description:  ��ȡ���ӳض���
Input:        NULL
Output:       NULL
Return:       IConnectionPool*ָ�����
Note:		    ���ص����ӳض������㵥��ʵ��
ǰ��������StartDBAccessModule�ɹ����ײ��ǵ�������StartDBAccessModule �Ĺ���һ����
**************************************************************************************************/
IConnectionPool*	DBAccessModule::GetConnectionPoolInstance()
{
	// �������ӳض���
	return	CDBConnectionPool::Instance() == NULL ? NULL : (IConnectionPool*)CDBConnectionPool::Instance();

}

IConnectionPool* DBAccessModule::CreateConnectionPool()
{
	return	CDBConnectionPool::Create();
}


/**************************************************************************************************
Function: StopDBAccessModule    
DateTime: 2010/5/23 10:51	
Description: ֹͣ���ݿ�ģ����񣬲��ڲ��ͷ����е���Դ
Input:       NULL
Output:      NULL
Return:      TRUE : �ɹ���FALSE:ʧ��
Note:		���ú������ṩ�ⲿ������ʹ�ã�ֻ�ṩ�ײ�����˳����˳��ӿ�
**************************************************************************************************/
BOOL	DBAccessModule::StopDBAccessModule()
{
	// �ͷ�ADO��
	//#ifdef _WIN32
#if  0
	::CoUninitialize();
#else
	// linux
#endif
	return	CDBConnectionPool::Instance() != NULL ? CDBConnectionPool::Instance()->Release() : FALSE;

}

/**************************************************************************************************
Function		: ErrorLogInfo(SQLHENV		hEnv,SQLHDBC	hDbc,SQLHSTMT	hStmt)
DateTime		: 2010/11/30 13:49	
Description		: ���������Ϣ�ĺ���
Input			: hEnv:���ݿ⻷�������hDbc�����ݿ����Ӿ����hStmt�����ݿ��ѯ�����
Output			: NULL
Return			: TRUE
Note			:
**************************************************************************************************/
BOOL	DBAccessModule::ErrorLogInfo(SQLHENV hEnv,SQLHDBC hDbc,SQLHSTMT hStmt, INT *piErrno)
{
	SQLCHAR		szTEMP[1024]={0};
	SQLSMALLINT	ItEMP = 0;
    SQLINTEGER iErrno = 0;
	char szState[32];
	memset(szState,0,sizeof(szState));
	SQLError(hEnv,hDbc,hStmt,(SQLCHAR *)szState,&iErrno,szTEMP,1024,(SQLSMALLINT*)&ItEMP);
	std::cout <<"---> DBAccess " << " Errno:" << iErrno << ", Error�� "<< szTEMP << "Status:"<< szState << endl;
    if( piErrno )
    {
       *piErrno = iErrno;


	   if(0== strncmp(szState, "01002",5)  ||
		   0== strncmp(szState, "08001",5)  ||
		   0== strncmp(szState, "08002",5)  ||
		   0== strncmp(szState, "08003",5)  ||
		   0== strncmp(szState, "08004",5)  || 
		   0== strncmp(szState, "08007",5)  ||
		   0== strncmp(szState, "S1T00",5)  )
	   {
		   printf("ODBC errno: %d,  error: %s\n", iErrno, szState);
		   *piErrno = 3114; //ERRNO_NETWORK_DISCNN;
	   }
	   
    }
	return TRUE;
}

