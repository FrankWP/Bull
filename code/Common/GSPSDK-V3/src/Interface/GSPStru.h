#ifndef GSP_GSPSTRU_DEF_H
#define GSP_GSPSTRU_DEF_H
#include <GSType.h>



/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : GSPSTRU.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/5/14 9:47
Description: ����GSP Э���õ���һЩ�����ͺ���������
********************************************
*/

/*
****************************************
brief :  һ�¶���Э���õĵ�һЩ���
****************************************
*/

    namespace GSP
    {
//�ṹΪ���ֽڶ���
#ifdef _WIN32
#define ATTRIBUTE_PACKED 
#pragma pack( push,1 )
#else
#define ATTRIBUTE_PACKED    __attribute__ ((packed)) 
#endif


#define GSP_PACKET_MAX_LEN (512*1024)


/*
****************************************
brief : ����ID�Ķ���
****************************************
*/
typedef enum
{
    GSP_CMD_ID_NONE  = 0,


//��������, 
    GSP_CMD_ID_REQUEST,             //���� StruGSPCmdRequest
    GSP_CMD_ID_RET_REQUEST,         //���� StruGSPCmdRetRequest

//�������� 
    GSP_CMD_ID_CTRL,                //����  StruGSPCmdCtrl
    GSP_CMD_ID_RET_CTRL,            //����  StruGSPCmdReturn

//Keepalive ����
     GSP_CMD_ID_KEEPAVLIE = 5,      //����  StruGSPCmdKeepalive
    //----������û�ظ�



//�Ͽ�����
     GSP_CMD_ID_CLOSE,              //���� StruGSPCmdReturn
     GSP_CMD_ID_RET_CLOSE,          //���� StruGSPCmdReturn 

//����(����)�������
     GSP_CMD_ID_COMPLETE,           //����  ��
     GSP_CMD_ID_RET_COMPLETE,       //����  StruGSPCmdReturn

//�ش����� 
     GSP_CMD_ID_RESEND = 10,        //����  StruGSPCmdResend
    //--�ظ�ͨ �ο� GSP �ش����Ͷ���


//ý�������Ϣ���� 
     GSP_CMD_ID_MEDIAATTRI,         // ����  StruMediaInfoTable

//����ظ����ŵ�״̬
     GSP_CMD_ID_REQUEST_STATUS,            // ����  StruRequestStatus;
     GSP_CMD_ID_RET_STATUS,            // ���� StruPlayStatus;
//�Ƿ�����ظ�
     GSP_CMD_ID_RET_UNKNOWN_COMMAND,         //����ظ�δ֪����  ����  StruGSPCmdReturn, ����iErrno ��ʾ����ʶ�������

     //�������쳣
     GSP_CMD_ID_STREAM_ASSERT = 15,       //����  StruGSPCmdReturn,


     //�����쳣�����ӽ����ر�
     GSP_CMD_ID_ASSERT_AND_CLOSE,       //����  StruGSPCmdReturn, ����Ҫ�ظ�

	//ʹ��UDP �������������
	 GSP_UDP_SET_SETUP,				//����  StruUdpSetupRequest, ��Ҫ�ظ�
	 GSP_UDP_SET_SETUP_RESPONSE,    //����  StruUdpSetupRespone, ��Ҫ�ظ�

	 GSP_UDP_SEND_TEST_ADDR,			//����UDP����TEST_ADDR ����,  ��
	 GSP_UDP_SEND_TEST_ADDR_RESPONSE = 20,	//GSP_UDP_SEND_TEST_ADDR �ظ�,  ����   StruGSPCmdReturn

	 GSP_RESET_TRANS_ON_TCP,		     //Ҫ�����ʹ��TCP ����������, ����  ��
	 GSP_RESET_TRANS_ON_TCP_RESPONSE,		//����  StruGSPCmdReturn
	 
}EnumGSPCommandID;


/*
****************************************
brief : ý������������
****************************************
*/
typedef enum
{
    GSP_MEDIA_ATTRI_NAME_NONE = 0,
    GSP_MEDIA_ATTRI_NAME_SECTION_BEGIN,
    GSP_MEDIA_ATTRI_NAME_SECTION_END,
    GSP_MEDIA_ATTRI_NAME_MEDIATYPE,  //ý������, �ο�ý������, EnumGSMediaType����
    GSP_MEDIA_ATTRI_NAME_SECTION_ID,    //��ͨ����
    GSP_MEDIA_ATTRI_NAME_CODE_ID,    //�������ͱ��

    GSP_MEDIA_ATTRI_NAME_VIDEBEGIN = 30,
    GSP_MEDIA_ATTRI_NAME_VIDEO_WIDTH,      //��Ƶ����ͼ��Ŀ�
    GSP_MEDIA_ATTRI_NAME_VIDEO_HEIGHT,     //��Ƶ����ͼ��ĸ�
    GSP_MEDIA_ATTRI_NAME_VIDEO_FRAMERATE,  //��Ƶ����֡�� , ��8λ��ʾ �������� ,��8λ��ʾ С������                                            
                                           //��  0x0c05 ��ʾ 12.5 ֡

    GSP_MEDIA_ATTRI_NAME_VIDEO_BITRATE,    //��Ƶ������   ��λ  Kbytes

  


    GSP_MEDIA_ATTRI_NAME_AUDIOBEGIN =  100, 
    GSP_MEDIA_ATTRI_NAME_AUDIO_SAMPLE,    //����Ƶ��     ��λ KHz
    GSP_MEDIA_ATTRI_NAME_AUDIO_CHANNELS,  //��Ƶ��ͨ����
    GSP_MEDIA_ATTRI_NAME_AUDIO_BITS,   //��Ƶ�Ĳ���λ��


    GSP_MEDIA_ATTRI_NAME_OSDBEGIN = 200,
    GSP_MEDIA_ATTRI_NAME_OSD_X,           //OSD���ӵ����� X
    GSP_MEDIA_ATTRI_NAME_OSD_Y,             //OSD���ӵ����� Y
    GSP_MEDIA_ATTRI_NAME_OSD_DATA_TYPE,         // OSD���� ��������
    GSP_MEDIA_ATTRI_NAME_OSD_TRANSPARENCY,   //OSD ���ӵ�͸����  0~100  100Ϊ��͸��


    GSP_MEDIA_ATTRI_NAME_BINARYBEGIN = 300,  //�����Ʊ���
    GSP_MEDIA_ATTRI_NAME_BINARY_LEN,        //���� 1~16 λ
    GSP_MEDIA_ATTRI_NAME_BINARY_LEN1,        //����16~32λ
    GSP_MEDIA_ATTRI_NAME_BINARY_LEN2,        //����32~48λ
    GSP_MEDIA_ATTRI_NAME_BINARY_LEN3,        //����48~64λ

    GSP_MEDIA_ATTRI_NAME_SYSHEADER_BEGIN = 400,  //ϵͳͷ
    GSP_MEDIA_ATTRI_NAME_SYSHEADER_LEN,   //���� 1~16 λ
    GSP_MEDIA_ATTRI_NAME_SYSHEADER_LEN1,   //����16~32λ

}EnumGSPMediaAttrName;




/*
****************************************
brief : GSP �ش����Ͷ���
****************************************
*/
const INT GSP_RESEND_TYPE_MEDIA_INFO   =   1;   //�ش� ý����Ϣ, 
//iSubChn, iArgs1,iArgs2, iArgs3 ������ʹ��
// ����  GSP_CMD_ID_MEDIAATTRI
const INT GSP_RESEND_TYPE_STREAM_SYS_INFO  =  2;   //�ش� ����ϵͳͷ����Щ������ϵͳͷ
//iSubChn, iArgs1,iArgs2, iArgs3 ������ʹ��
// ���� ����������ý�������

const INT  GSP_RESEND_TYPE_PACKET        =     3;   //�ش� ���ݰ�, 
//iSubChn   ��Ӧ������ͨ��,�ο���ͷ����
//iArgs1,   ��Ӧ����Seq ��,�ο���ͷ����
//iArgs2    ��Ӧ����SSeq ��ʼ��, ����������,�ο���ͷ����
//iArgs3    ��Ӧ����SSeq ������,, ����������,�ο���ͷ����      iArgs2<SSeq<iArgs3
// ���� �������İ�����



/*
****************************************
brief : Ч�鷽ʽ����
****************************************
*/

#define CRC_TYPE_NONE 0       //��Ч��
#define CRC_TYPE_CRC16 0x01  //crc16
#define CRC_TYPE_CRC32 0x02   //crc32
#define CRC_TYPE_MD5   0x03   // md5

/*
****************************************
brief : ��ǰGSP�汾�Ŷ���
****************************************
*/
const INT  GSP_VERSION =2;
const INT  GSP_VERSION_V1=0;

/*
****************************************
brief : GSP ���������Ͷ���
****************************************
*/
const INT  GSP_PACKET_TYPE_CMD  = 0;   //����Ϊ����
const INT  GSP_PACKET_TYPE_STREAM = 1; //����Ϊ��

/*
****************************************
brief : TSP Keepavlie ��������
****************************************
*/
const INT  KEEPALIVE_ARGS_NONE     = 0; //Ϊû�����壬 
const INT  KEEPALIVE_ARGS_REQUEST  = 1; //��ʾ �����߾��췢��Keepavlie

/*
****************************************
brief : GSP�������Ͷ���
****************************************
*/
const INT  GSP_TRAN_RTPLAY       = 0; //ʵʱԤ��
const INT  GSP_TRAN_REPLAY      =  1;  //�ļ��ط�
const INT  GSP_TRAN_DOWNLOAD    =  2;  //���ط�ʽ  

/*
****************************************
brief :  GSP Э�����Ŷ���
****************************************
*/

const INT  GSP_PRO_RET_SUCCES      =   0;  //�ɹ�
const INT  GSP_PRO_RET_EUNKNOWN    =   1;  //δ֪����
const INT  GSP_PRO_RET_EPERMIT     =   2;  //û��Ȩ��
const INT  GSP_PRO_RET_ENCHN       =   3;  //����ͨ��������
const INT  GSP_PRO_RET_EVER        =   4;  //��֧������İ汾
const INT  GSP_PRO_RET_ECODE       =   5;  //��֧������ı����ʽ
const INT  GSP_PRO_RET_ECLOSE      =   6;  //�����Ѿ����ر�
const INT  GSP_PRO_RET_EINVALID    =   7;  //�Ƿ�����
const INT  GSP_PRO_RET_EEND       =    8;  //���Ѿ�����
const INT  GSP_PRO_RET_ENEXIST      =  9;  //�����ڵ�������Դ
const INT  GSP_PRO_RET_EPRO         =   10; //�Ƿ�Э��
const INT   GSP_PRO_RET_ECMD        =    11; //�Ƿ�����
const INT  GSP_PRO_RET_EILLEGAL     =   12;//�߼�����
const INT  GSP_PRO_RET_EEXIST      =    13; //������Դ�Ѿ��ڴ�
const INT  GSP_PRO_RET_EASSERT     =    14;  //�����쳣
const INT  GSP_PRO_RET_EIO         =    15;  //�����������
const INT  GSP_PRO_RET_EBUSY       =     16;  //��������Դȱ��
const INT  GSP_PRO_RET_EPACKET     =     17;  //�յ��Ƿ����ݰ�
const INT  GSP_PRO_RET_EKP        =     18;      // Keepalive ��ʱ
const INT  GSP_PRO_RET_ESTREAM_ASSERT = 19;    //���쳣






/*
****************************************
brief :    GSP ������������
****************************************
*/

//iSubChn  0 ��ʾ��������ͨ�� ,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʾ�Ƿ��ɹؼ�֡��ʼ���� TRUE/FALSE
//iArgs2  ��ʹ��
const INT  GSP_CTRL_PLAY   =   (1<<0);    //����  


//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʹ��
//iArgs2 ��ʹ��
const INT  GSP_CTRL_STOP   =   (1<<1);    //ֹͣ 

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʹ��
//iArgs2 ��ʹ��
const INT  GSP_CTRL_PAUSE   =  (1<<2);     //��ͣ

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ����ı��� ��Χ 1~32
//iArgs2  ��ʹ��
const INT  GSP_CTRL_FAST   =   (1<<3);     //���

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ����ı��� ��Χ 1~32
//iArgs2  ��ʹ��
const INT  GSP_CTRL_BFAST   =  (1<<4);     //����

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʹ��
//iArgs2 ��ʹ��
const INT  GSP_CTRL_STEP   =   (1<<5) ;    //��֡ǰ

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʹ��
//iArgs2 ��ʹ��
const INT  GSP_CTRL_BSTEP   =  (1<<6) ;    //��֡��

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ����ı��� ��Χ 1~32
//iArgs2  ��ʹ��
const INT  GSP_CTRL_SLOW   =   (1<<7);     //����

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ����ı��� ��Χ 1~32
//iArgs2  ��ʹ��
const INT  GSP_CTRL_BSLOW   =  (1<<8) ;    //����

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ��ʾƫ������ (�ο� GSP ƫ�����Ͷ���)
//iArgs2  ƫ�Ƶ�ֵ
const INT  GSP_CTRL_SETPOINT = (1<<9)  ;   //�϶�

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1  ͼƬ��
//iArgs2  ʱ������������
const INT  GSP_CTRL_SECTION   = (1<<10) ;    //��Ƭ

//iSubChn  0 ��ʾ��������ͨ��,������ʾ���ƶ�Ӧ����ͨ����
//iArgs1   TRUE ��ʾ��ʼ�� FALSE ��ʾ����
//iArgs2 ��ʹ��
const INT  GSP_CTRL_FLOWCTRL   = (1<<11) ;    //��������




/*
****************************************
brief : GSP ƫ�����Ͷ���
****************************************
*/
const INT  GSP_OFFSET_TYPE_BYTES  = 0 ; //ƫ���ļ���ͷ���ֽ���
const INT  GSP_OFFSET_TYPE_SECS  = 1 ; //ƫ���ļ���ͷ������
const INT  GSP_OFFSET_TYPE_RATE  = 2 ; //ƫ�Ƶ���ֱ�


/*
****************************************
brief :  GSP Э�� �������Ͷ���
****************************************
*/
const INT  GSP_PRO_FILTER_TYPE_NKEY  = 1;  //���˷ǹؼ�֡



/*
*********************************************************************
*
*@brief : GSP Э�������䷽ʽ
*
*********************************************************************
*/
const INT GSP_STREAM_TRANS_MODE_MULTI_TCP = 0x00;    //�������� ��GSP�����ϸ��ϴ�����
const INT GSP_STREAM_TRANS_MODE_RTP_UDP = 0x01;		//ʹ��RTP/UDP ����
const INT GSP_STREAM_TRANS_MODE_TCP = 0x02;			//ʹ�ö�������TCP ����

/*
****************************************
brief :  ����GSPͨ�ŵ�ͷ��ʽ
****************************************
*/
typedef struct _StruGSPPacketHeader
{

    UINT32 iLen : 24;   //����������, ���� ͷ��+Playload+Ч��
    UINT32 iVersion : 3;   //GSP �汾��
    UINT32 iDataType : 1;  //�������� ���ο� GSP ���������Ͷ���)
    UINT32 iCRC : 3;       //Ч�鷽ʽ ( �ο�Ч�鷽ʽ���� )
    UINT32 bEnd : 1;       //�Ƿ�Ϊ������
    UINT16 iSeq;            //���ݰ���������
    UINT16 iSSeq;          //�ְ�������Ҫ����0��ʼ
    UINT8  iSubChn;        // �Ự����ͨ����,��1��ʼ��������ͨ����ָ��Ϊ1, �Ƿ�ͨ��
    UINT8  iExtraVal;     //����ֵ�����Թ����������ʵ��
    unsigned char cPlayload[2]; //����ռλ,����Ϊ2��Ϊ���ֽڶ��룬
    //������ͷ������ GSP_PACKET_HEADER_LEN��ȡ
} ATTRIBUTE_PACKED StruGSPPacketHeader;

/*
****************************************
brief : ����Э��ͷ�ĳ���
****************************************
*/
#define GSP_PACKET_HEADER_LEN  (sizeof(StruGSPPacketHeader)-2)


/*
****************************************
brief :  ý����Ϣ�б�ṹ
****************************************
*/
typedef struct _StruMediaAttribute
{
    UINT16 iNname; //������ , �ο� enumGSPMediaAttrName
    UINT16 iValue; //����ֵ
} ATTRIBUTE_PACKED StruMediaAttribute;


typedef struct _struMediaInfoTable
{
    UINT32 iRows;  //ָ�� aRows�ĸ��� 
    StruMediaAttribute aRows[1];
} ATTRIBUTE_PACKED StruMediaInfoTable;

static INLINE UINT32 GetMediaInfoTableSize( const StruMediaInfoTable *struTablePtr )
{
UINT32 iRet;
    if( !struTablePtr )
    {
        return 0;
    }
    iRet = struTablePtr->iRows*sizeof(StruMediaAttribute);
    return iRet+4;
}

/*
****************************************
brief : ���������ʽ
****************************************
*/
typedef struct _StruGSPCommand
{      
    UINT32 iCmdID; //����ID   �� �ο� ����ID�Ķ��� )
    UINT32 iTag;   //ʶ��ţ�������������䣬 �������ԭ������
    unsigned char cPlayload[4];  //��������������
} ATTRIBUTE_PACKED StruGSPCommand;

#define GSP_PRO_COMMAND_HEADER_LENGTH (sizeof(StruGSPCommand)-4)

/*
****************************************
brief : ͨ�õĻظ������
****************************************
*/
typedef struct _StruGSPCmdReturn
{    
    UINT32 iErrno;    //��������  (�ο� GSP Э�����Ŷ��� )
} ATTRIBUTE_PACKED StruGSPCmdReturn;



/*
****************************************
brief : ���������
****************************************
*/
#define GSP_MAGIC  ('G'<<24|'s'<<16|'p'<<8|0xa3)
#define MAX_URI_SIZE  128




//��������
typedef struct _StruGSPCmdRequest
{
    UINT32 iMagic;    //ħ���֣����� GSP_MAGIC��������Ƚ��ж�Ϊ�Ƿ�����
    UINT8 iTransMode; //����ģʽ ��GSP Э�������䷽ʽ)
	UINT8 iTransType; //�������� ���ο� GSP�������Ͷ���)   
	UINT16 iClientPort; // RTP_UDP/TCP ģʽʱ�Ŀͻ��˵�UDP�˿�/TCP �ͻ��˶˿�
	unsigned char czClientIP[66];  // RTP_UDP/TCP ģʽʱ�Ŀͻ��˵�IP
	UINT16 iURIBufLen; 
    unsigned char szURI[4];  //��Դ��λ  ������ iURIBufLen ����
    //StruMediaInfoTable *stStreamAttri; //������������
} ATTRIBUTE_PACKED StruGSPCmdRequest;

//���� struGSPCmdRequest ��̬��Ա stStreamAttri ֵ�ĺ�

#define GSPCMDREQUEST_STREAMATTRI(x)  ((StruMediaInfoTable*)((unsigned char *)(x)+8+68+(x)->iURIBufLen))

#define GSPCMDREQUEST_BASE_SIZE  (8+68)


//�ظ�����
typedef struct _StruGSPCmdRetRequest
{
    UINT32 iMagic;    //ħ���֣����� GSP_MAGIC��������Ƚ��ж�Ϊ�Ƿ�����  
    UINT16 iErrno;      //��������  (�ο� GSP Э�����Ŷ��� )
    UINT16 iKeepaliveTimeout; // keepalive ��ʱ���� �� ��λ ��
    UINT32 iAbilities; //������׼�� ��Ϊ��ʾ�� �� �ο� GSP ������������)
	UINT8 iTransType; //�������� ���ο� GSP�������Ͷ���)  	
	UINT8 iTransMode;  //����ģʽ ��GSP Э�������䷽ʽ)
    unsigned char czReserver[5];  //����
	UINT8  iRtpPlayloadType;  //��ʹ�� RTP_UDP ģʽʱ��RTP ʹ�õ�PT
	UINT32 iRtpSSRC;  //��ʹ�� RTP_UDP ģʽʱ��RTP ʹ�õ�SSRC
	UINT16 iServerPort; // RTP_UDP/TCP ģʽʱ�ķ�������UDP�˿�/TCP �ͻ��˶˿�
	unsigned char czServerIP[66];  // RTP_UDP/TCP ģʽʱ�ķ�������IP	
    StruMediaInfoTable stStreamAttri;  //�������� ����ΪNULL
} ATTRIBUTE_PACKED StruGSPCmdRetRequest;

/*
****************************************
brief : �Ͽ������
****************************************
*/
//������� ������Ϊ�� 

//�ظ����� ,������Ϊ StruGSPCmdReturn


/*
****************************************
brief : ���������
****************************************
*/

//����
typedef struct _StruGSPCmdCtrl
{
    UINT32 iCtrlID; //�������� �� �ο� GSP ������������)
    UINT16 iSubChn; //ָ�����Ƶ���ͨ���� 0 ��ʾ����ͨ��
    INT16 iArgs1;   //���ƵĲ��� 1 ,�Ϳ���������� �� �ο� GSP ������������)
    INT32 iArgs2;   //���ƵĲ��� 2 ���Ϳ����������  �� �ο� GSP ������������)


} ATTRIBUTE_PACKED StruGSPCmdCtrl;

//�ظ����� ,������Ϊ StruGSPCmdReturn


/*
****************************************
brief : Keepalive �����
****************************************
*/

typedef struct _StruGSPCmdKeepalive
{
    UINT32 iMagic; //ħ���֣����� GSP_MAGIC��������Ƚ��ж�Ϊ�Ƿ�����
    UINT32 iArgs;  //���������ο� GSP Keepavlie ��������) 
} ATTRIBUTE_PACKED StruGSPCmdKeepalive;

/*
****************************************
brief : �ش������
****************************************
*/

//��������
typedef struct _StruGSPCmdResend
{
    UINT16 iType;   //������ش�����  ( �ο� GSP �ش����Ͷ���)
    UINT16 iSubChn; //ָ�����Ƶ���ͨ���� 0 ��ʾ����ͨ��
    UINT16 iArgs1;  //��������������أ� ( �ο� GSP �ش����Ͷ���)
    UINT16 iArgs2;  //��������������أ� ( �ο� GSP �ش����Ͷ���)
    UINT16 iArgs3;   //��������������أ� ( �ο� GSP �ش����Ͷ���)
} ATTRIBUTE_PACKED StruGSPCmdResend;

//�ظ��������������������


/*
****************************************
brief : ý�������Ϣ�����
****************************************
*/
//������Ϊ StruMediaAttribute

/*
****************************************
brief :   GSP ״̬����
****************************************
*/
const INT   GSP_STATUS_READY =  0; //׼������
const INT   GSP_STATUS_NORMAL  = 1; //��������
const INT   GSP_STATUS_PAUSE   = 2; //��ͣ
const INT   GSP_STATUS_FAST   =  3;  //���
const INT   GSP_STATUS_SLOW    = 4;  //����
const INT   GSP_STATUS_BFAST  =  5;   //����
const INT   GSP_STATUS_BSLOW  =  6 ;  //������
const INT   GSP_STATUS_STEP    = 7;  //��֡ǰ��
const INT   GSP_STATUS_BSTEP  =  8 ; //��֡����


/*
****************************************
brief :   GSP ״̬������
****************************************
*/
const INT   GSP_REQUEST_ONE      =   1;  //ֻ����һ��  , iArgs������
const INT   GSP_REQUEST_INTERVAL  =  2 ; //����Զ����� , iArgs ����Ϊ��������� , 0 ��ʾ�Զ�
const INT   GSP_REQUEST_STOP      =  3 ; //ֹͣ�Զ�����״̬, iArgs������

typedef struct _StruRequestStatus
{
   UINT16 iRequest;  //����ʽ, �ο�GSP ״̬������
   UINT16 iArg;      //�ο�GSP ״̬������
} ATTRIBUTE_PACKED StruRequestStatus;

typedef struct _StruPlayStatus
{
   UINT8 iStatus;  //״̬, �ο� GSP ״̬���� 
   UINT8 iSpeed;
   UINT16 iPosition; //��ǰλ�ã� ��ֱ�
} ATTRIBUTE_PACKED StruPlayStatus;



#define MAX_UDP_PACKET_CHANNEL 200
//�ش����� 
#define UDP_CHANNEL_REQUEST_RESEND  (MAX_UDP_PACKET_CHANNEL+1)  
	//���� UDP ��ַ ����Ϊ0
#define UDP_CHANNEL_TEST_ADDR		(MAX_UDP_PACKET_CHANNEL+2)	
//UDP �˿ڶ��� ����Ϊ0
#define UDP_CHANNEL_KEEPALIVE_PORT  (MAX_UDP_PACKET_CHANNEL+3)   
 //UDP �뿪
#define UDP_CHANNEL_BYE				(MAX_UDP_PACKET_CHANNEL+4)     


typedef struct _StruUdpResendRequest
{
	UINT16 iSeq;
	UINT16 iSSeq;
	UINT16 iCounts;
} ATTRIBUTE_PACKED StruUdpResendRequest;


#define UDP_TRANSPORT_ON_RTP 0x00000001

typedef struct _StruUdpSetup
{	
	 //����ΪSDP
	INT32 iContentLen;
	BYTE m_vContent[1];
} ATTRIBUTE_PACKED StruUdpSetupRequest;

typedef struct _StruUdpSetupRespone
{
	//����ΪSDP
	INT32 iErrno;
	INT32 iContentLen;
	BYTE m_vContent[1];
} ATTRIBUTE_PACKED StruUdpSetupRespone;


#ifdef _WIN32
#pragma pack( pop )
#endif


}



using namespace GSP;

#endif
