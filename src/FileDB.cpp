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
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include "FileDB.h"

CFileDB::CFileDB(void)
{
}

CFileDB::~CFileDB(void)
{
}

int CFileDB::DbInit(char ip[],char dbname[],char username[],char password[])
{
	return -1;
}

int CFileDB::AddLog(LogStruct *LogInfo)
{
	return -1;
}

int CFileDB::GetAllDau(DAUInfoStruct **DAUInfo)
{
	char DAUID[4]={0};
	char DAUName[50]={0};
	char CommModel[2]={0};
	char IPAddr[20]={0};
	char TcpPort[8]={0};
	char COMName[5]={0};
	char SerialBaudRate[11]={0};
	char SerialDataBits[2]={0};
	char SerialPariy[2]={0};
	char ModemName[5]={0};
	char ModemBaudRate[11]={0};
	char ModemDataBits[2]={0};
	char ModemPariy[2]={0};
	char PhoneNumber[25]={0};

	//设备基本信息
	DAUInfoStruct *NewDAUInfo=new DAUInfoStruct;
	memset(NewDAUInfo,0,sizeof(DAUInfoStruct));
	NewDAUInfo->next = NULL;
	*DAUInfo = NewDAUInfo;

	GetPrivateProfileString("Device","DAUID","1",DAUID,sizeof(DAUID)-1,"./Communication.ini");
	GetPrivateProfileString("Device","DAUName","设备1",NewDAUInfo->DAUName,
		sizeof(NewDAUInfo->DAUName)-1,"./Communication.ini");
	NewDAUInfo->DAUID = ::atoi(DAUID);

	//板1信息
	NewDAUInfo->pBoard[0] = new BoardInfoStruct;
	GetPrivateProfileString("TransientBoard","CommModel","0",CommModel,sizeof(CommModel)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","IPAddr","192.168.1.141",
		NewDAUInfo->pBoard[0]->IPAddr,
		sizeof(NewDAUInfo->pBoard[0]->IPAddr)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","TcpPort","2404",TcpPort,sizeof(TcpPort)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","COMName","COM1",
		NewDAUInfo->pBoard[0]->COMName,
		sizeof(NewDAUInfo->pBoard[0]->COMName)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","SerialBaudRate","9600",SerialBaudRate,sizeof(SerialBaudRate)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","SerialDataBits","8",SerialDataBits,sizeof(SerialDataBits)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","SerialPariy","n",SerialPariy,sizeof(SerialPariy)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","ModemName","COM1",
		NewDAUInfo->pBoard[0]->ModemName,sizeof(NewDAUInfo->pBoard[0]->ModemName)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","ModemBaudRate","9600",ModemBaudRate,sizeof(ModemBaudRate)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","ModemDataBits","8",ModemDataBits,sizeof(ModemDataBits)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","ModemPariy","n",ModemPariy,sizeof(ModemPariy)-1,"./Communication.ini");
	GetPrivateProfileString("TransientBoard","PhoneNumber","25708154",
		NewDAUInfo->pBoard[0]->PhoneNumber,sizeof(NewDAUInfo->pBoard[0]->PhoneNumber)-1,"./Communication.ini");

	NewDAUInfo->pBoard[0]->BoardID = 1;
	NewDAUInfo->pBoard[0]->BoardType = 1;
	NewDAUInfo->pBoard[0]->CommModel = CommModel[0];
	NewDAUInfo->pBoard[0]->ModemBaudRate = ::atoi(ModemBaudRate);
	NewDAUInfo->pBoard[0]->ModemDataBits = ::atoi(ModemDataBits);
	NewDAUInfo->pBoard[0]->ModemPariy = ModemPariy[0];
	NewDAUInfo->pBoard[0]->SeriesBaudRate = atoi(SerialBaudRate);
	NewDAUInfo->pBoard[0]->SeriesDataBits = ::atoi(SerialDataBits);
	NewDAUInfo->pBoard[0]->SeriesPariy = SerialPariy[0];
	NewDAUInfo->pBoard[0]->TcpPort = atoi(TcpPort);

	NewDAUInfo->pBoard[0]->ProtocalType = 2;
	NewDAUInfo->pBoard[0]->DSPNum = 1;
	NewDAUInfo->pBoard[0]->IsActive = true;
	

	//稳态板信息
	NewDAUInfo->pBoard[1] = new BoardInfoStruct;
	GetPrivateProfileString("ConstBoard","CommModel","0",CommModel,sizeof(CommModel)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","IPAddr","192.168.1.142",
		NewDAUInfo->pBoard[1]->IPAddr,
		sizeof(NewDAUInfo->pBoard[1]->IPAddr)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","TcpPort","2404",TcpPort,sizeof(TcpPort)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","COMName","COM1",
		NewDAUInfo->pBoard[1]->COMName,
		sizeof(NewDAUInfo->pBoard[1]->COMName)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","SerialBaudRate","9600",SerialBaudRate,sizeof(SerialBaudRate)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","SerialDataBits","8",SerialDataBits,sizeof(SerialDataBits)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","SerialPariy","n",SerialPariy,sizeof(SerialPariy)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","ModemName","COM1",
		NewDAUInfo->pBoard[1]->ModemName,sizeof(NewDAUInfo->pBoard[1]->ModemName)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","ModemBaudRate","9600",ModemBaudRate,sizeof(ModemBaudRate)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","ModemDataBits","8",ModemDataBits,sizeof(ModemDataBits)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","ModemPariy","n",ModemPariy,sizeof(ModemPariy)-1,"./Communication.ini");
	GetPrivateProfileString("ConstBoard","PhoneNumber","25708154",
		NewDAUInfo->pBoard[1]->PhoneNumber,sizeof(NewDAUInfo->pBoard[1]->PhoneNumber)-1,"./Communication.ini");

	NewDAUInfo->pBoard[1]->BoardID = 1;
	NewDAUInfo->pBoard[1]->BoardType = 2;
	NewDAUInfo->pBoard[1]->CommModel = CommModel[0];
	NewDAUInfo->pBoard[1]->ModemBaudRate = ::atoi(ModemBaudRate);
	NewDAUInfo->pBoard[1]->ModemDataBits = ::atoi(ModemDataBits);
	NewDAUInfo->pBoard[1]->ModemPariy = ModemPariy[0];
	NewDAUInfo->pBoard[1]->SeriesBaudRate = atoi(SerialBaudRate);
	NewDAUInfo->pBoard[1]->SeriesDataBits = ::atoi(SerialDataBits);
	NewDAUInfo->pBoard[1]->SeriesPariy = SerialPariy[0];
	NewDAUInfo->pBoard[1]->TcpPort = atoi(TcpPort);

	NewDAUInfo->pBoard[1]->ProtocalType = 2;
	NewDAUInfo->pBoard[1]->DSPNum = 1;
	NewDAUInfo->pBoard[1]->IsActive = false;

	return 0;
}

int CFileDB::GetAllCfg(SystemCfg **ppSysCfg)
{
	char	attrCode[30] = {0};		//属性代码
	char	attrValue[300] = {0};	//属性值

	SystemCfg *pNewCfg = new SystemCfg();
	*ppSysCfg = pNewCfg;

	::strcpy(pNewCfg->attrCode,"DATAPATH");
	GetPrivateProfileString("Device","DATAPATH","c:\\data",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"IPADDRFORDAU");
	GetPrivateProfileString("Device","IPADDRFORDAU","192.168.1.124",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"IPADDRFORRMT");
	GetPrivateProfileString("Device","IPADDRFORRMT","192.168.1.124",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"MODEMBAUDRATE");
	GetPrivateProfileString("Device","MODEMBAUDRATE","115200",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"MODEMCOMNAMEFORRMT");
	GetPrivateProfileString("Device","MODEMCOMNAMEFORRMT","COM1",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"MODEMDATABITS");
	GetPrivateProfileString("Device","MODEMDATABITS","COM1",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"MODEMPARIY");
	GetPrivateProfileString("Device","MODEMPARIY","n",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"PARIY");
	GetPrivateProfileString("Device","PARIY","n",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"TCPPORTFORDAU");
	GetPrivateProfileString("Device","TCPPORTFORDAU","1503",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"TCPPORTFORRMT");
	GetPrivateProfileString("Device","TCPPORTFORRMT","3405",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"UDPPORTFORDAU");
	GetPrivateProfileString("Device","UDPPORTFORDAU","1501",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = new SystemCfg();
	pNewCfg = pNewCfg->next;
	::strcpy(pNewCfg->attrCode,"UDPPORTFORRLTDATA");
	GetPrivateProfileString("Device","UDPPORTFORRLTDATA","1504",
		pNewCfg->attrValue,sizeof(pNewCfg->attrValue)-1,"./Communication.ini");

	pNewCfg->next = NULL;

	return 0;
}

int CFileDB::ReleaseMemery(void *pVariant)
{
	delete pVariant;
	pVariant = NULL;
	return 0;
}

int CFileDB::GetToBeSendRpt2Rmt(const char *rmtIpAddr,RptStruct **RptInfo)
{
	*RptInfo = NULL;
	return -1;
}

int CFileDB::AddSendFaultInf(SendFaultInfo *pSendFaultInf)
{
	return -1;
}

int CFileDB::GetRptBySql(char *pSql,RptStruct **RptInfo)
{
	*RptInfo = NULL;
	return -1;
}

int CFileDB::GetRptById(unsigned char dauId, int rptId, RptStruct **RptInfo)
{
	*RptInfo = NULL;
	return -1;
}

int CFileDB::GetAllTimerCallInf(TimerCall **ppTimerCallInf)
{
	*ppTimerCallInf = NULL;
	return -1;
}

int CFileDB::GetMaxFileNo(unsigned char aduId,unsigned char fileType)
{
	return 0;
}

int CFileDB::AddCallFileInf(PCallFileInf pFileInf)
{
	return -1;
}

int CFileDB::GetNoCallFileByType(unsigned char dauId,unsigned char type,CallFileInf **ppFileInf)
{
	*ppFileInf = NULL;
	return -1;
}

int CFileDB::UpdateCallFileInfById(unsigned char DAUID, char *fileName, bool IsCallData)
{
	return -1;
}

int CFileDB::GetMarkedEvtBySql(char *pSql,MarkedEvtStruct **MarkedEvtInfo)
{
	*MarkedEvtInfo = NULL;
	return -1;
}
