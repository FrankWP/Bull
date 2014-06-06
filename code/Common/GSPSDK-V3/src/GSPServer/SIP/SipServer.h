/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPSERVER.H
Author :  ������
Version : 1.0.0.1
Date: 2012/10/30 9:27
Description: 
********************************************
*/

#ifndef _GS_H_SIPSERVER_H_
#define _GS_H_SIPSERVER_H_

#include "../IProServer.h"
#include "ThreadPool.h"
#include "SipStack.h"

#include <map>

namespace GSP
{

namespace SIP
{
class CSipSrvSession;


typedef UINT32 SipSessionConnecter;  //sip ͨ��
#define INVALID_SIP_SESSION_CONNECTER  0


class CSipServer :
	public CIProServer
{
public :
	typedef struct _StruSipServerConfig
	{
		INT iKeepaliveTimeouts;
		INT m_iRtpUdpPortBegin; // rtp udp �˿ڷ�Χ�Ŀ�ʼ�˿ڣ� -1 ��Ч
		INT m_iRtpUdpPortEnd;  // rtp udp �˿ڷ�Χ�Ľ����˿ڣ� -1 ��Ч
		
		
	}StruSipServerConfig;

	StruSipServerConfig m_stConfig;

private :
	SipServiceHandle m_hSipSrv; //SIP ������
	CGSPString m_strSipUdpBindIP; //SIP UDP �����󶨵�IP
	INT  m_iSipUdpPort;  //TCP �����˿�
	CGSString m_strServerName; //����������
	CGSPThreadPool m_csTaskDestorySession; //�첽�ͷ�Session �߳�

	BOOL m_bForceSendRTCP; //ǿ�Ʒ���RTCP 

	typedef std::map<SipSessionHandle, CSipSrvSession *> CMapSession;
	CMapSession m_mapSession;



	std::set<UINT32>  m_setExistSSRC; //����ʹ�õ�SSRC
	UINT32 m_iSSRCSeq;
	CGSMutex m_mutexOfSSRC;

	UINT32 m_iSSRCMin;
	UINT32 m_iSSRCMax;

	typedef std::map<CGSString  /*call-id*/ , CSipSrvSession*> CMapMySession;
	typedef struct _StruMySessionInfo
	{
		time_t tvLoginTv;
		UINT64 tvLastKeepalive;
		SipSessionHandle hHandle;
		SipSessionConnecter hCnner;
		CMapMySession mapMySessions;
		_StruMySessionInfo() : hHandle(SIP_INVALID_HANDLE), hCnner(INVALID_SIP_SESSION_CONNECTER)
		{
			mapMySessions.clear();
		}
	}StruMySessionInfo;


	CGSWRMutex m_mutexSipClients;
	SipSessionConnecter m_seqConnecter;
	typedef std::map<SipSessionConnecter , StruMySessionInfo> CMapSipClient;
	CMapSipClient m_mapSipClients;
	
	

public:
	CSipServer(void);
	virtual ~CSipServer(void);


	UINT32 GetSSRC(void);
	void ReleaseSSRC(UINT32 iSSRC );

// 	INLINE RTP::CRtpOutput *GetRtpOutput(void) const
// 	{
// 		return m_pGRtpOutput;
// 	}


	//�ӿ� 
	virtual EnumErrno Init( CServer *pServer );
	virtual void Unint(void);

	virtual EnumErrno Start(void);

	virtual void Stop(void);	


	BOOL IsForceRTCPKeepalive(void) const
	{
		return m_bForceSendRTCP;
	}

	void SessionDisconnectInvite( CSipSrvSession *pSession);

	EnumErrno SessionSendSipData( CSipSrvSession *pSession, 
				const StruSipData *pSendData, StruSipData *pRes,
				int iTimeouts,  StruSipDialogKey *pOutDlgKey );


	//�������� 
	void AsyncDestroySession( CSipSrvSession *pSession );


// 	INLINE RTP::CRtpOutput *GetGlobalRtpOut( void )
// 	{
// 		return m_pGRtpOutput;
// 	}

private :
	/*void *OnTcpListenSocketEvent(CISocket *pSocket, EnumSocketEvent iEvt,void *pParam, void *pParamExt);*/


	void OnAsyncDelSessionTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );

	EnumSipErrorCode OnSipClientConnectEvent(SipSessionHandle hNewSession );
	void OnSipClientDisconnectEvent(SipSessionHandle hSession);
	void OnSipPacketEvent(SipSessionHandle hSession, StruSipData *pData);



	//SIP Э��ջ�ص�
	//����
	static EnumSipErrorCode SipClientConnectCallback( SipServiceHandle hService,
		SipSessionHandle hNewSession );

	//�Ͽ�
	static void SipClientDisconnectCallback(SipServiceHandle hService, SipSessionHandle hSession);

	//�հ�
	static void   SipSIPPacketCallback(SipServiceHandle hService, 
		SipSessionHandle hSession,
		StruSipData *pData);

};

} //end namespace SIP

} //end namespace GSP

#endif //end _GS_H_SIPSERVER_H_
