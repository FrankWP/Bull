#ifndef  TABLEDATATYPE_H_
#define  TABLEDATATYPE_H_
/********************************************************************
    Copyright (C), 2010-2011, GOSUN 
    File name 	: TableDataType.h      
    Author 		: sdj      
    Version 	: V1.00        
    DateTime 	: 2010/21/7  16:11
    Description : ��ͷ�ļ���Ҫ�Ǹ������ݿ�ı�ṹ������Ӧ���ݽṹ������PMS���ݿ�ķ��ʼ����ݽṹ�Ķ��壬 ����֮�⻹
				  ����PMS��Excel����ת������ʹ��.��������豸��Ϣ��������Ϣ���Լ��豸���������Ϣ��

				  ----�豸����Ϣ��Ҫ�����豸������Ϣ���豸ͨ����Ϣ���豸�澯��Ϣ���豸�ͺ���Ϣ�ȼ������档

				  ----�������Ϣ

				  ----�豸���������Ϣ
				  
				  PMS�ڲ���ģ���Լ�����������Ҫ��Ϣ���в���ģ����Զ�����ʽ������ͬ����������
*********************************************************************/
#include "GSType.h"

#define TB_MAX_NAME_LEN 128
#define TB_MAX_PWD_LEN 128
#define TB_MAX_CONN_LEN 128
#define TB_MAX_REMARKS_LEN 256
#define TB_MAX_VERSION_LEN 32

/************************************************************************/
/*                         �����ṹ                                     */
/************************************************************************/

//�����ṹ

//��Ԫ���豸���ͻ��ˣ��Լ����񣩻�����Ϣ
typedef struct StruBaseInfo
{
	INT32 iID;//��ԪID
	INT32 iModelID;//��Ԫ�ͺ�ID
	char szVersion[TB_MAX_VERSION_LEN];//��Ԫ�汾
	
}StruBaseInfo;

typedef struct StruDevChn
{
	INT32 iDevID;//�豸ID
	INT32 iChnID;//ͨ��ID
}StruDevChn;
//��Ԫ����
typedef struct StruDescri
{	
	char szName[TB_MAX_NAME_LEN];//�豸����
	char szRemarks[TB_MAX_REMARKS_LEN];//��ע
}StruDescri;

//�豸��½��Ϣ
typedef struct StruDevLoginInfo 
{
	char szDevConn[TB_MAX_CONN_LEN];//�豸���ӷ�ʽ
	char szLoginName[TB_MAX_NAME_LEN];//��¼��
	char szLoginPWD[TB_MAX_PWD_LEN];// ��½����
}StruDevLoginInfo;

//��չ����
typedef struct StruMemBlock 
{
	INT32 iLen;
	void *pBlock;
}StruMemBlock;

//�豸����չ��Ϣ�������õ�
typedef struct StruDevExInfo 
{
	StruMemBlock stParam1;//��չ����1
	StruMemBlock stParam2;//��չ����2
	StruMemBlock stParam3;//��չ����3

}StruDevExInfo;

//��Ԫ����Ϣ
typedef struct StruDomain
{
	INT32 iDomain;//������
	
}StruDomain;

//������Ϣ
typedef struct StruTypeInfo 
{
	INT32 iTypeID;
	char szTypeName[TB_MAX_NAME_LEN];
}StruTypeInfo;

typedef struct StruTBTypeInfo
{
	INT32 iKey;
	StruTypeInfo stTypeInfo;
}StruTBTypeInfo;
/************************************************************************/
/*            ����Excel����ת������                                     */
/************************************************************************/

/////////////////�豸������Ϣ/////////////////////////////////////////////////////////////


//�豸������Ϣ
typedef struct StruBaseDevInfo 
{
	StruBaseInfo stBaseInfo;//������Ϣ
	StruDomain stDomain;//����Ϣ
	StruDevLoginInfo stDevLoginInfo;//��½��Ϣ
	StruDescri stDescri;//����	
}StruBaseDevInfo;

typedef struct StruDevInfo 
{
	INT32 iKey;//������ID
    StruBaseDevInfo stBaseDevInfo;
	StruDevExInfo stDevExInfo;//��չ����
}StruDevInfo;

//�豸�ͺ�
typedef struct StruBaseDevModel
{
	StruTypeInfo stModel;//�豸�ͺ���Ϣ
	INT32 iVendorID;//�豸����ID
	INT32 iTypeID;//�豸����ID

}StruBaseDevModel;

typedef struct StruDevModel
{
	INT32 iKey;//������ID
	StruBaseDevModel stBaseDevModel;
}StruDevModel;

//�豸����
typedef StruTBTypeInfo StruDevType;

//�豸����
typedef StruTBTypeInfo StruDevVendor;




///////////////////�豸�澯��Ϣ////////////////////////////////////////////////////////////////////////////////
//�豸�澯����
typedef StruTBTypeInfo StruAlarmType ;

//�豸�澯�ȼ�
typedef StruTBTypeInfo StruAlarmLevel;


//�豸�澯��Ϣ
typedef struct StruBaseAlarmInfo 
{
	StruDevChn stDevChn;//�豸��Ӧͨ��,ͨ��ID���ݶ�-1ʱ��ʾ���豸ͨ���޹صĸ澯�������豸���ϸ澯
	INT32 iAlarmTypeID;//�澯����ID
	INT32 iAlarmLevelID;//�澯�ȼ�ID
	char szAlarmRemark[256];//��ע
}StruBaseAlarmInfo;

typedef struct StruAlarmInfo 
{
	INT32 iKey;//������ID
    StruBaseAlarmInfo stBaseAlarmInfo;
}StruAlarmInfo;


///////////////////�豸ͨ����Ϣ////////////////////////////////////////////////////////////////////////////////

//�豸ͨ����Ϣ
typedef struct StruBaseChnInfo 
{
	StruDevChn stDevChn;//�豸��Ӧͨ��
	INT32 iChnTypeID;//�豸ͨ������ID
	StruDescri stChn;//����
}StruBaseChnInfo;

typedef struct StruChnInfo 
{
	INT32 iKey;//������ID
	StruBaseChnInfo stBaseChnInfo;
}StruChnInfo;

//�豸ͨ������
typedef StruTBTypeInfo StruChnType;


///////////������Ϣ///////////////////////////////////////////////////////////////////////////////////////

//������Ϣ
typedef struct StruBaseServInfo
{
	StruBaseInfo stBaseInfo;//������Ϣ
	StruDomain stDomain;//����Ϣ
	StruDescri stDescri;//����
	StruMemBlock stAttri;//����
}StruBaseServInfo;

typedef struct StruServInfo
{
	INT32 iKey;//������ID
	StruBaseServInfo stBaseServInfo;
}StruServInfo;
//��������
typedef StruTBTypeInfo StruServType;



//////////////////�����豸����////////////////////////////////

//���������Ϣ
typedef struct StruBaseSerDev
{
	INT32 iSerID;//����ID
	INT32 iDevID;//�豸ID
}StruBaseSerDev;

typedef struct StruSerDev
{
	INT32 iKey;//������ID
	StruBaseSerDev stBaseSerDev;
}StruSerDev;


/************************************************************************/
/*                                 ����CLI��PMS                          */
/************************************************************************/

//////////////////////////�豸��Ϣ/////////////////

//������Ϣ
typedef StruBaseDevInfo StruBaseDevItem;


//�豸�ͺ���Ϣ
typedef StruBaseDevModel StruDevModelItem;

//�豸����
typedef StruTypeInfo StruDevTypeItem;

//�豸����
typedef StruTypeInfo StruDevVendorItem;



///////////////////�豸�澯��Ϣ////////////////////////////////////////////////////////////////////////////////
//�豸�澯����
typedef StruTypeInfo StruAlarmTypeItem ;

//�豸�澯�ȼ�
typedef StruTypeInfo StruAlarmLevelItem;



//�豸�澯��Ϣ
typedef StruBaseAlarmInfo StruAlarmInfoItem;


///////////////////�豸ͨ����Ϣ////////////////////////////////////////////////////////////////////////////////


//�豸ͨ������
typedef StruTypeInfo StruChnTypeItem;

//�豸ͨ����Ϣ
typedef StruBaseChnInfo StruChnInfoItem;

///////////������Ϣ///////////////////////////////////////////////////////////////////////////////////////

//������Ϣ

typedef StruBaseServInfo StruBaseServItem;//������Ϣ

//��������
typedef StruTypeInfo StruServTypeItem;



//////////////////�����豸����////////////////////////////////

//���������Ϣ

typedef StruBaseSerDev StruSerDevItem;



/************************************************************************/
/*				   ����DAS��PMS                                         */
/************************************************************************/
typedef struct StruDASDevItem
{
	StruBaseInfo stBaseInfo;//������Ϣ
	StruDevLoginInfo stDevLoginInfo;//��½��Ϣ
}StruDASDevItem;



/************************************************************************/
/*         ����PMS�豸����ģ�飨PMS�ڲ�ʹ�ã�                                          */
/************************************************************************/

////////////////////////������Ϣ//////////////////////////////////
typedef struct StruPMSDevItem
{
	StruBaseDevItem stBaseDevItem;//������Ϣ
	INT32 iOnlineSta;//����״̬,0--������,1--����
	
}StruPMSDevItem;
//������Ϣ
typedef struct StruPMSServItem
{
	StruBaseServItem stBaseServItem;
	void * pNetChn;//����ͨ��ָ��,��ͨ��������ʱֵΪNULL,����ʱֵΪ��NULL
	INT32 iOnlineSta;//����״̬,0--������,1--����
}StruPMSServItem;

typedef		struct		StruDomainInfoItem 
{
	INT32					iDomainID;							//	��ID
	char					szDomainName[TB_MAX_NAME_LEN];		//	������
	INT32					iParentDomainID;					//	����ID
	INT32					iPlatfromID;						//  ����ƽ̨ID,��ʼֵΪ0
	INT32					iParentPlatfromID;					//	�ϼ�ƽ̨ID,û���ϼ�ƽ̨Ϊ0
}StruDomainInfoItem;

//MapSerDev MapDASDev,MapSTSDev,MapCSSDev,MapCli; //Ϊÿ�ַ��񶼶���һ��������map���豸ID��Ϊkey

#endif
