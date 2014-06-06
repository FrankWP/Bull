#ifndef IMEMCOMM_H_
#define IMEMCOMM_H_

#include "GSCommon.h"
#include "GSType.h"
#include "GSDefine.h"

#include <string>
using namespace std;

#define ERROR_MEMCOMM_OPER_SUCCESS			ERROR_BASE_SUCCESS
#define ERROR_MEMCOMM_START					(ERROR_BASE_START+5000)
#define ERROR_MEMCOMM_OPER_FAILED			(ERROR_MEMCOMM_START + 1)
#define ERROR_MEMCOMM_BUFFER_NOT_EXIST		(ERROR_MEMCOMM_START + 2)//������������
#define ERROR_MEMCOMM_BUFFERID_NOT_FIT		(ERROR_MEMCOMM_START + 3)//������ID����
#define ERROR_MEMCOMM_BUFFER_NOT_ENOUGH		(ERROR_MEMCOMM_START + 4)//����������
#define ERROR_MEMCOMM_BUFFER_IS_EMPTY		(ERROR_MEMCOMM_START + 5)//������Ϊ��
#define ERROR_MEMCOMM_BUFFER_NOT_LOCK		(ERROR_MEMCOMM_START + 6)//������û����

typedef enum
{
	MEM_COMM_OPEN,	// ֻ��
	MEM_COMM_CREATE,						// ֻ����
	MEM_COMM_OPEN_CREATE					// �ȳ��Դ򿪣�ʧ���򴴽�	
} EnumOperType;

class IMemComm
{
public:
	IMemComm(void){};
	virtual ~IMemComm(void){};
public:
	/*************************************************
	  Function:   Create 
	  DateTime:   2010-5-21 14:48:52   
	  Description: ������Ϣ������
	  Input:   dwSize   ��Ϣ��������С 
	           bLock    �Ƿ������TRUE��������FALSE��������
			   bLocal	�Ƿ񱾵���Ϣ��������TRUE��������FALSE��������
			   opType	������Ϣ��������ʽ��EnumOperType����
			   strName  ��Ϣ�����������ƣ�bLocalΪFALSEʱ����Ч
	  Output:  ��
	  Return:  TRUE�������ɹ���FALSE������ʧ��
	  Note:      
	*************************************************/
	virtual BOOL Create(UINT32 dwSize, BOOL bLock, BOOL bLocal,EnumOperType opType, string &strName) = 0;

	/*************************************************
	  Function:   Reset
	  DateTime:   2010-5-21 14:49:01   
	  Description: ������Ϣ������
	  Input:   ��     
	             
	  Output:  ��
	  Return:  �� 
	  Note:      
	*************************************************/
	virtual void Reset(void) = 0;

	/*************************************************
	  Function:   Free
	  DateTime:   2010-5-21 14:49:09   
	  Description: �ͷ���Ϣ��������Դ
	  Input:  ��     
	             
	  Output: TRUE���ͷ���Ϣ��������Դ�ɹ���FALSE���ͷ���Ϣ��������Դʧ��   
	  Return: bLocal
	  Note:      
	*************************************************/
	virtual BOOL Free(void) = 0;

	/*************************************************
	  Function:   Read
	  DateTime:   2010-5-21 14:49:16   
	  Description: ����Ϣ�������ж�ȡһ����Ϣ
	  Input:    pBuf			�������ȡ��Ϣ�Ŀռ��ַ
				dwMaxLen		�������ȡ��Ϣ�Ŀռ��С
				dwMilliSeconds  ��ȡ��ʱʱ�䣬�������
	  Output:   pBuf			����Ϣ���������ȡ����Ϣ  
				pRealLen		����ȡ��Ϣ�ĳ���
	  Return:   ERROR_MEMCOMM_OPER_SUCCESS			�����ɹ�
				ERROR_MEMCOMM_BUFFER_NOT_EXIST		��Ϣ������������
				ERROR_MEMCOMM_BUFFERID_NOT_FIT		��Ϣ������ID����ȷ
				ERROR_MEMCOMM_BUFFER_IS_EMPTY       ��Ϣ������������
				ERROR_MEMCOMM_BUFFER_NOT_ENOUGH     ��������С���޷�������Ϣ
	  Note:      
	*************************************************/
	virtual INT32 Read(void *pBuf, UINT32 dwMaxLen, UINT32 *pRealLen, UINT32 dwMilliSeconds) = 0;
	
	/*************************************************
	  Function:   Write
	  DateTime:   2010-5-21 14:49:33   
	  Description: ����Ϣ��������β��д��һ����Ϣ
	  Input:   pBuf				��д��Ϣ��ַ  
	           dwLen			��д��Ϣ����
			   dwMilliSeconds   д��ʱʱ�䣬�������
	  Output:    
	  Return:  ERROR_MEMCOMM_OPER_SUCCESS		�����ɹ�
			   ERROR_MEMCOMM_BUFFER_NOT_EXIST   ��Ϣ������������
			   ERROR_MEMCOMM_BUFFERID_NOT_FIT   ��Ϣ������ID����ȷ
			   ERROR_MEMCOMM_BUFFER_NOT_ENOUGH  ��Ϣ��������С
	  Note:      
	*************************************************/
	virtual INT32 Write(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds) = 0;

	/*************************************************
	  Function:   WriteUrgent
	  DateTime:   2010-5-21 14:49:42   
	  Description: ����Ϣ��������ͷ��д��һ����Ϣ
	  Input:  pBuf				��д��Ϣ��ַ
			  dwLen				��д��Ϣ����
			  dwMilliSeconds    д��ʱʱ�䣬�������
	             
	  Output: �� 
	  Return: ERROR_MEMCOMM_OPER_SUCCESS		�����ɹ�
			  ERROR_MEMCOMM_BUFFER_NOT_EXIST	��Ϣ������������
			  ERROR_MEMCOMM_BUFFERID_NOT_FIT	��Ϣ������ID����ȷ
			  ERROR_MEMCOMM_BUFFER_NOT_LOCK		��Ϣ������û����
			  ERROR_MEMCOMM_BUFFER_NOT_ENOUGH	��Ϣ��������С
	  Note:      
	*************************************************/
	virtual INT32 WriteUrgent(void *pBuf, UINT32 dwLen, UINT32 dwMilliSeconds) = 0;

};
extern IMemComm *GetMemInstance();
extern void ClearMemInstance(IMemComm *pMemComm);
#endif


