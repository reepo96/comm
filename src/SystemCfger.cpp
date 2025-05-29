// {{{RME classifier 'Logical View::SystemCfger'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "SystemCfger.h"
#endif

#include <RTSystem/Communication.h>
#include <SystemCfger.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SystemCfger.cpp
* 文件标识：
* 摘    要：系统配置类，用于保存系统的配置信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/
#include <time.h>
#include "Command.h"
#include <direct.h>

// }}}USR
// }}}RME

// {{{RME classAttribute '_instance'
SystemCfger SystemCfger::_instance;
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateCopyConstructor'
SystemCfger::SystemCfger( const SystemCfger & rtg_arg )
	: rmtLinkType( rtg_arg.rmtLinkType )
	, ADULinkType( rtg_arg.ADULinkType )
	, tcpPortForADU( rtg_arg.tcpPortForADU )
	, udpPortForADU( rtg_arg.udpPortForADU )
	, tcpPortForRemote( rtg_arg.tcpPortForRemote )
	, udpPortForRemote( rtg_arg.udpPortForRemote )
	, listenNum( rtg_arg.listenNum )
	, TransientDAUInfoMap( rtg_arg.TransientDAUInfoMap )
	, baud( rtg_arg.baud )
	, parity( rtg_arg.parity )
	, dataBit( rtg_arg.dataBit )
	, stopBit( rtg_arg.stopBit )
	, timeOut( rtg_arg.timeOut )
{
	for( int rtg_addrForADU_index = (16); --rtg_addrForADU_index >= 0; )
		addrForADU[ rtg_addrForADU_index ] = rtg_arg.addrForADU[ rtg_addrForADU_index ];
	for( int rtg_addrForRemote_index = (16); --rtg_addrForRemote_index >= 0; )
		addrForRemote[ rtg_addrForRemote_index ] = rtg_arg.addrForRemote[ rtg_addrForRemote_index ];
	for( int rtg_comNameForRmt_index = (5); --rtg_comNameForRmt_index >= 0; )
		comNameForRmt[ rtg_comNameForRmt_index ] = rtg_arg.comNameForRmt[ rtg_comNameForRmt_index ];
	for( int rtg_phoneNumForRmt_index = (20); --rtg_phoneNumForRmt_index >= 0; )
		phoneNumForRmt[ rtg_phoneNumForRmt_index ] = rtg_arg.phoneNumForRmt[ rtg_phoneNumForRmt_index ];
	for( int rtg_DataFilePath_index = (255); --rtg_DataFilePath_index >= 0; )
		DataFilePath[ rtg_DataFilePath_index ] = rtg_arg.DataFilePath[ rtg_DataFilePath_index ];
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateAssignmentOperator'
SystemCfger & SystemCfger::operator=( const SystemCfger & rtg_arg )
{
	if( this != &rtg_arg )
	{
		rmtLinkType = rtg_arg.rmtLinkType;
		ADULinkType = rtg_arg.ADULinkType;
		for( int rtg_addrForADU_index = (16); --rtg_addrForADU_index >= 0; )
			addrForADU[ rtg_addrForADU_index ] = rtg_arg.addrForADU[ rtg_addrForADU_index ];
		tcpPortForADU = rtg_arg.tcpPortForADU;
		udpPortForADU = rtg_arg.udpPortForADU;
		for( int rtg_addrForRemote_index = (16); --rtg_addrForRemote_index >= 0; )
			addrForRemote[ rtg_addrForRemote_index ] = rtg_arg.addrForRemote[ rtg_addrForRemote_index ];
		tcpPortForRemote = rtg_arg.tcpPortForRemote;
		udpPortForRemote = rtg_arg.udpPortForRemote;
		listenNum = rtg_arg.listenNum;
		TransientDAUInfoMap = rtg_arg.TransientDAUInfoMap;
		for( int rtg_comNameForRmt_index = (5); --rtg_comNameForRmt_index >= 0; )
			comNameForRmt[ rtg_comNameForRmt_index ] = rtg_arg.comNameForRmt[ rtg_comNameForRmt_index ];
		baud = rtg_arg.baud;
		parity = rtg_arg.parity;
		dataBit = rtg_arg.dataBit;
		stopBit = rtg_arg.stopBit;
		for( int rtg_phoneNumForRmt_index = (20); --rtg_phoneNumForRmt_index >= 0; )
			phoneNumForRmt[ rtg_phoneNumForRmt_index ] = rtg_arg.phoneNumForRmt[ rtg_phoneNumForRmt_index ];
		timeOut = rtg_arg.timeOut;
		for( int rtg_DataFilePath_index = (255); --rtg_DataFilePath_index >= 0; )
			DataFilePath[ rtg_DataFilePath_index ] = rtg_arg.DataFilePath[ rtg_DataFilePath_index ];
	}
	return *this;
}
// }}}RME

// {{{RME operation 'SystemCfger()'
SystemCfger::SystemCfger( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR

		//更改本地目录为当前目录
		char szCurDir[_MAX_PATH];
		GetModuleFileName( GetModuleHandle(NULL), szCurDir, _MAX_PATH );
		char *pPostfix = strrchr(szCurDir, '\\');
		*pPostfix = '\0';
		_chdir(szCurDir);

		iMaxAutoConnDevNum = 30;
		iMaxConnDevNum = 80;
		maxLenOfAutoAnaWaveFile = 5;
		timeOut = 13*1000;

		InitIniFileCfg();
	// }}}USR
}
// }}}RME

// {{{RME operation '~SystemCfger()'
SystemCfger::~SystemCfger( void )
{
	// {{{USR
	map< unsigned int, PDAUINFO >::iterator it = TransientDAUInfoMap.begin();
	for(;it != TransientDAUInfoMap.end();it++)
	{
		PDAUINFO pDauInfo = (*it).second;
		delete pDauInfo;
		pDauInfo = NULL;
	}
	TransientDAUInfoMap.clear();

	map< unsigned int, PDAUINFO >::iterator constIt = ConstantDAUInfoMap.begin();
	for(;constIt != ConstantDAUInfoMap.end();constIt++)
	{
		PDAUINFO pDauInfo = (*constIt).second;
		delete pDauInfo;
		pDauInfo = NULL;
	}
	ConstantDAUInfoMap.clear();

	map< string,vector<PREPORTHEAD>*>::iterator reportIt = toSendRmtReport.begin();
	for(;reportIt != toSendRmtReport.end();reportIt++)
	{
		vector<PREPORTHEAD> *pReportList = (*reportIt).second;
		vector<PREPORTHEAD>::iterator reportHeadIt = pReportList->begin();
		for(;reportHeadIt != pReportList->end();reportHeadIt++)
		{
			PREPORTHEAD pReprotHead = (*reportHeadIt);
			delete pReprotHead;
			pReprotHead = NULL;
		}
		pReportList->clear();
		delete pReportList;
		pReportList = NULL;
	}
	toSendRmtReport.clear();

	map< unsigned char,PCFGTIMERCALL>::iterator timerCallIt = DauTimerCallMap.begin();
	for(;timerCallIt != DauTimerCallMap.end();timerCallIt++)
	{
		PCFGTIMERCALL pTimerCall = (*timerCallIt).second;
		delete pTimerCall;
		pTimerCall = NULL;
	}
	DauTimerCallMap.clear();

	//boardMap
	map< unsigned int, PDAUINFO >::iterator boardIt = DAUInfoMap.begin();
	for(;boardIt != DAUInfoMap.end();boardIt++)
	{
		PDAUINFO pDauInfo = (*boardIt).second;
		delete pDauInfo;
		pDauInfo = NULL;
	}
	DAUInfoMap.clear();
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetInstance()'
SystemCfger & SystemCfger::GetInstance( void )
{
	// {{{USR
	    return _instance;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Rebuild(int)'
void SystemCfger::Rebuild( int type )
{
	// {{{USR
	        switch(type)
	        {
	        case 1:
	            InitSysCfg();
	            break;
	        case 2:
				//InitDauInfo();
				InitDauInfo2();
				break;
			case 3:
				InitOtherCfg();
				InitTimerCallInf();
				break;
			default:
	            break;
	        }

	// }}}USR
}
// }}}RME

// {{{RME operation 'Init()'
void SystemCfger::Init( void )
{
	// {{{USR
	listenNum =15;
	InitSysCfg();
	//InitDauInfo();
	InitDauInfo2();

	InitOtherCfg();
	InitTimerCallInf();
	// }}}USR
}
// }}}RME

// {{{RME operation 'InitDauInfo()'
void SystemCfger::InitDauInfo( void )
{
	// {{{USR
	    //获得DAU配置信息
/*	    DAUInfoStruct *pDAUDBInfo = NULL;

	    if(GetAllDau > 0 )
	        GetAllDau(&pDAUDBInfo);

		map<unsigned int,unsigned int> ValidDevIdMap; //有效的设备
		unsigned int devId = 0;

	    while(pDAUDBInfo != NULL)
	    {
			devId = pDAUDBInfo->DAUID;
			ValidDevIdMap.insert( map<unsigned int,unsigned int>::value_type(devId,devId));

			bool bHasAddIni = false;
			for(int i=0;i<2;i++)
			{
				PBoardInfo pBoard = NULL;
				if(i ==0)
				{
					pBoard = pDAUDBInfo->pTransientBoard;
				}
				else
				{
					pBoard = pDAUDBInfo->pSteadyBoard;
				}

				if(pBoard == NULL)
					continue;

				PDAUINFO pDauInfo = GetDAUInfo(devId,pBoard->BoardType);
				if( pDauInfo == NULL)//dau相关配置不存在
				{
					pDauInfo = new DAUINFO();
					memset(pDauInfo,0,sizeof(DAUINFO));

					//初始化配置文件
					if(!bHasAddIni)
					{
						m_IniMgr.AddDevice(pDAUDBInfo->StationID,devId,DataFilePath);
						bHasAddIni = true;
					}
				}
				else //dau相关配置已存在
				{
					if( pDauInfo->linkType != (CFG_LINK_TYPE)pBoard->CommModel ||
						::strcmp(pDauInfo->addr,pBoard->IPAddr) != 0 || 
						pDauInfo->tcpPort != pBoard->TcpPort || 
						::strcmp(pDauInfo->comName,pBoard->COMName) != 0 || 
						pDauInfo->baud != pBoard->SeriesBaudRate || pDauInfo->dataBit != pBoard->SeriesDataBits ||
						pDauInfo->parity != pBoard->SeriesPariy || strcmp(pDauInfo->phoneNum,pBoard->PhoneNumber) != 0)
					{
						pDauInfo->isRebuild = true; //相关配置已修改
					}
					else
					{
						pDauInfo->isRebuild = false;
					}
				}

				pDauInfo->DAUId = devId;
				pDauInfo->StationId = pDAUDBInfo->StationID;
				memcpy(pDauInfo->DAUName,pDAUDBInfo->DAUName,sizeof(pDauInfo->DAUName)-1);
				memcpy(pDauInfo->DAUType,pDAUDBInfo->DAUType,sizeof(pDauInfo->DAUType)-1);
				pDauInfo->linkType = (CFG_LINK_TYPE)pBoard->CommModel;
				memcpy(pDauInfo->addr,pBoard->IPAddr,strlen(pBoard->IPAddr));
				pDauInfo->tcpPort = pBoard->TcpPort;

				memcpy(pDauInfo->comName,pBoard->COMName,strlen(pBoard->COMName));
				pDauInfo->baud = pBoard->SeriesBaudRate;
				pDauInfo->dataBit = pBoard->SeriesDataBits;
				pDauInfo->parity = pBoard->SeriesPariy;

				memcpy(pDauInfo->phoneNum,pBoard->PhoneNumber,strlen(pBoard->PhoneNumber));

				if(pBoard->BoardType == 1)
				{
					pDauInfo->BoardType = 1;
					TransientDAUInfoMap.insert(map<unsigned int, PDAUINFO>::value_type(pDauInfo->DAUId,pDauInfo));
				}
				else
				{
					pDauInfo->BoardType = 2;
					ConstantDAUInfoMap.insert(map<unsigned int, PDAUINFO>::value_type(pDauInfo->DAUId,pDauInfo));
				}

				releaseMemery((void*)pBoard);
			}

	        DAUInfoStruct *pTmpDAUInfo = NULL;
	        pTmpDAUInfo = pDAUDBInfo;
	        pDAUDBInfo = pTmpDAUInfo->next;
	        releaseMemery((void*)pTmpDAUInfo);
	        pTmpDAUInfo = NULL;
	    }

		//删除不存在的设备信息
		map< unsigned int, PDAUINFO >::iterator devIt = TransientDAUInfoMap.begin();
		for(unsigned int i=0;i<TransientDAUInfoMap.size();i++)
		{
			unsigned int tmpDevId = (*devIt).first;
			if(ValidDevIdMap.find( tmpDevId ) == ValidDevIdMap.end())
			{
				//删除暂态设备信息
				map< unsigned int, PDAUINFO >::iterator tmpIt = devIt;
				tmpIt++;

				PDAUINFO pDauInf = (*devIt).second;
				if(pDauInf != NULL)
				{
					delete pDauInf;
				}
				pDauInf = NULL;
				TransientDAUInfoMap.erase(devIt);

				devIt = tmpIt;

				//删除稳态设备信息
				tmpIt = ConstantDAUInfoMap.find(tmpDevId);
				if( tmpIt != ConstantDAUInfoMap.end() )
				{
					pDauInf = (*tmpIt).second;
					if(pDauInf != NULL)
					{
						delete pDauInf;
					}
					pDauInf = NULL;
					ConstantDAUInfoMap.erase(tmpIt);
				}
			}
			else
			{
				devIt++;
			}
		}

		ValidDevIdMap.clear();
	// }}}USR
*/
	
}
// }}}RME

// {{{RME operation 'InitSysCfg()'
void SystemCfger::InitSysCfg( void )
{
	// {{{USR
	    ADULinkType = CFG_SOCKET_LINK;
	    memset(DataFilePath,0,sizeof(DataFilePath));
	    memcpy(DataFilePath,"c:/data/",strlen("c:/data/"));

		memset(modemComName,0,sizeof(modemComName));

	    //获得系统配置信息
	    SystemCfg *pSysCfgInf = NULL;

	    if(GetAllCfg > 0)
	        GetAllCfg(&pSysCfgInf);

	    while(pSysCfgInf != NULL)
	    {
	        if(strcmp(pSysCfgInf->attrCode,"DATAPATH") == 0)
	        {
	            memset(DataFilePath,0,sizeof(DataFilePath));
	            memcpy(DataFilePath,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"IPADDRFORDAU") == 0)
	        {
	            memset(addrForADU,0,sizeof(addrForADU));
	            memcpy(addrForADU,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
	            else if(strcmp(pSysCfgInf->attrCode,"TCPPORTFORDAU") == 0)
	        {
	            tcpPortForADU = ::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"UDPPORTFORDAU") == 0)
	        {
	            udpPortForADU = ::atoi(pSysCfgInf->attrValue);
	        }
	            else if(strcmp(pSysCfgInf->attrCode,"UDPPORTFORRLTDATA") == 0)
	        {
	            udpPortForRltData = ::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"COMMMDFORRMT") == 0)//与远程通讯的连接方式
	        {
	            rmtLinkType = (CFG_LINK_TYPE)::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"IPADDRFORRMT") == 0)
	        {
	            memset(addrForRemote,0,sizeof(addrForRemote));
	            memcpy(addrForRemote,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"TCPPORTFORRMT") == 0)
	        {
	            tcpPortForRemote = ::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"COMNAMEFORRMT") == 0)
	        {
	            memset(comNameForRmt,0,sizeof(comNameForRmt));
	            memcpy(comNameForRmt,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"BAUDRATE") == 0)
	        {
	            baud = ::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"DATABITS") == 0)
	        {
	            dataBit = ::atoi(pSysCfgInf->attrValue);
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"PARIY") == 0)
	        {
	            parity = pSysCfgInf->attrValue[0];
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"PHONENUMFORRMT") == 0)
	        {
	            memset(phoneNumForRmt,0,sizeof(phoneNumForRmt));
	            memcpy(phoneNumForRmt,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
			else if(strcmp(pSysCfgInf->attrCode,"MODEMCOMNAMEFORRMT") == 0)
	        {
	            memset(modemComName,0,sizeof(modemComName));
	            memcpy(modemComName,pSysCfgInf->attrValue,strlen(pSysCfgInf->attrValue));
	        }
			else if(strcmp(pSysCfgInf->attrCode,"MODEMBAUDRATE") == 0)
	        {
	           modemBaud = atoi(pSysCfgInf->attrValue);
	        }
			else if(strcmp(pSysCfgInf->attrCode,"MODEMPARIY") == 0)
	        {
	            modemParity = pSysCfgInf->attrValue[0];
	        }
			else if(strcmp(pSysCfgInf->attrCode,"MODEMDATABITS") == 0)
	        {
	            modemDataBit = ::atoi(pSysCfgInf->attrValue);
	        }
			else if(strcmp(pSysCfgInf->attrCode,"TCPPORTFORDAUAUTOSEND") == 0)
	        {
	            tcpPortForADUAutoSend = ::atoi(pSysCfgInf->attrValue);
	        }

			modemStopBit = 1;

	        SystemCfg *pTmpCfgInf = pSysCfgInf;
	        pSysCfgInf = pTmpCfgInf->next;
	        releaseMemery((void*)pTmpCfgInf);
	        pTmpCfgInf = NULL;
	    }
	// }}}USR
}
// }}}RME

// {{{RME operation 'InitTimerCallInf()'
void SystemCfger::InitTimerCallInf( void )
{
	// {{{USR
	TimerCall *pTimerCallInf = NULL;

	int i = sizeof(struct tm);

	if(getAllTimerCallInf > 0)
		getAllTimerCallInf(&pTimerCallInf);

	while(pTimerCallInf != NULL)
	{
		PCFGTIMERCALL pCfgTimerCall = new CFGTIMERCALL();
		memset(pCfgTimerCall,0,sizeof(CFGTIMERCALL));
		memcpy(pCfgTimerCall,pTimerCallInf,sizeof(CFGTIMERCALL)-sizeof(time_t));

		pCfgTimerCall->ttStartTime = mktime(&pCfgTimerCall->startTime);

		DauTimerCallMap.insert(map<unsigned char,PCFGTIMERCALL>::value_type(pTimerCallInf->DAUID,pCfgTimerCall));

		TimerCall *pTmpTimerCall = pTimerCallInf;
		pTimerCallInf = pTmpTimerCall->next;
		releaseMemery((void*)pTmpTimerCall);
		pTmpTimerCall = NULL;
	}
	// }}}USR
}
// }}}RME

//停用该函数
// {{{RME operation 'GetDAUInfo(unsigned int,int)'
PDAUINFO SystemCfger::GetDAUInfo( unsigned int DAUId, int devType )
{
	// {{{USR
	if(devType == 1)
	{
	    map<unsigned int, PDAUINFO>::iterator it = TransientDAUInfoMap.find(DAUId);
	    if( it != TransientDAUInfoMap.end())
	    {
	        return (*it).second;
	    }
	    else
	    {
	        return NULL;
	    }
	}
	else
	{
		 map<unsigned int, PDAUINFO>::iterator it = ConstantDAUInfoMap.find(DAUId);
	    if( it != ConstantDAUInfoMap.end())
	    {
	        return (*it).second;
	    }
	    else
	    {
	        return NULL;
	    }
	}

	// }}}USR
}
// }}}RME

//停用该函数
// {{{RME operation 'GetDAUInfo(char[16],int)'
PDAUINFO SystemCfger::GetDAUInfo( const char *ipAddr, int devType )
{
	// {{{USR
	if(devType == 1)
	{
		map<unsigned int, PDAUINFO>::iterator it = TransientDAUInfoMap.begin();
		for(;it != TransientDAUInfoMap.end();it++)
		{
			if(0 == strcmp((*it).second->addr,ipAddr))
			{
				return (*it).second;
			}
		}
	}
	else
	{
		map<unsigned int, PDAUINFO>::iterator it = ConstantDAUInfoMap.begin();
		for(;it != ConstantDAUInfoMap.end();it++)
		{
			if(0 == strcmp((*it).second->addr,ipAddr))
			{
				return (*it).second;
			}
		}
	}

	return NULL;

	// }}}USR
}
// }}}RME

// {{{RME operation 'RptStruct2CmdReport(RptStruct *,PREPORTHEAD)'
bool SystemCfger::RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport )
{
	// {{{USR
	if(RptInfo == NULL || pFaultReport == NULL)
		return false;

	pFaultReport->devID = RptInfo->DAUID;
	pFaultReport->rptID = RptInfo->RptID;

	//故障相别及类型
	pFaultReport->report.FPT.isOutArea = false;
	pFaultReport->report.FPT.isValid = true;

	switch(RptInfo->FaultType)
	{
	case 0://A相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 1://B相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 2://B相接地
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 3://BC两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;		
		break;
	case 4://AB两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 5://AC两相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 6://BC两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 7://AB两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 8://AC两相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 9://三相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 888://区外故障
		pFaultReport->report.FPT.isOutArea = true;
		break;	
	default:
		pFaultReport->report.FPT.isValid = false;
		break;
	}

	//跳闸相别
	pFaultReport->report.JPT.isValid = true;
	switch(RptInfo->TripPhase)
	{
	case 0://A相
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 1://B相
		pFaultReport->report.JPT.B_Jump = true;
		break;
	case 2://C相
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 3://BC相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 4://AB相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 5://CA相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 6://ABC相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;	
	default:
		pFaultReport->report.JPT.isValid = false;
	}

	//故障距离
	float scl = (float)atof(RptInfo->Locate);
	memcpy(&pFaultReport->report.SCL[0],&scl,4);
	/*string sLocate(RptInfo->Locate);
	string::size_type pos = sLocate.find_first_of(".");
	if(pos != string::npos)
	{
	string sTmp = sLocate.substr(0,pos);
	int intData = atoi( sTmp.c_str() ); //正数部分

	sTmp = sLocate.substr(pos+1,sLocate.size()-pos-1);
	int decData = atoi( sTmp.c_str() ); //小数部分
	}*/

	//故障时间
	int year = 0 ;
	int month = 0;
	int day = 0;
	int hours = 0;
	int minute =0;
	int second =0;
	int millisecond =0;
	string sFaultTime(RptInfo->FaultTime); //格式：yyyy-mm-dd HH:MM:ss.sss?
	string::size_type pos1 = sFaultTime.find_first_of("-");
	string::size_type pos2 = sFaultTime.find_last_of("-");
	if(pos1 != string::npos && pos2 != string::npos && pos2 > pos1)
	{
		string sTmp = sFaultTime.substr(0,pos1);
		year = atoi(sTmp.c_str());

		sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
		month = atoi(sTmp.c_str());

		pos1 = pos2;
		pos2 = sFaultTime.find(" ",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			day = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			hours = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			minute = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(".",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			second = atoi(sTmp.c_str());

			sTmp = sFaultTime.substr(pos2+1,sFaultTime.size() -pos2 -1);
			millisecond = atoi(sTmp.c_str());
		}

		pFaultReport->report.faultTime.year = year%200;
		pFaultReport->report.faultTime.month = month;
		pFaultReport->report.faultTime.day = day;
		pFaultReport->report.faultTime.hours = hours;
		pFaultReport->report.faultTime.minute = minute;
		pFaultReport->report.faultTime.milliseconds = second*1000 + millisecond;
	}

	strcpy(pFaultReport->report.recWaveFileName,RptInfo->RptFileName);
	strcpy(pFaultReport->report.BASName,RptInfo->FaultComponent);

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'InitReportList(string)'
void SystemCfger::InitReportList( string rmtIP )
{
	// {{{USR
	//初始化将要发送到远程的故障简报列表
	vector<PREPORTHEAD> *pReportList = NULL;
	map< string,vector<PREPORTHEAD>*>::iterator reportIt = toSendRmtReport.find(rmtIP);
	if(reportIt == toSendRmtReport.end()) //待发送到远程的故障简报列表没有初始化
	{
		pReportList = new vector<PREPORTHEAD>; 

		//从数据库读出未发送的故障简报
		if(getToBeSendRpt2Rmt >0 )
		{
			RptStruct *RptInfo = NULL;
			getToBeSendRpt2Rmt(rmtIP.c_str(),&RptInfo);
			while(RptInfo != NULL)
			{
				PREPORTHEAD pFaultReport = new REPORTHEAD();
				memset(pFaultReport,0,sizeof(REPORTHEAD));

				this->RptStruct2CmdReport(RptInfo,pFaultReport);
				pReportList->push_back(pFaultReport);

				//释放资源
				RptStruct *pTmpRptStruct = NULL;
				pTmpRptStruct = RptInfo;
				RptInfo = pTmpRptStruct->next;
				releaseMemery((void*)pTmpRptStruct);
				pTmpRptStruct = NULL;
			}
		}

		toSendRmtReport.insert(map<string,vector<PREPORTHEAD>*>::value_type(rmtIP,pReportList));
	}
	   
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetReportList(string)'
vector< PREPORTHEAD > * SystemCfger::GetReportList( string rmtIP )
{
	// {{{USR
	    vector<PREPORTHEAD> *pReportList = NULL;
	    map< string,vector<PREPORTHEAD>*>::iterator reportIt = toSendRmtReport.find(rmtIP);
	    if(reportIt != toSendRmtReport.end())
	        {
	            pReportList = (*reportIt).second;
	        }

	        return pReportList;
	// }}}USR
}
// }}}RME

// {{{RME operation 'AppendReportList(unsigned int,int)'
void SystemCfger::AppendReportList( unsigned int devId, int fileId )
{
	// {{{USR
	if(getRptById >0 )
	{
		RptStruct *RptInfo = NULL;
		getRptById(devId,fileId,&RptInfo);//从数据库查找故障简报

		if(RptInfo != NULL)
		{
			PREPORTHEAD pFaultReport = new REPORTHEAD();
			memset(pFaultReport,0,sizeof(REPORTHEAD));

			this->RptStruct2CmdReport(RptInfo,pFaultReport);

			//将简报添加到所有列表中
			map< string,vector<PREPORTHEAD>*>::iterator it = toSendRmtReport.begin();
			for(;it != toSendRmtReport.end();it++)
			{
				vector<PREPORTHEAD>* pList = (*it).second;

				//列表中的元素过多，表示列表可能不再使用，删除掉
				while(pList != NULL && pList->size() > 100)
				{
					vector<PREPORTHEAD>::iterator listIt = pList->begin();
					for(;listIt != pList->end(); listIt++)
					{
						PREPORTHEAD pReport = (*listIt);
						delete pReport;
						pReport = NULL;
					}
					pList->clear();
					delete pList;
					pList = NULL;

					map< string,vector<PREPORTHEAD>*>::iterator tmpIt = it;
					tmpIt ++;
					toSendRmtReport.erase(it);

					it = tmpIt;
					if(it != toSendRmtReport.end())
					{
						pList = (*it).second;
					}//end if
				} //end while

				if(pList != NULL)
				{
					pList->push_back(pFaultReport);
				}
			}//end for

			//释放资源
			RptStruct *pTmpRptStruct = NULL;
			pTmpRptStruct = RptInfo;
			RptInfo = pTmpRptStruct->next;
			releaseMemery((void*)pTmpRptStruct);
			pTmpRptStruct = NULL;
		}//end if(RptInfo != NULL)
	}//endif(getRptById >0 )

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

void SystemCfger::InitOtherCfg()
{
	//获得系统配置信息
	    SystemCfg *pSysCfgInf = NULL;

	    if(GetAllCfg > 0)
	        GetAllCfg(&pSysCfgInf);

	    while(pSysCfgInf != NULL)
	    {
	        if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_+S_V-") == 0)
	        {
				AutoSendRptCfg.PSV_LowerLimit=( atoi(pSysCfgInf->attrValue) > 0) ? true:false;
	        }
	        else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_+S_V+") == 0)
	        {
				AutoSendRptCfg.PSV_UpperLimit =( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_AD_CH-") == 0)
	        {
	            AutoSendRptCfg.DCB_LowerLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_AD_CH+") == 0)
	        {
	            AutoSendRptCfg.DCB_UpperLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_F_-") == 0)
	        {
	            AutoSendRptCfg.F_LowerLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_F_+") == 0)
	        {
	            AutoSendRptCfg.F_UpperLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_F_CH") == 0)
	        {
	            AutoSendRptCfg.F_ChangeRate = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_I_-") == 0)
	        {
	            AutoSendRptCfg.C_LowerLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_I_+") == 0)
	        {
	            AutoSendRptCfg.C_UpperLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_I_CH") == 0)
	        {
	            AutoSendRptCfg.C_Break = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_I_UP") == 0)
	        {
	            AutoSendRptCfg.C_OverLimit = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_P") == 0)
	        {
	            AutoSendRptCfg.Reverse_power = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_S_AC") == 0)
	        {
	            AutoSendRptCfg.ZSACV = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_-S_AC") == 0)
	        {
	            AutoSendRptCfg.NSACV = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_SHOCK") == 0)
	        {
	            AutoSendRptCfg.Shock = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_SW_CH") == 0)
	        {
	            AutoSendRptCfg.SWCH = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_V_CH") == 0)
	        {
	            AutoSendRptCfg.V_Break = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_ZONE_IN") == 0)
	        {
	            AutoSendRptCfg.Fault_InZone = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }
			else if(strcmp(pSysCfgInf->attrCode,"AUTOCALL_ZONE_OUT") == 0)
	        {
	            AutoSendRptCfg.Fault_OutZone = ( atoi(pSysCfgInf->attrValue) >0) ? true:false;
	        }

	        SystemCfg *pTmpCfgInf = pSysCfgInf;
	        pSysCfgInf = pTmpCfgInf->next;
	        releaseMemery((void*)pTmpCfgInf);
	        pTmpCfgInf = NULL;
	    }
}

bool SystemCfger::IsCandAutoSendRpt(unsigned short TrgTypeNum)
{
	return true;
}

void SystemCfger::InitIniFileCfg( void )
{
	char fileSize[8] = {0};
	char szLogLevel[20] = {0};
	GetPrivateProfileString("Log","LogLevel","ERROR",szLogLevel,sizeof(szLogLevel),"./Communication.ini");
	GetPrivateProfileString("Log","MaxFileSize","10",fileSize,sizeof(fileSize),"./Communication.ini");

	logFileMaxSize = ::atoi(fileSize);

	logLevel = 1;
	if(strcmp(szLogLevel,"NONE") == 0)
	{
		logLevel = 0;
	}
	else if(strcmp(szLogLevel,"ERROR") == 0)
	{
		logLevel = 1;
	}
	else if(strcmp(szLogLevel,"WARNING") == 0)
	{
		logLevel = 2;
	}
	else if(strcmp(szLogLevel,"INFO") == 0)
	{
		logLevel = 3;
	}
	else if(strcmp(szLogLevel,"DEBUG") == 0)
	{
		logLevel = 4;
	}
	else if(strcmp(szLogLevel,"DETAIL") == 0)
	{
		logLevel = 5;
	}

	//通讯协议的设定
	char szProtocol[128] = {0};
	GetPrivateProfileString("System","RMT_Protocol","Protocol104_GB",
		szProtocol,sizeof(szProtocol),"./Communication.ini");

	if( strcmp(szProtocol,"Protocol103_HB") == 0 )
	{
		rmtProtocolType = PRO_103_HB;
	}
	else if( strcmp(szProtocol,"Protocol104_GB") == 0 )
	{
		rmtProtocolType = PRO_104_GB;
	}
	else if( strcmp(szProtocol,"Protocol104_ZJ") == 0 )
	{
		rmtProtocolType = PRO_104_ZJ;
	}
	else
	{
		rmtProtocolType = PRO_104_GB;
	}
}

// }}}USR
// }}}RME
// }}}RME

void SystemCfger::InitDauInfo2(unsigned int paraDevId)
{
	DAUInfoStruct *pDAUDBInfo = NULL;
	if( paraDevId == 0 && GetAllDau >0)
		GetAllDau(&pDAUDBInfo);
	else if( paraDevId > 0 && GetOneDau >0)
		GetOneDau(paraDevId,&pDAUDBInfo);
	
	map<unsigned int,unsigned int> ValidDevIdMap; //有效的设备
	unsigned int devId = 0;
	while(pDAUDBInfo != NULL)
	{
		devId = pDAUDBInfo->DAUID;
		//	ValidDevIdMap.insert( map<unsigned int,unsigned int>::value_type(devId,devId));

		bool bHasAddIni = false;
		for(int i=0;i<4;i++)
		{
			PBoardInfo pBoard = NULL;
			pBoard = pDAUDBInfo->pBoard[i];
			if(pBoard == NULL||!pBoard->IsActive) 
			{
				if(pBoard)
				{
					releaseMemery((void*)pBoard);
				}
				continue;
			}

			ValidDevIdMap.insert( map<unsigned int,unsigned int>::value_type(pBoard->BoardID,pBoard->BoardID));
			PDAUINFO pDauInfo = GetDAUInfo(pBoard->BoardID);
			if( pDauInfo == NULL)//dau相关配置不存在
			{
				pDauInfo = new DAUINFO();
				memset(pDauInfo,0,sizeof(DAUINFO));
			}
			else //dau相关配置已存在
			{
				if( pDauInfo->linkType != (CFG_LINK_TYPE)pBoard->CommModel ||
					::strcmp(pDauInfo->addr,pBoard->IPAddr) != 0 || 
					pDauInfo->tcpPort != pBoard->TcpPort || 
					::strcmp(pDauInfo->comName,pBoard->COMName) != 0 || 
					pDauInfo->baud != pBoard->SeriesBaudRate || pDauInfo->dataBit != pBoard->SeriesDataBits ||
					pDauInfo->parity != pBoard->SeriesPariy || strcmp(pDauInfo->phoneNum,pBoard->PhoneNumber) != 0||
					pDauInfo->BoardType != pBoard->BoardType||pDauInfo->ProtocalType != pBoard->ProtocalType)
				{
					pDauInfo->isRebuild = true; //相关配置已修改
					bHasAddIni = true;
				}
				else
				{
					pDauInfo->isRebuild = false;
				}
			}

			pDauInfo->DAUId = devId;
			pDauInfo->StationId = pDAUDBInfo->StationID;
			memcpy(pDauInfo->DAUName,pDAUDBInfo->DAUName,sizeof(pDauInfo->DAUName)-1);
			memcpy(pDauInfo->DAUType,pDAUDBInfo->DAUType,sizeof(pDauInfo->DAUType)-1);
			pDauInfo->linkType = (CFG_LINK_TYPE)pBoard->CommModel;
			memcpy(pDauInfo->addr,pBoard->IPAddr,strlen(pBoard->IPAddr));
			pDauInfo->tcpPort = pBoard->TcpPort;

			memcpy(pDauInfo->comName,pBoard->COMName,strlen(pBoard->COMName));
			pDauInfo->baud = pBoard->SeriesBaudRate;
			pDauInfo->dataBit = pBoard->SeriesDataBits;
			pDauInfo->parity = pBoard->SeriesPariy;

			memcpy(pDauInfo->phoneNum,pBoard->PhoneNumber,strlen(pBoard->PhoneNumber));

			pDauInfo->BoardId = pBoard->BoardID;  //保存 板号
			pDauInfo->BoardType = pBoard->BoardType;
			pDauInfo->ProtocalType = pBoard->ProtocalType;
			pDauInfo->IsActive  = pBoard->IsActive;


			DAUInfoMap.insert(map<unsigned int, PDAUINFO>::value_type(pDauInfo->BoardId,pDauInfo));
			releaseMemery((void*)pBoard);
		}

		//初始化定值文件配置文件
		if(NULL==m_IniMgr.GetDevice(devId)) //不存在则添加
		{
			m_IniMgr.AddDevice(pDAUDBInfo->StationID,devId,DataFilePath);
		}

		DAUInfoStruct *pTmpDAUInfo = NULL;
		pTmpDAUInfo = pDAUDBInfo;
		pDAUDBInfo = pTmpDAUInfo->next;
		releaseMemery((void*)pTmpDAUInfo);
		pTmpDAUInfo = NULL;
	}

	if( paraDevId > 0 )
	{
		ValidDevIdMap.clear();
		return;
	}

	//删除不存在的设备信息
	map< unsigned int, PDAUINFO >::iterator devIt = DAUInfoMap.begin();
	for(unsigned int i=0;i<DAUInfoMap.size();i++)
	{
		unsigned int tmpBoardId = (*devIt).first;
		if(ValidDevIdMap.find( tmpBoardId ) == ValidDevIdMap.end())
		{
			//删除暂态设备信息
			map< unsigned int, PDAUINFO >::iterator tmpIt = devIt;
			tmpIt++;

			PDAUINFO pDauInf = (*devIt).second;
			if(pDauInf != NULL)
			{
				delete pDauInf;
			}
			pDauInf = NULL;
			DAUInfoMap.erase(devIt);

			devIt = tmpIt;
		}
		else
		{
			devIt++;
		}
	}

	ValidDevIdMap.clear();
}

PDAUINFO SystemCfger::GetDAUInfo(unsigned int BoardId)
{
	
	map<unsigned int, PDAUINFO>::iterator it = DAUInfoMap.find(BoardId);
	if( it != DAUInfoMap.end())
	{
		return (*it).second;
	}
	else
	{
		return NULL;
	}
}

PDAUINFO SystemCfger::GetDAUInfo(const char *ipAdd)
{
	
	map<unsigned int, PDAUINFO>::iterator it = DAUInfoMap.begin();
	for(;it != DAUInfoMap.end();it++)
	{
		if(0 == strcmp((*it).second->addr,ipAdd))
		{
			return (*it).second;
		}
	}
	return NULL;
	
}
