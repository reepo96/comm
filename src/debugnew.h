/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：debugnew.h
* 文件标识：
* 摘    要：重载操作符new、delete，在调试状态下，还记录内存分配信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年02月4日
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
