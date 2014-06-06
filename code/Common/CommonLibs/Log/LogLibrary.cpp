#include "LogLibrary.h"

#include <time.h>
#include <math.h>
#include <assert.h>

#ifdef WINCE
#include <windows.h>
#include <stdarg.h>
#elif _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/vfs.h>    // or <sys/statfs.h>
#endif

#ifdef WINCE
#define ACCESS _access
#define MKDIR(a) _mkdir((a),NULL)
static int _mkdir(char* path,LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	WCHAR wszDirectory[MAX_PATH];
	int nResult = MultiByteToWideChar(
		CP_ACP,    
		MB_PRECOMPOSED,
		path,
		strlen(path)+1,
		wszDirectory,
		sizeof(wszDirectory)/sizeof(wszDirectory[0]));
	if(0 == nResult)
	{
		return nResult;
	}
	return CreateDirectory(wszDirectory,lpSecurityAttributes) ? 0 : -1 ;
}
#define vsnprintf _vsnprintf

static int _access(const char* czPathName,int flag)
{

	WCHAR wszDirectory[MAX_PATH];
	int nResult = MultiByteToWideChar(
		CP_ACP,    
		MB_PRECOMPOSED,
		czPathName,
		strlen(czPathName)+1,
		wszDirectory,
		sizeof(wszDirectory)/sizeof(wszDirectory[0]));
	if(0 == nResult)
	{
		return false;
	}
	return GetFileAttributes(wszDirectory)==FILE_ATTRIBUTE_DIRECTORY ? 0 : -1;
}
#elif _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#elif _LINUX
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

#ifdef _LINUX
int _itoa(int val, char* buf, int _radix)
{
	// _radix not used, set radix to 10
	const unsigned int radix = 10;
	char* p;
	unsigned int a; //every digit
	int len;
	char* b; //start of the digit char
	char temp;
	unsigned int u;
	p = buf;
	if (val < 0)
	{
		*p++ = '-';
		val = 0 - val;
	}
	u = (unsigned int)val;
	b = p;
	do
	{
		a = u % radix;
		u /= radix;
		*p++ = a + '0';
	} while (u > 0);
	len = (int)(p - buf);
	*p-- = 0;
	//swap
	do
	{
		temp = *p;
		*p = *b;
		*b = temp;
		--p;
		++b;
	} while (b < p);
	return len;
}
#endif

#define MAX_LOGMSG_SIZE 2048

CLogLibrary::CLogLibrary(void)
{
	m_pfLog = NULL; 
	m_pCaller = NULL;
	m_pfnDealCB = NULL;
	m_uiLogSize = LOG_MAX_SIZE;//��־Ĭ�ϴ�С
	m_uiCurSize = 0;
	for(int i = 0;i<LOG_MEDIUM_NUM;i++)
	{
		m_stLogWay[i].uiMedium = (INT)(pow(2.0,i));
		m_stLogWay[i].bFlag = FALSE;
		m_stLogWay[i].uiLevel = LINFO;
	}
	m_uiWay = WREWIND;// �ļ���¼��ʽ
 
	memset(m_strDir,0x0,LOG_DIR_NAME_NUM);//��־·��
	memset(m_strDocu,0x0,LOG_DOCUMENT_NAME_NUM);//��ǰ��־�����ļ���
	memset(m_strFileName,0x0,LOG_FILE_NAME_NUM);//��־�ļ���
	memset(m_strFileEx,0x0,LOG_EX_NAME_NUM);	 //��־�ļ���չ��
	memset(m_strWholeName,0x0,1024);	 //��־�ļ���չ��
	m_pGSMutex = new CGSMutex;
	bDiskReady = TRUE;
}

CLogLibrary::~CLogLibrary(void)
{


	if (m_pfLog != NULL)
	{
		fclose(m_pfLog);
		m_pfLog = NULL;
	}

	if (m_pGSMutex != NULL)
	{
		delete m_pGSMutex;
		m_pGSMutex = NULL;
	}

}
BOOL CLogLibrary::CountDiskFreeSpace(const char *pDir,UINT64 &uiTotalFreeSize)// ��ȡ���̿���ʣ��ռ�
{
	
	BOOL bRetCode = FALSE;
#ifdef WINCE
	ULARGE_INTEGER TotalAvailable;

	WCHAR wszDirectory[MAX_PATH];
	int nResult = MultiByteToWideChar(
		CP_ACP,    
		MB_PRECOMPOSED,
		pDir,
		strlen(pDir)+1,
		wszDirectory,
		sizeof(wszDirectory)/sizeof(wszDirectory[0]));
	if(0 == nResult)
	{
		return nResult;
	}
	
	bRetCode = ::GetDiskFreeSpaceEx(wszDirectory, &TotalAvailable, NULL, NULL);	
	uiTotalFreeSize = TotalAvailable.QuadPart;
#elif _WIN32
	ULARGE_INTEGER TotalAvailable;
	string strDir(pDir);
	bRetCode = ::GetDiskFreeSpaceExA(strDir.c_str(), &TotalAvailable, NULL, NULL);	
	uiTotalFreeSize = TotalAvailable.QuadPart;
#else
	struct statfs stStatfs;
	bRetCode = ((statfs("./", &stStatfs) == 0) ? TRUE : FALSE);    // statfs64()
	uiTotalFreeSize = stStatfs.f_bavail;
#endif

	return bRetCode;

}
INT CLogLibrary::CreatLogDir(char *pszDir)
{

	INT32 i = 0;
	INT32 iRet;
	INT32 iLen = strlen(pszDir);
	//��ĩβ��/
	if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
	{
		pszDir[iLen] = '/';
		pszDir[iLen + 1] = '\0';
	}

	// ����Ŀ¼
	for (i = 0;i < iLen;i ++)
	{
		if (pszDir[i] == '\\' || pszDir[i] == '/')
		{	
			if (i == 0)//��ֹlinux��/home/a/b����ʧ��
			{
				continue;
			}
			pszDir[i] = '\0';

			//���������,����
			iRet = ACCESS(pszDir,0);
			if (iRet != 0)
			{
				iRet = MKDIR(pszDir);
				if (iRet != 0)
				{
					return -1;
				}	
			}
			//֧��linux,������\����/
			pszDir[i] = '/';
		}
	}

	return 0;
}

INT CLogLibrary::GenerateLogDir(const char *pszDir)
{
	//����·����Ϣ
	strcpy(m_strDir,pszDir);//����·���ַ���

	if (m_strDir[strlen(m_strDir) -1] != '/' && m_strDir[strlen(m_strDir) -1] != '\\')//Ŀ¼���'/'
	{
		strcat(m_strDir,"/");
	}
	
	// ����·��
	if (CreatLogDir(m_strDir) != 0)
	{
		return ERROR_LOGLIBRARY_MKDIR_FAIl;
	}
	
    //������־�ļ���
#ifdef WINCE
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sprintf(m_strWholeName,"log%d-%d-%d",sysTime.wYear,sysTime.wMonth,sysTime.wDay);
#else //�������winceϵͳ
	time_t tt;
	struct tm *ttime; 
	
	
	time(&tt);
	ttime = localtime(&tt);

	strftime(m_strWholeName,LOG_DOCUMENT_NAME_NUM,"log%Y-%m-%d",ttime); 
#endif	
	if (strcmp(m_strWholeName,m_strDocu) != 0)//�ļ��в�����,�������ļ���
	{
		strcpy(m_strDocu,m_strWholeName);//�����ļ���
		strcpy(m_strWholeName,m_strDir);//����·��
		strcat(m_strWholeName,m_strDocu);//������־�ļ�
		strcat(m_strWholeName,"/");
		if (ACCESS(m_strWholeName,0) == -1)//�жϴ��ļ��Ƿ����,���������,�����ļ���
		{
			if (MKDIR(m_strWholeName) != 0)
			{
				return ERROR_LOGLIBRARY_MKDIR_FAIl;
			}	
		}
		
	}
	
	//�����ļ���
	
	strcpy(m_strFileName,m_strDocu);//��ʱ��
	strcat(m_strFileName,"_");//���»���
#ifdef WINCE
	sprintf(m_strWholeName,"log%d-%d-%d",sysTime.wHour,sysTime.wMinute,sysTime.wSecond);
#else
	strftime(m_strWholeName,LOG_DOCUMENT_NAME_NUM,"%H-%M-%S",ttime);
#endif
	strcat(m_strFileName,m_strWholeName);//��ʱ���

	if (strlen(m_strFileEx) != 0)//����չ
	{
		strcat(m_strFileName,"_");
		strcat(m_strFileName,m_strFileEx);
	}

	strcat(m_strFileName,".txt");
	
	//����ȫ·��
	strcpy(m_strWholeName,m_strDir);
	strcat(m_strWholeName,m_strDocu);
	strcat(m_strWholeName,"/");
	strcat(m_strWholeName,m_strFileName);

	if (m_pfLog != NULL)
	{
		fclose(m_pfLog);
		m_pfLog = NULL;
	}

	m_pfLog = fopen(m_strWholeName,"w+");
	assert(m_pfLog);
	if (m_pfLog == NULL)
	{
		return ERROR_LOGLIBRARY_FILE_NOT_OPEN;
	}

	//	��־��ǰ��СΪ0
	m_uiCurSize = 0;

	return ERROR_LOGLIBRARY_OPER_SUCCESS;
}

INT CLogLibrary:: WriteToFile(const char *pszMsg)
{

	INT32 iRet = 0;
	// ����ļ������Ч���򴴽����ļ�
	if (m_pfLog == NULL)
	{
		iRet = GenerateLogDir(m_strDir);
		if (iRet != ERROR_LOGLIBRARY_OPER_SUCCESS)
		{
			return iRet;
		}
	}
	 
	// ������̿ռ䲻�����ͽ��м�⣬�����⵽�пռ䣬�ͼ���д��־
	if (!bDiskReady)
	{
		UINT64 uiFreeSize = 0;
		BOOL bFlag = FALSE;
		bFlag = CountDiskFreeSpace(m_strDir,uiFreeSize); // �������ʣ��ռ�
		if ((!bFlag)||(uiFreeSize < m_uiLogSize - m_uiCurSize))
		{
			assert(bFlag);// ���ʧ��
			bDiskReady = FALSE;
			return ERROR_LOGLIBRARY_DISK_ERROR;
		}

		bDiskReady = TRUE;
	}
		
	//����ļ���С�������ô�С������ָ����ʽ�½��ļ����߻ؾ��ļ���ͷд��
	INT32 iMsgLen = strlen(pszMsg);
	if (m_uiCurSize + iMsgLen > m_uiLogSize)
	{
		if (m_pfLog != NULL)
		{
			fclose(m_pfLog);
			m_pfLog = NULL;
		}
		
		// ��ǰ�ļ���С��0
		m_uiCurSize = 0;

		switch(m_uiWay)
		{
		case WREWIND://�ؾ�
			{
				m_pfLog = fopen(m_strWholeName,"w+");
				if (m_pfLog == NULL)
				{
					return ERROR_LOGLIBRARY_FILE_NOT_OPEN;
				}
				break;
			}
		case WCREATE:// �½�
		default:
			{
				iRet = GenerateLogDir(m_strDir);
				if (iRet !=ERROR_LOGLIBRARY_OPER_SUCCESS)
				{	
					m_pfLog = NULL;
					return iRet;
				}
			}
		}

	}
	// д�ļ�
	INT32 iCount = 0;	
	iCount = fwrite(pszMsg,1,iMsgLen,m_pfLog);

  //assert(iCount == (size_t)iMsgLen);

	// ���fwriteʧ�ܣ����������д�룬�����̱�־��ΪFALSE
	if (iCount != (size_t)iMsgLen)
	{
		bDiskReady = FALSE;
		return ERROR_LOGLIBRARY_DISK_ERROR;
	}

	// ���fflushʧ�ܣ����������д�룬�����̱�־��ΪFALSE
	iRet = fflush(m_pfLog);
  //assert(iRet == 0);
	if (iRet != 0)
	{
		bDiskReady = FALSE;
		return ERROR_LOGLIBRARY_DISK_ERROR;
	}

	m_uiCurSize += iCount;
	return 0;

}

INT  CLogLibrary::SetLogDir(const char *pszDir,const char *pszExtName)
{
    INT iRet;
	m_pGSMutex->Lock();
	if (pszExtName != NULL)
	{
		if (strlen(pszExtName) > LOG_EX_NAME_NUM - 1)
		{
			m_pGSMutex->Unlock();
			return ERROR_LOGLIBRARY_STRING_ILLEGAL;
		}
		strcpy(m_strFileEx,pszExtName);
	}
	iRet = GenerateLogDir(pszDir);
	m_pGSMutex->Unlock();
	
	return iRet;
}

INT  CLogLibrary::SetLogSize(UINT uiWay,UINT uiSize)
{
	if(uiSize < 0)//�ļ���С����
	{
		return ERROR_LOGLIBRARY_FILESIZE_ILLEGAL;
	}

	if (uiWay != WREWIND && uiWay != WCREATE)//���Բ���
	{
		return ERROR_LOGLIBRARY_FILEWAY_ILLEGAL;
	}
	m_pGSMutex->Lock();
	if (uiSize == 0)
	{
		m_uiLogSize = LOG_MAX_SIZE;
	}
	else
	{
		m_uiLogSize = uiSize * 1024 *1024;
	}

	m_uiWay = uiWay;
	m_pGSMutex->Unlock();
	
	return ERROR_LOGLIBRARY_OPER_SUCCESS;
}

INT CLogLibrary::SetLogLevel(UINT uiMedium,UINT uiLevel)//��ͬʱ֧�����ö��н��ʣ������ö���ȼ�,���ĳ��Ӧ�ȼ�����0����ȡ���������ʽ
{  
	//ý�ʲ��Ϸ�
	if (uiMedium > pow(2.0,LOG_MEDIUM_NUM )-1|| uiMedium <= 0x0)
	{
		return ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL;
	}

	//�ȼ����Ϸ�
	if (uiLevel > pow(2.0,LOG_LEVEL_NUM )-1)
	{
		return ERROR_LOGLIBRARY_LOGMEDIUM_ILLEGAL;
	}

	m_pGSMutex->Lock();

	//���ö�Ӧ���ý�ʼ����ӵȼ�
	for(int i = 0;i < LOG_MEDIUM_NUM;i++)
	{
		if (((uiMedium>>i) & 0x0001) == 1)
		{
			if (uiLevel == 0)
			{
				m_stLogWay[i].bFlag = FALSE;
			}
			else
			{
				m_stLogWay[i].bFlag = TRUE;
			}
			m_stLogWay[i].uiLevel = uiLevel;

		}
	}
	m_pGSMutex->Unlock();
	
	return ERROR_LOGLIBRARY_OPER_SUCCESS;
}

void CLogLibrary::SetLogCB(void *pCaller,void ( *pfnDealCB)(void *pCaller,const char * pszLevel,const char *pszPrefix,const char *pszMsg))
{
	m_pGSMutex->Lock();
	m_pCaller = pCaller;
	m_pfnDealCB = pfnDealCB;
	m_pGSMutex->Unlock();
}
string CLogLibrary::GetTimeStr()
{
	stringstream	streamTime;
	StruSysTime stTime;
	DoGetLocalTime(&stTime);

	//��
	streamTime<<stTime.wYear<<"-";
	//��
	if (stTime.wMonth < 10)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wMonth<<"-";

	//��
	if (stTime.wDay < 10)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wDay<<"-";
	
	//ʱ
	if (stTime.wHour < 10)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wHour<<"-";

	//��
	if (stTime.wMinute < 10)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wMinute<<"-";

	//��
	if (stTime.wSecond < 10)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wSecond<<"-";

	//����
	if (stTime.wMilliseconds < 10)
	{
		streamTime<<"00";
	}
	else if (stTime.wMilliseconds < 100)
	{
		streamTime<<"0";
	}
	streamTime<<stTime.wMilliseconds<<"  ";

	string str = streamTime.str();
	
	return str;
}
/*************************************************
Function:   Log
DateTime:   2010-5-21 11:46:07   
Description: д��־
Input:    uiLevel		��־�ļ�������ȼ�����ֵ�������ϵ LEMERG>LFATAL>LALERT>LCRIT>LERROR>LWARN
>LNOTICE>LINFO>LDEBUG>LNOTSET
pszPrefix	��־ǰ׺��ΪNULLʱ��ΪĬ��ǰ׺no���䳤�Ȳ�Ӧ����64�ֽ�
pszFormat	��־��ʽ������printf�и�ʽ���÷���ͬ
...			��ʽ���ж�Ӧֵ
Output:   ��
Return:   ERROR_LOGLIBRARY_OPER_SUCCESS		�����ɹ�
ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL	��־�ȼ��Ƿ�
Note:      
*************************************************/
INT  CLogLibrary::Log(UINT uiLevel,char *pszPrefix,char  *pszFormat,...)
{
	//��־��ʽ��ʱ��+�ȼ�+ǰ׺+���ݣ�֧�ֱ䳤��
	
	
	/*char	strLog[MAX_LOGMSG_SIZE] = {0x0};
	char	strContent[MAX_LOGMSG_SIZE] = {0x0};*/
	char	strLevel[16] = {0x0};
	char	strPrefix[64] = {0x0};
	int iRet = -1;

	/*�Եȼ����������Ƿ��ǺϷ��ȼ�*/
	int i = 0;
	for (i=0;i<LOG_LEVEL_NUM;i++)
	{
		if ((uiLevel>>i) == 1)
		{
			break;
		}
	}
	if (i >= LOG_LEVEL_NUM)
	{
		return ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL;
	}

	/*�ȼ���*/
	switch(uiLevel)
	{
	case LEMERG:
		strcpy(strLevel,"LEMERG");
		break;
	case LFATAL:
		strcpy(strLevel,"LFATAL");
		break;
	case LALERT:
		strcpy(strLevel,"LALERT");
		break;
	case LCRIT:
		strcpy(strLevel,"LCRIT");
		break;
	case LERROR:
		strcpy(strLevel,"LERROR");
		break;
	case LWARN:
		strcpy(strLevel,"LWARN");
		break;
	case LNOTICE:
		strcpy(strLevel,"LNOTICE");
		break;
	case LINFO:
		strcpy(strLevel,"LINFO");
		break;
	case LDEBUG:
		strcpy(strLevel,"LDEBUG");
		break;
	case LNOTSET:
		strcpy(strLevel,"LNOTSET");
		break;
	default:
		return ERROR_LOGLIBRARY_LOGLEVEL_ILLEGAL;
	}
	
	/*ǰ׺��*/
	if (pszPrefix == NULL || strlen(pszPrefix) > 63)//��ǰ׺
	{
		strcpy(strPrefix,"no");
	}

	else
	{
		strcpy(strPrefix,pszPrefix);
	}
	

	char			szlogContent[MAX_LOGMSG_SIZE] = {0x0};
	/*��Ϣ��������䳤����*/
	va_list  ap;
	va_start(ap, pszFormat);	
	iRet = vsnprintf(szlogContent,MAX_LOGMSG_SIZE - 1,pszFormat,ap);
	va_end(ap);

	// vsnprintf���ܻ�ִ�д��󣬱����ַ�������strContent����
	if (iRet == -1)
	{
		return ERROR_LOGLIBRARY_MSGSIZE_ILLEGAL;
	}
	m_pGSMutex->Lock();

	if (m_pfnDealCB != NULL)
	{
		m_pfnDealCB(m_pCaller,strLevel,strPrefix,szlogContent);
	}

	/*ʱ�䴮*/
	string strTime = GetTimeStr();
	string	strContent = string(szlogContent);
	stringstream	streamlog;
	streamlog <<strTime;
	streamlog <<string(strLevel)<<"  ";
	streamlog <<string(strPrefix)<<"  ";
	streamlog <<strContent;

	//�ж��Ƿ��л��з�
	streamlog<<"\n";
	
	string strlog = streamlog.str();

	// �����־�ļ��Ƿ����(�����Ƿ񲻴�ӡ)
	if (strlog.size() > MAX_LOGMSG_SIZE - 2)
	{
		m_pGSMutex->Unlock();
		return ERROR_LOGLIBRARY_STRING_ILLEGAL;
	}

	// ���趨�����ʽ�����־
	for (i=0;i<LOG_MEDIUM_NUM;i++)
	{
		if (m_stLogWay[i].bFlag == TRUE )//�������ʽ�Ƿ���Ч
		{
			if ((m_stLogWay[i].uiLevel & uiLevel) != 0)//�Ƿ��趨�ȼ�
			{
				if (m_stLogWay[i].uiMedium == MCONSOLE)//���������̨
				{
					printf("%s",strlog.c_str());
				}

				if (m_stLogWay[i].uiMedium == MFILE)//������ļ�
				{
					char chTemp[LOG_DOCUMENT_NAME_NUM] = {0};
#ifdef WINCE
					SYSTEMTIME sysTime;
					GetSystemTime(&sysTime);
					sprintf(chTemp,"log%d-%d-%d",sysTime.wYear,sysTime.wMonth,sysTime.wDay);
#else
					time_t tt;
					struct tm *ttime; 
					time(&tt);
					ttime = localtime(&tt);
					strftime(chTemp,LOG_DOCUMENT_NAME_NUM,"log%Y-%m-%d",ttime); 
#endif

					if (strcmp(chTemp,m_strDocu) != 0)//�µ�һ��,�����µ�Ŀ¼
					{
						if (GenerateLogDir(m_strDir) !=ERROR_LOGLIBRARY_OPER_SUCCESS)
						{
							m_pGSMutex->Unlock();
							return -1;
						}
					}
					WriteToFile(strlog.c_str());
				}
			}
		}
	}
	m_pGSMutex->Unlock();
	
	return ERROR_LOGLIBRARY_OPER_SUCCESS;
}

ILogLibrary *GetLogInstance()
{
	ILogLibrary *pLogLibrary = new CLogLibrary;

	if (pLogLibrary != NULL)
	{
		return pLogLibrary;
	}
	
	return NULL;
}
void ClearLogInstance(ILogLibrary *pLogLibrary)
{
	if (pLogLibrary != NULL)
	{
		delete pLogLibrary;
		pLogLibrary = NULL;
	}
}
