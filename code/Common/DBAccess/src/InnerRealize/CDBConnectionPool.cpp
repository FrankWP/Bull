#include "CDBConnectionPool.h"

using	namespace	DBAccessModule;



#define OCI_DLL32_DIR _T("ociliba.dll")
#define OCI_DLL64_DIR _T("ociliba.dll")
#define CLIENT_DIR "ocilib_client"


// �ײ�ģ���ļ���,����������
//#ifdef _DEBUG
//#pragma  comment(lib, "CommonLibsD.lib")
//#else
//#pragma	 comment(lib,"CommonLibs.lib")
//#endif


CDBConnectionPool*		CDBConnectionPool::m_pInstance = NULL;
CGSMutex				CDBConnectionPool::m_CsSingleton;
OciDllDeal*				DBAccessModule::m_pOciDeal = NULL;

CDBConnectionPool::CDBConnectionPool(void)
{
	m_pConnectionFactory = NULL;
}

// ��������
CDBConnectionPool::~CDBConnectionPool(void)
{
	// �ͷŶ���
	if (m_pConnectionFactory!=NULL)
	{
		delete	m_pConnectionFactory;
		m_pConnectionFactory = NULL;
	}

	Release();
	
	if(m_pOciDeal != NULL)
	{
		m_pOciDeal->UnLoad_Oci();
		delete m_pOciDeal;
		m_pOciDeal = NULL;
	}
}

//---------------------------------------------------------------------------------------------
// ������ʵ��
//---------------------------------------------------------------------------------------------
/**************************************************************************************************
  Function: Instance    
  DateTime: 2010/5/25 22:15	
  Description:    	������������
  Input:          	NULL
  Output:         	NULL
  Return:         	CDBConnectionPool*���͵�ָ�����
  Note:				
**************************************************************************************************/
CDBConnectionPool*	CDBConnectionPool::Instance()
{
	if (m_pInstance == NULL)
	{
		CGSAutoMutex	AutoLock(&m_CsSingleton);
		if (m_pInstance == NULL)
		{
			m_pInstance = new CDBConnectionPool();
		}
	}

	return	m_pInstance;

}

/**************************************************************************************************
  Function: Release    
  DateTime: 2010/5/25 22:14	
  Description:    	�ͷŵ�����Դ
  Input:          	NULL
  Output:         	NULL
  Return:         	
					TRUE:�����ɹ�
  Note:				
**************************************************************************************************/
BOOL	CDBConnectionPool::Release()
{
	m_CsConnectionPool.Lock();
	DeleteAllConnection();
	m_CsConnectionPool.Unlock();

	if(this == m_pInstance )
	{
		// �ر����е����Ӷ����ͷ����Ӷ���
		

		//delete m_pInstance;
		m_pInstance = NULL;
	}

	return TRUE;	
}

//---------------------------------------------------------------------------------------------
// OuterInterface [5/22/2010 liujs] 
//---------------------------------------------------------------------------------------------

/**************************************************************************************************
  Function:  GetConnection   
  DateTime: 2010/5/25 22:12	
  Description:      �����ӳ���ȡ��һ�����Ӷ��󣬹��ⲿʹ��
  Input:          	
					  szServer ��   ���ݿ��������ַ
					  szDatabase��  ���ݿ�����
					  szUser  ��    ���ݿ��û���
					  szPass  ��    ���ݿ�����
					  iDbType ��     ���ݿ�����[sql ,oracle ��]

  Output:         	NULL
  Return:         	���Ӷ���ָ��IConnection*
  Note:				
**************************************************************************************************/
IConnection*	CDBConnectionPool::GetConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType)
{
	CGSAutoMutex	AutoLock(&m_CsConnectionPool);

	IConnection *p = (IConnection*)GetOneConnectionFromPool();
	if( p )
	{
		return p;
	}

	// �ж����ӳ��Ƿ������Ӵ���
	if (InputDataVerify(szServer,szDatabase,szUser,szPass,iDbType) == FALSE)
	{
		return	NULL;
	}

	// û�д������ʹ������Ӷ��󣬻��߶���ʹ����û�е��������Ŀ���ڴ���5��
	if ( GetConnectionPoolNum()==0  || GetConnectionPoolNum() < coniConnectionPoolMaxMum )
	{
		// �����������Ӷ��󣬴������ӳ���,ÿ�δ��� coniConnectionBatchNum ��������ʧ�ܣ�����NULL
		BatchCreateConnHandle(szServer,szDatabase,szUser,szPass,iDbType,coniConnectionBatchNum);		
		
	}

	// �����ӳ��л�ȡ���Ӷ���
	return	(IConnection*)GetOneConnectionFromPool();
}


/**************************************************************************************************
  Function: InputDataVerify    
  DateTime: 2010/5/25 20:48	
  Description:    	�ж��û����������Ƿ�׼ȷ
  Input:          	
					  szServer ��   ���ݿ��������ַ
					  szDatabase��  ���ݿ�����
					  szUser  ��    ���ݿ��û���
					  szPass  ��    ���ݿ�����
					  iDbType ��     ���ݿ�����[sql ,oracle ��]
  Output:         	NULL
  Return:         	TRUE : ����Ϸ�   FALSE :�������
  Note:				
**************************************************************************************************/
BOOL			CDBConnectionPool::InputDataVerify(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType)
{
	// У����Ϣ
	if (strlen(szServer) > coniDataLen || strlen(szServer) <= coniZero)
	{
		return	FALSE;
	}

	if( strlen(szDatabase) > coniDataLen || strlen(szDatabase) <= coniZero)
	{
		return	FALSE;
	}

	if( strlen(szUser) > coniDataLen || strlen(szUser) <= coniZero)
	{
		return	FALSE;
	}

	if( strlen(szPass) > coniDataLen || strlen(szPass) <= coniZero)
	{
		return	FALSE;
	}

	if (iDbType == ORACLE  || iDbType == SQLSERVER || iDbType == MYSQL || iDbType == OCI)
	{
		return	TRUE;
	}

	return FALSE;

}


//---------------------------------------------------------------------------------------------
// ��������
//---------------------------------------------------------------------------------------------
/**************************************************************************************************
Function: BatchCreateConnHandle    
DateTime: 2010/5/25 20:32	
Description:    	   �����������Ӷ���,���þ���Ĺ������󴴽�
Input:          	
				       szServer ��   ���ݿ��������ַ
					   szDatabase��  ���ݿ�����
					   szUser  ��    ���ݿ��û���
					   szPass  ��    ���ݿ�����
					   iDbType ��     ���ݿ�����[sql ,oracle ��]
Output:         	   NULL
Return:         	  
				       TRUE:�ɹ�
	                   FALSE:ʧ��
Note:				// ��ע
**************************************************************************************************/
CConnection*    CDBConnectionPool::CreateConnectionHandle(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType)
{
	// ���ݿ����Ӷ���
	CConnection*	pConnection = NULL;
	switch (iDbType)
	{
	case ORACLE:
		// ORACLE ���ݿ�
		{	
			if (m_pConnectionFactory == NULL)
			{	
				// ��������
				m_pConnectionFactory	=	new CConcreteConnectionFactory();		
			}
			// ��������
			pConnection	=	m_pConnectionFactory->CreateOracleConnection(szServer,szDatabase,szUser,szPass);
		}		
		break;

	case MYSQL:
		// MySQL ���ݿ�
		{	
			if (m_pConnectionFactory == NULL)
			{	
				// ��������
				m_pConnectionFactory	=	new CConcreteConnectionFactory();		
			}
			// ��������
			pConnection	=	m_pConnectionFactory->CreateMySQLConnection(szServer,szDatabase,szUser,szPass);
		}		
		break;

	case SQLSERVER:
		// Sql Server ���ݿ�
		{
			if (m_pConnectionFactory == NULL)
			{	
				// ��������
				m_pConnectionFactory	=	new CConcreteConnectionFactory();		
			}
			// ��������
			pConnection	=	m_pConnectionFactory->CreateSqlConnection(szServer,szDatabase,szUser,szPass);

		}
		break;
	case ACCESS:
		// Access ���ݿ�
		{
			if (m_pConnectionFactory == NULL)
			{	
				// ��������
				m_pConnectionFactory	=	new CConcreteConnectionFactory();		
			}
			// ��������
			pConnection	=	m_pConnectionFactory->CreateAccessConnection(szServer,szDatabase,szUser,szPass);

		}
		break;
	case OCI:
		// OCI
		{	
			//OCI������ʼ��
			if(m_pOciDeal == NULL)
			{
				m_pOciDeal = new OciDllDeal;
				if(!m_pOciDeal->Load_Oci(OCI_DLL32_DIR,OCI_DLL64_DIR))
				{
					delete m_pOciDeal;
					m_pOciDeal = NULL;
					return NULL;
				}
			}
			if (!m_pOciDeal->pOCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)) 
			{
				delete m_pOciDeal;
				m_pOciDeal = NULL;
				return NULL;
			}
	
			if (m_pConnectionFactory == NULL)
			{	
				// ��������
				m_pConnectionFactory	=	new CConcreteConnectionFactory();		
			}
			// ��������
			pConnection	=	m_pConnectionFactory->CreateOciConnection(szServer,szDatabase,szUser,szPass);
		}		
		break;
	
		// ������ݿ⣬ֻ���ڴ˴������Ϳ���
	default:
		break;
	}

	// ���ش����ɹ��Ķ���
	return  pConnection;

}


/**************************************************************************************************
  Function: BatchCreateConnHandle    
  DateTime: 2010/5/25 20:32	
  Description:    	  ������������,��ӵ����ӳ���
  Input:          	
					  szServer ��   ���ݿ��������ַ
					  szDatabase��  ���ݿ�����
					  szUser  ��    ���ݿ��û���
					  szPass  ��    ���ݿ�����
					  iDbType ��     ���ݿ�����[sql ,oracle ��]
  Output:         	  NULL
  Return:         	  
					  TRUE:�ɹ�
					  FALSE:ʧ��
  Note:				// ��ע
**************************************************************************************************/
BOOL            CDBConnectionPool::BatchCreateConnHandle(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType,const INT iConnNum)
{
	BOOL  bRet = FALSE;
	// ����20�����Ӷ���
	for (int i=0;i< iConnNum ;i++)
	{
		CConnection*	pConnection = NULL;
		// ����������������
		pConnection = CreateConnectionHandle(szServer,szDatabase,szUser,szPass,iDbType);

		if(pConnection != NULL)
		{
			bRet = TRUE;
			// ��ӵ�����
			AddConnecttion(pConnection);
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}

	return bRet;

}





//---------------------------------------------------------------------------------------------
// ���ӳض��е���ز���
//---------------------------------------------------------------------------------------------

/**************************************************************************************************
Function: AddConnecttion    
DateTime: 2010/5/25 19:29	
Description:    	������Ӷ��󵽶�����
Input:          	pConnection:���Ӷ���
Output:         	NULL
Return:         	
					TRUE: �����ɹ�
					FALSE: ����ʧ��
Note:				// ��ע
**************************************************************************************************/
BOOL            CDBConnectionPool::AddConnecttion(CConnection*   pConnection)
{
	if (pConnection != NULL)
	{		
		m_ConnectionPool.push_back(pConnection);
		return	TRUE;
	}

	return  FALSE;

}

/**************************************************************************************************
  Function: DeleteConnection    
  DateTime: 2010/5/25 19:29	
  Description:    	ɾ�������е����Ӷ���
  Input:          	pConnection:���Ӷ���
  Output:         	NULL
  Return:         	
					TRUE: �����ɹ�
					FALSE: ����ʧ��
  Note:				// ��ע
**************************************************************************************************/
BOOL            CDBConnectionPool::DeleteConnection(CConnection* pConnection)
{
	if (pConnection!=NULL)
	{	
		for (Connection_Vector_Iterator	iter = m_ConnectionPool.begin();iter != m_ConnectionPool.end();iter++)
		{
			// ���Ӻ���Ⱦͱ�ʾ���
			if (pConnection->GetConnectID() == (*iter)->GetConnectID())
			{
				CConnection*	pCsConnnection = NULL;
				pCsConnnection = (*iter);
				if (pCsConnnection != NULL)
				{
					pCsConnnection->Close();
					delete pCsConnnection;
					pCsConnnection = NULL;
				}

				// ��������ɾ��
				m_ConnectionPool.erase(iter);
				break;
			}
		}
	}
	return  FALSE;

}

/**************************************************************************************************
  Function: DeleteAllConnection    
  DateTime: 2010/5/25 19:28	
  Description:    	ɾ�����ӳ������е�����
  Input:          	NULL
  Output:         	NULL
  Return:         	
					TRUE:�ɹ�
					FALSE:ʧ��
  Note:				
**************************************************************************************************/
BOOL			CDBConnectionPool::DeleteAllConnection()
{
	for (Connection_Vector_Iterator	iter = m_ConnectionPool.begin();iter != m_ConnectionPool.end();iter++)
	{
		CConnection*	pConnectionObj = NULL;
		pConnectionObj = (*iter);
		if (pConnectionObj != NULL)
		{
			// �ر�
			pConnectionObj->Close();

			delete pConnectionObj;
			pConnectionObj = NULL;
		}
	}

	// ��ն���
	m_ConnectionPool.erase(m_ConnectionPool.begin(),m_ConnectionPool.end());
	return	TRUE;

}


/**************************************************************************************************
  Function: GetOneConnectionFromPool    
  DateTime: 2010/5/25 22:03	
  Description:    	��ȡһ����Ч������
  Input:          	NULL
  Output:         	NULL
  Return:         	����һ����Ч�����Ӷ���
  Note:				
**************************************************************************************************/
CConnection*	CDBConnectionPool::GetOneConnectionFromPool()
{
	
	for (Connection_Vector_Iterator	iter = m_ConnectionPool.begin();iter != m_ConnectionPool.end();iter++)
	{
		CConnection*	pConnectionObj = NULL;
		pConnectionObj = (*iter);
		if (pConnectionObj != NULL)
		{
			// ���е�
			if (pConnectionObj->GetConnectionUseFlag() == CONNECTION_IDLESSE)
			{
				// ��Ϊʹ��
				pConnectionObj->SetConnectionUseFlag(CONNECTION_OCCUPY);
				pConnectionObj->m_pCnnPool = (CDBConnectionPool*)this;
				return	pConnectionObj;
			}
		}
	}
	return NULL ;

}
/**************************************************************************************************
  Function: GetConnectionPoolNum
  DateTime: 2010/5/25 20:43	
  Description:    	��ȡ���ӳ�Ԫ�ظ���    
  Input:          	NULL
  Output:         	NULL
  Return:           ���ӳ�Ԫ�ظ���
  Note:				
**************************************************************************************************/
INT				CDBConnectionPool::GetConnectionPoolNum()
{	
	return (INT)m_ConnectionPool.size();

}

/**************************************************************************************************
  Function: ReleaseConnection     
  DateTime: 2010/5/25 19:26	
  Description:    	�ͷ����ӣ������Ӷ���Ż����ӳ�
  Input:          	pConnection: ���Ӷ���
  Output:         	NULL
  Return:         	
					TRUE: �ͷųɹ�
					FALSE:�ͷ�ʧ��
  Note:				
**************************************************************************************************/
BOOL			CDBConnectionPool::ReleaseConnection(CConnection*	pConnection)
{
	CGSAutoMutex	AutoLock(&m_CsConnectionPool);
	for (Connection_Vector_Iterator	iter = m_ConnectionPool.begin();iter != m_ConnectionPool.end();iter++)
	{
		CConnection*	pConnectionObj = NULL;
		pConnectionObj = (*iter);
		if (pConnectionObj->GetConnectID() == pConnection->GetConnectID())
		{
			pConnectionObj->SetConnectionUseFlag(CONNECTION_IDLESSE);
			break;
		}
	}
	return TRUE;

}

// /**************************************************************************************************
// Function: BOOLAllConnectionUsed    
// DateTime: 2010/5/25 19:35	
// Description:    	�ж����е������Ƿ���ʹ��,��ʹ��
// Input:          	NULL
// Output:         	NULL
// Return:         	����TRUE,���򷵻�FALSE
// Note:				// ��ע
// **************************************************************************************************/
// BOOL            CDBConnectionPool::BOOLAllConnectionUsed()
// {		
// 	BOOL	bUseFlag  = TRUE;
// 	for (Connection_Vector_Iterator	iter = m_ConnectionPool.begin();iter != m_ConnectionPool.end();iter++)
// 	{
// 		CConnection*	pConnectionObj = NULL;
// 		pConnectionObj = (*iter);
// 		if (pConnectionObj != NULL)
// 		{
// 			if (pConnectionObj->GetConnectionUseFlag() == CONNECTION_IDLESSE)
// 			{
// 				bUseFlag = FALSE;
// 				break;
// 			}
// 		}
// 	}
// 	return bUseFlag ;
// 
// }

