
#include "Protocal104_SH.h"
#include "PQDef.h"
#include "dfrdef.h"
#include "Linker.h"
#include "Task.h"
#include "FileControl.h"
#include "MemMapMgr.h"
#include <string.h>
#include "Encrypt.h"

//##ModelId=4C625508000F
bool Protocal104_SH::GetSHFile(Linker* pLinker, struct Command* pCmd)
{
	// 参数齐全 但需要提取 相关参数 文件位置，类型，名称
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}
	if(pCmd->pParm == NULL||(pCmd->parmLen != (1+4*3+100+19+1)) )
	{
		printf("GetSHFile->parm  error\n");
		return false;
	}

	unsigned short fileType = (unsigned char)pCmd->pParm[0];//文件类型
	unsigned int fileNO = 0;//文件号
	unsigned int fileStartPos = 0;//文件开始位置
	unsigned int fileTotalLen = 0;//文件总长度
	char inputFileName[100]={0};//传入的文件名称
	char fileTime[25]={0};//文件时间:字符串,yyyy-mm-dd hh:MM:ss格式
	char IsNeedCompress = 0;//是否需要压缩，1为需要

	memcpy(&fileNO,&pCmd->pParm[1],4);
	memcpy(&fileStartPos,&pCmd->pParm[1+4],4);
	memcpy(&fileTotalLen,&pCmd->pParm[1+4+4],4);
	memcpy(inputFileName,&pCmd->pParm[1+4*3],100);
	memcpy(fileTime,&pCmd->pParm[1+4*3+100],19);
	IsNeedCompress = pCmd->pParm[1+4*3+100+19];
	//根据命令参数构建I帧并发送

	char szInfo[512]={0};
	sprintf(szInfo, "Get File:%s in Protocal104_SH::GetSHFile()",inputFileName);
	mylog.WriteLog2File(LogControl::LINFO,szInfo);

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
	pAsduInf->INF = 0X2A;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	string sFileTime(fileTime);
	string::size_type pos1 = sFileTime.find("-");
	string::size_type pos2 = sFileTime.find("-",pos1+1);
	string::size_type pos3 = sFileTime.find(" ",pos2+1);
	string::size_type pos4 = sFileTime.find(":",pos3+1);
	string::size_type pos5 = sFileTime.find(":",pos4+1);
	if(pos1 != 4 || pos2 != 7 || pos3 != 10|| pos4!= 13|| pos5 != 16)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PullFile()");
		//mylog.WriteLog(LogControl::LWARNING,pCmd,"error command param");
		pLinker->Close();
		return false;
	}

	string fileYear = sFileTime.substr(0,4);
	string fileMonth = sFileTime.substr(pos1+1,pos2-pos1-1);
	string fileDay = sFileTime.substr(pos2+1,pos3-pos2-1);
	string fileHour = sFileTime.substr(pos3+1,pos4-pos3-1);
	string fileMin = sFileTime.substr(pos4+1,pos5-pos4-1);

	FileControl::FILETYPE fileControlType;	//文件类型
	switch(fileType)
	{
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 105:
		//sRemotefilePath = string(RES_COMTRADE_DIR);
		//sRemotefilePath += ParseFileNameForGetTime(string(inputFileName), false, true);
		fileControlType = FileControl::FT_COMTRADE; //标准comtrade文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is comtrade file");
		break;
	case 10:
		//sRemotefilePath = string(RES_SHD_DIR);
		//sRemotefilePath += ParseFileNameForGetTime(string(inputFileName), false, true);
		fileControlType = FileControl::FT_SHD; //SHD文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SHD file");
		break;
	case 16:
	case 17:
	case 18:
		//sRemotefilePath = string(RES_COMTRADE_DIV_DIR);
		//sRemotefilePath += ParseFileNameForGetTime(string(inputFileName), false, true);
		//sRemotefilePath += fileTimeDir;
		fileControlType = FileControl::FT_COMTRADE_DIV; //分通道文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is DIV file");
		break;
	case 20:
	case 21:
	case 22:
		//sRemotefilePath = string(RES_PQCOMTRADE_DIR);
		//sRemotefilePath += ParseFileNameForGetTime(string(inputFileName), true, true);
		//sRemotefilePath += fileTimeDir;
		fileControlType = FileControl::FT_PQCOMTRADE; //电能质量暂态录波文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is PQComtrade file");
		break;
	case 101:
		//sRemotefilePath = string(RES_MARKENV_DIR);
		fileControlType = FileControl::FT_SOE; //标记事件文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SOE file");
		break;
	case 102:
		//sRemotefilePath = string(RES_LOG_DIR);
		fileControlType = FileControl::FT_LOG; //日记文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is LOG file");
		break;
	case 103:
		//sRemotefilePath = string(RES_PQTREND_DIR);
		//sRemotefilePath += fileYearMon;
		fileControlType = FileControl::FT_PQTRH; //电能质量趋势文件（没分子类型）
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is PQTRH file");
		break;
	case 104:
		//sRemotefilePath = string(RES_PQEVENT_DIR);
		//sRemotefilePath += fileYearMon;
		fileControlType = FileControl::FT_PQEVT; //电能质量事件文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is PQEvt file");
		break;
	case 210:
	case 211:
	case 212:
	case 220:
		//sRemotefilePath = string(RES_CONFIG_DIR);
		fileControlType = FileControl::FT_CFG; //配置相关文件
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is configer file");
		break;
	default:
		mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is unkown type file");
		return false;
	}

	/*if(220 ==fileType)
	{
	sRemotefilePath += string("SHDevice.dat");
	}
	else
	{
	sRemotefilePath += string(inputFileName);
	}*/


	//提取信息
	memcpy(&pASDU[iAsduLen],&fileType, sizeof(unsigned short)); //类型 参数
	iAsduLen+=sizeof(unsigned short);
	//const int szlength = sRemotefilePath.length()>100?100:sRemotefilePath.length();

	if(220 ==fileType)
	{
		memcpy(&pASDU[iAsduLen],"SHDevice.dat",strlen("SHDevice.dat")); //定值
	}
	else
	{
		memcpy(&pASDU[iAsduLen],inputFileName,strlen(inputFileName)); //名称
	}
	iAsduLen+=100;
	memcpy(&pASDU[iAsduLen],&fileStartPos,sizeof(int)); //开始位置
	iAsduLen +=sizeof(int);

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetSHFile():Send SendIFrame error");
		return false;
	}

	//加上年月目录(配置相关不创建年月目录)
	//string sztime =  string("Tmp");//fileYear+fileMonth;
	string sztime =  fileYear+fileMonth;
	bool rst = ProcessGernalFileCmdRst(pLinker,pCmd, 0X20,string(inputFileName),fileTotalLen,sztime, fileControlType);
	return rst;
}

bool Protocal104_SH::ProcessAutoSendData(Linker *pLinker,struct Command* pCmd)
{
	//判断入口参数
	if(NULL==pLinker||!(pLinker->IsConnect()))
	{
		printf("ProcessAutoSendData parm error\n");
		mylog.WriteLog2File(LogControl::LERROR,"ProcessAutoSendData parm error\n");
		return false;
	}

	if(NULL == m_szSrcAutoSendDataBuff)
	{
		mylog.WriteLog2File(LogControl::LERROR,"m_szSrcAutoSendDataBuff==NULL\n");
	}
	else
	{
		memset(m_szSrcAutoSendDataBuff,0, AutoSendDataBuffSize);
	}

	//
	//Apci解析帧
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE;


	APCI4Prot104  apci(m_iLenOfApdulen);
	PASDUID104  pAsduId =NULL;
	PASDUINFID104 pAsduInfoId = NULL;
	char* pAsduData =NULL;
	int iAsduDataLen =0;
	int rst = 0;
	struct SHAuotSendHead * pMsgHead = NULL;

	unsigned char  dataType = 0XFF;
	unsigned char bHasContinusFrame = true;
	short packNo = -1;
	bool bFirst = true;
	int dataLengthofCurFrame = 0;
	SHAuotSendHead* ptagHead = NULL;
	string  szData;

	int tCount =0;
	int actualRecLen =0;
	//Step 1  获取一次结果数据 .(例如3s一次的趋势数据)
	while(pLinker->IsConnect()&& !pLinker->IsPreDisConnect() && bHasContinusFrame)
	{
		//接收区清零
		memset(&szFrameData[0],0,PROSH104_FRAME_MAXSIZE+1);
		//分析帧
		rst = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,50);
		if(-1==rst)
		{
			//有错误
			//printf("AnalyseFrame error\n");
			mylog.WriteLog2File(LogControl::LERROR,"AnalyseFrame error ProcessAutoSendData");
			pLinker->Close();
			return false;
		}
		else if(0 == rst)//没数据
		{ 
			Sleep(100);
			tCount++;

			//自动数据已经被上传过，现超过4分钟没有数据断开连接
			if( tCount>=4*600 )//if(m_bAutoDataHasTrs && tCount>=150)
			{
				pLinker->Close();
				mylog.WriteLog2File(LogControl::LERROR,"TimeOut (4M) ProcessAutoSendData\n");
				m_bAutoDataHasTrs = false;
				return false;
			}
			/*else if(!m_bAutoDataHasTrs && tCount>=900)
			{
			pLinker->Close();
			mylog.WriteLog2File(LogControl::LERROR,"TimeOut (90s) ProcessAutoSendData\n");
			m_bAutoDataHasTrs = false;
			return false;
			}*/
			continue;
		}

		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			tCount =0;
			dataLengthofCurFrame = 0;
			char *pData = pAsduData;
			if(iAsduDataLen<(sizeof(SHAuotSendHead)+1+1+4))
			{
				printf("Frame Len  error\n");
				mylog.WriteLog2File(LogControl::LERROR,"Frame Len  error ,iAsduDataLen<(sizeof(SHAuotSendHead)+1+1+4)\n");
				pLinker->Close();
				return false;
			}
			pData +=1;
			bHasContinusFrame = *pData;  //后续帧标志位
			pData +=1;
			memcpy(&dataLengthofCurFrame, pData, sizeof(int));   //改帧数据部分长度
			pData +=4;
			ptagHead = (SHAuotSendHead*)(pData) ;              //数据帧标记头
			if(bFirst)
			{
				dataType = ptagHead->typ;
				packNo = ptagHead->packNo;
				if(ptagHead->shflag[0]!='S'||ptagHead->shflag[1]!='H')
				{
					mylog.WriteLog2File(LogControl::LERROR,"SHHeadFlag error\n");
					pLinker->Close();
					return false;
				}
				bFirst = false;
			}
			else
			{
				if((dataType !=ptagHead->typ)||
					(packNo !=ptagHead->packNo)||
					ptagHead->shflag[0]!='S'||
					ptagHead->shflag[1]!='H')
				{
					printf("SHHeadFlag error\n");
					mylog.WriteLog2File(LogControl::LERROR,"SHHeadFlag error\n");
					pLinker->Close();
					return false;
				}
			}
			pData +=sizeof(SHAuotSendHead);
			//数据长度校样
			if((dataLengthofCurFrame+sizeof(SHAuotSendHead)+6) != iAsduDataLen)
			{
				printf("Frame len  error\n");
				mylog.WriteLog2File(LogControl::LERROR,"Frame len error\n");
				pLinker->Close();
				return false;
			}
			//保存该帧有效数据
			//szData += string(pData,dataLengthofCurFrame);
			if((actualRecLen+dataLengthofCurFrame)>AutoSendDataBuffSize)
			{
				mylog.WriteLog2File(LogControl::LERROR,"m_szSrcAutoSendDataBuff is too small  !!! n");
				return false;
			}
			memcpy(m_szSrcAutoSendDataBuff+actualRecLen, pData, dataLengthofCurFrame);
			actualRecLen += dataLengthofCurFrame;

		}
		else 
		{
			printf("Frame style(I) error\n");
			mylog.WriteLog2File(LogControl::LERROR,"Frame style(I) error\n");
			pLinker->Close();
			return false;
		}
	}

	//Step2  根据数据类型　组建命令结果　并写入内存
	unsigned int  destDevId = 0;
	unsigned int devId =0;
	//根据IP 获取BOARDid  
	string  ipAddr = pLinker->GetConnectAddr();
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(ipAddr.c_str());
	if(NULL == pDauInf)
	{
		mylog.FormatLogInf(LogControl::LWARNING,"Not find the relative DevBoard() ip=%s \n",ipAddr.c_str());

		pLinker->Close();
		return false;
	}
	else
	{
		destDevId = pDauInf->BoardId;
		devId = pDauInf->DAUId;
	}


	if(!pLinker->IsConnect()&&szData.size()<=0)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"!pLinker->IsConnect()&&szData.size()<=0");
		return false;
	}

	if(m_pMapSectionSH104 != NULL)
		EnterCriticalSection(m_pMapSectionSH104);

	unsigned char pDevEnable = 0;
	map<unsigned int,unsigned char>::iterator it = m_DevAutoSendEnableMap.find(destDevId);
	if( it == m_DevAutoSendEnableMap.end() )
	{
		pDevEnable = (1<<2)|(1<<3);
		m_DevAutoSendEnableMap.insert(std::make_pair(destDevId, pDevEnable));
		if(m_pMapSectionSH104 != NULL)
		{
			LeaveCriticalSection(m_pMapSectionSH104);
		}
		return true;
	}
	else
	{
		pDevEnable = it->second;
	}

	m_bAutoDataHasTrs = true;

	bool bSuccee = false;
	//if(pDevEnable) //modify by libo
	//{
	//string  after_szData;
	int after_szData_len = 0; 
	switch(dataType)
	{
	case 1:    //录波器实时数据
	case 2:    //零漂数据
	case 3:    //同时包含实时显示数据和零漂数据
		//数据格式转化

		printf("Receive RCD_RT_Data Len =  %d \n",actualRecLen );
		//mylog.FormatLogInf(LogControl::LDEBUG,"Receive RCD_RT_Data Len =  %d",actualRecLen);
		after_szData_len = AdjustRTDataStyle(devId,m_szSrcAutoSendDataBuff, actualRecLen);
		if(after_szData_len>0)
		{

			//如果能发送实时数据，则将数据写入共享内存中
			//if(pDevEnable&(1<<0))
			//{
			mylog.WriteLog2File(LogControl::LDETAIL,"write real-time-data to share memery");
			//如果能发送零漂数据，则将数据写入共享内存中
			bSuccee = WriteAutoData2Mem(destDevId,CMD_READ_RLT_DATA,CDRT_POINTDATA,m_szDestAutoSendDataBuff,after_szData_len);
			//pDevEnable &=~(1<<0);
			//}
			if(pDevEnable&(1<<1))
			{
				bSuccee = WriteAutoData2Mem(destDevId,CMD_READ_ZEROOFFSET,CDRT_POINTDATA,m_szDestAutoSendDataBuff,after_szData_len);
				pDevEnable &=~(1<<1);
			}
			m_DevAutoSendEnableMap[destDevId] = pDevEnable;
			bSuccee = true;
		}
		else
		{
			//实时数据转换出错
			printf("AdjustRTDataStyle error\n");
			mylog.WriteLog2File(LogControl::LERROR,"AdjustRTDataStyle error\n");
			bSuccee = false;
		}
		break;
	case 4:    //SHDFR_A_1专用实时显示数据
		break;
	case 5:    //发电机试验数据
		break;
	case 6:	   //电能质量11.25周波实时数据
		//mylog.FormatLogInf(LogControl::LDEBUG,"Receive PQ_RT_Data Len =  %d",actualRecLen );
		printf("Receive PQ_RT_Data Len =  %d \n",actualRecLen );
		if(actualRecLen>0)
		{
			//if(pDevEnable&(1<<2))
			{
				bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTORTDATA,CDRT_POINTDATA,m_szSrcAutoSendDataBuff,actualRecLen);
			}
		}

		if(pDevEnable&(1<<1))
		{
			after_szData_len = PQRTToSchdeclueRtData(devId,m_szSrcAutoSendDataBuff, actualRecLen);
			if(after_szData_len>0)
			{
				bSuccee = WriteAutoData2Mem(destDevId,CMD_READ_ZEROOFFSET,CDRT_POINTDATA,m_szDestAutoSendDataBuff,after_szData_len);
				pDevEnable &=~(1<<1);
			}
			m_DevAutoSendEnableMap[destDevId] = pDevEnable;
		}
		break;
	case 7:    //电能质量趋势数据
		printf("Receive PQ_Trend_Data Len =  %d \n",actualRecLen );
		//mylog.FormatLogInf(LogControl::LDEBUG,"Receive PQ_Trend_Data Len =  %d",actualRecLen);
		if(actualRecLen>0)
		{
			//if(pDevEnable&(1<<3))
			{
				bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTOTRENDDATA,CDRT_PQ_TRENDDATA,m_szSrcAutoSendDataBuff,actualRecLen);
			}
		}
		break;
	case 8:		//电能质量事件信息
		printf("Receive PQ_Evt_Data Len =  %d \n",actualRecLen );

		if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTOEVENT,CDRT_PQ_EVENT,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 9:     //录波启动信息
		printf("Receive Rcd_StartInfo_Data Len =  %d \n",actualRecLen );
		if(actualRecLen>0)
		{
			mylog.FormatLogInf(LogControl::LDEBUG,"Receive Rcd_StartInfo_Data Len =  %d \n",actualRecLen);
			bSuccee = WriteAutoData2Mem(destDevId,CMD_STARTINFO,CDRT_STARTRECMSG,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 10:	//标记事件信息
		if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_AUTOSEND_MARKEVENT,CDRT_MARKEVENT,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 11:    //故障录波简报
		if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_AUTOSEND_REPORTFILE,CDRT_REPORT,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 12:    //电能质量趋势文件完结信息
		printf("Receive PQ_FileInf_Data Len =  %d \n",actualRecLen );
		mylog.FormatLogInf(LogControl::LDEBUG,"PQ_FileInf_Data Len =  %d \n",actualRecLen);
		if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTOFILEINF,CDRT_PQ_FILEINF,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 13:    //电能质量10周波周期数据
		printf("Receive PQ_10Cyc_Data Len =  %d \n",actualRecLen );
		mylog.FormatLogInf(LogControl::LDEBUG,"Receive PQ_10Cyc_Data Len =  %d \n",actualRecLen);
		//if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTOT10CYCDATA,CDRT_PQ_10CYCDATA,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	case 14:    //电能质量统计完成信息
		printf("Receive PQ_StatInf Len =  %d \n",actualRecLen );
		mylog.FormatLogInf(LogControl::LDEBUG,"Receive PQ_StatInf Len =  %d \n",actualRecLen);
		//if(actualRecLen>0)
		{
			bSuccee = WriteAutoData2Mem(destDevId,CMD_PQ_AUTOTSTATINF,CDRT_PQ_STATINF,m_szSrcAutoSendDataBuff,actualRecLen);
		}
		break;
	default: 
		mylog.FormatLogInf(LogControl::LDEBUG,"Unknow auto data type:%d",dataType);
		break;
	}
	/*}
	else
	{
	mylog.FormatLogInf(LogControl::LDEBUG,"Disnable auto data,:%d",pDevEnable);
	bSuccee =false;
	}*/
	if(m_pMapSectionSH104 != NULL)
		LeaveCriticalSection(m_pMapSectionSH104);
	return bSuccee;

}
//第一个参数为设备板ID 
bool Protocal104_SH::WriteAutoData2Mem(unsigned int devBoradId, unsigned char funcode, CMDRESULTTYPE resultType, const char *data, int data_len)
{
	if(data==NULL||data_len<=0||devBoradId<=0)
	{
		mylog.WriteLog2File(LogControl::LERROR,"data==NULL||data_len<=0||devBoradId<=0");
		return false;
	}
	PCMDRESULT pResult = new CMDRESULT();
	if(NULL == pResult)
	{
		mylog.WriteLog2File(LogControl::LERROR,"WriteAutoData2Mem, no memory for CmdRst to use");
		return false;
	}
	struct Command cmd;
	cmd.destDevId = devBoradId;
	cmd.from = CD_ASDU;
	cmd.msgId = -1;
	cmd.resultNum = 1 ;
	cmd.funCode = funcode;

	pResult->resultType = resultType;

	pResult->resultLen = data_len;
	pResult->pData = new char[pResult->resultLen+1];
	if(NULL == pResult->pData)
	{
		mylog.WriteLog2File(LogControl::LERROR,"WriteAutoData2Mem, no memory for pResult->pData to use");
		return false;
	}
	memset(pResult->pData,0,pResult->resultLen+1);
	memcpy(pResult->pData,data,pResult->resultLen);	
	cmd.resultList.push_back(pResult);

	MemMapMgr memMgr;
	memMgr.WriteTaskResultToMem(cmd);
	return true;
}

//调整监控板发送实时数据. 使其格式为后台类型
int  Protocal104_SH::AdjustRTDataStyle(int devId, char* szData, int srclen)
{

	const int FRM_HEAD_RT_LEN = sizeof(FRM_HEAD_RT);
	int data_len = srclen;
	if( data_len <= FRM_HEAD_RT_LEN )
	{
		mylog.FormatLogInf(LogControl::LWARNING, "The DataLen is:%d,short than FRM_HEAD_RT",data_len);
		return 0;
	}
	const char* pData = szData;


	if(NULL==m_szDestAutoSendDataBuff)
	{
		return 0;
	}
	else
	{
		memset(m_szDestAutoSendDataBuff, 0 , AutoSendDataBuffSize);
	}
	FRM_HEAD_RT headrt;	//原始实时数据头结构

	memcpy(&headrt,pData,FRM_HEAD_RT_LEN );
	char *pSrcData = (char *)pData;
	pSrcData += FRM_HEAD_RT_LEN;

	//const int PERSMP_DATA_LEN = headrt.AnChCnt*2 + 8*(( headrt.BinChCnt+63)/64) +16;
	const int PERSMP_DATA_LEN = headrt.AnChCnt*2 + 2*((headrt.BinChCnt+15)/16) +16;

	/*
	if((headrt.AnChCnt%32!=0)||(headrt.BinChCnt%64!=0))
	{
	mylog.FormatLogInf(LogControl::LWARNING, "The FRM_HEAD_RT:AnChCnt %d,BinChCnt %d",headrt.AnChCnt,headrt.BinChCnt);
	return 0;
	}
	*/

	//计算计算结果长度
	int iComputerRst = sizeof(ANCH_VAL)*headrt.AnChCnt + sizeof(ULIN_VAL)*headrt.ULinCnt + 
		sizeof(ILIN_VAL)*headrt.ILinCnt + sizeof(GN_VAL)*headrt.GnCnt + sizeof(TRN_VAL)*headrt.TrnCnt;

	if( data_len < (FRM_HEAD_RT_LEN + iComputerRst + headrt.CycleCnt*headrt.SmpRate*PERSMP_DATA_LEN +4 ) )
	{
		mylog.FormatLogInf(LogControl::LWARNING, "The DataLen is:%d,short than %d",
			data_len,(FRM_HEAD_RT_LEN + iComputerRst + headrt.CycleCnt*headrt.SmpRate*PERSMP_DATA_LEN ));
		return 0;
	}
	pSrcData += iComputerRst;


	CDeviceIniMgr *pDevIniMgr = &(SystemCfger::GetInstance().m_IniMgr);
	CDeviceIni *pDevIni = pDevIniMgr->GetDevice(devId);
	if(NULL==pDevIni)
	{
		pDevIniMgr->AddDevice(0,devId,SystemCfger::GetInstance().DataFilePath);

		pDevIni = pDevIniMgr->GetDevice(devId);
		if(NULL==pDevIni)
		{
			mylog.FormatLogInf(LogControl::LWARNING, "No find device(id:%d)'s ini file in Protocol61850::AutoRltData()",devId);
			return 0;
		}
	}
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

	//
	const int RTTotalLen = iNewDataLen + sizeof(POINTDATAHEAD);

	char *pDestData = m_szDestAutoSendDataBuff;
	memcpy(pDestData,&point,sizeof(point)); 
	pDestData += sizeof(point);//偏移头

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
#if 0
		short * pCh = (short*)(pDestData+8*2);//偏移8个通道 ，每个通道2字节
		printf("%d  %d  %d \n", *pCh , *(pCh+1), *(pCh+2));
#endif
		pDestData += point.ChannelNumber*2;
		//data += headrt.AnChCnt*2;

		//拷贝开关通道数据
		memcpy(pDestData,&pSrcData[FPGA_ANA_DATA_OFFSET+headrt.AnChCnt*2],iMinSwitchNumber*2);
		pDestData += point.SwitchNumber*2;
		pSrcData += PERSMP_DATA_LEN;
	}
	pDestData= NULL;
	pSrcData = NULL;

	return RTTotalLen;
}	

bool Protocal104_SH::ProcessComtradeFileCmdRst(Linker *pLinker,struct  Command *pCmd, int asduNo ,FileControl::FILETYPE fileControlType)
{
	//判断是否为Comtrade 格式
	if( (fileControlType != FileControl::FT_CONT)&&
		(fileControlType != FileControl::FT_COMTRADE)&&
		(fileControlType != FileControl::FT_CCOMTRADE)&&
		(fileControlType != FileControl::FT_PQCOMTRADE))
	{
		return false;
	}
	char fileName[100]={0};
	unsigned int fileLength =0;
	int pos =0;
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE; 
	int rst = 0;
	bool  bFirstFrame = true;
	char bHasContinueFrame = true;
	char szInfo[512]={0};

	APCI4Prot104  apci(m_iLenOfApdulen);
	PASDUID104  pAsduId =NULL;
	PASDUINFID104 pAsduInf = NULL;
	char* pAsduData =NULL;
	int iAsduDataLen =0;

	int iMaxValidateLen = PROSH104_FRAME_MAXSIZE-apci.GetApciLen();
	iMaxValidateLen-=ASDUID104_LEN;
	iMaxValidateLen-=ASDUINFID104_LEN;
	iMaxValidateLen-=(1+1+4);  //后续帧表示 + 起始位置
	bool error = false;
	//文件目录信息
	//构建文件目录 
	int destDevId = pCmd->destDevId&0XFF;
	FileControl fileCtrl;
	string comtradeDir;
	unsigned int devId = 0;
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		pLinker->Close();
		return false;
	}
	else
	{
		devId = pDauInf->DAUId;
	}
	comtradeDir = fileCtrl.GetFileTypeDirector(devId,fileControlType);

	if(comtradeDir.compare("") == 0)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "get the director of file type fail in Protocol61850::GetRecWaveFile()");
		pLinker->Close();
		return false;
	}

	//	string lcaCfgFileName ,lcaDateFileName,lcaHdrFileName,lcaInfFileName,lcaShcFileName,lcaSheFileName;
	string  localFiles[8] ;
	//保存文件
	bool bSaveSucess[8] = {false,false,false,false,false,false,false,false};
	///////////////////
	char iCurComtradefileType = 0;
	string sCurrentFileName;
	//处理进度信息
	unsigned int iCounter=0;			//帧循环次数
	int iProgress = 0;		//当前进度
	int iStepSize  = 0;     //该项纪录步长
	MemMapMgr memMgr;
	unsigned int iHasReceivedSize =0;
	bool bReceiveDat = false;
	char szSaveRemainData[5] = {0};	//保存上次未处理的数据
	int  iRemainDataLen = 0 ;	//未处理的数据长度
	int last_value = -1;
	//取数据
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		int iAsduLen =0;
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
		else if(0 == rst)//没数据
		{
			printf("Get CmdRst TimeOut  error\n");
			mylog.WriteLog2File(LogControl::LERROR, "Get CmdRst TimeOut in Protocal104_SH::ProcessComtradeFileCmdRst()\n");
			pLinker->Close();
			error = true;
			break;
		}

		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				printf("Unkown ASDU \n");
				mylog.WriteLog2File(LogControl::LWARNING, "Unkown ASDU");
				error = true;
				pLinker->Close();
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				printf("Get pAsduId->COT ==15 \n");
				mylog.WriteLog2File(LogControl::LWARNING, "Get pAsduId->COT ==15");
				break;
			}
			if(pAsduId->TYP!=asduNo)
			{
				printf("ASDU TYP error\n");
				mylog.WriteLog2File(LogControl::LWARNING, "ASDU TYP error");
				error = true;
				pLinker->Close();
				break;
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;
			if(validatelen>0)
			{	
				if(validatelen>=2)
				{
					bHasContinueFrame = *(pDataTmp+1); //后续标示 ???wenti 
					validatelen-=2;
					pDataTmp +=2;
				}
				else
				{
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
				//Comtrade 格式
				if(bFirstFrame)
				{
					struct CP56Time2a   time;
					if(validatelen>= 40+sizeof(struct CP56Time2a))
					{
						bFirstFrame = false;
						//文件名称 + 时间
						memcpy(&fileName[0],pDataTmp,40);
						pDataTmp +=40;
						validatelen -= 40;

						memcpy(&time,pDataTmp,sizeof(struct CP56Time2a));
						pDataTmp +=sizeof(struct CP56Time2a);
						validatelen -= sizeof(struct CP56Time2a);
					}
					else
					{
						printf("ASDU DATA error\n");
						mylog.WriteLog2File(LogControl::LERROR,"ASDU DATA error\n");
						error = true;
						pLinker->Close();
						break;
					}
					//获取目录信息
					{
						//分析文件时间 本地目录是只有年月子目录/200910/
						/*
						int year = 2000 + time.year;
						int month = time.month;
						int iYearMonth = year*100 + month;
						char szTimeDir[8] = {0};
						sprintf(szTimeDir,"%d\\",iYearMonth);
						*/
						string szTimeDir = ParseFileNameForGetTime(string(fileName), false,false);
						if(szTimeDir.size()<7)
						{
							return false;
						}
						string sPath = comtradeDir + szTimeDir;
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
						string sFilePathWithoutExt = comtradeDir +szTimeDir + string(fileName);
						//本地文件名
						localFiles[0]  = sFilePathWithoutExt + string(".CFG");
						localFiles[1]  = sFilePathWithoutExt + string(".INF");
						localFiles[2]  = sFilePathWithoutExt + string(".DAT");
						localFiles[3]  = sFilePathWithoutExt + string(".HDR");
						localFiles[4]  = sFilePathWithoutExt + string(".INI");
						localFiles[5]  = sFilePathWithoutExt + string(".XML");
						localFiles[6]  = sFilePathWithoutExt + string(".SHE");
						localFiles[7]  = sFilePathWithoutExt + string(".SHC");
					}
				}//end firist
				if(validatelen<=0)
				{
					continue;
				}

				while(validatelen>0)   //处理该帧数据部分
				{
					if(iCurComtradefileType ==0)
					{
						//获取类型
						iCurComtradefileType = (char)*pDataTmp;        //hdr 文件
						pDataTmp++;
						validatelen--;
						fileLength =0;
						//
						if(iCurComtradefileType<1||iCurComtradefileType>8)
						{
							printf("FileTyp %d Error! \n",iCurComtradefileType );
							pLinker->Close();
							return false ;
						}
						sCurrentFileName = localFiles[iCurComtradefileType-1];
						bool bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_WRITE);
						if( !bOpenFlag )
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" open failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							return false;
						}
						sprintf(szInfo, "StartDownLoad FN:%s ",sCurrentFileName.c_str());
						mylog.WriteLog2File(LogControl::LINFO, szInfo);
					}
					//处理 断口 
					if(fileLength==0)
					{
						if(iRemainDataLen+validatelen>=4)
						{
							memcpy(&szSaveRemainData[iRemainDataLen], pDataTmp, (4-iRemainDataLen));
							pDataTmp +=(4-iRemainDataLen);
							memcpy(&fileLength, szSaveRemainData, sizeof(int));
							memset(&szSaveRemainData[0], 0, 5);
							validatelen -= (4-iRemainDataLen);
							iRemainDataLen =0;
							iHasReceivedSize =0;
						}
						else  //不满 4 字节
						{
							memcpy(&szSaveRemainData[iRemainDataLen], pDataTmp, validatelen);
							pDataTmp += validatelen;
							iRemainDataLen += validatelen;
							validatelen =0;
							iHasReceivedSize =0;
							continue ;
						}
					}
					//判断当前文件长度及类型信息
					if(iHasReceivedSize+validatelen<fileLength) //全部写入
					{
						int iwrite = validatelen;
						if(iwrite != fileCtrl.WriteData(pDataTmp,iwrite))
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" write  failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							fileCtrl.CloseFile();
							return false;
						}
						iHasReceivedSize+=validatelen;
						validatelen=0;

					}
					else if((iHasReceivedSize+validatelen -fileLength )<5)
					{
						//写入数据
						int iwrite = fileLength-iHasReceivedSize;
						if(iwrite != fileCtrl.WriteData(pDataTmp,iwrite))
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" write  failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							fileCtrl.CloseFile();
							return false;
						}
						pDataTmp+=iwrite;
						validatelen -= iwrite;
						bSaveSucess[iCurComtradefileType-1] = true; //当前文件正常获取
						sprintf(szInfo, "Receive FN:%s, filedataLen:%d",localFiles[iCurComtradefileType-1].c_str() ,fileLength);
						mylog.WriteLog2File(LogControl::LINFO, szInfo);
						iCurComtradefileType =0;	//当前文件处理完毕
						iHasReceivedSize =0;
						fileLength = 0;
						//关闭文件
						fileCtrl.CloseFile();
						//获取文件类型及 部分文件长度 
						if(validatelen>0)
						{
							iCurComtradefileType = *pDataTmp;
							pDataTmp++;
							validatelen--;
							//打开新文件
							if(iCurComtradefileType<1||iCurComtradefileType>8)
							{
								printf("FileTyp %d Error! \n",iCurComtradefileType );
								pLinker->Close();
								return false ;
							}
							sCurrentFileName = localFiles[iCurComtradefileType-1];
							bool bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_WRITE);
							if( !bOpenFlag )
							{
								m_errCode = PEC_UNKOWN;
								string info = string(sCurrentFileName.c_str()) + string(" open failed in WriteComtreadeFile()");
								mylog.WriteLog2File(LogControl::LERROR,info.c_str());
								return false;
							}
							sprintf(szInfo, "StartDownLoad FN:%s ",sCurrentFileName.c_str());
							mylog.WriteLog2File(LogControl::LINFO, szInfo);
						}
						//保留断开文件长度
						if(validatelen>0)
						{
							iRemainDataLen = validatelen;
							memcpy(&szSaveRemainData[0],pDataTmp, validatelen);
							validatelen =0;
						}
					}
					else  //  当前帧包含多个文件的数据内容
					{
						//写入数据
						int iwrite = fileLength-iHasReceivedSize;
						if(iwrite != fileCtrl.WriteData(pDataTmp,iwrite))
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" write  failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							fileCtrl.CloseFile();
							return false;
						}
						pDataTmp+=iwrite;
						validatelen -= iwrite;      //该帧剩余的数据
						bSaveSucess[iCurComtradefileType-1] = true;  //当前文件正常获取
						sprintf(szInfo, "Receive FN:%s, filedataLen:%d",localFiles[iCurComtradefileType-1].c_str() ,fileLength);
						mylog.WriteLog2File(LogControl::LINFO, szInfo);
						iCurComtradefileType =0;	//当前文件处理完毕
						iHasReceivedSize =0;
						fileLength = 0;
						//关闭文件
						fileCtrl.CloseFile();

						//获取文件类型
						iCurComtradefileType = *pDataTmp;        //取文件类型
						if(iCurComtradefileType<1||iCurComtradefileType>8)
						{
							printf("FileTyp %d Error! \n",iCurComtradefileType );
							pLinker->Close();
							return false ;
						}
						pDataTmp++;
						validatelen--;
						//打开文件
						sCurrentFileName = localFiles[iCurComtradefileType-1];
						bool bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_WRITE);
						if( !bOpenFlag )
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" open failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							return false;
						}
						sprintf(szInfo, "StartDownLoad FN:%s ",sCurrentFileName.c_str());
						mylog.WriteLog2File(LogControl::LINFO, szInfo);
						//获取文件长度 
						memcpy(&fileLength, pDataTmp,sizeof(int));
						pDataTmp +=sizeof(int);
						validatelen-=sizeof(int);
					}
				}

				//进度处理，如果为Comtrade文件 则对的DAT处理，如果是其他一般文件 进度条
				if((iCurComtradefileType==3)&&(fileLength>0))
				{
					if(validatelen>0)
					{
						iHasReceivedSize +=validatelen;
					}
					if(iCounter == 0)
					{
						if(fileLength>0)
						{
							//文件长度/每帧发送的数据长度*20  进度最多到95 后剩余的时间用于处理其他过程 最后到100
							iStepSize = fileLength/20; //
							if(iStepSize<=0)
							{
								iStepSize=20;
							}
						}
						else
						{
							printf("filelen error\n");
							mylog.WriteLog2File(LogControl::LERROR,"filelen error\n");
							error = true;
							break;
						}
					}
					iCounter++;	
					if((iHasReceivedSize/iStepSize !=last_value) &&(iHasReceivedSize<=fileLength))
					{
						last_value = iHasReceivedSize/iStepSize;
						PCMDRESULT pResult = new CMDRESULT();
						if(pResult==NULL)
						{
							printf("Apply Memory fail !\n");

							mylog.WriteLog2File(LogControl::LERROR,"pResult = new CMDRESULT();Return NULL\n");
							continue;
						}
						memset(pResult,0,sizeof(CMDRESULT));
						pResult->resultType = CDRT_PROGRESS;
						pResult->resultLen = 1;
						pResult->pData = new  char[2];
						if(NULL==pResult->pData)
						{
							mylog.WriteLog2File(LogControl::LERROR,"pResult->pData = new char[2];Return NULL\n");
							if(pResult)
							{
								printf("Apply Memory fail !\n");
								delete pResult;
								pResult = NULL;
							}
							continue;
						}

						pResult->pData[1] = '\0';

						float  fPrgs = 100.0f;
						fPrgs = (fPrgs*iHasReceivedSize)/fileLength;
						iProgress = (int)fPrgs;
						iProgress = (iProgress>100)?100:iProgress;
						//	printf("#%d--",iProgress);
						pResult->pData[0] = iProgress;
						pCmd->resultList.push_back(pResult);
						pCmd->resultNum = 1;
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
			}//end if 
		}//end APCI4Prot104::FT_I
	}//end  取数据
	//printf("\n");
	if(iCurComtradefileType>0&&iCurComtradefileType<9)
	{
		fileCtrl.CloseFile();
		bSaveSucess[iCurComtradefileType-1] = true; //当前文件正常获取
		sprintf(szInfo, "Receive FN:%s, filedataLen:%d",localFiles[iCurComtradefileType-1].c_str() ,iHasReceivedSize);
		mylog.WriteLog2File(LogControl::LINFO, szInfo);
	}

	//返回
	PCMDRESULT pResult = NULL;
	if((bSaveSucess[1]==true)&&(bSaveSucess[2]==true)&&(bSaveSucess[0]==true))
	{
		//进度条到100
		pCmd->resultNum = 1;
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_PROGRESS;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[1] = '\0';
		pResult->pData[0] = 100;
		pCmd->resultList.push_back(pResult);
		memMgr.WriteTaskResultToMem(*pCmd);
		//清除结果
		delete pResult;
		pResult = NULL;
		pCmd->resultNum = 0;
		pCmd->resultList.clear();
		//返回写成功的文件名
		for( int j =0; j<3; j++)
		{
			if(bSaveSucess[j])
			{
				pResult = new CMDRESULT();
				if(pResult==NULL)
				{
					printf("Apply Memory fail !\n");
					mylog.WriteLog2File(LogControl::LERROR,"pResult = new CMDRESULT();Return NULL\n");
					continue;
				}
				pResult->resultType = CDRT_FILEPATH;
				pResult->resultLen = localFiles[j].size();
				pResult->pData = new char[pResult->resultLen+1];
				if(NULL==pResult->pData)
				{
					printf("Apply Memory fail !\n");
					if(pResult)
					{
						delete pResult;
						pResult = NULL;
					}
					mylog.WriteLog2File(LogControl::LERROR,"pResult->pData = new char[];Return NULL\n");
					continue;
				}
				pResult->pData[pResult->resultLen] = 0 ;
				memcpy(pResult->pData,localFiles[j].c_str(),pResult->resultLen);
				pCmd->resultList.push_back(pResult);
				pCmd->resultNum +=1;
				pResult = NULL;
				//	printf("###SuccFN, %s\n",localFiles[j].c_str());
			}
		}
		pResult = new CMDRESULT();
		if(NULL==pResult)
		{
			return false;
		}
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		if(pResult->pData==NULL)
		{
			delete pResult;
			pResult = NULL;
			return false;
		}
		pResult->pData[0] = 1 ; //执行成功
		pResult->pData[1] = 0 ;
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum +=1;

		//返回结果标识
		return true;
	}
	else
	{
		pCmd->resultNum = 1;
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0 ; //执行失败
		pResult->pData[1] = 0 ;
		pCmd->resultList.push_back(pResult);
	}

	if(error)
	{
		return false ;
	}
	return true;
}

//取命令返回   返回的命令中都加后续帧标示
bool Protocal104_SH::GetCmdRst(Linker *pLinker, int asduNo,int inf, string &szRstData )
{
	if(NULL==pLinker)
	{
		return false;
	}
	char fileName[100]={0};
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE; 
	int rst = 0;
	bool  bFirstFrame = true;
	char bHasContinueFrame = true;

	APCI4Prot104  apci(m_iLenOfApdulen);
	PASDUID104  pAsduId =NULL;
	PASDUINFID104 pAsduInf = NULL;
	char* pAsduData =NULL;
	int iAsduLen =0;


	string szfileData;
	bool IsError = false;

	int timeOut =14000;
	if(asduNo==0X20 && inf==0XA8) //更新 DSP 程序需较长时间目前设为等待244秒 监控板等待SVC为240秒
	{
		timeOut = 4*61*1000; 
	}
	//取数据
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		iAsduLen =0;
		memset(&szFrameData[0],0,PROSH104_FRAME_MAXSIZE+1);
		rst = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInf,&pAsduData,&iAsduLen,timeOut); //更新 DSP 程序需较长时间目前设为等待244秒 监控板等待SVC为240秒
		if(-1==rst)
		{
			//有错误
			printf("AnalyseFrame error\n");
			mylog.WriteLog2File(LogControl::LERROR, "AnalyseFrame error in Protocal104_SH::GetCmdRst(),分析数据错误");
			pLinker->Close();
			IsError = true;
			break;
		}
		else if(0 == rst)//没数据
		{
			printf("Get CmdRst TimeOut  error\n");
			mylog.WriteLog2File(LogControl::LERROR, "Get CmdRst TimeOut,没有数据");
			pLinker->Close();
			IsError = true;
			break;
		}

		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				printf("Unkown ASDU \n");
				mylog.WriteLog2File(LogControl::LERROR, "Unkown ASDU in Protocal104_SH::GetCmdRst()");
				IsError = true;
				pLinker->Close();
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				mylog.WriteLog2File(LogControl::LERROR, 
					"Get pAsduId->COT ==15 in Protocal104_SH::GetCmdRst()");
				printf("Get pAsduId->COT ==15 \n");
				IsError = true;
				break;
			}

			if(pAsduId->TYP!=asduNo)
			{
				mylog.WriteLog2File(LogControl::LERROR, 
					"ASDU TYP error in Protocal104_SH::GetCmdRst()");
				printf("ASDU TYP error\n");
				IsError = true;
				pLinker->Close();
				break;
			}
			if(asduNo == 0X20)
			{
				//验证信息序号
				if(inf != pAsduInf->INF)
				{
					printf("ASDU INF error\n");
					mylog.WriteLog2File(LogControl::LERROR,
						"ASDU INF error in Protocal104_SH::GetCmdRst()");
					IsError = true;
					pLinker->Close();
					break;
				}
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;
			if(validatelen>0)
			{	
				if(validatelen>=2)
				{
					bHasContinueFrame = *(pDataTmp+1); //后续标示
					validatelen-=2;
					pDataTmp +=2;
				}
				else
				{
					mylog.WriteLog2File(LogControl::LERROR,
						"validatelen error in Protocal104_SH::GetCmdRst()");
					IsError = true;
					pLinker->Close();
					break;
				}

				if(validatelen>0)
				{
					//保存携带de数据
					string  szDataOfCurFrame(pDataTmp, validatelen);
					szfileData += szDataOfCurFrame;
				}
			}//end if 
		}//end APCI4Prot104::FT_I
	}//end  取数据

	if(IsError)
	{
		return false ;
	}
	szRstData = szfileData;
	return true;
}

bool Protocal104_SH::InitConnect(Linker *pLinker)
{
	m_iLenOfApdulen = 2;
	m_bAutoDataHasTrs = false;

	if(pLinker==NULL&&!pLinker->IsConnect())
	{
		return false;
	}

	char szCryptKey[34] = {0};
	char szCryptDevCode[34] = {0};
	char szVal[64] = {0};
	char szFile[256] = {0};

	sprintf(szFile,"c:/data/%d/authcfg.ini",pLinker->GetDevId());
	GetPrivateProfileString("AuthInf","key","",szVal,sizeof(szVal)-1,szFile);
	SHDecrypt(szVal,szCryptKey);
	GetPrivateProfileString("AuthInf","dev_code","",szVal,sizeof(szVal)-1,szFile);
	SHDecrypt(szVal,szCryptDevCode);

	SHEncrypt(szCryptKey,szCryptDevCode,szVal);

	//根据命令参数构建I帧并发送

	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;

	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];
	pAsduId->TYP = 0X21;
	pAsduId->VSQ = 0X81;
	pAsduId->COT = 0X14;
	pAsduId->COMADDR = 0;

	iAsduLen += ASDUID104_LEN;
	PASDUINFID104 pAsduInf = (PASDUINFID104)&pASDU[ASDUID104_LEN];
	pAsduInf->FUN = 0XFF;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],szVal,strlen(szVal));//发送经过加密的设备码
	iAsduLen+=strlen(szVal);//参数中的长度
	m_iHasSendNum =0;
	m_iHasReceivNum =0;


	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,NULL);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::InitConnect():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X22, 0,szRstData );
	//返回命令结果到UI
	if(rst)
	{
		char result[3] = {0};
		if(szRstData.length()>=2)
		{
			memcpy(&result[0], szRstData.c_str(), 2*sizeof(char));
			if(result[0]=='S' &&result[1]=='H')
			{
				return true;
			}
		}
	}

	pLinker->Close();
	return false;
}

int Protocal104_SH::DealAutoSend(Linker *pLinker)
{
	printf("Enter Protocal104_SH::AutoSendData \n");
	//判断入口参数
	if(NULL==pLinker||!(pLinker->IsConnect()))
	{
		printf("AutoSendData parm error\n");
		mylog.WriteLog2File(LogControl::LERROR,"AutoSendData parm error\n");
		return -1;
	}

	int rst = 0;

	while(pLinker->IsConnect() && !pLinker->IsPreDisConnect() )
	{
		rst = ProcessAutoSendData(pLinker, NULL);  //一次处理一个完整的上送结果
		if(!rst)
		{
			mylog.WriteLog2File(LogControl::LERROR,"Deal with  AutoData fail");
			break;
		}
		//Sleep(100);
	}
	pLinker->Close();
	printf("Exit Protocal104_SH::AutoSendData \n");
	mylog.WriteLog2File(LogControl::LINFO,"Exit Protocal104_SH::AutoSendData");
	return rst;
}

//
Task * Protocal104_SH::Decode(Linker *pLinker)
{
	char	szFrameData[PROSH104_FRAME_MAXSIZE+1] = {0};
	int		iFrameLen = PROSH104_FRAME_MAXSIZE;
	APCI4Prot104	apci(2);
	PASDUID104		pAsduId = NULL;
	PASDUINFID104	pAsduInfoId = NULL;
	char		*pAsduData = NULL;
	int			iAsduDataLen = 0;
	int iResult = 0;

	unsigned int nWaitTime = 50;

	while(1)
	{
		iResult = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,nWaitTime);
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
			Task *pTask =  AnalyseTask(pAsduId,pAsduData,iAsduDataLen,pLinker);
			return pTask;
		}
		else if(apci.GetFrameType() == APCI4Prot104::FT_U)
		{
			Task *pTask = Protocol104::AnalyseTask(&apci,pLinker);
			if(pTask)
			{
				string  ipAddr = pLinker->GetConnectAddr();
				PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(ipAddr.c_str());
				if(NULL == pDauInf)
				{
					printf("Not find the relative DevBoard, the Linker Ip =%s \n", ipAddr);
					return NULL;
				}
				else
				{
					pTask->cmd.destDevId=pDauInf->BoardId;
				}
			}
			return pTask;
		}
		else
		{
			//s格式帧则继续解析任务,继续时，等待时间减到最小
			nWaitTime = 1;
			continue;
		}
	} //end while(1)
}


//目前只判断主动上送数据的类型(PROSH104_ASDU241)， 其他非PROSH104_ASDU241的不处理　
Task* Protocal104_SH::AnalyseTask(PASDUID104 pAsduId, char *pAsduData, int iAsduDataLen, Linker *pLinker)
{
	if(pAsduId == NULL || pAsduData == NULL)
	{
		return NULL;
	}
	//判断是否为ASDU241  主动上送数据
	if(pAsduId->TYP != 0XF1)
	{
		//写日志 收到的数据并非ASDU241主动上送数据
		return NULL;
	}
	//构建任务
	Task *pTask = new Task();
	pTask->Linker = pLinker;
	pTask->cmd.destDevId = (pAsduId->COMADDR)&0X00FF;
	pTask->cmd.from = CD_REMOTE;
	pTask->cmd.funCode = CMD_PQAUTOSEND;
	strcpy(pTask->cmd.describe,"主动上送数据");

	//是ASDU21，还要将信息序号作为参数传给命令
	if(iAsduDataLen>0)
	{
		pTask->cmd.parmLen = iAsduDataLen;
		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);
		memcpy(pTask->cmd.pParm,pAsduData+1,pTask->cmd.parmLen);
	}
	return pTask;
}

bool Protocal104_SH::CallGetPQWaveFile(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetPQRecWaveFile(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetPQEvent(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetPQEvent(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetRecWaveFileList(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetRecWaveFileList(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetRecWaveFile(Protocal104_SH *pProtocol, Linker *pLinker,struct Command& pCmd)
{
	return pProtocol->GetRecWaveFile(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetContData(Protocal104_SH *pProtocol, Linker *pLinker,struct Command& pCmd)
{
	return pProtocol->GetContData(pLinker,&pCmd); 
}

bool Protocal104_SH::CallReadRltData(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->ReadRcdRtData(pLinker,&pCmd); 
}

bool Protocal104_SH::CallReadZeroOffset(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->ReadRcdOffsetData(pLinker,&pCmd); 
}

bool Protocal104_SH::CallStartRecByManual(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->StartRecordByManune(pLinker,&pCmd); 
}

bool Protocal104_SH::CallSetTime(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->SetDeviceTime(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetTime(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)

{
	return pProtocol->GetDeviceTime(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetNetCfg(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetNetCfg(pLinker,&pCmd); 
}
bool Protocal104_SH::CallSetNetCfg(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->SetNetCfg(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetDauInf(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetDAUInfo(pLinker,&pCmd); 
}

bool Protocal104_SH::CallSetDeviceRunningState(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->SetDeviceRunningState(pLinker,&pCmd); 
}

bool Protocal104_SH::CallSetDeviceTestingState(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->SetDeviceTestingState(pLinker,&pCmd); 
}

bool Protocal104_SH::CallOperateDevice(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->OperateHardWare(pLinker,&pCmd); 
}

bool Protocal104_SH::CallPushFile(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->SHPushFile(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetSHFileList(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetSHFileList(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetSHFile(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetSHFile(pLinker,&pCmd); 
}

bool Protocal104_SH::CallGetRunLog(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->GetRunLog(pLinker,&pCmd); 
}

bool Protocal104_SH::CallProcessAutoSendData(Protocal104_SH *pProtocol, Linker *pLinker, struct Command& pCmd)
{
	return pProtocol->ProcessAutoSendData(pLinker,&pCmd); 
}


bool Protocal104_SH::CallGetPQTrendFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->GetPQTrendFile(pLinker,&pCmd); 
}

bool Protocal104_SH::CallPQTest(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->PQTest(pLinker,&pCmd); 
}

bool Protocal104_SH::CallPQResumSet(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->PQResumSet(pLinker,&pCmd);
}

bool Protocal104_SH::CallPQ10CycDataExp(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->PQ10CycDataExp(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetPQ10CycDataFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->GetPQ10CycDataFile(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetPQStatFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->GetPQStatFile(pLinker,&pCmd);
}

bool Protocal104_SH::CallGetAuditFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->GetAuditFile(pLinker,&pCmd);
}

bool Protocal104_SH::CallSendAuditAlarm(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->SendAuditAlarm(pLinker,&pCmd);
}

bool Protocal104_SH::CallUserLogin(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd)
{
	return pProtocol->UserLogin(pLinker,&pCmd);
}

bool Protocal104_SH::GetPQTrendFile(Linker *pLinker, struct Command *pCmd)
{
	if(pLinker==NULL||pCmd==NULL)
	{
		return false;
	}

	if(pCmd->pParm == NULL||(pCmd->parmLen != sizeof(struct PQTrendFileInf)))
	{
		printf("GetPQTrendFile->parm  error\n");
		return false;
	}
	struct PQTrendFileInf  pqFileinf;
	memcpy(&pqFileinf, pCmd->pParm, sizeof(struct PQTrendFileInf));

	//上年月目录(配置相关不创建年月目录)
	string fileName = string(pqFileinf.fileName);
	int fileSize = pqFileinf.fileSize;
	int iYearMonth = (pqFileinf.ctime.year+2000)*100 +pqFileinf.ctime.month;
	char szYearMonth[7]={0};
	::itoa(iYearMonth,szYearMonth,10);
	char *pYearMonth =  &szYearMonth[0];
	/*char szRemoteFilaPath[256]={0};
	sprintf(szRemoteFilaPath, "%s%s/%s",RES_PQTREND_DIR, pYearMonth, pqFileinf.fileName);*/
	char info[196]={0};
	sprintf(info,  "GetFile:%s in Protocal104_SH::GetPQTrendFile", pqFileinf.fileName);
	mylog.WriteLog2File(LogControl::LINFO,info);
	//根据命令参数构建I帧并发送
	char pASDU[PROSH104_FRAME_MAXSIZE] = {0};
	int iAsduLen =0;
	PASDUID104  pAsduId = (PASDUID104)&pASDU[0];


	pAsduId->TYP = 0XCD;
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
	//	pAsduInf->INF = 0X2A;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息

	unsigned short fileTyp = pqFileinf.filetype;
	memcpy(&pASDU[iAsduLen],&fileTyp,sizeof(unsigned short));  //类型
	iAsduLen+= sizeof(unsigned short);//
	memcpy(&pASDU[iAsduLen],pqFileinf.fileName,100);  //名称
	iAsduLen += 100;//参数中的长度
	iAsduLen += sizeof(int);//开始位置０

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetPQTrendFile():Send SendIFrame error");
		return false;
	}

	bool rst = ProcessGernalFileCmdRst(pLinker,pCmd, 0XCE,fileName,fileSize,pYearMonth, FileControl::FT_PQTRH);
	return rst;
}
unsigned char Protocal104_SH::GetDevId(unsigned int boardId)
{
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(boardId);
	if(NULL == pDauInf)
	{
		printf("Not find the relative DevBoard, the BoardId = %d \n", boardId);
		return 0;
	}
	else
	{
		return (pDauInf->DAUId&0XFF);
	}
}

//处理稳态数据及电能质量录波数据
bool Protocal104_SH::ProcessContComtradeFileCmdRst(Linker *pLinker,struct  Command *pCmd, int asduNo ,const char* fileName, 
												   FileControl::FILETYPE fileControlType,const char* fileTime)
{
	//判断是否为Comtrade 格式
	if( (fileControlType != FileControl::FT_CONT)&&
		(fileControlType != FileControl::FT_CCOMTRADE)&&
		(fileControlType != FileControl::FT_PQCOMTRADE))
	{
		return false;
	}

	int fileLength =0;
	int pos =0;
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE; 
	int rst = 0;

	char szInfo[512]={0};
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

	//文件目录信息
	//构建文件目录 
	int destDevId = pCmd->destDevId&0XFF;
	FileControl fileCtrl;
	string comtradeDir;
	unsigned int devId = 0;
	PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo(pCmd->destDevId);
	if(NULL == pDauInf)
	{
		return false;
	}
	else
	{
		devId = pDauInf->DAUId;
	}
	comtradeDir = fileCtrl.GetFileTypeDirector(devId,fileControlType);

	if(comtradeDir.compare("") == 0)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "get the director of file type fail in Protocol61850::GetRecWaveFile()");
		return false;
	}

	//const string szfileName ="2011-01-12_08-50-12_pq_test";
	string szfileName = string(fileName);
	string  localFiles[8] ;
	string szTimeDir ;
	char szYM[10] = {0};

	if(FileControl::FT_PQCOMTRADE != fileControlType)
	{
		szTimeDir = ParseFileNameForGetTime(szfileName, false, false);
	}
	else
	{
		if( fileTime == NULL )
			return false;

		int iYear=0,iMon=0,iDay=0,iHour=0,iMin=0,iSec=0,iMsec=0;
		sscanf(fileTime, "%04d-%02d-%02d %02d:%02d:%02d.%03d",&iYear,&iMon,&iDay,&iHour,&iMin,&iSec,&iMsec);
		sprintf(szYM,"%04d%02d/",iYear,iMon);

		szTimeDir = string( szYM );
	}
	if(szTimeDir.size()<5)
	{
		return false;
	}
	string sFilePathWithoutExt = comtradeDir + szTimeDir + szfileName ;
	//本地文件名

	localFiles[0]  = sFilePathWithoutExt + string(".CFG");
	localFiles[1]  = sFilePathWithoutExt + string(".INF");
	localFiles[2]  = sFilePathWithoutExt + string(".DAT");
	if( FileControl::FT_PQCOMTRADE == fileControlType)
	{
		localFiles[3]  = sFilePathWithoutExt + string(".HDR");	
	}
	else
	{
		localFiles[3]  = sFilePathWithoutExt + string(".TRH");
	}
	localFiles[4]  = sFilePathWithoutExt + string(".INI");
	localFiles[5]  = sFilePathWithoutExt + string(".XML");
	localFiles[6]  = sFilePathWithoutExt + string(".SHE");
	localFiles[7]  = sFilePathWithoutExt + string(".SHC");

	string sPath = comtradeDir + szTimeDir;
	if(!fileCtrl.IsExist( sPath.c_str() )) //检查目录是否存在
	{
		if(-1 == _mkdir(sPath.c_str()))
		{
			string sInf("Create directory:");
			sInf = sInf + sPath + string(" failed in GetRecWaveFile()");
			mylog.WriteLog2File(LogControl::LERROR, sInf.c_str());
			pLinker->Close();
			return false;
		}
	}

	//保存文件
	bool bSaveSucess[8] = {false,false,false,false,false,false,false,false};
	///////////////////
	int iCurComtradefileType = 0;
	string sCurrentFileName;
	//处理进度信息
	int iCounter=0;			//帧循环次数
	int iProgress = 0;		//当前进度
	int iStepSize  = 0;     //该项纪录步长
	MemMapMgr memMgr;
	unsigned int iHasReceivedSize =0;
	bool bReceiveDat = false;
	char szSaveRemainData[5] = {0};	//保存上次未处理的数据
	int  iRemainDataLen = 0 ;	//未处理的数据长度
	//取数据
	bool error = false;
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		int iAsduLen =0;
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
		else if(0 == rst)//没数据
		{ 
			printf("Get CmdRst TimeOut  error\n");
			pLinker->Close();
			error = true;
			break;
		}

		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				printf("Unkown ASDU \n");
				mylog.WriteLog2File(LogControl::LWARNING, "Unkown ASDU for server");
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				printf("Get pAsduId->COT ==15 \n");
				mylog.WriteLog2File(LogControl::LWARNING, "Get pAsduId->COT ==15");
				break;
			}
			if(pAsduId->TYP!=asduNo)
			{
				printf("ASDU TYP error\n");
				pLinker->Close();
				error = true;
				break;
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;
			if(validatelen>0)
			{	
				if(validatelen>=2)
				{
					bHasContinueFrame = *(pDataTmp+1); //后续标示 ???wenti 
					validatelen-=2;
					pDataTmp +=2;
				}
				else
				{
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
					pLinker->Close();
					error = true;
					break;
				}

				if(validatelen<=0)
				{
					continue;
				}

				while(validatelen>0)   //处理该帧数据部分
				{
					//取类型
					int tmp_typ = 0;
					memcpy(&tmp_typ, pDataTmp,sizeof(int));

					pDataTmp+=sizeof(int);
					validatelen-=sizeof(int);
					if(tmp_typ<1||tmp_typ>8)
					{
						pLinker->Close();
						return false;
					}
					if(tmp_typ!=iCurComtradefileType) 
					{
						if(iCurComtradefileType!=0)
						{
							fileCtrl.CloseFile();
							bSaveSucess[iCurComtradefileType-1] = true; //当前文件正常获取
							sprintf(szInfo, "Receive FN:%s, filedataLen:%d",localFiles[iCurComtradefileType-1].c_str() ,iHasReceivedSize);
							mylog.WriteLog2File(LogControl::LINFO, szInfo);
						}
						iCurComtradefileType = tmp_typ;
						iHasReceivedSize =0;
						//
						sCurrentFileName = localFiles[iCurComtradefileType-1];
						bool bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_WRITE);
						if( !bOpenFlag )
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" open failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							pLinker->Close();
							return false;
						}
					}

					if(validatelen>0)
					{
						int iwrite = validatelen;
						if(iwrite != fileCtrl.WriteData(pDataTmp,iwrite))
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" write  failed in WriteComtreadeFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							fileCtrl.CloseFile();
							pLinker->Close();
							return false;
						}
						iHasReceivedSize+=validatelen;
						validatelen=0;

					}
				}
				//进度处理，如果为Comtrade文件 则对的DAT处理，如果是其他一般文件 进度条
				if((iCurComtradefileType==3)||((iCurComtradefileType==4)&&(localFiles[3].find (".TRH")>0)))
				{
					if(iCounter == 0)
					{
						//虚拟进度条 假设 DAT 文件长度为10 兆
						fileLength = 10*1024*1024;
						if(iCurComtradefileType==4)
						{
							fileLength=2.5*1024*1024;
						}
						//文件长度/每帧发送的数据长度*20  进度最多到95 后剩余的时间用于处理其他过程 最后到100,写21次进度
						iStepSize = fileLength/(iMaxValidateLen *20); 
						if(iStepSize<=0)
						{
							iStepSize=2;
						}
					}
					iCounter++;	
					if(iCounter%iStepSize ==0)
					{

						pCmd->resultNum = 1;
						PCMDRESULT pResult = new CMDRESULT();
						memset(pResult,0,sizeof(pResult));
						pResult->resultType = CDRT_PROGRESS;
						pResult->resultLen = 1;
						pResult->pData = new char[2];
						pResult->pData[1] = '\0';

						//iProgress = (iHasReceivedSize/fileLength)*100;
						iProgress += 1;
						iProgress = (iProgress>100)?100:iProgress;
						pResult->pData[0] = iProgress;
						pCmd->resultList.push_back(pResult);
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
			}//end if 
		}//end APCI4Prot104::FT_I
	}//end  取数据
	if(iCurComtradefileType>0&&iCurComtradefileType<9)
	{
		fileCtrl.CloseFile();
		bSaveSucess[iCurComtradefileType-1] = true; //当前文件正常获取
		sprintf(szInfo, "Receive FN:%s, filedataLen:%d",localFiles[iCurComtradefileType-1].c_str() ,iHasReceivedSize);
		mylog.WriteLog2File(LogControl::LINFO, szInfo);
	}

	//返回
	PCMDRESULT pResult = NULL;
	if((bSaveSucess[1]==true)&&(bSaveSucess[2]==true)&&(bSaveSucess[0]==true)||(bSaveSucess[3]==true))
	{
		//进度条到100。 
		pCmd->resultNum = 1;
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_PROGRESS;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[1] = '\0';
		pResult->pData[0] = 100;
		pCmd->resultList.push_back(pResult);
		memMgr.WriteTaskResultToMem(*pCmd);
		//清除结果
		delete pResult;
		pResult = NULL;
		pCmd->resultNum = 0;
		pCmd->resultList.clear();
		//返回写成功的文件名
		for( int j =0; j<8; j++)
		{
			if(bSaveSucess[j])
			{
				pResult = new CMDRESULT();
				pResult->resultType = CDRT_FILEPATH;
				pResult->resultLen = localFiles[j].size();
				pResult->pData = new char[pResult->resultLen+1];
				pResult->pData[pResult->resultLen] = 0 ;
				memcpy(pResult->pData,localFiles[j].c_str(),pResult->resultLen);
				pCmd->resultList.push_back(pResult);
				pCmd->resultNum +=1;
			}
		}
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 1 ; //执行成功
		pResult->pData[1] = 0 ;
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum +=1;
		//返回结果标识
	}
	else
	{
		pResult = new CMDRESULT();
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 0 ; //执行失败
		pResult->pData[1] = 0 ;
		pCmd->resultList.push_back(pResult);
		pCmd->resultNum =1;
	}

	if(error)
	{
		return false ;
	}
	return true;
}


string Protocal104_SH::ParseFileNameForGetTime(const string pFileName, bool bPQ, bool bRemote)
{
	string  szTime;
	if(pFileName.length()<16)
	{
		return szTime;
	}
	string sTimes[5];
	char *pToken[5] = {"-","-","_","-","-"};
	int  iSubDirVal = 0;
	int  iYearMonth = 0;
	char szTimeDir[8] = {0};
	int i=0;
	string sFileTime(pFileName);
	string::size_type pos1 = 0;
	string::size_type pos2 = 0;

	if(bPQ)
	{
		int copyLen=6;
		for(int k=0; k<4; k++)
		{
			szTime += sFileTime.substr(pos1,copyLen);
			szTime += string("/");
			if(!bRemote)
			{
				break;
			}
			pos1=pos1+copyLen;
			copyLen=2;
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			pos2 = sFileTime.find(pToken[i],pos1);
			if( pos2 == string::npos )
			{
				return szTime;
			}
			sTimes[i] = sFileTime.substr(pos1,pos2-pos1);
			pos1 = pos2 + 1;
			szTime += sTimes[i];
			if(i>0)
			{
				szTime += string("/");
				if(!bRemote)
				{
					break;
				}
			}
		}
	}
	return szTime;
}

int  Protocal104_SH::PQRTToSchdeclueRtData(int devId, char *szData, int srclen )
{

	const int FRM_HEAD_RT_PQLEN = sizeof(FRM_HEAD_RT_SEND);
	int data_len = srclen;
	if( data_len <= FRM_HEAD_RT_PQLEN )
	{
		mylog.FormatLogInf(LogControl::LWARNING, "The DataLen is:%d,short than FRM_HEAD_RT_PQ",data_len);
		return 0;
	}

	if(NULL==m_szDestAutoSendDataBuff)
	{
		return 0;
	}
	else
	{
		memset(m_szDestAutoSendDataBuff, 0 , AutoSendDataBuffSize);
	}

	const char* pData = szData;


	FRM_HEAD_RT_SEND headrt;	//原始实时数据头结构

	memcpy(&headrt,pData,FRM_HEAD_RT_PQLEN );
	char *pSrcData = (char *)pData;
	pSrcData += FRM_HEAD_RT_PQLEN;

	const int PERSMP_DATA_LEN = headrt.SizePerSample;

	CDeviceIniMgr *pDevIniMgr = &(SystemCfger::GetInstance().m_IniMgr);
	CDeviceIni *pDevIni = pDevIniMgr->GetDevice(devId);
	if(NULL==pDevIni)
	{
		mylog.FormatLogInf(LogControl::LWARNING, "No find device(id:%d)'s ini file in Protocol61850::AutoRltData()",devId);
		return 0;
	}
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
	//point.point = (headrt.PointNum>1000)?1000:headrt.PointNum;
	//point.samplrate = 10; //10000
	point.point = headrt.PointNum;
	//point.samplrate = headrt.SmpRate/1000.00;
	point.samplrate = headrt.SmpRate;//*50/1000.00;
	unsigned int iNewDataLen = (point.ChannelNumber + point.SwitchNumber) * 2*point.point;
	const int RTTotalLen = iNewDataLen + sizeof(POINTDATAHEAD);

	char *pDestData = m_szDestAutoSendDataBuff;
	memcpy(pDestData,&point,sizeof(point)); 
	pDestData += sizeof(point);//偏移头

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

	pDestData= NULL;
	pSrcData = NULL;
	return RTTotalLen;
}


bool Protocal104_SH::GetWaveFileExitInDB(string & szDBFileList,struct CP56Time2a & t0,struct CP56Time2a & t1)
{
	//bool bNotExist = true;
	char szSql[255] = {0};
	/*
	time_t ltime;
	struct tm *today = NULL;
	time( &ltime );
	time_t l0time = 
	struct tm *today = NULL;
	today = localtime( &ltime );
	if( today == NULL)
	return false;
	*/
	sprintf(szSql,"select * from SHRpt,SHDAU Where SHRpt.DAUID=SHDAU.DAUID and FaultTime >= '%d-%02d-%02d %02d:%02d:00' and FaultTime <= '%d-%02d-%02d %02d:%02d:59' ",
		t0.year+2000,t0.month,t0.day,t0.hours,t0.minute,t1.year+2000,t1.month,t1.day,t1.hours,t1.minute);

	szDBFileList = string("");
	PRptInfo pRpt = NULL; 
	getRptBySql(szSql,&pRpt);

	while( pRpt != NULL )
	{
		//printf("%s\n",pRpt->RptFileName );
		szDBFileList += string( pRpt->RptFileName);

		PRptInfo pTmpRptInfo = NULL;
		pTmpRptInfo = pRpt;
		pRpt = pTmpRptInfo->next;
		releaseMemery((void*)pTmpRptInfo);
		pTmpRptInfo = NULL;
	}
	return true;
}


bool Protocal104_SH::GetCmdRst(Linker *pLinker, struct  Command *pCmd,int asduNo,int inf)
{
	if(NULL==pLinker||NULL==pCmd)
	{
		return false;
	}

	char fileName[100]={0};
	char szFrameData[PROSH104_FRAME_MAXSIZE+1]={0};
	int iFrameLen = PROSH104_FRAME_MAXSIZE; 
	int rst = 0;
	bool  bFirstFrame = true;
	char bHasContinueFrame = true;

	APCI4Prot104  apci(m_iLenOfApdulen);
	PASDUID104  pAsduId =NULL;
	PASDUINFID104 pAsduInf = NULL;
	char* pAsduData =NULL;
	int iAsduLen =0;

	bool error = false;	//网络故障等
	bool isFailer = false;//执行失败，如服务端找不到数据等
	int timeOut =14000;

	//const int PQEvtLen = sizeof(struct EventInf);
	char tmpbuff[1024]={0};
	int  tembuffInUse = 0;
	bool bfirstFrame = true;
	string sz_DBFilelist;
	if(CMD_GETWAVEFILELIST==pCmd->funCode)
	{
		struct CP56Time2a  t0,t1;
		const unsigned short timeStructLen = sizeof(struct CP56Time2a);
		memcpy(&t0,pCmd->pParm+1,timeStructLen);
		memcpy(&t1,pCmd->pParm+1+timeStructLen,timeStructLen);
		this->GetWaveFileExitInDB(sz_DBFilelist, t0, t1);
	}
	//struct EventInf* pevt = NULL;


	MemMapMgr memMgr;
	//取数据
	while(bHasContinueFrame&&pLinker->IsConnect())
	{
		pAsduId = NULL;
		pAsduInf = NULL;
		char* pAsduData = NULL;
		iAsduLen =0;
		memset(&szFrameData[0],0,PROSH104_FRAME_MAXSIZE+1);
		rst = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInf,&pAsduData,&iAsduLen,timeOut); //更新 DSP 程序需较长时间目前设为等待244秒 监控板等待SVC为240秒
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
			pLinker->Close();
			error = true;
			break;
		}
		//解析数据
		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			if(pAsduId->COT == 0X40)
			{
				printf("Unkown ASDU \n");
				mylog.WriteLog2File(LogControl::LERROR, "Unkown ASDU for server");
				isFailer = true;
				break;
			}
			if(pAsduId->COT == 0X15) //命令否定
			{
				printf("Get pAsduId->COT ==15 \n");
				mylog.WriteLog2File(LogControl::LERROR, "Get pAsduId->COT ==15");
				isFailer = true;
				break;
			}
			if(pAsduId->TYP!=asduNo)
			{
				printf("ASDU TYP error\n");
				error = true;
				pLinker->Close();
				break;
			}
			if(asduNo == 0X20)
			{
				//验证信息序号
				if(inf != pAsduInf->INF)
				{
					printf("ASDU INF error\n");
					error = true;
					pLinker->Close();
					break;
				}
			}

			int validatelen = iAsduLen;
			char *pDataTmp =  pAsduData;
			if(validatelen>0)
			{	
				if(validatelen>=2)
				{
					bHasContinueFrame = *(pDataTmp+1); //后续标示
					validatelen-=2;
					pDataTmp +=2;
				}
				else
				{
					error = true;
					pLinker->Close();
					break;
				}
				if(validatelen>0)
				{
					if(false ==ParserOneFrameRstData(pDataTmp, validatelen, tmpbuff, tembuffInUse,pCmd, memMgr, sz_DBFilelist))
					{
						error = true;
						pLinker->Close();
						break;
					}
				}
			}//end if 
		}//end APCI4Prot104::FT_I
	}//end  取数据

	PCMDRESULT pResult = new CMDRESULT();
	memset(pResult,0,sizeof(pResult));
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];

	if(error || isFailer)
	{
		pResult->pData[0] = 0 ; //执行失败
	}
	else
	{
		pResult->pData[0] = 1 ; //执行成功
	}

	pResult->pData[1] = 0 ;
	pCmd->resultList.push_back(pResult);
	pCmd->resultNum = 1;

	if(error)
		return false;

	return true;
}


bool Protocal104_SH::ParserOneFrameRstData(const char* pCommData, int validatelen, char * szLeftbuff, int& szLeftBuffLen,struct  Command *pCmd,MemMapMgr& memMgr,string& sz_DBFilelist)
{
	if(pCommData== NULL||szLeftbuff==NULL||pCmd==NULL)
	{
		return false;
	}
	unsigned char devId = GetDevId(pCmd->destDevId);
	unsigned int StructInfoLen = 0;
	unsigned char funcode = pCmd->funCode;
	CMDRESULTTYPE  _rstType = CDRT_FILELIST;
	if( CMD_GETPQEVENT==funcode )
	{
		StructInfoLen = sizeof(struct EventInf);
		_rstType = CDRT_PQ_EVENT;
	}
	else if( CMD_GETWAVEFILELIST==funcode )
	{
		StructInfoLen = sizeof(struct CommCmdWaveFileInf ) ;
		_rstType = CDRT_FILELIST;
	}
	else if( CMD_GETFILELIST==funcode )
	{
		StructInfoLen = sizeof(CMDDIRFILEINF);
		_rstType = CDRT_DIRFILEINF;
	}

	if(validatelen>0&&StructInfoLen>0)
	{
		int totalLen = szLeftBuffLen+validatelen;
		if(totalLen<StructInfoLen)
		{
			memcpy(szLeftbuff+szLeftBuffLen, pCommData, validatelen);
			szLeftBuffLen +=validatelen;
		}
		else
		{
			const int structCnt = totalLen/StructInfoLen;
			PCMDRESULT pResult = new CMDRESULT();
			memset(pResult,0,sizeof(pResult));
			pResult->resultType = _rstType;

			if(CMD_GETWAVEFILELIST==funcode)  
			{ //由于上传类型和原后台结构类型不一致固需转化
				struct CommCmdWaveFileInf   commWavefileInf;
				struct tagCmdFileList       cmdWavefileInf;
				const char * ptmpdata=  pCommData;
				unsigned int ValideCnt = 0;
				time_t  faultTime = 0;
				const long  _WaveFileInfoLen = sizeof(cmdWavefileInf)*structCnt;
				pResult->pData = new char[_WaveFileInfoLen];
				memset(pResult->pData,0,_WaveFileInfoLen);
				for(unsigned int i=0; i<structCnt; i++)
				{
					if(szLeftBuffLen>0&&szLeftBuffLen<=StructInfoLen)
					{
						memcpy(&commWavefileInf, szLeftbuff, szLeftBuffLen);
						memcpy(&commWavefileInf, ptmpdata, StructInfoLen-szLeftBuffLen);
						ptmpdata += StructInfoLen-szLeftBuffLen;
						szLeftBuffLen =0 ;
					}
					else
					{
						memcpy(&commWavefileInf, ptmpdata, StructInfoLen);
						ptmpdata += StructInfoLen;
					}
					memset(&cmdWavefileInf, 0, sizeof(cmdWavefileInf));


					//如果已经存在，则跳过该文件信息
					if(sz_DBFilelist.length()>0)
					{
						if(strstr(sz_DBFilelist.c_str(),commWavefileInf.fileName))
							continue;
					}

					//格式转化
					cmdWavefileInf.devID = devId;
					memcpy(cmdWavefileInf.fileName, commWavefileInf.fileName, 40);
					cmdWavefileInf.fileName[39]='0';
					faultTime = commWavefileInf.faultTime;
					struct tm * ptm= localtime(&faultTime);
					cmdWavefileInf.faultTime.IV = false;
					cmdWavefileInf.faultTime.day = ptm->tm_mday;
					cmdWavefileInf.faultTime.hours = ptm->tm_hour;
					cmdWavefileInf.faultTime.milliseconds =ptm->tm_sec *1000;
					cmdWavefileInf.faultTime.minute = ptm->tm_min;
					cmdWavefileInf.faultTime.week = (ptm->tm_wday>0)? ptm->tm_wday:7;
					cmdWavefileInf.faultTime.month = ptm->tm_mon+1;
					cmdWavefileInf.faultTime.year = ptm->tm_year+1900-2000;

					//if(BWaveFileNotExitInDB(commWavefileInf.fileName))
					//{
					memcpy(pResult->pData+(ValideCnt*sizeof(CMDFILELIST)), &cmdWavefileInf,sizeof(CMDFILELIST));
					ValideCnt++;
					//}
				}

				if(ValideCnt>0)
				{
					pResult->resultLen = ValideCnt*sizeof(CMDFILELIST);
					pCmd->resultList.push_back(pResult);
					pCmd->resultNum = 1;
				}
				else
				{
					if(pResult->pData)
					{
						delete []pResult->pData;
						pResult->pData = NULL;
					}
					pCmd->resultNum = 0;
				}
				pCommData += StructInfoLen*structCnt-szLeftBuffLen;
				validatelen -= (StructInfoLen*structCnt-szLeftBuffLen);
				szLeftBuffLen = 0;
			}
			else
			{
				pResult->resultLen = StructInfoLen*structCnt;
				pResult->pData = new char[pResult->resultLen];
				memcpy(pResult->pData, szLeftbuff, szLeftBuffLen);
				memcpy(pResult->pData+szLeftBuffLen, pCommData, StructInfoLen*structCnt-szLeftBuffLen);
				pCommData += StructInfoLen*structCnt-szLeftBuffLen;
				validatelen -= (StructInfoLen*structCnt-szLeftBuffLen);
				szLeftBuffLen = 0;
				pCmd->resultList.push_back(pResult);
				pCmd->resultNum = 1;
#if 0
				if(NULL==g_EvtDebugf)
				{
					g_EvtDebugf = fopen("PQEvtDebug.dat", "wb");
				}
				if(g_EvtDebugf)
				{
					if(pResult->resultLen>0)
					{

						fwrite(pResult->pData, 1, pResult->resultLen, g_EvtDebugf);
						fflush(g_EvtDebugf);
					}
				}
#endif
			}
			//	pevt = (struct EventInf*)pResult->pData;
			memMgr.WriteTaskResultToMem(*pCmd);
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
			if(validatelen>0)
			{
				if(validatelen<1024)
				{
					szLeftBuffLen = validatelen;
					memcpy(szLeftbuff, pCommData, validatelen);	
				}
				else
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool Protocal104_SH::GetNetCfg(Linker* pLinker, struct Command* pCmd)
{
	//1 字节已校样
	if(pLinker==NULL||pCmd==NULL)
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
	pAsduInf->INF = 0X2B;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度 

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::GetNetCfg():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XAC, szRstData );
	//返回命令结果到UI
	if(rst)
	{
		if(szRstData.length() == sizeof(SYS_NetCfg))
		{
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_IP;
			pResult->resultLen = sizeof(SYS_NetCfg);
			pResult->pData = new char[sizeof(SYS_NetCfg)+1];
			memset(pResult->pData, 0, sizeof(SYS_NetCfg)+1);
			memcpy(pResult->pData,szRstData.c_str() ,sizeof(SYS_NetCfg));
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

bool Protocal104_SH::SetNetCfg(Linker* pLinker, struct Command* pCmd)
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
	pAsduInf->INF = 0X2C;
	iAsduLen += ASDUINFID104_LEN;
	iAsduLen +=1; //返回标示符信息
	memcpy(&pASDU[iAsduLen],pCmd->pParm,pCmd->parmLen);
	iAsduLen+= pCmd->parmLen;//参数中的长度 

	int iResult = SendIFrame(pLinker,pASDU,iAsduLen,pCmd);
	if( -1 == iResult || 0 == iResult)
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_SH::SetNetCfg():Send SendIFrame error");
		return false;
	}

	//取命令返回结果
	string  szRstData;
	bool rst = GetCmdRst(pLinker, 0X20, 0XAD, szRstData );

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
