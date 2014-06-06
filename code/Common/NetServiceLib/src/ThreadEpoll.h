#if !defined (ThreadEpoll_DEF_H)
#define ThreadEpoll_DEF_H

#if OPERATING_SYSTEM

#elif _LINUX

#include "LinuxEpoll.h"
#include "CommunicationManager.h"
#include "ThreadPoolModule.h"

namespace NetServiceLib
{

class CThreadDealNetEvent :
	public CLinuxEpoll, public CCommunicationManager
{
public:
	CThreadDealNetEvent(void){};
	virtual ~CThreadDealNetEvent(void){};
	//�����������
	INT ThreadAcceptData(enumThreadEventType enumEvent, void* pObject);		
	//�߳�ִ�к��� ������ѯlinux��epoll�¼�
	INT  EpollWaitEvent(enumThreadEventType enumEvent, void* pObject);
private:
	// �����¼���Ӧ �̳���CCommunicationManager
	virtual	void OnEventModel( CSocketChannel* pclsSocketChannel );
public:
	CThreadPoolModule			m_clsThreadPool;

};

}
#endif

#endif


