/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : REFSOURCE.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/7/21 14:46
Description: CIRefSource ��ʵ��
********************************************
*/
#ifndef GSS_REFSOURCE_DEF_H
#define GSS_REFSOURCE_DEF_H

#include "GSPObject.h"





namespace GSP
{
class CIBaseSource;
class CBaseSource;

class CRefSource :
    public  CRefObject
{
public :
	typedef enum
	{
		eEVT_STREAM_FRAME = 0, //������,���� CFrameCache *
		eEVT_PLAY_STATUS, //����״̬����, ���� const StruPlayStatus *
		eEVT_SOURCE_RELEASE, //����Դ���ͷ�, ����NULL
		eEVT_SOURCE_ASSERT, //����Դ�쳣, ����NULL
		eEVT_PLAY_END, //�������, ����NULL
	}EnumRefSourceEvent;
	typedef EnumErrno (CGSPObject::*FuncPtrRefSourceEvent)(CRefSource *pRefSource,
									EnumRefSourceEvent eEvt, void *pParam);

protected :
	
    CBaseSource *m_pSrc;
    BOOL m_bWaitKey;     //�ȴ��ؼ�֡
    BOOL m_bStart;      //�����Ƿ��Ѿ�����
    BOOL m_bPlay;       //�Ƿ��ڲ���״̬
    UINT8 m_bEnableSysHeader; //��Ҫ����֡ͷ
	BOOL m_bQuickFrame; //����֡ͷ

    CGSPObject *m_pFnEvtOwner;
    FuncPtrRefSourceEvent m_fnEvtCallback;
	INT32 m_eTranModel;
	INT m_iFlowctrl;
private :
	void *m_pSourcePrivateData; //����Դ˽������
	UINT8 m_iDelayCounts;
public :

    

	INLINE void *GetSourcePrivate(void) const
	{
		return m_pSourcePrivateData;
	}

	INLINE void SetSourcePrivate( void *pData )
	{
		m_pSourcePrivateData = pData;
	}

	BOOL Start(void);
	void Stop(void);
	CIBaseSource *Source(void);
	void SetTransMode( INT32 eMode );	
	EnumErrno Ctrl(const StruGSPCmdCtrl &stCtrl);  
	void SetListener( CGSPObject *pFnEvtOwner, FuncPtrRefSourceEvent fnEvtCallback );
    INT32 GetCtrlAbilities(void);
  
    BOOL WaitSysHeader(void) ;

    INLINE void EnableWaitSysHeader( BOOL bEnable )
    {		
		if( bEnable )
		{
			m_bEnableSysHeader = 1;
		}
		else
		{
			m_bEnableSysHeader = 0;
		}
    }

    INLINE void EnableWaitKeyFrame( BOOL bEnable )
    {		
        m_bWaitKey = bEnable;
	}

	INLINE void EnableQuickFrame( BOOL bEnable )
	{
		m_bQuickFrame = bEnable;
	}

	INLINE BOOL IsQuickFrame(void)  const
	{
		return m_bQuickFrame;
	}

    INLINE BOOL IsRequestStream(void)  const
    {
        return m_bPlay && m_bStart;
    }
    virtual EnumErrno  OnSourceEvent( EnumRefSourceEvent eEvt, void *pParam);

	
	static CRefSource *Create(CBaseSource *pSource)
	{
		return new CRefSource(pSource);
	}

	void Release(void);
private :
	CRefSource(CBaseSource *pSource );
	virtual ~CRefSource(void);   

};




};

#endif
