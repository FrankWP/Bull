#ifndef NET_COMMON_STACK_DEF_H
#define NET_COMMON_STACK_DEF_H

#include "GSCommon.h"

#ifdef _WIN32

#else
#define CALLBACK 
#endif

//ID����
typedef struct StruID
{
	UINT uiPlatformID;
	UINT uiSysID;
}StruID;

class INetMessage
{
public:
	INetMessage(){};
	virtual ~INetMessage(){};
public:
	/***************************************************************************
	  Function:        	GetMSGLen
	  DATE:				2010-6-9   9:06
	  Description: 		��ȡ��Ϣ����   
	  Input:  			           
	  Output:     		    
	  Return:   		      
	  Note:						
	****************************************************************************/
	virtual INT32 GetMSGLen() = 0;
	/***************************************************************************
	  Function:        	GetMSG
	  DATE:				2010-6-9   9:06
	  Description: 		��ȡ��Ϣ   
	  Input:  			           
	  Output:     		    
	  Return:   		      
	  Note:						
	****************************************************************************/
	virtual void* GetMSG() = 0;
	/***************************************************************************
	  Function:        	GetSessionID
	  DATE:				2010-6-9   9:07
	  Description: 		��ȡ�ỰID   
	  Input:  			           
	  Output:     		    
	  Return:   		      
	  Note:						
	****************************************************************************/
	virtual UINT16 GetSessionID() = 0;
	/***************************************************************************
	  Function:        	GetCommandID
	  DATE:				2010-6-9   9:07
	  Description: 		��ȡ����ID   
	  Input:  			           
	  Output:     		    
	  Return:   		      
	  Note:						
	****************************************************************************/
	virtual UINT32 GetCommandID() = 0;
	virtual StruID* GetSrcModule() = 0;
	virtual StruID* GetDstModule() = 0;
};
class INetChannel;

#define NET_EVENT_CONNECT	0
#define NET_EVENT_READ		1
#define NET_EVENT_CLOSE		2
#define NET_EVENT_CONNECT_SERVER 3


typedef INT32 (CALLBACK* fnNetEvent)(void* pCaller, INetChannel* pChn, INT32 iEvent, INetMessage* pNetMessage);




//����Э�� SOCK_STREAM
#define PROTOCOL_TYPE_TCP				1			
//����Э�� SOCK_DGRAM
#define PROTOCOL_TYPE_UDP				2	

//Ĭ��֧�ֿͻ�����������
#define DEFAULT_NET_CHANNEL_NUM					64
//���֧�ֿͻ�����������
#define MAX_NET_CHANNEL_NUM						(1024*1024)	

#define SUPPORT_MESSAGE_BUF				1
#define SUPPORT_NO_MESSAGE_BUF			0

#define DEFAULT_MSG_BUF_SIZE			(1024*2)
#define MAX_MSG_BUF_SIZE				(1024*1024)

#define DEFAULT_MSG_SIZE				(1024*8)
#define MAX_MSG_SIZE					(1024*1024*2)

#define DEFAULT_NET_PULSE_TIME			5000

#define SUPPORT_RECONNECT				1
#define SUPPORT_NO_RECONNECT			0
#define DEFAULT_RECONNECT_TIME			2000

//30��û�յ����ݣ�����Ϊ��·�ж�
#define NET_DATA_TIMEOUT				30

//����ʱ�ͻ����Ƿ���ͬһ���˿ڽ������� 1��ʾĬ������.Ϊ�˼���֮ǰ��д�� ����Ĭ������
#define REUSE_NETPORT_FLAG					1

class INetConfig
{
public:
	INetConfig(){};
	virtual ~INetConfig(){};

public:
	virtual void SetProtocolType(INT32 iProtocolType) = 0;
	virtual INT32 GetProtocolType() = 0;

	virtual void SetMaxChnNum(INT32 iMaxChnNum) = 0;
	virtual INT32 GetMaxChnNum() = 0;

	virtual void SetSupportMsgBuf(INT32 iSupport) = 0;
	virtual INT32 GetSupportMsgBuf() = 0;
	virtual void SetMaxMsgBufSize(INT32 iBufSize) = 0;
	virtual INT32 GetMaxMsgBufSize() = 0;

	virtual void SetMaxMsgSize(INT32 iMsgSize) = 0;
	virtual INT32 GetMaxMsgSize() = 0;

	virtual void SetNetPulseTime(INT32 iPulseTime) = 0;
	virtual INT32 GetNetPulseTime() = 0;

	virtual void SetSupportReConnect(INT32 iSupport) = 0;
	virtual INT32 GetSupportReConnect() = 0;
	virtual void SetReConnectTime(INT32 iReConnTime) = 0;
	virtual INT32 GetReConnectTime() = 0;

	virtual void SetNetEventCB(void* pCaller, fnNetEvent EventEntry) = 0;
	virtual void* GetNetEventCaller() = 0;
	virtual fnNetEvent GetNetEventEntry() = 0;

	virtual void SetNetDataTimeout(INT32 iTimeOut) = 0;
	virtual INT32 GetNetDataTimeout() = 0;

	virtual void SetReuseNetPortFlag(INT32 iReuse) = 0;
	virtual INT32 GetReuseNetPortFlag() = 0;
};



#define NET_CHANNEL_STATUS_INVALID		0
#define NET_CHANNEL_STATUS_ACTIVE		1
#define NET_CHANNEL_STATUS_SHUTDOWN		2
#define NET_CHANNEL_STATUS_SOCKET_ERROR	3
#define NET_CHANNEL_STATUS_NO_CONNECT	4


class INetChannel
{
public:
	INetChannel(){};
	virtual ~INetChannel(){};

	virtual char* GetRemoteIPAddrString() = 0;
	virtual UINT32 GetRemoteIPAddr() = 0;
	virtual char* GetLocalIPAddrString() = 0;
	virtual UINT32 GetLocalIPAddr() = 0;
	virtual UINT16 GetRemotePort() = 0;
	virtual UINT16 GetLocalPort() = 0;
	/***************************************************************************
	  Function:        	GetLogicRemoteIPAddrString
	  DATE:				2013-6-5   9:01
	  Description: 		��ȡ�߼���Զ��ip��ַ����һ������������socket���ӵĵ�ַ�����û�����ѡ�����øõ�ַ��
						���磬����BS��½������ʱ������BS�ͻ��˵ĵ�ַ��������tomcat�ĵ�ַ��   
	  Return:   		�߼���Զ��IP��ַ 
	****************************************************************************/
	virtual void SetLogicRemoteIPAddrString(const char* pszIPAddr) = 0;
	/***************************************************************************
	  Function:        	GetLogicRemoteIPAddrString
	  DATE:				2013-6-5   9:01
	  Description: 		��ȡ�߼���Զ��ip��ַ����һ������������socket���ӵĵ�ַ�����û�����ѡ�����øõ�ַ��
						���磬����BS��½������ʱ������BS�ͻ��˵ĵ�ַ��������tomcat�ĵ�ַ��   
	  Return:   		�߼���Զ��IP��ַ 
	****************************************************************************/	
	virtual char* GetLogicRemoteIPAddrString() = 0;

	virtual INT32 ConnectServer(char* pszRemoteIPAddr, UINT16 uiRemotePort,
		char* pszBindingIPAddr, UINT16 uiBindingPort) = 0;
	virtual INT32 CloseChannel() = 0;
	virtual INT32 CloseChannelEx() = 0;
	virtual INT32 GetChnStatus() = 0;
	/***************************************************************************
	  Function:        	SendRemoteMsg
	  DATE:				2010-5-26   9:01
	  Description: 		ͨ��ͨ����Զ�˷�����Ϣ   
	  Input:  			uiCMDID:�����
						pMSG:���͵���Ϣ����
						iMSGLen:���͵ĳ���
						uiSessionID:�ỰID�����û���д
						pSrcID:����ԭ��Ԫ
						pDstID:����Ŀ�ĵ�Ԫ
	  Output:     		    
	  Return:   		���͵��ֽ�����������ʾʧ��      
	  Note:						
	****************************************************************************/
	virtual INT32 SendRemoteMsg(UINT32 uiCMDID,
								void* pMSG, 
								INT32 iMSGLen, 
								StruID* pSrcID,
								StruID* pDstID,
								UINT16 uiSessionID = 0) = 0;
};

class INetServer
{
public:
	INetServer(){};
	virtual ~INetServer(){};

	virtual void SetNetConfig(INetConfig* pCfg) = 0;
	virtual INetConfig* GetNetConfig() = 0;

	virtual INT32 StartListen(char* pszBindingIPAddr, UINT16 uiBindingPort, char* pszLogPath = NULL) = 0;
	virtual INT32 StopListen() = 0;
};


class INetClient
{
public:
	INetClient(){};
	virtual ~INetClient(){};

	virtual void SetNetConfig(INetConfig* pCfg) = 0;
	virtual INetConfig* GetNetConfig() = 0;

	virtual INT32 StartNetService(char* pszLogPath = NULL) = 0;
	virtual INT32 StopNetService() = 0;

	virtual INetChannel* AddNetChannel(char* pszRemoteIPAddr, UINT16 uiRemotePort, 
						char* pszBindingIPAddr, UINT16 uiBindingPort) = 0;
};

#ifdef _WIN32
EXPORT_API INT32 InitNetCommuStack();
EXPORT_API INetServer* CreateNetServer();
EXPORT_API INetClient* CreateNetClient();
#else
INT32 InitNetCommuStack();
INetServer* CreateNetServer();
INetClient* CreateNetClient();
#endif

#endif