#if !defined (NetService_DEF_H)
#define NetService_DEF_H

/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		NetService.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/04/29
	Description:     ����������࣬���ڴ�������ж���������ʹ�ó�Ա�����еľ�̬���������鲻ʹ��      

*********************************************************************/
#include "INetService.h"
#include "ServerChannel.h"
#include "MemoryPool.h"
#if OPERATING_SYSTEM
#include "ThreadSelect.h"
#elif _WIN32
#include "ThreadIOCP.h"
#else //LINUX
#include "ThreadEpoll.h"
#endif

namespace NetServiceLib
{

class CNetService :
	public INetService
{
public:
	CNetService(void);
	virtual ~CNetService(void);
public:
	//�̳нӿ��еĺ���
	//�µ�����ӿ�
	//��ʼ���������׼����һЩ��Դ �����߳� 
	virtual INT InitNetService();
	//��ʼ�����������
	virtual INT InitSimpleNetService();
	virtual INT AddServerChannel(const char* pszBindIP, UINT16 unPort, 	enumNetProtocolType eProtocolType, ISocketChannel** pSocketChnl);
	virtual INT AddClientChannel( const char *pszHost, UINT16 unDesPort,const char *pszBindIP, 
								UINT16 unLocalport,enumNetProtocolType eProtocolType,  ISocketChannel** pSocketChnl);
	virtual INT StopNetService();
	virtual INT CloseChannel(ISocketChannel* pSocketChnl);
	//���µ���������Ըö������������ͨ������Ч
	//�����Ƿ�֧������ �ͻ�������  
	virtual INT SetReConnect(bool bReConnect);
	//�����Ƿ�֧����Ϣ����
	virtual INT SetMsgBuffFlag(bool bMsgBuffFlag);
	//�������ӻ���ʱ��������λ��
	virtual INT SetActiveTime(UINT16 unTime);
	//��������������
	virtual INT SetMaxChannel(UINT16 unCount);
	/********************************************************************************
	Function:		SetNetBlockMode
	Description:	��������ģʽ�������������
	Input:  		bMode	TRUE:������		FALSE:����
	Output:      	   
	Return:  		       
	Note:			�����Ĭ��Ϊ������ʽ		
	Author:        	CHC
	Date:				2010/09/08
	********************************************************************************/
	virtual INT	SetNetBlockMode( BOOL bMode);
	//���ûص�����
	virtual INT SetOnEventCallBack(void* pUserData, pOnEventCallBack OnEventCallBack);

	/********************************************************************************
	Function:		SetSendBuf
	Description:	����ϵͳsocket�ķ��ͻ�������С
	Input:  		iBufSize ������ֵ����λByte
	Output:      	   
	Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
	Note:			���óɹ��󣬶Ա�����������½�����socket��Ч������ISocketChannel��ͬ
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
	Note:			���óɹ��󣬶Ա�����������½�����socket��Ч��	����ISocketChannel��ͬ	
	Author:        	CHC
	Date:				2010/09/26
	********************************************************************************/
	virtual	INT		SetRcvBuf(INT iBufSize);

	/********************************************************************************
	Function:		GetAllChannelNum
	Description:	��ȡ��ǰ����������е�ͨ����Ŀ
	Input:  		
	Output:      	   
	Return:  		����ͨ����Ŀ       
	Note:			�������ò��û��ʲôʵ�����ã�ֻ�����ڲ���ʱ��Ҫ��֤demo�е�ͨ����Ŀ�Ƿ�������һֱ		
	********************************************************************************/
	virtual	INT	GetAllChannelNum();
	/********************************************************************************
	Function:       InitLog
	Description:    ��ʼ����־
	Input:			czPathName	��־·��
	Output:         
	Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			  	
	********************************************************************************/
	virtual INT		InitLog(const char *czPathName);

private:

	/********************************************************************************
	Function:       InitNetLib
	Description:	��ʼ�������
	Input:			iThreadCount	�߳���. Ĭ��ֵ0����Ϊ��ϵͳ���о���
	Output:         
	Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			  	
	********************************************************************************/
	INT	InitNetLib(UINT iThreadNum = 0);

#if _LINUX
	// �����źţ�������������linux��Ҫ����
	static void		ReceiveSignal( INT32 iSignalNum );

#endif
	
private:
	//�ö��󴴽���಻�ܳ���3����Ҳ�������ü�����
	static UINT16			s_unRefCount;		

	//�Ƿ��Ѿ���ʼ��
	bool					m_bIsInit;
	
	//��ɶ˿ڻ�epoll
	CThreadDealNetEvent		m_clsThreadDealNetEvent;

	// ��֤�̰߳�ȫ
	CGSMutex				m_GSMutex;

	// �ͻ��˴�����
	CClientChannel			m_clsClientChannel;

	// �������˴�����		
	CServerChannel			m_clsServerChannel;

	// �˳���־ TRUE �˳� FALSE 
	BOOL					m_bIsExit;

	// SocketChannel�ڴ�
	CMemoryPool<CSocketChannel>*	m_SocketChannelMemoryPtr;

	// ��ɶ˿��߳���
	INT32					m_iIocpThreadCount;



};
}
#endif


