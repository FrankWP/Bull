/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : PROSIPCHANNEL.H
Author :  ������
Version : 1.0.0.1
Date: 2012/11/13 16:17
Description: SIP Э��ͨ�� 
********************************************
*/

#ifndef _GS_H_PROSIPCHANNEL_H_
#define _GS_H_PROSIPCHANNEL_H_



#include "../IProChannel.h"
#include "ISocket.h"
#include "MainLoop.h"
#include "RTP/RtpStru.h"
#include "SipStack.h"
#include "RTP/RtpNet.h"
#include "RTP/SdpParser.h"
#include "SIP/SIPParser.h"
#include "SIP/ES2PS.h"
#include "GSPMemory.h"
#include "StreamPackConvert.h"

using namespace GSP::RTP;
namespace GSP
{
	class  CClientChannel;
	class  CClientSection;
	

	namespace SIP
	{
//#define TEST_SERVER

	class  CProSipClientService;
	class  CStreamEncoder;
#define CLICHN_KEEPALIVE_TIMER_ID 1
#define CLICHN_ACTIVE_TIMER_ID  2
#define CLICHN_CLOSE_TIMER_ID  3

		typedef UINT32 SipClientConnecter;  //sip ͨ��		
		#define INVALID_SIP_CLIENT_CNNER 0
		

class CSipChannel :
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
	static GSAtomicInter s_iYSSRCSeq;
	CClientChannel *m_pParent;
	CProSipClientService *m_pSipSrv;
	const UINT32 m_iAutoID;
	UINT32 m_iYSSRC;
	
	
	GSAtomicInter  m_iCTagSequence; //����CSeq
	

	
	INT m_iMaxKeepaliveTimeouts;

	CWatchTimer m_csSendKeepaliveTimer; //����keepalive 

	CWatchTimer m_csKeepaliveTestTimer; //�������

	INT m_iKeepalivePlugs; //����������

	CGSWRMutex m_csWRMutex;



	C28181ParserSubject m_csSubject;
	
	CRtpUdpReader *m_pRtpReader;
	CSdpParser m_csSdp; //sdp ����

	BOOL m_bStreamStart; //�Ƿ�ʼ������

	BOOL m_bSendAck;


	CGSString m_strUsername;
	CGSString m_strDevId;

	UINT16 m_iKpSN;

	EnumRTPPayloadType m_eRtpPT; 

	EnumGSCodeID m_eInputCodeId;

 	
	
	INT64  m_iBeginTimestamp; //��ǰ��ʼ�����ʱ����� ��λ ��,utcʱ��
	UINT32 m_iTimestamp; //��� m_iBeginTimestamp ��ƫ��,��λ����

	UINT64 m_iLastPTS; //PS ��ʱ�����һ֡��ʱ���

	INT64 m_iFileLengthTv; //�ļ�ʱ��
	INT64 m_iFileStartTv; //�ļ���ʼʱ��, �طŻ�����
	INT64 m_iLastPos; //���һ�εĽ���

	BOOL m_bWouldSendByte; //��Ҫ����Byte

	StruSipDialogKey m_stInviteDlgKey;

	int m_iPSSampleRate; 

	double m_fSpeed;

	CStreamPackConvert *m_pPkgCvt;

	EnumStreamPackingType m_eInputPkgType;
	EnumStreamPackingType m_eOutputPkgType;  //Ĭ�ϵ��� eSTREAM_PKG_GSPS

	CStreamEncoder *m_pDecoder; //ý���װ�ض���

	BOOL m_bTestRtcp; //���RTCP ��
public :
	SipClientConnecter m_hCliCnner;
	CGSString m_strInviteCallId;
	BOOL m_bTransProxy; //�������ģʽ
#ifdef TEST_SERVER
	bool m_bTEST_SERVER;
#endif

public  :
    //�����ǽӿ�ʵ��
	INLINE CClientChannel* GetParent(void)
	{
		return m_pParent;
	}
	virtual EnumErrno Open(const CUri &csUri, BOOL bAsyn, INT iTimeouts);
	virtual EnumErrno Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts);
	virtual EnumErrno CtrlOfManstrsp(const char *czMansrtsp, BOOL bAsync,INT iTimeouts, StruGSPCmdCtrl &stGspCtrl);
	virtual void DestoryBefore(void);

	virtual EnumErrno FlowCtrl( BOOL bStart );


	virtual CGSString GetSdp(void)
	{
		if( !m_bSendAck )
		{
			return CGSString();
		}
		return m_csSdp.Serial();
	}

	/*
	*********************************************************************
	*
	*@brief : �����ӽӿ�
	*
	*********************************************************************
	*/
	CSipChannel(CClientChannel *pParent);

	virtual ~CSipChannel(void);


	//����Ͽ�
	void OnDisconnectEvent(void); 

	// SIP ������ ���ݰ�
	void HandleSipData(StruSipData *pData); 




private :


	



	//��ʱ���ص�
	void OnTimerEvent( CWatchTimer *pTimer );


	//RTP �������ݻص�
	void OnRtpReaderEvent( EnumRtpNetEvent eEvt, void *pEvtArgs );

	EnumErrno SendAck(void); //���� Ack ����
	EnumErrno SendMANSRTSP( const StruGSPCmdCtrl &stCtrl ); //���� MANSRTSP
	EnumErrno SendBye(void); //���� Bye ����


	//�����Ǵ���Sip ���ݰ�
	void ResponseBadRequest(StruSipData *pData,int iSipErrno, const char *czError  );

	void ResponseSimple( StruSipData *pData,int iSipErrno, const char *czInfo  );

	void SendKeepalive( BOOL bResponse );

	//������
	void HandleRtpStreamFrame(CFrameCache *pFrame);


	UINT64 IntervalOfPTS(UINT64 iStart, UINT64 iNow);


	EnumErrno ConvertFrameInfo(CFrameCache *pFrame,  StruFrameInfo &stInfo );

};


//Sip �ͻ��� ����
class CProSipClientService : public CGSPObject
{
public :
	CClientSection *m_pParent;
	SipServiceHandle m_hSipSrv;
private :
	CGSString m_strSipUdpBindIp;
	INT m_iSipUdpPort;
	CGSString m_strSipServerName;
	INT m_iRtpUdpPortBegin;
	INT m_iRtpUdpPortEnd;

	BOOL m_bForceSendRTCP;  //ʹ������� Ĭ�ϲ�ʹ��

	BOOL m_bReady;

	typedef struct _StruMyChnInfo
	{
		CSipChannel *pChn;		
		StruSipDialogKey stInviteDlgKey;
		_StruMyChnInfo(void)
		{
			pChn = NULL;			
			bzero(&stInviteDlgKey, sizeof(stInviteDlgKey));
		}
	}StruMyChnInfo;


	typedef std::map<CGSString  /*call-id*/ , StruMyChnInfo > CMapMyChannel;
	typedef struct _StruMyCnnerInfo
	{
		CGSString strRemoteHost;
		int iRemotePort;
		CGSString strUserName;
		CGSString strPassword;

		time_t tvLoginTv;
		UINT64 tvLastKeepalive;
		SipSessionHandle hHandle;
		SipClientConnecter hCnner;
		CMapMyChannel mapMyChannels;
		BOOL m_bRegistered;
		BOOL m_iRegistering;
		GSAtomicInter m_iBusy;
#ifdef TEST_SERVER
		bool m_bTEST_SERVER;
#endif
		_StruMyCnnerInfo() : hHandle(SIP_INVALID_HANDLE), hCnner(INVALID_SIP_CLIENT_CNNER)
		{
			mapMyChannels.clear();
			m_bRegistered = FALSE;
			tvLoginTv = 0;
			iRemotePort = -1;
			tvLastKeepalive = 0;
			m_iRegistering = FALSE;
			m_iBusy = 0;
#ifdef TEST_SERVER
			m_bTEST_SERVER = false;
#endif
		}
	}StruMyCnnerInfo;


	CGSWRMutex m_mutexSipClients;
	SipClientConnecter m_seqConnecter;
	typedef std::map<SipClientConnecter , StruMyCnnerInfo> CMapSipClient;
	CMapSipClient m_mapSipClients;
	CMapMyChannel m_mapSipClientsProxy; //����ģʽ


	CWatchTimer m_csKeepaliveTestTimer; //�������

public :


	CProSipClientService(CClientSection *pParent );
	virtual ~CProSipClientService(void);

	EnumErrno Init( CConfigFile &cfCfg);

	EnumErrno Start(void);

	EnumErrno Stop(void);

	EnumErrno Uninit(void);

	BOOL IsForceRTCPKeepalive(void) const
	{
		return m_bForceSendRTCP;
	}


	void SipDisconnectChannel(CSipChannel *pChannel);
	EnumErrno SipCreateConnecter( CSipChannel *pChannel,const CGSString &strDevId,
								  const CGSString &strRemoteHost,int iRemotePort,
								  const CGSString &strUserName, 
								  const  CGSString &strPassword,
								  const StruSipData *pInviteReq, 
								  StruSipData *pResInviteRes);

	EnumErrno SessionSendSipData( CSipChannel *pChannel, 
									const StruSipData *pSendData, StruSipData *pRes,
									int iTimeouts,  StruSipDialogKey *pOutDlgKey );

	EnumErrno OnTransRecvData(const StruSipData *pSendData);

	void SipConnecterNetError(CSipChannel *pChannel, const CGSString &strDevId,
		const CGSString &strRemoteHost,int iRemotePort,
		const CGSString &strUserName, 
		const  CGSString &strPassword, INT eSipErrno);

private :
	//�հ�
	static void   SipSIPPacketCallback(SipServiceHandle hService, 
		SipSessionHandle hSession,
		StruSipData *pData);
	static EnumSipErrorCode SipClientConnectCallback( SipServiceHandle hService,
		SipSessionHandle hNewSession );
	//�Ͽ�
	static void SipClientDisconnectCallback(SipServiceHandle hService, SipSessionHandle hSession);


	EnumSipErrorCode OnSipClientConnectEvent(SipSessionHandle hNewSession );
	void OnSipClientDisconnectEvent(SipSessionHandle hSession);
	void OnSipPacketEvent(SipSessionHandle hSession, StruSipData *pData);

	StruMyCnnerInfo* RefCnnerInfo(SipClientConnecter hCnner );
	INLINE void UnrefCnnerInfo( StruMyCnnerInfo*pInfo)
	{
		if( pInfo)
		{
			AtomicInterDec(pInfo->m_iBusy);
		}
	}

	//��ʱ���ص�
	void OnTimerEvent( CWatchTimer *pTimer );

	EnumErrno SessionSendProxySipData( CSipChannel *pChannel, 
		const StruSipData *pSendData, StruSipData *pRes,
		int iTimeouts,  StruSipDialogKey *pOutDlgKey );

	

};


} //end namespace SIP

} //end namespace GSP



#endif //end _GS_H_PROSIPCHANNEL_H_
