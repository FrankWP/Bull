/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : PRORTSPCHANNEL.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/4/20 14:04
Description: RTSP �ͻ���ͨ��Э���ʵ��
********************************************
*/

#ifndef _GS_H_PRORTSPCHANNEL_H_
#define _GS_H_PRORTSPCHANNEL_H_


#include "../IProChannel.h"
#include "MySocket.h"
#include "MainLoop.h"
#include "RTSPStru.h"



namespace GSP
{
	class  CClientChannel;
	class  CClientSection;

	namespace RTSP
	{


		class CRtspTcpDecoder;
		class CRtspIPacket;
		class CRtspHeader;
		class CRtspSyncWaiter;

#define CLICHN_KEEPALIVE_TIMER_ID 1
#define CLICHN_ACTIVE_TIMER_ID  2
#define CLICHN_CLOSE_TIMER_ID  3



class CRtspChannel :
    public CIProChannel
{
public:
    typedef enum
    {
       MY_ST_INIT,
       MY_ST_READY,
       MY_ST_WREQUEST,
       MY_ST_PLAYING,
       MY_ST_ERR,
       MY_ST_ASSERT,
    }EnumMyStatus;

    typedef enum
    {
        CLOSE_NONE,
        CLOSE_SUCCESS,    //����
        CLOSE_EINVALID_DATA,   //���յ��Ƿ�����
        CLOSE_EILLEGAL_OPERATION, //�Ƿ�����
        CLOSE_EIO, //IO����
        CLOSE_EPERMIAT, //��Ȩ��
        CLOSE_ASSERT,
        CLOSE_REMOTE, //Զ�˹ر�
        CLOSE_EREQUEST,
        CLOSE_EKEEPALIVE, //KEEPALIVE ��ʱ
    }EnumCloseType;

private :
		
	CClientChannel *m_pParent;
	CISocket *m_pRtspTcpSocket;
	const UINT32 m_iAutoID;
	
	GSAtomicInter  m_iCTagSequence; //����CSeq

	CGSWRMutex m_csAsyncWRMutex; // m_csAsyncCmdList ͬ����
	CList m_csAsyncCmdList;  //�洢���� CRtspSyncWaiter*
	

	BOOL m_bRtspTcpSending;    //��׼TCP �Ƿ����ڷ���
	CList m_csRtspTcpWaitSendQueue;  //Tcp�ȴ��������ݵĶ���, ����Ϊ CIPacket *

	BOOL m_bRtspWaitSendFinish;

	CGSPThreadPool m_csRtspTcpRcvTask; //������յ��������߳�
	CRtspTcpDecoder *m_pRtspTcpDecoder; //TCP ��Э�����

	

	INT m_iMaxKeepaliveTimeouts;

	CWatchTimer m_csSendKeepaliveTimer; //����keepalive 

	CWatchTimer m_csAsyncWaiterTimer; //ͬ����ⶨʱ��

	CWatchTimer m_csKeepaliveTestTimer; //�������

	INT m_iKeepalivePlugs; //����������
	INT m_iCurSetupChn;
	CGSWRMutex m_csWRMutex;
	
public  :
    //�����ǽӿ�ʵ��
	EnumErrno Open(const CUri &csUri, BOOL bAsyn, INT iTimeouts);
	EnumErrno Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts);
	virtual void DestoryBefore(void);

	virtual EnumErrno FlowCtrl( BOOL bStart );

	/*
	*********************************************************************
	*
	*@brief : �����ӽӿ�
	*
	*********************************************************************
	*/
	CRtspChannel(CClientChannel *pParent);

	virtual ~CRtspChannel(void);


	void OnAsyncWaiterEvent(CGspSyncWaiter *pWaiter);

private :
	// TCP ����Socket ����ص� 
	void *OnRtspTcpSocketEvent(	CISocket *pSocket, 
							EnumSocketEvent iEvt,
							void *pParam, void *pParamExt );

	//����TCP д�¼�
	CIPacket *HandleRtspTcpSocketWriteEvent( CIPacket *pPacket );

	//����TCP ���¼�
	BOOL HandleRtspTcpSocketReadEvent(CRefBuffer *pBuffer);


	//TCP �����̳߳� �ص�
	void OnRtspTcpRcvTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );



	void WakeupAsync(EnumRTSPComandMask iCommandID, INT32 iCSeq, EnumErrno eErrno );

	//��ʱ���ص�
	void OnTimerEvent( CWatchTimer *pTimer );

    EnumErrno FlowCtrl( BOOL bStart );


	EnumErrno ParserRequest( EnumRTSPComandMask eCmdJD,  CRtspIPacket **ppResult, INT32 &iCSeq );

	EnumErrno SendRtspCmdPacket( CRtspIPacket *pPacket );

	EnumErrno ParserCtrl(const StruGSPCmdCtrl &stGssCtrl, CRtspIPacket **ppResult, INT32 &iCSeq);

	void HandleRtspTcpPacket(CRtspIPacket *pPacket);

	CRtspSyncWaiter *GetSyncWaiter( INT32 iCSeq );


	void HandleOptionsResponse(CRtspSyncWaiter *pAsync, CRtspHeader *PHeader);
	void HandleDescribeResponse(CRtspSyncWaiter *pAsync, CRtspHeader *PHeader, 
								const CGSString &strContent);
	void HandleSetupResponse(CRtspSyncWaiter *pAsync, CRtspHeader *pHeader);
};

} //end namespace GSP

} //end namespace RTSP

#endif //end _GS_H_PRORTSPCHANNEL_H_
