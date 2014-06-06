/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : GSPSERVER.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/20 9:45
Description: GSP Э�����
********************************************
*/

#ifndef _GS_H_GSPSERVER_H_
#define _GS_H_GSPSERVER_H_
#include "../IProServer.h"
#include "ISocket.h"
#include "ThreadPool.h"
#include "AsyncTcpSrvSocket.h"

namespace GSP
{
class CGspSrvSession;

class CGspServer :
	public CIProServer
{
public :
	typedef struct _StruGspServerConfig
	{
		INT iKeepaliveTimeouts;
		BOOL bEnableUDP;

		BOOL bEnableRtp;
		INT iRtpUdpPortBegin;
		INT iRtpUdpPortEnd;
	}StruGspServerConfig;
	
	
	StruGspServerConfig m_stConfig;
private :
		
	CTCPServerSocket *m_pListenSocket;	

	CGSPString m_strTcpBindIP; //TCP �����󶨵�IP
	INT  m_iTCPPort;  //TCP �����˿�

	BOOL m_bUDP; //ʹ�� UDP Э��
	CGSPString m_strUdpBindIP; //UDP �����󶨵�IP
	INT m_iUDPPort;    //UDP �����˿�


	CGSPThreadPool m_csTaskDestorySession; //�첽�ͷ�Session �߳�

	CAsyncTcpSrvEventContext *m_pAsyncTcpEvtCtx; //��ʹ�ö���TCP������ʱ�� �������Ӷ˿�
public:
	CGspServer(void);
	virtual ~CGspServer(void);

	/*
	*********************************************************************
	*
	*@brief : �ӿ�ʵ��
	*
	*********************************************************************
	*/
	virtual EnumErrno Init( CServer *pServer );
	virtual void Unint(void);

	virtual EnumErrno Start(void);

	virtual void Stop(void);	


	/*
	*********************************************************************
	*
	*@brief : ���ӽӿ�
	*
	*********************************************************************
	*/
	void AsyncDestroySession( CGspSrvSession *pSession );

	CAsyncTcpSrvSocket *CreateAsyncTcpSrvSocket(void);

private :
	void *OnTcpListenSocketEvent(CISocket *pSocket, EnumSocketEvent iEvt,void *pParam, void *pParamExt);


	void OnAsyncDelSessionTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );


	
};

} //end namespace GSP

#endif //end _GS_H_GSPSERVER_H_
