/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : RTPCONTENT.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/4/23 14:41
Description: 
********************************************
*/

#ifndef _GS_H_RTPCONTENT_H_
#define _GS_H_RTPCONTENT_H_
#include "ISocket.h"
#include "../MainLoop.h"
#include "../RTPAnalyer.h"


namespace GSP
{
	
#define RTP_CMD_CHANNEL_ID  (0x7F-1)  //CMD ͨ��ʹ�õ��ŵ��� X

class CRtpContent : public CGSPObject
{
public :
	typedef enum
	{
		eEVT_RCV_FRAME = 1, //���յ����ݣ� ���� CSliceFrame *
		eEVT_RCV_LOST_FRAME, //��֡�� ����ΪNULL
		eEVT_RCV_NAL_PACKET, //�յ�NAL ��
		eEVT_RCV_RETRY_RESEND, //�����ش��� ���� Ϊ INT �� ��ʾ�ش��ĸ���
	}EnumEvent;

	/*
	 *********************************************
	 Function : FuncPtrEventCallback
	 DateTime : 2012/4/25 14:52
	 Description :  �¼��ص�
	 Input :  eEvt �¼����Ͳο� EnumEvent
	 Input : pEvtArg �¼����� �ο� EnumEvent ˵��
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	typedef void (CGSPObject::*FuncPtrEventCallback)(EnumEvent eEvt, void *pEvtArg);

	typedef struct _StruRtpCoreData
	{	
		RTP::StruRTPHeader stRtpHeader; //�������ݵ�Rtpͷ
		UINT16 iRtpSeq; //�������ݵ�Rtpͷ��SEQ
		INT16 iCompletes; //�յ��������ĸ���
		UINT32  iTimestamp; //�������ݵ�Rtpͷ��TP
		CList lRcvPacket; //���ܵ������� �洢����Ϊ RTSP::CRtpRecvBuffer *
		
		UINT32 iRcvDiscardTimestamp; //���ʧЧ֡��ʱ��


		std::vector<RTP::CRtpSliceFrame *> vFrameCache;
		INT iCacheW; // Cache дָ��
		INT iMaxCaches;
		void Init( FuncPtrFree fnFreeRcvPacket )
		{
			
			INIT_DEFAULT_RTH_HEADER(&stRtpHeader);
			iRtpSeq = 0;
			iTimestamp = 0;
			iCompletes = 0;
			lRcvPacket.Clear();
			iRcvDiscardTimestamp = 0;
			iCacheW = 0;
			iMaxCaches = 0;
			
			if( fnFreeRcvPacket )
			{
				lRcvPacket.SetFreeCallback(fnFreeRcvPacket);
			}
			for( INT i = 0; i<5; i++ )
			{
				RTP::CRtpSliceFrame *p = new RTP::CRtpSliceFrame();
				if( p )
				{
					vFrameCache.push_back(p);
				}
				
			}
			iMaxCaches = vFrameCache.size();
			GS_ASSERT_EXIT(iMaxCaches>0, -1);
		}
		void Clear(void)
		{
			lRcvPacket.Clear();
			for( UINT i = 0; i<vFrameCache.size(); i++ )
			{
				delete vFrameCache[i];
			}
			vFrameCache.clear();
		}
		
	}StruRtpCoreData;




	
	StruLenAndSocketAddr m_stRemoteAddr; //�Զ˵�����ַ��Ϣ
	StruRtpCoreData m_vRtpData[GSP_MAX_MEDIA_CHANNELS]; //RTP ͷʹ�õ�����
	StruRtpCoreData m_stCmdRtpData; //Command ͨ�� RTP ͷʹ�õ�����
	UINT16 m_iLocalSSRC;  //SSRC ���ز���
	UINT16 m_iRemoteSSRC; //SSRC Զ�� ����
	UINT32 m_iSSRC; // SSRC
	StruLenAndSocketAddr m_stNalRemoteAddr; //����NAL ���͵İ�
	BOOL m_bNalEnable; //�ͷ�ʹ�� NALͨ��
	BOOL m_bConnect; //��ǰ�Ƿ�����
	BOOL m_bNalInit; //���Ѿ���ʼ��NAL

	CWatchTimer m_csNalTimer; // ����NAL �Ķ�ʱ��
	INT m_iNalInterval; //NAL ����ʱ������ ��λ��

	CGSString  m_strRemoteIP; // �Զ�IP
	INT m_iRemotePort; //�Զ�ͨ�Ŷ˿�

	CMyID *m_pMyID;

	BOOL m_bTestConnect; //���ڼ�����

	CGSMutex m_csMutex;
	CGSCondEx m_csCond; //�������Ӳ��Եȴ�

	BOOL m_bServerType; //�Ƿ�Ϊ��������

	CGSPObject    *m_pFuncOwner; //�¼��ص� ��������
	FuncPtrEventCallback m_fnCallback; //�ص�����

	INT m_iSendNalTicks;  //����NAL ����

	BOOL m_bInit;
private :
	CRtpSocket *m_pSocket;
public :

	CRtpContent(void);

	virtual ~CRtpContent(void);

	INLINE INT LocalPort(void)
	{
		CGSAutoMutex locker(&m_csMutex);
		if( m_pSocket )
		{
			return m_pSocket->Socket().LocalPort();
		}
		return 0;
	}


	void SetEventCallback( CGSPObject *pFuncOwner, FuncPtrEventCallback fnCallback );



	/*
	 *********************************************
	 Function : Init
	 DateTime : 2012/4/24 19:05
	 Description :  ��ʼ��
	 Input :  pSocket ͨ��ʹ�õ�RTP SOCKET
	 Output : 
	 Return :  ���ش�����
	 Note :   
	 *********************************************
	 */
	EnumErrno Init( CRtpSocket *pSocket );

	/*
	 *********************************************
	 Function : Unint
	 DateTime : 2012/4/24 19:21
	 Description :  ж��
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void Unint(void);

	/*
	 *********************************************
	 Function : Connect
	 DateTime : 2012/4/24 19:06
	 Description :  ���ӶԶ�
	 Input :  szRemoteIP �Զ�IP
	 Input :  iRemotePort �Զ˶˿�
	 Input : iRemoteSSRC �Զ�SSRC
	 Input : szNalIP NAL IP�� NULL ������ NAL
	 Input : iNalPort NAL �˿ڣ� < 1  ������ NAL
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	EnumErrno Connect(const char *szRemoteIP, UINT16 iRemotePort,UINT16 iRemoteSSRC, 
						const char *szNalIP, INT iNalPort );

	/*
	 *********************************************
	 Function : EnableNal
	 DateTime : 2012/4/24 19:09
	 Description :  ʹ��NAL ����
	 Input :  bEnable ʹ��
	 Input :  iInterval ���ͼ���� ��λ�� ��
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	BOOL EnableNal(BOOL bEnable, INT iInterval = 250);
	
	/*
	 *********************************************
	 Function : SendFrame
	 DateTime : 2012/4/24 19:10
	 Description :  ����ý������
	 Input :  pFrame ý������֡
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	EnumErrno SendFrame(CSliceFrame *pFrame );

	/*
	 *********************************************
	 Function : ConnectTest
	 DateTime : 2012/4/24 19:11
	 Description :  ��������Ƿ����
	 Input :  iTimeouts ��ʱʱ�䣬 ��λ����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	EnumErrno ConnectTest(INT iTimeouts);
private :

	/*
	 *********************************************
	 Function : OnTimerEvent
	 DateTime : 2012/4/24 19:11
	 Description :  ��ʱ���ص�
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void OnTimerEvent( CWatchTimer *pTimer );


	/*
	 *********************************************
	 Function : OnRtpRecvEvent
	 DateTime : 2012/4/24 19:30
	 Description :  RTP SOCKET �������ݻص����ο� CRtpSocket::FuncPtrRtpDataRecvEvent
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL OnRtpRecvEvent(const StruLenAndSocketAddr &stRemoteAddr, RTP::CRtpRecvBuffer *pPacket );

	/*
	 *********************************************
	 Function : SendTestConnectCommand
	 DateTime : 2012/4/24 20:21
	 Description :  �������Ӳ�������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL SendTestConnectCommand(void); 

	/*
	 *********************************************
	 Function : HandleCommand
	 DateTime : 2012/4/24 20:35
	 Description :  ����ͨ������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL HandleCommand(const StruLenAndSocketAddr &stRemoteAddr, 
		RTP::CRtpRecvBuffer *pPacket);


	/*
	*********************************************
	Function : HandleCommand
	DateTime : 2012/4/24 20:35
	Description :  ����������
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	BOOL HandleStream(const StruLenAndSocketAddr &stRemoteAddr, 
		RTP::CRtpRecvBuffer *pPacket);


	void SendEvent( EnumEvent eEvt, void *pArgs )
	{
		if( m_fnCallback )
		{
			(m_pFuncOwner->*m_fnCallback)(eEvt, pArgs);
		}
	}

};

} //end namespace GSP

#endif //end _GS_H_RTPCONTENT_H_