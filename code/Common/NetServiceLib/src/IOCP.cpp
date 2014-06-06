
#include "IOCP.h"

#if OPERATING_SYSTEM

#elif _WIN32

using namespace NetServiceLib;

CIOCP::CIOCP()
{
	m_CompletionPort = NULL;
}

CIOCP::~CIOCP()
{
	if (m_CompletionPort != NULL)
	{
		CloseHandle(m_CompletionPort);
	}

}

HANDLE CIOCP::CreateIoCompletionPortEx()
{
	if (m_CompletionPort == NULL)
	{
		//m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);//GetNumberOfProcessors());//Ӧ������Ϊcpu*2+2��Ϊ��
		m_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, GetNumberOfProcessors() * 2 + 2);//Ӧ������Ϊcpu*2+2��Ϊ��
		return m_CompletionPort;
	}
	else
	{
		return NULL;	//�Ѿ�����
	}
	
}

//������ɶ˿�
HANDLE CIOCP::AddToIoCompletionPort(HANDLE hHandle, DWORD CompletionKey)
{
	if (m_CompletionPort == NULL)
	{
		return NULL;
	}

	HANDLE hDle = NULL;
	hDle = CreateIoCompletionPort(hHandle, m_CompletionPort, CompletionKey, 0);

	int n = GetLastError();

	if ( hDle == NULL)
	{
		return NULL;
	}
	
	return hDle;
}

/*
���ܣ���ѯ��ɶ˿��Ƿ������ݵ�����򷵻��յ�������
������lpIOData
*/
BOOL CIOCP::GetQueuedCompletionStatusEx(LPDWORD		BytesTransferred,
										LPDWORD		PerHandleData,										
										LPOVERLAPPED* lpOverlapped,
										LPPER_IO_OPERATION_DATA* lpIOData)
{
	if (m_CompletionPort == NULL)
	{
		return FALSE;
	}


	BOOL	bRet = TRUE;
	bRet = GetQueuedCompletionStatus(m_CompletionPort,BytesTransferred, PerHandleData, lpOverlapped,INFINITE);

	
	// ���ɹ��ķ��أ����Ҫע��ʹ�������CONTAINING_RECORD
	//LPPER_IO_OPERATION_DATA IOData;
	//IOData = (LPPER_IO_OPERATION_DATA)CONTAINING_RECORD(*lpOverlapped, PER_IO_OPERATION_DATA, Overlapped);
    *lpIOData = (LPPER_IO_OPERATION_DATA)CONTAINING_RECORD(*lpOverlapped, PER_IO_OPERATION_DATA, Overlapped);

	return bRet;

	 /*if (BytesTransferred == 0)
	 {
		 return FALSE;
	 }
	 
	 return TRUE;*/
}

BOOL CIOCP::PostQueuedCompletionStatusEx()
{
	for (int i=0; i < GetNumberOfProcessors()*2 + 2; i++)
	{
		PostQueuedCompletionStatus(m_CompletionPort, 0, (DWORD) NULL, NULL);
	}
	
	return TRUE;
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
DWORD CIOCP::GetNumberOfProcessors()
{
#define Max_Accept_ThreadNum 10
	//�̶���10���߳�����������
	return Max_Accept_ThreadNum;
	//SYSTEM_INFO si;
	//// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	//PGNSI pfnGNSI = (PGNSI) GetProcAddress(GetModuleHandle((LPCTSTR)"kernel32.dll"), "GetNativeSystemInfo");
	//if(pfnGNSI)
	//{
	//	pfnGNSI(&si);
	//}
	//else
	//{
	//	GetSystemInfo(&si);
	//}
	//return si.dwNumberOfProcessors;
}

#else//linux
//linux ������ļ�

#endif	//end #if _WIN32

