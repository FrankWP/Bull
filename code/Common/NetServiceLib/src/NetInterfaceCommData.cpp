#include "NetInterfaceCommData.h"
#include "ServerChannel.h"

using namespace NetServiceLib;

// ��־ָ��
ILogLibrary*			NetServiceLib::g_clsLogPtr = NULL;

#define		MAX_ACTIVE_TIME		0	//ͨ����������δͨѶʱ�䣬��λ�롣Ĭ��Ϊ0��ʾ��ʹ�øù���
#define     MAX_CHANNEL_COUNT	65535 //���ͨ����Ĭ��ֵ�������ϲ�ָ��

CNetInterfaceCommData::CNetInterfaceCommData(void)
{
	m_bReConnect = false;
	m_bMsgBufFlag = false;
	m_unActiveTime = MAX_ACTIVE_TIME;
	m_unMaxChannel = MAX_CHANNEL_COUNT;

	m_pUserData = NULL;
	m_pfnOnEventCallBack = NULL;
	m_vectorSocketChannel.clear();

#if OPERATING_SYSTEM
	m_bBlockMode = TRUE;

#elif _WIN32

	m_bBlockMode = FALSE;
#else
	// linux Ĭ��Ϊ������. ��Ϊlinux�Ľ��ջ��ƺ�win��ͬ��epoll����ɶ˿�������ͬ
	m_bBlockMode = TRUE;

#endif
	

	m_unSocketSendBuf = 0;
	m_unSocketRcvBuf = 0;

	m_clsLogPtr = NULL;
}

CNetInterfaceCommData::~CNetInterfaceCommData(void)
{
	m_GSMutexVerChnl.Lock();
	for ( VectorChannelPoint::size_type i=0; i< m_vectorSocketChannel.size(); i++ )
	{
		//����ͷ�
		if ( NULL != m_vectorSocketChannel[i])
		{
			m_vectorSocketChannel[i]->CloseChannel();
			delete m_vectorSocketChannel[i];
			m_vectorSocketChannel[i] = NULL;
		}
	}

	m_vectorSocketChannel.clear();

	m_GSMutexVerChnl.Unlock();

	m_GSMutexVerFaultChnl.Lock();

	VectorChannelPoint::size_type i=0;
	for ( i=0; i< m_vectorFaultChannel.size(); i++)
	{
		
		//����ͷ�
		if ( NULL != m_vectorFaultChannel[i])
		{
			m_vectorFaultChannel[i]->CloseChannel();
			delete m_vectorFaultChannel[i];
			m_vectorFaultChannel[i] = NULL;
		}

	}

	m_vectorFaultChannel.clear();

	m_GSMutexVerFaultChnl.Unlock();

	if ( m_clsLogPtr )
	{
		if( g_clsLogPtr == m_clsLogPtr ) // zouyx ���
		{
			//g_clsLogPtr = NULL; // zouyx ���
			MSLEEP(50); // zouyx ���
		} 
		//delete m_clsLogPtr;
		m_clsLogPtr = NULL;
	}

}
void CNetInterfaceCommData::Init()
{
	// ����Ҫ��ʼ��
}
//����ͨ��
INT	CNetInterfaceCommData::SaveSocketChannel(CSocketChannel* pSocketChannel)
{
	if (pSocketChannel == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}
	
	m_GSMutexVerChnl.Lock();

	vector<CSocketChannel*>::iterator pIter;
	pIter = find(m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), pSocketChannel);

	if (pIter == m_vectorSocketChannel.end())
	{
		m_vectorSocketChannel.push_back(pSocketChannel);

		m_GSMutexVerChnl.Unlock();

		return ERROR_BASE_SUCCESS;
	}

	

	m_GSMutexVerChnl.Unlock();

	return ERROR_NET_UNKNOWN;
}
INT	CNetInterfaceCommData::DeleteSocketChannel(CSocketChannel* pSocketChannel)
{

	if (pSocketChannel == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_GSMutexVerChnl.Lock();

	vector<CSocketChannel*>::iterator pIter;
	pIter = find(m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), pSocketChannel);

	if (pIter != m_vectorSocketChannel.end())
	{
		m_vectorSocketChannel.erase(pIter);

		m_GSMutexVerChnl.Unlock();

		return ERROR_BASE_SUCCESS;
	}

	m_GSMutexVerChnl.Unlock();

	return ERROR_NET_CHANNEL_NOT_EXIST;

}

/********************************************************************************
  Function:       GetExistSocketChannel
  Description:    
  Input:  
  Output:         
  Return:        CSocketChannel*	���ھͷ��ظ�ͨ��ָ�룬��������ھͷ���NULL 
  Note:				
********************************************************************************/
CSocketChannel*	 CNetInterfaceCommData::GetExistSocketChannel( DWORD dwRemoteIP, UINT16 unRemotePort )
{
	m_GSMutexVerChnl.Lock();

	for ( VectorChannelPoint::size_type i=0; i< m_vectorSocketChannel.size(); i++)
	{

		//���� ������� �򵥵�
		DWORD   dwIP;
		UINT16	unPort = 0;
		m_vectorSocketChannel[i]->GetDWORDRemoteIPPort(dwIP, unPort);
		if ( dwIP == dwRemoteIP && unPort == unRemotePort)
		{
			
			m_GSMutexVerChnl.Unlock();
			return m_vectorSocketChannel[i];
		}

	}

	m_GSMutexVerChnl.Unlock();

	m_GSMutexVerFaultChnl.Lock();

	for ( VectorChannelPoint::size_type i=0; i< m_vectorFaultChannel.size(); i++)
	{

		//���� ������� �򵥵�
		DWORD   dwIP;
		UINT16	unPort = 0;
		m_vectorFaultChannel[i]->GetDWORDRemoteIPPort(dwIP, unPort);
		if ( dwIP == dwRemoteIP && unPort == unRemotePort)
		{
			
			m_GSMutexVerFaultChnl.Unlock();
			return m_vectorFaultChannel[i];
		}

	}

	m_GSMutexVerFaultChnl.Unlock();

	return NULL;
}

/********************************************************************************
  Function:		IfExistSocketChannel
  Description:	����ͨ��ָ���ж�ͨ���Ƿ���ڡ����ڷ���TRUE�������ڷ���FALSE
  Input:  		pclsSocketChannel
  Output:      	   
  Return:  		TRUE ͨ������ FALSE ͨ��������       
  Note:					
  Author:        	CHC
  Date:				2010/09/25
********************************************************************************/
BOOL CNetInterfaceCommData::IfExistSocketChannel( CSocketChannel* pclsSocketChannel )
{
	if ( NULL == pclsSocketChannel )
	{
		return FALSE;
	}
	m_GSMutexVerChnl.Lock();

	VectorChannelPoint::iterator pIter = find( m_vectorSocketChannel.begin(), m_vectorSocketChannel.end(), pclsSocketChannel);

	if ( pIter != m_vectorSocketChannel.end() )
	{
		// �ҵ�
		m_GSMutexVerChnl.Unlock();
		return TRUE;
	}

	m_GSMutexVerChnl.Unlock();

	m_GSMutexVerFaultChnl.Lock();

	pIter = find( m_vectorFaultChannel.begin(), m_vectorFaultChannel.end(), pclsSocketChannel);

	if ( pIter != m_vectorFaultChannel.end() )
	{
		// �ҵ�
		m_GSMutexVerFaultChnl.Unlock();
		return TRUE;
	}	

	m_GSMutexVerFaultChnl.Unlock();

	return NULL;
}

INT	CNetInterfaceCommData::SetOnEventCallBack(void* pUserData, pOnEventCallBack OnEventCallBack)
{
	if ( NULL == OnEventCallBack)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_pUserData = pUserData;
	m_pfnOnEventCallBack = OnEventCallBack;

	return ERROR_BASE_SUCCESS;
}

INT	CNetInterfaceCommData::OnEventCallBack(ISocketChannel* pSocketChnl,  enumNetEventType enumType, void* pData, UINT32 uiDataSize)
{
	//pData������null �����������ʱ
	if (pSocketChnl == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}
	
	if ( m_pfnOnEventCallBack != NULL )
	{
		
		
		return m_pfnOnEventCallBack(pSocketChnl, m_pUserData, enumType, pData, uiDataSize);
	}
	
	return ERROR_NET_CALLBACK_NOT_INIT;
	
}
/********************************************************************************
  Function:       TestActiveTime
  Description:    ͨ���Ļʱ����
  Input:  
  Output:         
  Return:         
  Note:				����ͨ��������ʱ��� 
					//ɾ��ͨ��CSocketChannelʱ �����Ƕ�д��������ʱ. 
					�����ʱ֪ͨ�ϲ���ϲ㲻��ǣ���һ�μ���ֳ�ʱ���죿 Ƶ����֪ͨ�ϲ㣿
********************************************************************************/
INT	CNetInterfaceCommData::TestActiveTime()
{
	if ( m_vectorSocketChannel.size() == 0)
	{
		return ERROR_NET_UNKNOWN;
	}

	m_GSMutexVerChnl.Lock();

	for ( VectorChannelPoint::size_type i=0; i< m_vectorSocketChannel.size(); i++)
	{
		CSocketChannel*  pSocketChannel = (CSocketChannel*)m_vectorSocketChannel[i];
		if ( COMM_CHANNEL == pSocketChannel->GetChannelType() && pSocketChannel->GetChannelStatus() != CHANNEL_TIMEOUT )
		{
			if (DoGetTickCount() - pSocketChannel->GetLastActiveTime() > m_unActiveTime * 1000)
			{
				//֪ͨ�û���ʱ
				OnEventCallBack(pSocketChannel, NET_TIMEOUT, NULL, 0); //�������ص�ִ�кܾã���ô���m_GSWRMutex�������ܾ�
				pSocketChannel->SetChannelStatus( CHANNEL_TIMEOUT );//����Ϊ��ʱ
			}
		}
		
	}

	m_GSMutexVerChnl.Unlock();

	

	return ERROR_BASE_SUCCESS;
}
/********************************************************************************
  Function:       TestMaxChannelCount
  Description:    ������������
  Input:  
  Output:         
  Return:         true: �Ѿ��ﵽ���������     false��δ�ﵽ���������
  Note:				
********************************************************************************/
bool CNetInterfaceCommData::TestMaxChannelCount()
{
	m_GSMutexVerChnl.Lock();
	m_GSMutexVerFaultChnl.Lock();


	if (m_vectorSocketChannel.size() + m_vectorFaultChannel.size() >= m_unMaxChannel)
	{
		m_GSMutexVerFaultChnl.Unlock();
		m_GSMutexVerChnl.Unlock();
		return true;
	}

	m_GSMutexVerFaultChnl.Unlock();
	m_GSMutexVerChnl.Unlock();

	return false;

}


/********************************************************************************
  Function:		GetAllChannelNum
  Description:	��ȡ����ͨ����Ŀ
  Input:  		
  Output:      	   
  Return:  		       
  Note:					
********************************************************************************/
INT	CNetInterfaceCommData::GetAllChannelNum()
{
	m_GSMutexVerChnl.Lock();
	m_GSMutexVerFaultChnl.Lock();

	INT16 iNum = m_vectorSocketChannel.size() + m_vectorFaultChannel.size();

	m_GSMutexVerFaultChnl.Unlock();
	m_GSMutexVerChnl.Unlock();

	return iNum;

}

//�������ͨ������
INT	CNetInterfaceCommData::SaveToFaultVector(CSocketChannel* pSocketChannel)
{
	if (pSocketChannel == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_GSMutexVerFaultChnl.Lock();

	VectorChannelPoint::iterator pIter;
	pIter = find(m_vectorFaultChannel.begin(), m_vectorFaultChannel.end(), pSocketChannel);

	if (pIter == m_vectorFaultChannel.end())
	{
		m_vectorFaultChannel.push_back(pSocketChannel);
		m_GSMutexVerFaultChnl.Unlock();

		return ERROR_BASE_SUCCESS;
	}	

	m_GSMutexVerFaultChnl.Unlock();

	return ERROR_NET_UNKNOWN;

}

//�ӹ���ͨ��������ɾ��ͨ�������ͷ�ͨ����Դ����һ����
INT	CNetInterfaceCommData::DeleteoFromFaultVector( CSocketChannel* pSocketChannel )
{
	if (pSocketChannel == NULL)
	{
		return ERROR_NET_PARAM_WRONG;
	}

	m_GSMutexVerFaultChnl.Lock();

	VectorChannelPoint::iterator pIter;
	pIter = find(m_vectorFaultChannel.begin(), m_vectorFaultChannel.end(), pSocketChannel);

	if (pIter != m_vectorFaultChannel.end())
	{
		m_vectorFaultChannel.erase(pIter);

		m_GSMutexVerFaultChnl.Unlock();

		return ERROR_BASE_SUCCESS;
	}

	m_GSMutexVerFaultChnl.Unlock();

	return ERROR_NET_CHANNEL_NOT_EXIST;
}
//�������ͨ����
INT	CNetInterfaceCommData::SetMaxChannel(UINT16 unMaxChannel)
{
	m_GSMutexVerChnl.Lock();

	if ( unMaxChannel > MAX_CHANNEL_COUNT || unMaxChannel < 2)
	{
		m_unMaxChannel = MAX_CHANNEL_COUNT;
	}
	else
	{
		m_unMaxChannel = unMaxChannel; 

	}

	m_GSMutexVerChnl.Unlock();
	
	return ERROR_BASE_SUCCESS;
}




// ����socket���ͻ�������С
INT	CNetInterfaceCommData::SetSendBuf(INT iBufSize)
{
	
	if ( iBufSize <=0 )
	{
		return -1;
	}
	m_GSMutexVerChnl.Lock();

	m_unSocketSendBuf = iBufSize;

	m_GSMutexVerChnl.Unlock();

	return ERROR_BASE_SUCCESS;

}
// ����socket���ջ�������С
INT	CNetInterfaceCommData::SetRcvBuf(INT iBufSize)
{
	
	if ( iBufSize <=0 )
	{
		return -1;
	}

	m_GSMutexVerChnl.Lock();

	m_unSocketRcvBuf = iBufSize;

	m_GSMutexVerChnl.Unlock();

	return ERROR_BASE_SUCCESS;
}

// ������־·��
void CNetInterfaceCommData::SetLogPath( const char*	czPathName )
{
	if ( NULL == czPathName || strlen(czPathName) == 0)
	{
		return;
	}

	m_GSMutexVerChnl.Lock();

	if ( NULL == m_clsLogPtr )
	{
		m_clsLogPtr = GetLogInstance();
		g_clsLogPtr = m_clsLogPtr;
	}

	char szPathName[256] = { 0 };
	strcpy(szPathName,czPathName);

	INT iLen = strlen(szPathName);

	if ( szPathName[iLen-1] != '\\' && szPathName[iLen-1] != '/')
	{
		strcat( szPathName, "/" );
	}

	strcat(szPathName, "NetServiceLog/");
	m_clsLogPtr->SetLogDir(szPathName, "NetService");
	m_clsLogPtr->SetLogLevel(MFILE, LNOTSET);
	m_clsLogPtr->SetLogSize(WCREATE, 100);

	m_GSMutexVerChnl.Unlock();

}
// ������־��ָ�룬����ͬʱ��ʼ���������ˡ��ͻ��˵����
void CNetInterfaceCommData::SetLogInstancePtr(ILogLibrary* pLog)
{ 
	if (pLog != NULL)
	{
		m_clsLogPtr = pLog;
	}
}

/**************************************************************************
Function    : Utility::GetApplicationPath    
DateTime    : 2010/8/26 11:13	
Description : ��ȡӦ�ó���Ŀ¼
Input       : ��
Output      : ��
Return      : ����Ӧ�ó���Ŀ¼
Note        :	
**************************************************************************/
string NetServiceLib::GetApplicationPath(void)
{
#ifdef WINCE
	return "";

#elif _WIN32
	// ��ȡִ�г����ļ�·��
	CHAR exeFullPath[MAX_PATH];
	ZeroMemory(exeFullPath, MAX_PATH);
	GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);

	string AppPath = exeFullPath;
	return AppPath.substr(0, AppPath.rfind("\\")) + "\\";
#else

	CHAR exeFullPath[260];
	memset(exeFullPath, 0x0, 260);
	int n;

	n = readlink("/proc/self/exe" , exeFullPath , sizeof(exeFullPath));

	string AppPath = "";
	if( n > 0 && n < sizeof(exeFullPath))
	{
		AppPath = exeFullPath;

		return AppPath.substr(0, AppPath.rfind("/")) + "/";
	}
	return "./";	// CHC�޸� 2010-10-18 linux�ĵ�ǰ·��
#endif
}





