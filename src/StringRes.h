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
#pragma once

#include "public.h"
#include "../Resource.h"
#include <map>
using namespace std;

class CStringRes
{
public:
	~CStringRes(void);

	static CStringRes * GetInstance( void );

	const char * GetStrById(unsigned int nStrId,const char *pDefault=NULL);

protected:
	CStringRes(void);

	static CStringRes *_instance;

	HINSTANCE m_hRCInstance;
	map<unsigned int,char *> m_StrMap;
	CRITICAL_SECTION	m_MapSection;
};
