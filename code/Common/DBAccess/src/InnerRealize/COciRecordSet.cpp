/**************************************************************************************************
* Copyrights 2013  ������
*                  ����Ӧ����
* All rights reserved.
*
* Filename��
*       COciRecordSet.cpp
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


#include "COciRecordSet.h"
#include "COciConnection.h"
#include "CDBConnectionPool.h"

using	namespace	DBAccessModule;

/**************************************************************************************************
*@input             Nothing
*@output            Nothing
*@return
*@description       ���캯��
*@frequency of call ����OCI���ݼ���
**************************************************************************************************/
COciRecordSet::COciRecordSet(void)
{

	m_bIsOpen = FALSE;

	// �༭ģʽ
	m_iEditMode = EM_UnKnown;

	// ������
	m_strTableName = constrDefaultString ;


	// �Ƿ��Ǽ�¼��β��
	m_bEof = TRUE;

	m_pConnection = NULL;
}

/**************************************************************************************************
*@input             Nothing
*@output            Nothing
*@return
*@description       ��������
*@frequency of call Nothing
**************************************************************************************************/
COciRecordSet::~COciRecordSet(void)
{
	if (IsOpen())
	{
		Close();
	}

	if (m_bIsOpen)
	{
		m_bIsOpen = FALSE;

		if (this->m_pResultset)
			m_pOciDeal->pOCI_ReleaseResultsets(this->m_pStmtset);

		if (this->m_pStmtset)
			m_pOciDeal->pOCI_StatementFree(this->m_pStmtset);

	}
	m_iEditMode = EM_UnKnown;
	m_bEof = TRUE;
}

/**************************************************************************************************
  Function:    harder_error    
  DateTime:    2013/5	
  Description: ������
  Input:       NULL
  Output:      NULL
  Return:      
			   �ɹ���TRUE
			   ʧ�ܣ�FALSE
  Note:		   
**************************************************************************************************/
void COciRecordSet::harder_error(OCI_Error* err)
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
  Function:			IsOpen    
  DateTime:			2013/05
  Description:    	�ж����ݼ��Ƿ��
  Input:          	NULL
  Output:         	NULL
  Return:         	
					TRUE:��
					FALSE:�ر�
  Note:				// ��ע
**************************************************************************************************/
BOOL	COciRecordSet::IsOpen()
{
	return	m_bIsOpen;
}

/**************************************************************************************************
  Function:		 Close    
  DateTime:		 2013/05	
  Description:   �ر����ݼ�
  Input:         NULL
  Output:        NULL
  Return:        
				 TRUE: �رճɹ�
				 FALSE: �ر�ʧ��
  Note:		
**************************************************************************************************/
BOOL	COciRecordSet::Close()
{

	if (IsOpen())
	{
		m_bIsOpen = FALSE;
		if (this->m_pResultset)
			m_pOciDeal->pOCI_ReleaseResultsets(this->m_pStmtset);

		if (this->m_pStmtset)
			m_pOciDeal->pOCI_StatementFree(this->m_pStmtset);

	}
	return	TRUE;

}

/**************************************************************************************************
  Function:		 Open    
  DateTime:		 2013/05	
  Description:   �����ݼ�
  Input:         ��׼Sql���
  Output:        NULL
  Return:        
				 TRUE: �򿪳ɹ�
				 FALSE:��ʧ��
  Note:			
**************************************************************************************************/
BOOL	COciRecordSet::Open(const char*  szSql)
{
	try
	{
		// �ر����ݼ�����
		if (IsOpen()) 
		{
			Close();
		}
		//�����ݼ�
		if (Init())
		{
			if(!m_pOciDeal->pOCI_ExecuteStmt(m_pStmtset, szSql))
			{
				harder_error(m_pOciDeal->pOCI_GetLastError());
				return FALSE;
		
			}
			m_pResultset = m_pOciDeal->pOCI_GetResultset(m_pStmtset);
			if(m_pResultset == NULL)
			{
				
				harder_error(m_pOciDeal->pOCI_GetLastError());
				return FALSE;
			}

			m_strTableName = constrDefaultString ;
			// �Ѿ�ִ�гɹ��������ݼ���
			m_bIsOpen = TRUE;
			// ��ȡ���������Ϣ�����б�
			//GetColumnList();
			
		//	if(!OCI_FetchFirst(m_pResultset)) 
			if(!m_pOciDeal->pOCI_FetchNext(m_pResultset))
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

		}
	}

	catch (...)
	{
		// ����Ͽ������ˣ�Ҫ������������
		m_pConnection->GetConnectionErrorAndDeal();
		m_bIsOpen = FALSE;
		return FALSE;
	}

	return TRUE;

}

/**************************************************************************************************
  Function		: Init()    
  DateTime		: 2013/05	
  Description	: ��ʼ��STMT��������Ϣ
  Input			: NULL
  Output		: NULL
  Return		: TRUE :�ɹ�  FALSE : ʧ��
  Note			��OPEN�����е���
**************************************************************************************************/
BOOL		COciRecordSet::Init()
{

	if ( !m_pConnection->IsOpen() ) 
		if ( !m_pConnection->Open() )
			return FALSE;
/*
	if (this->m_pStmtset)
		m_pOciDeal->pOCI_StatementFree(this->m_pStmtset);
*/
	this->m_pStmtset = m_pOciDeal->pOCI_StatementCreate(m_pConnection->GetOciCon());

	if (!this->m_pStmtset) 
	{
		harder_error(m_pOciDeal->pOCI_GetLastError());
		return FALSE;
	}

	return TRUE;
}

/**************************************************************************************************
  Function:			QuerySql    
  DateTime:			2013/05	
  Description:    	ִ��Sql��䷵�����ݼ�
  Input:          	��׼Sql���
  Output:         	NULL
  Return:         	
					IRecordSet* �������ݼ�����
  Note:				ִ��SELECT��ѯ����
**************************************************************************************************/
IRecordSet*		COciRecordSet::QuerySql(const char*	szSql)
{	

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

/**************************************************************************************************
Function:			SetConnection    
DateTime:			2013/05	
Description:    	�������Ӷ���
Input:          	pConnection : ���Ӷ���
Output:         	NULL
Return:         	// ��������ֵ��˵��
Note:				// ��ע
**************************************************************************************************/
BOOL  COciRecordSet::SetConnection(COciConnection* pConnection)
{
	if (pConnection != NULL)
	{
		m_pConnection = pConnection;
		return	TRUE;
	}
	return FALSE;

}
/**************************************************************************************************
  Function		: ReleaseRecordSet    
  DateTime		: 2013/5	
  Description	: �ͷ����ݼ�����
  Input			: NULL
  Output		: NULL
  Return		: TRUE:�ɹ���FALSE:ʧ��
  Note			: ���øú������������Ͳ������ˣ������ڽ��е��ö������ط�����
**************************************************************************************************/
BOOL	COciRecordSet::ReleaseRecordSet()
{
	delete	this;
	return  TRUE;
}

/**************************************************************************************************
  Function:			Eof   
  DateTime:			2013/05	
  Description:    	��ѯ�α��Ƿ����ڼ�¼��β
  Input:          	NULL
  Output:           NULL
  Return:         	
					TRUE: ��¼��β��
					FALSE:���Ǽ�¼��β��
  Note:				
**************************************************************************************************/
BOOL	COciRecordSet::Eof()
{
	m_iEditMode = EM_UnKnown;
	return m_bEof;

}

/**************************************************************************************************
  Function:			MoveLast    
  DateTime:			2013/05
  Description:    	�������һ����¼
  Input:          	NULL
  Output:         	NULL
  Return:         	
					FALSE: ��¼���Ƶ����һ����¼��ʧ��
					TRUE: ��¼���Ƶ����һ����¼���ɹ�
  Note:				
**************************************************************************************************/
BOOL	COciRecordSet::MoveLast()
{
	if (!m_pOciDeal->pOCI_FetchLast (this->m_pResultset)) 
	{
		this->harder_error(m_pOciDeal->pOCI_GetLastError());
		
		m_bEof = TRUE;

		return FALSE;
	}
	else
	{
		InitCollumnIndexID();
		m_bEof = FALSE;
	}
	return TRUE;

}


/**************************************************************************************************
  Function:			MoveNext   
  DateTime:			2013/05
  Description:      ���ݼ��ƶ�����һ����¼
  Input:          	NULL
  Output:         	NULL
  Return:         	
					TRUE  : �ƶ��ɹ�
					FALSE : �ƶ�ʧ��
  Note:				
**************************************************************************************************/
BOOL	COciRecordSet::MoveNext()
{
	if (!m_pOciDeal->pOCI_FetchNext(this->m_pResultset)) 
	{
		this->harder_error(m_pOciDeal->pOCI_GetLastError());
		m_bEof = TRUE;
		return FALSE;
	}
	else
	{
		// ����������к����־
		InitCollumnIndexID();
		m_bEof = FALSE;
		return	TRUE;
	}
	return TRUE;
}


/**************************************************************************************************
  Function:			AddNew() 
  DateTime:			2013/05
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
BOOL    COciRecordSet::AddNew()
{
	m_iEditMode = EM_AddNew;
	// ����������к����־
	InitCollumnIndexID();
	ClearFieldInfoList();
	return	TRUE;


}


/**************************************************************************************************
Function		: AddNew(IConnection*	pConnection,const	char*	szTableName)
DateTime		: 2013/5
Description		: ��һ�������ϣ�ָ���ı������������
Input			: pConnection�����Ӷ���
szTableName		��������
Output			: NULL
Return			: TRUE:�ɹ�
Note			:
**************************************************************************************************/
BOOL	COciRecordSet::AddNew(const	char*	szTableName)
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
  Function:			Update    
  DateTime:			2013/05	
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
BOOL    COciRecordSet::Update()
{

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
	return TRUE;

}


INT COciRecordSet::GetColumnNumber(void)
{

	return m_pOciDeal->pOCI_GetColumnCount(m_pResultset); 	

}


BOOL COciRecordSet::GetCloumnName(INT iColIndex, std::string &oStrName )
{

	int n;
	n  = m_pOciDeal->pOCI_GetColumnCount(m_pResultset);  
	if(iColIndex > n)
		return FALSE;
  
    OCI_Column *col = m_pOciDeal->pOCI_GetColumn(m_pResultset, iColIndex);  
	oStrName = m_pOciDeal->pOCI_ColumnGetName(col);

	return TRUE;
}
/**************************************************************************************************
  Function:			GetCollect(const char* szFieldName,string&	szValue)    
  DateTime:			2013/5
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
BOOL	COciRecordSet::GetCollect(const char* szFieldName,string&	szValue)
{

	if (m_pOciDeal->pOCI_IsNull2(this->m_pResultset, szFieldName))
		return FALSE;

	szValue = m_pOciDeal->pOCI_GetString2(this->m_pResultset, szFieldName);

	return TRUE;

}

/**************************************************************************************************
  Function:     
  DateTime:			2013/05	
  Description:    	��ȡ���ݼ��ж��Ƿ���Խ��б༭
  Input:          	NULL
  Output:         	NULL
  Return:         
					TRUE:	���Ա༭
					FALSE:  �����Ա༭

  Note:				NULL
**************************************************************************************************/
BOOL		COciRecordSet::GetEditMode()
{
	return	FALSE; 
}

/**************************************************************************************************
  Function:			PutCollect(const char* szFieldName,const char*     szValue)    
  DateTime:			2013/5
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
BOOL	COciRecordSet::PutCollect(const char* szFieldName,const char*     szValue)
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
		return FALSE;

}


/**************************************************************************************************
  Function		: PutDtCollect(const char* szFieldName,const char* szValue)
  DateTime		: 2013/5	
  Description	: �������������ö�Ӧʱ���ֶε�ֵ
  Input			: szFieldName���ֶ����ƣ�szValue��ʱ��ֵ���ַ���
  Output		: NULL
  Return		: NULL
  Note			:
**************************************************************************************************/
BOOL	COciRecordSet::PutDtCollect(const char* szFieldName,const INT iYear,const INT iMonth,const INT iDay,const INT iHour,const INT iMinute,const INT iSecond)
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
		// ����ֵ
		pcsDbFieldInfoItem->SetDateTimeValue(iYear,iMonth,iDay,iHour,iMinute,iSecond);

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
		return FALSE;
}



/**************************************************************************************************
  Function:			GetCollect(const char* szFieldName,INT&	iValue)    
  DateTime:			2013/5
  Description:    	���������ƻ�ȡ��Ӧ�ֶε�ֵ������INT �����ַ���
  Input:          	szFieldName:�ֶ�����
  Output:         	iValue:��ȡ�õ���ֵ
  Return:         	
					TRUE:�����ɹ�
					FALSE:����ʧ��
  Note:				
					���������ƻ�ȡ��Ӧ�ֶε�ֵ������INT �����ַ���
**************************************************************************************************/
BOOL	COciRecordSet::GetCollect(const char* szFieldName,INT&	iValue)
{

	if (m_pOciDeal->pOCI_IsNull2(this->m_pResultset, szFieldName))
		return FALSE;

	iValue = m_pOciDeal->pOCI_GetUnsignedInt2(this->m_pResultset, szFieldName);

	return TRUE;

}

/**************************************************************************************************
  Function:			PutCollect(const char* szFieldName,const INT	iValue)    
  DateTime:			2013/5
  Description:    	�����������ö�Ӧ�е�ֵ
  Input:          	szFieldName:�ֶ�����
		         	iValue:���õ�����ֵ
  Output:			NULL
  Return:           TRUE:�ɹ�
					FALSE:ʧ��
  Note:				
					Ҫ��ʹ���ߵ��õ�ʱ�򣬿��ƣ�Ҫ���õ��ֶα�����int���͵Ĳſ���
**************************************************************************************************/
BOOL	COciRecordSet::PutCollect(const char* szFieldName,const INT	iValue)
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
		return	FALSE;

}

// ���������ƻ�ȡ��Ӧ�ֶε�ֵ������viod** �����ַ���
/**************************************************************************************************
  Function:			GetCollect(const char* szFieldName,void**	   pValue,INT&	iDataLen)   
  DateTime:			2013/5
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
BOOL	COciRecordSet::GetCollect(const char* szFieldName,void*	   pValue,const INT iBuffSize,INT&	iDataLen)
{
	SQLCHAR*		pszData = NULL;
	try
	{


		// ��ǰ�ֶε��ֽڳ���
		UINT		iActualDataSize = 0;
/*
		// ��ȡ������
		SQLSMALLINT		iColumnIndex = GetColumnIndexByName(szFieldName);
		if(iColumnIndex == ERROR_SQL_COLUMN_INDEX)
		{
			return FALSE;
		}
*/
		// �����ڴ�
		pszData = new SQLCHAR[iBuffSize + 1];
		memset(pszData,0x0,iBuffSize+1);

		if (m_pOciDeal->pOCI_IsNull2(this->m_pResultset, szFieldName))
		{
			if (pszData != NULL)
			{
					delete[] pszData;
					pszData = NULL;
			}

			return FALSE;
		}

		OCI_Lob* lob = m_pOciDeal->pOCI_GetLob2(this->m_pResultset, szFieldName);
		iActualDataSize = m_pOciDeal->pOCI_LobGetLength(lob);

		if (iActualDataSize <= 0)
		{
			if (pszData != NULL)
			{
					delete[] pszData;
					pszData = NULL;
			}

			return FALSE;
		}
		m_pOciDeal->pOCI_LobRead(lob, pszData, iActualDataSize);
		// �������ݣ����ҷ��ش�С
		memcpy(pValue,pszData,iActualDataSize);
		iDataLen = iActualDataSize;
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

	return TRUE;

}
BOOL COciRecordSet::GetBlobCollect(const char* szFieldName,string& strValue)
{
	SQLCHAR*		pszData = NULL;
	try
	{
		// ��ǰ�ֶε��ֽڳ���
		UINT		iActualDataSize = 0;

		// �����ڴ�
		if (m_pOciDeal->pOCI_IsNull2(this->m_pResultset, szFieldName))
		{		
			return FALSE;
		}

		OCI_Lob* lob = m_pOciDeal->pOCI_GetLob2(this->m_pResultset, szFieldName);
		iActualDataSize = m_pOciDeal->pOCI_LobGetLength(lob);

		if (iActualDataSize <= 0)
		{			
			return FALSE;
		}
		strValue.resize(iActualDataSize);
		m_pOciDeal->pOCI_LobRead(lob, (void*)strValue.c_str(), iActualDataSize);
		
	}
	catch(...)
	{
		strValue.clear();
		return	FALSE;
	}

	return TRUE;
}
/**************************************************************************************************
  Function:			PutCollect(const char* szFieldName,const  void*	   pValue,const INT	iDataLen)    
  DateTime:			2013/5	
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
BOOL	COciRecordSet::PutCollect(const char* szFieldName,const  void*	   pValue,const INT	iDataLen)
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

		return TRUE;

}

/**************************************************************************************************
Function		: ClearFieldInfoList()    
DateTime		: 2013/5	
Description		: ��������ֶ��б�m_FieldInfoList �� AddNew �� Update��ʱ���õģ�
Input			: NULL
Output			: NULL
Return			: TRUE
Note			:��m_FieldInfoList �� AddNew �� Update��ʱ���õģ�	
**************************************************************************************************/
BOOL		COciRecordSet::ClearFieldInfoList()
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

/**************************************************************************************************
  Function:			CancelUpdate    
  DateTime:			2013/05	
  Description:		ȡ���ڵ��� Update ����ǰ�Ե�ǰ��¼���¼�¼�������κθ���.
  Input:          	NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE ʧ�ܣ�FALSE
  Note:				
**************************************************************************************************/
BOOL	COciRecordSet::CancelUpdate()
{
	ClearFieldInfoList();	
	
	// ���ñ༭ģʽΪUnKnown
	m_iEditMode = EM_UnKnown;


	return FALSE;

}


/**************************************************************************************************
  Function:			AddFieldItem2FieldInfoList    
  DateTime:			2013/05	
  Description:		���ֶ��б�������ֶ���Ϣ
  Input:          	NULL
  Output:         	NULL
  Return:         	�ɹ���TRUE ʧ�ܣ�FALSE
  Note:				
**************************************************************************************************/
BOOL		COciRecordSet::AddFieldItem2FieldInfoList(CDBFieldInfo*	pcsDbFieldInfoItem)
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
  DateTime		: 2013/5	
  Description	: ���ݲ�ѯ���ݵ�sql�������ȡ���еı�����(��Ҫ�ǶԱ��ѯ�󣬽��в������ݺ͸������ݵ�ʱ����)
  Input			: Sql ��ѯ��䣬���磺SELECT * from TB_TEST;
  Output		: strTableName:�����ı�����
  Return		: TRUE:�ɹ���FALSE:ʧ��
  Note			: �����ݼ��ĵط�Ҫʹ�ã������ݼ��Ķ�Ӧ�ı����ƴ������������ڸ��ºͲ���ʱ����
**************************************************************************************************/
BOOL		COciRecordSet::GetTableNameFromSqlText(const char* szSql,string& strTableName)
{
	try
	{
		string	strSql  = string(szSql);
		// ���������һ��from������ַ���


		// ת��Ϊ��д
		// transform (strSql.begin(),strSql.end(), strSql.begin(),toupper); 
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
DateTime		: 2013/5	
Description		: ����Sql�����в���
Input			: NULL
Output			: NULL
Return			: TRUE: �ɹ� ��FALSE :ʧ��
Note			: 
**************************************************************************************************/
BOOL		COciRecordSet::BuildInsertUpdateSqlTxt()
{
	// ȷ����Ԫ�ش���
	if (m_FieldInfoList.empty())
	{
		return FALSE;
	}

	// �ر����ݼ�����
	if (IsOpen()) 
	{
		Close();
	}
	//�����ݼ�
	if (Init())
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
				m_strSqlText += ":";
				m_strSqlText += m_FieldInfoList[j]->GetFieldName();
				if (j!=m_FieldInfoList.size() - 1)
				{
					m_strSqlText += ",";
				}
				else
				{
					m_strSqlText += ")";
				}
			}
			

			m_pOciDeal->pOCI_Prepare(m_pStmtset,(char*)m_strSqlText.c_str());


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
				m_strSqlText += " = ";
				m_strSqlText += ":";
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
			m_pOciDeal->pOCI_Prepare(m_pStmtset,(char*)m_strSqlText.c_str());

		}
		else
		{	
			// ֻ�������ºͲ���ģ����״̬��Ϊ����
			if (m_pStmtset)
				m_pOciDeal->pOCI_StatementFree(m_pStmtset);
			return FALSE;
		}

	}
	else
	{	
		return FALSE;	
	}

	return	TRUE;

}


/**************************************************************************************************
Function		: AppendValueToSqlTxt()    
DateTime		: 2013/5
Description		: ��Sql���������Ӷ�Ӧ��ֵ
Input			: NULL
Output			: NULL
Return			: TRUE:�ɹ���FALSE:ʧ��
Note			: ǰ��������BuildInsertUpdateSqlTxt()ִ�гɹ�
				���������ExecuteInsertUpdate()��ִ��
**************************************************************************************************/
BOOL		COciRecordSet::AppendValueToSqlTxt()
{
	// ȷ����Ԫ�ش���
	if (m_FieldInfoList.empty() || m_strSqlText.empty())
	{
		return FALSE;
	}

	// �ж϶����Ƿ����
	if (m_pStmtset) 
	{	
		// �ж�����
		if (m_iEditMode == EM_AddNew)
		{
			// ���ֵ�İ����
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				switch (m_FieldInfoList[i]->GetFieldType())
				{
				case	FT_Integer:
					{
						// ������
						if (!m_pOciDeal->pOCI_BindInt(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), &m_FieldInfoList[i]->m_iValue)) 
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
					} 
					break;
				case	FT_String:
					{
						
						if (!m_pOciDeal->pOCI_BindString(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), (char*)m_FieldInfoList[i]->m_strValue.c_str(), m_FieldInfoList[i]->m_strValue.size()+1))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
						
					} 
					break;
				case	FT_DateTime:
					{
						
						OCI_Date* date = m_pOciDeal->pOCI_DateCreate(NULL);
						if (!date) 
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}

						m_pOciDeal->pOCI_DateSetDateTime(date,
							m_FieldInfoList[i]->m_stDateTime.year, m_FieldInfoList[i]->m_stDateTime.month,m_FieldInfoList[i]->m_stDateTime.day,
							m_FieldInfoList[i]->m_stDateTime.hour,m_FieldInfoList[i]->m_stDateTime.minute,m_FieldInfoList[i]->m_stDateTime.second);

						if(!m_pOciDeal->pOCI_BindDate(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), date))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
						

					}
					break;
				case FT_Binary:
					{
						if (!m_pOciDeal->pOCI_BindString(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), (char*)m_FieldInfoList[i]->m_pValue, m_FieldInfoList[i]->GetBinaryValueLen()+1))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
						
					} 
					break;
				default:
					break;
				}

			} 

		}
		else if (m_iEditMode == EM_Edit)
		{
			for (DBFieldInfoVector::size_type i=0;i<m_FieldInfoList.size();i++)
			{
				switch (m_FieldInfoList[i]->GetFieldType())
				{
				case	FT_Integer:
					{ 
						// ������
						if (!m_pOciDeal->pOCI_BindInt(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), &m_FieldInfoList[i]->m_iValue)) 
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
					} 
					break;
				case	FT_String:
					{
						if (!m_pOciDeal->pOCI_BindString(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), (char*)m_FieldInfoList[i]->m_strValue.c_str(), m_FieldInfoList[i]->m_strValue.size()+1))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
					} 
					break;
				case	FT_DateTime:
					{
						OCI_Date* date = m_pOciDeal->pOCI_DateCreate(NULL);
						if (!date) 
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}

						m_pOciDeal->pOCI_DateSetDateTime(date,
							m_FieldInfoList[i]->m_stDateTime.year, m_FieldInfoList[i]->m_stDateTime.month,m_FieldInfoList[i]->m_stDateTime.day,
							m_FieldInfoList[i]->m_stDateTime.hour,m_FieldInfoList[i]->m_stDateTime.minute,m_FieldInfoList[i]->m_stDateTime.second);

						if(!m_pOciDeal->pOCI_BindDate(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), date))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}
						

					}
					break;
				case FT_Binary:
					{
						if (!m_pOciDeal->pOCI_BindString(m_pStmtset, m_FieldInfoList[i]->GetFieldName().c_str(), (char*)m_FieldInfoList[i]->m_pValue, m_FieldInfoList[i]->GetBinaryValueLen()+1))
						{
							harder_error(m_pOciDeal->pOCI_GetLastError());
							return FALSE;
						}

					} 
					break;
				default:
					break;
				}

			}

		}
		else
		{	
			// ֻ�������ºͲ���ģ����״̬��Ϊ����
			if (m_pStmtset)
				m_pOciDeal->pOCI_StatementFree(m_pStmtset);
			return FALSE;
		}

	}
	else
	{	
		return FALSE;	
	}

	return	TRUE;
}

/**************************************************************************************************
  Function		: ExecuteInsertUpdate()    
  DateTime		: 2013/5	
  Description	: ִ��SQL ������߸��²���(INSERT �� UPDATE)
  Input			: NULL
  Output		: NULL
  Return		: TRUE:�ɹ���FALSE:ʧ��
  Note			: ǰ��������BuildInsertUpdateSqlTxt()ִ�гɹ�
							AppendValueToSqlTxt()ִ�гɹ�	
**************************************************************************************************/
BOOL		COciRecordSet::ExecuteInsertUpdate()
{
	// ִ�в���
	if (m_pStmtset)
	{
		m_pOciDeal->pOCI_Execute(m_pStmtset);
		if(!m_pOciDeal->pOCI_Commit(m_pConnection->GetOciCon()))
			return	FALSE;
	}
	else
	{
		return	FALSE;
	}

	return TRUE;
}

/**************************************************************************************************
  Function		: GetColumnList()    
  DateTime		: 2013/5	
  Description	: ��ȡ�ֶ������б�
  Input			: NULL
  Output		: NULL
  Return		: �ɹ�����TRUE,ʧ�ܷ���FALSE
  Note			: NULL
**************************************************************************************************/
BOOL		COciRecordSet::GetColumnList()
{
	return	FALSE;
}

/**************************************************************************************************
  Function		: ClearColumnList()    
  DateTime		: 2013/5	
  Description	: ����ֶ��б�
  Input			: NULL
  Output		: NULL
  Return		: TRUE
  Note			: NULL
**************************************************************************************************/
BOOL		COciRecordSet::ClearColumnList()
{
	//m_ColumnItemList.clear();
	return	FALSE;

}


/**************************************************************************************************
  Function		: GetColumnIndexByName(const char* szFieldName)   
  DateTime		: 2013/5	
  Description	: �����ֶ����ƣ���ȡ�е�����
  Input			: �ֶ�����
  Output		: NULL
  Return		: �е�����
  Note			: ��ȡʧ�ܷ��� ERROR_SQL_COLUMN_INDEX
**************************************************************************************************/
INT		  COciRecordSet::GetColumnIndexByName(const char* szFieldName)
{
	/*
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
	*/
	return 0;

}