#include "OSThread.h"
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <Mmsystem.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>

#include "GSPMemory.h"
#include "List.h"
#include "BTree.h"
#include "ISocket.h"
#include "ThreadPool.h"
#include "MainLoop.h"
#include "Log.h"
using namespace GSP;




/*
****************************************
brief : CSection ��ʵ��
****************************************
*/

CSection::CSection(void)
{
#ifdef _WIN32
    ::InitializeCriticalSection(&m_stSection);
#else 
    if( ::pthread_mutexattr_init(&m_stAttr) )
    {
        GSP_ASSERT_EXIT(0, -1);
    }
    if( ::pthread_mutexattr_settype(&m_stAttr, PTHREAD_MUTEX_RECURSIVE) ) 
    {        
        //��ΪǶ����
        GSP_ASSERT_EXIT(0, -1);
    }
     if( ::pthread_mutex_init( &m_stSection,&m_stAttr) )
     {                          
         GSP_ASSERT_EXIT(0, -1);
     }
#endif
}

CSection::~CSection(void)
{
#ifdef _WIN32
    ::DeleteCriticalSection(&m_stSection);
#else
    ::pthread_mutex_destroy(&m_stSection);
    ::pthread_mutexattr_destroy(&m_stAttr);
#endif
}

BOOL CSection::Lock(void)
{
#ifdef _WIN32
	GS_ASSERT( 0==m_stSection.SpinCount );

    ::EnterCriticalSection(&m_stSection);   
#else
    if( pthread_mutex_lock(&m_stSection) )
    {
        GSP_ASSERT(0);
        return FALSE;
    }
#endif
    return TRUE;
}

BOOL CSection::Unlock(void)
{
#ifdef _WIN32
    ::LeaveCriticalSection(&m_stSection);   
#else
    if( pthread_mutex_unlock(&m_stSection) )
    {
          GSP_ASSERT(0);
        return FALSE;
    }
#endif
    return TRUE;
}


/*
****************************************
brief : OSThread ��ʵ��
****************************************
*/



INT64 COSThread::Milliseconds(void)
{ 
#ifdef _WIN32 
    SYSTEMTIME stTm;
    struct tm stLcTm = {0};
    ::GetSystemTime(&stTm);
    stLcTm.tm_year = stTm.wYear-1900;  
    stLcTm.tm_mon = stTm.wMonth-1;  
    stLcTm.tm_mday = stTm.wDay;  
    stLcTm.tm_hour = stTm.wHour;  
    stLcTm.tm_min = stTm.wMinute;  
    stLcTm.tm_sec = stTm.wSecond;

    INT64 curTime;
    curTime = mktime(&stLcTm);
    curTime *= 1000;                // sec -> msec
    curTime += stTm.wMilliseconds;    // usec -> msec
    return curTime;
#else
    struct timeval t;
    int theErr = ::gettimeofday(&t, NULL);
    GSP_ASSERT(theErr == 0);

    INT64 curTime;
    curTime = t.tv_sec;
    curTime *= 1000;                // sec -> msec
    curTime += t.tv_usec / 1000;    // usec -> msec
    return curTime;
  //  return (curTime - sInitialMsec) + sMsecSince1970;
#endif
}

// BOOL COSThread::CTime( UINT32 iSecs, CGSSString &stFmt )
// {
//     time_t t = iSecs;
// #ifdef _WIN32
//     stFmt = ctime(&t );
// #else
//     CAtomic::Lock();
//     stFmt = ctime(&t );
//     CAtomic::Unlock();
// #endif
//     return TRUE;
// }

BOOL COSThread::GetCTimeOfGMT( CGSPString &stFmt)
{
	char szTemp[200];
#ifdef _WINCE
	SYSTEMTIME SystemTime;
	GetSystemTime(&SystemTime);
	WCHAR dateFormat[] = L"ddd, MMM dd yyyy";
	WCHAR timeFormat[] = L"HH:mm:ss GMT\r\n";
	WCHAR inBuf[200];
	DWORD locale = LOCALE_NEUTRAL;

	int ret = GetDateFormat(locale, 0, &SystemTime,
		(LPTSTR)dateFormat, (LPTSTR)inBuf, sizeof(inBuf) );
	inBuf[ret - 1] = ' ';
	ret = GetTimeFormat(locale, 0, &SystemTime,
		(LPTSTR)timeFormat,
		(LPTSTR)inBuf + ret, (sizeof inBuf) - ret);
	wcstombs(szTemp, inBuf, wcslen(inBuf));
#else
	time_t tt = time(NULL);
	strftime(szTemp, sizeof(szTemp), "%a, %b %d %Y %H:%M:%S GMT", gmtime(&tt));
#endif
	stFmt = szTemp;
	return TRUE;
}

INT COSThread::CurrentDeviceCPUNumber(void)
{
static GSAtomicInter s_iCpus = -1;
     if( s_iCpus != -1 )
     {
         return s_iCpus;
     }
#ifdef _WIN32
     typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
    SYSTEM_INFO si;
    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
    PGNSI pfnGNSI = (PGNSI) GetProcAddress(GetModuleHandle((LPCTSTR)"kernel32.dll"), "GetNativeSystemInfo");
    if(pfnGNSI)
    {
        pfnGNSI(&si);
    }
    else
    {
        GetSystemInfo(&si);
    }
    AtomicInterCompareExchange( s_iCpus,-1, si.dwNumberOfProcessors  );
#else
    FILE * pFile = fopen("/proc/cpuinfo", rb);
    if (!pFile)
        return 1;
    int iFileLen = KBYTES*512; //�������õĴ�Щ
    char * pBuff = NULL;
    int iRet = 0;
    int iCnts = 0;
    while( 1 )
    {
        
        pBuff = (char*)malloc(iFileLen);
        GS_ASSERT_EXIT(pBuff, -1);
        fseek(pFile, 0L, SEEK_SET);   
        pBuff[0] ='\0';
        iRet = fread(pBuff, 1 , iFileLen, pFile);//�����ļ�����
        if( iRet < iFileLen )
        {
            break;
        }
        free(pBuff);
        iFileLen += KBYTES*512;
    } 
    char *p = pBuff;
    do
    {
        p = strstr(p, "processor");
        if( p )
        {
           iCnts++;
           p += 6;
        }        
    }while(p);
    free(pBuff);
    AtomicInterCompareExchange( s_iCpus,-1, si.dwNumberOfProcessors  );
#endif
    if( s_iCpus < 1 )
    {
        GS_ASSERT(0);
        AtomicInterSet(s_iCpus, 1);
    }
    return s_iCpus;
}


namespace GSP
{
    typedef struct _StrucInfoNameNode
    {
        INT iType;
        char *czName;
    }StrucInfoNameNode;
#define  INFO_NAME_NODE_VALUE( t, stv ) { (INT)(t), stv }




    static const char *_GetInfoName(const StrucInfoNameNode *pVNode, INT iVNodeNums,  INT iType, const char *pDefault )
    {
        for( INT i=0; i<iVNodeNums; i++ )
        {
            if( pVNode[i].iType==iType )
            {
                return pVNode[i].czName;
            }
        }
        return  pDefault;
    }

    static INT _GetInfoType(const StrucInfoNameNode *pVNode, INT iVNodeNums,  const char *czName, INT iDefault )
    {
        for( INT i=0; i<iVNodeNums; i++ )
        {
            if( strcasecmp(pVNode[i].czName, czName)==0 )
            {
                return pVNode[i].iType;
            }
        }
        return  iDefault;
    }

    static const StrucInfoNameNode s_sProList[] =
    {
        INFO_NAME_NODE_VALUE(ePRO_GSP,  _GSTX("gsp") ),
        INFO_NAME_NODE_VALUE(ePRO_RTSP, _GSTX("rtsp") ),
		INFO_NAME_NODE_VALUE(ePRO_SIP, _GSTX("sip") ),
        INFO_NAME_NODE_VALUE(ePRO_UNK,  _GSTX("unk") ),
    };

     const char *GetProtocolName( EnumProtocol ePro )
    {
        INT iCnts = ARRARY_SIZE(s_sProList);
        return _GetInfoName(s_sProList, iCnts-1,(INT)ePro,  s_sProList[iCnts-1].czName );
    }

     EnumProtocol GetProtocolType( const char *strName)
    {
        INT iCnts = ARRARY_SIZE(s_sProList);
        return (EnumProtocol)_GetInfoType(s_sProList, iCnts-1,strName, s_sProList[iCnts-1].iType );
    }

//      const char *GetPlayCommandName( EnumPlayCommand eCommand )
//     {
//         static const StrucInfoNameNode s_sPlayCmdList[] =
//         {
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_PLAY, _GSTX("����") ),
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_STOP, _GSTX("ֹͣ")  ), 
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_PAUSE, _GSTX("��ͣ") ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_FAST, _GSTX("���")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_BFAST, _GSTX("����")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_STEP, _GSTX("��֡ǰ")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_BSTEP, _GSTX("��֡��")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_SLOW, _GSTX("����")  ), 
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_BSLOW, _GSTX("���� ")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_FILTER, _GSTX("����")  ),  
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_SETPERCENT, _GSTX("�϶�:�ٷֱ�")  ),        
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_SETPOINT, _GSTX("�϶�:����ֵ  ")  ), 
//             INFO_NAME_NODE_VALUE(ePLAY_CMD_SETTIME, _GSTX("�϶�:ʱ���")  ),
//             INFO_NAME_NODE_VALUE(0, _GSTX("δ֪") ),
//         };
// 
//         INT iCnts = ARRARY_SIZE(s_sPlayCmdList);
//         return _GetInfoName(s_sPlayCmdList, iCnts-1,(INT)eCommand, s_sPlayCmdList[iCnts-1].czName );
//     };

//      const char *GetModuleStatusName(  EnumModuleStatus eStatus )
//     {
//         static const StrucInfoNameNode s_stStatusList[] =
//         {
//             INFO_NAME_NODE_VALUE(eSTATUS_MOD_UINIT, _GSTX("δ��ʼ��") ), 
//             INFO_NAME_NODE_VALUE(eSTATUS_MOD_RUNNING, _GSTX("������") ),  
//             INFO_NAME_NODE_VALUE(eSTATUS_MOD_STOP, _GSTX("ֹͣ")  ), 
//             INFO_NAME_NODE_VALUE(eSTATUS_MOD_ERR, _GSTX("����")  ),  
//             INFO_NAME_NODE_VALUE(-1, _GSTX("δ֪") ),
//         };
//         INT iCnts = ARRARY_SIZE(s_stStatusList);
//         return _GetInfoName(s_stStatusList, iCnts-1,(INT)eStatus, s_stStatusList[iCnts-1].czName );
// 
//     }

    const char *GetMediaName( EnumGSMediaType eType)
    {
        static const StrucInfoNameNode s_stMTypeList[] =  
        {   
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_NONE, _GSTX("None")), 
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_VIDEO,_GSTX("Video")), 
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_AUDIO,_GSTX("Audio")), 
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_PICTURE,_GSTX("Picture")), 
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_OSD,_GSTX("OSD") ), 
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_BINARY,_GSTX("Binary") ),
            INFO_NAME_NODE_VALUE(GS_MEDIA_TYPE_SYSHEADER,_GSTX("SysHeader") ),
            INFO_NAME_NODE_VALUE(-1,_GSTX("Unknown") ),
        };
        INT iCnts = ARRARY_SIZE(s_stMTypeList);
        return _GetInfoName(s_stMTypeList, iCnts-1,(INT)eType, s_stMTypeList[iCnts-1].czName );

    }


	const char *GetError( EnumErrno eErrno )
	{
		static const StrucInfoNameNode s_stErrnoList[] =  
		{
			INFO_NAME_NODE_VALUE(eERRNO_SUCCESS,  _GSTX("�ɹ�") ),
			INFO_NAME_NODE_VALUE(eERRNO_EUNKNOWN,  _GSTX("δ֪����") ),
			INFO_NAME_NODE_VALUE(eERRNO_EFATAL,  _GSTX("δ֪�Ĳ����޸��Ĵ���") ),
			INFO_NAME_NODE_VALUE(eERRNO_ENONE,  _GSTX("�޴���") ),


			INFO_NAME_NODE_VALUE(eERRNO_SYS_EINVALID,  _GSTX("��Ч����") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EFLOWOUT,  _GSTX("�������") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ENMEM,  _GSTX("û���ڴ�") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EOPER,  _GSTX("��֧�ֵĲ���") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ESTATUS,  _GSTX("����״̬�µĲ���") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ENCTRL,  _GSTX("��֧�ֵĿ��Ʋ���") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EPERMIT,  _GSTX("û��Ȩ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ETIMEOUT,  _GSTX("��ʱ") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EKPL,  _GSTX("keepalive ��ʱ") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EBUSY,  _GSTX("��Դȱ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ETKP,  _GSTX("�������ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EFUNC,  _GSTX("�ù���û��ʵ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EPRO,  _GSTX("��֧�ֵ�Э��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_EPERMIT,  _GSTX("û��Ȩ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SYS_ENEXIST,  _GSTX("�����Ѿ�������") ),
			


			INFO_NAME_NODE_VALUE(eERRNO_NET_EUNK,  _GSTX("�������: δ֪�������������") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EDISCNN,  _GSTX("�������: �Զ�����ر�") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_ECLOSED,  _GSTX("�������: Socket �Ѿ��ر�") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EREG,  _GSTX("�������: ע��ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EWEVT,  _GSTX("�������: ����д�¼�ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EREVT,  _GSTX("�������: ����д�¼�ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EEVT,  _GSTX("�������: �����¼�ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EBIN,  _GSTX("�������: �󶨶˿�ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_EOPT,  _GSTX("�������: ���Բ���ʧ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_ESEND,  _GSTX("�������: д����") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_ERCV,  _GSTX("�������: ������") ),
			INFO_NAME_NODE_VALUE(eERRNO_NET_ESOCKET,  _GSTX("�������: ����SOCKET ʧ��") ),


			INFO_NAME_NODE_VALUE(eERRNO_SRC_ENXIST,  _GSTX("����Դ������") ),
			INFO_NAME_NODE_VALUE(eERRNO_SRC_EUNUSED,  _GSTX("����Դ����ʹ��") ),
			INFO_NAME_NODE_VALUE(eERRNO_SRC_ECLOSE,  _GSTX("����Դ���ر�") ),
			INFO_NAME_NODE_VALUE(eERRNO_SRC_EASSERT,  _GSTX("����Դ�쳣") ),
			INFO_NAME_NODE_VALUE(eERRNO_SRC_ECODER,  _GSTX("����Դ��֧�ֱ�������") ),
			


			INFO_NAME_NODE_VALUE(eERRNO_CLI_ECLOSE,  _GSTX("�ͻ��˴��� �Զ�����ر�") ),
			INFO_NAME_NODE_VALUE(eERRNO_CLI_EASSERT,  _GSTX("�ͻ��˴��� �Զ��쳣�ر�") ),
			INFO_NAME_NODE_VALUE(eERRNO_CLI_ENSRC,  _GSTX("�ͻ��˴��� ���󲻴�������Դ") ),


			INFO_NAME_NODE_VALUE(eERRNO_SRV_REFUSE,  _GSTX("�ϲ�ܾ�����") ),


			INFO_NAME_NODE_VALUE(eERRNO_EVT_CONTINUE_NEXT,  _GSTX("Continue Next") ),


			INFO_NAME_NODE_VALUE(-1,    _GSTX("��Ч�����") ),
		};
		INT iCnts = ARRARY_SIZE(s_stErrnoList);
		return _GetInfoName(s_stErrnoList, iCnts-1,(INT)eErrno, s_stErrnoList[iCnts-1].czName );
	}


// 	GS_API const char *GetClientStatusText( EnumClientStatus eStatus)
// 	{
// 		static const StrucInfoNameNode s_stErrnoList[] =  
// 		{
// 			INFO_NAME_NODE_VALUE(eST_CLI_INIT,  _GSTX("��ʼ��") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_CONNECT,  _GSTX("�ȴ���") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_CONNECT,  _GSTX("����") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_READY,  _GSTX("׼��") ),			
// 			INFO_NAME_NODE_VALUE(eST_CLI_CONNECTING,  _GSTX("������") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_WAIT_REQUEST,  _GSTX("�ȴ�URI����") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_REQUEST,  _GSTX("������URI") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_REQUESTING,  _GSTX("��������URI") ),	
// 			INFO_NAME_NODE_VALUE(eST_CLI_WAIT_START,  _GSTX("�ȴ���ʼ") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_PLAYING,  _GSTX("������") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_STOP,  _GSTX("ֹͣ") ),
// 			INFO_NAME_NODE_VALUE(eST_CLI_ASSERT,  _GSTX("�쳣") ),
// 			INFO_NAME_NODE_VALUE(-1,    _GSTX("δ֪״̬") ),
// 		};
// 		INT iCnts = ARRARY_SIZE(s_stErrnoList);
// 		return _GetInfoName(s_stErrnoList, iCnts-1,(INT)eStatus, s_stErrnoList[iCnts-1].czName );
// 	}


}


/*
*********************************************************************
*
*@brief : ģ���ʼ��
*
*********************************************************************
*/
namespace GSP
{


typedef void (*FuncPtrModuleInit)(void);

typedef struct _StruInitFunc
{
	FuncPtrModuleInit fnInit;
	FuncPtrModuleInit fnUninit;
}StruInitFunc;

static GSAtomicInter _s_iInitRefs = 0;

static StruInitFunc s_InitFunc[] =
{              
	{ CMemoryPool::InitModule, CMemoryPool::UninitModule},
	{ CGSPThreadPool::InitModule, CGSPThreadPool::UninitModule}, 
	{ CMainLoop::InitModule, CMainLoop::UninitModule},
	{ CISocket::InitModule, CISocket::UninitModule},
	
};


void GSPModuleInit(void)
{
	if( AtomicInterInc(_s_iInitRefs ) > 1 )
	{
		return;
	}
	CGSPObject::g_bModuleRunning = TRUE;
	for( int i =0; i<ARRARY_SIZE(s_InitFunc); i++ )
	{
		if( s_InitFunc[i].fnInit )
		{
			s_InitFunc[i].fnInit();
		}
	}
}

void GSPModuleUnint(void)
{
	if(_s_iInitRefs>0 && AtomicInterDec(_s_iInitRefs ) == 0 )
	{
		CGSPObject::g_bModuleRunning = FALSE;
		
		for( int i=ARRARY_SIZE(s_InitFunc); i>0; i-- )
		{
			if( i==1 )
			{
				if( g_pLog )
				{
					g_pLog->UnrefObject();
					g_pLog = NULL;
				}
			}
			if( s_InitFunc[i-1].fnUninit )
			{
				s_InitFunc[i-1].fnUninit();
			}
		}

	}

}

} //end namespace GSP