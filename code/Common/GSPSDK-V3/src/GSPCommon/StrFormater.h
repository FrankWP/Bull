/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : STRFORMATER.H
Author :  ������
Version : 1.0.0.1
Date: 2013/4/28 9:58
Description: �ַ�������һЩ����
********************************************
*/

#ifndef _GS_H_STRFORMATER_H_
#define _GS_H_STRFORMATER_H_
#include "GSPObject.h"

namespace GSP
{



/*
*********************************************************************
*
*@brief : CStrFormater �ַ�������һЩ����
*
*********************************************************************
*/
#define RTP_SPACE_CHARS " \r\n\t"

class CStrFormater : CGSPObject
{
public :	
	CStrFormater(void);
	~CStrFormater(void);
	static void Skip(const char **pp, const char *czEnd);

	static  void SkipSpaces(const char **pp);

	static BOOL StrIsStart(const char *str, const char *pfx, 
		const char **ptr);			
	static void GetWordUntilChars(CGSString &strWord, 
		const char *sep, const char **pp);

	static INLINE void GetWordSep( CGSString &strWord, 
		const char *sep, const char **pp)
	{
		if (**pp == '/') (*pp)++;
		GetWordUntilChars(strWord, sep, pp);
	}

	static INLINE void GetWord(CGSString &strWord, const char **pp)
	{
		GetWordUntilChars(strWord,RTP_SPACE_CHARS, pp );
	}

	static void RightTrim( CGSString& strSrc, const char *szTrimKey ) 
	{
		std::string t = strSrc;
		t.erase(t.find_last_not_of(szTrimKey) + 1);
		strSrc = t;				
	};

	// �� ����ʱ�� ת��Ϊ UTC��1970��   yyyymmddhhmmss
	static INT64 ParserTimeString(const CGSString &strValue );

	//�ѽ�����תΪ�ַ���
	static void BinaryToString(const BYTE *pBind, int iLen, CGSString &strResult );

	//iBufLen �������
	static BOOL StringToBinary( const CGSString &strString, BYTE *pBuf, int &iBufLen );


};

} //end namespace GSP

#endif //end _GS_H_STRFORMATER_H_
