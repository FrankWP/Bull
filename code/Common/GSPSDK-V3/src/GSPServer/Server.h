#ifndef GSS_SERVER_DEF_H
#define GSS_SERVER_DEF_H

#include "IServer.h"
#include "GSPObject.h"
#include "Uri.h"
#include "IMediaInfo.h"
#include "List.h"



namespace GSP
{
#define MAX_SOURCE_MANAGER 3000

	class CRefSource;
	class CIProServer;
	class CLog;
	class CBaseSource;

	typedef enum
	{
		eEVT_PROSRV_ACCEPT, //���ܵ�������, pEvtArg Ϊ CISrvSession *
		eEVT_PROSRV_SESSION_RELEASE, // �Ự���ͷţ� pEvtArg Ϊ CISrvSession *
	}EnumProServerEvent;

	typedef enum
	{
		eEVT_SRC_REF,   // pEvtParam == NULL
		eEVT_SRC_UNREF, // pEvtParam == NULL
		eEVT_SRC_RELEASE, // pEvtParam == NULL
		eEVT_SRC_CTRL, //�����¼� pEvtParam ==   StruGSPCmdCtrl *
		eEVT_SRC_FRAME, //֡���� pEvtParam == CFrame *
		eEVT_SRC_REQST, //��ȡ״̬�� pEvtParam == NULL
	}EnumSourceEvent;


	class CISecurity : public CGSPObject
	{
	public :
		virtual CGSPString &GetSecurityKey( const char *szSourceKey ) = 0;
		virtual EnumErrno  SecurityCheck( const char *szSecurityKey, const char *szSourceKey ) = 0;
	};

	
class CServer :
    public CGSPObject, public CIServer
{
    
private:
	const static int s_iMaxServerManageSource = 3000;
    CLog *m_pLog;
    CGSMutex m_csMutex;
	
	CGSWRMutex m_csSrcWRMutex; //����Դ��
	std::vector<CBaseSource *> m_vSource; //�ܹ��������Դ
	typedef std::map<CGSString, CBaseSource *> CSourceMap; //��Ч������Դ
	CSourceMap m_mSource;
	CConfigFile m_csConfig;
	typedef std::map<EnumProtocol, CIProServer *> CProServerModuleMap;
	CProServerModuleMap m_mIServer;
	
	void *m_pEventFnParam;       //�¼��ص�����
	GSPServerEventFunctionPtr m_fnOnEvent; //�¼��ص�

	CISecurity *m_pSecurityCenter;
	INT m_bDestroying;

	CList m_csSessionList;   //���ӵ�Session �б�
	CGSWRMutex m_csSessionMutex; // m_csSessionList ��



public :
    CServer(void);
    virtual ~CServer(void);

	void *operator new(size_t iSize)
	{
		return ::malloc(iSize);
	}

	void operator delete(void *pBuffer)
	{
		::free(pBuffer);
	}

	/*
	*********************************************
	Function :  InitLog 
	DateTime : 2010/6/11 16:19
	Description : ������־���ݽӿ�
	Input :   czPathName ������־�ӿڵ�·��
	Output :  
	Return :  
	Note :
	*********************************************
	*/
	virtual void InitLog( const char *czPathName );


	virtual CIUri *CreateEmptyUriObject(void);

	/*
	*********************************************
	Function : Init
	DateTime : 2010/6/11 16:22
	Description :  ��������
	Input :
	Output :
	Return : TRUE/FALSE
	Note :
	*********************************************
	*/
	virtual BOOL Init( const char *csConfigFilename,const char *csLogPathName);

	/*
	*********************************************
	Function : Stop
	DateTime : 2010/6/11 16:22
	Description :  ֹͣ����
	Input :
	Output :
	Return : TRUE/FALSE
	Note :
	*********************************************
	*/
	virtual BOOL Stop(void);


	/*
	*********************************************
	Function :   FindSource
	DateTime : 2010/6/11 16:27
	Description :  �ü�ֵ����������Դ
	Input : czKey Ҫ���ҵ�����Դ��ֵ
	Output :
	Return :  �����ڷ���NULL
	Note :
	*********************************************
	*/
	virtual CISource *FindSource(const char *csKey);

	virtual CISource *FindSource(UINT16 iSourceIndex );


	/*
	*********************************************
	Function :   AddPushSource
	DateTime : 2010/6/11 16:27
	Description :  �����ģʽ����Դ
	Input : szKey ����Դ��ֵ
	Output :
	Return : ���ؽ�������ģʽ����Դ�� ʧ�ܷ���NULL
	Note :
	*********************************************
	*/
	virtual CISource *AddSource( const char *szKey );

	virtual CISource *AddPullSource( const char *czKey );




	virtual CISource::EnumRetNO WriteData( UINT16 iSourceIndex,  
		const void *pData, INT iLen, UINT iChn, BOOL bKey); 


	virtual CISource::EnumRetNO WriteSysHeader( UINT16 iSourceIndex, 
		const void *pData, INT iLen, UINT iChn) ;

	virtual CISource::EnumRetNO WriteDataV( UINT16 iSourceIndex, 
		const StruIOV *pIOV, INT iVNums, UINT iChn, BOOL bKey); 


	virtual CISource::EnumRetNO WriteSysHeaderV( UINT16 iSourceIndex, 
		const StruIOV *pIOV, INT iVNums, UINT iChn);


	/*
	*********************************************
	Function : SetEventCallback
	DateTime : 2010/6/11 16:32
	Description :  ���÷��������¼��ص�
	Input :   fnOnEvent �ص�����
	Input : pParam  �ص����û�����
	Output :
	Return : 
	Note :
	*********************************************
	*/
	virtual void SetEventCallback( GSPServerEventFunctionPtr fnOnEvent, void *pParam);



	virtual void InitURI( CIUri &csRet, const char *czKey, const char *czPro = "gsp", const char *szRemoteIP = NULL);

	/*
	*********************************************
	Function : QueryStatus
	DateTime : 2012/3/13 15:38
	Description :  ״̬��ѯ
	Input :  eQueryStatus ָ����ѯ��ѡ��
	Output : ppResult ���ص����ݽ��
	Output : pResultSize ����Ĵ�С
	Return : 
	Note :   �����Ҫ���� FreeQueryStatusResult �ͷ�
	*********************************************
	*/
	virtual BOOL QueryStatus(const EnumGSPServerStatus eQueryStatus, 
		char **ppResult,INT *pResultSize  );

	/*
	*********************************************
	Function : FreeQueryStatusResult
	DateTime : 2012/3/13 15:44
	Description :  �ͷŲ�ѯ�Ľ����
	Input :  
	Output : 
	Return : 
	Note :   
	*********************************************
	*/
	virtual void FreeQueryStatusResult( char *pResult);



	virtual BOOL GetOptions(CGSPServerOptions &Opts) const
	{
		return TRUE;
	}

	virtual BOOL SetOptions(const CGSPServerOptions &csOptions)
	{
		return TRUE;
	}


	/*
	*********************************************************************
	*
	*@brief : Ϊ�ڲ��ӿ�
	*
	*********************************************************************
	*/
	EnumErrno RequestSource(const CUri &csUri,const CIMediaInfo &csRquestMedia , INT eTransModel,
							CRefSource **ppResult);

	void  OnProServerEvent(CIProServer *pServer, EnumProServerEvent eEvt, void *pEvtArg);

	void  *OnSourceEvent(CBaseSource *pSource, EnumSourceEvent eEvt, void *pEvtArg);	

	INLINE CConfigFile &GetConfig(void)
	{
		return m_csConfig;
	}

private :
	BOOL InitProModule(void);
	void *ProcessServerEvent(  CISource *pSource,EnumGSPServerEvent eEvent, 
		void *pEventPararms, INT iLen);

	CGSString GetRouteIP(const char * czRemoteIP, CIProServer *pServer );


	void RemoveSource( CISource *pSource);


	BOOL RequestOutSource(void);

	

	CISource *AddSource( const char *szKey, CISource::EnumMode eMode );

};


};

#endif
