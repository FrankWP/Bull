#ifndef ICONNECTIONFACTORY_DEF_H
#define ICONNECTIONFACTORY_DEF_H

// ͷ�ļ�
#include "DbAccessDataDef.h"
#include "COracleConnection.h"
#include "CSqlServerConnection.h"
#include "MySQLConnection.h"
#include "COciConnection.h"

/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name : ICONNECTIONFACTORY.H      
Author : liujs     
Version : Vx.xx        
DateTime : 2010/5/23 16:27
Description :  ���ӹ����ӿ�
**************************************************************************************************/
namespace	DBAccessModule
{

	class IConnectionFactory
	{
	public:
		IConnectionFactory(void);
		virtual ~IConnectionFactory(void);

	protected:
		// ���Ӷ���������ID
		static          INT         m_iAutoConnectID;  	

	public:
		// �������������Ӷ���ID
		inline  INT             GenerateAutoConnectID() const 
		{
			return  m_iAutoConnectID++;
		}

	public:
		// Oracle ���ݿ�����
		virtual	CConnection*		CreateOracleConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass) = 0;


		// Sql ���ݿ�����
		virtual	CConnection*		CreateSqlConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass) = 0;

		// Access ���ݿ�����
		virtual	CConnection*		CreateAccessConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass) = 0;

		// MySQL ���ݿ�����
		virtual CConnection*		CreateMySQLConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass) = 0;
		// Oci ���ݿ�����
		virtual	CConnection*		CreateOciConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass) = 0;

	};

}
#endif // ICONNECTIONFACTORY_DEF_H
