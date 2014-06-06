#include "CSqlServerRecordSet.h"

using	namespace DBAccessModule;

// �α�����
INT		CSqlServerRecordSet::m_iCursorIndex = 0;

CSqlServerRecordSet::CSqlServerRecordSet(void)
{
#if 0
	// �������ݼ�ʵ��
	m_pRecordset = NULL;
	m_pRecordset.CreateInstance("ADODB.Recordset");
#else
	// linux
	m_hSqlStmt = SQL_NULL_HSTMT;
	m_bIsOpen = FALSE;
	// �༭ģʽ
	m_iEditMode = EM_UnKnown;
	// ������
	m_strTableName = constrDefaultString ;

	// �����õ�
	m_hUpdateSqlStmt = SQL_NULL_HSTMT;
	// �Ƿ��Ǽ�¼��β��
	m_bEof = TRUE;
#endif
	m_pConnection = NULL;

}

CSqlServerRecordSet::~CSqlServerRecordSet(void)
{
	if (IsOpen())
	{
		Close();
	}
#if 0
	if (m_pRecordset!=NULL)
	{
		m_pRecordset.Release();
		m_pRecordset = NULL;
	}
#else
	// linux
	if (m_bIsOpen)
	{
		m_bIsOpen = FALSE;
		SQLFreeHandle(SQL_HANDLE_STMT,m_hSqlStmt);
		m_hSqlStmt = SQL_NULL_HSTMT;
	}
	// �����õ�
	if (m_hUpdateSqlStmt != SQL_NULL_HSTMT)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hUpdateSqlStmt);
		m_hUpdateSqlStmt = SQL_NULL_HSTMT;
	}
	m_iEditMode = EM_UnKnown;
	m_bEof = TRUE;

#endif

}



// ---------------------------------------------------------------------------------
// OuterInterface���� 
// ---------------------------------------------------------------------------------
/**************************************************************************************************
Function			:  Eof   
DateTime			: 2010/5/25 11:15	
Description			:    	��ѯ�α��Ƿ����ڼ�¼��β
Input				:          	NULL
Output				:           NULL
Return:         	
TRUE				: ��¼��β��
FALSE:���Ǽ�¼��β��
Note:				
**************************************************************************************************/
BOOL	CSqlServerRecordSet::Eof()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// ��ѯ�α��Ƿ����ڼ�¼��β
	try
	{
		if (m_pRecordset->RecordCount > 0 )
		{
			return m_pRecordset->adoEOF;
		}
		else
		{
			return TRUE;
		}
		
	}
	catch (_com_error e)
	{
		//		TRACE(_T("Warning: IsEOF ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return TRUE;
	}
#else
	// linux 

	m_iEditMode = EM_UnKnown;
	return m_bEof;
#endif
	return	TRUE;

}

/**************************************************************************************************
Function: MoveLast    
DateTime: 2010/5/25 11:22	
Description:    	�������һ����¼
Input:          	NULL
Output:         	NULL
Return:         	
FALSE: ��¼���Ƶ����һ����¼��ʧ��
TRUE: ��¼���Ƶ����һ����¼���ɹ�
Note:				
**************************************************************************************************/
BOOL	CSqlServerRecordSet::MoveLast()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// �ƶ��α��λ�õ����ݼ������һ����¼
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveLast());
		}
	}
	catch (_com_error e)
	{
		//		TRACE(_T("Warning: MoveLast ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 
#else
	// linux
	// ��¼�α��ƶ������һ��λ��
	SQLRETURN	retCode = 0;
	if(m_hSqlStmt != SQL_NULL_HSTMT)
	{
		// �Ƶ����һ����¼
		retCode = SQLFetchScroll(m_hSqlStmt,SQL_FETCH_LAST,0);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO)
		{
			// ����������к����־
			InitCollumnIndexID();

			m_bEof = FALSE;
			return	TRUE;
		}
		else
		{
			m_bEof = TRUE;
		}
	}
	else
	{
		return	FALSE;
	}

#endif

	// �쳣����
	return	FALSE;

}


/**************************************************************************************************
Function:  MoveNext   
DateTime: 2010/5/25 11:30	
Description:      ���ݼ��ƶ�����һ����¼
Input:          	NULL
Output:         	NULL
Return:         	
TRUE  : �ƶ��ɹ�
FALSE : �ƶ�ʧ��
Note:				
**************************************************************************************************/
BOOL	CSqlServerRecordSet::MoveNext()
{
#if 0
	// ���ݼ��������Ч�Լ��
	//	ASSERT(m_pRecordset != NULL);

	// ����ǰ��¼��ǰ�ƶ�һ����¼
	try
	{
		if (m_pRecordset != NULL) 
		{
			return SUCCEEDED(m_pRecordset->MoveNext());
		}
	}
	catch (_com_error e)
	{
		//		TRACE(_T("Warning: MoveNext ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	}
#else
	// linux

	SQLRETURN	retCode = 0;
	if (m_hSqlStmt != SQL_NULL_HSTMT)
	{
		// �ƶ�����һ����¼
		retCode = SQLFetchScroll(m_hSqlStmt, SQL_FETCH_NEXT, 0);
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO)
		{
			// ����������к����־
			InitCollumnIndexID();

			m_bEof = FALSE;
			return	TRUE;
		}
		else
		{
			m_bEof = TRUE;
			return	FALSE;
		}
	}
	else
	{
		// �Ѿ������һ����¼�ˣ�������������ƶ�����
		return	FALSE;
	}

#endif
	// �쳣����
	return FALSE;

}


/**************************************************************************************************
Function: AddNew() 
DateTime: 2010/5/25 11:07	
Description:    	���һ���ռ�¼
Input:          	NULL
Output:         	NULL
Return:         	
TRUE:�ɹ�
FALSE:ʧ��
Note:							
����AddNew()
����PutCollect()
����Update()
**************************************************************************************************/
BOOL    CSqlServerRecordSet::AddNew()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// ���һ���յļ�¼
	try
	{
		if (m_pRecordset != NULL) 
		{
			if (m_pRecordset->AddNew() == S_OK)
			{
				return TRUE;
			}
		}
	}
	catch (_com_error e)
	{
		// 		strError.Format(_T("Warning: AddNew ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 

#else
	// linux

	m_iEditMode = EM_AddNew;

	// ����������к����־
	InitCollumnIndexID();
	ClearFieldInfoList();

	return	TRUE;

#endif
	return FALSE;

}





/**************************************************************************************************
Function		: AddNew(IConnection*	pConnection,const	char*	szTableName)
DateTime		: 2010/11/27 7:47	
Description		: ��һ�������ϣ�ָ���ı������������
Input			: pConnection�����Ӷ���
szTableName		��������
Output			: NULL
Return			: TRUE:�ɹ�
Note			:
**************************************************************************************************/
BOOL	CSqlServerRecordSet::AddNew(const	char*	szTableName)
{
	// ��ʼ������
	Init();

	// ���ò����־
	m_iEditMode = EM_AddNew;

	// ����������к����־
	InitCollumnIndexID();
	ClearFieldInfoList();


	// ���ñ���
	m_strTableName	= string(szTableName);

	return	TRUE;
}



/**************************************************************************************************
Function: Update    
DateTime: 2010/5/25 11:05	
Description:    	�������ݼ�
Input:          	NULL
Output:         	NULL
Return:         	TRUE : �ɹ�
FALSE: ʧ��
Note:				
����AddNew()
����PutCollect()
����Update()
**************************************************************************************************/
BOOL    CSqlServerRecordSet::Update()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// ִ�����ݼ����²���
	try
	{
		if (m_pRecordset != NULL) 
		{
			if (m_pRecordset->Update() == S_OK)
			{
				return TRUE;
			}
		}
	}
	catch (_com_error e)
	{	
		// �������ʧ��,��ȡ������
		CancelUpdate();
		//TRACE(_T("Warning: Update ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return	FALSE;
	}

	// �������ʧ��,��ȡ������
	CancelUpdate();
#else
	// linux

	// �����ַ���
	if (BuildInsertUpdateSqlTxt())
	{
		// ���ֵ
		if (AppendValueToSqlTxt())
		{
			// ִ�в���
			if (ExecuteInsertUpdate())
			{
				// ����������к����־
				InitCollumnIndexID();
				// ����б�
				ClearFieldInfoList();

				return TRUE;
			}
		}
	}
	// ����б�
	ClearFieldInfoList();

	// ����������к����־
	InitCollumnIndexID();

	// �������ʧ��,��ȡ������
	CancelUpdate();
#endif
	return	FALSE;

}

/************************************************************************/
/* OuterInterface����                                                         */
/************************************************************************/

/**************************************************************************************************
Function: GetCollect(const char* szFieldName,string&	szValue)    
DateTime: 2010/5/26 14:53	
Description:    	���������ֶ����ƻ�ȡ�ַ���ֵ
Input:          	strFiledName : �ֶ�����
Output:         	szValue:��ȡ��ֵ
Return:         	
TRUE:�ɹ�
FALSE:ʧ��
Note:				
���������ƻ�ȡ��Ӧ�ֶε�ֵ��
����string �����ַ���(INT64,long long��float,double�ȶ�����string����)
**************************************************************************************************/
BOOL	CSqlServerRecordSet::GetCollect(const char* szFieldName,string&	szValue)
{
#if 0
	//	ASSERT(m_pRecordset != NULL);
	// ��ȡָ���ֶε�ֵ
	try
	{
		// �ж����ݼ��Ƿ��
		if (!IsOpen())
		{
			return FALSE;
		} 
		if (m_pRecordset->adoEOF)
		{
			return FALSE;
		}
		// ��ȡֵ��Ϣ
		_variant_t value = m_pRecordset->GetCollect(_variant_t(szFieldName));
		// ת���ɱ�׼���ַ���
		szValue = string((char*)(_bstr_t)value);

		return TRUE;	
	}
	catch (_com_error e)
	{
		//	TRACE(_T("Warning: �ֶη���ʧ��. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	}
#else
	// linux
	try
	{	
		// ��ǰ�ֶε��ֽڳ���
		SQLLEN		iActualDataSize = 0;
		SQLCHAR			szData[FIELD_DATA_LEN] = {0};
		memset(szData,0x0,FIELD_DATA_LEN);
		SQLRETURN		iRetFlag = 0;

		// ��ȡ������
		SQLSMALLINT		iColumnIndex = GetColumnIndexByName(szFieldName);
		if(iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return FALSE;
		}
		// ��ȡ����
		iRetFlag = SQLGetData(m_hSqlStmt,iColumnIndex,SQL_C_CHAR,szData,FIELD_DATA_LEN,&iActualDataSize);
		// �жϻ�ȡ�����Ƿ�ɹ�
		if (SQL_SUCCEEDED(iRetFlag)) 
		{
			// ������ȴ�С���ԣ����ش���
			if (iActualDataSize > FIELD_DATA_LEN)
			{
				return	FALSE;
			}
			else
			{
				// ��ȡ�����ݳ���С�ڴ���ʵ�ʵ����ݳ���
				// ��ȡ���ݳɹ�
				szValue	=	string((char*)szData);
			}
			return	TRUE;

		}
		else
		{
			// ��ȡ����ʧ��
			return	FALSE;
		}

	}
	catch(...)
	{
		return	FALSE;
	}

#endif
	return FALSE;

}


/**************************************************************************************************
Function: PutCollect(const char* szFieldName,const char*     szValue)    
DateTime: 2010/5/26 15:11	
Description:    	�����������ö�Ӧ�е�ֵ
Input:          	
szFieldName:�ֶ�����
szValue:�ֶ�ֵ
Output:         	NULL
Return:         	
�����ɹ���TRUE
����ʧ�ܣ�FALSE
Note:				
�����ַ���(INT64,long long��float,double�ȶ�����string����)
**************************************************************************************************/
BOOL	CSqlServerRecordSet::PutCollect(const char* szFieldName,const char*     szValue)
{
#if 0
	//	ASSERT(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			_variant_t vt;
			vt.SetString(szValue);

			// ����ֵ
			m_pRecordset->put_Collect(_variant_t(szFieldName), vt);
			return TRUE;
		}
	}
	catch (_com_error e)
	{
		return FALSE;
		//TRACE(_T("Warning: PutCollect ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
	} 
#else
	// linux

	try
	{
		// ��ȡ��Ӧ��������
		INT		iColumnIndex = GenerateCollumnIndexID();/* GetColumnIndexByName(szFieldName);*/
		if (iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return	FALSE;
		}

		// �����ֶ���Ϣ
		CDBFieldInfo*	pcsDbFieldInfoItem = NULL;
		pcsDbFieldInfoItem = new CDBFieldInfo();

		// ����������
		pcsDbFieldInfoItem->SetColumnIndex(iColumnIndex);
		// ����������
		pcsDbFieldInfoItem->SetFieldName(szFieldName);
		// �����ֶ�����
		pcsDbFieldInfoItem->SetFieldType(FT_String);
		// ����ֵ
		pcsDbFieldInfoItem->SetStringValue(szValue);

		// ��ӵ��б���
		if(AddFieldItem2FieldInfoList(pcsDbFieldInfoItem))
		{
			// ���ñ༭ģʽ
			if (m_iEditMode == EM_UnKnown )
			{
				m_iEditMode = EM_Edit;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		return	FALSE;
	}

#endif
	return	FALSE;

}



// �ͷ��������
/**************************************************************************************************
Function		: ReleaseRecordSet    
DateTime		: 2010/6/1 18:50	
Description	: �ͷ����ݼ�����
Input			: NULL
Output		: NULL
Return		: TRUE:�ɹ���FALSE:ʧ��
Note			: ���øú������������Ͳ������ˣ������ڽ��е��ö������ط�����
**************************************************************************************************/
BOOL	CSqlServerRecordSet::ReleaseRecordSet()
{
	delete	this;
	return  TRUE;
}

INT CSqlServerRecordSet::GetColumnNumber(void)
{
	if(m_hSqlStmt == SQL_NULL_HSTMT)
	{
		return	-1;
	}
	return m_ColumnItemList.size();	

}


BOOL CSqlServerRecordSet::GetCloumnName(INT iColIndex, std::string &oStrName )
{
	oStrName.clear();
	if(m_hSqlStmt == SQL_NULL_HSTMT || iColIndex<0 || iColIndex>=(int) m_ColumnItemList.size() )
	{
		return	FALSE;
	}
	oStrName = m_ColumnItemList[iColIndex].strColumn;
	return TRUE;
}

/**************************************************************************************************
Function: GetCollect(const char* szFieldName,INT&	iValue)    
DateTime: 2010/5/26 14:58	
Description:    	���������ƻ�ȡ��Ӧ�ֶε�ֵ������INT �����ַ���
Input:          	szFieldName:�ֶ�����
Output:         	iValue:��ȡ�õ���ֵ
Return:         	
TRUE:�����ɹ�
FALSE:����ʧ��
Note:				
���������ƻ�ȡ��Ӧ�ֶε�ֵ������INT �����ַ���
**************************************************************************************************/
BOOL	CSqlServerRecordSet::GetCollect(const char* szFieldName,INT&	iValue)
{
#if 0
	//	ASSERT(m_pRecordset != NULL);
	try
	{
		// ��ȡֵ
		_variant_t value = m_pRecordset->GetCollect(_variant_t(szFieldName));
		// ��ֵ����ת��
		iValue = atol((char*)(_bstr_t)value);

		return TRUE;
	}
	catch (_com_error e)
	{
		iValue = -1;
		return FALSE;
	} 	
#else
	// linux

	try
	{	
		// ��ǰ�ֶε��ֽڳ���
		SQLLEN		iActualDataSize = 0;
		SQLCHAR			szData[FIELD_DATA_LEN] = {0};
		memset(szData,0x0,FIELD_DATA_LEN);
		SQLRETURN		iRetFlag = 0;

		// ��ȡ������
		SQLSMALLINT		iColumnIndex = GetColumnIndexByName(szFieldName);
		if(iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return FALSE;
		}
		// ��ȡ����
		iRetFlag = SQLGetData(m_hSqlStmt,iColumnIndex,SQL_C_CHAR,szData,FIELD_DATA_LEN,&iActualDataSize);
		if (iRetFlag  != SQL_SUCCESS_WITH_INFO && iRetFlag != SQL_SUCCESS)
		{
			int i = 0;
			SQLCHAR SqlState[SQL_MAX_MESSAGE_LENGTH];
			SQLCHAR Msg[SQL_MAX_MESSAGE_LENGTH];
			memset(SqlState,0x0,SQL_MAX_MESSAGE_LENGTH);
			memset(Msg,0x0,SQL_MAX_MESSAGE_LENGTH);
			SQLINTEGER NativeError = 0;
			SQLSMALLINT MsgLen;
			SQLRETURN retcode = 0;
			while ((retcode = SQLGetDiagRec(SQL_HANDLE_STMT, m_hSqlStmt, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen) != SQL_NO_DATA_FOUND))
			{
				SqlState;
				NativeError;
				Msg;
				++i;
				memset(SqlState,0x0,SQL_MAX_MESSAGE_LENGTH);
				memset(Msg,0x0,SQL_MAX_MESSAGE_LENGTH);
			}
		}

		// �жϻ�ȡ�����Ƿ�ɹ�
		if (SQL_SUCCEEDED(iRetFlag)) 
		{
			// ������ȴ�С���ԣ����ش���
			if (iActualDataSize > FIELD_DATA_LEN)
			{
				return	FALSE;
			}
			else
			{
				// ��ȡ�����ݳ���С�ڴ���ʵ�ʵ����ݳ���
				// ��ȡ���ݳɹ�
				iValue = atol((char*)szData);
			}

			return	TRUE;

		}
		else
		{
			// ��ȡ����ʧ��
			return	FALSE;
		}

	}
	catch(...)
	{
		return	FALSE;
	}

#endif
	return TRUE;

}

/**************************************************************************************************
Function: PutCollect(const char* szFieldName,const INT	iValue)    
DateTime: 2010/5/26 15:08	
Description:    	�����������ö�Ӧ�е�ֵ
Input:          	szFieldName:�ֶ�����
iValue:���õ�����ֵ
Output:			NULL
Return:           TRUE:�ɹ�
FALSE:ʧ��
Note:				
Ҫ��ʹ���ߵ��õ�ʱ�򣬿��ƣ�Ҫ���õ��ֶα�����int���͵Ĳſ���
**************************************************************************************************/
BOOL	CSqlServerRecordSet::PutCollect(const char* szFieldName,const INT	iValue)
{
#if 0
	//	ASSERT(m_pRecordset != NULL);
	try
	{
		if (m_pRecordset != NULL) 
		{
			// ������ת�����ַ�������
			_variant_t vt;
			char	szTemp[20] = {0};
			sprintf(szTemp,"%d",iValue);
			vt.SetString(szTemp);
			// ����ֵ
			m_pRecordset->put_Collect(_variant_t(szFieldName), vt);
			memset(szTemp,0x0,20);
			return TRUE;
		}
	}
	catch (_com_error e)
	{
		return FALSE;
		//TRACE(_T("Warning: PutCollect ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
	} 

#else
	// linux
	try
	{
		// ��ȡ��Ӧ��������
		INT		iColumnIndex = GenerateCollumnIndexID();/*GetColumnIndexByName(szFieldName);*/
		if (iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return	FALSE;
		}

		// �����ֶ���Ϣ
		CDBFieldInfo*	pcsDbFieldInfoItem = NULL;
		pcsDbFieldInfoItem = new CDBFieldInfo();
		// ����������
		pcsDbFieldInfoItem->SetColumnIndex(iColumnIndex);
		// ����������
		pcsDbFieldInfoItem->SetFieldName(szFieldName);
		// �����ֶ�����
		pcsDbFieldInfoItem->SetFieldType(FT_Integer);
		// ����ֵ
		pcsDbFieldInfoItem->SetIntegerValue(iValue);

		// ��ӵ��б���
		if(AddFieldItem2FieldInfoList(pcsDbFieldInfoItem))
		{
			// ���ñ༭ģʽ
			if (m_iEditMode == EM_UnKnown )
			{
				m_iEditMode = EM_Edit;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		return	FALSE;
	}
#endif
	return	FALSE;

}

// �������������ö�Ӧʱ���ֶε�ֵ
BOOL	CSqlServerRecordSet::PutDtCollect(const char* szFieldName,const INT iYear,const INT iMonth,const INT iDay,const INT iHour,const INT iMinute,const INT iSecond)
{
#if 0

#else

	try
	{
		// ��ȡ��Ӧ��������
		INT		iColumnIndex = GenerateCollumnIndexID();
		if (iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return	FALSE;
		}

		// �����ֶ���Ϣ
		CDBFieldInfo*	pcsDbFieldInfoItem = NULL;
		pcsDbFieldInfoItem = new CDBFieldInfo();
		// ����������
		pcsDbFieldInfoItem->SetColumnIndex(iColumnIndex);
		// ����������
		pcsDbFieldInfoItem->SetFieldName(szFieldName);
		// �����ֶ�����
		pcsDbFieldInfoItem->SetFieldType(FT_DateTime);
		// ����ֵ,sql server Ӧ�ÿ���ֱ�����ַ�������ʱ�䣬������оͲο�oracle�ķ���
		char	szDt[64] = {0};
		sprintf(szDt,"%d-%02d-%02d %02d:%02d:%02d",iYear,iMonth,iDay,iHour,iMinute,iSecond);
		pcsDbFieldInfoItem->SetStringValue(szDt);

		// ��ӵ��б���
		if(AddFieldItem2FieldInfoList(pcsDbFieldInfoItem))
		{
			// ���ñ༭ģʽ
			if (m_iEditMode == EM_UnKnown )
			{
				m_iEditMode = EM_Edit;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		return	FALSE;
	}

#endif
	return	FALSE;
}



// ���������ƻ�ȡ��Ӧ�ֶε�ֵ������viod** �����ַ���
/**************************************************************************************************
Function: GetCollect(const char* szFieldName,void**	   pValue,INT&	iDataLen)   
DateTime: 2010/5/26 14:28	
Description:    	���������ƻ�ȡ��Ӧ�ֶε�ֵ������viod** ��������
Input:			strFileName : �ֶ�����	
iBuffSize:	�������Ĵ�С
pValue:	������÷���õ��ڴ��ַ
Output:         
pValue:	����void*���͵�����
iDalaLen:���ݳ���
Return:         	
TRUE:�����ɹ�
FALSE:����ʧ��
Note:				���ڲ��������ƴ����
**************************************************************************************************/
BOOL	CSqlServerRecordSet::GetCollect(const char* szFieldName,void*	   pValue,const INT iBuffSize,INT&	iDataLen)
{
#if 0

	// ��ȡ�ֶμ�ָ��
	FieldPtr		pField = GetField(szFieldName);
	if (pField == NULL)
	{
		return	FALSE;
	}

	// ���ݳ��ȴ�С
	long lDataSize = pField->ActualSize;

	// ������������
	//	UCHAR chData;
	long index = 0;

	// �����ݴ��ڣ��������������ڴ�ռ���������ڴ�ռ�
	if (index < lDataSize && iBuffSize > iDataLen)
	{ 
		try
		{
			// һ��ȫ��ȡ����
			_variant_t varChunk = pField->GetChunk(lDataSize);

			// �ж��������� :�ж������Ƿ�ΪBYTE���͵����� 
			if (varChunk.vt != (VT_ARRAY | VT_UI1))
			{
				return FALSE;
			}

			char *pBuf = NULL;
			// ��ȡ����
			HRESULT	hr2 = SafeArrayAccessData(varChunk.parray,(void **)&pBuf);
			if (SUCCEEDED(hr2))
			{
				// ��������
				memcpy(pValue,pBuf,lDataSize);				
				// �Ƿ��ڴ�
				SafeArrayUnaccessData (varChunk.parray);
			}
			else
			{
				FALSE;
			}

			// �������ݳ���
			iDataLen = lDataSize;

			// 			for (long i = 0; i < lDataSize; i++)
			// 			{
			// 				HRESULT hr = SafeArrayGetElement(varChunk.parray, &i, &chData);
			// 				if (SUCCEEDED(hr))
			// 				{
			// 					((UCHAR*)pValue)[index] = chData;
			// 					index++;
			// 				}
			// 				else
			// 				{
			// 					break;
			// 				}
			// 			}
		}
		catch (_com_error e)
		{			
			return FALSE;
		}
	}
	else
	{
		// ���س���
		iDataLen = lDataSize;
		// ���Ȳ�����
		return FALSE;
	}
#else
	// linux

	// �������ݴ洢��
	SQLCHAR*		pszData = NULL;
	try
	{	
		// ��ǰ�ֶε��ֽڳ���
		SQLLEN		iActualDataSize = 0;

		// �����ڴ�
		pszData = new SQLCHAR[iBuffSize + 1];
		memset(pszData,0x0,iBuffSize+1);
		SQLRETURN		iRetFlag = 0;

		// ��ȡ������
		SQLSMALLINT		iColumnIndex = GetColumnIndexByName(szFieldName);
		if(iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return FALSE;
		}
		// ��ȡ����
		iRetFlag = SQLGetData(m_hSqlStmt,iColumnIndex,SQL_C_BINARY,pszData,iBuffSize,&iActualDataSize);
		// �жϻ�ȡ�����Ƿ�ɹ�
		if (SQL_SUCCEEDED(iRetFlag)) 
		{
			if (iBuffSize < iActualDataSize)
			{
				// ������С���������ݳ��ȣ�ֻ���ش�С
				iDataLen = iActualDataSize;
			}
			else if (iActualDataSize <=0)
			{
				iDataLen = coniZero;
			}
			else
			{
				// �������ݣ����ҷ��ش�С
				memcpy(pValue,pszData,iActualDataSize);
				iDataLen = iActualDataSize;
			}

		}
		else
		{
			if (pszData != NULL)
			{
				delete[] pszData;
				pszData = NULL;
			}
			// ��ȡ����ʧ��
			return	FALSE;
		}

		if (pszData != NULL)
		{
			delete[] pszData;
			pszData = NULL;
		}

	}
	catch(...)
	{
		if (pszData != NULL)
		{
			delete[] pszData;
			pszData = NULL;
		}
		return	FALSE;
	}
#endif
	return TRUE;

}

/**************************************************************************************************
Function: PutCollect(const char* szFieldName,const  void*	   pValue,const INT	iDataLen)    
DateTime: 2010/5/26 14:14	
Description:    	�����ֶ�����������Ӧ��ֵ
Input:          	
strFileName:
pValue:
iDataLen:
Output:         	NULL
Return:         
TRUE: �ɹ�
FALSE:ʧ��
Note:				���ڲ��������ƴ����
**************************************************************************************************/
BOOL	CSqlServerRecordSet::PutCollect(const char* szFieldName,const  void*	   pValue,const INT	iDataLen)
{
#if 0
	// ��ȡ�ֶμ�ָ��
	FieldPtr		pField = GetField(szFieldName);
	if (pField == NULL)
	{
		return	FALSE;
	}

	// ���� SAFEARRAY ��  SAFEARRAYBOUND �����в���
	SAFEARRAY FAR *pSafeArray = NULL;
	SAFEARRAYBOUND rgsabound[1];

	try
	{
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = iDataLen;
		pSafeArray = SafeArrayCreate(VT_UI1, 1, rgsabound);

		// ����ת��
		char*		pBuff	=	(char*)pValue;

		for (long i = 0; i < (long)iDataLen; i++)
		{
			// д������
			HRESULT hr = SafeArrayPutElement(pSafeArray, &i, pBuff++);
			if (FAILED(hr))	
			{
				return FALSE;
			}
		}

		_variant_t varChunk;
		///��varChunk����������ΪBYTE���͵����� 
		varChunk.vt = VT_ARRAY | VT_UI1;
		varChunk.parray = pSafeArray;

		return (pField->AppendChunk(varChunk) == S_OK);
	}
	catch (_com_error e)
	{
		//TRACE(_T("Warning: AppendChunk ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	}
#else
	// linux

	try
	{
		// ��ȡ��Ӧ��������
		INT		iColumnIndex = GenerateCollumnIndexID();/*GetColumnIndexByName(szFieldName);*/
		if (iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return	FALSE;
		}

		// �����ֶ���Ϣ
		CDBFieldInfo*	pcsDbFieldInfoItem = NULL;
		pcsDbFieldInfoItem = new CDBFieldInfo();
		// ����������
		pcsDbFieldInfoItem->SetColumnIndex(iColumnIndex);
		// ����������
		pcsDbFieldInfoItem->SetFieldName(szFieldName);
		// �����ֶ�����
		pcsDbFieldInfoItem->SetFieldType(FT_Binary);
		// ���ö��������ݵĳ���
		pcsDbFieldInfoItem->SetBinaryValueLen(iDataLen);
		// ���ö���������ֵ(���ݵ�ֵ�ʹ�С)
		pcsDbFieldInfoItem->SetBinaryValue(pValue,iDataLen);

		// ��ӵ��б���
		if(AddFieldItem2FieldInfoList(pcsDbFieldInfoItem))
		{
			// ���ñ༭ģʽ
			if (m_iEditMode == EM_UnKnown )
			{
				m_iEditMode = EM_Edit;
			}
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch(...)
	{
		return	FALSE;
	}

#endif
	return TRUE;

}


// ---------------------------------------------------------------------------------
// �ڲ��ӿڲ���  
// ---------------------------------------------------------------------------------
/**************************************************************************************************
Function: QuerySql    
DateTime: 2010/5/25 19:11	
Description:    	ִ��Sql��䷵�����ݼ�
Input:          	��׼Sql���
Output:         	NULL
Return:         	
IRecordSet* �������ݼ�����
Note:				ִ��SELECT��ѯ����
**************************************************************************************************/
IRecordSet*		CSqlServerRecordSet::QuerySql(const char*	szSql)
{	
	//	ASSERT(m_pRecordset != NULL)
	// ִ�в����������ݼ�����
	if (Open(szSql))
	{
		return (IRecordSet*)this;
	}
	else
	{
		return	NULL;
	}

}

// ---------------------------------------------------------------------------------
// �ڲ�����
// ---------------------------------------------------------------------------------
/**************************************************************************************************
Function:     
DateTime: 2010/5/25 10:58	
Description:    	��ȡ���ݼ��ж��Ƿ���Խ��б༭
Input:          	NULL
Output:         	NULL
Return:         
TRUE:	���Ա༭
FALSE:  �����Ա༭

Note:				NULL
**************************************************************************************************/
BOOL		CSqlServerRecordSet::GetEditMode()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// ��ȡ��¼���ı༭ģʽ
	try
	{
		if (m_pRecordset != NULL) 
		{
			return m_pRecordset->GetEditMode() == adEditNone ? TRUE : FALSE;
		}
	}
	catch (_com_error e)
	{
		//	TRACE(_T("Warning: UpdateBatch ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 
#else
	//linux
#endif
	return	FALSE; 

}


/**************************************************************************************************
Function: SetConnection    
DateTime: 2010/5/25 10:21	
Description:    	�������Ӷ���
Input:          	pConnection : ���Ӷ���
Output:         	NULL
Return:         	// ��������ֵ��˵��
Note:				// ��ע
**************************************************************************************************/
BOOL			CSqlServerRecordSet::SetConnection(CSqlServerConnection* pConnection)
{
	if (pConnection != NULL)
	{
		m_pConnection	=	pConnection;
		return	TRUE;
	}
	return FALSE;
}

/**************************************************************************************************
Function: CancelUpdate    
DateTime: 2010/5/25 10:57	
Description:		ȡ���ڵ��� Update ����ǰ�Ե�ǰ��¼���¼�¼�������κθ���.
Input:          	NULL
Output:         	NULL
Return:         	�ɹ���TRUE ʧ�ܣ�FALSE
Note:				
**************************************************************************************************/
BOOL	CSqlServerRecordSet::CancelUpdate()
{
#if 0
	//	ASSERT(m_pRecordset != NULL);

	// ȡ������
	try
	{
		if (m_pRecordset != NULL) 
		{
			if (GetEditMode() || m_pRecordset->CancelUpdate() == S_OK)
			{
				return TRUE;
			}
		}
	}
	catch (_com_error e)
	{
		//	TRACE(_T("Warning: CancelUpdate ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 
#else
	//linux
	ClearFieldInfoList();	

	// ���ñ༭ģʽΪUnKnown
	m_iEditMode = EM_UnKnown;

	// �ͷŸ��¾��
	if (m_hUpdateSqlStmt!=SQL_NULL_HSTMT)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hUpdateSqlStmt);
		m_hUpdateSqlStmt = SQL_NULL_HSTMT;
	}
#endif
	// �쳣����
	return FALSE;

}


/**************************************************************************************************
Function: IsOpen    
DateTime: 2010/5/25 17:23	
Description:    	�ж����ݼ��Ƿ��
Input:          	NULL
Output:         	NULL
Return:         	
TRUE:��
FALSE:�ر�
Note:				// ��ע
**************************************************************************************************/
BOOL	CSqlServerRecordSet::IsOpen()
{
#if 0
	try
	{
		if (m_pRecordset != NULL)
		{
			// �򿪵����
			if ( m_pRecordset->GetState() & adStateOpen)
			{
				return	TRUE;
			}			
		}
	}
	catch (_com_error e)
	{
		//	TRACE(_T("Warning: IsOpen���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return FALSE;
	} 
#else
	// linux
	return	m_bIsOpen;
#endif
	return FALSE;

}

/**************************************************************************************************
Function: Close    
DateTime: 2010/5/25 17:33	
Description:   �ر����ݼ�
Input:         NULL
Output:        NULL
Return:        
TRUE: �رճɹ�
FALSE: �ر�ʧ��
Note:		
**************************************************************************************************/
BOOL	CSqlServerRecordSet::Close()
{
#if 0
	// �رմ��ڴ�״̬�����ݼ�����
	try
	{
		if (m_pRecordset != NULL && m_pRecordset->State != adStateClosed)
		{
			if (GetEditMode() == adEditNone)
			{
				CancelUpdate();
			}

			m_pRecordset->Close();
		}
	}
	catch (_com_error e)
	{
		// TRACE(_T("Warning: Close ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return	FALSE;
	}
#else 
	//linux
	if (IsOpen())
	{
		m_bIsOpen = FALSE;
		SQLFreeHandle(SQL_HANDLE_STMT,m_hSqlStmt);
		m_hSqlStmt = SQL_NULL_HSTMT;
	}
#endif
	return	TRUE;

}

/**************************************************************************************************
Function: Open    
DateTime: 2010/5/25 17:34	
Description:   �����ݼ�
Input:         ��׼Sql���
Output:        NULL
Return:        
TRUE: �򿪳ɹ�
FALSE:��ʧ��
Note:			
**************************************************************************************************/
BOOL	CSqlServerRecordSet::Open(const char*  szSql)
{
#if 0
	// ���Ӷ�������ݼ��������Ч�Լ��
	if (m_pConnection == NULL || m_pRecordset == NULL)
	{
		return FALSE;
	}

	// �����ݼ�����
	try
	{
		// �ر����ݼ�����
		if (IsOpen()) 
		{
			Close();
		}

		// ��չ�����
		m_pRecordset->PutFilter("");

		// ���´����ݼ����� , ����˵�����ο�ADO�������ݿ�
		m_pRecordset->CursorLocation	=	adUseClient;
		HRESULT hr = m_pRecordset->Open(_variant_t(szSql),_variant_t((IDispatch*)m_pConnection->GetConnection(), true),adOpenStatic, adLockOptimistic, adCmdText);

		// �����ݼ�ʧ��
		if(!SUCCEEDED(hr))   
		{
			return FALSE;
		}

		// �ƶ����һ��
		if (m_pRecordset->RecordCount > 0)
		{
			m_pRecordset->MoveFirst();
		}
	}
	catch (_com_error e)
	{
		/*
		TRACE(_T("Warning: �򿪼�¼�������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		TRACE(_T("%s\r\n"), GetLastError());
		*/
		// ����Ͽ������ˣ�Ҫ������������
		m_pConnection->GetConnectionErrorAndDeal();
		return FALSE;
	}
#else
	//linux
	// �����ݼ�����
	try
	{
		// �ر����ݼ�����
		if (IsOpen()) 
		{
			Close();
		}

		// �ж�׼�������Ƿ�ɹ�
		if (Init()) 
		{	
			// ִ��sql���
			SQLINTEGER	iRetCode = -1;
			// ����Ϊ��̬�α�
			//iRetCode = SQLSetStmtOption(m_hSqlStmt, SQL_CURSOR_TYPE, SQL_CURSOR_STATIC);
			iRetCode	=	SQLExecDirect(m_hSqlStmt, (SQLCHAR*)szSql, SQL_NTS);
			if ( iRetCode == SQL_ERROR)
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------

				Close();
				return FALSE;
			}
			else
			{
				m_strTableName = constrDefaultString ;
				// �Ѿ�ִ�гɹ��������ݼ���
				m_bIsOpen = TRUE;
				// ��ȡ���������Ϣ�����б�
				GetColumnList();
				// ����ƶ�����һ����¼
				iRetCode = SQLFetchScroll(m_hSqlStmt, SQL_FETCH_FIRST, 0);
				// �ж��Ƿ��м�¼����
				if((iRetCode != SQL_SUCCESS) && (iRetCode != SQL_SUCCESS_WITH_INFO))
				{
					m_bEof = TRUE;
				}
				else
				{
					m_bEof = FALSE;
				}
				// ��Sql�ַ�������ȡ��������
				if (!GetTableNameFromSqlText(szSql,m_strTableName))
				{
					// ����Ҳ��������ƣ��Ͳ��ܹ����£���ӵȲ���
					m_iEditMode = EM_NoUpdate;
				}

				// ����������к����־
				InitCollumnIndexID();

				return  TRUE;		
			}
		}
		else
		{	
			return FALSE;	
		}

	}
	catch (...)
	{
		// ����Ͽ������ˣ�Ҫ������������
		m_pConnection->GetConnectionErrorAndDeal();

		m_bIsOpen = FALSE;
		return FALSE;
	}
#endif
	return TRUE;

}



//------------------------------------------------------------------------------------------------------
// �ֶμ��Ļ�ȡ
#if 0

/**************************************************************************************************
Function: GetFields   
DateTime: 2010/5/26 13:57	
Description:    	�ֶμ��Ļ�ȡ
Input:          	NULL
Output:         	NULL
Return:         	FieldsPtr : ���͵��ֶμ�
Note:				
**************************************************************************************************/
FieldsPtr CSqlServerRecordSet::GetFields()
{
	//	ASSERT(m_pRecordset != NULL);
	try
	{
		return m_pRecordset->GetFields();
	}
	catch (_com_error e)
	{
		//TRACE(_T("Warning: GetFields ���������쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return NULL;
	} 
	return NULL;

}


/**************************************************************************************************
Function:GetField     
DateTime: 2010/5/26 13:57	
Description:  �ֶμ��Ļ�ȡ
Input:        �ֶ�����
Output:       NULL
Return:       FieldsPtr : ���͵��ֶμ�
Note:		
**************************************************************************************************/
FieldPtr  CSqlServerRecordSet::GetField(const char* szFieldName)
{
	try
	{
		return GetFields()->GetItem(_variant_t(szFieldName));
	}
	catch (_com_error e)
	{
		//	TRACE(_T("Warning: GetField�����쳣. ������Ϣ: %s; �ļ�: %s; ��: %d\n"), e.ErrorMessage(), __FILE__, __LINE__);
		return NULL;
	}

}
#else
// linux

// --------------------------------------------------------------------------------------------
// ��ʼ�������Ϣ
/**************************************************************************************************
Function		: Init()    
DateTime		: 2010/6/21 20:14	
Description		: ��ʼ��STMT��������Ϣ
Input			: NULL
Output			: NULL
Return			: TRUE :�ɹ�  FALSE : ʧ��
Note			��OPEN�����е���
**************************************************************************************************/
BOOL		CSqlServerRecordSet::Init()
{
	try
	{ 
		// ���жϾ���Ƿ����
		if ( (m_pConnection->GetOdbcSqlHDbc() == SQL_NULL_HANDLE ) || (!m_pConnection->IsOpen()))
		{
			return FALSE;
		}

		SQLRETURN	retCode = 0;
		//����SQL�����
		retCode = SQLAllocHandle( SQL_HANDLE_STMT,m_pConnection->GetOdbcSqlHDbc(),&m_hSqlStmt);

		// �ж��Ƿ�����ɹ�
		if (retCode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO) 
		{	
			//ָ��Ҫʹ�õ��α겢������
			retCode = SQLSetStmtAttr(m_hSqlStmt, SQL_ATTR_CONCURRENCY,(SQLPOINTER) SQL_CONCUR_ROWVER, 0);
			if ( (retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO)) 
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------
				Close(); 
				return FALSE;
			}


			//���ù������Ϊ�������,
			//��������ܹ���⵽�е�ɾ�����޸ģ������޷���⵽��⵽�е���Ӻͽ����˳��仯��
			//��Ϊ�ڹ�괴��ʱ�ʹ������������������������м�¼��˳���Ѿ����̶���
			//��һ��;�̬���һ�������Լ���������˵��һ�ֽ��ھ�̬���Ͷ�̬���֮��Ĺ�����͡�
			retCode = SQLSetStmtAttr(m_hSqlStmt, SQL_ATTR_CURSOR_TYPE, (SQLPOINTER)SQL_CURSOR_DYNAMIC /* SQL_CURSOR_KEYSET_DRIVEN*/, 0);
			if ( (retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO)) 
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------

				Close(); 
				return FALSE;
			}

			// ��������

			/*			// ���÷���-1��֪��Ϊɶ
			retCode = SQLSetStmtAttr(m_hSqlStmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)coniOpenTimeOut, SQL_IS_UINTEGER);
			if ( (retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO)) 
			{
			Close(); 
			return FALSE;
			}
			*/
			// �����α�����
			char		szCursorName[MAX_CURSOR_NAME_LEN] = {0x0};
			sprintf(szCursorName,"%s_%d",CURRENT_CURSOR_NAME,GetCursorIndex());
			retCode = SQLSetCursorName(m_hSqlStmt,reinterpret_cast<SQLCHAR*>(szCursorName), SQL_NTS);
			if ( (retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO)) 
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------

				Close(); 
				return FALSE;
			}
		}
		else
		{
			Close();
			return FALSE;
		}
	}
	catch (...)
	{
		Close();
		return FALSE;
	}

	return TRUE;
}

//---------------------------------------------------------------------------------------------
// ��ȡֵ��ظ�������

/**************************************************************************************************
Function		: GetColumnList()    
DateTime		: 2010/6/19 14:52	
Description		: ��ȡ�ֶ������б�
Input			: NULL
Output			: NULL
Return			: �ɹ�����TRUE,ʧ�ܷ���FALSE
Note			: NULL
**************************************************************************************************/
BOOL		CSqlServerRecordSet::GetColumnList()
{
	if (m_hSqlStmt == SQL_NULL_HSTMT)
	{
		return FALSE;
	}
	m_ColumnItemList.clear();

	// ����
	SQLSMALLINT		iColumnNum = 0;
	// ��ȡ������
	SQLNumResultCols(m_hSqlStmt, &iColumnNum);
	SQLCHAR			szColumnName[MAX_FNAME_LEN] = {0};


	int nType = SQL_C_DEFAULT;
	// ��Щ���ݣ�����������У����ǲ��ù���
	SQLSMALLINT		nSwCol=0, nSwType=0, nSwScale=0, nSwNull=0; 
	SQLULEN		pcbColDef=0;
	SQLRETURN		iRet=0;

	// ��ȡ����
	for(INT	i = 1;i<= iColumnNum ;i++)
	{
		// ����
		memset(szColumnName,0x0,MAX_FNAME_LEN);
		// ��ȡ�е������Ϣ
		iRet = SQLDescribeCol(m_hSqlStmt, i,(SQLTCHAR*)szColumnName,MAX_FNAME_LEN,&nSwCol, &nSwType, &pcbColDef,&nSwScale, &nSwNull); 
		if (iRet == SQL_SUCCESS || iRet == SQL_SUCCESS_WITH_INFO)
		{
			// ��ȡ��������Ϣ
			StruColumnItem		stColumnItem;
			stColumnItem.strColumn = string((char*)szColumnName);
			stColumnItem.iColumnIndex = i;

			// �������
			m_ColumnItemList.push_back(stColumnItem);
		}
		else
		{
			// �ر�
			Close();
			return FALSE;
		}
	}

	return	TRUE;

}

/**************************************************************************************************
Function		: ClearColumnList()    
DateTime		: 2010/6/19 14:51	
Description		: ����ֶ��б�
Input			: NULL
Output			: NULL
Return			: TRUE
Note			: NULL
**************************************************************************************************/
BOOL		CSqlServerRecordSet::ClearColumnList()
{
	m_ColumnItemList.clear();
	return	TRUE;

}


/**************************************************************************************************
Function		: GetColumnIndexByName(const char* szFieldName)   
DateTime		: 2010/6/19 11:03	
Description		: �����ֶ����ƣ���ȡ�е�����
Input			: �ֶ�����
Output			: NULL
Return			: �е�����
Note			: ��ȡʧ�ܷ��� ERROR_SQL_COLUMN_INDEX
**************************************************************************************************/
INT		  CSqlServerRecordSet::GetColumnIndexByName(const char* szFieldName)
{
	if(m_hSqlStmt == SQL_NULL_HSTMT)
	{
		return	ERROR_SQL_COLUMN_INDEX;
	}

	INT		iColumnIndex = ERROR_SQL_COLUMN_INDEX;
	// �����ַ���
	string	strColumnName = string(szFieldName);
	// ��ȡ�����ƶ�Ӧ����������
	for (ColumnItemVector::size_type i = 0;i< m_ColumnItemList.size();i++)
	{
		if(strColumnName == m_ColumnItemList[i].strColumn)
		{
			iColumnIndex = m_ColumnItemList[i].iColumnIndex;
			break;
		}
	}

	// �����е�����
	return	iColumnIndex;
}

//---------------------------------------------------------------------------------------------
// ����ֵ��ظ�������

/**************************************************************************************************
Function		: ClearFieldInfoList()    
DateTime		: 2010/6/20 18:20	
Description		: ��������ֶ��б�m_FieldInfoList �� AddNew �� Update��ʱ���õģ�
Input			: NULL
Output			: NULL
Return			: TRUE
Note			:��m_FieldInfoList �� AddNew �� Update��ʱ���õģ�	
**************************************************************************************************/
BOOL		CSqlServerRecordSet::ClearFieldInfoList()
{
	// ����ֶ��б�
	for (DBFieldInfoVector::size_type i = 0;i<m_FieldInfoList.size();i++)
	{
		// ����ڴ�
		CDBFieldInfo*	pDbFieldInfo = NULL;
		pDbFieldInfo = m_FieldInfoList[i];
		if (pDbFieldInfo!=NULL)
		{
			pDbFieldInfo->Clear();
			delete	pDbFieldInfo;
			pDbFieldInfo = NULL;
		}
	}
	m_FieldInfoList.clear();

	return TRUE;

}


// ���ֶ��б�������ֶ���Ϣ
BOOL		CSqlServerRecordSet::AddFieldItem2FieldInfoList(CDBFieldInfo*	pcsDbFieldInfoItem)
{	
	// ���ж������Ƿ����
	for (DBFieldInfoVector::size_type i = 0 ;i <m_FieldInfoList.size();i++)
	{
		if (m_FieldInfoList[i]->GetFieldName() == pcsDbFieldInfoItem->GetFieldName())
		{
			// �Ѿ����ھͷ���
			return	FALSE;
		}
	}

	// ��ӵ��б���
	m_FieldInfoList.push_back(pcsDbFieldInfoItem);
	return	TRUE;

}

/**************************************************************************************************
Function		: GetTableNameFromSqlText(const char* szSql)    
DateTime		: 2010/6/20 9:54	
Description		: ���ݲ�ѯ���ݵ�sql�������ȡ���еı�����(��Ҫ�ǶԱ��ѯ�󣬽��в������ݺ͸������ݵ�ʱ����)
Input			: Sql ��ѯ��䣬���磺SELECT * from TB_TEST;
Output			: strTableName:�����ı�����
Return			: TRUE:�ɹ���FALSE:ʧ��
Note			: �����ݼ��ĵط�Ҫʹ�ã������ݼ��Ķ�Ӧ�ı����ƴ������������ڸ��ºͲ���ʱ����
**************************************************************************************************/
BOOL		CSqlServerRecordSet::GetTableNameFromSqlText(const char* szSql,string& strTableName)
{
	try
	{
		string	strSql  = string(szSql);
		// ���������һ��from������ַ���

		// ת��Ϊ��д
		// transform (strSql.begin(),strSql.end(), strSql.begin(), toupper); 
		transform(strSql.begin(),strSql.end(),strSql.begin(),ToUpper());

		// FROM �ַ����ֵ�λ��
		string::size_type	nFromPos = strSql.find(constrFromKeyWord.c_str());

		// û���ҵ�
		if (nFromPos == string::npos)
		{
			return	FALSE;
		}

		// ȥ��FROM��ǰ���ַ���
		strSql	=	strSql.substr(nFromPos+4,strSql.size() - nFromPos -4);

		// ɾ������ǰ��Ŀո��ַ���
		strSql.erase(0,strSql.find_first_not_of(" "));
		// ȥ������Ŀո�
		strSql.erase(strSql.find_last_not_of(" ") + 1,strSql.size()-1);

		// ��ʼ���ұ����� "   TABLE "����" TABLE"��
		// �����ƽ���λ��
		string::size_type	nTableTailPos  = strSql.find_first_of(" ");
		if (nTableTailPos == string::npos)
		{
			// ��������û�пո�
			strTableName = strSql;
		}
		else
		{
			// ��ȡ����
			strTableName	=	strSql.substr(0,nTableTailPos);
		}

	}
	catch(...)
	{
		return	FALSE;
	}

	return	TRUE;
}


/**************************************************************************************************
Function		: BuildInsertUpdateSqlTxt()    
DateTime		: 2010/6/20 21:04	
Description		: ����Sql�����в���
Input			: NULL
Output			: NULL
Return			: TRUE: �ɹ� ��FALSE :ʧ��
Note			: 
**************************************************************************************************/
BOOL		CSqlServerRecordSet::BuildInsertUpdateSqlTxt()
{
	// ȷ����Ԫ�ش���
	if (m_FieldInfoList.empty())
	{
		return FALSE;
	}

	if (m_hUpdateSqlStmt != SQL_NULL_HSTMT)
	{
		SQLFreeHandle(SQL_HANDLE_STMT,m_hUpdateSqlStmt);
		m_hUpdateSqlStmt = SQL_NULL_HSTMT;
	}

	// ���ؾ��
	SQLRETURN retcode;
	// ����Sql�����
	retcode = SQLAllocHandle(SQL_HANDLE_STMT,m_pConnection->GetOdbcSqlHDbc(),&m_hUpdateSqlStmt);	
	// �ж��Ƿ�����ɹ�
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{	
		// ����ַ���
		m_strSqlText = constrDefaultString;

		// �ж�����
		if (m_iEditMode == EM_AddNew)
		{
			// INSERT ����
			m_strSqlText = "INSERT INTO ";
			m_strSqlText += m_strTableName;
			m_strSqlText += " ( ";
			// ������Ƽ���
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				m_strSqlText += m_FieldInfoList[i]->GetFieldName();
				if (i!=m_FieldInfoList.size() - 1)
				{
					m_strSqlText += ",";
				}
				else
				{
					m_strSqlText += " ";
				}
			}

			m_strSqlText += " ) VALUES (";

			// ����ֵ
			for (DBFieldInfoVector::size_type j=0;j<m_FieldInfoList.size();j++)
			{
				m_strSqlText += "?";
				if (j!=m_FieldInfoList.size() - 1)
				{
					m_strSqlText += ",";
				}
				else
				{
					m_strSqlText += ")";
				}
			}

			// ���磺 INSERT INTO TB_TEST(ID,NAME,KK) VALUES(?,?,?)

			// ׼��Sql ���
			SQLRETURN	retCode = 0;
			retcode = SQLPrepare(m_hUpdateSqlStmt,(SQLCHAR*)m_strSqlText.c_str(),SQL_NTS);
			if (retcode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO)
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------

				return TRUE;
			}
		}
		else if (m_iEditMode == EM_Edit)
		{
			// UPDATE ����
			m_strSqlText = "UPDATE ";
			m_strSqlText += m_strTableName;
			m_strSqlText += " Set ";

			// ��������
			// ������Ƽ���
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				m_strSqlText += m_FieldInfoList[i]->GetFieldName();
				m_strSqlText += " = ? ";
				if (i!=m_FieldInfoList.size() - 1)
				{
					m_strSqlText += ",";
				}
				else
				{
					m_strSqlText += " ";
				}
			}

			// ������������,ͨ���α�������
			// m_strSqlText += " WHERE CURRENT OF CURRENT_CURSOR_NAME";

			m_strSqlText += " WHERE CURRENT OF ";
			SQLRETURN	nRetCode = 0;
			unsigned char	szCursorName[MAX_CURSOR_NAME_LEN+1] = {0};
			memset(szCursorName,0x0,MAX_CURSOR_NAME_LEN+1);
			SQLSMALLINT			iLenght = 0;
			nRetCode	=	SQLGetCursorName(m_hSqlStmt,szCursorName,MAX_CURSOR_NAME_LEN,&iLenght);
			if (nRetCode == SQL_SUCCESS || nRetCode == SQL_SUCCESS_WITH_INFO)
			{
				string	strTemp = (char*)szCursorName;
				m_strSqlText	+= strTemp;
			}
			else
			{
				return FALSE;
			}

			// ���磺 UPDATE TB_TEST SET NAME = ? ,PASS= ? WHERE KK = 'E'

			// ׼��Sql ���
			SQLRETURN	retCode = 0;
			retcode = SQLPrepare(m_hUpdateSqlStmt,(SQLCHAR*)m_strSqlText.c_str(),SQL_NTS);
			if (retcode == SQL_SUCCESS || retCode == SQL_SUCCESS_WITH_INFO)
			{
				//----------------------------------------------------------------------------------
				// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
				ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
				//----------------------------------------------------------------------------------

				return TRUE;
			}
		}
		else
		{	
			// ֻ�������ºͲ���ģ����״̬��Ϊ����
			SQLFreeHandle(SQL_HANDLE_STMT,m_hUpdateSqlStmt);
			m_hUpdateSqlStmt = SQL_NULL_HSTMT;

			return FALSE;
		}
	}
	else
	{	
		// ����Sql�����ʧ��
		SQLFreeHandle(SQL_HANDLE_STMT, m_hSqlStmt);
		return FALSE;	
	}

	return	FALSE;

}


/**************************************************************************************************
Function		: AppendValueToSqlTxt()    
DateTime		: 2010/6/20 23:09	
Description		: ��Sql���������Ӷ�Ӧ��ֵ
Input			: NULL
Output			: NULL
Return			: TRUE:�ɹ���FALSE:ʧ��
Note			: ǰ��������BuildInsertUpdateSqlTxt()ִ�гɹ�
�����������ExecuteInsertUpdate()��ִ��
**************************************************************************************************/
BOOL		CSqlServerRecordSet::AppendValueToSqlTxt()
{
	// ȷ����Ԫ�ش���
	if (m_FieldInfoList.empty() || m_strSqlText.empty())
	{
		return FALSE;
	}

	// SQL ����ֵ
	SQLRETURN	retCode;

	// �ж϶����Ƿ����
	if (m_hUpdateSqlStmt != SQL_NULL_HSTMT) 
	{	
		// �ж�����
		if (m_iEditMode == EM_AddNew)
		{
			// ���磺 INSERT INTO TB_TEST(ID,NAME,KK) VALUES(?,?,?)

			// ���ֵ�İ����
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				switch (m_FieldInfoList[i]->GetFieldType())
				{
				case FT_Integer:
					{
						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(),
							SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER, 0, 
							0,(SQLINTEGER*)&(m_FieldInfoList[i]->m_iRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						m_FieldInfoList[i]->m_iRefValue = m_FieldInfoList[i]->GetIntegerValue();
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					} 
					break;
				case FT_String:
					{
						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(), 
							SQL_PARAM_INPUT,SQL_C_CHAR,SQL_VARCHAR,	FIELD_DATA_LEN,0,
							(SQLCHAR*)(m_FieldInfoList[i]->m_szRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						strcpy((m_FieldInfoList[i]->m_szRefValue),m_FieldInfoList[i]->GetStringValue().c_str());
						// ��������
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					}
					break;
				case FT_Binary:
					{
						// �����ƴ���� FT_Binary
						// �ر�˵����ORACLE �� SQL SERVER �Ķ����Ʋ���������һ������ķ�����һ��		
						// ����˳

						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt,  m_FieldInfoList[i]->GetColumnIndex(),
							SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY,
							m_FieldInfoList[i]->GetBinaryValueLen(), 0, 
							(SQLPOINTER) m_FieldInfoList[i]->m_pValue, 
							0, (SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������					
						/*
						The length of the parameter value stored in *ParameterValuePtr. This is ignored except for character or binary C data.
						SQL_NTS. The parameter value is a null-terminated string.
						*/
						m_FieldInfoList[i]->m_icpValue = m_FieldInfoList[i]->GetBinaryValueLen();

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					} // end  FT_Binary
					break;
				case FT_DateTime:
					{
						// ������		
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(), 
							SQL_PARAM_INPUT,SQL_C_CHAR,SQL_VARCHAR,	FIELD_DATA_LEN,0,
							(SQLCHAR*)(m_FieldInfoList[i]->m_szRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						strcpy((m_FieldInfoList[i]->m_szRefValue),m_FieldInfoList[i]->GetStringValue().c_str());
						// ��������
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					} 
					break;
				default:
					break;
				}

			} // end for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)

		} // end  if (m_iEditMode == EM_AddNew)
		else if (m_iEditMode == EM_Edit)
		{
			// UPDATE ����
			// ���磺 UPDATE TB_TEST SET NAME = ? ,PASS= ? WHERE KK = 'E'

			// ���ֵ�İ����
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				switch (m_FieldInfoList[i]->GetFieldType())
				{
				case FT_Integer:
					{
						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(),
							SQL_PARAM_INPUT,SQL_C_LONG,SQL_INTEGER, 0, 
							0,(SQLINTEGER*)&(m_FieldInfoList[i]->m_iRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						m_FieldInfoList[i]->m_iRefValue = m_FieldInfoList[i]->GetIntegerValue();
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------
							return FALSE;
						}

					} // end if (m_FieldInfoList[i]->GetFieldType() == FT_Integer)
					break;
				case FT_String:
					{
						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(), 
							SQL_PARAM_INPUT,SQL_C_CHAR,SQL_VARCHAR,	FIELD_DATA_LEN,0,
							(SQLCHAR*)(m_FieldInfoList[i]->m_szRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						strcpy((m_FieldInfoList[i]->m_szRefValue),m_FieldInfoList[i]->GetStringValue().c_str());
						// ��������
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------


							return FALSE;
						}

					} // else if (m_FieldInfoList[i]->GetFieldType() == FT_String)
					break;
				case FT_Binary:
					{
						// �����ƴ���� FT_Binary

						// �ر�˵����ORACLE �� SQL SERVER �Ķ����Ʋ���������һ������ķ�����һ��		
						// ����˳
						// ������
						retCode = SQLBindParameter(m_hUpdateSqlStmt,  m_FieldInfoList[i]->GetColumnIndex(),
							SQL_PARAM_INPUT, SQL_C_BINARY, SQL_LONGVARBINARY,
							m_FieldInfoList[i]->GetBinaryValueLen(), 0, 
							(SQLPOINTER) m_FieldInfoList[i]->m_pValue, 
							0, (SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������					
						/*
						The length of the parameter value stored in *ParameterValuePtr. This is ignored except for character or binary C data.
						SQL_NTS. The parameter value is a null-terminated string.
						*/
						m_FieldInfoList[i]->m_icpValue = m_FieldInfoList[i]->GetBinaryValueLen();

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					} // end  FT_Binary
					break;
				case FT_DateTime:
					{
						// ������		
						retCode = SQLBindParameter(m_hUpdateSqlStmt, m_FieldInfoList[i]->GetColumnIndex(), 
							SQL_PARAM_INPUT,SQL_C_CHAR,SQL_VARCHAR,	FIELD_DATA_LEN,0,
							(SQLCHAR*)(m_FieldInfoList[i]->m_szRefValue),0,
							(SQLLEN*)&(m_FieldInfoList[i]->m_icpValue));

						// ��������
						strcpy((m_FieldInfoList[i]->m_szRefValue),m_FieldInfoList[i]->GetStringValue().c_str());
						// ��������
						m_FieldInfoList[i]->m_icpValue = SQL_NTS;

						// �жϰ��Ƿ�ɹ�
						if ((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
						{		
							
							//----------------------------------------------------------------------------------
							// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
							ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
							//----------------------------------------------------------------------------------

							return FALSE;
						}

					}
					break;
				default:
					break;
				}

			} // end for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)

		} //  end else if (m_iEditMode == EM_Edit)
		else
		{	
			// ֻ�������ºͲ���ģ����״̬��Ϊ����
			SQLFreeHandle(SQL_HANDLE_STMT,m_hUpdateSqlStmt);
			m_hUpdateSqlStmt = SQL_NULL_HSTMT;

			return FALSE;
		}

	}	//  end if (m_hUpdateSqlStmt != SQL_NULL_HSTMT) 
	else
	{	
		return FALSE;	
	}

	return TRUE;

}

/**************************************************************************************************
Function		: ExecuteInsertUpdate()    
DateTime		: 2010/6/21 0:19	
Description		: ִ��SQL ������߸��²���(INSERT �� UPDATE)
Input			: NULL
Output			: NULL
Return			: TRUE:�ɹ���FALSE:ʧ��
Note			: ǰ��������BuildInsertUpdateSqlTxt()ִ�гɹ�
AppendValueToSqlTxt()ִ�гɹ�	
**************************************************************************************************/
BOOL		CSqlServerRecordSet::ExecuteInsertUpdate()
{
	// ִ�в���
	if (m_hUpdateSqlStmt != SQL_NULL_HSTMT)
	{
		// SQL ����ֵ
		SQLRETURN	retCode;

		// ִ��Sql����
		retCode = SQLExecute(m_hUpdateSqlStmt);

		if((retCode != SQL_SUCCESS) && (retCode != SQL_SUCCESS_WITH_INFO))
		{
			//----------------------------------------------------------------------------------
			// ���������Ϣ [11/30/2010 13:57 Modify by Liujs]
			ErrorLogInfo(m_pConnection->GetOdbcSqlHEven(),m_pConnection->GetOdbcSqlHDbc(),m_hUpdateSqlStmt);
			//----------------------------------------------------------------------------------

			return	FALSE;
		}
	}
	else
	{
		return	FALSE;
	}

	return TRUE;

}

#endif



