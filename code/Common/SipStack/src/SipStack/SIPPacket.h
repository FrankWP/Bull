/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPPACKET.H
Author :  ������
Version : 1.0.0.1
Date: 2012/8/16 15:28
Description: 
********************************************
*/

#ifndef _GS_H_SIPPACKET_H_
#define _GS_H_SIPPACKET_H_
#include "SIPCommonDef.h"


namespace GSSIP
{

	
	/*
	********************************************************************
	��ע��
	����    :    CRefObject
	����    :    ������
	�汾    :	 1.0.0.1
	����ʱ��:    2012/8/16 16:26
	������  :   �������õ������ 
	*********************************************************************
	*/

	class CRefObject : public CGSObject
	{
	private :
		GSAtomicInter m_iRefs; //������
	public :
		virtual void Ref(void);
		virtual void Unref(void);

		//����������
		INLINE long GetRefCounts(void) const 
		{
			return m_iRefs;
		}
	protected :
		CRefObject(void) :
			  CGSObject(),
				  m_iRefs(1)
			  {

			  }

			  virtual ~CRefObject(void)
			  {

			  }
	};	



/*
********************************************************************
��ע��
����    :    CPacket
����    :    ������
�汾    :	 1.0.0.1
����ʱ��:    2012/8/16 16:26
������  :    SIP ���ݰ�������
*********************************************************************
*/

class CPacket : public CRefObject
{
public :
	// body ������
	typedef enum
	{
		eBODY_STRING = 0, //�ı���ʽ
		eBODY_BINARY, //��������
	}EnumBodyType;

	//�����
	typedef enum 
	{
		eDIR_REQUEST = 0, //����
		eDIR_RESPONSE = 1, //�ظ�
	}EnumDirect;
protected :

	EnumBodyType m_eBodyType;
	CGSString m_strBody;   //�ڲ�����ռ� APP��������ʱ��д
	BYTE *m_bBody;
	int  m_iBodyLength;   //_body����	   APP��������ʱ��д
	int m_iBodyBufSize; //

	CGSString	m_strFromCode;//from��@֮ǰ�ĵ�ַ���� APP��������ʱ��д
	CGSString	m_strFromAdr;	//from��@֮��ĵ�ַ     APP��������ʱ��д
	CGSString	m_strToCode;	//to��@֮ǰ�ĵ�ַ����   APP��������ʱ��д
	CGSString	m_strToAdr;	//to��@֮��ĵ�ַ       APP��������ʱ��д

	int		m_iCmdId;						//Э����� APP��������ʱ��д �յ�����Ӧ��ʱ���㴦�� 
										//��Ϊ��Щ�����Ӧ����û�а���Ĺ�����

	EnumSipMethod	m_eMethod;		//sip��������	SIP���յ����������Լ�APP����������ʱ��д
	int     m_iSubexpires;                //�澯Ԥ����ʱ��


	StruSipSessionInfo	m_stSipSessionInfo;	//sip���յ���������ʱ��д��
											//Ϊ���������session��Ϣ ���ڷ���Ӧ��

	int		m_iSequence;					//APP���͵�����˳��� 
											//���䷢������ʱ��д ���ڴ�������Ӧ��


	EnumDirect	m_eDirect;					//����

public:
	CPacket( EnumDirect eDirect, EnumBodyType eBodyType );
	virtual ~CPacket(void);

	INLINE EnumDirect	GetDirect(void) const
	{
		return m_eDirect;
	}

	INLINE int		GetSequence(void) const 
	{
		return m_iSequence;
	}

	INLINE void	SetSequence(int iSequence)
	{
		m_iSequence = iSequence;
	}

	const CGSString &GetStrBody(void) const
	{
		return m_strBody;
	}

	INLINE int GetBodyLength(void) const
	{
		if( m_eBodyType )
		{
			return m_iBodyLength;
		}
		return m_strBody.size();
	}

	const BYTE*	GetBinaryBody(void) const
	{
		return m_bBody;
	}

	BOOL	SetBody( const CGSString &strBody );

	BOOL   SetBody( BYTE *bBuf, int iSize);



	
};


/*
********************************************************************
��ע��
����    :    CRequest
����    :    ������
�汾    :	 1.0.0.1
����ʱ��:    2012/8/16 16:26
������  :		Request �������ݰ� ������
*********************************************************************
*/

class CRequest : public CPacket
{
public:
	CRequest( EnumBodyType eBodyType )
		:CPacket(CPacket::eDIR_REQUEST, eBodyType)
	{

	}
	virtual ~CRequest(void)
	{

	}
};

/*
********************************************************************
��ע��
����    :    CRequest
����    :    ������
�汾    :	 1.0.0.1
����ʱ��:    2012/8/16 16:26
������  :		Response �������ݰ� ������
*********************************************************************
*/
class CResponse : public CPacket
{
public:
	EnumSipResponseCode	 m_eResonseCode;
	int  m_iOSipEvtCode;			//osip event�е���Ϣ 
	CGSString m_strMessage;

public:
	CResponse( EnumBodyType eBodyType ):
	  CPacket(CPacket::eDIR_RESPONSE, eBodyType)
	{
		m_eResonseCode = eSIP_RESPONSE_OK;
		m_iOSipEvtCode = 1;
		m_strMessage = "Success";
	}
	virtual ~CResponse(void);
};



/*
********************************************************************
��ע��
����    :    CRegRequest
����    :    ������
�汾    :	 1.0.0.1
����ʱ��:    2012/8/31 10:12
������  :		ע������
*********************************************************************
*/

class CRegRequest:public CRequest
{
public:
	CGSString	m_strAllow;
	CGSString	m_strSupported;
	CGSString	m_strUserAgent;
	CGSString	m_strAuthorization;
	CGSString    m_strContact;

	CGSString	m_strSipUserName;
	CGSString    m_strSipPassword;
	CGSString    m_strWholeContact;
	int		m_iExpirse;

public:
	CRegRequest(void)
		:CRequest(CPacket::eBODY_STRING)
	{
		m_iExpirse = 0;
	}
	virtual ~CRegRequest(void)
	{

	}


};




/*
********************************************************************
��ע��
����    :    CRegResponse
����    :    ������
�汾    :	 1.0.0.1
����ʱ��:    2012/8/31 10:13
������  :		ע������ظ�
*********************************************************************
*/

class CRegResponse:public CResponse
{
public:
	CGSString	m_strWWWAuthenticate;
	int     m_iSipRid;
	CGSString    m_strWholeContact;
	int		m_iExpirse;
public:
	CRegResponse(void)
		:CResponse(CPacket::eBODY_STRING)
	{
		m_iSipRid = 0;
		m_iExpirse = 0;
	}

	virtual ~CRegResponse(void)
	{

	}


};


} //end namespace GSSIP

#endif //end _GS_H_SIPPACKET_H_
