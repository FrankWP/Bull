/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : RTSPSERVER.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/4/18 10:11
Description: RTSP ��������
********************************************
*/

#ifndef _GS_H_RTSPSERVER_H_
#define _GS_H_RTSPSERVER_H_


#include "../IProServer.h"
#include "ISocket.h"
#include "ThreadPool.h"

namespace GSP
{
	namespace RTSP
	{
		class CRtspSrvSession;

		class CRtspServer :
			public CIProServer
		{
		public :
			typedef struct _StruRtspServerConfig
			{
				INT iKeepaliveTimeouts;
				INT m_iRtpUdpPortBegin; // rtp udp �˿ڷ�Χ�Ŀ�ʼ�˿ڣ� -1 ��Ч
				INT m_iRtpUdpPortEnd;  // rtp udp �˿ڷ�Χ�Ľ����˿ڣ� -1 ��Ч

				CGSPString m_strRtspTcpBindIP; //TCP �����󶨵�IP
				INT  m_iRtspTcpPort;  //TCP �����˿�
			}StruRtspServerConfig;

			StruRtspServerConfig m_stConfig;
		private :
			CTCPServerSocket *m_pListenSocket;	
			CGSPThreadPool m_csTaskDestorySession; //�첽�ͷ�Session �߳�
		public:
			CRtspServer(void);
			virtual ~CRtspServer(void);

			/*
			*********************************************************************
			*
			*@brief : �ӿ�ʵ��
			*
			*********************************************************************
			*/
			virtual EnumErrno Init( CServer *pServer );
			virtual void Unint(void);

			virtual EnumErrno Start(void);

			virtual void Stop(void);	


			/*
			*********************************************************************
			*
			*@brief : ���ӽӿ�
			*
			*********************************************************************
			*/
			void AsyncDestroySession( CRtspSrvSession *pSession );

		private :
			void *OnTcpListenSocketEvent(CISocket *pSocket, EnumSocketEvent iEvt,void *pParam, void *pParamExt);


			void OnAsyncDelSessionTaskPoolEvent( CObjThreadPool *pTkPool, void *pData );
		};
	} //end namespace RTSP
} //end namespace GSP



#endif //end _GS_H_RTSPSERVER_H_
