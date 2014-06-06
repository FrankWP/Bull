/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : JOUXML.H
Author :  zouyx
Version : 0.0.0.0
Date: 2011/9/20 14:01
Description: �Ѳ�ѯ���ݺͽ����֮���ת��
********************************************
*/

#ifndef _GS_H_JOUXML_H_
#define _GS_H_JOUXML_H_
#include "GSCommon.h"
#include <vector>
#include <list>


/*
*********************************************************************
*
*@brief : XML ��ʽ˵��, ��Сд����
*

</Version GSML1.0>\r\n
\r\n
<FieldName>\r\n
 </Filed �ֶ���>\r\n
		.
		.
		.
 </Filed �ֶ���>\r\n 
</FieldName>\r\n
\r\n
<Data>\r\n
<Row></Value ֵ> ..</Value ֵ></Row>\r\n
		.
		.
		.
<Row></Value ֵ> ..</Value ֵ></Row>\r\n
</Data>\r\n
\r\n



*********************************************************************
*/


namespace JOUXML
{
	typedef std::vector<CGSString> CXmlRow;
	typedef std::list<CXmlRow> CXmlTable;
	typedef std::list<CXmlRow>::iterator CXmlTableIterator;

	
	/*
	********************************************************************
	��ע��
	����    :    CJouXmlMaker
	����    :    zouyx
	����ʱ��:    2011/9/20 14:13
	������  :		�ϳ�XML
	*********************************************************************
	*/
	
class CJouXmlMaker
{
public:
	CJouXmlMaker(void);
	~CJouXmlMaker(void);
	
	/*
	 *********************************************
	 Function : Init
	 DateTime : 2011/9/20 14:05
	 Description :  ��ʼ��
	 Input :  vColName ����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL Init(const std::vector<CGSString> &vColName );

	/*
	 *********************************************
	 Function : AddRow
	 DateTime : 2011/9/20 14:06
	 Description :  ���һ������
	 Input :  
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	BOOL AddRow(void);

	/*
	 *********************************************
	 Function : PutValue
	 DateTime : 2011/9/20 14:07
	 Description :  д������
	 Input :  iColIndex �к�
	 Input :  strValue ��ӵ�����
	 Output : 
	 Return : TRUE/FALSE
	 Note :   
	 *********************************************
	 */
	BOOL PutRowValue( INT iColIndex,const CGSString &strValue );


	/*
	 *********************************************
	 Function : SerialToXml
	 DateTime : 2011/9/20 14:09
	 Description :  ת��ΪXML�ı�
	 Input :  oStrValue ���ص�����
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL SerialToXml( CGSString &oStrValue );
private :

	CXmlRow m_vColName; //����
	CXmlTable m_csTables;
	INT m_iCols; //����
};



/*
********************************************************************
��ע��
����    :    CJouXmlParser
����    :    zouyx
����ʱ��:    2011/9/20 14:13
������  :    �����ֽ�XML
*********************************************************************
*/

class CJouXmlParser
{
public:
	CJouXmlParser(void);
	~CJouXmlParser(void);

	BOOL Init(const char *czXML, INT iXMLSize );

	BOOL GetColName( std::vector<CGSString> &vColName );

	INT32 GetRows(void);

	BOOL IsEof(void);

	BOOL MoveFirst(void);

	void Next(void);

	/*
	 *********************************************
	 Function : GetValue
	 DateTime : 2011/9/20 14:15
	 Description :  ��ȡ��ǰ�е�������
	 Input :  
	 Output : 
	 Return : 
	 Note :   
	 *********************************************
	 */
	BOOL GetValue(INT iColIndex, CGSString &oStrValue );
private :
	CXmlRow m_vColName; //����
	CXmlTable m_csTables;
	INT m_iCols; //����
	CXmlTableIterator m_csCur; //�α�

};

} //end namespace GSS

#endif //end _GS_H_JOUXML_H_