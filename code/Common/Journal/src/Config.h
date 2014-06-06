/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : CONFIG.H
Author :  zouyx
Version : 0.0.0.0
Date: 2011/9/8 15:47
Description: 
********************************************
*/

#ifndef _GS_H_CONFIG_H_
#define _GS_H_CONFIG_H_
#include "JouObj.h"



namespace JOU
{

class CConfig :
	public CJouObj
{
public:
	CConfig(void);
	virtual ~CConfig(void);

	void LoadConfig( const char *czConfFilename);


	CGSString m_strIniFilename; //�����ļ���

	//��־�������
	UINT m_iLogLevelConsole;   //��־��Ļ���� Ĭ�� ���д�
	UINT m_iLogLevelFile;    //��¼��־�ļ�� Ĭ�� LV_FATAL, LV_ERROR, LV_WARN, LV_DEBUG
	CGSString m_strLogDir; //��־Ŀ¼, Ĭ��Ϊ ./JouLog


	


	//��־���ݿ�����
	CGSString m_strDBHostname; //���ݿ������IP
	CGSString m_strDBName;     //���ݿ���
	CGSString m_strDBUser;     //�û���
	CGSString m_strDBPWD;		 //����
	INT    m_eDbaseType;		//���ݿ�����


	//��־��������
	
	//������־ ɾ����ʽ
	UINT32 m_iRcdOperDays;   //���������죬 0 ����Ч, Ĭ�� 240
//	UINT32 m_iRcdOperRows;   //������������ 0 ����Ч, Ĭ�� 5,000,000

	//������־ ɾ����ʽ
	UINT32 m_iRcdStatusDays;   //���������죬 0 ����Ч, Ĭ�� 120
//	UINT32 m_iRcdStatusRows;   //������������ 0 ����Ч, Ĭ�� 1,000,000

	//��½��־ ɾ����ʽ
	UINT32 m_iRcdLoginDays;   //���������죬 0 ����Ч, Ĭ�� 240
//	UINT32 m_iRcdLoginRows;   //������������ 0 ����Ч, Ĭ�� 5,000,000

	//���ػ������ݴ洢Ŀ¼
	CGSString m_strCachePath;  //Ĭ��Ϊ , Ĭ��Ϊ ./JouData
	//�Ƿ�ʹ�û��湦��
	INT m_bEnableCache;

	INT m_iCacheInterval; //�����ʱ����, Ĭ�� 20 ��


};

} //end namespace GSS

#endif //end _GS_H_CONFIG_H_
