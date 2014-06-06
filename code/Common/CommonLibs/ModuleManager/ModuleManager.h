#ifndef _MODULE_MANAGER_H
#define _MODULE_MANAGER_H
 
#include"IModuleBase.h"

#if !defined(_WIN32) && !defined(_LINUX)
#error This program only support Windows, Linux...
#endif

#ifdef _LINUX
#include <dlfcn.h>

typedef void* HMODULE;
#endif

class CModuleManager;

typedef std::vector<IModuleBasePtr> VectModuleList;
typedef std::map<IModuleBasePtr, HMODULE> MapModuleList;

class CModuleManager
{
public:
	CModuleManager();
	virtual ~CModuleManager();

public:
	//���ݲ��·�����ز��
	IModuleBasePtr Load(const std::string strDllPath, const INT32 iModuleType);		

	//���ݲ������ж�ز��
	BOOL UnLoad(const std::string strDllName);

	//ж��ȫ����� 
	BOOL UnloadAll();

	//���ݲ�����ƻ�ȡ������ƽṹָ��
	IModuleBasePtr GetModule(const std::string strDllName);

	//��ȡ����ṹ��Ϣ�б�
	INT32 GetModuleList(VectModuleList& vectModuleList);

private:

	MapModuleList m_mapModuleList;
};

#endif			//_MODULE_MANAGER_H
