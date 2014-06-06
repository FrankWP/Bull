#ifndef ISYSTEMLAYINTERFACE_DEF_H
#define ISYSTEMLAYINTERFACE_DEF_H

#if !defined(_DEBUG) && !defined(DEBUG)
#ifndef NDEBUG
#	define NDEBUG
#endif
#endif

#include <assert.h>


#include "ISystemDataType.h"
#ifdef WINCE
#include <windows.h>
#elif	_WIN32	
#include <process.h>
#endif






/********************************************************************************************
  Copyright (C), 2010-2011, GOSUN 
  File name 	: ISYSTEMLAYINTERFACE.H      
  Author 		: hf      
  Version 		: Vx.xx        
  DateTime 		: 2010/6/9 16:31
  Description 	:     // ʵ��ϵͳ��װ
						1���߳�		CGSThread��
						2����ͨ��	CGSMutex��
						3���Զ���	CGSAutoMutex��
						4��������	CGSProcessMutex��
						5����д��	CGSWRMutex��
						6���ź���	CGSSem��
						7����������	CGSCond��
						8��Ŀ¼ö�� CGSDir��
						9��ö�ٴ���	GSGetSysDisk����
						10��ʱ�亯�� DoGetTickCount()�� DoGetLocalTime����
********************************************************************************************/

/*******************************************************************************
����˵�����߳�

ʹ��˵�����̺߳����ڵ���ʱ���������·�ʽ��
void threadfun(CGSThread *gsThreadHandle, void *pUserParam)
{
	while(!gsThreadHandle->TestExit()&&...)
	{
		...
	}
}
*******************************************************************************/
class CGSThread;
class CGSCond;
class CGSMutex;

/********************************************************************************************
  Function		: �̺߳����Ķ���    
  DateTime		: 2010/6/9 16:43	
  Description	: �̺߳�������
  Input			: CGSThread *gsThreadHandle���߳̾��
					void *pUserParam��		�û�����
  Output		: NULL
  Return		: NULL
  Note			: NULL	
********************************************************************************************/
typedef void (*GSThreadCallbackFunction)(CGSThread *gsThreadHandle, void *pUserParam );

/*#ifdef _WIN32
extern DWORD WINAPI CGSThreadProcFunc(HANDLE param);
#elif _LINUX
extern void *CGSThreadProcFunc(HANDLE param);
#endif*/ 

class GS_CLASS CGSThread
{
//public:
//#ifdef _WIN32
//	friend DWORD WINAPI CGSThreadProcFunc(HANDLE param);
//#elif _LINUX
//	friend void *CGSThreadProcFunc(HANDLE param);
//#endif 
	
public:
	CGSThread();
	virtual ~CGSThread(void);
	virtual BOOL Start(GSThreadCallbackFunction fnOnEvent=NULL, void *pUserParam=NULL);	 //��ʼ�̣߳��ɹ�����TRUE,ʧ�ܷ���FALSE	
	virtual BOOL Stop();	//ֹͣ�̣߳��ɹ�����TRUE,ʧ�ܷ���FALSE	
	virtual BOOL TestExit();	//�ж��߳��Ƿ��˳������̺߳�����ѭ���е��ã�ִ����ͣ�ͻ��Ѳ���	
	BOOL Join();	//�ȴ��߳̽������ɹ�����TRUE,ʧ�ܷ���FALSE	
	BOOL Join(INT mseconds);	//�ȴ�һ��ʱ�䣬���߳̽���,�ɹ�����TRUE,ʧ�ܷ���FALSE	
	BOOL Suspend();		//�߳���ͣ,�ɹ�����TRUE,ʧ�ܷ���FALSE	
	BOOL Resume();		//�����߳�,�ɹ�����TRUE,ʧ�ܷ���FALSE	
	BOOL IsRunning ();	//�����߳�����״̬��TRUEΪ�������У�FALSEΪδ����
	void Cancel();		//ǿ���˳�
	GSThread GetThreadHandle(); //��ȡ�߳̾�������ػ�ȡ�ľ��������������ڣ�����NULL
	void UnInitData();	//�ͷ��߳̾��

protected:
	void Kill(void);	//ǿ���߳��˳�	
	void Detach(void);	//linux�·����߳�	

public:
	BOOL m_bRunningState;
	BOOL m_bExit;
	GSThreadCallbackFunction m_fnUser;
	void *m_pFnUserParam;
	CGSMutex *m_GSMutexUnit;
	BOOL m_bMutexLock;
	CGSCond  *m_GSCond;	

	BOOL	m_bthreadfinish;
	CGSMutex *m_GSMutexthreadfinish;

protected:
	GSThread m_GSThread;
	CGSCond	 *m_GSCondPause;
	BOOL m_bExiting;
	BOOL m_bPause;
	BOOL m_bJoin;
	BOOL m_bDetached;
	CGSMutex *m_GSMutexPause;

};




/*******************************************************************************
����˵������ͨ��
*******************************************************************************/
class GS_CLASS CGSMutex
{
public:
	CGSMutex();
	~CGSMutex(void);
	BOOL	Lock();		//�������ɹ�����TRUE��ʧ�ܷ���FALSE  
	void	Unlock();	//�������ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL	TryLock();	//�������ļ������ɹ�����TRUE,ʧ�ܷ���FALSE

public:
	GSMutex m_GSMutex;
};

/*******************************************************************************
����˵�������̼����
*******************************************************************************/
class GS_CLASS CGSProcessMutex
{
public:
	CGSProcessMutex( const char *czKey);
	~CGSProcessMutex(void);
	BOOL	LockProcess();  //�������ɹ�����TRUE,ʧ�ܷ���FALSE  
	void	UnlockProcess(); //�������ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL	TryLockProcess();//�������ļ������ɹ�����TRUE,ʧ�ܷ���FALSE

private:
	GSProcessMutex m_GSProcessMutex;
};

/*******************************************************************************
����˵�����ź���
*******************************************************************************/
class GS_CLASS CGSSem
{
public:
	CGSSem( const char *czKey = NULL,BOOL bProcess=TRUE ); //���������const char *czKey ���ź���������
															//BOOL bProcess���Ƿ��ǽ��̼���ź�����TRUE:�ǣ�FALSE:��
	~CGSSem();
	BOOL IsValid( void ); //�ж��ź����Ƿ���Ч,�ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL Signal(); //�����ź������ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL Wait(); //�ȴ��ź������ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL Wait( UINT mSeconds ); //�ȴ��ź���һ��ʱ�䣬UINT mSeconds���ȴ���ʱ�䣬�ɹ�����TRUE,ʧ�ܷ���FALSE

private:
	GSSem m_GSSem;
	BOOL  m_bIsValid;
	
};

/*******************************************************************************
����˵������д��
*******************************************************************************/
class GS_CLASS CGSWRMutex
{
public:
	CGSWRMutex();
	~CGSWRMutex(void);
	BOOL	LockReader(); //�������������ɹ�����TRUE,ʧ�ܷ���FALSE 
	BOOL	TryLockReader();  //�������ļ������������ɹ�����TRUE,ʧ�ܷ���FALSE  
	void	UnlockReader(); //����������
	BOOL	LockWrite(); //����д�������ɹ�����TRUE,ʧ�ܷ���FALSE
	BOOL	TryLockWrite(); //�������ļ���д�������ɹ�����TRUE,ʧ�ܷ���FALSE
	void	UnlockWrite(); //����д����

private:
#ifdef WINCE
	BOOL GSStopReadWaitSetWrite();
	HANDLE m_GSReadEvent;
	HANDLE m_GSWriteEvent;
	LONG m_readCount;
	LONG m_writeCount;
#elif _WIN32
	BOOL GSStopReadWaitSetWrite();
	HANDLE m_GSReadEvent;
	HANDLE m_GSWriteEvent;
	volatile LONG m_readCount;
	volatile LONG m_writeCount;
#ifdef _DEBUG
#define _MUTEX_DEBUG  
#endif

#ifdef _MUTEX_DEBUG
	volatile LONG m_iRRefDebug;
	volatile LONG m_iWRefDebug;
#endif
#elif _LINUX
	GSRwmutex	m_GSRwmutex;
#endif
};

class GS_CLASS CGSAutoReaderMutex
{
private :
	CGSWRMutex *m_pMutex;
public :
	CGSAutoReaderMutex(CGSWRMutex *pWRMutex )
		: m_pMutex(pWRMutex)
	{
		m_pMutex->LockReader();
	}
	~CGSAutoReaderMutex(void)
	{
		m_pMutex->UnlockReader();
	}

};

class GS_CLASS CGSAutoWriterMutex
{
private :
	CGSWRMutex *m_pMutex;
public :
	CGSAutoWriterMutex(CGSWRMutex *pWRMutex )
		: m_pMutex(pWRMutex)
	{
		m_pMutex->LockWrite();
	}
	~CGSAutoWriterMutex(void)
	{
		m_pMutex->UnlockWrite();
	}

};

/*******************************************************************************
����˵�����Զ���
*******************************************************************************/
class GS_CLASS CGSAutoMutex
{

public :
	CGSAutoMutex( CGSMutex *locker ); //����
	~CGSAutoMutex(void); //����

private:
	CGSMutex *m_locker;

};

/*******************************************************************************
����˵������������
*******************************************************************************/
class GS_CLASS CGSCond
{
public:
	CGSCond();
	~CGSCond(void);
	INT	Wait(); //�ȴ����������� ����0Ϊ�ɹ���������ʾʧ��  
	INT	WaitTimeout(INT mseconds); //�ȴ���������һ��ʱ�䣬mseconds���ȴ�����������ʱ�䣬����0Ϊ�ɹ���������ʾʧ��
	INT	Signal(); //�����źţ�����0Ϊ�ɹ���������ʾʧ��
	INT	BroadcastSignal(); //���͹㲥�źţ�����0Ϊ�ɹ���������ʾʧ��

private:
	GSCond m_GSCond;
#ifdef _WIN32
	HANDLE m_mutex; 
#elif _LINUX
	CGSMutex *m_CondMutex;
#endif


};


class GS_CLASS CGSCondEx
{
private :
#ifdef _WIN32
	HANDLE m_hEvent;
	INT m_iWaitConts; 
#else
	pthread_cond_t  m_hEvent;
#endif

public :
	//����ֵ
	static const INT R_SUCCESS = 0; //�ɹ�
	static const INT R_ESYSTEM = 1; //��ʱ
	static const INT R_ETIMEOUT = 2; //ϵͳ�������ô���



	CGSCondEx(void);
	~CGSCondEx(void);
	INT	Wait( CGSMutex *pMutex ); //�ȴ���������, 	 ;
	INT	WaitTimeout(CGSMutex *pMutex, INT mseconds); //�ȴ���������һ��ʱ�䣬
	//mseconds���ȴ�����������ʱ�䣬
	//��ʱ���� R_ETIMEOUT	
	INT	Signal(void); //�����źţ�
	INT	BroadcastSignal(void); //���͹㲥�źţ�����0Ϊ�ɹ���������ʾʧ��
	
};


/******************************************************************************
����˵����Ŀ¼ö��
******************************************************************************/
class  GS_CLASS CGSDir
{
public:
	CGSDir();
	~CGSDir(void);
	BOOL	OpenDir(const char *czDirPath);   //��ָ��·����Ŀ¼���ɹ�����TRUE,ʧ�ܷ���FALSE 
	void	CloseDir(void);						//�ر�Ŀ¼
	void	ReadDir(std::vector<StruGSFileInfo> &vectFileList); //��ȡĿ¼��Ϣ���������std::vector<StruGSFileInfo> &vectFileList

//	UINT64 GetDirSize(void); //��ȡĿ¼��С�� �򿪺����
private:
	HANDLE	m_GSDir;
	CGSString m_strPathName;
#ifdef WINCE
private:
	WCHAR	m_wczDirPath[MAX_PATH];
#endif

};

/******************************************************************************
����˵����������Ϣ
******************************************************************************/
class  GS_CLASS CGSProcessInfo
{
public:
	CGSProcessInfo();
	~CGSProcessInfo(void);
	INT32	GSGetTotalCPUUsage(INT32 &iCPU);   //��ȡ�ܵ�CPUʹ���ʣ�����CPUʹ���ʣ�����-1����ȡʧ��
	INT32	GSGetTotalMemoryUsage(DWORD &dwMem);	//��ȡ�ܵ��ڴ�ʹ����

private:
#ifdef _WIN32
	INT64 CompareFileTime (FILETIME time1, FILETIME time2);
#endif
	//���CPU�ĺ���
	INT32	GSGetProcessorNumber(); 

private:
	//cpu����
	INT32 m_processor_count;
};
/******************************************************************************
����˵��������������ȡ
******************************************************************************/
class  GS_CLASS CGSNetInfo
{
public:
	CGSNetInfo();
	~CGSNetInfo(void);
	INT32	GSGetNetCount();	//��ȡ������������������������ʧ�ܷ���0���߸���
	INT32	GSGetNetUsage(StruGSNETSTAT * pstNetStat,INT32 iBuffLen);		//��ȡ������������
private:
	StruGSNETSTATTable* GetLastNetStat(const char* szName);
private:
	UINT64				m_uiLastTime;		//��һ�λ�ȡ������ʱ��
	INT32				m_iNetCounts;		//��������
	StruGSNETSTATTable	m_stLastNetStat[MAX_NET_COUNTS];//��һ�θ�������������
	char*				m_pDataBuf;
	INT32				m_iBufLen;
};
/*******************************************************************************
����˵����ö�ٴ���
*******************************************************************************/

  INT	GSGetSysDisk(std::vector<std::string> &vectDiskList); //��ȡ������Ϣ������ڴ��̱��У����ش��̸���

/*******************************************************************************
����˵����ϵͳʱ��
*******************************************************************************/
  UINT64   DoGetTickCount(); //��ȡ�Ӳ���ϵͳ�����������������ĺ����������شӲ���ϵͳ�����������������ĺ�����

  void    DoGetLocalTime(StruSysTimePtr pLoaltime); //��ȡ���صĵ�ǰϵͳ���ں�ʱ�䣬����ڰ������ں�ʱ����Ϣ�Ľṹ��

// ��iVal ���е��������� �������Ӻ��ֵ
 long AtomicInterInc( GSAtomicInter &iVal );
// ��iVal ���еݼ������� ���صݼ����ֵ
 long AtomicInterDec( GSAtomicInter &iVal );

//�Ƚ�iVal �ͷ� ��iOldVal ��ȣ� �����ȣ��� iVal �趨Ϊ iNewVal, ������TRUE�� ���򷵻�FALSE
 BOOL AtomicInterCompareExchange(GSAtomicInter &iVal, const long iOldVal, const long iNewVal);

//���� iVal Ϊ iNewVal, ���� �趨ǰ iVal ��ֵ
 long AtomicInterSet(GSAtomicInter &iVal, const long iNewVal );

//�� iVal ִ�� AND ���� iFlag, ���� �趨ǰ iVal ��ֵ
 long AtomicInterAnd(GSAtomicInter &iVal, const long iFlag );

//�� iVal ִ�� Or ���� iFlag, ���� �趨ǰ iVal ��ֵ
 long AtomicInterOr(GSAtomicInter &iVal, const long iFlag );



//���ص�ǰ���ó�������ڵ�Ŀ¼
CGSString GSGetApplicationPath(void);

//��ʽ��Ŀ¼
void GSPathParser( CGSString &strPath);

//���czPath �Ƿ���ڣ� ��������� ��������Ŀ¼
BOOL GSTestAndCreateDir( const char *czPath);


#ifndef _WIN32
#define  localtime_s localtime_r
#endif

#endif // ISYSTEMLAYINTERFACE_DEF_H
