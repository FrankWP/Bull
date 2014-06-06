#if !defined (INETSERVICE_DEF_H)
#define INETSERVICE_DEF_H
/********************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		INetService.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/04/28
	Description:    ���ļ�Ϊ������Ψһ�Ķ���ӿڡ��ϲ������Դ���3����������࣬ÿ����������඼����ȫ�����ģ�
					�������õ��������Ժͻص����������ֻ������3���������������Ϊÿ���������������ӵ��2*CPU
					�ں˸������̣߳�����ⲻϣ������������߳�.
					ISocketChannel��ͨ������ࣺ	�ϲ�ͨ����������ṩ�Ľӿڽ��������շ��ͻ�ȡͨ��������ԡ�
													ͨ������ֱ�����ⲿ����delete ISocketChannel �����ͷ���Դ��
													��������ʽ����CloseChannel()���������ͷš�
					INetService�������������ࣺ	�ϲ�ͨ����������ṩ�Ľӿڳ�ʼ����������Լ����ӡ�ɾ��ͨ����
													������������������ԡ�

*********************************************************************/
#include "DataType.h"

using namespace NetServiceLib;

namespace NetServiceLib
{

class ISocketChannel
{
public:
/********************************************************************************
  Function:       GetLocalIPPort
  Description:    ��ȡͨ���ı��ض˿�
  Input:			
  Output:         pszIP��IP��ַ		unPort���˿�
  Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
  Note:				
********************************************************************************/
	virtual	INT		GetLocalIPPort(char* pszIP, UINT16& unPort)=0;
/********************************************************************************
  Function:       GetReMoteIPPort
  Description:    ��ȡͨ����Զ�̶˿�
  Input:			
  Output:         pszIP��IP��ַ		unPort���˿�
  Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
  Note:				
********************************************************************************/
	virtual INT		GetReMoteIPPort(char* pszIP, UINT16& unPort)=0;
/********************************************************************************
  Function:       SendData
  Description:    �������ݡ�����⽫ֱ�ӵ���API��������
  Input:		  pData������ָ�룬	unDataLen�����ݳ���
  Output:         
  Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
  Note:			  ע�⣬����ʧ��ʱ���˺������᷵�س�����API�����룬����������ж���Ĵ�����
********************************************************************************/
	virtual INT		SendData(void* pData, UINT unDataLen)=0;
	//���ô˺����ر�ͨ��,��INetService�е�CloseChannel��һ���ġ�
/********************************************************************************
  Function:			CloseChannel
  Description:		�ر�ͨ��
  Input:  
  Output:         
  Return:			ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
  Note:				
********************************************************************************/
	virtual INT		CloseChannel()=0;
/********************************************************************************
	Function:			CloseChannelEx
	Description:		�ر�ͨ��
	Input:  
	Output:         
	Return:			ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			��CloseChannel��ͬ�ĵط��Ǵ�CloseChannelEx���Ը�ͨ���ӻص���	
********************************************************************************/
	virtual INT		CloseChannelEx()=0;
/********************************************************************************
  Function:       GetListenChannel
  Description:    ��ȡͨ���ļ���ͨ����TCP��UDP�����ԡ�
  Input:  
  Output:         
  Return:         ����У������ؼ���ͨ����ָ��ISocketChannel*�����û�н�����NULL��
  Note:			  ����ÿ��ͨ�����м���ͨ��������ͻ���ͨ����û�С�
********************************************************************************/
	virtual ISocketChannel*		GetListenChannel()=0;
/********************************************************************************
  Function:       GetNetProType
  Description:    ��ȡ����Э������
  Input:  
  Output:         
  Return:         ���enumNetProtocolType
  Note:				
********************************************************************************/
	virtual enumNetProtocolType	GetNetProType()=0;
public:
/********************************************************************************
  Function:		SetUserData
  Description:	�����û����ݡ��ϲ����Ϊÿ��ͨ�����������û�����
  Input:  		pUserData: void*ָ�� �û�����
  Output:      	   
  Return:  		       
  Note:					
********************************************************************************/
	virtual void	SetUserData( void* pUserData )=0;
/********************************************************************************
  Function:		GetUserData
  Description:	�����û�����ָ��
  Input:  		
  Output:      	   
  Return:  		����У�������void*�����û�������û����ݣ�����NULL       
  Note:					
********************************************************************************/
	virtual void*	GetUserData()=0;
/********************************************************************************
  Function:		ManReConnect
  Description:	�˹�������������TCP�ͻ��˵���
  Input:  		
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣       
  Note:					
********************************************************************************/
	virtual INT		ReConnect()=0;
/********************************************************************************
  Function:		SetSendBuf
  Description:	����ϵͳsocket�ķ��ͻ�������С
  Input:  		iBufSize ������ֵ����λByte
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
  Note:			������ý���Ը�ͨ����socket����Ӱ������ͨ����socket������		
  Author:        	CHC
  Date:				2010/09/26
********************************************************************************/
	virtual	INT		SetSendBuf(INT iBufSize)=0;
/********************************************************************************
Function:		SetRcvBuf
Description:	����ϵͳsocket�Ľ��ջ�������С
Input:  		iBufSize ������ֵ����λByte
Output:      	   
Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
Note:			������ý���Ը�ͨ����socket����Ӱ������ͨ����socket������		
Author:        	CHC
Date:				2010/09/26
********************************************************************************/
	virtual	INT		SetRcvBuf(INT iBufSize)=0;
/********************************************************************************
	Function:		SetNetBlockMode
	Description:	��������ģʽ�������������
	Input:  		bMode	TRUE:������		FALSE:����
	Output:      	   
	Return:  		       
	Note:			�����Ĭ��Ϊ������ʽ. 	����Ե���ͨ�������������������ʽ	
	Author:        	CHC
	Date:				2010/09/08
********************************************************************************/
	virtual INT	SetNetBlockMode( BOOL bMode)=0;
/********************************************************************************
	Function:       SendDataEx
	Description:    �������ݡ�����⽫ֱ�ӵ���API��������
	Input:		  pData������ָ�룬	unDataLen�����ݳ���
	Output:         
	Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			  ע�⣬����ʧ��ʱ���˺������᷵�س�����API�����룬����������ж���Ĵ�����
					�˺�����SendData()�������ǲ�����select����������socket�Ƿ���д����
********************************************************************************/
	virtual INT		SendDataEx(void* pData, UINT unDataLen)=0;
/********************************************************************************
	Function:       GetSocketHandle
	Description:    ��ȡsocket��handle
	Input:		  
	Output:         
	Return:         ����handle  NULLΪ��Чֵ
	Note:			  	
********************************************************************************/
	virtual INT		GetSocketHandle(void)=0;

protected:
	ISocketChannel(void){};	
	virtual ~ISocketChannel(void){};

};

class INetService
{

public:
/********************************************************************************
  Function:		InitNetService
  Description:	��ʼ���������
  Input:  		
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣       
  Note:			��Ҫ���̣�	1����������  2�������߳�  3�����ü�����1	4��������ɶ˿ڻ�poll 
							5������cpu��Ŀ���߳�׼����������			
********************************************************************************/
	virtual INT InitNetService()=0;
/********************************************************************************
  Function:		InitSimpleNetService
  Description:	��ʼ�����������
  Input:  		
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣       
  Note:			��Ҫ���̣�	1����������  2�������߳�  3�����ü�����1	4��������ɶ˿ڻ�poll 
							5������cpu��Ŀ���߳�׼����������
				InitSimpleNetService �� InitNetService ����Ҫ������ֻ����1���߳�ȥ������ɶ˿����ݣ�
				�����߳��������ʺ���Ϊ�ͻ���ʱ��ʼ��ʹ�á�����ֻ�ܵ�������һ����
********************************************************************************/
	virtual INT InitSimpleNetService()=0;

/********************************************************************************
  Function:		AddServerChannel
  Description:	���ӷ�����ͨ��
  Input:  		pszBindIP����IP������������IP����ʽ��������"127.0.0.1"��	���Ҫʹ��Ĭ��IP������NULL��"0"��unPort���˿ڡ�
				eProtocolType��Э�����͡�
  Output:      	pSocketChnl������ָ�룬ָ��ͨ����ָ�롣   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣           
  Note:			�ϲ�Ӧ����ͨ��ָ��pSocketChnl���Ա�ͨ����ָ�������ز�����		
********************************************************************************/
	virtual INT AddServerChannel(const char* pszBindIP, UINT16 unPort, enumNetProtocolType eProtocolType, 
								ISocketChannel** pSocketChnl)=0;
/********************************************************************************
  Function:		AddClientChannel
  Description:	���ӿͻ���ͨ��
  Input:  		pszhost��������ַ�����Զ˷�������IP����ʽ��������"127.0.0.1",����Ϊ��������	unDesPort���Է��˿�
				pszBindIP����IP��������IP�����Ҫʹ��Ĭ��IP������NULL��"0"��unLocalPort�����ض˿ڡ�eProtocolType��Э�����͡�
  Output:      	pSocketChnl������ָ�룬ָ��ͨ����ָ�롣   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣           
  Note:			�ϲ�Ӧ����ͨ��ָ��pSocketChnl���Ա�ͨ����ָ�������ز�����		
********************************************************************************/
	virtual INT AddClientChannel( const char *pszHost, UINT16 unDesPort,const char *pszBindIP, 
								UINT16 unLocalPort,enumNetProtocolType eProtocolType,  ISocketChannel** pSocketChnl)=0;
/********************************************************************************
  Function:		StopNetService
  Description:	ֹͣ��������⽫�ͷŸ�����������������ͨ�������ô˺�����Ϻ��ϲ��������ͷ���������
				������ֱ�ӵ���InitNetService()������ʹ������⡣
  Input:  		
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
  Note:			��Ҫ���̣�1��ֹͣ�߳� 2���ر�����socket 3���ͷ�����ͨ�� 4���ͷ��̺߳����� 5�����ü�����һ
  6���������ü����ж��Ƿ��ͷ�������ԴWSACleanup		
********************************************************************************/
	virtual INT StopNetService()=0;
/********************************************************************************
  Function:		CloseChannel
  Description:	�ر�ͨ�������ͷ�ͨ����
  Input:  		pSocketChnl:��Ҫ�رյ�ͨ��ָ��
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
  Note:			�ر�ͨ����pSocketChnlָ�뽫ʧЧ���ϲ�Ӧע�⡣	
				����������ú�ISocketChannel::CloseChannel()��ȫ��ͬ
********************************************************************************/
	virtual INT CloseChannel(ISocketChannel* pSocketChnl)=0;
	//���µ���������Ըö������������ͨ������Ч 
/********************************************************************************
  Function:		SetReConnect
  Description:	����������־�������Ĭ��Ϊ����
  Input:  		bReConnect:ֵΪtrue��ʾ������false��������
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣       
  Note:			��������ͨ���ڲ����½���socket��ͨ��ָ��δ�ı䣬�����ɹ����ϲ���Լ���ʹ�ø�ͨ��ָ�롣
				����ʧ�ܻ�ɹ����ص�֪ͨ�ϲ㡣
********************************************************************************/
	virtual INT SetReConnect(bool bReConnect)=0;
	//�����Ƿ�֧����Ϣ����
/********************************************************************************
  Function:		SetMsgBuffFlag
  Description:	�����Ƿ�֧����Ϣ����.�����Ĭ��Ϊ֧��
  Input:  		bMsgBuffFlag:ֵΪtrue��ʾ֧�֣�false��֧�֡�
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
  Note:			Ŀǰ�������δʵ�ִ˹���		
********************************************************************************/
	virtual INT SetMsgBuffFlag(bool bMsgBuffFlag)=0;
	
/********************************************************************************
  Function:		SetActiveTime
  Description:	����ͨ������ʱ����.�����Ĭ��Ϊ10��
  Input:  		unTime:ʱ�䣬��λ��
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣           
  Note:			ͨ����������unTimeʱ��������ͨѶһ�Σ������������Ϊ��ͨ����ʱ����֪ͨ�ϲ㡣		
********************************************************************************/
	virtual INT SetActiveTime( UINT16 unTime )=0;
	
/********************************************************************************
  Function:		SetMaxChannel
  Description:	����ͨ��������.�����Ĭ��Ϊ200����
  Input:  		unCount:���ͨ����
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣           
  Note:			�������ͨ����������Զ��ر��½�������ӡ�		
********************************************************************************/
	virtual INT SetMaxChannel( UINT16 unCount )=0;
/********************************************************************************
  Function:		SetOnEventCallBack
  Description:	���ûص�����
  Input:  		pUserData:�û�����,����ΪNULL��	OnEventCallBack���ص�����ָ�롣
  Output:      	   
  Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣           
  Note:				
********************************************************************************/
	virtual INT SetOnEventCallBack(void* pUserData, pOnEventCallBack OnEventCallBack)=0;
/********************************************************************************
  Function:		SetNetBlockMode
  Description:	��������ģʽ�������������
  Input:  		bMode	TRUE:������		FALSE:����
  Output:      	   
  Return:  		       
  Note:			�����Ĭ��Ϊ������ʽ		
  Author:        	CHC
  Date:				2010/09/08
********************************************************************************/
	virtual INT	SetNetBlockMode( BOOL bMode)=0;
/********************************************************************************
	Function:		SetSendBuf
	Description:	����ϵͳsocket�ķ��ͻ�������С
	Input:  		iBufSize ������ֵ����λByte
	Output:      	   
	Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
	Note:			���óɹ��󣬶Ա�����������½�����socket��Ч������ISocketChannel��ͬ
	Author:        	CHC
	Date:				2010/09/26
********************************************************************************/
	virtual	INT		SetSendBuf(INT iBufSize)=0;
/********************************************************************************
	Function:		SetRcvBuf
	Description:	����ϵͳsocket�Ľ��ջ�������С
	Input:  		iBufSize ������ֵ����λByte
	Output:      	   
	Return:  		ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣        
	Note:			���óɹ��󣬶Ա�����������½�����socket��Ч��	����ISocketChannel��ͬ	
	Author:        	CHC
	Date:				2010/09/26
********************************************************************************/
	virtual	INT		SetRcvBuf(INT iBufSize)=0;
/********************************************************************************
  Function:		GetAllChannelNum
  Description:	��ȡ��ǰ����������е�ͨ����Ŀ
  Input:  		
  Output:      	   
  Return:  		����ͨ����Ŀ       
  Note:			�������ò��û��ʲôʵ�����ã�ֻ�����ڲ���ʱ��Ҫ��֤demo�е�ͨ����Ŀ�Ƿ�������һֱ		
********************************************************************************/
	virtual	INT	GetAllChannelNum()=0;
/********************************************************************************
	Function:       InitLog
	Description:    ��ʼ����־
	Input:			czPathName	��־·��
	Output:         
	Return:         ERROR_BASE_SUCCESS����ʾ�ɹ�������Ϊ������������붨�塣
	Note:			  	
********************************************************************************/
	virtual INT		InitLog(const char *czPathName)=0;
protected:
	INetService(void){};
public:
	virtual ~INetService(void){ };
};

/********************************************************************************
  Function:		CreateNetService
  Description:	����CNetService�����ȫ�ֺ����������ɹ��󣬱������InitNetService���г�ʼ��
  Input:  		
  Output:      	   
  Return:  		�����ɹ�����INetServiceָ�룬ʧ�ܷ���NULL��       
  Note:					
********************************************************************************/


}

EXPORT_API INetService* CreateNetService();

#endif

