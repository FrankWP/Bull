/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : GSMEDIADEFS.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/7/22 9:15
Description: �����ý���õ���һЩ����
********************************************
*/
#ifndef GSP_GSMEDIADEFS_DEF_H
#define GSP_GSMEDIADEFS_DEF_H

#include <GSCommon.h>

//�ṹΪ���ֽڶ���
#ifdef _WIN32
#define GS_MEDIA_ATTRIBUTE_PACKED 
#pragma pack( push,1 )
#else
#define GS_MEDIA_ATTRIBUTE_PACKED    __attribute__ ((packed)) 
#endif


/*
****************************************
brief : �����������
****************************************
*/
typedef enum
{
    GS_CODEID_NONE = 0, //�ޱ�������
    GS_CODEID_BINARY,   //�����Ʊ���

	GS_CODEID_GS_PS,	// GOSUN PS ��̬����


    //��׼��Ƶ���� 
    GS_CODEID_ST_MP4        = 0x00000010,       //��׼�� MP4 ����
    GS_CODEID_ST_H264,                          //��׼�� H264 ����
	GS_CODEID_ST_SVAC,                           //��׼�� SVAC ����
 
    //��׼��Ƶ����
    GS_CODEID_ST_MP3        = 0x00000300,       //��׼MP4
	GS_CODEID_AUDIO_ST_PCM,	
	GS_CODEID_AUDIO_ST_G711A,
	GS_CODEID_AUDIO_ST_G711U,
	GS_CODEID_AUDIO_ST_G721,
	GS_CODEID_AUDIO_ST_G722,
	GS_CODEID_AUDIO_ST_G723,
	GS_CODEID_AUDIO_ST_G729,
	GS_CODEID_AUDIO_ST_H263,
	GS_CODEID_AUDIO_ST_SVAC,

	GS_CODEID_PS = 0x00000400,


    //��׼ͼ�����
    GS_CODEID_ST_JPEG       = 0x00000600,         //��׼JPG
    GS_CODEID_ST_BMP,                             //��׼JPG
    GS_CODEID_ST_YUV420P,                         //��׼YUV420P


    //�����̽��е��������



    //=====��������
    GS_CODEID_GS_V2160I     = 0x00001000,       // GOSUN 2160I ��Ƶ����
    GS_CODEID_GS_A2160I,                        // GOSUN 2160I ��Ƶ����
    GS_CODEID_GS_V462C,                         //GOSUN BASS462C ��Ƶ����
    GS_CODEID_GS_A462C,                         //GOSUN BASS462C ��Ƶ����
    GS_CODEID_GS_V461C,                         //GOSUN BASS461C ��Ƶ����
    GS_CODEID_GS_A461C,                         //GOSUN BASS461C ��Ƶ����
	GS_CODEID_GS_V461A,                         //GOSUN BASS461A ��Ƶ����
	GS_CODEID_GS_V2160IV,						// GOSUN 2160I_V ��Ƶ����
	GS_CODEID_GS_A2160IV,                       // GOSUN 2160I_V ��Ƶ����
	GS_CODEID_GS_VIPC,							// GOSUN IPC	 ��Ƶ����
	GS_CODEID_GS_AIPC,							// GOSUN IPC	 ��Ƶ����
	

    //===== ��ʨ 
    GS_CODEID_HI_VDEFAULT   = 0x00001200,        //��ʨ����Ƶͨ�ñ���
    GS_CODEID_HI_ADEFAULT,                      //��ʨ����Ƶͨ�ñ���
    GS_CODEID_HI_VH264,                         //��ʨ H264
    GS_CODEID_HI_VMP4,                          //��ʨ H264
    GS_CODEID_HI_COMPLEX,                          //��ʨ����ͨ�ñ���


    //===== ����
    GS_CODEID_HK_VDEFAULT    = 0x00001300,      //��������Ƶͨ�ñ���
    GS_CODEID_HK_ADEFAULT,                      //��������Ƶͨ�ñ���
    GS_CODEID_HK_VMP4,                          //���� MP4
    GS_CODEID_HK_VH264,                         //���� H264
    GS_CODEID_HK_COMPLEX,                       //��������ͨ�ñ���




    //===== ��
    GS_CODEID_DH_VDEFAULT    = 0x00001400,      //�󻪵���Ƶͨ�ñ���
    GS_CODEID_DH_ADEFAULT ,                     //�󻪵���Ƶͨ�ñ���
    GS_CODEID_DH_VMP4,                          //�� MP4
    GS_CODEID_DH_VH264,                         //�� H264
    GS_CODEID_DH_COMPLEX,                       //�󻪸���ͨ�ñ���

    //===== ����
    GS_CODEID_HY_VDEFAULT   = 0x00001500,       //�������Ƶͨ�ñ���
    GS_CODEID_HY_ADEFAULT,                      //�������Ƶͨ�ñ���
    GS_CODEID_HY_VMP4,                          //���� MP4
    GS_CODEID_HY_VH264,                         //���� H264
    GS_CODEID_HY_COMPLEX,                       //���渴��ͨ�ñ���


	//=====�ֻ�
	GS_CODEID_WM_XVID		= 0x00001600,		//�ֻ��ͻ��˽���1,̷־��Ӳ���

	// ===== �ƺ�
	GS_COIDEID_HH_VDEFULT   = 0x00001700,       //�ƺӵ���Ƶͨ�ñ���

	// ===== �б�
	GS_CODEID_ZBEN_VDEFULT  = 0x00001800,       //�б�����Ƶ����
	GS_CODEID_ZBEN_ADEFULT,                     //�б�����Ƶ����

	// ===== �߿���
	GS_CODEID_CALSYS_VDEFULT = 0x00001900,         //�߿��ӵ���Ƶͨ�ñ��� 
	GS_CODEID_CALSYS_ADEFULT,                      //�߿��ӵ���Ƶͨ�ñ���

    // ==== ��������
	GS_COIDEID_ALRS_VDEFULT   = 0x00002000,       //�������ӵ���Ƶͨ�ñ���

	// ====  Ѷ��
    GS_CODEID_XM_VDEFULT   = 0x00002100,          //Ѷ����Ƶͨ�ñ���

   // ====  �߿���GS76xxϵ��
    GS_CODEID_CALSYSGS76XX_VDEFULT = 0x00002200,         //�߿���GS76XX����Ƶͨ�ñ��� 

    GS_CODEID_END,   //���ڽ�β 

}EnumGSCodeID;








/*
****************************************
brief : ����ý������
****************************************
*/
typedef enum 
{
    GS_MEDIA_TYPE_NONE = 0,           //δ֪ý������
    GS_MEDIA_TYPE_VIDEO = 0x01,   //��Ƶ��, ��������  StruVideoDescri
    GS_MEDIA_TYPE_AUDIO,            //��Ƶ��, ��������  StruAudioDescri
    GS_MEDIA_TYPE_PICTURE,          //ͼƬ����,�������� StruPictureDescri
    GS_MEDIA_TYPE_OSD,              //OSD ����,�������� StruOSDDescri
    GS_MEDIA_TYPE_BINARY,          //��������, ��������  StruBinaryDescri   
	GS_MEDIA_TYPE_PROGRESS,        //���ȣ��������� StruPlayStatus
	GS_MEDIA_TYPE_RECORD_INFO,     //¼����Ϣ���������� GSStruRecordInfo
	GS_MEDIA_TYPE_IFRAME_INDEX,   //I֡������ ����Ϊ���������� �ṹ�ο� GMFDefs.h ����
	GS_MEDIA_TYPE_FILE_TO_END,    //�ļ�����

	GS_MEDIA_TYPE_USER_END,		   //������Ϣ�� ����ͳ��
    GS_MEDIA_TYPE_SYSHEADER = 0x40,         //��Ϣͷ
}EnumGSMediaType;


/*
****************************************
brief :  ¼����Ϣ����
****************************************
*/
typedef struct _GSStruRecordInfo
{
	UINT32 iType;              // 1��ʼ 0����
	unsigned char szTime[16];  // ʱ��: 20111013194000 
}GS_MEDIA_ATTRIBUTE_PACKED GSStruRecordInfo;
#define TYPE_RECORD_END   0
#define TYPE_RECORD_START 1

/*
****************************************
brief :  ��Ƶ����Ϣ����
****************************************
*/

typedef struct _StruVideoDescri
{
    UINT32 eCodeID;  //��������   EnumGSCodeID
    UINT16 iWidth;         //ͼ���         0 ��ʾ��Ч
    UINT16 iHeight;       //ͼ���          0 ��ʾ��Ч
    UINT8 iFrameRate;    //֡�� ��������    0 ��ʾ��Ч
    UINT8 iFrameRate2;    //֡�� С������  
    UINT16 iReserver;      //������
}GS_MEDIA_ATTRIBUTE_PACKED StruVideoDescri;

/*
****************************************
brief :  ��Ƶ����Ϣ����
****************************************
*/
typedef struct _StruAudioDescri
{
    UINT32 eCodeID;    //��������   EnumGSCodeID
    UINT32 iSample;    //����Ƶ�� Hz         0 ��ʾ��Ч
    UINT16 iBits;      //����λ��  0 ��ʾ��Ч
    UINT16 iChannels;  //ͨ����     0 ��ʾ��Ч
}GS_MEDIA_ATTRIBUTE_PACKED StruAudioDescri;

/*
****************************************
brief : ͼƬ��Ϣ����
****************************************
*/
typedef struct _StruPictureDescri
{
    UINT32 eCodeID;    //��������   EnumGSCodeID
}GS_MEDIA_ATTRIBUTE_PACKED StruPictureDescri;


/*
****************************************
brief :  OSD��Ϣ����
****************************************
*/
//OSD ����������  ���ַ���
#define GS_OSD_TYPE_STRING  0x01   
//OSD ����������  ��RGB����
#define GS_OSD_TYPE_RBG     0x02   

typedef struct _StruOSDDescri
{
    UINT16 iPosX; //��ʼ��X����, �����ͼƬ���Ͻ�
    UINT16 iPosY; //��ʼ��Y����, �����ͼƬ���Ͻ�
    UINT8 iDataType; //��������     �ο� GS_OSD_TYPE_xxx
    UINT8 iTransparency; //͸����  0~100    0 ��ʾ��ȫ͸���� 100 ��ʾ��ȫ��͸��
}GS_MEDIA_ATTRIBUTE_PACKED StruOSDDescri;

/*
****************************************
brief : ��������������
****************************************
*/
typedef struct _StruBinaryDescri
{
    UINT64 iSize;  //�������������ĳ��ȣ� 0 ��ʾ���޳�
}GS_MEDIA_ATTRIBUTE_PACKED StruBinaryDescri;

/*
****************************************
brief : ��Ϣͷ��������
****************************************
*/
typedef struct _StruSysHeader
{
    UINT32 iSize; //��ʾ��Ϣͷ�ĳ��ȣ� 0 ��ʾδ֪ 
}GS_MEDIA_ATTRIBUTE_PACKED StruSysHeaderDescri;

/*
****************************************
brief : ý��������װ
****************************************
*/
typedef union 
{
    StruVideoDescri struVideo;
    StruAudioDescri struAudio;
    StruPictureDescri struPicture;
    StruOSDDescri struOsd;
    StruBinaryDescri struBinary;
    StruSysHeaderDescri struSysHeader;
}UnionMediaDescri;


/*
****************************************
brief :  ��ý��������װ
****************************************
*/
typedef struct _StruGSMediaDescri
{
    UINT32  eMediaType;  //ý������ EnumGSMediaType
    UnionMediaDescri unDescri;  //ý������
}GS_MEDIA_ATTRIBUTE_PACKED StruGSMediaDescri;


/*
****************************************
brief :  ����ý�塢�洢֡��ʽ��װ
**************************************** 
����(3Bytes)|�Ƿ�ؼ�֡(1bytes)|ý������(7bytes)|ʱ����4Bytes)| ����... 
*/
#define INVALID_TIME_STAMP ((UINT32)-1)
#define GS_FRAME_HEADER_MAGIC  (('G'<<24) | ('s'<<16) | ('c'<<8) | 'N') 
   
typedef struct _StruGSFrameHeader
{
    UINT32 iMagic;        //ħ���֣� ���ڱ�ʶÿ֡�Ŀ�ͷ
    UINT32 iLenght : 24;  //���ݳ��ȣ� ����������ͷ , ָ aFrameData����
    UINT32 bKey : 1;      //�Ƿ��ǹؼ�֡
    UINT32 eMediaType : 7;  //ý������
    UINT32 iTimeStamp;      //ʱ���   (-1) Ϊ��Чʱ��� INVALID_TIME_STAMP // (��1970��1��1��0ʱ0��0�뵽��ʱ������) modified by jiangsx 20121213
    //   unsigned char aFrameData[];  //֡����
}GS_MEDIA_ATTRIBUTE_PACKED StruGSFrameHeader;

/*
*********************************************************************
*
*@brief : GS_CODEID_GS_PS ����װ��֡ͷ
*
*********************************************************************
*/
typedef struct _StruGSPSFrameHeader
{
#define GSPSFRAMEHEADER_MAGIC  0x10000000
#define GSPSFRAMEHEADER_MARK1  (('G'<<0) | ('S'<<8))
#define GSPSFRAMEHEADER_MARK2  (('P'<<0) | ('S'<<8))

#define GSPS_CODETYPE_NONE 0x00


#define GSPS_CODETYPE_V_H264	0x1B
#define GSPS_CODETYPE_V_MP4		0x10
#define GSPS_CODETYPE_V_SVAC	0x80

#define GSPS_CODETYPE_A_G711	0X90
#define GSPS_CODETYPE_A_G722	0X92
#define GSPS_CODETYPE_A_G723 0X93
#define GSPS_CODETYPE_A_G729 0X99
#define GSPS_CODETYPE_A_SVAC 0X9B

	//���� GSPSFRAMEHEADER_MAGIC 
	UINT32 iMagic;
	 // ���� 0xFFFF
	UINT16 iLabel;
	 //��  GSPSFRAMEHEADER_MARK1
	UINT16 iGSMark1;
	//��   GSPSFRAMEHEADER_MARK2
	UINT16 iGSMark2; 
	//��������, �ο� GB28181 PS ��װ����
	UINT8  iCodeType; 
	//���� �Ƿ�ؼ�֡
	UINT8  bKey : 1;  
	//���� 0xF7	
	UINT8  iReserver : 7;  
	//
	UINT64 iTimestamp;
} GS_MEDIA_ATTRIBUTE_PACKED StruGSPSHeader;

#define GSPSHEADER_INIT(h, streamtype, xbKey) \
do { \
 (h).iMagic = GSPSFRAMEHEADER_MAGIC; \
 (h).iLabel = 0xFFFF; \
 (h).iGSMark1 = GSPSFRAMEHEADER_MARK1; \
 (h).iGSMark2 = GSPSFRAMEHEADER_MARK2; \
 (h).iCodeType = (streamtype); \
 (h).bKey = ((xbKey) ? 1 : 0); \
 (h).iReserver = 0xF7; (h).iTimestamp=(UINT64)-1; }while(0)

#define IS_GSPSHEADER(h) \
	((h).iMagic==GSPSFRAMEHEADER_MAGIC && (h).iLabel==0xFFFF && \
	 (h).iGSMark1==GSPSFRAMEHEADER_MARK1 && (h).iGSMark2==GSPSFRAMEHEADER_MARK2)

#ifdef _WIN32
#pragma pack( pop )
#endif


INLINE static  EnumGSMediaType CodeID2MediaType( EnumGSCodeID codeID)
{
    switch( codeID)
    {
    case GS_CODEID_NONE :
        return GS_MEDIA_TYPE_NONE;
        break;
    case GS_CODEID_BINARY :
        return GS_MEDIA_TYPE_BINARY;
        break;

        //��Ƶ����
    case GS_CODEID_GS_V2160I:
    case GS_CODEID_GS_V462C:
    case GS_CODEID_GS_V461C:
	case GS_CODEID_GS_V461A:
	case GS_CODEID_GS_V2160IV:
	case GS_CODEID_GS_VIPC:

    case GS_CODEID_HI_VDEFAULT:
    case GS_CODEID_HI_VMP4:
    case GS_CODEID_HI_VH264:
    case GS_CODEID_HI_COMPLEX :

    case GS_CODEID_HK_VDEFAULT:
    case GS_CODEID_HK_VMP4:
    case GS_CODEID_HK_VH264:
    case GS_CODEID_HK_COMPLEX :

    case GS_CODEID_DH_VDEFAULT:
    case GS_CODEID_DH_VMP4:
    case GS_CODEID_DH_VH264:
    case GS_CODEID_DH_COMPLEX :

    case GS_CODEID_HY_VDEFAULT:
    case GS_CODEID_HY_VMP4:
    case GS_CODEID_HY_VH264:
    case GS_CODEID_HY_COMPLEX :

	case GS_COIDEID_HH_VDEFULT :

	case GS_CODEID_ZBEN_VDEFULT:

	case GS_CODEID_CALSYS_VDEFULT:

	case GS_COIDEID_ALRS_VDEFULT:

	case GS_CODEID_XM_VDEFULT:

	case GS_CODEID_PS:
	case GS_CODEID_GS_PS:

        return GS_MEDIA_TYPE_VIDEO;
        break;

        //��Ƶ����
    case GS_CODEID_GS_A2160I:
    case GS_CODEID_GS_A462C:
    case GS_CODEID_GS_A461C:
    case GS_CODEID_GS_A2160IV:
	case GS_CODEID_GS_AIPC:

    case GS_CODEID_HI_ADEFAULT: 

    case GS_CODEID_HK_ADEFAULT :

    case GS_CODEID_DH_ADEFAULT :

    case GS_CODEID_HY_ADEFAULT :

	case GS_CODEID_ZBEN_ADEFULT:

	case GS_CODEID_CALSYS_ADEFULT:


        return GS_MEDIA_TYPE_AUDIO;
        break;

    default :
        if( codeID>=GS_CODEID_ST_MP4 && codeID<GS_CODEID_ST_MP3)
        {
            return GS_MEDIA_TYPE_VIDEO;
        }
        else if( codeID>=GS_CODEID_ST_MP3 && codeID<GS_CODEID_ST_JPEG )
        {
            return GS_MEDIA_TYPE_AUDIO;
        }
        else if( codeID>=GS_CODEID_ST_JPEG && codeID<GS_CODEID_GS_V2160I )
        {
            return GS_MEDIA_TYPE_PICTURE;
        }
        break;
    }
    return GS_MEDIA_TYPE_NONE;
}


#endif
