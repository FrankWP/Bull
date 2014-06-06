#ifndef BASE_CHANNEL_DEF_H
#define BASE_CHANNEL_DEF_H

class CBaseChannel
{
public:
	CBaseChannel();
	virtual ~CBaseChannel();


public:
	char* BaseGetRemoteIPAddrString();
	UINT32 BaseGetRemoteIPAddr();
	char* BaseGetLocalIPAddrString();
	UINT32 BaseGetLocalIPAddr();
	UINT16 BaseGetRemotePort();
	UINT16 BaseGetLocalPort();

	void SetRemoteIPAddrString(char* pszIPAddr);
	void SetRemoteIPAddr(UINT32 uiIPAddr);
	void SetLocalIPAddrString(char* pszIPAddr);
	void SetLocalIPAddr(UINT32 uiIPAddr);
	void SetRemotePort(UINT16 uiPort);
	void SetLocalPort(UINT16 uiPort);
	//�߼���Զ��ip��ַ����һ������������socket���ӵĵ�ַ�����û�����ѡ�����øõ�ַ��
	//���磬����BS��½������ʱ������BS�ͻ��˵ĵ�ַ��������tomcat�ĵ�ַ��
	void SetLogicRemoteIPAddrString(const char* pszIPAddr);
	//�߼���Զ��ip��ַ����һ������������socket���ӵĵ�ַ�����û�����ѡ�����øõ�ַ��
	//���磬����BS��½������ʱ������BS�ͻ��˵ĵ�ַ��������tomcat�ĵ�ַ��
	char* GetLogicRemoteIPAddrString();

protected:
	char m_szRemoteIPAddr[20];
	char m_szBindingIPAddr[20];
	UINT32 m_uiRemoteIPAddr;
	UINT32 m_uiLocalIPAddr;
	UINT16 m_uiRemotePort;
	UINT16 m_uiLocalPort;
	//�߼���Զ��ip��ַ����һ������������socket���ӵĵ�ַ�����û�����ѡ�����øõ�ַ��
	//���磬����BS��½������ʱ������BS�ͻ��˵ĵ�ַ��������tomcat�ĵ�ַ��
	char	m_szLogicRemoteIPAddr[16];
};


#endif