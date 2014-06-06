/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : IURI.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/13 15:46
Description:  URI �����ӿ�

* scheme://host:port/path/filename?attrname&attrname1=attrvalue1

* scheme = ͨ��Э�� (���õ�http,ftp,maito ��)
* host = ���� (������IP)
* port = �˿ں�(��ѡ)
* path = ·��
* filename = ����
* attrname ������
* attrname1 ����1��
* attrvalue1 ����1��ֵ

* host:port ΪNetInfo
* path/filename ΪKey
********************************************
*/

#ifndef _GS_H_IURI_H_
#define _GS_H_IURI_H_




#include "GSPConfig.h"


namespace GSP
{

// URI ���� Attributive
#define MAX_URI_ATTRI_NAME_LEN 32
#define MAX_URI_ATTRI_VALUE_LEN 1024
typedef struct _StruUriAttr
{
	char szName[MAX_URI_ATTRI_NAME_LEN];
	char szValue[MAX_URI_ATTRI_VALUE_LEN];
}StruUriAttr;


/*
********************************************************************
��ע��
����    :    CIUri 
����    :    zouyx
����ʱ��:    2012/4/24 11:01
������  :    URI ����������ӿ�
*********************************************************************
*/

class CIUri
{    
public:
    virtual ~CIUri(void)
	{

	}

	/*
	 *********************************************
	 Function : Clear
	 DateTime : 2012/4/24 11:01
	 Description :  �����������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
    virtual void Clear(void) = 0;

	/*
	 *********************************************
	 Function : GetScheme
	 DateTime : 2012/4/24 11:01
	 Description :  ���� URI �� Scheme ��
	 Input :  
	 Output : 
	 Return : ʧ�ܷ���NULL
	 Note :   
	 *********************************************
	 */
    virtual const char *GetScheme(void) const = 0;

	/*
	 *********************************************
	 Function : SetScheme
	 DateTime : 2012/4/24 11:02
	 Description :  ���� URI �� Scheme ��
	 Input :  szScheme ���õ�ֵ
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
    virtual void  SetScheme(const char *szScheme) = 0;

	/*
	*********************************************
	Function : GetHost
	DateTime : 2012/4/24 11:01
	Description :  ���� URI �� Host ��
	Input :  
	Output : 
	Return : ʧ�ܷ���NULL
	Note :   
	*********************************************
	*/
    virtual const char *GetHost(void) const = 0;

	/*
	*********************************************
	Function : SetHost
	DateTime : 2012/4/24 11:02
	Description :  ���� URI �� Host ��
	Input :  szHost ���õ�ֵ
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
    virtual void  SetHost( const char *szHost ) = 0;



	/*
	*********************************************
	Function : GetHost
	DateTime : 2012/4/24 11:01
	Description :  ���� URI �� Port ��
	Input :  
	Output : 
	Return : Ĭ��ֵΪ0
	Note :   
	*********************************************
	*/
    virtual UINT GetPort(void) const = 0;

	/*
	*********************************************
	Function : SetPortArgs
	DateTime : 2012/4/24 11:02
	Description :  ���� URI �� Host ��
	Input :  iPort ���õ�ֵ
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
    virtual void SetPortArgs(UINT iPort) = 0;
  
	/*
	*********************************************
	Function : GetKey
	DateTime : 2012/4/24 11:01
	Description :  ���� URI �� Key ��
	Input :  
	Output : 
	Return : ʧ�ܷ���NULL
	Note :   
	*********************************************
	*/
    virtual const char *GetKey(void) const = 0;

	/*
	*********************************************
	Function : SetKey
	DateTime : 2012/4/24 11:02
	Description :  ���� URI �� Key ��
	Input :  szKey ���õ�ֵ
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
    virtual void SetKey(const char *szKey) = 0;

	/*
	*********************************************
	Function : GetAttr
	DateTime : 2012/4/24 11:01
	Description : ����������
	Input :  szAttrName ���ҵ�������
	Output : 
	Return : ʧ�ܷ���NULL, ����ж��ͬ�����Է��� ����һ��
	Note :   
	*********************************************
	*/
    virtual StruUriAttr *GetAttr(const char *szAttrName) const = 0;

	/*
	*********************************************
	Function : AttrBegin
	DateTime : 2012/4/24 11:01
	Description : ���ص�һ��������
	Input :  
	Output : 
	Return : ʧ�ܷ���NULL
	Note :   
	*********************************************
	*/
	virtual StruUriAttr *AttrBegin(void)= 0;

	/*
	 *********************************************
	 Function : AttrNext
	 DateTime : 2012/4/24 11:09
	 Description :  ������Ե���һ������
	 Input :  pAttr ��ǰ����
	 Output : 
	 Return : ��β����NULL
	 Note :   
	 *********************************************
	 */
	virtual StruUriAttr *AttrNext(const StruUriAttr *pAttr) = 0;



	/*
	 *********************************************
	 Function : AddAttr
	 DateTime : 2012/4/24 11:07
	 Description :  ��������� ͬһ�������������Ӷ��
	 Input :  szAttrName ������
	 Input : szAttrValue ����ֵ
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	virtual BOOL AddAttr( const char *szAttrName, const char *szAttrValue ) = 0;

	/*
	 *********************************************
	 Function : ClearAttr
	 DateTime : 2012/4/24 11:14
	 Description :  ������� 
	 Input :  szName ��������Ե��������� ͬ�����������Զ��ᱻ���
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	virtual void ClearAttr( const char *szName) = 0; 

	/*
	 *********************************************
	 Function : Analyse
	 DateTime : 2012/4/24 11:16
	 Description :  ����URI
	 Input :  szURI ������URI �ַ���
	 Output : 
	 Return : TRUE/FLASE
	 Note :   
	 *********************************************
	 */
    virtual BOOL Analyse(const char *szURI ) = 0;

	/*
	 *********************************************
	 Function : GetURI
	 DateTime : 2012/4/24 11:17
	 Description :  ����URI �ַ���
	 Input :  
	 Output : 
	 Return : ʧ�ܷ���NULL
	 Note :   
	 *********************************************
	 */
    virtual const char *GetURI(void) const = 0;

	/*
	 *********************************************
	 Function : Clone
	 DateTime : 2012/4/24 11:17
	 Description :  ������ǰ����
	 Input :  
	 Output : 
	 Return : ʧ�ܷ���NULL
	 Note :   
	 *********************************************
	 */
	virtual CIUri *Clone(void) const = 0;
};


};

#endif //end _GS_H_IURI_H_
