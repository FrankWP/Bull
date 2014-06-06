/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : GSPSRVSESSION.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/20 10:56
Description: GSP Э��Ự
********************************************
*/

#ifndef _GS_H_GSPSRVSESSION_H_
#define _GS_H_GSPSRVSESSION_H_

#include "../IProServer.h"
#include "ThreadPool.h"
#include "MainLoop.h"
#include "ISocket.h"
#include "../RefSource.h"
#include "GSPMemory.h"
#include "CircleQueue.h"
#include "RTP/RtpNet.h"
#include "AsyncTcpSrvSocket.h"


namespace GSP
{
class CGspServer;
class CGspTcpDecoder;
class CGspCommand;
class CIMediaInfo;



class CGspSrvSession :
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

	//״̬
	typedef enum
	{
		eST_NONE = 0,   //��ʼ
		eST_WAIT_REQUEST, //�ȴ�URI����
		eST_WAIT_START, //�ȴ���ʼ
		eST_PLAYING, //������
		eST_ASSERT, //�쳣
	}EnumStatus;

	static const UINT32 INVALID_COMMAND_TAG = MAX_UINT32;

	CGspServer *m_pGspServer;
	CGspTcpDecoder *m_pTcpDecoder; //TCP ��Э�����
//	CGSPThreadPool m_csTcpTask; // TCP���ݴ����̳߳�, Ϊ CRefBuffer *

	CGSWRMutex m_csWRMutex;
	CWatchTimer m_csKeepaliveTimer; //���鶨ʱ��	
	INT m_iKeepalivePlugs;
	CWatchTimer m_csSendKeepaliveTimer; //����Keepalive Timer 
	CISocket *m_pTcpSocket;
	CGSPString m_strURI;

	
	BOOL m_bTcpSending;    //��׼TCP �Ƿ����ڷ���
	CList m_csTcpWaitSendQueue;  //Tcp�ȴ��������ݵĶ���, ����ΪCProFrame *
	INT64 m_iWaitSendSize; //�ȴ����͵����ݴ�С


	CList m_csStreamWaitSendQueue;  //���ȴ��������ݵĶ���, ����ΪCProFrame *�� ��ʹ��TCP ģʽ��Ч	
	INT64 m_iWaitSendStreamSize;
	BOOL m_bStreamTcpSending;    //��TCP �Ƿ����ڷ���

	EnumStatus m_eStatus;

	CRefSource *m_pRefSource; //���õ�����Դ

	StruPlayStatus m_stPlayStatus;  //��ǰ����״̬

	INT32 m_eTransModel; //����ģʽ

	StruGSPPacketHeader m_stProCmdHeader; //�����Э��ͷ
	StruGSPPacketHeader m_vProStreamHeader[GSP_MAX_MEDIA_CHANNELS]; //�����Э��ͷ

	GSAtomicInter m_iTagSequence; //���������

	BOOL m_bWaitSendFinish; //�ȴ��������
	
	BOOL m_bStopStream;  //ͣ����
	
	EnumSignal m_eSignal;

	

	BOOL  m_bCtrlIng; //��ǰ���ڽ��п���, ��֤CTRL ����ǰ������������
	BOOL  m_bPlayCtrlIng; //��ǰ���ڽ��в��ſ���, ��֤CTRL ����ǰ������������

	INT m_eCtrlIngCmd; //��ǰ��������

	

	BOOL m_bPlayEnd;

	UINT32 m_iSendKeySeq;

	INT m_iGspVersion; 


	INT m_eGspStreamTransMode; //������ģʽ

	

	RTP::CRtpUdpSender  *m_pRtpUdpSender; //ʹ��RTP_UDP ģʽ��Ч
	CISocket *m_pTcpStreamSender;  //ʹ��RTP_TCP ʱ��Ч
	CAsyncTcpSrvSocket *m_pAsyncSrvSocket; //ʹ��RTP_TCP ʱ��Ч
	BOOL m_bFirstFrame; //��һ֡ 
	

	CFrameCache* m_pSysFrame; //ϵͳͷ
	//std::set<UINT32> m_setWaitingWrite;

	int m_bFlowCtrl; //�������� 
public:
	CGspSrvSession( CGspServer *pProServer);
	virtual ~CGspSrvSession(void);

	
	EnumErrno Init( CISocket *pSocket );


	/*
	*********************************************************************
	*
	*@brief :  �ӿ�
	*
	*********************************************************************
	*/
	virtual void DeleteBefore(void); //ɾ��ǰ����
	virtual void Start(void);

	
private :

	
	// TCP ����Socket ����ص� 
	void *OnTcpSocketEvent(	CISocket *pSocket, 
		EnumSocketEvent iEvt,
		void *pParam, void *pParamExt );

	// Stream TCP ����Socket ����ص� 
	void *OnStreamTcpSocketEvent(	CISocket *pSocket, 
		EnumSocketEvent iEvt,
		void *pParam, void *pParamExt );

	//Async  TCP Server ����Socket ����ص� 
	void *OnATcpSrvSocketEvent(	CISocket *pSocket, 
		EnumSocketEvent iEvt,
		void *pParam, void *pParamExt );

	void OnTimerEvent( CWatchTimer *pTimer );
	
	//����TCP д�¼�
	void *HandleTcpSocketWriteEvent( const StruAsyncSendEvent *pEvt );

	//����TCP ���¼�
	BOOL HandleTcpSocketReadEvent(CGSPBuffer *pBuffer);


	//�����
	void HandleCommand( CGspCommand *pCommand );


	//���� URI �������� GSP_CMD_ID_REQUEST
	void HandleRequest( CGspCommand *pCommand  );
	
	//���� RTP UDP �������� GSP_UDP_SET_SETUP
	void HandleUdpSetupRequest( CGspCommand *pCommand );

	//����Keepalive ���� GSP_CMD_ID_KEEPAVLIE
	void HandleKeepalive( CGspCommand *pCommand   );

	//�����ش�����  GSP_CMD_ID_RESEND
	void HandleRequestResend(CGspCommand *pCommand  );  

	//�����������  GSP_CMD_ID_CTRL
	void HandleCtrl(CGspCommand *pCommand   );

	//����״̬�������� GSP_CMD_ID_REQUEST_STATUS
	void HandleRequestStatus(CGspCommand *pCommand  );

	//����ر�ǿ�� GSP_CMD_ID_CLOSE
	void HandleRequestClose(CGspCommand *pCommand  );

	//�����쳣�ر�ǿ��, ���ûظ� GSP_CMD_ID_ASSERT_AND_CLOSE
	void HandleRemoteAssert(CGspCommand *pCommand  );

	//ͳһ����û�д������ʶ������
	void HandleUnknownCommand( CGspCommand *pCommand  );


	void SendCommand(EnumGSPCommandID eCommandID, const void *pCommandPlayload, UINT iSize, 
					 UINT32 iTag = INVALID_COMMAND_TAG);


	void SendRequestResponse( UINT16 iErrno,UINT32 iTag, UINT32 iAbilities = 0,
							const CIMediaInfo  *pMediaInfo = NULL);

	void Signal(EnumSignal eSig );


	//ֹͣ˵�в���
	void StopAllAction(void);

	//����Դ�¼�
	EnumErrno OnSourceEvent(CRefSource *pRefSource,
							CRefSource::EnumRefSourceEvent eEvt, void *pParam);

	//����������֡
	EnumErrno SendStreamFrame( CFrameCache* pFrame );

	//����ͨ������������
	EnumErrno SendMultiTcpStreamFrame(CFrameCache* pFrame, int iChnNo);

	//RTPUDP ͨ����������
	EnumErrno SendRtpUdpStreamFrame(CFrameCache* pFrame, int iChnNo);


	//���� TCP ͨ����������
	EnumErrno SendTcpStreamFrame(CFrameCache* pFrame, int iChnNo);

};

} //end namespace GSP

#endif //end _GS_H_GSPSRVSESSION_H_
