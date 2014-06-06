#if _WIN32

#include "BaseSocket.h"

using namespace NetServiceLib;

CWinUdpSocket::CWinUdpSocket(void)
{

}

CWinUdpSocket:: ~CWinUdpSocket(void)
{
	CloseSocket();
}
INT	CWinUdpSocket::Visitor(ICreater* pclsVisitor)
{
	return pclsVisitor->CreaterUdpChannel(this);
}
INT	CWinUdpSocket::CreateSocket(const char* pszHost,UINT16 unPort, INT iExclusive)//����socket ����IP�˿�
{
	//pszhostҪ�ж�
	if ( NULL == pszHost)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_hSocket  = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0,WSA_FLAG_OVERLAPPED);

	if (m_hSocket  == INVALID_SOCKET)
	{
		m_hSocket = NULL;
		return ERROR_NET_CREATE_SOCKET_FAIL;
	}	

	// �����׽���Ϊ��������
	SetSockOption();

	sockaddr_in service;						//�����׽��ֵĵ�ַ
	service.sin_family = AF_INET;				//�����IP��ַ һֱ�����AF_INET
	if ( strlen(pszHost) == 0 || pszHost[0] == '0')
	{
		service.sin_addr.s_addr = INADDR_ANY;

	}
	else
	{
		service.sin_addr.s_addr = inet_addr(pszHost);//htonl(INADDR_ANY);//INADDR_ANY��ʾ��������������IP������ʹ�ø��׽��� ��Щ�����ж������IP
	}
	service.sin_port = htons(unPort);	

	if (BindSocket(service) == -1) 
	{
		closesocket(m_hSocket);
		m_hSocket = NULL;
		return ERROR_NET_BIND_SOCKET_FAIL;
	}

	return ERROR_BASE_SUCCESS;
}
INT	CWinUdpSocket::BindSocket(sockaddr_in& service)
{
	if (bind( m_hSocket , (SOCKADDR*) &service, sizeof(service)) == SOCKET_ERROR) 
	{
		return -1;
	}

	return 0;
}
INT	CWinUdpSocket::CloseSocket()//�رսӿ�
{
	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	if (m_hSocket != NULL)
	{
		shutdown(m_hSocket,SD_BOTH);
		closesocket(m_hSocket);//�ر��׽���
		m_hSocket = NULL;
		return ERROR_BASE_SUCCESS;
	}


	return ERROR_NET_UNKNOWN;

}

/********************************************************************************
Function:		SetSockOptionSec
Description:	����windows socket ���еĲ���
Input:  		
Output:      	   
Return:  		       
Note:					
Author:        	CHC
Date:				2010/09/03
********************************************************************************/
INT	CWinUdpSocket::SetSockOptionSec()
{
	// UDPûʲô�����õ�

	/*
	����Ϊ��������������ʽ
	*/
	u_long mode = m_bBlock;
	ioctlsocket(m_hSocket,FIONBIO,&mode);

	struct linger ling;	
	ling.l_onoff=1;		//�ڵ���closesocket()ʱ��������δ�����꣬����ȴ�
	ling.l_linger=0;	//�ȴ�ʱ��0��
	setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,(const   char*)&ling,sizeof(ling));  

	return 0;

}
INT	CWinUdpSocket::Connect(const char *pszDesHost, UINT16 unDesPort)//��������
{
	//����Ҫ
	return ERROR_NET_UNKNOWN;

}
INT	CWinUdpSocket::Listen()//����
{
	//����Ҫ
	return ERROR_NET_UNKNOWN;
}
SOCKETHANDLE CWinUdpSocket::AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen)
{
	//����Ҫ
	return ERROR_NET_UNKNOWN;
}

INT	CWinUdpSocket::SendData(void*	pData, UINT	uiLen)
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;

}
INT	CWinUdpSocket::SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort)
{
	if ( NULL == pData   || uiLen <= 0 || NULL == pszDesIP)
	{
		return ERROR_BASE_SUCCESS; //����0���� ���������ֵ �û�����˵������Ϊ�Ƿ����˲�������
	}

	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	sockaddr_in desSockadr;	

	desSockadr.sin_family = AF_INET;				
	desSockadr.sin_addr.s_addr = inet_addr(pszDesIP);//IP
	desSockadr.sin_port = htons(unDesPort);			//�˿�
	
	return sendto(m_hSocket, (char*)pData, uiLen, 0, (sockaddr*)&desSockadr, sizeof(sockaddr));

}
INT	CWinUdpSocket::RecvData(void* pstruRecv )
{
	if ( NULL == pstruRecv )
	{
		return ERROR_NET_PARAM_WRONG;
	}	

	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	LPPER_IO_OPERATION_DATA	pIOCP_IOData_Recv = (LPPER_IO_OPERATION_DATA)pstruRecv;
	DWORD	dwRecvBytes = 0;
	DWORD	dwFlags = 0;

	if ( m_hSocket )
	{
		
		int iRet = -1;

#if OPERATING_SYSTEM

		return recvfrom( m_hSocket, pIOCP_IOData_Recv->DataBuf.buf, pIOCP_IOData_Recv->DataBuf.len, 0,
			(sockaddr*)&(pIOCP_IOData_Recv->struSockAddrFrom), (INT32*)&(pIOCP_IOData_Recv->iAddrFromLen) );
#elif _WIN32

		iRet = WSARecvFrom( m_hSocket, &(pIOCP_IOData_Recv->DataBuf), 1, &dwRecvBytes, &dwFlags, 
			(sockaddr*)&(pIOCP_IOData_Recv->struSockAddrFrom), &(pIOCP_IOData_Recv->iAddrFromLen), &(pIOCP_IOData_Recv->Overlapped), NULL );
#endif


		if( iRet != 0 ) 
		{
			if(WSAGetLastError() == WSA_IO_PENDING  ) 
			{
				iRet = 0; 
			}
			else
			{
				return ERROR_NET_PARAM_WRONG;
			}
		}

		return ERROR_BASE_SUCCESS;
	}
	else
	{
		return ERROR_NET_PARAM_WRONG;
	}

	
	
}

INT	CWinUdpSocket::SendDataEx(void*	pData, UINT	uiLen)
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;
}

#endif //end #if _WIN32


