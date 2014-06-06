/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : STREAMPACKDECODER.H
Author :  ������
Version : 1.0.0.1
Date: 2013/5/8 10:12
Description: �Ѹ������̵����ʽ���Ϊ��׼������
********************************************
*/

#ifndef _GS_H_STREAMPACKDECODER_H_
#define _GS_H_STREAMPACKDECODER_H_

#include "GSMediaDefs.h"
#include "MediaInfo.h"
#include "GSPMemory.h"
#include "List.h"
namespace GSP
{



class CStreamPackDecoder : public CGSPObject
{
public :
	//bOutFactorStream ������������� ���������׼����
	virtual EnumErrno Init( BOOL bOutFactorStream ) = 0;
	//bExistGSFH ���� GSFameHeader
	virtual EnumErrno Decode( CFrameCache *pFrame, BOOL bExistGSFH ) = 0;
	virtual CFrameCache *Get(void) = 0;
	virtual void BindChannelInfo( UINT iChnNo, EnumGSMediaType eMType ) = 0;
	virtual void BindChannelInfo( const CMediaInfo &csMediaInfo ) = 0;
	virtual EnumGSCodeID GetCodeId( UINT iChnNo ) = 0;
	virtual EnumGSMediaType GetMeidaType(UINT iChnNo ) = 0;
	virtual ~CStreamPackDecoder(void)
	{

	}
	static CStreamPackDecoder *Make(EnumStreamPackingType eSrcPktType);
	


	static void ResigterExtDecoder(EnumStreamPackingType ePktType, 
									CStreamPackDecoder*(*CreateFunc)(EnumStreamPackingType ePktType) );
	
protected :
	CStreamPackDecoder(void) : CGSPObject()
	{

	}
};

/*
*********************************************************************
*
*@brief : CStPkDdBase CStreamPackDecoder �Ļ���ʵ��
*
*********************************************************************
*/
class CStPkDdBase : public CStreamPackDecoder
{
protected :
	CList m_listFrameCache; // �洢 CFrameCache * ������Ľ��
	typedef struct _StruInfoCtx
	{
		EnumGSMediaType eMType;
		EnumGSCodeID eCodeId;
		UINT iChnNo;
	}StruInfoCtx;
	StruInfoCtx m_vInfoCtx[GSP_MAX_MEDIA_CHANNELS];
	EnumStreamPackingType m_eSrcPktType;
	BOOL m_bOutFactorStream;
public :

	virtual EnumErrno Init( BOOL bOutFactorStream );
	virtual CFrameCache *Get(void);
	virtual void BindChannelInfo( UINT iChnNo, EnumGSMediaType eMType);

	virtual void BindChannelInfo( const CMediaInfo &csMediaInfo );
	virtual EnumGSCodeID GetCodeId( UINT iChnNo );

	virtual EnumGSMediaType GetMeidaType(UINT iChnNo );
protected :
	CStPkDdBase(EnumStreamPackingType eSrcPktType);
	virtual ~CStPkDdBase(void);
	UINT16 GetMediaChannel(EnumGSMediaType eMType );
	void SetMediaCodeId(EnumGSMediaType eMType, EnumGSCodeID eCodeId );
};

} //end namespace GSP

#endif //end _GS_H_STREAMPACKDECODER_H_