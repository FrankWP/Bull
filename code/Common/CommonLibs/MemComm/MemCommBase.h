#ifndef MEMCOMMBASE_H_
#define MEMCOMMBASE_H_

#include "IMemComm.h"
#define MEMCOMM_ALIGN_BYTE_NUM 4 //���ֽڶ���
#define MEMCOMM_ALIGN_BYTE_NUM 4  //��ʶ���ȵ��ֽ���
#define MEMCOMM_SHARE_ID *(int *)"DAPM"//��������ʶID

typedef struct
{
	UINT32 uiMemId;//��������ʶ
	UINT32 uiOffSet;//��Ч��������ʼ��
	UINT32 uiTotalSize;//�������ܴ�С
	//UINT32 uiFreeSize;//������ʣ��ռ�
	UINT32 uiLock;// �Ƿ������0��������1����
	UINT32 uiWritePos;//дλ��
	UINT32 uiReadPos;//��λ��
	char chMutexName[32];//������
	char chEventName[32];//�¼����֣����ڶ�д֪ͨ
}StruMemCookie;
class CMemCommBase
{
public:
	CMemCommBase(void);
	virtual ~CMemCommBase(void);
protected:
	
	void * m_pHead;			//��Ϣ�������Ŀ�ʼ��ַ
	StruMemCookie *m_pStruCookie;
public:
	BOOL Create(UINT32 dwSize, BOOL bLock, EnumOperType opType, string &strName);
	void Reset(void);
	virtual BOOL Free(void)=0;
	INT32 Read(void *pBuf, UINT32 dwMaxLen, UINT32 *pRealLen, UINT32 dwMilliSeconds);
	INT32 Write(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds);
	INT32 WriteUrgent(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds);
protected:
	virtual BOOL Lock()=0;
	virtual void Unlock()=0;
	virtual BOOL Open(UINT32 dwSize, BOOL bLock, string &strName) = 0;
	virtual BOOL ReCreate(UINT32 dwSize, BOOL bLock, string &strName) = 0;
};

#endif


