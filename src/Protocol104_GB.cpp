// {{{RME classifier 'Logical View::Protocol104_GB'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol104_GB.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104_GB.h>
#include <FileControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：Protocol104_GB.cpp
* 文件标识：
* 摘    要：实现GB104协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

#include "Linker.h"
#include "Task.h"
#include "FileControl.h"
#include <MsgQueue.h>
#include <QueRegister.h>
#include "Device.h"
#include "DataControl.h"

#include "StringRes.h"

PRO104GBFRAME2CMD pro104GBFR2CmdMap[] = {
	{PRO104_ASDU21,CMD_104GB_GEN_COMMAND,GenCommand,(CStringRes::GetInstance())->GetStrById(IDS_GEN_CMD,"通用分类命令")},
	{PRO104_ASDU120,CMD_104GB_GET_SERIALRECORDER,GetSerialRecorder,(CStringRes::GetInstance())->GetStrById(99999,"调取连续记录波形")},
	{PRO104_ASDU124,CMD_104GB_QUERY_SERIALDATA,QuerySerialData,(CStringRes::GetInstance())->GetStrById(IDS_QUY_CONT_DATA,"连续记录检索")},
	{PRO104_ASDU101,CMD_104GB_GENERAL_FILE_LIST,GeneralFileList,(CStringRes::GetInstance())->GetStrById(IDS_CALL_GEN_FILELIST,"通用文件列表")},
	{PRO104_ASDU103,CMD_104GB_GENERAL_READ_FILE,ReadGeneralFile,(CStringRes::GetInstance())->GetStrById(IDS_GEN_FILE_READ,"通用文件读")},
	{PRO104_ASDU105,CMD_104GB_GENERAL_WRITE_FILE,WriteGeneralFile,(CStringRes::GetInstance())->GetStrById(IDS_GEN_FILE_WRITE,"通用文件写")},
	{PRO104_ASDU221,CMD_104GB_COMM_PARAM_ARRANGE,CommParamArrange,(CStringRes::GetInstance())->GetStrById(IDS_COMM_ARRANGE,"通讯参数协商")},
	{PRO104_ASDU122,CMD_104GB_COMM_STOP_TRANSFER,StopDataTransfer,(CStringRes::GetInstance())->GetStrById(IDS_STOP_DT,"终止数据传输")}
};

//线程参数
struct tagDevThreadParm104GB
{
	unsigned int devId;
	unsigned char funCode;
	unsigned short cmdParmLen;
	char *pCmdParm;
};

DWORD WINAPI DeviceThread_104GB(LPVOID pParm);     //与前台设备通讯线程

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol104_GB::~Protocol104_GB( void )
{
}
// }}}RME

// {{{RME operation 'Protocol104_GB()'
Protocol104_GB::Protocol104_GB( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_iLenOfApdulen = 2;
	// }}}USR
}
// }}}RME

// {{{RME operation 'Decode(Linker*)'
Task * Protocol104_GB::Decode( Linker * pLinker )
{
	// {{{USR
	char	szFrameData[PRO104GB_FRAME_MAXSIZE+1] = {0};
	int		iFrameLen = PRO104GB_FRAME_MAXSIZE;
	APCI4Prot104	apci(m_iLenOfApdulen);
	PASDUID104		pAsduId = NULL;
	PASDUINFID104	pAsduInfoId = NULL;
	char		*pAsduData = NULL;
	int			iAsduDataLen = 0;
	int iResult = 0;

	while(1)
	{
		iResult = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,50);
		if(iResult == -1) //有错误
		{
			pLinker->Close();
			return NULL;
		}
		else if(iResult == 0) //没有数据
		{
			return NULL;
		}

		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			Task *pTask =  AnalyseTask(pAsduId,pAsduInfoId,pAsduData,iAsduDataLen,pLinker);
			if(pTask == NULL)
			{
				//是本系统不支持的命令,则将传送原因置为44后，原帧送回
				apci.SetSendNum(m_iHasSendNum);//设置发送序列号
				apci.SetReceivNum(m_iHasReceivNum);//设置接收序列号
				memcpy(szFrameData,apci.Encode().c_str(),apci.GetApciLen());

				pAsduId->COT = 44;//未知的类型标识

				if(iFrameLen == pLinker->SendData(szFrameData,iFrameLen,0))
				{
					m_iHasSendNum++; //发送序列号加1
				}
			}

			return pTask;
		}
		else if(apci.GetFrameType() == APCI4Prot104::FT_U)
		{
			return Protocol104::AnalyseTask(&apci,pLinker);
		}
		else
		{
			//s格式帧则继续解析任务
			continue;
		}
	} //end while(1)

	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseTask(PASDUID104,PASDUINFID104,char*,int,Linker*)'
Task * Protocol104_GB::AnalyseTask( PASDUID104 pAsduId, PASDUINFID104 pInfId, char * pAsduData, int iAsduDataLen, Linker * pLinker )
{
	// {{{USR
		if(pAsduId == NULL || pAsduData == NULL)
		{
			return NULL;
		}

		int i;
	    int mapCount = (sizeof(pro104GBFR2CmdMap)/sizeof(PRO104GBFRAME2CMD));
		for(i=0;i<mapCount;i++)
		{
			if(pro104GBFR2CmdMap[i].type == pAsduId->TYP)
			{
				break;
			}
		}

		if( i >= mapCount)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			return NULL;
		}

		//构建任务
		Task *pTask = new Task();
	    pTask->Linker = pLinker;
		pTask->cmd.destDevId = pAsduId->COMADDR&0X00FF;
	    pTask->cmd.from = CD_REMOTE;
	    pTask->cmd.to = CD_NEURON;
		pTask->cmd.funCode = pro104GBFR2CmdMap[i].cmdFunCode;
		strcpy(pTask->cmd.describe,pro104GBFR2CmdMap[i].cmdName);

		//是ASDU21，还要将信息序号作为参数传给命令
		if(pAsduId->TYP == PRO104_ASDU21)
		{
			pTask->cmd.parmLen = iAsduDataLen;
		}
		else
		{
			pTask->cmd.parmLen = iAsduDataLen -1;
		}
		
		if(pTask->cmd.parmLen > 0)
		{
			pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
			memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

			//是ASDU21，还要将信息序号作为参数传给命令
			if(pAsduId->TYP == PRO104_ASDU21)
			{
				pTask->cmd.pParm[0] = pInfId->INF;
				memcpy(&(pTask->cmd.pParm[1]),pAsduData+1,pTask->cmd.parmLen-1);
			}
			else
			{
				memcpy(pTask->cmd.pParm,pAsduData+1,pTask->cmd.parmLen);
			}
		}

		return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol104_GB::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	int i;
	int mapCount = (sizeof(pro104GBFR2CmdMap)/sizeof(PRO104GBFRAME2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro104GBFR2CmdMap[i].cmdFunCode == cmd.funCode )
		{
			break;
		}
	}

	if( i >= mapCount)
	{
		return Protocol104::ExecuteCmd(pLinker,cmd);
	}

	return pro104GBFR2CmdMap[i].exuteCmd(this,pLinker,cmd);
	// }}}USR
}
// }}}RME

// {{{RME operation 'CommParamArrange(Linker *,struct Command*)'
bool Protocol104_GB::CommParamArrange( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::CommParamArrange( )");
	if(!m_bCanSendData)
	{
		m_errCode = PEC_STARTDT_NOACT;
		mylog.WriteLog2File(LogControl::LWARNING, "Protocol104_GB::CommParamArrange(): Cann't send data,because no getting the command:STARTDT");
		return false;
	}

	//构造ASDU_220
	char szAsduData[255] = {0};
	char *pParm = &szAsduData[1];
	memcpy(pParm,&P104_K,sizeof(P104_K));	//k
	pParm += sizeof(P104_K);

	memcpy(pParm,&P104_W,sizeof(P104_W));	//w
	pParm += sizeof(P104_W);

	pParm += 4; //t0

	memcpy(pParm,&P104_T1,sizeof(P104_T1));	//t1
	pParm += sizeof(P104_T1);

	memcpy(pParm,&P104_T2,sizeof(P104_T2));	//t2
	pParm += sizeof(P104_T2);

	memcpy(pParm,&P104_T3,sizeof(P104_T3));	//t3
	pParm += sizeof(P104_T3);

	int iAsduDataLen = pParm - szAsduData;

	return SendAsduFrame(pLinker,pCmd,PRO104_ASDU220,STRNO_GLIB,0,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen);
				
	// }}}USR
}
// }}}RME

// {{{RME operation 'GenCommand(Linker *,struct Command*)'
bool Protocol104_GB::GenCommand( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GenCommand( )");
	//分析命令参数
	if(pCmd->parmLen < 2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::GenCommand()");
		return false;
	}

	bool bResult = false;

	unsigned char INF = pCmd->pParm[0];	//信息序号
	unsigned char NOG = pCmd->pParm[1];	//通用分类标示数目
	unsigned char groupNo = 0;	//组号
	unsigned char entryNo = 0;	//条目
	unsigned char KOD = 0;		//类型描述

	if(NOG != (pCmd->parmLen-2)/3)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::GenCommand()");
		return false;
	}

	char *pItem = &pCmd->pParm[2];
	for(unsigned char i=0;i<NOG;i++)
	{
		groupNo = *pItem;
		pItem++;
		entryNo = *pItem;
		pItem++;
		KOD = *pItem;
		pItem++;

		if(groupNo == 0x08 && entryNo ==0 && INF == ASDU_INF_R_ALLENTRY_VAL && KOD == ASDU_KOD_ACT_VAL)
		{
			bResult = true;
			//查询所有历史事件
			if(!GetMarkEvn(pLinker,pCmd))
				return false;
		}
		else if(groupNo == 0x0A && entryNo ==0 && INF == ASDU_INF_R_ALLENTRY_VAL && KOD == ASDU_KOD_DESCRIPTION)
		{
			bResult = true;
			//查询所有数据检索类型
			if(!GetSearchType(pLinker,pCmd))
				return false;
		}
		else if(groupNo == 0x0C && entryNo ==0 && INF == ASDU_INF_R_ALLENTRY_VAL && KOD == ASDU_KOD_DESCRIPTION)
		{
			bResult = true;
			//查询所有数据检索对象描述
			if(!GetAllDataObjDesc(pLinker,pCmd))
				return false;
		}
		else if((groupNo == 0x0C || groupNo == 0x0D) && entryNo !=0 && 
			INF == ASDU_INF_R_ALLENTRY_VAL && KOD == ASDU_KOD_ACT_VAL)
		{
			bResult = true;
			//查询单个数据检索对象值
			if(!GetDataObjAttr(pLinker,pCmd,groupNo,entryNo))
				return false;
		}
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GenCommand( )");
	return bResult ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetMarkEvn(Linker *,struct Command*)'
bool Protocol104_GB::GetMarkEvn( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GeneralCmd( )");


	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GeneralCmd( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetSearchType(Linker *,struct Command*)'
bool Protocol104_GB::GetSearchType( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GeneralCmd( )");

	const int ITEM_NUM = 13;
	char *pTypeDesc[ITEM_NUM] ={"数据检索类型","幅值","突变","正序","负序","零序","频率","功率","相角","谐波15","频率变化率","振荡","开关量"};

	GI entrys[ITEM_NUM] ={
		GI(0X0A,0X00,10,1,12,1,12,(unsigned char *)pTypeDesc[0]),
		GI(0X0A,0X01,10,1,4,1,4,(unsigned char *)pTypeDesc[1]),
		GI(0X0A,0X02,10,1,4,1,4,(unsigned char *)pTypeDesc[2]),
		GI(0X0A,0X03,10,1,4,1,4,(unsigned char *)pTypeDesc[3]),
		GI(0X0A,0X04,10,1,4,1,4,(unsigned char *)pTypeDesc[4]),
		GI(0X0A,0X05,10,1,4,1,4,(unsigned char *)pTypeDesc[5]),
		GI(0X0A,0X06,10,1,4,1,4,(unsigned char *)pTypeDesc[6]),
		GI(0X0A,0X07,10,1,4,1,4,(unsigned char *)pTypeDesc[7]),
		GI(0X0A,0X08,10,1,4,1,4,(unsigned char *)pTypeDesc[8]),
		GI(0X0A,0X09,10,1,6,1,6,(unsigned char *)pTypeDesc[9]),
		GI(0X0A,0X0A,10,1,10,1,10,(unsigned char *)pTypeDesc[10]),
		GI(0X0A,0X0B,10,1,4,1,4,(unsigned char *)pTypeDesc[11]),
		GI(0X0A,0X0C,10,1,6,1,6,(unsigned char *)pTypeDesc[12])
	};

	//构造ASDU_11
	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[2];
	unsigned char i = 0;

	for(;i<ITEM_NUM;i++)
	{
		CpyGI2SendBuf(&pData,(sizeof(szAsduData) - (pData-szAsduData)),entrys[i]);
	}

	NGD ngd;
	ngd.s.NO = i;
	memcpy(&szAsduData[1],&ngd,1);
	int iAsduDataLen = pData - szAsduData;

	return SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,0,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetAllDataObjDesc(Linker *,struct Command*)'
bool Protocol104_GB::GetAllDataObjDesc( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GetAllDataObjDesc( )");

	//构造ASDU_11
	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[2];
	unsigned char COT = 0x14;
	NGD ngd; //通用分类数据集数目
	unsigned char groupNO = 0x0C;	//组号
	unsigned char entryNO = 0x00;	//条目号
	char szDesc[64] = {0};

	CDeviceIniMgr devImiMgr = SystemCfger::GetInstance().m_IniMgr;
	CDeviceIni *pDevIni = devImiMgr.GetDevice(pCmd->destDevId);

	if(pDevIni == NULL)
	{
		//发送否定的确认
		COT = 0x15;
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,COT,(pCmd->destDevId & 0XFF),szAsduData,1))
		{
			return false;
		}		
	}
	else
	{
		//组描述
		ngd.s.NO++;
		::strcpy(szDesc,(CStringRes::GetInstance())->GetStrById(99998,"数据检索对象"));
		GI gi(groupNO,entryNO,10,1,12,1,12,(unsigned char *)szDesc);
		CpyGI2SendBuf(&pData,(sizeof(szAsduData) - (pData-szAsduData)),gi);

		//模拟量通道
		unsigned int i=0;
		std::vector<SH_ANCH_PARM> anaChList = pDevIni->GetChannelInfo();		
		for(i;i < anaChList.size();i++)
		{
			ngd.s.NO++;
			entryNO++;
			int iDescLen = strlen(anaChList[i].AnCfg.Name);
			GI gi(groupNO,entryNO,10,1,iDescLen,1,iDescLen,(unsigned char *)anaChList[i].AnCfg.Name);

			bool bCpyResult = CpyGI2SendBuf(&pData,(sizeof(szAsduData) - (pData-szAsduData)),gi);
			if(!bCpyResult	|| ngd.s.NO >= 63)
			{
				//发送缓冲区已满或数据集数目达到63
				if(!bCpyResult)
				{
					ngd.s.NO--;
					entryNO--;
					i--;
				}
				int iAsduLen = pData - szAsduData;

				ngd.s.CONT = 1;
				szAsduData[1] = ngd.ngd;
				if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduLen))
				{
					return false;
				}

				ngd.ngd = 0;
				pData = &szAsduData[2];
			}
		}//end 模拟量通道

		entryNO = 96;//1~96为模拟通道号

		//开关通道
		std::vector<SH_BINCH_PARM> swChList = pDevIni->GetSwitchInfo();
		for(i=0;i<swChList.size();i++)
		{
			ngd.s.NO++;
			entryNO++;
			if(entryNO > 255 || entryNO == 0)
			{
				groupNO++;
				entryNO = 1;
			}

			int iDescLen = strlen(swChList[i].BinCfg.Name);
			GI gi(groupNO,entryNO,10,1,iDescLen,1,iDescLen,(unsigned char *)swChList[i].BinCfg.Name);

			bool bCpyResult = CpyGI2SendBuf(&pData,(sizeof(szAsduData) - (pData-szAsduData)),gi);
			if(!bCpyResult || ngd.s.NO >= 63)
			{
				//发送缓冲区已满或数据集数目达到63
				if(!bCpyResult)
				{
					ngd.s.NO--;
					entryNO--;
					i--;
				}
				
				if(entryNO == 0)
				{
					entryNO = 255;
					groupNO--;
				}
				
				int iAsduLen = pData - szAsduData;

				ngd.s.CONT = 1;
				szAsduData[1] = ngd.ngd;
				if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduLen))
				{
					return false;
				}

				ngd.ngd = 0;
				pData = &szAsduData[2];
			}
		}//end 开关通道

		//组号为0x0C时，条目号97~255为开关通道号；组号为0x0D时，1~33为开关通道号
		groupNO = 0x0D;
		entryNO = 33;

		//线路
		std::vector<SH_ILIN_PARM> lineList = pDevIni->GetLineInfo();
		for(i=0;i<lineList.size();i++)
		{
			ngd.s.NO++;
			entryNO++;
			
			int iDescLen = strlen(lineList[i].LinCfg.Name);
			GI gi(groupNO,entryNO,10,1,iDescLen,1,iDescLen,(unsigned char *)lineList[i].LinCfg.Name);

			bool bCpyResult = CpyGI2SendBuf(&pData,(sizeof(szAsduData) - (pData-szAsduData)),gi);
			if(!bCpyResult || ngd.s.NO >= 63 || i == lineList.size()-1)
			{
				//发送缓冲区已满或数据集数目达到63
				if(!bCpyResult)
				{
					ngd.s.NO--;
					entryNO--;
					i--;
				}				
				
				int iAsduLen = pData - szAsduData;

				if(i == lineList.size()-1)
				{
					ngd.s.CONT = 0;
				}
				else
				{
					ngd.s.CONT = 1;
				}

				szAsduData[1] = ngd.ngd;
				if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduLen))
				{
					return false;
				}

				ngd.ngd = 0;
				pData = &szAsduData[2];
			}
		}//end 线路
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GetAllDataObjDesc( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetDataObjAttr(Linker *,struct Command*,unsigned char,unsigned char)'
bool Protocol104_GB::GetDataObjAttr( Linker * pLinker, struct Command * pCmd, unsigned char groupNo, unsigned char entryNo )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GeneralCmd( )");

	//构造ASDU_11
	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[1];
	unsigned char COT = 0x14;
	NDE nde; //描述元素的数目
	GDD gdd;

	CDeviceIniMgr devImiMgr = SystemCfger::GetInstance().m_IniMgr;
	CDeviceIni *pDevIni = devImiMgr.GetDevice(pCmd->destDevId);

	if(pDevIni == NULL)//配置文件不存在
	{
		//发送否定的确认
		COT = 0x15;
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_GROUP_ALL_ENTRY_VAL,COT,(pCmd->destDevId & 0XFF),szAsduData,1))
		{
			return false;
		}
	}
	else
	{
		char szUnit[11] = {0}; //量纲
		char szDesc[65] = {0}; //描述
		unsigned char iSearchTypeNum = 0; //可支持检索类型个数
		unsigned short searchGins[20]={0}; //检索类型的gin
		unsigned char ucType =0;//类型

		if(groupNo==0x0C && entryNo>0 && entryNo<97)//模拟通道
		{
			vector<SH_ANCH_PARM> anaChList = pDevIni->GetChannelInfo();
			if(entryNo > anaChList.size())//序号与实际的通道数不对应
			{
				//发送否定的确认
				COT = 0x15;
				SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_ONE_ENTRY_DIR,COT,(pCmd->destDevId & 0XFF),szAsduData,1);
				return true;
			}//end if(entryNo > anaChList.size())

			SH_ANCH_PARM anaCh = anaChList[entryNo-1];

			//可检索的类型GIN
			iSearchTypeNum = 11;
			unsigned short gins[11] = {0X0A01,0X0A02,0X0A03,0X0A04,0X0A05,0X0A06,0X0A07,0X0A08,0X0A09,0X0A0A,0X0A0B};
			memcpy(&searchGins,&gins,iSearchTypeNum*sizeof(unsigned short));

			//量纲
			::itoa(anaCh.AnCfg.Unit,szUnit,10);

			//描述
			strcpy(szDesc,anaCh.AnCfg.Name);

			//类别
			switch(anaCh.AnCfg.ChTyp)
			{
			case 1:
				ucType = 1;	//交流电压
				break;
			case 2:
				ucType = 2;	//交流电流
				break;
			case 3:
				ucType = 5;	//高频通道
				break;
			case 4:
				ucType = 3;	//直流通道
				break;
			default:
				break;
			}

		}//end if(groupNo==0x0C && entryNo>0 && entryNo<97)//模拟通道

		//开关通道
		else if((groupNo==0x0C && entryNo>96) || (groupNo==0x0D && entryNo>0 && entryNo<34) )
		{
			std::vector<SH_BINCH_PARM> swList = pDevIni->GetSwitchInfo();
			int iIndex =0;
			if(groupNo==0x0C )
			{
				iIndex = entryNo - 97;
			}
			else
			{
				iIndex = (255 - 97) + entryNo;
			}

			if( (iIndex +1) > (int)swList.size() )//序号与实际的通道数不对应
			{
				//发送否定的确认
				COT = 0x15;
				SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_ONE_ENTRY_DIR,COT,(pCmd->destDevId & 0XFF),szAsduData,1);
				return true;
			}//end if(entryNo > anaChList.size())

			SH_BINCH_PARM swCh = swList[iIndex];

			//可检索的类型GIN
			iSearchTypeNum = 1;
			searchGins[0] = 0X0A0C;

			//量纲(无)
			memset(szUnit,0,sizeof(szUnit));

			//描述
			strcpy(szDesc,swCh.BinCfg.Name);

			//类别
			ucType = 100;	//开关量通道
		}//end if((groupNo==0x0C && entryNo>96) || (groupNo==0x0D && entryNo>0 && entryNo<34) )

		//线路
		else
		{
			std::vector<SH_ILIN_PARM> lineList = pDevIni->GetLineInfo();
			int iIndex = entryNo -34;

			if( (iIndex +1) > (int)lineList.size() )//序号与实际的通道数不对应
			{
				//发送否定的确认
				COT = 0x15;
				SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_ONE_ENTRY_DIR,COT,(pCmd->destDevId & 0XFF),szAsduData,1);
				return true;
			}//end if(entryNo > anaChList.size())

			SH_ILIN_PARM line = lineList[iIndex];

			//可检索的类型GIN
			iSearchTypeNum = 11;
			unsigned short gins[11] = {0X0A01,0X0A02,0X0A03,0X0A04,0X0A05,0X0A06,0X0A07,0X0A08,0X0A09,0X0A0A,0X0A0B};
			memcpy(&searchGins,&gins,iSearchTypeNum*sizeof(unsigned short));

			//量纲(无)
			memset(szUnit,0,sizeof(szUnit));

			//描述
			strcpy(szDesc,line.LinCfg.Name);

			//类别
			if(line.LinCfg.LinTyp == 1)
			{
				ucType = 200;	//母线
			}
			else
			{
				ucType = 201;	//线路
			}			
		}

		*pData = groupNo;
		pData++;
		*pData = entryNo;
		pData++;

		//NDE
		if(strlen(szUnit) >0)
		{
			nde.NO = 4;
		}
		else
		{
			nde.NO = 3;
		}

		memcpy(pData,&nde,sizeof(nde));
		pData += sizeof(nde);

		//可检索的类型GIN
		*pData = 1;	//KOD,实际值
		pData++;

		gdd.dataType = 15;
		gdd.dataLen = 2;
		gdd.dataNum = iSearchTypeNum;
		memcpy(pData,&gdd,sizeof(gdd));
		pData += sizeof(gdd);


		for(unsigned char i=0;i<iSearchTypeNum;i++)
		{
			memcpy(pData,&searchGins[i],sizeof(unsigned short));
			pData += sizeof(unsigned short);
		}

		//量纲
		if(strlen(szUnit) >0)
		{
			*pData = 9;	//KOD,量纲
			pData++;

			gdd.dataType = 1;
			gdd.dataLen = strlen(szUnit);
			gdd.dataNum = 1;
			memcpy(pData,&gdd,sizeof(gdd));
			pData += sizeof(gdd);

			strcpy(pData,szUnit);
			pData += gdd.dataLen;
		}

		//描述
		*pData = 10;	//KOD,描述
		pData++;

		gdd.dataType = 1;
		gdd.dataLen = strlen(szDesc);
		gdd.dataNum = 1;
		memcpy(pData,&gdd,sizeof(gdd));
		pData += sizeof(gdd);

		strcpy(pData,szDesc);
		pData += gdd.dataLen;

		//类别
		*pData = 102;	//KOD,类别
		pData++;

		gdd.dataType = 3;
		gdd.dataLen = 1;
		gdd.dataNum = 1;
		memcpy(pData,&gdd,sizeof(gdd));
		pData += sizeof(gdd);

		*pData = ucType;
		pData++;

		int iAsduLen = pData - szAsduData;
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU11,STRNO_ONE_ENTRY_DIR,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduLen))
		{
			return false;
		}
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GeneralCmd( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetSerialRecorder(Linker *,struct Command*)'
bool Protocol104_GB::GetSerialRecorder( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GetSerialRecorder( )");

	//分析命令参数
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 + 4 + 128*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::GeneralFileList()");
		return false;
	}

	struct CP56Time2a startTime,endTime;
	struct tm tmStartTime,tmEndTime;
	time_t ttStartTime,ttEndTime;
	//开始时间
	memcpy(&startTime,pCmd->pParm,sizeof(struct CP56Time2a));

	//结束时间
	memcpy(&endTime,&pCmd->pParm[sizeof(struct CP56Time2a)],sizeof(struct CP56Time2a));

	CP56Time2aTotm(startTime,tmStartTime);
	CP56Time2aTotm(endTime,tmEndTime);
	ttStartTime = mktime(&tmStartTime);
	ttEndTime = mktime(&tmEndTime);
	unsigned char ucTimeLen = (unsigned char)( ttEndTime - ttStartTime );

	char szAnaChMark[12] = {0};	//模拟通道掩码
	char szSwitchChMark[24] = {0};	//开关通道掩码

	memcpy(szAnaChMark,&pCmd->pParm[sizeof(struct CP56Time2a)*2 +4],sizeof(szAnaChMark) );
	memcpy(szSwitchChMark,&pCmd->pParm[sizeof(struct CP56Time2a)*2 +4+128],sizeof(szSwitchChMark) );

	unsigned int ucDevId = pCmd->destDevId;

	//创建前台通讯线程
	DataControl::DataQue *pDataQue = DataControl::CreateDataQue(1024*1024);

	struct tagDevThreadParm104GB parm;
	parm.cmdParmLen = 1+4+4+1+12+24 + sizeof(DataControl::DataQue*);
	parm.pCmdParm = new char[parm.cmdParmLen];
	memset(parm.pCmdParm,0,parm.cmdParmLen);
	parm.devId = ucDevId;
	parm.funCode = CMD_GET_CONT_DATA;

	char *pTmpParm = parm.pCmdParm;
	*pTmpParm = 1;	//读动态记录数据
	pTmpParm++;

	memcpy(pTmpParm,&ttStartTime,4);	//开始时间
	pTmpParm += 4;

	pTmpParm += 4;	//时间间隔(忽略)
	*pTmpParm = ucTimeLen;	//时间长度
	pTmpParm++;

	memcpy(pTmpParm,szAnaChMark,sizeof(szAnaChMark)); //模拟通道掩码
	pTmpParm += sizeof(szAnaChMark);
	memcpy(pTmpParm,szSwitchChMark,sizeof(szSwitchChMark)); //开关通道掩码
	pTmpParm += sizeof(szSwitchChMark);

	memcpy(pTmpParm,&pDataQue,sizeof(DataControl::DataQue*));//DataQue对象指针

	HANDLE hDevTread = CreateThread(NULL,0,&DeviceThread_104GB,&parm,0,NULL);

	unsigned int uiStartPos =0;

	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	int iRemainLen = PRO104GB_ASDUDATA_MAXSIZE -1;
	char *pData = &szAsduData[1];
	unsigned char COT = 0x14;
	DWORD  exitCode = 0;
	bool isComplete = false;
	bool bResult = true;

	::Sleep(10);
	while(1)
	{
		GetExitCodeThread(hDevTread,&exitCode);
		int iReadLen = pDataQue->pop(pData,iRemainLen);//从数据队列中读出数据
		pData += iReadLen;
		iRemainLen -= iReadLen;
		
		//帧已满,或数据完毕
		if(iRemainLen <= 0 || 
			(pDataQue->IsEmpty() && (pDataQue->IsCancle() || exitCode != STILL_ACTIVE)) )
		{
			if(pDataQue->IsEmpty() && (pDataQue->IsCancle() || exitCode != STILL_ACTIVE))
			{
				szAsduData[0]  = 0; //传输完毕
				isComplete = true;
			}
			else
			{
				szAsduData[0]  = 1; //还有后续帧
				pData = &szAsduData[1];
				iRemainLen = PRO104GB_ASDUDATA_MAXSIZE -1;
			}

			int iAsduDataLen = pData - szAsduData;
			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU121,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
			{
				pDataQue->Cancle();
				bResult = false;
				::Sleep(100);
				break;
			}
		}//end if(iRemainLen <= 0)//帧已满

		if(isComplete)
			break;
	}

	delete pDataQue;
	pDataQue = NULL;
	::CloseHandle(hDevTread);

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GetSerialRecorder( )");
	return bResult ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'QuerySerialData(Linker *,struct Command*)'
bool Protocol104_GB::QuerySerialData( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::QuerySerialData( )");


	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::QuerySerialData( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GeneralFileList(Linker *,struct Command*)'
bool Protocol104_GB::GeneralFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::GeneralFileList( )");

	//分析命令参数
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 + PRO104GB_FILEPATH_LEN || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::GeneralFileList()");
		return false;
	}

	struct CP56Time2a startTime,endTime;
	char szFilePath[256]={0};

	char *pParm = pCmd->pParm;

	//查询开始时间
	memcpy(&startTime,pParm,sizeof(struct CP56Time2a));
	pParm += sizeof(struct CP56Time2a);

	//查询结束时间
	memcpy(&endTime,pParm,sizeof(struct CP56Time2a));
	pParm += sizeof(struct CP56Time2a);

	//目录/文件名
	//传入的目录参数的根目录为本系统数据文件存放所在目录
	memcpy(szFilePath,SystemCfger::GetInstance().DataFilePath,strlen(SystemCfger::GetInstance().DataFilePath));
	memcpy(&szFilePath[strlen(SystemCfger::GetInstance().DataFilePath)],pParm,PRO104GB_FILEPATH_LEN);

	FileControl fileCtrl;
	vector< GENERALFILEPATHINF > filePathList;

	if( szFilePath[strlen(szFilePath)-1] == '/' || szFilePath[strlen(szFilePath)-1] == '\\')
	{
		szFilePath[strlen(szFilePath)] = '*';
	}

	//根据参数查询目录及文件
	fileCtrl.GetFileList(szFilePath,0,startTime,endTime,filePathList);

	//构造ASDU_102
	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[2];

	int iItemNum = 0;	//条目数
	int	iItemLen = sizeof(GENERALFILEPATHINF);//条目长度
	int iMaxItemNum = (PRO104GB_ASDUDATA_MAXSIZE -2)/iItemLen;//每次最多能发送的条目数

	//最大条目数受NDE影响，不能大于63
	iMaxItemNum = (iMaxItemNum>63)?63:iMaxItemNum;

	vector< GENERALFILEPATHINF >::iterator it = filePathList.begin();
	while(it != filePathList.end())
	{
		GENERALFILEPATHINF filePathInf = (*it);
		it++;
		iItemNum++;

		//没有超出帧最大长度且还有数据
		if( iItemNum <= iMaxItemNum && it != filePathList.end() )
		{
			memcpy(pData,&filePathInf,iItemLen);
			pData += iItemLen;
		}
		else//超出帧最大长度或数据发送完
		{
			NDE nde;
			nde.COUNT = 0;

			if(iItemNum > iMaxItemNum )
			{
				//帧已满
				nde.CONT = 1; //仍有数据
				nde.NO = iItemNum -1;
				it--;
			}
			else
			{
				//帧未满，但数据已经完毕
				nde.CONT = 0; //数据已经完毕
				memcpy(pData,&filePathInf,iItemLen);
				nde.NO = iItemNum;
				pData += iItemLen;
			}

			int iAsduDataLen = pData - szAsduData;

			//szAsduData[1] = nde;
			memcpy(&szAsduData[1],&nde,1);
			iItemNum = 0;

			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU102,STRNO_GEN,0,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
			{
				return false;
			}
			
			memset(szAsduData,0,sizeof(szAsduData));
			pData = &szAsduData[2];
		}
	}

	//没有目录，发空包过去
	if(filePathList.size() == 0)
	{
		NDE nde;
		nde.COUNT = 0;
		nde.NO = 0;
		nde.COUNT = 0;

		int iAsduDataLen = 2;

		memcpy(&szAsduData[1],&nde,1);
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU102,STRNO_GEN,0,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
		{
			return false;
		}
	}

	filePathList.clear();


	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::GeneralFileList( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'ReadGeneralFile(Linker *,struct Command*)'
bool Protocol104_GB::ReadGeneralFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::ReadGeneralFile( )");

	//分析命令参数
	if(pCmd->parmLen != PRO104GB_FILEPATH_LEN + 4 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::ReadGeneralFile()");
		return false;
	}

	char szFilePath[256]={0};
	unsigned int uiStartPos = 0;

	char *pParm = pCmd->pParm;

	//目录/文件名
	//传入的目录参数的根目录为本系统数据文件存放所在目录
	memcpy(szFilePath,SystemCfger::GetInstance().DataFilePath,strlen(SystemCfger::GetInstance().DataFilePath));
	memcpy(&szFilePath[strlen(SystemCfger::GetInstance().DataFilePath)],pParm,PRO104GB_FILEPATH_LEN);
	pParm +=PRO104GB_FILEPATH_LEN ;
	szFilePath[255] = '\0';

	//起始位置
	memcpy(&uiStartPos,pParm,4);

	FileControl fileCtrl;

	if(!fileCtrl.OpenFile(szFilePath,FileControl::OM_READ))
	{
		this->m_errCode = PEC_OPERAT_FILE_FAIL;

		char szLogInfo[500] = {0};
		sprintf(szLogInfo,"Protocol104_GB::ReadGeneralFile():Open the file:%s error",szFilePath);
		mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
		return false;
	}

	fileCtrl.Seek(uiStartPos);

	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE]={0};//存放从数据库读取的数据
	int	iReadMaxLen = PRO104GB_ASDUDATA_MAXSIZE-8;//每次读取的最大长度
	int iFactReadLen = 0;							//实际读取的长度
	unsigned int uiHasReadDataLen = uiStartPos;

	NDE nde;
	nde.NO = 1;
	nde.COUNT = 0;

	while(1)
	{
		iFactReadLen = fileCtrl.ReadData(&szAsduData[6],iReadMaxLen);
		if(iFactReadLen < 0)
		{
			this->m_errCode = PEC_OPERAT_FILE_FAIL;

			char szLogInfo[500] = {0};
			sprintf(szLogInfo,"Protocol104_GB::ReadGeneralFile():Read the file:%s error",szFilePath);
			mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
			return false;
		}

		if( iFactReadLen < iReadMaxLen)
		{
			nde.CONT = 0; //NDE
			nde.NO = 0;
		}
		else
		{
			nde.CONT = 1; //NDE
		}

		//szAsduData[1] = nde;
		memcpy(&szAsduData[1],&nde,1);

		memcpy(&szAsduData[2],&uiHasReadDataLen,4);
		uiHasReadDataLen += iFactReadLen;

		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU104,STRNO_GEN,0,(pCmd->destDevId & 0XFF),szAsduData,iFactReadLen+6))
		{
			return false;
		}

		if( iFactReadLen < iReadMaxLen)
		{
			break;
		}
	}


	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::ReadGeneralFile( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteGeneralFile(Linker *,struct Command*)'
bool Protocol104_GB::WriteGeneralFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_GB::WriteGeneralFile( )");

	//分析命令参数
	if(pCmd->parmLen != 1+ PRO104GB_FILEPATH_LEN + sizeof(struct CP56Time2a) + 4 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_GB::WriteGeneralFile()");
		return false;
	}

	char szFilePath[256]={0};
	struct CP56Time2a fileTime;
	unsigned int uiFileLen = 0;

	char *pTempParm = &pCmd->pParm[1];

	//目录/文件名
	//传入的目录参数的根目录为本系统数据文件存放所在目录
	memcpy(szFilePath,SystemCfger::GetInstance().DataFilePath,strlen(SystemCfger::GetInstance().DataFilePath));
	memcpy(&szFilePath[strlen(SystemCfger::GetInstance().DataFilePath)],pTempParm,PRO104GB_FILEPATH_LEN);
	pTempParm += PRO104GB_FILEPATH_LEN;

	//文件时间	
	memcpy(&fileTime,pTempParm,sizeof(struct CP56Time2a));
	pTempParm += sizeof(struct CP56Time2a);

	//文件长度
	memcpy(&uiFileLen,pTempParm,4);

	bool		bCanWriteFile = true;	//是否可以写文件
	unsigned char COT = 0X14;			//传送原因
	FileControl fileCtrl;

	if(!fileCtrl.CreateEmptyFile(szFilePath,&fileTime))
	{
		this->m_errCode = PEC_OPERAT_FILE_FAIL;

		char szLogInfo[500] = {0};
		sprintf(szLogInfo,"Protocol104_GB::ReadGeneralFile():Open the file:%s error",szFilePath);
		mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
		bCanWriteFile = false;
		COT = 0x15;
	}

	if(!fileCtrl.OpenFile(szFilePath,FileControl::OM_WRITE))
	{
		this->m_errCode = PEC_OPERAT_FILE_FAIL;

		char szLogInfo[500] = {0};
		sprintf(szLogInfo,"Protocol104_GB::ReadGeneralFile():Open the file:%s error",szFilePath);
		mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
		bCanWriteFile = false;
		COT = 0x15;
	}

	//发送通用文件写命令确认ASDU_106
	char szAsduData[PRO104GB_ASDUDATA_MAXSIZE] = {0};
	memcpy(szAsduData,pCmd->pParm,pCmd->parmLen);

	if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU106,STRNO_GLIB,0,(pCmd->destDevId & 0XFF),szAsduData,pCmd->parmLen))
	{
		fileCtrl.CloseFile();
		return false;
	}

	if(!bCanWriteFile)
	{
		fileCtrl.CloseFile();
		return false;
	}

	//接收数据
	char	szFrameData[PRO104GB_FRAME_MAXSIZE+1] = {0};
	int		iFrameLen = PRO104GB_FRAME_MAXSIZE;
	APCI4Prot104	apci(m_iLenOfApdulen);
	PASDUID104		pAsduId = NULL;
	PASDUINFID104	pAsduInfoId = NULL;
	char		*pAsduData = NULL;
	int			iAsduDataLen = 0;

	unsigned int uiFilePos = 0; //文件位置
	unsigned int uiHasReceivLen = 0; //已接收的数据长度
	unsigned char ucContinueFlag = 1; //后续标志

	int iNoDataCount = 0 ;		//连续取不到数据的次数
	DWORD dwT2BeginTime = 0;	//t2开始时间
	DWORD dwT1BeginTime = 0;	//t1开始时间

	while(1)
	{
		//t1超时则退出
		if(dwT1BeginTime > 0 && (GetTickCount()-dwT1BeginTime)/1000 >= P104_T1)
		{
			this->m_errCode = PEC_TIME_OUT;
			pLinker->Close();
			fileCtrl.CloseFile();
			mylog.WriteLog2File(LogControl::LERROR, "Protocol104_GB::WriteGeneralFile( ):Time out");
			return false;
		}

		//如果接收到的I格式帧到达参数w,或者写数据失败，或者远程数据已发送完或者超时，则发送认可帧
		if(m_iNoAckNum >= P104_W || bCanWriteFile == false || ucContinueFlag == 0 || 
			(dwT2BeginTime >0 && (GetTickCount()-dwT2BeginTime)/1000 >= P104_T2 ))
		{
			//发送通用文件写命令确认ASDU_108
			memcpy(&szAsduData[2],&uiHasReceivLen,4);

			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU108,STRNO_GEN,COT,(pCmd->destDevId & 0XFF),szAsduData,6))
			{
				fileCtrl.CloseFile();
				return false;
			}

			m_iNoAckNum = 0;
		}

		if(bCanWriteFile == false || ucContinueFlag == 0)
		{
			break;
		}

		//接收数据
		int iResult = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,50);
		if(iResult == -1) //有错误
		{
			pLinker->Close();
			fileCtrl.CloseFile();
			return NULL;
		}
		else if(iResult == 0) //没有数据
		{
			if(dwT2BeginTime ==0)
			{
				dwT2BeginTime = ::GetTickCount();//t2开始计时
			}

			if(dwT1BeginTime ==0)
			{
				dwT1BeginTime = ::GetTickCount();//t1开始计时
			}

			iNoDataCount++;
			continue;
		}

		iNoDataCount = 0;

		if(apci.GetFrameType() == APCI4Prot104::FT_U)//收到U格式帧
		{
			Task *pNewTask = Protocol104::AnalyseTask(&apci,pLinker);
			if(pNewTask != NULL)
			{
				(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pNewTask);
				continue;
			}
			else
			{
				this->m_errCode = PEC_ERR_FRAME;
				pLinker->Close();
				fileCtrl.CloseFile();
				mylog.WriteLog2File(LogControl::LERROR, "Protocol104_GB::WriteGeneralFile( ):Get an unknow frame");
				return false;
			}

			continue;
		}
		else if(apci.GetFrameType() == APCI4Prot104::FT_S)//收到S格式帧
		{
			//s格式帧则继续
			continue;
		}

		dwT2BeginTime = 0;
		dwT1BeginTime = 0;
				
		//I格式帧		
		if(pAsduId->TYP != PRO104_ASDU104)
		{
			Task *pTask =  AnalyseTask(pAsduId,pAsduInfoId,pAsduData,iAsduDataLen,pLinker);
			if(pTask != NULL)
			{
				if(pTask->cmd == *pCmd) //重复的命令
				{
					delete pTask;
					pTask = NULL;
					continue;
				}

				(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pTask);
			}//end if(pNewTask != NULL)
			else
			{
				m_errCode = PEC_ERR_FRAME;
				pLinker->Close();
				fileCtrl.CloseFile();
				mylog.WriteLog2File(LogControl::LERROR, "Protocol104_GB::WriteGeneralFile( ):Get an unknow frame");
				return false;
			} //end if(pTask != NULL) - esle

			continue;

		}//end if(pAsduId->TYP != PRO104_ASDU104)

		//处理数据
		memcpy(&ucContinueFlag,&pAsduData[1],1);
		ucContinueFlag = ucContinueFlag&0x80;

		memcpy(&uiFilePos,&pAsduData[2],4);

		int iFileDataLen = 0; //文件数据长度
		iFileDataLen = apci.iApduLen - P104_CONTROLAREA_LEN - ASDUID104_LEN - ASDUINFID104_LEN -6;
		uiHasReceivLen += iFileDataLen;

		if(uiFilePos > uiFileLen || (unsigned int)iFileDataLen > uiFileLen)
		{
			bCanWriteFile = false;
			COT = 0X15;
		}

		fileCtrl.Seek(uiFilePos);
		int iFactWriteLen = fileCtrl.WriteData(&pAsduData[6],iFileDataLen);
		if(iFactWriteLen != iFileDataLen)
		{
			bCanWriteFile = false;
			COT = 0X15;
		}
		
	}//end while(1)
	fileCtrl.CloseFile();

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol104_GB::WriteGeneralFile( )");
	return true ;

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

bool Protocol104_GB::StopDataTransfer( Linker * pLinker, struct Command * pCmd )
{
	string sSendData = CreateSFrame();

	if(SendData(pLinker,sSendData.c_str(),sSendData.size(),P104_T2*1000) != sSendData.size())
	{
		return false;
	}

	return true;
}

int Protocol104_GB::SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd)
{
	if(!m_bCanSendData)
	{
		this->m_errCode = PEC_STARTDT_NOACT;
		return 0;
	}

	m_dwBeginT2Time = 0;
	int count = 0;

	//未被确认的帧超过了参数K，则停止发送，等待对方确认
	while( (m_iHasSendNum - m_iHasAckNum) >= P104_K)
	{
		count++;

		if( m_dwBeginT2Time == 0)
		{
			m_dwBeginT2Time = ::GetTickCount();//t2计时开始
		}
		else
		{
			DWORD dwNowTime = GetTickCount();
			if( (dwNowTime - m_dwBeginT2Time)/1000 >= P104_T2)
			{
				//超时未确认
				m_errCode = PEC_TIME_OUT;
				pLinker->Close();
				return -1;
			}
		}

		if(count > 20)
		{
			Sleep(100);
		}

		Task *pNewTask = Decode(pLinker);

		if(pNewTask != NULL)
		{
			if(pNewTask->cmd == *pCmd) //重复发送的任务
			{
				delete pNewTask;
				pNewTask = NULL;
			}
			else if(pNewTask->cmd.funCode == CMD_104GB_COMM_STOP_TRANSFER )
			{
				//远程终止传输
				this->m_errCode = PEC_RMT_STOP_TRANSFER;
				(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pNewTask);

				return 0;
			}
			else if(pNewTask->cmd.funCode == CMD_104_STOPDT)
			{
				//远程终止传输
				this->m_errCode = PEC_RMT_STOP_TRANSFER;
				(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pNewTask);
				m_bCanSendData = false;

				return 0;
			}
			else
			{
				(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pNewTask);
			}
		}//end if(pNewTask != NULL)
		
	}

	string sSendData = CreateIFrame(pAsdu,iAsduLen);

	if(sSendData.size() != SendData(pLinker,sSendData.c_str(),sSendData.size(),P104_T1*1000))
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_GB::CommParamArrange( ):Send ack frame error");
		return -1;
	}

	m_iHasSendNum++;
	return 1;
}

bool Protocol104_GB::CpyGI2SendBuf( char **ppDestBuf, int iBufLen,GI& gi)
{
	if(ppDestBuf == NULL || *ppDestBuf == NULL)
		return false;

	int iGIHeadLen = sizeof(gi.GIN)+sizeof(gi.KOD)+sizeof(gi.gdd);
	if(iGIHeadLen + gi.dataLen > iBufLen)
		return false;

	memcpy(*ppDestBuf,&gi,iGIHeadLen);
	*ppDestBuf += iGIHeadLen;

	memcpy(*ppDestBuf,gi.data,gi.dataLen);
	*ppDestBuf += gi.dataLen;

	return true;
}

bool Protocol104_GB::SendAsduFrame(Linker * pLinker,struct Command * pCmd,unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen )
{
	string sAsdu = this->EncodeAsdu(type,asduStructNo,cot,addr,pAsduData,iAsduDataLen);
	if(sAsdu == "" || sAsdu.size() < 4)
	{
		return false;
	}

	int iResult = SendIFrame(pLinker,sAsdu.c_str(),sAsdu.size(),pCmd);

	if( -1 == iResult )
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_GB::SendAsduFrame( ):Send ASDU error");
		return false;
	}
	else if(iResult == 0)
	{
		return false;
	}

	return true;
}

DWORD WINAPI DeviceThread_104GB(LPVOID pParm)
{
	DWORD dwResult = 0;

	struct tagDevThreadParm104GB *pThreadParm = (struct tagDevThreadParm104GB*)pParm;

	unsigned int ucDevId = pThreadParm->devId;
	map<unsigned int, PDAUINFO>::iterator it = SystemCfger::GetInstance().ConstantDAUInfoMap.find(ucDevId);
	if(it != SystemCfger::GetInstance().ConstantDAUInfoMap.end())//找到设备相关配置
	{
		//创建前台设备
		PDAUINFO pDAUInfo = (*it).second;
		Device *pDev = Device::Build(pDAUInfo,NULL);

		//构造任务
		Task *pTask = new Task();
		pTask->Linker = pDev->GetLinker();
		pTask->cmd.destDevId = ucDevId;
		pTask->cmd.from = CD_REMOTE;
		pTask->cmd.to = CD_ASDU;
		pTask->cmd.funCode = pThreadParm->funCode;
		pTask->cmd.parmLen = pThreadParm->cmdParmLen;
		pTask->cmd.pParm = pThreadParm->pCmdParm;

		if(!pTask->Linker->ExecuteCmd(pTask->cmd))
			dwResult = 2;

		pTask->Linker->Close();

		pTask->Linker = NULL;
		delete pTask;
		pTask = NULL;

		delete pDev;
		pDev = NULL;

		return dwResult;
	}
	else
	{
		return 1;
	}
}

	
	//通用分类命令
	bool GenCommand(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->GenCommand(pLinker,&cmd);
	}

	//调取连续记录波形
	bool GetSerialRecorder(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->GetSerialRecorder(pLinker,&cmd);
	}

	//连续记录检索
	bool QuerySerialData(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->QuerySerialData(pLinker,&cmd);
	}

	//通用文件列表
	bool GeneralFileList(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->GeneralFileList(pLinker,&cmd);
	}

	//通用文件读
	bool ReadGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->ReadGeneralFile(pLinker,&cmd);
	}

	//通用文件写
	bool WriteGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->WriteGeneralFile(pLinker,&cmd);
	}

	//通讯参数协商
	bool CommParamArrange(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->CommParamArrange(pLinker,&cmd);
	}

	//终止数据传输
	bool StopDataTransfer(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->StopDataTransfer(pLinker,&cmd);
	}

// }}}USR
// }}}RME

// }}}RME
