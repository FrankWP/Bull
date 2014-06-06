/**************************************************************************************************
* Copyrights 2013  ������
*                  ����Ӧ����
* All rights reserved.
*
* Filename��
*       COciConnection.h
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

#ifndef COCICONNECTION_DEF_H
#define COCICONNECTION_DEF_H


#include "CConnection.h"
#include "ocilib.h"

namespace	DBAccessModule
{

	class COciConnection :
		public CConnection
	{
	public:
		COciConnection(void);
		virtual ~COciConnection(void);

	public:
		// ִ��Sql��䣬�������
		// const char*  szSql:ִ�в�����Sql���
		// ����ֵ���ɹ�TRUE,ʧ��FALSE
		BOOL			ExecuteSql(const char*  szSql);

		// ִ��Sql��䣬�������
		// const char*  szSql:ִ�в�����Sql���
		// ����ֵ���ɹ�TRUE,ʧ��FALSE
		BOOL			ExecuteSql(const char*  szSql,const char* szTable,INT64& lRowId);

		// ��ѯsql���,�������
		// const char*  szSql:ִ�в�����Sql���
		// ����ֵ��IRecordset����ӿڶ��󣬿��Ը���IRecordset�ӿ�ִ����Ӧ�Ľӿ��ṩ����
		IRecordSet*		ExecuteQuery(const char*  szSql);	

		// ����һ���յ����ݼ���Addnew���ݵ�ʱ���õ���
		IRecordSet*		GetEmptyRecordSet();

		// �ͷ����Ӷ���ӿڣ���ʹ����Ϻ�����Ӷ��󣬷��ص����ӳ���
		// ����˵��
		// ����ֵ: �ɹ�TRUE��ʧ��FALSE
		BOOL			ReleaseConnection();

		// ��ҳ��ѯ����
		IRecordSet*		ExecutePageQuery(const char*  szSql,const INT32	iStartRow,const INT32 iRowNum);

		// ʱ���ַ���ת����ʱ�����ӿ�
		string			ToTime(const char*	szDateTime);
		string			ToDate(const char*	szDateTime);
		string			ToDateTime(const char*	szDateTime);

	public:
		// �ڲ��ӿ�
		BOOL	Open();
		// �ر�
		BOOL	Close();
		// �ж��Ƿ��
		BOOL	IsOpen();
		// �ͷ���Դ
		BOOL fini();
		// ��ʼ������
		BOOL init();
		// ������
		void dump_error(OCI_Error* err);

	public:
		// ��ʼִ������
		UINT	BeginTrans();
		// �ع�����
		BOOL	RollbackTrans();
		// �ύ����
		BOOL	CommitTrans();

	public:
		// ��ȡ�������Ӳ�������Ӧ��������Ҫ�������Ͽ������ݿ�Ͽ��Ȳ�������
		BOOL	GetConnectionErrorAndDeal();
	public:

		// �������ݿ����Ӷ�����
		inline	OCI_Connection*	GetOciCon()
		{
			return	m_pCon;
		}


	protected:
		// ���ҹؼ��֣��ɹ���TRUE��ʧ�ܣ�FALSE �����عؼ���ǰ����ַ���
		BOOL	Find_keyWord_GetString(const char* szSql,const char* szKeyWord,string& strBefore,string& strAfter);
		// ��ȡsql����С�from�� ǰ����ַ���
		BOOL	GetBeforeFromSqlText(const char* szSql,string& strBeforeFromSql);
		// ��Select ������� ROWNUM R���ؼ��� 
		BOOL	InsertRowinfo2SqlText(const char* szSql,string& strSql);
		// ���򵥵�sql��䣬��ʵ�кţ��������ϳ��������ַ���
		BOOL	GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql);

	protected:

		//���Ӷ���
		OCI_Connection* m_pCon;

		//����������
		char m_error[1024];

		//�رձ�־
		boolean m_bclose;

		

	};

}
#endif // COCICONNECTION_DEF_H

