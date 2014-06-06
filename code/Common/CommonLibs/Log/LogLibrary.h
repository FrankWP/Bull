#pragma once

#include <stdio.h>
#include <string.h>
#include "ILogLibrary.h"

#define LOG_MAX_SIZE				2*1024*1024		//��־��С
#define LOG_MEDIUM_NUM				2				//��־ý����Ŀ������̨���ļ���
#define LOG_LEVEL_NUM				10				//��־�ȼ���Ŀ
#define LOG_DIR_NAME_NUM			512				//·����󳤶�
#define LOG_DOCUMENT_NAME_NUM		32				//��־�ļ�������󳤶�
#define LOG_FILE_NAME_NUM			64				//��־�ļ�����󳤶�
#define LOG_EX_NAME_NUM				32				//��־�ļ���չ��
class CLogLibrary:public ILogLibrary
{
public:
	CLogLibrary(void);
	virtual ~CLogLibrary(void);

public:


	/*��ý�ʶ�Ӧ�ȼ���Ϣ*/
	typedef struct 
	{
		UINT uiMedium;//ý��
		BOOL bFlag;//�Ƿ���Ч
		UINT uiLevel;//��Ӧ�ȼ�
	}StruLogWay;

private:
	FILE *m_pfLog;//��־�ļ����

	char m_strDir[LOG_DIR_NAME_NUM];//��־·��,���û��趨
	char m_strDocu[LOG_DOCUMENT_NAME_NUM];//��ǰ��־�����ļ��У��Ǹ��ݵ�ǰ�����Զ�����,����"log2010_12_31"
	char m_strFileName[LOG_FILE_NAME_NUM];//��־�ļ���
	char m_strFileEx[LOG_EX_NAME_NUM];	 //��־�ļ���չ��
    char m_strWholeName[1024];//��־ȫ·��
	void *m_pCaller;//�û�����
	void ( *m_pfnDealCB)(void *pCaller,const char *pszLevel,const char *pszPrefix,const char *pszMsg);
	UINT64 m_uiLogSize;//��־��С
	UINT64 m_uiCurSize; // ��־��ǰ��С
	UINT m_uiWay;//�ļ����ԣ��ؾ���½�
	StruLogWay m_stLogWay[LOG_MEDIUM_NUM];//��¼��־���������ʽ����صȼ���Ϣ
	CGSMutex *m_pGSMutex;//����֧�ֶ��߳�
	BOOL bDiskReady;// ����׼���ñ�־
public:

	INT  SetLogDir(const char *pszDir,const char *pszExtName);//������־·��
	INT  SetLogSize(UINT uiWay,UINT uiSize);// ������־��С���Ƿ�ؾ�
	INT  SetLogLevel(UINT uiMedium,UINT uiLevel);//������־�����ʽ������̨���ļ�������Ӧ��־����
	void SetLogCB(void *pCaller,void ( *pfnDealCB)(void *pCaller,const char *pszLevel,const char *pszPrefix,const char *pszMsg));//������־�ص�������
	INT  Log(UINT uiLevel,char *pszPrefix,char  *pszFormat,...);//֧�ֱ䳤
private:
	INT GenerateLogDir(const char *pszDir);
	INT WriteToFile(const char *pszMsg);
	string GetTimeStr();
	INT CreatLogDir(char *pszDir);
	BOOL CountDiskFreeSpace(const char *pDir,UINT64 &uiTotalFreeSize);// ��ȡ���̿���ʣ��ռ�

};
