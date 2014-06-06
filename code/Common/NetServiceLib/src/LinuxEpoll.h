#if !defined (LinuxEpoll_DEF_H)
#define LinuxEpoll_DEF_H

#if _LINUX


#include "NetServiceDataType.h"

namespace NetServiceLib
{



#define MAX_EPOLL_SIZE  100		//poll�����Χ

typedef struct _overlaped		//�����ʵû��ʵ���� ֻ��ʹ�ÿ�������IOCP�����ƶ���
{
	int base;
}OVERLAPPED;

typedef struct _WSABUF
{
	void *buf;
	int len;
}WSABUF;

typedef OVERLAPPED *LPOVERLAPPED;

typedef struct 
{
	OVERLAPPED	Overlapped;
	WSABUF		DataBuf; 
	char		Buffer[DATA_BUFSIZE];
	INT			OptionType;					//��������
	sockaddr_in	struSockAddrFrom;	//UDP��������ʱ�ĵ�ַ
	INT			iAddrFromLen;		//UDP��������ʱ�ĵ�ַ�ĳ���

} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

class CLinuxEpoll
{
public:
	CLinuxEpoll(void);
	virtual ~CLinuxEpoll(void);
	//����epoll�ļ�������
	INT32	CreateEpoll();
	//��epoll�ļ�������ע���¼�����˼�ǣ���epoll�ļ�����������sokcet��ɾ��socket���޸�sokcet
	INT32	EpollCtrl(INT32 Op, INT32 fd, struct epoll_event *event);
	//�ȴ�epoll�ļ���������ĳһ��sokcet���¼����� ��EPOLLOUT��EPOLLIN
	INT32	EpollWait(struct epoll_event * events,int maxevents,int timeout);
	//��ȡcpu�ں���Ŀ
	DWORD	GetNumberOfProcessors();
private:
	INT32		m_nPollHandle;			//epoll���
};

#endif //_linux

}

#endif

