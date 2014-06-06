
#ifndef CORACLECONNECTION_DEF_H
#define CORACLECONNECTION_DEF_H

#include "CConnection.h"
#include <set>
#include <map>
#include <string>
/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name : CORACLECONNECTION.H      
Author : liujs     
Version : Vx.xx        
DateTime : 2010/5/22 11:30
Description :   
ORACLE���ݿ���ʶ���,  WINDOWS ������ADO �� LINUX����ODBC
**************************************************************************************************/
namespace	DBAccessModule
{

	class COracleConnection :
		public CConnection
	{
	public:
		COracleConnection(void);
		virtual ~COracleConnection(void);

		// ������ 
		std::set<string > m_setSeqName;
		std::string m_strEmptySeqName;
		//������ ������
		std::map<string,string > m_mapSeq;

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
	#if 0    
		// ���ݿ����Ӷ����Ա����
		_ConnectionPtr	m_pConnection;
	#else
		// Linux
		SQLHENV			m_hEnv;			// ODBC �������
		SQLHDBC			m_hDbc;			// ODBC ���Ӷ�����
		BOOL			m_bConnectFlag;	// �ж����ݿ��Ƿ����ӣ�TRUE������  FALSE:������
	#endif   


	public:
	#if 0
		// ���Ӷ��� ----------------------------------
		inline	_ConnectionPtr& GetConnection() 
		{
			return m_pConnection;
		};
	#else
		// linux 
		// ����ODBC���ݿ⻷�����
		inline	SQLHENV&		GetOdbcSqlHEven()
		{
			return	m_hEnv;
		}
		// ����ODBC���ݿ����Ӷ�����
		inline	SQLHANDLE&		GetOdbcSqlHDbc()
		{
			return	m_hDbc;
		}
	#endif

	private :
		BOOL LoadSeqMap(void);
		const std::string &GetIDSequenceNameOfTableName( const std::string &strTableName );

	}; 

}
#endif // CORACLECONNECTION_DEF_H

