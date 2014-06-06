
#   include "IAsyncIO.h"

#ifdef GSP_ASYNCIO_USED_IOCP

#   include <winsock2.h>
#	include <ws2tcpip.h>
#	include "mswsock.h"
#	pragma comment(lib, "ws2_32.lib")

#include "Log.h"
#include "ThreadPool.h"
#include "OSThread.h"
#include "GSPMemory.h"
#include "ISocket.h"
#include "md5.h"
#include <list>


using namespace GSP;




#define OVERLAPPED_EVT_READ             0x01
#define OVERLAPPED_EVT_WRITE            0x02
#define OVERLAPPED_EVT_EXIT             0x04
#define OVERLAPPED_EVT_RCVFROM          0x08

//����: һ�¶��岻���Ըı�
// #define BUSY_FLAGS_RCB					0x10
// #define BUSY_FLAGS_SCB					0x20
// #define BUSY_FLAGS_RTMP					0x40
// #define BUSY_FLAGS_STMP					0x80

namespace GSP
{

static GSAtomicInter _s_iiDebugSeq = 0;
static CISocket *_s_pExitKeyMark = ((CISocket *)&_s_iiDebugSeq); //�˳���Ϣ��KEY 



#define EXIT_SOCKET_KEY   ((CISocket *)&_s_iiDebugSeq)


typedef struct _StruASyncReleaseNode
{
	CISocket *pSocket;
	long iAddTime;
	long iTicks;
	_StruASyncReleaseNode(void) :
	pSocket(NULL), iAddTime(0),iTicks(0)
	{

	}
}StruASyncReleaseNode;



typedef struct _StruIOCPOverlapped
{
	OVERLAPPED stBase;            
	int iEvent;
}StruIOCPOverlapped;

class CMyIOCPPrivData
{
public :

    CGSCondEx csExitCond;
	CGSMutex  csMutex;

	std::vector<CProPacket *> vSendPackets;   
	INT iSendIdx;
	void *pSendKey;
	UINT64 iSendBytes;

	struct iovec stVSendIO[32];
	UINT iVSendIOCounts;

	CFixedBuffer *pRcvPktBuf;
	struct iovec stRcvIO;
	StruLenAndSocketAddr  stRcvFromAddr;


	
    UINT8  bUnregister;   //�Ѿ�ע�� 
	UINT8  bWaitUnregister;  
	UINT8  bRcvStart; //��ʼ����
    StruIOCPOverlapped stReadOverlapped;
    StruIOCPOverlapped stWriteOverlapped;

	UINT8 iFlags;

	INT  iWouldSendTotals;
	INT  iCurSendBeginIdx;



	GSAtomicInter iSeq; 

    CMyIOCPPrivData(void)
        : csExitCond()  
		, csMutex()
    {   
		iSeq = AtomicInterInc(_s_iiDebugSeq);

        bUnregister = 0; 
	
		bRcvStart = 0;	
		bWaitUnregister = 0;
		iVSendIOCounts = 0;	

        bzero(&stReadOverlapped, sizeof(StruIOCPOverlapped));
        bzero(&stWriteOverlapped, sizeof(StruIOCPOverlapped));

        stReadOverlapped.iEvent = OVERLAPPED_EVT_READ;
        stWriteOverlapped.iEvent = OVERLAPPED_EVT_WRITE;       
		iSendIdx = 0;
		iSendBytes = 0;
		pRcvPktBuf = NULL;
		pSendKey = NULL;		
		iFlags = 0;
		iWouldSendTotals = 0;
		iCurSendBeginIdx = 0;
       
    }

    ~CMyIOCPPrivData(void)
    {
        csExitCond.BroadcastSignal();		
		iSendIdx = 0;
		iSendBytes = 0;

		for( UINT i = 0; i<vSendPackets.size(); i++ )
		{
			vSendPackets[i]->UnrefObject();
		}
		vSendPackets.clear();

		if( pRcvPktBuf )
		{
			pRcvPktBuf->UnrefObject();
			pRcvPktBuf = NULL;           
		}
    }
	INLINE void SetSendData(void *pKey, std::vector<CProPacket *> vPkt)
	{
		GS_ASSERT(vSendPackets.empty() && iVSendIOCounts==0  );
		
		vSendPackets = vPkt;
		for( UINT i = 0; i<vPkt.size(); i++ )
		{
			vPkt[i]->RefObject();
		}
		pSendKey = pKey;
		iSendIdx = 0;
		iSendBytes = 0;
		iVSendIOCounts = 0;
	}

	INLINE void SetSendData(void *pKey, std::vector<CProFrame*> vFrame )
	{
		GS_ASSERT(vSendPackets.empty() && iVSendIOCounts==0 );
		
		CProPacket **ppPks = NULL;
		UINT iCounts;
		for( UINT i = 0; i<vFrame.size(); i++ )
		{
			ppPks = NULL;
			iCounts = vFrame[i]->GetPackets(&ppPks);
			for( UINT j = 0; j<iCounts; j++ )
			{
				vSendPackets.push_back(ppPks[j]);
				ppPks[j]->RefObject();
			}
		}
		pSendKey = pKey;
		iSendIdx = 0;
		iSendBytes = 0;
		iVSendIOCounts = 0;

	}

	INLINE BOOL PrepareSendData(void)
	{
		if( iSendIdx>= (INT) vSendPackets.size() )
		{
			return FALSE;
		}
		
		iVSendIOCounts = 0;
		iWouldSendTotals = 0;
		iCurSendBeginIdx = iSendIdx;
		while( iVSendIOCounts<ARRARY_SIZE(stVSendIO) && iSendIdx< (INT) vSendPackets.size() )
		{
			const StruPktInfo &stInfo = vSendPackets[iSendIdx]->GetParser();
			/*GS_ASSERT(stInfo.iHeaderSize==10);*/
			if( stInfo.iHeaderSize > 0 )
			{

				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bHeader;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iHeaderSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			if( stInfo.iPlayloadSize > 0 )
			{
				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bPlayload;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iPlayloadSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			if( stInfo.iTailerSize > 0 )
			{
				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bTailer;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iTailerSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			iSendIdx++;
		}
		return TRUE;
	}


	 BOOL PrepareSendData(int iSended )
	{
		if( iCurSendBeginIdx >= (INT) vSendPackets.size() )
		{
			return FALSE;
		}
		iVSendIOCounts = 0;
		iWouldSendTotals = 0;

		INT bFit = 0;
		int i = iCurSendBeginIdx;

		for(  ; !bFit && i < (INT) iSendIdx; i++  )
		{
			//�������
			const StruPktInfo &stInfo = vSendPackets[i]->GetParser();
			/*GS_ASSERT(stInfo.iHeaderSize==10);*/
			if( stInfo.iHeaderSize > 0 )
			{
				iSended -= stInfo.iHeaderSize;	
				if( iSended < 0 )
				{
					stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bHeader+stInfo.iHeaderSize+iSended;
					stVSendIO[iVSendIOCounts].iov_len = -iSended;
					iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
					iVSendIOCounts++;
					bFit = 1;
				}
				else if( iSended == 0 )
				{
					bFit = 1;					
				}
			}
			if( stInfo.iPlayloadSize > 0 )
			{			
				if( bFit )
				{
					stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bPlayload;
					stVSendIO[iVSendIOCounts].iov_len = stInfo.iPlayloadSize;
					iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
					iVSendIOCounts++;
				}
				else
				{

					iSended -= stInfo.iPlayloadSize;	
					if( iSended < 0 )
					{
						stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bPlayload+stInfo.iPlayloadSize+iSended;
						stVSendIO[iVSendIOCounts].iov_len = -iSended;
						iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
						iVSendIOCounts++;
						bFit = 1;
					}
					else if( iSended == 0 )
					{
						bFit = 1;						
					}
				}
			}
			if( stInfo.iTailerSize > 0 )
			{			
				if( bFit )
				{
					stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bTailer;
					stVSendIO[iVSendIOCounts].iov_len = stInfo.iTailerSize;
					iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
					iVSendIOCounts++;
					
				}
				else
				{

					iSended -=  stInfo.iTailerSize;
					if( iSended < 0 )
					{
						stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bTailer+stInfo.iTailerSize+iSended;
						stVSendIO[iVSendIOCounts].iov_len = -iSended;
						iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
						iVSendIOCounts++;
						bFit = 1;						
					}
					else if( iSended == 0 )
					{
						bFit = 1;
						
					}
				}				
			}
		}

		GS_ASSERT_RET_VAL( bFit && i>=0, FALSE);

		iCurSendBeginIdx =  i-1;
		
		iSendIdx = i;

		while( iVSendIOCounts<ARRARY_SIZE(stVSendIO) && iSendIdx< (INT) vSendPackets.size() )
		{
			const StruPktInfo &stInfo = vSendPackets[iSendIdx]->GetParser();
			/*GS_ASSERT(stInfo.iHeaderSize==10);*/
			if( stInfo.iHeaderSize > 0 )
			{

				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bHeader;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iHeaderSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			if( stInfo.iPlayloadSize > 0 )
			{
				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bPlayload;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iPlayloadSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			if( stInfo.iTailerSize > 0 )
			{
				stVSendIO[iVSendIOCounts].iov_base = (char*) stInfo.bTailer;
				stVSendIO[iVSendIOCounts].iov_len = stInfo.iTailerSize;
				iWouldSendTotals += stVSendIO[iVSendIOCounts].iov_len;
				iVSendIOCounts++;
			}
			iSendIdx++;
		}
		return TRUE;
	}


	INLINE void ClearSendData(void)
	{		

		for( UINT i = 0; i<vSendPackets.size(); i++ )
		{
			vSendPackets[i]->UnrefObject();
		}
		vSendPackets.clear();
		iSendIdx = 0;
		iSendBytes = 0;
		iVSendIOCounts = 0;
		
	}

	


    INLINE BOOL InitRcvBuffer(void)
    {
		GS_ASSERT(NULL==pRcvPktBuf);		
	
		pRcvPktBuf = pRcvPktBuf->Create();
		if( pRcvPktBuf )
		{
			stRcvIO.iov_base = (char *) pRcvPktBuf->m_bBuffer;
			stRcvIO.iov_len = pRcvPktBuf->m_iBufferSize;
			return TRUE;
		}
		return FALSE;
    }

	INLINE void UninitRcvBuffer(void)
	{
		if( pRcvPktBuf )
		{
			pRcvPktBuf->UnrefObject();			      
			pRcvPktBuf = NULL;
		}
	}

};


static CMyIOCPPrivData *GetPrivateData(void)
{
	return new CMyIOCPPrivData();
}

static void SafeFreePrivateData( CMyIOCPPrivData **ppData )
{
	if( ppData && *ppData )
	{
		delete *ppData;
		*ppData = NULL;
	}
}


class CWinIOCP : public CIAsyncIO
{
private :
	CGSPThreadPool m_csThreadPool;
	HANDLE m_hIOCPHandle;
	GSAtomicInter m_nThreads;
	GSAtomicInter m_iThreadConts;
	BOOL m_bRunning;
	BYTE *m_vThreadMask;

	std::list<StruASyncReleaseNode> m_listAsyncRelease;
	CGSMutex m_mutexListAsyncR;
	CGSThread m_threadAR;
public :
	CWinIOCP(INT iID);
	virtual ~CWinIOCP(void);
    
	virtual void Unregister( CISocket *pSocket );  
	virtual EnumErrno AsyncRcvFrom(CISocket *pSocket,BOOL bStart); 
	virtual EnumErrno AsyncRcv(CISocket *pSocket,BOOL bStart); 
	virtual EnumErrno AsyncAccept(CISocket *pSocket,	BOOL bStart);   
	virtual EnumErrno AsyncSend(CISocket *pSocket, void *pKey, std::vector<CProPacket *> vProPacket );
	virtual EnumErrno AsyncSend(CISocket *pSocket, void *pKey,std::vector<CProFrame *> vProFrame);

protected : 
	virtual INT  GetMaxChannelContain(void)
	{
		return 5000;
	}

	virtual INT  GetEmptyCounts(void)
	{
		return 3000;
	}
	virtual BOOL Init(void);
	virtual void Uninit(void);
	virtual BOOL Register(CISocket *pSocket, CNetError &csError);
private :
	void OnTaskPoolEvent( CObjThreadPool *pTkPool, void *pArgs );
	EnumErrno AsyncRcvRealy( CISocket *pSocket, INT &iSysErrno  , BOOL bInner  );
	EnumErrno AsyncSendRealy( CISocket *pSocket, INT &iSysErrno );
	EnumErrno AsyncRcvFromRealy(CISocket *pSocket, INT &iSysErrno , BOOL bInner  );
	BOOL IsFreeThread(void);
	void AReleaseEntry(void);

	static void AReleaseThreadCallback(CGSThread *pThread, void *pParam)
	{
		CWinIOCP *p = (CWinIOCP*)pParam;
		p->AReleaseEntry();
	}



};

} //end namespace GSP


#define MY_PRI(pPri)  ((CMyIOCPPrivData*)pPri)


CWinIOCP::CWinIOCP(INT iID)
:CIAsyncIO(iID)
,m_csThreadPool("WinIOCP")
{

    m_hIOCPHandle = NULL;
    m_bRunning = FALSE;     
    m_nThreads = 0;    
	m_iThreadConts = 0;
	m_vThreadMask = NULL;
}

CWinIOCP::~CWinIOCP(void)
{
	if( m_bRunning )
	{
		Uninit();
	}
	if( m_vThreadMask )
	{
		::free(m_vThreadMask);
		m_vThreadMask = NULL;
	}

	 std::list<StruASyncReleaseNode> listTemp;
	 m_mutexListAsyncR.Lock();
	 listTemp.swap(m_listAsyncRelease);
	 m_mutexListAsyncR.Unlock();

	for( std::list<StruASyncReleaseNode>::iterator csIt = listTemp.begin();
		csIt!=listTemp.end(); ++csIt )
	{
		csIt->pSocket->UnrefObject();
	}
}

BOOL CWinIOCP::Init(void)
{
    GS_ASSERT_RET_VAL(m_hIOCPHandle==NULL, FALSE);
    INT nThreads = 1; 
    nThreads = 2*COSThread::CurrentDeviceCPUNumber()+2;

	if( nThreads>16 )
	{
		//���ʹ��16 ���߳�
		nThreads = 16;
	} 
	else if( nThreads<8 )
	{
		nThreads = 8;
	}



	if( m_vThreadMask )
	{

		::free(m_vThreadMask);
	}
	m_vThreadMask = (BYTE *)::malloc( sizeof(BYTE)*(nThreads+2) );
	GS_ASSERT_RET_VAL(m_vThreadMask, FALSE);
	::memset(m_vThreadMask,1, sizeof(BYTE)*(nThreads+2) );

	m_iThreadConts = 0;

	INT bRet  = m_csThreadPool.Init(this, (FuncPtrObjThreadPoolEvent)&CWinIOCP::OnTaskPoolEvent,
						nThreads, TRUE);
    GS_ASSERT_RET_VAL(bRet, FALSE);
    m_hIOCPHandle = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE,
												NULL,(ULONG_PTR )0, nThreads );
    GS_ASSERT_RET_VAL(m_hIOCPHandle, FALSE);    
    m_bRunning = TRUE;
    for( int i=0 ;i <nThreads; i++ )
    {
        AtomicInterInc(m_nThreads);
	    if(CGSPThreadPool::RSUCCESS != m_csThreadPool.Task((void*)1) )
        {
            AtomicInterDec(m_nThreads);
            GS_ASSERT(0);
        }
    }
	MY_DEBUG(   "WinIOCP Init Threads: %d\n", m_nThreads );

	m_threadAR.Start(CWinIOCP::AReleaseThreadCallback, (void*)this);
    return TRUE;
}


void CWinIOCP::AReleaseEntry(void)
{
std::list<StruASyncReleaseNode> listTemp;
std::list<StruASyncReleaseNode>::iterator csIt;
  CMyIOCPPrivData *pPri;
long iTv;
	while( m_bRunning )
	{
		iTv = (long) time(NULL);
		listTemp.clear();
		m_mutexListAsyncR.Lock();
		for( csIt = m_listAsyncRelease.begin(); csIt != m_listAsyncRelease.end(); ++csIt )
		{
			csIt->iTicks ++;
			pPri = MY_PRI(csIt->pSocket->m_pEvtPri );
			if( (pPri->iFlags==0 && csIt->iTicks>10 ) ||
				(labs(iTv-csIt->iAddTime) > 120 && csIt->iTicks>3000) )
			{
				listTemp.push_back(*csIt);				
				csIt = m_listAsyncRelease.erase(csIt);
				if( csIt == m_listAsyncRelease.end() )
				{
					break;
				}
			}
		}		
		m_mutexListAsyncR.Unlock();
		for( csIt = listTemp.begin(); csIt != listTemp.end(); ++csIt )
		{
			pPri = MY_PRI(csIt->pSocket->m_pEvtPri );
			pPri->csMutex.Lock();				
			if( pPri->iFlags )
			{
				MY_LOG_WARN(g_pLog, "��ɶ˿�ǿ���ͷ���Դ!Seq:%d Flag:0x%x, %s\n",
					pPri->iSeq,
					(int)pPri->iFlags, csIt->pSocket->GetDescri().c_str() );
			}
					
			pPri->csMutex.Unlock();
			csIt->pSocket->UnrefObject();
		}
		MSLEEP(40);
	}
}

void CWinIOCP::Uninit(void)
{
    GS_ASSERT_RET(m_bRunning);
    m_bRunning = FALSE; 

    int iTrys =  m_nThreads;

    while( m_hIOCPHandle!=NULL &&  iTrys--  )
    {          
        ::PostQueuedCompletionStatus( m_hIOCPHandle, 0, (ULONG_PTR)EXIT_SOCKET_KEY, NULL );
    } 
    if( m_hIOCPHandle )
    {
        ::CloseHandle( m_hIOCPHandle);
        m_hIOCPHandle = NULL;
    }
	m_csThreadPool.Uninit();
	m_threadAR.Stop();
	m_threadAR.Join(20000);
    MY_DEBUG( "Uninit" );
}


void CWinIOCP::OnTaskPoolEvent(CObjThreadPool *pTkPool, void *pArgs )
{
    LPOVERLAPPED lpOverlapped;
    StruIOCPOverlapped *pMyOverlapped;
    DWORD iBytes = 0;
    ULONG iKey = 0;
    int iRet;
    CMyIOCPPrivData *pPri;
	BOOL bContinue;
	int iErrno;        
    CISocket *pSocket;
    CNetError csError( eERRNO_NET_EUNK, 0, _GSTX("GIOCP fail.") );
	char czDebug[512] = {0};
	StruAsyncSendEvent stSendEvt;
	int iMaskIndex = AtomicInterInc(m_iThreadConts);

	int iThreadID = COSThread::CurrentThreadID();

	MY_DEBUG_PRINTF( "****WinIOCP Threads Mask: %d\n",iMaskIndex);
	bzero(&stSendEvt, sizeof(stSendEvt));
    while( m_bRunning && g_bModuleRunning  )
    {
        iBytes = 0;
        iKey = 0XFFFFFFFF;
        lpOverlapped = NULL;
		m_vThreadMask[iMaskIndex] =  0;

        iRet = ::GetQueuedCompletionStatus(m_hIOCPHandle, 
            &iBytes, &iKey, &lpOverlapped, INFINITE); 
		m_vThreadMask[iMaskIndex] = 1;
		

		pSocket = ( CISocket *)iKey; 
		if( pSocket == EXIT_SOCKET_KEY )
		{
			//�˳�			
			continue;
		}

        pMyOverlapped = (StruIOCPOverlapped*)lpOverlapped;

        if( !pMyOverlapped )
        {
           /* GS_ASSERT(0);*/
            continue;
        }
        
        if( pMyOverlapped->iEvent==OVERLAPPED_EVT_EXIT || !pSocket )
        {
            MY_DEBUG("GIOCP Exit wakeup\n");
            continue;
        }

        pPri = MY_PRI(pSocket->m_pEvtPri);
		GS_ASSERT(pPri);


//  		MY_LOG_NOTICE(g_pLog, "###%d,Pri:0x%p, Evt:0x%x, iRefs:%d, Rcvs:%d, Seq:%d, iRet:%d\n", 
//  			 			iThreadID, pPri,pMyOverlapped->iEvent, 
//  						pPri->iBusyRefs, iBytes, pPri->iSeq, iRet );
        if( !iRet )
        {   
			//����
			
            iErrno = CISocket::GetSocketErrno();  //GetLastError() ;
			MY_LOG_DEBUG(g_pLog, "***%d,Pri:0x%p, Evt:0x%x, Errno:%d, Seq:%d\n", 
				iThreadID, pPri,pMyOverlapped->iEvent, 
				iErrno, pPri->iSeq );

            csError.m_eErrno = eERRNO_NET_EDISCNN;			
            switch(iErrno )
            {
			case ERROR_PORT_UNREACHABLE :
			{
				// UDP �˿�����
				if( pMyOverlapped->iEvent == OVERLAPPED_EVT_RCVFROM )
				{
					//
				/*	INT ii = 0;*/
					SOCKET sk = pSocket->GetOSSocket();
					bzero( &pPri->stReadOverlapped, sizeof(StruIOCPOverlapped) );
					pPri->stReadOverlapped.iEvent = OVERLAPPED_EVT_RCVFROM;
					pPri->stRcvFromAddr.Reset();
					
					DWORD iCnts = 0;
					DWORD iFlag = 0;					
					iRet = ::WSARecvFrom( sk,(LPWSABUF)&pPri->stRcvIO,1, &iCnts, &iFlag,
						&pPri->stRcvFromAddr.sn.sa, &pPri->stRcvFromAddr.len, 
						(LPOVERLAPPED) &pPri->stReadOverlapped, NULL );

					if( iRet==0 || (iErrno=WSAGetLastError())== WSA_IO_PENDING  ) 
					{  
						goto next_loop;
					}
					else
					{
						GS_ASSERT(0);
					}
				}
			}
			break;
			case WAIT_TIMEOUT :
				{
					//GS_ASSERT(0);
				}
			break;
            case WSAECONNRESET :
                //Զ������ǿ�ȹر���һ�����е����ӡ�
                //����ǶԷ����������������ֹ���Ǿ�ֻ�и��������ˡ�
                //��������������д�ģ�����ֻ��hard close���Ǿ��ɲ��ñ����ˡ����٣���Ҫ֪�������Ĵ����Ѿ������ˣ��Ͳ�Ҫ�ٷѾ��ļ���Ͷ�ݻ�ȴ��ˡ�
                break;

            case WSAENOBUFS  :
                // ����ϵͳ�������ռ䲻����ж�����������ִ���׽����ϵĲ�����
                //����������Ҳ鿴������־���������һ������
                //��Ϊ������������Ϣ�շ�����ȷ���ƣ�
                //�������������Ӧ����ʹ����ˣ�
                //�����ܴ���send/recv ʧ�ܰ���
                //�������������֮ǰ�İ汾�м���û�г��ֹ���
                //��connect��accept��Ϊ�������ռ䲻�㶼������⣬����Σ�ղ��ߣ�
                //�����send/recv���ӵ�²�����ѭ����ȥ���鷳�ʹ��ˣ�����˵��֮ǰ����֤�߼�����©��
                // WSASendʧ�ܵ�ԭ���ǣ�The Windows Sockets provider reports a buffer deadlock. 
                //�����ᵽ����buffer deadlock����Ȼ�����ڶ��߳�I/OͶ�ݲ�������ġ�
                csError.m_eErrno = eERRNO_SYS_ENMEM;                
                break;
            case WSA_OPERATION_ABORTED :
			case ERROR_NETNAME_DELETED :
				{
					//�����߳��˳���Ӧ�ó��������ѷ���I/O ������
					//�������һ��������peer socket��closesocket����WSACleanup�رպ�
					//�����Щsocket��pending overlapped I/O operation����ֹ��
					csError.m_eErrno = eERRNO_NET_ECLOSED;					
				}
            case WSAENOTSOCK :
                //��һ�����׽����ϳ�����һ��������
                // ʹ��closesocket�ر�socket֮����Ը�invalid socket���κβ��������øô���
                csError.m_eErrno = eERRNO_SYS_EINVALID;		
            default :
                break;
            }		
			pSocket->Shutdown();		
            csError.m_iSysErrno = iErrno;  
            pSocket->OnAIOEventError(csError);  
			
			goto event_finish;
            
        }

        switch(pMyOverlapped->iEvent )
        {
        case OVERLAPPED_EVT_READ :
            {
			   if( pPri->bUnregister )
			   {
				   pPri->bRcvStart = 0;
				   goto event_finish;
			   }			   
                if( iBytes == 0 )
                {
                    //�ر�
					pPri->bRcvStart = 0;
					pPri->bUnregister = 1;                   
                    csError.m_eErrno = eERRNO_NET_EDISCNN;
                    csError.m_iSysErrno = 0;
                    csError.m_strError = _GSTX("Disconnect of remote.");					
                    pSocket->Shutdown();				
                    pSocket->OnAIOEventError(csError);   	
//  					MY_LOG_NOTICE(g_pLog, "$$$%d,Pri:0x%p, Evt:0x%x, iRefs:%d, Seq:%d\n", 
//  							iThreadID, pPri,pMyOverlapped->iEvent, pPri->iBusyRefs,  pPri->iSeq );
                    goto event_finish;
                } 

               
               
				pPri->pRcvPktBuf->m_iDataSize = iBytes;
				if( pPri->bRcvStart && !pPri->bUnregister )
				{
					bContinue = pSocket->OnAIOEventRcv(pPri->pRcvPktBuf);               
				}
				else
				{
					bContinue = 0;
				}
                pPri->UninitRcvBuffer();
				if( bContinue  )
				{  			
					csError.m_eErrno = AsyncRcvRealy(pSocket, csError.m_iSysErrno, TRUE );
					if( csError.m_eErrno  )
					{					
						csError.m_eErrno = eERRNO_NET_ERCV;					
						csError.m_strError = _GSTX("WinIOCP ����ϵͳ�첽����ʧ��.");
						pSocket->OnAIOEventError( csError );  							
						goto event_finish;
					}					
				}  
				else
				{
					pPri->bRcvStart = 0;
					goto event_finish;
				}
            }
            break;        
        case OVERLAPPED_EVT_WRITE :
            {		
				if( pPri->bUnregister )
				{
					goto event_finish;
				}
				if( iBytes < 1 )
				{
					//�ر�
					pPri->bRcvStart = 0;
					pPri->bUnregister = 1;                   
					csError.m_eErrno = eERRNO_NET_EDISCNN;
					csError.m_iSysErrno = 0;
					csError.m_strError = _GSTX("Disconnect of remote.");					
					pSocket->Shutdown();				
					pSocket->OnAIOEventError(csError);  					
					goto event_finish;
				} 

				pPri->iSendBytes += iBytes;

				if( pPri->iWouldSendTotals > (INT) iBytes )
				{
					
					//û�з�������
					if( pPri->PrepareSendData(iBytes) )
					{					
						//����
						csError.m_eErrno = AsyncSendRealy(pSocket, csError.m_iSysErrno);
						if( csError.m_eErrno   )
						{
							csError.m_eErrno = eERRNO_NET_EWEVT;					
							csError.m_strError = _GSTX("WinIOCP ����ϵͳ�첽����ʧ��.");
							pSocket->OnAIOEventError( csError );  							
							pPri->ClearSendData();							
							goto event_finish;
						}					
					}
					else
					{
						//��������
						GS_ASSERT(0);
						csError.m_eErrno = eERRNO_NET_EWEVT;					
						csError.m_strError = _GSTX("WinIOCP ����ϵͳ�첽�������ݲ�����.");
						pSocket->OnAIOEventError( csError );  	
						pPri->ClearSendData();
						goto event_finish;

					}
				}
				else
				{
					if( pPri->PrepareSendData() )
					{					
						//����
						csError.m_eErrno = AsyncSendRealy(pSocket, csError.m_iSysErrno);
						if( csError.m_eErrno   )
						{

							csError.m_eErrno = eERRNO_NET_EWEVT;					
							csError.m_strError = _GSTX("WinIOCP ����ϵͳ�첽����ʧ��.");
							pSocket->OnAIOEventError( csError );  	
							pPri->ClearSendData();
							goto event_finish;
						}					
					}
					else
					{
						//���
						stSendEvt.iSends = pPri->iSendBytes;
						stSendEvt.pKey = pPri->pSendKey;	


						pPri->csMutex.Lock();		
						pPri->iFlags &= ~pMyOverlapped->iEvent;		
						pPri->ClearSendData();
						pPri->csMutex.Unlock();		

						pSocket->OnAIOEventSend(stSendEvt);

						if( pPri->bUnregister )
						{
							pPri->csExitCond.Signal();
						}
						goto next_loop;

					}
				}
            }
            break;
			
        case OVERLAPPED_EVT_RCVFROM :
            {
			
				if( pPri->bUnregister )
				{
					pPri->bRcvStart = 0;
					goto event_finish;
				}
				if( iBytes == 0 )
				{
					//�ر�
					pPri->bUnregister = 1;
					pPri->bRcvStart = 0;
					csError.m_eErrno = eERRNO_NET_EDISCNN;
					csError.m_iSysErrno = 0;
					csError.m_strError = _GSTX("Disconnect of remote.");				
					pSocket->Shutdown();					
					pSocket->OnAIOEventError(csError);    

					goto event_finish;
				}  

				GS_ASSERT(NULL!=pPri->pRcvPktBuf);

				pPri->pRcvPktBuf->m_iDataSize = iBytes;
				if( pPri->bRcvStart  && !pPri->bUnregister )
				{
					bContinue = pSocket->OnAIOEventRcvFrom(pPri->pRcvPktBuf, &pPri->stRcvFromAddr );   
				}
				else
				{
					bContinue = FALSE;
					
				}
				pPri->UninitRcvBuffer();

				if( bContinue   )
				{  

					csError.m_eErrno = AsyncRcvFromRealy(pSocket,csError.m_iSysErrno, TRUE); 
					if( csError.m_eErrno  )
					{					
						csError.m_eErrno = eERRNO_NET_ERCV;					
						csError.m_strError = _GSTX("WinIOCP ����ϵͳ�첽From����ʧ��.");
						pSocket->OnAIOEventError( csError );  							
						goto event_finish;
					}				
				} 
				else
				{
					pPri->bRcvStart = 0;
					goto event_finish;
				}

            }
            break;
        default :			
            GS_ASSERT(0);
            break;
        }

next_loop :
		//ѭ��		
		continue;
event_finish :
		//�¼����
		pPri->csMutex.Lock();		
		pPri->iFlags &= ~pMyOverlapped->iEvent;		
		if( pPri->bUnregister )
		{
			pPri->csExitCond.Signal();
		}
		pPri->csMutex.Unlock();
    }  //end while
	
    if( 0 == AtomicInterDec(m_nThreads) )
    {
        //ȫ���߳��˳�
        MY_DEBUG(_GSTX("WinIOCP �����߳��˳�.\n"));
    }
	MY_DEBUG_PRINTF(_GSTX("****WinIOCP thread exit %d..***\n"), m_nThreads);
}


BOOL CWinIOCP::Register(CISocket *pSocket, CNetError &csError)
{
	GS_ASSERT_RET_VAL( NULL ==pSocket->m_pEvtPri, FALSE );

    SOCKET sk = pSocket->GetOSSocket();
    EnumSocketType eSkType = pSocket->SocketType();
    if( eSkType == eSOCKET_TCP_SERVER  )
    {
        GS_ASSERT(0);
        csError.m_eErrno = eERRNO_SYS_EOPER;
        csError.m_strError = _GSTX("IOCP not support for tcp server");
        //�����˿ڲ�ʹ����ɶ˿�
        return FALSE;
    }

    CMyIOCPPrivData *pPri = GetPrivateData();
    if( !pPri )
    {
        csError.m_eErrno = eERRNO_SYS_ENMEM;
        csError.m_iSysErrno = 0;
        csError.m_strError = _GSTX("IOPC Create CMyIOCPPrivData fail.");
        return FALSE;
    }
	pSocket->RefObject();
	pSocket->m_pEvtPri = pPri;
	pSocket->m_fnSaftFreeEvtPri = (void(*)(void**)) SafeFreePrivateData;

    HANDLE hRet =  ::CreateIoCompletionPort((HANDLE)sk, m_hIOCPHandle, ( ULONG_PTR)pSocket, 0);
    if( hRet == NULL )  
    {      
		pSocket->UnrefObject();
        MY_LOG_ERROR(g_pLog,  "RegisterOS Create IOCP fail of sk:%d.\n", sk );  
        csError.m_eErrno = eERRNO_NET_EUNK;
        csError.m_iSysErrno =  COSSocket::GetSocketErrno();
        csError.m_strError = _GSTX("IOPC CreateIoCompletionPort fail.");
        return FALSE;
    }    
    return TRUE;

}

BOOL CWinIOCP::IsFreeThread(void)
{
	for( int i =0; i<m_iThreadConts; i++ )
	{
		if( m_vThreadMask[i] == 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

int GetBitSetNum( UINT8 iFlags )
{
	int iRefs = 0;
	for( int i = 0; i<8; i++ )
	{
		if( (iFlags>>i)&0x01 )
		{
			iRefs++;
		}
	}
	return iRefs==1;
}

void CWinIOCP::Unregister(CISocket *pSocket)
{
    GS_ASSERT_RET(pSocket->m_pEvtPri); 
    CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 	


	pPri->csMutex.Lock();
	if( pPri->bWaitUnregister )
	{
		GS_ASSERT(0);
		pPri->csMutex.Unlock();
		return;
	}
	pSocket->Shutdown();
	SOCKET sk = pSocket->GetOSSocket();
	if( sk != INVALID_SOCKET )
	{
		CancelIo((HANDLE) sk); 
	}
	pPri->bWaitUnregister = 1; 
	pPri->bUnregister = 1;
	BOOL bSelf = m_csThreadPool.IsSelfThread();
	INT iTrys = 900;




	while( pPri->iFlags != 0  )
	{

		if( iTrys-- < 1)
		{
// 			pPri->csMutex.Unlock();
// 			MY_LOG_FATAL(g_pLog, _GSTX("CWinIOCP ע����ʱ.bSelft: %d, ThreadID:%d,Pri:0x%p, iRefs: %d, %s\n"), 
// 				bSelf,
// 				COSThread::CurrentThreadID(), pPri, pPri->iBusyRefs, pSocket->GetDescri().c_str() );			
// 			pPri->csMutex.Lock();
			break;
		}


		if( bSelf && (GetBitSetNum(pPri->iFlags)==1 || !IsFreeThread() ) )
		{		
			break;
		}
		pPri->csExitCond.WaitTimeout(&pPri->csMutex, 100 );		
	}	
	pPri->csMutex.Unlock();

	

	m_mutexListAsyncR.Lock();
	pSocket->CloseSocket();
	StruASyncReleaseNode stNode;
	stNode.iAddTime = (long)time(NULL);
	stNode.pSocket = pSocket;	
	m_listAsyncRelease.push_back(stNode);
	m_mutexListAsyncR.Unlock();
}

EnumErrno CWinIOCP:: AsyncSend(CISocket *pSocket, void *pKey, 
										 std::vector<CProPacket *> vProPacket )
{
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);
	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �첽IOû��ע��.\n"),
			(int) pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}
	if( !pPri->vSendPackets.empty() || pPri->iFlags&OVERLAPPED_EVT_WRITE )
	{
		//�Ѿ��ڷ���
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �����Ѿ�����.\n"),
			(int)  pSocket->GetOSSocket() ,pPri); 
		GS_ASSERT(0);
		pPri->csMutex.Unlock();
		return eERRNO_SYS_EEXIST;
	}

	pPri->SetSendData(pKey, vProPacket);
	if( !pPri->PrepareSendData() )
	{
		//û������
		GS_ASSERT(0);
		pPri->ClearSendData();
		pPri->csMutex.Unlock();
		return eERRNO_SUCCESS;
	}	
	pPri->iFlags |= OVERLAPPED_EVT_WRITE;
	pPri->csMutex.Unlock();
	INT iSysErrno = 0;
	EnumErrno eRet = AsyncSendRealy(pSocket, iSysErrno);
	if( eRet )
	{
		pPri->csMutex.Lock();		
		pPri->iFlags &= ~OVERLAPPED_EVT_WRITE;
		pPri->ClearSendData();		
		pPri->csMutex.Unlock();
		return eRet;
	}
	return eERRNO_SUCCESS;	
}

EnumErrno CWinIOCP::AsyncSend(CISocket *pSocket, void *pKey,
							  std::vector<CProFrame *> vProFrame )
{

// 	if( pPacket->GetTotalSize() == 2068  )
// 	{
// 		CGSPBuffer *pPmg =  pPacket->MergePlayload();
// 		unsigned char md5res[MD5_LEN], md5resB[MD5_LEN];
// 		MD5Sum((BYTE*) pPmg->m_bBuffer,pPmg->m_iDataSize-MD5_LEN, md5res);
// 		memcpy( md5resB,pPmg->m_bBuffer+pPmg->m_iDataSize-MD5_LEN, MD5_LEN );
// 		if( memcmp(md5res, md5resB, MD5_LEN) )
// 		{
// 			GS_ASSERT(0);
// 		}
// 		int iii = 0;
// 	}
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);
	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �첽IOû��ע��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}
	if( !pPri->vSendPackets.empty()   || pPri->iFlags&OVERLAPPED_EVT_WRITE  )
	{
		//�Ѿ��ڷ���
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �����Ѿ�����.\n"),
			 (int)  pSocket->GetOSSocket() ,pPri); 
		GS_ASSERT(0);
		pPri->csMutex.Unlock();
		return eERRNO_SYS_EEXIST;
	}
	
	pPri->SetSendData(pKey, vProFrame);
	if( !pPri->PrepareSendData() )
	{
		//û������
		GS_ASSERT(0);
		pPri->ClearSendData();
		pPri->csMutex.Unlock();
		return eERRNO_NET_EWEVT;
	}	
	pPri->iFlags |= OVERLAPPED_EVT_WRITE;
	pPri->csMutex.Unlock();
	INT iSysErrno = 0;
	EnumErrno eRet = AsyncSendRealy(pSocket, iSysErrno);
	if( eRet )
	{
		pPri->csMutex.Lock();
		pPri->ClearSendData();			
		pPri->iFlags &= ~OVERLAPPED_EVT_WRITE;
		pPri->csMutex.Unlock();
		return eRet;
	}
	return eERRNO_SUCCESS;	
}

EnumErrno CWinIOCP::AsyncSendRealy(CISocket *pSocket, INT &iSysErrno)
{
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
    SOCKET sk;   
    DWORD iFlag = 0;
    int ret;
	
	sk = pSocket->GetOSSocket();
    bzero( &pPri->stWriteOverlapped, sizeof(StruIOCPOverlapped) );
    pPri->stWriteOverlapped.iEvent = OVERLAPPED_EVT_WRITE;

    ret = ::WSASend( sk,(LPWSABUF)&pPri->stVSendIO[0],(DWORD) pPri->iVSendIOCounts, NULL, iFlag,
        (LPOVERLAPPED) &pPri->stWriteOverlapped, NULL );
  
    if( SOCKET_ERROR==ret && (iSysErrno=WSAGetLastError())!= WSA_IO_PENDING  ) 
    {  
		MY_LOG_ERROR(g_pLog, 
				_GSTX("CWinIOCP Sk:%d,pri:0x%p �첽����ʧ��. ϵͳ����WSASend����ʧ��:%d .\n"),
            (int) sk ,pPri, iSysErrno );     

        return eERRNO_NET_EWEVT;
    }
    return eERRNO_SUCCESS;
}

EnumErrno CWinIOCP::AsyncRcv(CISocket *pSocket,
				   BOOL bStart  )
{
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);

	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �첽IOû��ע��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}



	if( bStart ) 
	{
		bStart = 1;

		if( pPri->iFlags&OVERLAPPED_EVT_READ )
		{
			MY_LOG_WARN(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽�����Ѿ�������.\n"),
				(int)  pSocket->GetOSSocket() ,pPri ); 
			pPri->csMutex.Unlock();
			return eERRNO_SUCCESS;
		}
	}


    if( pPri->bRcvStart  == bStart )
    {
		pPri->csMutex.Unlock();
		MY_LOG_WARN(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽�����¼��Ѿ�����.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
        return eERRNO_SUCCESS;
    }

	pPri->bRcvStart = bStart;
	pPri->csMutex.Unlock();

    if( !bStart )
    {		
        return eERRNO_SUCCESS;
    }
	INT iSysErrno;
	EnumErrno eRet = AsyncRcvRealy(pSocket, iSysErrno, FALSE);
    return eRet;
}

EnumErrno CWinIOCP::AsyncRcvRealy( CISocket *pSocket, INT &iSysErrno, BOOL bInner )
{
	
    CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);
	iSysErrno = 0;
	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{		
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �첽IO�Ѿ�ע��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}

	if( !pPri->InitRcvBuffer() )
	{
		pPri->bRcvStart = 0;
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽����ʧ��. �ڴ����ʧ��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_SYS_ENMEM;
	}
	if( !bInner)
	{		
		pPri->iFlags |= OVERLAPPED_EVT_READ;
	}
	pPri->csMutex.Unlock();



    bzero( &pPri->stReadOverlapped, sizeof(StruIOCPOverlapped) );
    pPri->stReadOverlapped.iEvent = OVERLAPPED_EVT_READ; 
    SOCKET sk = pSocket->GetOSSocket();
   // DWORD iCnts = 0;
    DWORD iFlag = 0;
    int ret; 	


    ret = ::WSARecv( sk,(LPWSABUF)&pPri->stRcvIO,1, NULL, &iFlag,
            (LPOVERLAPPED) &pPri->stReadOverlapped, NULL ); 
     if( SOCKET_ERROR==ret && (iSysErrno=WSAGetLastError()) != WSA_IO_PENDING  ) 
    {  
		
		MY_LOG_ERROR(g_pLog, 
			_GSTX("CWinIOCP Sk:%d,pri:0x%p �첽����ʧ��. ϵͳ����WSARecv����ʧ��:%d .\n"),
			(int) sk ,pPri, iSysErrno );  
		pPri->csMutex.Lock();
		pPri->UninitRcvBuffer();
		pPri->bRcvStart = 0;		
		if( !bInner)
		{		
			pPri->iFlags &= ~OVERLAPPED_EVT_READ;
		}
		pPri->csMutex.Unlock();       

        return eERRNO_NET_EREVT;
    } 	
    return eERRNO_SUCCESS;
}

EnumErrno CWinIOCP::AsyncAccept(CISocket *pSocket,	BOOL bStart)
{
    GS_ASSERT(0);
    return eERRNO_SYS_EOPER;

}

EnumErrno CWinIOCP::AsyncRcvFrom(CISocket *pSocket,BOOL bStart)
{
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);

	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽From����ʧ��. �첽IOû��ע��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}

	if( !IS_UDP_SOCKET(pSocket->SocketType( )) )
	{
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽From����ʧ��. ��UDP ����.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}

	if( bStart ) 
	{
		bStart = 1;
	}


	if( pPri->bRcvStart  == bStart )
	{
		pPri->csMutex.Unlock();
		MY_LOG_WARN(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽From�����¼��Ѿ�����.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		return eERRNO_SUCCESS;
	}

	pPri->bRcvStart = bStart;
	pPri->csMutex.Unlock();

	if( !bStart )
	{		
		return eERRNO_SUCCESS;
	}
	INT iSysErrno;
	EnumErrno eRet = AsyncRcvFromRealy(pSocket, iSysErrno, FALSE);
	return eRet;
}

EnumErrno CWinIOCP::AsyncRcvFromRealy(CISocket *pSocket,  INT &iSysErrno, BOOL bInner )
{
	iSysErrno = 0;
	CMyIOCPPrivData *pPri = MY_PRI(pSocket->m_pEvtPri); 
	GS_ASSERT(pPri);
	iSysErrno = 0;
	pPri->csMutex.Lock();
	if( pPri->bUnregister  )
	{		
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽From����ʧ��. �첽IO�Ѿ�ע��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_NET_EREG;
	}

	if( !pPri->InitRcvBuffer() )
	{
		pPri->bRcvStart = 0;
		MY_LOG_ERROR(g_pLog,  _GSTX("WinIOCP Sk:%d,pri:0x%p �첽From����ʧ��. �ڴ����ʧ��.\n"),
			(int)  pSocket->GetOSSocket() ,pPri ); 
		pPri->csMutex.Unlock();
		return eERRNO_SYS_ENMEM;
	}
	if( !bInner )
	{		
		pPri->iFlags |= OVERLAPPED_EVT_RCVFROM;
	}
	pPri->csMutex.Unlock();




	

    bzero( &pPri->stReadOverlapped, sizeof(StruIOCPOverlapped) );
    pPri->stReadOverlapped.iEvent = OVERLAPPED_EVT_RCVFROM;
    pPri->stRcvFromAddr.Reset();
    SOCKET sk = pSocket->GetOSSocket();  
    DWORD iFlag = 0;
    int ret;	

	
    ret = ::WSARecvFrom( sk,(LPWSABUF)&pPri->stRcvIO,1,NULL, &iFlag,
        &pPri->stRcvFromAddr.sn.sa, &pPri->stRcvFromAddr.len, 
        (LPOVERLAPPED) &pPri->stReadOverlapped, NULL );
    if( ret && (iSysErrno=WSAGetLastError())!= WSA_IO_PENDING  ) 
    {  
		
		MY_LOG_ERROR(g_pLog, 
			_GSTX("CWinIOCP Sk:%d,pri:0x%p �첽From����ʧ��. ϵͳ����WSARecv����ʧ��:%d .\n"),
			(int) sk ,pPri, iSysErrno );  
		pPri->csMutex.Lock();
		pPri->UninitRcvBuffer();	
		pPri->bRcvStart = 0;	
		if( !bInner )
		{			
			pPri->iFlags &= ~OVERLAPPED_EVT_RCVFROM;
		}
		pPri->csMutex.Unlock();

        return eERRNO_NET_EREVT;
    }	
    return eERRNO_SUCCESS;
}




namespace GSP
{

CIAsyncIO *CreateAsyncIO(INT iId )
{
	if( iId == 0 )
	{
	  MY_DEBUG( _GSTX("GSP ����� ʹ�� Win IOCP ģ��...\n" ) );
	}
		return new CWinIOCP(iId);
}

} //end namespace GSP

#endif //end GSP_ASYNCIO_USED_IOCP

