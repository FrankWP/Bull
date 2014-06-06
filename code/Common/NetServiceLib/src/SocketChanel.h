#if !defined (SocketChanel_DEF_H)
#define SocketChanel_DEF_H

/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		SocketChanel.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/04/28
	Description:    ÿ��socket����Ϣ��ÿ��socket����һ��ͨ��

*********************************************************************/


#include "NetServiceDataType.h"

#include "INetService.h"
#include "BaseSocket.h"
#include "IOCP.h"

namespace NetServiceLib
{

class CSocketChannel : public ISocketChannel
{
public:
	CSocketChannel(void);	
	virtual ~CSocketChannel(void);
public:
	INT		GetLocalIPPort(char* pszIP, UINT16& unPort);
	INT		GetReMoteIPPort(char* pszIP, UINT16& unPort);
	INT		SendData(void* pData, UINT unDataLen);
	INT		CloseChannel();

	ISocketChannel*		GetListenChannel();
	enumNetProtocolType	GetNetProType();
	//�����û�����
	inline	void	SetUserData( void* pUserData ){ m_pUserData = pUserData; };
	//�����û�����ָ��
	inline	void*	GetUserData(){ return m_pUserData; };
	//�˹�������������TCP�ͻ��˵���	
	virtual INT		ReConnect();
	/********************************************************************************
	Function:			CloseChannelEx
	Description:		�ر�ͨ��
	Input:  
	Output:         
	Return:			ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			��CloseChannel��ͬ�ĵط��Ǵ�CloseChannelEx���Ը�ͨ���ӻص���	
	********************************************************************************/
	virtual INT		CloseChannelEx();
	/********************************************************************************
	Function:		SetSendBuf
	Description:	����ϵͳsocket�ķ��ͻ�������С
	Input:  		iBufSize ������ֵ����λByte
	Output:      	   
	Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
	Note:			������ý���Ը�ͨ����socket����Ӱ������ͨ����socket������		
	Author:        	CHC
	Date:				2010/09/26
	********************************************************************************/
	virtual	INT		SetSendBuf(INT iBufSize);
	/********************************************************************************
	Function:		SetRcvBuf
	Description:	����ϵͳsocket�Ľ��ջ�������С
	Input:  		iBufSize ������ֵ����λByte
	Output:      	   
	Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
	Note:			������ý���Ը�ͨ����socket����Ӱ������ͨ����socket������		
	Author:        	CHC
	Date:				2010/09/26
	********************************************************************************/
	virtual	INT		SetRcvBuf(INT iBufSize);
	/********************************************************************************
	Function:		SetNetBlockMode
	Description:	��������ģʽ�������������
	Input:  		bMode	TRUE:������		FALSE:����
	Output:      	   
	Return:  		       
	Note:			�����Ĭ��Ϊ������ʽ. 	����Ե���ͨ�������������������ʽ	
	Author:        	CHC
	Date:				2010/09/08
	********************************************************************************/
	virtual INT	SetNetBlockMode( BOOL bMode);
	/********************************************************************************
	Function:       SendDataEx
	Description:    �������ݡ�����⽫ֱ�ӵ���API��������
	Input:		  pData������ָ�룬	unDataLen�����ݳ���
	Output:         
	Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			  ע�⣬����ʧ��ʱ���˺������᷵�س�����API�����룬����������ж���Ĵ�����
	�˺�����SendData()�������ǲ�����select����������socket�Ƿ���д����
	********************************************************************************/
	virtual INT		SendDataEx(void* pData, UINT unDataLen);
/********************************************************************************
	Function:       GetSocketHandle
	Description:    ��ȡsocket��handle
	Input:		  
	Output:         
	Return:         ����handle  NULLΪ��Чֵ
	Note:			  	
********************************************************************************/
	virtual INT		GetSocketHandle(void);
public:
	INT		RecvData();
	INT		SetLocalIPPort(const char* pszIP, UINT16 unPort);
	INT		SetReMoteIPPort(const char* pszIP, UINT16 unPort);
	INT		SetCbaseSocketPoint(CBaseSocket* pBaseSocket);
	INT		SetListenSocketChannel( CSocketChannel*	pSocketChannel );
	//�ر�socket
	INT		CloseHandle();//������ʽ����
	inline	void	SetNetProtocolType(enumNetProtocolType  enumNetProType){ m_enumNetProType = enumNetProType;};
	inline	void	SetServerType( enumServerType enumSvrType ){ m_enumServerType = enumSvrType; };
	inline	enumServerType	GetServerType(){ return m_enumServerType; };
	inline enumNetProtocolType	GetNetProtocolType(){return m_enumNetProType;};
	inline  CBaseSocket* GetCbaseSocketPoint(){return m_pBaseSocket;};
	inline	void	SetChannelType( enumChannelType enumCnlType ){ m_enumChannelType = enumCnlType; };
	inline	enumChannelType GetChannelType(){ return m_enumChannelType; };
	inline	INT64	GetLastActiveTime(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); return m_uiLastActiveTime;};
	void	SetLastActiveTime();
	inline	void	SetDWORDRemoteIP( DWORD dwRemoteIP){ m_dwRemoteIP = dwRemoteIP; };
	void		GetDWORDRemoteIPPort(DWORD& dwRemoteIP, UINT16& unPort){ dwRemoteIP = m_dwRemoteIP; unPort = m_unRemotePort; };
	void	SetChannelStatus(enumChannelStatus enumStatus);
	inline	enumChannelStatus	GetChannelStatus(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); return m_enumChannelStatus; };

	// ��������
	inline	void	AddRefCount(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex);  m_uiRefCount++; };
	// ��������
	inline	void	SubRefCount(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex);  m_uiRefCount--; };
	
	// ������ɶ˿��Ƿ�ʹ�ø�ͨ��
	inline	void	SetIOCPNoUse( BOOL bIOCPNoUse){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); m_bIOCPNoUse = bIOCPNoUse; };
	// ��ȡm_bICOPNoUse
	inline	BOOL	GetIOCPNoUse(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); return m_bIOCPNoUse; };

	inline	UINT64	GetCloseTick(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); return m_uiCloseTick; };


	// ��ü���
	UINT16	GetRefCount(){ CGSAutoMutex	GSAutoMutex(&m_GSMutex); return m_uiRefCount; };

	// ������־ָ��
	void		SetLogInstancePtr( ILogLibrary* clsLogPtr){ m_clsLogPtr = clsLogPtr; };

	
public:
#if OPERATING_SYSTEM
	inline	LPPER_IO_OPERATION_DATA		GetIORecvData(){ return m_pIOData_Recv; };

#elif _WIN32
	inline	LPPER_HANDLE_DATA GetIOCPHandleData(){ return m_pIOCP_HdlData; };
	inline	void			InitIOCPHandleData(){ m_pIOCP_HdlData->pUserData = this; };
#else //_LINUX
	inline  epoll_event*	GetEpollEvent(){ return m_pstruEpollEvent; };
	inline	LPPER_IO_OPERATION_DATA		GetIORecvData(){ return m_pIOData_Recv; };
	void					InitEpollEvent();
	inline	void			AddEpollEvent( INT32	iEvent ){ m_pstruEpollEvent->events |= iEvent; };
	inline	void			DelEpollEvent( INT32	iEvent ){ m_pstruEpollEvent->events &= iEvent; };
#endif

	// ��Ҫ���ڻص�ʱ��סͨ��,ע���m_GSMutex��ʹ��˳�򣬷����������
	CGSMutex				m_csCallBackGSMutex;

	// ���Ա�־  ������ʾͨ�����ں��ϲ㽻��ʲô 0:ʲô��û 1:��ʽ�ص����� 2:����֪ͨ�ж� ��ʱ������
	UINT16					m_uiChnIng;

protected:
	CBaseSocket*			m_pBaseSocket;	//�����ͷŴ˶���ʱ�൱��Ť����Ϊ����ͨ���ǲ���delete�ġ��Ҳ�ȡ��ʽ���õķ���
	//���������ͨ���ļ���ͨ����ֻ��TCP�������˲��д����
	CSocketChannel*			m_pListenSocketChannel;
	//ͨ�����ͣ�����ͨ��������ͨ��
	enumChannelType			m_enumChannelType;
	//CGSWRMutex			m_GSWRMutex;	//�������ݻ����޸Ĵ��������ʱ�ô���
	CGSMutex				m_GSMutex;//�ĳ���ͨ������
	//ͨ�����һ�ζ�д���ݵ�ʱ�䣬Ҫ��Ҫ��д�ɹ��ż�¼ʱ��
	UINT64					m_uiLastActiveTime;
	//Э������
	enumNetProtocolType		m_enumNetProType;
	//��������
	enumServerType			m_enumServerType;//�ͻ��˻��������
	//����IP
	char					m_szLocalIP[16];
	//���ض˿�
	UINT16					m_unLocalPort;
	//Զ��IP��������
	char					m_szRemoteHost[256];
	//Զ�̶˿�
	UINT16					m_unRemotePort;
	//Զ��IP��ַ��������ʽ��Ŀ�������ڿ��ٱȽ�
	DWORD					m_dwRemoteIP;
	//�û�����ָ�룬���ϲ����ָ��������ģ�鲻���κδ���
	void*					m_pUserData;
	//ͨ��״̬	ͨ���ͷŵ�����֮һ
	enumChannelStatus		m_enumChannelStatus;

	// ʹ�ü��� ���ü���Ϊ0ʱ�������ͷŸ�ͨ�� ͨ���ͷŵ�����֮��
	UINT16					m_uiRefCount;

	// ��ɶ˿ڲ���ʹ�ø�ͨ��	TRUE IOCPʹ�� FALSE ��ʹ��
	BOOL					m_bIOCPNoUse;

	// �رռ�ʱ ����ϲ�����CloseChannel ��3���ӱ���ǿ���ͷ���Դ
	UINT64					m_uiCloseTick;

	// socket���ͻ�������С ��������� �����ϵͳ��Ĭ��ֵ
	UINT16					m_unSocketSendBuf;

	// socket���ջ�������С ��������� �����ϵͳ��Ĭ��ֵ
	UINT16					m_unSocketRcvBuf;

	// ��־ָ��
	ILogLibrary*			m_clsLogPtr;
	

	LPPER_IO_OPERATION_DATA		m_pIOData_Recv;		//�յ�������
	
#if OPERATING_SYSTEM

#elif _WIN32
	LPPER_HANDLE_DATA			m_pIOCP_HdlData;			//��ɶ˿ھ������	
#else //_LINUX
	epoll_event*	m_pstruEpollEvent;
#endif
};

/*
�ͷ�ͨ���ڴ������
�������
1��m_enumChannelStatus == CHANNEL_CLOSE
2��m_uiRefCount == 0
3��m_bICOPNoUse == FALSE (��WIN32)
�쳣���
1��m_enumChannelStatus == CHANNEL_CLOSE
2����m_uiCloseTick�Ƚ� ��ʱ3��
*/

}

#endif

