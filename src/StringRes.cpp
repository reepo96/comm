/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：StringRes.h
* 文件标识：
* 摘    要：字符串资源类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2009年7月24日
*
**************************************************************************/

#include "StringRes.h"

CStringRes *CStringRes::_instance = NULL;

CStringRes::CStringRes(void)
{
	InitializeCriticalSection(&m_MapSection);

	m_hRCInstance = NULL;
	char szLanguage[64] = {0};
	GetPrivateProfileString("System","Language","Chinese",szLanguage,sizeof(szLanguage),"./Communication.ini");

	if( strcmp(szLanguage,"Chinese") == 0 )
	{
		m_hRCInstance = LoadLibrary("SHCommRC_CH.dll");
	}
	else if( strcmp(szLanguage,"English") == 0 )
	{
		m_hRCInstance = LoadLibrary("SHCommRC_EN.dll");
	}
	else
	{
		m_hRCInstance = LoadLibrary("SHCommRC_CH.dll");
	}
}

CStringRes::~CStringRes(void)
{
}

CStringRes * CStringRes::GetInstance( void )
{
	if( _instance == NULL)
		_instance = new CStringRes();
	return _instance;
}

const char * CStringRes::GetStrById(unsigned int nStrId,const char *pDefault)
{
	char *pDesc = NULL;

	EnterCriticalSection(&m_MapSection);
	map<unsigned int,char *>::const_iterator it = m_StrMap.find(nStrId);

	if( it != m_StrMap.end() )//map中已经存在
	{
		pDesc = (*it).second;
		LeaveCriticalSection(&m_MapSection);
		return pDesc;
	}
	else if( m_hRCInstance != NULL )//从资源中查找串
	{
		pDesc = new char[64];
		memset(pDesc,0,64);

		if( 0 != LoadString(m_hRCInstance,nStrId,pDesc,64) )
		{
			m_StrMap.insert( map<unsigned int,char *>::value_type(nStrId,pDesc) );
			LeaveCriticalSection(&m_MapSection);
			return pDesc;
		}
		else
		{
			delete []pDesc;
			pDesc = NULL;
		}
	}

	//前面都没找到对应的字符串
	if( pDefault != NULL)
	{
		pDesc = new char[64];
		memset(pDesc,0,64);

		strncpy(pDesc,pDefault,63);
		m_StrMap.insert( map<unsigned int,char *>::value_type(nStrId,pDesc) );
	}

	LeaveCriticalSection(&m_MapSection);
	return pDesc;
}