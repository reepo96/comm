
#include "Protocal104_SH.h"
#include "PQDef.h"
#include "dfrdef.h"
#include "Linker.h"
#include "Task.h"
#include "FileControl.h"
#include "MemMapMgr.h"
#include <string.h>

PRO104SHFRAME2CMD pro104SHFR2CmdMap[] = {
	{CMD_GETPQWAVEFILE,&Protocal104_SH::CallGetPQWaveFile,"获电能质量录波文件"},
	{CMD_GETPQEVENT,&Protocal104_SH::CallGetPQEvent,"获取电能质量事件信息"},
	{CMD_GETPQTRHFILE,&Protocal104_SH::CallGetPQTrendFile,"获取电能趋势文件"},
	{CMD_GETWAVEFILELIST,&(Protocal104_SH::CallGetRecWaveFileList),"调录波文件列表"},
	{CMD_GETWAVEFILE,&Protocal104_SH::CallGetRecWaveFile,"召唤录波文件"},
	{CMD_GET_CONT_DATA,&Protocal104_SH::CallGetContData,"读动态记录数据"},
	{CMD_READ_RLT_DATA,&Protocal104_SH::CallReadRltData,"实时显示"},
	{CMD_READ_ZEROOFFSET,&Protocal104_SH::CallReadZeroOffset,"读实零漂刻度"},
	{CMD_STARTREC_MANUAL,&Protocal104_SH::CallStartRecByManual,"手动启动录波"},
	{CMD_SETTIME,&Protocal104_SH::CallSetTime,"时间同步(主到从)"},
	{CMD_GETTIME,&Protocal104_SH::CallGetTime,"获取时间"},
	{CMD_GET_NET_CFG,&Protocal104_SH::CallGetNetCfg,"调取监控板网络配置"},
	{CMD_SET_NET_CFG,&Protocal104_SH::CallSetNetCfg,"设置监控板网络配置"},
	{CMD_GETDAUINF,&Protocal104_SH::CallGetDauInf,"查询设备自检信息"},
	{CMD_SET_DEVICE_STATE,&Protocal104_SH::CallSetDeviceRunningState,"设备进入运行状态"},
	{CMD_STOPREC,&Protocal104_SH::CallSetDeviceTestingState,"设备进入调试调试"},
	{CMD_OPERATE_DEVICE,&Protocal104_SH::CallOperateDevice,"硬件操作"},
	{CMD_PUSHFILE,&Protocal104_SH::CallPushFile,"下传文件"},
	{CMD_GETFILELIST,&Protocal104_SH::CallGetSHFileList,"召唤双合自定义文件列表"},
	{CMD_PULLFILE,&Protocal104_SH::CallGetSHFile,"获取双合自定义文件"},
	{CMD_GETRUNNINGLOG,&Protocal104_SH::CallGetRunLog,"获取运行日志"},
	{CMD_PQAUTOSEND,&Protocal104_SH::CallProcessAutoSendData,"主动上送数据"},
	{CMD_PQTEST,&Protocal104_SH::CallPQTest,"电能质量性能检验"},
	{CMD_PQRESUMSET,&Protocal104_SH::CallPQResumSet,"恢复出厂设置"},
	{CMD_PQ_10CYCDATA_EXP,&Protocal104_SH::CallPQ10CycDataExp,"电能质量10周波周期数据导出"},
	{CMD_PQ_GET_10CYCDATA_FILE,&Protocal104_SH::CallGetPQ10CycDataFile,"取电能质量10周波周期数据文件"},
	{CMD_PQ_GETSTAT_FILE,&Protocal104_SH::CallGetPQStatFile,"调取统计文件"},
	{CMD_GET_AUDIT_FILE,&Protocal104_SH::CallGetAuditFile,"调取审计文件"},
	{CMD_SEND_AUDIT_ALARM,&Protocal104_SH::CallSendAuditAlarm,"审计告警"},
	{CMD_USER_LOGIN,&Protocal104_SH::CallUserLogin,"用户登录"}
};

unsigned char CalMonthBySmpTime( const SMP_TIMESTAMP & smpTime );
unsigned char CaldayBySmpTime( const SMP_TIMESTAMP & smpTime );
void SmpTimeToCP56Time2a( SMP_TIMESTAMP & inTime,struct CP56Time2a & outTime );

CRITICAL_SECTION *	Protocal104_SH::m_pMapSectionSH104 = NULL;
std::map<unsigned int ,unsigned char> Protocal104_SH::m_DevAutoSendEnableMap;

// 一年(平年)每月天数
const ST_INT8 DaysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//根据SMP_TIMESTAMP时间结构中的年中天数计算出月份
unsigned char CalMonthBySmpTime( const SMP_TIMESTAMP & smpTime )
{
	ST_UINT16 year = smpTime.year+2000;
	bool isLeapYear = ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0);
	ST_UINT16 days = smpTime.day;
	days = days%(isLeapYear?367:366);

	// 初始化月份
	ST_UINT8 month = 1;

	// 保存每月天数
	ST_UINT16 nDays = DaysOfMonth[0];

	// 减去每月天数
	for ( ST_UINT i=1; days>nDays; i++)
	{
		days -= nDays;
		month++;
		nDays = (isLeapYear && i==1) ? 29 : DaysOfMonth[i];
	}

	return month;
}

//根据SMP_TIMESTAMP时间结构中的年中天数计算出月中天
unsigned char CaldayBySmpTime( const SMP_TIMESTAMP & smpTime )
{
	ST_UINT16 year = smpTime.year+2000;
	bool isLeapYear = ((year%4 == 0) && (year%100 != 0)) || (year%400 == 0);
	ST_UINT16 days = smpTime.day;
	days = days%(isLeapYear?367:366);

	// 保存每月天数
	ST_UINT16 nDays = DaysOfMonth[0];

	for ( ST_UINT i=1; days>nDays; i++)
	{        
		days -= nDays;
		nDays = (isLeapYear && i==1) ? 29 : DaysOfMonth[i]; 
	}
	return days;
}

void SmpTimeToCP56Time2a( SMP_TIMESTAMP & inTime,struct CP56Time2a & outTime )
{
	outTime.year = inTime.year;
	outTime.hours = inTime.hour;
	outTime.minute = inTime.min;
	outTime.milliseconds = inTime.msec+inTime.sec*1000;
	outTime.month = CalMonthBySmpTime(inTime);
	outTime.day = CaldayBySmpTime(inTime);
}


//FILE* g_EvtDebugf=NULL;
//##ModelId=4C6241FB038A
Protocal104_SH::Protocal104_SH()
{
	this->m_iLenOfApdulen = 2;
	m_bAutoDataHasTrs = false;
	//m_pMapSectionSH104 = NULL;
	if (m_pMapSectionSH104 == NULL)
	{
		mylog.WriteLog2File(LogControl::LDEBUG, "Create CRITICAL_SECTION:m_pMapSectionSH104 in Protocal104_SH::Protocal104_SH()");

		m_pMapSectionSH104 = new CRITICAL_SECTION;
		InitializeCriticalSection(m_pMapSectionSH104);

		mylog.WriteLog2File(LogControl::LDEBUG, "After init CRITICAL_SECTION:m_pMapSectionSH104");
	}

	memset(RES_COMTRADE_DIR,0,sizeof(RES_COMTRADE_DIR));
	memset(RES_COMTRADE_DIV_DIR,0,sizeof(RES_COMTRADE_DIV_DIR));
	memset(RES_SHD_DIR,0,sizeof(RES_SHD_DIR));
	memset(RES_MARKENV_DIR,0,sizeof(RES_MARKENV_DIR));
	memset(RES_CONFIG_DIR,0,sizeof(RES_CONFIG_DIR));
	memset(RES_REPORT_DIR,0,sizeof(RES_REPORT_DIR));
	memset(RES_PROGRAM_DIR,0,sizeof(RES_PROGRAM_DIR));

	memset(RES_LOG_DIR,0,sizeof(RES_LOG_DIR));
	memset(RES_PQTREND_DIR,0,sizeof(RES_PQTREND_DIR));
	memset(RES_PQEVENT_DIR,0,sizeof(RES_PQEVENT_DIR));
	memset(RES_PQCOMTRADE_DIR,0,sizeof(RES_PQCOMTRADE_DIR));


	GetPrivateProfileString("Dir","Comtrade","/LDTrns/comtrade_all/",RES_COMTRADE_DIR,sizeof(RES_COMTRADE_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","ComtradeDiv","/LDTrns/comtrade_div/",RES_COMTRADE_DIV_DIR,sizeof(RES_COMTRADE_DIV_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","shd","/LDTrns/trns_raw_data/",RES_SHD_DIR,sizeof(RES_SHD_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","MarkEnv","/LDCont/event/",RES_MARKENV_DIR,sizeof(RES_MARKENV_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Config","/conf/",RES_CONFIG_DIR,sizeof(RES_CONFIG_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Report","/LDTrns/fault_report/",RES_REPORT_DIR,sizeof(RES_REPORT_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Program","/data/",RES_PROGRAM_DIR,sizeof(RES_PROGRAM_DIR),"./IEC61850.ini");

	GetPrivateProfileString("Dir","Log","/Log/run/",RES_LOG_DIR,sizeof(RES_LOG_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","PQTrend","/LDPQ/trend/",RES_PQTREND_DIR,sizeof(RES_PQTREND_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","PQTrans","/LDPQ/trans/",RES_PQCOMTRADE_DIR,sizeof(RES_PQCOMTRADE_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","PQEvent","/LDPQ/event/",RES_PQEVENT_DIR,sizeof(RES_PQEVENT_DIR),"./IEC61850.ini");

	m_szSrcAutoSendDataBuff = NULL;
	m_szDestAutoSendDataBuff = NULL;
	m_szSrcAutoSendDataBuff = new char[AutoSendDataBuffSize];

	m_szDestAutoSendDataBuff = new char[AutoSendDataBuffSize];
}

Protocal104_SH::~Protocal104_SH()
{
	if(m_szSrcAutoSendDataBuff)
	{
		delete []m_szSrcAutoSendDataBuff;
		m_szSrcAutoSendDataBuff = NULL;
	}
	if(m_szDestAutoSendDataBuff)
	{
		delete []m_szDestAutoSendDataBuff;
		m_szDestAutoSendDataBuff = NULL;
	}
}
//##ModelId=4C6244B2000F

/*Task* Protocal104_SH::DoIdle(Linker * pLinker)
{
return NULL;
}*/

bool Protocal104_SH::ExecuteCmd(Linker *pLinker, struct Command & cmd)
{
	int i=0;
	int mapCount = (sizeof(pro104SHFR2CmdMap)/sizeof(PRO104SHFRAME2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro104SHFR2CmdMap[i].cmdFunCode == cmd.funCode )
		{
			break;
		}
	}
	if( i >= mapCount)
	{
		return Protocol104::ExecuteCmd(pLinker,cmd);
	}
	memcpy(cmd.describe, pro104SHFR2CmdMap[i].cmdName,64);
	return pro104SHFR2CmdMap[i].exuteCmd(this,pLinker,cmd);
}

//##ModelId=4C62462D003E
int Protocal104_SH::AnalyseFrame(Linker* pLinker, char* framePacket, int* frameLen, APCI4Prot104* pAPCI, PASDUID104* pOutASDUID, PASDUINFID104* pOutASDUInfID, char** pOutAsduData, int* pAsduDataLen, unsigned int uiTimeOut)
{
	return Protocol104::AnalyseFrame(pLinker,framePacket, frameLen, pAPCI, pOutASDUID, pOutASDUInfID, pOutAsduData, pAsduDataLen, uiTimeOut);
}

//##ModelId=4C624C04035B
bool Protocal104_SH::GetPQRecWaveFile(Linker* pLinker, struct Command* pCmd)
{
	//Step1检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != (84+25) ) //开始文件名 + 结束文件名+ 起始位置+文件时间
	{
		printf("GetPQRecWaveFile_Cmd->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;
	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XC9;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}
	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,(pCmd->parmLen-25) );//文件时间不用发给服务端
	iAsduLen+= (pCmd->parmLen-25);//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQRecWaveFile():Send SendIFrame error");
		return false;
	}

	//处理命令结果
	char fileName[52]= {0};
	memcpy(&fileName[0],pCmd->pParm, 40);

	char fileTime[25]= {0};
	memcpy(&fileTime[0],&(pCmd->pParm[84]), 25);

	char szInfo[126]={0};
	sprintf(szInfo, "Get File:%s in Protocal104_SH::GetPQRecWaveFile()",fileName);
	mylog.WriteLog2File(LogControl::LINFO, szInfo);
	bool rst = ProcessContComtradeFileCmdRst(pLinker,pCmd, 0XCA ,&fileName[0], FileControl::FT_PQCOMTRADE,&fileTime[0]);
	return rst ;
}

//##ModelId=4C624DE300CB
bool Protocal104_SH::GetPQEvent(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  GetPQEvent() \n");
	//检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 4) 
	{
		printf("GetPQEvent_Cmd->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XCB;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQEvent():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	return GetCmdRst(pLinker,pCmd, 0XCC, 0);
}

bool Protocal104_SH::PQTest(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  PQTest() \n");
	//检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}

	if(pCmd->pParm == NULL||pCmd->parmLen != 5) 
	{
		printf("PQTest: Cmd->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XCF;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQEvent():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	return GetCmdRst(pLinker,pCmd, 0XD0, 0);
}

bool Protocal104_SH::PQResumSet(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  PQResumSet() \n");

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XD1;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::PQResumSet():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	return GetCmdRst(pLinker,pCmd, 0XD2, 0);
}

bool Protocal104_SH::PQ10CycDataExp(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  PQ10CycDataExp() \n");

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XD3;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::PQResumSet():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	return GetCmdRst(pLinker,pCmd, 0XD4, 0);
}

bool Protocal104_SH::GetPQ10CycDataFile(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  GetPQ10CycDataFile() \n");
	mylog.WriteLog2File(LogControl::LINFO, "Enter  GetPQ10CycDataFile()");

	char szInfo[100] = {0};

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XD5;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		sprintf(szInfo,"GetPQ10CycDataFile fail,devId=%d",devId);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQ10CycDataFile():Send SendIFrame error");
		return false;
	}

	return ProcessGernalFileCmdRst(pLinker,pCmd,0XD6,"pq10cycdata.dat",0,"",FileControl::FT_CFG);
}

bool Protocal104_SH::GetPQStatFile(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  GetPQStatFile() \n");
	mylog.WriteLog2File(LogControl::LINFO, "Enter  GetPQStatFile()");

	if(pCmd->pParm == NULL||pCmd->parmLen == 0 )
	{
		printf("GetPQStatFile ->parm  error\n");
		return false;
	}

	char szInfo[100] = {0};

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XD7;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		sprintf(szInfo,"GetPQStatFile fail,devId=%d",devId);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);  //文件名
	iAsduLen += pCmd->parmLen;

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQStatFile():Send SendIFrame error");
		return false;
	}

	time_t ttTime = time(NULL)-3600;
	struct tm tmTime = *localtime(&ttTime);
	char szYearMonth[8] = {0};
	sprintf(szYearMonth,"%d%02d",tmTime.tm_year+1900,tmTime.tm_mon+1);

	return ProcessGernalFileCmdRst(pLinker,pCmd,0XD8,pCmd->pParm,0,szYearMonth,FileControl::FT_PQSTAT);
}

bool Protocal104_SH::GetAuditFile(Linker* pLinker, struct Command* pCmd)
{	
	printf("Enter  GetAuditFile() \n");
	mylog.WriteLog2File(LogControl::LINFO, "Enter  GetAuditFile()");

	char szInfo[100] = {0};

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0XD9;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		sprintf(szInfo,"GetAuditFile fail,devId=%d",devId);
		mylog.WriteLog2File(LogControl::LERROR,szInfo);
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetAuditFile():Send SendIFrame error");
		return false;
	}

	return ProcessGernalFileCmdRst(pLinker,pCmd,0XDA,"audit_inf_jkb.dat",0,"",FileControl::FT_CFG);
}

bool Protocal104_SH::SendAuditAlarm(Linker* pLinker, struct Command* pCmd)
{	
	//1 字节已校样
	if(pLinker==NULL||pCmd==NULL || pCmd->pParm==NULL || pCmd->parmLen<1)
	{
		return false;
	}

	//根据命令参数构建I帧并发送
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X2D;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度 

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SendAuditAlarm():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XAE, szRstData );

	//返回命令结果到UI
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	else
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if(pLinker->IsConnect())
		return true;

	return false;
}


bool Protocal104_SH::UserLogin(Linker* pLinker, struct Command* pCmd)
{	
	//1 字节已校样
	if(pLinker==NULL||pCmd==NULL || pCmd->pParm==NULL || pCmd->parmLen<1)
	{
		return false;
	}

	//根据命令参数构建I帧并发送
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X2E;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度 

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::UserLogin():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XAF, szRstData );

	//返回命令结果到UI
	PCMDRESULT pResult = new CMDRESULT();

	if( rst && szRstData.size()>0 )
	{
		pResult->resultType = CDRT_USER_INFO;
		pResult->resultLen = szRstData.size();
		pResult->pData = new char[pResult->resultLen+1];
		memset(pResult->pData,0,pResult->resultLen+1);
		memcpy(pResult->pData,szRstData.c_str() ,szRstData.size());
	}
	else
	{
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0;
		pResult->pData[1] = '\0';
	}

	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if(pLinker->IsConnect())
		return true;

	return false;
}

bool Protocal104_SH::ProcessGernalFileCmdRst(Linker* pLinker, struct Command* pCmd, int asduNo,string fileName, int fileSize,string YearMonth, FileControl::FILETYPE fileControlType)
{
	//构建文件目录 
	int destDevId = pCmd->destDevId&0XFF;
	FileControl fileCtrl;
	string typeDir;
	unsigned int devId = 0;
	char szInfo[256]={0};
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::ProcessGernalFileCmdRst():fail,pDauInf==NULL");
		return false;
	}
	else
	{
		devId = pDauInf->DAUId;
	}
	typeDir = fileCtrl.GetFileTypeDirector(devId,fileControlType);

	if(typeDir.compare("") == 0)
	{
		typeDir = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "get the director of file type fail in Protocol61850::GetRecWaveFile()");
		return false;
	}
	string sPath = typeDir ;
	if(FileControl::FT_CFG != fileControlType)
	{
		sPath += YearMonth+string("\\");
	}
	if(!fileCtrl.IsExist( sPath.c_str() )) //检查目录是否存在
	{
		if(-1 == _mkdir(sPath.c_str()))
		{
			string sInf("Create directory:");
			sInf = sInf + sPath + string(" failed in GetRecWaveFile()");
			mylog.WriteLog2File(LogControl::LERROR, sInf.c_str());
			return false;
		}
	}
	string loaclFilePath = sPath+fileName;

	sprintf(szInfo,"Begin get file,local filepath=%s",loaclFilePath.c_str());
	mylog.WriteLog2File(LogControl::LINFO,szInfo);

	int pos =0;
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE; 
	char bHasContinueFrame = true;

	APCI4Prot104  apci(m_iLenOfApdulen);
	PASDUID104  pAsduId =NULL;
	PASDUINFID104 pAsduInf = NULL;
	char* pAsduData =NULL;
	int iAsduDataLen =0;

	int iMaxValidateLen = PROSH104_FRAME_MAXSIZE-apci.GetApciLen();
	iMaxValidateLen-=ASDUID104_LEN;
	iMaxValidateLen-=ASDUINFID104_LEN;
	iMaxValidateLen-=(1+1+4);  //后续帧表示 + 起始位置

	bool bOpenFlag = fileCtrl.OpenFile(loaclFilePath.c_str(),FileControl::OM_WRITE);
	if( !bOpenFlag )
	{
		m_errCode = PEC_UNKOWN;
		string info = string(loaclFilePath.c_str()) + string(" open failed in ProcessGernalFileCmdRst()");
		mylog.WriteLog2File(LogControl::LERROR,info.c_str());
		return false;
	}
	unsigned int  hasReceivedSize =0;
	unsigned int iProgress =0;
	int iStep =0;
	int iCounter =0;
	bool error = false;
	bool isfailer = false;//非网络故障的失败，如服务找不到文件

	if (fileName.compare("SHDevice_1.dat") == 0)
	{
		printf("开始获取定值文件：SHDevice_1.dat\n");
	}
	else if (fileName.compare("SHDevice_2.dat") == 0)
	{
		printf("开始获取定值文件：SHDevice_2.dat\n");
	}
	else
	{

	}

	//取数据
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		int iAsduLen =0;
		int rst =0;
		memset(&szFrameData[0],0,PROSH104_FRAME_MAXSIZE+1);
		rst = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInf,&pAsduData,&iAsduLen,13000);
		if(-1==rst)
		{
			//有错误
			printf("AnalyseFrame error\n");
			mylog.WriteLog2File(LogControl::LERROR, "AnalyseFrame error\n");
			pLinker->Close();
			error = true;
			break;
		}
		else if(0 == rst)//超时
		{
			printf("Get CmdRst TimeOut  error\n");
			mylog.WriteLog2File(LogControl::LERROR, "Get CmdRst TimeOut  error");
			pLinker->Close();
			error = true;
			return false;
		}
		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				mylog.WriteLog2File(LogControl::LERROR, "Unkown ASDU");
				printf("Unkown ASDU \n");
				pLinker->Close();
				error = true;
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				mylog.WriteLog2File(LogControl::LERROR, "Get pAsduId->COT ==15");
				printf("Get pAsduId->COT ==15 \n");
				isfailer = true;
				return false;
			}
			if(pAsduId->TYP!=asduNo)
			{
				sprintf(szInfo,"ASDU TYP error,pAsduId->TYP=%d,asduNo=%d",pAsduId->TYP,asduNo);
				mylog.WriteLog2File(LogControl::LERROR, szInfo);
				error = true;
				pLinker->Close();
				break;
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;

			if(validatelen>=2)
			{
				bHasContinueFrame = *(pDataTmp+1); //后续标示
				validatelen-=2;
				pDataTmp +=2;
			}
			else
			{
				sprintf(szInfo,"validatelen=%d,error",validatelen);
				mylog.WriteLog2File(LogControl::LERROR, szInfo);
				error = true;
				pLinker->Close();
				break;
			}

			if(validatelen>=4)
			{
				memcpy(&pos, pDataTmp, sizeof(int));  //文件位置信息
				validatelen -=4;
				pDataTmp +=4;
			}
			else
			{
				error = true;
				pLinker->Close();
				break;
			}
			if(validatelen>0)
			{
				//写文件
				int iwrite = validatelen;
				if(iwrite != fileCtrl.WriteData(pDataTmp,iwrite))
				{
					m_errCode = PEC_UNKOWN;
					string info = string(loaclFilePath.c_str()) + string(" write  failed in ProcessGernalFileCmdRst()");
					mylog.WriteLog2File(LogControl::LERROR,info.c_str());
					fileCtrl.CloseFile();
					pLinker->Close();
					return false;
				}
				hasReceivedSize += iwrite;
			}
		}
		//进度信息
		if(fileSize>0)
		{
			if(iCounter==0)
			{
				iStep = fileSize/(iMaxValidateLen*20) ;
				if(iStep==0||iStep==1)
				{
					iStep =2;
				}
			}
			iCounter++;
			if(iCounter%iStep==0)
			{
				pCmd->resultNum = 1;
				PCMDRESULT pResult = new CMDRESULT();
				memset(pResult,0,sizeof(pResult));
				pResult->resultType = CDRT_PROGRESS;
				pResult->resultLen = 1;
				pResult->pData = new char[2];
				pResult->pData[1] = '\0';

				if(hasReceivedSize>0)
				{
					iProgress = (hasReceivedSize*100/fileSize);
					iProgress = (iProgress>100)?100:iProgress;
				}
				pResult->pData[0] = iProgress;
				pCmd->resultList.push_back(pResult);
				MemMapMgr memMgr;
				memMgr.WriteTaskResultToMem(*pCmd);
				//清除结果
				if(pResult)
				{
					if(pResult->pData)
					{
						delete []pResult->pData;
						pResult->pData= NULL;
					}
					delete pResult;
					pResult = NULL;
				}
				pCmd->resultNum = 0;
				pCmd->resultList.clear();
			}
		}
	}

	fileCtrl.CloseFile();	

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if( isfailer || error )
	{
		sprintf(szInfo, "Get file fail,Receive FileDataLen = %d", hasReceivedSize);
		mylog.WriteLog2File(LogControl::LWARNING, szInfo);

		pResult->pData[0] = 0 ; //执行失败
	}
	else
	{
		sprintf(szInfo, "Get file success,Receive FileDataLen = %d", hasReceivedSize);
		mylog.WriteLog2File(LogControl::LINFO, szInfo);

		pResult->pData[0] = 1 ; //执行成功
	}
	pResult->pData[1] = 0 ;
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum =1;

	if(error)
	{
		return false;
	}
	return true;
}

//##ModelId=4C624DF702FD
bool Protocal104_SH::GetContData(Linker* pLinker, struct Command* pCmd)
{
	//Step1检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != (1+4+4+4+12+24+sizeof(unsigned short) +100)) //
	{
		printf("GetContData->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;


	char dataType = *(pCmd->pParm); //命令中第一个字符即 调取的文件类型

	unsigned  int start_soc =0;
	memcpy(&start_soc, pCmd->pParm+1,sizeof(unsigned  int));

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X69;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	//	memcpy(&pASDU[iAsduLen],pCmd->pParm,(pCmd->parmLen-100-sizeof(unsigned short)));// 原参数中的文件名称 和超时时间不发送
	//iAsduLen+= pCmd->parmLen -100 - sizeof(short);//参数中的长度

	CONTSEARCHPARM   searchParm;  // 完成 
	memset(&searchParm,0, sizeof(CONTSEARCHPARM));
	searchParm.Type = dataType;
	searchParm.StartTime = start_soc;
	memcpy(&searchParm.Interval, pCmd->pParm+1+4, 4);
	memcpy(&searchParm.LastTime, pCmd->pParm+1+4+4, 4);
	memcpy(&searchParm.AnChFlag, pCmd->pParm+1+4+4+4, 12);
	memcpy(&searchParm.BinChFlag, pCmd->pParm+1+12+12, 24);
	memcpy(&searchParm.FileName, pCmd->pParm+1+12+12+24+2, 64);

	memcpy(&pASDU[iAsduLen],&searchParm,sizeof(CONTSEARCHPARM) );// 
	iAsduLen+= sizeof(CONTSEARCHPARM) ;//参数长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetContData():Send SendIFrame error");
		return false;
	}

	//获取命令结果
	bool rst = false;


# if defined(_MSC_VER)  &&  (_MSC_VER >= 1400)//VC 2005
	//struct tm tmStartTime = *_localtime32((const __time32_t *)&cmd_data.startTime);
	struct tm tmStartTime = *_localtime32((const __time32_t *)&start_soc);
#else
	struct tm tmStartTime = *localtime((time_t*)&start_soc);
#endif

	int iYearMonth = (tmStartTime.tm_year+1900)*100 + tmStartTime.tm_mon+1;
	char szYearMonth[10]={0};
	::itoa(iYearMonth,szYearMonth,10);
	char *pTime = &szYearMonth[0];

	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	char szFileName[100] = {0};
	char szDevName[64] = {0};
	if( pDauInfo != NULL)
	{
		memcpy(szDevName,pDauInfo->DAUName,sizeof(pDauInfo->DAUName) );
	}
	else
	{
		memcpy(szDevName,"录波器",strlen("录波器") );
	}

	sprintf(szFileName,"%d-%02d-%02d_%02d-%02d-%02d_lta_%s",tmStartTime.tm_year+1900,tmStartTime.tm_mon+1,
		tmStartTime.tm_mday,tmStartTime.tm_hour,tmStartTime.tm_min,tmStartTime.tm_sec,szDevName);

	string fileName(szFileName);
	rst = ProcessContComtradeFileCmdRst(pLinker,pCmd, 0X6A ,szFileName, FileControl::FT_CONT);
	return rst ;


}

//##ModelId=4C624E0C00BB
bool Protocal104_SH::GetRecWaveFile(Linker* pLinker, struct Command* pCmd)
{
	//Step1检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 45) //类型 文件名 +  起始位置
	{
		printf("GetRecWaveFile->parm  error\n");
		return false;
	}
	char fileName[64]={0};
	char szInfo[512]={0};
	strncpy(fileName,pCmd->pParm+1,40); 
	sprintf(szInfo, "Get File:%s in Protocal104_SH::GetRecWaveFile()",fileName);
	mylog.WriteLog2File(LogControl::LINFO,szInfo);

	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X67;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm+1,(pCmd->parmLen-1));  // 类型本协议中不处理
	iAsduLen+= pCmd->parmLen-1;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetRecWaveFile():Send SendIFrame error");
		return false;
	}

	//处理命令结果
	//文件名
	bool rst = ProcessComtradeFileCmdRst(pLinker,pCmd, 0X68 ,FileControl::FT_COMTRADE);

	return rst ;
}


//##ModelId=4C6252B8009C
bool Protocal104_SH::GetRecWaveFileList(Linker* pLinker, struct Command* pCmd)
{

	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||(pCmd->parmLen != 1+sizeof(struct CP56Time2a)*2) )
	{
		printf("GetRecWaveFileList->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X65;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],(pCmd->pParm+1),pCmd->parmLen-1); //第一个字符不用
	iAsduLen+= pCmd->parmLen-1;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetRecWaveFileList():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	return  GetCmdRst(pLinker,pCmd, 0X66,0 );
}


//##ModelId=4C6252E9004E
bool Protocal104_SH::ReadRcdOffsetData(Linker* pLinker, struct Command* pCmd)
{
	// 待处理
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1) 
	{
		printf("ReadRcdOffsetData->parm  error\n");
		return false;
	}

	bool bTypeEnable =true;
	int boardId = pCmd->destDevId; //提取设备监控板号

	if(m_pMapSectionSH104 != NULL)
		EnterCriticalSection(m_pMapSectionSH104);

	map<unsigned int ,unsigned char>::iterator  iter ;
	iter = m_DevAutoSendEnableMap.find(boardId);
	if(iter == m_DevAutoSendEnableMap.end())
	{
		//struct AutoSendEnable* pDevEnable = new struct AutoSendEnable;
		unsigned char pDevEnable = (1<<2)|(1<<3);
		if(bTypeEnable)
		{
			pDevEnable |= 1<<1;
		}
		m_DevAutoSendEnableMap.insert(std::make_pair(boardId, pDevEnable));

		if(m_pMapSectionSH104 != NULL)
			LeaveCriticalSection(m_pMapSectionSH104);
		return false;
	}
	else
	{
		if(bTypeEnable)
		{
			iter->second |= 1<<1;
		}
		else
		{
			iter->second &= ~(1<<1);
		}

		if(m_pMapSectionSH104 != NULL)
			LeaveCriticalSection(m_pMapSectionSH104);
		return true;
	}

}

//##ModelId=4C6253000232
bool Protocal104_SH::ReadRcdRtData(Linker* pLinker, struct Command* pCmd)
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_SH::ReadRcdRtData()");

	//已校样 参数 2 字节
	if(pLinker==NULL||pCmd==NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::ReadRcdRtData():parm error1");
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 2) 
	{
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::ReadRcdRtData():parm error2");
		printf("ReadRcdRtData_Cmd->parm  error\n");
		return false;
	}

	//根据命令参数构建I帧并发送
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;
	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X6B;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::ReadRcdRtData():devId error");
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::ReadRcdRtData():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X6C,0, szRstData );
	//返回命令结果到UI

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	else
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect() )
	{
		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_SH::ReadRcdRtData() succ");
		return true;
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_SH::ReadRcdRtData() fail");
	return false;
}

//##ModelId=4C62531F02CE
bool Protocal104_SH::StartRecordByManune(Linker* pLinker, struct Command* pCmd)
{
	printf("#####Protocal104_SH::StartRecordByManune\n");
	//不带任何参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1) 
	{
		printf("StartRecordByManune->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X6F;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::StartRecordByManune():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X70,0, szRstData );
	//返回命令结果到UI

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	else
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect() )
		return true;

	return false;
}

//##ModelId=4C6253830157
bool Protocal104_SH::SetDeviceTime(Linker* pLinker, struct Command* pCmd)
{
	//参数已交验，1+SHTime  第一字节不使用
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1+sizeof(struct SHTimeStructDAU)) 
	{
		printf("SetDeviceTime->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X20;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm+1,sizeof(struct SHTimeStructDAU));
	iAsduLen+= pCmd->parmLen-1;//参数中的长度  第一个字节不使用

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SetDeviceTime():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA1, szRstData );
	//返回命令结果到UI

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	else
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if(pLinker->IsConnect())
		return true;

	return false;
}

//##ModelId=4C62539101D4
bool Protocal104_SH::GetDeviceTime(Linker* pLinker, struct Command* pCmd)
{
	//1 字节已校样
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1) 
	{
		printf("GetDeviceTime->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X21;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度 

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetDeviceTime():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA2, szRstData );
	//返回命令结果到UI
	if(rst)
	{
		if(szRstData.length() == sizeof(SMP_TIMESTAMP))
		{
			SMP_TIMESTAMP  smp_time ;
			struct CP56Time2a getTime;
			memset(&getTime,0,sizeof(struct CP56Time2a));
			memcpy(&smp_time, szRstData.c_str(), sizeof( SMP_TIMESTAMP));

			SmpTimeToCP56Time2a(smp_time, getTime);
			const int size = sizeof(struct CP56Time2a);
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_TIME;
			pResult->resultLen = size;
			pResult->pData = new char[size+1];
			memset(pResult->pData, 0, size+1);
			memcpy(pResult->pData,&getTime ,size);
			pCmd->resultList.push_back(pResult);
			pCmd->resultNum = 1;
			return true;
		}	
	}
	else
	{
		PCMDRESULT pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum = 1;
	}

	if(pLinker->IsConnect())
		return true;

	return false;
}

bool Protocal104_SH::SetDeviceRunningState(Linker* pLinker, struct Command* pCmd)
{
	//   4 字节参数  
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != (1+1+2)) 
	{
		printf("SetDeviceRunningState->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X23;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度  

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SetDeviceRunningState():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA4,szRstData );
	//返回命令结果到UI
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect())
		return true;

	return false;
}
bool Protocal104_SH::SetDeviceTestingState(Linker* pLinker, struct Command* pCmd)
{
	//   0字节参数  自己组织必要的参数 由于之前版本的进入调试状态时没有传入参数 ，
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->parmLen != 0) 
	{
		printf("SetDeviceTestingState- pCmd->parmLen error\n");
		return false;
	}
	//根据命令参数构建I帧并发送
	char state[4] ={0};
	state[0] = 2; //暂态录波
	state[1] = 2; //调试状态

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X23;

	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],&state[0],4);
	iAsduLen+= (1+1+2);//参数中的长度  

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SetDeviceTestingState():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA4,szRstData );
	//返回命令结果到UI
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect())
		return true;

	return false;
}

//##ModelId=4C6253C0030D
bool Protocal104_SH::GetDeviceState(Linker* pLinker, struct Command* pCmd)
{
	// 1字节参数  已 校对
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1) 
	{
		printf("GetDeviceState->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X22;    //信息序号    34
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetDeviceState():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA3,szRstData );
	//返回命令结果到UI
	if(rst)
	{
		char result = 0;
		if(szRstData.length()>0)
		{
			memcpy(&result, szRstData.c_str(), sizeof(char));
			int size = szRstData.length()-1;
			if(result&&(size>0))
			{
				PCMDRESULT pResult = new CMDRESULT();
				pResult->resultType = CDRT_CHAR;
				pResult->resultLen = size;
				pResult->pData = new char[size+1];
				memset(pResult->pData, 0, size+1);
				memcpy(pResult->pData, (szRstData.c_str()+1), size);
				pCmd->resultList.push_back(pResult);
				pCmd->resultNum = 1;
				return true;	
			}
		}
	}
	else
	{
		PCMDRESULT pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum = 1;
	}

	if( pLinker->IsConnect())
		return true;
	return false;
}

//##ModelId=4C6253D901C5
bool Protocal104_SH::GetDAUInfo(Linker* pLinker, struct Command* pCmd)
{
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 1)   //
	{
		printf("GetDAUInfo->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X24;    //信息序号    36
	iAsduLen += ASDUINFID104_LEN;   
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetDAUInfo():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA5, szRstData );
	//返回命令结果到UI
	if(rst)
	{
		const int LenOfCheck = sizeof(CHECK);
		if(szRstData.length()==LenOfCheck) //待定 长度 监控板返回的自检 结构
		{
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_TRUFALSE;
			pResult->resultLen = 1;
			pResult->pData = new char[2];
			pResult->pData[0] = 1;
			pResult->pData[1] = '\0';
			pCmd->resultList.push_back(pResult);
			pCmd->resultNum += 1;

			// memcpy(&result, szRstData.c_str(), 0x555);
			//待定 解析结构
			pResult = new CMDRESULT();
			pResult->resultType = CDRT_CHAR;
			pResult->resultLen = LenOfCheck;
			pResult->pData = new char[LenOfCheck];
			memset(pResult->pData, 0, LenOfCheck);
			memcpy(pResult->pData,szRstData.c_str() , LenOfCheck);
			CHECK* pChcek = (CHECK*)pResult->pData;
			pCmd->resultList.push_back(pResult);
			pCmd->resultNum += 1;
			return true;
		}
	}
	else
	{
		PCMDRESULT pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum = 1;
	}

	if( pLinker->IsConnect())
		return true;
	return false;
}

//##ModelId=4C62540B03B9
bool Protocal104_SH::OperateHardWare(Linker* pLinker, struct Command* pCmd)
{
	// 以校样 参数5 个字节 
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != 5)   //
	{
		printf("GetDAUInfo->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X25;    //信息序号    37
	iAsduLen += ASDUINFID104_LEN;   
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::OperateHardWare():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XA6, szRstData );
	//返回命令结果到UI
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	if(rst)
	{
		pResult->pData[0] = 1;
	}
	else
	{
		pResult->pData[0] = 0;
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect() )
		return true;

	return false;
}

//##ModelId=4C625440036B
bool Protocal104_SH::GetGeneralFilelist(Linker* pLinker, struct Command* pCmd)
{
	return true;
}

//##ModelId=4C6254580148
bool Protocal104_SH::GetGeneralFile(Linker* pLinker, struct Command* pCmd)
{
	//这个 暂不处理
	return true;
}

//##ModelId=4C62546502EE
bool Protocal104_SH::GetRunLog(Linker* pLinker, struct Command* pCmd)
{
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||pCmd->parmLen != sizeof(struct CP56Time2a)*2)   //
	{
		printf("GetRunLog->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送
	char szFrameData[PROSH104_FRAME_MAXSIZE] = {0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE;
	int iAsduLen =0;
	APCI4Prot104  apci(m_iLenOfApdulen);
	int iMaxValidateLen = PROSH104_FRAME_MAXSIZE-apci.GetApciLen();
	iMaxValidateLen-=ASDUID104_LEN;
	iMaxValidateLen-=ASDUINFID104_LEN;
	iMaxValidateLen-=2;  //后续帧表示 


	PASDUID104  pAsduId = (PASDUID104)&szFrameData[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&szFrameData[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X28;    //信息序号    40
	iAsduLen += ASDUINFID104_LEN;   
	iAsduLen +=1; //返回标示符信息
	memcpy(&szFrameData[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度

	int iResult = SendIFrame(pLinker,szFrameData,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetRunLog():Send SendIFrame error");
		return false;
	}

	bool error = false;
	char bHasContinueFrame=1;

	string logContext;
	//取数据
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		int iAsduLen =0;
		int rst =0;
		memset(&szFrameData[0],0,PROSH104_FRAME_MAXSIZE);
		rst = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInf,&pAsduData,&iAsduLen,13000);
		if(-1==rst)
		{
			//有错误
			printf("AnalyseFrame error\n");
			mylog.WriteLog2File(LogControl::LERROR, "AnalyseFrame error\n");
			pLinker->Close();
			error = true;
			break;
		}
		else if(0 == rst)//没数据
		{
			printf("Get CmdRst TimeOut  error\n");
			return false;
		}
		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				printf("Unkown ASDU \n");
				pLinker->Close();
				error = true;
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				printf("Get pAsduId->COT ==15 \n");
				return false;
			}
			if(pAsduId->TYP!=0X20)
			{
				printf("ASDU TYP error\n");
				error = true;
				pLinker->Close();
				break;
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;
			if(validatelen>=2)
			{
				bHasContinueFrame = *(pDataTmp+1); //后续标示
				validatelen-=2;
				pDataTmp +=2;
			}

			if(validatelen>0)
			{
				logContext += string(pDataTmp, validatelen);
			}
		}
	}

	if(error)
	{
		return false;
	}
	else
	{
		const int Loglength = logContext.length();
		const char *pContext = logContext.c_str();
		int LogItemCount = (Loglength)/sizeof(RunLogInf) ;
		if(LogItemCount>0)
		{
			while(LogItemCount>40)
			{
				RunLogInf* pLogItme = (RunLogInf* )pContext;
				PCMDRESULT pResult = new CMDRESULT();
				pResult->resultType = CDRT_CHAR;
				pResult->resultLen = sizeof(RunLogInf)*40;
				pResult->pData = new char[pResult->resultLen];
				memcpy(pResult->pData, pContext,sizeof(RunLogInf)*40);
				pContext += sizeof(RunLogInf)*40;
				pCmd->resultList.push_back(pResult);
				pCmd->resultNum += 1;
				LogItemCount-=40;
			}
			//剩余事件入结果列表
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_CHAR;
			pResult->resultLen = sizeof(RunLogInf)*LogItemCount;
			pResult->pData = new char[pResult->resultLen];
			memcpy(pResult->pData, pContext,sizeof(RunLogInf)*LogItemCount);
			pCmd->resultList.push_back(pResult);
			pCmd->resultNum += 1;
			return true;
		}
	}
	return false;
}

//##ModelId=4C62547600FA
bool Protocal104_SH::SHPushFile(Linker* pLinker, struct Command* pCmd)
{ 
	//检查参数 文件类型， 用户名 秘密 + 文件名 需要提取类型 文件名称参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||(pCmd->parmLen != 1+20+10+100))
	{
		printf("SHPushFile_Cmd->parm  error\n");
		return false;
	}
	//根据命令参数构建I帧并发送
	char filePath[100] ={0};
	memcpy(&filePath[0],(pCmd->pParm+1+20+10),100); //取 文件全路径
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	FileControl  fileCtrl;
	unsigned int fileSize = fileCtrl.GetFileLen(filePath);
	if(fileSize<=0)
	{
		printf("file Parm is error \n");
		return false;
	}


	char* pdest = strrchr(filePath, '\\');
	int pos = pdest-filePath ;
	if(pdest ==NULL||pos>99)
	{
		return false;
	}


	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X26;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,sizeof(char));
	unsigned char fileType = *pCmd->pParm;

	if( fileType == 20 )
	{
		memcpy(&pASDU[iAsduLen+1],"ipconfig.dat",strlen("ipconfig.dat"));
	}
	else if(10 == fileType)  //向监控板下发定值文件  监控板定值文件名称统一为 "SHDevice.dat"
	{
		memcpy(&pASDU[iAsduLen+1],"SHDevice.dat",strlen("SHDevice.dat"));
	}
	else
	{
		memcpy(&pASDU[iAsduLen+1],&filePath[pos+1],(100-pos));
	}


	iAsduLen+=(1+100);//参数中的长度
	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SHPushFile():Send SendIFrame error");
		return false;
	}

	//返回确认
	string  szRstData;
	bool rst = GetCmdRst(pLinker,  0X20, 0XA7, szRstData );
	if(!rst || (szRstData.length()==1 && szRstData.at(0) !=1) )
	{
		PCMDRESULT pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];

		if(rst && szRstData.length()==1 && szRstData.at(0) == 1 )
		{
			pResult->pData[0] = 1;
		}
		else
		{
			pResult->pData[0] = 0;
		}
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum = 1;

		if( pLinker->IsConnect())
			return true;

		return false;
	}

	//下发文件
	int fixLen = ASDUINFID104_LEN+ASDUID104_LEN+2+4;
	unsigned int validatelen = PROSH104_FRAME_MAXSIZE - P104_STARTCHAR_LEN ; //去掉启动字符
	validatelen -= m_iLenOfApdulen;
	validatelen -=  P104_CONTROLAREA_LEN;  //去掉控制域
	validatelen -=fixLen;

	pAsduInf->INF = 0X27;

	int *fpos = (int*)(&pASDU[fixLen-4]);  //位置
	char *bContinue = (char*)&pASDU[fixLen-5]; //后续标识

	int needSendCount = (fileSize+validatelen-1)/validatelen;

	printf("开始下发文件：%s\n",filePath);
	bool bFlag = fileCtrl.OpenFile(filePath,FileControl::OM_READ);
	if( !bFlag )
	{
		m_errCode = PEC_UNKOWN;
		string info = string(filePath) + string(" open failed in SHPushFile()");
		mylog.WriteLog2File(LogControl::LERROR,info.c_str());
		return false;
	}

	unsigned int hasSendLength =0;
	int i =0;
	while(hasSendLength<fileSize)
	{
		int iReadLen = validatelen;
		*bContinue = 1;
		if((fileSize-hasSendLength)<=validatelen)
		{
			*bContinue = 0;
			iReadLen = fileSize -hasSendLength;

		}
		if(iReadLen != fileCtrl.ReadData(&pASDU[fixLen], iReadLen))
		{
			m_errCode = PEC_UNKOWN;
			string info = string(filePath) + string(" read failed in SHPushFile()");
			printf("读取文件:%s,失败\n",filePath);

			mylog.WriteLog2File(LogControl::LERROR,info.c_str());
			fileCtrl.CloseFile();
			return false;
		}
		iAsduLen = fixLen+iReadLen;
		*fpos = hasSendLength;
		//发送数据
		int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
		if( -1 == iResult || 0 == iResult)
		{
			pLinker->Close();
			mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SHPushFile():Send SendIFrame error");
			fileCtrl.CloseFile();
			return false;
		}
		hasSendLength += iReadLen;
	}

	if (fileSize == hasSendLength)
	{
		printf("下发文件：%s，成功\n",filePath);
		string str(filePath);
		str += "写文件成功";
		mylog.WriteLog2File(LogControl::LINFO, str.c_str());
	}
	else
	{
		printf("下发文件：%s，失败\n",filePath);
		string str(filePath);
		str += "写文件失败";
		mylog.WriteLog2File(LogControl::LINFO, str.c_str());
	}

	fileCtrl.CloseFile();
	//获取结果下发
	szRstData.resize(0) ;
	rst = GetCmdRst(pLinker,  0X20, 0XA8, szRstData );

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];

	//int len = szRstData.length();
	//int Rst = szRstData.at(0);
	//char tmp[100];
	//sprintf(tmp,"len = %d,Rst = %d,rst = %d",len,Rst,rst);
	//string strInfo(tmp);
	//mylog.WriteLog2File(LogControl::LERROR,strInfo.c_str());

	if(rst && szRstData.length() ==1 && szRstData.at(0) == 1 )
	{
		pResult->pData[0] = 1;
		mylog.WriteLog2File(LogControl::LERROR,"pResult->pData[0] = 1");
	}
	else
	{
		pResult->pData[0] = 0;
		mylog.WriteLog2File(LogControl::LERROR,"pResult->pData[0] = 0");
	}
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if( pLinker->IsConnect())
		return true;

	return false;
}

//##ModelId=4C625484004E
bool Protocal104_SH::GetSHFileList(Linker* pLinker, struct Command* pCmd)
{
	// 3 个有效参数  目录类型  已校对 //开始时间//结束时间
	//检查参数
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	const int TIMESTARUCTLEN = sizeof(struct CP56Time2a);
	if(pCmd->pParm == NULL||(pCmd->parmLen != (1+4+TIMESTARUCTLEN*2 +1)) )
	{
		printf("GetGetSHFileList_Cmd->parm  error\n");
		return false;
	}

	if(pCmd->pParm[5+2*TIMESTARUCTLEN] !=2)
	{
		printf("GetGetSHFileList_ParmTyp error\n");
		return false;
	}
	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X14;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	unsigned char devId = GetDevId(pCmd->destDevId);
	if(devId>0)
	{
		pAsduId->COMADDR = (devId&0XFF)<<8; //提取设备号
	}
	else
	{
		return false;
	}

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	pAsduInf->INF = 0X29;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	//提取参数
	unsigned short dirTyp = 0;
	dirTyp = pCmd->pParm[0];
	memcpy(&pASDU[iAsduLen],&dirTyp,sizeof(short));
	memcpy(&pASDU[iAsduLen+2],&pCmd->pParm[1+4],sizeof(struct CP56Time2a)*2 );
	iAsduLen += (2+sizeof(struct CP56Time2a)*2);//参数中的长度

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetSHFileList():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	return GetCmdRst(pLinker, pCmd, 0X20, 0XAA );
}
