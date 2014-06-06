#ifndef GSS_URI_DEF_H
#define GSS_URI_DEF_H




/*
******************************************
Copyright (C), 2010-2011, GOSUN
Filename : GSPURI.H
Author :  zouyx
Version : 0.1.0.0
Date: 2010/5/14 16:01
Description: GSP �� uri �ķ���

* scheme://host:port/path/filename?attrname&attrname1=attrvalue1

* scheme = ͨ��Э�� (���õ�http,ftp,maito ��)
* host = ���� (������IP)
* port = �˿ں�(��ѡ)
* path = ·��
* filename = ����
* attrname ������
* attrname1 ����1��
* attrvalue1 ����1��ֵ

* host:port ΪNetInfo
* path/filename ΪKey
********************************************
*/

#include <vector>
#include "GSPObject.h"
#include "IUri.h"

namespace GSP
{


    typedef std::vector<StruUriAttr>  CURIAttrList;

    class  CUri :
        public CGSPObject , public CIUri
    {
 
    protected :
         BOOL m_bValid;
        CGSPString m_strScheme;
        CGSPString m_strHost;
        UINT m_iPort;
        CGSPString m_strKey;
        CURIAttrList m_csAttrs;
        CGSPString m_strURI;


    public:
        CUri(void);
        CUri( const CUri &csDest);
        virtual ~CUri(void);

		virtual void Clear(void);

		virtual const char *GetScheme(void) const;
		virtual void  SetScheme(const char *szScheme);

		virtual const char *GetHost(void) const;
		virtual void  SetHost( const char *szHost );

		virtual UINT GetPort(void) const;
		virtual void SetPortArgs(UINT iPort);

		virtual const char *GetKey(void) const;
		virtual void SetKey(const char *szKey);


		virtual StruUriAttr *GetAttr(const char *szAttrName) const;

		virtual StruUriAttr *AttrBegin(void);

		virtual StruUriAttr *AttrNext(const  StruUriAttr *pAttr);


		virtual BOOL AddAttr( const char *szAttrName, const char *szAttrValue );
		virtual void ClearAttr( const char *szName); 


		virtual BOOL Analyse(const char *szURI );

		virtual const char *GetURI(void) const;

		virtual CIUri *Clone(void) const;

		CUri &operator=( const CUri &csUri );

	private :
		CURIAttrList::const_iterator FindAttr(const char *szAttrName ) const;
		StruUriAttr *AddAttrInner( const char *szAttrName, const char *szAttrValue );
    };


};

#endif
