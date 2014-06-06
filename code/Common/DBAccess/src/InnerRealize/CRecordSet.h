#ifndef CRECORDSET_DEF_H
#define CRECORDSET_DEF_H

// ͷ�ļ�
#include "CConnection.h"
// OuterInterfaceͷ�ļ�
#include "../OuterInterface/IDBAccessModule.h"

/**************************************************************************************************
Copyright (C), 2010-2011, GOSUN 
File name : CRECORDSET.H      
Author :  liujs      
Version : Vx.xx        
DateTime : 2010/5/22 14:35
Description : 
���ݼ�������
**************************************************************************************************/
namespace	DBAccessModule
{

	class CRecordSet:public IRecordSet
	{
	public:
		CRecordSet(void);
		virtual ~CRecordSet(void);

	public:
		// OuterInterface֮��̳�IRecordSet�� [4/26/2010 liujs]

		//��ȡ�����ƴ�����
		BOOL	GetBlobCollect(const char* szFieldName,string&	strValue);

	public:
		// �ڲ��ӿڵĶ���
		// ��ѯ�ӿ�
		virtual	IRecordSet*		QuerySql(const char*  szSql) = 0;

		
	};

}
#endif // CRECORDSET_DEF_H

