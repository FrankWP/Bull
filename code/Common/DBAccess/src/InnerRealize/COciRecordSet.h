/**************************************************************************************************
* Copyrights 2013  ������
*                  ����Ӧ����
* All rights reserved.
*
* Filename��
*       COciRecordSet.h
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

#ifndef COCIRECORDSET_DEF_H
#define COCIRECORDSET_DEF_H

#include "CRecordSet.h"
#include "COciConnection.h"
#include "DbAccessDataDef.h"
#include "CDBFieldInfo.h"

//#include "ocilib.h"

namespace	DBAccessModule
{

	class COciRecordSet :public CRecordSet
	{
	public:

		COciRecordSet(void);

		virtual ~COciRecordSet(void);


	public:

		// �ƶ�����
		// ��¼��β��
		BOOL	Eof();
		// �������һ����¼
		BOOL	MoveLast();
		// ��һ����¼
		BOOL	MoveNext();
		// ��Ӽ�¼
		BOOL    AddNew();
		// ���¼�¼
		BOOL    Update();
		// ��Ӽ�¼
		BOOL	AddNew(const	char*	szTableName);


	public:

		// ֵ�Ļ�ȡ������
		// ���������ƻ�ȡ��Ӧ�ֶε�ֵ������string �����ַ���(INT64,long long��float,double�ȶ�����string����)
		BOOL	GetCollect(const char* szFieldName,string&	strValue);

		BOOL GetBlobCollect(const char* szFieldName,string& strValue);
		// �����������ö�Ӧ�е�ֵ
		BOOL	PutCollect(const char* szFieldName,const char*     szValue);

		// ���������ƻ�ȡ��Ӧ�ֶε�ֵ������INT �����ַ���
		BOOL	GetCollect(const char* szFieldName,INT&	iValue);
		// �����������ö�Ӧ�е�ֵ
		BOOL	PutCollect(const char* szFieldName,const INT	iValue);

		// ���������ƻ�ȡ��Ӧ�ֶε�ֵ������viod* �����ַ���������
		BOOL	GetCollect(const char* szFieldName,void*	   pValue,const INT iBuffSize,INT&	iDataLen);
		// �����������ö�Ӧ�е�ֵ
		BOOL	PutCollect(const char* szFieldName,const  void*	   pValue,const INT	iDataLen);
		// �������������ö�Ӧʱ���ֶε�ֵ
		BOOL	PutDtCollect(const char* szFieldName,const INT iYear,const INT iMonth,const INT iDay,const INT iHour,const INT iMinute,const INT iSecond);
		// �ͷ��������
		BOOL	ReleaseRecordSet();


		INT GetColumnNumber(void);


		BOOL GetCloumnName(INT iColIndex, std::string &oStrName );


	public:    

		// ��ѯ�ӿ�
		IRecordSet*		QuerySql(const char*  szSql);

	public:

		// �������Ӷ���
		BOOL			SetConnection(COciConnection* pConnection);


	protected:

		void harder_error(OCI_Error* err);

		// ��ȡ���ݼ��ж��Ƿ���Խ��б༭
		BOOL	GetEditMode();
		// ȡ���ڵ��� Update ����ǰ�Ե�ǰ��¼���¼�¼�������κθ���.
		BOOL	CancelUpdate();
		// �ж��Ƿ��
		BOOL	IsOpen();
		// �ر����ݼ�
		BOOL	Close();
		// �����ݼ�
		BOOL	Open(const char*  szSql);

		// ��ʼ��STMT��������Ϣ
		BOOL		Init();

		// ��ȡֵ
		// ͨ���ֶ����ƻ�ȡ��Ӧ���е�����
		INT			GetColumnIndexByName(const char* szFieldName);
		// ��ȡ�ֶ������б�
		BOOL		GetColumnList();
		// ����ֶ������б�
		BOOL		ClearColumnList();

		// ����ֵ
		// ���ݲ�ѯ���ݵ�sql�������ȡ���еı�����
		BOOL		GetTableNameFromSqlText(const char* szSql,string& strTableName);
		// ��������ֶ��б�m_FieldInfoList �� AddNew �� Update��ʱ���õģ�
		BOOL		ClearFieldInfoList();
		// ���ֶ��б�������ֶ���Ϣ
		BOOL		AddFieldItem2FieldInfoList(CDBFieldInfo*	pcsDbFieldInfoItem);

	public:
		// ����ִ�е�Sql��䣨INSERT �� UPDATE ����䣩
		BOOL		BuildInsertUpdateSqlTxt();
		// ��Sql���������Ӷ�Ӧ��ֵ
		BOOL		AppendValueToSqlTxt();
		// ִ��SQL ������߸��²���(INSERT �� UPDATE)
		BOOL		ExecuteInsertUpdate();


	protected:	

		//����
		OCI_Statement*  m_pStmtset;

		//�����
		OCI_Resultset*  m_pResultset;

		//������ָ��
		COciConnection*	    m_pConnection;

		//����������
		char m_error[1024];

		// ���ݼ��Ƿ��
		BOOL					m_bIsOpen;

		// �ж��Ƿ��Ǽ�¼��β��
		BOOL					m_bEof;

		// ������Ӧ������ID,��Insert �� Update����ʱ���õ�
		INT						m_iCollumIndexID;


	public:
		// �������������
		inline	void	InitCollumnIndexID()
		{
			m_iCollumIndexID = coniZero;
		}
		// ���������Ĳ���
		inline		INT	GenerateCollumnIndexID()
		{
			return	++m_iCollumIndexID;
		}

	protected:
		// �ֶ������б�(��ȡֵ��ʱ���õ���)
		//ColumnItemVector		m_ColumnItemList;

	protected:
		// ����͸������ݵ�ʱ���õ�

		// ����͸��µ�ʱ���õ�
		//SQLHSTMT				m_hUpdateSqlStmt;
		// �༭ģʽ				
		INT						m_iEditMode;
		// �����ı����ƣ�����͸��µ�ʱ���õ�����ѯ��ʱ���SQL ����н����Զ��Ľ���������
		string					m_strTableName;
		// ������SQL�ַ���
		string					m_strSqlText;
		// �����ѯ��ȥ��һ�����ݵ�ID
		INT						m_iCurrenID;
		// �������ֶ���Ϣ��ϣ�AddNew �� Update��ʱ���õģ�
		DBFieldInfoVector		m_FieldInfoList;



	};

}
#endif // COCIRECORDSET_DEF_H

