#include"CSimpleLog.h"
#include <direct.h>
#include <time.h>


std::string GetAppPath()
{

	char szPath[MAX_PATH];
	memset(szPath, 0, MAX_PATH);


	DWORD ret = GetModuleFileNameA(NULL, szPath, MAX_PATH);
	if (0 == ret)
	{
		return ".\\";
	}


	std::string strPath = szPath;
	return strPath.substr(0, strPath.rfind("\\")) + "\\";

}


CSimpleLog::CSimpleLog():m_logdir(""),m_fp(NULL)
{

	::InitializeCriticalSection(&m_crit);   //��ʼ���ٽ���

}


CSimpleLog::~CSimpleLog()
{
	if(m_fp)
	{
		fclose(m_fp);
		m_fp = NULL;
	}

	::DeleteCriticalSection(&m_crit);    //�ͷ����ٽ���
}



int CSimpleLog::SetLogPath(char* chPath)
{



	m_logdir = chPath;

	return 0;
}

void CSimpleLog::Add(const char* fmt, ...)
{



	if(!m_fp)
	{

		// ���õ�ǰĿ¼Ϊ��ý��������Ŀ¼
		std::string app_path = GetAppPath();

		app_path = app_path + m_logdir;

		_mkdir(app_path.c_str());


		struct tm *now;
		time_t ltime;

		time(&ltime);
		now = localtime(&ltime);

		char chFile[512] = {0};
	
		

		sprintf_s(chFile, "%s\\Log_%d_%d_%d %02d_%02d_%02d.log", app_path.c_str()
			, now->tm_year+1900, now->tm_mon+1, now->tm_mday,now->tm_hour,now->tm_min,now->tm_sec);


		m_fp = fopen(chFile, "w+"); //����ӵķ�ʽ������ļ�
		if (!m_fp)
			return;

	}



	/*-----------------------�����ٽ���(д�ļ�)------------------------------*/	
	::EnterCriticalSection(&m_crit);   
	try      
	{
		va_list argptr;          //�����ַ����ĸ�ʽ
		va_start(argptr, fmt);
		_vsnprintf(m_tBuf, BUFSIZE, fmt, argptr);
		va_end(argptr);
	}
	catch (...)
	{
		m_tBuf[0] = 0;
	}


	char szDate[64] = {0};
	char szTime[64] = {0};
	_strdate_s(szDate);
	_strtime_s(szTime);

	fprintf(m_fp,"%s %s\t", szDate, szTime);
	fprintf(m_fp, "%s\n", m_tBuf);	
	fflush(m_fp);

	::LeaveCriticalSection(&m_crit);  
	/*----------------------------�˳��ٽ���---------------------------------*/	

}