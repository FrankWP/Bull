
#ifndef CDBCONNECTIONPOOL_DEF_H
#define CDBCONNECTIONPOOL_DEF_H

// ͷ�ļ�
#include "../OuterInterface/IDBAccessModule.h"			//  �ⲿ�ӿ�ͷ�ļ���
#include "CConnection.h"								//	���������
#include "CConcreteConnectionFactory.h"					//  ���幤�������
#include "DbAccessDataDef.h"							//  ������������
#include "OciDllDeal.h"


/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name : CDBCONNECTIONPOOL.H      
Author : liujs     
Version : Vx.xx        
DateTime : 2010/5/22 10:21
Description : ���ӳض�����
**************************************************************************************************/
namespace	DBAccessModule
{

	class CDBConnectionPool : public IConnectionPool
	{
	protected:
		// ���ṩ�ⲿ��������ķ������õ����ṩ�ķ���
		CDBConnectionPool(void);
		virtual ~CDBConnectionPool(void);

	private:
		// ����ģ��
		static	CDBConnectionPool*		m_pInstance;
		// ������
		static	CGSMutex				m_CsSingleton;


	public:
		// ��������
		static	CDBConnectionPool*		Instance();

		static  CDBConnectionPool* Create(void)
		{
			return new CDBConnectionPool();
		}
		// �ͷ���Դ
		BOOL	Release();

	public:
		// ��ȡ���Ӷ���ӿ� [5/22/2010 liujs] 
		virtual IConnection*	GetConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType);

		// �ͷ����ӣ������Ӷ���Ż����ӳ�
		BOOL			ReleaseConnection(CConnection*	pConnection);
	private:
		// �ж��û����������Ƿ�׼ȷ
		BOOL			InputDataVerify(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType);

//	public:
		// �����������Ӷ���
		CConnection*    CreateConnectionHandle(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType);
		// ������������,��ӵ����ӳ���
		BOOL            BatchCreateConnHandle(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass,const INT iDbType,const INT iConnNum);
// 		// �ж����е������Ƿ���ʹ��,��ʹ�ã�����TRUE,���򷵻�FALSE
// 		BOOL            BOOLAllConnectionUsed();

//	public:
		// ��ӵ�������
		BOOL            AddConnecttion(CConnection*   pConnection);
		// ɾ�������е�Ԫ��
		BOOL            DeleteConnection(CConnection* pConnection);
		// ɾ�����е�Ԫ��
		BOOL			DeleteAllConnection();
		// ��ȡһ����Ч������
		CConnection*	GetOneConnectionFromPool();
		// ��ȡ���ӳ�Ԫ�ظ���
		INT				GetConnectionPoolNum();
		



	protected:
		// ���ӳ�,��ʱ����vector�У������칹��ķ�ʽ��֧�ֶ����͵Ĵ���
		// Ψһ������ID,��������ID�����ӳ����ҵ�Ψһ������
		Connection_Vector			m_ConnectionPool;  
		// ������������
		CGSMutex					m_CsConnectionPool;
		// ���ӳ��󹤳�����
		IConnectionFactory*			m_pConnectionFactory;

	};
	// DLL������
	extern OciDllDeal*          m_pOciDeal;


}
#endif // CDBCONNECTIONPOOL_DEF_H

