/*
******************************************
Copyright (C), 2011-2012, GOSUN
Filename : SIPBASE.H
Author :  ������
Version : 1.0.0.1
Date: 2012/8/16 16:46
Description: SIPЭ��������ײ�ʹ��exosip������;	
			ҲӦ�ý���һ���洢��
*			�У���heart�в���ȡ�����ݽ����ڲ�����
********************************************
*/

#ifndef _GS_H_SIPBASE_H_
#define _GS_H_SIPBASE_H_

namespace GSP
{


#include <eXosip2/eXosip.h>
#include <stdio.h>
#include <stdlib.h>
#include "SIPCommonDef.h"



#define ALLOW_HEAD		"INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO"
#define SUPPORTED_HEAD	"eventlist"


class ISIPListener : public CGSObject
{
public:
	virtual int OnClientConnectEvent(int iEngineId, int iClientId, const CGSString &strIP, int port) = 0;

	virtual int OnClientDisconnectEvent(int iEngineId, int iClientId) = 0;

	//���ϲ�Ļص����ô˽ӿ� ����������Ļص�����Ӧ��ص����� packet���;���
	virtual int OnSIPPacketEvent(int iEngineId, int iConn, CSIPPacket* pdu) = 0;
};

class CSIPBase  : public CGSObject
{

protected:
	/*!
	* \brief
	* �ϲ���������������ɹ�������,���ȴ�������һ�˵�Ӧ��;
	*
	* \remarks
	* ��ֵΪexosip�е�����ı�־ֵcallId������ǻỰ����������Cseq��
	*/
	CSentPacketMap	m_mapSendQueue; //�ȴ����͵����ݶ���
	CGSMutex m_csSendQueueMutex; // m_mapSendQueue ��


	/*!
	* \brief
	* �ϲ�ص�ָ��
	*/
	ISIPListener* _listener;

	int			m_id;
	EnumConnectType		eConnectType;

public:

	CSIPBase(int iID, EnumConnectType eCnnType, ISIPListener* pListener);
	virtual ~CSIPBase(void);

	virtual EnumErrorCode Connect( const char* czHost, int iPort) = 0;
	virtual EnumErrorCode Connect(const char* czLocalIp, int iLocalPort, 
								  const char* czRemoteIp, int iRemotePort) = 0;
	virtual EnumErrorCode Start( const char* czIp, int port) = 0;

	//sand���ֱ�ӷ���;�����ݷ�cseq�洢packet;�ϲ����������sequence������������
	//heartbeat���ܵ�Ӧ�𣬸���cseq�ҵ� �����packet;����Ӧ��packetӦ��APP
	virtual EnumErrorCode SendPacket(int iConn, CSIPPacket* pPacket) = 0;
	virtual EnumErrorCode Heartbeat( int iTimeout = 0);
	virtual EnumErrorCode Close(void);

protected:
	virtual EnumErrorCode CheckNetReqConnSession(const eXosip_event_t* pEXOEvt,
													OUT UINT32 &iConnId ) = 0;
	virtual EnumErrorCode CheckNetResConnSession(const eXosip_event_t* pEXOEvt,
													OUT UINT32 &iConnId ) = 0;

public:
	EnumErrorCode	RemoveSentPdu(const CGSString &strSequence);
	static EnumErrorCode ParseIpAndPortFromAddress( const char* czAddress,OUT CGSString &strIp,OUT int & port);

protected:
	/**
	*@brief ����SIP����������͸��ݺ���������
	*@param conn	��Чid
	*@param	pdu		sip�������
	*/
	int SendInvite(int iConn, CSIPPacket* pPacket);	
	int SendInfo(int iConn, CSIPPacket* pPacket);
	int SendNotify(int iConn, CSIPPacket* pPacket);
	int SendSubscribe(int iConn, CSIPPacket* pPacket);//
	int SendBye(int iConn, CSIPPacket* pPacket);//Ӧ�ò���Ҫ�ٴ���Ӧ����
	int SendCancel(int iConn, CSIPPacket* pPacket);

	/**
	*@brief	����Ĭ�ϵ�sip�����������ĳ���Ự
	*@note	����������ķ�����������������Ҫ�Ƿ��ͻỰ�������������Invite��Info��Notify����.
	*@param conn	��ʱ��Ч
	*@param pdu		sip�����
	*/
	int SendRequestEx(int iConn, CSIPPacket* pdu);

	/**
	*@brief	����Ĭ�ϵ�sip�����Ӧ��
	*@see	sendRequestEx
	*@note	����������ķ�����������������Ҫ�Ƿ��ͻỰ�������������Invite��Info��Notify����.
	*@param conn	��ʱ��Ч
	*@param pdu		sip�����
	*/
	int SendRequestExRes(int conn, CSIPPacket* pdu);

	/**
	*@brief ����Ӧ��
	*@param conn	��Чid
	*@param	pdu		sip�������
	*/
	int SendInviteRes(int conn, CSIPPacket* pdu);	
	int SendInfoRes(int conn, CSIPPacket* pdu);
	int SendNotifyRes(int conn, CSIPPacket* pdu);
	int SendSubscribeRes(int conn, CSIPPacket* pdu);

	//�ص� ����������� ����
	int BuildAndSendRes(eXosip_event_t* envent, osip_body_t* body, int bSuc = 1);	
	int BuildAndSendReq(eXosip_event_t* envent, osip_body_t* body, int sipType);

	int FetchAddreInfoFromReq(eXosip_event_t* envent,CSIPPacket* sipReq);
	int FetchAddreInfoFromRes(eXosip_event_t* envent,CSIPPacket* sipRes);

	int FetchAuthorization(eXosip_event_t* pEvent,CSIPPacket* sipReq);
	int BuildAndSendRegReq(eXosip_event_t* envent);
	int BuildAndSendRegRes(eXosip_event_t* pEvent);
	int FetchWWWAuthenticateFromRes(eXosip_event_t* envent,CSIPPacket* sipRes);

	int AddSentPdu(std::string sequence, CSIPPacket* packet );
	int DelSentPdu(std::string  sequence );
	CSIPPacket* FetchSentPdu(std::string sequence );
	CSIPPacket* GetSentPdu(std::string sequence );
	int ClearSentPduMap();

	void AddAllowHeadToSipMsg(osip_message *pSipMsg, const char *pContent = ALLOW_HEAD);
	void AddSupportedHeadToSipMsg(osip_message *pSipMsg, const char *pContent = SUPPORTED_HEAD);



};



} //end namespace GSP

#endif //end _GS_H_SIPBASE_H_
