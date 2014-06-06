#include "Config.h"
#include "Service.h"

using namespace JOU;

#define LOG_SUBDIR   "JouLog"

CConfig::CConfig(void)
:CJouObj()
{
	m_strIniFilename =  "./JouConf.ini";

	m_iLogLevelConsole = CLog::LV_ALL;
	m_iLogLevelFile = (CLog::LV_FATAL|CLog::LV_ERROR|CLog::LV_WARN|CLog::LV_DEBUG);
	m_strLogDir = "./";
	m_strLogDir += LOG_SUBDIR; 

	m_strDBHostname = "127.0.0.1";
	m_strDBName = "C3M_VIDEO";
	m_strDBUser = "video01";
	m_strDBPWD = "video01";
	m_eDbaseType = (INT) DBAccessModule::ORACLE;

	//������־ ɾ����ʽ
	m_iRcdOperDays =  240;   //���������죬 0 ����Ч, Ĭ�� 240
//	m_iRcdOperRows = 5000000;   //������������ 0 ����Ч, Ĭ�� 5,000,000

	//������־ ɾ����ʽ
	m_iRcdStatusDays = 120;   //���������죬 0 ����Ч, Ĭ�� 120
//	m_iRcdStatusRows = 1000000;   //������������ 0 ����Ч, Ĭ�� 1,000,000

	//��½��־ ɾ����ʽ
	m_iRcdLoginDays = 240;   //���������죬 0 ����Ч, Ĭ�� 240
//	m_iRcdLoginRows = 5000000;   //������������ 0 ����Ч, Ĭ�� 5,000,000

	//���湦��
	m_strCachePath = "./JouData";
	m_bEnableCache = 1;

	m_iCacheInterval = 20;
	
}

CConfig::~CConfig(void)
{
}


void CConfig::LoadConfig( const char *czConfFilename)
{
	CConfigFile		csCfg;
	if( !csCfg.LoadFile( (char*)czConfFilename) )
	{
		MY_ERROR("Config load file: %s fail.\n", czConfFilename);
		GS_ASSERT(0);
		return ;
	}
	m_strIniFilename = czConfFilename;

	// ��ȡ��־���������Ϣ

	int iValue = 0;

	m_iLogLevelConsole = 0;
	if( csCfg.GetProperty("LogLevelConsole", "Fatal", 1) )
	{
		iValue |= CLog::LV_FATAL;
	}
	if( csCfg.GetProperty("LogLevelConsole", "Err", 1) )
	{
		iValue |= CLog::LV_ERROR;
	}
	if( csCfg.GetProperty("LogLevelConsole", "Warnnig",1) )
	{
		iValue |= CLog::LV_WARN;
	}
	if( csCfg.GetProperty("LogLevelConsole", "Debug",1) )
	{
		iValue |= CLog::LV_DEBUG;
	}
	if( csCfg.GetProperty("LogLevelConsole", "Info",1) )
	{
		iValue |= CLog::LV_INFO;
	}
	m_iLogLevelConsole = iValue;

	iValue = 0;
	if( csCfg.GetProperty("LogLevelFile", "Fatal", 1) )
	{
		iValue |= CLog::LV_FATAL;
	}
	if( csCfg.GetProperty("LogLevelFile", "Err", 1) )
	{
		iValue |= CLog::LV_ERROR;
	}
	if( csCfg.GetProperty("LogLevelFile", "Warnnig",1) )
	{
		iValue |= CLog::LV_WARN;
	}
	if( csCfg.GetProperty("LogLevelFile", "Debug",1) )
	{
		iValue |= CLog::LV_DEBUG;
	}
	if( csCfg.GetProperty("LogLevelFile", "Info",1) )
	{
		iValue |= CLog::LV_INFO;
	}
	m_iLogLevelFile = iValue;	

	//��־·��
	 m_strLogDir = csCfg.GetProperty("LogPath", "AbsolutePath", "`");
	 if( m_strLogDir=="`")
	 {
		//û��ʹ�þ���·��
		m_strLogDir = GSGetApplicationPath();
		 std::string strSubDir = csCfg.GetProperty("LogPath", "SubPath", LOG_SUBDIR);
		 m_strLogDir += strSubDir;
	 }
	 m_strLogDir += "/";
	 GSPathParser(m_strLogDir);
	 GSTestAndCreateDir(m_strLogDir.c_str()); //����Ŀ¼


	//��ȡ���ݿ�����
	m_strDBHostname = csCfg.GetProperty("JOU_DB_PARAM", "DB_SERVER_ADDR",
										"127.0.0.1");
	m_strDBName = csCfg.GetProperty("JOU_DB_PARAM", "DB_NAME",
										"C3M_VIDEO");
	m_strDBUser = csCfg.GetProperty("JOU_DB_PARAM", "DB_USER_NAME",
										"video01");
	m_strDBPWD = csCfg.GetProperty("JOU_DB_PARAM", "DB_USER_PWD",
										"video01");
	m_eDbaseType = csCfg.GetProperty("JOU_DB_PARAM", "DB_TYPE",
						m_eDbaseType);

	//��־��������

	//������־ ɾ����ʽ
	m_iRcdOperDays = csCfg.GetProperty("JOUOPERATOR", "DAYS",m_iRcdOperDays );
//	m_iRcdOperRows = csCfg.GetProperty("JOUOPERATOR", "ROWS",m_iRcdOperRows );

	//������־ ɾ����ʽ
	m_iRcdStatusDays = csCfg.GetProperty("JOURUNSTATUS", "DAYS",m_iRcdStatusDays );
//	m_iRcdStatusRows = csCfg.GetProperty("JOURUNSTATUS", "ROWS",m_iRcdStatusRows );

	//������־ ɾ����ʽ
	m_iRcdLoginDays = csCfg.GetProperty("JOULOGIN", "DAYS",m_iRcdLoginDays );
//	m_iRcdLoginRows = csCfg.GetProperty("JOULOGIN", "ROWS",m_iRcdLoginRows );

	//��������

	//ʹ��
	m_bEnableCache = csCfg.GetProperty("Cache", "ENABLE", m_bEnableCache);
	
	//��������Ŀ¼
	m_strCachePath = csCfg.GetProperty("Cache", "AbsolutePath", "`");
	if( m_strCachePath=="`")
	{
		//û��ʹ�þ���·��
		m_strCachePath = GSGetApplicationPath();
		std::string strSubDir = csCfg.GetProperty("Cache", "SubPath", "JouData");
		m_strCachePath += strSubDir;
	}
	m_strCachePath += "/";
	GSPathParser(m_strCachePath);
	GSTestAndCreateDir(m_strCachePath.c_str() ); //����Ŀ¼

	m_iCacheInterval = csCfg.GetProperty("Cache", "Interval", m_iCacheInterval);
	

}
