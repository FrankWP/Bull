/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : RTPANALYER.H
Author :  ������
Version : 1.0.0.1
Date: 2013/5/7 15:27
Description: RTP ��������������
********************************************
*/

#ifndef _GS_H_RTPANALYER_H_
#define _GS_H_RTPANALYER_H_
#include "../GSPMemory.h"
#include "../List.h"
#include "RtpProFrame.h"

namespace GSP
{

namespace RTP
{

	typedef struct _StruRtpPktSn
	{
		UINT8 iPT;   /* payload type */	
		UINT16 iSeq;      /* sequence number */
		UINT32 iSSRC;     /* synchronization source */
	}StruRtpPktSn;


	

class CRtpPacketMerger : public CGSPObject
{
private :
	typedef struct _StruCore
	{	
		//	RTSP::StruRTPHeader stRtpHeader; //�������ݵ�Rtpͷ			
		CList lRcvPacket; //���ܵ������� �洢����Ϊ // CRtpProPacket *
		UINT64 iLastTs; //���ʧЧ֡��ʱ��
		UINT32 iLastSeq; //���һ��SEQ
		UINT32 iPt; //Playload ����
		UINT32 iSSRC;




		void Init( FuncPtrFree fnFreeRcvPacket)
		{

			//INIT_DEFAULT_RTH_HEADER(&stRtpHeader);			
			lRcvPacket.Clear();
			iLastTs = MAX_UINT64;				
			iPt = MAX_UINT32;
			iLastSeq = MAX_UINT32;
			iSSRC = MAX_UINT32;

			if( fnFreeRcvPacket )
			{
				lRcvPacket.SetFreeCallback(fnFreeRcvPacket);
			}

		}
		void Clear(void)
		{				
			lRcvPacket.Clear();
			iLastTs = MAX_UINT64;				
			iPt = MAX_UINT32;
			iLastSeq = MAX_UINT32;
		}
	}StruCore;

	CList m_listFinish; //��ɵ�֡ CRtpProFrame *
	StruCore m_vChannel[GSP_MAX_MEDIA_CHANNELS]; //RTP ͷʹ�õ�����

	
public :
	CRtpPacketMerger(void);
	virtual ~CRtpPacketMerger(void);

	//�����ݷ��� eERRNO_SUCCESS 
	// ���� eERRNO_SUCCESS ���� Get �õ�������������
	// ���� eERRNO_SYS_ELOSE ���� vLoser ���ض�ʧ����Ϣ
	EnumErrno Rcv( CGSPBuffer *pBuffer, std::vector<StruRtpPktSn> &vLoser ); //������ݰ�

	INLINE CRtpProFrame *Get(void)
	{
		void *p = NULL;
		m_listFinish.RemoveFront(&p);
		return (CRtpProFrame *)p;
	}
};


class CRtpDecoder : public CGSPObject
{
private :	
	CRtpPacketMerger m_csMeger;
public :
	CRtpDecoder(void);
	virtual ~CRtpDecoder(void);
	EnumErrno Decode( CGSPBuffer *pBuffer, std::vector<StruRtpPktSn> &vLoser ); //������ݰ�

	/*
	ý����Ϣ�е�
	iChn ΪRTP �� PT
	TimeStamp Ϊ  RTP �� timestamp
	eMediaType Ϊ RTP SSRC
	*/
	CFrameCache *Get(void);
};

} //end namespace RTP

} //end namespace GSP

#endif //end _GS_H_RTPANALYER_H_