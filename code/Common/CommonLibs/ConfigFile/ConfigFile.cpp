/*************************************************
Copyright (C), 2010-2011, GOSUN 
File name : ConfigFile.cpp      
Author : laodx, jinfn
Version : Vx.xx        
DateTime : 2010/5/11 16:06
Description :     // ������ϸ˵���˳����ļ���ɵ���Ҫ���ܣ�������ģ��
// �����Ľӿڣ����ֵ��ȡֵ��Χ�����弰������Ŀ�
// �ơ�˳�򡢶����������ȹ�ϵ
*************************************************/ 

#include "ConfigFile.h"

// ���캯��
CConfigFile::CConfigFile(void)
{
	m_vectSectionList.clear();
	m_szSplitToken = '#';
}

// ��������
CConfigFile::~CConfigFile(void)
{
	ClearAllProperty();
}

/*************************************************
  Function:	LoadFile     
  DateTime: 2010/5/11 16:15	
  Description: װ�������ļ�
  Input:
		pszFileName:	�����ļ���·��������Ϊ��
  Output:         	
  Return:
		TRUE:	�ɹ�
		FALSE:	ʧ��
  Note:
*************************************************/
BOOL CConfigFile::LoadFile(char *pszFileName, char szToken /* = */ )
{
	m_szSplitToken = szToken;

	StruSectionInfo *pstSectionInfo = NULL;
	StruItemInfo stItemInfo;

	FILE *fpConfig = NULL;
	char szBuf[MAX_LEN_OF_LINE];
	string::size_type pos;
	string strComment;

	// �ļ���Ϊ���򷵻س���
	if (pszFileName == NULL)
		return FALSE;

	// ���ļ���ʧ���򷵻س���
#ifdef WINCE
	fpConfig = fopen(pszFileName, "r");
#elif _WIN32
	fopen_s(&fpConfig, pszFileName, "r");
#elif _LINUX
	fpConfig = fopen(pszFileName, "r");
#endif
	if (fpConfig == NULL)
		return FALSE;

	// ��¼�������ļ���·����
	m_strConfigFile = pszFileName;

	// ��ȡ�����ļ��е�ÿһ��
	while (fgets(szBuf, MAX_LEN_OF_LINE, fpConfig))
	{
		// ȥ��ǰ��Ŀո��
		string strLine = TrimLeft(szBuf);

		// ����ǿ�������ԣ�������һ��
		if (strLine.empty() || strLine.at(0)=='\r' || strLine.at(0)=='\n')
			continue;

		// �Ƿ�Ϊע��
		if (strLine.at(0) == COMMENT_CHAR)
		{
			// ��֮ǰ��ע�ͣ��򱣴�� szToken ����������ݣ�
			// ��֮ǰ�Ѵ���ע�ͣ��򽫱��е�ע�����ݣ�����szToken)�����ں���
			if (strComment.empty())
				strComment = strLine.substr(1);
			else
				strComment += strLine;

			continue;
		}

		// �Ƿ�Ϊ��
		if (strLine.at(0) == '[')
		{
			// ��������ڶ�Ӧ�Ľ�������']'������Ϊ�Ƿ����ݣ�����֮����ɾ��֮ǰ��ע�ͣ�Ȼ�������һ��
			// ������']'����Ϊ�Ϸ��Ķ����ƣ������µĶ���Ϣ�ṹ����д�����ƺ�ע�ͣ�Ȼ������������
			pos = strLine.find(']');
			if (pos != string::npos)
			{
				pstSectionInfo = new StruSectionInfo;

				// ��д���öε����ƣ���Ϊ�ַ����а�����'['�����substr()��count����Ϊpos-1��
				pstSectionInfo->strName = Trim(strLine.substr(1, pos-1).c_str());

				// ����ҷ��Ƿ���ע�ͣ�������д���
				strLine = Trim(strLine.substr(pos+1).c_str());
				if (!strLine.empty() && strLine.at(0) == szToken)
					pstSectionInfo->strCommentRight = Trim(strLine.substr(1).c_str());
				else
					pstSectionInfo->strCommentRight = "";

				// ����Ϸ�����ע�ͣ�����д���
				pstSectionInfo->strCommentTop = TrimRight(strComment.c_str());	// ɾ������ķָ����ͻس���

				// �������ö�������
				m_vectSectionList.push_back(pstSectionInfo);
			}

			// ����Ϸ���ע������
			strComment.clear();

			continue;
		}

		// �ų����Ͽ��ܣ�ʣ�µ�Ӧ�����������ʽΪ��
		// 1) KEY = VALUE
		// 2) STRING

		// ��д��������Ϣ�ṹ����������ֵ��ע�ͣ��������Ӧ�νṹ������������

		// ����'='������������ΪSTRING���͵�������
		pos = strLine.find('=');
		if (pos == string::npos)
		{
			//STRING���͵��������strKey��Ϊ��
			stItemInfo.strKey = "";
		}
		else
		{
			// ��дKey��substr��count����Ϊpos��
			stItemInfo.strKey = Trim(strLine.substr(0, pos).c_str());

			// ȥ��Key���ֵ����ݺ͵Ⱥţ�ʣ��������ǣ�Value��ע��
			strLine.erase(0, pos + 1);
		}

		// ����ҷ��Ƿ���ע�ͣ���������Ӧ�Ĵ���Value���ҷ�ע��
		pos = strLine.find(szToken);
		if (pos != string::npos)
		{
			// ��дValue��'szToken'��ߵ�����
			stItemInfo.strValue = Trim(strLine.substr(0, pos - 1).c_str());

			// ��дע�ͣ�'szToken'�ұߵ�����
			stItemInfo.strCommentRight = Trim(strLine.substr(pos + 1).c_str());
		}
		else
		{
			stItemInfo.strValue = Trim(strLine.c_str());
			stItemInfo.strCommentRight = "";
		}

		// ��д�Ϸ���ע��
		stItemInfo.strCommentTop = TrimRight(strComment.c_str());	// ɾ������ķָ����ͻس���
		strComment.clear();

		// �ҵ������������֮ǰ��δ�����κ����öΣ��򴴽�һ���յ����ö�
		if (pstSectionInfo == NULL)
		{
			pstSectionInfo = new StruSectionInfo;
			m_vectSectionList.push_back(pstSectionInfo);
		}

		// ������������뵱ǰ�����öε��������б���
		pstSectionInfo->vectItemList.push_back(stItemInfo);
	}

	fclose(fpConfig);

	return TRUE;
}

/*************************************************
  Function:	SaveFile     
  DateTime: 2010/5/11 16:16	
  Description: ������Ĺ��������ļ�
  Input:
		pszFileName:	�����ñ��浽���ļ������ò���Ϊ�գ��򱣴浽LoadFile()ʱָ�����ļ���
  Output:
  Return:
		TRUE:	�ɹ�
		FALSE:	ʧ��
  Note:				
*************************************************/
BOOL CConfigFile::SaveFile(char *pszFileName)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;

	FILE *fpConfig;
	char *fnSaveFile;

	fnSaveFile = (pszFileName != NULL) ? pszFileName : (char *)m_strConfigFile.c_str();
	if (fnSaveFile == NULL)
		return FALSE;

#ifdef WINCE
	fpConfig = fopen(pszFileName, "w");
#elif _WIN32
	fopen_s(&fpConfig, fnSaveFile, "w");
#elif _LINUX
	fpConfig = fopen(fnSaveFile, "w");
#endif
	if (fpConfig == NULL)
		return FALSE;

	// ����ÿ��Section
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		//
		// ��Sectionд���ļ���
		//

		// дע��
		if (! pSectionInfo->strCommentTop.empty())
			fprintf(fpConfig, "%c%s\n", m_szSplitToken, pSectionInfo->strCommentTop.c_str());

		// д�ε�����
		if (! pSectionInfo->strName.empty())
		{
			// д��[������]
			fprintf(fpConfig, "[%s]", pSectionInfo->strName.c_str());

			// �ҷ��Ƿ���ע�ͣ�
			if (! pSectionInfo->strCommentRight.empty())
				fprintf(fpConfig, "\t%c %s\n", m_szSplitToken, pSectionInfo->strCommentRight.c_str());
			else
				fprintf(fpConfig, "\n");
		}

		// д���ڵ�����������
		for (vector<StruItemInfo>::iterator iter2 = pSectionInfo->vectItemList.begin(); iter2 != pSectionInfo->vectItemList.end(); iter2++)
		{
			// �Ϸ��Ƿ���ע�ͣ�
			if (! iter2->strCommentTop.empty())
				fprintf(fpConfig, "%c%s\n", m_szSplitToken, iter2->strCommentTop.c_str());

			if (iter2->strKey.empty())
			{
				// д��STRING
				fprintf(fpConfig, "%s", iter2->strValue.c_str());
			}
			else
			{
				// д��Key = Value
				fprintf(fpConfig, "%s = %s", iter2->strKey.c_str(), iter2->strValue.c_str());
			}

			// �ҷ��Ƿ���ע�ͣ�
			if (! iter2->strCommentRight.empty())
				fprintf(fpConfig, "\t%c %s\n", m_szSplitToken, iter2->strCommentRight.c_str());
			else
				fprintf(fpConfig, "\n");
		}

		// ��ÿ�����öεĽ���ʱ������һ������
		fprintf(fpConfig, "\n");
	}

	fclose(fpConfig);

	return TRUE;
}

/*************************************************
  Function:	GetSectionNameList    
  DateTime:	2010/5/11 16:11	
  Description: ��ȡ�������öεĸ�������������
  Input:
  Output:
		vectNameList:	�������������Ƶ�����
  Return:
		���öε�����
  Note:
*************************************************/
INT32 CConfigFile::GetSectionNameList(vector<string>& vectNameList)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;

	int iSectionNum = 0;

	// ����ÿ�����ö�
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		// �����öε����Ƽ���������
		vectNameList.push_back(pSectionInfo->strName);

		iSectionNum++;
	}

	return iSectionNum;
}

/*************************************************
  Function:	GetAllStringOfSection
  DateTime:	2010/8/18
  Description: ��ȡָ�����ö��µ��������ô�
  Input:
  Output:
        pszSection: ָ�����öε����ƣ�����Ϊ��
		vectString:	������ô�������
  Return:
		���ô�������
  Note:
*************************************************/
INT32 CConfigFile::GetAllStringOfSection(char *pszSection, vector<string>& vectString)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;
	INT32 iNum = 0;

	// pszSection����Ϊ��
	if (pszSection == NULL)
		return FALSE;

	// ����ÿ��Section
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		// ��ָ����Section�������ж�Section�Ƿ�ΪҪ�ҵ�Section����������ԣ�����������һ��Section
		if ((pszSection != NULL) && (strcmp(pSectionInfo->strName.c_str(), pszSection) != 0))
			continue;

		// �������е�������
		for (vector<StruItemInfo>::iterator iter2 = pSectionInfo->vectItemList.begin(); iter2 != pSectionInfo->vectItemList.end(); iter2++)
		{
			if (iter2->strKey.empty())
				vectString.push_back(iter2->strValue); //STRING���͵�������
			else
				vectString.push_back(iter2->strKey+"="+iter2->strValue); //KEY=VALUE���͵�������

			iNum++;
		}

		break;
	}

	return iNum;
}

/*************************************************
  Function:	GetProperty
  DateTime: 2010/5/11 16:19	
  Description:  ��ȡĳ���������ֵ
  Input:
		pszSection:	���ö����ƣ�Ϊ�����ʾ���������ö��н��в���
		pszKey:		������ļ�
		pszDefaultValue:����������ڣ��򷵻ظ�Ĭ��ֵ
  Output:       
  Return:
		�����������ֵ������������ڣ��򷵻�pszDefaultValue��ֵ        
  Note:				
*************************************************/
string CConfigFile::GetProperty(char *pszSection,char *pszKey, char *pszDefaultValue)
{
	StruItemInfoPtr pItemInfo = FindItem(pszSection, pszKey);
	if (pItemInfo == NULL)
		return (pszDefaultValue != NULL) ? pszDefaultValue : "";	// δ�ҵ�������
	else
	{		
		string str = Trim(pItemInfo->strValue.c_str());
		// ����ַ���Ϊ������ԣ�����Ĭ������ֵ
		if (str.empty() || str.at(0)=='\r' || str.at(0)=='\n')
		{
			return (pszDefaultValue != NULL) ? pszDefaultValue : "";	// δ�ҵ�������
		}
	}
	return pItemInfo->strValue;	// �ҵ�������
}

INT32 CConfigFile::GetProperty(char *pszSection,char *pszKey, INT32 iDefaultValue)
{
	StruItemInfoPtr pItemInfo = FindItem(pszSection, pszKey);
	return (pItemInfo == NULL) ? iDefaultValue : atoi(pItemInfo->strValue.c_str());
}

/*************************************************
  Function:	SetProperty
  DateTime: 2010/5/11 16:23	
  Description: �޸�ĳ���������ֵ
  Input:
		pszSection:	���öε����ƣ�Ϊ�����ʾ���������ö��н��в���
		pszKey:		������ļ�������Ϊ��
		pszValue:	�������ֵ������Ϊ��
		pszComment:	�������ע��
  strKey�����������ƣ�
  strDefaultValue:Ĭ��ֵ           
  Output:        
  Return:	�����Ƿ�ɹ���ʧ�ܷ���FALSE���ɹ�����TRUE         	
  Note:				
*************************************************/
BOOL CConfigFile::SetProperty(char *pszSection, char *pszKey, char *pszValue, char *pszComment)
{
	StruItemInfoPtr pItemInfo;

	// ������ļ���ֵ������Ϊ��
	if (pszKey == NULL || pszValue == NULL)
		return FALSE;

	// ����������Ƿ���ڣ��������򷵻س���
	pItemInfo = FindItem(pszSection, pszKey);
	if (pItemInfo == NULL)
		return FALSE;

	// �޸��������ֵ��ע��

	pItemInfo->strValue = pszValue;

	if (pszComment != NULL)
		pItemInfo->strCommentRight = pszComment;

	return TRUE;
}

BOOL CConfigFile::SetProperty(char *pszSection, char *pszKey, INT32 iValue, char *pszComment)
{
	ostringstream oss;
	oss << iValue;
	return SetProperty(pszSection, pszKey, (char *)oss.str().c_str(), pszComment);
}

/*************************************************
  Function:	AddProperty
  DateTime: 2010/5/11 16:23	
  Description:	����һ��������
  Input:
		pszSection:	���ö����ƣ���ָ�������öβ����ڣ��򴴽��µ����öΣ����ò���Ϊ�գ�����������ӵ����һ�����ö��У�
		pszKey:		������ļ�
		pszValue:	�������ֵ
		pszComment:	�������ע��
  Output:        
  Return:
		FALSE:		ʧ��
		TRUE:		�ɹ�         	
  Note:				
*************************************************/
BOOL CConfigFile::AddProperty(char *pszSection,char *pszKey, char *pszValue, char *pszComment)
{
	StruItemInfoPtr pItemInfo = NULL;
	StruSectionInfoPtr pSectionInfo = NULL;
	VectSectionList::iterator iter;

	// ������ļ���ֵ������Ϊ��
	if (pszKey == NULL || pszValue == NULL)
		return FALSE;
	
	// ����������Ƿ���ڣ������򷵻س���
	pItemInfo = FindItem(pszSection, pszKey);
	if (pItemInfo != NULL)
		return FALSE;

	if (pszSection == NULL)
	{
		// pszSectionΪ�գ�����������ӵ����һ�����ö���

		if (! m_vectSectionList.empty())
			pSectionInfo = m_vectSectionList.back();
	}
	else
	{
		// ָ����pszSection������Ҹ�Section�Ƿ����

		for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
		{
			pSectionInfo = *iter;

			// �ҵ�Section���˳�
			if (strcmp(pSectionInfo->strName.c_str(), pszSection) == 0)
				break;

			pSectionInfo = NULL;
		}
	}

	// pSectionInfoΪ�գ���ʾ���öβ����ڣ���Ҫ�����µ����ö�
	if (pSectionInfo == NULL)
	{
		pSectionInfo = new StruSectionInfo;
		pSectionInfo->strName = (pszSection ? pszSection : "");
		m_vectSectionList.push_back(pSectionInfo);
	}

	// ��д���������Ϣ������ӵ���Ӧ���öε�������������
	pItemInfo = new StruItemInfo;
	pItemInfo->strKey = pszKey;
	pItemInfo->strValue = pszValue;
	pItemInfo->strCommentRight = (pszComment != NULL) ? pszComment : "";
	pSectionInfo->vectItemList.push_back(*pItemInfo);
	delete pItemInfo;

	// ��ӳɹ�
	return TRUE;
}

BOOL CConfigFile::AddProperty(char *pszSection,char *pszKey, INT32 iValue, char *pszComment)
{
	ostringstream oss;
	oss << iValue;
	return AddProperty(pszSection, pszKey, (char *)oss.str().c_str(), pszComment);
}

/*************************************************
  Function:	DeleteProperty  
  DateTime: 2010/5/11 16:27	
  Description: ɾ��ָ����������
  Input:
		pszSection:	���öε����ƣ�Ϊ�����ʾ���������ö��н��в���
		pszKey:		����������ƣ�Ϊ�����ʾɾ���������ö�
  strKey�����������ƣ�         	
  Output:         
  Return:ɾ��ĳ���������Ƿ�ɹ����ɹ�����TRUE��ʧ�ܷ���FALSE         	
  Note:				
*************************************************/
BOOL CConfigFile::DeleteProperty(char *pszSection, char *pszKey)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;

	// ������һ����Ϊ��
	//
	// pszSection���գ�pszKey���գ�	��pszSection�в���pszKey����ɾ��
	// pszSection�գ�pszKey���գ�	������Section�в���pszKey����ɾ��
	// pszSection���գ�pszKey�գ�	ɾ������pszSection
	if ((pszSection == NULL) && (pszKey == NULL))
		return FALSE;

	// ����ÿ��Section
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		// ��ָ����Section�������ж�Section�Ƿ�ΪҪ�ҵ�Section����������ԣ�����������һ��Section
		if ((pszSection != NULL) && (strcmp(pSectionInfo->strName.c_str(), pszSection) != 0))
			continue;

		// pszKeyΪ�գ���ʾɾ������section
		if (pszKey == NULL)
		{
			pSectionInfo->vectItemList.clear();
			m_vectSectionList.erase(iter);
			delete pSectionInfo;
			return TRUE;
		}

		// ����ͬ����������ҵ�����ɾ��
		for (vector<StruItemInfo>::iterator iter2 = pSectionInfo->vectItemList.begin(); iter2 != pSectionInfo->vectItemList.end(); iter2++)
		{
			if (strcmp(iter2->strKey.c_str(), pszKey) == 0)
			{
				pSectionInfo->vectItemList.erase(iter2);

				// ������������Ϊ�գ������Ӧ�����ö�Ҳɾ��
				if (pSectionInfo->vectItemList.empty())
					m_vectSectionList.erase(iter);

				return TRUE;
			}
		}
	}

	// δ�ҵ���������س���
	return FALSE;
}

/*************************************************
  Function:	ClearAllProperty
  DateTime: 2010/5/11 16:28	
  Description;	ɾ�����е�������
  Input:
  Output:         
  Return:
  Note:				
*************************************************/
void CConfigFile::ClearAllProperty(void)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;

	// ����ÿ�����ö�
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		// ������ö��е�����������
		pSectionInfo->vectItemList.clear();

		// ������öζ�Ӧ�Ľṹ
		delete pSectionInfo;
	}

	// ������ö�����
	m_vectSectionList.clear();
}

/*************************************************
  ����Ϊ˽�к���
*************************************************/

/*************************************************
  Function:	FindItem     
  DateTime: 2010/6/04 10:28	
  Description: ���ݶ���������������������
  Input:
		pszSection:	���öε����ƣ�Ϊ�����ʾ���������ö��н��в���
		pszKey:		����������ƣ�����Ϊ��
  Output:
  Return:
		NULL:		δ�ҵ���Ӧ��������
		��NULL:		��Ӧ���������Ϣ�ṹָ��
  Note:				
*************************************************/
StruItemInfoPtr CConfigFile::FindItem(char *pszSection, char *pszKey)
{
	StruSectionInfoPtr pSectionInfo;
	VectSectionList::iterator iter;

	// ����������Ϊ�գ����򷵻س���
	if (pszKey == NULL)
		return NULL;

	// ����ÿ�����ö�
	for (iter = m_vectSectionList.begin(); iter != m_vectSectionList.end(); iter++)
	{
		pSectionInfo = *iter;

		if (pSectionInfo == NULL)
			continue;

		// ��ָ����Section�������ж�Section�Ƿ�ΪҪ�ҵ�Section����������ԣ�����������һ��Section
		if ((pszSection != NULL) && (strcmp(pSectionInfo->strName.c_str(), pszSection) != 0))
			continue;

		// �����ö��б���ÿ��������ҵ�ͬ������������������������ض�Ӧ��������ṹ
		for (vector<StruItemInfo>::iterator iter2 = pSectionInfo->vectItemList.begin(); iter2 != pSectionInfo->vectItemList.end(); iter2++)
		{
			if (strcmp(iter2->strKey.c_str(), pszKey) == 0)
			{				
				return &(*iter2);
			}
			
		}
	}

	// δ�ҵ�ͬ������������س���
	return NULL;
}

/*************************************************
  Function:	Trim     
  DateTime: 2010/6/04 10:28	
  Description: ȥ���ַ���ǰ��ķָ������������ո��Ʊ�����س������С�
  Input:
		pszLine:	��������ַ���
  Output:
  Return:
		ȥ���ָ������string
  Note:				
*************************************************/
string CConfigFile::Trim(const char *pszLine)
{
	string::size_type pos;
    string str = pszLine;

	pos = str.find_first_not_of(" \t\n\r");
	if (pos != string::npos)
		str.erase(0, pos);

	// ɾ��ǰ��Ŀո��Ʊ�����س�������
	str.erase(0,str.find_first_not_of(" "));

	pos = str.find_last_not_of(" \t\n\r");
	if (pos != string::npos)
		str.erase(pos + 1);
	//ɾ������Ŀո��Ʊ�����س�������
	str.erase(str.find_last_not_of(" ") + 1,str.size()-1);

    return str;
}

string CConfigFile::TrimLeft(const char *pszLine)
{
	string::size_type pos;
    string str = pszLine;

	pos = str.find_first_not_of(" \t\n\r");
	if (pos != string::npos)
		str.erase(0, pos);
	// ɾ��ǰ��Ŀո�
	str.erase(0,str.find_first_not_of(" "));
    return str;
}

string CConfigFile::TrimRight(const char *pszLine)
{
	string::size_type pos;
    string str = pszLine;

	pos = str.find_last_not_of(" \t\n\r");
	if (pos != string::npos)
		str.erase(pos + 1);
	//ɾ������Ŀո�
	str.erase(str.find_last_not_of(" ") + 1,str.size()-1);

    return str;
}
