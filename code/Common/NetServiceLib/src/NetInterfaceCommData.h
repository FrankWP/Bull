#if !defined (NetInterfaceCommData_DEF_H)
#define NetInterfaceCommData_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		NetInterfaceCommData.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/05/12
	Description:     ����ӿڵĹ������� ,����ͨ�����洢�ڴˣ��Լ���������

*********************************************************************/


#include "NetServiceDataType.h"

#include "SocketChanel.h"

namespace NetServiceLib
{


typedef vector<CSocketChannel*>		VectorChannelPoint;

class CNetInterfaceCommData
{
public:
	CNetInterfaceCommData(void);
	virtual ~CNetInterfaceCommData(void);
	void	Init();
	INT		OnEventCallBack(ISocketChannel* pSocketChnl,  enumNetEventType enumType, void* pData, UINT32 uiDataSize);	
public:
	//�����Ƿ�����
	INT		SetReConnect(bool bReConnect){m_bReConnect = bReConnect;return ERROR_BASE_SUCCESS;};
	//����ͨ��������
	INT		SaveSocketChannel(CSocketChannel* pSocketChannel);//����ͨ��
	//�Ӷ�����ɾ��ͨ�������ͷ�ͨ����Դ����һ����
	INT		DeleteSocketChannel( CSocketChannel* pSocketChannel );
	// ��ȡ���ڵ�ͨ��,�����ڷ���NULL
	CSocketChannel*		GetExistSocketChannel( DWORD dwRemoteIP, UINT16 unRemotePort );
	// ����ͨ��ָ���ж�ͨ���Ƿ���ڡ����ڷ���TRUE�������ڷ���FALSE
	BOOL	IfExistSocketChannel( CSocketChannel* pclsSocketChannel );
	//�����û����ݺͻص�����
	INT		SetOnEventCallBack(void* pUserData, pOnEventCallBack OnEventCallBack);
	//�����Ƿ�֧����Ϣ����
	INT		SetMsgBufFlag(bool bMsgBufFlag){m_bMsgBufFlag = bMsgBufFlag; return ERROR_BASE_SUCCESS;};
	//�������ӻ���ʱ��
	INT		SetActiveTime(UINT16 unActiveTime){m_unActiveTime = unActiveTime; return ERROR_BASE_SUCCESS;};
	//�������ͨ����
	INT		SetMaxChannel(UINT16 unMaxChannel);
	//��ȡ����ͨ����Ŀ
	INT		GetAllChannelNum();
	//�������ͨ������
	INT		SaveToFaultVector(CSocketChannel* pSocketChannel);
	//�ӹ���ͨ��������ɾ��ͨ�������ͷ�ͨ����Դ����һ����
	INT		DeleteoFromFaultVector( CSocketChannel* pSocketChannel );
	// ���������������ģʽ
	void	SetBlockMode( BOOL bMode){  m_bBlockMode = bMode; };
	// ��ȡm_bBlockModeֵ
	BOOL	GetBlockMode(){ return m_bBlockMode; };
	// ����socket���ͻ�������С
	INT		SetSendBuf(INT iBufSize);
	// ����socket���ջ�������С
	INT		SetRcvBuf(INT iBufSize);

	ILogLibrary* GetLogInstancePtr(){ return m_clsLogPtr; };

	// ������־��ָ�룬����ͬʱ��ʼ���������ˡ��ͻ��˵����
	void SetLogInstancePtr(ILogLibrary* pLog);

	// ������־·��
	void	SetLogPath( const char*	czPathName );


public: 
	//����
	INT		TestActiveTime();
	//������������
	bool	TestMaxChannelCount();
protected:
	//true:���� false����������   Ĭ������
	bool			m_bReConnect;	

	//��Ϣ���� true��ʹ����Ϣ���� false��ʹ�� Ĭ�ϲ�ʹ��
	bool			m_bMsgBufFlag;	

	//ͨ����������δͨѶʱ�䣬��λ��
	UINT16			m_unActiveTime;	

	//��������� 
	UINT16			m_unMaxChannel;		

	//ģʽ	TRUE: ������	FALSE: ������ Ĭ��FALSE
	BOOL			m_bBlockMode;

	// socket���ͻ�������С ��������� �����ϵͳ��Ĭ��ֵ
	UINT16			m_unSocketSendBuf;

	// socket���ջ�������С ��������� �����ϵͳ��Ĭ��ֵ
	UINT16			m_unSocketRcvBuf;
	
	//�ص�
	pOnEventCallBack	m_pfnOnEventCallBack;//�ص�ָ��

	//�û����룬�ص�ʱ�������û�
	void*				m_pUserData;		
	//ͨ������ �������ͨ��
	VectorChannelPoint		m_vectorSocketChannel;	

	//��д�� �������m_vectorSocketChannel���ж�ʹ�á�
	CGSWRMutex			m_GSWRMutexVectorChannel;	
	CGSMutex			m_GSMutexVerChnl;

	//����ͨ������	����й��ϵ�ͨ�����У����жϡ��رա���ʱ�ȵ�
	VectorChannelPoint		m_vectorFaultChannel;	

	//��д�� �������m_vectorFaultChannel���ж�ʹ�á�
	CGSWRMutex			m_GSWRMutexVecFaultChannel;	
	CGSMutex			m_GSMutexVerFaultChnl;

	// ��־ָ��
	ILogLibrary*		m_clsLogPtr;


};

}

#endif

