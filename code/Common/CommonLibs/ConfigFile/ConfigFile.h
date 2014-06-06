/*************************************************
Copyright (C), 2010-2011, GOSUN 
File name : CConfigFile.H      
Author : laodx, jinfn      
Version : V1.00       
DateTime : 2010/5/11 16:06
Description :�����ļ���������ɶ������ļ���װ�أ���ȡ���޸ģ�ɾ��������Ĳ���
*************************************************/

#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include "GSCommon.h"
#include "GSType.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define COMMENT_CHAR '#'
#define MAX_LEN_OF_LINE 256

typedef struct
{
	string strKey;
	string strValue;

	string strCommentTop;
	string strCommentRight;
} StruItemInfo, *StruItemInfoPtr;

typedef struct
{
	string strName;

	string strCommentTop;
	string strCommentRight;

	vector<StruItemInfo> vectItemList;
} StruSectionInfo, *StruSectionInfoPtr;

typedef vector<StruSectionInfoPtr> VectSectionList;

class CConfigFile
{
public:
	CConfigFile(void);
	~CConfigFile(void);

public:
	//���������ļ�
	BOOL LoadFile(char *pszFileName, char szToken = '#');
  
	//������Ĺ��������ļ�
	BOOL SaveFile(char *pszFileName);

	//����������öεĸ���
	INT32 GetSectionNameList(vector<string>& vectNameList);

	//���ָ��Section�µ��������ô�
	INT32 GetAllStringOfSection(char *pszSection, vector<string>& vectString);

	//��ȡĳ���������ֵ
	string GetProperty(char *pszSection, char *pszKey, char *pszDefaultValue);
	INT32 GetProperty(char *pszSection, char *pszKey, INT32 iDefaultValue);

	//����ĳ���������ֵ
	BOOL SetProperty(char *pszSection, char *pszKey, char *pszValue, char *pszComment);
	BOOL SetProperty(char *pszSection, char *pszKey, INT32 iValue, char *pszComment);

	//����ĳ���������ֵ
	BOOL AddProperty(char *pszSection, char *pszKey, char *pszValue, char *pszComment);
	BOOL AddProperty(char *pszSection, char *pszKey, INT32 iValue, char *pszComment);

	//ɾ��ĳ��������
	BOOL DeleteProperty(char *pszSection, char *pszKey);

	//ɾ������������
	void ClearAllProperty(void);

private:
	string Trim(const char *pszLine);
	string TrimLeft(const char *pszLine);
	string TrimRight(const char *pszLine);
	StruItemInfoPtr FindItem(char* pszSection, char* pszKey);	

private:
	string m_strConfigFile;
	VectSectionList m_vectSectionList;
	char m_szSplitToken;
};
#endif // _CONFIG_FILE_H_
