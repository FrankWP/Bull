#ifndef _GOSUNCN_SIMPLELOG_H_
#define _GOSUNCN_SIMPLELOG_H_

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h> 
#include <stdio.h>
#include <string>

/************************************************************************/
/* �����¼������־�ļ���־��*/
/************************************************************************/


class CSimpleLog
{
public:
	CSimpleLog();
	virtual ~CSimpleLog();


	int SetLogPath(char* chPath);
	void Add(const char* fmt, ...);



private:
	enum {BUFSIZE = 3000};  //����������
	char	m_tBuf[BUFSIZE];

	CRITICAL_SECTION  m_crit;  	//����һ���ٽ���
	FILE*             m_fp;
	std::string       m_logdir;
};

#endif 