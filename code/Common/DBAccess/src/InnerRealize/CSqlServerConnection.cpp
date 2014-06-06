#include "CSqlServerConnection.h"
#include "CDBConnectionPool.h"
#include "CSqlServerRecordSet.h"

using	namespace DBAccessModule;

CSqlServerConnection::CSqlServerConnection(void)
{

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

CSqlServerConnection::~CSqlServerConnection(void)
{
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
�ɹ���TURE
ʧ�ܣ�FALSE
Note:		   
**************************************************************************************************/
BOOL	CSqlServerConnection::Open()	
{
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

	// SQL SERVER �����ַ���
	// Provider=SQLOLEDB.1;Password=1234;Persist Security Info=True;User ID=sa;Initial Catalog=C3M_VIDEO_TEST;Data Source=192.168.5.101\SQLEXPRESS
	sprintf_s(szConnectStr,"Provider=SQLOLEDB.1;Password=%s;Persist Security Info=True;User ID=%s;Initial Catalog=%s;Data Source=%s",m_szPass,m_szUser,m_szDatabase,m_szServer);


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
		//	HRESULT hr=m_pConnection->Open(_bstr_t("Provider=SQLOLEDB.1;Password=1234;Persist Security Info=True;User ID=sa;Initial Catalog=C3M_VIDEO_TEST;Data Source=192.168.5.101\SQLEXPRESS"),"","",adModeUnknown);
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

				// �ж������ӳɹ�
				if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
				{					
					std::cout<<"---> DBAccess Error��CSqlServerConnection::Open() DB SQLConnect failed !"<<endl;
					SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); 
					SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
					return FALSE;
				}
				else
				{
					printf( "INFO SqlServer Connect %s@%s success!\n",
						m_szDatabase, m_szUser );
				}
			} 
			else
			{
				std::cout<<"---> DBAccess Error��CSqlServerConnection::Open() DB SQLAllocHandle failed !"<<endl;
				// ����Sql���Ӿ��ʧ��
				SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc); 
				SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
				return FALSE;
			}
		}
		else
		{
			std::cout<<"---> DBAccess Error��CSqlServerConnection::Open() DB SQLSetEnvAttr failed !"<<endl;
			// ����Sql �������ʧ��
			SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
			return FALSE;
		}
	}
	else
	{
		std::cout<<"---> DBAccess Error��CSqlServerConnection::Open() DB SQLAllocHandle failed !"<<endl;
		// ����Sql�������ʧ��
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
		return FALSE;
	}

	m_bConnectFlag = TRUE;
#endif
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
BOOL	CSqlServerConnection::Close()
{
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
BOOL	CSqlServerConnection::IsOpen()
{
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
UINT	CSqlServerConnection::BeginTrans()
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
BOOL	CSqlServerConnection::RollbackTrans()
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
	// 	catch (_com_error e)
	catch(...)
	{
		//	TRACE(_T("Warning: RollbackTrans ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
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
BOOL	CSqlServerConnection::CommitTrans()
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
	// 	catch (_com_error e)
	catch(...)
	{
		//	TRACE(_T("Warning: RollbackTrans ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
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
BOOL			CSqlServerConnection::ExecuteSql(const char*  szSql)
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
					return	TRUE;
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
		}
		catch(...)
		{
			// �ж��Ƿ������ӳ���
			GetConnectionErrorAndDeal();
		}
	}
#else
	//	linux 
	try
	{
		if (!IsOpen())
		{
			Open();
			// ���ش���
			return FALSE;
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
			// ����Ϊ��̬�α�
			//retcode = SQLSetStmtOption(hstmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
			// ִ�гɹ�	// ִ��Sql���
			retcode = SQLExecDirect(hstmt, (SQLCHAR*)szSql, SQL_NTS) ;
			
			// SQL SERVER��ɾ�����ݿ��ʱ����Ȼִ�гɹ�����������ݿ���û�����ݵĻ᷵��SQL_NO_DATA
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO || retcode == SQL_NO_DATA) 
			{
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
				return	TRUE;
			}
			else
			{				
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_hEnv,m_hDbc,hstmt);
				//----------------------------------------------------------------------------------

				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
				return	FALSE;
			}
		}
		else
		{	

			//----------------------------------------------------------------------------------
			// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
			ErrorLogInfo(m_hEnv,m_hDbc,hstmt);
			//----------------------------------------------------------------------------------

			// ����Sql ���ִ�о�������ͷ�Sql��ѯ�����
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
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
Function		: ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
DateTime		: 2010/12/2 10:26	
Description		: ��ҳ��ѯ
Input			: NULL
Output			: NULL
Return			: NULL
Note			:
**************************************************************************************************/
IRecordSet*		CSqlServerConnection::ExecutePageQuery(const char *szSql, const INT32 iStartRow, const INT32 iRowNum)
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
	// select top N * from tablename where id not in (select top M * from tablename) 
	// ���Ӽ������ѯ���ݾͿ�����
	string	strFullSql = "";
	if (GetFullPageQuerySql(szSql,iStartRow,iRowNum,strFullSql))
	{
		// ����һ�����󣬷���β��ʹ��
		CSqlServerRecordSet*		pcsCSqlServerRecordSet = NULL;
		pcsCSqlServerRecordSet = new CSqlServerRecordSet();
		// �������Ӷ���ָ��
		pcsCSqlServerRecordSet->SetConnection(this);

		// ��ѯ���ݼ�
		if (pcsCSqlServerRecordSet->QuerySql(szSql)!=NULL)
		{
			return	pcsCSqlServerRecordSet;
		}
		else
		{
			delete	pcsCSqlServerRecordSet;
			pcsCSqlServerRecordSet = NULL;
			return NULL;
		}
	}
	return	NULL;
}

// ���ҹؼ��֣��ɹ���TRUE��ʧ�ܣ�FALSE �����عؼ���ǰ����ַ���
BOOL	CSqlServerConnection::Find_keyWord_GetString(const char* szSql,const char* szKeyWord,string& strBefore,string& strAfter)
{
	strBefore	=	"";
	strAfter	=	"";

	// char*ת��Ϊstring
	string	strSql		= string(szSql);
	string	strKeyWord	= string(szKeyWord);

	// ת��Ϊ��д
	transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper());
	transform(strKeyWord.begin(),strKeyWord.end(),strKeyWord.begin(),ToUpper());

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


// �����ַ�������
// ���룺SELECT [id],[PerSonNo],[EmployerNo] FROM TB_DEVINFO EMP 
// �����
/* select top 20  [id] ,[PerSonNo],[EmployerNo] from [pub_tbPerSon] where id not in (select top 10 [id] from [pub_tbPerSon] ) */
BOOL		CSqlServerConnection::GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql)
{
	// char*ת��Ϊstring
	string	strSql = string(szSql);

	// strSql���ַ�ȫ��ת��Ϊ��д
	transform (strSql.begin(),strSql.end(), strSql.begin(),ToUpper()); 

	// �����м���ȵ�string����
	string	strTemp = "";

	// ��������Ϣ���ַ���
	stringstream	strInfo;
	strInfo.str("");

	// �ϳ��µ��ַ���
	strInfo << "SELECT ";
	strInfo << "TOP ";
	strInfo << iRowNum;
	strInfo << " ";

	// SELECT�ַ���һ�γ��ֵ�λ��
	string::size_type	n_SELECT_Pos = strSql.find(constrSelectKeyword.c_str(),0);

	// ��ȡSELECT������ַ���
	strSql	=	strSql.substr(n_SELECT_Pos + 6,strSql.size() - n_SELECT_Pos - 6);

	// FROM������szSql�е�λ��
	string::size_type n_FROM_Pos = strSql.find(constrFromKeyWord.c_str(),0);

	// ���ڴ洢WHERE\ORDER\GROUP��szSql��������ֵ�λ��
	string::size_type n_KEYWORD_pos = string::npos;

	// WHERE������szSql�е�λ��
	string::size_type n_WHERE_Pos = strSql.find(constrWhereKeyWord.c_str(),0);

	// GROUP������szSql�е�λ��
	string::size_type n_GROUP_Pos = strSql.find(constrGroupKeyWord.c_str(),0);

	// ORDER������szSql�е�λ��
	string::size_type n_ORDER_Pos = strSql.find(constrOrderKeyword.c_str(),0);

	// �ж�WHERE\ORDER\GROUP��szSql��������ֵ�λ��
	if (string::npos != n_WHERE_Pos)
	{
		n_KEYWORD_pos = n_WHERE_Pos;
	}
	else
	{
		if (string::npos != n_GROUP_Pos)
		{
			n_KEYWORD_pos = n_GROUP_Pos;
		}
		else
		{
			if (string::npos != n_ORDER_Pos)
			{
				n_KEYWORD_pos = n_ORDER_Pos;
			}
		}
	}

	// ���strSql�д���WHERE\ORDER\GROUP������ؼ���
	if (string::npos != n_KEYWORD_pos)
	{
		// ��ȡWHERE\ORDER\GROUP��szSql��������ֵ�λ��֮ǰ���ַ���
		strTemp = strSql.substr(0,n_KEYWORD_pos);

		// ���strTemp�е�����
		strInfo << strTemp;

		// �������WHERE�ؼ���
		if (string::npos != n_WHERE_Pos)
		{
			if(string::npos != n_GROUP_Pos)
			{
				// �������GROUP,��ȡWHERE�ؼ��ֺ�����ж����
				strTemp = strSql.substr(n_WHERE_Pos,n_GROUP_Pos - n_WHERE_Pos);
				strInfo << strTemp;
				strInfo << " AND ID";
			}
			else
			{
				// ���������GROUP�ؼ���
				if (string::npos != n_ORDER_Pos)
				{
					// �������ORDER,��ȡWHERE�ؼ��ֺ�����ж����
					strTemp = strSql.substr(n_WHERE_Pos,n_ORDER_Pos - n_WHERE_Pos);
					strInfo << strTemp;
					strInfo << " AND ID";
				}
				else
				{
					// ���������ORDER,��ȡWHERE�ؼ��ֺ�����ж����
					strTemp = strSql.substr(n_WHERE_Pos,n_ORDER_Pos - n_WHERE_Pos);
					strInfo << strTemp;
					strInfo << " AND ID";
				}
			}
		}
		else
		{
			strInfo << " WHERE ID";
		}

		strInfo << " NOT IN (SELECT TOP ";
		strInfo << iStartRow;
		strInfo << " ID ";

		// szSql��FROM�ؼ��ֳ��ֵ�λ��
		string::size_type	n_FROM_Pos =strSql.find(constrFromKeyWord.c_str());

		// ���strSql�д���GROUP�ؼ���
		if (string::npos != n_GROUP_Pos)
		{
			// ��ȡszSql�д�FROM��GROUP֮����ַ���
			strTemp = strSql.substr(n_FROM_Pos,strSql.length() - n_FROM_Pos);
			strInfo << strTemp;
			strInfo << ")";

			// ��ȡszSql�д�GROUP����β���ַ���
			strTemp = strSql.substr(n_GROUP_Pos,strSql.length() - n_GROUP_Pos);
			strInfo << strTemp;
		}
		// ���strSql�в�����GROUP������GROUP�ؼ���
		else if(string::npos != n_ORDER_Pos)
		{
			// ��ȡszSql�д�FROM��GROUP֮����ַ���
			strTemp = strSql.substr(n_FROM_Pos,strSql.length() - n_FROM_Pos);
			strInfo << strTemp;
			strInfo << ") ";

			// ��ȡszSql�д�ORDER����β���ַ���
			strInfo << " ";
			strTemp = strSql.substr(n_ORDER_Pos,strSql.length() - n_ORDER_Pos);
			strInfo << strTemp;
		}
		// strSql�в�����GROUP��ORDER�ַ�������ֻ����WHERE�ؼ���
		else
		{
			// ��ȡszSql��FROM����β���ַ���
			strTemp = strSql.substr(n_FROM_Pos,strSql.length() - n_FROM_Pos);
			strInfo << strTemp;
			strInfo << ")";
		}
	}
	// strSql�в�����WHERE\ORDER\GROUP�ؼ���
	else
	{
		strTemp = strSql;

		// ���strTemp�е�����
		strInfo << strTemp;

		// ��szSql�н�ȡFROM������ַ���
		string::size_type	n_FROM_Pos =strSql.find(constrFromKeyWord.c_str());
		strTemp = strSql.substr(n_FROM_Pos,strSql.length()-n_FROM_Pos);

		strInfo << " WHERE ID NOT IN (SELECT TOP ";
		strInfo << iStartRow;
		strInfo << " ID ";
		strInfo << strTemp;
		strInfo << ")";
	}

	strFullSql	=	strInfo.str();
	return	TRUE;
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
BOOL			CSqlServerConnection::ExecuteSql(const char*  szSql,const char* szTable,INT64& lRowId)
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
					// Sql ��ѯ���
					// SELECT  ISNULL(MAX(ID),1) AS ID  FROM dbo.VID_TB_TEST_2
					sprintf(szInsertIdSql," SELECT  ISNULL(MAX(ID),1) AS %s  FROM %s",INSERT_ID_FIELED_NAME,szTable);

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

					// �ύ����
					if (bSucFlag)
					{
						return	TRUE;
					}				
				}
				else
				{
					// �ع�����
					return	FALSE;
				}
			}// if (IsOpen())	
			else
			{
				// �����ݿ�����
				Open();
			}
		}
		catch(...)
		{
			// �ж��Ƿ������ӳ���
			GetConnectionErrorAndDeal();
		}
	}
#else
	// linux
	try
	{
		if (!IsOpen())
		{
			Open();
			// ���ش���
			return FALSE;
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
			// ����Ϊ��̬�α�
			//retcode = SQLSetStmtOption(hstmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
			// ִ��Sql���
			// ִ�гɹ�	// ִ��Sql���
			retcode = SQLExecDirect(hstmt, (SQLCHAR*)szSql, SQL_NTS) ;
			
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
			{			
				// �Ƿ����
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);	

				// ����SQL�������ÿ��ִ��SQL��䶼�����������������ִ����ɺ��ͷ�
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &hstmtQuery);	
				if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) 
				{
					//----------------------------------------------------------------------------------
					// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
					ErrorLogInfo(m_hEnv,m_hDbc,hstmtQuery);
					//----------------------------------------------------------------------------------
					SQLFreeHandle(SQL_HANDLE_STMT,	hstmt);	
					SQLFreeHandle(SQL_HANDLE_STMT,	hstmtQuery);
					return FALSE;
				}
				// ��ȡ��Ӱ����еĶ�ӦID
				char	szInsertIdSql[coniSqlStrLen] = {0};
				memset(szInsertIdSql,0x0,coniSqlStrLen);
				// Sql �����÷�
				// SELECT  NVL(MAX(ID),1) AS ID FROM VID_TB_TEST_2
				sprintf(szInsertIdSql," SELECT  ISNULL(MAX(ID),1) AS %s  FROM %s",INSERT_ID_FIELED_NAME,szTable);

				// ָ��Ҫʹ�õ��α겢������
				SQLSetStmtAttr(hstmtQuery, SQL_ATTR_CONCURRENCY,(SQLPOINTER) SQL_CONCUR_ROWVER, 0);
				// ���ù������Ϊ�������,
				SQLSetStmtAttr(hstmtQuery, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC /* SQL_CURSOR_KEYSET_DRIVEN*/, 0);

				// ����Ϊ��̬�α�
				//retcode = SQLSetStmtOption(hstmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
				// ִ��sql���
				retcode = SQLExecDirect(hstmtQuery, (SQLCHAR*)szInsertIdSql, SQL_NTS);

				if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) 
				{
					//----------------------------------------------------------------------------------
					// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
					ErrorLogInfo(m_hEnv,m_hDbc,hstmtQuery);
					//----------------------------------------------------------------------------------
					SQLFreeHandle(SQL_HANDLE_STMT, hstmtQuery);	
					return FALSE;
				}
				else
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
						ErrorLogInfo(m_hEnv,m_hDbc,hstmtQuery);
						//----------------------------------------------------------------------------------
						// ��ȡ����ʧ��
						SQLFreeHandle(SQL_HANDLE_STMT, hstmtQuery);	
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
				ErrorLogInfo(m_hEnv,m_hDbc,hstmt);
				//----------------------------------------------------------------------------------
				// ִ��ʧ��
				SQLFreeHandle(SQL_HANDLE_STMT, hstmt);	
				return FALSE;
			}			
		}
		else
		{	
			// ����Sql ���ִ�о�������ͷ�Sql��ѯ�����
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
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
Function: ExecuteQuery(const char*  szSql)    
DateTime: 2010/5/26 17:18	
Description:    	// �������ܡ����ܵȵ�����
Input:          	ִ�в�����Sql���
Output:         	NULL
Return:         	IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
Note:				
IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
**************************************************************************************************/
IRecordSet*		CSqlServerConnection::ExecuteQuery(const char*  szSql)
{
	// ����һ�����󣬷���β��ʹ��
	CSqlServerRecordSet*		pcsCSqlServerRecordSet = NULL;
	pcsCSqlServerRecordSet = new CSqlServerRecordSet();
	// �������Ӷ���ָ��
	pcsCSqlServerRecordSet->SetConnection(this);

	// ��ѯ���ݼ�
	if (pcsCSqlServerRecordSet->QuerySql(szSql)!=NULL)
	{
		return	pcsCSqlServerRecordSet;
	}
	else
	{
		delete	pcsCSqlServerRecordSet;
		pcsCSqlServerRecordSet = NULL;
		return NULL;
	}
}	

/**************************************************************************************************
  Function		: GetEmptyRecordSet()
  DateTime		: 2010/11/29 17:53	
  Description	: ����һ���յ����ݼ���Addnew���ݵ�ʱ���õ���
  Input			: NULL
  Output		: NULL
  Return		: NULL
  Note			:
**************************************************************************************************/
IRecordSet*		CSqlServerConnection::GetEmptyRecordSet()
{
	CSqlServerRecordSet*		pcsCSqlServerRecordSet = NULL;
	pcsCSqlServerRecordSet = new CSqlServerRecordSet();
	// �������Ӷ���ָ��
	pcsCSqlServerRecordSet->SetConnection(this);

	// ��������� ReleaseRecordSet�����ͷ�
	return	pcsCSqlServerRecordSet;
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
BOOL			CSqlServerConnection::ReleaseConnection() 
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
  DateTime		: 2010/6/11 14:48	
  Description	: ��ȡ�������Ӳ�������Ӧ��������Ҫ�������Ͽ������ݿ�Ͽ��Ȳ�������
  Input			: NULL
  Output		: NULL
  Return		: TRUE:�ɹ�  FALSE:ʧ��
  Note			:
**************************************************************************************************/
BOOL	CSqlServerConnection::GetConnectionErrorAndDeal()
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
		for (long n = 0; n < pErrors->Count; n++)
		{
			ErrorPtr pError = pErrors->GetItem(n);

			// ״̬
			string	strSQLState = string((char*)pError->SQLState);
			// ����
			long NativeError=pError->NativeError;

			long nNumber = pError->GetNumber();
			//pError->Description;
			//pError->Source;

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

