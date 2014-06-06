#ifndef _IMODULE_BASE_H
#define _IMODULE_BASE_H

#include "GSType.h"

#if !defined(_WIN32) && !defined(_LINUX)
#error This program only support Windows, Linux...
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
#define MODULE_API extern "C" __declspec(dllexport)
#else /* _LINUX */
#define MODULE_API extern "C"
#endif

//�����Ϣ
typedef struct _Module_Info
{
	INT32 iType;			//�������
	INT32 iVersion;			//����屾
	std::string strName;	//�������
	std::string strPath;	//���·��

	BOOL bUsing;			//��¼�ò����ʹ��״̬
	
	//��ʼ�������Ϣ
	_Module_Info() : iType(0), iVersion(0), strName(""), strPath(""), bUsing(FALSE)
	{
	}
} StruModuleInfo, *StruModuleInfoPtr;

class IModuleBase
{
public:

	//ȱʡ���캯��
	IModuleBase(void) {};

	//ȱʡ��������
	virtual ~IModuleBase(void) {};

	//��ȡģ����Ϣ
	virtual StruModuleInfo& GetModuleInfo()
	{
		return m_ModuleInfo;
	};

	//��ʼ������ڲ�����Դ
	virtual void Init(void) {};			

	//�ͷŲ���ڲ�����Դ
	virtual void UnInit(void) {};

protected:

	StruModuleInfo m_ModuleInfo;
};

typedef IModuleBase* IModuleBasePtr;

typedef IModuleBasePtr (*FuncModuleEntrancePtr)();	//�����ں��������Ͷ���

//���������������
#define MODULE_ENTRANCE_FUNC "ModuleEntrance"

//����������ں���
#define DECLARE_MODULE_ENTRANCE_FUNC() \
MODULE_API IModuleBasePtr ModuleEntrance()

//ʵ�ֲ������ں���
#define IMPLEMENT_MODULE_ENTRANCE_FUNC(module_class_name, type, ver, name) \
MODULE_API IModuleBasePtr ModuleEntrance(void) \
{ \
	module_class_name * pcsModule; \
 \
    pcsModule = new module_class_name(); \
 \
	pcsModule->GetModuleInfo().iType = type; \
	pcsModule->GetModuleInfo().iVersion = ver; \
	pcsModule->GetModuleInfo().strName = name; \
	pcsModule->GetModuleInfo().strPath = ""; \
	pcsModule->GetModuleInfo().bUsing = FALSE; \
 \
	return pcsModule; \
}

typedef enum
{
	MODULE_TYPE_SDK = 1,			//SDK���
	MODULE_TYPE_STREAMING_PROTOCOL,	//��ת��Э����

	MODULE_TYPE_TEST_ADD = 1001,	//�����õ�ADD���
	MODULE_TYPE_TEST_SUB,			//�����õ�SUB���

	MODULE_TYPE_ANY = -1,			//��������
} EnumModuleType;

#endif		//_IMODULE_BASE_H
