/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPSRVSESSION.H
Author :  ������
Version : 1.0.0.1
Date: 2012/11/7 14:08
Description: SIP �Ự
********************************************
*/

#ifndef _GS_H_SIPSRVSESSION_H_
#define _GS_H_SIPSRVSESSION_H_


#include "../IProServer.h"
#include "ThreadPool.h"
#include "MainLoop.h"
#include "ISocket.h"
#include "../RefSource.h"
#include "SipStack.h"
#include "RTP/RtpNet.h"
#include "RTP/SdpParser.h"
#include "SIP/SIPParser.h"
#include "SIP/ES2PS.h"
#include "SipServer.h"
#include "MediaInfo.h"
#include "StreamPackConvert.h"

using namespace GSP::RTP;

namespace GSP
{

namespace SIP
{



class CStreamEncoder;



class CSipSrvSession :
	public CISrvSession
{

private :
	typedef enum
	{
		eSIG_NONE = 0,
		eSIG_RELEASE, //ɾ��
		eSIG_REMOTE_DISCONNECT, //�Զ˹ر�����
		eSIG_ASSERT, //�쳣
		eSIG_REMOTE_CLOSE, //�Զ˹ر�
	}EnumSignal;

	CSipServer *m_pSipSrv;
	
	CGSPThreadPool m_csHandleSipDataTask; // ����Sip ���� �� ����Ϊ  StruSipData *pData

	StruPlayStatus m_stPlayStatus;  //��ǰ����״̬

	INT32 m_eTransModel; //����ģʽ

	CRefSource *m_pRefSource; //���õ�����Դ

	CSdpParser m_csSdp; //sdp ����

	C28181ParserSubject m_csSubject;

	CRtpUdpSender *m_pRtpSender;

	EnumSignal m_eSignal;

	CGSWRMutex m_csWRMutex;

	UINT m_iKeepalivePlugs;
	CWatchTimer m_csKeepaliveTimer; //���鶨ʱ��
	
	CGSString m_strRemoteRtpIp; //�Զ� Rtp Ip
	int m_iRemoteRtpPort; //�Զ� rtp port

	UINT32 m_iPSSRC;
	UINT32 m_iYSSRC;

	EnumRTPPayloadType m_eOutRtpPlayloadType; 	
	


	CFrameCache *m_pSysHeaderFrame; //ϵͳͷ
	INT m_iSendSysHeaderTicks; //������Ϣͷ����, UDP ����ʱ ÿ 90 ֡����һ��


	StruSipDialogKey m_stInviteDlgKey; //�Ի���Key

	UINT m_iSNSeq; //SN ����
	INT64 m_iFileBegin; //�ļ�ʱ��
	INT64 m_iFileEnd; //�ļ�ʱ��


	//���װ
	CStreamPackConvert   *m_pPkgCvt;  //��ʽת����
	EnumStreamPackingType m_eInputStreamPkt; //�������ķ�װ����
	EnumStreamPackingType m_eOutputStreamPkt; //��������������	


	double m_fSpeed; //��ǰ�ٶ�

	BOOL m_bFirstFrame;

	BOOL m_bTestRtcp;
public :
	CGSString m_strInviteCallId;
	SipSessionConnecter m_hSipCnner;

	CSipSrvSession(CSipServer *pParent);
	virtual ~CSipSrvSession(void);

	/*
	*********************************************************************
	*
	*@brief :  �ӿ�
	*
	*********************************************************************
	*/
	virtual void DeleteBefore(void); //ɾ��ǰ����
	virtual void Start(void);


	BOOL Init(SipSessionConnecter cnner, const CGSString &strInviteCallId,const StruSipConnnectInfo &stNetInfo );


	void OnDisconnectEvent(void); //����Ͽ�


	void HandleSipData(StruSipData *pData);


private :

	//�����Ǵ���Sip ���ݰ�
	void HandleSipDataOfError(StruSipData *pData, const char *czError  );

	void SendBye(void);


	//����SIP ���̳߳ػص�
	void OnHandleSipDataTaskPoolEvent( CObjThreadPool *pTkPool, StruSipData *pData );


	//����Invite ����
	void HandleRequestInvite(StruSipData *pData);

	//����Ack ����
	void HandleRequestAck(StruSipData *pData);

	//����Bye ����
	void HandleRequestBye(StruSipData *pData);

	//����MANRTSP ����
	void HandleMANSRTSP(StruSipData *pData);

	//��������Դ
	BOOL BuildSource( const CGSString &strSourceID, CSdpParser *pRequestSdp ); 


	void Signal(EnumSignal eSig );


	//ֹͣ˵�в���
	void StopAllAction(void);


	void OnTimerEvent( CWatchTimer *pTimer );

	EnumErrno OnSourceEvent(CRefSource *pRefSource,
		CRefSource::EnumRefSourceEvent eEvt, void *pParam);

	EnumErrno SendStreamFrame( CFrameCache* pFrame );
// 
// 	EnumErrno SendPSFrame( CFrameCache* pFrame );
// 
// 	EnumErrno SendNonePSFrame(CFrameCache* pFrame);
// 
// 	EnumErrno SendConvertToStandartFrame( CFrameCache* pFrame, int isNonePS );

	//���ͽ�������				
	void SendEndMessage(void);

	//Rtp �����̳߳� �ص�
	void OnRtpReaderEvent( EnumRtpNetEvent eEvt, void *pEvtArgs );

};


} //end namespace SIP


} //end namespace GSP

#endif //end _GS_H_SIPSRVSESSION_H_
