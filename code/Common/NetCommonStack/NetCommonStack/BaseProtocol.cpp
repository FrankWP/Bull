#include "GSCommon.h"
#include "BaseProtocol.h"
#include "Log.h"

CBaseProtocol::CBaseProtocol()
{
	m_pProtocolData = NULL;
	m_iDataSize = 0;
}

CBaseProtocol::~CBaseProtocol()
{
	if(m_pProtocolData)
	{
		free(m_pProtocolData);
		m_pProtocolData = NULL;
	}
	m_iDataSize = 0;
}

/***************************************************************************
  Function:        	PackData
  DATE:				2010-5-12   12:01
  Description: 		�������,���û����ݰ�Э�����   
  Input:  			pSrcData: �û�����
					iSrcLen: �û����ݳ���
					pDstAddr:�����ĵ�ַ
					pLen:�����ĳ���
  Output:     		    
  Return:   		0������ɹ�������ֵ��ʾ���ʧ��      
  Note:						
****************************************************************************/
INT32 CBaseProtocol::PackData(void* pSrcData, INT32 iSrcLen, 
							void* pDstAddr, INT32 *pLen, 
							INT32 iSessionID, INT16 iTotalPack, INT16 iCurPacket)
{
	StruCommuPacket* pCommuPacket;

	if(iSrcLen < 0 || 
		NULL == pDstAddr ||
		NULL == pLen)
	{
        LOG2_ERROR(  "Args invalid.\n");
		return -1;
	}

	pCommuPacket = (StruCommuPacket*)pDstAddr;
	pCommuPacket->Header.uiHeader = COMMU_PACK_HEADER;
	pCommuPacket->Header.iLength = iSrcLen;
	pCommuPacket->Header.iSessionID = iSessionID;
	pCommuPacket->Header.iTotalPacket = iTotalPack;
	pCommuPacket->Header.iCurPacket = iCurPacket;
	if(iSrcLen > 0 && NULL != pSrcData)
	{
		memcpy(pCommuPacket->Payload, pSrcData, iSrcLen);
	}
	
	*pLen = sizeof(StruCommuHeader) + iSrcLen;
	return 0;
}

/***************************************************************************
  Function:        	DePackData
  DATE:				2010-5-11   11:38
  Description: 		�������   
  Input:  			pData:		������ݻ�����
					iDataLen:	����������
					pHeadPos:	������ͷλ��
					pPacketLen: ������
  Output:     		    
  Return:   		0:����ɹ���pHeadPos �� pPacketLen ����ȷ��ֵ
					1:pHeadPos����ȷ��ֵ����pPacketLenδ����ֵ
					2:��������û�а�ͷ
					3:pHeadPos �� pPacketLen ����ȷ��ֵ������û��ȫ
					4:�������룬��У�������
					-1:��ͷû����
					-2:��������
  Note:						
****************************************************************************/
INT32 CBaseProtocol::DePackData(void* pData, INT32 iDataLen, void **pHeadPos, INT32 *pPacketLen, void** pCopyData)
{
	INT32 i;
	char* pHeader;
	INT32 iTotalLen;
	INT32 iContentLen;
	INT32 iHeadPos = 0;
	StruCommuPacket *pCommuPacket;

	if(iDataLen < sizeof(DWORD))
	{
		//�����ͷ��û���ֱ꣬�ӷ���        
		return -1;
	}
	if(NULL == pData || NULL == pHeadPos || NULL == pPacketLen)
	{
         LOG2_ERROR(  "Args invalid.\n");
		return -2;
	}

	*pHeadPos = NULL;
	*pPacketLen = 0;
	pHeader = (char*)pData;
	//Ѱ�Ұ�ͷ
	for(i = 0; i < iDataLen - 4; i++)
	{
		if(0xAB == (pHeader[i] & 0x0ff) &&
			0x00 == (pHeader[i + 1] & 0x0ff) &&
			0x00 == (pHeader[i + 2] & 0x0ff) &&
			0xAB == (pHeader[i + 3] & 0x0ff))
		{
			//�ҵ���ͷ
			*pHeadPos = pHeader + i;
			iHeadPos = i;
			break;
		}
	}
	//���������ް�ͷ
	if(NULL == *pHeadPos)
	{
        LOG2_WARN( "Not found packet header, size:%d.\n",
            iDataLen);
		return 2;
	}
	//�����ֶλ�û����
	if(iDataLen - iHeadPos < sizeof(StruCommuHeader))
	{
		return 1;
	}
	if(NULL == m_pProtocolData)
	{
		m_pProtocolData = (char*)malloc(iDataLen);
		if(NULL == m_pProtocolData)
		{
            LOG2_FATAL(  "Malloc buffer, size:%d fail.\n",
                        iDataLen); 
			return 2;
		}
		m_iDataSize = iDataLen;
	}
	else
	{
		if(m_iDataSize < iDataLen)
		{
			free(m_pProtocolData);
			m_pProtocolData = (char*)malloc(iDataLen);
			if(NULL == m_pProtocolData)
			{
                LOG2_FATAL( "Malloc buffer, size:%d fail.\n",
                    iDataLen);  
				return 2;
			}
			m_iDataSize = iDataLen;
		}
	}
	memcpy(m_pProtocolData, *pHeadPos, iDataLen - i);
	*pCopyData = m_pProtocolData;
	pCommuPacket = (StruCommuPacket *)m_pProtocolData;
	//�����ֶ�
	iContentLen = pCommuPacket->Header.iLength;
	if(iContentLen < 0)
	{
        LOG2_FATAL("Packet invalid of size: %d < 0 .\n",
            iContentLen); 

		return 2;
	}
	
	//���ܳ�Ӧ���� ͷ + �����ֶ� + ���ݳ���
	iTotalLen = iContentLen + sizeof(StruCommuHeader);
	*pPacketLen = iTotalLen;
	if(iDataLen - iHeadPos < iTotalLen)
	{        
		return 3;
	}
	return 0;
}