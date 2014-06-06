/*******************************************************************************
  Copyright (C), 2010-2011, GOSUN 
  File name   : DEVPARACFGDEF.H      
  Author      : jiangsx      
  Version     : Vx.xx        
  DateTime    : 2011/10/18 11:42
  Description : �豸�������ö��壨����˵���ο��ĵ����豸��������Э��˵��.docx����
*******************************************************************************/
#ifndef DEVPARACFGDEF_DEF_H
#define DEVPARACFGDEF_DEF_H

#include "GSCommon.h"

// Э�������ռ�
namespace	CmdProtocolDef
{
	// �����������
#define GS_CFG_OPT_SUCCESS "success"
#define GS_CFG_OPT_FAIL "fail"
#define GS_CFG_OPT_UNKNOWN_ERROR "unknownerror"


	// ��������ö��
	typedef enum
	{
		VIDEO_STREAM = 0,         // ��Ƶ��
		COMPLEX_STREAM            // ������
	} EnumGSStreamType;

	// �ֱ�������
#define GS_DCIF		"DCIF"
#define GS_CIF		"CIF"
#define GS_QCIF		"QCIF"
#define GS_4CIF     "4CIF"
#define GS_2CIF		"2CIF"
#define GS_D1		"D1"
#define GS_QVGA		 "QVGA"
#define GS_VGA		 "VGA"
#define GS_720P		"720P"
#define GS_1080P	"1080P"
#define GS_USRDEF	"USRDEF"

	// ��������
#define GS_BITRATE_CBR      "CBR"    // ������
#define GS_BITRATE_VBR	    "VBR"    // ������
#define GS_BITRATE_MBR		"MBR"    // �������

	// ͼ������ö��
	typedef enum
	{
		PQ_BEST = 0,   // ���
		PQ_SUB,        // �κ�
		PQ_BETTER,     // �Ϻ�
		PQ_GENERIC,    // һ��
		PQ_WORSE,            // �ϲ�
		PQ_WORST            // ��
	} EnumGSPicQuality;

	// ��Ƶ��������
#define GS_VIDEO_ENC_H264  "h264"
#define GS_VIDEO_ENC_MPEG4  "mpeg4"

	//��Ƶ�������� 0-OggVorbis;1-G711_U;2-G711_A;6-G726
	// ��Ƶ��������
#define GS_AUDIO_ENC_OGGVORBIS   "OggVorbis"
#define GS_AUDIO_ENC_G711_U       "G711_U"
#define GS_AUDIO_ENC_G711_A       "G711_A"
#define GS_AUDIO_ENC_G726         "G726"

	// ��Ƶ��ʽ
#define GS_VIDEO_FORMAT_PAL  "PAL"
#define GS_VIDEO_FORMAT_NTSC  "NTSC"



	// ���¶����������ͨ�ùؼ���
#define GS_CFG_PARAM  "cfgparam"
#define GS_CMD_ID   "cmd"



#define GS_GET_CFG   "getcfg"
#define GS_RES_GET_CFG "responsegetcfg"
#define GS_SET_CFG   "setcfg"
#define GS_RES_SET_CFG "responsesetcfg"

#define GS_DEV_PARAM  "devparam"
#define GS_DEV_ID   "devid"

#define GS_DEV_TYPE "devtype"

#define GS_OPT_CONTEXT  "context"        // ������ȡʱ�õ�
#define GS_RESULT   "result"			 // �������:  �ɹ���success  ʧ�ܣ� fail...

#define GS_CHANNEL_PARAM "chanparam"
#define GS_CHANNEL_TYPE "chantype"
#define GS_CHANNEL_ID  "chanid"


#define GS_ALL_CFG    "allcfg"


// �豸����
#define GS_DEV_CFG   "devcfg"


// �������
#define GS_NET_WORK  "network"                 
#define GS_DEV_IP     "devip"                   // �豸IP
#define GS_DEV_PORT    "devport"				// �豸�˿�
#define GS_DEV_USR_NAME  "devusrname"			// ��½�û���
#define GS_DEV_PASSWORD  "devpassword"			// ��½����
#define GS_DEV_MAC   "devmac"					// �豸MAC

// ѹ������
#define GS_COMPRESS   "compress"
#define GS_STREAM_TYPE          "streamtype"           // ��������
#define GS_RESOLUTION           "resolution"           // �ֱ���
#define GS_BITRATE_TYPE         "bitratetype"          // ��������
#define GS_PIC_QUALITY          "picquality"		   // ͼ������
#define GS_VIDEO_BITRATE        "videobitrate"         // ����
#define GS_VIDEO_FRAME_RATE     "videoframerate"       // ֡��
#define GS_INTERVAL_FRAME_I     "intervalframei"       // I֡��� 
#define GS_VIDEO_ENCODE_TYPE	"videoenctype"         // ��Ƶ�������� 
#define GS_AUDIO_ENCODE_TYPE    "audioenctype"         // ��Ƶ��������
#define GS_STREAM_TYPE_EX2      "streamtypeex2"        // ��������
#define GS_RESOLUTION_EX2       "resolutionex2"        // �ֱ���
#define GS_BITRATE_TYPE_EX2     "bitratetypeex2"       // ��������
#define GS_PIC_QUALITY_EX2      "picqualityex2"		   // ͼ������
#define GS_VIDEO_BITRATE_EX2    "videobitrateex2"      // ����
#define GS_VIDEO_FRAME_RATE_EX2  "videoframerateex2"   // ֡��
#define GS_INTERVAL_FRAME_I_EX2   "intervalframeiex2"  // I֡��� 
#define GS_VIDEO_ENCODE_TYPE_EX2	"videoenctypeex2"  // ��Ƶ�������� 
#define GS_AUDIO_ENCODE_TYPE_EX2   "audioenctypeex2"   // ��Ƶ��������
//#define GS_STREAM_TYPE_EX3  "streamtypeex3"          // ��������
//#define GS_RESOLUTION_EX3  "resolutionex3"           // �ֱ���
//#define GS_BITRATE_TYPE_EX3  "bitratetypeex3"        // ��������
//#define GS_PIC_QUALITY_EX3   "picqualityex3"		   // ͼ������
//#define GS_VIDEO_BITRATE_EX3  "videobitrateex3"      // ����
//#define GS_VIDEO_FRAME_RATE_EX3  "videoframerateex3" // ֡��
//#define GS_INTERVAL_FRAME_I_EX3   "intervalframeiex3"// I֡��� 
//#define GS_VIDEO_ENCODE_TYPE_EX3	"videoenctypeex3"  // ��Ƶ�������� 
//#define GS_AUDIO_ENCODE_TYPE_EX3   "audioenctypeex3" // ��Ƶ��������


// ͼ�����
#define GS_PICTURE   "picture"
#define GS_CHANNEL_NAME   "channame"							  // ͨ������
#define GS_VIDEO_FORMAT   "videoformat"							  // ��Ƶ��ʽ��1-NTSC��2-PAL 
#define GS_BRIGHTNESS    "brightness"							  // ���ȣ�ȡֵ��Χ[0,255]	
#define GS_CONTRAST    "contrast"								  // �Աȶȣ�ȡֵ��Χ[0,255] 
#define GS_SATURATION  "saturation"								  // ���Ͷȣ�ȡֵ��Χ[0,255] 
#define GS_HUE   "hue"											  // ɫ����ȡֵ��Χ[0,255] 


// OSD����
#define GS_OSD   "osd" 
#define GS_SHOW_CHANNELE_NAME  "showchanname"                     // Ԥ����ͼ�����Ƿ���ʾͨ������:0-����ʾ��1-��ʾ�������С704*576��
#define GS_SHOW_NAME_TOP_LEFT_X  "shownametopleftx"               // ͨ��������ʾλ�õ�x���� 
#define GS_SHOW_NAME_TOP_LEFT_Y  "shownametoplefty"              // ͨ��������ʾλ�õ�y����
#define GS_SHOW_OSD    "showosd"                                  // Ԥ����ͼ�����Ƿ���ʾOSD
#define GS_OSD_TOP_LEFT_X    "osdtopleftx"                        // OSD��x���� 
#define GS_OSD_TOP_LEFT_Y    "osdtoplefty"                        // OSD��y����
#define GS_OSD_TYPE   "osdtype"                                   // OSD����
#define GS_DISP_WEEK  "dispweek"                                  // �Ƿ���ʾ���ڣ�0-����ʾ��1-��ʾ 
#define GS_OSD_ATTRIB  "osdattrib"                                // OSD����
#define GS_OSD_STRING_TOP_LEFT_X   "stringtopleftx"               // �ַ����ݵ�x����
#define GS_OSD_STRING_TOP_LEFT_Y   "stringtoplefty"               // �ַ����ݵ�y����
#define GS_OSD_STRING   "osdstring"                               // �ַ�����


// ��Ƶ���β���
#define GS_VIDEO_MASK    "videomask"                    
#define GS_MASK_ENABLE    "enable"           // ʹ�ܿ���
#define GS_AREA           "area"             // ����
#define GS_INDEX          "index"            // ����(��1��ʼ)
#define GS_MASK_X         "x"                // x����
#define GS_MASK_Y         "y"                // y����
#define GS_MASK_HEIGHT    "h"                // �߶�
#define GS_MASK_WIDTH     "w"                // ���


// �豸��Ӳ������
#define GS_SOFTHARDWARE "softhardware"
#define GS_SOFTHARDWARE_DEVNAME              "devname"                     //�豸����
#define GS_SOFTHARDWARE_DEVNUM               "devnum"					   //�豸���
#define GS_SOFTHARDWARE_DEVTYPE              GS_DEV_TYPE                   //�豸�ͺ�
#define GS_SOFTHARDWARE_SERIALNO             "devserialno"                 //�豸���к�
#define GS_SOFTHARDWARE_SOFTWAREVERSION      "SoftWareVersion"             //����汾�� 
#define GS_SOFTHARDWARE_HARDWAREVERSION      "hardsoftversion"             //Ӳ���汾��
#define GS_SOFTHARDWARE_VIDEONUM             "videonum"                    //��Ƶͨ������
#define GS_SOFTHARDWARE_AUDIONUM             "audionum"                    //����ͨ������
#define GS_SOFTHARDWARE_ALARMINNUM           "alarminnum"                  //�����������
#define GS_SOFTHARDWARE_ALARMOUTNUM          "alarmoutnum"                 //�����������
#define GS_SOFTHARDWARE_NETIONUM              "netionum"                   //�������
#define GS_SOFTHARDWARE_COMIONUM             "comionum"                    //��������


// ������Ϣ����
#define GS_GPS_INFO						"gpsinfo"				
#define GS_GPS_VALID					"gpsvalid"
#define GS_GPS_LONGITUDE_TYPE			"longitudetype"
#define GS_GPS_LONGITUDE				"longitude"
#define GS_GPS_LATITUDE_TYPE			"latitudetype"
#define GS_GPS_LATITUDE					"latitude"
#define GS_GPS_ALTITUDE					"altitude"
#define GS_GPS_SPEED					"gpspeed"
#define GS_GPS_TIME						"gpstime"

//   ����ΪSIP28181�豸��ز�����(Э��ο�28181�涨)
/*********** begin *******/

#define GS_SIP_GBDEVICEID    "DeviceID"
#define GS_SIP_SUMNUM        "SumNum"
#define GS_SIP_ITEM           "Item"
#define GS_SIP_DEVICELIST     "DeviceList"
#define GS_SIP_NUM            "Num"


// �豸Ŀ¼��ѯ����
#define GS_SIP_CATALOG        "Catalog"
#define GS_SIP_NAME                       "Name"
#define GS_SIP_MANUFACTURER               "Manufacturer"
#define GS_SIP_MODEL                      "Model"
#define	GS_SIP_OWNER                      "Owner"
#define GS_SIP_CIVILCODE                  "CivilCode"
#define GS_SIP_ADDRESS                    "Address"
#define GS_SIP_PARENTAL                   "Parental"
#define GS_SIP_SAFETYWAY                  "SafetyWay"
#define GS_SIP_REGISTERWAY                "RegisterWay"
#define GS_SIP_SECRECY                    "Secrecy"
#define GS_SIP_STATUS                     "Status"

// �豸��Ϣ��ѯ����
#define GS_SIP_DEVICEINFO     "DeviceInfo"
#define GS_SIP_DEVICETYPE                 "DeviceType"
// #define GS_SIP_MANUFACTURER               "Manufacturer"
// #define GS_SIP_MODEL                      "Model"
#define GS_SIP_FIRMWARE                   "Firmware"
#define GS_SIP_MAXOUT                     "MaxOut"

// �豸״̬��ѯ����
#define GS_SIP_DEVICESTATUS    "DeviceStatus"
#define GS_SIP_ONLINE                     "Online"
#define GS_SIP_STATUS                     "Status"
#define GS_SIP_RECORD                     "Record"
#define GS_SIP_DEVICETIME                 "DeviceTime"
#define GS_SIP_ALARMSTATUS                "Alarmstatus"
#define GS_SIP_DUTYSTATUS                 "DutyStatus"
/*********** end ******* /

	// to add:


/*************************************************
// ex 1: get
<cfgparam>
	<cmd>getcfg</cmd>
	<devparam devid=1234>
		<context>compress</context> 
	</devparam>
	<devparam devid=1235>
		<chanparam chanid=1 chantype=101></chanparam>
		<chanparam chanid=2 chantype=101></chanparam>
		<context>picture</context>
		<context>compress</context> 
		<context>softhardware</context>
	</devparam>
</cfgparam>

// ex2: get response
<cfgparam>
	<cmd>responsegetcfg</cmd>
	<devparam devid=1234>
		<compress>
			<result>fail</result> 
		</compress>
	</devparam>
	<devparam devid=1235>
		<picture>
			<chanparam chanid=1 chantype=101>
				<result>success</result> 
				<channame>���������԰����</channame>
				<videoformat>PAL</videoformat>
			</chanparam>
			<chanparam chanid=2 chantype=101>
				<result>success</result> 
				<channame>���������԰�������</channame>
				<videoformat>1</videoformat>
			</chanparam>
		</picture>
		<compress>
			<result>success</result> 
			<streamtype>1</streamtype>
			<resolution>5</resolution>
			<bitratetype>1</bitratetype>
			<picquality>0</picquality>
			<videobitrate>1024</videobitrate>
			<videoframerate>25</videoframerate>
		</compress>
		<softhardware>
			 <result>success</result> 
		     <devname>Embedded Net DVR</devname>
             <devnum>255</devnum>
			 <devtype>""</devtype>
			 <devserialno>DS-8116HF-ST1620110715BBRR404140705WCVU</devserialno>
			 <SoftWareVersion>V2.0.1 build 110704</SoftWareVersion>
			 <hardsoftversion>0xc300</hardsoftversion>
			 <videonum>16</videonum>
			 <audionum>4</audionum>
			 <alarminnum>16</alarminnum>
			 <alarmoutnum>4</alarmoutnum>
			 <netionum>1</netionum>
			 <comionum>1</comionum>
		</softhardware>
	</devparam>
</cfgparam>

// ex 3:  set
<cfgparam>
	<cmd>setcfg</cmd>
	<devparam devid=1234>
		<network>
			<devip>192.168.15.88</devip>
			<devport>8000</devport>
			<devusrname>admin</devusrname>
			<devpassword>12345</devpassword>
		</network>
		<compress>
			<streamtype>1</streamtype>
			<resolution>5</resolution>
			<bitratetype>1</bitratetype>
			<picquality>0</picquality>
			<videobitrate>1024</videobitrate>
			<videoframerate>25</videoframerate>
		</compress>
		<picture>
			<chanparam chanid=1 chantype=101>
				<channame>���������԰����</channame>
				<videoformat>1</videoformat>
			</chanparam>
			<chanparam chanid=2 chantype=101>
				<channame>���������԰�������</channame>
				<videoformat>1</videoformat>
			</chanparam>
		</picture>
	</devparam>
	<devparam devid=1235>
		<picture>
			<chanparam chanid=1 chantype=101>
				<channame>���������԰����</channame>
				<videoformat>1</videoformat>
			</chanparam>
		</picture>
	</devparam>
</cfgparam>

// ex4: set response

<cfgparam>
	<cmd>responsesetcfg</cmd>
	<devparam devid=1234>
		<network>
			<result>success</result> 
		</network>
		<compress>
			<result>success</result> 
		</compress>
		<picture>
			<chanparam chanid=1 chantype=101>
				<result>fail</result> 
			</chanparam>
			<chanparam chanid=2 chantype=101>
				<result>success</result> 
			</chanparam>
		</picture>
	</devparam>
	<devparam devid=1235>
		<picture>
			<chanparam chanid=1 chantype=101>
				<result>success</result> 
			</chanparam>
		</picture>
	</devparam>
</cfgparam>

**************************************************/

//
// ��������XML��װ��
//
class CXMLCfg
{
public:
	CXMLCfg(void){};
	~CXMLCfg(void){};

	XMLNode& GetXMLNode(void) { return m_csXMLNode; };

private:
	XMLNode m_csXMLNode;
};

// 
// XML�����
// 
typedef CXMLCfg CXMLMaker;

//
//  XML������
//
typedef CXMLCfg CXMLParser;

}

#endif // DEVPARACFGDEF_DEF_H
