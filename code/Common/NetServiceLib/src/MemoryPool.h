#if !defined (MemoryPool_DEF_H)
#define MemoryPool_DEF_H

/***************************************************************************************************
	Copyright (C), 1997-2010, gosun Tech. Co., Ltd.
	FileName: 		MemoryPool.h
	Author:        	�´�
	Version :       v1.0   
	Date:			2010/08/25
	Description:    �ڴ��ģ����
					ʵ���������͵Ķ����ṹ����ڴ�ء�
					�ڴ�ص�ÿһ��ĽṹΪTYPE|Index
					Index��������������������һ�����õ��ڴ���λ��

****************************************************************************************************/

#include "NetServiceDataType.h"

namespace NetServiceLib
{

#define	MAX_TYPE_COUNT		100*10000				// ��������TYPE��Ŀ
#define	MIN_TYPE_COUNT		10						// ��С������TYPE��Ŀ


template<class TYPE>
class CMemoryPool
{
public:
	CMemoryPool();
	virtual ~CMemoryPool(void);

	// ��ʼ���ڴ��
	BOOL	Init(INT32 iTYPECount );

	// ����һ��TYPE���ڴ�
	TYPE*	Allocate();

	// �ͷ�һ��TYPE�ڴ�
	BOOL	DeAllocate(void* pType);

	// �����ڴ��
	BOOL	Destroy();

private:
	// �ж�ָ���Ƿ�Ϸ�
	BOOL	IsValidPtr( void* pType);

	// ׷���ڴ�  
	BOOL	ReAllocate( INT32 iTYPECount );

private:
	//	ģ������TYPE�Ĵ�С sizeof(TYPE)
	INT32				m_iTYPESize;

	// ģ������TYPE����Ŀ. ����Ϊ�ڴ�ش�С=m_iTYPECount*��m_iTYPESize + sizeof��INT32����
	INT32				m_iTYPECount;

	// ��ǰ�Ŀ��ÿ�����
	INT32				m_iAvailableBlockIndex;

	// ���õĿ���Ŀ
	INT32				m_iAvailableBlockCount;

	// �ڴ����ʼ��ַ
	BYTE*				m_pMemStartAddress;

	// ��
	CGSMutex			m_GSMutex;

};

// ʵ��

template<class TYPE>
CMemoryPool<TYPE>::CMemoryPool() : 
m_iTYPESize(sizeof(TYPE)),
m_iTYPECount(0),
m_iAvailableBlockIndex(0),
m_iAvailableBlockCount(0),
m_pMemStartAddress(NULL)
{

}
template<class TYPE>
CMemoryPool<TYPE>::~CMemoryPool(void)
{
	Destroy();
}

/********************************************************************************
Function:		Init
Description:	��ʼ���ڴ��
Input:  		
Output:      	   
Return:  		�ɹ�����TRUE ʧ�ܷ���FALSE       
Note:			���ݴ���Ĳ���,�����ڴ�		
Author:        	CHC
Date:				2010/08/25
********************************************************************************/
template<class TYPE>
BOOL CMemoryPool<TYPE>::Init(INT32 iTYPECount )
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if (iTYPECount<MIN_TYPE_COUNT || iTYPECount > MAX_TYPE_COUNT)
	{
		return FALSE;
	}
	if (m_iTYPESize < 0)
	{
		return FALSE;
	}

	// ����iTYPECount��m_iTYPESize+Index�������ڴ�
	m_pMemStartAddress = (BYTE*)malloc( iTYPECount * (m_iTYPESize + sizeof(INT32)) );

	if ( NULL == m_pMemStartAddress )
	{
		return FALSE;
	}

	// ��ʼ������

	m_iAvailableBlockIndex = 0;

	// ��ʼʱ m_iAvailableBlockCount = m_iTYPECount
	m_iAvailableBlockCount = iTYPECount;
	m_iTYPECount = iTYPECount;

	// ��ʼ��ÿ��TYPE������ 
	INT32		iIndex = 0;
	INT32*		pNext = NULL;
	BYTE*		pPos = m_pMemStartAddress;

	for ( ; iIndex != iTYPECount; pPos += ( m_iTYPESize + sizeof( INT32 )) )
	{
		// ��ʼ��ÿһ���ڴ�TYPE|Index�ṹ�е�Index
		pNext = (INT32*)( pPos + m_iTYPESize );
		*pNext = ++iIndex;
	}

	return TRUE;


}

/********************************************************************************
Function:		Allocate
Description:	����һ��TYPE���ڴ�
Input:  		
Output:      	   
Return:  		�ɹ�����TYPE��ָ�룬ʧ�ܷ���NULL       
Note:					
Author:        	CHC
Date:				2010/08/25
********************************************************************************/
template<class TYPE>
TYPE* CMemoryPool<TYPE>::Allocate()
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	// �ж����޿��õĿ�
	if ( m_iAvailableBlockCount <= 0 )
	{
		if ( !ReAllocate( m_iTYPECount*2 ))
		{
			return NULL;
		}
		
		
	}

	BYTE*	pResult = NULL;

	// ��ȡ���õ��ڴ���ָ��
	pResult = m_pMemStartAddress + m_iAvailableBlockIndex * ( m_iTYPESize + sizeof( INT32 ) );
	//memset(pResult, 0x0, m_iTYPESize);	// ������������ڴ������

	// ������һ�����õ��ڴ��
	m_iAvailableBlockIndex = *(INT32*)( pResult + m_iTYPESize );


	// ���ÿ������һ
	m_iAvailableBlockCount--;

	return static_cast<TYPE*>((void*)pResult);

}

/********************************************************************************
Function:		DeAllocate
Description:	�ͷ�һ��TYPE�ڴ�
Input:  		
Output:      	   
Return:  		�ɹ�����TRUE��ʧ�ܷ���FALSE
Note:			�൱�ڰ��ڴ�黹���ڴ��		
Author:        	CHC
Date:				2010/08/25
********************************************************************************/
template<class TYPE>
BOOL CMemoryPool<TYPE>::DeAllocate(void* pType)
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	// �ж��ڴ�Ϸ�
	if ( !IsValidPtr(pType))
	{
		return FALSE;
	}
	 

	BYTE*	pRelease = static_cast<BYTE*>(pType);

	// �ͷŵĿ��ڴ������ TYPE|Index ��ֵΪ��ǰ�Ŀ��ÿ�����
	*((INT32*)( pRelease + m_iTYPESize )) = m_iAvailableBlockIndex;

	// Ȼ����㵱ǰ�Ŀ��ÿ�����
	m_iAvailableBlockIndex = ( pRelease - m_pMemStartAddress )/( m_iTYPESize + sizeof( INT32 ) );

	// ���ÿ������һ
	m_iAvailableBlockCount++;

	return TRUE;


}

/********************************************************************************
Function:		Destroy
Description:	�����ڴ��
Input:  		
Output:      	   
Return:  		       
Note:			�ɹ�����TRUE��ʧ�ܷ���FALSE		
Author:        	CHC
Date:				2010/08/25
********************************************************************************/
template<class TYPE>
BOOL CMemoryPool<TYPE>::Destroy()
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	// ����ǰ�ж��Ƿ����е��ڴ�鶼�Ѿ��ͷ� 
	if ( m_iAvailableBlockCount < m_iTYPECount )
	{
		return FALSE;
	}

	if ( m_pMemStartAddress )
	{
		free(m_pMemStartAddress);
		m_pMemStartAddress = NULL;
	}

	return TRUE;


}

/********************************************************************************
  Function:		IsValidPtr
  Description:	 �ж�ָ���Ƿ�Ϸ�
  Input:  		
  Output:      	   
  Return:  		TRUE �Ϸ� FALSE ���Ϸ�       
  Note:					
  Author:        	CHC
  Date:				2010/09/13
********************************************************************************/
template<class TYPE>
BOOL CMemoryPool<TYPE>::IsValidPtr( void* pType)
{
	//�ڴ治��������档Ҳ����������ġ�
	if(pType< m_pMemStartAddress || pType > (m_pMemStartAddress + (m_iTYPESize + sizeof( INT32 )) * m_iTYPECount ) ) 
	{
		return FALSE;
	}

	//ָ��û��blockSize�߽��϶��룮�϶����������MemPool�����
	INT32 iResult = ((BYTE*)pType - m_pMemStartAddress) % (m_iTYPESize + sizeof( INT32));
	if(iResult != 0) 
	{
		return FALSE;
	}

	return TRUE;
}

// ׷���ڴ�  
/********************************************************************************
  Function:		ReAllocate
  Description:	�ڴ���ڴ治��ʱ,׷���ڴ�
  Input:  		iTYPECount ��Ҫ���ӵĿ���
  Output:      	   
  Return:  		TRUE ����ɹ� FALSE ʧ��       
  Note:					
  Author:        	CHC
  Date:				2010/09/13
********************************************************************************/
template<class TYPE>
BOOL CMemoryPool<TYPE>::ReAllocate( INT32 iTYPECount )
{
	CGSAutoMutex	GSAutoMutex(&m_GSMutex);

	if ( m_iAvailableBlockCount > 0)
	{
		return TRUE;
	}

	if ( m_iTYPECount + iTYPECount<MIN_TYPE_COUNT || m_iTYPECount + iTYPECount > MAX_TYPE_COUNT)
	{
		return FALSE;
	}
	if (m_iTYPESize < 0)
	{
		return FALSE;
	}

	// ����iTYPECount��m_iTYPESize+Index�������ڴ�
	BYTE* pAddress = (BYTE*)realloc( m_pMemStartAddress, iTYPECount * (m_iTYPESize + sizeof(INT32)) );
	

	if ( NULL == pAddress )
	{
		return FALSE;
	}

	// �µ�ַ,�µ�ַ���ܺ;ɵ�ַ��ͬ
	m_pMemStartAddress = pAddress;

	// ��ʼ��ÿ���������TYPE������ 
	INT32		iIndex = m_iTYPECount;
	INT32*		pNext = NULL;
	BYTE*		pPos = m_pMemStartAddress + (m_iTYPESize + sizeof( INT32 )) * m_iTYPECount ;

	for ( ; iIndex != iTYPECount; pPos += ( m_iTYPESize + sizeof( INT32 )) )
	{
		// ��ʼ��ÿһ���ڴ�TYPE|Index�ṹ�е�Index
		pNext = (INT32*)( pPos + m_iTYPESize );
		*pNext = ++iIndex;
	}

	// ���ڵĿ��ÿ�����
	m_iAvailableBlockIndex = m_iTYPECount + 1;
	// ���� m_iAvailableBlockCount �����Ե��� = m_iTYPECount
	m_iAvailableBlockCount = iTYPECount;
	m_iTYPECount += iTYPECount;

	return TRUE;
}

}

#endif

