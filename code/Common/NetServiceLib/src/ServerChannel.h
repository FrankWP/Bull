#if !defined (ServerChannel_DEF_H)
#define ServerChannel_DEF_H
/***************************************************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		ServerChannel.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/25
	Description:     // ģ������      

****************************************************************************************************/

#include "ICreater.h"
#include "SocketChanel.h"

namespace NetServiceLib
{
class CServerChannel : public CSocketChannel,public ICreater 
{
public:
	CServerChannel(){};
	virtual ~CServerChannel(){};

	INT		CreaterTcpChannel(CBaseSocket* pclsSocket);
	INT		CreaterUdpChannel(CBaseSocket* pclsSocket);
public:
	INT		CreateChannel(CBaseSocket* pclsSocket);


};

class CClientChannel : public ICreater, public CSocketChannel
{
public:
	CClientChannel(){};
	virtual ~CClientChannel(){};
	INT		CreateChannel(CBaseSocket* pclsSocket);
	INT		CreaterTcpChannel(CBaseSocket* pclsSocket);//���տͻ��˵Ĳ��轨��udpͨ��
	INT		CreaterUdpChannel(CBaseSocket* pclsSocket);//���տͻ��˵Ĳ��轨��UDPͨ��

};
}

#endif


