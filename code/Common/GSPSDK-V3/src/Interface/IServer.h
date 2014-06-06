#ifndef GSP_ISERVER_DEF_H
#define GSP_ISERVER_DEF_H

#include "GSPConfig.h"
#include "IUri.h"
#include "ISource.h"



namespace GSP
{

//Ϊ����ʹ��



	class  CGSPServerOptions 
	{
	public:
		BOOL m_bSecurity; //ʹ�ܰ�ȫ���
		BOOL m_bTCP; //ʹ�� TCP Э��
		INT  m_iTCPPort;  //TCP �����˿�
		UINT32 m_iMaxKeepalive; //��
		BOOL m_bUDP; //ʹ�� UDP Э��
		INT m_iUDPPort;    //UDP �����˿�
		UINT32 m_iMaxClients;            
		BOOL m_bFlowCtrl; //ʹ����������
		BOOL m_bCoderConver; //ʹ��ת�빦��	
		std::string m_strDynamicModulePath; //��̬ģ�����Ŀ¼
		UINT16 m_iRetryPktsCache; //�ش�����CACHE������С  Ĭ�� GSP_RETRYS_PACKETS_CACHE
		BOOL m_bEnableSecurity; //�Ƿ����ð�ȫӡ֤ ,Ĭ�� TRUE
		UINT m_iLogLevelConsole;
		UINT m_iLogLevelFile;

		CGSPServerOptions(void);

	};
 


/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : IGSPSERVER.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/6/11 12:10
Description:  GSP �������ӿ�
********************************************
*/
class CIServer;


/*
*********************************************
Function :   GSPServerEventFunctionPtr
DateTime : 2010/6/11 16:46
Description : �������¼��ص�����
Input :  pServer ��Ӧ�ķ�����
Input :  pSource  �����¼�������Դ, ����Ƿ������¼���ֵΪNULL�� ����ο� EnumGSPServerEvent
Input :  eEvent �¼����ͣ� �ο�  EnumGSPServerEvent����
Input :  pEventParams �¼������� ����ֵ�� eEvent������ �ο�  EnumGSPServerEvent����
Input :  iLen pEventParams �ĳ���
Input :  pParamData �ص��û�����

Output :
Return : ����URI����������Դ, ʧ�ܷ���NULL
Note :
*********************************************
*/
typedef enum
{
   GSP_SRV_EVT_CTRL,  //��������Դ�¼���pSource ��Ч��  �¼�����pEventPararms Ϊ StruGSPCmdCtrl *, ����TRUE/FALSE
   GSP_SRV_EVT_REF,   //����Դ���������¼���pSource ��Ч��  �¼�����pEventPararms Ϊ NULL, ����ֵ����
   GSP_SRV_EVT_UNREF, //����Դ���ü����¼���pSource ��Ч��  �¼�����pEventPararms Ϊ NULL, ����ֵ����
   GSP_SRV_EVT_RQUEST_SOURCE, //��������Դ�ص�����, pSource ��Ч�� �¼�����pEventPararms Ϊ CIUri *, ���� CISource *
}EnumGSPServerEvent;     

typedef void* (*GSPServerEventFunctionPtr)(CIServer *pServer, CISource *pSource,
										  EnumGSPServerEvent eEvent, 
                                               void *pEventPararms, 
											   INT iLen,  void *pParamData );

//״̬��ѯ
typedef enum
{
	GSP_SRV_STATUS_SUPPORT_PROTOCOL = 0, //֧�ֵ�Э��	, ���ݽ��Ϊ StruProtocolService*
	GSP_SRV_STATUS_SOURCE_LIST, //�洢������Դ. ���ݽ��Ϊ StruSourceVector *
	GSP_SRV_STATUS_GET_CLIENTS_INFO, //���ؿͻ�������,  StruClientVector *
}EnumGSPServerStatus;


typedef struct _StruProtocolService
{
	INT iNums; //ָ�� vService �ĸ���
	struct _StruServiceInfo
	{
		const  char *czProtocol;
		const char *czSrvBindIP;
		INT iSrvBindPort;
		INT iStatus;   // 0 ������ �����쳣
	}vService[1];
}StruProtocolService;

typedef struct _StruSourceVector
{
	INT iNums; //ָ�� vSourceIndex �ĸ���
	UINT16 vSourceIndex[1]; //����Դ������
}StruSourceVector;


typedef struct _StruClientInfo
{
	EnumProtocol eProtocol; //Э��
	char szRemoteIP[GSP_STRING_IP_LEN]; //�Զ�IP
	INT iReomtePort; //�Զ˶˿�
	char szSourceKey[GSP_MAX_URI_KEY_LEN]; //��������Դ��KEY
	UINT64 iRecv; //���յ��ֽ���
	UINT64 iSend; //���͵��ֽ���
	UINT32 iLostFrames; //��֡��
	UINT32 iResendPackets; //�ش�����
} StruClientInfo;

typedef struct _StruClientVector
{
	INT iNums; //ָ�� vSourceIndex �ĸ���
	StruClientInfo vClient[1];
}StruClientVector;



class  CIServer 
{
public:
	
    virtual ~CIServer(void)
    {

    }

    /*
    *********************************************
    Function :  InitLog 
    DateTime : 2010/6/11 16:19
    Description : ������־���ݽӿ�
    Input :   czPathName ������־�ӿڵ�·��
    Output :  
    Return :  
    Note :
    *********************************************
    */
    virtual void InitLog( const char *czPathName ) = 0;


    

    /*
    *********************************************
    Function : Init
    DateTime : 2010/6/11 16:22
    Description :  ��������
    Input :
    Output :
    Return : TRUE/FALSE
    Note :
    *********************************************
    */
    virtual BOOL Init(const char *csConfigFilename,const char *csLogPathName) = 0;

    /*
    *********************************************
    Function : Stop
    DateTime : 2010/6/11 16:22
    Description :  ֹͣ����
    Input :
    Output :
    Return : TRUE/FALSE
    Note :
    *********************************************
    */
    virtual BOOL Stop(void) = 0;


    /*
    *********************************************
    Function :   FindSource
    DateTime : 2010/6/11 16:27
    Description :  �ü�ֵ����������Դ
    Input : czKey Ҫ���ҵ�����Դ��ֵ
    Output :
    Return :  �����ڷ���NULL
    Note :
    *********************************************
    */
    virtual CISource *FindSource(const char *czKey) = 0;

    virtual CISource *FindSource(UINT16 iSourceIndex ) = 0;


    /*
    *********************************************
    Function :   AddPushSource
    DateTime : 2010/6/11 16:27
    Description :  �����ģʽ����Դ
    Input : czKey ����Դ��ֵ
    Output :
    Return : ���ؽ�������ģʽ����Դ�� ʧ�ܷ���NULL
    Note :
    *********************************************
    */
    virtual CISource *AddSource( const char *czKey ) = 0;


	/*
	*********************************************
	Function :   AddPushSource
	DateTime : 2010/6/11 16:27
	Description :  �����ģʽ����Դ
	Input : czKey ����Դ��ֵ
	Output :
	Return : ���ؽ�������ģʽ����Դ�� ʧ�ܷ���NULL
	Note :
	*********************************************
	*/
	virtual CISource *AddPullSource( const char *czKey ) = 0;


 

    virtual CISource::EnumRetNO WriteData( UINT16 iSourceIndex,  const void *pData, INT iLen, UINT iChn, BOOL bKey) = 0; 


    virtual CISource::EnumRetNO WriteSysHeader( UINT16 iSourceIndex, const void *pData, INT iLen, UINT iChn) = 0;

    virtual CISource::EnumRetNO WriteDataV( UINT16 iSourceIndex,  const StruIOV *pIOV, INT iVNums, UINT iChn, BOOL bKey) = 0; 


    virtual CISource::EnumRetNO WriteSysHeaderV( UINT16 iSourceIndex, const StruIOV *pIOV, INT iVNums, UINT iChn) = 0;


    /*
    *********************************************
    Function : SetEventCallback
    DateTime : 2010/6/11 16:32
    Description :  ���÷��������¼��ص�
    Input :   fnOnEvent �ص�����
    Input : pParam  �ص����û�����
    Output :
    Return : 
    Note :
    *********************************************
    */
    virtual void SetEventCallback( GSPServerEventFunctionPtr fnOnEvent, void *pParam) = 0;

	
	virtual CIUri *CreateEmptyUriObject(void) = 0;

    virtual void InitURI( CIUri &csRet, const char *czKey, const char *czPro = "gsp", const char *szRemoteIP = NULL) = 0;

	/*
	 *********************************************
	 Function : QueryStatus
	 DateTime : 2012/3/13 15:38
	 Description :  ״̬��ѯ
	 Input :  eQueryStatus ָ����ѯ��ѡ��
	 Output : ppResult ���ص����ݽ��
	 Output : pResultSize ����Ĵ�С
	 Return : 
	 Note :   �����Ҫ���� FreeQueryStatusResult �ͷ�
	 *********************************************
	 */
	virtual BOOL QueryStatus(const EnumGSPServerStatus eQueryStatus, char **ppResult,INT *pResultSize  ) = 0;

	/*
	 *********************************************
	 Function : FreeQueryStatusResult
	 DateTime : 2012/3/13 15:44
	 Description :  �ͷŲ�ѯ�Ľ����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	virtual void FreeQueryStatusResult( char *pResult) = 0;



	/*
	*********************************************
	Function : GetOptions
	DateTime : 2010/6/11 16:19
	Description : ���ط�������������
	Input :
	Output :  ����������
	Return :
	Note : �ĺ����Ѿ�����
	*********************************************
	*/
	virtual BOOL GetOptions(CGSPServerOptions &Opts) const  = 0;

	/*
	*********************************************
	Function : SetOptions
	DateTime : 2010/6/11 16:19
	Description : ���÷�������������
	Input :   csOptions ������ �ο�  CGSPServerOptions ����
	Output :  
	Return :  TRUE/FALSE
	Note :  �ĺ����Ѿ�����
	*********************************************
	*/
	virtual BOOL SetOptions(const CGSPServerOptions &csOptions) = 0;
 
};




/*
*********************************************
Function :   CreateGSPServerInterface
DateTime : 2010/8/5 17:22
Description :  �����������ӿڶ���
Input :
Output :  ����ģ��ӿڶ��� ʧ�ܷ���NULL
Return :
Note :
*********************************************
*/
GS_API GSP::CIServer *CreateGSPServerInterface(void);


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

};

#endif
