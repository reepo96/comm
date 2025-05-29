/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：CFileDB.h
* 文件标识：
* 摘    要：文件数据库，即模仿数据库接口从文件获取数据，主要是为了通讯模块在没有数据库的情况下也能获得必要的数据，而又使
*			使用的数据库接口函数有效
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年12月10日
*
**************************************************************************/

#pragma once

#include <CommDataType.h>

class CFileDB
{
public:
	CFileDB(void);
public:
	virtual ~CFileDB(void);

	static int _stdcall DbInit(char ip[],char dbname[],char username[],char password[]);

	static int _stdcall AddLog(LogStruct *LogInfo);

	static int _stdcall GetAllDau(DAUInfoStruct **DAUInfo);

	static int _stdcall GetAllCfg(SystemCfg **ppSysCfg);

	static int _stdcall ReleaseMemery(void *pVariant);

	static int _stdcall GetToBeSendRpt2Rmt(const char *rmtIpAddr,RptStruct **RptInfo);

	static int _stdcall AddSendFaultInf(SendFaultInfo *pSendFaultInf);

	static int _stdcall GetRptBySql(char *pSql,RptStruct **RptInfo);

	static int _stdcall GetRptById(unsigned char dauId, int rptId, RptStruct **RptInfo);

	static int _stdcall GetAllTimerCallInf(TimerCall **ppTimerCallInf);

	static int _stdcall GetMaxFileNo(unsigned char aduId,unsigned char fileType);

	static int _stdcall AddCallFileInf(PCallFileInf pFileInf);

	static int _stdcall GetNoCallFileByType(unsigned char dauId,unsigned char type,CallFileInf **ppFileInf);

	static int _stdcall UpdateCallFileInfById(unsigned char DAUID, char *fileName, bool IsCallData);

	static int _stdcall GetMarkedEvtBySql(char *pSql,MarkedEvtStruct **MarkedEvtInfo);
};
