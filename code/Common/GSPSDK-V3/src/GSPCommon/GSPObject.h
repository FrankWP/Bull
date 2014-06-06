#ifndef GSP_GSPOBJECT_DEF_H
#define GSP_GSPOBJECT_DEF_H
#include "GSPConfig.h"
#include "GSPErrno.h"


/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : GSPOBJECT.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/5/14 14:17
Description: ��GSP SDK ��ʹ�õĹ��������࣬ �����඼�Ǹ��������
********************************************
*/





/*
*********************************************
ClassName :  CGSPObject
DateTime : 2010/5/28 9:06
Description :   GSP Э��ջ�õ��Ļ�����
Note :
*********************************************
*/

namespace GSP
{




	class CGSPObject : public CGSObject
	{
	public:
		static BOOL g_bModuleRunning;
		static CGSMutex g_csMutex;
		CGSPObject(void);
		virtual ~CGSPObject(void);

		void *operator new(size_t iSize);
		void operator delete(void *pBuffer);

	};




	class CRefObject :
		public CGSPObject
	{


	private :
		GSAtomicInter m_iRef; //���ü���        
	protected :
		CRefObject()
			: CGSPObject()            
			,m_iRef(1)
		{

		}    
		virtual ~CRefObject(void)
		{

		}
	public :
#if defined(DEBUG) || defined(_DEBUG)
		INT64 m_iiTest;
#endif
		/*
		*********************************************
		Function : GetRefCounts
		DateTime : 2010/8/17 14:21
		Description : ��ȡ�������õĴ���
		Input :
		Output :
		Return :  ����Ӧ�õĴ���
		Note :
		*********************************************
		*/
		virtual UINT GetRefCounts(void) const
		{
			return m_iRef;
		}

		/*
		*********************************************
		Function :  RefObject
		DateTime : 2010/8/17 14:23
		Description :  ���ö���
		Input :
		Output :
		Return :  ����������
		Note :   ���øú��������� OnRefObjectEvent 
		*********************************************
		*/
		virtual void RefObject(void)
		{

			//����������ô����<= 1 ???
			GS_ASSERT_EXIT(AtomicInterInc(m_iRef)>1, -1);
			OnRefObjectEvent();
		}


		/*
		*********************************************
		Function :  UnrefObject
		DateTime : 2010/8/17 14:25
		Description : ���ٶ�������
		Input :
		Output :
		Return :
		Note :  ������û�б�������ʱ����䱻�ͷ�
		���øú��������� OnUnrefObjectEvent 
		*********************************************
		*/
		virtual void UnrefObject(void)
		{
			OnUnrefObjectEvent();
			INT iRef = AtomicInterDec(m_iRef);           
			GS_ASSERT_EXIT(iRef>-1, -1); //���ָ��� ��������
			if( iRef<1 ) 
			{
				delete this;       
			}
		}




		/*
		*********************************************
		Function :  Clone
		DateTime : 2010/8/17 14:25
		Description : ���ض���Ŀ���,���е����ݽ�������
		Input :
		Output :
		Return : �����µĶ���
		Note :
		*********************************************
		*/
		virtual CRefObject *Clone(void) const
		{
			return NULL;
		}
	protected :
		virtual void OnRefObjectEvent(void)
		{

		}

		virtual void OnUnrefObjectEvent(void)
		{

		}
	private :
		CRefObject(CRefObject &csDest);
		CRefObject &operator=(CRefObject &csDest);


	};

#if defined(DEBUG) || defined(_DEBUG)

#define REF_OBJ_SET_TEST_TICKS( pObj ) do{ pObj->m_iiTest = DoGetTickCount(); }while(0)
#define REF_OBJ_GET_TEST_TICKS( pObj)  ((pObj)->m_iiTest)

#else
#define REF_OBJ_SET_TEST( pObj ) do{}while(0)
#define REF_OBJ_GET_TEST_TICKS( pObj)  (0xFFFFFFFFLL)
#endif


#define SAFE_DESTROY_REFOBJECT(ppObj) \
	do{ if( *(ppObj) ){ (*(ppObj))->UnrefObject(); *(ppObj)=NULL ; } } while(0)

#define SAFE_DELETE_OBJECT(ppObj) \
	do{ if( *(ppObj) ){  delete (*(ppObj)); *(ppObj)=NULL ; } } while(0)

};


#define CLASS_NO_COPY(classname ) \
	private : \
		classname( const classname &csDest ); \
		classname &operator=( const classname &csDest );

#endif
