#if !defined (BaseSocket_DEF_H)
#define BaseSocket_DEF_H

/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		BaseSocket.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/07
	Description:     ����windows linux��sokcetͨѶ�Ĺ��з��������ԣ�֮����������ͬ��ƽ̨�ࡣ
					��ʵҲ�������첽�͹ر�shutdown��һ��
					 ����tcp����udp�˿ڣ��������Ƿ������ˡ�Ҳ�����ǿͻ��ˡ��ɷ����߾���

*********************************************************************/

#include "NetServiceDataType.h"
#include "ICreater.h"

#if _WIN32
#include "IOCP.h"
#else
#include "LinuxEpoll.h"
#endif

namespace NetServiceLib
{



class CBaseSocket
{
public:
	CBaseSocket(void);
	virtual ~CBaseSocket(void);
public:
	//�ɷ��ӿ�
	virtual INT		Visitor(ICreater* pclsVisitor)=0;

	//����socket ����IP�˿�
	virtual INT		CreateSocket(const char* pszHost,UINT16 unPort, INT iExclusive=0)=0;

	virtual	INT		CloseSocket()=0;
	//��������
	virtual INT		Connect(const char *pszDesHost, UINT16 unDesPort)=0;

	//����
	virtual INT		Listen()=0;

	virtual	SOCKETHANDLE		AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen)=0;

	//TCP��������
	virtual INT		SendData(void*	pData, UINT	uiLen)=0;

	//TCP��������
	virtual INT		SendDataEx(void*	pData, UINT	uiLen)=0;

	//UDP��������
	virtual INT		SendDataTo(void* pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort)=0;

	virtual	INT		RecvData(void* pstruRecv)=0;

public:
	SOCKETHANDLE	GetSocket();

	void			SetSocket(SOCKETHANDLE hSocket);

	//����socketѡ��
	INT				SetSockOption(INT iExclusive=0);	

	// ����������ʽ���������ʽ FALSE ����  TRUE������ ����൱�ڱ������
	inline	void			SetBlockMode( BOOL bMode){ m_bBlock = bMode; };

	// ����������ʽ���������ʽ FALSE ����  TRUE������
	void			SetBlockModeEx( BOOL bMode);

	// ����sokect�Ƿ����� ֻ�з�����ģʽ����Ҫ����, ���� 0x1ffff ��ʾ�ɹ��� 0 ��ʾ��ʱ��  < 0 ��ʾ����
	INT				TestSocket(INT iFlag);

	// ���÷��ͻ�������С
	INT			SetSendBuf(INT iBufSize);

	// ���ý��ջ�������С
	INT			SetRcvBuf(INT iBufSize);

	//iTimeOut��λΪ��, ���� 0x1ffff ��ʾ�ɹ��� 0 ��ʾ��ʱ��  < 0 ��ʾ����
	INT			TestSocketEx(INT iFlag, INT iTimeOut, INT& iErrNo);
protected:
	virtual	INT		SetSockOptionSec()=0;

protected:
	//socket���
	SOCKETHANDLE		m_hSocket;	

	//ֻ��UDP�ķ������˲���Ҫ��������Ϊ���ܶ��ͨ��ͬʱ����Ϣ
	CGSMutex			m_GSMutex;		

	// ��������������ʽ	TRUE ������ģʽ  FALSE ������ʽ
	BOOL				m_bBlock;		
};

#if _WIN32

class CWinTcpSocket :
	public CBaseSocket
{
public:
	CWinTcpSocket(void);
	virtual ~CWinTcpSocket(void);
public:
	virtual	INT		Visitor(ICreater* pclsVisitor);
	virtual	INT		CreateSocket(const char* pszHost,UINT16 unPort, INT iExclusive=0);//����socket ����IP�˿�
	//��
	virtual	INT				BindSocket(sockaddr_in& service);

	virtual	INT		CloseSocket();//�رսӿ�
	virtual	INT		Connect(const char *pszDesHost, UINT16 unDesPort);//��������
	virtual	INT		Listen();//����
	virtual	SOCKETHANDLE		AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen);
	virtual	INT		SendData(void*	pData, UINT	uiLen );
	//TCP��������
	virtual INT		SendDataEx(void*	pData, UINT	uiLen);
	virtual	INT		SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort);
	virtual	INT		RecvData(void* pstruRecv);
protected:
	virtual	INT		SetSockOptionSec();
	static	INT		m_iCount;

};

class CWinUdpSocket :
	public CBaseSocket
{
public:
	CWinUdpSocket(void);
	virtual ~CWinUdpSocket(void);
public:
	INT		Visitor(ICreater* pclsVisitor);
	INT		CreateSocket(const char* pszHost,UINT16 unPort, INT iExclusive);//����socket ����IP�˿�
	//��
	INT				BindSocket(sockaddr_in& service);

	INT		CloseSocket();//�رսӿ�
	INT		Connect(const char *pszDesHost, UINT16 unDesPort);//��������
	INT		Listen();//����
	SOCKETHANDLE		AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen);
	INT		SendData(void*	pData, UINT	uiLen );
	//TCP��������
	virtual INT		SendDataEx(void*	pData, UINT	uiLen);
	INT		SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort);
	INT		RecvData(void* pstruRecv );
protected:
	virtual	INT		SetSockOptionSec();

};

#endif //end #if _WIN32

}

#endif



