#if _LINUX
#include "LinuxSocket.h"

using namespace NetServiceLib;

CLinuxUdpSocket::CLinuxUdpSocket(void)
{

}

CLinuxUdpSocket:: ~CLinuxUdpSocket(void)
{
	CloseSocket();
}

INT	CLinuxUdpSocket::Visitor(ICreater* pclsVisitor)
{
	return pclsVisitor->CreaterUdpChannel(this);
}
INT	CLinuxUdpSocket::CreateSocket(const char* pszHost,UINT16 unPort)//����socket ����IP�˿�
{
	//pszhostҪ�ж�
	m_hSocket  = socket( AF_INET, SOCK_DGRAM, 0 );

	if (m_hSocket  == -1)
	{
		printf(" bind fail");
		return ERROR_NET_CREATE_SOCKET_FAIL;
	}	

	SetSockOption();//����Ϊ�����������ຯ��

	sockaddr_in service;						//�����׽��ֵĵ�ַ
	service.sin_family = AF_INET;				//�����IP��ַ һֱ�����AF_INET

	if ( strlen(pszHost) == 0 || pszHost[0] == '0')
	{
		service.sin_addr.s_addr = INADDR_ANY;

	}
	else
	{
		inet_aton( pszHost, &service.sin_addr);
	}

	service.sin_port = htons(unPort);	

	if (bind( m_hSocket , (sockaddr*) &service, sizeof(service)) == -1) 
	{
		close(m_hSocket);
		m_hSocket = -1;
		printf(" bind fail");
		return ERROR_NET_BIND_SOCKET_FAIL;
	}

	return ERROR_BASE_SUCCESS;
}
INT	CLinuxUdpSocket::CloseSocket()//�رսӿ�
{
	if (m_hSocket != -1)
	{
		shutdown(m_hSocket,2);
		close(m_hSocket);//�ر��׽���
		m_hSocket = -1;

		return ERROR_BASE_SUCCESS;
	}

	return ERROR_NET_UNKNOWN;


}

/********************************************************************************
Function:		SetSockOptionSec
Description:	����linux socket���еĲ���
Input:  		
Output:      	   
Return:  		0��ʾ�ɹ�����ֵʧ��       
Note:					
Author:        	CHC
Date:				2010/09/03
********************************************************************************/
INT	CLinuxUdpSocket::SetSockOptionSec()
{
	SetBlockModeEx( m_bBlock );

	struct linger ling;	
	ling.l_onoff=1;		//�ڵ���closesocket()ʱ��������δ�����꣬����ȴ�
	ling.l_linger=0;	//�ȴ�ʱ��2��
	setsockopt(m_hSocket,SOL_SOCKET,SO_LINGER,(const   char*)&ling,sizeof(ling)); 

	return 0;

}

INT	CLinuxUdpSocket::Connect(const char *pszDesHost, UINT16 unDesPort)//��������
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;

}
INT	CLinuxUdpSocket::Listen()//����
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;
}
SOCKETHANDLE CLinuxUdpSocket::AcceptConnect(struct sockaddr* pstruAddr, INT* piAddrLen)
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;
}
INT	CLinuxUdpSocket::SendData(void*	pData, UINT	uiLen)
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;
}
INT	CLinuxUdpSocket::SendDataTo(void*	pData, UINT	uiLen, char* pszDesIP, UINT16 unDesPort)
{
	if ( NULL == pData   || uiLen <= 0 || NULL == pszDesIP)
	{
		return ERROR_BASE_SUCCESS; //����0���� ���������ֵ �û�����˵������Ϊ�Ƿ����˲�������
	}

	CGSAutoMutex	GAutoMutex( &m_GSMutex );

	sockaddr_in desSockadr;	

	desSockadr.sin_family = AF_INET;				
	
	struct hostent *hp = NULL;
	if( 0 == inet_aton(pszDesIP, &desSockadr.sin_addr) )
	{ //ת��ʧ������������

		#ifdef HAVE_GETHOSTBYNAME2
		hp=(struct hostent*)gethostbyname2( pszDesIP, AF_INET );
		#else
		hp=(struct hostent*)gethostbyname( pszDesIP );
		#endif

		if( hp==NULL ) 
		{	
			return ERROR_NET_PARAM_WRONG;
		}	

		memcpy( (void*)&desSockadr.sin_addr, (void*)hp->h_addr_list[0], hp->h_length );
	}   

	desSockadr.sin_port = htons(unDesPort);			//�˿�

	return sendto(m_hSocket, (char*)pData, uiLen, 0, (sockaddr*)&desSockadr, sizeof(sockaddr));
}
INT	CLinuxUdpSocket::RecvData(void* pstruRecv)
{
	LPPER_IO_OPERATION_DATA	pIOCP_IOData_Recv = (LPPER_IO_OPERATION_DATA)pstruRecv;
	return recvfrom( m_hSocket, pIOCP_IOData_Recv->DataBuf.buf, pIOCP_IOData_Recv->DataBuf.len, 0,
		(sockaddr*)&(pIOCP_IOData_Recv->struSockAddrFrom), (socklen_t*)&(pIOCP_IOData_Recv->iAddrFromLen) );	
}

INT	CLinuxUdpSocket::SendDataEx(void*	pData, UINT	uiLen)
{
	//UDP����Ҫ
	return ERROR_NET_UNKNOWN;
}

#endif	//end #if _WIN32


