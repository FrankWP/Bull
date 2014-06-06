/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : CLIENTCHANNEL.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/7/14 14:36
Description: �ͻ���ͨ����ʵ��
********************************************
*/
#ifndef GSP_CLIENTCHANNEL_DEF_H
#define GSP_CLIENTCHANNEL_DEF_H
#include "IGSPClient.h"
#include "GSPObject.h"
#include "Uri.h"
#include "MediaInfo.h"
#include "MainLoop.h"
#include "CircleQueue.h"

namespace GSP
{

#define CLICHN_KEEPALIVE_TIMER_ID 1 //��ʱ�� keepalive 
// #define CLICHN_ACTIVE_TIMER_ID  2
// #define CLICHN_CLOSE_TIMER_ID  3

class CClientSection;
class CIProChannel;
class CLog;
class CFrameCache;


class CClientChannel : public CGSPObject,  public CIClientChannel
{

public :
	//�ź�������
	typedef enum
	{
		eSIG_DISCONNECT, //����˿�
		eSIG_ASSERT, //�쳣
		eSIG_REMOTE_DISCONNECT ,//�Զ˹ر�
		eSIG_STREAM_FINISH, //���Ž���
		eSIG_REMOTE_CLOSE, //�Զ˹ر�, ����ȴ����ݷ������
		eSIG_REMOTE_ASSERT, //�Զ��쳣�ر�
	}EnumSignal;
public:
	const UINT32 m_iAutoID;  //������������ID�� ����ʹ��
	CClientSection *m_pSection; //������Section ������
	CIClientChannel::EnumErrno m_eErrno; //�����
	CGSPString m_strURI; //����򿪵�URI
	CUri m_csURI; //URI ������
	EnumProtocol m_eProtocol; //ʹ�õ�Э��
	
	CMediaInfo m_csMediaInfo; //ͨ����ý����Ϣ����
	CIClientChannel::EnumStatus m_eStatus; //ͨ��״̬
	INT32 m_iCtrlAbilities; //֧�ֵĿ�������
	StruPlayStatus m_stPlayStatus; //����״̬
	CLog *m_pLog; //��־����
	INT32 m_eTranModel; //����ģʽ
	CIClientChannel::StruChannelInfo m_stChannelInfo; //ͨ����Ϣ
	long m_iDebugListTv; //��֡��Ϣ����
	INT m_VSeq[GSP_MAX_MEDIA_CHANNELS]; //��ͨ����Seq ����
	EnumGSMediaType m_vMediaType[GSP_MAX_MEDIA_CHANNELS]; //��ͨ����ý�����ͣ�������ٶ�ȡ



	CGSString m_strSdp;
	BOOL m_bFinish;
	

	BOOL m_bEnableRtp;
	BOOL m_bStep;
private :
	CGSMutex m_csMutex; //����ͬ��
    static GSAtomicInter s_iAutoIDSequence;   //���������м���
	CIProChannel *m_pProChannel; //��ǰʹ�õ�Э��ͨ������
	void *m_pUserPrivateData; //�û�����
	BOOL m_bOpenning; //��ǰ�Ƿ�����ִ�д�����
	CGSPThreadPool m_csTaskPool; //���ݴ����߳�

	INT m_iMaxKeepaliveStreamTimeouts; //���ʱʱ�� , ��
	INT m_iKeepaliveStreamPlugs;  //keepalive ��������

	CWatchTimer m_csKeepaliveStreamTimer; //�������


 	CGSMutex m_csCacheMutex; //���尲ȫ��
	CCircleQueue<CFrameCache*> m_csStreameQueue;  //������
	CCircleQueue<StruPlayStatus*> m_csStatusQueue; //״̬����

	INT m_iFlowCtrl; //��ǰ��������

	BOOL m_bClosing; //���ڹر� 

	BOOL m_bAcceptEvent; //�Ƿ�����Ϣ

	INT m_iOpenTimeouts;
	CGSPThreadPool m_csAsyncOpenTaskPool; //�첽��
	CGSCondEx m_condOpen;
	GSP::EnumErrno m_eOpenErrno;

	INT m_iSpeed; //��ǰ�ٶ�

	INT64 m_iWaitFrameSize; //��������ݴ�С
	CGSMutex m_csMutexCacheSize; // m_iWaitFrameSize ��

	CFrameCache *m_pSysHeaderCache;
public :
	
	/*
	 *********************************************
	 Function : CClientChannel
	 DateTime : 2012/4/24 10:06
	 Description :  
	 Input :  pSestion �����Ĺ������
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
   CClientChannel(CClientSection *pSestion);
    virtual ~CClientChannel(void);

	/*
	*********************************************************************
	*
	*@brief : �����ǽӿ�ʵ��
	*
	*********************************************************************
	*/
	
	virtual CIClientSection *GetSection(void) const;


	virtual void Release(void);              


	virtual CIClientChannel::EnumErrno GetErrno(void) const;


	virtual BOOL SetURI( const char *czURI);

	virtual BOOL SetURIOfSip( const char *czURI, const char *czSdp );


	virtual const char *GetURI( void ) const;


	virtual BOOL AddRequestInfo(const StruGSMediaDescri *pInfo, INT iLen) ;


	virtual void ClearRequestInfo( EnumGSMediaType eType = GS_MEDIA_TYPE_NONE  ) ;


	


	virtual BOOL Open(INT iTransType, INT iTimeouts = 20000 ) ; 


	virtual void Close(void);



	virtual CIClientChannel::EnumStatus GetStatus(void) const;   


	virtual CIMediaInfo &GetMediaInfo(void);    

	virtual const char *GetSdp(void);


	virtual BOOL Ctrl(const StruGSPCmdCtrl &stCtrl, INT iTimeouts = 5000);

	virtual BOOL CtrlOfManstrsp(const char *czMansrtsp, INT iTimeouts = 5000);


	virtual UINT32 GetCtrlAbilities(void) const;


	virtual BOOL EnableAutoConnect(BOOL bEnable = TRUE);


	virtual BOOL SetReconnectInterval(UINT iSecs);


	virtual BOOL SetReconnectTryMax(UINT iCounts); 


	virtual const char *GetDescri(void) const;


	virtual const StruPlayStatus *GetPlayStatus(void) const;

	virtual const CIClientChannel::StruChannelInfo *GetInfo(void) const;

	virtual void SetUserData(void *pData) ;

	virtual void *GetUserData(void) const;

	 virtual UINT32 GetAutoID(void) const;

	/*
	*********************************************************************
	*
	*@brief : ����Ϊ�����ӿ�
	*
	*********************************************************************
	*/



	/*
	 *********************************************
	 Function : IsOpened
	 DateTime : 2012/4/24 10:06
	 Description :  �ж��Ƿ��Ѿ���
	 Input :  
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	INLINE BOOL IsOpened(void) const
	{
		return (m_eStatus==ST_READY || m_eStatus==ST_PLAYING || m_eStatus==ST_PAUSE);			
	}

	/*
	 *********************************************
	 Function : OnProChannelSignalEvent
	 DateTime : 2012/4/24 10:07
	 Description :  ������Э��ͨ���������ź�
	 Input :  eSignal �ź�
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void OnProChannelSignalEvent( EnumSignal eSignal );

	/*
	 *********************************************
	 Function : HandleStream
	 DateTime : 2012/4/24 10:08
	 Description :  ������Э��ͨ��������ý��������
	 Input :  pFrame ����֡
	 Input :  bSafeThread ��֡��֤���̰߳�ȫ
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	GSP::EnumErrno HandleStream( CFrameCache *pFrame, BOOL bSafeThread = TRUE );

	/*
	 *********************************************
	 Function : HandlePlayStatus
	 DateTime : 2012/4/24 10:08
	 Description :  ������Э��ͨ�������Ĳ���״̬
	 Input :  stPlayStatus ״̬
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void HandlePlayStatus(const StruPlayStatus &stPlayStatus);


	/*
	 *********************************************
	 Function : IsInit
	 DateTime : 2012/4/24 10:09
	 Description :  �����Ƿ��ʼ���ɹ�
	 Input :  
	 Output : 
	 Return : TRUE/FLASE
	 Note :   �����ʼ�����ɹ��� �Ķ�����ʹ��
	 *********************************************
	 */
	BOOL IsInit(void); 

	/*
	 *********************************************
	 Function : SendEvent
	 DateTime : 2012/4/24 10:10
	 Description :  ����ʱ�䵽�ϴ�, �ο� EnumGSPClientEventType ����
	 Input :  eEvtType �¼�����
	 Input : pEventData �¼�����, �ο� EnumGSPClientEventType ����
	 Input : iEvtDataLen pEventData �ĳ���
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	INLINE INT SendEvent( EnumGSPClientEventType eEvtType,
		void *pEventData = NULL ,  INT iEvtDataLen = 0);

	/*
	 *********************************************
	 Function : CountSeqStep
	 DateTime : 2012/4/24 10:12
	 Description :  ���� ����SEQ �Ĳ�ֵ
	 Input :  iCurSeq ��ǰ��Seq
	 Input : iLastSeq ��һ��Seq
	 Output : 
	 Return : ���ز�ֵ
	 Note :   
	 *********************************************
	 */
	static INT CountSeqStep(INT iCurSeq, INT iLastSeq );


	
	/*
	 *********************************************
	 Function : RefreshMediaInfo
	 DateTime : 2012/4/24 10:13
	 Description :  ����m_vMediaType ��ֵ
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */

	void RefreshMediaInfo(void);

	//�������Ļ
	void KeepStreamAlive(void)
	{
		m_iKeepaliveStreamPlugs  = 0;
	}

private :
	//m_csTaskPool �ص�
	void OnTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );


	//��ʱ���ص�
	void OnTimerEvent( CWatchTimer *pTimer );

	
	//m_csAsyncOpenTaskPool �ص�
	void OnAsyncOpenTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );

};


};

#endif
