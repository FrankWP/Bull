#include "GSUtility.h"
#include <assert.h>
using namespace std;



/*
*********************************************************************
*
*@brief : �ַ�����������
*
*********************************************************************
*/


#define MAX_STRING_LENGHT (100L<<20) //����ַ������


namespace GS
{

	class CTokenizer
	{
	protected:
		INT m_iOffset;
		const std::string m_strTemp;
		std::string m_strToken;
		std::string m_strDelimiters;
	public:
#define DEFAULT_DELIMITERS " "

		CTokenizer(const std::string& str)
			: m_iOffset(0), m_strTemp(str)
			, m_strDelimiters(DEFAULT_DELIMITERS)
		{

		}

		CTokenizer(const std::string& str, const std::string& delimiters)
			: m_iOffset(0),
			m_strTemp(str),
			m_strDelimiters(delimiters)
		{

		}

		// ������һ����ʶ
		bool NextToken(void)
		{
			return NextToken(m_strDelimiters);
		}

		bool NextToken(const std::string& delimiters)
		{
			// ������һ����ʶ��ʼ�ַ�λ��
			UINT i = m_strTemp.find_first_not_of(delimiters, m_iOffset);
			if (i == std::string::npos) {
				m_iOffset = m_strTemp.length();
				return false;
			}

			// ���ұ�ʶ����λ��
			UINT j = m_strTemp.find_first_of(delimiters, i);
			if (j == std::string::npos) {
				m_strToken = m_strTemp.substr(i);
				m_iOffset = m_strTemp.length();
				return true;
			}

			m_strToken = m_strTemp.substr(i, j - i);
			m_iOffset = j;
			return true;
		}

		// ��ȡ��ʶ�ַ���
		const std::string GetToken(void) const
		{
			return m_strToken;
		}

		// ���ñ�ʶλ��
		void Reset(void)
		{
			m_iOffset = 0;
		}


	};

} // end namespace GS


void GSStrUtil::Split( std::vector<CGSString> &csResult, const CGSString& str, const CGSString& delimiters)
{
	GS::CTokenizer csTemp(str, delimiters);
	csResult.clear();

	while( csTemp.NextToken( ) )
	{
		csResult.push_back( csTemp.GetToken() );
	}
}

BOOL GSStrUtil::EqualsIgnoreCase( const CGSString& strSrc, const CGSString& strDest)
{

	return ToLower(strSrc) == ToLower(strDest);
}

CGSString GSStrUtil::TrimLeft(const CGSString& str) 
{
	std::string t = str;
	t.erase(0, t.find_first_not_of(" \t\n\r"));
	return t;
};


CGSString GSStrUtil::TrimRight(const CGSString& str) 
{
	std::string t = str;
	t.erase(t.find_last_not_of(" \t\n\r") + 1);
	return t;
};


CGSString GSStrUtil::Trim(const CGSString& str) 
{
	std::string t = str;
	t.erase(0, t.find_first_not_of(" \t\n\r"));
	t.erase(t.find_last_not_of(" \t\n\r") + 1);
	return t;
};

// ----------------------------------------------------------------------
// ������: GSStrUtil::ToLower
// ����: jiangsx
// ����: ת��ΪСд�ַ���
// ����ֵ: [string] ����ַ���
// ����: 
//   [const string& str] �����ַ���
// ----------------------------------------------------------------------
CGSString GSStrUtil::ToLower(const CGSString& str) 
{
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), ::tolower);
	return t;
};


CGSString GSStrUtil::ToUpper(const CGSString& str) 
{
	string t = str;
	std::transform(t.begin(), t.end(), t.begin(), ::toupper);
	return t;
};



BOOL GSStrUtil::VFormat(CGSString &strOString,const char *czFormat, va_list ap )
{
	char sBuffer[256];
	va_list apStart;
	char *pBuffer;
	int n, size = 256; 


	strOString.clear();

	apStart = ap;       
	pBuffer = sBuffer;
	while (pBuffer) {           
		ap = apStart;
		n = vsnprintf (pBuffer, size, czFormat, ap);          
		if (n > -1 && n < size )
		{
			//�ɹ���ʽ��
			//pBuffer[n] = '\0';
			strOString =  pBuffer;
			if( pBuffer!=sBuffer )
			{      
				::free(pBuffer);
			}
			return TRUE;
		}
		if( pBuffer!=sBuffer )
		{      
			::free(pBuffer);
		}
		pBuffer = NULL;
		size *= 2;
		if( size>MAX_STRING_LENGHT )
		{
			assert(0);
			return FALSE;
		}
		
		pBuffer = (char*) ::malloc(size);
		assert(pBuffer!=NULL);
	}
	if( pBuffer && pBuffer!=sBuffer  )
	{
		::free(pBuffer);
	}
	return FALSE;
}



BOOL GSStrUtil::Format(CGSString &strOString, const char* czFormat, ...)
{
	BOOL bRet;
	va_list ap;
	va_start(ap, czFormat);
	bRet = GSStrUtil::VFormat(strOString, czFormat, ap);
	va_end(ap);
	return  bRet;

}

BOOL GSStrUtil::AppendWithFormat(CGSString &strIOString, const char* czFormat, ...)
{
	BOOL bRet; 
	CGSString strTemp;
	va_list ap;
	va_start(ap, czFormat);
	bRet = GSStrUtil::VFormat(strTemp, czFormat, ap);
	va_end(ap);

	if( bRet )
	{
		strIOString += strTemp;
	}
	return bRet;
}
