#if !defined (LinuxSocket_DEF_H)
#define LinuxSocket_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		LinuxSocket.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/12
	Description:     // ģ������      

*********************************************************************/
#if _LINUX

#include "BaseSocket.h"

namespace NetServiceLib
{


class CLinuxTcpSocket :
	public CBaseSocket
{
public:
	CLinuxTcpSocket(void);
	virtual ~CLinuxTcpSocket(void);
public:
	INT		Visitor(ICreater* pclsVisitor);
	INT		CreateSocket(const char* pszHost,UINT16 unPort, INT iExclusive=0);//����socket ����IP�˿�
	INT		CloseSocket();//�رսӿ�
	INT		Connect(const char *pszDesHost, UINT16 unDesPort);//��������
	INT		Listen();//����
	SOCKETHANDLE		AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen);
	INT		SendData(void*	pData, UINT	uiLen);
	//TCP��������
	virtual INT		SendDataEx(void*	pData, UINT	uiLen);
	INT		SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort);
	INT		RecvData(void* pstruRecv);
protected:
	virtual	INT		SetSockOptionSec();



};

class CLinuxUdpSocket :
	public CBaseSocket
{
public:
	CLinuxUdpSocket(void);
	virtual ~CLinuxUdpSocket(void);
public:
	INT		Visitor(ICreater* pclsVisitor);
	INT		CreateSocket(const char* pszHost,UINT16 unPort);//����socket ����IP�˿�
	INT		CloseSocket();//�رսӿ�
	INT		Connect(const char *pszDesHost, UINT16 unDesPort);//��������
	INT		Listen();//����
	SOCKETHANDLE		AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen);
	INT		SendData(void*	pData, UINT	uiLen);
	//TCP��������
	virtual INT		SendDataEx(void*	pData, UINT	uiLen);
	INT		SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort);
	INT		RecvData(void* pstruRecv);
protected:
	virtual	INT		SetSockOptionSec();

};
#endif	//end

}

#endif	//end #if !defined (LinuxSocket_DEF_H)	


