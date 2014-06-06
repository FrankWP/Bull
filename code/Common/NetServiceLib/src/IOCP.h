#if !defined (IOCP_DEF_H)
#define IOCP_DEF_H

#include "NetServiceDataType.h"

namespace NetServiceLib
{
#ifndef _LINUX
typedef struct 
{
   OVERLAPPED Overlapped;
   WSABUF DataBuf; 
   char	 Buffer[DATA_BUFSIZE];
   INT	OptionType;					//��������
   sockaddr_in	struSockAddrFrom;	//UDP��������ʱ�ĵ�ַ
   INT			iAddrFromLen;		//UDP��������ʱ�ĵ�ַ�ĳ���

} PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

#endif

#if OPERATING_SYSTEM
#elif _WIN32
//
//	�����������ṹ��
//  �ýṹ���ں�IOCP��������
//
typedef struct
{
	void*	pUserData;		//������������ָ��ͨ��ָ��
   
} PER_HANDLE_DATA,  *LPPER_HANDLE_DATA;

class CIOCP  
{
public:
	CIOCP();
	virtual ~CIOCP();
public:
	BOOL PostQueuedCompletionStatusEx();
	//������ɶ˿�
	HANDLE	CreateIoCompletionPortEx();
	//������ɶ˿�
	HANDLE	AddToIoCompletionPort(HANDLE hHandle, DWORD CompletionKey);
	//��ѯ��ɶ˿��Ƿ������ݵ���
	BOOL	GetQueuedCompletionStatusEx(LPDWORD		BytesTransferred,
										LPDWORD		PerHandleData,										
										LPOVERLAPPED* lpOverlapped,
										LPPER_IO_OPERATION_DATA* lpIOData
										);
	HANDLE	GetCompletionPort(){ return m_CompletionPort;}
	DWORD	GetNumberOfProcessors();

protected:
	HANDLE					m_CompletionPort;						// ��ȫ�˿ھ��
};

#else//linux
//linux ������ļ�

#endif	//end #if _WIN32

}

#endif


