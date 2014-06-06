#if !defined (ThreadSelect_DEF_H)
#define ThreadSelect_DEF_H

/***************************************************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		ThreadSelect.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/11/22
	Description:    selectģ�ͣ�����������win mobile ��win32��linuxϵͳ

****************************************************************************************************/
#include "CommunicationManager.h"
#include "ThreadPoolModule.h"

#if OPERATING_SYSTEM


namespace NetServiceLib
{

	class CThreadDealNetEvent :
		public CCommunicationManager
	{
	public:
		CThreadDealNetEvent(void){};
		virtual ~CThreadDealNetEvent(void){};
		//�����������
		INT ThreadAcceptData(enumThreadEventType enumEvent, void* pObject);			
		//�߳�ִ�к��� ������ѯlinux��epoll�¼�
		INT  SelectEvent(enumThreadEventType enumEvent, void* pObject);
		
	private:
		// �����¼���Ӧ �̳���CCommunicationManager
		virtual	void OnEventModel( CSocketChannel* pclsSocketChannel );
	public:
		CThreadPoolModule			m_clsThreadPool;
		// �ź���
		CGSCond						m_GSCond;

	};

}

#endif


#endif


