/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : JOURNALIF.H
Author :  zouyx
Version : 0.0.0.0
Date: 2011/9/6 13:56
Description: ������־����Ľӿ�
********************************************
*/

#ifndef _GS_H_JOURNALIF_H_
#define _GS_H_JOURNALIF_H_

#include "CMD_PROTOCOL_DEF.H"


#ifdef _WIN32

#ifdef JOU_API_EXPORT

#define JOU_API EXPORT_API

#else 

#define JOU_API IMPORT_API

#endif

#else

#define JOU_API

#endif


#define JOU_OPERATOR_ID_LEN  MAX_NAME_LEN   //����ԱID ����
#define JOU_DESCRI_LEN 128		 //�����ַ�����
#define JOU_HOSTNAME_LEN 128	 //�����IP/�� ����
#define  JOU_OPERATOR_CONTENT_LEN 256

/*
*********************************************************************
*
*@brief : ��־����
*
*********************************************************************
*/
typedef enum
{
	eJOU_TYPE_OPERATOR = 0, //������־
	eJOU_TYPE_RUNSTATUS, //������־
	eJOU_TYPE_LOGIN,	//��½��־
}EnumJournalType;

/*
*********************************************************************
*
*@brief : ������־
*
*********************************************************************
*/
typedef struct _StruJouOperation
{
	INT32 iCliPmsID;						//�ͻ���PMS ID
	INT32 iCliID;							//�ͻ���ID
	INT32 iCmdSectionID;					//����SectionID


	char OperatorID[JOU_OPERATOR_ID_LEN];  //������Ա����
	char czHostName[JOU_HOSTNAME_LEN];     //�ͻ���IP
	CmdProtocolDef::EnumClientType eClientType;			   //�������豸����
	INT32 iPmsID;                          //�������豸ƽ̨
	INT32 iDevID;				           //�������豸ID�� ����Ƿ���Ϊ����ID
	INT32 iChn;							   //�������豸ͨ���ã� ��ЧΪ-1
	CmdProtocolDef::EnumChannelType	    eChnType;		   //�豸����, ���iChn��Ч�� ��ֵҲ��Ч
	char czContent[JOU_OPERATOR_CONTENT_LEN];  //��������
	INT32 eResult; //���
	char  czFailure[JOU_DESCRI_LEN]; //ʧ��ԭ��

	
}StruJouOperation;


/*
*********************************************************************
*
*@brief : ����״̬��־
*
*********************************************************************
*/
typedef struct _StruJouRunStatus
{
	CmdProtocolDef::EnumClientType eClientType;			//�豸����
	INT32 iPmsID;						//�豸����ƽ̨
	INT32 iDevID;						//�豸ID�� ����Ƿ���Ϊ����ID	
	INT32 iChnID;						// ͨ��ID
	INT32 iChnType;						// ͨ������
	INT32 iOnline;						// ���߱�־
	char czContent[128];  //��������
	char  czDescri[JOU_DESCRI_LEN];		//����
}StruJouRunStatus;


/*
*********************************************************************
*
*@brief : ��½��־
*
*********************************************************************
*/
typedef struct _StruJouLogin
{

	char OperatorID[JOU_OPERATOR_ID_LEN];  //������Ա����	
	char czHostName[JOU_HOSTNAME_LEN];	   //��½��IP
	char czContent[128];					//��������
	INT32 eResult;						   //���
	char czDescri[JOU_DESCRI_LEN];		   //��������
	char  czFailure[JOU_DESCRI_LEN];	   //ʧ��ԭ��
}StruJouLogin;


/*
*********************************************************************
*
*@brief : ��־��Ϣ
*
*********************************************************************
*/
typedef struct _StruJournalInfo
{
	EnumJournalType eType; //��־����
	CmdProtocolDef::StruDateTime stTime;   //����ʱ��
	INT64 iSeqID; // �ڲ�ʹ��
	union
	{
		StruJouLogin stLogin;  //��½��־
		StruJouRunStatus stRunStatus; //������־
		StruJouOperation stOperation; //������־		
	}unLog;	 //��־�ľ������ݣ� ��eType ȷ��
}StruJournalInfo;


/*
*********************************************************************
*
*@brief :  ��ѯ����
*
*********************************************************************
*/
#define JOU_FUNC_NAME_LEN 16  //���ܺ�����
#define JOU_ARG_LEN  1024     //��������



typedef struct _StruQueryArgs
{
	UINT32 iCliPmsID;  //�����ѯ�Ŀͻ���������PMS
	UINT32 iCliID;	  //�����ѯ�Ŀͻ���ID
	UINT32 iCmdTag;	  //���β�ѯ�������
	void *pChn;

	UINT32 iRowStart; //ָ�����ؽ���Ŀ�ʼ�кţ� ��1 ��ʼ
	UINT32 iPageRows; //ÿҳ�������, 0 ��ָ���ɳ���ָ��Ĭ�� Ϊ 1000, ����ܳ��� 10000

	char czFuncName[JOU_FUNC_NAME_LEN]; //��ѯ���ܣ� ͳһ��Сд��ʹ���ַ�
	char czArgs[JOU_ARG_LEN]; //��ѯ������ ����ʹ��  ';' �ָ�
}StruQueryArgs;


/*
*********************************************************************
* ���ڸ�ʽ YYYY-MM-DD HH:MM:SS  �� 2011-09-17 23:23:06
*@brief : Ĭ�ϲ�ѯ���ܺ���
*
****������־
����ֵ : ID,����ʱ��,����Ա,�豸,ͨ��,�������� 

* ������־������ʱ��β�ѯ:
��ѯ�������֣�  op_tm    ������ ��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]

* ������־������Ա:
��ѯ�������֣�  op_user    ����������Ա;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)] 

* ������־���豸��:
��ѯ�������֣�  op_devname    �������豸��;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]

* ���豸ID
��ѯ�������֣�  op_devid    �������豸ID;ͨ��ID;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]

* ���豸ͨ��
��ѯ�������֣�  op_devchn    �������豸ID;ͨ��ID;ͨ������;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]

* ������
��ѯ�������֣�  op_domname    ��������ʼʱ��;����ʱ��;����; asc(������ʱ������)|[desc  (������ʱ�併��)]

* ����ID
��ѯ�������֣�  op_domid    ��������ʼʱ��;����ʱ��;��ID; asc(������ʱ������)|[desc  (������ʱ�併��)]


3. ������־������:
��ѯ�������֣�  op_multi1    ����������Ա;�豸��;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]
��ѯ�������֣�  op_multi2    ����������Ա;�豸ID;ͨ��ID;ͨ������;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]
��ѯ�������֣�  op_multi3    ����������Ա;��ʼʱ��;����ʱ��;����; asc(������ʱ������)|[desc  (������ʱ�併��)]
��ѯ�������֣�  op_multi4    ����������Ա;�豸ID;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ�併��)]
��ѯ�������֣�  op_multi5    ����������Ա;��ʼʱ��;����ʱ��;��ID; asc(������ʱ������)|[desc  (������ʱ�併��)]


****��¼��־
����ֵ : ID,ʱ��,����Ա,��¼IP,��������,���,����ԭ��

1. ��¼��־������ʱ��β�ѯ:
��ѯ�������֣�  login_tm    ������ ��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ������)]

2. ��¼��־����Ա:
��ѯ�������֣�  login_user    ����������Ա;��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ������)] 

****������־
����ֵ : ID,ʱ��,�豸����,�豸ID,����

1. ������־������ʱ��β�ѯ:
��ѯ�������֣�  run_tm    ������ ��ʼʱ��;����ʱ��; asc(������ʱ������)|[desc  (������ʱ������)]



*********************************************************************
*/

/*
*********************************************************************
*
*@brief :  ��־ģ�鷵��ֵ
*
*********************************************************************
*/
typedef enum
{
	eJOU_R_SUCCESS = 0,
	eJOU_E_UNKNOWN, 
	eJOU_E_NINIT,   //û�г�ʼ��
	eJOU_E_NJOUTYPE, //�����ڵ���־����
	eJOU_E_INVALID,  //�Ƿ�����
	eJOU_E_NFUNCTION, //��֧�ָù���
	eJOU_E_BUSY,	//ϵͳ��æ
	eJOU_E_NMEM, //ϵͳ��Դ�ѷ�

	eJOU_E_DB_MODULE, //��ʼ�����ݿ�ģ��ʧ��
	eJOU_E_DB_CONNPOOL, //��ʼ�����ݿ����ӳ�ʧ��
	eJOU_E_DB_GETCONN, //��ȡ���ݿ�����ʧ��
	eJOU_E_DB_EXESQL,		//���ݿ�ִ��SQL����
	eJOU_E_DB_ASSERT, //���ݿ�ִ�з����쳣

}EnumJouErrno;


/*
*********************************************************************
*
*@brief :  ��ѯ�������ֵ
*
*********************************************************************
*/
typedef struct _StruQueryResult
{
	UINT32 iCliPmsID;  //�����ѯ�Ŀͻ���������PMS
	UINT32 iCliID;	  //�����ѯ�Ŀͻ���ID
	UINT32 iCmdTag;	  //���β�ѯ�������
	void *pChn;

	EnumJouErrno    eResult;   //���
	//���eResultʧ�ܣ�����ֵ����
	UINT32 iTotals;		//�ܵļ�¼��
	UINT32 iRowStart;  //��ǰ�������ʼ���кţ� �� 1 ��ʼ
	UINT32 iRows;      //��ǰ���������
	INT  iResultSize;  // ������ݵ�pResultData ��С��
	void *pResultData; // �������, ���ݸ�ʽΪ XML
}StruQueryResult;


/*
 *********************************************
 Function : JouGetError
 DateTime : 2011/9/6 15:11
 Description :  ���ش�������
 Input :  
 Output : 
 Return : ���ش��������ַ���
 Note :   
 *********************************************
 */
JOU_API const char * JouGetError( EnumJouErrno eErrno);


/*
*********************************************
Function : LogModuleInit
DateTime : 2011/9/6 9:19
Description :  ��ʼ����־ģ��
Input :  czConfFilename �����ļ�����
Input :  pConnectionPoolArgs ���ӳز���������Ϊ StruConnectPoolArgs
Output : 
Return : �ο� EnumJouErrno ����
Note :   Ҫʹ����־��������Ҫ���øú���
*********************************************
*/
typedef struct _StruConnectPoolArgs
{
	void *pConnectPool; //���ӳ�
	const char *szServer;  //���ݿ����IP
	const char *szDatabase; //���ݿ���
	const char *szUser;  //�û���
	const char *szPWD;  //����
	INT  eDbaseType; //EnumDatabaseType
}StruConnectPoolArgs;

JOU_API EnumJouErrno  JouModuleInit( const char *czConfFilename,void* pConnectionPoolArgs);

/*
*********************************************
Function : LogModuleUnint
DateTime : 2011/9/6 9:19
Description :  ж������־ģ��
Input :  
Output : 
Return : ��
Note :  �ڳ����˳�ʱһ��Ҫ���ã� ��������л�������ݸ���ʧ
*********************************************
*/
JOU_API void  JouModuleUnint(void);

/*
 *********************************************
 Function : JouAdd
 DateTime : 2011/9/6 15:24
 Description :  д��־��Ϣ
 Input :  pLog Ҫд����Ϣ����
 Output : 
 Return : �ο� EnumJouErrno ����
 Note :   �����첽��ʽ�� ��Ϣ�ŵ�����󽫷���
 *********************************************
 */
JOU_API EnumJouErrno JouAdd( const StruJournalInfo *pLog);

/*
 *********************************************
 Function : JouDelete
 DateTime : 2011/9/6 15:25
 Description : ɾ����־��¼ 
 Input :  eType ��־����
 Input :  iKeyID ��־��������ID
 Input :  czFuncName ���õĹ��ܺ���, ͳһ��Сд��ʹ���ַ�
 Input :  czArgs ���ú����Ĳ���������';' �ָ�
 Output : 
 Return : �ο� EnumJouErrno ����
 Note :   
 *********************************************
 */
JOU_API EnumJouErrno  JouDelete(  EnumJournalType eType, INT64 iKeyID);
JOU_API EnumJouErrno  JouDeleteExt( const char czFuncName[JOU_FUNC_NAME_LEN],
								   const char czArgs[JOU_ARG_LEN] );


/*
 *********************************************
 Function : JouUpdate
 DateTime : 2011/9/6 15:25
 Description :  ���²�����־״̬
 Input :  eType �����µ���־����
 Input :  iKeyID ��������־��������ID 
 Input :  pNewLog �µ���־��Ϣ
 Output : 
 Return :  �ο� EnumJouErrno ����
 Note :   ��־���Ա������� ????
 *********************************************
 */
typedef struct _StruJouOperationUpdate
{
	INT32 iCliPmsID;						//�ͻ���PMS ID
	INT32 iCliID;							//�ͻ���ID
	INT32 iCmdSectionID;					//����SectionID

	INT32 eResult; //���
	char  czFailure[JOU_DESCRI_LEN]; //ʧ��ԭ��
}StruJouOperationUpdate;

JOU_API EnumJouErrno  JouUpdateOperation( const StruJouOperationUpdate *pData );



/*
 *********************************************
 Function : JouAsyncQuery
 DateTime : 2011/9/6 15:51
 Description :  ��ѯ�ӿ�
 Input :  pArgs ��ѯ�Ĳ�����
 Output : 
 Return : �ο� EnumJouErrno ����
 Note :   ʹ�õ��첽��ѯ��ʽ, ������� JouSetAsyncQueryCallback �趨�ص�����
 *********************************************
 */
JOU_API EnumJouErrno  JouAsyncQuery( const StruQueryArgs *pArgs );


/*
 *********************************************
 Function : JouFuncPtrAsyncQueryCallback
 DateTime : 2011/9/6 15:51
 Description :  ��ѯ����Ļص���������
 Input :  
 Output : pResult ���صĽ�����ο� StruQueryResult
 Output : pUserContent �û���������
 Return : ��
 Note :   
 *********************************************
 */
typedef void   JouFuncPtrAsyncQueryCallback(  const StruQueryResult *pResult,  
											void *pUserContent );
/*
 *********************************************
 Function : LogSetAsyncQueryCallback
 DateTime : 2011/9/6 15:51
 Description :  �趨�첽��ѯ�Ļص�����
 Input :  fnCallback �ص������� �ο� JouFuncPtrAsyncQueryCallback����
 Input :  pUserContent �û�����������
 Output :  
 Return : �ο� EnumJouErrno ����
 Note :   
 *********************************************
 */
JOU_API EnumJouErrno  JouSetAsyncQueryCallback( JouFuncPtrAsyncQueryCallback fnCallback, 
											   void *pUserContent );

#endif //end _GS_H_JOURNALIF_H_


