#include "COracleConnection.h"
#include "COracleRecordSet.h"
#include "CDBConnectionPool.h"

using namespace DBAccessModule;


#define MY_ERR_THROW( ) int iTimes = 20;\
	do { \
	if( iErrno==ERRNO_NETWORK_DISCNN || iErrno==ERRNO_NETWORK_LOSTCNN) \
	{    \
       Close(); \
	} \
	else break;\
    /* GetConnectionErrorAndDeal();*/ \
	} while((!Open())&&(--iTimes))

COracleConnection::COracleConnection(void)
{
//#ifdef _WIN32
#if 0
	// ���� Connection ����
	m_pConnection.CreateInstance("ADODB.Connection");

#else
	// linux
	// ���Ӿ��
	m_hDbc	=	SQL_NULL_HANDLE;
	// �������
	m_hEnv	=	SQL_NULL_HANDLE;	
	// ����״̬��־
	m_bConnectFlag = FALSE;
#endif

}

// ��������
COracleConnection::~COracleConnection(void)
{
//#ifdef _WIN32
#if 0
	if (m_pConnection != NULL)
	{
		if (IsOpen())
		{
			Close();
		}

		m_pConnection.Release();
		m_pConnection = NULL;
	}
#else
	//linux
	if (IsOpen())
	{	
		// �Ͽ�����
		SQLDisconnect(m_hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);

		// ������Դ
		m_hEnv = SQL_NULL_HANDLE;
		m_hDbc = SQL_NULL_HANDLE;

		// ����״̬��־
		m_bConnectFlag = FALSE; 

	}
#endif

}


//---------------------------------------------------------------------------------
// �ڲ��ӿ�:�򿪣��رգ��жϴ�
//---------------------------------------------------------------------------------
/**************************************************************************************************
  Function: Open    
  DateTime: 2010/5/24 21:01	
  Description: �����ݿ�����
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
BOOL	COracleConnection::Open()	
{
//#ifdef _WIN32
#if 0
	// �����ַ�������
	const	INT		coniConStrLen = 1024;
	
	// ��ʼ�����Ӵ�
	char	szConnectStr[coniConStrLen] = {0};
	memset(szConnectStr,0x0,coniConStrLen);

	if (m_pConnection == NULL)
	{
		m_pConnection.CreateInstance("ADODB.Connection");
	}

	// ��ȡ�����ַ��� Microsoft OLE DB Provider for Oracle �����ģ���֧��ORACLE ��CLOB�ȴ��ֶ�
	// Provider=MSDAORA.1;Password=gxx01;User ID=gxx01;Data Source=192.168.26.127:1521/C3MLSC;Persist Security Info=True
	// sprintf_s(szConnectStr,"Provider=MSDAORA.1;Password=%s;User ID=%s;Data Source=%s:1521/%s;Persist Security Info=True",m_szUser,m_szPass,m_szServer,m_szDatabase);
	
	// ��ȡ�����ַ���  Oracle Provider for OLE DB �����ģ�֧��ORACLE ��CLOB�ȴ��ֶ�
	// Provider=OraOLEDB.Oracle.1;Password=video01;Persist Security Info=True;User ID=video01;Data Source=192.168.5.101:1521/C3M_VIDEO
	sprintf_s(szConnectStr,"Provider=OraOLEDB.Oracle.1;Password=%s;Persist Security Info=True;User ID=%s;Data Source=%s",m_szPass,m_szUser,m_szDatabase);


	// �����Ӷ������Ч�Խ��м��
	if (m_pConnection == NULL)
	{
		return	ERROR_DB_CREATE_CONNECTION_ERROR;
	}

	// ��������Ƿ��Ѿ��� 
	if (IsOpen())
	{
		Close();
	}

	// �����ݿ�����
	try
	{
		//	�������ݿ�
		//	[adConnectUnspecified]:ͬ����ʽ
		//	[adAsyncConnect]:�첽��ʽ
		HRESULT hr=m_pConnection->Open(_bstr_t(szConnectStr),"","",adModeUnknown);
		if(!SUCCEEDED(hr))   
		{
			return ERROR_DB_CONNECT_ERROR;
		}

	}
	catch (_com_error e)
	{
		char		szErrMsg[2013]={0};
		sprintf_s(szErrMsg,"Warning: �ر����ݿⷢ���쳣. ������Ϣ: %s; �ļ�: %s; ��: %d",e.ErrorMessage(), __FILE__, __LINE__);
		return ERROR_DB_CONNECT_ERROR;
	}
#else
	// linux
	// ����ֵ����
	SQLRETURN	retcode;
	
	// ����Sql�������
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv) ;
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{
		// ����SQL �����������
		retcode = SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
		{
			// ����sql���Ӿ��
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
			{
				// �������ݿ�
				retcode = SQLConnect(m_hDbc, (SQLCHAR*)m_szDatabase, SQL_NTS, (SQLCHAR*)m_szUser, SQL_NTS, 
					(SQLCHAR*)m_szPass, SQL_NTS);

				/*int i = 0;
				SQLCHAR SqlState[SQL_MAX_MESSAGE_LENGTH];
				SQLCHAR Msg[SQL_MAX_MESSAGE_LENGTH];
				memset(SqlState,0x0,SQL_MAX_MESSAGE_LENGTH);
				memset(Msg,0x0,SQL_MAX_MESSAGE_LENGTH);
				SQLINTEGER NativeError = 0;
				SQLSMALLINT MsgLen;
				while ((retcode = SQLGetDiagRec(SQL_HANDLE_STMT, m_hDbc, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen) != SQL_NO_DATA_FOUND))
				{
					std::cout<<"����״̬:"<<SqlState<<endl;
					std::cout<<"�������:"<<NativeError<<endl;
					std::cout<<"��������:"<<Msg<<endl;
					++i;
					memset(SqlState,0x0,SQL_MAX_MESSAGE_LENGTH);
					memset(Msg,0x0,SQL_MAX_MESSAGE_LENGTH);
				}*/
				
				// �ж������ӳɹ�
				if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
				{					
					std::cout<<"---> DBAccess Error��COracleConnection::Open() DB SQLConnect failed !"<<endl;
					SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); 
					SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
					return FALSE;
				}
				else
				{
					printf( "INFO Oracle Connect %s@%s success!\n",
						m_szDatabase, m_szUser );
				}
			} 
			else
			{
				std::cout<<"---> DBAccess Error��COracleConnection::Open() DB SQLAllocHandle failed !"<<endl;
				// ����Sql���Ӿ��ʧ��
				SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); 
				SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
				return FALSE;
			}
		}
		else
		{
			std::cout<<"---> DBAccess Error��COracleConnection::Open() DB SQLSetEnvAttr failed !"<<endl;
			// ����Sql �������ʧ��
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
			return FALSE;
		}
	}
	else
	{
		std::cout<<"---> DBAccess Error��COracleConnection::Open()  DB SQLAllocHandle failed !"<<endl;
		// ����Sql�������ʧ��
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		return FALSE;
	}

	m_bConnectFlag = TRUE;

#endif
	LoadSeqMap();
	return  TRUE;

}

/**************************************************************************************************
  Function: Close    
  DateTime: 2010/5/24 22:36	
  Description:  �ر�����
  Input:        NULL
  Output:       NULL
  Return:       TRUE : �ɹ�   FALSE :ʧ��
  Note:		����NULL
**************************************************************************************************/
BOOL	COracleConnection::Close()
{
//#ifdef _WIN32
#if 0
	// ���ݿ����Ӷ������Ч�Լ��
	if (m_pConnection == NULL)
	{
		return FALSE;
	}

	// ������ݿ������Ƿ��Ѿ���
	if (!IsOpen())
	{
		return TRUE;
	}

	// �ر����ݿ�����
	try
	{
		if (m_pConnection != NULL && IsOpen()) 
		{
			m_pConnection->Close();
		}
		return TRUE;
	}
	catch (_com_error e)
	{
	//	m_strError.Format(_T("Warning: �ر����ݿⷢ���쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	}
#else
	// linux
	// ����Ǵ򿪵�
	if(m_bConnectFlag)
	{
		// �Ͽ����ݿ�����
		SQLDisconnect(m_hDbc);
		// �ͷ����ݿ����Ӿ��
		SQLFreeHandle(SQL_HANDLE_DBC,m_hDbc); 
		// �ͷ����ݿ⻷�����
		SQLFreeHandle(SQL_HANDLE_ENV,m_hEnv);

		// ���Ӿ��
		m_hDbc	=	SQL_NULL_HANDLE;
		// �������
		m_hEnv	=	SQL_NULL_HANDLE;	
		
		// ���ӱ�־
		m_bConnectFlag = FALSE;
	}
#endif
	return	TRUE;

}

/**************************************************************************************************
  Function: IsOpen    
  DateTime: 2010/5/24 22:40	
  Description:    	�ж����ݿ��Ƿ��
  Input:            NULL
  Output:         	NULL
  Return:         	TRUE : ��   FALSE:�ر�
  Note:				NULL
**************************************************************************************************/
BOOL	COracleConnection::IsOpen()
{
//#ifdef _WIN32
#if 0
	// ��ȡ���ݿ����ӵ�״̬,��ȡ�ĸ�״ֵ̬�еĴ򿪲���adStateOpen
	try
	{
		return (m_pConnection != NULL && (m_pConnection->State & adStateOpen));
	}
	catch (_com_error e)
	{
		// TRACE(_T("Warning: IsOpen ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 
#else
	// linux
	return	m_bConnectFlag;
#endif

	return	TRUE;

}



//---------------------------------------------------------------------------------
// �ڲ��ӿ�:������
//---------------------------------------------------------------------------------
/**************************************************************************************************
  Function:   BeginTrans  
  DateTime: 2010/5/24 22:41	
  Description:    	��ʼִ������
  Input:          	NULL
  Output:         	NULL
  Return:         	��ʼִ�е�����ļ�¼
  Note:				һ�������� BeginTrans ����, �ڵ��� CommitTrans �� RollbackTrans
					��������֮ǰ, ���ݿ⽫���������ύ�������κθ���
**************************************************************************************************/
UINT	COracleConnection::BeginTrans()
{
#if 0
	// �ύ����
	try
	{
		return m_pConnection->BeginTrans();
	}
	catch (_com_error e)
	{
	//	TRACE(_T("Warning: BeginTrans ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return ERROR_DB_TRANS_RET_ERROR;
	}
#else
// linux
	// ��ʼ����
	SQLRETURN	retcode;
	// ��ʼ������

	// ����Ϊ�ֶ��ύģʽ, SQL_AUTOCOMMIT_OFF
	retcode	=	SQLSetConnectAttr (m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, SQL_IS_POINTER);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		return	ERROR_DB_TRANS_RET_SUCCESS;
	}

	return	ERROR_DB_TRANS_RET_ERROR;

#endif
	return	ERROR_DB_TRANS_RET_ERROR;

}

/**************************************************************************************************
  Function: RollbackTrans   
  DateTime: 2010/5/24 22:41	
  Description:    	����Ļع�����
  Input:            NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE  ʧ�ܣ�FALSE
  Note:				
**************************************************************************************************/
BOOL	COracleConnection::RollbackTrans()
{
#if 0
	// �ع�����
	try
	{
		if (SUCCEEDED(m_pConnection->RollbackTrans()))
		{
			return	TRUE;
		}		
	}
	catch(...)
	{
		return FALSE;
	} 
#else
	SQLRETURN	retcode;
	// ��ʼ�ع�����
	retcode	=	SQLTransact(m_hEnv,m_hDbc,SQL_ROLLBACK);
	if(retcode == SQL_SUCCESS_WITH_INFO || retcode == SQL_SUCCESS)
	{
		// ����Ϊ�Զ��ύģʽ
		retcode	=	SQLSetConnectAttr (m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, SQL_IS_POINTER);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			return	ERROR_DB_TRANS_RET_SUCCESS;
		}
	}
	return	ERROR_DB_TRANS_RET_ERROR;
#endif
	// �쳣����
	return FALSE;

}


/**************************************************************************************************
Function: CommitTrans   
DateTime: 2010/5/24 22:41	
Description:    	������ύ
Input:				NULL
Output:         	NULL
Return:         	�ɹ���TRUE  ʧ�ܣ�FALSE
Note:				
**************************************************************************************************/
BOOL	COracleConnection::CommitTrans()
{
#if 0
	// �ύ����
	try
	{
		if (SUCCEEDED(m_pConnection->CommitTrans()))
		{
			return	TRUE;
		}		
	}
	catch(...)
	{
		return FALSE;
	} 
#else
	SQLRETURN	retcode;
	// ��ʼ������
	retcode	=	SQLTransact(m_hEnv,m_hDbc,SQL_COMMIT);
	if(retcode == SQL_SUCCESS_WITH_INFO || retcode ==  SQL_SUCCESS)
	{
		// ����Ϊ�Զ��ύģʽ
		retcode	=	SQLSetConnectAttr (m_hDbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, SQL_IS_POINTER);
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			return	ERROR_DB_TRANS_RET_SUCCESS;
		}
	}
	return	ERROR_DB_TRANS_RET_ERROR;
#endif
	// �쳣����
	return FALSE;

}
//------------------------------------------------------------------------------------------------
// OuterInterface��ʵ��
//------------------------------------------------------------------------------------------------
/**************************************************************************************************
  Function: ExecuteSql(const char*  szSql)    
  DateTime: 2010/5/26 17:14	
  Description:    ִ��Sql���
  Input:          strSql����׼Sql�ַ���
  Output:         NULL
  Return:         TRUE:�ɹ�			FALSE:ʧ��
  Note:				
**************************************************************************************************/
BOOL			COracleConnection::ExecuteSql(const char*  szSql)
{
#if 0
	if (m_pConnection != NULL)
	{
		try
		{
			// ���Ӵ���
			if (IsOpen())
			{
				// ִ��sql���,ִ�к����ݼ���Ϊ��
				if (m_pConnection->Execute(_bstr_t(szSql),NULL,adCmdText) != NULL)
				{
					// �ύ����
					return	TRUE;
				}
				else
				{
					// �ع�����
					return	FALSE;
				}
			}
			else
			{
				// �����ݿ�����
				Open();
			}
		}// end try
		catch(...)
		{
			// ����쳣���ж��Ƿ������ݿ����ӳ���
			GetConnectionErrorAndDeal();
		}
	}
#else
	// linux
	try
	{
		if (!IsOpen())
		{
			if( !Open() )
			{

				// ���ش���
				return FALSE;
			}
		}
		// Sql ��ѯ�����
		SQLHSTMT	hstmt;
		// ����ֵ����
		SQLRETURN	retcode;

		// ����SQL�������ÿ��ִ��SQL��䶼�����������������ִ����ɺ��ͷ�
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hstmt);	
		// �ж�����Sql������Ƿ�ɹ����ɹ���ִ��Sql���
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
		{
			// ִ�гɹ�	// ִ��Sql���
			retcode = SQLExecDirect(hstmt, (SQLCHAR*)szSql, SQL_NTS) ;
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO || retcode == SQL_NO_DATA) 
			{
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
				return	TRUE;
			}
			else
			{				
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
                INT iErrno = 0;
				ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);
				//----------------------------------------------------------------------------------
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                MY_ERR_THROW();
				return	FALSE;
			}
		}
		else
		{	
			//----------------------------------------------------------------------------------
			// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
            INT iErrno = 0;
            ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);
			//----------------------------------------------------------------------------------

			// ����Sql ���ִ�о�������ͷ�Sql��ѯ�����
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
            MY_ERR_THROW();
			return FALSE;	
		}	
	}
	catch(...)
	{
		// ���ϴ���
		GetConnectionErrorAndDeal();
	}
#endif
	return FALSE;

}


/**************************************************************************************************
  Function: ExecuteSql(const char*  szSql,INT64& lRowId)    
  DateTime: 2010/5/26 17:16	
  Description:    	ִ��Sql����
  Input:          	��׼Sql��ѯ���,szTable ������
  Output:         	lRowID:Ӱ����е�ID
  Return:         	�ɹ�TRUE,ʧ��FALSE
  Note:				// ��ע
**************************************************************************************************/
BOOL			COracleConnection::ExecuteSql(const char*  szSql,const char* szTable,INT64& lRowId)
{
#if 0
	if (m_pConnection != NULL)
	{
		try
		{
			// ���Ӵ���
			if (IsOpen())
			{
				// ������ʶ
				BOOL		bSucFlag = FALSE;

				// ִ��sql���,ִ�к����ݼ���Ϊ��
				if (m_pConnection->Execute(_bstr_t(szSql),NULL,adCmdText) != NULL)
				{

					// ��ȡ���ֵID
					char	szInsertIdSql[coniSqlStrLen] = {0};
					memset(szInsertIdSql,0x0,coniSqlStrLen);
					// Sql �����÷�
					// SELECT  NVL(MAX(ID),1) AS ID FROM VID_TB_TEST_2
					sprintf(szInsertIdSql,"SELECT  NVL(MAX(ID),1) AS  %s  FROM %s",INSERT_ID_FIELED_NAME,szTable);

					// ִ��sql��ѯ
					_RecordsetPtr	_ResordSet;
					_ResordSet = m_pConnection->Execute(_bstr_t(szInsertIdSql),NULL,adCmdText);
					while (!_ResordSet->adoEOF)
					{
						// ��ȡֵ��Ϣ
						_variant_t value = _ResordSet->GetCollect(_variant_t(INSERT_ID_FIELED_NAME));
						// ��ֵ����ת��
						lRowId = atol((char*)(_bstr_t)value);
						bSucFlag = TRUE;
						break;
					}				

					memset(szInsertIdSql,0x0,coniSqlStrLen);
					// 
					if (bSucFlag)
					{
						return	TRUE;
					}	
				}
				else
				{
					return	FALSE;
				}
			}// if (IsOpen())
			else
			{
				// �����ݿ�����
				Open();
			}
		}// end try
		catch(...)
		{
			// ����쳣���ж��Ƿ������ݿ����ӳ���
			GetConnectionErrorAndDeal();
		}
	}
	
#else
	// linux
	try
	{
		if (!IsOpen())
		{
			if( !Open() )
			{

				// ���ش���
				return FALSE;
			}
		}
		
		// ������ʶ
		BOOL		bSucFlag = FALSE;

		// Sql ��ѯ�����
		SQLHSTMT	hstmt;
		SQLHSTMT	hstmtQuery;
		// ����ֵ����
		SQLRETURN	retcode;
		
		// ����SQL�������ÿ��ִ��SQL��䶼�����������������ִ����ɺ��ͷ�
		retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hstmt);	
		// �ж�����Sql������Ƿ�ɹ����ɹ���ִ��Sql���
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
		{	
			// ִ��Sql���
			retcode = SQLExecDirect(hstmt, (SQLCHAR*)szSql, SQL_NTS) ;
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
			{			
				// �Ƿ����
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);	

				// ����SQL�������ÿ��ִ��SQL��䶼�����������������ִ����ɺ��ͷ�
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hstmtQuery);	
				if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
				{
					SQLFreeHandle(SQL_HANDLE_STMT,	hstmt);	
					SQLFreeHandle(SQL_HANDLE_STMT,	hstmtQuery);                    
					return FALSE;
				}
				// ָ��Ҫʹ�õ��α겢������
				SQLSetStmtAttr(hstmtQuery, SQL_ATTR_CONCURRENCY,(SQLPOINTER) SQL_CONCUR_ROWVER, 0);
				// ���ù������Ϊ�������,
				SQLSetStmtAttr(hstmtQuery, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC /* SQL_CURSOR_KEYSET_DRIVEN*/, 0);

				// ��ȡ��Ӱ����еĶ�ӦID
				char	szInsertIdSql[coniSqlStrLen] = {0};
				memset(szInsertIdSql,0x0,coniSqlStrLen);
				// Sql �����÷�

				const std::string &strSeqName = GetIDSequenceNameOfTableName(szTable);
				if( strSeqName.empty() )
				{

					sprintf(szInsertIdSql,"select S_%s.CURRVAL AS  %s  from dual",
						szTable,INSERT_ID_FIELED_NAME);
				}
				else
				{
					sprintf(szInsertIdSql,"select %s.CURRVAL AS  %s  from dual",
						strSeqName.c_str() ,INSERT_ID_FIELED_NAME);
				}
				retcode = SQLExecDirect(hstmtQuery, (SQLCHAR*)szInsertIdSql, SQL_NTS);

				if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) 
				{
					assert(0);
					//û������
					sprintf(szInsertIdSql,"SELECT  NVL(MAX(ID),1) AS  %s  FROM %s",INSERT_ID_FIELED_NAME,szTable);



					// ִ��sql���
					retcode = SQLExecDirect(hstmtQuery, (SQLCHAR*)szInsertIdSql, SQL_NTS);
					if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) 
					{
						
						// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
						INT iErrno = 0;
						ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);						
						SQLFreeHandle(SQL_HANDLE_STMT, hstmtQuery);	
						MY_ERR_THROW();
						return FALSE;
					}

				}

				{				
					// ��ȡ����
					SQLRETURN	iRetFlag = 0;
					SQLCHAR			szData[FIELD_DATA_LEN] = {0};
					memset(szData,0x0,FIELD_DATA_LEN);
					SQLLEN		iActualDataSize = 0;
					// ��һ��,������1��ʼ
					SQLSMALLINT	iIdCollumn = 1;					
					// ����ƶ�����һ����¼
					SQLFetchScroll(hstmtQuery, SQL_FETCH_FIRST, 0);

					// ��ȡֵ����
					iRetFlag = SQLGetData(hstmtQuery,iIdCollumn,SQL_C_CHAR,szData,FIELD_DATA_LEN,&iActualDataSize);
					// �жϻ�ȡ�����Ƿ�ɹ�
					if (SQL_SUCCEEDED(iRetFlag)) 
					{
						// ��ȡ����ID
						lRowId = atol((char*)szData);
					}
					else
					{
						//----------------------------------------------------------------------------------
						// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
                        INT iErrno = 0;
                        ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);
						//----------------------------------------------------------------------------------
						// ��ȡ����ʧ��
						SQLFreeHandle(SQL_HANDLE_STMT, hstmtQuery);	
                         MY_ERR_THROW();
						return	FALSE;
					}
					SQLFreeHandle(SQL_HANDLE_STMT, hstmtQuery);	
					return  TRUE;		
				}

			}
			else
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
                INT iErrno = 0;
                ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);
				//----------------------------------------------------------------------------------

				// ִ��ʧ��
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);	
                 MY_ERR_THROW();
				return FALSE;
			}			
		}
		else
		{	
			//----------------------------------------------------------------------------------
			// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
            INT iErrno = 0;
            ErrorLogInfo(m_hEnv,m_hDbc,hstmt, &iErrno);
			//----------------------------------------------------------------------------------

			// ����Sql ���ִ�о�������ͷ�Sql��ѯ�����
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
             MY_ERR_THROW();
			return FALSE;	
		}	
	}
	catch(...)
	{
		// ���ϴ���
		GetConnectionErrorAndDeal();
	}

#endif
	// �޷�ʵ��
	lRowId = -1;
	return	FALSE;

}

/**************************************************************************************************
  Function		: ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
  DateTime		: 2010/12/2 10:26	
  Description	: ��ҳ��ѯ
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:
**************************************************************************************************/
IRecordSet*		COracleConnection::ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
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
	// SELECT ID FROM (SELECT ROWNUM R ,ID FROM TB_DEVINFO EMP WHERE ROWNUM < 200 ) WHERE R>100
	// ���Ӽ������ѯ���ݾͿ�����
	string	strFullSql = "";
	if (GetFullPageQuerySql(szSql,iStartRow,iRowNum,strFullSql))
	{
		// ����һ�����󣬷���β��ʹ��
		COracleRecordSet*		pcsCOracleRecordSet = NULL;
		pcsCOracleRecordSet = new COracleRecordSet();
		// �������Ӷ���ָ��
		pcsCOracleRecordSet->SetConnection(this);

		// ��ѯ���ݼ�
		if (pcsCOracleRecordSet->QuerySql(strFullSql.c_str())!=NULL)
		{
			return	pcsCOracleRecordSet;
		}
		else
		{
			delete	pcsCOracleRecordSet;
			pcsCOracleRecordSet = NULL;
			return NULL;
		}
	}
	return	NULL;
}


// ʱ���ַ���ת����ʱ�����ӿ�
string	COracleConnection::ToTime(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'HH24:Mi:SS')", szDateTime);
	return	string(szDt);
}
string	COracleConnection::ToDate(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'YYYY-MM-DD')", szDateTime);
	return	string(szDt);
}
string	COracleConnection::ToDateTime(const char*	szDateTime)
{
	char	szDt[64] = {0};
	sprintf(szDt, "TO_DATE('%s', 'YYYY-MM-DD HH24:Mi:SS')", szDateTime);
	return	string(szDt);
}


// ���ҹؼ��֣��ɹ���TRUE��ʧ�ܣ�FALSE �����عؼ���ǰ����ַ���
BOOL	COracleConnection::Find_keyWord_GetString(const char* szSql,const char* szKeyWord,string& strBefore,string& strAfter)
{
	strBefore	=	"";
	strAfter	=	"";

	// char*ת��Ϊstring
	string	strSql		= string(szSql);
	string	strKeyWord	= string(szKeyWord);

	// ת��Ϊ��д
	//transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper());
	//transform(strKeyWord.begin(),strKeyWord.end(),strKeyWord.begin(),ToUpper());

	// szKeyWord������szSql�е�λ��
	string::size_type	nFromPos = strSql.find(strKeyWord.c_str(),0);

	// û���ҵ�����FALSE
	if (string::npos == nFromPos)
	{
		return FALSE;
	}

	// ����ҵ�������ȡ����
	try
	{
		// ��ȡstrKeyWord�ĳ���
		string::size_type	nLenKeyWord = strKeyWord.length();

		// ��ȡstrSql�ĳ���
		string::size_type	nLenSql		= strSql.length();

		// ��ȡstrSql�г���strKeyWord֮ǰ���ַ�
		strBefore	= strSql.substr(0,nFromPos);

		// ��ȡstrSql�г���strKeyWord֮����ַ�
		strAfter	= strSql.substr(nFromPos+nLenKeyWord,nLenSql-nFromPos-nLenKeyWord);

		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}

	return	TRUE;


}

// ��ȡ FROMǰ����ַ���
// SELECT ROWNUM R,ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP 
// ---> SELECT ROWNUM R,ID,DEV_ID,DEV_NAME 
BOOL		COracleConnection::GetBeforeFromSqlText(const char* szSql,string& strBeforeFromSql)
{
	try
	{
		string	strSql  = string(szSql);
		// ���������һ��from������ַ���

		// ת��Ϊ��д
		//transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper()); 

		// FROM �ַ����ֵ�λ��
		string::size_type	nFromPos = strSql.find(constrFromKeyWord.c_str());

		// û���ҵ�
		if (nFromPos == string::npos)
		{
			return	FALSE;
		}

		// ��ȡ from ��ǰ���ַ���
		strBeforeFromSql  =	strSql.substr(0,nFromPos);

	}
	catch(...)
	{
		return	FALSE;
	}

	return	TRUE;
}


// ��� ROWNUM R , ��SELECT ����
// SELECT ID,DEV_ID,DEV_NAME 
// SELECT ROWNUM R,ID,DEV_ID,DEV_NAME 
BOOL		COracleConnection::InsertRowinfo2SqlText(const char* szSql,string& strSql)
{
	try
	{
		// ��������Ϣ���ַ���
		stringstream	strInfo;
		strInfo.str("");

		strSql  = string(szSql);
		string	strTemp = "";
		// ���������һ��SELECT ������ַ���

		// ת��Ϊ��д
		//transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper());

		// FROM �ַ����ֵ�λ��
		string::size_type	nSelectPos = strSql.find(constrSelectKeyword.c_str());

		// û���ҵ�
		if (nSelectPos == string::npos)
		{
			return	FALSE;
		}

		// ��ȡSELECT ������ַ���
		strSql	=	strSql.substr(nSelectPos + 6,strSql.size() - nSelectPos - 6);

		// DISTINCT������szSql�е�λ��
		string::size_type	nDISTINCTPos;
		nDISTINCTPos = strSql.find(constrDISTINCTKeyWord.c_str());

		// û���ҵ�DISTINCT
		if (nDISTINCTPos == string::npos)
		{
			// �ϳ��µ��ַ���
			strInfo << "SELECT ";
			strInfo << " ROWNUM R";
			strInfo << " , ";
			strInfo	<< strSql;
		}
		else
		{
			// ��ȡDISTINCT������ַ���
			strSql	=	strSql.substr(nDISTINCTPos + 8,strSql.size() - nDISTINCTPos - 8);

			// �ϳ��µ��ַ���
			strInfo << "SELECT DISTINCT";
			strInfo << " ROWNUM R";
			strInfo << " , ";
			strInfo	<< strSql;
		}
		
		// ��ȡ�ַ���
		strSql	=	strInfo.str();

		strInfo.str("");
	}
	catch(...)
	{
		return	FALSE;
	}

	return	TRUE;
}

// �����ַ�������
// ���룺SELECT ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP 
// �����SELECT  * FROM (SELECT ROWNUM R,TB_GS_SELECT.* FROM((SELECT ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP  )) TB_GS_SELECT ) WHERE  ROWNUM  < 51 AND R >= 1
BOOL		COracleConnection::GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql)
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

		//�ָ���
	// �����ַ�������
	// ���룺SELECT ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP 
	// �����SELECT ID,DEV_ID,DEV_NAME  FROM (SELECT ROWNUM R,ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP WHERE ROWNUM < 200 )WHERE R>=100
	/*BOOL		COracleConnection::GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql)
	{*/

	//___________________________________________________________________________________
	//string	strSql(szSql);
	//// ת��Ϊ��д,�����û���ʱ����Ϊ�д�Сд֮�֣�����ֱ��ת��Ϊ��д��֮�����޸ģ�hf�޸���2010.01.26
	////transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper()); 

	//stringstream	strFullSqlInfo;

	//// ��ʱ�ַ���
	//string		strTemp;
	//strTemp = "";

	//// ��ȡfromǰ����ַ���
	//if (!GetBeforeFromSqlText(strSql.c_str(),strTemp))
	//{
	//	return	FALSE;
	//}

	////����ѯ����������˱�������������������ʱȫ��ȡ����hf�޸���2010.01.26
	////strFullSqlInfo << strTemp;
	//strFullSqlInfo << "SELECT * FROM ( ";

	//// ��ȡ������ѯ�ַ���
	//string		strConditionSql = "" ;

	//// ����ؼ�����Ϣ [ROWNUM R,] ����
	//// ���룺SELECT ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP 
	//// �����SELECT ROWNUM R,ID,DEV_ID,DEV_NAME FROM TB_DEVINFO EMP 
	//if (!InsertRowinfo2SqlText(strSql.c_str(),strConditionSql))
	//{
	//	return	FALSE;
	//}

	//// ��ʼ���ҹؼ�����Ϣ [12/9/2010 10:08 Modify by Liujs]
	//// ����˳��
	//// 1. WHERE
	//// 2. GROUP
	//// 3. ORDER
	//string		strBefore;
	//string		strAfter;
	//// ����where
	//if (Find_keyWord_GetString(strConditionSql.c_str(),constrWhereKeyWord.c_str(),strBefore,strAfter))
	//{
	//	// �ҵ�WHERE������GROUP
	//	if (Find_keyWord_GetString(strConditionSql.c_str(),constrGroupKeyWord.c_str(),strBefore,strAfter))
	//	{
	//		// 1.����GROUP,�����Ϣ
	//		strFullSqlInfo << strBefore;
	//		strFullSqlInfo << " AND ";
	//		strFullSqlInfo << " ROWNUM ";
	//		strFullSqlInfo << " < ";
	//		strFullSqlInfo << iStartRow + iRowNum;
	//		strFullSqlInfo << " ";
	//		strFullSqlInfo << constrGroupKeyWord;
	//		strFullSqlInfo << " ";

	//		string		strTemp = strAfter;

	//		// ����BY,�����Ϣ
	//		if (Find_keyWord_GetString(strTemp.c_str(),constrBYKeyWord.c_str(),strBefore,strAfter))
	//		{
	//			strFullSqlInfo << constrBYKeyWord;
	//			strFullSqlInfo << " ROWNUM, ";
	//			strFullSqlInfo << strAfter;
	//		}
	//	}
	//	else
	//	{
	//		// 2.û���ҵ�GROUP���Ͳ���ORDER
	//		if (Find_keyWord_GetString(strConditionSql.c_str(),constrOrderKeyword.c_str(),strBefore,strAfter))
	//		{
	//			// �ҵ�ORDER
	//			strFullSqlInfo << strBefore;
	//			strFullSqlInfo << " AND ";
	//			strFullSqlInfo << " ROWNUM ";
	//			strFullSqlInfo << " < ";
	//			strFullSqlInfo << iStartRow + iRowNum;
	//			strFullSqlInfo << " ";
	//			strFullSqlInfo << constrOrderKeyword;
	//			strFullSqlInfo << " ";
	//			strFullSqlInfo << strAfter;				
	//		}
	//		else
	//		{
	//			// û���ҵ�ORDER
	//			strFullSqlInfo << strConditionSql;
	//			strFullSqlInfo << " AND ";
	//			strFullSqlInfo << " ROWNUM ";
	//			strFullSqlInfo << " < ";
	//			strFullSqlInfo << iStartRow + iRowNum;
	//		}	
	//	}
	//}
	//else
	//{
	//	// û���ҵ� WHERE������GROUP
	//	if (Find_keyWord_GetString(strConditionSql.c_str(),constrGroupKeyWord.c_str(),strBefore,strAfter))
	//	{
	//		// 1.����GROUP,�����Ϣ
	//		strFullSqlInfo << strBefore;
	//		strFullSqlInfo << " WHERE ";
	//		strFullSqlInfo << " ROWNUM ";
	//		strFullSqlInfo << " < ";
	//		strFullSqlInfo << iStartRow + iRowNum;
	//		strFullSqlInfo << " ";
	//		strFullSqlInfo << constrGroupKeyWord;
	//		strFullSqlInfo << " ";

	//		string		strTemp = strAfter;
	//		// ����BY,�����Ϣ
	//		if (Find_keyWord_GetString(strTemp.c_str(),constrBYKeyWord.c_str(),strBefore,strAfter))
	//		{
	//			strFullSqlInfo << constrBYKeyWord;
	//			strFullSqlInfo << " ROWNUM, ";
	//			strFullSqlInfo << strAfter;
	//		}
	//	}
	//	else
	//	{
	//		// 2.û���ҵ�GROUP���Ͳ���ORDER
	//		if (Find_keyWord_GetString(strConditionSql.c_str(),constrOrderKeyword.c_str(),strBefore,strAfter))
	//		{
	//			// �ҵ�ORDER
	//			strFullSqlInfo << strBefore;
	//			strFullSqlInfo << " WHERE ";
	//			strFullSqlInfo << " ROWNUM ";
	//			strFullSqlInfo << " < ";
	//			strFullSqlInfo << iStartRow + iRowNum;
	//			strFullSqlInfo << " ";
	//			strFullSqlInfo << constrOrderKeyword;
	//			strFullSqlInfo << " ";
	//			strFullSqlInfo << strAfter;				
	//		}
	//		else
	//		{
	//			// û���ҵ�ORDER
	//			strFullSqlInfo << strConditionSql;
	//			strFullSqlInfo << " WHERE ";
	//			strFullSqlInfo << " ROWNUM ";
	//			strFullSqlInfo << " < ";
	//			strFullSqlInfo << iStartRow + iRowNum;
	//		}	
	//	}
	//}

	//strFullSqlInfo << " ) ";

	//strFullSqlInfo << " WHERE R >= ";
	//strFullSqlInfo << iStartRow;

	/*strFullSql	=	strFullSqlInfo.str();

	return	TRUE;*/
//}



/**************************************************************************************************
  Function: ExecuteQuery(const char*  szSql)    
  DateTime: 2010/5/26 17:18	
  Description:    	// �������ܡ����ܵȵ�����
  Input:          	ִ�в�����Sql���
  Output:         	NULL
  Return:         	IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
  Note:				
					IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
**************************************************************************************************/
IRecordSet*		COracleConnection::ExecuteQuery(const char*  szSql)
{
	// ����һ�����󣬷���β��ʹ��
	COracleRecordSet*		pcsCOracleRecordSet = NULL;
	pcsCOracleRecordSet = new COracleRecordSet();
	// �������Ӷ���ָ��
	pcsCOracleRecordSet->SetConnection(this);

	// ��ѯ���ݼ�
	if (pcsCOracleRecordSet->QuerySql(szSql)!=NULL)
	{
		return	pcsCOracleRecordSet;
	}
	else
	{
		delete	pcsCOracleRecordSet;
		pcsCOracleRecordSet = NULL;
		return NULL;
	}

}	

/**************************************************************************************************
Function		: GetEmptyRecordSet()
DateTime		: 2010/11/29 17:53	
Description		: ����һ���յ����ݼ���Addnew���ݵ�ʱ���õ���
Input			: NULL
Output			: NULL
Return			: NULL
Note			:
**************************************************************************************************/
IRecordSet*		COracleConnection::GetEmptyRecordSet()
{
	// ����һ�����󣬷���β��ʹ��
	COracleRecordSet*		pcsCOracleRecordSet = NULL;
	pcsCOracleRecordSet = new COracleRecordSet();
	// �������Ӷ���ָ��
	pcsCOracleRecordSet->SetConnection(this);

	// ��������� ReleaseRecordSet�����ͷ�
	return	pcsCOracleRecordSet;
}

/**************************************************************************************************
  Function:ReleaseConnection()     
  DateTime: 2010/5/26 17:22	
  Description:    	�ͷ����Ӷ���ӿڣ���ʹ����Ϻ�����Ӷ��󣬷��ص����ӳ���
  Input:          	NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE��ʧ�ܣ�FALSE
  Note:				
					�ͷ����Ӷ���ӿڣ���ʹ����Ϻ�����Ӷ��󣬷��ص����ӳ���
**************************************************************************************************/
BOOL			COracleConnection::ReleaseConnection() 
{
	// ��ȡ����
	if( m_pCnnPool) 
	{
		CDBConnectionPool *p = (CDBConnectionPool*) m_pCnnPool;
		p->ReleaseConnection((CConnection*)this);
	}
	else
	{


		if (CDBConnectionPool::Instance()!=NULL)
		{
			CDBConnectionPool::Instance()->ReleaseConnection((CConnection*)this);
		}
	}
	return TRUE;

}



/**************************************************************************************************
Function		: GetConnectionErrorAndDeal()    
DateTime		: 2010/6/11 14:48	
Description		: ��ȡ�������Ӳ�������Ӧ��������Ҫ�������Ͽ������ݿ�Ͽ��Ȳ�������
Input			: NULL
Output			: NULL
Return			: TRUE:�ɹ�  FALSE:ʧ��
Note			:
**************************************************************************************************/
BOOL	COracleConnection::GetConnectionErrorAndDeal()
{
#if 0

	// ��ȡ��ϸ�Ĵ������,��������Ӧ����
	{
		// ��ȡADO���Ӷ���
		_ConnectionPtr PtrCon = GetConnection();
		if (PtrCon == NULL)
		{
			return FALSE;
		}

		// ��ȡ�������
		ErrorsPtr pErrors = PtrCon->Errors;
		int i = pErrors->Count;
		for (long n = 0; n < pErrors->Count; n++)
		{
			ErrorPtr pError = pErrors->GetItem(n);
			/*
			// ״̬
			string	strSQLState = string((char*)pError->SQLState);
			// ����
			long NativeError=pError->NativeError;

			long nNumber = pError->GetNumber();

			// �������ӶϿ�:���½�������
			if ((strSQLState == "08S0 ") && (NativeError==11))
			{
				Open();
			}
			// �������ӶϿ�:���½�������
			if ((strSQLState == "08001") && (NativeError==17))
			{
				Open();
			}
			// �������ӶϿ�:���½�������
			if ((strSQLState == "08S0 ") && (nNumber==0x80004005))
			{
				Open();
			}
			*/
			// �����ORACLE���ݿ��ֱ����������
			Open();
			break;
		}
	}

	return TRUE;
#else
	// linux
	Close();
	Open();
#endif

	return	TRUE;
}



BOOL COracleConnection::LoadSeqMap(void)
{
	m_setSeqName.clear();
	m_mapSeq.clear();
	m_strEmptySeqName.clear();

	IRecordSet *pSet = ExecuteQuery("select SEQUENCE_OWNER,SEQUENCE_NAME from ALL_SEQUENCES");
	if( !pSet )
	{
		GS_ASSERT(0);
		return FALSE;
	}
	std::string strVal;
	for( ; !pSet->Eof(); pSet->MoveNext() )
	{
		strVal.clear();
		pSet->GetCollect("SEQUENCE_NAME", strVal);
		if( !strVal.empty() )
		{
			m_setSeqName.insert(GSStrUtil::ToUpper(strVal));
		}
	}
	pSet->ReleaseRecordSet();
	return TRUE;
}

const std::string &COracleConnection::GetIDSequenceNameOfTableName( const std::string &strTableName )
{
	

	
	std::string strTemp = GSStrUtil::ToUpper(strTableName);

	std::map<std::string, std::string>::iterator csMapIt = m_mapSeq.find(strTemp);
	if( csMapIt != m_mapSeq.end() )
	{
		return csMapIt->second;
	}
	else
	{

		std::set<std::string >::iterator csIt;
		UINT iPos;
		UINT iPos2;
		for( csIt = m_setSeqName.begin(); csIt != m_setSeqName.end(); ++csIt )
		{
			iPos = (*csIt).find(strTemp);
			iPos2 = iPos+strTemp.length();
			if( iPos != std::string::npos &&
				(iPos==0 || (*csIt)[iPos-1] == '_' ) &&
				((*csIt)[iPos2] == '_' || (*csIt)[iPos2]=='\0') )
			{
				//�ҵ�
				m_mapSeq.insert(make_pair(strTemp, *csIt) ); //���뵽map �ӿ��´εĲ���
				return *csIt;
			}
		}
		std::string strOther = strTemp.substr(strTemp.find("TB_")+3);
		for( csIt = m_setSeqName.begin(); csIt != m_setSeqName.end(); ++csIt )
		{
			iPos = (*csIt).find(strOther);
			iPos2 = iPos+strOther.length();
			if( iPos != std::string::npos &&
				(iPos==0 || (*csIt)[iPos-1] == '_' ) &&
				((*csIt)[iPos2] == '_' || (*csIt)[iPos2]=='\0') )
			{
				//�ҵ�
				m_mapSeq.insert(make_pair(strTemp, *csIt) ); //���뵽map �ӿ��´εĲ���
				return *csIt;
			}
		}

	}
	return m_strEmptySeqName;
}