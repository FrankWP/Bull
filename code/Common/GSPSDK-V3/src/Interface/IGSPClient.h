/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : IGSPCLIENT.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/7/13 9:46
Description: �ͻ��˽ӿ���
********************************************
*/

#ifndef GSP_IGSPCLIENT_DEF_H
#define GSP_IGSPCLIENT_DEF_H

#include "GSPConfig.h"
#include "IMediaInfo.h"



namespace GSP
{
class CIClientSection;
class CIClientChannel;

/*
 *********************************************
 Function : GSPClientEventFunction
 DateTime : 2010/6/11 17:41
 Description :  �ͻ��˻ص�����
 Input :  pcsClient �����¼��ĻỰ���� , 
 Input : pChannel  �����¼�������ͨ������ ����Section�¼� �� ��ֵΪNULL�� ����ο�EnumGSPClientEventType
 Input : eEvtType �¼����� , �ο� EnumGSPClientEventType
 Input : pEventData��   �¼������� �ο� EnumGSPClientEventType
 Input :  iEvtDataLen  pEvtData ����
  Input :  pUserData �û�����
 Output :
 Return : ����ֵ�� �ο� EnumGSPClientEventType
 Note :
 *********************************************
 */



 typedef enum
{
    GSP_EVT_CLI_FRAME = 1, //ͨ���¼�, pChannel��Ч�� �յ�һ֡���� ,        ����ֵ���ã�   ����Ϊ CFrame *, 
    GSP_EVT_CLI_STATUS,    //ͨ���¼�, pChannel��Ч�� �յ�״̬�������ݰ� , ����ֵ���ã�  ����Ϊ StruPlayStatus *
    GSP_EVT_CLI_TRYFAIL,  //ͨ���¼�, pChannel��Ч�� ����ʧ�ܣ�����ֵ���� TRUE �����ԣ�FALSE ��ֹͣ���ԣ� ����ΪINT �� ��ʾ���ԵĴ��� 
    GSP_EVT_CLI_ASSERT_CLOSE, //ͨ���¼�, pChannel��Ч�� �ͻ����챻�ر�, ����Ϊ char * error ��������
    GSP_EVT_CLI_CTRL_OK,  //ͨ���¼�, pChannel��Ч�� ���ƽ���ɹ���  ����Ϊ  StruGSPCmdCtrl *
    GSP_EVT_CLI_CTRL_FAIL, //ͨ���¼�, pChannel��Ч�� ���ƽ��ʧ�ܣ�  ����Ϊ  StruGSPCmdCtrl *

    GSP_EVT_CLI_DESTROY, //ͨ���¼�, pChannel��Ч�� ������Section �ر�ʱ�ᷢ��
    GSP_EVT_CLI_COMPLETE, //ͨ���¼�, pChannel��Ч�� ���š����ؽ���
    GSP_EVT_CLI_RETREQUEST, //ͨ���¼�, pChannel��Ч��  ����ʧ��, ����Ϊ BOOL �� TRUE ��ʾ�ɹ��� FALSE ��ʾʧ��
    GSP_EVT_CLI_LOSTDATA, ////ͨ���¼�, pChannel��Ч, ��֡�¼��� ����ΪINT ��ʾ��֡����ͨ��

	GSP_EVT_CLI_SIP_SEND, ///ͨ������ pChannel��Ч�� ����SIP ���ݣ� ����Ϊ StruGSPSendSip *, ����TRUE/FALSE
}EnumGSPClientEventType;

 
typedef struct _StruGSPSendSip
{	
	const void *pSendSipData; // const StruSipData *
	void *pRes; // StruSipData *
	int iTimeouts;
	void *pOutDlgKey; // StruSipData *
}StruGSPSendSip;


typedef INT (*GSPClientEventFunctionPtr)(CIClientSection *pcsClient,CIClientChannel *pChannel, 
                                            EnumGSPClientEventType eEvtType, 
                                            void *pEventData,  INT iEvtDataLen, void *pUserData );


class CIClientChannel
{ 
protected :
      virtual ~CIClientChannel(void)
	  {

	  }
public :
     typedef enum
     {
         ST_INIT,   //����
         ST_READY,  //׼������
         ST_PLAYING, //������
         ST_PAUSE,   //��ͣ
         ST_ASSERT,  //�쳣
		 ST_WOPEN, //�ȴ���

     }EnumStatus;

	 typedef struct _StruChannelInfo
	 {
		INT64 iRcvFromNet; //���ܵ��ֽ���
		INT64 iSendToNet;  //���͵��ֽ���
		INT64 iLostFrames;  //��ʧ��֡��
		INT64 iLostNetFrames; //û�н��ܵ��Զ˶���ʧ������
	 }StruChannelInfo;

     typedef enum
     {
         ERRNO_SUCCESS = 0,
         ERRNO_EUNKNOWN = 1,
		 ERRNO_EURI,
		 ERRNO_ENPROTOCOL,
         ERRNO_EOFFLINE, //����
		 ERRNO_ENCTRL,
		 ERRNO_ENFUNC,		 
     }EnumErrno;


	 static const char *StatusString(EnumStatus eStatus)
	 {
		switch(eStatus )
		{
		case ST_INIT :
			{
				static const char *szResult = "Init";
				return szResult;
			}
		break;
		case ST_READY :
			{
				static const char *szResult = "Ready";
				return szResult;
			}
			break;
		case ST_PLAYING :
			{
				static const char *szResult = "Playing";
				return szResult;
			}
			break;
		case ST_PAUSE :
			{
				static const char *szResult = "Pause";
				return szResult;
			}
			break;
		case ST_ASSERT :
			{
				static const char *szResult = "Assert";
				return szResult;
			}
			break;
		case ST_WOPEN :
			{
				static const char *szResult = "WaitOpen";
				return szResult;
			}
			break;			
		}
		static const char *szNone = "Invalid";
		return szNone;
	 }

 
     /*
     *********************************************
     Function : GetSection
     DateTime : 2010/8/6 10:15
     Description :����������Section
     Input :
     Output :
     Return :  ��������Section�����ָ��
     Note :
     *********************************************
     */
     virtual CIClientSection *GetSection(void) const =0;

     /*
     *********************************************
     Function :   Release
     DateTime : 2010/8/6 10:16
     Description : �ͷ�ͨ������
     Input :
     Output :
     Return :
     Note :
     *********************************************
     */
     virtual void Release(void) = 0;              
    

     /*
     *********************************************
     Function :GetErrno
     DateTime : 2010/8/6 10:16
     Description : ���ش����
     Input :
     Output :
     Return :  ���ش���ţ� �ο�  CIClientChannel::EnumErrno����
     Note :
     *********************************************
     */
     virtual CIClientChannel::EnumErrno GetErrno(void) const = 0;


     /*
     *********************************************
     Function :  SetURI/SetURIOfSip
     DateTime : 2010/6/12 8:20
     Description : ����Ҫ���ӵ�URI
     Input :  czURI Ҫ���õ�URI�ִ� 
	 Input : czSdp  SDP ����
     Output : TRUE/FALSE�� ��������Ѿ��򿪣����ý�ʧ��

     Return :
     Note :
     *********************************************
     */
     virtual BOOL SetURI( const char *czURI) = 0;
	 virtual BOOL SetURIOfSip( const char *czURI, const char *czSdp ) = 0;

     /*
     *********************************************
     Function : GetURI
     DateTime : 2010/6/12 8:21
     Description : �������ӵ�URI
     Input :
     Output :
     Return :    �����ַ����� ʧ�ܷ���NULL
     Note :
     *********************************************
     */
     virtual const char *GetURI( void ) const = 0;

     /*
     *********************************************
     Function : AddRequestInfo
     DateTime : 2010/6/12 8:22
     Description : ���������������
     Input : pInfo ����
     Input : iLen ���Եĳ��� ��������Ҫ    
     Output :
     Return : TRUE/FALSE
     Note :   ���Ӵ򿪺�������Բ�������, 
     *********************************************
     */
     virtual BOOL AddRequestInfo(const StruGSMediaDescri *pInfo, INT iLen)  = 0;

     /*
     *********************************************
     Function :  ClearRequestInfo
     DateTime : 2010/6/12 8:25
     Description :  ���������������
     Input :  eType ��������ͣ� ���Ϊ GS_MEDIA_TYPE_NONE ���ȫ��
     Output :
     Return :
     Note :
     *********************************************
     */
     virtual void ClearRequestInfo( EnumGSMediaType eType = GS_MEDIA_TYPE_NONE  )  = 0;


     /*
     *********************************************
     Function :  Open
     DateTime : 2010/6/12 8:26
     Description : ����
     Input : iTransType �������Ĵ������ͣ� �ο���GSPStru.h��GSP�������Ͷ���
     Input : iTimeouts �첽��ʽ��ͬ����ʱʱ��, < 1 ��ʾ�첽������ > 0 ��ʾͬ����ʽ�� ��ʱʱ�䵥λΪ ����
     Output :
     Return :  TRUE/FALSE 
     Note :  Ҫ������URI�� ����ɹ����󽫴��� READY
     *********************************************
     */
     virtual BOOL Open(INT iTransType, INT iTimeouts = 20000) = 0; 

     /*
     *********************************************
     Function :  Close
     DateTime : 2010/8/6 10:17
     Description :  �رյ�ǰ����
     Input :
     Output :
     Return :
     Note :  �رպ���Դ��´�
     *********************************************
     */
     virtual void Close(void) = 0;


     /*
     *********************************************
     Function :   GetStatus
     DateTime : 2010/8/6 10:18
     Description :  ����ͨ����ǰ״̬
     Input :
     Output :
     Return :  ����ͨ����ǰ״̬  �ο� CIClientChannel::EnumStatus ���� 
     Note :
     *********************************************
     */
     virtual CIClientChannel::EnumStatus GetStatus(void) const= 0;   

     /*
     *********************************************
     Function : GetMediaInfo 
     DateTime : 2010/8/6 10:19
     Description :����������������Դ��ý����Ϣ
     Input :
     Output :
     Return : ����������������Դ��ý����Ϣ�Ķ�������
     Note :   ֻ�д򿪳ɵ����ӣ� ��ȡ����Ϣ��������
     *********************************************
     */
     virtual CIMediaInfo &GetMediaInfo(void) = 0;   

	/*
	 *********************************************
	 Function : GetSdp
	 Version : 1.0.0.1
	 Author : ������
	 DateTime : 2012/11/7 17:33
	 Description :  ��ȡ����SDP ����
	 Input :  
	 Output : 
	 Return : ��Ƶ����NULL
	 Note :   
	 *********************************************
	 */
	 virtual const char *GetSdp(void) = 0;

     /*
     *********************************************
     Function : Ctrl
     DateTime : 2010/6/12 8:29
     Description : ���ſ���
     Input :  struCtrl �ο� StruGSPCmdCtrl ����
     Input : iTimeouts �첽��ʽ��ͬ����ʱʱ��, < 1 ��ʾ�첽������ > 0 ��ʾͬ����ʽ�� ��ʱʱ�䵥λΪ ����
     Output :
     Return :
     Note : ֻ���� READY, PLAYING, PAUSE�� ״̬����Ч
     *********************************************
     */
     virtual BOOL Ctrl(const StruGSPCmdCtrl &stCtrl, INT iTimeouts = 5000) = 0;


	  virtual BOOL CtrlOfManstrsp(const char *czMansrtsp, INT iTimeouts = 5000) = 0;

     /*
     *********************************************
     Function :  GetCtrlAbilities
     DateTime : 2010/8/6 10:20
     Description :  �������ӵ�����Դ�ṩ�Ŀ�������
     Input :
     Output :
     Return :���������ṩ�Ŀ�������, �ο� ��GSPStru.h�� GSP ������������
     Note : ֻ�ж���Ч�Ŀ��ƽ��в�����������
     *********************************************
     */
     virtual UINT32 GetCtrlAbilities(void) const = 0;

     /*
     *********************************************
     Function : EnableAutoConnect
     DateTime : 2010/6/12 8:43
     Description : �����Ƿ��Զ�����
     Input :  bEnable ��ͣ
     Output :
     Return :  TRUE/FALSE
     Note :  Ĭ�ϼ̳�������Section������
     *********************************************
     */
     virtual BOOL EnableAutoConnect(BOOL bEnable = TRUE) = 0;

     /*
     *********************************************
     Function : SetReconnectInvalid
     DateTime : 2010/6/12 8:43
     Description : �����������
     Input :  iSecs �������� ��λ ��
     Output :
     Return :  TRUE/FALSE
     Note :  Ĭ�ϼ̳�������Section������
     *********************************************
     */
     virtual BOOL SetReconnectInterval(UINT iSecs) = 0;

     /*
     *********************************************
     Function : SetReconnectTryMax
     DateTime : 2010/6/12 8:43
     Description : ���������������
     Input :  iCounts ���������� ������������󽫱�����
     Output :
     Return :  TRUE/FALSE
     Note :  Ĭ�ϼ̳�������Section������
     *********************************************
     */
     virtual BOOL SetReconnectTryMax(UINT iCounts) = 0; 

     /*
     *********************************************
     Function :  GetDescri
     DateTime : 2010/6/12 8:46
     Description :  ���ؿͻ��˵�����
     Input :
     Output :
     Return : �������ִ�
     Note :
     *********************************************
     */
     virtual const char *GetDescri(void) const= 0;

     /*
     *********************************************
     Function :  GetPlayStatus
     DateTime : 2010/8/6 10:22
     Description :  ���ص�ǰ�Ĳ���״̬
     Input :
     Output :
     Return : ���ص�ǰ�Ĳ���״̬�Ķ���ָ�룬 , �ο� ��GSPStru.h�� StruPlayStatus����
     Note :  ��״̬�ı�ʱ�� ��ͨ���ص�֪ͨ�� �ù���ֻ�ǲ�ѯ���һ�εĲ���״̬
     *********************************************
     */
     virtual const StruPlayStatus *GetPlayStatus(void) const = 0;

	 virtual const StruChannelInfo *GetInfo(void) const = 0;

     virtual void SetUserData(void *pData) = 0;

     virtual void *GetUserData(void) const = 0;


	 virtual UINT32 GetAutoID(void) const = 0;

};

/*
 *********************************************
 Function : FuncPtrFetchClientChannelCallback
 DateTime : 2012/3/28 11:21
 Description :  ö�ٰ����ĳ�Աͨ���ص�����
 Input :  
 Output : pChannel ��Աͨ��
 Output : pUserParam ͸�����û�����
 Return : ����FALSE �жϣ� TRUE ������һ��
 Note :   
 *********************************************
 */
typedef BOOL (*FuncPtrFetchClientChannelCallback)(const CIClientChannel*pChannel,
														void *pUserParam );


class  CIClientSection 
{    
protected :
    virtual ~CIClientSection(void)
	{

	}
public:
	
  
    /*
    *********************************************
    Function : EnableAutoConnect
    DateTime : 2010/6/12 8:43
    Description : ���ÿͻ����Ƿ��Զ�����
    Input :  bEnable TRUE����/FALSE��ֹ
    Output :
    Return :  TRUE/FALSE
    Note : 
    *********************************************
    */    
    virtual BOOL EnableAutoConnect(BOOL bEnable = TRUE) = 0;


    /*
    *********************************************
    Function : SetReconnectInterval
    DateTime : 2010/6/12 8:43
    Description : �����������Ե�ʱ����
    Input :  iSecs �������Ե�ʱ������ ��λ��
    Output :
    Return :  TRUE/FALSE
    Note :  
    *********************************************
    */
    virtual BOOL SetReconnectInterval(UINT iSecs) = 0;

    /*
    *********************************************
    Function : SetReconnectTryMax
    DateTime : 2010/6/12 8:43
    Description : ���ÿͻ���ÿ�ζ��ߺ������������
    Input :  iCounts ���������� ���������󽫲����Զ�����
    Output :
    Return :  TRUE/FALSE
    Note : 
    *********************************************
    */
    virtual BOOL SetReconnectTryMax(UINT iCounts) = 0;

    /*
    *********************************************
    Function :  FetchClientChannel
    DateTime : 2010/6/12 8:49
    Description : ö�ٿͻ����б�
    Input :  fnCallback �ص������� �ο� FuncPtrFetchClientChannelCallback ���� 
    Output : pUserParam �û������� �ɻص�����
    Return :  -1 ���� 0 �ɹ��� 1 ��ʾ���ص��ж�
    Note :
    *********************************************
    */
	virtual INT FetchClientChannel( FuncPtrFetchClientChannelCallback fnCallback,
								void *pUserParam )=0;

     /*
     *********************************************
     Function :  Release
     DateTime : 2010/8/6 10:24
     Description :  �ͷŻỰ�������
     Input :
     Output :
     Return :
     Note :
     *********************************************
     */
    virtual BOOL Release(void) = 0;

    /*
    *********************************************
    Function :  InitLog 
    DateTime : 2010/6/11 16:19
    Description : ������־���ݽӿ�
    Input :   czPathName ��־�Ĵ洢·��
    Output :  
    Return :  
    Note :
    *********************************************
    */

	#define GSP_LV_FATAL  0x00000002
	#define GSP_LV_ERROR  0x00000010
	#define GSP_LV_WARN   0x00000020
	#define GSP_LV_DEBUG  0x00000100
	#define GSP_LV_INFO   0x00000080
	#define GSP_LV_NOTICE 0x00000040
#if defined(_DEBUG) || defined(DEBUG)
	#define	GSP_LV_CONSLE (GSP_LV_NOTICE|GSP_LV_FATAL|GSP_LV_ERROR|GSP_LV_WARN|GSP_LV_DEBUG)
	#define	GSP_LV_FILE (GSP_LV_NOTICE|GSP_LV_FATAL|GSP_LV_ERROR|GSP_LV_WARN|GSP_LV_DEBUG)
#else
	#define	GSP_LV_CONSLE GSP_LV_NOTICE //(GSP_LV_NOTICE|GSP_LV_FATAL|GSP_LV_ERROR)
	#define	GSP_LV_FILE (GSP_LV_NOTICE|GSP_LV_FATAL|GSP_LV_ERROR)
#endif

    virtual void InitLog( const char *czPathName, INT lvConsole=GSP_LV_CONSLE, INT lvFile =GSP_LV_FILE ) = 0;

    /*
    *********************************************
    Function : Init
    DateTime : 2010/6/11 16:22
    Description :  ������ǰ�Ự
    Input : szIniFilename �����ļ��� ����NULL ��ʹ��Ĭ��ֵ
    Output :
    Return : TRUE/FALSE
    Note :
    *********************************************
    */
    virtual BOOL Init( const char *szIniFilename  = NULL) = 0;

    /*
    *********************************************
    Function : CreateChannel
    DateTime : 2010/8/6 10:26
    Description : �½�������ͨ��
    Input :
    Output :
    Return :  �½�������ͨ���Ķ���ָ�룬 ʧ�ܷ���NULL
    Note :  �û���ʹ�øö�����ҪReleas
    *********************************************
    */
    virtual CIClientChannel *CreateChannel(void) = 0;

    /*
    *********************************************
    Function : SetEventListener
    DateTime : 2010/8/6 10:28
    Description : �����¼��ص�
    Input :  fnCallback �¼��ص������� �ο�   GSPClientEventFunctionPtr����
    Input : pUserParam ͸�����ص����û�����
    Output :
    Return :
    Note : pUserParamֻ����ָ�븳ֵ�� �������û�����
    *********************************************
    */
    virtual void SetEventListener( GSPClientEventFunctionPtr fnCallback, void *pUserParam) = 0;


	/*
	 *********************************************
	 Function : OnTransRecvData
	 Version : 1.0.0.1
	 Author : ������
	 DateTime : 2013/1/15 9:13
	 Description :  ��ת��ģʽ�£����ܵ�������
	 Input :  czProtocol ���ܵ�Э������
	 Input : pData ���ܵ�����
	 Input : iDataSize pData �ĳ���
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	virtual BOOL OnTransRecvCmdData(const char *czProtocol,const void *pData, int iDataSize ) = 0;


	/*
	*********************************************
	Function : SetGspStreamTransMode
	Version : 1.0.0.1
	Author : ������
	DateTime : 2013/7/25 17:20
	Description :  ������ʹ��GSPЭ�飬 ��Ƶ���Ĵ��䷽ʽ
	Input :  eGspStreamTransMode ���䷽ʽ �ο� GSP Э�������䷽ʽ
	GSP_STREAM_TRANS_MODE_MULTI_TCP (0)   /��GSP�����ϸ��ϴ�����
	GSP_STREAM_TRANS_MODE_RTP_UDP (1)		RTP_UDP ����
	GSP_STREAM_TRANS_MODE_TCP (2)			ʹ�ö���TCP ����

	Output : 
	Return : 
	Note : Ĭ��ʹ�� �����ļ��趨 ���û�����ļ� Ĭ��Ϊ GSP_STREAM_TRANS_MODE_TCP  
	*********************************************
	*/
	virtual void SetGspStreamTransMode(int eGspStreamTransMode ) = 0;
};




/*
*********************************************************************
*
*@brief : ת��װ��
*
*********************************************************************
*/

class CIPkgConverter
{
public :
	typedef struct _StruPkg
	{
		void *pData;
		long iDataSize;		
		EnumGSMediaType eMediaType;
		BOOL isKey;
		unsigned long iTimestamp; // (��1970��1��1��0ʱ0��0�뵽��ʱ������) 
	}StruPkg;
	/*
	 *********************************************
	 Function : InputData
	 Version : 1.0.0.1
	 Author : ������
	 DateTime : 2014/4/21 9:07
	 Description :  ����ý����
	 Input : pData ý��������, ������ StruGSFrameHeaderͷ
	 Input : iSize ý�������ݳ���
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	virtual BOOL InputData(const void *pData, long iSize ) = 0;

	//���ص�ǰ����, ����NULL ��ʾ��ȡ���, ���ص�ý�����ݣ� StruGSFrameHeader
	virtual const StruPkg *GetData(void) = 0;
	
	//��һ����
	virtual void  NextData(void) = 0;


	/*
	����
		
		InputData(....);
		StruPkg *p;
		while( (p=GetData()))
		{
			DoSomething(p);
			NextData();
		}
		
	*/

	virtual ~CIPkgConverter(void)
	{

	}
protected :
	CIPkgConverter(void)
	{

	}

private :
	CIPkgConverter(CIPkgConverter &csDest );
	CIPkgConverter &operator=(CIPkgConverter &csDest );

};

typedef  enum
{
	ePkgType_PS = 1,
	ePkgType_H264 = 2,
	ePkgType_MP4 = 3
} EnumPkgType;

GS_API GSP::CIPkgConverter *CreateGSPPkgConverter( EnumGSCodeID eInputDataCodeID,
												  EnumPkgType ePkgType  );


/*
*********************************************
Function : CreateGSPClientSectionInterface
DateTime : 2010/6/12 8:54
Description :  �½��ͻ��˹���ӿڶ���
Input :
Output :
Return :  ʧ�ܷ���NULL
Note :
*********************************************
*/

GS_API GSP::CIClientSection *CreateGSPClientSectionInterface(void);

/*
*********************************************
Function : GspSetMaxCacheMemorySize
Version : 1.0.0.1
Author : ������
DateTime : 2013/5/14 11:20
Description :  ���û�������С
Input :  iSizeMByte �趨ֵ�� ��λ �� 
Output : 
Return : 
Note :   �� CreateGSPServerInterface �� CreateGSPClientSectionInterface ����ǰ�趨��Ч
*********************************************
*/
GSP_API  void GspSetMaxCacheMemorySize( UINT iSizeMByte );

}; //end GSP





#endif
