#ifndef GSP_ISOURCE_DEF_H
#define GSP_ISOURCE_DEF_H  
#include "GSPConfig.h"
#include "IMediaInfo.h"
#include "IUri.h"


/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : IGSPSOURCE.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/6/22 9:25
Description: ����Դ�ӿڶ���
********************************************
*/

namespace GSP
{




class CISource
{   
public :
	typedef enum
	{
		eMODE_PUSH = 0, //��ģʽ
		eMODE_PULL = 1, //��ģʽ
	}EnumMode;

    typedef enum
    {
        SRC_RET_SUCCESS = 0,
        SRC_RET_EUNKNOWN,  //δ֪����
        SRC_RET_EINVALID,  //��Ч����Դ
        SRC_RET_EFLOWOUT,  //д���������
        SRC_RET_EUNUSED,    //����ʹ��
    }EnumRetNO;

	typedef enum
	{
		eEVT_RELEASE = 0, //�ͷ�, ����ֵ��ʹ��
		eEVT_REQUEST_FRAME,  //�������ݣ� ����ֵ Ϊ INT ����
							 //  < 0 ��ʾֹͣ���������ݣ� > 0 ��ʾ �´ε��õļ��
							 // 0 ��ʾ ���������ݣ� 
							 // * ��Ҫ�ٻص���ʹ����ѭ����������,���������ϵ���������뷵��0

	}EnumEvent;

typedef void *(*FuncPtrISourceEventCallback)(CISource *pSource, EnumEvent eEvt, void *pUserArgs);


	virtual CISource::EnumMode GetMode(void) const = 0;

	virtual void SetEventCallback(CISource::FuncPtrISourceEventCallback fnCallback, void *pUserArgs) = 0;

	virtual BOOL EnablePullEvent( BOOL bStart, INT iMSecs ) = 0;


	/*
	 *********************************************
	 Function : RefObject
	 DateTime : 2012/4/24 10:39
	 Description :  ���Ӷ�������
	 Input :  
	 Output : 
	 Return : ��������Դ
	 Note :   
	 *********************************************
	 */
	virtual CISource *RefObject(void) = 0;

	/*
	 *********************************************
	 Function : UnrefObject
	 DateTime : 2012/4/24 10:39
	 Description :  ���ٶ���Ӧ��
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	virtual void UnrefObject(void) = 0;

	/*
	 *********************************************
	 Function : GetSrcRefs
	 DateTime : 2012/4/24 10:40
	 Description :  ��������Դ�����õĸ���
	 Input :  
	 Output : 
	 Return : 
	 Note :   ����ͬ�Զ������������ ��ʾ���Ǹ�����Դ�����ٿͻ���ʹ��
	 *********************************************
	 */
    virtual UINT GetSrcRefs(void) = 0; 

	/*
	 *********************************************
	 Function : GetAutoID
	 DateTime : 2012/4/24 10:42
	 Description :  ������������ID, ����Ψһ��
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
     virtual UINT32 GetAutoID(void) = 0;

	/*
	 *********************************************
	 Function : GetSourceID
	 DateTime : 2012/4/24 10:42
	 Description :  ��������Դ��ID�� ���յ�ID �����ظ�ʹ��
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
     virtual UINT16 GetSourceID(void) = 0;

	 /*
	  *********************************************
	  Function : SetMediaInfo
	  DateTime : 2012/4/24 10:43
	  Description :  �������ý������
	  Input :  iChn ͨ����
	  Input :  pInfo ý������
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void SetMediaInfo( UINT iChn,const StruGSMediaDescri *pInfo ) = 0;

	 /*
	  *********************************************
	  Function : SetCtrlAbilities
	  DateTime : 2012/4/24 10:44
	  Description :  ���ÿ����ṩ���ƵĹ�������
	  Input :  iAbilities ��������, �ο� : <<GSPStru.h>>  GSP ������������ 
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void SetCtrlAbilities(UINT32 iAbilities) = 0;

	 /*
	  *********************************************
	  Function : SourceEnableRef
	  DateTime : 2012/4/24 10:45
	  Description :  ��������Դ�ͷ�ͬʱ���Ա�����ʹ��
	  Input :  bEnable ʹ��
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void SourceEnableRef(BOOL bEnable ) = 0;

	/*
	 *********************************************
	 Function : SourceEnableValid
	 DateTime : 2012/4/24 10:46
	 Description :  ��������Դ�Ƿ���Ч
	 Input :  bEnable ��Ч��
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
     virtual void SourceEnableValid( BOOL bEnable) = 0;


	 /*
	  *********************************************
	  Function : MakeURI
	  DateTime : 2012/4/24 10:47
	  Description :  ��������Դ��URI
	  Input :  czPro ʹ�õ�Э��
	  Output : 
	  Return : ʧ�ܷ���NULL�� ���صĶ�����Ҫ��delete �ͷ�
	  Note :   
	  *********************************************
	  */
     virtual CIUri *MakeURI( const char *czPro = "gsp", const char *pRemoteIP=NULL ) = 0;


	 /*
	  *********************************************
	  Function : SetPlayStatus
	  DateTime : 2012/4/24 10:48
	  Description :  ���ò���״̬
	  Input :  pStatus ����״̬
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void SetPlayStatus( const StruPlayStatus *pStatus )=0;

	 /*
	  *********************************************
	  Function : ReplayEnd
	  DateTime : 2012/4/24 10:48
	  Description :  �طŻ����ط�ʽʱ�����ļ�����
	  Input :  
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void ReplayEnd(void) = 0;


	/*
	 *********************************************
	 Function : WriteData
	 DateTime : 2012/4/24 10:50
	 Description :  ��������
	 Input :  pData ����
	 Input : iLen  pData���ݳ���
	 Input : iChn ��������ͨ��
	 Input : bKey  �Ƿ��ǹؼ�����, �ñ�׼��������, ���� I֡�� ��Ƶ��ϵͳͷ ��ΪTRUE
	 Output : 
	 Return : EnumRetNO �ο����� 
	 Note :   
	 *********************************************
	 */
     virtual CISource::EnumRetNO WriteData( const void *pData, INT iLen, UINT iChn, BOOL bKey) = 0; 

	/*
	 *********************************************
	 Function : WriteSysHeader
	 DateTime : 2012/4/24 10:52
	 Description :  дϵͳͷ
	 Input :  pData ����
	 Input : iLen  pData���ݳ���
	 Input : iChn ��������ͨ��
	 Output : 
	 Return : EnumRetNO �ο����� 
	 Note :   
	 *********************************************
	 */
     virtual CISource::EnumRetNO WriteSysHeader( const void *pData, INT iLen, UINT iChn) = 0;

	/*
	 *********************************************
	 Function : WriteDataV
	 DateTime : 2012/4/24 10:53
	 Description :  �������ݣ� ���ܶ�Ӧ�� WriteData
	 Input :  pIOV ���͵���������
	 Input :  iVNums ָ�� pIOV �ĸ���
	 Input : iChn ��������ͨ��
	 Input : bKey  �Ƿ��ǹؼ�����, �ñ�׼��������, ���� I֡�� ��Ƶ��ϵͳͷ ��ΪTRUE
	 Output : 
	 Return : 
	 Note :   һ��WriteDataV ���ö�� StruIOV�� �ϲ�Ϊһ֡���ݷ���
	 *********************************************
	 */
     virtual CISource::EnumRetNO WriteDataV( const StruIOV *pIOV, INT iVNums, UINT iChn, BOOL bKey) = 0; 
	 /*
	 *********************************************
	 Function : WriteDataV
	 DateTime : 2012/4/24 10:53
	 Description :  дϵͳͷ�� ���ܶ�Ӧ�� WriteSysHeader
	 Input :  pIOV ���͵���������
	 Input :  iVNums ָ�� pIOV �ĸ���
	 Input : iChn ��������ͨ��	
	 Output : 
	 Return : 
	 Note :   һ��WriteDataV ���ö�� StruIOV�� �ϲ�Ϊһ��ϵͳͷ����
	 *********************************************
	 */
     virtual CISource::EnumRetNO WriteSysHeaderV( const StruIOV *pIOV, INT iVNums, UINT iChn) = 0;


     /*
     *********************************************
     Function :SetUserData
     DateTime : 2010/8/6 8:56
     Description :�����û�����
     Input : pData �û�������
     Output :
     Return :
     Note : ֻ����ָ��Ĵ洢,�����Ҫ�ͷŲ����� ���û����д���
     *********************************************
     */
     virtual void SetUserData(void *pData) = 0;

     /*
     *********************************************
     Function : GetUserData
     DateTime : 2010/8/6 8:57
     Description : �����û�����
     Input :
     Output : 
     Return :  ����  SetUserData �趨��ֵ 
     Note : ���û�е��ù� SetUserData�� Ĭ��ֵΪNULL
     *********************************************
     */
     virtual void *GetUserData(void ) = 0;

	 /*
	  *********************************************
	  Function : Release
	  DateTime : 2012/4/24 10:56
	  Description :  �ͷŶ���
	  Input :  
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
     virtual void Release(void) = 0;


	 /*
	  *********************************************
	  Function : GetKey
	  DateTime : 2012/4/24 10:57
	  Description :  ���ع�������Դ�Ǵ��ݵ�����ԴKEY
				ָ CIServer::AddSource ָ����key ֵ
	  Input :  
	  Output : 
	  Return : 
	  Note :   
	  *********************************************
	  */
      virtual const char *GetKey(void) = 0;

protected :
	virtual ~CISource()
	{
	}
     
    
};


};


#endif
