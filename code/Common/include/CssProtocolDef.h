#ifndef CSSPROTOCOLDEF_DEF_H
#define CSSPROTOCOLDEF_DEF_H
/**************************************************************************************************
  Copyright (C), 2010-2011, GOSUN 
  File name 	: CSSPROTOCOLDEF.H      
  Author 		: Liujs      
  Version 		: Vx.xx        
  DateTime 		: 2010/11/15 9:22
  Description 	: CSS �ڲ�ʹ�õ�ͨ��Э�飬Ҳ���ǣ�Master��Slaverͨ���õ�����Э��
				  ������������ܻ��CMD_PROTOCOL_DEF_DEF_H����ͬ��ʹ�õ�ʱ��
				  Ҫ����������ռ�CmdCssProtocolDef
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
namespace	CmdCssProtocolDef
{
	// ¼�������ļ�����
	typedef		enum		EnumRecordTaskFileType
	{
		T_F_T_VIDEO	=	1,		//	��Ƶ¼��
		T_F_T_PICTURE ,			//	ͼƬ��
		T_F_T_AUDIO				//	��Ƶ¼��
	}EnumRecordTaskFileType;

	// ����״̬��Ϣ
	typedef		enum		EnumRecordTaskStatus
	{
		INIT=0,			// 0(INIT): ��ʼ���� Master����ʱ���ã� �ȴ�Slaveȷ��
		WAIT,			// 1(WAIT): Master �������񣬵ȴ�Slave���
		DOING,			// 2(DOING): ��������
		ASSERT,			// 3(ASSERT): ����
		SLAVEROFFLINE,	// 4��SLAVEOFFLINE��:Slave ����
		DISKERR,		// 5��DISKERR��: ����ʧЧ
		COMPLETE,		// 6 (COMPLETE) : �������
		EXCEPTION       // 7 (EXCEPTION) : �쳣
	}EnumRecordTaskStatus;

	/**************************************************************************************************
		CMD_ID		: EnumClientType
		CMD_NAME	: �����豸��Ԫ���Ͷ���
		DateTime	: 2010/11/15 9:33	
		Description	: �����豸��Ԫ���Ͷ���
		Author		: Liujs
		Note		: CLIENT_TYPE_MASTER��CLIENT_TYPE_SLAVERΪCSS�ڲ�ʹ�ã�������CMD_PROTOCOL_DEF.H�ж�����ͬ
	**************************************************************************************************/
	typedef		enum	EnumClientType
	{
		CLIENT_TYPE_PMS = 101,							// PMS
		CLIENT_TYPE_DAS	,								// DAS
		CLIENT_TYPE_STS	,								// STS
		CLIENT_TYPE_LMS ,								// LMS
		CLIENT_TYPE_AMS ,								// AMS
		CLIENT_TYPE_CSS ,								// CSS
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
		OPER_RESULT_FAIL				= CmdProtocolDef::OPER_RESULT_FAIL,					// ����ʧ��
		OPER_UNKNOW_ERROR				= CmdProtocolDef::OPER_UNKNOW_ERROR,					// δ֪����

		//--------------------------------------------------------------------
		// ��½���ؽ��
		LOG_RESULT_USER_NAME_ERROR		= CmdProtocolDef::LOG_RESULT_USER_NAME_ERROR,					// �û���������	
		LOG_RESULT_PWD_ERROR			= CmdProtocolDef::LOG_RESULT_PWD_ERROR,					// �����	
		LOG_RESULT_HAS_EXIST			= CmdProtocolDef::LOG_RESULT_HAS_EXIST,					// ��¼ID�Ѵ���	
		LOG_RESULT_SERVICE_LOG_FULL		= CmdProtocolDef::LOG_RESULT_SERVICE_LOG_FULL,					// �����������ﵽ����

		//--------------------------------------------------------------------
		// �������ؽ��
		GET_STREAM_NO_STREAM			= CmdProtocolDef::GET_STREAM_NO_STREAM,					// ��������
		GET_STREAM_NO_DEVICE			= CmdProtocolDef::GET_STREAM_NO_DEVICE,					// �豸�����ڣ����߲�����ʹ��
		GET_STREAM_NO_CHANNEL			= CmdProtocolDef::GET_STREAM_NO_CHANNEL,					// �豸��Ӧͨ��������
		GET_STREAM_TIME_OUT				= CmdProtocolDef::GET_STREAM_TIME_OUT,					// ������ʱ
		
		//--------------------------------------------------------------------
		// ��ʼ¼��
		START_RECORD_ERROR				= -11,					// ��ʼ¼��ʧ��

		//--------------------------------------------------------------------
		// ֹͣ¼��
		STOP_RECORD_ERROR				= -20,					// ֹͣ¼��ʧ��
		
		//--------------------------------------------------------------------
		// �ı�洢λ��
		CHANGE_STORAGE_ERROR			= -30,					// �ı�洢¼��λ��ʧ��

		//--------------------------------------------------------------------
		// ɾ���ļ�
		DELETE_FILE_ERROR				= -40,					// ɾ���ļ�ʧ��
		DELETE_FILE_NO_FILE				= -41,					// �Ҳ���Ҫɾ�����ļ�

		//--------------------------------------------------------------------
		// ��ѯ������Ϣ
		QUERY_DISK_INFO_ERROR			= -50,					// ��ѯ������Ϣʧ��

	

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
		// ��ʼ¼��
		{	START_RECORD_ERROR,										"��ʼ¼��ʧ��"},
		//--------------------------------------------------------------------
		// ֹͣ¼��
		{	STOP_RECORD_ERROR,										"ֹͣ¼��ʧ��"},
		//--------------------------------------------------------------------
		// �ı�洢λ��
		{	CHANGE_STORAGE_ERROR,									"�ı�洢¼��λ��ʧ��"},
		//--------------------------------------------------------------------
		// ɾ���ļ�
		{	DELETE_FILE_ERROR,										"ɾ���ļ�ʧ��"},
		{	DELETE_FILE_NO_FILE,									"�Ҳ���Ҫɾ�����ļ�"},
		//--------------------------------------------------------------------
		// ��ѯ������Ϣ
		{	QUERY_DISK_INFO_ERROR,									"��ѯ������Ϣʧ��"}

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
	CMD_ID			: CMD_SLAVE_CAPABILITY_REQUEST 
	CMD_NAME		: CMD_SLAVE_CAPABILITY_REQUEST       
	DateTime		: 2012/9/5 15:26	
	Author 			: Jiangsx      
	Description		: �ӷ���������ȡ����
	Note			: 
	**************************************************************************************************/
	#define CMD_SLAVE_CAPABILITY_REQUEST  11

	/**************************************************************************************************
	CMD_ID			: CMD_SLAVE_CAPABILITY_RESPONSE 
	CMD_NAME		: CMD_SLAVE_CAPABILITY_RESPONSE       
	DateTime		: 2012/9/5 15:26	
	Author 			: Jiangsx      
	Description		: �ӷ���������ȡ�ظ�
	Note			: 
	**************************************************************************************************/
	#define CMD_SLAVE_CAPABILITY_RESPONSE  12
	
	// �ӷ��������ṹ����
	typedef struct StruSlaveCapability
	{
		INT32 iMaxRecordTasks;              // ���¼���������������ƻ�¼�񡢸澯¼���ֶ�¼��ȣ�
	}StruCapability, *StruCapabilityPtr;


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
		// �豸 �澯
	/*	ALARM_IO_START						= CmdProtocolDef::A_T_ALARM_IO_START,						// IO�澯��ʼ
		ALARM_IO_END						= CmdProtocolDef::A_T_ALARM_IO_END,							// IO�澯����

		ALARM_MD_START						= CmdProtocolDef::A_T_ALARM_MD_START,						// �ƶ����澯��ʼ
		ALARM_MD_END						= CmdProtocolDef::A_T_ALARM_MD_END,							// �ƶ����澯����

		ALARM_VIDEO_COVER_START				= CmdProtocolDef::A_T_ALARM_VIDEO_COVER_START,				// ��Ƶ�ڵ��澯��ʼ
		ALARM_VIDEO_COVER_END				= CmdProtocolDef::A_T_ALARM_VIDEO_COVER_END,				// ��Ƶ�ڵ��澯����

		ALARM_VIDEO_MISSING_START			= CmdProtocolDef::A_T_ALARM_VIDEO_MISSING_START,			// ��Ƶ��ʧ�澯��ʼ
		ALARM_VIDEO_MISSING_END				= CmdProtocolDef::A_T_ALARM_VIDEO_MISSING_END,				// ��Ƶ��ʧ�澯����

		ALARM_VIDEO_SIGNAL_ABNORMAL_START	= CmdProtocolDef::A_T_ALARM_VIDEO_SIGNAL_ABNORMAL_START,	// ��Ƶ�ź��쳣�澯��ʼ
		ALARM_VIDEO_SIGNAL_ABNORMAL_END		= CmdProtocolDef::A_T_ALARM_VIDEO_SIGNAL_ABNORMAL_END,		// ��Ƶ�ź��쳣�澯����

		ALARM_DISK_DAMAGE					= CmdProtocolDef::A_T_ALARM_DISK_DAMAGE,					// �豸�����𻵸澯
		ALARM_FLASH_FAULT					= CmdProtocolDef::A_T_ALARM_FLASH_FAULT,					// �豸FLASH���ϸ澯
		ALARM_DISK_FULL						= CmdProtocolDef::A_T_ALARM_DISK_FULL,						// �豸�������澯

		//--------------------------------------------------------------------------------------------------------------------
		// CLI �澯
		ALARM_MANUAL_START					= CmdProtocolDef::A_T_ALARM_MANUAL_START,					// CLI�ֶ��澯��ʼ
		ALARM_MANUAL_END					= CmdProtocolDef::A_T_ALARM_MANUAL_END,						// CLI�ֶ��澯����

		//--------------------------------------------------------------------------------------------------------------------
		// CSS �澯
		ALARM_STORAGE_FULL					= CmdProtocolDef::A_T_ALARM_STORAGE_FULL,					// CSS�洢�����澯
		ALARM_RW_DISK_ERROR					= CmdProtocolDef::A_T_ALARM_RW_DISK_ERROR					// CSS��д���̳���澯
*/
	}EnumAlarmType;

	/**************************************************************************************************
		CMD_ID		: CMD_START_RECORD_REQUEST
		CMD_NAME	: ��ʼ¼������
		DateTime	: 2010/11/15 16:45	
		Description	: ��ʼ¼������
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_START_RECORD_REQUEST				50			

	// �ļ���������
	typedef		enum	EnumFileNameType
	{
		F_N_T_BY_ID,						// �ļ���������Ϊ��ID
		F_N_T_BY_NAME						// �ļ���������Ϊ������
	}EnumFileNameType;

	// ¼����������
	typedef		enum	EnumRecordTastType
	{
		R_T_T_PLAN			=	0,			// �ƻ�¼������
		R_T_T_ALARM,						// �澯¼������
		R_T_T_DOWNLOAD,                      // ����¼������
		R_T_T_MANUAL,						//�ֶ�¼��
	}EnumRecordTastType;

	// ��ʼ¼������ṹ
	typedef		struct	StruCmdStartRecordRequest
	{
		UINT64		iCtrlIndex;							// ¼�����Ψһ����
		INT32		iPlatformID;						// ¼��ƽ̨ID
		INT32		iDeviceID;							// ¼���豸ID
		INT32		iChannelID;							// ¼��ͨ��ID
		char		szDevName[MAX_NAME_LEN];			// ¼���豸����
		char		szChnName[MAX_NAME_LEN];			// ¼��ͨ������
		INT32		iRecordTaskType;					// ¼���������ͣ��ο�EnumRecordTastType�ṹ
		INT32       iStreamType;                        // �ο�ƽ̨�������Ͷ��� CmdProtocolDef::EnumStreamType

		INT32		iAlarmType;							// �澯���Ͳο�EnumAlarmType������Ǽƻ�¼����Ϊ��NO_ALARM
		INT32		iAlarmPlatformID;					// �澯ƽ̨ID
		INT32		iAlarmDeviceID;						// �澯�豸ID
		INT32		iAlarmChannelID;					// �澯ͨ��ID
        char		szAlarmDevName[MAX_NAME_LEN];		// �澯�豸����
		char		szAlarmChnName[MAX_NAME_LEN];		// �澯ͨ������
		CmdProtocolDef::StruDateTime	stAlarmStartDateTime;	// �澯ʱ�俪ʼʱ��
        UINT64      iAlarmIndex;                        // �澯����

		INT32		iUsedName;							// �ļ���ʹ��������Ϣ���ο��ṹEnumFileNameType
		INT32		iMinLenTM;							// ���¼��ʱ�䣬��λ������
		INT32		iMaxLenTM;							// �¼��ʱ��  -1 ��ʾ���޳�����λ������
		INT32		iFileLenTM;							// ÿ���ļ���¼��ʱ�䳤�ȣ���λ������
		char		szStorageUUID[MAX_NAME_LEN];		// ¼��Ĵ�����������ID 

		CmdProtocolDef::StruDateTime	stStartTime;	// ��ʼʱ��
		CmdProtocolDef::StruDateTime	stEndTime;	    // ����ʱ��

		INT32    bFileIndex; //�ļ������Ƿ�ʹ��
		UINT64   iFileIndex;  //�ļ�����
		INT32	 iStorageGroupID; //�洢����ID
       
	}StruCmdStartRecordRequest,*StruCmdStartRecordRequestPtr;


	/**************************************************************************************************
		CMD_ID		: CMD_START_RECORD_RESPONSE
		CMD_NAME	: ��ʼ¼��ظ�
		DateTime	: 2010/11/15 17:56	
		Description	: ��ʼ¼��ظ�����SLAVER��ʼ¼�����Ժ󣬻ظ�MASTER������
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_START_RECORD_RESPONSE				51			

	// ��ʼ¼��ظ�����ṹ��
	typedef		struct	StruCmdStartRecordResponse 
	{
		INT32		iOperResult;						// ����������ο�EnumErrorCode
		UINT64		iCtrlIndex;							// ¼�����Ψһ����	
	}StruCmdStartRecordResponse,*StruCmdStartRecordResponsePtr;


	/**************************************************************************************************
		CMD_ID		: CMD_STOP_RECORD_REQUEST
		CMD_NAME	: ¼��ֹͣ����
		DateTime	: 2010/11/15 18:02	
		Description	: ¼��ֹͣ����
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_STOP_RECORD_REQUEST					52

	// ֹͣ¼������
	typedef		struct StruCmdStopRecordRequest
	{
		UINT64		iCtrlIndex;							// ¼�����Ψһ����
	}StruCmdStopRecordRequest,*StruCmdStopRecordRequestPtr;


	/**************************************************************************************************
		CMD_ID		: CMD_STOP_RECORD_RESPONSE
		CMD_NAME	: ֹͣ¼��ظ�
		DateTime	: 2010/11/15 18:44	
		Description	: ¼��ֹͣ����ظ�
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_STOP_RECORD_RESPONSE					53

	// ֹͣ¼��ظ�����
	typedef		struct	StruCmdStopRecordResponse 
	{
		INT32		iOperResult;						// ����������ο�EnumErrorCode
		UINT64		iCtrlIndex;							// ¼�����Ψһ����
	}StruCmdStopRecordResponse,*StruCmdStopRecordResponsePtr;


	/**************************************************************************************************
		CMD_ID		: CMD_CHANGE_STORAGE_REQUEST
		CMD_NAME	: �ı�洢λ������
		DateTime	: 2010/11/15 18:02	
		Description	: ¼�����Ǩ��������¾ɴ�����ͬһSLAVE�½���
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_CHANGE_STORAGE_REQUEST					54

	// �ı�洢λ����������ṹ
	typedef		struct StruCmdChangeStorageRequest
	{
		UINT64		iCtrlIndex;							// ¼�����Ψһ����
		char		szStorageUUID[MAX_NAME_LEN];		// ����uuid��־
	}StruCmdChangeStorageRequest,*StruCmdChangeStorageRequestPtr;


	/**************************************************************************************************
		CMD_ID		: CMD_CHANGE_STORAGE_RESPONSE
		CMD_NAME	: �ı�洢λ������ظ�
		DateTime	: 2010/11/15 18:52	
		Description	: �ı�洢λ������ظ�
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_CHANGE_STORAGE_RESPONSE					55

	// �ı�洢��������ظ������Ӧ���ݽṹ
	typedef		struct	StruCmdChangeStorageResponse 
	{
		INT32		iOperResult;						// ����������ο�EnumErrorCode
		UINT64		iCtrlIndex;							// ¼�����Ψһ����
	}StruCmdChangeStorageResponse,*StruCmdChangeStorageResponsePtr;

	
	/**************************************************************************************************
		CMD_ID		: CMD_DELETE_FILE_REQUEST
		CMD_NAME	: ɾ��¼���ļ�����
		DateTime	: 2010/11/15 18:02	
		Description	: ɾ��¼���ļ���������
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_DELETE_FILE_REQUEST						56

	//ɾ��¼���ļ���Ӧ�ṹ,����Ϊ��
	
	/**************************************************************************************************
		CMD_ID		: CMD_DELETE_FILE_RESPONSE
		CMD_NAME	: ɾ���ļ��ظ�����
		DateTime	: 2010/11/15 19:01	
		Description	: ɾ���ļ��ظ�����
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_DELETE_FILE_RESPONSE					57

	// ɾ���ļ���Ӧ�ظ�����Ϊ��

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
		CMD_ID		: CMD_RESOURCES_INFO_QUERY_REQUEST
		CMD_NAME	: ��Դ��Ϣ��ѯ��������
		DateTime	: 2010/11/15 18:03	
		Description	: ��Դ��Ϣ��ѯ��������
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_RESOURCES_INFO_QUERY_REQUEST					200

	// ��Դ��Ϣ��ѯ��������ṹ,������������

	/**************************************************************************************************
		CMD_ID		: CMD_RESOURCES_INFO_QUERY_RESPONSE
		CMD_NAME	: ��Դ��Ϣ��ѯ�ظ�����
		DateTime	: 2010/11/15 19:25	
		Description	: ��Դ��Ϣ��ѯ�ظ�����
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_RESOURCES_INFO_QUERY_RESPONSE				201

	// ���̿���ʹ�ñ�־
	typedef		enum	EnumDiskUseFlag
	{
		D_U_F_NO			= 0	,			// ������ʹ��
		D_U_F_YES							// ����ʹ��
	}EnumDiskUseFlag;

	// ��������ö��
	typedef	enum	EnumDiskLocationType
	{
		DISK_LOCAL			=	1,					// ���ش���
		DISK_NAS,									// NAS
		DISK_BACKUP                                 // ������
	}EnumDiskLocationType;

	// ������Ϣ��ѯ����ṹ��Ϣ�ظ�����
	typedef		struct StruDiskQueryResponseItem
	{
		INT32		iOperResult;							// ����������ο�EnumErrorCode
		char		szStorageUUID[MAX_NAME_LEN];			// ����UUIDΨһ��־
		UINT64		iTotalSize;								// ���̿ռ��ܴ�С����λ��MB
		UINT64		iTotalFree;								// ����ʣ��ռ��С,��λ��MB
		INT16		iUseRate;								// ����ʹ�ðٷֱ�[0-100]	
		INT16		iCanUseFlag;							// �Ƿ����ʹ�ñ�־���ο�	EnumDiskUseFlag
		INT16       iDiskType;                              // �������ͣ��ο�  EnumDiskLocationType
		char        szDisk[MAX_DISK_PARTITION_LEN+32];      // ���̱�ʶ(�� "c:/"����"//hame//...")
	}StruDiskQueryResponseItem,*StruDiskQueryResponseItemPtr;

	// ��Դ��Ϣ��ѯ�ظ��ṹ
	typedef		struct StruCmdResourcesQueryResponse 
	{
		UINT32                          iWorkingSetSize;                           // �ڴ�    (��λ:MB)
		UINT32                          iPagefileUsage;                            // �����ڴ�(��λ:MB)
		UINT32							iCpuUsage;								   // CPUʹ����
		INT32						    iDiskNum;						           // �ڵ����
		StruDiskQueryResponseItem	    stDiskQueryResponseItem[1];		           // ������Ϣ��ѯ�ṹ
	}StruCmdResourcesQueryResponse,*StruCmdResourcesQueryResponsePtr;


	/**************************************************************************************************
		CMD_ID		: CMD_TASK_SYNCHRONIZATION
		CMD_NAME	: CMD_TASK_SYNCHRONIZATION
		DateTime	: 2010/12/3 10:43	
		Description	: ¼��������Ϣͬ��
		Author		: Liujs
		Note		: NULL
	**************************************************************************************************/
	#define		CMD_TASK_SYNCHRONIZATION					250

	// ��������Ҫ����
	
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

	/**************************************************************************************************
	CMD_ID		: CMD_RECORD_FILE_OPERATE_REQUEST
	CMD_NAME	: CMD_RECORD_FILE_OPERATE_REQUEST
	DateTime	: 2011/02/23 16:19	
	Description	: ¼��ط���������
	Author		: CHC
	Note		: NULL
	**************************************************************************************************/
	#define		CMD_RECORD_FILE_DOWN_REPLAY_REQUEST				350
	#define     CMD_RECORD_FILE_DOWN_REPLAY_REQUEST_EX          352

	//¼���ļ���������ö�ٽṹ
	typedef		enum	EnumRecordOperType
	{
		RECORD_FILE_REPLAY,		//�ļ��ط�
		RECORD_FILE_DOWNLOAD,	//�ļ�����
		RECORD_TIME_REPLAY,		// ¼��ʱ��ط�
		RECORD_TIME_DOWNLOAD	// ¼��ʱ������
	};

	// ¼���ļ���Ϣ�ṹ
	typedef struct StruRecFileInfo
	{
		char  strFileTime[MAX_ID_STRING_LEN];		// ¼���ļ�ʱ�䣺 ��ʼʱ��ͽ���ʱ�䣨�� 2011-01-01-00-00-00,2011-01-01-01-00-00��
		char  szRcdFileName[MAX_NAME_LEN_256];		// ¼���ļ�����
		char  szStorageUUID[64];					// ¼��Ĵ�����������ID 
	} StruRecFileInfo, *StruRecFileInfoPtr;

	//¼���ļ�������������ṹ
	typedef struct StruRecordFileOperRequest
	{
		INT32 iOperType;							// �������ͣ���ö��EnumRecordOperType����ʾ��¼���ļ����еĲ������ͣ�0	��ʾ�ļ��طţ�1	��ʾ�ļ����أ�2 ��ʾʱ��طţ�3 ��ʾʱ������
		INT32 iVersion;								// ��Ҫ�����ڼ��ݲ�ͬ�洢����¼���ļ�ID��ͬ��ʾ��������ʱ��0
        
        INT32 iCliPlatformID;						// ������������Ŀͻ��˵�ƽ̨ID  **�ڻظ��д���
        INT32 iCliUserID;							//  ��������������豸ID    **�ڻظ��д���
		char  szIPAddr[MAX_IP_ADDR_LEN];			// �ͻ���IP��ַ
        
        INT32 iPlatformID;							// ƽ̨ID
		INT32 iDevID;								// �豸ID
		INT32 iChnID;								// ͨ��ID

		INT16 iStorageType;							// �洢���ͣ��ο�ö������EnumStorageType   **�ڻظ��д���
		INT16 iReserve;								// ����

		char  strRcdFileID[MAX_ID_STRING_LEN];		// �ļ���¼���ļ�ID  ʱ�䣺 ��ʼʱ��ͽ���ʱ�䣨�� 2011-01-01-00-00-00,2011-01-01-01-00-00�� **�ڻظ��д���

		INT16 iFileNum;                             // �ļ�����
		StruRecFileInfo stFileInfo[1] ;             // ¼���ļ���Ϣ�ṹ

	}StruRecordFileOperRequest,*StruRecordFileOperRequestPtr;

	/**************************************************************************************************
	CMD_ID		: CMD_RECORD_FILE_OPERATE_RESPONSE
	CMD_NAME	: CMD_RECORD_FILE_OPERATE_RESPONSE
	DateTime	: 2011/02/23 16:19	
	Description	: ¼��ط����ػظ�
	Author		: CHC
	Note		: NULL
	**************************************************************************************************/
	#define		CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE			351
	#define		CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE_EX			353

	//¼���ļ���������ظ�����ṹ
	typedef struct StruRecordFileOperResponse
	{
		INT32 iOperResult;					    // �ظ����,ö��EnumErrorCode����
		INT32 iOperType;						// �������ͣ���ö��EnumRecordOperType����ʾ��¼���ļ����еĲ������ͣ�0	��ʾ�ļ��طţ�1	��ʾ�ļ����أ�2 ��ʾʱ��طţ�3 ��ʾʱ������
		INT32 iVersion;							// ��Ҫ�����ڼ��ݲ�ͬ�洢����¼���ļ�ID��ͬ��ʾ��������ʱ��0
        
        INT32 iCliPlatformID;						// ������������Ŀͻ��˵�ƽ̨ID
        INT32 iCliUserID;							//  ��������������豸ID

        char  strRcdFileID[MAX_ID_STRING_LEN];	// ¼���ļ�ID	
		char  szRecFileURI[MAX_URI_LEN];		// ¼���ļ���Դ��ַ
		INT16 iStorageType;						// �洢���ͣ��ο�ö������EnumStorageType
		INT16 iReserve;							// ����
	}StruRecordFileOperResponse,*StruRecordFileOperResponsePtr;


	/**************************************************************************************************
	CMD_ID		: CMD_BACKUP_FILE_SEARCH_REQUEST
	CMD_NAME	: CMD_BACKUP_FILE_SEARCH_REQUEST
	DateTime	: 2012/03/01 14:36	
	Description	: �����ļ���������
	Author		: Jiangsx
	Note		: 
	**************************************************************************************************/
    #define CMD_BACKUP_FILE_SEARCH_REQUEST     381


	// �ļ�״̬��Ϣö��
	typedef enum EnumFileStatusMark
	{
		INIT_STATE     = 0x00,          // ��ʼ״̬
		LOCKED         = 0x01,          // ������
		NEED_BACKUP    = 0x02,          // ��Ҫ����
		ALREADY_BACKUP = 0x04           // �ѱ���

		// to add: ����Ӹ�״̬����Ҫͬ���޸�Master��Slave�õ���SQL���
	};

	// �����ļ���������ṹ
	typedef CmdProtocolDef::StruRecordFileRetrievalRequest StruBackupFileSearchReuqest;


	/**************************************************************************************************
	CMD_ID		: CMD_BACKUP_FILE_SEARCH_RESPONSE
	CMD_NAME	: CMD_BACKUP_FILE_SEARCH_RESPONSE
	DateTime	: 2012/03/01 14:36	
	Description	: �����ļ������ظ�
	Author		: Jiangsx
	Note		: 
	**************************************************************************************************/
	 #define CMD_BACKUP_FILE_SEARCH_RESPONSE     382

	// �����ļ������ظ��ṹ
	typedef CmdProtocolDef::StruRecordFileRetrievalResponse StruBackupFileSearchResponse;


	/**************************************************************************************************
	CMD_ID		: CMD_FILE_BACKUP_OPER_REQUEST
	CMD_NAME	: CMD_FILE_BACKUP_OPER_REQUEST
	DateTime	: 2012/03/01 14:36	
	Description	: �ļ����ݲ�������
	Author		: Jiangsx
	Note		: 
	**************************************************************************************************/
	 #define CMD_FILE_BACKUP_OPER_REQUEST     383

	// �ļ����ݲ�������ö��
	 typedef enum EnumFileBackupOperType
	 {
		BF_BACKUP = 0,       // ����
		BF_RESUME,           // �ָ�
		BF_DELETE            // ɾ��
	 };

	// �洢��Ϣ�ṹ����
	typedef struct StruStorageInfo 
	{
		INT8  bCover;                               // �Ƿ񸲸�
		char  szRcdFileID[MAX_ID_STRING_LEN];	    // �ļ�ID	
		char  szStorageUUID[64];					// �ļ��Ĵ�����������ID 
	} StruStorageInfo, *StruStorageInfoPtr;
	
	// �ļ����ݲ�������ṹ
	typedef struct StruFileBackupOperRequest
	{
		INT32 iOperType;                     // �������ͣ��ο�ö��EnumFileBackupOperType
		INT32 iNum;                          // �洢��Ϣ����
		StruStorageInfo stStorageInfo[1];    // �ο��ṹStruStorageInfo
	} StruFileBackupOperRequest, *StruFileBackupOperRequestPtr;


	/**************************************************************************************************
	CMD_ID		: CMD_FILE_BACKUP_OPER_RESPONSE
	CMD_NAME	: CMD_FILE_BACKUP_OPER_RESPONSE
	DateTime	: 2012/03/01 14:36	
	Description	: �ļ����ݲ����ظ�
	Author		: Jiangsx
	Note		: 
	**************************************************************************************************/
	 #define CMD_FILE_BACKUP_OPER_RESPONSE     384
	
	typedef struct StruFileBackupOperResponse 
	{
		INT32 iOperType;    // �������ͣ��ο�ö��EnumFileBackupOperType
		INT32 iResult;      // �ظ����,ö��EnumErrorCode����
	} StruFileBackupOperResponse, *StruFileBackupOperResponsePtr;




	/*
	¼��������� ���� PTN�� �ֶ�¼�� �澯¼��
	*/
	


	//¼������������������ṹ
	 #define CMD_RECORD_TASK_REQUEST     385
	typedef struct StruRecordTaskRequest
	{
		INT64 iTaskID;							    //����ID
		INT32 iTaskType;							// �������� �ο� EnumRecordTastType
		INT32 iCliPlatformID;						// ������������Ŀͻ��˵�ƽ̨ID  **�ڻظ��д���
		INT32 iCliUserID;							//  ��������������豸ID    **�ڻظ��д���
		INT32 iCmdSN;								//�������,**�ڻظ��д���
		INT32 iSessionID;							//�������,**�ڻظ��д���
		
		INT32 iPlatformID;							// ƽ̨ID
		INT32 iDevID;								// �豸ID
		INT32 iChnID;								// ͨ��ID

		INT32 iRcdTimeLen;							//¼��ʱ�䳤�ȣ� ��λ ����, �ֶ�¼�񣬺͸澯¼����Ч
		//��������
		INT32 iArgStrLen;							

		unsigned char szArg[1];  //����
		/*
		 ΪPTN ����ʱΪ��   ��ʼʱ��;����ʱ��  yyyy-mm-ss hh24:mi:ss;yyyy-mm-ss hh24:mi:ss
		 Ϊ�澯¼��ʱ ��ʾ�澯����  : KeyName$Value&KeyName$Value 
		KeyName :
		AlType �澯����; AlPmsId �澯ƽ̨ID; AlDevId �澯�豸ID; AlChn �澯ͨ��ID;
		AlIdx  �澯����; AlDevType �澯�豸����;AlChnType �澯ͨ������;
		AlLevel �澯����; AlStart �澯��ʼʱ��(yyyy-mm-ss hh24:mi:ss);
		AlDevName �澯�豸����; AlChnName �澯ͨ������
		*/

	}StruRecordTaskRequest,*StruRecordTaskRequestPtr;


	//¼��������������ظ��ṹ
	 #define CMD_RECORD_TASK_RESPONSE     386
	typedef struct StruRecordTaskResponse
	{
		INT32 iOperResult;					    // �ظ����,ö��EnumErrorCode����
		INT32 iTaskType;						// �������� �ο� EnumRecordTastType
		INT64 iTaskID;							 //����ID		

		//¼���ƽ̨ͨ��
		INT32 iPlatformID;							// ƽ̨ID
		INT32 iDevID;								// �豸ID
		INT32 iChnID;								// ͨ��ID


		INT32 iCmdSN;							//������������
		INT32 iCliPlatformID;					// ������������Ŀͻ��˵�ƽ̨ID
		INT32 iCliUserID;						//  ��������������豸ID
		INT32 iSessionID;						//������������	
		char  strRcdFileID[MAX_ID_STRING_LEN];	//¼����ļ�ID�� �ֶ�¼����Ҫ����
	}StruRecordTaskResponse,*StruRecordTaskResponsePtr;


	//ֹͣ¼��������������ṹ
	#define CMD_STOP_RECORD_TASK_REQUEST    387
	typedef struct StruStopRecordTaskRequest
	{
		INT64 iTaskID;					    //¼�������ID
		INT32 iCmdSN;							//������������
		INT32 iCliPlatformID;					// ������������Ŀͻ��˵�ƽ̨ID
		INT32 iCliUserID;						//  ��������������豸ID
		INT32 iSessionID;						//������������
	}StruStopRecordTaskRequest,*StruStopRecordTaskRequestPtr;


	//ֹͣ¼����������ظ�����ṹ
	#define CMD_STOP_RECORD_TASK_RESPONSE    388
	typedef struct StruStopRecordTaskResponse
	{
		INT64 iTaskID;					    //¼�������ID
		INT32 iResult;							//���
		INT32 iCmdSN;							//������������
		INT32 iCliPlatformID;					// ������������Ŀͻ��˵�ƽ̨ID
		INT32 iCliUserID;						//  ��������������豸ID
		INT32 iSessionID;						//������������
	}StruStopRecordTaskResponse,*StruStopRecordTaskResponsePtr;


	//�����澯
	#define CMD_DISK_FULL_ALARM    389
	typedef struct _StruDiskFullAlarm
	{		
		//�洢����ID
		INT64 iRcdStoragePly;
		//�洢������
		unsigned char czPlyName[256];
	}StruDiskFullAlarm;



	typedef	enum
	{
		// ��½ 
		E_CMD_LOGIN_REQUEST					=	CMD_LOGIN_REQUEST,				// ��½����
		E_CMD_LOGIN_RESPONSE				=	CMD_LOGIN_RESPONSE,				// ��½Ӧ������

		E_CMD_SLAVE_CAPABILITY_REQUEST      =   CMD_SLAVE_CAPABILITY_REQUEST,   // �ӷ���������ȡ����
		E_CMD_SLAVE_CAPABILITY_RESPONSE     =   CMD_SLAVE_CAPABILITY_RESPONSE,  // �ӷ���������ȡ�ظ�

		// ����
		E_CMD_GET_STREAM_REQUEST			=	CMD_GET_STREAM_REQUEST,			// ��������
		E_CMD_GET_STREAM_RESPONSE			=	CMD_GET_STREAM_RESPONSE,		// ����Ӧ������

		// ������
		E_CMD_START_RECORD_REQUEST			=	CMD_START_RECORD_REQUEST,		// ¼����������
		E_CMD_START_RECORD_RESPONSE			=	CMD_START_RECORD_RESPONSE,		// ¼������ظ�

		E_CMD_STOP_RECORD_REQUEST			=	CMD_STOP_RECORD_REQUEST,		// ֹͣ¼��
		E_CMD_STOP_RECORD_RESPONSE			=	CMD_STOP_RECORD_RESPONSE,		// ֹͣ¼��ظ�
		
		E_CMD_CHANGE_STORAGE_REQUEST		=	CMD_CHANGE_STORAGE_REQUEST,		// �ı�洢λ��
		E_CMD_CHANGE_STORAGE_RESPONSE		=	CMD_CHANGE_STORAGE_RESPONSE,	// �ı�洢λ��

		E_CMD_DELETE_FILE_REQUEST			=	CMD_DELETE_FILE_REQUEST,		// ɾ���ļ�����
		E_CMD_DELETE_FILE_RESPONSE			=	CMD_DELETE_FILE_RESPONSE,		// ɾ���ļ�����ظ�

		E_CMD_RESOURCES_INFO_QUERY_REQUEST		=	CMD_RESOURCES_INFO_QUERY_REQUEST,	// ��Դ��Ϣ��ѯ����
		E_CMD_RESOURCES_INFO_QUERY_RESPONSE		=	CMD_RESOURCES_INFO_QUERY_RESPONSE,	// ��Դ��Ϣ��ѯ�ظ�		

		E_CMD_TASK_SYNCHRONIZATION			=	CMD_TASK_SYNCHRONIZATION,		// ¼��������Ϣͬ��
	
		E_CMD_TIME_SYNCHRONIZATION			=	CMD_TIME_SYNCHRONIZATION,		// ʱ��ͬ������

		E_CMD_ALARM_NOTICE					=	CMD_ALARM_NOTICE,				// �澯֪ͨ

		E_CMD_RECORD_FILE_DOWN_REPLAY_REQUEST	    =	CMD_RECORD_FILE_DOWN_REPLAY_REQUEST,	// ¼��ط���������
		E_CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE	    =	CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE,	// ¼��ط����ػظ�
		E_CMD_RECORD_FILE_DOWN_REPLAY_REQUEST_EX	=	CMD_RECORD_FILE_DOWN_REPLAY_REQUEST_EX,	// ¼��ط���������
		E_CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE_EX	=	CMD_RECORD_FILE_DOWN_REPLAY_RESPONSE_EX,// ¼��ط����ػظ�

		E_CMD_BACKUP_FILE_SEARCH_REQUEST  = CMD_BACKUP_FILE_SEARCH_REQUEST,    // �����ļ�����
		E_CMD_BACKUP_FILE_SEARCH_RESPONSE = CMD_BACKUP_FILE_SEARCH_RESPONSE,   // �����ļ������ظ�
		E_CMD_FILE_BACKUP_OPER_REQUEST    = CMD_FILE_BACKUP_OPER_REQUEST,      // �ļ����ݲ���
		E_CMD_FILE_BACKUP_OPER_RESPONSE   = CMD_FILE_BACKUP_OPER_RESPONSE,     // �ļ����ݲ����ظ�


		E_CMD_RECORD_TASK_REQUEST = CMD_RECORD_TASK_REQUEST, //¼������ ���ƻ����¼������
		E_CMD_RECORD_TASK_RESPONSE = CMD_RECORD_TASK_RESPONSE, //¼������ ���ƻ����¼������
		E_CMD_STOP_RECORD_TASK_REQUEST = CMD_STOP_RECORD_TASK_REQUEST, //ֹͣ¼������
		E_CMD_STOP_RECORD_TASK_RESPONSE = CMD_STOP_RECORD_TASK_RESPONSE,
		E_CMD_DISK_FULL_ALARM = CMD_DISK_FULL_ALARM,

	}EnumCmdType;

}


#ifdef _WIN32
#pragma		pack(pop)
#endif

//���ݰ汾
//�豸��Ϣ
#define CSS_DATAVERKEY_DEVINFO   "aDevInfo"
//�豸״̬
#define CSS_DATAVERKEY_DEVSTATUS   "aDevStatu"
//�ƻ�¼��ƻ�
#define CSS_DATAVERKEY_PLANRCDPOLICY   "aPlanPly"
//�洢����
#define CSS_DATAVERKEY_STORAGEPOLICY   "aStgPly"

//����������Ϣ
#define CSS_DATAVERKEY_DISKCONFIG "aDiskCfg"

//Slaver ������Ϣ
#define CSS_DATAVERKEY_SLAVERCONFIG "aSlaveCfg"
//ǿ��ͬ������
#define CSS_DATAVERKEY_FORCE_SYNCDATA "aFSync"

#endif 
// CSSPROTOCOLDEF_DEF_H
