/**************************************************************************************************
* Copyrights 2013  ������
*                  ����Ӧ����
* All rights reserved.
*
* Filename��
*       COciConnection.cpp
* Indentifier��
*
* Description��
*       ���ݷ��ʲ��ⲿ�ӿ�
* Author:
*       LiuHongPing
* Finished��
*
* History:
*       2013��05�£�����
*
**************************************************************************************************/

#include "COciConnection.h"
#include "COciRecordSet.h"
#include "CDBConnectionPool.h"

using namespace DBAccessModule;



/**************************************************************************************************
*@input             Nothing
*@output            Nothing
*@return
*@description       ���캯��
*@frequency of call ����OCI������ʱ
**************************************************************************************************/
COciConnection::COciConnection(void)
: m_pCon(NULL)
, m_bclose(TRUE)
{
}

/**************************************************************************************************
*@input             Nothing
*@output            Nothing
*@return
*@description       ��������
*@frequency of call Nothing
**************************************************************************************************/
COciConnection::~COciConnection (void)
{
	this->fini();


}


/**************************************************************************************************
  Function:    COciConnection    
  DateTime:    2013/5	
  Description: ��ʼ������
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
BOOL COciConnection::init(void)
{
	/*
	if (!DBAccessModule::m_pOciDeal->pOCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT)) 
	{
		this->dump_error(m_pOciDeal->pOCI_GetLastError());
		return FALSE;
	}
	*/
	return TRUE;
}


/**************************************************************************************************
  Function:    COciConnection    
  DateTime:    2013/5	
  Description: �ͷ���Դ
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
BOOL COciConnection::fini(void)
{
	if (m_bclose)
	{
		m_bclose = FALSE;
		return m_pOciDeal->pOCI_Cleanup();

		
	}
	return TRUE;
}

/**************************************************************************************************
  Function:    dump_error    
  DateTime:    2013/5	
  Description: ������
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
void COciConnection::dump_error(OCI_Error* err)
{
	memset (this->m_error, 0, sizeof (m_error));

	sprintf (m_error, "code: ORA-%05i\n"
					   "type: %i\n"
					   "text: %s",
					   m_pOciDeal->pOCI_ErrorGetOCICode (err), 
					   m_pOciDeal->pOCI_ErrorGetType (err), 
					   m_pOciDeal->pOCI_ErrorGetString (err));
}

/**************************************************************************************************
  Function:    Open    
  DateTime:    2013/5	
  Description: �����ݿ�����
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
BOOL	COciConnection::Open()
{
    char	m_db[coniDataLen];

	memset(m_db,0x00,coniDataLen);
	sprintf(m_db,"%s/%s",m_szServer,m_szDatabase);


	if (!init())
		return FALSE;

	this->m_pCon = m_pOciDeal->pOCI_ConnectionCreate(m_db, m_szUser, m_szPass, OCI_SESSION_DEFAULT);

	if (!this->m_pCon) 
	{
		this->dump_error(m_pOciDeal->pOCI_GetLastError());
		return FALSE;
	}
	else
	{
		printf( "INFO Oci Connect %s@%s success!\n",
			m_szDatabase, m_szUser );
	}
	return TRUE;

}

/**************************************************************************************************
  Function:     Close    
  DateTime:     2013/5
  Description:  �ر�����
  Input:        NULL
  Output:       NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		����NULL
**************************************************************************************************/
BOOL	COciConnection::Close()
{
	if (m_bclose)
	{
		m_bclose = FALSE;

		if (m_pCon)
			m_pOciDeal->pOCI_ConnectionFree(m_pCon);

	}
	return TRUE;

}

/**************************************************************************************************
  Function:         IsOpen    
  DateTime:         2013/5
  Description:    	�ж����ݿ��Ƿ��
  Input:            NULL
  Output:         	NULL
  Return:      
			        �ɹ���TRUE
			        ʧ�ܣ�FALSE
  Note:				NULL
**************************************************************************************************/
BOOL	COciConnection::IsOpen()
{

	return	m_bclose;


}

/**************************************************************************************************
  Function:         BeginTrans  
  DateTime:         2013/5	
  Description:    	��ʼִ������
  Input:          	NULL
  Output:         	NULL
  Return:         	��ʼִ�е�����ļ�¼
  Note:				
**************************************************************************************************/
UINT	COciConnection::BeginTrans()
{
	if (!m_pOciDeal->pOCI_Commit(this->m_pCon)) 
	{
		this->dump_error(m_pOciDeal->pOCI_GetLastError());
		return 0;
	}
	return 1;

}

/**************************************************************************************************
  Function:         RollbackTrans   
  DateTime:         2013/5	
  Description:    	����Ļع�����
  Input:            NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE  ʧ�ܣ�FALSE
  Note:				
**************************************************************************************************/
BOOL	COciConnection::RollbackTrans()
{
	if (!m_pOciDeal->pOCI_Rollback(this->m_pCon)) 
	{
		this->dump_error(m_pOciDeal->pOCI_GetLastError());
		return FALSE;
	}
	return TRUE;

}


/**************************************************************************************************
Function:			CommitTrans   
DateTime:			2013/5	
Description:    	������ύ
Input:				NULL
Output:         	NULL
Return:         	�ɹ���TRUE  ʧ�ܣ�FALSE
Note:				
**************************************************************************************************/
BOOL	COciConnection::CommitTrans()
{
	if (!m_pOciDeal->pOCI_Commit(this->m_pCon)) 
	{
		this->dump_error(m_pOciDeal->pOCI_GetLastError());
		return FALSE;
	}
	return TRUE;

}

/**************************************************************************************************
  Function:		  ExecuteSql(const char*  szSql)    
  DateTime:		  2013/5
  Description:    ִ��Sql���
  Input:          strSql����׼Sql�ַ���
  Output:         NULL
  Return:         TRUE:�ɹ�			FALSE:ʧ��
  Note:				
**************************************************************************************************/
BOOL	COciConnection::ExecuteSql(const char*  szSql)
{
	try
	{
		//statement
		OCI_Statement*  hStmt;

		//result
		OCI_Resultset*  hResult;

		if (!IsOpen())
		{
			if( !Open() )
			{
				return FALSE;
			}
		}

		hStmt = m_pOciDeal->pOCI_StatementCreate(m_pCon);

		if (!hStmt) 
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());
			//MY_ERR_THROW();

			return FALSE;
		}

		m_pOciDeal->pOCI_ExecuteStmt(hStmt, szSql);
		if (!m_pOciDeal->pOCI_Commit(m_pCon))
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());

			if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);

			//MY_ERR_THROW();

			return FALSE;
		}

		if (strstr (szSql, "select") || strstr (szSql, "SELECT"))
		{
			hResult = m_pOciDeal->pOCI_GetResultset(hStmt);
			if(hResult != NULL)
			{
				if (hResult)
					m_pOciDeal->pOCI_ReleaseResultsets(hStmt);

				if (hStmt)
					m_pOciDeal->pOCI_StatementFree(hStmt);

				return TRUE;
			}
		}
		if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);
		return TRUE;
	}
	catch(...)
	{
		// ���ϴ���
		GetConnectionErrorAndDeal();
	}
	return FALSE;
}


/**************************************************************************************************
  Function:			ExecuteSql(const char*  szSql,INT64& lRowId)    
  DateTime:			2013/5
  Description:    	ִ��Sql����
  Input:          	��׼Sql��ѯ���,szTable ������
  Output:         	lRowID:Ӱ����е�ID
  Return:         	�ɹ�TRUE,ʧ��FALSE
  Note:				// ��ע
**************************************************************************************************/
BOOL	COciConnection::ExecuteSql(const char*  szSql,const char* szTable,INT64& lRowId)
{

	try
	{
		//statement
		OCI_Statement*  hStmt;

		//result
		OCI_Resultset*  hResult;

		if (!IsOpen())
		{
			if( !Open() )
			{
				return FALSE;
			}
		}

		hStmt = m_pOciDeal->pOCI_StatementCreate(m_pCon);

		if (!hStmt) 
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());
			//MY_ERR_THROW();

			return FALSE;
		}

		m_pOciDeal->pOCI_ExecuteStmt(hStmt, szSql);
		if (!m_pOciDeal->pOCI_Commit(m_pCon))
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());

			if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);
			

			//MY_ERR_THROW();

			return FALSE;
		}

		// ��ȡ��Ӱ����еĶ�ӦID
		char	szInsertIdSql[coniSqlStrLen] = {0};

		memset(szInsertIdSql,0x0,coniSqlStrLen);

		sprintf(szInsertIdSql,"select S_%s.CURRVAL AS  %s  from dual",
							szTable,INSERT_ID_FIELED_NAME);

		m_pOciDeal->pOCI_ExecuteStmt(hStmt, szInsertIdSql);
		if (!m_pOciDeal->pOCI_Commit(m_pCon))
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());

			if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);

			//MY_ERR_THROW();

			return FALSE;
		}
		sprintf(szInsertIdSql,"SELECT  NVL(MAX(ID),1) AS  %s  FROM %s",INSERT_ID_FIELED_NAME,szTable);

		m_pOciDeal->pOCI_ExecuteStmt(hStmt, szInsertIdSql);
		if (!m_pOciDeal->pOCI_Commit(m_pCon))
		{
			this->dump_error(m_pOciDeal->pOCI_GetLastError());

			if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);

			//MY_ERR_THROW();

			return FALSE;
		}
		else
		{

			hResult = m_pOciDeal->pOCI_GetResultset(hStmt);

			if (!m_pOciDeal->pOCI_FetchLast(hResult)) 
			{
				this->dump_error(m_pOciDeal->pOCI_GetLastError());

				if (hStmt)
					m_pOciDeal->pOCI_StatementFree(hStmt);

				//MY_ERR_THROW();

				return FALSE;
			}
			lRowId = m_pOciDeal->pOCI_GetInt(hResult, 1);

		}

/*
		if (strstr (szSql, "select") || strstr (szSql, "SELECT"))
		{
			hResult = OCI_GetResultset(hStmt);
			if(hResult != NULL)
			{
				if (hResult)
					OCI_ReleaseResultsets(hStmt);

				if (hStmt)
					OCI_StatementFree(hStmt);

				return TRUE;
			}
		}
*/
		if (hStmt)
				m_pOciDeal->pOCI_StatementFree(hStmt);

		if (hResult)
				m_pOciDeal->pOCI_ReleaseResultsets(hStmt);
		


		return TRUE;
	}
	catch(...)
	{
		// ���ϴ���
		GetConnectionErrorAndDeal();
	}
	return FALSE;

}

/**************************************************************************************************
  Function		: ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
  DateTime		: 2013/5	
  Description	: ��ҳ��ѯ
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:
**************************************************************************************************/
IRecordSet*		COciConnection::ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
{
	if (iStartRow < coniZero)
	{
		return NULL;
	}
	if (iRowNum <= coniZero)
	{
		return	NULL;
	}

	// ��������ַ������ϳ�������ַ�����ʽ
	string	strFullSql = "";
	if (GetFullPageQuerySql(szSql,iStartRow,iRowNum,strFullSql))
	{
		// ����һ�����󣬷���β��ʹ��
		COciRecordSet*		pcsCOciRecordSet = NULL;
		pcsCOciRecordSet = new COciRecordSet();
		// �������Ӷ���ָ��
		pcsCOciRecordSet->SetConnection(this);

		// ��ѯ���ݼ�
		if (pcsCOciRecordSet->QuerySql(strFullSql.c_str())!=NULL)
		{
			return	pcsCOciRecordSet;
		}
		else
		{
			delete	pcsCOciRecordSet;
			pcsCOciRecordSet = NULL;
			return NULL;
		}
	}
	return	NULL;
}

/**************************************************************************************************
  Function		: GetFullPageQuerySql
  DateTime		: 2013/5	
  Description	: �����ַ�������
  Input			: 
  Output		: 
  Return		: 
  Note			:
**************************************************************************************************/
BOOL		COciConnection::GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql)
{
	//�޸�ʵ�ַ�ʽ��ֱ�ӽ��ӿڴ�������SQL���з�װ����,hf�޸���2011.03.23
	string strSql(szSql);
	stringstream	strFullSqlInfo;

	//���SQLǰ�����
	strFullSqlInfo << "SELECT  * FROM (SELECT ROWNUM R,GS_SELECT.* FROM ( ";

	strFullSqlInfo << strSql;

	//���SQL�������
	strFullSqlInfo << " ) GS_SELECT WHERE ROWNUM < ";
	strFullSqlInfo <<  iStartRow + iRowNum; 
	strFullSqlInfo << " ) WHERE R >= ";
	strFullSqlInfo << iStartRow;

	strFullSql	=	strFullSqlInfo.str();

	return	TRUE;
}

/**************************************************************************************************
  Function: ExecuteQuery(const char*  szSql)    
  DateTime: 2013/5	
  Description:    	// �������ܡ����ܵȵ�����
  Input:          	ִ�в�����Sql���
  Output:         	NULL
  Return:         	IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
  Note:				
					IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
**************************************************************************************************/
IRecordSet*		COciConnection::ExecuteQuery(const char*  szSql)
{
	// ����һ�����󣬷���β��ʹ��
	COciRecordSet*		pcsCOciRecordSet = NULL;
	pcsCOciRecordSet = new COciRecordSet();
	// �������Ӷ���ָ��
	pcsCOciRecordSet->SetConnection(this);

	// ��ѯ���ݼ�
	if (pcsCOciRecordSet->QuerySql(szSql)!=NULL)
	{
		return	pcsCOciRecordSet;
	}
	else
	{
		delete	pcsCOciRecordSet;
		pcsCOciRecordSet = NULL;
		return NULL;
	}

}	

/**************************************************************************************************
Function		: GetEmptyRecordSet()
DateTime		: 2013/5	
Description		: ����һ���յ����ݼ���Addnew���ݵ�ʱ���õ���
Input			: NULL
Output			: NULL
Return			: NULL
Note			:
**************************************************************************************************/
IRecordSet*		COciConnection::GetEmptyRecordSet()
{
	// ����һ�����󣬷���β��ʹ��
	COciRecordSet*		pcsCOciRecordSet = NULL;
	pcsCOciRecordSet = new COciRecordSet();
	// �������Ӷ���ָ��
	pcsCOciRecordSet->SetConnection(this);

	// ��������� ReleaseRecordSet�����ͷ�
	return	pcsCOciRecordSet;
}

/**************************************************************************************************
  Function:ReleaseConnection()     
  DateTime: 2013/5
  Description:    	�ͷ����Ӷ���ӿڣ���ʹ����Ϻ�����Ӷ��󣬷��ص����ӳ���
  Input:          	NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE��ʧ�ܣ�FALSE
  Note:				
					�ͷ����Ӷ���ӿڣ���ʹ����Ϻ�����Ӷ��󣬷��ص����ӳ���
**************************************************************************************************/
BOOL			COciConnection::ReleaseConnection() 
{
	// ��ȡ����
	if (CDBConnectionPool::Instance()!=NULL)
	{
		CDBConnectionPool::Instance()->ReleaseConnection((CConnection*)this);
	}
	return TRUE;

}



/**************************************************************************************************
Function		: GetConnectionErrorAndDeal()    
DateTime		: 2013/5	
Description		: ��ȡ�������Ӳ�������Ӧ��������Ҫ�������Ͽ������ݿ�Ͽ��Ȳ�������
Input			: NULL
Output			: NULL
Return			: TRUE:�ɹ�  FALSE:ʧ��
Note			:
**************************************************************************************************/
BOOL	COciConnection::GetConnectionErrorAndDeal()
{
//	Close();
//	Open();
	return	FALSE;
}

// ʱ���ַ���ת����ʱ�����ӿ�
string	COciConnection::ToTime(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'HH24:Mi:SS')", szDateTime);
	return	string(szDt);
}
string	COciConnection::ToDate(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'YYYY-MM-DD')", szDateTime);
	return	string(szDt);
}
string	COciConnection::ToDateTime(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'YYYY-MM-DD HH24:Mi:SS')", szDateTime);
	return	string(szDt);
}


// ���ҹؼ��֣��ɹ���TRUE��ʧ�ܣ�FALSE �����عؼ���ǰ����ַ���
BOOL	COciConnection::Find_keyWord_GetString(const char* szSql,const char* szKeyWord,string& strBefore,string& strAfter)
{

	return	TRUE;

}
// ��ȡ FROMǰ����ַ���
BOOL		COciConnection::GetBeforeFromSqlText(const char* szSql,string& strBeforeFromSql)
{
	return	FALSE;
}

// ��� ROWNUM R , ��SELECT ����
BOOL		COciConnection::InsertRowinfo2SqlText(const char* szSql,string& strSql)
{
	return	FALSE;
}
