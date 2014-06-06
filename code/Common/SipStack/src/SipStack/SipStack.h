/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPSTACK.H
Author :  ������
Version : 1.0.0.1
Date: 2012/9/7 8:44
Description: SIP Э��ջ�ӿڶ���
********************************************
*/

#ifndef _GS_H_SIPSTACK_H_
#define _GS_H_SIPSTACK_H_

#define SIP_DLL_API 


//SIP �Ự ���ƾ������
typedef void * SipSessionHandle;


typedef void * SipServiceHandle;


typedef void * SipDialogHandle;

#define SIP_INVALID_HANDLE NULL


//Э��ջ����ֵ������
typedef enum
{	
	eSIP_RET_SUCCESS = 0,
	eSIP_RET_FAILURE = -1,
	eSIP_RET_INIT_FAILURE					=	-11,  //��ʼ��ʧ��
	eSIP_RET_LISTEN_ADDR_FAILURE			=	-12,  //���Ӽ�����ʧ��
	eSIP_RET_BUILD_REGISTER_FAILURE			=	-13,  //����ע������ʧ��
	eSIP_RET_SEND_REGISTER_FAILURE			=	-14,  //����ע��ʧ��
	eSIP_RET_REGISTER_ACK_FAILURE			=	-15,  //ע��Զ˷���ʧ��
	eSIP_RET_REGISTER_ACK_TIMEOUT			=	-16,  //ע��ȴ����س�ʱ
	eSIP_RET_BUILD_INVITE_FAILURE			=	-17,  //���� INVITE ������ʧ��
	eSIP_RET_SEND_INVITE_FAILURE			=	-18,  //���� INVITE ʧ��
	eSIP_RET_NOT_FIND_INVITESESSION			=	-19,  //
	eSIP_RET_BUILD_INFO_FAILURE				=	-20,
	eSIP_RET_SEND_INFO_FAILURE				=	-21,
	eSIP_RET_BUILD_NOTIFY_FAILURE			=	-22,
	eSIP_RET_SEND_NOTIFY_FAILURE			=	-23,
	eSIP_RET_BUILD_SUBSCRIBE_FAILURE		=	-24,
	eSIP_RET_SEND_SUBSCRIBE_FAILURE			=	-25,
	eSIP_RET_BUILD_DEFAULT_MSG_FAILURE		=	-26,
	eSIP_RET_SEND_DEFAULT_MSG_FAILURE		=	-27,
	eSIP_RET_ADD_AUTHENTICATION_FAILURE		=	-28,
	eSIP_RET_BUILD_ACK_FAILURE = -29,

	eSIP_RET_E_STATUS = -100, //����״̬
	eSIP_RET_E_NMEN = -101, //û���ڴ�
	eSIP_RET_E_SNNEXIST = -102, //session ������
	eSIP_RET_E_TIMEOUT = -103, //��ʱ
	eSIP_RET_E_INVALID = -104, //��Ч����
	eSIP_RET_E_NFUNC = -105, //�ù���ûʵ��

	eSIP_RET_OSIP_E_NINIT = -1000, //OSIP û�г�ʼ��
	eSIP_RET_OSIP_E_OPER = -10000, //OSIP ��������ʧ��
	

} EnumSipErrorCode;


//SIP Э��ķ�ʽ
typedef enum
{
	eSIP_METHOD_INVALID = 0,

	eSIP_METHOD_REGISTER,

	eSIP_METHOD_INVITE,
	eSIP_METHOD_INFO,
	eSIP_METHOD_NOTIFY,
	eSIP_METHOD_SUBSCRIBE,
	eSIP_METHOD_BYE,
	eSIP_METHOD_CANCEL,

	eSIP_METHOD_INFOEX,
	eSIP_METHOD_NOTIFYEX,

	eSIP_METHOD_MESSAGE,

	eSIP_METHOD_ACK,

	eSIP_METHOD_RESPONSE,

}EnumSipMethod;


//SIP Content ����
typedef enum
{	
	eSIP_CONTENT_NONE = 0, //û������
	eSIP_CONTENT_SDP = 1,           //sdp ��������
	eSIP_CONTENT_MANSCDP_XML = 2, // manscdp_xml ��������
	eSIP_CONTENT_INNER = 3, //�ڲ�ʹ��	
	eSIP_CONTENT_UNKNOWN = 4,
	eSIP_CONTENT_MANSRTSP = 5, // mansrtsp
	eSIP_CONTENT_DATE = 6, //����ʱ��   yyyy-mm-dd hh:mm:ss
	eSIP_CONTENT_RTSP = 7, //RTSP
}EnumSipContentType;


//SIP ���ӷ�ʽ
typedef enum
{
	eSIP_CONNECT_UDP = 0,
	eSIP_CONNECT_TCP = 1
}EnumSipConnectType;


#define SIP_MAX_IP_LEN 32
//������Ϣ
typedef struct _StruSipConnnectInfo
{
	EnumSipConnectType eConnectType;
	int bOnline;
	int iLocalPort;
	int iRemotePort;
	char szLocalIp[SIP_MAX_IP_LEN+1];
	char szRemoteIp[SIP_MAX_IP_LEN+1];
}StruSipConnnectInfo;

//SIP Э��ظ��붨��
#define SIP_RESPONSE_CODE_SUCCESS  200  //�ɹ�

#define SIP_RESPONSE_CODE_FAILURE		400  // 
 // �������� �����˴����﷨
#define SIP_RESPONSE_CODE_BAD_REQUEST   400 
// δ��Ȩ
#define SIP_RESPONSE_CODE_UNAUTHORIZED	401 
// ��ֹ����������ʶ���﷨�� �ܾ����
#define SIP_RESPONSE_CODE_FORBIDDEN	   403 
// û���ҵ�, �û�û���ҵ�
#define SIP_RESPONSE_CODE_NOTFOUND	   404
// ����������
#define SIP_RESPONSE_CODE_NOTALLOWED	   405
// ����ʱ
#define SIP_RESPONSE_CODE_REQTIMEOUT	   408
// ��֧�ֵ�ý������, content ����
#define SIP_RESPONSE_CODE_BADMEDIA		415
// ϵͳæ
#define SIP_RESPONSE_CODE_BUSY		486
// ����������
#define SIP_RESPONSE_CODE_ESRV		500
// δʵ�нӿ�
#define SIP_RESPONSE_CODE_UNIMP		501

//SIP �ظ��Ļ������
#define SIP_MAX_RESPONSE_ERROR_LEN 128
typedef struct _StruSipResponse
{
	bool bOk;
	int  iSipErrno;
	char szMessage[SIP_MAX_RESPONSE_ERROR_LEN];
}StruSipResponse;



#define SIP_MAX_CONTENT_LEN  (32*1024)



//SIP ���ݰ�������
typedef enum
{
	eSIP_DATA_REQUEST = 0,
	eSIP_DATA_RESPONSE = 1
}EnumSipDataType;

#define SIP_MAX_SUBJECT_STRING 127

#define SIP_MAX_DIGID_STRING 128
#define SIP_MAX_TAG_STRING   128
#define SIP_MAX_VIAHOST_STIRNG 128

typedef struct _StruSipDialogKey
{
	long iCnnId; //����ID
	int iCallId;   //ͨ�ŵ�CALL ID
	int iDialogId; //�Ի�ID
	int iTransId; // id for transactions (to be used for answers)	
	int iCSeq;	
	char szContactScheme[8];
	char szContactHost[128]; 
	char szContackUsername[128];
	int  iContactPort;  // host:port	
	int iExpirse;
	long iRcvTime; //���ܵ������ʱ��
	char szSubject[SIP_MAX_SUBJECT_STRING+1];
	char szFrom[256];
	char szTo[256];

	char czDialogKey[SIP_MAX_DIGID_STRING+1];
	char czFromTag[SIP_MAX_TAG_STRING+1];
	char czToTag[SIP_MAX_TAG_STRING+1];
	char czViaHost[SIP_MAX_VIAHOST_STIRNG+1];
}StruSipDialogKey;



typedef struct _StruSipData
{
	StruSipDialogKey stDialog;
	EnumSipDataType eDataType;
	EnumSipMethod eMethod;	
	EnumSipContentType eContentType;
	int iContentLength;
	char vContent[SIP_MAX_CONTENT_LEN+1];
	StruSipResponse stResponseResult;
}StruSipData;

//�ؼ��ֶ���

#define SIP_STRKEY_USERNAME   "UserName"   //�û���
#define SIP_STRKEY_RESULT     "Result"     //�������

#define SIP_STRKEY_CMDTYPE                         "CmdType"        //��������
#define SIP_STRKEY_QUERY                           "Query"          //��ѯ
#define SIP_STRKEY_SN                              "SN"             //�������
#define SIP_STRKEY_DEVICEID                        "DeviceID"       //�豸ID
#define SIP_STRKEY_RESPONSE                        "Response"       //�ظ�
#define SIP_STRKEY_SUMNUM                          "SumNum"         //����
#define SIP_STRKEY_DEVICELIST                      "DeviceList"     //�豸�б�
#define SIP_STRKEY_NUM                             "Num"            //����
#define SIP_STRKEY_ITEM                            "Item"           //��Ŀ
#define SIP_STRKEY_NAME                            "Name"           //����
#define SIP_STRKEY_MANUFACTURER                    "Manufacturer"   //����
#define SIP_STRKEY_MODEL                           "Model"          //�豸�ͺ�
#define SIP_STRKEY_OWNER                           "Owner"          //�豸����
#define SIP_STRKEY_CIVILCODE                       "CivilCode"      //��������
#define SIP_STRKEY_BLOCK                           "Block"          //����
#define SIP_STRKEY_ADDRESS                         "Address"        //�豸��װ��ַ
#define SIP_STRKEY_PARENTAL                        "Parental"       //�ӽڵ���ڱ�ʶ
#define SIP_STRKEY_PARENTID                        "ParentID"       //���ڵ�ID
#define SIP_STRKEY_SAFEWAY                         "SafeWay"        //���ȫģʽ
#define SIP_STRKEY_REGISTERWAY                     "RegisterWay"    //ע�᷽ʽ
#define SIP_STRKEY_CERTNUM                         "CertNum"        //֤�����к�
#define SIP_STRKEY_CERTIFIABLE                     "Certifiable"    //֤����Ч��ʶ
#define SIP_STRKEY_ERRCODE                         "ErrCode"        //��Чԭ��
#define SIP_STRKEY_STARTTIME                       "StartTime"      //��ʼʱ��
#define SIP_STRKEY_ENDTIME                         "EndTime"        //����ʱ��
#define SIP_STRKEY_SECRECY                         "Secrecy"        //��������
#define SIP_STRKEY_IPADDRESS                       "IPAddress"      //IP��ַ
#define SIP_STRKEY_PORT                            "Port"           //�˿�
#define SIP_STRKEY_PASSWORD                        "Password"       //����
#define SIP_STRKEY_STATUS                          "Status"         //״̬
#define SIP_STRKEY_LONGITUDE                       "Longitude"      //����
#define SIP_STRKEY_LATITUDE                        "Latitude"       //γ��
#define SIP_STRKEY_DEVICETYPE                      "DeviceType"     //�豸����(DVR��)
#define SIP_STRKEY_FIRMWARE                        "Firmware"       //�豸�̼��汾
#define SIP_STRKEY_MAXCAMERA                       "MaxCamera"      //�����Ƶͨ����
#define SIP_STRKEY_MAXALARM                        "MaxAlarm"       //���澯ͨ����
#define SIP_STRKEY_ONLINE                          "Online"         //�Ƿ�����
#define SIP_STRKEY_STATUS                          "Status"         //�Ƿ���������
#define SIP_STRKEY_ENCODE                          "Encode"         //�Ƿ����
#define SIP_STRKEY_RECORD                          "Record"         //�Ƿ�¼��
#define SIP_STRKEY_DEVICETIME                      "DeviceTime"     //�豸ʱ�������
#define SIP_STRKEY_ALARMSTATUS                     "Alarmstatus"    //�����豸״̬
#define SIP_STRKEY_DUTYSTATUS                      "DutyStatus"     //����״̬
#define SIP_STRKEY_NOTIFY                          "Notify"         //ͨ��
#define SIP_STRKEY_CONTROL                         "Control"        //����
#define SIP_STRKEY_PTZCMD                          "PTZCmd"         //PTZ����
#define SIP_STRKEY_INFO                            "Info"           //��Ϣ
#define SIP_STRKEY_CONTROLPRIORITY                 "ControlPriority" //�������ȼ�
#define SIP_STRKEY_RECORDCMD                       "RecordCmd"      //¼������
#define SIP_STRKEY_FILEPATH                        "FilePath"       //�ļ�·����
#define SIP_STRKEY_TYPE                            "Type"           //¼���������
#define SIP_STRKEY_RECORDERID                      "RecorderID"     //¼�񴥷���ID
#define SIP_STRKEY_RECORDLIST                      "RecordList"     //�ļ�Ŀ¼���б�
#define SIP_STRKEY_NOTIFYTYPE                      "NotifyType"     //ͨ������
#define SIP_STRKEY_GUARDCMD                        "GuardCmd"       //����/��������
#define SIP_STRKEY_ALARMCMD                        "AlarmCmd"       //������λ����
#define SIP_STRKEY_TELEBOOT                        "TeleBoot"       //Զ��������������
#define SIP_STRKEY_ALARMPRIORITY                   "AlarmPriority"  //��������
#define SIP_STRKEY_ALARMMETHOD                     "AlarmMethod"    //������ʽ
#define SIP_STRKEY_ALARMTIME                       "AlarmTime"      //����ʱ��
#define SIP_STRKEY_ALARMDESCRIPTION                "AlarmDescription" //������������
#define SIP_STRKEY_CHANNEL                         "Channel"        //��Ƶ����ͨ����



#define SIP_STRKEY_SESSION_NAME                    "s"              //����ý�����Ĳ�������
#define SIP_STRKEY_MEDIA_ATTRIBUTE                 "a"              //ý���������
#define SIP_STRKEY_URI_DESCRIPTION                 "u"              //ý��Դ��ʶ
#define SIP_STRKEY_TIME_DESCRIPTION                "t"              //ʱ������


// �����
#define SIP_STR_CATALOG                         "Catalog"        //Ŀ¼
#define SIP_STR_DEVICEINFO                      "DeviceInfo"     //�豸��Ϣ
#define SIP_STR_DEVICESTATUS                    "DeviceStatus"   //�豸״̬
#define SIP_STR_KEEPALIVE                       "Keepalive"      //���
#define SIP_STR_RECORDINFO                      "RecordInfo"     //¼����Ϣ
#define SIP_STR_DEVICECONTROL                   "DeviceControl"  //�豸����
#define SIP_STR_RECORD                          "Record"         //¼��
#define SIP_STR_STOPRECORD                      "StopRecord"     //¼��
#define SIP_STR_MEDIASTATUS                     "MediaStatus"    //ý��״̬
#define SIP_STR_SETGUARD                        "SetGuard"       //����
#define SIP_STR_RESETGUARD                      "ResetGuard"     //����
#define SIP_STR_RESETALARM                      "ResetAlarm"     //������λ
#define SIP_STR_ALARM                           "Alarm"          //����֪ͨ
#define SIP_STR_RESETALARM                      "ResetAlarm"     //������λ
#define SIP_STR_BOOT                            "Boot"           //�豸����

#define SIP_STR_PLAY                             "Play"          //ʵʱ���㲥
#define SIP_STR_PLAYBACK                         "Playback"      //��ʷ�ط�
#define SIP_STR_DOWNLOAD                         "Download"      //�ļ�����


#define SIP_STR_RTSP_PLAY                        "PLAY"          //����
#define SIP_STR_RTSP_PAUSE                       "PAUSE"         //��ͣ
#define SIP_STR_RTSP_TEARDOWN                    "TEARDOWN"      //ֹͣ
#define SIP_STR_MANSRTSP_VERSION                 "MANSRTSP/1.0"  //�汾
#define SIP_STR_RTSP_CSEQ                        "CSeq"          //���
#define SIP_STR_RTSP_SCALE                       "Scale"         //�ٶ�
#define SIP_STR_RTSP_RANGE                       "Range"         //��Χ
#define SIP_STR_RTSP_NPT                         "npt"           //ʱ��
#define SIP_STR_RTSP_NOW                         "now"           //��ǰ



/*
 *********************************************
 Function : StruSipDialogKey_Cmp
 Version : 1.0.0.1
 Author : ������
 DateTime : 2012/12/7 9:33
 Description :  StruSipDialogKey �ıȽϺ���
 Input :  pSrc �Ƚϵ�����
 Input :  pDest �Ƚϵ�����
 Output : 
 Return : ��ȷ��� 0�� ��� *pSrc > *pDest ���� 1�� �������� -1
 Note :   
 *********************************************
 */
SIP_DLL_API int SipDialogKey_Cmp(const StruSipDialogKey *pSrc,const StruSipDialogKey *pDest );


/*
*********************************************
Function : SipSession_SetUserData
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:45
Description :  �洢�û����Ự����
Input : hSipSession  �Ự�ľ��
Input :  pData �û�����
Output : 
Return : 
Note :   ��ʼĬ��Ϊ NULL
*********************************************
*/
SIP_DLL_API void SipSession_SetUserData(SipSessionHandle hSipSession, void *pUserData );


/*
*********************************************
Function : SipSession_GetUserData
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:46
Description :  ���� �Ự�洢���û�����
Input : hSipSession  �Ự�ľ��
Input :  
Output : 
Return : ����SetUserData�洢����
Note :   
*********************************************
*/
SIP_DLL_API void *SipSession_GetUserData(SipSessionHandle hSipSession);





/*
*********************************************
Function : SipSession_Resigter
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:47
Description :  �Ự����ע������
Input : hSipSession  �Ự�ľ��
Input :  czUsername �û���
Input :  csPassword  ����
Input : iTimeout ��ʱʱ��,��λ ����
Output : pRes ���صĽ���� ��� ΪNULL ��ʾ ���ȴ��������
Input : iTimeouts �ȴ����صĳ�ʱʱ�� ����λ ����
Return : 
Note :   
*********************************************
*/
SIP_DLL_API EnumSipErrorCode SipSession_Resigter(SipSessionHandle hSipSession,
												 const char *czUserName, 
												 const char *czPassword,
												 StruSipData *pRes, int iTimeout );
SIP_DLL_API EnumSipErrorCode SipSession_Unresigter(SipSessionHandle hSipSession,
												 StruSipData *pRes, int iTimeout );

/*
*********************************************
Function : SipSession_Disconnect
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:50
Description :  �Ͽ����λỰ
Input : hSipSession  �Ự�ľ��
Input :  
Output : 
Return : 
Note :   
*********************************************
*/
SIP_DLL_API EnumSipErrorCode SipSession_Disconnect(SipSessionHandle hSipSession);

SIP_DLL_API void SipSession_Release(SipSessionHandle hSipSession);
/*
*********************************************
Function : SipSession_SendMessage
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:17
Description :  ��������
Input : hSipSession  �Ự�ľ��
Input :  eMethod ���͵ķ���
Input : eContentType ���ݵ���������
Input : czContent �ı���ʽContent
Input : pContent �����Ʒ�ʽ��Content
Input : iContentLength pContent ����
Output : ppRes ���صĽ���� ��� ΪNULL ��ʾ ���ȴ��������
Input : iTimeouts �ȴ����صĳ�ʱʱ�� ����λ ����
Output : pOutDlgKey �����ΪNULL ���� �Ự�ĵ�Key
Return : 
Note :   
*********************************************
*/
SIP_DLL_API EnumSipErrorCode SipSession_SendMessage(SipSessionHandle hSipSession,			   
								  const StruSipData *pSendData, StruSipData *pRes,
								  int iTimeouts,  StruSipDialogKey *pOutDlgKey);



//��ȡ�Ự��������Ϣ
SIP_DLL_API EnumSipErrorCode SipSession_GetConnectInfo(SipSessionHandle hSipSession, StruSipConnnectInfo *pRes);

//��ȡ�Ự�Ĺ������
SIP_DLL_API SipServiceHandle SipSession_GetService(SipSessionHandle hSipSession);


//����, ��������Ҫ����ע��
SIP_DLL_API EnumSipErrorCode SipSession_Reconnect(SipSessionHandle hSipSession);


//����ע���½���û���
SIP_DLL_API const char *SipSession_Authorization_Username(SipSessionHandle hSipSession);

/*
 *********************************************
 Function : SipSession_Authorize
 Version : 1.0.0.1
 Author : ������
 DateTime : 2012/9/26 8:51
 Description :  Ȩ�޺�׼, �ж� ������ czUsername��czPasswrd �Ƿ����ע���յ���Ȩ��֤�����
 Input :  czUsername �������û���������
 Input :  czPasswrd ����������, ����
 Output : 
 Return : �����жϷ��� SUCCESS, ������ʾ����
 Note :   
 *********************************************
 */
SIP_DLL_API EnumSipErrorCode SipSession_Authorize(SipSessionHandle hSipSession, 
												   const char *czUsername, const char *czPasswrd );

/*
 *********************************************
 Function : SipSession_SetOptions
 Version : 1.0.0.1
 Author : ������
 DateTime : 2012/12/26 14:20
 Description :  Session ��ѡ����ƺ���
 Input :  iOptionName ѡ������
 Input :  pValue ѡ��ֵ
 Input :  iValueSize pValue �ĳ���
 Output : 
 Return : 
 Note :   
 *********************************************
 */
//���� Session ���ߺ��Ƿ� ���� Unresigter , pValue Ϊ int *  1 ��ʾ �ǣ� 0 ��ʾ ��
#define SIP_SESSION_O_UNREGISTER  1  
//������־�ȼ��� pValue Ϊ int *  �� ֵ 0~7
#define SIP_GLOBAL_DEBUG_LEVEL 2
SIP_DLL_API EnumSipErrorCode SipSession_SetOptions(SipSessionHandle hSipSession, 
												  int iOptionName, const void *pValue, int iValueSize );

/*
*********************************************************************
*
*@brief : 
*
*********************************************************************
*/

//�¼�������
typedef struct _StruSipListener
{
	/*
	*********************************************
	Function : OnClientConnectEvent
	Version : 1.0.0.1
	Author : ������
	DateTime : 2012/8/17 9:53
	Description :  ���ܵ�������
	Input :  hService ���ܵ����ӵķ�����
	Input : pNewSession �µ�����
	Output : 
	Return : ������������ʾ�ܾ�����
	Note :   
	*********************************************
	*/
	EnumSipErrorCode (*OnClientConnectEvent)( SipServiceHandle hService,
										   SipSessionHandle hNewSession );

	//�Ͽ�����
	void (*OnClientDisconnectEvent)(SipServiceHandle hService, SipSessionHandle hNewSession);

	//���ϲ�Ļص����ô˽ӿ� ����������Ļص�����Ӧ��ص����� pData���;���
	void (*OnSIPPacketEvent)(SipServiceHandle hService, 
						SipSessionHandle hNewSession,
						StruSipData *pData);
}StruSipListener;



SIP_DLL_API SipServiceHandle SipService_Create(const StruSipListener *pListener);

SIP_DLL_API void SipService_Release(SipServiceHandle hSipService);

SIP_DLL_API void SipService_SetUserData(SipServiceHandle hSipService, void *pUserData );

SIP_DLL_API void *SipService_GetUserData(SipServiceHandle hSipService);

//���÷���������
SIP_DLL_API EnumSipErrorCode SipService_Set_ServerName(SipServiceHandle hSipService, const char *czServerName);

//��������
SIP_DLL_API EnumSipErrorCode SipService_Set_Gateway(SipServiceHandle hSipService, const char *czGatewayIPV4);


/*
*********************************************
Function : SipService_Start
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:56
Description :  ��ʼ����
Input : hSipService ���Ƶķ�����
Input :  eCnnType ��������, ��ǰֻ֧�� UDP
Input :  czLocalIp ���ذ󶨵�IP�� ���Ϊ NULL ��ʾȫ����
Input :  iLocalPort ���ذ󶨵Ķ˿�,��� Ϊ 0�� ϵͳָ��ѡ������
		�� SipService_Get_LocalPort ������ȡ��ʵ�˿�
Output : 
Return : 
Note :   ������ֻ���Ե���һ��
*********************************************
*/
SIP_DLL_API EnumSipErrorCode SipService_Start(SipServiceHandle hSipService,
										  EnumSipConnectType eCnnType,
										  const char *czLocalIp, int iLocalPort);

//���ر��ض˿ڣ� ��Start iLocalPort Ϊ < 1 ʱ�� ��ϵͳָ��ʱ�� �����ɴ˺���������ʵ�ļ����˿�
SIP_DLL_API int SipService_Get_LocalPort(SipServiceHandle hSipService);


//ֹͣ����
SIP_DLL_API void SipService_Stop(SipServiceHandle hSipService);

/*
*********************************************
Function : SipService_Connect/SipService_ConnectEx
Version : 1.0.0.1
Author : ������
DateTime : 2012/8/17 9:58
Description :  ��������
Input : hSipService ����ľ��
Input :  eCnnType ��������
Input : czRemoteHost �Զ˵�ַ
Input :  iRemotePort �Զ�IP
Output : 
Return : ʧ�ܷ��� SIP_INVALID_HANDLE�� ���򷵻���Ϣ�Ự���
Note :   
*********************************************
*/
SIP_DLL_API SipSessionHandle    SipService_Connect( SipServiceHandle hSipService,
								EnumSipConnectType eCnnType, 
								const char *czRemoteHost,int iRemotePort
								,const char *czRemoteSipServerName  = 0);


/*
 *********************************************
 Function : SipService_GetListenInfo
 Version : 1.0.0.1
 Author : ������
 DateTime : 2012/9/7 9:54
 Description :  ��ȡ������������Ϣ
 Input : hSipService ����ľ��
 Output : pRes ��ȡ������Ϣ
 Return : 
 Note :   
 *********************************************
 */
SIP_DLL_API EnumSipErrorCode SipService_GetListenInfo(SipServiceHandle hSipService, StruSipConnnectInfo *pRes);

#endif //end _GS_H_SIPSTACK_H_
