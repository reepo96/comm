/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�debugnew.h
* �ļ���ʶ��
* ժ    Ҫ�����ز�����new��delete���ڵ���״̬�£�����¼�ڴ������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��02��4��
*
**************************************************************************/

#if !defined(AFX_DEBUGNEW_H__20A7D468_1132_46E1_967A_B5982F4C5297__INCLUDED_)
#define AFX_DEBUGNEW_H__20A7D468_1132_46E1_967A_B5982F4C5297__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4291 4786)

#include <stdio.h>
#include <stdlib.h>

#include <map>
using namespace std;

void* operator new[](unsigned int size);

void* operator new[](unsigned int size,const char *pFileName,int iLine);

void operator delete[](void *p);

void* operator new(unsigned int size);

void* operator new(unsigned int size,const char *pFileName,int iLine);

void operator delete(void *p);

class DebugNewTracer
{
private:
	typedef struct tagMyNewInfo
	{
		unsigned int	memAddr;
		unsigned int	memLen;
		char			fileName[300];
		int				fileLine;
	}MYNEWINFO,*PMYNEWINFO;

	map<unsigned int,MYNEWINFO>	m_MyMallocInfo;
	unsigned int			m_TotalSize;

public:
	 DebugNewTracer();
	 virtual ~DebugNewTracer();

	 void Add (void* p, const char* file, int line, unsigned int size);
	 void Remove (void* p);
	 void Dump ();
};


#endif // !defined(AFX_DEBUGNEW_H__20A7D468_1132_46E1_967A_B5982F4C5297__INCLUDED_)
