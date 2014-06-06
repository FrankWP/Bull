/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : RTSPSRVSESSION.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/4/18 10:31
Description: RTSP �Ự
********************************************
*/

#ifndef _GS_H_RTSPSRVSESSION_H_
#define _GS_H_RTSPSRVSESSION_H_



#include "../IProServer.h"
#include "ThreadPool.h"
#include "MainLoop.h"
#include "ISocket.h"
#include "../RefSource.h"
#include "RTSP/RTSPStru.h"
#include "RTSP/RTSPAnalyer.h"
#include "RTP/SdpParser.h"
#include "MediaInfo.h"
#include "StreamPackConvert.h"
#include "RTP/RtpNet.h"

namespace GSP
{




namespace RTSP
{

class CRtspServer;


class CRtspSrvSession :
	public CISrvSession
{
private :
	//�����ź�
	typedef enum
	{
		eSIG_NONE = 0,
		eSIG_RELEASE, //ɾ��
		eSIG_REMOTE_DISCONNECT, //�Զ˹ر�����
		eSIG_ASSERT, //�쳣
		eSIG_REMOTE_CLOSE, //�Զ˹ر�
	}EnumSignal;

	CGSString m_strSessionID;

	CRtspServer *m_pRtspServer;
	CRtspTcpDecoder *m_pRtspTcpDecoder;





	BOOL m_bRtspTcpSending;    //��׼TCP �Ƿ����ڷ���
	CList m_csRtspTcpWaitSendQueue;  //Tcp�ȴ��������ݵĶ���, ����Ϊ CIPacket *
	BOOL m_bRtspTcpWaitSendFinish; //�ȴ��������
	INT64 m_iWaitSendSize; //�ȴ����͵����ݴ�С

	EnumHTTPState m_eHttpStatus;
	EnumTransportMode m_eTransport;
	EnumRTSPComandMask m_eCurAllowMask; //��ǰ���е�����

	
	

	CGSWRMutex m_csWRMutex;
	
	INT m_iKeepalivePlugs;
	CWatchTimer m_csSendKeepaliveTimer; //����Keepalive Timer 
	CISocket *m_pRtspTcpSocket;
	
	BOOL m_bStopStream;

	EnumSignal m_eSignal;
	
	CWatchTimer m_csKeepaliveTimer; //���鶨ʱ��	
	StruPlayStatus m_stPlayStatus;  //��ǰ����״̬
	INT32 m_eTransModel; //����ģʽ GSP �Ĳ���ģʽ


	CRefSource *m_pRefSource; //���õ�����Դ


	CSdpParser m_csSdp; //sdp ����


	EnumRTPPayloadType m_eOutRtpPlayloadType; 	

	CStreamPackConvert   *m_pPkgCvt;  //��ʽת����
	EnumStreamPackingType m_eInputStreamPkt; //�������ķ�װ����
	EnumStreamPackingType m_eOutputStreamPkt; //��������������	
	double m_fSpeed; //��ǰ�ٶ�
	BOOL m_bFirstFrame;

	UINT m_iSNSeq; //SN ����
	INT64 m_iFileBegin; //�ļ�ʱ��
	INT64 m_iFileEnd; //�ļ�ʱ��

	CGSString m_strRemoteRtpIp; //�Զ� Rtp Ip
	int m_iRemoteRtpPort; //�Զ� rtp port
	CRtpUdpSender *m_pRtpSender;
	
	UINT32 m_iSendKeySeq;

	BOOL m_bCtrlIng;

	BOOL m_bPlayEnd;

	CGSString m_strUserAgent;

	BOOL m_bSteup;
public:
	CRtspSrvSession( CRtspServer *pProServer);
	virtual ~CRtspSrvSession(void);

	
	EnumErrno Init( CISocket *pRtspTcpSocket );


	/*
	*********************************************************************
	*
	*@brief :  �ӿ�
	*
	*********************************************************************
	*/
	virtual void DeleteBefore(void); //ɾ��ǰ����
	virtual void Start(void);

	

	void OnRtpContentKeepalive(void); //��RTP Content ����
private :
	//RTSP TCP �̳߳� �ص�
//	void OnRtspTcpTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );
	
	// RTSP TCP ����Socket ����ص� 
	void *OnRtspTcpSocketEvent(	CISocket *pSocket, 
		EnumSocketEvent iEvt,
		void *pParam, void *pParamExt );

	void OnTimerEvent( CWatchTimer *pTimer );
	
	//����TCP д�¼�
	BOOL HandleRtspTcpSocketWriteEvent( StruAsyncSendEvent *pEvt );

	//����TCP ���¼�
	BOOL HandleRtspTcpSocketReadEvent(CGSPBuffer *pBuffer);


// 	//�����
 	void HandleRtspTcpCommand( CRtspProPacket *pPacket );


	void CloseOfBadRequest(void);
	void HandleOptions( CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandleDescribe( CRtspHeader *pHeader , CRtspProPacket *pRcvPacket );
	void HandleTeardown(CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandleSetup( CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandlePlay( CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandlePause(CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandleGetParameter( CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );
	void HandleSetParameter(CRtspHeader *pHeader, CRtspProPacket *pRcvPacket);
	void HandleInvalidCmmand(CRtspHeader *pHeader, CRtspProPacket *pRcvPacket );


	void ResponseError(const CRtspHeader *pHeader, EnumResponseStatus eErrno );



	void Signal(EnumSignal eSig );


	//ֹͣ˵�в���
	void StopAllAction(void);

	//����Դ�¼�
	EnumErrno OnSourceEvent(CRefSource *pRefSource,
							CRefSource::EnumRefSourceEvent eEvt, void *pParam);

	//��������֡
	EnumErrno SendStreamFrame( CFrameCache* pFrame );


	void SendRtspCmdPacket( CRtspProPacket *pPacket );

	//��������Դ
	EnumErrno BuildSource(const CUri &csUri, const CRtspHeader *pHeader, const CGSString &strReqSdp  );

	//��ȡ����Դ��SDP ����
	CGSString GetSDPDescription( CUri &csUri );

};

} //end namespace RTSP

} //end namespace GSP



#endif //end _GS_H_RTSPSRVSESSION_H_
