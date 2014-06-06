/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : GSPCONFIG.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/13 15:11
Description: GSP ��صĶ���
********************************************
*/

#ifndef _GS_H_GSPCONFIG_H_
#define _GS_H_GSPCONFIG_H_




// �� Windows ͷ���ų�����ʹ�õ�����
#define WIN32_LEAN_AND_MEAN             



#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#if !defined(_WIN32) && !defined(_LINUX)

#error  Only support WIN32 or LINUX

#else

#if defined(_WIN32) && defined(_LINUX)
#error  Only support WIN32 or LINUX in one times
#endif

#endif

#if defined(_DEBUG) || defined(DEBUG)

#define GSP_DEBUG_MSG

#else
#ifndef NDEBUG
#define NDEBUG
#endif
#endif

#include <GSType.h>
#include <GSCommon.h> 

namespace GSP
{


	//��������
	//IP �ַ�����
#define GSP_STRING_IP_LEN 64  
	//·���ַ�����
#define GSP_PATH_LEN 256  

#define CGSPString CGSString //ת��




// URI ��󳤶�
#define GSP_MAX_URI_LEN 1024  

//���֧�ֵ�ý��ͨ����
#define GSP_MAX_MEDIA_CHANNELS 8  

//��Чͨ����  
#define GSP_INVALID_SUBCHANNEL  (-1) 


 //UDP ������󳤶�
#define MAX_UDP_PACKET 1500 

//GSP ������󳤶�
#define GSP_PACKET_V1_SIZE 1500 

#define GSP_PACKET_SIZE_OFFSET_HEADER 64

#define GSP_PACKET_V2_SIZE (2*KBYTES)

#define GSP_PACKET_SIZE GSP_PACKET_V2_SIZE

#define  FIX_BUFFER_SIZE GSP_PACKET_V2_SIZE

#define MAX_GSP_COMMAND_LEN (2*KBYTES)

	//URI Key ���ֵ���󳤶�
#define GSP_MAX_URI_KEY_LEN 128  

	//���������
#define GSP_MAX_CONNECT  3000  




 //ʹ��GSP Э�� ����GSP ģ��
#define  ENABLE_GSP_MODULE 

//ʹ��RTSP Э�� ����RTSP ģ��
//#define  ENABLE_RTSP_MODULE

	//ʹ��SIP28181 Э�� ����SIP28181 ģ��
#define  ENABLE_SIP_MODULE

 //ʹ��Ƕ��ʽ
	//#define ON_EMBEDDED

//ʹ�õ����� ��������
#define ENABLE_MANUFACTURERS_STREAM_ANALYZER


#ifndef ON_EMBEDDED

#ifdef _WINCE
#define ON_EMBEDDED
#endif

#endif


	//���ݽṹ����
	typedef struct _StruIOV
	{		
		unsigned long iSize;
		void *pBuffer;
	}StruIOV;

	/*
	 *********************************************
	 Function : FuncPtrFree
	 DateTime : 2012/4/24 10:30
	 Description :  �ͷź�������
	 Input :  pData ���ͷŵĶ���
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	typedef void (*FuncPtrFree)(void *pData);

	/*
	 *********************************************
	 Function : FuncPtrCompares
	 DateTime : 2012/4/24 10:31
	 Description :  �ȽϺ�������
	 Input :  pData �Ƚ�ֵ
	 Input : pKey �Ƚ�ֵ
	 Output : 
	 Return :  ��� *pData == *pKey  ���� 0, *pData > *pKey ���� 1�� ��������-1
	 Note :   
	 *********************************************
	 */
	typedef int (*FuncPtrCompares)(const void *pData,const void *pKey);


	typedef enum 
	{
		ePRO_GSP = 0, // gsp Э��
		ePRO_RTSP = 1, // rtsp Э��
		ePRO_SIP = 2, // sip28181 Э��
		ePRO_UNK = 100, //δ֪
	}EnumProtocol;

 //server gsp tcp �����˿�
	#define GSP_SERVER_DEFAULT_TCP_LISTEN_PORT    8443

	//server gsp udp �����˿�
	#define GSP_SERVER_DEFAULT_UDP_LISTEN_PORT	  8444	 

// rtsp tcp �����˿�
    #define RTSP_SERVER_DEFAULT_TCP_LISTEN_PORT    554 

	// rtp udp �˿ڷ�Χ��Сֵ
	#define RTP_DEFAULT_UDP_PORT_RANGE_BEGIN    0  

	// rtp udp �˿ڷ�Χ���ֵ
	#define RTP_DEFAULT_UDP_PORT_RANGE_END    0 


	// SIP UDP �����˿�
	 #define SIP_SERVER_DEFAULT_UDP_LISTEN_PORT    5060



 //������
#define GSP_API   

//������
#define GSP_ASSERT_RET GS_ASSERT_RET 

	//������  
#define GSP_ASSERT    GS_ASSERT   

	//������
#define GSP_ASSERT_RET_VALUE    GS_ASSERT_RET_VAL   

	//������
#define GSP_ASSERT_EXIT    GS_ASSERT_EXIT 

#define GSP_STACK_VERSION2  2
}  //end namespace GSP


#include "GSPStru.h"
#include "GSMediaDefs.h"

 //����ʹ��
#define GSP_PRINTF(...) do{}while(0) 

#endif //end _GS_H_GSPCONFIG_H_
