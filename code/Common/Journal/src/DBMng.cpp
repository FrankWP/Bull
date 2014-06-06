/*******************************************************
Copyright (C), 2010-2011, GOSUN 
File name   : DATABASEMANAGER.CPP      
Author      : jiangshx     
Version     : Vx.xx        
DateTime    : 2010/11/12 9:39
Description :  ���ݿⷶΧ�ӿڲ�������
*******************************************************/
#include "DBMng.h"
#include "Service.h"

using namespace JOU;
using namespace std;



/**************************************************************************
Function    : CDBManager::CDBManager    
DateTime    : 2010/11/12 10:26	
Description : ���캯��
Input       : 
Output      : 
Return      : 
Note        :	
**************************************************************************/
CDBManager::CDBManager(void)
:CJouModule("DBManager")
{

    m_pConnPool = NULL;
	m_pOutConnPool = NULL;

}

/**************************************************************************
Function    : CDBManager::~CDBManager    
DateTime    : 2010/11/12 10:26	
Description : ��������
Input       : 
Output      : 
Return      : 
Note        :	
**************************************************************************/
CDBManager::~CDBManager(void)
{
    m_strServer = "";
    m_strDatabase = "";
    m_strUser = "";
    m_strPWD = "";
    m_eDbaseType = ORACLE;
}



EnumJouErrno CDBManager::Init( CService *pServer )
{
EnumJouErrno eErrno;
	eErrno = CJouModule::Init(pServer);
	GS_ASSERT_RET_VAL(!eErrno, eErrno );


	
	m_strServer = pServer->m_csCfg.m_strDBHostname;
	m_strDatabase = pServer->m_csCfg.m_strDBName;
	m_strUser =  pServer->m_csCfg.m_strDBUser;
	m_strPWD =  pServer->m_csCfg.m_strDBPWD;
	m_eDbaseType =  (EnumDatabaseType)pServer->m_csCfg.m_eDbaseType;
	return eJOU_R_SUCCESS;

}




EnumJouErrno CDBManager::Start(void* pData)
{
	EnumJouErrno eRet = CJouModule::Start(pData);
	GS_ASSERT_RET_VAL(!eRet, eRet );

	if (pData != NULL  )
	{
		StruConnectPoolArgs *pArgs = (StruConnectPoolArgs*)pData;
		if( pArgs->pConnectPool )
		{
			m_pConnPool = (IConnectionPool*) pArgs->pConnectPool;
			m_pOutConnPool = m_pConnPool;
			m_strServer = pArgs->szServer;
			m_strDatabase = pArgs->szDatabase;
			m_strUser =  pArgs->szUser;
			m_strPWD =  pArgs->szPWD;
			m_eDbaseType =  (EnumDatabaseType)pArgs->eDbaseType;
		}
	}
	else 
	{		
		//�������ݿ�ģ��
		if (!StartDBAccessModule())
		{
			return eJOU_E_DB_MODULE;
		}
		//��ȡ���ӳ�,ע:�˽ӿ�

		m_pConnPool = GetConnectionPoolInstance();
		if(m_pConnPool == NULL)
		{
			StopDBAccessModule();
			return eJOU_E_DB_CONNPOOL;
		}

	}
	
    IConnection *pCnn  = GetConnection();
    if( pCnn )
    {		
        pCnn->ReleaseConnection();
        return eJOU_R_SUCCESS;
    }
	MY_LOG_ERROR( g_pLog, "Connect DB:'%s@%s' USER:'%s' PWD:'%s' Type:%d FAIL.\n",
			 m_strDatabase.c_str() ,m_strServer.c_str(),
			 m_strUser.c_str(), m_strPWD.c_str(), 
			 (INT)m_eDbaseType );
    return eJOU_E_DB_GETCONN;
}

/********************************************************************
Function	:	CDBManager::StopRoutine
DateTime 	:	2010/18/8  19:03     
Description :   ���ݿ����ֹͣ  
Input	 	:	
Output 		:	
Return		: 	[EnumRetCode] �μ�����ֵ���Ͷ���
Note		:		
*********************************************************************/
void CDBManager::Stop(void)
{
    //ж�����ݿ�ģ��
	if( m_eDbaseType )
	{
		if( m_pOutConnPool != m_pConnPool )
		{
			
			if( m_pConnPool )
			{
				delete m_pConnPool;
				m_pConnPool = NULL;
			}
			StopDBAccessModule();
		}
	}
	CJouModule::Stop();
}


/********************************************************************
Function	: CDBManager::ConnectDB
DateTime 	: 2010/18/8  19:10     
Description : �������ݿ�  
Input       : szServer :   ���ݿ��������ַ
szDatabase:  ���ݿ�����
szUser  :    ���ݿ��û���
szPass  :    ���ݿ�����
iDbType :    ���ݿ�����[sql ,oracle ��]
ORACLE		 0		// oracle 
SQLSERVER	 1		// SqlServer
MYSQL		 2		// MySql 
ACCESS		 3		// Access
EXCEL		 4		// Excel���ݿ�
Output 		:	
Return		: [EnumRetCode] �μ�����ֵ���Ͷ���
Note		:		
*********************************************************************/
IConnection *CDBManager::GetConnection(void)
{

    return m_pConnPool->GetConnection(m_strServer.c_str(),
        m_strDatabase.c_str(),
        m_strUser.c_str(),
        m_strPWD.c_str(),
        m_eDbaseType);
}


