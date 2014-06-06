#ifndef CCONCRETECONNECTIONFACTORY_DEF_H
#define CCONCRETECONNECTIONFACTORY_DEF_H


#include "IConnectionFactory.h"
/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name 	: CCONCRETECONNECTIONFACTORY.H      
Author		: liujs     
Version 		: Vx.xx        
DateTime 		: 2010/6/7 14:24
Description 	: ���幤����ʵ��
**************************************************************************************************/
namespace	DBAccessModule
{

	class CConcreteConnectionFactory :public IConnectionFactory
	{
	public:
		CConcreteConnectionFactory(void);
		virtual ~CConcreteConnectionFactory(void);

	public:
		// �ӿ�ʵ��

		// Oracle ���ݿ�����
		CConnection*		CreateOracleConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass);
		// Sql ���ݿ�����
		CConnection*		CreateSqlConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass);	
		// Access ���ݿ�����
		CConnection*		CreateAccessConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass);
		// MySQL ���ݿ�����
		CConnection*		CreateMySQLConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass);
		// Oci ���ݿ�����
		CConnection*		CreateOciConnection(const	char*   szServer,const	char*   szDatabase,const	char*   szUser,const	char*   szPass);
	};

}

#endif // CCONCRETECONNECTIONFACTORY_DEF_H


