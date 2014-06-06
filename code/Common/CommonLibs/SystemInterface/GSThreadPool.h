/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : GSTHREADPOOL.H
Author :  zouyx
Version : 0.0.0.0
Date: 2011/9/9 10:28
Description: �����̳߳�
********************************************
*/

#ifndef _GS_H_GSTHREADPOOL_H_
#define _GS_H_GSTHREADPOOL_H_

#include "GSType.h"
#include "GSUtility.h"
#include "ISystemLayInterface.h"
#include <set>


class CGSThreadPool;

/*
*********************************************
Function :   GSThreadPoolCallback
DateTime : 2010/5/19 8:38
Description :   �̳߳صĹ��������ص�
Input : pcsPool �����������̳߳ض���
Input : TaskData �������ݿ�
Input : pDebugInfo ����ʹ�õ���Ϣ
Output :
Return :
Note :
*********************************************
*/
typedef void (*GSThreadPoolCallback)(CGSThreadPool *pcsPool, void *TaskData, void *pDebugInfo );

class CGSTaskThread;



/*
********************************************************************
��ע��
����    :    CGSThreadGlobal
����    :    zouyx
����ʱ��:    2012/4/24 9:10
������  :	�����̹߳������ �����߳��ڸ��������̳߳صĵ���
*********************************************************************
*/

class CGSThreadGlobal
{
private :
	friend CGSThreadPool;
	
	friend CGSTaskThread;



	static CGSThreadGlobal *s_pDefaultGlobal; //Ĭ�ϵĹ����̹߳������
	static GSAtomicInter s_iInitRefs; //�̳߳�ģ�鱻���õļ���

	//�������������
	StruGSListNode *m_pCache;
	StruGSListNode *m_vCacheBufferBegin;
	unsigned char *m_vCacheBufferEnd;
	UINT32 m_iCaches;


	StruGSListNode m_stSchedules; //�ȴ����ȵ��̳߳�
	StruGSListNode m_stGIdleThreads; //���е��߳�
	CGSMutex m_csMutex;  // m_stSchedules ��ͬ����


	std::set<CGSTaskThread*> m_csThreadQueue; //�̶߳���
	

	GSAtomicInter  m_iExitThreads;  //��ǰ�Ѿ��������߳���
	INT16	 m_iWaitQueueSize; //�ȴ����ȵ��̳߳ظ���
	INT16   m_iGIdleThreads; //��ǰ�����߳�
	UINT16  m_iGMaxIdleThreads; //����������еĹ����߳���	
	UINT16	m_iGMaxThreads; //��󹫹��߳���
	UINT16	m_iGThreads;  //��ǰ�����߳���	

	GSAtomicInter m_iRefs; //�Ķ�������ü���

	INT32 m_iDefaultIdleWait;


	CGSThreadGlobal(void);
	~CGSThreadGlobal(void);
	
	/*
	 *********************************************
	 Function : Ref
	 DateTime : 2012/4/24 9:14
	 Description :  ��������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void Ref(void); 

	/*
	 *********************************************
	 Function : Unref
	 DateTime : 2012/4/24 9:15
	 Description :  ��������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void Unref(void);


	/*
	 *********************************************
	 Function : GetListNode
	 DateTime : 2012/4/24 9:15
	 Description :  ���ڴ�ط��� StruGSListNode ����
	 Input :  
	 Output : 
	 Return :  ʧ�ܷ���NULL
	 Note :   
	 *********************************************
	 */
	INLINE StruGSListNode *GetListNode(void);

	/*
	 *********************************************
	 Function : FreeListNode
	 DateTime : 2012/4/24 9:16
	 Description : �ͷ� GetListNode ���صĶ���
	 Input :  pNode ���ͷŵĶ���
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	INLINE void FreeListNode(StruGSListNode *pNode);


	/*
	 *********************************************
	 Function : Push
	 DateTime : 2012/4/24 9:16
	 Description :  ���̳߳ؼӵ��ȴ����ȶ���
	 Input :  pThread ָ�����̳߳�
	 Input : bFirst �ͷ����ȵ���
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	BOOL Push( CGSThreadPool *pThread, BOOL bFirst  );

	/*
	 *********************************************
	 Function : Clear
	 DateTime : 2012/4/24 9:18
	 Description :  ����̳߳صĵ�������
	 Input :   pThread ָ�����̳߳�
	 Output : 
	 Return : ��������ĵ���������
	 Note :   
	 *********************************************
	 */
	INT Clear( CGSThreadPool *pThread );

	/*
	 *********************************************
	 Function : ClearSource
	 DateTime : 2012/4/24 9:19
	 Description :  �ͷű�������Դ
	 Input :  
	 Output : 
	 Return : 
	 Note :   �����ͷ�ʱ����
	 *********************************************
	 */
	void ClearSource(void);

public :
	/*
	 *********************************************
	 Function : Global
	 DateTime : 2012/4/24 9:19
	 Description :  ����ȫ���ԵĹ������
	 Input :  
	 Output : 
	 Return : ʧ�ܷ���NULL 
	 Note :  ֻ�е����� CGSThreadPool::InitModule�������
	 *********************************************
	 */
	static CGSThreadGlobal *Global(void);

	/*
	 *********************************************
	 Function : Create
	 DateTime : 2012/4/24 9:21
	 Description :  ���� CGSThreadGlobal ����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	static CGSThreadGlobal *Create(void);


	/*
	 *********************************************
	 Function : Release
	 DateTime : 2012/4/24 9:22
	 Description :  �ͷű�����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void Release(void)
	{
		ClearSource();
		Unref();
	}


	/*
	*********************************************
	Function : SetMaxGlobalThread
	DateTime : 2011/9/15 10:55
	Description :  �趨�����̳߳�ʹ�õ���󹫹��߳������� �����ָ��
	�߳����ݽ����̳߳ص�ʵ���������ӡ�
	��ֵ��������ռģʽ�̵߳�������

	Input :  iNums ����߳���
	Output : 
	Return : 
	Note :   0 ��ʾ���趨
	*********************************************
	*/
	void SetMaxGlobalThread(  UINT16 iNums = 32 );

	/*
	*********************************************
	Function : GetMaxGlobalThread
	DateTime : 2011/9/9 10:59
	Description :  ���ع����̳߳����������ٸ������߳��ڵȴ�
	Input :  
	Output : 
	Return : ���� SetMaxGlobalThread ���趨ֵ
	Note :   
	*********************************************
	*/
	INLINE  UINT16  GetMaxGlobalThread(void) const
	{
		return m_iGMaxThreads;
	}

	/*
	*********************************************
	Function : GetGlobalThreads
	DateTime : 2011/9/15 11:34
	Description :  ���ص�ǰ�����̳߳ص��̸߳���
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	INLINE UINT16 GetGlobalThreads(void) const
	{
		return m_iGThreads;
	}



	/*
	*********************************************
	Function :  SetMaxIdleThreads
	DateTime : 2010/5/19 15:03
	Description :  �趨�����̳߳����������ٸ������߳��ڵȴ�, Ĭ�� 32 
	Input :  iNums ��������
	Output :
	Return :
	Note :
	*********************************************
	*/
	void   SetMaxIdleThreads( UINT16 iNums = 32);

	
	/*
	*********************************************
	Function : GetMaxIdleThreads
	DateTime : 2011/9/9 10:59
	Description :  ���ع����̳߳����������ٸ������߳��ڵȴ�
	Input :  
	Output : 
	Return : ���� SetMaxIdleThreads ���趨ֵ
	Note :   
	*********************************************
	*/
	INLINE UINT16  GetMaxIdleThreads(void) const
	{
		return m_iGMaxIdleThreads;
	}


	/*
	*********************************************
	Function : GetIdleThreads
	DateTime : 2011/9/9 11:01
	Description :  ���ص�ǰ���е��߳���
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	INLINE UINT16 GetIdleThreads(void) const
	{
		return m_iGIdleThreads;
	}

	
	/*
	*********************************************
	Function : SetMaxIdleWait
	DateTime : 2011/9/9 11:02
	Description :  �趨Ĭ�Ͽ��м���ֵ
	Input :  iMSec ���ֵ�� ��λ����
	��ʾ�ԷǶ�ռģʽ �̳߳س�����ʱ��û�����̳߳ص��߳̽����յ������̳߳أ�
	���̳߳�������ʱ�ٴӹ����̳߳ش��»�ȡ�߳�
	<0 ��ʾ���õȴ�
	Ĭ��ֵ 500 ����
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	void  SetDefaultIdleWait( INT32 iMSec = 500 );





};




/*
********************************************************************
��ע��
����    :    CGSThreadPool
����    :    zouyx
����ʱ��:    2012/4/24 9:25
������  :		�̳߳أ� Ҳ�����Ϊ�����
*********************************************************************
*/

class CGSThreadPool
{

public:
	friend class CGSTaskThread;

	/*
	 *********************************************
	 Function : CGSThreadPool
	 DateTime : 2011/9/29 9:44
	 Description :  �����̳߳�
	 Input :  pThreadGlobal �����Ĺ����̹߳����ߣ� �����ָ����ʹ�� CGSThreadGlobal::Global Ϊ������
	 Output : 
	 Return : 
	 Note :   ÿ���̳߳ر�������ĳ�������̹߳�����
	 *********************************************
	 */
	CGSThreadPool( CGSThreadGlobal *pThreadGlobal = NULL);
	~CGSThreadPool(void);

	//���ش���Ŷ���
	static const INT RSUCCESS = 0;   //�ɹ�
	static const INT EUNKNOWN = -1;  //δ֪����
	static const INT EFLOWOUT = -2;  //�����޶���Χ���
	static const INT ENMEMORY = -3;  //û���ڴ�
	static const INT EINVALID = -4; //��������


	/*
	 *********************************************
	 Function : GetThreadGlobal
	 DateTime : 2011/9/29 9:42
	 Description :  ���ع������̵߳Ĺ�����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	CGSThreadGlobal &GetThreadGlobal(void);



	/*
	*********************************************
	Function : SetIdleWait
	DateTime : 2011/9/9 11:08
	Description :  �趨���߳̿��м���ֵ
	Input :  iMSec �趨ֵ, �ο� CGSThreadGlobal::SetDefaultIdleWait
	Output : 
	Return : 
	Note :   ������趨�� ���̳������̹߳����ߵ�Ĭ��ֵ, �ο�CGSThreadGlobal::SetDefaultIdleWait
	*********************************************
	*/
	void SetIdleWait( INT32 iMSec );



	/*
	*********************************************
	Function : Init
	DateTime : 2011/9/9 10:31
	Description :  ��ʼ���̳߳�
	Input : fnOnCallback �ص�����
	Input : iThreadCounts ָ�� �̳߳ز���ʹ�õ��̸߳���
	Input : bExclusive ָ���߳��Ƿ��Ƕ�ռ��ʽ
	*��ռ���ڱ��̳߳��ͷ�ǰ�����ͷ�������
	�ȴ��̶߳����ڸ��߳�����
	Output : 
	Return :  TRUE/FALSE
	Note :   ֻ�г�ʼ����ſ���ʹ��
	*********************************************
	*/

	BOOL Init( GSThreadPoolCallback fnOnCallback, 
		UINT16 iThreadCounts = 1, 
		BOOL bExclusive = FALSE);

	/*
	*********************************************
	Function : Uinit
	DateTime : 2011/9/9 10:42
	Description :  ֹͣ�̳߳�
	Input :  bWaitJobsComplete �Ƿ�ȴ�ִ����ȴ�������
	TRUE �ȴ�ִ�������еĴ�������
	FALSE ��ִ�еȴ�����, �ȴ����񽫱��趨�Ļص��ͷ�, �ο�SetFreedTaskDataFunction
	����TRUE/FALSE �ĺ�������ȵ��������߳��˳��ŷ���
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	void Uninit(BOOL bJobsComplete = FALSE );


	/*
	*********************************************
	Function : IsInit
	DateTime : 2011/9/9 11:19
	Description :  �����ͷ��Ѿ�����
	Input :  
	Output : 
	Return : TRUE/FALSE
	Note :   
	*********************************************
	*/
	BOOL IsInit(void);

	/*
	*********************************************
	Function : SetFreedTaskDataFunction
	DateTime : 2011/9/9 10:38
	Description :  �趨�ͷ��������ݵĻص�����					
	Input :   fnFreeFunc �ص�����
	����趨�� ����������ʱ��û��ִ�е�������󽫵��øú����ͷ�
	Output : 
	Return : 
	Note :   ������趨�� ������ܵ����ڴ����
	*********************************************
	*/
	void SetFreedTaskDataFunction( void (*fnFreeFunc)(void *)  );

	/*
	*********************************************
	Function : SetMaxWaitTask
	DateTime : 2011/9/9 10:53
	Description :  �趨���ĵȴ�������
	Input :  iMaxVal �趨��ֵ�� < 0 ��ʾ����Ч
	Output : 
	Return : 
	Note :   �趨��ֵ�� ����ȴ����ȵ�������ڵ��ڸ�ֵ�� �������ʧ�ܣ������� EFLOWOUT
	*********************************************
	*/
	void SetMaxWaitTask( INT32 iMaxVal = -1 );

	/*
	*********************************************
	Function : Task
	DateTime : 2011/9/9 10:56
	Description :  �������
	Input :  pTaskData ��������
	Input :  bFirst �ͷżӵ��Ѿ����ڵ�����ǰ�� TRUE �ӵ���ͷ�� ����ִ�У� FALSE �ӵ���β
	Output : 
	Return : �ɹ����� 0�� ������ʾ����
	EFLOWOUT �� �����趨�����ֵ
	ENMEMORY    ��ʾ ��ȡ�ڴ�ʧ��			
	Note :   
	*********************************************
	*/
	INT Task( void *pTaskData, BOOL bFirst = FALSE );

	/*
	*********************************************
	Function : GetWaitTask
	DateTime : 2011/9/9 11:11
	Description :  ��ȡ��ǰ�ȴ����ȵ���������
	Input :  
	Output : 
	Return : ��������ֵ
	Note :   
	*********************************************
	*/
	UINT32 GetWaitTask(void);

	/*
	*********************************************
	Function : Clear
	DateTime : 2011/9/9 11:15
	Description :  ������еȴ����ȵ�����
	Input :  
	Output : 
	Return : 
	Note :   �ͷŵ����ݽ������趨�Ļص��ͷţ� �ο� SetFreedTaskDataFunction
	*********************************************
	*/
	void Clear(void);

	/*
	*********************************************
	Function : RemoveTask
	DateTime : 2011/9/9 11:16
	Description :  �Ƴ�����
	Input :  pTaskData ���Ƴ�������
	Output : 
	Return : �����ܹ��Ƴ��ĸ����� ������-1
	Note :   
	*********************************************
	*/
	INT RemoveTask(void *pTaskData);

	/*
	*********************************************
	Function : IsSelfThread
	DateTime : 2011/9/9 11:18
	Description :  �ж��ͷ�Ϊ���̳߳ص��߳� 
	Input :  
	Output : 
	Return : TRUE/FALSE
	Note :   
	*********************************************
	*/
	BOOL IsSelfThread(void);

	/*
	*********************************************
	Function : SetUserData
	DateTime : 2011/9/9 11:20
	Description :  �洢�û�����
	Input :  pUserData �û�����, Ĭ��ֵΪ NULL
	Output : 
	Return : 
	Note :  �û�Ҫ�Լ��ͷŸ����ݣ� ע�ⲻҪ�ڴ�й¶ 
	*********************************************
	*/
	void SetUserData( void *pUserData );

	/*
	*********************************************
	Function : GetUserData
	DateTime : 2011/9/9 11:21
	Description :  ��ȡ�洢���û����ݣ� �ο�SetUserData
	Input :  
	Output : 
	Return : ���� SetUserData ���趨ֵ
	Note :   
	*********************************************
	*/
	void *GetUserData(void);

	/*
	 *********************************************
	 Function : Disable
	 DateTime : 2011/10/13 15:09
	 Description :  ��ֹʹ��
	 Input :  bJobsComplete �Ƿ�ȴ�ִ����ȴ������� �ο� Uninit
	 Output : 
	 Return : 
	 Note :   ������Ѿ����ڵ�����
	 *********************************************
	 */
	void Disable( BOOL bJobsComplete = FALSE );

	/*
	 *********************************************
	 Function : Enable
	 DateTime : 2011/10/13 15:10
	 Description :  ����, Disable �ķ�����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void Enable(void);

	/*
	*********************************************
	Function : IsEnable
	DateTime : 2011/10/13 15:10
	Description :  ���ص�ǰ�Ƿ�ʹ��
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	BOOL IsEnable(void);


	

	/*
	*********************************************
	Function : InitModule
	DateTime : 2011/9/9 11:42
	Description :  ��ʼ���̳߳�ģ��, ֻ�е��øĽӿں����ʹ���̳߳ع���
	Input :  
	Output : 
	Return : 
	Note :   ���Ҫʹ���߳�ģ�飬 ������øú���
	*********************************************
	*/
	static void InitModule(void);

	/*
	*********************************************
	Function : UninitModule
	DateTime : 2011/9/9 11:43
	Description :  �ͷ��̳߳ع���ģ�飬 ��InitModule���ʹ��
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	static void UninitModule(void);

private :
	/*
	 *********************************************
	 Function : Run
	 DateTime : 2012/4/24 9:26
	 Description :  �̹߳����ߵ�����������
	 Input :   pThread ִ�б�����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL Run(CGSTaskThread *pThread );


	
	static INT32      s_iGIdleMSecs; //Idle ��ʱ���� ���룬 ��ǰ����
	static GSAtomicInter m_iInitRefs; //���ü�����


	
	/*
	********************************************************************
	��ע��
	����    :    CCoreData
	����    :    zouyx
	����ʱ��:    2012/4/24 9:29
	������  :		�ڲ����ݹ�����
	*********************************************************************
	*/
	
	class CCoreData
	{
	public :
		CGSThreadPool *m_pPool;
		CGSThreadGlobal *m_pThreadGlobal;  //�̹߳�����
		GSThreadPoolCallback m_fnCallback; //�̻߳ص�
		void (*m_fnFreeFunc)(void *);  //�������ݶ����ͷź���
		void *m_pUserData;    //�û�����
		INT32 m_iIdleInterval; //������IDLE ʱ�䣬 ����

		INT32 m_iMaxJobs;			//�������ȴ��������
		UINT32 m_iRefs;
		UINT32 m_iWaitTask;			//�ȴ�ִ�е���������
		UINT32 m_iCaches;           //���ػ������

		UINT16 m_iPermitThreads;    //��������߳���  
		UINT16 m_iNumThreads;		//��ǰʹ�õ��߳���
		UINT16 m_iStatus;			//״̬
		UINT16 m_bExclusive;		//�Ƿ��ռ
		UINT16 m_bWaiting;			//�ͷŵȴ�ִ�����
		UINT16 m_iIdleThreads;



		

		StruGSListNode m_stTaskQueue; //�������
		StruGSListNode *m_pTaskCache; //����ṹ�ڴ�ض���

		CGSMutex m_csMutex;        //ͬ������
		CGSCondEx m_csCond;        //ͬ���ź�

		CGSCondEx  m_csExitCond;			//�ȴ��˳�

		
		/*
		 *********************************************
		 Function : CCoreData
		 DateTime : 2012/4/24 9:31
		 Description :  
		 Input :  pPool ���ڵ��̳߳�
		 Input :  pThreadGlobal  pPool �Ĺ�����
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		CCoreData(CGSThreadPool *pPool, CGSThreadGlobal *pThreadGlobal);
		~CCoreData(void);


		/*
		 *********************************************
		 Function : Unref
		 DateTime : 2012/4/24 9:31
		 Description :  ��������
		 Input :  
		 Output : 
		 Return : TRUE/FALSE
		 Note :   
		 *********************************************
		 */
		BOOL Unref(void);

		/*
		 *********************************************
		 Function : Ref
		 DateTime : 2012/4/24 9:32
		 Description :  ��������
		 Input :  
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		void Ref(void);

	    /*
	     *********************************************
	     Function : FreeWaitTask
	     DateTime : 2012/4/24 9:32
	     Description :  �ͷ��ڵȴ�����������
	     Input :  
	     Output : 
	     Return : 
	     Note :   
	     *********************************************
	     */
		void FreeWaitTask(void);

		/*
		 *********************************************
		 Function : RemoveTask
		 DateTime : 2012/4/24 9:33
		 Description :  �Ƴ��ȴ�������
		 Input :  
		 Output : 
		 Return : �����Ƴ��ĸ���
		 Note :   
		 *********************************************
		 */
		INT RemoveTask(void *pTaskData);

		/*
		 *********************************************
		 Function : FreeTaskNode
		 DateTime : 2012/4/24 9:34
		 Description :  GetTaskNode �ͷŵķ��ض���
		 Input :  pNode ���ͷŶ���
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		void FreeTaskNode(StruGSListNode *pNode);

		/*
		 *********************************************
		 Function : GetTaskNode
		 DateTime : 2012/4/24 9:34
		 Description :  ���ڴ�ط��� GetTaskNode ����
		 Input :  
		 Output : 
		 Return : NULL��ʾʧ��
		 Note :   
		 *********************************************
		 */
		StruGSListNode *GetTaskNode(void);	

		/*
		 *********************************************
		 Function : AddTask
		 DateTime : 2012/4/24 9:35
		 Description :  �������
		 Input :  pData �������
		 Input : bFirst �ͷ���Ч
		 Output : 
		 Return : NULL ��ʾʧ��
		 Note :   
		 *********************************************
		 */
		StruGSListNode * AddTask( void *pData, BOOL bFirst);

	};

	CCoreData *m_pCore; //��������

};


/*
*********************************************************************
*
*@brief : CObjThreadPool , �����̳߳�
*
*********************************************************************
*/

class CObjThreadPool;


/*
 *********************************************
 Function : FuncPtrObjThreadPoolEvent
 DateTime : 2012/4/24 9:37
 Description :  �����̳߳ػص�����
 Input :  pcsPool ��Ӧ���̳߳�
 Input : pTaskData ��������
 Output : 
 Return : 
 Note :   
 *********************************************
 */
typedef void (CGSObject::*FuncPtrObjThreadPoolEvent)(CObjThreadPool *pcsPool, void *pTaskData);






/*
********************************************************************
��ע��
����    :    CObjThreadPool
����    :    zouyx
����ʱ��:    2012/4/24 9:37
������  :		�����̳߳�
*********************************************************************
*/

class CObjThreadPool : public CGSThreadPool
{		
public :
	/*
	 *********************************************
	 Function : CObjThreadPool
	 DateTime : 2012/4/24 9:38
	 Description :  
	 Input :  czName �̳߳�����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	CObjThreadPool( const char *czName=NULL );

	/*
	 *********************************************
	 Function : CObjThreadPool
	 DateTime : 2012/4/24 9:38
	 Description :  
	 Input :  pThreadGlobal ָ���̹߳�����
	 Input :  czName �̳߳�����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	CObjThreadPool( CGSThreadGlobal *pThreadGlobal, const char *czName=NULL );
	virtual ~CObjThreadPool(void);


	/*
	 *********************************************
	 Function : Init
	 DateTime : 2012/4/24 9:39
	 Description :  ��ʼ���̳߳�
	 Input :  pTaskFnOwner ����ص����������Ķ���
	 Input :  pFnTaskCallback ����ص���������ص�����
	 Input : pcsPool �����������̳߳ض���
	 Input : TaskData �������ݿ�
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	BOOL Init( CGSObject *pTaskFnOwner, FuncPtrObjThreadPoolEvent pFnTaskCallback,  
		UINT16 iThreadCounts,  BOOL bExclusive);
private :
	CGSObject *m_pTaskFnOwner; //����ص����������Ķ���
	FuncPtrObjThreadPoolEvent m_pFnTaskCallback; //����ص���������ص�����
	CGSString m_strName; //����

	/*
	 *********************************************
	 Function : ProxyEntry
	 DateTime : 2012/4/24 9:41
	 Description :  CGSThreadPool �ĺ������, �ο�GSThreadPoolCallback˵��
				ͨ��������ת����������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	static void ProxyEntry( CGSThreadPool *pcsPool, void *TaskData, void *pDebugInfo);

};

#endif //end _GS_H_GSTHREADPOOL_H_
