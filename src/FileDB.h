/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�CFileDB.h
* �ļ���ʶ��
* ժ    Ҫ���ļ����ݿ⣬��ģ�����ݿ�ӿڴ��ļ���ȡ���ݣ���Ҫ��Ϊ��ͨѶģ����û�����ݿ�������Ҳ�ܻ�ñ�Ҫ�����ݣ�����ʹ
*			ʹ�õ����ݿ�ӿں�����Ч
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��12��10��
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
