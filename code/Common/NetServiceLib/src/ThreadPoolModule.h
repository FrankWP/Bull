#if !defined (ThreadPoolModule_DEF_H)
#define ThreadPoolModule_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		ThreadPoolModule.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/04/30
	Description:    �̳߳�ģ�顣֧��linux windows��

*********************************************************************/


#include "NetServiceDataType.h"

#include "CommunicationManager.h"


namespace NetServiceLib
{

#define		THREADPOOL_MAX_THREAD_COUNT		50		//����߳���
//#define		USE_NETHREAD	1

#ifdef _WIN32

#define NetThread			HANDLE				//�߳�����

#else

#define NetThread			pthread_t

#endif

class CNetThread;

typedef void (*ThreadCallbackFunction)(CNetThread* pclsThread, void *pUserParam );


//�¼���Ҫ�м��������ܡ������ݡ�������

//�߳�����ṹ��
typedef struct ThreadPoolTask
{
	enumThreadEventType		enumEvent;					// �¼������������ӡ����ܡ����ջص������ͣ���ͬ���͵��¼��̳߳����߳̽�ִ�в�ͬ����
	pThreadPoolExecute_Fn	pFunction;					// �߳�ִ�к���.
	void*					pObject;					// ָ��CThreadIOCP����.
	void*					pObject2;					// ���û�ָ����Ҳ������NULL ��
}struThreadTask, *pstruThreadPoolTask;

class CNetThread
{
public:
	CNetThread();
	virtual ~CNetThread();

	// �����߳�����ָ��
	BOOL Start(ThreadCallbackFunction fnOnEvent=NULL, void *pUserParam=NULL);
public:
	ThreadCallbackFunction		m_fnUser;
	NetThread					m_hThread;
	void*						m_pFnUserParam;

};
class CThreadPoolModule
{
public:
	CThreadPoolModule(void);
	virtual ~CThreadPoolModule(void);
public:
	// ��ʼ�̳߳�
	INT			Initialize( UINT uiThreads=2 );

	// �ͷ��̳߳�
	void		Uninitialize();
	
	//��������
	bool		AssignTask(pstruThreadPoolTask pTask);

	// �̳߳ع����߳�
	static void ManagerThreadProc(CNetThread* pclsThread, void * pInfo);
	
	//��̬�������߳�ִ�к���.
	static void WorkerThreadProc(CNetThread* pclsThread, void * pInfo);

	

	// �����̳߳��˳�
	inline	void		SetIsExitWorkerThreadProc(){ m_bIsExitWorkerThreadProc = false;};

	// �����߳� �˺����ص��ṩ���ⲿ�����ⲿ����������Ҫ���������̡߳�����ⲿ�����ѣ������߳�Ҳ�ỽ���߳���ִ������ֻ��������Ӧ��һ��
	inline	void	WakeUpThread(){ m_GSCond.Signal(); }
public:

	// �̳߳ع����߳�ִ�к���
	void		ManagerThreadProc(CNetThread* pclsThread);

	//�߳�������ִ�к���
	void		WorkerThreadProc(CNetThread* pclsThread);

	

	//������Ϊ�˲���ʱȡ�����ݶ���ӵĺ���
	//��ȡ�����߳���Ŀ
	INT16		GetIdleThreadCount();
	//��ȡæµ�߳���Ŀ
	INT16		GetBusyThreadCount(){ CGSAutoMutex	AutoMutex(&m_GSMutexDequeThread); return m_VecBusyThread.size(); };
	//��ȡ�����߳���Ŀ
	INT16		GetAllThreadCount(){ CGSAutoMutex	AutoMutex(&m_GSMutexDequeThread); return m_VectorThread.size(); };
	//��ȡ��ǰ���ڵ�������Ŀ
	INT16		GetTaskCount(){ CGSAutoMutex	AutoMutex(&m_GSMutexDequeTask); return m_DequeTask.size(); };
	//��ȡ��ǰ��ɾ�����߳���
	INT16		GetWaitDelThreadNum(){ return m_iWaitDelThreadNum; };
	//��ȡ��ǰɾ�������е��߳���
	INT16		GetVecWaitDelThread(){ CGSAutoMutex	AutoMutex(&m_GSMutexDequeThread); return m_VecWaitDelThread.size(); };
	// ������־ָ��
	void		SetLogInstancePtr( ILogLibrary* clsLogPtr){ m_clsLogPtr = clsLogPtr; };
private:
	INT			AllocNewThreads(UINT uiThreadCount);
	INT			MoveToBusyVec(CNetThread* pclsThread);//�Ƶ�æµ����
	INT			MoveToIdleVec(CNetThread* pclsThread);//�ƶ����ж��� 
	// �ͷ��߳�
	INT			FreeThread();
	//�����߳�
	INT			AddThread(UINT16 usThreads);
	//�����߳� 
	INT			SubThread(UINT16 usThreads);
	//�Ӷ����Ƴ��߳�
	INT			DeleteFromDeque( CNetThread* pclsThread );
private:	//����
	deque<pstruThreadPoolTask>				m_DequeTask;			//�������
	vector<CNetThread*>						m_VectorThread;			//�̶߳���
	vector<CNetThread*>						m_VecBusyThread;		//æµ�̶߳���
	vector<CNetThread*>						m_VecIdleThread;		//�����̶߳���
	// �ȴ�ɾ�����̶߳���
	vector<CNetThread*>						m_VecWaitDelThread;

	UINT									m_uiMaxThreadCount;		//����߳���
	UINT									m_uiCurThreadCount;		//��ǰ�߳���
	CGSMutex								m_GSMutexDequeTask;		//���������
	CGSMutex								m_GSMutexDequeThread;	//�̶߳�����
	bool									m_bIsExitWorkerThreadProc;//���߳��˳���־
	INT										m_iIdleThreadMaxNum;		//�����߳������ֵ ���ڴ�ֵӦ�����߳�
	INT										m_iIdleThreadMinNum;		//�����߳�����Сֵ ���ڴ�ֵӦ�����߳�

	
	
	// ��ɾ�����̼߳���
	INT										m_iWaitDelThreadNum;

	// �ź���
	CGSCond									m_GSCond;		

	// �����߳�
	CNetThread								m_clsManagerThread;

	// �˳���ʱ
	UINT64									m_unExitTick;

	// �����߳��˳���־	TRUE:���˳�   FALSE��δ�˳�
	BOOL									m_bIsExitManangeThread;

	// ��־ָ��
	ILogLibrary*							m_clsLogPtr;

public:
	UINT16									m_ExitThreadCount;


};

}

#endif

