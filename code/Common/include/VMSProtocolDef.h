#ifndef VMSPROTOCOLDEF_DEF_H
#define VMSPROTOCOLDEF_DEF_H
/**************************************************************************************************
  Copyright (C), 2010-2011, GOSUN 
  File name 	: VMSPROTOCOLDEF.H      
  Author 		: Liujs      
  Version 		: Vx.xx        
  DateTime 		: 2010/11/15 9:22
  Description 	: VMS �ڲ�ʹ�õ�ͨ��Э�飬Ҳ���ǣ�Master��Slaverͨ���õ�����Э��
				  ������������ܻ��CMD_PROTOCOL_DEF_DEF_H����ͬ��ʹ�õ�ʱ��
				  Ҫ����������ռ�CmdVMSProtocolDef
**************************************************************************************************/

// �ֽڶ����С
#define		PRAGMA_PACK_SIZE		1
#ifdef _WIN32
#define		ATTRIBUTE_PACKED 
#pragma		pack(push,PRAGMA_PACK_SIZE )
#else
#define ATTRIBUTE_PACKED    __attribute__ ((packed)) 
#endif

// ͷ�ļ���ʹ������ĳ��ȵĶ���ĺ꣬Ŀ�ľ����¿������Ȳ�һ����ʱ����ɳ��Ȳ�һ�����
// ��Ĳ��ֶ�����
#include "CMD_PROTOCOL_DEF.H"

#define		DELETE_FILE_NUM					256										// ɾ���ļ�����

// �����Լ��������ռ�
namespace	CmdVMSProtocolDef
{
	/**************************************************************************************************
		CMD_ID		: EnumClientType
		CMD_NAME	: �����豸��Ԫ���Ͷ���
		DateTime	: 2010/11/15 9:33	
		Description	: �����豸��Ԫ���Ͷ���
		Author		: Liujs
		Note		: CLIENT_TYPE_MASTER��CLIENT_TYPE_SLAVERΪVMS�ڲ�ʹ�ã�������CMD_PROTOCOL_DEF.H�ж�����ͬ
	**************************************************************************************************/
	typedef		enum	EnumClientType
	{
		CLIENT_TYPE_PMS = 101,							// PMS
		CLIENT_TYPE_DAS	,								// DAS
		CLIENT_TYPE_STS	,								// STS
		CLIENT_TYPE_LMS ,								// LMS
		CLIENT_TYPE_AMS ,								// AMS
		CLIENT_TYPE_VMS ,								// VMS
		CLIENT_TYPE_CLI ,								// CLI
		CLIENT_TYPE_CMU ,								// CMU
		CLIENT_TYPE_PU	,								// PU
		CLIENT_TYPE_USER,								// USER
		CLIENT_TYPE_MASTER,								// MASTER
		CLIENT_TYPE_SLAVER								// SLAVER
	}EnumClientType;

	/**************************************************************************************************
		CMD_ID		: EnumErrorCode
		CMD_NAME	: ���󷵻ش���
		DateTime	: 2010/11/15 9:45	
		Description	: ���󷵻ش���
		Author		: Liujs
		Note		: ��Master��Slaver���Լ�����������
	**************************************************************************************************/
	typedef		enum	EnumErrorCode
	{
		//--------------------------------------------------------------------
		// ͨ�ò����ɹ���ʧ��
		OPER_RESULT_SUCCESS				= 0,				// �����ɹ�
		OPER_RESULT_FAIL				,					// ����ʧ��
		OPER_UNKNOW_ERROR				,					// δ֪����

		//--------------------------------------------------------------------
		// ��½���ؽ��
		LOG_RESULT_USER_NAME_ERROR		,					// �û���������	
		LOG_RESULT_PWD_ERROR			,					// �����	
		LOG_RESULT_HAS_EXIST			,					// ��¼ID�Ѵ���	
		LOG_RESULT_SERVICE_LOG_FULL		,					// �����������ﵽ����

		//--------------------------------------------------------------------
		// �������ؽ��
		GET_STREAM_NO_STREAM			,					// ��������
		GET_STREAM_NO_DEVICE			,					// �豸�����ڣ����߲�����ʹ��
		GET_STREAM_NO_CHANNEL			,					// �豸��Ӧͨ��������
		GET_STREAM_TIME_OUT									// ������ʱ
		
		

		//--------------------------------------------------------------------
		// ��ȡ���ݰ汾���

		//--------------------------------------------------------------------
		// �澯��ش���

	}EnumErrorReturnCode;
	
	// ����ֵ��Ϣ�ṹ
	typedef		struct	StruRetInfo 
	{
		INT32			iRetCode;
		std::string		strRetInfo;
	}StruRetCodeInfo;
	
	// �������ķ���ֵ
	const		StruRetCodeInfo		conStRetInfo[]={
		{	OPER_RESULT_SUCCESS,									"�����ɹ�"},
		{	OPER_RESULT_FAIL,										"����ʧ��"},
		{	OPER_UNKNOW_ERROR,										"δ֪����"},
		//--------------------------------------------------------------------
		// ��½���ؽ��
		{	LOG_RESULT_USER_NAME_ERROR,								"�û���������"},
		{	LOG_RESULT_PWD_ERROR,									"�������"},
		{	LOG_RESULT_HAS_EXIST,									"��¼ID�Ѵ���"},
		{	LOG_RESULT_SERVICE_LOG_FULL,							"�����������ﵽ����"},
		//--------------------------------------------------------------------
		// �������ؽ��
		{	GET_STREAM_NO_STREAM,									"��������"},
		{	GET_STREAM_NO_DEVICE,									"�豸�����ڣ����߲�����ʹ��"},
		{	GET_STREAM_NO_CHANNEL,									"�豸��Ӧͨ��������"},
		{	GET_STREAM_TIME_OUT,									"������ʱ"},
		
		//--------------------------------------------------------------------
		// �������
	};

	
	/**************************************************************************************************
	CMD_ID			: CMD_LOGIN    
	CMD_NAME		: ��½PMS    
	DateTime		: 2010/7/29 10:39	
	Author 			: Liujs      
	Description		: ��½����
	Note			: �ͻ��˵�½PMS����
	**************************************************************************************************/
	#define			CMD_LOGIN_REQUEST			1			// ƽ̨�ڿͻ��ˣ���½PMS����½����

		// �յĵ�½ID,��дΪ0
	#define			EMPTY_LOGIN_ID				0

	// ��½�������ݽṹ��
	typedef		struct		StruCmdLoginInfo 
	{
		INT32		iLoginID;								// ��½ID�����û�о���д EMPTY_LOGIN_ID
		INT32		iLoginType;								// ��½����	�ο�EnumClientType�ж���
		char		szUserName[MAX_NAME_LEN];				// ��½����
		char		szPassword[MAX_NAME_LEN];				// ��½����
		char        szClientIP[MAX_IP_ADDR_LEN];            // ���ؿͻ���IP
		INT32       iClientPort;                            // ���ؿͻ��˶˿�
		char		szVersion[MAX_VERSION_LEN];				// �汾�ţ�CLI��¼��0�����������¼�������İ汾��
	}StruCmdLoginInfo,*StruCmdLoginInfoPtr;	



	/**************************************************************************************************
	CMD_ID			: CMD_LOGIN_RESPONSE 
	CMD_NAME		: ��½Ӧ������       
	DateTime		: 2010/7/29 11:03	
	Author 			: Liujs      
	Description		: ��½����ظ�
	Note			: 
	**************************************************************************************************/
	#define		CMD_LOGIN_RESPONSE				2			// ��½����ظ�

	// ��½����ظ��ṹ��
	typedef		struct		StruCmdLoginResponse 
	{
		INT32		iLogResult;								// ��½������ο�EnumErrorCode�ж���
		INT32		iMasterID;								// MasterID,ͨ�ŵ�ʱ���õ�
		INT32		iLoginID;								// ��½����ID
	}StruCmdLoginResponse,*StruCmdLoginResponsePtr;


	/**************************************************************************************************
	CMD_ID		: EnumAlarmType
	CMD_NAME	: �澯���Ͷ���
	DateTime	: 2010/11/4 10:24	
	Description	: �澯���ͣ��豸�澯���ͣ�����澯����
	Author		: Liujs
	Note		: NULL
	**************************************************************************************************/
	typedef		enum	EnumAlarmType
	{
		//--------------------------------------------------------------------------------------------------------------------\\
		// û�и澯��д-1
		NO_ALARM							= CmdProtocolDef::NO_ALARM,									// û�и澯

		//--------------------------------------------------------------------------------------------------------------------

	}EnumAlarmType;



	/**************************************************************************************************
	CMD_ID			: CMD_GET_STREAM_REQUEST   
	CMD_NAME		: �������
	DateTime		: 2010/7/29 11:50	
	Author 			: Liujs      
	Description		: ��������
	Note			: ��ע
	**************************************************************************************************/
	#define		CMD_GET_STREAM_REQUEST			101			// ������������

	// �����������������
	typedef		struct		StruCmdGetStream
	{
		INT32		iPlatformID;							// ƽ̨ID
		INT32		iDeviceID;								// �豸ID
		INT32		iChannelNum;							// ͨ����
		INT32		iStreamType;							// �������ͣ�����
	}StruCmdGetStream,*StruCmdGetStreamPtr;


	/**************************************************************************************************
	CMD_ID			: CMD_GET_STREAM_RESPONSE  
	CMD_NAME		: ���������������
	DateTime		: 2010/7/29 14:14	
	Author 			: Liujs      
	Description		: ��������ؽ����Ϣ
	Note			: 
	**************************************************************************************************/
	#define		CMD_GET_STREAM_RESPONSE			102			// �������ؽ������

	// �������ؽ����������
	typedef		struct		StruCmdGetStreamResponse 
	{
		INT32		iGetStreamResult;						// �������ؽ�����ο�EnumErrorCode	
		INT32		iPlatformID;							// ƽ̨ID
		INT32		iDeviceID;								// �豸ID
		INT32		iChannelNum;							// ����ͨ����Ϣ
		char		szStreamURI[MAX_URI_LEN];				// ��������URI��Դ��Ϣ����󳤶�Ϊ512�ַ�,256�����ַ�
	}StruCmdGetStreamResponse,*StruCmdGetStreamResponsePtr;

	/**************************************************************************************************
	CMD_ID			: CMD_CHNLMATCH_NOTICE_RESPONSE  
	CMD_NAME		: ͨ��������Ϣ֪ͨ����
	DateTime		: 2011/7/29 14:14	
	Author 			: chc      
	Description		: ͨ��������Ϣ֪ͨ����
	Note			: 
	**************************************************************************************************/
	#define		SLV_CMD_CHNLMATCH_NOTICE_RESPONSE			111			// ͨ��������Ϣ֪ͨ����
	typedef struct StruCmdChnl
	{
		INT32	iLogicalChnl;					// �߼�ͨ��
		INT32	iPhysicsChnl;					// ����ͨ��

	}StruCmdChnl,*StruCmdChnlPtr;

	typedef struct StruCmdChnlInfo
	{
		INT32	iNum;							// ͨ����Ŀ
		StruCmdChnl	stCmdchnl[1];				// ͨ���ṹ,�䳤

	}StruCmdChnlInfo,*StruCmdChnlInfoPtr;

	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/5/12 9:00	
	Description	: ������ǽ��������
	Author		: chc
	Note		: �ͻ���ָ��ĳһ·������ǽʱ��ָ��
	**************************************************************************************************/
	#define		SLV_CMD_STREAM_MATRIX_REQUEST			115
	typedef struct StruDeviceLoginInfo
	{
		CHAR  szIP[MAX_IP_ADDR_LEN];
		INT32 iPort;
		CHAR  szUserName[MAX_NAME_LEN];
		CHAR  szPwd[MAX_PWD_LEN];
		CHAR  szDevCodeID[MAX_NAME_LEN_256];
	}StruDeviceLoginInfo,*StruDeviceLoginInfoPtr;
	typedef struct StruDeviceInfoRequest
	{
		INT32 iPlatformID;				// ƽ̨ID
		INT32 iDevID;					// �豸ID
		INT32 iChnID;					// ͨ��ID
		INT32 iLogicalOutChnlID;		// �߼����ͨ��ID
		INT32 iStreamType;				// 0��ʵʱ�� 1��¼����
		CHAR  szURI[MAX_URI_LEN];		// �����¼����ǽ �������URI
		StruDeviceLoginInfo stDevLoginInfo;	//�豸��½��Ϣ
	}StruDeviceInfoRequest,*StruDeviceInfoRequestPtr;

	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/5/12 9:00	
	Description	: ������ǽ�ظ�����
	Author		: chc
	Note		: 
	**************************************************************************************************/
	#define		SLV_CMD_STREAM_MATRIX_RESPONSE			116
	typedef struct StruDeviceInfoResponse
	{
		INT32 iOperResult;				// �ظ����,ö��EnumErrorCode����
		StruDeviceInfoRequest	stDeviceInfo;	// �豸��Ϣ 
	}StruDeviceInfoResponse,*StruDeviceInfoResponsePtr;

	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/5/12 9:00	
	Description	: ֹͣ������ǽ����
	Author		: chc
	Note		: �ͻ���ָ��ֹͣĳһ·������ǽʱ��ָ��
	**************************************************************************************************/
#define		SLV_CMD_STOP_MATRIX_REQUEST			117
	// ע�⣺����SLV_CMD_STREAM_MATRIX_REQUEST������ͬ

	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/5/12 9:00	
	Description	: ֹͣ������ǽ�ظ�����
	Author		: chc
	Note		: �ͻ���ָ��ֹͣĳһ·������ǽʱ�Ļظ�ָ��
	**************************************************************************************************/
#define		SLV_CMD_STOP_MATRIX_RESPONSE			118
	// ע�⣺����SLV_CMD_STREAM_MATRIX_RESPONSE������ͬ

	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/12/12 9:00	
	Description	: ������뿨��Ϣ����
	Author		: chc
	Note		: 
	**************************************************************************************************/
#define		SLV_CMD_GET_DECODECARD_REQUEST			119
	// ע��:�������޲���


	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/12/12 9:00	
	Description	: ���뿨��Ϣ����ظ�����
	Author		: chc
	Note		: 
	**************************************************************************************************/
#define		SLV_CMD_GET_DECODECARD_RESPONSE			120
	typedef struct StruDecodecardInfo
	{
		char	szCardName[MAX_NAME_LEN];
		INT32	iCardType;
		INT32	iLen;
		char	szPChnlSplitMode[1];	//����ͨ���ָ�ģʽ,�䳤

	}StruDecodecardInfo, *StruDecodecardInfoPtr;
	typedef struct StruDecodecardResponse
	{
		INT32				iOperResult;					//	�ظ����,ö��EnumErrorCode����
		INT32				iNum;							// ���뿨��Ŀ
		StruDecodecardInfo	stDecodecardInfo[1];//�䳤
	}StruDecodecardResponse, *StruDecodecardResponsePtr;

		/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/12/12 9:00	
	Description	: ���������ͨ��ӳ������
	Author		: chc
	Note		: 
	**************************************************************************************************/
#define		SLV_CMD_GET_SPLITCHNL_REQUEST			121
	// ע��:�������޲���


	/**************************************************************************************************
	CMD_ID		: 
	CMD_NAME	: 
	DateTime	: 2011/12/12 9:00	
	Description	: ������ͨ��ӳ������ظ�����
	Author		: chc
	Note		: 
	**************************************************************************************************/
#define		SLV_CMD_GET_SPLITCHNL_RESPONSE			122
	typedef struct StruChnlConfigResponse
	{
		INT32				iResult;						// ���
		INT32				iLen;							// ����
		char				pContent[1];					// ����
	}StruChnlConfigResponse,*StruChnlConfigResponsePtr;

	/**************************************************************************************************
	CMD_ID		: CMD_STATUSSYN_MATRIX_NOTICE_VMS
	CMD_NAME	: CMD_STATUSSYN_MATRIX_NOTICE_VMS
	DateTime	: 2012/07/26 11:00		
	Description	: ����ǽ״̬ͬ��֪ͨ��slave����master
	Author		: CHC
	Note		: �������ҪӦ��
	**************************************************************************************************/
	#define CMD_STATUSSYN_MATRIX_NOTICE_VMS			123
	typedef	enum EnumStatusMatrixType
	{
		enum_DecoderOnLineType,			//������
		enum_CurMatrixStreamType,		// ��ǰ��ǽ�ϵ�������Ϣ�����ݽṹ��SLV_CMD_STREAM_MATRIX_REQUEST��ȫ��ͬ
	}EnumStatusMatrixType;

	//enum_DecoderOnLineType���͵����ݽṹ
	typedef		struct		StruDecoderOnLineResponse
	{
		CHAR	szName[MAX_NAME_LEN];	//����������
		CHAR	szDecoderIP[MAX_IP_ADDR_LEN];	//������IP
		CHAR	szSalveName[MAX_NAME_LEN];	//����slave
		INT32	iOnLine;	//���EnumDevStatusType
		StruDecoderOnLineResponse()
		{
			memset(szName, 0x0, MAX_NAME_LEN);
			memset(szDecoderIP, 0x0, MAX_IP_ADDR_LEN);
			memset(szSalveName, 0x0, MAX_NAME_LEN);
		};
	}StruDecoderOnLineResponse,*StruDecoderOnLineResponsePtr;
	
	//enum_CurMatrixStreamType���͵����ݽṹ��SLV_CMD_STREAM_MATRIX_REQUEST��ȫ��ͬ

	//�����ݽṹ
	typedef		struct		StruStatusSynMatrixNotice 
	{		
		INT32			iRequestType;									// �������ͣ��ο��ṹEnumStatusMatrixType
		INT32			iDataLen;										// ���ݳ���	
		/* 
		�������ݣ�����EnumStatusMatrixType������������
		��������						���ݽṹ��
		enum_OnLineType				StruOnLineResponse
		*/
		void*			pData;											// �������ݣ�����EnumStatusMatrixType������������											
	}StruStatusSynMatrixNotice,*StruStatusSynMatrixNoticePtr;

	/**************************************************************************************************
		CMD_ID		: CMD_TIME_SYNCHRONIZATION
		CMD_NAME	: CMD_TIME_SYNCHRONIZATION
		DateTime	: 2010/12/3 10:48	
		Description	: ʱ��ͬ������
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_TIME_SYNCHRONIZATION					280
	
	// ʱ��ͬ������
	typedef	struct	StruCmdSynchronizationTime 
	{
		CmdProtocolDef::StruDateTime		stDateTime;							// ʱ����Ϣ
	}StruCmdSynchronizationTime,*StruCmdSynchronizationTimePtr;


	/**************************************************************************************************
	CMD_ID		: CMD_ALARM_NOTICE
	CMD_NAME	: CMD_ALARM_NOTICE
	DateTime	: 2010/12/8 16:19	
	Description	: ���̸澯֪ͨ
	Author		: Liujs
	Note		: NULL
	**************************************************************************************************/
	#define		CMD_ALARM_NOTICE							300

	// �澯��������ṹ
	typedef		struct	StruCmdAlarmNotice 
	{
		INT32				iAlarmType;							// �澯����,�ο�EnumAlarmType
		char				szStorageUUID[MAX_NAME_LEN];		// ����UUIDΨһ��־
	}StruCmdAlarmNotice,*StruCmdAlarmNoticePtr;

#define SLV_CMD_GET_DECODE_DEVICE_PARA_REQUEST 301
#define SLV_CMD_GET_DECODE_DEVICE_PARA_RESPONSE 302

	// �豸�������ò�����Ϣ
	typedef struct StruDevParaCfg 
	{
		INT32 iLen;                   // XML���ݳ���
		char pXMLData[1];   // XML���� (�������ݲο�XML��ز�������)
	} StruDevParaCfg, *StruDevParaCfgPtr;

	// �豸�������ý���ظ���Ϣ
	typedef struct StruDevParaCfgResult
	{
		INT32 iResult;                   // �������
		StruDevParaCfg stDevParaCfg;     // �豸�������ò�����Ϣ (������ȡ���õ�)
	} StruDevParaCfgResult, *StruDevParaCfgResultPtr;

	/**************************************************************************************************
	CMD_ID		: CMD_DATA_UPDATE_NOTICE_EX_RESPONSE
	CMD_NAME	: CMD_DATA_UPDATE_NOTICE_EX_RESPONSE
	DateTime	: 2012/07/17 10:48	
	Description	: master��slave֮������ݸ���֪ͨ
	Author		: chc
	Note		: ����Ҫ�ظ�
	**************************************************************************************************/
	#define		CMD_DATA_UPDATE_NOTICE_VMS			905

	typedef	enum
	{
		// ��½ 
		E_CMD_LOGIN_REQUEST					=	CMD_LOGIN_REQUEST,				// ��½����
		E_CMD_LOGIN_RESPONSE				=	CMD_LOGIN_RESPONSE,				// ��½Ӧ������

		// ����
		E_CMD_GET_STREAM_REQUEST			=	CMD_GET_STREAM_REQUEST,			// ��������
		E_CMD_GET_STREAM_RESPONSE			=	CMD_GET_STREAM_RESPONSE,		// ����Ӧ������

		// ������
		E_SLV_CMD_STREAM_MATRIX_REQUEST		=	SLV_CMD_STREAM_MATRIX_REQUEST,		// ������ǽ
		E_SLV_CMD_STREAM_MATRIX_RESPONSE	=	SLV_CMD_STREAM_MATRIX_RESPONSE,			// ������ǽ�ظ�
		E_SLV_CMD_STOP_MATRIX_REQUEST		=	SLV_CMD_STOP_MATRIX_REQUEST,		// ֹͣ������ǽ
		E_SLV_CMD_STOP_MATRIX_RESPONSE		=	SLV_CMD_STOP_MATRIX_RESPONSE,			// ֹͣ������ǽ�ظ�
		
		E_SLV_CMD_CHNLMATCH_NOTICE_RESPONSE   = SLV_CMD_CHNLMATCH_NOTICE_RESPONSE,		//ͨ��ӳ��֪ͨ.����ʹ�ô�����chc2011.12.12
		E_SLV_CMD_GET_DECODECARD_REQUEST   = SLV_CMD_GET_DECODECARD_REQUEST,		//��ȡ���뿨��Ϣ
		E_SLV_CMD_GET_DECODECARD_RESPONSE   = SLV_CMD_GET_DECODECARD_RESPONSE,		//��ȡ���뿨��Ϣ�ظ�
		E_SLV_CMD_GET_SPLITCHNL_REQUEST   = SLV_CMD_GET_SPLITCHNL_REQUEST,			//��ȡͨ��ӳ����Ϣ
		E_SLV_CMD_GET_SPLITCHNL_RESPONSE   = SLV_CMD_GET_SPLITCHNL_RESPONSE,		//��ȡͨ��ӳ����Ϣ�ظ�

		E_SLV_CMD_GET_DECODE_DEVICE_PARA_REQUEST    = SLV_CMD_GET_DECODE_DEVICE_PARA_REQUEST,   // �������豸������ȡ����
		E_SLV_CMD_GET_DECODE_DEVICE_PARA_RESPONSE   = SLV_CMD_GET_DECODE_DEVICE_PARA_RESPONSE,  // �������豸������ȡ�ظ�

		E_CMD_STATUSSYN_MATRIX_NOTICE_VMS	= CMD_STATUSSYN_MATRIX_NOTICE_VMS,		//״̬ͬ������

		E_CMD_TIME_SYNCHRONIZATION			=	CMD_TIME_SYNCHRONIZATION,		// ʱ��ͬ������

		E_CMD_DATA_UPDATE_NOTICE_VMS		= CMD_DATA_UPDATE_NOTICE_VMS,	// master��slave����֮������ݸ���֪ͨ�ظ�

		E_CMD_ALARM_NOTICE					=	CMD_ALARM_NOTICE,				// �澯֪ͨ

	}EnumCmdType;

}


#ifdef _WIN32
#pragma		pack(pop)
#endif


#endif 
// VMSPROTOCOLDEF_DEF_H
