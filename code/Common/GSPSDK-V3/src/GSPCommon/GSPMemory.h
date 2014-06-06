/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : GSPMEMORY.H
Author :  ������
Version : 1.0.0.1
Date: 2013/3/21 9:08
Description: �ڴ������ز���
********************************************
*/

#ifndef _GS_H_GSPMEMORY_H_
#define _GS_H_GSPMEMORY_H_
#include "GSPObject.h"

namespace GSP
{
//���Լ�¼��ջ
//#define DEBUG_MEMORY_STACK  
//ʹ��ϵͳ����
#define MALLOC_FROM_SYSTEM
//�����ڴ�
#define ENABLE_MEM_LOCK_OF_PHY

//�ֽڶ���
#define ALIGNE_SIZE 32


	class CSliceManager;
	class CMemoryPool
	{
	private :	
		static CMemoryPool *g_pIntance;
		CSliceManager **m_vManager;
		int m_iVectorSize;
		CGSMutex m_csMutex;
		UINT64 m_iAllocFromSys;
	public :
		static UINT64 g_iMemMaxSize;
	public:
		

		static void *Malloc(UINT iSize );
		static void Free(void *pBuffer);	
		static void InitModule(void);
		static void UninitModule(void);
		static BOOL IsNoMemory(void);
	private :
		CMemoryPool(void);
		~CMemoryPool(void);
		void Init(void);
		void Uninit(void);
		
		
	};

#if  defined(_WIN32) || defined(_WIN64) || defined(_WINCE)
	struct iovec {
		u_long  iov_len; 
		char FAR*   iov_base; 
	};
#endif
	typedef  StruIOV StruBaseBuf;




	typedef struct _StruFrameInfo
	{
		UINT16 iChnNo;
		EnumGSMediaType eMediaType;
		BOOL bSysHeader;
		BOOL bKey;
		UINT64 iTimestamp;
		_StruFrameInfo(void) : iChnNo(-1), 
			eMediaType(GS_MEDIA_TYPE_NONE),
			bSysHeader(FALSE),
			bKey(FALSE),
			iTimestamp(MAX_UINT64)

		{

		}
	}StruFrameInfo;

	//Э���
	typedef struct _StruPktInfo
	{
		BYTE *bPlayload;
		BYTE *bHeader;
		BYTE *bTailer;
		UINT iPlayloadSize;
		UINT iHeaderSize;
		UINT iTailerSize;
		_StruPktInfo() : bPlayload(NULL), bHeader(NULL), bTailer(NULL)
			, iPlayloadSize(0),iHeaderSize(0), iTailerSize(0)
		{

		}

		INLINE UINT GetTotalSize(void) const
		{
			return (iPlayloadSize+iHeaderSize+iTailerSize);
		}

	}StruPktInfo;

	class CGSPBuffer : public CRefObject
	{
	public :
		BYTE *m_bBuffer;
		UINT m_iBufferSize;
		UINT m_iDataSize;	
	protected :
		CGSPBuffer(const BYTE *pBuffer, UINT iBufferSize ) 
			: CRefObject()
			,m_bBuffer((BYTE*)pBuffer), m_iBufferSize(iBufferSize), m_iDataSize(0)
		{


		}
		virtual ~CGSPBuffer(void)
		{

		}

		void Zero(void)
		{
			m_iDataSize = 0;
			m_iBufferSize = 0;
			m_bBuffer = NULL;
		}
	public :

		INLINE UINT GetDataSize(void) const
		{
			return m_iDataSize;
		}

		INLINE const BYTE *GetData(void)
		{
			return m_bBuffer;
		}

		INLINE UINT GetBufFreeSize(void) const
		{
			return m_iBufferSize-m_iDataSize;
		}

		INLINE EnumErrno SetData(const BYTE *pData, UINT iSize )
		{
			if( iSize>m_iBufferSize )
			{
				return eERRNO_SYS_ENMEM;
			}
			
			if( iSize )
			{
				::memcpy((void*)m_bBuffer, pData, iSize);
			}
			m_iDataSize = iSize;
			return eERRNO_SUCCESS;
		}

		INLINE EnumErrno AppendData(const BYTE *pData, UINT iSize )
		{
			if( (iSize+m_iDataSize)>m_iBufferSize )
			{
				return eERRNO_SYS_ENMEM;
			}
			
			if( iSize )
			{
				::memcpy((void*)(m_bBuffer+m_iDataSize), pData, iSize);
			}
			m_iDataSize += iSize;
			return eERRNO_SUCCESS;
		}

		INLINE UINT TryAppend(const BYTE *pData, UINT iSize)
		{
			UINT iTemp = MIN(iSize, (m_iBufferSize-m_iDataSize) );
			if( iTemp )
			{
				::memcpy((void*)(m_bBuffer+m_iDataSize), pData, iTemp);
				m_iDataSize += iTemp;
			}
			return iTemp;

		}

		INLINE UINT TryAppend(const StruBaseBuf *vBuf, UINT iSize )
		{
			UINT iRet = 0;
			UINT iTemp; 
			for( UINT i = 0; i<iSize; i++ )
			{
				iTemp = TryAppend( (const BYTE*) vBuf[i].pBuffer, vBuf[i].iSize) ;
				if(  iTemp < vBuf[i].iSize  )
				{
					return iRet;
				}
				iRet += iTemp;
			}
			return iRet;
		}

		INLINE static UINT SumArrayBufTotalSize(const StruBaseBuf *vBuf, UINT iSize)
		{
			UINT iRet = 0;
			for( UINT i = 0; i<iSize; i++ )
			{
				iRet += vBuf[i].iSize;
			}
			return iRet;
		}

		INLINE void CutFront(UINT iCutSize)
		{
			if( iCutSize<m_iDataSize )
			{
				m_bBuffer += iCutSize;
				m_iDataSize -= iCutSize;
				m_iBufferSize -= iCutSize;
			}
			else
			{
				m_iDataSize = 0;
			}
		}
		INLINE void CutBack( UINT iCutSize)
		{
			if( iCutSize<m_iDataSize )
			{				
				m_iDataSize -= iCutSize;
				m_iBufferSize -= iCutSize;
			}
			else
			{
				m_iDataSize = 0;
			}
		}
	};


	class CPasteBuffer : public CGSPBuffer
	{
	private : 
		CRefObject *m_pRef;
	public :
		static CPasteBuffer *Create(CRefObject *pRef, const BYTE *pBuffer,
							UINT iBufferSize, UINT iDataSize )
		{
			return new CPasteBuffer(pRef, pBuffer, iBufferSize, iDataSize);
		}
	private :
		CPasteBuffer(CRefObject *pRef, const BYTE *pBuffer,UINT iBufferSize, UINT iDataSize )
			: CGSPBuffer(pBuffer, iBufferSize)
			, m_pRef(pRef)
		{
			m_iDataSize = iDataSize;
			if( pRef )
			{
				pRef->RefObject();
			}
		}
		virtual ~CPasteBuffer(void)
		{
			this->Zero();
			if( m_pRef )
			{
				m_pRef->UnrefObject();
				m_pRef = NULL;
			}
		}
	};


	template <int iSize>
	class CFixBufferTemplate : public CGSPBuffer
	{
	private :
		BYTE m_bPri[iSize]; //Packet ������ 
	public :
		/*
		*********************************************
		Function : Create
		DateTime : 2012/3/14 9:59
		Description :  ��������
		Input :  
		Output : 
		Return : ʧ�ܷ���NULL.
		Note :   
		*********************************************
		*/
		static CFixBufferTemplate<iSize> *Create(void)
		{
			CFixBufferTemplate<iSize> *pRet = new CFixBufferTemplate<iSize>();
			GS_ASSERT_RET_VAL(pRet, NULL);
			return pRet;
		}


		/*
		*********************************************
		Function : Clone
		DateTime : 2012/3/14 10:00
		Description :  ��������
		Input :  
		Output : 
		Return : ʧ�ܷ���NULL.
		Note :   
		*********************************************
		*/
		virtual CRefObject *Clone(void) const
		{
			CFixBufferTemplate<iSize> *pRet = pRet->Create();
			GS_ASSERT(pRet);
			if( pRet )
			{
				pRet->SetData(m_bBuffer, m_iDataSize);
			}
			return pRet;
		}

	protected :		
		CFixBufferTemplate(void)  : CGSPBuffer(&m_bPri[0], iSize)
		{

		}
		virtual ~CFixBufferTemplate(void)
		{	
			this->Zero();
		}
	};

	//�̶����ݰ��� ���ڲ��������� ���ݻ���
	
	typedef  CFixBufferTemplate<FIX_BUFFER_SIZE> CFixedBuffer;



	class CProPacket : public CRefObject
	{
	protected :
		StruPktInfo m_stPktParser;
	public :
		INLINE const StruPktInfo &GetParser(void) const
		{
			return m_stPktParser;
		}
	protected :
		CProPacket(void)
			: CRefObject()
			,m_stPktParser()
		{

		}
		virtual ~CProPacket(void)
		{

		}
	};





	//Э��֡, �ɶ��Э������
	class CProFrame : public CRefObject
	{
	protected :
		CProPacket **m_ppPkts;
		UINT m_iPktNumber;
		UINT m_iPktMax;
		UINT m_iTotalSize;
	public :
		INLINE UINT GetPackets( CProPacket ***pppPkts ) const
		{
			if( pppPkts )
			{
				*pppPkts = m_ppPkts;
			}
			return m_iPktNumber;
		}
		INLINE UINT GetTotalSize(void) const
		{
			return m_iTotalSize;
		}

		CGSPBuffer *MergePlayload(void) const;

		virtual CRefObject *Clone(void) const;

		EnumErrno AppendBack(  CProPacket *pPkt );

		EnumErrno AppendFront( CProPacket *pPkt );
	protected :
		CProFrame(void);
		virtual ~CProFrame(void);
		
		void FreeData(void);
	};

	/*
	*********************************************************************
	*
	*@brief : 
	*
	*********************************************************************
	*/
	class CProFrameCache : public CProFrame
	{
	public :
		StruFrameInfo m_stFrameInfo;
	public :
		static CProFrameCache *Create(void)
		{
			return new CProFrameCache();
		}

		EnumErrno AddBack(const void *pData, UINT iSize);

		EnumErrno AddBack(const StruBaseBuf *vBuf, UINT iSize );

		EnumErrno AddFront(const void *pData, UINT iSize);

		EnumErrno AddFront(const StruBaseBuf *vBuf, UINT iSize );
	protected :
		CProFrameCache(void) : CProFrame(), m_stFrameInfo()
		{
			

		}
		virtual ~CProFrameCache(void)
		{

		}

	};



	
	class CDynamicBuffer : public CGSPBuffer
	{
	private :
		BYTE *m_bPri;
	public :
		static CDynamicBuffer* Create( UINT iMaxBufSize );
		virtual CRefObject *Clone(void) const;
		
	protected :
		CDynamicBuffer( BYTE *pBuf, UINT iMaxBufSize  );
		virtual ~CDynamicBuffer(void);
	};


	class CFrameCache : public CRefObject
	{
	private :
		CGSPBuffer *m_pBuf;
	public :
		StruFrameInfo m_stFrameInfo;
	public :
		static CFrameCache *Create(const StruBaseBuf *vBuf, UINT iSize );

		static CFrameCache *Create(CRefObject *pRefObj,const void *pData,  UINT iSize );
	
		//ֻʹ�� ��Ч����
		static CFrameCache *Create(CProFrame *pFrame);
		static CFrameCache *Create(UINT iMaxSize);

		static CFrameCache *CreateMerge(const StruBaseBuf *vBuf, UINT iSize , CProFrame *pFrame);

		//��ǰ���������ݺ�ϲ�
		CFrameCache *MergeFront(const StruBaseBuf *vBuf, UINT iSize );
	
		virtual CRefObject *Clone(void) const;	
		

		INLINE void CutFront(UINT iCutSize)
		{
			GS_ASSERT(m_pBuf);
			m_pBuf->CutFront(iCutSize);
		}

		INLINE void CutBack( UINT iCutSize)
		{
			GS_ASSERT(m_pBuf);
			m_pBuf->CutBack(iCutSize);
		}

		INLINE CGSPBuffer &GetBuffer(void) const
		{
			return *m_pBuf;
		}
		
	private :
		CFrameCache(void);
		virtual ~CFrameCache(void);

		EnumErrno Set(const StruBaseBuf *vBuf, UINT iSize );

	};


/*
Server :
input->Parser(CFrameCache)->Event(CFrameCache)->Parser(CProFrame)->CProPacket->SocketSend

SocketRead->CFixBuffer->Parser->CProPacket->Parser(CProFrame)


input->CFrameCache->Source(Dispatch)-->RefSouce(TaskThread(1))->
					|
	<---------------|			


*/

} //end namespace GSP

#endif //end _GS_H_GSPMEMORY_H_
