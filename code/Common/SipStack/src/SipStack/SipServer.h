/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPSERVER.H
Author :  ������
Version : 1.0.0.1
Date: 2012/9/24 17:54
Description: SIP �������
********************************************
*/

#ifndef _GS_H_SIPSERVER_H_
#define _GS_H_SIPSERVER_H_
#include "SIPCommonDef.h"
#include "SipStack.h"

namespace GSSIP
{
class CSipServer;

typedef struct _StruSessionPri
{
	CSipServer *pSrv;
	long iCnnId;
	void *pUserData;
	_StruSessionPri() : pSrv(NULL), iCnnId(0),pUserData(NULL)
	{

	}
}StruSessionPri;


#define INVALID_REG_ID -1

class CRefObject
{
private :
	GSAtomicInter m_iRefs;
public :
	void Ref(void)
	{
		AtomicInterInc(m_iRefs);
	}

	void Unref(void)
	{
		if(0 == AtomicInterDec(m_iRefs) )
		{
			delete this;
		}

	}
protected :
	CRefObject(void) : m_iRefs(1)
	{

	}
	virtual ~CRefObject(void)
	{
		GS_ASSERT(m_iRefs==0);
	}
private :

	

	CRefObject(const CRefObject &csDest);
	CRefObject &operator=(const CRefObject &csDest);
};

//��λһ���ͻ�������
class CClientConnecter
{
public :
	CGSString m_strRemoteIp;
	INT m_iRemotePort;

	CGSString m_strFromUser;
	CGSString m_strToUser;
	
	INT64 m_iCnnId;  //�ظ���ʹ��
	bool m_bRequest;

	CClientConnecter(void)
	{
		Reset();
	}

	~CClientConnecter(void)
	{

	}
	void Reset(void)
	{
		m_strRemoteIp.clear();
		m_iRemotePort = -1;
		m_iCnnId = -1;
		m_bRequest = false;
		m_strFromUser.clear();
		m_strToUser.clear();
	}
	bool Parser( osip_message_t *pMsg, bool bRequest);	
};

class CClientInfo
{
private :
	static GSAtomicInter m_iDlgSeq;
	GSAtomicInter m_iRefCnts;
	
public :

	CGSString m_strRemoteSrvName; 
	CGSString m_strLocalSrvName;


	CSipServer *m_pServer;
	long m_iCnnID; //����ID
	int m_iRegId; //ע���ID
	int m_bRegisted; // �Ѿ�ע��ɹ�
	CGSString m_strAuthorization; //ӡ֤
	CGSString m_strUsername;
	CGSString m_strPasswrd;

	//������Ϣ, ע��ʱȷ��
	EnumSipConnectType m_eCnnType;
	CGSString m_strRemoteIp;
	int m_iRemotePort;


	CGSString m_strLocalIp;
	INT m_iLocalPort;



	void *m_pUserData; //�û�����

	CGSString m_strCallId; //���� ��½�� �� CallId
	CGSString m_strRemoteSipSrvName; // �Զ�Sip��������


	CGSString m_strNonce;  //ע��ʱʹ��
	CGSString m_strRealm; //ע��ʱʹ��

	INT m_iExpiresRegister;

	StruSessionPri *m_pSnPri;

	BOOL m_bTypeClient; //��׼�Ƿ���˻��� �ͻ���, true Ϊ �ͻ���

	CGSString m_strRegViaUri; //ע��ʱ��IP �˿�
	
	time_t m_tvRegisted; //��½ʱ��

	UINT64 m_tvLastActive; //�����ʱ��

	BOOL m_bOOffUnresiter; //�Ƿ��ֹ����ע������

	BOOL m_b28181Username; //�Ƿ�Ϊ28181 �û���

	CClientInfo(CSipServer *pServer, int iRegId)
	{
		m_iRefCnts = 1;
		m_pServer = pServer;
		m_iRegId = iRegId;
		m_iCnnID = AtomicInterInc(m_iDlgSeq);
		m_bRegisted = 0;
		m_strAuthorization.clear();
		m_strUsername = "admin";
		m_strPasswrd = "admin";
		m_eCnnType = eSIP_CONNECT_UDP;
		m_strRemoteIp.clear();
		m_iRemotePort = 0;
		m_strLocalIp.clear();
		m_iLocalPort = 0;

		m_pUserData = NULL;
		m_strCallId = "null";
		m_strRemoteSipSrvName.clear();
		GSStrUtil::Format(m_strRealm, "\"%ul\"", (unsigned long) m_iCnnID);
		GSStrUtil::Format(m_strNonce, "\"%02x\"", (unsigned long) m_iCnnID);
		m_pSnPri = NULL;
		m_iExpiresRegister = 3600;
		m_bTypeClient = FALSE;
		m_tvRegisted = 0;
		m_strRemoteSrvName.clear();
		m_strLocalSrvName.clear();
		m_bOOffUnresiter = 0;
		m_b28181Username = FALSE;
		
	}

	

	CGSString MakeTag(const CGSString &strSipTag); //���� Tag


	INLINE BOOL IsRegister(void) const
	{
		return m_iRegId!=INVALID_REG_ID;
	}

	UINT32 CreateDlgId(void)
	{
		return (UINT32)AtomicInterInc(m_iDlgSeq);
	}
	void SetDlgKeyMember( StruSipDialogKey &stKey )
	{
		stKey.iCnnId = stKey.iCnnId;
	}

	void Ref(void)
	{
		AtomicInterInc(m_iRefCnts);

	}
	void Unref(void)
	{
		if( 0==AtomicInterDec(m_iRefCnts) )
		{
			delete this;
		}
	}

	class CAutoUnref
	{
	private :
		CClientInfo *m_pCli;
	public :
		CAutoUnref(CClientInfo *p ) : m_pCli(p)
		{

		}
		~CAutoUnref(void)
		{
			if( m_pCli )
			{
				m_pCli->Unref();
			}
		}


	};

	
private :
	~CClientInfo(void)
	{
		
	}
	CClientInfo(const CClientInfo &csDest)
	{
		*this = csDest;
	}
	CClientInfo &operator=(const CClientInfo &csDest)
	{
		if( this != &csDest )
		{
			m_pServer = csDest.m_pServer;
			m_iRegId = csDest.m_iRegId;
			m_iCnnID = csDest.m_iCnnID;
			m_bRegisted = csDest.m_bRegisted;
			m_strAuthorization= csDest.m_strAuthorization;
			m_strUsername= csDest.m_strUsername;
			m_strPasswrd= csDest.m_strPasswrd;
			m_eCnnType = csDest.m_eCnnType;
			m_strRemoteIp = csDest.m_strRemoteIp;
			m_iRemotePort = csDest.m_iRemotePort;
			m_strLocalIp= csDest.m_strLocalIp;
			m_iLocalPort = csDest.m_iLocalPort;
			m_pUserData = csDest.m_pUserData;
			m_strCallId = csDest.m_strCallId;
			m_strRemoteSipSrvName= csDest.m_strRemoteSipSrvName;
			m_strRealm= csDest.m_strRealm;
			m_strNonce= csDest.m_strNonce;
			m_pSnPri = csDest.m_pSnPri;
			m_iExpiresRegister= csDest.m_iExpiresRegister;
			m_bTypeClient= csDest.m_bTypeClient;
			m_tvRegisted = csDest.m_tvRegisted;
			m_strRemoteSrvName = csDest.m_strRemoteSrvName;
			m_strLocalSrvName = csDest.m_strLocalSrvName;
			m_bOOffUnresiter = csDest.m_bOOffUnresiter;
			m_b28181Username =  csDest.m_b28181Username;
		}
		return *this;
	}
	
	
};



#define INVALID_CNN_ID ((long)0)

typedef std::map< long , CClientInfo *> CMapClientOfCnnId;
typedef std::set<CClientInfo *> CSetClientOfNet;


typedef struct _StruNetIf
{
	CGSString strIp;
	
	CGSString strNetmask;
	INT iNetmask;

	_StruNetIf()
	{
		strIp.clear();
		strNetmask.clear();
		iNetmask = 0;
	}

}StruNetIf;

class CSipServer
{
private :
	CGSThread m_csThread;
	static GSAtomicInter s_iOSipRefs; //��׼osip ��ĳ�ʼ��
	static int s_bOSipInit; //�Ƿ��Ѿ���ʼ��

	StruSipListener m_stListener;

	CSetClientOfNet m_mapClientsOfNet; //�ͻ����б� key
	CMapClientOfCnnId m_mapClientsOfCnnId; // key cnn id
	CGSWRMutex m_wrMutex;

	eXosip_t *m_hEXosip;
	BOOL m_bStart;

	void *m_pUserContext; //�û�����

	EnumSipConnectType m_eListenCnnType;
	CGSString m_strListenIP;
	int m_iListenPort;

	CGSString m_strServerName; //����������


	int m_iExpiresRegister;

	std::vector<StruNetIf> m_vNetIf;
	class CResponseWaiter : public CRefObject
	{
		//�ظ��ȴ���

	public :
		EnumSipMethod m_eMethod;
		CGSString m_strSipCallId; // call id
		UINT32 m_iCSeq;
		CGSString m_strSipDlgId; // dialog id

		EnumSipErrorCode m_eErrno;
		CGSCondEx m_csCond;
		CGSMutex m_csMutex;
		StruSipData m_stResData;
		INT64 m_iTvStart;
		INT m_iTimeouts; //����


	public :
		static INLINE CResponseWaiter *Create(EnumSipMethod eMethod,
			const CGSString &strSipCallId, 
			UINT32 iCSeq, const char *czDlgId )
		{
			return new CResponseWaiter(eMethod , strSipCallId, iCSeq, czDlgId);
		}


		BOOL IsEqual(EnumSipMethod eMethod,
			const CGSString &strSipCallId, 
			UINT32 iCSeq, const char *czDlgId )
		{
			if( eMethod==m_eMethod  && strSipCallId==m_strSipCallId &&
				(iCSeq==m_iCSeq || eMethod==eSIP_METHOD_REGISTER) )
			{
				if( (m_strSipDlgId.empty() && czDlgId==NULL) || 
					(czDlgId!=NULL &&	0==m_strSipDlgId.compare(czDlgId)) )
				{
					return TRUE;
				}
			}
			return FALSE;
		}

		void Wakeup(const StruSipData *pData )
		{
			m_csMutex.Lock();
			m_eErrno = eSIP_RET_SUCCESS;
			::memcpy(&m_stResData, pData, sizeof(*pData));
			m_csCond.Signal();
			m_csMutex.Unlock();
		}

		EnumSipErrorCode WaitTimeout(StruSipData *pResult, int iTimeouts )
		{
			m_csMutex.Lock();
			m_eErrno = eSIP_RET_FAILURE;
			m_iTimeouts = iTimeouts;
			m_iTvStart = DoGetTickCount();
			m_csCond.WaitTimeout(&m_csMutex, m_iTimeouts);
			EnumSipErrorCode eRet = m_eErrno;
			if( eRet == eSIP_RET_SUCCESS )
			{
				::memcpy(pResult ,&m_stResData, sizeof(*pResult) );
			}
			else
			{
				eRet = eSIP_RET_E_TIMEOUT;
			}
			m_csMutex.Unlock();
			return eRet;
		}
	private :	
		CResponseWaiter(EnumSipMethod eMethod,
			const CGSString &strSipCallId, 
			UINT32 iCSeq, const char *czDlgId ) : 
		    CRefObject(),
			m_eMethod(eMethod),		   
			m_strSipCallId(strSipCallId),			
			m_iCSeq(iCSeq),
			m_strSipDlgId()
		{
			if( czDlgId )
			{
				m_strSipDlgId = czDlgId;
			}
			else
			{
				m_strSipDlgId.clear();
			}
			m_eErrno = eSIP_RET_FAILURE;
			m_iTimeouts = 1000;
			m_iTvStart = DoGetTickCount();
			bzero(&m_stResData, sizeof(m_stResData));
		}
		virtual ~CResponseWaiter(void)
		{
			
		}

	};

	std::set<CResponseWaiter *> m_setWaiter;

	time_t m_tvLast;
public:
	CSipServer(void);
	~CSipServer(void);

	

	EnumSipErrorCode Init(const StruSipListener &stListener );

	/*
	 *********************************************
	 Function : Start
	 Version : 1.0.0.1
	 Author : ������
	 DateTime : 2012/9/25 9:54
	 Description :  ��������
	 Input :  
	 Output : 
	 Return : 
	 Note :   ���Զ�ε���
	 *********************************************
	 */
	EnumSipErrorCode Start(EnumSipConnectType eCnnType,
							const char *czLocalIp, int iLocalPort);

	//ֹͣ����
	void Stop(void);

	void SetUserData(void *pData )
	{
		m_pUserContext = pData;
	}

	void *GetUserData(void)
	{
		return m_pUserContext;
	}

	void SetServerName(const char *czName )
	{
		m_strServerName = czName;
	}

	
	INLINE int GetLocalPort(void) const
	{
		return m_iListenPort;
	}


	void SetGateway(const char *czGetewayIPV4 );


	/*
	 *********************************************
	 Function : Connect
	 Version : 1.0.0.1
	 Author : ������
	 DateTime : 2012/9/27 14:46
	 Description :  ����Զ��
	 Input :  eCnnType ��������
	 Input : czRemoteHost �Զ˵�ַ
	 Input :  iRemotePort �Զ�IP
	 Input :  czLocalIp ���ذ󶨵�IP�� ���Ϊ NULL ϵͳ�Զ�ѡ��
	 Input :  iLocalPort ���ذ󶨵Ķ˿�, ��� Ϊ 0�� ϵͳָ��ѡ��
	 Output : pSnPri �����µ����ӿ��ƾ��
	 Return : 
	 Note :   
	 *********************************************
	 */
	EnumSipErrorCode Connect( EnumSipConnectType eCnnType, 
							  const char *czRemoteHost,int iRemotePort, 
							  const char *czRemoteSipServerName, 
							  StruSessionPri **pSnPri);


	void FreeSession(StruSessionPri *pSnPri);

	//���ؼ�����Ϣ
	EnumSipErrorCode SipService_GetListenInfo(StruSipConnnectInfo *pRes);

	

	//ע��
	EnumSipErrorCode SipSessionResigter(long iCnnId,
		const char *czUserName, 
		const char *czPassword, 
		StruSipData *pRes, int iTimeout, int iExpires=-1);


	EnumSipErrorCode SipSessionDisconnect(long iCnnId);

	

	void SipSessionRelease(long iCnnId);

	EnumSipErrorCode SipSessionSendMessage(long iCnnId,	const StruSipData *pSendData, 
										StruSipData *pRes,int iTimeouts, 
										StruSipDialogKey *pOutDlgKey);

	EnumSipErrorCode SipSessionReconnect(long iCnnId);

	const char *SipSessionAuthorizationUsername(long iCnnId);

	EnumSipErrorCode SipSessionCmpAuthorize(long iCnnId, const CGSString &strUsername, 
											const CGSString &strPasswrd);


	EnumSipErrorCode SipSessionSetOptions(long iCnnId,  INT iOptionName, const char *pValue, int iValueSize);

	EnumSipErrorCode GetConnectInfo(long iCnnId, 
		StruSipConnnectInfo *pRes);
	
private :


	EnumSipErrorCode SendSyncRequest(CClientInfo *pClient,
		const StruSipDialogKey &stRemoteKey,
		EnumSipMethod eMethod, osip_message_t *pMsg,
		StruSipData *pRes,int iTimeouts, 
		StruSipDialogKey *pOutDlgKey );

	EnumSipErrorCode BuildAck(CClientInfo *pClient,const StruSipData *pSendData, 
							  osip_message_t **ppOSipMsg);

	EnumSipErrorCode BuildInfo(CClientInfo *pClient,const StruSipData *pSendData, 
		osip_message_t **ppOSipMsg);

	EnumSipErrorCode  BuildSubscribe(CClientInfo *pClient,
								const StruSipData *pSendData, 
								osip_message_t **ppOSipMsg);


	EnumSipErrorCode  BuildInvite(CClientInfo *pClient,
								const StruSipData *pSendData, 
								osip_message_t **ppOSipMsg);

	EnumSipErrorCode  BuildNormal(CClientInfo *pClient,
								const StruSipData *pSendData,EnumSipMethod eMethod,
								osip_message_t **ppOSipMsg);

	//�̻߳ص�
	static void ThreadCallback(CGSThread *pThread, void *pParam );

	//�̻߳ص��¼����
	void OnThreadEventEntry(void);


	//������Ҫע���������Ϣ
	void HandleRequestWithRegister( eXosip_event_t *pXEvent );

	//������ͨ������
	void HandleRequest(eXosip_event_t *pXEvent,  EnumSipMethod eMethod );

	//����Ӧ������
	void HandleResponse(eXosip_event_t *pXEvent,  EnumSipMethod eMethod );

	//����ע��ɹ�����

	void HandleResponseWithRegister( eXosip_event_t *pXEvent );



	void SendSimpleResponse(eXosip_event_t *pXEvent, int iSipErrno, 
		const char *czMessage, CGSString *pTo, CClientInfo *pClient );

	//ע��ʱ�ظ� Unauthorized
	void SendResponseUnauthorized(eXosip_event_t *pXEvent, CClientConnecter *pCnner );


	void DeleteClient(long iCnnId);


	void InitSipData(const long iCnnId,const eXosip_event_t *pXEvent,const EnumSipDataType eDataType, StruSipData &stData );

	void SetDlgKeyMember(osip_message_t *pOSipMsg, StruSipDialogKey &stKey );

	INLINE CClientInfo *FindClientOfCnnId( long iCnnid )
	{
		CMapClientOfCnnId::iterator csIt = m_mapClientsOfCnnId.find(iCnnid); 

		if( csIt==m_mapClientsOfCnnId.end() )
		{
			return NULL;
		}
		return csIt->second;
	}

	CClientInfo *FindClientOfNet( const CClientConnecter &nner);

	CResponseWaiter *PrepareWaitResponse(const osip_message_t *posipMsg,
							EnumSipMethod eMethod ); //׼���ȴ�����

	void CancelWaitResponse(CResponseWaiter *pWaiter);

	EnumSipErrorCode WaitResponse(CResponseWaiter *pWaiter, StruSipData *pResult, int iTimeouts );

	BOOL WakeupWaitResponse(EnumSipMethod eMethod, const eXosip_event_t *pXEvent, StruSipData *pRes);

	
	

	//�Ͽ�Ҳ���Ƿ��� ע����Ϣ
	void Unresigter( int iCnnID );

	//����osip ��Ϣ�� ������
	void SetOSipMessageContent(const StruSipData *pSipData, osip_message_t *pOSipMsg);
	
	
	CGSString GetNewRealm(void);
	CGSString GetNewNonce(void);

	static EnumSipContentType GetOSipMessageContentType(const osip_message_t *pOSipMsg, 
				CGSString *pStrContentTypeName = NULL);

	CGSString GuessLocalIp( const CGSString &strRemoteIp );

	//���ر���IP��Ϣ
	BOOL LoadLocalIf(void);

	//���� Tag
	void RequestResetFromTag( osip_message_t *pOSipMsg, CClientInfo *pClient);
	//void ResponseResetTag( osip_message_t *pOSipMsg, CClientInfo *pClient);

	//����ǻ�õĿͻ���
	void ClearDisActiveClient(void);


	CGSString HostName2Ip(const CGSString &strHost);

};

} //end namespace SIP

#endif //end _GS_H_SIPSERVER_H_
