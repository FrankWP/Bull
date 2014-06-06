#ifndef GSS_MEDIAINFO_DEF_H
#define GSS_MEDIAINFO_DEF_H
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

#include "GSPObject.h"
#include "IMediaInfo.h"



namespace GSP
{

	//����װ����
	typedef enum
	{
		eSTREAM_PKG_NONE = 0,   //�޻�δ֪
		eSTREAM_PKG_Standard = 1, //��׼����
		eSTREAM_PKG_GSPS, //��׼ GXX PS ��
		eSTREAM_PKG_28181PS, //��׼ 28181 PS ��
		eSTREAM_PKG_GS461C, //gosun  461C
		eSTREAM_PKG_GS2160I, //gosun  2160I
		eSTREAM_PKG_GS2160IV, //gosun  2160I-V
		eSTREAM_PKG_HuangHe,  //�ƺ� 
		eSTREAM_PKG_DaHua, //��
		eSTREAM_PKG_HengYi, //����
		eSTREAM_PKG_HiKVS, //����
		eSTREAM_PKG_ZBeng, //�б�
		eSTREAM_PKG_CLS, //�߿���
		eSTREAM_PKG_ALRS, //��������
		eSTREAM_PKG_XM, //Ѷ��
		eSTREAM_PKG_Hi, //��ʨ
		eSTREAM_PKG_CALSYSGS76XX, //�߿���GS76xxϵ��	
		eSTREAM_PKG_GSC3MVIDEO, // ������ �� StruGSFrameHeader ͷ
		eSTREAM_PKG_GSIPC, //��������Ƶ����
	}EnumStreamPackingType;


/*
*********************************************
ClassName :  CMediaInfo
DateTime : 2010/8/6 10:37
Description : ý����Ϣ�࣬ Ϊ����ͽṹ��ý�������Ϣ�ṩ���ӿ�
Note :
*********************************************
*/
class  CMediaInfo :
    public CGSPObject,public CIMediaInfo
{
private :
	CIMediaInfo::StruMediaChannelInfo m_vChannels[GSP_MAX_MEDIA_CHANNELS];
	UINT m_iChannels;
public:
    CMediaInfo(void);
    CMediaInfo(const CIMediaInfo &csBase );
    virtual ~CMediaInfo(void);

	//�ӿ�====
	virtual const CIMediaInfo::StruMediaChannelInfo *GetSubChannel( INT iSubChannel ) const;
	virtual const CIMediaInfo::StruMediaChannelInfo *GetChannel( UINT16 iIndex ) const;
	virtual BOOL GetChannel(UINT16 iIndex, StruGSMediaDescri *&ppResult, INT &iSize) const;
	virtual UINT16 GetChannelNums(void) const;

	virtual EnumGSMediaType GetMediaType( UINT iIndex) const;

	//��������
     void Set( const CIMediaInfo &csBase );

     INT AddChannel(const StruGSMediaDescri *pMedia ,INT iChn, const char *szRtpSdpFmtp );




    /*
    *********************************************
    Function :  Clear
    DateTime : 2010/8/6 10:43
    Description : ���ý��ͨ��
    Input : iChn ָ����ý��ͨ���� ���Ϊ-1���������ͨ��
    Output :
    Return :
    Note :
    *********************************************
    */
     void Clear(INT iChn = -1);

     void Clear( EnumGSMediaType eType);

    /*
    *********************************************
    Function :  Serial2String
    DateTime : 2010/8/6 10:47
    Description :  ���ý����Ϣ����
    Input :
    Output :   csStr�������Ϣ
    Return :
    Note :
    *********************************************
    */
    CGSPString Serial2String(void) const; 


    CMediaInfo &operator=(const CMediaInfo &csDest ) ; 

	
   

    INT IsSupport( const  CIMediaInfo *pRequest ) const;

    static INT IsSupport( const  CIMediaInfo *pSrc ,const  CIMediaInfo *pRequest );

    static BOOL CmpMediaDescri(const StruGSMediaDescri *pSrc, const StruGSMediaDescri *pRequest );

	//�� GXX �������� ���� ���ķ�װ����
	static EnumStreamPackingType GetStreamPkt4GsCodeId(EnumGSCodeID eCodeId );

	//������װ����һ����������
	static EnumGSCodeID TestGsCodeId4StreamPkt(EnumStreamPackingType  ePkgType );


	static UINT8 GetGSPSStreamType4GsCodeId(EnumGSCodeID eCodeId );
	static EnumGSCodeID GetGsCodeId4GSPSStreamType(INT  iType );
};






};

#endif
