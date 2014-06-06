/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : IPROCHANNEL.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/28 11:09
Description: Э��ͨ���ӿڶ���
********************************************
*/

#ifndef _GS_H_IPROCHANNEL_H_
#define _GS_H_IPROCHANNEL_H_

#include "GSPObject.h"
#include "Uri.h"

namespace GSP
{

	
	/*
	********************************************************************
	��ע��
	����    :    CIProChannel
	����    :    zouyx
	����ʱ��:    2012/4/24 10:22
	������  :		�ͻ���Э��ͨ���Ľӿڶ���
	*********************************************************************
	*/
	
class CIProChannel :
	public CGSPObject
{
public:
	CIProChannel(void);
	virtual ~CIProChannel(void);

	/*
	 *********************************************
	 Function : Open
	 DateTime : 2012/4/24 10:22
	 Description :  ��URI
	 Input :  csUri  �򿪵�URI
	 Input : bAsync ���첽
	 Input : iTimeouts ��ʱʱ�䣬 ����
	 Output : 
	 Return : �ο�EnumErrno����
	 Note :   
	 *********************************************
	 */
	virtual EnumErrno Open(const CUri &csUri, BOOL bAsync, INT iTimeouts) = 0;

	/*
	*********************************************
	Function : Ctrl
	DateTime : 2012/4/24 10:22
	Description :  ����ͨ��
	Input :  stCtrl  ���Ƶ�����
	Input : bAsync ���첽
	Input : iTimeouts ��ʱʱ�䣬 ����
	Output : 
	Return : �ο�EnumErrno����
	Note :   
	*********************************************
	*/
	virtual EnumErrno Ctrl(const StruGSPCmdCtrl &stCtrl, BOOL bAsync,INT iTimeouts) = 0;

	virtual EnumErrno CtrlOfManstrsp(const char *czMansrtsp, BOOL bAsync,INT iTimeouts,StruGSPCmdCtrl &stGspCtrl)
	{
		return eERRNO_SYS_EFUNC;
	}

	/*
	 *********************************************
	 Function : FlowCtrl
	 DateTime : 2012/4/24 10:24
	 Description :  ��������
	 Input :  bStart ��ʼ�����
	 Output : 
	 Return : �ο�EnumErrno����
	 Note :   
	 *********************************************
	 */
	virtual EnumErrno FlowCtrl( BOOL bStart ) = 0;



	/*
	 *********************************************
	 Function : DestoryBefore
	 DateTime : 2012/4/24 10:24
	 Description :  �����ͷ�ǰ����,�����ͷŶ�����Դ
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	virtual void DestoryBefore(void) = 0;


	virtual CGSString GetSdp(void)
	{
		return CGSString();
	}
};

} //end namespace GSP

#endif //end _GS_H_IPROCHANNEL_H_
