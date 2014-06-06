#include "ServerChannel.h"

using namespace NetServiceLib;

INT	CServerChannel::CreateChannel(CBaseSocket* pclsSocket)
{
	return pclsSocket->Visitor(this);
	
}
INT	CServerChannel::CreaterTcpChannel(CBaseSocket* pclsSocket)
{
	//���շ������˵Ĳ��轨��tcpͨ��
	//CTRLPRINTF("CreaterTcpChannel\n");
	return	pclsSocket->CreateSocket(m_szLocalIP, m_unLocalPort,1);
	
}
INT	CServerChannel::CreaterUdpChannel(CBaseSocket* pclsSocket)
{
	//���շ������˵Ĳ��轨��UDPͨ��
	return pclsSocket->CreateSocket(m_szLocalIP, m_unLocalPort);	
}




