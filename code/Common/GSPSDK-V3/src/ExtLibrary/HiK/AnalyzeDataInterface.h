/*****************************************************
Copyright 2008-2011 Hikvision Digital Technology Co., Ltd.

FileName: AnalyzeDataInterface.h

Description: ����������ӿ�

current version: 4.0.0.1
 	 
Modification History: 2010/4/13 ������
*****************************************************/

#ifndef _ANALYZEDATA_INTERFACE_H_
#define _ANALYZEDATA_INTERFACE_H_

#if defined(_WINDLL)
	#define ANALYZEDATA_API  extern "C" __declspec(dllexport) 
#else 
	#define ANALYZEDATA_API  extern "C" __declspec(dllimport) 
#endif

//���ݰ�����
#define FILE_HEAD			            0 
#define VIDEO_I_FRAME		            1
#define VIDEO_B_FRAME		            2
#define VIDEO_P_FRAME		            3
#define AUDIO_PACKET		            10
#define PRIVT_PACKET                    11

//E֡���
#define HIK_H264_E_FRAME        	    (1 << 6)
	    
//������		    
#define ERROR_NOOBJECT                  -1
#define ERROR_NO                        0
#define ERROR_OVERBUF                   1
#define ERROR_PARAMETER                 2
#define ERROR_CALL_ORDER                3
#define ERROR_ALLOC_MEMORY              4
#define ERROR_OPEN_FILE                 5
#define ERROR_MEMORY                    11
#define ERROR_SUPPORT                   12
#define ERROR_UNKNOWN                   99

typedef struct _PACKET_INFO
{
	int		nPacketType;    /*������ 
						    0  - �ļ�ͷ
	                        1  -	I֡	
							2  -	B֡	
							3  -	P֡
							10 - ��Ƶ��
							11 -˽�а�*/

	char*	pPacketBuffer;//��ǰ֡���ݵĻ�������ַ
	DWORD	dwPacketSize;//���Ĵ�С
	
	int		nYear;	//ʱ�꣺��
	int		nMonth; //ʱ�꣺��
	int		nDay;//ʱ�꣺��
	int		nHour;//ʱ�꣺ʱ
	int		nMinute;//ʱ�꣺��
	int		nSecond;//ʱ�꣺��

	DWORD   dwTimeStamp;//ʱ���������

} PACKET_INFO;

typedef struct _PACKET_INFO_EX
{
	int		nPacketType;    /*������ 
						    0  - �ļ�ͷ
	                        1  -	I֡	
							2  -	B֡	
							3  -	P֡
							10 - ��Ƶ��
							11 -˽�а�*/

	char*	pPacketBuffer;//��ǰ֡���ݵĻ�������ַ
	DWORD	dwPacketSize;//���Ĵ�С
	
	int		nYear;	//ʱ�꣺��
	int		nMonth; //ʱ�꣺��
	int		nDay;//ʱ�꣺��
	int		nHour;//ʱ�꣺ʱ
	int		nMinute;//ʱ�꣺��
	int		nSecond;//ʱ�꣺��

	DWORD   dwTimeStamp;//ʱ�����32λ������

	DWORD          dwFrameNum;//֡��
	DWORD          dwFrameRate;//֡��
	unsigned short uWidth;//���
	unsigned short uHeight;//�߶�
	DWORD		   dwTimeStampHigh;//ʱ�����32λ������
	DWORD          dwFlag;//֡���
	DWORD		   dwFilePos;//����֡���ļ��е�λ�ã���ģʽ����Ч
	DWORD          Reserved[4];//������

} PACKET_INFO_EX;
/***************************************************************************************
��  �ܣ� ��ȡ���þ����
�����������
�����������
����ֵ�� 0~499  ��Ч
        -1ʧ�ܣ��Ѿ��޿��þ��
****************************************************************************************/
ANALYZEDATA_API int   __stdcall AnalyzeDataGetSafeHandle();

/***************************************************************************************
��  �ܣ� ����ʽ��
���������LONG lHandle -�Ϸ������
          PBYTE pHeader -ͷ������
          
�����������
����ֵ�� �ɹ����       
****************************************************************************************/
ANALYZEDATA_API BOOL  __stdcall	AnalyzeDataOpenStreamEx(IN LONG lHandle, IN PBYTE pHeader);
/***************************************************************************************
��  �ܣ��ͷ�AnalyzeDataOpenStreamEx ()�򿪵ľ��
���������LONG lHandle- �Ϸ������
��������� ��
����ֵ��  ��
****************************************************************************************/

ANALYZEDATA_API void  __stdcall	AnalyzeDataClose(IN LONG lHandle);
/***************************************************************************************
��  �ܣ��������������
���������LONG lHandle -�Ϸ������
          PBYTE pBuffer-�������ݵ�ַ
		  DWORD dwSize-�������ݳ���
���������
����ֵ���ɹ����
****************************************************************************************/
	
ANALYZEDATA_API BOOL  __stdcall	AnalyzeDataInputData(IN LONG lHandle, IN PBYTE pBuffer, IN DWORD dwSize);
/***************************************************************************************
��  �ܣ���SDK�������л�ȡPACKET_INFO������ʹ��AnalyzeDataInputData�������ݳɹ����뷴������AnalyzeDataGetPacketֱ�����ط�0ֵ����ȷ����ȡ������������Ч���ݡ�
���������LONG lHandle -�Ϸ������
���������PACKET_INFO* pstPacket -�����ⲿ�������ݵĽṹ
����ֵ���ɹ�����0�����򷵻ش�����
****************************************************************************************/
	
ANALYZEDATA_API int	  __stdcall AnalyzeDataGetPacket(IN LONG lHandle, OUT PACKET_INFO* pstPacket);
/***************************************************************************************
��  �ܣ���SDK�������л�ȡPACKET_INFO������ʹ��AnalyzeDataInputData�������ݳɹ����뷴������AnalyzeDataGetPacketֱ�����ط�0ֵ����ȷ����ȡ������������Ч���ݡ�
���������LONG lHandle -�Ϸ������
���������PACKET_INFO* pstPacket -�����ⲿ�������ݵĽṹ
����ֵ���ɹ�����0�����򷵻ش�����
****************************************************************************************/

ANALYZEDATA_API int	  __stdcall AnalyzeDataGetPacketEx(IN LONG lHandle, OUT PACKET_INFO_EX* pstPacket);
/***************************************************************************************
��  �ܣ���ȡ���뻺���еĲ�������
���������lHandle-�Ϸ������
���������pData-�������ݵ�ַָ��
          pdwSize - �������ݴ�Сָ��

����ֵ���ɹ����
****************************************************************************************/

ANALYZEDATA_API BOOL  __stdcall AnalyzeDataGetTail(IN LONG lHandle, OUT BYTE** pData, OUT DWORD* pdwSize);
/***************************************************************************************
��  �ܣ���ȡ������
���������lHandle-�Ϸ������
�����������
����ֵ��������
****************************************************************************************/

ANALYZEDATA_API DWORD __stdcall AnalyzeDataGetLastError(IN LONG lHandle);

#endif