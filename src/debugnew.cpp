/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：debugnew.cpp
* 文件标识：
* 摘    要：重载操作符new、delete，在调试状态下，还记录内存分配信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年02月4日
*
**************************************************************************/

#include "debugnew.h"
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include "LogControl.h"

//#define min(a,b)	((a)<(b))?(a):(b)

extern LogControl mylog;     //日志
extern LPCRITICAL_SECTION 	g_pSection4NewMem; //用于分配内存同步的临界区

#ifdef _DEBUG
	extern DebugNewTracer myNewTracer;
#endif

void* operator new[](unsigned int size)
{
	//解决双核系统分配内存可能失败的问题
	void *pObj = NULL;
	int	  iCount = 0;

	if(size <= 0)
		return NULL;
	
	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	while(iCount < 5)
	{
		iCount++;
		try
		{
			pObj = malloc(size);
			break;
		}
		catch(...)
		{
			pObj = NULL;
			mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when malloc memery");
			Sleep(100);
		}
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
	
	return pObj;
}

void* operator new[](unsigned int size,const char *pFileName,int iLine)
{
	//解决双核系统分配内存可能失败的问题
	void *pObj = NULL;
	int	  iCount = 0;

	if(size <= 0)
		return NULL;
	
	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	while(iCount < 5)
	{
		iCount++;
		try
		{
			pObj = malloc(size);

#ifdef _DEBUG
			myNewTracer.Add(pObj,pFileName,iLine,size);
#endif

			break;
		}
		catch(...)
		{
			pObj = NULL;
			mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when malloc memery");
			Sleep(100);
		}
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
	
	return pObj;
}

void operator delete[](void *p)
{
	if(p == NULL)
		return;

	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	try
	{	
#ifdef _DEBUG
		//清除记录信息
		myNewTracer.Remove(p);
#endif

		free(p);
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when free memery");
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
}

void* operator new(unsigned int size)
{
	//解决双核系统分配内存可能失败的问题
	void *pObj = NULL;
	int	  iCount = 0;

	if(size <= 0)
		return NULL;
	
	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	while(iCount < 5)
	{
		iCount++;
		try
		{
			pObj = malloc(size);
			break;
		}
		catch(...)
		{
			Sleep(100);
			pObj = NULL;
			mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when malloc memery");
		}
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
	
	return pObj;
}

void* operator new(unsigned int size,const char *pFileName,int iLine)
{
	//解决双核系统分配内存可能失败的问题
	void *pObj = NULL;
	int	  iCount = 0;

	if(size <= 0)
		return NULL;
	
	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	while(iCount < 5)
	{
		iCount++;
		try
		{
			pObj = malloc(size);

#ifdef _DEBUG	
			//记录内存分配信息
			myNewTracer.Add(pObj,pFileName,iLine,size);
#endif

			break;
		}
		catch(...)
		{
			Sleep(100);
			pObj = NULL;
			mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when malloc memery");
		}
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
	
	return pObj;
}

void operator delete(void *p)
{
	if(p == NULL)
		return;

	if(g_pSection4NewMem != NULL)
		EnterCriticalSection(g_pSection4NewMem);

	try
	{
#ifdef _DEBUG
		//清除记录信息
		myNewTracer.Remove(p);
#endif

		free(p);
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"Catch an exception when free memery");
	}

	if(g_pSection4NewMem != NULL)
		LeaveCriticalSection(g_pSection4NewMem);
}

DebugNewTracer::DebugNewTracer()
{
	m_TotalSize = 0;
}

DebugNewTracer::~DebugNewTracer()
{
	Dump();
}

void DebugNewTracer::Add (void* p, const char* file, int line, unsigned int size)
{
	try
	{
		//记录内存分配信息
		MYNEWINFO myNewInfo;
		memset(&myNewInfo,0,sizeof(myNewInfo));
		
		myNewInfo.memAddr = (unsigned int)p;
		myNewInfo.memLen = size;
		myNewInfo.fileLine = line;
		memcpy(myNewInfo.fileName,file,min(sizeof(myNewInfo.fileName)-1,strlen(file)));
		
		m_MyMallocInfo.insert(map<unsigned int,MYNEWINFO>::value_type((unsigned int)p,myNewInfo));

		m_TotalSize += size;
	}
	catch(...)
	{
	}
}

void DebugNewTracer::Remove(void* p)
{
	try
	{
		map<unsigned int,MYNEWINFO>::iterator it = m_MyMallocInfo.find((unsigned int)p);
		if(it != m_MyMallocInfo.end())
		{
			MYNEWINFO myNewInfo = (*it).second;
			m_TotalSize -= myNewInfo.memLen;
			m_MyMallocInfo.erase(it);
		}
	}
	catch(...)
	{
		
	}
}

void DebugNewTracer::Dump ()
{
	LogControl myLog;
	char szLogInfo[350]={0};

	if(m_MyMallocInfo.size() > 0 )
	{
		mylog.WriteLog2File(LogControl::LWARNING,"              ");
		mylog.WriteLog2File(LogControl::LWARNING,"              ");
		sprintf(szLogInfo,"Memery leak,Total size:%d ---->",m_TotalSize);
		mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
	}

	map<unsigned int,MYNEWINFO>::iterator it = m_MyMallocInfo.begin();
	for(;it != m_MyMallocInfo.end();it++)
	{
		MYNEWINFO myNewInfo = (*it).second;

		sprintf(szLogInfo,"File:%s,Line:%d,Addr:%X,mem size:%d",
			myNewInfo.fileName,myNewInfo.fileLine,myNewInfo.memAddr,myNewInfo.memLen);
		mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
	}

	m_MyMallocInfo.clear();

	mylog.WriteLog2File(LogControl::LWARNING,"              ");
	mylog.WriteLog2File(LogControl::LWARNING,"              ");
}

