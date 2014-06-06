/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : CLIENTSECTION.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/26 14:58
Description: ʵ�ֿͻ��˻Ự����
********************************************
*/

#ifndef _GS_H_CLIENTSECTION_H_
#define _GS_H_CLIENTSECTION_H_
#include "GSPObject.h"
#include "IGSPClient.h"
#include "Log.h"
#include "List.h"
#include "SipStack.h"


namespace GSP
{
namespace SIP
{
	class CProSipClientService;
}
	
	
	
	/*
	********************************************************************
	��ע��
	����    :    CClientSection
	����    :    zouyx
	����ʱ��:    2012/4/24 10:15
	������  :		�ͻ��˻Ự����
	*********************************************************************
	*/
	
	class CClientSection : public CGSPObject , public CIClientSection
	{

	private :
		CConfigFile m_csConfig; //�ͻ�������
		void *m_pClientCallbackParam; //�ص����������Ķ���
		GSPClientEventFunctionPtr m_fnClientCallback; //�ص�����
		CGSWRMutex m_csChannelMutex; // m_csChannelList ��ͬ����
		CList m_csChannelList; //ͨ������ �洢���� CIClientChannel *

		CGSString m_strLogPath; //��־Ŀ¼
		INT m_lvLogConsole; //��־��Ļ���
		INT m_lvLogFile;  //��־�ļ����

		BOOL m_bUseRTP; //�Ƿ�ʹ��RTP ������
		INT m_iRtpUDPPort; //UDP ʹ�õĶ˿�


		CGSString m_strSipUdpBindIp;
		INT m_iSipUdpPort;

		CGSString m_strSipServerName;

		INT m_iRtpUdpPortBegin;
		INT m_iRtpUdpPortEnd;

		INT m_eGspStreamTransMode; // ������ģʽ
	public :
		CLog *m_pLog; //��־����
		BOOL m_bGspEnableRtp;
#ifdef ENABLE_SIP_MODULE
		SIP::CProSipClientService *m_pSipSrv;
#endif
	public:
		
		

		/*
		*********************************************************************
		*
		*@brief : �ӿ�ʵ��
		*
		*********************************************************************
		*/
		virtual BOOL EnableAutoConnect(BOOL bEnable = TRUE);


		virtual BOOL SetReconnectInterval(UINT iSecs);


		virtual BOOL SetReconnectTryMax(UINT iCounts);


		virtual INT FetchClientChannel( FuncPtrFetchClientChannelCallback fnCallback,
			void *pUserParam );


		virtual BOOL Release(void);

		virtual void InitLog( const char *czPathName, 
						INT lvConsole=GSP_LV_CONSLE, INT lvFile =GSP_LV_FILE );


		virtual BOOL Init( const char *szIniFilename );

		
		virtual CIClientChannel *CreateChannel(void);

		
		virtual void SetEventListener( GSPClientEventFunctionPtr fnCallback, 
										void *pUserParam);

		virtual BOOL OnTransRecvCmdData(const char *czProtocol,const void *pData, int iDataSize );

		virtual void SetGspStreamTransMode(int eGspStreamTransMode );

		/*
		*********************************************************************
		*
		*@brief : ���ӵĽӿ�
		*
		*********************************************************************
		*/

		/*
		 *********************************************
		 Function : Config
		 DateTime : 2012/4/24 10:16
		 Description :  ��ȡ���ö���
		 Input :  
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		INLINE CConfigFile &Config(void)
		{
			return m_csConfig;
		}


		/*
		 *********************************************
		 Function : SendEvent
		 DateTime : 2012/4/24 10:16
		 Description :  ���û������¼�, �ο�GSPClientEventFunctionPtr ˵��
		 Input :  
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		INT SendEvent(CIClientChannel *pChannel, 
			EnumGSPClientEventType eEvtType, 
			void *pEventData,  INT iEvtDataLen)
		{
			if( m_fnClientCallback  )
			{
				return m_fnClientCallback(this, pChannel, eEvtType, pEventData, iEvtDataLen, m_pClientCallbackParam);
			}
			return 0;
		}
		CClientSection(void);



		/*
		 *********************************************
		 Function : OnClientChannelReleaseEvent
		 DateTime : 2012/4/24 10:17
		 Description :  ����ͻ���ͨ�������ͷŵ��¼�
		 Input :  pChannel  �ͷŵ�ͨ��
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		void OnClientChannelReleaseEvent( CIClientChannel *pChannel);



		INT GetGspStreamTransMode(void) const
		{
			if( m_eGspStreamTransMode>-1)
			{
				return m_eGspStreamTransMode;
			}
			return GSP_STREAM_TRANS_MODE_MULTI_TCP;
		}
	protected :
		~CClientSection(void);

	private :
		//SIP Э��ջ�ص�
		//����
		static EnumSipErrorCode SipClientConnectCallback( SipServiceHandle hService,
			SipSessionHandle hNewSession );

		//�Ͽ�
		static void SipClientDisconnectCallback(SipServiceHandle hService, SipSessionHandle hSession);
#ifdef ENABLE_SIP_MODULE
		//�հ�
		static void   SipSIPPacketCallback(SipServiceHandle hService, 
			SipSessionHandle hSession,
			StruSipData *pData);

		EnumSipErrorCode OnSipClientConnectEvent(SipSessionHandle hNewSession );
		void OnSipClientDisconnectEvent(SipSessionHandle hSession);
		void OnSipPacketEvent(SipSessionHandle hSession, StruSipData *pData);
#endif

	};

} //end namespace GSP

#endif //end _GS_H_CLIENTSECTION_H_
