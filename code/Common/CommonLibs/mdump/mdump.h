/*************************************************
  Copyright (C), 2011-2012, GOSUN 
  File name   : mdump.H      
  Author      : HongShuGuang
  Version     : V1.0.0        
  DateTime    : 2012/5/18
  Description : �����쳣�����ɼ�¼dump���������
*************************************************/

#ifndef _GOSUNCN_MDUMP_H_
#define _GOSUNCN_MDUMP_H_


//����UnhandledExceptionFilter,���������������Ӧdump�ļ�������ڳ���һ��ʼ�ĵط����á�
//IsDumpWithFullMemory�Ƿ����������ڴ��dump�ļ�(dump���ܶ�)��true��������dump��false����normal��dump
//IsPreventOtherExceptionHandling������ó�true�����������ط���SetUnhandledExceptionFilter�ĵ���
void MiniDump(bool IsDumpWithFullMemory = true,
			  bool IsPreventOtherExceptionHandling = true);


//IsDumpWithFullMemory�Ƿ����������ڴ��dump�ļ�(dump���ܶ�)��true��������dump��false����normal��dump
bool getDump(bool IsDumpWithFullMemory = true);





#endif