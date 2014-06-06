#include "BaseSocket.h"

using namespace NetServiceLib;

CBaseSocket::CBaseSocket(void)
{
	m_hSocket = -1;

#if OPERATING_SYSTEM
	m_bBlock = TRUE;

#elif _WIN32

	// win Ĭ��Ϊ����
	m_bBlock = FALSE;

#else
	// linux Ĭ��Ϊ������. ��Ϊlinux�Ľ��ջ��ƺ�win��ͬ��epoll����ɶ˿�������ͬ
	m_bBlock = TRUE;

#endif
	
}

CBaseSocket::~CBaseSocket(void)
{
}
INT	CBaseSocket::SetSockOption(INT iExclusive)
{

	SetSockOptionSec();

	if (iExclusive == 1)
	{	// һ���Ƿ������ζ�ռ
		int nReuseAddr = 0;
		setsockopt(m_hSocket ,SOL_SOCKET,SO_REUSEADDR,(const char *)&nReuseAddr,sizeof(int)); 

		nReuseAddr = 1;
		setsockopt(m_hSocket ,SOL_SOCKET,SO_EXCLUSIVEADDRUSE,(const char *)&nReuseAddr,sizeof(int));
	}
	else
	{
		int nReuseAddr = 1;
		setsockopt(m_hSocket ,SOL_SOCKET,SO_REUSEADDR,(const char *)&nReuseAddr,sizeof(int)); 

	}

	

	return 0;
	
}



void CBaseSocket::SetSocket(SOCKETHANDLE hSocket)
{
	if (hSocket == -1)
	{
		return;
	}

	m_hSocket = hSocket;
}
// ���÷��ͻ�������С
INT CBaseSocket::SetSendBuf(INT iBufSize)
{
	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	if ( iBufSize <= 0 )
	{
		return -1;
	}

	// ���ط�0ֵ��ʾ�ɹ�
	if ( setsockopt(m_hSocket,SOL_SOCKET,SO_SNDBUF,(const char *)&iBufSize,sizeof(int)) == 0 )
	{
		return -2;
	}

	return 0;

}

// ���ý��ջ�������С
INT	CBaseSocket::SetRcvBuf(INT iBufSize)
{
	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	if ( iBufSize <= 0 )
	{
		return -1;
	}

	// ���ط�0ֵ��ʾ�ɹ�
	if ( setsockopt(m_hSocket,SOL_SOCKET,SO_RCVBUF,(const char *)&iBufSize,sizeof(int)) == 0 )
	{
		return -2;
	}

	return 0;
	;

}
/********************************************************************************
  Function:		TestSocket
  Description:	����sokcet�Ƿ�����
  Input:  		
  Output:      	   
  Return:  		       
  Note:			��Ҫ���ڷ�����ģʽ�����		
  Author:        	CHC
  Date:				2010/09/06
********************************************************************************/
INT	CBaseSocket::TestSocket(INT iFlag)
{
	timeval timeout;
	int rt;
	fd_set fd,efd;
	FD_ZERO(&fd);
	FD_SET(m_hSocket,&fd);
	FD_ZERO(&efd);
	FD_SET(m_hSocket,&efd);
	timeout.tv_sec=3;//3��
	timeout.tv_usec=0;
	switch(iFlag)
	{
	case 0:	// read
		rt=select(m_hSocket+1,&fd,NULL,&efd,&timeout);
		break;
	case 1: // write
		rt=select(m_hSocket+1,NULL,&fd,&efd,&timeout);
		break;
	case 2: // error
	default:
		rt=select(m_hSocket+1,NULL,NULL,&efd,&timeout);
		break;
	}
	if( rt>0 )
	{		
		if( FD_ISSET(m_hSocket,&efd) )
		{
			//����			
			return -1;
		}
		return 0x1ffff;
	}
	return rt;
}

INT	CBaseSocket::TestSocketEx(INT iFlag, INT iTimeOut, INT& iErrNo)
{
	timeval timeout;
	int rt;
	fd_set fd,efd;
	FD_ZERO(&fd);
	FD_SET(m_hSocket,&fd);
	FD_ZERO(&efd);
	FD_SET(m_hSocket,&efd);
	timeout.tv_sec=iTimeOut;
	timeout.tv_usec=0;
	switch(iFlag)
	{
	case 0:	// read
		rt=select(m_hSocket+1,&fd,NULL,&efd,&timeout);
		break;
	case 1: // write
		rt=select(m_hSocket+1,NULL,&fd,&efd,&timeout);
		break;
	case 2: // error
	default:
		rt=select(m_hSocket+1,NULL,NULL,&efd,&timeout);
		break;
	}

	if( rt>0 )
	{		
		if( FD_ISSET(m_hSocket,&efd) )
		{
			//����
			iErrNo = GetLastError();
			return -1;
		}
		return 0x1ffff;
	}
	else if( rt < 0 )
	{
		iErrNo = GetLastError();
	}
	
	return rt;

}

void CBaseSocket::SetBlockModeEx( BOOL bMode)
{
	if ( NULL == m_hSocket)
	{
		return;
	}

	/*
	����Ϊ��������������ʽ
	*/

#if _WIN32

	u_long mode = bMode;
	ioctlsocket(m_hSocket,FIONBIO,&mode);

#else
	
	if ( bMode )
	{
		// ������
		int val = fcntl(m_hSocket, F_GETFL);
		fcntl(m_hSocket, F_SETFL, val | O_NONBLOCK);
	}
	else
	{
		// ����

		int val = fcntl(m_hSocket, F_GETFL);
		fcntl(m_hSocket, F_SETFL, val & O_NONBLOCK);
	}
#endif

	m_bBlock = bMode;
	
}



SOCKETHANDLE CBaseSocket::GetSocket()
{
	if ( m_hSocket <= 0 )
	{
		return -1;
	}
	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	if ( m_hSocket <= 0 )
	{
		return -1;
	}
	return m_hSocket;
}
