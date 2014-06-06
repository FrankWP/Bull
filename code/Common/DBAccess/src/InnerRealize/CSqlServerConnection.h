#ifndef CSQLSERVERCONNECTION_DEF_H
#define CSQLSERVERCONNECTION_DEF_H

// ͷ�ļ�
#include "CConnection.h"

/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name 	: CSQLSERVERCONNECTION.H      
Author		: liujs     
Version 		: Vx.xx        
DateTime 		: 2010/6/7 14:49
Description 	:  Sql Server 2000 / 2005 ���ݿ����Ӷ�����
**************************************************************************************************/
namespace	DBAccessModule
{
	class CSqlServerConnection :public CConnection
	{
	public:
		CSqlServerConnection(void);
		virtual ~CSqlServerConnection(void);

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
		string			ToTime(const char*	szDateTime)
		{	
			char	szDt[64] = {0};
			sprintf(szDt, "'%s'", szDateTime);
			return	string(szDt);
		}
		string			ToDate(const char*	szDateTime)
		{	
			char	szDt[64] = {0};
			sprintf(szDt, "'%s'", szDateTime);
			return	string(szDt);
		}	
		string			ToDateTime(const char*	szDateTime)
		{
			char	szDt[64] = {0};
			sprintf(szDt, "'%s'", szDateTime);
			return	string(szDt);
		}

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
		// ��ȡ������sql���
		BOOL	GetFullPageQuerySql(const char* szSql,const INT32	iStartRow,const INT32 iRowNum,string& strFullSql);
		// ���ҹؼ��֣��ɹ���TRUE��ʧ�ܣ�FALSE �����عؼ���ǰ����ַ���
		BOOL	Find_keyWord_GetString(const char* szSql,const char* szKeyWord,string& strBefore,string& strAfter);

	public:
	#if 0
		// ���Ӷ��� ----------------------------------
		inline	_ConnectionPtr& GetConnection() {return m_pConnection;};
	#else
		// Linux
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

	};


}

#endif // CSQLSERVERCONNECTION_DEF_H
