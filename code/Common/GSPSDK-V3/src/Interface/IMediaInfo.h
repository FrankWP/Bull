#ifndef GSS_IMEDIAINFO_DEF_H
#define GSS_IMEDIAINFO_DEF_H
/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : MEDIAINFO.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/6/29 15:06
Description: ��ý�����ͽ�����������ʽ��
********************************************
*/
#include <GSMediaDefs.h>
#include "GSPConfig.h"



namespace GSP
{

/*
*********************************************
ClassName :  CIMediaInfo
DateTime : 2010/8/6 10:37
Description : ý����Ϣ�࣬ Ϊ����ͽṹ��ý�������Ϣ�ṩ���ӿ�
Note :
*********************************************
*/


class CIMediaInfo   
{   
public :
	static const INT INVALID_CHANNEL_ID = -1; //��Чͨ����
    typedef struct _StruMediaChannelInfo
    {
        INT iSubChannel; //ͨ����
        StruGSMediaDescri stDescri; //ý������
    }StruMediaChannelInfo;

	/*
	 *********************************************
	 Function : GetSubChannel
	 DateTime : 2012/4/24 10:36
	 Description :  ����ͨ���Ż�ȡý����Ϣ
	 Input :  iSubChannel ͨ����
	 Output : 
	 Return :  ���ͨ�������ڷ���NULL
	 Note :   
	 *********************************************
	 */
	virtual const CIMediaInfo::StruMediaChannelInfo *GetSubChannel( INT iSubChannel ) const = 0;

	/*
	*********************************************
	Function : GetChannel
	DateTime : 2012/4/24 10:36
	Description :  ���������Ż�ȡý����Ϣ
	Input :  iIndex ������
	Output : 
	Return :  ��������Ŷ�Ӧͨ�������ڷ���NULL
	Note :   
	*********************************************
	*/
    virtual const CIMediaInfo::StruMediaChannelInfo *GetChannel( UINT16 iIndex ) const = 0;

	/*
	*********************************************
	Function : GetChannel
	DateTime : 2012/4/24 10:36
	Description :  ���������Ż�ȡý����Ϣ
	Input :  iIndex ������
	Output : ppResult ���ص�ý����Ϣ
	Return :  TRUE/FALSE
	Note :   
	*********************************************
	*/
	virtual BOOL GetChannel(UINT16 iIndex, StruGSMediaDescri *&ppResult, 
							INT &iSize) const = 0;

	/*
	 *********************************************
	 Function : GetChannelNums
	 DateTime : 2012/4/24 10:38
	 Description :  �����ܵ�ͨ����
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
    virtual UINT16 GetChannelNums(void) const = 0;


	/*
	*********************************************
	Function :  GetMediaType
	DateTime : 2010/8/6 10:41
	Description : ����ͨ����ý����Ϣ����
	Input :
	Output :
	Return :   ����ͨ����ý����Ϣ����, �ο�EnumGSMediaType���� , ���ͨ�������ڷ��� GS_MEDIA_TYPE_NONE
	Note :
	*********************************************
	*/
	virtual EnumGSMediaType GetMediaType( UINT iIndex) const= 0;

	virtual ~CIMediaInfo(void)
	{

	} 

};

} // GSP

#endif
