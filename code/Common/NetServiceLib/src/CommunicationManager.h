#if !defined (CommunicationManager_DEF_H)
#define CommunicationManager_DEF_H

/***************************************************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		CommunicationManager.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/24
	Description:    ͨѶ�����࣬������������ݡ�TCP���������ӻ��⹦�ܡ��ر�ͨ����
					�����ҵ���ƣ������ʹ�þ�̬��Ա������������

****************************************************************************************************/
#include "NetInterfaceCommData.h"
#include "BaseSocket.h"
#include <queue>

#if _LINUX

#include "LinuxSocket.h"

#endif

namespace NetServiceLib
{

// �궨��
#define WAITFOREVENT_SLEEP_SPACE(dwSleep,dwSpace) if(WaitForEvent(dwSleep,dwSpace)==-1){return -1;}
#define WAITFOREVENT_SLEEP(dwSleep) WAITFOREVENT_SLEEP_SPACE(dwSleep,10)
//#define WAITFOREVENT() WAITFOREVENT_SLEEP(10)

#define	ACTIVE_TEST_TIME		2*1000	//ͨ�������ʱ����

typedef struct	STRUSOCKETINFO
{
	SOCKETHANDLE		iSocket;
	CSocketChannel*		pListenChannel;	
	INT					nLen;
	sockaddr_in			ClientAddr;
	STRUSOCKETINFO():iSocket(NULL),pListenChannel(NULL)
	{
		nLen = sizeof(sockaddr_in);
		memset(&ClientAddr, 0x0, nLen);		
	}

}StruSocketInfo,*StruSocketInfoPtr;

typedef	queue<StruSocketInfoPtr>	SocketInfoList;

class CCommunicationManager
	: public CNetInterfaceCommData
{
public:
	CCommunicationManager(void);
	virtual ~CCommunicationManager(void);

	INT		Init();

	//�����������
	virtual	INT ThreadAcceptData(enumThreadEventType enumEvent, void* pObject)=0;

	//�߳�ִ�к��� TCP
	INT  Listen(enumThreadEventType enumEvent, void* pObject);

	INT	ThreadChannelStatus(enumThreadEventType enumEvent, void* pObject);

	// ֪ͨ�ϲ������ӵ���
	INT	NoticeUpNewConnect(enumThreadEventType enumEvent, void* pObject);

#if OPERATING_SYSTEM

	virtual INT  SelectEvent(enumThreadEventType enumEvent, void* pObject)=0;

	//�����˳������̱߳�־
	inline	void	SetExitSelectEvent(){ m_GSMutexExit.Lock(); m_bIsExitSelectEvent = false; m_GSMutexExit.Unlock(); };
#elif _LINUX

	//�߳�ִ�к��� ������ѯlinux��epoll�¼�
	virtual	INT  EpollWaitEvent(enumThreadEventType enumEvent, void* pObject)=0;

	//�����˳������̱߳�־
	inline	void	SetExitEpollEventWait(){ m_GSMutexExit.Lock(); m_bIsExitEpollEventWait = false;m_GSMutexExit.Unlock(); };

#endif

	//ͨ������
	INT	ChannelActiveTest(enumThreadEventType enumEvent, void* pObject);

	//�ж����������
	bool	IfMaxChannelCount(){return TestMaxChannelCount();};

	//�����˳������̱߳�־
	inline	void	SetExitListen(){ m_GSMutexExit.Lock(); m_bIsExitLinsten = false; m_GSMutexExit.Unlock(); };

	//�����˳������̱߳�־
	inline	void	SetExitAcceptData(){ m_GSMutexExit.Lock();m_bIsExitAcceptData = false; m_GSMutexExit.Unlock();};

	//�����˳������̱߳�־
	inline	void	SetExitActiveTest(){ m_GSMutexExit.Lock(); m_bIsExitActiveTest = false; m_GSMutexExit.Unlock(); };

	//�����˳�ͨ��״̬�����̱߳�־
	inline	void	SetExitChannelStatus(){ m_GSMutexExit.Lock(); m_bIsExitChannelStatus = false; m_GSCond.Signal(); m_GSMutexExit.Unlock(); };

	//�����˳�֪ͨ�ϲ������ӵ����־
	inline	void	SetExitAcceptUpNotice(){ m_GSMutexExit.Lock(); m_bIsExitAcceptUpNotice = false; m_GSCondAcceptUpNotice.Signal(); m_GSMutexExit.Unlock(); };

	//�ͷ����и���ͨ��
	INT		FreeAllExtraChannel(CSocketChannel* pclsSocketChannel);

	//�ͷ�ȫ��ͨ��
	INT		FreeAllChannel();

	//��������
	INT		ReConnectChannel( CSocketChannel* pclsSocketChannel );

protected:

	// UDP,���ݸ�ͨ��������ͨ����������������� ʧ�ܷ���NULL
	CSocketChannel*		CreateUdpChannel( CSocketChannel* pParentSocketChannel, LPPER_IO_OPERATION_DATA PerIoData );

	INT		WaitForEvent( DWORD dwSleep, DWORD dwSpace);
	
	// �������ͨ������
	void	CheckNormalChannelDeque();

	// ������ͨ������
	void	CheckFaultChannelDeque();

	// �¼���Ӧģ��
	virtual	void OnEventModel( CSocketChannel* pclsSocketChannel )=0;

	//ͨ������
	void	ChannelReconnect(CSocketChannel*	pclsSocketChannel);
	//�Ͽ�����
	void	DealDisnChannel( CSocketChannel*	pclsSocketChannel);

protected:
	//�˳������̱߳�־
	bool		m_bIsExitLinsten;
	//�˳����������̱߳�־
	bool		m_bIsExitAcceptData;
	//�˳������̱߳�־
	bool		m_bIsExitActiveTest;
	//�˳�ͨ��״̬�����̱߳�־
	bool		m_bIsExitChannelStatus;
	// �˳�֪ͨ�ϲ������ӵ���
	bool		m_bIsExitAcceptUpNotice;
	// �� ����רְ���˳���־���ж�
	CGSMutex	m_GSMutexExit;
	CGSCond		m_GSCond;

	SocketInfoList		m_SocketInfoList;
	//����m_SocketInfoList������
	CGSMutex	m_GSMutexSocketInfoList;
	CGSCond		m_GSCondAcceptUpNotice;
	// ֪ͨ�ϲ������ӵ����߳��Ƿ���������״̬ true:����
	bool		m_bIsSleep;

#if _LINUX
	//�߳�ִ�к��� ������ѯlinux��epoll�¼�
	bool		m_bIsExitEpollEventWait;
#endif
#if OPERATING_SYSTEM
	//�˳�selectevent�̺߳���
	bool		m_bIsExitSelectEvent;
#endif
private:
	
};

typedef INT (CCommunicationManager::*pThreadPoolExecute_Fn)(enumThreadEventType enumEvent, void* pObject);

}




#endif //end #if !defined (CommunicationManager_DEF_H)

