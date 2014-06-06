/************************************************************
  Copyright (C), 2010-2011, GOSUN 
  FileName:     ILogLibrary.h
  Author:       sdj   
  Version:      1.0.0.0    
  Date:         2010-5-25 9:40:02
  Description:  �ṩ��һ����־�ӿڣ���������ͬʱ���ö��������ʽ��������־����
                ���ص���Ч�ʽϸߣ��̰߳�ȫ
***********************************************************/
#if !defined (ILOGLIBRARY_DEF_H)
#define ILOGLIBRARY_DEF_H
#include <stdio.h>
#include <string>
#include <iostream>

#include "GSCommon.h"
#include "GSType.h"
#include "GSDefine.h"

using namespace std;

/*��־����:�����ɸߵ��ͣ�LEMERG��ߣ�LNOTSET���*/
#define LEMERG  0x00000001
#define LFATAL  0x00000002
#define LALERT  0x00000004
#define LCRIT   0x00000008
#define LERROR  0x00000010
#define LWARN   0x00000020
#define LNOTICE 0x00000040
#define LINFO   0x00000080
#define LDEBUG  0x00000100
#define LNOTSET 0x00000200

/*��־�����ʽ������̨���ļ�*/
#define MCONSOLE   0x00000001//����̨
#define MFILE      0x00000002//�ļ�

/*�ļ�������ʽ*/
#define WREWIND  0x00000001//�ؾ�
#define WCREATE  0x00000002//�½�


/*�����붨��*/
#define ERROR_LOGLIBRARY_OPER_SUCCESS ERROR_BASE_SUCCESS 
#define ERROR_LOGLIBRARY_START (ERROR_BASE_START+4000)
#define ERROR_LOGLIBRARY_DIR_NOT_EXIST		(ERROR_LOGLIBRARY_START + 1) //·������
#define ERROR_LOGLIBRARY_FILE_NOT_OPEN		(ERROR_LOGLIBRARY_START + 2) //�ļ�û�д�
#define ERROR_LOGLIBRARY_FILESIZE_ILLEGAL	(ERROR_LOGLIBRARY_START + 3) //��С�Ƿ�
#define ERROR_LOGLIBRARY_FILEWAY_ILLEGAL	(ERROR_LOGLIBRARY_START + 4) //�ļ��������ԷǷ�
#define ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL	(ERROR_LOGLIBRARY_START + 5) //��־�ȼ��Ƿ�
#define ERROR_LOGLIBRARY_LOGMEDIUM_ILLEGAL	(ERROR_LOGLIBRARY_START + 6) //��־������ʷǷ�
#define ERROR_LOGLIBRARY_MSGSIZE_ILLEGAL	(ERROR_LOGLIBRARY_START + 7) //��־��Ϣ��С�Ƿ�
#define ERROR_LOGLIBRARY_STRING_ILLEGAL		(ERROR_LOGLIBRARY_START + 8) //�ַ�����С�Ƿ�
#define ERROR_LOGLIBRARY_MKDIR_FAIl			(ERROR_LOGLIBRARY_START + 9) //�����ļ���ʧ��
#define ERROR_LOGLIBRARY_DISK_ERROR         (ERROR_LOGLIBRARY_START + 10)//���̴���
class ILogLibrary
{

public:
	ILogLibrary(){};
	virtual ~ILogLibrary(void){};
public:
	/*************************************************
	  Function:   SetLogDir
	  DateTime:   2010-5-21 11:39:37   
	  Description: ������־·��
	  Input:    pszDir	��־�ļ��Ĵ��·���ַ��� 
				pszExtName	��־�ļ�����׺
	             
	  Output:   �� 
	  Return:   ERROR_LOGLIBRARY_OPER_SUCCESS	�����ɹ�	
				ERROR_LOGLIBRARY_DIR_NOT_EXIST	·��������
				ERROR_LOGLIBRARY_FILE_NOT_OPEN	��־�ļ�û�д�
	  Note:     ·��������þ���·��,����·������Ϸ�,������ɴ������ɹ�  
	*************************************************/
	virtual INT  SetLogDir(const char *pszDir,const char *pszExtName) = 0;

	/*************************************************
	  Function:   SetLogSize
	  DateTime:   2010-5-21 11:44:58   
	  Description:������־��С���Ƿ�ؾ� 
	  Input:   uiWay     ��־�ļ��Ƿ�ؾ�ֵΪWREWIND���ؾ�ֵΪWCREATE���½�
	           uiSize	 ��־�ļ���С��Ϊ0ʱ����־�ļ�ΪĬ�ϴ�С2MB��Ϊ������ʱ������־�ļ�������ֽ�������λΪMB��������������
	  Output:  ��
	  Return:  ERROR_LOGLIBRARY_OPER_SUCCESS		�����ɹ�
			   ERROR_LOGLIBRARY_FILESIZE_ILLEGAL	��־�ļ���С�Ƿ�
			   ERROR_LOGLIBRARY_FILEWAY_ILLEGAL		��־�ļ���ʽ�Ƿ�

	  Note:     
	*************************************************/
	virtual INT  SetLogSize(UINT uiWay,UINT uiSize) = 0;

	/*************************************************
	  Function:   SetLogLevel
	  DateTime:   2010-5-21 11:45:42   
	  Description: ������־�����ʽ������̨���ļ�������Ӧ��־����
	  Input:   uiMedium	��־�ļ������ʽ��ֵΪMCONSOLE�����������̨��ֵΪMFILE��������ļ���
			   uiLevel	��־�ļ�������ȼ�����ֵ�������ϵ LEMERG>LFATAL>LALERT>LCRIT>LERROR>LWARN
			            >LNOTICE>LINFO>LDEBUG>LNOTSET
	             
	  Output:  ��
	  Return:  ERROR_LOGLIBRARY_OPER_SUCCESS		�����ɹ�
			   ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL	��־�ȼ��Ƿ�	
			   ERROR_LOGLIBRARY_LOGMEDIUM_ILLEGAL	��־���ý��Ƿ�
	  Note:    ��������һ�������ʽ�Ķ������    
	*************************************************/
	virtual INT SetLogLevel(UINT uiMedium,UINT uiLevel) = 0;
    
	/*************************************************
	  Function:   SetLogCB
	  DateTime:   2010-5-21 11:45:58   
	  Description: ������־�ص�����
	  Input:    pCaller		�û�����
	             
	  Output:   pCaller		���û������
				pszLevel    �ȼ�
				pszPrefix	ǰ׺
				pszMsg		��־��Ϣ
	  Return:   ��
	  Note:      
	*************************************************/
	virtual void SetLogCB(void *pCaller,void (  *pfnDealCB)(void *pCaller,const char *pszLevel,const char *pszPrefix,const char *pszMsg)) = 0;

	/*************************************************
	  Function:   Log
	  DateTime:   2010-5-21 11:46:07   
	  Description: д��־
	  Input:    uiLevel		��־�ļ�������ȼ�����ֵ�������ϵ LEMERG>LFATAL>LALERT>LCRIT>LERROR>LWARN
							>LNOTICE>LINFO>LDEBUG>LNOTSET
	            pszPrefix	��־ǰ׺��ΪNULLʱ��ΪĬ��ǰ׺no���䳤�Ȳ�Ӧ����64�ֽ�
				pszFormat	��־��ʽ������printf�и�ʽ���÷���ͬ
				...			��ʽ���ж�Ӧֵ
	  Output:   ��
	  Return:   ERROR_LOGLIBRARY_OPER_SUCCESS		�����ɹ�
				ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL	��־�ȼ��Ƿ�
	  Note:      
	*************************************************/
	virtual INT  Log(UINT uiLevel,char *pszPrefix,char  *pszFormat,...) = 0;
	
};
extern ILogLibrary *GetLogInstance();
extern void ClearLogInstance(ILogLibrary *pLogLibrary);
#endif
