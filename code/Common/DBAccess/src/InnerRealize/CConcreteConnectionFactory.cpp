// ͷ�ļ�
#include "CConcreteConnectionFactory.h"
using	namespace	DBAccessModule;
CConcreteConnectionFactory::CConcreteConnectionFactory(void)
{
}

CConcreteConnectionFactory::~CConcreteConnectionFactory(void)
{
}

/**************************************************************************************************
  Function: CreateOracleConnection    
  DateTime: 2010/5/23 21:39	
  Description: ����һ��Oracle���ݿ����Ӷ���
  Input:   	
		  szServer ��   ���ݿ��������ַ
		  szDatabase��  ���ݿ�����
		  szUser  ��    ���ݿ��û���
		  szPass  ��    ���ݿ�����
  Output:  NULL
  Return:  CConnection*
  Note:	   �������Ӷ���
**************************************************************************************************/
CConnection*		CConcreteConnectionFactory::CreateOracleConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
{
	COracleConnection*	pOracleConnection = NULL;
	pOracleConnection = new COracleConnection();
	if (pOracleConnection != NULL)
	{
		// ��������ID
		pOracleConnection->SetConnectID(GenerateAutoConnectID());
		// ��ʼ����Ϣ
		pOracleConnection->Initial(szServer,szDatabase,szUser,szPass);
		// �����ݿ�����
		if (pOracleConnection->Open())
		{
			// �������Ӷ���
			return (CConnection*)pOracleConnection;
		}		
		else
		{
			// ������ݿ��ʧ�ܣ��ͷŶ���
			delete pOracleConnection;
			pOracleConnection = NULL;
		}
	}

	return	(CConnection*)NULL;

}


/**************************************************************************************************
  Function		: CreateSqlConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)    
  DateTime		: 2010/6/7 14:27	
  Description	: SqlServer ���Ӷ�����幤����������
  Input			: 
				  szServer ��   ���ݿ��������ַ
				  szDatabase��  ���ݿ�����
				  szUser  ��    ���ݿ��û���
				  szPass  ��    ���ݿ�����
  Output		: NULL
  Return		: CConnection*
  Note			: Sql Server���ݿ����Ӷ���
**************************************************************************************************/
CConnection*		CConcreteConnectionFactory::CreateSqlConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
{

	CSqlServerConnection*	pSqlServerConnection = NULL;
	pSqlServerConnection = new CSqlServerConnection();
	if (pSqlServerConnection != NULL)
	{
		// ��������ID
		pSqlServerConnection->SetConnectID(GenerateAutoConnectID());
		// ��ʼ����Ϣ
		pSqlServerConnection->Initial(szServer,szDatabase,szUser,szPass);
		// �����ݿ�����
		if (pSqlServerConnection->Open())
		{
			// �������Ӷ���
			return (CConnection*)pSqlServerConnection;
		}		
		else
		{
			delete	pSqlServerConnection;
			pSqlServerConnection = NULL;
		}
	}

	return	(CConnection*)NULL;
}


/**************************************************************************************************
Function		: CreateAccessConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)    
DateTime		: 2010/6/7 14:27	
Description		: Access ���Ӷ�����幤����������
Input			: 
					szServer ��   ���ݿ��������ַ
					szDatabase��  ���ݿ�����
					szUser  ��    ���ݿ��û���
					szPass  ��    ���ݿ�����
Output			: NULL
Return			: CConnection*
Note			: Access���ݿ����Ӷ���
**************************************************************************************************/
CConnection*		CConcreteConnectionFactory::CreateAccessConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
{
	return NULL;
}

/**************************************************************************************************
  Function		: CreateMySQLConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
  DateTime		: 2011/4/14 17:36	
  Author 		: yopo  
  Description	: MySQL ���Ӷ�����幤����������
  Input			:
					szServer ��   ���ݿ��������ַ
					szDatabase��  ���ݿ�����
					szUser  ��    ���ݿ��û���
					szPass  ��    ���ݿ�����
  Output		: NULL
  Return		: CConnection*
  Note			: MySQL���ݿ����Ӷ���
**************************************************************************************************/
CConnection*		CConcreteConnectionFactory::CreateMySQLConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
{
	CMySQLConnection*	pMySQLConnection = NULL;
	pMySQLConnection = new CMySQLConnection();
	if (pMySQLConnection != NULL)
	{
		// ��������ID
		pMySQLConnection->SetConnectID(GenerateAutoConnectID());
		// ��ʼ����Ϣ
		pMySQLConnection->Initial(szServer,szDatabase,szUser,szPass);
		// �����ݿ�����
		if (pMySQLConnection->Open())
		{
			// �������Ӷ���
			return (CConnection*)pMySQLConnection;
		}		
		else
		{
			// ������ݿ��ʧ�ܣ��ͷŶ���
			delete pMySQLConnection;
			pMySQLConnection = NULL;
		}
	}
	return	(CConnection*)NULL;
}
/**************************************************************************************************
  Function: CreateOciConnection    
  DateTime: 2010/5/23 21:39	
  Description: ����һ��Oci���ݿ����Ӷ���
  Input:   	
		  szServer ��   ���ݿ��������ַ
		  szDatabase��  ���ݿ�����
		  szUser  ��    ���ݿ��û���
		  szPass  ��    ���ݿ�����
  Output:  NULL
  Return:  CConnection*
  Note:	   �������Ӷ���
**************************************************************************************************/
CConnection*		CConcreteConnectionFactory::CreateOciConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass)
{
	COciConnection*	pOciConnection = NULL;
	pOciConnection = new COciConnection();
	if (pOciConnection != NULL)
	{
		// ��������ID
		pOciConnection->SetConnectID(GenerateAutoConnectID());
		// ��ʼ����Ϣ
		pOciConnection->Initial(szServer,szDatabase,szUser,szPass);
		// �����ݿ�����
		if (pOciConnection->Open())
		{
			// �������Ӷ���
			return (CConnection*)pOciConnection;
		}		
		else
		{
			// ������ݿ��ʧ�ܣ��ͷŶ���
			delete pOciConnection;
			pOciConnection = NULL;
		}
	}

	return	(CConnection*)NULL;

}