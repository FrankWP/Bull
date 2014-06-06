

/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : GSRCDFILEDEFS.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/7/22 11:55
Description: ���ļ�����¼���ļ��ṹ����ظ�ʽ�� ע��ο� <GSMediaDefs.h>
********************************************
*/

#ifndef GSP_GMFDEFS_DEF_H
#define GSP_GMFDEFS_DEF_H

#include "GSMediaDefs.h"

#ifdef _WIN32
#pragma pack( push,1 )
#endif

/*
****************************************
brief : �洢�ļ���ʽ
****************************************

==========�ļ����ṹ����======================
�ļ�������ṹ  ��������(����(21Bytes).������(�8Bytes)

���ֽṹ:
¼��ʱ��(YYYYMMHHMMSS)-ID(XXXXXXXX)

��:  201007230101-F1234567.bmp


========�ļ��ṹ����  ֻ������Ƶ¼��ʹ�ã� ����ͼƬֱ�Ӵ�Ϊ��׼��ʽ=====================

�ļ�ͷ | ֡�ṹ����(StruGSFrameHeader)

�ļ�ͷ�ṹ:

ħ����(4Bytes)| ͷ����(2Bytes) | �ļ�ͷ�汾(1Byte) | ý�����Ը���(1Byte) | ¼��ʱ�䣨YYYYMMDDHHMMSS) | ý��ͨ��0����(StruGSMediaDescri)|...|ý��ͨ��n����(StruGSMediaDescri)
*/

#define GS_RCV_FILE_MAGIC  (('g'<<24)|('s'<<14)|(0xaf<<16)|(0xe3))
//#define GS_RCV_VERSION     0x00
#define GS_RCV_VERSION     0x01

// ֡������ʼ��־
#define GS_INDEX_BEGIN_MARK (('g'<<24)|('b'<<16)|('i'<<8)|('g'))
// ֡����������־
#define GS_INDEX_END_MARK (('d'<<24)|('e'<<16)|('i'<<8)|('g'))

typedef struct _StruIFrameIndex
{
	UINT32 iTimeStamp;      // ʱ��� (��1970��1��1��0ʱ0��0�뵽��ʱ������)
	UINT32 iOffset;         // ��Ӧ�ļ���ƫ����
} GS_MEDIA_ATTRIBUTE_PACKED StruIFrameIndex;

//���������ڵ���Ϣ
typedef struct _StruIFrameIdxEndNode
{
	UINT32 iIdxStartOffset; //������Ϣ����ļ���ͷ��ƫ��, ָ��  GS_INDEX_BEGIN_MARK ��ͷ
	UINT32 iEndMarsk; //�������־ ���� GS_INDEX_END_MARK

}GS_MEDIA_ATTRIBUTE_PACKED StruIFrameIdxEndNode;

/* 
�ļ�ͷ | ֡�ṹ���� | ... | ֡�ṹ���� | ����֡ͷ | ֡������ʼ��־��GS_INDEX_BEGIN_MARK��| ������Ϣ��StruIFrameIndex��|...| ������Ϣ��StruIFrameIndex��| ������ʼ��־λ�� | ֡����������־��GS_INDEX_END_MARK��
*/

typedef struct _StruGMFHeader
{
    UINT32 iMagic;       //ħ���� Ϊ GS_RCV_FILE_MAGIC
    UINT16 iHeaderSize;   //ͷ����
    UINT8  iVersion;      //ͷ�汾
    UINT8  iChannel;       //ý��ͨ����  aMediaDescri �ĸ���
    unsigned char aTv[16];   //¼��ʱ��
    StruGSMediaDescri aMediaDescri[1]; //ý��ͨ������
}GS_MEDIA_ATTRIBUTE_PACKED StruGMFHeader;

#ifdef _WIN32
#pragma pack( pop )
#endif


#endif
