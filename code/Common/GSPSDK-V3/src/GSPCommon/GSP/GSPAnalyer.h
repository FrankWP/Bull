

#ifndef GSS_GSPANALYER_DEF_H
#define GSS_GSPANALYER_DEF_H

/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : ANALYER.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/6/13 8:43
Description: ����Э�����
********************************************
*/

#include "../GSPObject.h"
#include "../List.h"
#include "../GSPProDebug.h"
//#include "../MediaInfo.h"
#include "../md5.h"
#include "../GSPMemory.h"
#include "../MediaInfo.h"


namespace GSP
{

#define GSP_MAX_CHK_CRC_LEN (GSP_PACKET_SIZE_OFFSET_HEADER-GSP_PACKET_HEADER_LEN)  //Ч�����ݵ������

	
	class CGspProPacket : public CProPacket
	{
	private :
		union
		{
			BYTE bBuffer[sizeof(StruGSPPacketHeader)]; // header buffer
			StruGSPPacketHeader stHeader;
		}m_unHeader;
		BYTE m_bTBuffer[GSP_MAX_CHK_CRC_LEN]; // tailer buffer
		BYTE *m_pPBuffer; // playload buffer
		UINT m_iPBufMaxSize;  // m_pPBuffer max size

	
		UINT m_iWholeSize;

		INT m_iAnalyseErrno;

		CRefObject *m_pPasteBuf;
	public :
		static CGspProPacket *Create( UINT iMaxWholeSize );

		static CGspProPacket *Create(CRefObject *pPasteBuf, 
							const BYTE **ppPlayload, UINT &iSize, INT iVersion);

		/*
		 *********************************************
		 Function : Analyse
		 Version : 1.0.0.1
		 Author : ������
		 DateTime : 2013/4/12 9:49
		 Description :  ��������
		 Input :  ppData �����������ݣ� ��������� ����ʹ�ú��ָ��
		 Input :  iSize ���ݳ��� �� ��������� ����ʹ�ú�Ĵ�С
		 Output : 
		 Return : ���� 				
		 Note :   
		 *********************************************
		 */
		EnumErrno Analyse( const BYTE **ppData, UINT &iSize );

		/*
		 *********************************************
		 Function : AnalyseResult
		 Version : 1.0.0.1
		 Author : ������
		 DateTime : 2013/4/12 9:53
		 Description :  �������
		 Input :  
		 Output : 
		 Return :  0 ������ɣ��Ѿ���������,
				-1 û������
				-2 û������ͷ
				-3 ���ݰ汾����
				-4 Ч�鲻��
				-5 ���ݳ��Ȳ���
				> 0 ����Ҫ�����ݳ���
		 Note :   
		 *********************************************
		 */
		INT AnalyseResult(void);


		/*
		 *********************************************
		 Function : AnalyseReset
		 Version : 1.0.0.1
		 Author : ������
		 DateTime : 2013/4/12 9:56
		 Description :  ����
		 Input :  
		 Output : 
		 Return : 
		 Note :   
		 *********************************************
		 */
		void Reset(void);

		StruGSPPacketHeader &GetHeader(void)
		{
			return m_unHeader.stHeader;
		}

		INLINE BOOL IsCommand(void) const
		{
			return m_unHeader.stHeader.iDataType == GSP_PACKET_TYPE_CMD;
		}

		static INLINE BOOL IsCommand(const StruGSPPacketHeader &stH) 
		{
			return (stH.iDataType == GSP_PACKET_TYPE_CMD);
		}



		//����� ���
		EnumErrno Packet(const StruGSPPacketHeader &stHeader );

		EnumErrno AppendPlayload( const BYTE *pData, UINT iSize );

		//�������������
		EnumErrno AppendPlayloadInMaxC( const BYTE **pData, UINT &iSize );

		UINT GetFreeBuffSize(void) const;

		virtual CRefObject *Clone(void) const
		{
			return NULL;
		}

	protected :

	private :
		CGspProPacket(void);
		~CGspProPacket(void);

		BOOL InitOfPaste(CRefObject *pPasteBuf, 
			const BYTE **ppPlayload, UINT &iSize, INT iVersion);
	
		BOOL CheckCRC(void);
		void MakeCRC(void);

	};


	class CGspProFrame : public CProFrame
	{
	public :
		INT m_iGspVersion;   //GSP �汾��
		INT m_iGspDataType;  //�������� ���ο� GSP ���������Ͷ���)
		UINT16 m_iGspSeq;            //���ݰ���������	
		UINT8  m_iGspSubChn;        // �Ự����ͨ����,��1��ʼ��������ͨ����ָ��Ϊ1, �Ƿ�ͨ��
		UINT8 m_iGspExtraVal;
	public :
		INLINE BOOL IsCommand(void) const
		{
			return m_iGspDataType == GSP_PACKET_TYPE_CMD;
		}

		static CGspProFrame *Create(void)
		{
			return new CGspProFrame();
		}

	

		//����Ƿ���Ч
		BOOL CheckValid(void);

		virtual CRefObject *Clone(void) const
		{
			return NULL;
		}
	protected :
		CGspProFrame(void);
		virtual ~CGspProFrame(void);
	};




	class CGspCommand : public CGSPObject
	{
	private :
		union
		{
			StruGSPCommand stCmd;
			BYTE bBuffer[MAX_GSP_COMMAND_LEN];
		}m_unData;
		UINT m_iPlayloadSize;
		UINT m_iWholeSize;
	public :
		INT m_iGspVersion;   //GSP �汾��
		INT m_iGspDataType;  //�������� ���ο� GSP ���������Ͷ���)
		UINT16 m_iGspSeq;            //���ݰ���������	
		UINT8  m_iGspSubChn;        // �Ự����ͨ����,��1��ʼ��������ͨ����ָ��Ϊ1, �Ƿ�ͨ��
		UINT8 m_iGspExtraVal;

	public :
// 		INLINE static CGspCommand *Create(void)
// 		{
// 			return new CGspCommand();
// 		}

		

		StruGSPCommand &GetCommand(void)
		{
			return m_unData.stCmd;
		}

		INLINE BYTE *CommandPlayload(void)
		{
			return m_unData.stCmd.cPlayload;
		}

		INLINE UINT CommandPlayloadSize(void) const
		{
			return m_iPlayloadSize;
		}

		INLINE BYTE *GetWholeData(void)
		{
			return m_unData.bBuffer;
		}

		INLINE  UINT GetWholeDataSize(void)
		{
			return m_iWholeSize;
		}

		EnumErrno Parser( const char *pData, UINT iSize );
		EnumErrno Parser(CGspProFrame *pProFrame);
		void Reset(void);

	

		EnumErrno AddCommandPlayload(const void *pData, UINT iSize );


		template< typename playloadtype >
		static playloadtype &CastSubCommand(CGspCommand &csCmd )
		{
			playloadtype *pRet = (playloadtype *)csCmd.CommandPlayload(); 
			return *pRet;
		}

		virtual ~CGspCommand(void);
		CGspCommand(void);
		
	
		
	};

	
	

	
	
	/*
	*********************************************************************
	*
	*@brief : CGspTcpDecoder GSP TCP Э�������
	*
	*********************************************************************
	*/


	
	class CGspTcpDecoder : public CGSPObject
	{
	
	private :
		CGspProPacket *m_pCurPacket;
		CGspProFrame *m_pCurFrame; //��ǰ������		
		CList m_csFinishList; //������ݶ���, ����Ϊ  CGspProFrame *
		UINT m_iMaxPacketSize;
		INT m_iVersion;
		EnumErrno m_eAssert; //�Ƿ��쳣
	public :
		CGspTcpDecoder(void);
		virtual ~CGspTcpDecoder(void);

		EnumErrno Decode( CGSPBuffer *pInBuffer );
		CGspProFrame *Pop( void );
		void Reset(void);	
	};

	/*
	*********************************************************************
	*
	*@brief : CGspTcpEncoder GSP TCP Э����
	*
	*********************************************************************
	*/
	class CGspTcpEncoder : public CGSPObject
	{
	public :
		CGspTcpEncoder(void);
		virtual ~CGspTcpEncoder(void);
		static CGspProFrame * Encode( CGSPBuffer *pInBuffer, 
									StruGSPPacketHeader &stHeader );
		static CGspProFrame * Encode( CFrameCache *pSliceFrame, 
									StruGSPPacketHeader &stHeader );
		static CGspProFrame * Encode( const StruBaseBuf *vBuf, INT iBufNums, 
									StruGSPPacketHeader &stHeader );
		

	};



	

	
		/*
		*********************************************************************
		*
		*@brief : StruMediaInfoTable �ĸ�ʽ������
		*
		*********************************************************************
		*/
		class CGspMediaFormat : public CGSPObject
		{
			
		public : 
			static BOOL StructToInfo( const StruMediaInfoTable *pTable, CMediaInfo &csResult );

			static BOOL InfoToStruct( const CMediaInfo &csInfo,  StruMediaInfoTable *pResBuf, INT iBufSize  );
		protected :
			static EnumGSMediaType FormatSection( const   StruMediaInfoTable *pMediaInfo,
				StruGSMediaDescri &stDescri, INT &iChn, UINT iIdxStart, UINT iIdxEnd);
			static void FormatVideo( const StruMediaInfoTable *pMediaInfo,StruVideoDescri &stVideo, 
				UINT iIdxStart, UINT iIdxEnd);
			static void FormatAudio( const StruMediaInfoTable *pMediaInfo,StruAudioDescri &stAudio, 
				UINT iIdxStart, UINT iIdxEnd);
			static void FormatPicture( const StruMediaInfoTable *pMediaInfo,StruPictureDescri &stPicture, 
				UINT iIdxStart, UINT iIdxEnd);
			static void FormatOSD( const StruMediaInfoTable *pMediaInfo,StruOSDDescri &stOSD, 
				UINT iIdxStart, UINT iIdxEnd);
			static void FormatBin( const StruMediaInfoTable *pMediaInfo,StruBinaryDescri &stBin, 
				UINT iIdxStart, UINT iIdxEnd);
			static void FormatSysHeader( const StruMediaInfoTable *pMediaInfo,StruSysHeaderDescri &stSys, 
				UINT iIdxStart, UINT iIdxEnd);
		};




		
		//�ѱ��ش����� ת��Ϊ GSP �����
		INT32 ErrnoLocal2Gsp(EnumErrno eGSSRet );

		//��GSP ����� ת��Ϊ  ���ش����� 
		EnumErrno ErrnoGsp2Local(INT32 iGspRet );



} //end namespace GSP

#endif


