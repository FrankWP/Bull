#ifndef ISYSTEMDATATYPE_DEF_H
#define ISYSTEMDATATYPE_DEF_H

#include "GSDefine.h"
#include "GSType.h"

#ifdef _WIN32
#elif _LINUX
#include <semaphore.h>
#endif

/******************************************************************************/
/********************************���ݽṹ����*********************** ***/
/******************************************************************************/

#define GS_FILE_FIFO		0x1000  //�ܵ��ļ�
#define GS_FILE_OTHER		0x2000	//��������	
#define GS_FILE_DIRECTORY   0x4000	//Ŀ¼
#define GS_FILE_COMMON		0x8000  //��ͨ�ļ�

/*�ļ���Ϣ�ṹ*/
typedef struct _StruGSFileInfo
{
	std::string strFileName;		//�ļ���
	INT	  iFileType;				//�ļ����� :Ŀ¼����ͨ�ļ����ܵ��ļ�����������
}StruGSFileInfo, *StruGSFileInfoPtr;


/* ϵͳʱ��ṹ�� */
typedef struct _StruSysTime
{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
}StruSysTime, *StruSysTimePtr;

/******************************************************************************/
/********************************������Ϣ��ȡ���ݽṹ����*********************** ***/
/******************************************************************************/
//����ת���ֽ�Ϊ1K
#define DIV 1024
#define	GS_MAX_NET_NAME_LEN	256
#define	MAX_NET_COUNTS		16

typedef struct StruGSNETSTATTable
{	
	char szName[GS_MAX_NET_NAME_LEN]; //������������
	INT64 iRecv;					//�ܹ������ֽ���
	INT64 iTrans;					//�ܹ������ֽ���
	double dRecvSpeed;				//��������ٶ�
	double dTransSpeed;				//���紫���ٶ�

}StruGSNETSTATTable,*StruGSNETSTATTablePtr;

typedef struct StruGSNETSTAT
{	
	INT32				iNetNum;			//��������
	StruGSNETSTATTable	stNetStatTable[1];	//����������Ϣ
}StruGSNETSTAT,*StruGSNETSTATPtr;
/******************************************************************************/
/********************************�߳����Ԫ�ض���*********************** ***/
/******************************************************************************/

#ifdef _WIN32

#ifndef  GS_INIT_SEM_COUNT						
#define  GS_INIT_SEM_COUNT  (LONG)1
#endif
#ifndef	 GS_MAX_SEM_COUNT
#define  GS_MAX_SEM_COUNT   (LONG)1
#endif

#define CAN_NOT_READ_SIGN   0x80000000
#define CAN_NOT_WRITE_SIGN  0x40000000
#define READ_COUNT_MASK     0x3fffffff

#define GSThread			HANDLE				//�߳�����
#define GSProcessMutex		HANDLE				//������
#define GSMutex				CRITICAL_SECTION	//�߳���
#define GSRwmutex			CRITICAL_SECTION	//��д��
#define GSCond				HANDLE				//��������
#define GSSem				HANDLE				//�ź���

#elif _LINUX

#define GSThread			pthread_t
#define GSProcessMutex		INT
#define GSMutex				pthread_mutex_t
#define GSRwmutex			pthread_rwlock_t
#define GSCond				pthread_cond_t
#define GSSem				sem_t*


#endif


#endif // ISYSTEMDATATYPE_DEF_H
