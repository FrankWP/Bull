/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPPARSER.H
Author :  ������
Version : 1.0.0.1
Date: 2012/11/7 16:41
Description: 2818 ��һЩ����
********************************************
*/

#ifndef _GS_H_SIPPARSER_H_
#define _GS_H_SIPPARSER_H_
#include "../GSPObject.h"
namespace GSP
{

namespace SIP
{


	class CSIPParser : public CGSPObject
	{
	public:
		CSIPParser(void);
		~CSIPParser(void);
	};

	class C28181ParserSubject
	{
	public :
		CGSString m_strSendDevID; //ý�����������豸����
		CGSString m_strSendStreamSeq; //���Ͷ�ý�������к�
		CGSString m_strRcvDevID;  //ý�����������豸����
		CGSString m_strRcvStreamSeq; //���ն�ý����ϵ�к�

		C28181ParserSubject(const CGSString &strStr  );
		C28181ParserSubject(void);
		BOOL Decode( const CGSString &strStr );		
		CGSString Encode(void);
	};







	class CMANSCDP_XMLParser : public CGSPObject
	{
	private :
		typedef CMANSCDP_XMLParser *(*FuncPtrMXmlParser)(const CGSString &strCmdType,
			XMLNode &stXMLNode);

		static FuncPtrMXmlParser s_vDecoders[];
	public :
		CGSString m_strCmdType;
	public :
		CMANSCDP_XMLParser(void);
		virtual ~CMANSCDP_XMLParser(void);

		
		static CMANSCDP_XMLParser *GuessParser(const CGSString &strVal);

		virtual CGSString Encode(void) = 0;
	};

	class CMXmlMediaStatusParser : public CMANSCDP_XMLParser
	{
	public :
		static const char s_czCmdType[];
		CGSString m_strSn;
		CGSString m_strDevId;
		CGSString m_strNotifyType;
	public :
		CMXmlMediaStatusParser(void);
		~CMXmlMediaStatusParser(void);
		virtual CGSString Encode(void);	
		static CMANSCDP_XMLParser *Create( const CGSString &strCmdType,
												XMLNode &stXMLNode);
	};


	class CMansRtspPaser  : public CGSPObject
	{
	public :
		StruGSPCmdCtrl m_stGspCtrl;	
		double m_fSpeed;
	public :
		CMansRtspPaser(void);
		~CMansRtspPaser(void);
		EnumErrno Decode( const CGSString &strVal);
		CGSString Encode(UINT32 iCSeq);
	};



	//28181 ����ģʽת��
	INT TransModel28181SSName2I( const CGSString &strSName );
	const char *TransModel28181I2SName( INT iModel );


} //end namespace SIP

} //end namespace GSP

#endif //end _GS_H_SIPPARSER_H_
