#if !defined (NetServiceDataType_DEF_H)
#define NetServiceDataType_DEF_H

/***************************************************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		NetServiceDataType.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/06/03
	Description:    ����⹫����ͷ�ļ����������ͣ�ָ������Լ��ڲ�ʹ�õġ� 

****************************************************************************************************/

#ifdef WINCE
#define FD_SETSIZE      1024
#define	OPERATING_SYSTEM	1  
#endif

#include "DataType.h"
#include <assert.h>
#include <deque>			//C++STL������
#include <map>
#include <vector>
#include <algorithm>
using namespace std;

namespace NetServiceLib
{

#define ERROR_CTRLPRINTF	//���������Ϣ

	#ifdef _WIN32
		#ifdef ERROR_CTRLPRINTF	
			#define CTRLPRINTF( xLog,xFormat, ... ) \
				do { \
					if( NULL != xLog) \
						(xLog)->Log(LNOTSET, NULL, " (%s %d) " xFormat,__FILE__, __LINE__, __VA_ARGS__); \
				}while(0)
				

		#else
			#define CTRLPRINTF( xLog, xFormat, ... )  do { } while( 0 )
		#endif
	#else
		#ifdef ERROR_CTRLPRINTF	
			#define CTRLPRINTF( xLog, xFormat, ... ) \
			do { \
				if( NULL != xLog) \
					(xLog)->Log(LNOTSET,NULL," (%s %d) " xFormat,__FILE__, __LINE__, ##__VA_ARGS__); \				
			}while(0)

		#else
			#define CTRLPRINTF( xLog, xFormat, ...)  do { } while( 0 )
		#endif
#endif

#ifdef _DEBUG
#define INFO_CTRLPRINTF	//DEBUG���������Ϣ
#endif
#ifdef _WIN32
	#ifdef INFO_CTRLPRINTF	
		#define CTRLPRINTF_D( xLog, xFormat, ... ) \
			do { \
				if( NULL != xLog) \
					(xLog)->Log(LNOTSET,NULL," (%s %d) " xFormat,__FILE__, __LINE__, __VA_ARGS__); \
			}while(0)


	#else
		#define CTRLPRINTF_D( xLog, xFormat, ... )  do { } while( 0 )
	#endif
#else
	#ifdef INFO_CTRLPRINTF	
		#define CTRLPRINTF_D( xLog, xFormat, ... ) \
			do { \
				if( NULL != xLog) \
				(xLog)->Log(LNOTSET,NULL," (%s %d) " xFormat,__FILE__, __LINE__, ##__VA_ARGS__); \				
			}while(0)

	#else
		#define CTRLPRINTF_D( xLog, xFormat, ...)  do { } while( 0 )
	#endif
#endif

#ifdef _WIN32

#include <Windows.h>
#else	//linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <netdb.h>
#include <sys/epoll.h>

//#define HAVE_GETHOSTBYNAME2

#endif


#ifdef _WIN32
typedef 		SOCKET		SOCKETHANDLE;
#else
typedef			int			SOCKETHANDLE;
#endif



typedef enum
{
	LISTEN,			//����
	ACCEPTDATA,		//��������
	ACTIVETEST,		//����
	EPOLLEVENTWAIT,	//epoll�¼�
	CHANNELSTATUS,	//ͨ��״̬
	ACCEPTNOTICE,	// ������֪ͨ��ָ�����

}enumThreadEventType;

//ͨ��״̬
typedef enum
{
	CHANNEL_NORMAL,		//����״̬
	CHANNEL_DISCNN,		//�ж�״̬			//ֻ��TCP�������״̬
	CHANNEL_CLOSE,		//�ر�
	CHANNEL_TIMEOUT,	//��ʱ
	CHANNEL_ADD_IOCPOREPOLL, //ͨ����Ҫ����IOCP��epoll
}enumChannelStatus;


#define	 DATA_BUFSIZE	8192	// socket���ջ����С
// ��������
#define   RECV_POSTED			1001  
#define   SEND_POSTED			1002
#define   ACCESS_POSTED			1003

// ��־ָ��
extern ILogLibrary*			g_clsLogPtr;
/**************************************************************************
Function    : Utility::GetApplicationPath    
DateTime    : 2010/8/26 11:13	
Description : ��ȡӦ�ó���Ŀ¼
Input       : ��
Output      : ��
Return      : ����Ӧ�ó���Ŀ¼
Note        :	
**************************************************************************/
string GetApplicationPath(void);
}

#endif


