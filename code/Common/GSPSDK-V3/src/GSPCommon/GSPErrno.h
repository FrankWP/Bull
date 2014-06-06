/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : MYSOCKET.H
Author :  zouyx
Version : 0.0.0.0
Date: 2012/3/13 16:40
Description: ����Ŷ���
********************************************
*/

#ifndef _GS_H_MYSOCKET_H_
#define _GS_H_MYSOCKET_H_

namespace GSP
{

	typedef enum
	{
		eERRNO_SUCCESS = 0,
		eERRNO_EUNKNOWN,  //δ֪����
		eERRNO_EFATAL, //�����޸��Ĵ���
		eERRNO_ENONE, 

		//===================================================

		eERRNO_SYS_EINVALID = 0x0100,  //��Ч����
		eERRNO_SYS_EFLOWOUT,         //�������       
		eERRNO_SYS_ENMEM,            //û���ڴ�
		eERRNO_SYS_EOPER,           //��֧�ֵĲ���
		eERRNO_SYS_ESTATUS,         //����״̬�µĲ��� 
		eERRNO_SYS_ENCTRL,          //��֧�ֵĿ��Ʋ���
		eERRNO_SYS_ETIMEOUT,        //��ʱ
		eERRNO_SYS_EKPL,            // keepalive ��ʱ
		eERRNO_SYS_EBUSY,           //��Դȱ��
		eERRNO_SYS_ETKP,            //�������ʧ��
		eERRNO_SYS_EFUNC,           //�ù���û��ʵ��
		eERRNO_SYS_EPRO,            //��֧�ֵ�Э��
		eERRNO_SYS_EPERMIT,			//û��Ȩ��
		eERRNO_SYS_ENEXIST,			//�����ڵĶ���
		eERRNO_SYS_EEXIST,			//�Ѿ����ڶ���
		eERRNO_SYS_ELOSE,			//��ʧ����  -- TODO
		eERRNO_SYS_ECRC,			//Ч�����  -- TODO
		eERRNO_SYS_ECODEID,			//������� -- TODO

		//===================================================

		eERRNO_NET_EUNK = 0x0200,            //δ֪�������������
		eERRNO_NET_EDISCNN,                //�Զ�����ر�
		eERRNO_NET_ECLOSED,                  //Socket �ر�
		eERRNO_NET_EREG,                    //�¼�ע��ʧ��
		eERRNO_NET_EWEVT,                    //����д�¼�ʧ��
		eERRNO_NET_EREVT,                    //������¼�ʧ��
		eERRNO_NET_EEVT,                    //������¼�ʧ��
		eERRNO_NET_EBIN,                    //�󶨶˿�ʧ��
		eERRNO_NET_EOPT,                    //SOCKET ���Բ���ʧ��
		eERRNO_NET_ESEND,                  //д����
		eERRNO_NET_ERCV,                   //������
		eERRNO_NET_ESOCKET,                 //����SOCKET ʧ��

		eERRNO_NET_END,

		//===================================================

		eERRNO_SRC_ENXIST = 0x0400,       //�����ڵ�����Դ
		eERRNO_SRC_EUNUSED,             //����ʹ��
		eERRNO_SRC_ECLOSE,				//����Դ���ر�
		eERRNO_SRC_EASSERT,				//����Դ�쳣
		eERRNO_SRC_ECODER,				//��֧�ֵı��뷽ʽ

		//===================================================

		eERRNO_CLI_ECLOSE = 0x0600,       //�Զ�����ر�
		eERRNO_CLI_EASSERT,             //�Զ��쳣�ر� 
		eERRNO_CLI_ENSRC,               //����������Դ

		//================================================
		eERRNO_SRV_REFUSE = 0x0800,  //�ϲ�ܾ�����

		//===================================================
		eERRNO_EVT_CONTINUE_NEXT = 0x0a00, //������һ������
	}EnumErrno;

#define IS_NETWORK_ERRNO( xErrno) ((INT)(xErrno)>=(INT)eERRNO_NET_EUNK && (INT)(xErrno)<(INT)eERRNO_NET_END)


	const char *GetGSPError( EnumErrno eErrno );


} //end namespace GSP

#endif //end _GS_H_MYSOCKET_H_