/*************************************************************************
* Copyright (c) 2007,

* All rights reserved.
* 
* 文件名称：Protocol61850.cpp
* 文件标识：

* 摘    要：解析61850协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2009年5月9日

**************************************************************************/

#include "Protocol61850.h"
#include "shtypes.h"
#include "reqptr.h"
#include "filedir.h"
#include "dfr_mmsexdata.h"
#include "dfr_req.h"
#include "MsgQueue.h"
#include "MemMapMgr.h"
#include "Linker.h"

#include "StringRes.h"

map<unsigned int ,bool>	Protocol61850::m_RltEnableList;
map<unsigned int ,bool>	Protocol61850::m_ZeroOffsetEnableList;
CRITICAL_SECTION *	Protocol61850::m_pMapSection = NULL;

Protocol::FUNCODE2CMD pro61850Fun2CmdMap[] = {
	{CMD_GETWAVEFILELIST,&(Protocol61850::CallGetRecWaveFileList),(CStringRes::GetInstance())->GetStrById(IDS_CALL_REC_FILELIST,"调录波文件列表")},
	{CMD_GETDAUINF,&Protocol61850::CallGetDauInf,(CStringRes::GetInstance())->GetStrById(IDS_QUY_DEVSTAT,"查询录波器运行状态信息")},
	{CMD_SETTIME,&Protocol61850::CallSetTime,(CStringRes::GetInstance())->GetStrById(IDS_TIME_SYN,"时间同步(主到从)")},
	{CMD_GETTIME,&Protocol61850::CallGetTime,(CStringRes::GetInstance())->GetStrById(IDS_GET_TIME,"获取时间")},
	{CMD_GETWAVEFILE,&Protocol61850::CallGetRecWaveFile,(CStringRes::GetInstance())->GetStrById(IDS_CALL_REC_FILE,"召唤录波文件")},
	{CMD_GETFILELIST,&Protocol61850::CallGetFileList,(CStringRes::GetInstance())->GetStrById(IDS_CALL_REC_FILELIST,"召唤通用文件列表")},
	{CMD_STOPREC,&Protocol61850::CallStopRecWave,(CStringRes::GetInstance())->GetStrById(IDS_DEV_TEST,"设备调试")},
	{CMD_STARTREC_MANUAL,&Protocol61850::CallStartRecByManual,(CStringRes::GetInstance())->GetStrById(IDS_MANUAL_STAR_REC,"手动启动录波")},
	{CMD_GET_CONT_DATA,&Protocol61850::CallGetContData,(CStringRes::GetInstance())->GetStrById(IDS_GET_CONT_DATA,"读动态纪录数据")},
	{CMD_PULLFILE,&Protocol61850::CallPullFile,(CStringRes::GetInstance())->GetStrById(IDS_GET_FILE,"获取文件")},
	{CMD_PUSHFILE,&Protocol61850::CallPushFile,(CStringRes::GetInstance())->GetStrById(IDS_PUSH_FILE,"下传文件")},
	{CMD_READ_RLT_DATA,&Protocol61850::CallReadRltData,(CStringRes::GetInstance())->GetStrById(IDS_RT_SHOW,"实时显示")},
	{CMD_READ_ZEROOFFSET,&Protocol61850::CallReadZeroOffset,(CStringRes::GetInstance())->GetStrById(IDS_GET_OFFSET,"读实零漂刻度")},
	{CMD_SET_DEVICE_STATE,&Protocol61850::CallSetDeviceState,(CStringRes::GetInstance())->GetStrById(IDS_RESUM_RUN,"恢复运行")},
	{CMD_OPERATE_DEVICE,&Protocol61850::CallOperateDevice,(CStringRes::GetInstance())->GetStrById(IDS_DEV_OPERATE,"硬件操作")}
};

Protocol61850::Protocol61850(const char *pCfgFile,const char *pDeviceName,const char *pARName)
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::Protocol61850(const char *pCfgFile,const char *pDeviceName,const char *pARName)");

	m_pIEC61850_Adaptor = NULL;
	memset(RES_COMTRADE_DIR,0,sizeof(RES_COMTRADE_DIR));
	memset(RES_COMTRADE_DIV_DIR,0,sizeof(RES_COMTRADE_DIV_DIR));
	memset(RES_SHD_DIR,0,sizeof(RES_SHD_DIR));
	memset(RES_MARKENV_DIR,0,sizeof(RES_MARKENV_DIR));
	memset(RES_CONFIG_DIR,0,sizeof(RES_CONFIG_DIR));
	memset(RES_REPORT_DIR,0,sizeof(RES_REPORT_DIR));
	memset(RES_PROGRAM_DIR,0,sizeof(RES_PROGRAM_DIR));

	memset(RES_STARTREC_CTRL,0,sizeof(RES_STARTREC_CTRL));
	memset(RES_DEVSTATE_CTRL,0,sizeof(RES_DEVSTATE_CTRL));
	memset(RES_DEVOPER_CTRL,0,sizeof(RES_DEVOPER_CTRL));

	if (m_pMapSection == NULL)
	{
		mylog.WriteLog2File(LogControl::LDEBUG, "Create CRITICAL_SECTION:m_pMapSection in Protocol61850::Protocol61850()");

		m_pMapSection = new CRITICAL_SECTION;
		InitializeCriticalSection(m_pMapSection);

		mylog.WriteLog2File(LogControl::LDEBUG, "After init CRITICAL_SECTION:m_pMapSection");
	}

	m_pDeviceName = pDeviceName;//保存设备名

	m_bCanSendRltData = false;
	m_bCanSendZeroOffsetData = false;
	m_bAdaptorHasIni = false;

	reloadOsiCfg(pCfgFile);

	/*int ret = 0;
	//初始化ACSI环境
	ret = initACSIEnvironment( pCfgFile);
	if ( ret != 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR, "initACSIEnvironment return fail");
		return;
	}

	//启动ACSI通讯线程
	ret = startACSICommThread();
	if ( ret != 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR, "startACSICommThread return fail");
		return;
	}*/

	m_pIEC61850_Adaptor = new SHDFR_B_RemoteServer( pARName );

	//设置实时波形数据处理函数
	m_pIEC61850_Adaptor->setRtDataProcFunc(AutoRltData);
	//设置标志事件处理函数
	m_pIEC61850_Adaptor->setMrkProcFunc(AutoMarkEvt);
	//设置启动信息处理函数
	m_pIEC61850_Adaptor->setTrgProcFunc(AutoStartInf);

	GetPrivateProfileString("Dir","Comtrade","/LDTrns/comtrade_all/",RES_COMTRADE_DIR,sizeof(RES_COMTRADE_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","ComtradeDiv","/LDTrns/comtrade_div/",RES_COMTRADE_DIV_DIR,sizeof(RES_COMTRADE_DIV_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","shd","/LDTrns/trns_raw_data/",RES_SHD_DIR,sizeof(RES_SHD_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","MarkEnv","/LDCont/event/",RES_MARKENV_DIR,sizeof(RES_MARKENV_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Config","/conf/",RES_CONFIG_DIR,sizeof(RES_CONFIG_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Report","/LDTrns/fault_report/",RES_REPORT_DIR,sizeof(RES_REPORT_DIR),"./IEC61850.ini");
	GetPrivateProfileString("Dir","Program","/data/",RES_PROGRAM_DIR,sizeof(RES_PROGRAM_DIR),"./IEC61850.ini");

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::Protocol61850(const char *pCfgFile,const char *pDeviceName,const char *pARName)");
}

Protocol61850::~Protocol61850(void)
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::~Protocol61850(void)");
	if(m_pIEC61850_Adaptor != NULL)
	{
		m_pIEC61850_Adaptor->clear();
		delete m_pIEC61850_Adaptor;
		m_pIEC61850_Adaptor = NULL;
	}
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::~Protocol61850(void)");
}

bool Protocol61850::Connect()
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::Connect()");

	if(m_pIEC61850_Adaptor == NULL)
		return false;

	bool	bRt = false;

	if(0 == m_pIEC61850_Adaptor->connect())
	{
		if( !m_bAdaptorHasIni)
		{
			if(0 == m_pIEC61850_Adaptor->init() )
			{
				m_bAdaptorHasIni = true;

				mylog.WriteLog2File(LogControl::LINFO, "Connect and init success in Protocol61850::Connect()");
				bRt = true;
			}
			else
			{
				m_pIEC61850_Adaptor->disconnect();

				mylog.WriteLog2File(LogControl::LERROR, "Connect success but init fail in Protocol61850::Connect()");
				bRt = false;
			}
		}
		else
		{
			mylog.WriteLog2File(LogControl::LINFO, "Connect success in Protocol61850::Connect()");
			bRt = true;
		}
	}
	else
	{
		mylog.WriteLog2File(LogControl::LERROR, "Connect error in Protocol61850::Connect()");
		bRt = false;
	}


	if( bRt )
	{
		const char *pLDName = m_pIEC61850_Adaptor->getTrnsLdName();

		if ( pLDName != NULL )
		{
			strncpy(RES_STARTREC_CTRL,pLDName,sizeof(RES_STARTREC_CTRL));
			strncpy(RES_DEVSTATE_CTRL,pLDName,sizeof(RES_DEVSTATE_CTRL));
			strncpy(RES_DEVOPER_CTRL,pLDName,sizeof(RES_DEVOPER_CTRL));

			char szTmpCtlVar[64] = {0};
			GetPrivateProfileString("Control","StartRec","LDTrns/RDRE1.RcdTrg",szTmpCtlVar,sizeof(szTmpCtlVar),"./IEC61850.ini");
			strncat(RES_STARTREC_CTRL,szTmpCtlVar,sizeof(RES_STARTREC_CTRL) - strlen(pLDName));
			
			GetPrivateProfileString("Control","SetDevState","LDTrns/LLN0.Mod",szTmpCtlVar,sizeof(szTmpCtlVar),"./IEC61850.ini");
			strncat(RES_DEVSTATE_CTRL,szTmpCtlVar,sizeof(RES_DEVSTATE_CTRL) - strlen(pLDName));

			GetPrivateProfileString("Control","DevOper","LDTrns/LLN0.LEDRs",szTmpCtlVar,sizeof(szTmpCtlVar),"./IEC61850.ini");
			strncat(RES_DEVOPER_CTRL,szTmpCtlVar,sizeof(RES_DEVOPER_CTRL) - strlen(pLDName));
		}
	}

	return bRt;
}

bool Protocol61850::Close()
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::Close()");

	if(m_pIEC61850_Adaptor == NULL)
		return false;

	if( !m_pIEC61850_Adaptor->isConnected() )
	{
		return true;
	}

	if(0 == m_pIEC61850_Adaptor->disconnect())
	{
		return true;
	}
	return false;
}

Task * Protocol61850::Decode( Linker * pLinker )
{
	IsError( -1 ,pLinker);	//检测链接是否正常

	return NULL;
}

bool Protocol61850::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::exuteCmd( )");

	//获取命令对应的映射索引

	int index = -1;
	int mapCount = (sizeof(pro61850Fun2CmdMap)/sizeof(FUNCODE2CMD));
	for(int i=0;i<mapCount;i++)
	{
		if(pro61850Fun2CmdMap[i].cmdFunCode == cmd.funCode )
		{
			index = i;
			break;
		}
	}

	if( index < 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command in Protocol61850::ExecuteCmd()");
		return false;
	}

	strcpy(cmd.describe,pro61850Fun2CmdMap[index].cmdName);
	return pro61850Fun2CmdMap[index].exuteCmd(this,pLinker,cmd); //执行命令函数
}

bool Protocol61850::GetDauInf( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetDauInf( )");

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		return false;
	}

	//分析命令参数
	if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetDauInf()");
		return false;
	}

	bool bHasErr = false;
	CHECK checkInf;
	memset(&checkInf,0,sizeof(checkInf));
//	checkInf.mm_err_no = 1<<10;
//	checkInf.md_err_no = 1<<10;

	DFR_WARN_INFO warninf;
	DSP_STATUS_INFO statusInf;
	memset(&warninf,0,sizeof(warninf));
	memset(&statusInf,0,sizeof(statusInf));

	do
	{
		//获取自检信息
		int ret = m_pIEC61850_Adaptor->readDFRWarnInfo(&warninf,SystemCfger::GetInstance().timeOut/1000);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Read warning info fail.");
			bHasErr = true;
			break;
		}
		//printf("111 Get DSP warninf(%X)\n",warninf.DSPWarn);

		//获得版本信息
		ret = m_pIEC61850_Adaptor->readDFRStatusInfo(&statusInf,SystemCfger::GetInstance().timeOut/1000);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Read status info info fail.");
			bHasErr = true;
			break;
		}
		//printf("222 Get DSP warninf(%X)\n",warninf.DSPWarn);

		//获得时间信息
		ret = m_pIEC61850_Adaptor->readSysTime((int*)&checkInf.time_sec,SystemCfger::GetInstance().timeOut/1000);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Read time info fail.");
			bHasErr = true;
			break;
		}
		printf("333 Get DSP warninf(%X)\n",warninf.DSPWarn);
	}while(0);

	printf("444 Get DSP warninf(%X)\n",warninf.DSPWarn);

	PCMDRESULT pCmdResult = NULL;
	PCMDRESULT pCheckResult = NULL;

	pCmd->resultNum = 1;
	pCmdResult = new CMDRESULT();
	pCmdResult->resultType = CDRT_TRUFALSE;
	pCmdResult->resultLen = 1;
	pCmdResult->pData = new char[2];
	pCmdResult->pData[0] = !bHasErr;
	pCmdResult->pData[1] = 0;
	pCmd->resultList.push_back(pCmdResult);

	if( !bHasErr )
	{
		printf("555 Get DSP warninf(%X)\n",warninf.DSPWarn);

		//将自检信息返回给主界面.
		pCheckResult = new CMDRESULT();

		checkInf.dsp_ver = statusInf.DSPVer;
		checkInf.Module_ver[0] = statusInf.MonVer;
		checkInf.Module_ver[1] = statusInf.MonVer;
		checkInf.dsp_ver = statusInf.DSPVer;
	//	checkInf.md_ver = statusInf.MonVer;

		printf("666 Get DSP warninf(%X)\n",warninf.DSPWarn);

		memcpy(&checkInf.dsp_err_no,&warninf.DSPWarn,sizeof(warninf.DSPWarn));


		if( pCmd->to ==  CD_CONST_DAU)
		{
			printf("Copy CWarn info.\n");
			memcpy(&checkInf.m_err_no,&warninf.CWarn,sizeof(warninf.CWarn));
		}
		else
		{
			printf("Copy TWarn info.\n");
			memcpy(&checkInf.m_err_no,&warninf.TWarn,sizeof(warninf.TWarn));
		}

		printf("777 Get DSP warninf(%X)\n",warninf.DSPWarn);

		checkInf.dsp_err_no = checkInf.dsp_err_no | 1;
	//	checkInf.mm_err_no = checkInf.mm_err_no | 1;
	//	checkInf.md_err_no = checkInf.md_err_no | 1;

		pCheckResult->resultType = CDRT_CHAR;
		pCheckResult->resultLen = sizeof(CHECK);
		pCheckResult->pData = new char[pCheckResult->resultLen];
		memcpy(pCheckResult->pData,&checkInf,pCheckResult->resultLen);
		pCmd->resultNum += 1;
		pCmd->resultList.push_back(pCheckResult);
	}

	//如果此命令被分成两份执行，则等待另一部分的执行结果
/*	if( pCmd->isSplitCmd )
	{
		printf("The cmd is splited.\n");

		Task *pNewTask = new Task;
		pNewTask->Linker = pLinker;
		pCmd->Clone( &(pNewTask->cmd) );

		/*--------->自检信息需要将暂态和稳态部分的信息整合一起<----------*/
	/*	MsgQueue *pResultQue = QueRegister::GetInstance().RegisterQue(COMM_INSIDE_RESULTQUE_NAME);
		Task *pAnotherTask = pResultQue->FindMsg(pCmd->msgId);
		if( pAnotherTask == NULL)
		{
			//另一部分信息还没有得到，先将自己这部分信息保存到队列中
			printf("Send the check info to Queue.(cmd is sent to:%d)\n",pCmd->to);

			if( false == pResultQue->SendMsg(pNewTask))
			{
				pResultQue->ClearAll();
				pResultQue->SendMsg(pNewTask);
			}
			//改成来自内部的命令(就不会将计算结果返回主界面)
			pCmd->from = CD_NEURON;
		}
		else
		{
			//已经获得另一部分信息
			delete pNewTask;
			pNewTask = NULL;

			CHECK newCheckInf;
			memset(&newCheckInf,0,sizeof(newCheckInf));

			//只有一个结果，说明前一个命令执行失败；本次执行失败也直接返回失败
			if( pAnotherTask->cmd.resultList.size() <= 1 || bHasErr || pCheckResult == NULL )
			{
				printf("pAnotherTask result size:%d,bHasErr:%d,pCheckResult:%d.\n",
					pAnotherTask->cmd.resultList.size(),bHasErr,pCheckResult);

				delete pAnotherTask;
				pAnotherTask = NULL;
				return !bHasErr;
			}

			pCmdResult = NULL;
			vector< PCMDRESULT >::iterator it = pAnotherTask->cmd.resultList.begin();
			for(; it != pAnotherTask->cmd.resultList.end();it++ )
			{
				pCmdResult = (*it);
				if( pCmdResult->resultType == CDRT_CHAR )
					break;
			}

			if( pCmdResult == NULL || pCmdResult->resultType != CDRT_CHAR || pCmdResult->resultLen != sizeof(CHECK) )
			{
				delete pAnotherTask;
				pAnotherTask = NULL;
				return false;
			}

			memcpy(&newCheckInf,pCmdResult->pData,sizeof(newCheckInf));

			//将自身信息和另一部分的信息整合起来
			if( pCmd->to ==  CD_CONST_DAU)
			{
				//自身是稳态的命令
				printf("Copy new trans check info\n");
				checkInf.mm_ver = newCheckInf.mm_ver;
				checkInf.mm_err_no = newCheckInf.mm_err_no;
			}
			else
			{
				//自身暂态的命令
				printf("Copy new const check info\n");
				checkInf.md_ver = newCheckInf.md_ver;
				checkInf.md_err_no = newCheckInf.md_err_no;
			}
			memcpy(pCheckResult->pData,&checkInf,pCheckResult->resultLen);

			delete pAnotherTask;
			pAnotherTask = NULL;
		}//end if( pAnotherTask == NULL)
	}//end if( !bHasErr )
	*/
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::GetDauInf()");
	return true;
}

bool Protocol61850::GetRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetRecWaveFile( )");

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		return false;
	}

	//分析命令参数
	if(pCmd->parmLen != 45 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::GetRecWaveFile()");
		return false;
	}

	char szLogInf[312] = {0};
	unsigned char recWaveType = pCmd->pParm[0];	//录波类型
	char pFileName[41] = {0};//不含扩展名的文件名

	memcpy(pFileName,&pCmd->pParm[1],40);
	int  fileNameLen = strlen(pFileName);
	unsigned int   startPos = 0;//起始传输位置
	memcpy(&startPos,&pCmd->pParm[41],4);

	FileControl fileCtrl;
	string sFilePathWithoutExt("");	//不带扩展名的录波文件路径
	bool bCfgHasDown = false;	//cfg文件已经下载
	bool bDatHasDown = false;	//dat文件已经下载
	int iMaxFileLen = 0;				//最大的文件长度

	//获取comtrade文件目录
	unsigned int devId = 0;

	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		return "";
	}
	else
	{
		devId = pDauInf->DAUId;
	}

	string comtradeDir("");
	if( recWaveType == 3 )//分通道录波文件
	{
		comtradeDir = fileCtrl.GetFileTypeDirector(devId,FileControl::FT_COMTRADE_DIV);
	}
	else
	{
		comtradeDir = fileCtrl.GetFileTypeDirector(devId,FileControl::FT_COMTRADE);
	}

	if(comtradeDir.compare("") == 0)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "get the director of file type fail in Protocol61850::GetRecWaveFile()");
		pLinker->Close();
		return false;
	}

	//远程文件
	string rmtComtradeDir("");//远程comtrade目录
	if( recWaveType == 3 )//分通道录波文件
	{
		rmtComtradeDir = string(RES_COMTRADE_DIV_DIR);
	}
	else
	{
		rmtComtradeDir = string(RES_COMTRADE_DIR);
	}

	//分析文件时间
	string sTimes[5];	//保存分析之后的时间串：年、月、日、时、分
	char *pToken[5] = {"-","-","_","-","-"};//分割符
	int  iSubDirVal = 0;//时间子目录对应的数值
	int  iYearMonth = 0;
	char szTimeDir[8] = {0};

	string sFileTime(pFileName);
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;

	//服务器目录结构是/年月/日/时/分，如：200910/08/13/20
	for(int i=0;i<5;i++)
	{
		pos2 = sFileTime.find(pToken[i],pos1);
		if( pos2 == string::npos )
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error filename format in Protocol61850::GetRecWaveFile()");
			return false;
		}

		sTimes[i] = sFileTime.substr(pos1,pos2-pos1);
		pos1 = pos2 + 1;

		if( i == 0 )
			continue;

		if( i == 1 )
		{
			iSubDirVal = ::atoi(sTimes[0].c_str())*100 + ::atoi(sTimes[1].c_str());
			iYearMonth = iSubDirVal;
		}
		else
		{
			iSubDirVal = ::atoi(sTimes[i].c_str());
		}
		
		sprintf(szTimeDir,"%02d/",iSubDirVal);
		rmtComtradeDir = rmtComtradeDir + string(szTimeDir);
	}
	
	//本地目录是只有年月子目录，如：/200910/
	sprintf(szTimeDir,"%d/",iYearMonth);
	string sPath = comtradeDir +string(szTimeDir);
	if(!fileCtrl.IsExist( sPath.c_str() )) //检查目录是否存在
	{
		if(-1 == _mkdir(sPath.c_str()))
		{
			string sInf("Create directory:");
			sInf = sInf + sPath + string(" failed in GetRecWaveFile()");
			mylog.WriteLog2File(LogControl::LERROR, sInf.c_str());

			m_errCode = PEC_UNKOWN;
			pLinker->Close();
			return false;
		}
	}

	//获取简报文件目录
	/*string sReportDir = fileCtrl.GetFileTypeDirector(pCmd->destDevId,FileControl::FT_REPORT);
	sReportDir += string(szTimeDir);
	if(!fileCtrl.IsExist( sReportDir.c_str() )) //检查目录是否存在
	{
		if(-1 == _mkdir(sReportDir.c_str()))
		{
			string sInf("Create directory:");
			sInf = sInf + sReportDir + string(" failed in GetRecWaveFile()");
			mylog.WriteLog2File(LogControl::LERROR, sInf.c_str());

			m_errCode = PEC_UNKOWN;
			pLinker->Close();
			return false;
		}
	}
	string sLReportFile = sReportDir + string(pFileName) + string(".xml");
	string sRReportFile = string(RES_REPORT_DIR) + string(szTimeDir) + string(pFileName) + string(".xml");
	*/

	sFilePathWithoutExt = comtradeDir +string(szTimeDir) + string(pFileName);

	//本地文件
	string lcaCfgFileName = sFilePathWithoutExt + string(".CFG");
	string lcaDateFileName = sFilePathWithoutExt + string(".DAT");
	string lcaHdrFileName = sFilePathWithoutExt + string(".HDR");
	string lcaInfFileName = sFilePathWithoutExt + string(".INF");
	string lcaShcFileName = sFilePathWithoutExt + string(".SHC");
	string lcaSheFileName = sFilePathWithoutExt + string(".SHE");

	string rmtCfgFileName = rmtComtradeDir + string(pFileName) + string(".CFG");
	string rmtDateFileName = rmtComtradeDir + string(pFileName) + string(".DAT");
	string rmtHdrFileName = rmtComtradeDir + string(pFileName) + string(".HDR");
	string rmtInfFileName = rmtComtradeDir + string(pFileName) + string(".INF");
	string rmtShcFileName = rmtComtradeDir + string(pFileName) + string(".SHC");
	string rmtSheFileName = rmtComtradeDir + string(pFileName) + string(".SHE");

	string  localFiles[6] ={lcaCfgFileName,lcaDateFileName,lcaHdrFileName,lcaInfFileName,lcaShcFileName,
		lcaSheFileName};
	string  remotFiles[6] ={rmtCfgFileName,rmtDateFileName,rmtHdrFileName,rmtInfFileName,rmtShcFileName,
		rmtSheFileName};

	bool bHasErr = false;
	int ret = 0;
	PCMDRESULT pResult = NULL;
	vector< PCMDRESULT > fileList;//保存接收到的文件列表

	int iDownFileNum = 6;//下载文件数量（前几个文件）
	if( recWaveType == 3 )//分通道录波文件
	{
		iDownFileNum = 3;
	}
	else
	{
		iDownFileNum = 6;
	}

	for(int i=0;i<iDownFileNum;i++)
	{
		//下载文件
		ret = GetFile(pCmd,localFiles[i].c_str(),remotFiles[i].c_str());
		if( ret <0 )
		{
			bool bIsConnected = false;

			if(m_pIEC61850_Adaptor != NULL)
				bIsConnected = m_pIEC61850_Adaptor->isConnected();

			if(i == 0 || i == 1 || !bIsConnected )//cfg或dat文件下载失败或连接断开
			{
				vector< PCMDRESULT >::iterator it = fileList.begin();
				for(;it != fileList.end();it++)
				{
					PCMDRESULT pTmpResult = (*it);
					delete []pTmpResult->pData;
					pTmpResult->pData = NULL;
					delete pTmpResult;
					pTmpResult = NULL;
				}
				fileList.clear();

				pCmd->resultNum = 0;
				pCmd->resultList.clear();

				if( !bIsConnected )
				{
					pLinker->Close();
					mylog.WriteLog2File(LogControl::LERROR, "Down Rec File fail,because linker disconnect");
				}

				mylog.WriteLog2File(LogControl::LERROR, "Down CFG or DAT file fail");
				return false;
			}
			else//其它文件下载失败
			{
				continue;
			}
		}

		//保存文件路径名
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_FILEPATH;
		pResult->resultLen = localFiles[i].size();
		pResult->pData = new char[pResult->resultLen+1];
		pResult->pData[pResult->resultLen] = 0 ;
		memcpy(pResult->pData,localFiles[i].c_str(),pResult->resultLen);

		fileList.push_back(pResult);
	}

	pCmd->resultNum = 1 + fileList.size();
	pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	pResult->pData[0] = 1 ; //执行成功
	pResult->pData[1] = 0 ;

	pCmd->resultList.insert(pCmd->resultList.begin(), fileList.begin(),fileList.end());
	pCmd->resultList.push_back(pResult);

	return true;
}

int Protocol61850::GetFile( struct Command * pCmd,const char *locFile,const char *rmtFile )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetFile()");

	assert(pCmd != NULL);
	assert(locFile != NULL);
	assert(rmtFile != NULL);

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		return -1;
	}

	char szLogInf[512] = {0};
	sprintf(szLogInf,"Begin get rmt file:%s,write to local file:%s", rmtFile,locFile);
	mylog.WriteLog2File(LogControl::LDEBUG, szLogInf);

	//GetFileReq fileReq(m_pIEC61850_Adaptor,rmtFile,locFile);
	RequestPtr reqPtr;
	int ret = m_pIEC61850_Adaptor->GetFileA( locFile, rmtFile, &reqPtr);
	if(ret != 0)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor->GetFileA() return fail");
		return -1;
	}

	return PutOrGetFileInf( pCmd, 2,&reqPtr);
}

bool Protocol61850::GetRecWaveFileList( Linker * pLinker, struct Command * pCmd )
{
	if(GetAllDau <= 0 )
		return false;

	//从数据库查询当天录波文件
	char szSql[255] = {0};
	time_t ltime;
	struct tm *today = NULL;
	time( &ltime );
	today = localtime( &ltime );
	if( today == NULL)
		return false;

	sprintf(szSql,"select * from SHRpt,SHDAU Where SHRpt.DAUID=SHDAU.DAUID and FaultTime >= '%d-%02d-%02d %02d:00:00' and FaultTime <= '%d-%02d-%02d %02d:59:59' ",
		today->tm_year+1900,today->tm_mon+1,today->tm_mday,today->tm_hour,today->tm_year+1900,today->tm_mon+1,today->tm_mday,today->tm_hour);
	
	string	sAllFileNames = string("");
	PRptInfo pRpt = NULL; 
	getRptBySql(szSql,&pRpt);

	while( pRpt != NULL )
	{
		sAllFileNames += string( pRpt->RptFileName);

		PRptInfo pTmpRptInfo = NULL;
		pTmpRptInfo = pRpt;
		pRpt = pTmpRptInfo->next;
		releaseMemery((void*)pTmpRptInfo);
		pTmpRptInfo = NULL;
	}

	//列服务器文件目录
	char szTimeDir[32] = {0};
	sprintf(szTimeDir,"%d/%02d/%02d/",( (today->tm_year+1900)*100 + today->tm_mon+1),today->tm_mday,today->tm_hour);

	string sRemoteDir = string(RES_COMTRADE_DIR);
	sRemoteDir += string(szTimeDir);

	FileDirNode *pDirNode = NULL;
	FileDirTree dirTree;
	dirTree.setRootPath( sRemoteDir.c_str() );

 	int ret = m_pIEC61850_Adaptor->GetFileDir(&dirTree,NULL,1,18);
	if( IsError(ret,pLinker) )
	{
		mylog.FormatLogInf(LogControl::LERROR, "GetFileDir(%s) return fail in Protocol61850::GetRecWaveFileList()",sRemoteDir.c_str());
		return false;
	}

	pDirNode = dirTree.firstChild();

	vector< CMDFILELIST > NoInFileList;
	FindFileInStr(pCmd->destDevId,pDirNode,sAllFileNames.c_str(),&NoInFileList);
	dirTree.clear();

	vector< CMDFILELIST >::iterator it = NoInFileList.begin();
	for(;it != NoInFileList.end();it++)
	{
		PCMDRESULT pCmdResult = new CMDRESULT;
		memset(pCmdResult,0,sizeof(CMDRESULT));

		pCmdResult->pData  = new char[sizeof(CMDFILELIST)+1];
		memset(pCmdResult->pData,0,sizeof(CMDFILELIST)+1);

		pCmdResult->resultType = CDRT_FILELIST;//返回的结果是文件列表类型
		pCmdResult->resultLen = sizeof(CMDFILELIST);
		memcpy(&pCmdResult->pData[0],&(*it),sizeof(CMDFILELIST)); //复制数据

		pCmd->resultNum++;
		pCmd->resultList.push_back(pCmdResult);
	}

	if( NoInFileList.size() > 0)
	{
		NoInFileList.clear();
		return true;
	}
	else
	{
		return false;
	}
}

bool Protocol61850::SetTime( Linker * pLinker, struct Command * pCmd )
{
	//对时
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::SetTime()");

	bool result = true;

	//分析命令参数
	if(pCmd->parmLen != 1+1+sizeof(struct SHTimeStruct) || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::SetTime()");
		return false;
	}

	struct SHTimeStruct shtSetTime;
	memset(&shtSetTime,0,sizeof(struct SHTimeStruct));
	memcpy(&shtSetTime,&pCmd->pParm[2],sizeof(struct SHTimeStruct));

	struct tm tmSetTime;
	memset(&tmSetTime,0,sizeof(struct tm));
	tmSetTime.tm_year = shtSetTime.Year - 1900;
	tmSetTime.tm_mon = shtSetTime.Month -1;
	tmSetTime.tm_mday = shtSetTime.Day;
	tmSetTime.tm_hour = shtSetTime.Hour;
	tmSetTime.tm_min = shtSetTime.Minute;
	tmSetTime.tm_sec = shtSetTime.Second;

# if defined(_MSC_VER)  &&  (_MSC_VER >= 1400)//VC 2005
	__time32_t ttSetTime = _mktime32(&tmSetTime);
#else
	time_t ttSetTime = mktime(&tmSetTime);
#endif

	int ret = m_pIEC61850_Adaptor->writeSysTime((int*)&ttSetTime);
	if( IsError(ret,pLinker) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "SetTime fail");
		return false;
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::SetTime()");
	return true;
	
}

bool Protocol61850::GetTime( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetTime()");

	//printf("Enter Protocol61850::GetTime( Linker * pLinker, struct Command * pCmd )\r\n");
	if(m_pIEC61850_Adaptor == NULL)
		return false;

	time_t ttTime = 0;
	int ret = m_pIEC61850_Adaptor->readSysTime( (int*)&ttTime,SystemCfger::GetInstance().timeOut );
	if( IsError(ret,pLinker) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "GetTime fail");
		return false;
	}

	struct tm timeData;
	timeData = *gmtime( &ttTime );

	//设置系统时间
	struct CP56Time2a getTime;
	memset(&getTime,0,sizeof(struct CP56Time2a));
	getTime.year = (timeData.tm_year+1900)%100;
	getTime.month = timeData.tm_mon+1;
	getTime.day = timeData.tm_mday;
	getTime.hours = timeData.tm_hour;
	getTime.minute = timeData.tm_min;
	getTime.milliseconds = timeData.tm_sec*1000;

	//写结果

	pCmd->resultNum = 1;
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TIME;
	pResult->resultLen = sizeof(struct CP56Time2a);
	pResult->pData = new char[pResult->resultLen];
	memcpy(pResult->pData,&getTime,pResult->resultLen);
	pCmd->resultList.push_back(pResult);

	return true;
}

bool Protocol61850::StartRecByManual( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::StartRecByManual()");

	if(m_pIEC61850_Adaptor == NULL)
		return false;

	int ret = m_pIEC61850_Adaptor->sendCtrl(RES_STARTREC_CTRL,1,SystemCfger::GetInstance().timeOut);
	if( IsError(ret,pLinker) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "StartRecByManual fail");
		return false;
	}
	return true;
}

bool Protocol61850::StopRecWave( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::StopRecWave()");

	if(m_pIEC61850_Adaptor == NULL)
		return false;

	/*int ret = m_pIEC61850_Adaptor->sendCtrl(RES_STARTREC_CTRL,0,SystemCfger::GetInstance().timeOut);
	if( IsError(ret,pLinker) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "StopRecWave fail");
		return false;
	}*/
	int ret = m_pIEC61850_Adaptor->sendCtrl(RES_DEVSTATE_CTRL,3,30);
	if( IsError(ret,pLinker) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "Switch the device to test state fail");
		return false;
	}
	return true;
}

bool Protocol61850::GetFileList( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetFileList( )");

	if(m_pIEC61850_Adaptor == NULL)
		return false;

	//分析命令参数
	if(pCmd->parmLen != (1+4+sizeof(struct CP56Time2a)*2 +1) || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
		return false;
	}

	unsigned char dirType = pCmd->pParm[0];//目录类型
	unsigned int fileNum = 0; //最近的文件数，0表示目录下所有的文件
	memcpy(&fileNum,&pCmd->pParm[1],4);

	string sCurrentDir("");
	switch(dirType)
	{
	case 90://故障录波原始（SHD）目录
		sCurrentDir = string(RES_SHD_DIR);
		break;
	case 97://故障录波（Comtrade）目录
		sCurrentDir = string(RES_COMTRADE_DIR);
		break;
	case 92://标记事件目录
		sCurrentDir = string(RES_MARKENV_DIR);
		break;
	case 99://分通道目录
		sCurrentDir = string(RES_COMTRADE_DIV_DIR);
		break;
	default:
		mylog.WriteLog2File(LogControl::LERROR,"Unknow directory type");
		return false;
	}

	FileDirNode *pDirNode = NULL;
	FileDirTree dirTree;
	dirTree.setRootPath( sCurrentDir.c_str() );

	int ret = m_pIEC61850_Adaptor->GetFileDir(&dirTree,NULL,0,10);
	if( IsError(ret,pLinker) )
	{
		mylog.FormatLogInf(LogControl::LERROR, "GetFileDir(%s) return fail in Protocol61850::GetFileList()",sCurrentDir.c_str());
		return false;
	}

	pDirNode = dirTree.firstChild();
	vector<const char *> *pFileExtNameList = new vector<const char *>;

	switch(dirType)
	{
	case 90://故障录波原始（SHD）目录
		pFileExtNameList->push_back(".SHD");
		pFileExtNameList->push_back(".shd");
		GetFileFromDirNode(pCmd,&dirTree,pDirNode,true,pFileExtNameList);
		break;
	case 97://故障录波（Comtrade）目录
	case 99://分通道目录
		pFileExtNameList->push_back(".CFG");
		pFileExtNameList->push_back(".cfg");
		GetFileFromDirNode(pCmd,&dirTree,pDirNode,false,pFileExtNameList);
		break;
	case 92://标记事件目录
		pFileExtNameList->push_back(".SOE");
		pFileExtNameList->push_back(".soe");
		GetFileFromDirNode(pCmd,&dirTree,pDirNode,true,pFileExtNameList);
		break;
	default:
		return false;
	}//end switch

	if( pFileExtNameList != NULL )
	{
		delete pFileExtNameList;
		pFileExtNameList = NULL;
	}

	dirTree.clear();

	pCmd->resultNum += 1;
	PCMDRESULT pComdResult = new CMDRESULT();
	pComdResult->resultType = CDRT_TRUFALSE;

	pComdResult->resultLen = 1;
	pComdResult->pData = new char[2];
	memset(pComdResult->pData,0,2);
	pComdResult->pData[0] = 1;
	pCmd->resultList.push_back(pComdResult);
	
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::GetFileList( )");
	return true;
}

bool Protocol61850::GetFileFromDirNode( struct Command * pCmd,FileDirTree *pDirTree,FileDirNode *pDirNode,bool bNeedCpyExt,vector<const char *> *pFileExtNameList )
{
	if(pCmd == NULL || pDirNode == NULL)
		return false;

	bool bIsNeedFile = false; //是否是需要的文件
	const char *pTmpFileName = NULL;
	const char *pExtFileName = NULL;//扩展名

	char szFileTime[25] = {0};
	int	iCopyLen = 0;

	MemMapMgr memMgr;

	while(pDirNode != NULL)
	{
		bIsNeedFile = false;
		if( pDirNode->isFile() )//是文件
		{

			pTmpFileName = pDirNode->getFileName();
			if( pFileExtNameList != NULL && pFileExtNameList->size() > 0)//需要根据扩展名查找文件
			{
				vector<const char *>::iterator it = pFileExtNameList->begin();
				for(;it != pFileExtNameList->end();it++)
				{
					pExtFileName = strstr(pTmpFileName,(*it) );
					if( pExtFileName != NULL)//只要找到一个匹配的扩展名，就退出循环

					{
						if(bNeedCpyExt)
						{
							iCopyLen = strlen(pTmpFileName);
						}
						else
						{
							//不需要扩展文件名，要减除它

							iCopyLen = pExtFileName - pTmpFileName;
						}

						bIsNeedFile = true;
						break;
					}
				}//end for(;it != pFileExtNameList->end();it++)
			}
			else//不需要根据扩展名查找文件
			{
				iCopyLen = strlen(pTmpFileName);
				bIsNeedFile = true;
			}
		}
		else//是目录
		{
			int ret = m_pIEC61850_Adaptor->GetFileDir(pDirTree,pDirNode,0,10);
			if( ret != 0 )
			{
				mylog.FormatLogInf(LogControl::LWARNING, "GetFileDir return fail in Protocol61850::GetFileFromDirNode()");
				return false;
			}

			FileDirNode *pTmpNode = pDirNode->firstChild();
			GetFileFromDirNode(pCmd,pDirTree,pTmpNode,bNeedCpyExt,pFileExtNameList);
		}

		if( bIsNeedFile)
		{
			FileAttr *pFileAttr;
			pFileAttr = pDirNode->getFileAttr();
			if( pFileAttr == NULL)
			{
				mylog.WriteLog2File(LogControl::LERROR,"pDirNode->getFileAttr() return NULL in Protocol60_103::GetFileFromDirNode()");
				return false;
			}

			memset(szFileTime,0,sizeof(szFileTime));
			if(pFileAttr->mtimpres)
			{
				struct tm tmFileTime = *localtime( &(pFileAttr->mtime) );
				sprintf(szFileTime,"%d-%02d-%02d %02d:%02d:%02d",tmFileTime.tm_year+1900,tmFileTime.tm_mon+1,
					tmFileTime.tm_mday,tmFileTime.tm_hour,tmFileTime.tm_min,tmFileTime.tm_sec);
			}

			CMDDIRFILEINF dirFileInf ;
			memset(&dirFileInf,0,sizeof(CMDDIRFILEINF));
			dirFileInf.fileNo = 0;//fileNo;
			memcpy(dirFileInf.fileName,pTmpFileName,iCopyLen);
			memcpy(dirFileInf.fileTime,szFileTime,sizeof(szFileTime));
			dirFileInf.fileLen = (int)pFileAttr->fsize;

			pCmd->resultNum += 1;
			PCMDRESULT pComdResult = new CMDRESULT();
			pComdResult->resultType = CDRT_DIRFILEINF;//返回结果是目录信息类型

			pComdResult->resultLen = sizeof(CMDDIRFILEINF);
			pComdResult->pData = new char[sizeof(CMDDIRFILEINF)+1];
			memset(pComdResult->pData,0,sizeof(CMDDIRFILEINF)+1);

			memcpy(pComdResult->pData,&dirFileInf,sizeof(dirFileInf));

			//pCmd->resultList.clear();
			pCmd->resultList.push_back(pComdResult);

			if( pCmd->resultNum >= 20 )
			{
				memMgr.WriteTaskResultToMem(*pCmd);

				vector< PCMDRESULT >::iterator it = pCmd->resultList.begin();
				for(;it != pCmd->resultList.end();it++)
				{
					PCMDRESULT pTmpResult = (*it);
					if(pTmpResult != NULL)
					{
						if(pTmpResult->pData != NULL)
						{
							delete []pTmpResult->pData;
							pTmpResult->pData = NULL;
						}
						delete pTmpResult;
					}
					pTmpResult = NULL;
				}
				pCmd->resultList.clear();
				pCmd->resultNum = 0;

				::Sleep(1000);
			}
		}

		FileDirNode *pTmpNode = pDirNode;
		pDirNode = pTmpNode->next();
		//delete pTmpNode;
		//pTmpNode = NULL;
	}//end while

	return true;
}

void Protocol61850::FindFileInStr(unsigned char devID, FileDirNode *pDirNode,const char *pAllFileNames,vector< CMDFILELIST > *pNoInFileList )
{
	if(pNoInFileList == NULL || pDirNode == NULL)
		return ;

	const char *pTmpFileName = NULL;
	const char *pExtFileName = NULL;//扩展名
	int iCopyLen = 0;
	CMDFILELIST FileInf;
	memset(&FileInf,0,sizeof(FileInf));

	while(pDirNode != NULL)
	{
		if( pDirNode->isFile() )//是文件
		{
			pTmpFileName = pDirNode->getFileName();
			pExtFileName = strstr(pTmpFileName,".CFG" );
			if( pExtFileName != NULL)
			{
				//不需要扩展文件名，要减除它
				iCopyLen = pExtFileName - pTmpFileName;
				strncpy(FileInf.fileName,pTmpFileName,min(iCopyLen,40) );
				if ( strstr( pAllFileNames,FileInf.fileName ) == NULL )//录波文件不存在于pAllFileNames
				{
					FileInf.devID = devID;

					FileAttr *pFileAttr = pDirNode->getFileAttr();
					if( pFileAttr != NULL && pFileAttr->mtimpres)
					{
						struct tm tmFileTime = *localtime( &(pFileAttr->mtime) );
						tmToCP56Time2a( tmFileTime, FileInf.faultTime );
					}
					pNoInFileList->push_back(FileInf);
				}//end if ( strstr( pAllFileNames,pTmpFileName ) == NULL )
			}//end if( pExtFileName != NULL)
		}
		else//是目录
		{
			FileDirNode *pTmpNode = pDirNode->firstChild();
			FindFileInStr(devID,pTmpNode,pAllFileNames,pNoInFileList);
		}

		FileDirNode *pTmpNode = pDirNode;
		pDirNode = pTmpNode->next();
		//delete pTmpNode;
		//pTmpNode = NULL;
	}//end while
}

string Protocol61850::GetRmtFileDirAndFileType(const unsigned char ucType , FileControl::FILETYPE & fileControlType )
{
	string sRmtDir("");
	switch(ucType)
	{
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		sRmtDir = string(RES_COMTRADE_DIR);
		fileControlType = FileControl::FT_COMTRADE; //标准comtrade文件
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is comtrade file");
		break;
	case 10:
		sRmtDir = string(RES_SHD_DIR);
		fileControlType = FileControl::FT_SHD; //SHD文件
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is SHD file");
		break;
	case 16:
	case 17:
	case 18:
		sRmtDir = string(RES_COMTRADE_DIV_DIR);
		fileControlType = FileControl::FT_COMTRADE_DIV; //comtrade分文件
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is comtrade(div) file");
		break;
	case 101:
		sRmtDir = string(RES_MARKENV_DIR);
		fileControlType = FileControl::FT_SOE; //标记事件文件
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is SOE file");
		break;
	case 210:
	case 211:
	case 212:
	case 220:
		sRmtDir = string(RES_CONFIG_DIR);
		fileControlType = FileControl::FT_CFG; //配置相关文件
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is configer file");
		break;
	default:
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is unkown type file");
		return sRmtDir;
	}
	return sRmtDir;
}

bool Protocol61850::PullFile( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::PullFile()");

	//分析命令参数
	if(pCmd->parmLen != 1+4+4+4+100+19+1 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::PullFile()");
		return false;
	}

	unsigned char fileType = pCmd->pParm[0];//文件类型
	FileControl::FILETYPE fileControlType;	//文件类型
	unsigned int fileNO = 0;//文件号

	unsigned int fileStartPos = 0;//文件开始位置

	unsigned int fileTotalLen = 0;//文件总长度

	char inputFileName[100]={0};//传入的文件名称

	char fileTime[25]={0};//文件时间:字符串,yyyy-mm-dd hh:MM:ss格式
	char IsNeedCompress = 0;//是否需要压缩，1为需要


	string sRmtFileName;	//远程文件名


	memcpy(&fileNO,&pCmd->pParm[1],4);
	memcpy(&fileStartPos,&pCmd->pParm[1+4],4);
	memcpy(&fileTotalLen,&pCmd->pParm[1+4+4],4);
	memcpy(inputFileName,&pCmd->pParm[1+4*3],100);
	memcpy(fileTime,&pCmd->pParm[1+4*3+100],19);
	IsNeedCompress = pCmd->pParm[1+4*3+100+19];

	//根据文件类型获得对应远程文件路径
	sRmtFileName = GetRmtFileDirAndFileType(fileType,fileControlType);
	if( sRmtFileName.compare("") == 0 )
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is unkown type file");
		return false;
	}

	//分析文件时间
	int  iSubDirVal = 0;//时间子目录对应的数值
	int  iFileYearMonth = 0;
	char szTimeDir[8] = {0};

	string sFileTime(fileTime);
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;

	//根据文件时间找到年、月
	pos1 = sFileTime.find("-",0);
	if( pos1 != string::npos )
		pos2 = sFileTime.find("-",pos1+1);

	if( pos1 == string::npos || pos2 == string::npos )
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"Error filetime format in Protocol61850::PullFile()");
		pLinker->Close();
		return false;
	}
	iSubDirVal = ::atoi( sFileTime.substr(0,pos1).c_str() )*100 
		+ ::atoi( sFileTime.substr(pos1+1,pos2-pos1-1).c_str() );
	iFileYearMonth = iSubDirVal;

	//根据文件名找服务器目录结构，其结构是/年月/日/时/分，如：200910/08/13/20
	string sTimes[5];	//保存分析之后的时间串：年、月、日、时、分
	char *pToken[5] = {"-","-","_","-","-"};//分割符

	string sInputFileName(inputFileName);
	pos1 = 0;
	pos2 = 0;

	for(int i=0;i<5;i++)
	{
		switch(fileType)
		{		
		case 101://标记事件文件
		case 210://配置相关文件
		case 211:
		case 212:
		case 220:
			//标记事件和配置相关文件没有日期目录
			i = 6;
			continue;
			break;
		default:
			break;
		}

		pos2 = sInputFileName.find(pToken[i],pos1);
		if( pos2 == string::npos )
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error filename format in Protocol61850::PullFile()");
			pLinker->Close();
			return false;
		}

		sTimes[i] = sInputFileName.substr(pos1,pos2-pos1);
		pos1 = pos2 + 1;

		if( i == 0 )
			continue;

		if( i == 1 )
		{
			iSubDirVal = ::atoi(sTimes[0].c_str())*100 + ::atoi(sTimes[1].c_str());
		}
		else if( i > 1 )
		{
			iSubDirVal = ::atoi(sTimes[i].c_str());
		}
		
		sprintf(szTimeDir,"%02d/",iSubDirVal);
		sRmtFileName += string(szTimeDir);
	}


	FileControl fileCtrl;
	unsigned int devId = 0;
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		return "";
	}
	else
	{
		devId = pDauInf->DAUId;
	}
	string sLocalFile = fileCtrl.GetFileTypeDirector(devId,fileControlType);

	//加上年月目录
	char szYearMonth[7]={0};
	if(fileControlType != FileControl::FT_CFG)
	{
		::itoa(iFileYearMonth,szYearMonth,10);
		sLocalFile += string(szYearMonth) + string("/");
		if(!fileCtrl.IsExist(sLocalFile.c_str()))
		{
			_mkdir(sLocalFile.c_str());
		}
	}

	sLocalFile += string(inputFileName);
	//向监控板取定值文件  监控板定值文件名称统一为 "SHDevice.dat" 
	if(220 ==fileType)
	{
		sRmtFileName += string("SHDevice.dat");
	}
	else
	{
		sRmtFileName += string(inputFileName);
	}

	//下载文件
	int ret = GetFile(pCmd,sLocalFile.c_str(),sRmtFileName.c_str());

	//下载失败
	if( IsError(ret,pLinker) )
	{
		mylog.FormatLogInf(LogControl::LERROR, "Pull file to %s from %s fail",sLocalFile.c_str(),sRmtFileName.c_str());
		return false;
	}

	//下载成功
	pCmd->resultNum = 2;

	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	pResult->pData[0] = 1 ; //执行成功
	pResult->pData[1] = 0 ;
	pCmd->resultList.push_back(pResult);

	pResult = new CMDRESULT();
	pResult->resultType = CDRT_FILEPATH;
	pResult->resultLen = sLocalFile.size();
	pResult->pData = new char[pResult->resultLen+1];
	pResult->pData[pResult->resultLen] = 0 ;
	memcpy(pResult->pData,sLocalFile.c_str(),sLocalFile.size());
	pCmd->resultList.push_back(pResult);

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::PullFile()");
	return true;
}

bool Protocol61850::PushFile( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::PushFile()");

	//分析命令参数
	if(pCmd->parmLen != 1+20+10+100 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PushFile()");
		return false;
	}

	unsigned char fileType = pCmd->pParm[0];	//文件类型
	unsigned char userName[20]={0};			//用户名

	unsigned char passward[10]={0};		//密码
	char szLocalFileName[101]={0};		//本地文件名称(含路径)
	string sLocalFileName;				//本地文件名称(含路径)
	string sFileName("");				//文件名(不含路径)

	memcpy(&userName[0],&pCmd->pParm[1],20);
	memcpy(&passward[0],&pCmd->pParm[1+20],10);
	memcpy(&szLocalFileName[0],&pCmd->pParm[1+20+10],100);

	//解析出文件名称

	sLocalFileName =  string(szLocalFileName);
	int pos1 = sLocalFileName.find_last_of("/");
	int pos2 = sLocalFileName.find_last_of("\\");

	string::size_type pos = max(pos1,pos2);
	if(pos != string::npos)
	{
		sFileName = sLocalFileName.substr(pos+1,sLocalFileName.size() -pos -1);
	}
	else
	{
		sFileName = sLocalFileName;//
	}

	string sRmtFileName("");

	//根据文件类型获得对应远程文件路径
	switch( fileType )
	{
	case 10://定值文件
	case 21:
	case 22:
	case 20:
		sRmtFileName = string(RES_CONFIG_DIR);
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is configer file");
		break;
	case 1://程序文件
	case 4:
		sRmtFileName = string(RES_PROGRAM_DIR);
		mylog.WriteLog2File(LogControl::LDEBUG,"File type is configer file");
		break;
	default:
		break;
	}

	if( sRmtFileName.compare("") == 0 )
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Push file type is unkown type file");
		return false;
	}

	if( fileType == 20 )
	{
		sRmtFileName = sRmtFileName + string("ipconfig.dat");
	}
	else if(10 == fileType)  //向监控板下发定值文件  监控板定值文件名称统一为 "SHDevice.dat"
	{
		sRmtFileName += string("SHDevice.dat");
	}
	else
	{
		sRmtFileName += sFileName;
	}

	int ret = PutFile(pCmd, sLocalFileName.c_str() , sRmtFileName.c_str() );

	//下传失败
	if( IsError(ret,pLinker) )
	{
		mylog.FormatLogInf(LogControl::LERROR, "PushFile %s to %s fail",sLocalFileName.c_str(),sRmtFileName.c_str());
		return false;
	}

	if(fileType == 10)//是定值文件则通知服务端已下定值
	{
		bool bNeedSendToDsp = true;

		if( pCmd->to == CD_CONST_DAU )
		{
			bNeedSendToDsp = false;
		}

		ret = m_pIEC61850_Adaptor->sendReloadSGCMD(bNeedSendToDsp,sRmtFileName.c_str());
		if( IsError(ret,pLinker) )
		{
			mylog.FormatLogInf(LogControl::LERROR, "Note the monitor fail after send configur file");
			return false;
		}
	}
	else if( fileType == 1 ) //DSP程序
	{
		ret = m_pIEC61850_Adaptor->sendUpdateProgCMD( 0,sRmtFileName.c_str(),60*5 );
		if( IsError(ret,pLinker) )
		{
			mylog.FormatLogInf(LogControl::LERROR, "Note the monitor fail after send DSP program file");
			return false;
		}
	}
	else if( fileType == 4 ) //监控板程序
	{
		ret = m_pIEC61850_Adaptor->sendUpdateProgCMD( 1,sRmtFileName.c_str(),60*2 );
		if( IsError(ret,pLinker) )
		{
			mylog.FormatLogInf(LogControl::LERROR, "Note the monitor fail after send Monitor program file");
			return false;
		}
	}
	else if( fileType == 20 ) //组网配置文件
	{
		ret = m_pIEC61850_Adaptor->sendIpConfigCMD( sRmtFileName.c_str(),15 );
		if( IsError(ret,pLinker) )
		{
			mylog.FormatLogInf(LogControl::LERROR, "Note the monitor fail after send Monitor program file");
			return false;
		}
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::PushFile()");
	return true;
}

int Protocol61850::PutFile( struct Command * pCmd,const char *locFile,const char *rmtFile )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::PutFile()");

	assert(pCmd != NULL);
	assert(locFile != NULL);
	assert(rmtFile != NULL);

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		return -1;
	}

	mylog.FormatLogInf(LogControl::LDEBUG, "Begin Put local file:%s write to remot file:%s", locFile,rmtFile);

	//SetFileReq fileReq(m_pIEC61850_Adaptor,rmtFile,locFile);
	RequestPtr reqPtr;
	int ret = m_pIEC61850_Adaptor->SetFileA( locFile, rmtFile, &reqPtr);
	if(ret != 0)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor->SetFileA() return fail");
		return -1;
	}

	return PutOrGetFileInf( pCmd, 1,&reqPtr);
}


int Protocol61850::PutOrGetFileInf(struct Command * pCmd, int iType,const RequestPtr *pReqPtr )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::PutOrGetFileInf()");

	assert(pCmd != NULL );
	assert(pReqPtr != NULL );

	//是取文件或下文件的异步对象才继续
	ReqBase *pBaseReq = pReqPtr->ptr();
	if( pBaseReq == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "The pBaseReq is NULL in Protocol61850::PutOrGetFileInf()");
		return -1;
	}

	while( REQID_GetFile != pBaseReq->reqType() && REQID_SetFile != pBaseReq->reqType() )
	{
		//失败
		if( pBaseReq->isFailed() )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Send or get file fail");
			return -1;
		}

		::Sleep(6);

		pBaseReq = pReqPtr->ptr();
		if( pBaseReq == NULL)
		{
			mylog.WriteLog2File(LogControl::LERROR, "The pBaseReq is NULL in while at Protocol61850::PutOrGetFileInf()");
			return -1;
		}
	}

	FileAttr *pFileAttr = NULL;

	if( iType == 1)
		pFileAttr = ((SetFileReq*)pBaseReq)->fileAttr();
	else
		pFileAttr = ((GetFileReq*)pBaseReq)->fileAttr();

	if( pFileAttr == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "The pFileAttr is NULL in Protocol61850::GetFile()");
		return -1;
	}


	unsigned int uiTotalSize = pFileAttr->fsize;
	int iRecvSize = 0;
	int count =0;
	int iSendCount = 0;
	int iProgress = 0;
	int iStep = 5;	//每次发送进度信息的步长

	MemMapMgr memMgr;

	while(iProgress != 100 )
	{
		count++;
		uiTotalSize = pFileAttr->fsize;
		if( uiTotalSize <= 10*1024*1024 ) //10M
		{
			iStep = 5;
		}
		else if( uiTotalSize <= 50*1024*1024 ) //50M
		{
			iStep = 2;
		}
		else
		{
			iStep = 1;
		}

		//失败
		if( pBaseReq->isFailed() )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Send or get file fail");
			return -1;
		}

		if( iType == 1)
			iRecvSize = ((SetFileReq*)pBaseReq)->getTransBytes();
		else
			iRecvSize = ((GetFileReq*)pBaseReq)->getTransBytes();

		//计算进度
		if( uiTotalSize > 0 )
		{
			iProgress = 100*iRecvSize/uiTotalSize;
		}
		else
		{
			if( count%25 == 0)
			{
				iProgress++;
				if(iProgress >= 100)
				{
					iProgress = 0;
					iSendCount = 0;
				}
			}
		}

		//完成
		if(   pBaseReq->isFinished() )
		{
			iProgress = 100;
		}

		//发送进度信息
		if( (iProgress >= (iSendCount+1)*iStep) ||
			iProgress == 100 )
		{
			iSendCount++;

			pCmd->resultNum = 1;
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_PROGRESS;
			pResult->resultLen = 1;
			pResult->pData = new char[2];
			pResult->pData[1] = '\0';

			pResult->pData[0] = iProgress;
			pCmd->resultList.push_back(pResult);
			memMgr.WriteTaskResultToMem(*pCmd);

			//清除结果
			delete pResult;
			pResult = NULL;
			pCmd->resultNum = 0;
			pCmd->resultList.clear();
		}

		//下传文件完不代表成功
		while( iType == 1 && iProgress == 100 && !pBaseReq->isFinished() && !pBaseReq->isFailed())
		{
			Sleep(20);
		}

		if( iProgress == 100 && pBaseReq->isFailed() )
		{
			return -1;
		}

		::Sleep(20);
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::PutOrGetFileInf()");
	return 0;
}

bool Protocol61850::ReadRltData( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::ReadRltData()");

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		pLinker->Close();
		return false;
	}

	//分析命令参数
	if(pCmd->parmLen != 2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::ReadRltData()");
		return false;
	}

	unsigned char dataType = pCmd->pParm[0];//数据类型
	unsigned char StartType = pCmd->pParm[1];//启动类型
	int ret = 0;

	if(StartType == 1)
	{
		::strcat(pCmd->describe, (CStringRes::GetInstance())->GetStrById(IDS_START,"（启动）") );
		ret = m_pIEC61850_Adaptor->setRTRptEnable(true);
		m_bCanSendRltData = true;
	}
	else
	{
		::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_STOP,"（停止）") );
		ret = m_pIEC61850_Adaptor->setRTRptEnable(false);
		m_bCanSendRltData = false;
	}

	if(m_pMapSection != NULL)
		EnterCriticalSection(m_pMapSection);

	if( IsError(ret,pLinker) )
	{
		m_bCanSendRltData = !m_bCanSendRltData;

		if( m_RltEnableList.find(pCmd->destDevId) == m_RltEnableList.end())
		{
			m_RltEnableList.insert(map<unsigned int,bool>::value_type(pCmd->destDevId,m_bCanSendRltData));
		}
		else
		{
			m_RltEnableList[pCmd->destDevId] = m_bCanSendRltData;
		}
		
		if(m_pMapSection != NULL)
			LeaveCriticalSection(m_pMapSection);

		mylog.WriteLog2File(LogControl::LERROR, "ReadRltData fail");
		return false;
	}

	if( m_RltEnableList.find(pCmd->destDevId) == m_RltEnableList.end())
	{
		m_RltEnableList.insert(map<unsigned int,bool>::value_type(pCmd->destDevId,m_bCanSendRltData));
	}
	else
	{
		m_RltEnableList[pCmd->destDevId] = m_bCanSendRltData;
	}

	if(m_pMapSection != NULL)
		LeaveCriticalSection(m_pMapSection);

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::ReadRltData()");
	return true;
}

bool Protocol61850::ReadZeroOffset( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::ReadZeroOffset()");

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		pLinker->Close();
		return false;
	}

	int ret = 0;

	//实时数据未启动，则启动实时数据
	if( !m_bCanSendRltData)
	{
		ret = m_pIEC61850_Adaptor->setRTRptEnable(true);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "ReadZeroOffset fail");
			return false;
		}
	}

	m_bCanSendZeroOffsetData = true;

	if(m_pMapSection != NULL)
		EnterCriticalSection(m_pMapSection);

	if( m_ZeroOffsetEnableList.end() != m_ZeroOffsetEnableList.find(pCmd->destDevId) )
	{
		m_ZeroOffsetEnableList[pCmd->destDevId] = true;
	}
	else
	{
		m_ZeroOffsetEnableList.insert(map<unsigned int ,bool>::value_type(pCmd->destDevId,m_bCanSendZeroOffsetData));
	}

	if(m_pMapSection != NULL)
		LeaveCriticalSection(m_pMapSection);

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::ReadZeroOffset()");
	return true;
}

bool Protocol61850::IsError( int errorno ,Linker * pLinker)
{
	if( errorno != 0 )
	{
		bool bIsConnected = false;

		if(m_pIEC61850_Adaptor != NULL)
			bIsConnected = m_pIEC61850_Adaptor->isConnected();

		if( !bIsConnected )
		{
			mylog.WriteLog2File(LogControl::LERROR, "The linker has disconnected");
			pLinker->Close();
		}

		return true;
	}
	else
	{
		return false;
	}
}

void Protocol61850::AutoRltData(SHDFR_B_RemoteServer *pServer, void* data, int data_len )
{
	if( pServer == NULL || data == NULL)
		return;

	char szIPAddr[20] = {0};
	const char *pIPAddr = pServer->getServerIPAddr(szIPAddr);

	//PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pIPAddr,1);
	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pIPAddr);
	if(pDauInfo == NULL)
	{
		mylog.FormatLogInf(LogControl::LWARNING,"Get real-time data from %s,but no find the device's config",pIPAddr);
		return;
	}
	unsigned int devId = pDauInfo->DAUId;

	CDeviceIniMgr *pDevIniMgr = &(SystemCfger::GetInstance().m_IniMgr);
	CDeviceIni *pDevIni = pDevIniMgr->GetDevice(devId);

	if(pDevIni == NULL)//配置文件不存在

	{
		mylog.FormatLogInf(LogControl::LWARNING, "No find device(id:%d)'s ini file in Protocol61850::AutoRltData()",devId);
		return;
	}

	const int FRM_HEAD_RT_LEN = sizeof(FRM_HEAD_RT);
	if( data_len <= FRM_HEAD_RT_LEN )
	{
		mylog.FormatLogInf(LogControl::LWARNING, "The DataLen is:%d,short than FRM_HEAD_RT",data_len);
		return;
	}

	FRM_HEAD_RT headrt;	//原始实时数据头结构

	memcpy(&headrt,data,FRM_HEAD_RT_LEN );
	char *pSrcData = (char *)data;
	pSrcData += FRM_HEAD_RT_LEN;

	const int PERSMP_DATA_LEN = headrt.AnChCnt*2 + 8*(( headrt.BinChCnt+63)/64) +16;

	//计算计算结果长度
	int iComputerRst = sizeof(ANCH_VAL)*headrt.AnChCnt + sizeof(ULIN_VAL)*headrt.ULinCnt + 
		sizeof(ILIN_VAL)*headrt.ILinCnt + sizeof(GN_VAL)*headrt.GnCnt + sizeof(TRN_VAL)*headrt.TrnCnt;

	if( data_len < (FRM_HEAD_RT_LEN + iComputerRst + headrt.CycleCnt*headrt.SmpRate*PERSMP_DATA_LEN +4 ) )
	{
		mylog.FormatLogInf(LogControl::LWARNING, "The DataLen is:%d,short than %d",
			data_len,(FRM_HEAD_RT_LEN + iComputerRst + headrt.CycleCnt*headrt.SmpRate*PERSMP_DATA_LEN ));
		return;
	}
	pSrcData += iComputerRst;

	vector<SH_ANCH_PARM> anaChList = pDevIni->GetChannelInfo();
	std::vector<SH_BINCH_PARM> swList = pDevIni->GetSwitchInfo();

	//填充后台实时数据头结构信息

	POINTDATAHEAD point;//后台实时数据头结构

	memset(&point,0,sizeof(point));

	point.ChannelNumber = anaChList.size();
	point.SwitchNumber = (swList.size()+15)/16;

	point.Chan[0] = 0xffffffff;
	point.Chan[1] = 0xffffffff;
	point.Chan[2] = 0xffffffff;
	point.point = headrt.CycleCnt * headrt.SmpRate;
	point.samplrate = headrt.SmpRate*50/1000;
	unsigned int iNewDataLen = (point.ChannelNumber + point.SwitchNumber) * 2*point.point;

	//开始拷贝数据

	struct Command cmd;
	cmd.from = CD_ASDU;
	cmd.msgId = -1;
	cmd.destDevId = pDauInfo->BoardId;
	
	PCMDRESULT pResult = new CMDRESULT();
	cmd.resultNum = 1 ;
	pResult->resultType = CDRT_POINTDATA;
	pResult->resultLen = iNewDataLen + sizeof(POINTDATAHEAD);
	pResult->pData = new char[pResult->resultLen +1];
	memset(pResult->pData,0,pResult->resultLen+1);
	
	memcpy(pResult->pData,&point,sizeof(point));
	char *pDestData = &pResult->pData[sizeof(POINTDATAHEAD)];

	int iMinChannelNumber = ( point.ChannelNumber > headrt.AnChCnt) ? headrt.AnChCnt : point.ChannelNumber;
	int iMinSwitchNumber = (headrt.BinChCnt+15)/16;
	iMinSwitchNumber = ( point.SwitchNumber > iMinSwitchNumber) ? iMinSwitchNumber : point.SwitchNumber;

	//拷贝数据
	const int FPGA_ANA_DATA_OFFSET = 16;	//FPGA每个采样点第一个模拟量的偏移量(16字节的时标)
	for(unsigned int i=0;i<point.point;i++) //采样点总数
	{
		//data += FPGA_ANA_DATA_OFFSET;

		//拷贝模拟通道数据
		memcpy(pDestData,&pSrcData[FPGA_ANA_DATA_OFFSET],iMinChannelNumber*2);
		pDestData += point.ChannelNumber*2;
		//data += headrt.AnChCnt*2;

		//拷贝开关通道数据
		memcpy(pDestData,&pSrcData[FPGA_ANA_DATA_OFFSET+headrt.AnChCnt*2],iMinSwitchNumber*2);
		pDestData += point.SwitchNumber*2;
		pSrcData += PERSMP_DATA_LEN;
	}

	cmd.resultList.push_back(pResult);
	MemMapMgr memMgr;
	bool	bCanSendRltData = false;	//能否发送实时数据


	//如果能发送实时数据，则将数据写入共享内存中

	if(m_pMapSection != NULL)
		EnterCriticalSection(m_pMapSection);

	map<unsigned int,bool>::iterator it = m_RltEnableList.find(pDauInfo->BoardId);
	if( it != m_RltEnableList.end() && (*it).second == true)
	{
		mylog.WriteLog2File(LogControl::LDETAIL,"write real-time-data to share memery");
		cmd.funCode = CMD_READ_RLT_DATA;
		memMgr.WriteTaskResultToMem(cmd);
		bCanSendRltData = true;
	}

	//如果能发送零漂数据，则将数据写入共享内存中


	it = m_ZeroOffsetEnableList.find(pDauInfo->BoardId);
	if( it != m_ZeroOffsetEnableList.end() && (*it).second == true)
	{
		mylog.WriteLog2File(LogControl::LDETAIL,"write zero-offset-data to share memery");
		cmd.funCode = CMD_READ_ZEROOFFSET;
		m_ZeroOffsetEnableList[devId] = false;

		memMgr.WriteTaskResultToMem(cmd);

		//如果实时数据是停止的，则发停止命令

		if( !bCanSendRltData )
		{
			pServer->setRTRptEnable(false);
		}
	}

	if(m_pMapSection != NULL)
		LeaveCriticalSection(m_pMapSection);

}

void Protocol61850::AutoMarkEvt(SHDFR_B_RemoteServer *pServer, void* data, int data_len )
{
	if( pServer == NULL || data == NULL)
		return;

	char szIPAddr[20] = {0};
	const char *pIPAddr = pServer->getServerIPAddr(szIPAddr);

	mylog.WriteLog2File(LogControl::LDEBUG,"Get mark event and begin write to share memery in Protocol61850::AutoMarkEvt()");
	WriteAutoData2Mem( pIPAddr,CMD_AUTOSEND_MARKEVENT,CDRT_MARKEVENT,(char *)data,data_len);
}

void Protocol61850::AutoStartInf(SHDFR_B_RemoteServer *pServer, void* data, int data_len )
{
	char szIPAddr[20] = {0};
	const char *pIPAddr = pServer->getServerIPAddr(szIPAddr);

	mylog.WriteLog2File(LogControl::LDEBUG,"Get starting info and begin write to share memery in Protocol61850::AutoStartInf()");
	WriteAutoData2Mem( pIPAddr,CMD_STARTINFO,CDRT_STARTRECMSG,(char *)data,data_len);
}

void Protocol61850::WriteAutoData2Mem( const char *pRmtIpAddr,unsigned char funcode,CMDRESULTTYPE resultType,const char *data,int data_len  )
{
//	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pRmtIpAddr,1);
	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pRmtIpAddr);
	
	if(pDauInfo == NULL)
	{
		
		mylog.FormatLogInf(LogControl::LWARNING,"Get auto data from %s,but no find the device's config",pRmtIpAddr);
		return;

	}
	unsigned int devId = pDauInfo->BoardId;

	PCMDRESULT pResult = new CMDRESULT();
	struct Command cmd;
	cmd.destDevId = devId;
	cmd.from = CD_ASDU;
	cmd.msgId = -1;
	cmd.resultNum = 1 ;
	cmd.funCode = funcode;
	pResult->resultType = resultType;

	pResult->resultLen = data_len;
	pResult->pData = new char[pResult->resultLen+1];
	memset(pResult->pData,0,pResult->resultLen+1);
	memcpy(pResult->pData,data,pResult->resultLen);	
	cmd.resultList.push_back(pResult);

	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(cmd);
}

bool Protocol61850::GetContData( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::GetContData()");

	if(m_pIEC61850_Adaptor == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR, "m_pIEC61850_Adaptor is NULL");
		pLinker->Close();
		return false;
	}

	//分析命令参数
	if(pCmd->parmLen != 1+4+4+4+12+24+sizeof(unsigned short)+100|| pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::GetContData()");
		return false;
	}

	//参数处理
	SearchContFileCMDData cmd_data;
	memset(&cmd_data,0,sizeof(SearchContFileCMDData));

	cmd_data.dataType = pCmd->pParm[0];	//数据类型(1:动态记录数据  2:趋势记录数据)
	memcpy(&cmd_data.startTime,&pCmd->pParm[1],4);	//开始时间

	memcpy(&cmd_data.TimeInterval,&pCmd->pParm[1+4],4);	//数据间隔时间
	memcpy(&cmd_data.timeLen,&pCmd->pParm[1+4+4],4);	//数据长度(单位：秒)
	memcpy(cmd_data.anaChMark,&pCmd->pParm[1+4+4+4],12);//模拟量通道掩码
	memcpy(cmd_data.switchChMark,&pCmd->pParm[1+4+4+4+12],24);//开关量通道掩码

	memcpy(cmd_data.FileName,&pCmd->pParm[1+4+4+4+12+24+sizeof(unsigned short)],100);//文件名


# if defined(_MSC_VER)  &&  (_MSC_VER >= 1400)//VC 2005
	struct tm tmStartTime = *_localtime32((const __time32_t *)&cmd_data.startTime);
#else
	struct tm tmStartTime = *localtime((time_t*)&cmd_data.startTime);
#endif

	int iYearMonth = (tmStartTime.tm_year+1900)*100 + tmStartTime.tm_mon+1;
	char szYearMonth[7]={0};
	::itoa(iYearMonth,szYearMonth,10);

	FileControl fileCtrl;
	unsigned int devId = 0;
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		return "";
	}
	else
	{
		devId = pDauInf->DAUId;
	}
	//获得文件存放目录
	string writeFileName;
	string filePath = fileCtrl.GetFileTypeDirector(devId,FileControl::FT_CONT);
	filePath = filePath + string(szYearMonth) + string("/");
	if(!fileCtrl.IsExist(filePath.c_str()))
	{
		_mkdir(filePath.c_str());
	}

	//文件名
	//PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId,2);
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

	filePath += string(szFileName);

	RequestPtr	reqPtr;
	RequestPtr	subReqPtr;		//子集异步对象
	ReqBase *pBaseReq = NULL;	//父异步对象

	ReqBase *pSubBaseReq = NULL;//子异步对象

	int		iCount =0;
	int		iRet = 0;
	PCMDRESULT pResult = NULL;

	string sTrendFilePath = filePath + string(".TRH");//趋势记录数据文件名

	if( (cmd_data.dataType & 0x01) == 1 )//动态记录数据
	{
		writeFileName = filePath+string(".CFG");
		iRet = m_pIEC61850_Adaptor->getContFileA(&cmd_data,filePath.c_str(),&reqPtr);
	}
	else if( (cmd_data.dataType & 0x02) == 2 )//趋势记录数据
	{
		writeFileName = sTrendFilePath;
		iRet = m_pIEC61850_Adaptor->getTrendFileA((SearchTrendFileCMDData*)&cmd_data,sTrendFilePath.c_str(),&reqPtr);
	}

	//调用失败
	if( IsError(iRet,pLinker) )
	{
		mylog.FormatLogInf(LogControl::LERROR, "getContFileA or getTrendFileA return fail,local file:%s",filePath);
		return false;
	}

	//等待连续记录数据的异步对象
	while( 1 )
	{
		pBaseReq = reqPtr.ptr();
		if( pBaseReq == NULL)
		{
			mylog.FormatLogInf(LogControl::LERROR, "reqPtr.ptr() return NULL in Protocol61850::GetContData()");
			return false;
		}

		//失败
		if( pBaseReq->isFailed() )
		{
			mylog.WriteLog2File(LogControl::LERROR, "pBaseReq->isFailed() in Protocol61850::GetContData()");
			IsError(-1,pLinker);//如果链路异常，还要关闭链路

			return false;
		}

		//等待调连续数据的异步对象
		if( REQID_GetContFile != pBaseReq->reqType() && REQID_GetTrendFile != pBaseReq->reqType() )
		{
			::Sleep(6);
			continue;
		}
		break;
	}

	//调取连续数据
	while(1)
	{
		if( REQID_GetContFile == pBaseReq->reqType() )
		{
			subReqPtr = ((GetContFileReq *)pBaseReq)->getSubReq();
		}
		else
		{
			subReqPtr = ((GetTrendFileReq *)pBaseReq)->getSubReq();
		}

		//检查是否处理完成（失败或成功）
		//失败
		if( pBaseReq->isFailed() )
		{
			//如果是网络错误，还需关闭链接
			if(!m_pIEC61850_Adaptor->isConnected())
				pLinker->Close();
			mylog.WriteLog2File(LogControl::LERROR, "Get cont or trend file fail");
			return false;
		}

		//完成
		if(  pBaseReq->isFinished() )
		{
			pCmd->resultNum = 2;
			pResult = new CMDRESULT;
			pResult->resultType = CDRT_FILEPATH;
			pResult->resultLen = writeFileName.size();
			pResult->pData = new char[writeFileName.size()+1];
			pResult->pData[writeFileName.size()] = 0 ;
			memcpy(pResult->pData,writeFileName.c_str(),writeFileName.size());
			pCmd->resultList.push_back(pResult);

			pResult = new CMDRESULT;
			pResult->resultType = CDRT_TRUFALSE;
			pResult->resultLen = 1;
			pResult->pData = new char[2];
			pResult->pData[0] = 1 ; //执行成功
			pResult->pData[1] = 0 ;
			pCmd->resultList.push_back(pResult);

			mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::GetContData()");
			return true;
		}

		//取子集异步对象信息

		pSubBaseReq = subReqPtr.ptr();

		if( pSubBaseReq == NULL || REQID_GetFile != pSubBaseReq->reqType() )
		{
			//非GetFile的异步对象，则忽略它
			::Sleep(20);
			continue;
		}
		else
		{
			//是GetFile的异步对象，则将进度信息发送给主界面

			PutOrGetFileInf( pCmd, 2,&subReqPtr);
		}//end if( REQID_GetFile != pBaseReq->reqType() )
	}//end while( 1 )

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::GetContData()");
	return true;
}

bool Protocol61850::SetDeviceState( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::SetDeviceState( )");

	//分析命令参数
	if(pCmd->parmLen != 1+1+2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::SetDeviceState()");
		return false;
	}
	int ret = 0;
	unsigned char uSetState = pCmd->pParm[1];//设定状态

	switch( uSetState )
	{
	case 1://运行
		ret = m_pIEC61850_Adaptor->sendCtrl(RES_DEVSTATE_CTRL,1,30);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Switch the device to run fail");
			return false;
		}
		break;
	case 2://调试
		ret = m_pIEC61850_Adaptor->sendCtrl(RES_DEVSTATE_CTRL,3,30);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Switch the device to test state fail");
			return false;
		}
		break;
	default:
		mylog.WriteLog2File(LogControl::LERROR, "Unkown command in Protocol61850::SetDeviceState()");
		return false;
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::SetDeviceState( )");
	return true;
}

bool Protocol61850::OperateDevice( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol61850::OperateDevice( )");

	//分析命令参数
	if(pCmd->parmLen != 1+4 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol61850::OperateDevice()");
		return false;
	}

	int ret = 0;
	unsigned char uOperType = pCmd->pParm[0];//类型
	switch( uOperType )
	{
	case 1://告警复归
		ret = m_pIEC61850_Adaptor->sendCtrl(RES_DEVOPER_CTRL,1,20);
		if( IsError(ret,pLinker) )
		{
			mylog.WriteLog2File(LogControl::LERROR, "Warnning revert fail in Protocol61850::OperateDevice()");
			return false;
		}
		break;
	default:
		mylog.WriteLog2File(LogControl::LERROR, "Unkown command in Protocol61850::OperateDevice()");
		return false;
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol61850::OperateDevice( )");
	return true;
}

///call
bool Protocol61850::CallGetDauInf(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetDauInf( pLinker ,&cmd );
}

bool Protocol61850::CallGetRecWaveFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetRecWaveFile( pLinker ,&cmd );
}

bool Protocol61850::CallGetRecWaveFileList(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetRecWaveFileList( pLinker ,&cmd );
}

bool Protocol61850::CallGetFileList(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetFileList( pLinker ,&cmd );
}

bool Protocol61850::CallSetTime(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->SetTime( pLinker ,&cmd );
}

bool Protocol61850::CallGetTime(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetTime( pLinker ,&cmd );
}

bool Protocol61850::CallStopRecWave(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->StopRecWave( pLinker ,&cmd );
}

bool Protocol61850::CallStartRecByManual(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->StartRecByManual( pLinker ,&cmd );
}

bool Protocol61850::CallGetContData(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->GetContData( pLinker ,&cmd );
}

bool Protocol61850::CallPullFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->PullFile( pLinker ,&cmd );
}

bool Protocol61850::CallPushFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->PushFile( pLinker ,&cmd );
}

bool Protocol61850::CallReadRltData(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->ReadRltData( pLinker ,&cmd );
}

bool Protocol61850::CallReadZeroOffset(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->ReadZeroOffset( pLinker ,&cmd );
}

bool Protocol61850::CallSetDeviceState(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->SetDeviceState( pLinker ,&cmd );
}

bool Protocol61850::CallOperateDevice(Protocol *pProtocol,Linker * pLinker,struct Command& cmd)
{
	return ((Protocol61850*)pProtocol)->OperateDevice( pLinker ,&cmd );
}