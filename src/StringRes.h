/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�StringRes.h
* �ļ���ʶ��
* ժ    Ҫ���ַ�����Դ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2009��7��24��
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
