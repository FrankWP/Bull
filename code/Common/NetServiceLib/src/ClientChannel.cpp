#include "ServerChannel.h"

using namespace NetServiceLib;

INT	CClientChannel::CreateChannel(CBaseSocket* pclsSocket)
{
	return pclsSocket->Visitor(this);
}
INT	CClientChannel::CreaterTcpChannel(CBaseSocket* pclsSocket)
{
	//���տͻ��˵Ĳ��轨��tcpͨ��
	pclsSocket->CreateSocket(m_szLocalIP, m_unLocalPort);
	return pclsSocket->Connect(m_szRemoteHost, m_unRemotePort);
}
INT	CClientChannel::CreaterUdpChannel(CBaseSocket* pclsSocket)
{
	//���շ������˵Ĳ��轨��UDPͨ��
	return pclsSocket->CreateSocket(m_szLocalIP, m_unLocalPort);
}

