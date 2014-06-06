#include "ModuleManager.h"

#ifdef _WIN32
#define LOAD_MODULE(dll)			LoadLibraryA(dll)
#define FREE_MODULE(handle)			FreeLibrary(handle)
#define GET_PROC(handle, proc_name)	GetProcAddress(handle, proc_name)
#else /* _LINUX */
#define LOAD_MODULE(dll)			dlopen(dll, RTLD_LAZY)
#define FREE_MODULE(handle)			dlclose(handle)
#define GET_PROC(handle, proc_name)	dlsym(handle, proc_name)
#endif

CModuleManager::CModuleManager()
{
	m_mapModuleList.clear();
}

CModuleManager::~CModuleManager()
{
	UnloadAll();
}

/************************************************************************************************************/
/*funtion:���ݲ��·�����ز��,��ȡ������ƽṹ,ִ�в�����ƽṹ��Init����									*/
/*parameter:strDllPath Ҫ���صĲ���ľ���·��																*/
/*          iModuleType Ҫ���صĲ��������																	*/
/*return: �ɹ��򷵻ز���Ŀ��ƽṹָ��,ʧ���򷵻�NULL														*/
/*author:hwh																								*/
/*time:2010.5.17-08:44																						*/
/************************************************************************************************************/
IModuleBasePtr CModuleManager::Load(const std::string strDllPath, const INT32 iModuleType)
{
	//���ز��(��̬���ӿ�)
	HMODULE hModule = NULL;

	hModule = LOAD_MODULE(strDllPath.c_str());
	if (!hModule)
		return NULL;

	MapModuleList::iterator iter;

	//���Ҹò���Ƿ��ڲ���б����� 
	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		if(hModule == iter->second)
		{
			return iter->first;
		}
	}

	//��ȡ�����ں���
	FuncModuleEntrancePtr pfModuleEntrance = NULL;

#ifdef WINCE
	pfModuleEntrance = (FuncModuleEntrancePtr)GET_PROC(hModule, TEXT("MODULE_ENTRANCE_FUNC"));
#else
	pfModuleEntrance = (FuncModuleEntrancePtr)GET_PROC(hModule, MODULE_ENTRANCE_FUNC);
#endif

	if (! pfModuleEntrance)
	{
		FREE_MODULE(hModule);
		return NULL;
	}

	//��ȡ�����Ϣ���ƽṹ
	IModuleBasePtr pModule = NULL;

	pModule = pfModuleEntrance();
	if (!pModule)
	{
		FREE_MODULE(hModule);
		return NULL;
	}

	if ((iModuleType != MODULE_TYPE_ANY) && (pModule->GetModuleInfo().iType != iModuleType))
	{
		FREE_MODULE(hModule);
		return NULL;
	}

	//���ݲ������ж����ǰ���ص�ͬ�����
	UnLoad(pModule->GetModuleInfo().strName);

	//��ʼ������ڲ�����Դ
	pModule->Init();

	//���ò����·�� 
	pModule->GetModuleInfo().strPath = strDllPath;
	
	//�������������Ϣ���ƽṹ�б����� 
	m_mapModuleList[pModule] = hModule;
	
	//���ز�����ƽṹָ�� 
	return pModule;
}

/************************************************************************************************************/
/*funtion:���ݲ������ж�ز��																				*/
/*parameter:strModuleName Ҫж�صĲ��������																*/
/*return: �ɹ���TRUE,ʧ���򷵻�FALSE																		*/
/*author:hwh																								*/
/*time:2010.5.17-09:24																						*/
/************************************************************************************************************/
BOOL CModuleManager::UnLoad(const std::string strModuleName)
{
	MapModuleList::iterator iter;
	IModuleBasePtr pModule = NULL;
	HMODULE hModule = NULL;

	//���Ҳ��
	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		pModule = iter->first;

		if(strModuleName == pModule->GetModuleInfo().strName)
		{
			//�ͷŲ���ڲ�����Դ
			pModule->UnInit();

			delete pModule;
			pModule = NULL;

			//ж�ظò�� 
			hModule = iter->second;

			FREE_MODULE(hModule);

			//�Ӳ���б����Ƴ��ò��
			m_mapModuleList.erase(iter);

			return TRUE;
		}
	}

	return FALSE;
}

/************************************************************************************************************/
/*funtion:ж��ȫ�����																						*/
/*parameter:��																								*/
/*return: �ɹ���TRUE,ʧ���򷵻�FALSE																		*/
/*author:hwh																								*/
/*time:2010.5.17-09:34																						*/
/************************************************************************************************************/
BOOL CModuleManager::UnloadAll()
{
	MapModuleList::iterator iter;
	IModuleBasePtr pModule = NULL;
	HMODULE hModule = NULL;

	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		pModule = iter->first;
		hModule = iter->second;

		//�ͷŲ���ڲ�����Դ
		pModule->UnInit();

		//delete pModule;
		//pModule = NULL;

		////ж�ظò�� 
		//FREE_MODULE(hModule);
	}

	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		pModule = iter->first;
		hModule = iter->second;

		////�ͷŲ���ڲ�����Դ
		//pModule->UnInit();

		delete pModule;
		pModule = NULL;

		//ж�ظò�� 
		FREE_MODULE(hModule);
	}


	m_mapModuleList.clear();

	return TRUE;
}

/************************************************************************************************************/
/*funtion:���ݲ�����ƻ�ȡ�����Ϣ���ƽṹ																	*/
/*parameter:strDllName Ҫ��ȡ������ƽṹ�Ĳ��������														*/
/*return: �ɹ��򷵻���Ҫ�Ĳ�����ƽṹ��ָ��,ʧ���򷵻�NULL													*/
/*author:hwh																								*/
/*time:2010.5.17-09:45																						*/
/************************************************************************************************************/
IModuleBasePtr CModuleManager::GetModule(const std::string strDllName)
{
	MapModuleList::iterator iter;
	IModuleBasePtr pModule = NULL;

	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		pModule = iter->first;
		if(strDllName == pModule->GetModuleInfo().strName)
		{
			return pModule;
		}
	}

	return NULL;
}

/************************************************************************************************************/
/*funtion:��ȡ������ƽṹmap,������map�м��صĲ���ĸ���													*/
/*parameter:ModuleStruInfoMap ���ڱ����Ĳ����Ϣ���ƽṹmao												*/
/*return: �ɹ��򷵻�map�м��صĲ���ĸ���																	*/
/*author:hwh																								*/
/*time:2010.5.17-10:52																						*/
/************************************************************************************************************/
INT32  CModuleManager::GetModuleList(VectModuleList& vectModuleList)
{
	INT32 iRet = 0;
	MapModuleList::iterator iter;

	for(iter = m_mapModuleList.begin(); iter != m_mapModuleList.end(); iter ++)
	{
		vectModuleList.push_back(iter->first);
		
		iRet++;
	}

	return iRet;
}

