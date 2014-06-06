#include "MemCommProcess.h"

#ifdef _LINUX
#include <errno.h>
extern int errno;
#endif

CMemCommProcess::CMemCommProcess(void)
{

	m_pCGSProcessMutex = NULL;
#ifdef _WIN32
	m_pMemHandle = NULL;
#endif

#ifdef _LINUX
	m_iShareMemID = 0;
#endif

}

CMemCommProcess::~CMemCommProcess(void)
{
	
}


BOOL CMemCommProcess::Free(void)
{

	Lock();

#ifdef _WIN32
	if (m_pHead != NULL)//�ڴ���
	{
		UnmapViewOfFile(m_pHead);
		m_pHead = NULL;
		m_pStruCookie = NULL;
	}
	if (m_pMemHandle != NULL)
	{
		CloseHandle(m_pMemHandle);
		m_pMemHandle = NULL;
	}

	m_pMemHandle = NULL;
#endif

#ifdef _LINUX

	if (m_pHead != NULL)
	{
		/*if(shmdt(m_pHead)==-1)//������ɾ�������빲���ڴ�ĵ���ϵ
		{
			Unlock();
			return FALSE;
		}*/
		m_pHead = NULL;
	}

    if(shmctl(m_iShareMemID,IPC_RMID,0)==-1)//��ϵͳ��ɾ�������ڴ�
	{
		Unlock();
		return FALSE;
	}
	m_iShareMemID = 0;

#endif

	if (m_pCGSProcessMutex != NULL)
	{
		Unlock();
		delete m_pCGSProcessMutex;
		m_pCGSProcessMutex = NULL;
		return TRUE;
	}
	Unlock();
	return TRUE;
}

BOOL CMemCommProcess::Lock()
{

	if (m_pStruCookie != NULL)
	{
		if (m_pStruCookie->uiLock == 1)
		{
			if (m_pCGSProcessMutex != NULL)
			{
				return m_pCGSProcessMutex->LockProcess();
			}
		}
	}
	return TRUE;

}
void CMemCommProcess::Unlock()
{
	if (m_pStruCookie != NULL)
	{
		if (m_pStruCookie->uiLock == 1)
		{
			if (m_pCGSProcessMutex != NULL)
			{
				m_pCGSProcessMutex->UnlockProcess();
			}
		}
	}

}
BOOL CMemCommProcess::Open(UINT32 dwSize, BOOL bLock, string &strName)
{

	dwSize = (dwSize + MEMCOMM_ALIGN_BYTE_NUM -1) / MEMCOMM_ALIGN_BYTE_NUM * MEMCOMM_ALIGN_BYTE_NUM + MEMCOMM_ALIGN_BYTE_NUM;
	dwSize = sizeof(StruMemCookie) + dwSize;

#ifdef _WIN32
	m_pMemHandle = CreateFileMapping(HANDLE(-1), NULL, PAGE_READWRITE, 0, dwSize, (LPCWSTR)strName.c_str());
	if (m_pMemHandle == NULL)
		return FALSE;

	if (GetLastError() != ERROR_ALREADY_EXISTS)//������
	{
		CloseHandle(m_pMemHandle);
		m_pMemHandle = NULL;
		return FALSE;
	}

	//ӳ�乲���ڴ�
	m_pHead = MapViewOfFile(m_pMemHandle, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, dwSize);//������
	CloseHandle(m_pMemHandle);
	if (m_pHead == NULL)
	{
		return FALSE;
	}

#endif


#ifdef _LINUX
	
	//��ȡ�Ѿ����ڹ����ڴ�ID
	m_iShareMemID = shmget((key_t)(*(int *)strName.c_str()),0,0);
	if(m_iShareMemID == -1)//���������´�����
	{
		m_pHead = NULL;
		Free();
		return FALSE;
	}

	//��ȡ�����ڴ��ַ
	m_pHead = shmat(m_iShareMemID, (void *)0, 0);
	if (m_pHead == (void *) -1)
	{
		m_pHead = NULL;
		Free();
		return FALSE;
	}

#endif

	m_pStruCookie = (StruMemCookie *)m_pHead;

	//������
	if (m_pStruCookie->uiLock == 1)
	{

		if (m_pCGSProcessMutex == NULL)
		{
			m_pCGSProcessMutex = new CGSProcessMutex(m_pStruCookie->chMutexName);
			if (m_pCGSProcessMutex == NULL)
			{

				Free();
				return FALSE;
			}

		}
	}

	return TRUE;
}
BOOL CMemCommProcess::ReCreate(UINT32 dwSize, BOOL bLock, string &strName)
{
	//���������ڴ�
	UINT32 uiAlignSize;
	uiAlignSize = (dwSize + MEMCOMM_ALIGN_BYTE_NUM -1) / MEMCOMM_ALIGN_BYTE_NUM * MEMCOMM_ALIGN_BYTE_NUM + MEMCOMM_ALIGN_BYTE_NUM * 2;//����1��λ���ڴ�ų��ȣ�1��λ�������ֻ������Ƿ�Ϊ��
	dwSize = sizeof(StruMemCookie) + dwSize;

#ifdef _WIN32
	m_pMemHandle = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, dwSize, (LPCWSTR)strName.c_str());
	if (m_pMemHandle == NULL)
	{
		return FALSE;
	}

	//��ȡ�����ڴ��ַ
	m_pHead = MapViewOfFile(m_pMemHandle, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, dwSize);

	if (m_pHead == NULL)
	{
		CloseHandle(m_pMemHandle);
		m_pMemHandle = NULL;
		return FALSE;
	}
#endif

#ifdef _LINUX
	//���������ڴ�
	
	m_iShareMemID = shmget((key_t)(*(int *)strName.c_str()), dwSize, 0666|IPC_CREAT|IPC_EXCL);//ֻ�е������ڴ治����ʱ�����´����ɹ����Ż᷵�سɹ�������֤�����´�����
	if (m_iShareMemID == -1)
	{
		if (errno != EEXIST)//����Ѿ����ڣ�ɾ���������´���
		{
			return FALSE;
		}
		shmctl(m_iShareMemID,IPC_RMID,0);
		m_iShareMemID = shmget((key_t)(*(int *)strName.c_str()), dwSize, 0666|IPC_CREAT|IPC_EXCL);
		if (m_iShareMemID == -1)
		{
			return FALSE;
		}	
	}

	//��ȡ�����ڴ��ַ
	m_pHead = shmat(m_iShareMemID,(void *)0,0);

	if (m_pHead == (void *) -1)
	{
		m_pHead = NULL;
		Free();
		return FALSE;
	}

#endif
    printf("Recreate Mem!\n");
	//  ������
	if (bLock)
	{
		if (m_pCGSProcessMutex == NULL)
		{
			strName += "ProcessMutex";
			m_pCGSProcessMutex = new CGSProcessMutex(strName.c_str());//TODO�������¼�������һ��������
			if (m_pCGSProcessMutex == NULL)
			{
				Free();
				return FALSE;
			}

		}

	}

	//д���ڴ�cookie

	m_pStruCookie = (StruMemCookie *)m_pHead;

	m_pStruCookie->uiMemId = MEMCOMM_SHARE_ID;
	m_pStruCookie->uiOffSet = sizeof(StruMemCookie);
	m_pStruCookie->uiTotalSize = (dwSize - sizeof(StruMemCookie))/MEMCOMM_ALIGN_BYTE_NUM;
	m_pStruCookie->uiWritePos = 0;
	m_pStruCookie->uiReadPos = 0;
	if (bLock)
	{
		m_pStruCookie->uiLock = 1;
	}
	else
	{
		m_pStruCookie->uiLock = 0;
	}

	strcpy(m_pStruCookie->chMutexName,strName.c_str()); 
	strcpy(m_pStruCookie->chEventName,strName.c_str());
	return TRUE;
}

