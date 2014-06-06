/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : PROGSPCHANNEL.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/28 17:35
Description: GSP �ͻ���ͨ��Э���ʵ��
********************************************
*/

#ifndef _GS_H_PROGSPCHANNEL_H_
#define _GS_H_PROGSPCHANNEL_H_

#include "../IProChannel.h"
#include "ISocket.h"
#include "MainLoop.h"
#include "RTP/RtpNet.h"


using namespace GSP::RTP;

namespace GSP
{

class  CClientChannel;
class  CClientSection;
class  CGspSyncWaiter;
class CGspTcpDecoder;
class CGspCommand;


#define CLICHN_KEEPALIVE_TIMER_ID 1
#define CLICHN_ACTIVE_TIMER_ID  2
#define CLICHN_CLOSE_TIMER_ID  3



class CGspChannel :
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
	const static LONG INVALID_COMMAND_TAG = -1; //��Ч�������
	
	CClientChannel *m_pParent;     //ִ�й����ͨ��
	CISocket *m_pTcpSocket;				//�ͻ��˵�����SOCKET
	const UINT32 m_iAutoID;				//�Ķ����������ID�� ����ʹ��
	StruGSPPacketHeader m_stProCmdHeader; //�����Э��ͷ
	GSAtomicInter m_iCmdTagSequence;    //�������ż���

	CGSWRMutex m_csAsyncWRMutex; // m_csAsyncCmdList ͬ����
	CList m_csAsyncCmdList;  //�洢���� CGspSyncWaiter*
	

	BOOL m_bTcpSending;    //��׼TCP �Ƿ����ڷ���
	CList m_csTcpWaitSendQueue;  //Tcp�ȴ��������ݵĶ���, ����Ϊ CIPacket *

	BOOL m_bWaitSendFinish;

	CGSPThreadPool m_csCommandTask; //���������߳�
	CGspTcpDecoder *m_pTcpDecoder; //TCP ��Э�����

	

	INT m_iMaxKeepaliveTimeouts; //ͨ�����ʱʱ��

	CWatchTimer m_csSendKeepaliveTimer; //����keepalive ��ʱ��

	CWatchTimer m_csAsyncWaiterTimer; //ͬ����ⶨʱ��

	CWatchTimer m_csKeepaliveTestTimer; //�������

	INT m_iKeepalivePlugs; //����������

	CGSWRMutex m_csWRMutex; //ȫ��ͬ����




	GSAtomicInter m_iSendKeySeq;

	INT m_iSysHeaderDataChn;

	INT m_iGspVersion;

	GSAtomicInter m_iSkTest;


	INT m_eGspStreamTranMode;  //������ģʽ
	CTCPClientSocket *m_pTcpStreamReader;
	RTP::CRtpUdpReader *m_pRtpUdpReader;

	CGspTcpDecoder *m_pStreamTcpDecoder; //��TCP ��Э�����
		

public  :
    //�����ǽӿ�ʵ��
	virtual EnumErrno Open(const CUri &csUri, BOOL bAsyn, INT iTimeouts);
	virtual EnumErrno Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts);
	
	virtual void DestoryBefore(void);

	virtual EnumErrno FlowCtrl( BOOL bStart );

	/*
	*********************************************************************
	*
	*@brief : �����ӽӿ�
	*
	*********************************************************************
	*/
	CGspChannel(CClientChannel *pParent);

	virtual ~CGspChannel(void);

	/*
	 *********************************************
	 Function : OnAsyncWaiterEvent
	 DateTime : 2012/4/24 8:45
	 Description :  �����첽�����¼�
	 Input :  pWaiter �첽�¼�ͬ������
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void OnAsyncWaiterEvent(CGspSyncWaiter *pWaiter);

private :
	/*
	 *********************************************
	 Function : OnTcpSocketEvent
	 DateTime : 2012/4/24 8:46
	 Description : TCP ����Socket ����ص�����  
	 Input :  
	 Output : 
	 Return : 
	 Note :   �ο� <<MySocket.h>> FuncPtrSocketEvent ����
	 *********************************************
	 */
	void *OnTcpSocketEvent(	CISocket *pSocket, 
							EnumSocketEvent iEvt,
							void *pParam, void *pParamExt );


	//��TCP Socket ʱ��ص�
	void *OnStreamTcpSocketEvent(	CISocket *pSocket, 
		EnumSocketEvent iEvt,
		void *pParam, void *pParamExt );

	//UDP RTP Socket �������ݻص�
	void OnRtpReaderEvent( EnumRtpNetEvent eEvt, void *pEvtArgs );


	/*
	 *********************************************
	 Function : HandleTcpSocketWriteEvent
	 DateTime : 2012/4/24 8:48
	 Description :  ����TCP д�¼�
	 Input :  pPacket ���ص��Ѿ���ɵ����ݰ�
	 Output : 
	 Return : ���ؽ���д������ݰ�  �ο� <<MySocket.h>> FuncPtrSocketEvent ����
	 Note :   
	 *********************************************
	 */
	void *HandleTcpSocketWriteEvent( const StruAsyncSendEvent *pEvt );



	/*
	 *********************************************
	 Function : HandleTcpSocketReadEvent
	 DateTime : 2012/4/24 8:50
	 Description :  ����TCP ���¼�
	 Input :  pPacket ��ȡ��������
	 Output : 
	 Return :  TRUE/FALSE�� TRUE ��ʾ������ȡ�� FALSE ֹͣ��ȡ
					�ο� <<MySocket.h>> FuncPtrSocketEvent ����
	 Note :   
	 *********************************************
	 */
	BOOL HandleTcpSocketReadEvent(CGSPBuffer *pBuffer);

	//������TCP ��������
	BOOL HandleStreamTcpSocketReadEvent(CGSPBuffer *pBuffer);


	//������RTP-UDP ��������
	void HandleRtpStreamFrame(CFrameCache *pFrame);

	/*
	 *********************************************
	 Function : SendCommand
	 DateTime : 2012/4/24 8:51
	 Description :  ��������
	 Input :  eCommandID ����ID �ο� EnumGSPCommandID ˵��
	 Input :  pCommandPlayload ����������ݣ� ָ StruGSPCommand::cPlayload
	 Input :  iSize pCommandPlayload �ĳ���
	 Input :  iTag ���������ţ� ָ StruGSPCommand::iTag��
				���Ϊ INVALID_COMMAND_TAG �����ڲ����Զ�����һ�����
	 Output : 
	 Return : �ο� EnumErrno ˵��
	 Note :   
	 *********************************************
	 */
	EnumErrno SendCommand(EnumGSPCommandID eCommandID,
						const void *pCommandPlayload,
						UINT iSize, 
					    UINT32 iTag = INVALID_COMMAND_TAG);

	
	/*
	 *********************************************
	 Function : HandleCommandResquestResponse
	 DateTime : 2012/4/24 8:55
	 Description :  ����Request �Ļظ�����
	 Input :  pCmd �ظ�������
	 Input : bAsync �Ƿ� �첽����
	 Input : iTimeouts ��ʱʱ�䣬 ��λ����
	 Output : 
	 Return : �ο� EnumErrno ˵��
	 Note :   
	 *********************************************
	 */
	EnumErrno HandleCommandResquestResponse(const StruGSPCommand *pCmd, BOOL bAsync, INT iTimeouts );



	/*
	 *********************************************
	 Function : OnTcpRcvTaskPoolEvent
	 DateTime : 2012/4/24 8:57
	 Description : ������̳߳� �ص�, m_csCommandTask �Ļص�
	 Input :  
	 Output : 
	 Return : 
	 Note :   �ο�<<GSThreadPool.h>> FuncPtrObjThreadPoolEvent ˵��
	 *********************************************
	 */
	void OnCommandTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );


	/*
	 *********************************************
	 Function : HandleCommand
	 DateTime : 2012/4/24 9:01
	 Description :   �����
	 Input :  pCommand �������������
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void HandleCommand( CGspCommand *pCommand );

	/*
	 *********************************************
	 Function : HandleKeepalive
	 DateTime : 2012/4/24 9:02
	 Description :  ����Keepalive ����
	 Input :  pCmdHeader ������� �ο� StruGSPCommand ˵��
	 Input : iPlayloadSize ָ ������ĳ���, StruGSPCommand::cPlayload����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void HandleKeepalive( CGspCommand *pCommand );

	//
	/*
	 *********************************************
	 Function : HandleRequestResponse
	 DateTime : 2012/4/24 9:04
	 Description :  ����Request ����
	 Input :   pCmdHeader ������� �ο� StruGSPCommand ˵��
	 Input :  iPlayloadSize ָ ������ĳ���, StruGSPCommand::cPlayload����
	 Input :  iGSPVersion ���������GSP �汾��
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void HandleRequestResponse( CGspCommand *pCommand );




	/*
	 *********************************************
	 Function : WakeupAsync
	 DateTime : 2012/4/24 9:06
	 Description :  �����첽ͬ������
	 Input :   iCommandID �ȴ�������
	 Input : iCmdTag �ȴ��������
	 Input : eErrno ���ѽ��
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void WakeupAsync(INT32 iCommandID, INT32 iCmdTag, EnumErrno eErrno );


	/*
	 *********************************************
	 Function : OnTimerEvent
	 DateTime : 2012/4/24 9:07
	 Description :  ��ʱ���ص�
	 Input :  
	 Output : 
	 Return : 
	 Note :   �ο� FuncPtrTimerCallback ˵��
	 *********************************************
	 */
	void OnTimerEvent( CWatchTimer *pTimer );


	/*
	 *********************************************
	 Function : SendKeepalive
	 DateTime : 2012/4/24 9:08
	 Description :  ����Keepalive ����
	 Input :  bResponse ָ��������Զ��Ƿ���Ҫ�ظ�
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void SendKeepalive( BOOL bResponse );





	//���������첽�ȴ���
	void WakeupAllAsyncWaiter(EnumErrno eErrno);


	INT GetMediaChannel(EnumGSMediaType eType);

    
};

} //end namespace GSP


#endif //end _GS_H_PROGSPCHANNEL_H_
