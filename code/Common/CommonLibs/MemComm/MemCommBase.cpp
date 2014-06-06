#include "MemCommBase.h"

CMemCommBase::CMemCommBase(void)
{
	m_pStruCookie = NULL;
	m_pHead = NULL;
}

CMemCommBase::~CMemCommBase(void)
{

	if (m_pStruCookie != NULL)
	{
		m_pStruCookie = NULL;
	}
}
void CMemCommBase::Reset(void)
{
	Lock();
	if (m_pStruCookie != NULL)
	{
		m_pStruCookie->uiWritePos = 0;
		m_pStruCookie->uiReadPos = 0;

	}
	Unlock();
}
BOOL CMemCommBase::Create(UINT32 dwSize, BOOL bLock, EnumOperType opType, string &strName)
{
	switch(opType)
	{
	case MEM_COMM_OPEN://��
		{
			return Open(dwSize,bLock,strName);
		}
	case MEM_COMM_CREATE://����
		{
			return ReCreate(dwSize,bLock,strName);
		}
	case MEM_COMM_OPEN_CREATE://���Դ򿪺󴴽�
		{
			if (Open(dwSize,bLock,strName) == TRUE)	
			{
				return TRUE;
			}
			return ReCreate(dwSize,bLock,strName);
		}
	default:
		return FALSE;
	}
	return FALSE;

}

INT32 CMemCommBase::Read(void *pBuf, UINT32 dwMaxLen, UINT32 *pRealLen, UINT32 dwMilliSeconds)
{
	if (m_pStruCookie == NULL || pBuf == NULL)//�޺Ϸ�������
	{
		return ERROR_MEMCOMM_BUFFER_NOT_EXIST;
	}

	if (m_pStruCookie->uiMemId != MEMCOMM_SHARE_ID)//��������ʶ����
	{
		return ERROR_MEMCOMM_BUFFERID_NOT_FIT;
	}

	Lock();
	if (m_pStruCookie->uiReadPos == m_pStruCookie->uiWritePos)//�����������ݣ�TODO:�ȴ���ʱ
	{
		Unlock();
		return ERROR_MEMCOMM_BUFFER_IS_EMPTY;
	}

	char *pMemPos = (char *)(m_pHead) + m_pStruCookie->uiOffSet;
	UINT32 uiReadSize = 0;

	UINT32 dwLen = *(UINT32 *)(pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM); //��ȡ�����ֽ�
	
	if (dwMaxLen < dwLen)//pBuf��С����
	{
		*pRealLen = 0;
		Unlock();
		return ERROR_MEMCOMM_BUFFER_NOT_ENOUGH;
	}

	m_pStruCookie->uiReadPos++;

	UINT dwRightSize = m_pStruCookie->uiTotalSize - m_pStruCookie->uiReadPos;//���㻺������ָ���Ҳ�ռ��С

	if (dwRightSize * MEMCOMM_ALIGN_BYTE_NUM < dwLen)
	{
		uiReadSize = dwRightSize * MEMCOMM_ALIGN_BYTE_NUM;
		memcpy(pBuf,pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM,uiReadSize);//��������ǰ���
		m_pStruCookie->uiReadPos = 0;
		uiReadSize = dwLen - uiReadSize;
		memcpy((char *)pBuf + dwRightSize * MEMCOMM_ALIGN_BYTE_NUM,pMemPos,uiReadSize);//�������ݺ���
		m_pStruCookie->uiReadPos =(uiReadSize + MEMCOMM_ALIGN_BYTE_NUM - 1) / MEMCOMM_ALIGN_BYTE_NUM;
	}
	else
	{
		uiReadSize = dwLen;
		memcpy(pBuf, pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM,uiReadSize);
		m_pStruCookie->uiReadPos = m_pStruCookie->uiReadPos + (uiReadSize + MEMCOMM_ALIGN_BYTE_NUM - 1) / MEMCOMM_ALIGN_BYTE_NUM;
		if (m_pStruCookie->uiReadPos == m_pStruCookie->uiTotalSize)
		{
			m_pStruCookie->uiReadPos = 0;
		}
	}

	*pRealLen = dwLen;
	Unlock();

	return ERROR_MEMCOMM_OPER_SUCCESS;
}
INT32 CMemCommBase::Write(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds)
{
	if (m_pStruCookie == NULL || pBuf == NULL)//�޺Ϸ�������
	{
		return ERROR_MEMCOMM_BUFFER_NOT_EXIST;
	}

	if (m_pStruCookie->uiMemId != MEMCOMM_SHARE_ID)//��������ʶ����
	{
		return ERROR_MEMCOMM_BUFFERID_NOT_FIT;
	}

	UINT32 dwLenAlign = (dwLen + MEMCOMM_ALIGN_BYTE_NUM -1) / MEMCOMM_ALIGN_BYTE_NUM + 1;//����һ�ֽ����ڱ������ݳ���

	Lock();

	UINT32 uiFreeSize;//���л�������С
	if (m_pStruCookie->uiReadPos == m_pStruCookie->uiWritePos)//������Ϊ��
	{
		uiFreeSize = m_pStruCookie->uiTotalSize;
	}
	else
		if (m_pStruCookie->uiReadPos > m_pStruCookie->uiWritePos)
		{
			uiFreeSize =m_pStruCookie->uiReadPos - m_pStruCookie->uiWritePos;
		}
		else
		{
			uiFreeSize = m_pStruCookie->uiTotalSize - m_pStruCookie->uiWritePos + m_pStruCookie->uiReadPos;
		}

		if (uiFreeSize < dwLenAlign + 1)//�������ռ䲻��������һ���ռ䣨4�ֽڣ��������𻺳����Ƿ�Ϊ�� TODO:�ȴ���ʱ
		{
			Unlock();
			return ERROR_MEMCOMM_BUFFER_NOT_ENOUGH;
		}

		char *pMemPos = (char *)m_pHead + m_pStruCookie->uiOffSet;//��������ʼλ��
		UINT32 uiWriteSize = 0;//ÿ��д�볤��

		UINT32 dwRightSize = m_pStruCookie->uiTotalSize- m_pStruCookie->uiWritePos;//���㻺����дָ���ռ��С

		if (dwRightSize < dwLenAlign)
		{

			*(UINT32 *)(pMemPos + m_pStruCookie->uiWritePos * MEMCOMM_ALIGN_BYTE_NUM) = dwLen;//����д�볤��

			m_pStruCookie->uiWritePos++;
			dwRightSize --;

			uiWriteSize = dwRightSize * MEMCOMM_ALIGN_BYTE_NUM;
			memcpy( pMemPos + m_pStruCookie->uiWritePos * MEMCOMM_ALIGN_BYTE_NUM,pBuf,uiWriteSize);//��������ǰ���

			m_pStruCookie->uiWritePos = 0;
			uiWriteSize = dwLen - uiWriteSize;
			memcpy(pMemPos,(char *)pBuf + dwRightSize * MEMCOMM_ALIGN_BYTE_NUM,uiWriteSize);//�������ݺ���
			m_pStruCookie->uiWritePos = dwLenAlign - 1 - dwRightSize;

		}
		else//���ο���
		{
			*(UINT32 *)(pMemPos + m_pStruCookie->uiWritePos * MEMCOMM_ALIGN_BYTE_NUM) = dwLen;//����д�볤��

			m_pStruCookie->uiWritePos ++;

			uiWriteSize = dwLen;
			memcpy(pMemPos + m_pStruCookie->uiWritePos * MEMCOMM_ALIGN_BYTE_NUM,pBuf,uiWriteSize);//��������

			m_pStruCookie->uiWritePos = m_pStruCookie->uiWritePos + dwLenAlign -1;

			if (m_pStruCookie->uiWritePos == m_pStruCookie->uiTotalSize)
			{
				m_pStruCookie->uiWritePos = 0;// дλ����0
			}
		}

		Unlock();

		return ERROR_MEMCOMM_OPER_SUCCESS;
}
INT32 CMemCommBase::WriteUrgent(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds)
{
	if (m_pStruCookie == NULL || pBuf == NULL)//�޺Ϸ�������
	{
		return ERROR_MEMCOMM_BUFFER_NOT_EXIST;
	}

	if (m_pStruCookie->uiMemId != MEMCOMM_SHARE_ID)//��������ʶ����
	{
		return ERROR_MEMCOMM_BUFFERID_NOT_FIT;
	}

	Lock();
	if (m_pStruCookie->uiLock != 1)//û��������ֱ�ӷ��ش���
	{
		Unlock();
		return ERROR_MEMCOMM_BUFFER_NOT_LOCK;
	}

	UINT32 dwLenAlign = (dwLen + MEMCOMM_ALIGN_BYTE_NUM -1) / MEMCOMM_ALIGN_BYTE_NUM + 1;//����һ�ֽ����ڱ������ݳ���

	UINT32 uiFreeSize;//���л�������С
	if (m_pStruCookie->uiReadPos == m_pStruCookie->uiWritePos)//������Ϊ��
	{
		uiFreeSize = m_pStruCookie->uiTotalSize;
	}
	else
		if (m_pStruCookie->uiReadPos > m_pStruCookie->uiWritePos)
		{
			uiFreeSize =m_pStruCookie->uiReadPos - m_pStruCookie->uiWritePos;
		}
		else
		{
			uiFreeSize = m_pStruCookie->uiTotalSize - m_pStruCookie->uiWritePos + m_pStruCookie->uiReadPos;
		}

		if (uiFreeSize < dwLenAlign + 1)//�������ռ䲻��������һ���ռ䣨4�ֽڣ��������𻺳����Ƿ�Ϊ�� TODO:�ȴ���ʱ
		{
			Unlock();
			return ERROR_MEMCOMM_BUFFER_NOT_ENOUGH;
		}

		char *pMemPos = (char *)m_pHead + m_pStruCookie->uiOffSet;//��������ʼλ��
		UINT32 uiWriteSize = 0;//ÿ��д�볤��

		UINT32 dwLeftSize = m_pStruCookie->uiReadPos;//���㻺����дָ���ռ��С

		if (dwLeftSize < dwLenAlign)
		{
			
			if (m_pStruCookie->uiReadPos == 0)
			{
				m_pStruCookie->uiReadPos = m_pStruCookie->uiTotalSize -(dwLenAlign -dwLeftSize);
				*(UINT32 *)(pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM) = dwLen;//����д�볤��
				uiWriteSize = dwLen;
				memcpy( pMemPos +( m_pStruCookie->uiReadPos + 1) * MEMCOMM_ALIGN_BYTE_NUM,pBuf,uiWriteSize);//�����������ݵ�������β��
			}
			else
			{
				m_pStruCookie->uiReadPos = m_pStruCookie->uiTotalSize -(dwLenAlign -dwLeftSize);
				*(UINT32 *)(pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM) = dwLen;//����д�볤��

				uiWriteSize = (dwLenAlign -dwLeftSize -1) * MEMCOMM_ALIGN_BYTE_NUM;
				memcpy( pMemPos +( m_pStruCookie->uiReadPos + 1) * MEMCOMM_ALIGN_BYTE_NUM,pBuf,uiWriteSize);//��������ǰ���

				uiWriteSize = dwLen - uiWriteSize;
				memcpy(pMemPos,(char *)pBuf + (dwLenAlign -dwLeftSize -1) * MEMCOMM_ALIGN_BYTE_NUM,uiWriteSize);//�������ݺ���
			}
			
		}
		else//���ο���
		{
			m_pStruCookie->uiReadPos = m_pStruCookie->uiReadPos - dwLenAlign;

			*(UINT32 *)(pMemPos + m_pStruCookie->uiReadPos * MEMCOMM_ALIGN_BYTE_NUM) = dwLen;//����д�볤��

			uiWriteSize = dwLen;
			memcpy(pMemPos + (m_pStruCookie->uiReadPos + 1) * MEMCOMM_ALIGN_BYTE_NUM,pBuf,uiWriteSize);//��������
		}
		Unlock();
		return ERROR_MEMCOMM_OPER_SUCCESS;
}

