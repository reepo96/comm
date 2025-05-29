/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�debugnew.cpp
* �ļ���ʶ��
* ժ    Ҫ�����ز�����new��delete���ڵ���״̬�£�����¼�ڴ������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��02��4��
*
**************************************************************************/

#include "debugnew.h"
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include "LogControl.h"

//#define min(a,b)	((a)<(b))?(a):(b)

extern LogControl mylog;     //��־
extern LPCRITICAL_SECTION 	g_pSection4NewMem; //���ڷ����ڴ�ͬ�����ٽ���

#ifdef _DEBUG
	extern DebugNewTracer myNewTracer;
#endif

void* operator new[](unsigned int size)
{
	//���˫��ϵͳ�����ڴ����ʧ�ܵ�����
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
	//���˫��ϵͳ�����ڴ����ʧ�ܵ�����
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
		//�����¼��Ϣ
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
	//���˫��ϵͳ�����ڴ����ʧ�ܵ�����
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
	//���˫��ϵͳ�����ڴ����ʧ�ܵ�����
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
			//��¼�ڴ������Ϣ
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
		//�����¼��Ϣ
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
		//��¼�ڴ������Ϣ
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

