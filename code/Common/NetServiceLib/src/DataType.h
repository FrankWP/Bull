#if !defined (DataType_DEF_H)
#define DataType_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		DataType.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/04/28
	Description:    ���ļ������ṩ���ⲿ�ӿ�ʹ�õ��������ͻ�ͷ�ļ�

*********************************************************************/
//������ͷ
#include "GSType.h"
#include "GSCommon.h"
#include "GSDefine.h"



namespace NetServiceLib
{

class ISocketChannel;

typedef enum 
{
	CLIENT,				//�ͻ���
	SERVER,				//��������
}enumServerType;
//ͨ�����ͽṹ
typedef enum 
{
	LISTEN_CHANNEL,		//����ͨ��
	COMM_CHANNEL,		//ͨѶͨ��
}enumChannelType;
typedef enum
{
	NET_PROTOCOL_TCP,	//TCPЭ��
	NET_PROTOCOL_UDP,	//UDPЭ��
}enumNetProtocolType;
typedef enum
{
	NET_ACCEPT,			//��������
	NET_READ,			//��ȡ�����յ�����
	NET_TIMEOUT,		//ͨ�����ⳬʱ����ĳͨ�����趨��ʱ����û���κ�ͨѶ
	NET_WRITE_FAIL,		//����ʧ��
	NET_REMOTE_DISCNN,	//�Զ������Ͽ����ӣ���Ҫ���TCP��ʽ
	NET_RECONNECT_FAIL,	//����ʧ�ܣ����TCP�ͻ��˷�ʽ
	NET_RECONNECT_SUCCESS,	//�����ɹ������TCP�ͻ��˷�ʽ
	NET_DISCNN,			//�ж�
	NET_ERR,			//��������
	NET_CLOSE,			//�ر�
	NET_NORMAL,			//����״̬����ʾ��ͨ�������շ�����
	
}enumNetEventType;//���ⲿ����

//�ص��������壬�ص������������̰߳�ȫ��
typedef INT (*pOnEventCallBack)(ISocketChannel* pSocketChnl, void* pUserData, enumNetEventType enumType, void* pData, UINT32 uiDataSize);





#define		ERROR_NET_UNKNOWN						(ERROR_NET_START + 1)	//δ֪����
#define		ERROR_NET_PARAM_WRONG					(ERROR_NET_START + 2)	//����Ĳ�������
#define		ERROR_NET_OVERFLOW_MAX_THREADS			(ERROR_NET_START + 3)	//��������߳���
#define		ERROR_NET_REPEAT_INIT					(ERROR_NET_START + 4)	//�ظ���ʼ��
#define		ERROR_NET_CREATE_SOCKET_FAIL			(ERROR_NET_START + 5)	//����socketʧ��
#define		ERROR_NET_BIND_SOCKET_FAIL				(ERROR_NET_START + 6)	//��socketʧ��
#define		ERROR_NET_MAX_CHANNEL_COUNT				(ERROR_NET_START + 7)	//�Ѵﵽ���������
#define		ERROR_NET_WSASTARTUP_FAIL				(ERROR_NET_START + 8)	//��������ʧ��
#define		ERROR_NET_MAX_CNETSERVICE_COUNT			(ERROR_NET_START + 9)	//�ﵽCNetService������
#define		ERROR_NET_CHANNEL_NOT_EXIST				(ERROR_NET_START + 10)	//ͨ��������
#define		ERROR_NET_THREAD_NOT_EXIST				(ERROR_NET_START + 11)	//�̲߳�����
#define		ERROR_NET_SOCKET_NOT_EXIST				(ERROR_NET_START + 12)	//SOKCET������
#define		ERROR_NET_SOCKET_CONNECT_FAIL			(ERROR_NET_START + 13)	//����ʱʧ��
#define		ERROR_NET_CREATE_CHANNEL_FAIL			(ERROR_NET_START + 14)	//����ͨ��ʧ��
#define		ERROR_NET_CALLBACK_NOT_INIT				(ERROR_NET_START + 15)	//�ص�����δ����ֵ
#define		ERROR_NET_INVALID_CHANNEL_POINT			(ERROR_NET_START + 16)	//��Ч��ͨ��ָ�룬����ͨ���Ѿ���ɾ��
#define		ERROR_NET_CHANNEL_STUTAS_CLOSE			(ERROR_NET_START + 17)	//ͨ��������Ч״̬
#define		ERROR_NET_CREATE_EPOLL_FAIL				(ERROR_NET_START + 18)	//����EPOLLʧ��

}

using namespace NetServiceLib;

#endif //end #if !defined (DataType_DEF_H)


