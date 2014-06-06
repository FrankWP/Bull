#if !defined (ICreater_DEF_H)
#define ICreater_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		ICreater.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/12
	Description:    ����SOCKETͨ�����������˻�ͻ���

*********************************************************************/


#include "NetServiceDataType.h"

namespace NetServiceLib
{


class CBaseSocket;
class ICreater
{
public:
	ICreater(){};
	virtual ~ICreater(){};
public:
	virtual INT		CreaterTcpChannel(CBaseSocket* pclsSocket)=0;
	virtual INT		CreaterUdpChannel(CBaseSocket* pclsSocket)=0;

};

}

#endif

