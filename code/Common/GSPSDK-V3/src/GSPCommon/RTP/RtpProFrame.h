/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : RTPPROFRAME.H
Author :  ������
Version : 1.0.0.1
Date: 2013/4/3 9:17
Description: RTP Э��֡
********************************************
*/

#ifndef _GS_H_RTPPROFRAME_H_
#define _GS_H_RTPPROFRAME_H_

#include "../GSPMemory.h"
#include "RtpStru.h"
#include "RTPPacket.h"

namespace GSP
{


	namespace RTP
	{

		class CRtpProPacket : public CProPacket
		{
		public :		
			static const UINT iDEFAULT_PACKET_SIZE = 1400;
		private :
			//���İ�����
			BYTE *m_bPriBuffer;
			UINT m_iPriMaxBuf;
			CRtpHeader m_csRtpHeader;
			UINT m_iPriBufDataSize;
			CGSPBuffer *m_pRefBuf;
		public :
			static CRtpProPacket *Create( const BYTE *pPacket, int iPacketLen );
			static CRtpProPacket *Create( CGSPBuffer *pBuf );

		//	static CRtpProPacket *Create( const StruRTPHeader &stHeader );
			static CRtpProPacket *Create( const CRtpHeader &csHeader , UINT iMaxBuffer);
			//������ -1�� ��������ʹ�õ� ���ݴ�С, < iSize ��ʾ�������Ѿ�����
			INT MaxAppendPlayload(const BYTE *pPlayload, UINT iSize );

			INLINE CRtpHeader &GetHeader(void)
			{
				return m_csRtpHeader;
			}
		protected :
			CRtpProPacket(void);
			virtual ~CRtpProPacket(void);
		};


		class CRtpProFrame :
			public CProFrame
		{
		public:
			
			// bEnableMark �� TRUE ÿ֡���� �� iMark ��Ϊ 1
			static CRtpProFrame *Create(CFrameCache *pFrameData, 
									CRtpHeader &csHeader, BOOL bEnableMark = TRUE);
			static CRtpProFrame *Create(void)
			{
				return new CRtpProFrame();
			}


			CRtpHeader *GetFirstHeader(void);
		private :
			CRtpProFrame(void);
			virtual ~CRtpProFrame(void);
		};
	}
} //end namespace GSP

#endif //end _GS_H_RTPPROFRAME_H_
