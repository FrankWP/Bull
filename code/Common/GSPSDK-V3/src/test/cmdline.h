/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : CMDLINE.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/8/18 8:50
Description: �����д���
********************************************
*/
#ifndef GSP_CMDLINE_DEF_H
#define GSP_CMDLINE_DEF_H

//���ط�0 ��ʾ��������
typedef int (*CommandHandleFunctionPtr)(const char *czCmd,const char *czArgs);
typedef struct _StruOptionLine
{
    char *czCmd;         //�� - ��ͷ
    char *czShortCmd;   //�� -- ��ͷ
    char *czDescri;
    CommandHandleFunctionPtr fnHandle;
}StruOptionLine;

//-e  ���� --exit  Ĭ��Ϊ�˳� ��Ҫʹ��
//-�� --�� Ϊ����

// ������czCmd==NULL && czShortCmd==NULL Ϊ��
extern void OptionsEntery( StruOptionLine Options[]);

//args �¹���ʽ -key: value -key1 -key2 -ke3: val
extern char *ArgsGetParser(const char *czArgs, char *czBuffer);
// -1 ��ʾʧ�ܣ� 0 ��ʾ������ 1 ��ʾ���Լ���
extern int ParserFetch(char **ppParser, char **czOptionKey, char **czOptionValue);

#ifndef strcasecmp

#ifdef WINCE
#define strcasecmp    strcmp
#elif _WIN32
#define strcasecmp    stricmp
#endif

#endif





extern BOOL TestCreateMyThread( void* (*fnCb)(void*), void *pParam );


#endif