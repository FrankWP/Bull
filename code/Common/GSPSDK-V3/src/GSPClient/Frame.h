/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : FRAME.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/30 9:37
Description: ����V1 �Ľӿ�ʹ�õĶ���
********************************************
*/

#ifndef _GS_H_FRAME_H_
#define _GS_H_FRAME_H_

#include "GSPConfig.h"

namespace GSP
{

class CFrameCache;
class CGSPBuffer;

namespace CPAT //����
{

class CMyCpatFrame;

class  CRefBuffer
{
public :
	friend class CMyCpatFrame;

	
private :
	GSP::CGSPBuffer *m_pData;
public :
	virtual ~CRefBuffer(void);
	CRefBuffer(void);

	/*
	 *********************************************
	 Function : GetDataSize
	 DateTime : 2012/4/24 10:18
	 Description :  �������ݳ���
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	UINT GetDataSize(void) const;

	/*
	 *********************************************
	 Function : GetData
	 DateTime : 2012/4/24 10:19
	 Description :  ��������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	void *GetData(void) const;
	CRefBuffer &operator=(const CRefBuffer &csDest);	
protected :		
	void SetData(GSP::CGSPBuffer *pData);
	CRefBuffer(const CRefBuffer &csDest);
	
};


class CFrame
{
public :
	EnumGSMediaType m_eMediaType; //ý������
	INT m_iChnID;
public:
	/*
	 *********************************************
	 Function : GetFrameSize
	 DateTime : 2012/4/24 10:19
	 Description :  ��������֡�Ĵ�С
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	virtual UINT GetFrameSize(void) = 0;

	/*
	 *********************************************
	 Function : GetFrameData
	 DateTime : 2012/4/24 10:20
	 Description :  ��ȡ����֡���ڴ�����
	 Input :  
	 Output : csBuffer ���صĻ���
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	virtual BOOL GetFrameData( CRefBuffer &csBuffer ) = 0;
protected :	
	virtual ~CFrame(void)
	{

	}
	CFrame(void)
	{
		m_eMediaType = GS_MEDIA_TYPE_NONE;
		m_iChnID = -1;
	}
	
	CFrame(const CFrame &csDest);
	CFrame &operator=(const CFrame &csDest);
};


/*
 *********************************************
 Function : CMyCpatFrame
 DateTime : 2012/4/24 10:20
 Description :  CFrame �ľ���ʵ��
 Input :  
 Output : 
 Return : 
 Note :   
 *********************************************
 */
class CMyCpatFrame : public CFrame
{
private :
	GSP::CGSPBuffer *m_pMerge;
public:
	virtual UINT GetFrameSize(void);
	virtual BOOL GetFrameData( CRefBuffer &csBuffer );
	virtual ~CMyCpatFrame(void);
	CMyCpatFrame(void);
	BOOL Set(GSP::CFrameCache *pFrame);
};





} //end namespace CPAT


} //end namespace GSP

#ifndef GSPCPAT
using namespace GSP::CPAT;
#endif


#endif //end _GS_H_FRAME_H_
