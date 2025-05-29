// {{{RME classifier 'Logical View::Protocol60_103'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol60_103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol60_103.h>
#include <FileControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
*All rights reserved
*
* 文件名称：Protocol60_103.cpp
* 文件标识：
* 摘    要：解析60_103协议
*
* 当前版本：1.0
* 作    者：libo
* 完成日期:2007年7月4日
***********************************************************************/
#include <string>
#include <memory>
#include <time.h>

#include "Linker.h"
#include "Task.h"
#include "MemMapMgr.h"

#include "StringRes.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

#include "Protocol103_struct.h"
using namespace std;

DWORD WINAPI DelUdpDataThread(void *pParm);//处理udp数据的线程

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
Protocol60_103::Protocol60_103( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol60_103::~Protocol60_103( void )
{
}
// }}}RME

// {{{RME operation 'GetDauInf(Linker *,struct Command*)'
bool Protocol60_103::GetDauInf( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetDauInf()");

	if( pCmd->from == CD_REMOTE ) //来自远程
	{
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //来自于UI、定时器等
	{
		//分析命令参数
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetDauInf()");
			return false;
		}

		unsigned char type = pCmd->pParm[0];	//类型

		//asdu10带执行的写的报文
		unsigned char asdu10_attr[1]={0};	//属性结构
		asdu10_attr[0] = type;	//类型

		GI asdu10_gi[1] ={GI(0X1201,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21（控制方向）―― 读返回的自检信息
		GI asdu21_gi[1] ={GI(0X1202,0X67,23,0,1,0,NULL)};

		char pIniData[PRO103_ASDUDATA_MAXSIZE+1] ={0};//
		char *ppData[1] ={ &pIniData[0]};
		int  iniDataLen[1] = {PRO103_ASDUDATA_MAXSIZE};
		int	*pIniDataLen[1] = {&iniDataLen[0]};
		int iAttrNum = 1;

		PCMDRESULT pCmdResult = new CMDRESULT;

		//获取通用分类服务返回的属性结构数据
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppData,pIniDataLen,&iAttrNum);
		if(iReturn != 1)
		{
			//返回失败结果
			pCmdResult->resultType = CDRT_TRUFALSE;
			pCmdResult->resultLen = 1;
			pCmdResult->pData = new char[2];
			pCmdResult->pData[0] = 0;	//false
			pCmdResult->pData[1] = '\0';

			pCmd->resultNum = 1;
			pCmd->resultList.push_back(pCmdResult);

			if(iReturn == -1)
			{
				mylog.WriteLog2File(LogControl::LERROR,"GetGIAttrData return error in Protocol60_103::GetDauInf()");
				pLinker->Close();
				return false;
			}

			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fault in Protocol60_103::GetDauInf()");
			return false;
		}

		//将自检信息放入结果中
		pCmdResult->resultType = CDRT_CHAR;
		pCmdResult->resultLen = iniDataLen[0];
		pCmdResult->pData = new char[pCmdResult->resultLen];
		memcpy(pCmdResult->pData,pIniData,pCmdResult->resultLen);

		pCmd->resultNum = 1;
		pCmd->resultList.push_back(pCmdResult);

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetDauInf()");
		return true;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'PreGetFile(Linker *,struct Command*,int,char*,int,unsigned int,unsigned char)'
bool Protocol60_103::PreGetFile( Linker * pLinker, struct Command * pCmd, int type, char * pFileName, int fileNameLen, unsigned int startPos, unsigned char recWaveType )
{
	// {{{USR
	//获取文件前的处理工作
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::PreGetFile()");

	int msgLen = 0; //消息长度
	int sendDataLen = 0;	//发送的数据长度

	FIXFRAME fixFrame;	//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	static bool preFCB = false;

	char ASDUData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//ASDU数据
	int  ASDUDataLen = 0;

	if( type == 1 || type == 3) //录波文件
	{
		ASDUData[0] = recWaveType;			//录波类型
		memcpy(&ASDUData[1],pFileName,fileNameLen);	//录波文件名（不含扩展名）
		memcpy(&ASDUData[41],&startPos,4);	//起始传输位置
		ASDUDataLen = 45;
	}
	else //一般文件
	{
		memcpy(&ASDUData[0],pFileName,fileNameLen);	//文件名
		memcpy(&ASDUData[100],&startPos,4);	//起始传输位置
		ASDUDataLen = 104;
	}

	preFCB = !preFCB;
	auto_ptr<string> pDataPack;
	if( type == 1) //录波文件
	{
		//发送ASDU13命令
		pDataPack = auto_ptr<string>(CreateMsg(C_EX13_NA_3, pCmd->destDevId,preFCB,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
	}
	else //一般文件
	{
		//发送ASDU103命令
		pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,preFCB,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
	}

	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据
	if(sendDataLen != msgLen)
	{
		mylog.WriteLog2File(LogControl::LERROR, "send data error in Protocol60_103::PreGetFile()");
		return false;
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::PreGetFile()");
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
bool Protocol60_103::GetRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//召唤录波文件
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetRecWaveFile()");

	//分析命令参数
	if(pCmd->parmLen != 45 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetRecWaveFile()");
		return false;
	}

	char szLogInf[312] = {0};
	unsigned char recWaveType = pCmd->pParm[0];	//录波类型
	char pFileName[41] = {0};//不含扩展名的文件名
	memcpy(pFileName,&pCmd->pParm[1],40);
	int  fileNameLen = strlen(pFileName);
	unsigned int   startPos = 0;//起始传输位置
	memcpy(&startPos,&pCmd->pParm[41],4);

	string	sDownInfFile;//保存下载信息的临时文件（保存断点续传信息）

	struct CP56Time2a faultTime;//故障时间
	memset(&faultTime,0,sizeof(faultTime));

	SYSTEMTIME fileSysTime;//新建文件的系统时间
	memset(&fileSysTime,0,sizeof(fileSysTime));

	FILETIME fileTime;//新建文件的文件结构时间
	memset(&fileTime,0,sizeof(fileTime));

	FileControl fileCtrl;

	//保存中断环境
	struct tagEnVirionment
	{
		unsigned int startPos;
		int iCurrentComtradeFileType;
		int iCurrentFileLen;
		int iCurrentFileHasWriteLen;
		char sFilePathWithoutExt[512];
		bool bNeedUnCompress;	//dat文件需要解压
	};
	struct tagEnVirionment *pEV = NULL;
	bool	bHasError = false;	//有错误

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

	string comtradeDir = fileCtrl.GetFileTypeDirector(devId,FileControl::FT_COMTRADE);
	if(comtradeDir.compare("") == 0)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "get the director of file type fail in Protocol60_103::GetRecWaveFile()");
		pLinker->Close();
		return false;
	}

	sDownInfFile = comtradeDir;
	sDownInfFile.append(pFileName);
	sDownInfFile.append(".dwn");

	auto_ptr<string> pDataPack ;//要发送的数据包

	FIXFRAME fixFrame;//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	if( pCmd->from == CD_REMOTE )
	{
		//命令来自于远程，则将录波文件发送到远程
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else
	{
		//命令来自于UI、定时器等
		MemMapMgr memMgr;
		int iCurrentComtradeFileType = 0 ;	//当前录波文件类型，0表示未知
		int iCurrentFileLen = -1;	//当前文件长度
		int iCurrentFileHasWriteLen = 0;	//当前文件已被写的长度
		string sFilePathWithoutExt("");	//不带扩展名的录波文件路径
		string sCurrentFileName("");	//当前处理的文件名
		char szSaveRemainData[4] = {0};	//保存上次未处理的数据
		int  iRemainDataLen = 0 ;	//未处理的数据长度
		bool bCfgHasDown = false;	//cfg文件已经下载
		bool bDatHasDown = false;	//dat文件已经下载
		bool bNeedUnCompress = false;	//dat文件需要解压

		bool bNeedAutoSendReportFile = true;//是否要自动上传故障报告
		int iMaxFileLen = 0;				//最大的文件长度

		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Resume the command:GetRecWaveFile");
			pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
			startPos = pEV->startPos;
			iCurrentComtradeFileType = pEV->iCurrentComtradeFileType;
			iCurrentFileLen = pEV->iCurrentFileLen;
			iCurrentFileHasWriteLen = pEV->iCurrentFileHasWriteLen;
			sFilePathWithoutExt = string(pEV->sFilePathWithoutExt);
		}
		else
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "First run the command:GetRecWaveFile");
			pEV = new struct tagEnVirionment;
			memset(pEV,0,sizeof(struct tagEnVirionment));

			//检查保存下载信息的临时文件是否存在
			if( fileCtrl.IsExist( sDownInfFile.c_str() ) )
			{
				//读取下载信息
				fileCtrl.OpenFile(sDownInfFile.c_str(),FileControl::OM_READ);
				fileCtrl.ReadData((char *)pEV,sizeof(struct tagEnVirionment));
				fileCtrl.CloseFile();

				startPos = pEV->startPos ;
				iCurrentComtradeFileType = pEV->iCurrentComtradeFileType;
				iCurrentFileLen = pEV->iCurrentFileLen;
				iCurrentFileHasWriteLen = pEV->iCurrentFileHasWriteLen;
				sFilePathWithoutExt = string(pEV->sFilePathWithoutExt);
				bNeedUnCompress = pEV->bNeedUnCompress;
			}
			else
			{
				pEV->startPos = startPos;
				pEV->iCurrentComtradeFileType = iCurrentComtradeFileType;
				pEV->iCurrentFileLen = iCurrentFileLen;
				pEV->iCurrentFileHasWriteLen = iCurrentFileHasWriteLen;
				pEV->bNeedUnCompress = bNeedUnCompress;
			}

			pCmd->_state.pStoreParm =  (void*)pEV;
		}

		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run the first step of GetRecWaveFile:pregetfile");

			//准备获取文件前的处理
			if( !PreGetFile(pLinker,pCmd,1,pFileName, fileNameLen, startPos, recWaveType) )
			{
				mylog.WriteLog2File(LogControl::LERROR, "Return error from PreGetFile in GetRecWaveFile()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run the step of GetRecWaveFile:wait for M_CON_NA_3");
			//期待采集装置发送M_CON_NA_3(确认帧)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LERROR, "Wait M_CON_NA_3 error in GetRecWaveFile()");
				return false;
			}
			pCmd->_state.ToNextState();
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4; //每次可读取的最大数据长度
		int count = 0;	//循环计数

		while( !bHasError )
		{
			count ++;

			if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			{
				mylog.WriteLog2File(LogControl::LDETAIL, "Run the step 3 of GetRecWaveFile:send question for level 1 data");
				//请求1级用户数据(如果数据没有发送完，则从请求1级用户数据开始循环)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR, "Send question for level 1 data error in GetRecWaveFile()");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}
				pCmd->_state.ToNextState();
			}

			//期待采集装置发送ASDU14命令
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
			PVFRAMEHEAD pVFrameHead = NULL;	//帧头
			PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
			PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

			char *pAsduData=NULL;
			int  AsduDataLen=0;

			int framePacketLen=0;	//帧实际的长度

			if(pCmd->_state._state == CmdState::CS_STEP4) //第4步
			{
				mylog.WriteLog2File(LogControl::LDETAIL, "Run the step 4 of GetRecWaveFile:wait for ASDU14");
				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LERROR, "No get ASDU14 frame in GetRecWaveFile().");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG, "GetRecWaveFile is interrupted.");

					//保存中断环境
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置
					pEV->iCurrentComtradeFileType = iCurrentComtradeFileType;
					pEV->iCurrentFileLen = iCurrentFileLen;
					pEV->iCurrentFileHasWriteLen = iCurrentFileHasWriteLen;
					bHasError = true;
					break;
					//return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_14)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU14,but it isn't in GetRecWaveFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU14,but it isn't");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}

				memcpy(&startPos,&pAsduData[0],4);

				unsigned int fileStartPos = 0;	//写文件的起始位置
				unsigned int  writeLen = 0;	//要写入文件的数据长度

				while(AsduDataLen > 4  && !bHasError)//AsduDataLen包含4字节的起始位置和打包文件长度
				{
					//首次处理返回的数据
					if(iCurrentComtradeFileType == 0)
					{
						if(startPos != 0 || AsduDataLen < (4 + PRO103_ASDU_FILENAMELEN +7 +1 +4) )
						{
							//接收到的数据不符要求
							mylog.WriteLog2File(LogControl::LERROR,"comtrade file data is invalid in GetRecWaveFile()");
							//mylog.WriteLog(LogControl::LERROR,pCmd,"comtrade file data is invalid");
							m_errCode = PEC_ERR_ANSWER;
							pLinker->Close();
							bHasError = true;
							break;
							//return false;
						}

						//获得故障时间
						memcpy(&faultTime,&pAsduData[4 +PRO103_ASDU_FILENAMELEN],sizeof(faultTime));

						//设定文件时间
						fileSysTime.wYear = 2000 + faultTime.year;
						fileSysTime.wMonth = faultTime.month;
						fileSysTime.wDay = faultTime.day;
						fileSysTime.wHour = faultTime.hours;
						fileSysTime.wMinute = faultTime.minute;
						fileSysTime.wSecond = faultTime.milliseconds/1000;

						SystemTimeToFileTime(&fileSysTime,&fileTime);

						//查找或创建文件存放目录
						int year = 2000 + faultTime.year;
						int month = faultTime.month;
						int yearMonth = year*100 + month;
						char szYearMonth[7] = {0};
						comtradeDir.append(::itoa(yearMonth,szYearMonth,10));
						if(!fileCtrl.IsExist(comtradeDir.c_str())) //检查目录是否存在
						{
							if(-1 == _mkdir(comtradeDir.c_str()))
							{
								string sInf("Create directory:");
								sInf = sInf + comtradeDir + string(" failed in GetRecWaveFile()");
								mylog.WriteLog2File(LogControl::LERROR, sInf.c_str());

								m_errCode = PEC_UNKOWN;
								pLinker->Close();
								bHasError = true;
								break;
								//return false;
							}
						}

						//指定文件路径
						comtradeDir.append("/");
						sFilePathWithoutExt = comtradeDir + string(pFileName);

						memcpy(pEV->sFilePathWithoutExt,
							sFilePathWithoutExt.c_str(),
							sFilePathWithoutExt.size());
						sFilePathWithoutExt = string(pEV->sFilePathWithoutExt);

						//文件类型
						iCurrentComtradeFileType = (int)pAsduData[4 +PRO103_ASDU_FILENAMELEN +7];
						//文件长度
						memcpy(&iCurrentFileLen,&pAsduData[4 +PRO103_ASDU_FILENAMELEN +7 +1],4);

						startPos = PRO103_ASDU_FILENAMELEN +7 +1 +4;
						pAsduData += PRO103_ASDU_FILENAMELEN +7 +1 +4;

						AsduDataLen = AsduDataLen - (PRO103_ASDU_FILENAMELEN +7 +1 +4);
						if( AsduDataLen <= 4 )//数据处理完
						{
							mylog.WriteLog2File(LogControl::LWARNING,"The first data-pack only include the RecWaveFile head in GetRecWaveFile()");
							break;
						}

						//文件已经存在，则不再自动上传故障简报
						string sTmpFile = sFilePathWithoutExt + string(".cfg");
						if(fileCtrl.IsExist( sTmpFile.c_str() ))
						{
							bNeedAutoSendReportFile = false;
						}

					}//end if(iCurrentComtradeFileType == 0)

					//文件类型未知
					if(iCurrentComtradeFileType == -1)
					{
						if( iRemainDataLen > 0)
						{
							iCurrentComtradeFileType = (int)szSaveRemainData[0];
							iRemainDataLen -= 1;
							::memmove(&szSaveRemainData[0],&szSaveRemainData[1],iRemainDataLen+1);
						}
						else
						{
							iCurrentComtradeFileType = (int)pAsduData[4];
							AsduDataLen--;
							startPos += 1;
							pAsduData++;
						}
					}

					if(sCurrentFileName == string(""))
					{
						//获得当前处理文件名称
						switch(iCurrentComtradeFileType)
						{
						case 1: //hdr文件
							sCurrentFileName = sFilePathWithoutExt + string(".HDR");
							break;
						case 2: //cfg文件
							sCurrentFileName = sFilePathWithoutExt + string(".cfg");
							break;
						case 3: //dat文件
							sCurrentFileName = sFilePathWithoutExt + string(".DAT");
							break;
						case 4: //inf文件
							sCurrentFileName = sFilePathWithoutExt + string(".INF");
							break;
						case 5: //she文件
							sCurrentFileName = sFilePathWithoutExt + string(".SHE");
							break;
						case 6: //shc文件(定值文件)
							sCurrentFileName = sFilePathWithoutExt + string(".SHC");
							break;
						case 7: //dat压缩文件
							sCurrentFileName = sFilePathWithoutExt + string(".SHZip");
							bNeedUnCompress = true;
							pEV->bNeedUnCompress = bNeedUnCompress;
							break;
						default:
							//接收到的数据不符要求
							mylog.WriteLog2File(LogControl::LERROR,"comtrade file data is invalid in GetRecWaveFile()");
							//mylog.WriteLog(LogControl::LERROR,pCmd,"comtrade file data is invalid");
							m_errCode = PEC_ERR_ANSWER;
							pLinker->Close();
							bHasError = true;
							break;
							//return false;
						}//end switch

						sprintf(szLogInf,"Begin write file : %s in Protocol60_103::GetRecWaveFile()",sCurrentFileName.c_str());
						mylog.WriteLog2File(LogControl::LDEBUG,szLogInf);

						//open file
						bool bOpenFlag = true;
						if(iCurrentFileHasWriteLen > 0 )
						{
							bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_APPEND);
						}
						else
						{
							//修改文件时间
							HANDLE hFile = CreateFile(sCurrentFileName.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,
								CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
							if(hFile == INVALID_HANDLE_VALUE)
							{
								m_errCode = PEC_UNKOWN;
								string info = string(sCurrentFileName.c_str()) + string(" open failed in GetRecWaveFile()");
								mylog.WriteLog2File(LogControl::LERROR,info.c_str());
								pLinker->Close();
								bHasError = true;
								break;
								//return false;
							}

							SetFileTime(hFile,&fileTime,&fileTime,&fileTime);
							::CloseHandle(hFile);
							hFile = NULL;

							bOpenFlag = fileCtrl.OpenFile(sCurrentFileName.c_str(),FileControl::OM_WRITE);
						}
						if( !bOpenFlag )
						{
							m_errCode = PEC_UNKOWN;
							string info = string(sCurrentFileName.c_str()) + string(" open failed in GetRecWaveFile()");
							mylog.WriteLog2File(LogControl::LERROR,info.c_str());
							//mylog.WriteLog(LogControl::LERROR,pCmd,info.c_str());
							pLinker->Close();
							bHasError = true;
							break;
							//return false;
						}

					}//end if(sCurrentFileName == string(""))

					//当前文件长度还未知
					if(iCurrentFileLen == -1)
					{
						char szLenData[4] = {0};

						//先拷贝上次剩余的数据
						memcpy(&szLenData[0],&szSaveRemainData[0],iRemainDataLen);

						//再拷贝本次接收到的数据
						int iNeedCpy = 4 - iRemainDataLen;
						memcpy(&szLenData[iRemainDataLen],&pAsduData[4],iNeedCpy);
						AsduDataLen = AsduDataLen - iNeedCpy;
						pAsduData += iNeedCpy;
						startPos += 4;
						iRemainDataLen = 0;

						//文件长度
						memcpy(&iCurrentFileLen,&szLenData[0],4);
						if(iMaxFileLen < iCurrentFileLen)
						{
							iMaxFileLen = iCurrentFileLen;
						}
					}

					//将数据写入文件
					fileCtrl.Seek(iCurrentFileHasWriteLen);
					int iRemain =iCurrentFileLen - iCurrentFileHasWriteLen;
					writeLen = (iRemain - (AsduDataLen-4))>0?(AsduDataLen-4):iRemain;

					fileCtrl.WriteData(&pAsduData[4],writeLen);
					pAsduData += writeLen ;
					startPos += writeLen;
					iCurrentFileHasWriteLen += writeLen;
					AsduDataLen = AsduDataLen - writeLen;

					//当前处理的文件已经处理完毕，处理下个文件
					if(AsduDataLen > 4 || iCurrentFileHasWriteLen == iCurrentFileLen)
					{
						sprintf(szLogInf,
							"The current file's downloading completes,remaind asdu-data-len:%d,current file len:%d,has write len:%d in Protocol60_103::GetRecWaveFile()",
							AsduDataLen,iCurrentFileLen,iCurrentFileHasWriteLen);
						mylog.WriteLog2File(LogControl::LDEBUG,szLogInf);

						if(iCurrentComtradeFileType == 2)
						{
							bCfgHasDown = true;//cfg文件已经下载
						}
						else if(iCurrentComtradeFileType == 3 )
						{
							bDatHasDown = true;//dat文件已经下载
						}
						else if(iCurrentComtradeFileType == 7)
						{
							bDatHasDown = true;//dat文件已经下载
							bNeedUnCompress = true;//需要解压
						}

						//重新初始化
						fileCtrl.CloseFile();
						sCurrentFileName = string("");
						iCurrentFileHasWriteLen = 0;
						iCurrentFileLen = -1 ;

						int iRemain = AsduDataLen -4;

						//文件类型
						if(iRemain > 0)
						{
							iCurrentComtradeFileType = (int)pAsduData[4];
							iRemain = iRemain -1;
							AsduDataLen--;
							pAsduData++;
							startPos += 1;
						}
						else
						{
							iCurrentComtradeFileType = -1;
						}

						//文件长度
						if(iRemain < 4)//剩余的数据不足以表示文件长度
						{
							iRemainDataLen = iRemain;
							memcpy(&szSaveRemainData[0],&pAsduData[4],iRemain);
							AsduDataLen = AsduDataLen - iRemain;
							pAsduData += iRemain;
						}
						else
						{
							iRemainDataLen = 0;
							memcpy(&iCurrentFileLen,&pAsduData[4],4);
							AsduDataLen = AsduDataLen - 4;
							pAsduData += 4;
							startPos += 4;
						}

						continue;
					}

				}//end while(AsduDataLen > 4)

				int maxCount = iCurrentFileLen/readMaxLen; //最大循环次数
				int iInterval = maxCount/20;    //间隔多少次循环之后写一次进度信息(全过程最多写20次进度信息)
				iInterval = (iInterval <= 0)?30:iInterval;

				//写进度信息(仅显示dat文件的信息)
				if(count%iInterval == 0 && iCurrentComtradeFileType == 3)
				{
					pCmd->resultNum = 1;
					PCMDRESULT pResult = new CMDRESULT;
					pResult->resultType = CDRT_PROGRESS;
					pResult->resultLen = 1;
					pResult->pData = new char[2];
					pResult->pData[1] = '\0';

					int iProgress = 100 * (startPos+writeLen)/iCurrentFileLen;
					iProgress = (iProgress>100)?100:iProgress;
					pResult->pData[0] = iProgress;
					pCmd->resultList.push_back(pResult);
					memMgr.WriteTaskResultToMem(*pCmd);

					//清除结果
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();
				}

				if(pVFrameHead->control.m.acd == false)
				{
					//如果文件长度大于规定的长度则不自动故障分析
					//if(SystemCfger::GetInstance().maxLenOfAutoAnaWaveFile *1024*1024 < iMaxFileLen)
					//{
					//	bNeedAutoSendReportFile = false;
					//}
					bNeedAutoSendReportFile = false;//不对外通讯，不自动上传故障简报

					//需要自动上传故障简报
					if(bNeedAutoSendReportFile)
					{
						//构建任务
						Task *pTask = new Task();
						pTask->Linker = NULL;
						pTask->cmd.destDevId = pCmd->destDevId;
						pTask->cmd.from = CD_NEURON;
						pTask->cmd.to = CD_REMOTE;
						pTask->cmd.funCode = CMD_CREATEREPORTFILE;

						pTask->cmd.parmLen = 512;
						pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
						memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);
						memcpy(pTask->cmd.pParm,sFilePathWithoutExt.c_str(),sFilePathWithoutExt.size());

						//将任务发送到远程任务队列中
						if(!(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pTask))
						{
							delete pTask;
						}
					}

					//清除下载信息文件
					string sCmd = string("del ");
					sCmd.append(sDownInfFile);
					while(1)
					{
						string::size_type pos = sCmd.find("/");
						if(pos == string::npos)
							break;
						sCmd = sCmd.replace(pos,1,"\\");
					}
					::system(sCmd.c_str());

					//文件传输完毕
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					if(!bCfgHasDown || !bDatHasDown)
					{
						//cfg文件或dat文件没有下载，则返回错误
						mylog.WriteLog2File(LogControl::LWARNING,"Download complete,but the cfg or the dat file don't downloaded,leave Protocol60_103::GetRecWaveFile()");
						return false;
					}
					else
					{
						if( bNeedUnCompress )
						{
							//解压文件
							string sDatFileName = sFilePathWithoutExt + string(".DAT");
							string sZipFileName = sFilePathWithoutExt + string(".SHZip");

							sprintf(szLogInf,
								"Begin uncompress the file :%s to the file :%s in Protocol60_103::GetRecWaveFile()",
								sZipFileName.c_str(),sDatFileName.c_str());
							mylog.WriteLog2File(LogControl::LDEBUG,szLogInf);

							if(!fileCtrl.UnCompressFile(sZipFileName.c_str(),sDatFileName.c_str()))
							{
								sprintf(szLogInf,
									"Uncompress the file :%s to the file :%s fail in Protocol60_103::GetRecWaveFile()",
									sZipFileName.c_str(),sDatFileName.c_str());
								mylog.WriteLog2File(LogControl::LERROR,szLogInf);

								m_errCode = PEC_OPERAT_FILE_FAIL;
								return false;
							}

							DeleteFile(sZipFileName.c_str());
							mylog.WriteLog2File(LogControl::LDEBUG,"Uncompress the file success");
						}
						mylog.WriteLog2File(LogControl::LINFO, "Download complete success,leave Protocol60_103::GetRecWaveFile()");
						return true;
					}
				}//end if(pVFrameHead->control.m.acd == false)
				else
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP3);
					continue;
				}

			}//end if(pCmd->_state._state == CmdState::CS_STEP4) //第4步

		}//end while(1)

		//保存下载信息
		pEV->startPos = startPos;
		pEV->iCurrentComtradeFileType = iCurrentComtradeFileType;
		pEV->iCurrentFileLen = iCurrentFileLen;
		pEV->iCurrentFileHasWriteLen = iCurrentFileHasWriteLen;

		fileCtrl.OpenFile(sDownInfFile.c_str(),FileControl::OM_WRITE);
		fileCtrl.WriteData((char *)pEV,sizeof(struct tagEnVirionment));
		fileCtrl.CloseFile();

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetRecWaveFile() with error");
		return false;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
bool Protocol60_103::GetRecWaveFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetRecWaveFileList()");

	//分析命令参数
	if(pCmd->parmLen != 1+sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetRecWaveFileList()");
		return false;
	}

	struct CP56Time2a startTime;	//查询开始时间
	struct CP56Time2a endTime;	//查询结束时间
	memcpy(&startTime,&pCmd->pParm[1],sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[1+sizeof(startTime)],sizeof(endTime));

	FileControl fileCtrl;
	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	FIXFRAME fixFrame;//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //命令来自于远程，则将录波文件发送到远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//发送的数据

		memcpy(&sendData[0],pCmd->pParm,1);	//录波类型
		memcpy(&sendData[1],&startTime,sizeof(startTime));//查询起始时间
		memcpy(&sendData[1+sizeof(startTime)],&endTime,sizeof(endTime));//查询终止时间

		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetRecWaveFileList:send ASDU15.");

			//发送ASDU15命令
			auto_ptr<string> pDataPack = auto_ptr<string>(CreateMsg(C_EX15_NA_3, pCmd->destDevId,false,0,CINF_0,&sendData[0],2*sizeof(struct CP56Time2a)+1,&msgLen));
			sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LERROR, "send ASDU15 error in Protocol60_103::GetRecWaveFileList()");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetRecWaveFileList:wait for M_CON_NA_3.");

			//期待采集装置发送M_CON_NA_3(确认帧)
			int iReturn = WantConfirm(pLinker,pCmd);
			if(iReturn != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"No get M_CON_NA_3 in Protocol60_103::GetRecWaveFileList().");
				if(iReturn == -1)
				{
					pLinker->Close();
				}
				return false;
			}
			pCmd->_state.ToNextState();
		}

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetRecWaveFileList:Send C_PL1_NA_3.");
				//请求1级用户数据
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::GetRecWaveFileList().");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//期待采集装置发送ASDU16命令
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
			PVFRAMEHEAD pVFrameHead = NULL;	//帧头
			PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
			PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

			char *pAsduData=NULL;
			int  AsduDataLen=0;

			int framePacketLen=0;   //帧实际的长度

			if(pCmd->_state._state == CmdState::CS_STEP4) //第4步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetRecWaveFileList:wait for ASDU16.");
				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LERROR,"No get ASDU16 in GetRecWaveFileList().");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetRecWaveFileList has be interrupted.");
					m_errCode = PEC_INTERRUPT;
					return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_16)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU16,but it isn't in Protocol60_103::GetRecWaveFileList()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU16,but it isn't");
					pLinker->Close();
					return false;
				}

				//将ASDU数据解析成命令参数
				unsigned short fileCount =0; //发送过来的文件数
				memcpy(&fileCount,&pAsduData[0],2);

				if(AsduDataLen < (int)(2 + 2*sizeof(struct CP56Time2a) +fileCount*sizeof(CMDFILELIST) ) )
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"The asdu data is too short in Protocol60_103::GetRecWaveFileList()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"The asdu data is too short");
					pLinker->Close();
					return false;
				}

				pAsduData += 2 + 2*sizeof(struct CP56Time2a);

				//取出文件
				for(int i=0;i<fileCount;i++)
				{
					PCMDRESULT pCmdResult = new CMDRESULT;
					memset(pCmdResult,0,sizeof(CMDRESULT));

					pCmdResult->pData  = new char[sizeof(CMDFILELIST)+1];
					memset(pCmdResult->pData,0,sizeof(CMDFILELIST)+1);

					pCmdResult->resultType = CDRT_FILELIST;//返回的结果是文件列表类型
					pCmdResult->resultLen = sizeof(CMDFILELIST);
					memcpy(&pCmdResult->pData[0],&pAsduData[0],sizeof(CMDFILELIST)); //复制数据

					pCmd->resultNum++;
					pCmd->resultList.push_back(pCmdResult);

					pAsduData += sizeof(CMDFILELIST);
				}

				if(pVFrameHead->control.m.acd == false)
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}

				pCmd->_state.ChangeStateTo(CmdState::CS_STEP3);

			}//end  if(pCmd->_state._state == CmdState::CS_STEP4) //第4步

		}//end while(1)

	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetRecWaveFileList()");
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
bool Protocol60_103::GetReportFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//获取简要录波报告
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetReportFile()");

#ifdef _PILING
	//测试桩
	CMDREPORT report;
	PCMDRESULT pCmdResult = new CMDRESULT();

	memset(&report,0,sizeof(CMDREPORT));
	memcpy(report.BASName,"线路1",strlen("线路1"));
	memcpy(report.recWaveFileName,"SH20070318132700",strlen("SH20070318132700"));

	report.faultTime.year=7;
	report.faultTime.month=3;
	report.faultTime.day=18;
	report.faultTime.hours=13;
	report.faultTime.minute=27;
	report.faultTime.milliseconds=680;

	report.FPT.isValid = true;
	report.FPT.A_Fault = true;
	report.JPT.isValid = true;
	report.JPT.A_Jump = true;

	//将简报数据放入结果中
	pCmdResult->resultType = CDRT_REPORT;
	pCmdResult->resultLen = sizeof(CMDREPORT);
	pCmdResult->pData = new char[sizeof(CMDREPORT)+1];
	memset(pCmdResult->pData,0,sizeof(CMDREPORT)+1);
	memcpy(pCmdResult->pData,&report,sizeof(CMDREPORT));

	pCmd->resultNum = 1;
	pCmd->resultList.push_back(pCmdResult);

	return true;
#endif

	FileControl fileCtrl;
	int msgLen = 0; //消息长度
	int sendDataLen = 0;	//发送的数据长度

	FIXFRAME fixFrame;	//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else
	{
		//命令来自于UI、定时器等

		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetReportFile:send C_PL2_NA_3.");
			//发送C_PL2_NA_3（召唤2级数据）
			auto_ptr<string> pQurey( CreateMsg(C_PL2_NA_3, pCmd->destDevId,true,0,0,NULL,0,&msgLen) );
			if( pQurey.get() != NULL)
			{
				sendDataLen = SendData(pLinker,pQurey->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送请求帧
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send C_PL2_NA_3 error in GetReportFile()");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
					pLinker->Close();
					return false;
				}
			}
			else
			{
				m_errCode = PEC_UNKOWN;
				mylog.WriteLog2File(LogControl::LERROR, "create msg error in GetReportFile()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetReportFile:wait for M_NV_NA_3.");
			//期待采集装置发送M_NV_NA_3（无所要求数据，ACD=1）
			int iReturn = WantConfirm(pLinker,pCmd);
			if(iReturn != 1)
			{
				if(iReturn == -1)
				{
					pLinker->Close();
				}
				return false;
			}
			pCmd->_state.ToNextState();
		}

		//是否有简报数据
		//if( fixFrame.control.m.acd != true)
		//{
		//没有数据，返回
		//pLinker->Close();
		//	return true;
		//}

		if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetReportFile:send C_PL1_NA_3.");
			//请求1级用户数据
			if(!SendLevelOne(pLinker,pCmd->destDevId))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		//期待采集装置发送ASDU12命令
		char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
		PVFRAMEHEAD pVFrameHead = NULL;	//帧头
		PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
		PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

		char *pAsduData=NULL;
		int  AsduDataLen=0;

		int framePacketLen=0;	//帧实际的长度

		if(pCmd->_state._state == CmdState::CS_STEP4) //第4步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetReportFile:wait for ASDU12.");
			//取前台的回答帧
			int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
				&framePacketLen,
				&pVFrameHead,
				&pASDUID,
				&pASDUInfId,
				&pAsduData,
				&AsduDataLen,
				NULL,
				SystemCfger::GetInstance().timeOut);

			if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
			{
				pLinker->Close();
				return false;
			}
			if( iReturn == 3) //中断帧
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"GetReportFile has be interrupted.");
				m_errCode = PEC_INTERRUPT;
				return false;
			}

			//是否是期待的命令
			if( pVFrameHead->control.m.fun != MFUNID_8 || pASDUID->TYP != ASDU_12)
			{
				m_errCode = PEC_ERR_ANSWER;
				mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU12,but it isn't");
				//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU12,but it isn't");
				pLinker->Close();
				return false;
			}

			//获取简报数据
			CMDREPORT report;
			PCMDRESULT pCmdResult = new CMDRESULT;

			if(AsduDataLen>0)
			{
				memset(&report,0,sizeof(CMDREPORT));
				memcpy(&report,&pAsduData[0],sizeof(CMDREPORT));

				//将简报数据放入结果中
				pCmdResult->resultType = CDRT_REPORT;
				pCmdResult->resultLen = sizeof(CMDREPORT);
				pCmdResult->pData = new char[sizeof(CMDREPORT)+1];
				memset(pCmdResult->pData,0,sizeof(CMDREPORT)+1);
				memcpy(pCmdResult->pData,&report,sizeof(CMDREPORT));
			}
			else
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Receive null asdudata with asdu12 of GetReportFile.");

				pCmdResult->resultType = CDRT_TRUFALSE;
				pCmdResult->resultLen = 1;
				pCmdResult->pData = new char[2];
				pCmdResult->pData[0] = 0;	//失败
				pCmdResult->pData[1] = '\0';
			}

			pCmd->resultNum = 1;
			pCmd->resultList.push_back(pCmdResult);

		}//end if(pCmd->_state._state == CmdState::CS_STEP4) //第4步

	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetReportFile()");
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SetTime(Linker *,struct Command*)'
bool Protocol60_103::SetTime( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//对时
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::SetTime()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		//pLinker->Close();
		return false;

	}// end if( pCmd->from == CD_REMOTE ) //命令来自于远程
	else
	{
		/*
		//分析命令参数
		if(pCmd->parmLen != sizeof(struct CP56Time2a) || pCmd->pParm == NULL)
		{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
		return false;
		}

		//发送C_SYN_TA_3（ASDU6）报文
		memcpy(&sendData[0],pCmd->pParm,sizeof(struct CP56Time2a));
		auto_ptr<string> pDataPack(CreateMsg(C_SYN_TA_3, pCmd->destDevId,false,8,MINF_0,&sendData[0],sizeof(struct CP56Time2a),&msgLen));

		sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据
		if(sendDataLen != msgLen)
		{
		mylog.WriteLog2File(LogControl::LERROR, "send data error");
		//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
		pLinker->Close();
		return false;
		}*/

		bool result = true;

		//分析命令参数
		if(pCmd->parmLen != 1+1+sizeof(struct SHTimeStruct) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char asdu10_attr[1+1+sizeof(struct SHTimeStruct)]={0};  //属性结构
		memcpy(asdu10_attr,pCmd->pParm,pCmd->parmLen);

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X0101,0X67,23,13,1,13,&asdu10_attr[0])};

		//发送ASDU10带执行的写
		int iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iReturn != 1)
		{
			result = false;
		}

		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = result;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);

		pCmd->resultNum = 1;

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::SetTime()");
		return true;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetTime(Linker *,struct Command*)'
bool Protocol60_103::GetTime( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetTime()");

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//分析命令参数
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
			return false;
		}

		unsigned char timeType = pCmd->pParm[0];	//时间类型

		//asdu10带执行的写的报文
		unsigned char asdu10_attr[1]={0};	//属性结构
		asdu10_attr[0] = timeType;
		GI asdu10_gi[1] ={GI(0X0201,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21（控制方向）―― 读时间
		GI asdu21_gi[1] ={GI(0X0202,0X67,23,6,1,0,NULL)};

		FileControl fileCtrl;

		//获取通用分类服务返回的属性结构数据
		struct SHTimeStructDAU timeData;
		char	*pTime[1] = {(char*)&timeData};
		int  timeDataLen = sizeof(timeData);
		int *pDataLen[1] = {&timeDataLen};
		memset(&timeData,0,timeDataLen);
		int iAttrNum = 1;

		unsigned char RII = 0;
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,pTime,pDataLen,&iAttrNum,&RII);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::SetTime()");

			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//设置系统时间
		struct CP56Time2a getTime;
		memset(&getTime,0,sizeof(struct CP56Time2a));
		getTime.year = timeData.Year%100;
		getTime.month = timeData.Month;
		getTime.day = timeData.Day;
		getTime.hours = timeData.Hour;
		getTime.minute = timeData.Minute;
		getTime.milliseconds = timeData.Second*1000 + timeData.Millisecond;

		//写结果
		pCmd->resultNum = 1;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TIME;
		pResult->resultLen = sizeof(struct CP56Time2a);
		pResult->pData = new char[pResult->resultLen];
		memcpy(pResult->pData,&getTime,pResult->resultLen);
		pCmd->resultList.push_back(pResult);

	} //end if( pCmd->from == CD_REMOTE )

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetTime()");
	return result;

	// }}}USR
}
// }}}RME

// {{{RME operation 'StartRecByManual(Linker *,struct Command*)'
bool Protocol60_103::StartRecByManual( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//启动录波
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartRecByManual()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		//分析命令参数
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
			return false;
		}

		unsigned char startType = pCmd->pParm[0];	//启动类型
		unsigned char operateType = 1;	//操作类型(1：工程师站或后台操作)
		unsigned short GIN = 0;	//通用分类标识符

		GIN = 0X0301;   //手动启动

		//asdu10带执行的写的报文
		unsigned char attr[2];  //属性结构
		attr[0] = operateType;
		attr[1] = startType;
		GI gi[1] ={GI(GIN,0X67,23,2,1,2,&attr[0])};

		return StartRec(pLinker,pCmd,gi);
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'StartRecByTimer(Linker *,struct Command*)'
bool Protocol60_103::StartRecByTimer( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//启动录波
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartRecByTimer()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;

	}
	else //命令来自于UI、定时器等
	{
		//分析命令参数
		if(pCmd->parmLen != 1+sizeof(struct SHTimeStruct)+1+sizeof(unsigned short) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::StartRecByTimer()");
			return false;
		}

		unsigned char startType = pCmd->pParm[0];	//启动类型

		struct SHTimeStruct startTime;	//启动开始时间
		memset(&startTime,0,sizeof(startTime));
		memcpy(&startTime,&pCmd->pParm[1],sizeof(startTime));

		unsigned char interval = pCmd->pParm[1+sizeof(startTime)];	//间格,故障录波单位秒，必须大于5秒。低速录波单位分钟，大于设定低速记录时间长度

		unsigned short startCount =0;	//启动次数
		memcpy(&startCount,&pCmd->pParm[1+sizeof(startTime)+1],sizeof(startCount));

		unsigned short GIN = 0;	//通用分类标识符
		GIN = 0X0401;   //定时启动

		unsigned char attr[10];	//属性结构
		attr[0] = startType;	//启动类型

		memcpy(&attr[1],&startTime.Microsecond,2);	//启动开始时间的微秒

		//启动开始时间的Time_t时间
		struct tm t;
		memset(&t,0,sizeof(t));
		t.tm_hour = startTime.Hour;
		t.tm_isdst = 0;
		t.tm_mday = startTime.Day;
		t.tm_min = startTime.Minute;
		t.tm_mon = startTime.Month-1;
		t.tm_sec = 0;
		t.tm_year = startTime.Year-1900;

		time_t tt= mktime(&t);
		memcpy(&attr[3],&tt,4);

		attr[7] = interval; //间格

		memcpy(&attr[8],&startCount,2);	//启动次数

		//asdu10带执行的写的报文
		GI gi[1] ={GI(GIN,0X67,23,10,1,10,&attr[0])};

		return StartRec(pLinker,pCmd,gi);
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'StartRec(Linker *,struct Command*,GI[])'
bool Protocol60_103::StartRec( Linker * pLinker, struct Command * pCmd, GI GIs[] )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartRec()");

	//发送ASDU10带执行的写
	int iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,GIs,1);
	if(iReturn == 1)
	{
		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::StartRec()");
		return true;
	}
	else if(iReturn == -1)
	{
		mylog.WriteLog2File(LogControl::LERROR, "SendAsdu10WithExeWrite return error in Protocol60_103::StartRec()");
		pLinker->Close();
		return false;
	}

	mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::StartRec()");
	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetGIAttrData(Linker *,struct Command *,GI[],unsigned char,GI[],unsigned char,char*,int*,int*,unsigned char*)'
int Protocol60_103::GetGIAttrData( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber, GI asdu21_GIs[], unsigned char asdu21_GINumber, char * outAttrData[], int * outAttrLen[], int * outAttrNum, unsigned char * outRII )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetGIAttrData()");

	int msgLen=0;
	int sendDataLen =0;
	FIXFRAME fixFrame;	//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));
	int iReturn = 0;

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};//可变帧数据
	PVFRAMEHEAD pVFrameHead = NULL;	//帧头
	PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;	//帧实际的长度

	//发送ASDU10带执行的写
	iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_GIs,asdu10_GINumber);
	if(iReturn != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::GetGIAttrData()");
		return iReturn;
	}

	if(pCmd->_state._state == CmdState::CS_STEP5) //第五步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 5 of GetGIAttrData:send asdu21.");
		//ASDU21（控制方向）―― 读返回的文件名
		auto_ptr<string> pDataPack(CreateGIMsg(C_GC_NA_3,pCmd->destDevId,0X2A,CINF_241,asdu21_GIs,asdu21_GINumber,&msgLen));
		if( pDataPack.get() != NULL)
		{
			sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送asdu21带执行的写的报文 
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LERROR, "send C_GC_NA_3 error in Protocol60_103::GetGIAttrData()");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"send C_GC_NA_3 error");
				return -1;
			}
		}
		else
		{
			m_errCode = PEC_UNKOWN;
			mylog.WriteLog2File(LogControl::LERROR, "create C_GC_NA_3 msg error in Protocol60_103::GetGIAttrData()");
			return -1;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP6) //第六步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 6 of GetGIAttrData:wait for M_CON_NA_3.");
		//期待采集装置发送M_CON_NA_3 （确认，ACD=1）
		iReturn = WantConfirm(pLinker,pCmd);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING, "wait M_CON_NA_3 fail in Protocol60_103::GetGIAttrData()");
			return iReturn;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP7) //第七步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 7 of GetGIAttrData:send C_PL1_NA_3.");
		//请求1级用户数据
		if(!SendLevelOne(pLinker,pCmd->destDevId))
		{
			mylog.WriteLog2File(LogControl::LERROR, "send C_PL1_NA_3 error in Protocol60_103::GetGIAttrData()");
			return -1;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP8) //第八步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 8 of GetGIAttrData:wait for Asdu10.");
		//期待Asdu10（监视方向）―― 返回一个组的全部条目值 
		memset(&framePacket[0],0,sizeof(framePacket));

		//取前台的回答帧
		int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
			&framePacketLen,
			&pVFrameHead,
			&pASDUID,
			&pASDUInfId,
			&pAsduData,
			&AsduDataLen,
			NULL,
			SystemCfger::GetInstance().timeOut);

		if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
		{
			mylog.WriteLog2File(LogControl::LWARNING, "wait for Asdu10 fail in Protocol60_103::GetGIAttrData()");
			pLinker->Close();
			return -1;
		}
		if( iReturn == 3) //中断帧
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"GetGIAttrData has be interrupted.");
			return 0;
		}

		//是否是期待的命令
		if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_10)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU10,but it isn't in Protocol60_103::GetGIAttrData()");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU10,but it isn't");
			return -1;
		}

		//判断返回
		if(pASDUID->COT != 0X2A) //不是响应读条目值
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"The command isn't the response for reading an entry in Protocol60_103::GetGIAttrData().");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"The frame isn't the response for reading an entry");
			return -1;
		}

		if(outRII != NULL)
		{
			*outRII = pAsduData[0];
		}

		unsigned char unNGD = pAsduData[1];//条目个数

		char *pTmpAsduData = pAsduData;
		pTmpAsduData += 2; //跳过RII(1字节)+NGD(1字节)

		//分析出所有条目
		GDD gdd;
		unsigned short GIN = 0;//通用分类标识序号
		char KOD = 0;
		int minAttrDataLen = 0;
		unsigned char i=0;
		int iAttrNum = 0;
		for(;i< unNGD && iAttrNum< *outAttrNum; i++)
		{
			memcpy(&GIN,pTmpAsduData,2);//得到gin
			pTmpAsduData += 2;
			KOD = *pTmpAsduData;//得到kod
			pTmpAsduData += 1;
			memcpy(&gdd,pTmpAsduData,3); //得到gdd
			pTmpAsduData += 3;//跳过GDD结构(3字节)

			if( (GIN & 0X00FF) > 1)
			{
				minAttrDataLen = min( gdd.dataLen * gdd.dataNum ,*outAttrLen[iAttrNum]);
				memcpy(outAttrData[iAttrNum],pTmpAsduData,minAttrDataLen); //获得属性结构数据
				*outAttrLen[iAttrNum] = minAttrDataLen;
				iAttrNum++;
			}

			pTmpAsduData += gdd.dataLen * gdd.dataNum;
		}

		*outAttrNum = iAttrNum;

		pCmd->_state.ToNextState();
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetGIAttrData()");
	return 1;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileList(Linker *,struct Command*)'
bool Protocol60_103::GetFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//召唤通用文件列表
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetFileList( )");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		//pLinker->Close();
		return false;

	}
	else //命令来自于UI、定时器等
	{
#ifdef _PILING
		//测试桩
		for(int i=0;i<6;i++)
		{
			string fileName("TestFile_");
			char no[7]={0};
			sprintf(no,"%d.thr",i);
			fileName.append(no);

			string fileTime("2008-01-18 10:30:59");

			CMDDIRFILEINF dirFileInf ;
			memset(&dirFileInf,0,sizeof(CMDDIRFILEINF));
			dirFileInf.fileNo = 0;//fileNo;
			memcpy(dirFileInf.fileName,fileName.c_str(),fileName.size());
			memcpy(dirFileInf.fileTime,fileTime.c_str(),fileTime.size());
			dirFileInf.fileLen = 1024*(3+i);

			pCmd->resultNum += 1;
			PCMDRESULT pComdResult = new CMDRESULT();
			pComdResult->resultType = CDRT_DIRFILEINF;    //返回结果是真假类型
			pComdResult->resultLen = sizeof(CMDDIRFILEINF); //结果长度1字节
			pComdResult->pData = new char[sizeof(CMDDIRFILEINF)+1];
			memset(pComdResult->pData,0,sizeof(CMDDIRFILEINF)+1);

			memcpy(pComdResult->pData,&dirFileInf.fileNo,sizeof(dirFileInf));

			pCmd->resultList.push_back(pComdResult);
		}
		return true;
#endif

		//分析命令参数
		if(pCmd->parmLen != 5 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		int msgLen = 0; //消息长度
		int sendDataLen = 0;	//发送的数据长度

		unsigned char dirType = pCmd->pParm[0];//目录类型
		unsigned int fileNum = 0; //最近的文件数，0表示目录下所有的文件
		memcpy(&fileNum,&pCmd->pParm[1],4);

		//asdu10带执行的写的报文
		unsigned char asdu10_attr[110]={0};	//属性结构
		asdu10_attr[0] = dirType;
		memcpy(&asdu10_attr[1],&fileNum,sizeof(unsigned int));

		GI asdu10_gi[1] ={GI(0XA001,0X67,23,110,1,110,&asdu10_attr[0])};

		//ASDU21（控制方向）―― 读返回的文件名
		GI asdu21_gi[1] ={GI(0XA002,0X67,1,100,1,0,NULL)};

		char pFileName[100+1] ={0};	//文件名称
		char *ppFileName[1] = {&pFileName[0]};
		int  fileNameLen = 100;
		int *pLen[1] = {&fileNameLen};
		int iAttrNum = 1;

		//获取通用分类服务返回的文件名称
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppFileName,pLen,&iAttrNum);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::GetFileList()");
			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//保存中断环境
		struct tagEnVirionment
		{
			unsigned int startPos;
			char fileName[100+1];	//文件名称
			int  fileNameLen;
		};

		unsigned int startPos = 0;	//文件起始传输位置

		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetFileList.");

			struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
			startPos = pEV->startPos;
			fileNameLen = pEV->fileNameLen;
			memcpy(pFileName,pEV->fileName,sizeof(pEV->fileName));
		}

		if(pCmd->_state._state == CmdState::CS_STEP9) //第九步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetFileList:pregetfile");

			struct tagEnVirionment *pEV = new struct tagEnVirionment;
			pEV->fileNameLen = fileNameLen;
			pEV->startPos = startPos;
			memcpy(pEV->fileName,pFileName,sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//准备获取文件前的处理
			if( !PreGetFile(pLinker,pCmd,2,pFileName, 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in Protocol60_103::GetFileList()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState(); //改变任务状态到下状态
		}

		if(pCmd->_state._state == CmdState::CS_STEP10) //第十步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetFileList:wait for M_CON_NA_3");
			//期待采集装置发送M_CON_NA_3(确认帧)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,":wait for M_CON_NA_3 fail in Protocol60_103::GetFileList()");
				return false;
			}
			pCmd->_state.ToNextState(); //改变任务状态到下状态
		}

		char directorData[PRO103_ASDUDATA_MAXSIZE+512]={0}; //目录文件数据
		int remain = 0 ;	//还剩多少数据没有分析

		while(1)
		{
			//清空数据
			memset(&directorData[remain],0,(sizeof(directorData) - remain));

			if(pCmd->_state._state == CmdState::CS_STEP11) //第十一步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 11 of GetFileList:send C_PL1_NA_3");
				//请求1级用户数据(从请求1级用户数据开始循环)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,":send C_PL1_NA_3 error in Protocol60_103::GetFileList()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState(); //改变任务状态到下状态
			}

			if(pCmd->_state._state == CmdState::CS_STEP12) //第十二步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 12 of GetFileList:wait for ASDU104");
				//期待采集装置发送ASDU104命令
				char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
				PVFRAMEHEAD pVFrameHead = NULL;	//帧头
				PASDUID pASDUID = NULL;      //应用服务数据单元标识符
				PASDUINFID pASDUInfId = NULL;   //应用服务数据单元信息体标识符

				char *pAsduData=NULL;
				int  AsduDataLen=0;
				int framePacketLen=0;	//帧实际的长度

				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LERROR,"wait for ASDU104 error in Protocol60_103::GetFileList()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Get an in interrupt frame in Protocol60_103::GetFileList()");
					m_errCode = PEC_INTERRUPT;
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置
					return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::GetFileList()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//解析ASDU数据
				if(pAsduData == NULL || AsduDataLen <= 0)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LDEBUG,"No find ASDU data in Protocol60_103::GetFileList()");
					return false;
				}

				memcpy(&startPos,&pAsduData[0],4);
				int writeLen = AsduDataLen - 4;
				memcpy(&directorData[remain],&pAsduData[4],writeLen);

				//解析目录文件数据
				char szLine[256]={0};//保存1行数据
				int startLinePos = 0;
				int endLinePos = 0;
				while( startLinePos < (remain + writeLen))
				{
					if(directorData[endLinePos] == ';')
					{
						//取1行数据
						memcpy(szLine,&directorData[startLinePos],endLinePos-startLinePos);
						startLinePos = endLinePos +2;
						endLinePos++;

						//分析数据行
						string sLine(szLine);
						memset(szLine,0,sizeof(szLine));

						string::size_type pos1 = sLine.find(",");
						string::size_type pos2 = sLine.find(",",pos1+1);
						//string::size_type pos3 = sLine.find(",",pos2+1);
						if(pos1 == string::npos || pos2 == string::npos)// || pos3 == string::npos)
						{
							continue;
						}

						//int fileNo = ::atoi(sLine.substr(0,pos1).c_str());
						string fileTime = sLine.substr(0,pos1);
						string fileName = sLine.substr(pos1+1,pos2-pos1-1);
						int fileLen = ::atoi(sLine.substr(pos2+1,sLine.size()-pos2-1).c_str());

						CMDDIRFILEINF dirFileInf ;
						memset(&dirFileInf,0,sizeof(CMDDIRFILEINF));
						dirFileInf.fileNo = 0;//fileNo;
						memcpy(dirFileInf.fileName,fileName.c_str(),fileName.size());
						memcpy(dirFileInf.fileTime,fileTime.c_str(),fileTime.size());
						dirFileInf.fileLen = fileLen;

						pCmd->resultNum += 1;
						PCMDRESULT pComdResult = new CMDRESULT;
						pComdResult->resultType = CDRT_DIRFILEINF;//返回结果是真假类型
						pComdResult->resultLen = sizeof(CMDDIRFILEINF);//结果长度1字节
						pComdResult->pData = new char[sizeof(CMDDIRFILEINF)+1];
						memset(pComdResult->pData,0,sizeof(CMDDIRFILEINF)+1);

						memcpy(pComdResult->pData,&dirFileInf,sizeof(dirFileInf));

						pCmd->resultList.push_back(pComdResult);

					}
					else
					{
						endLinePos++;
					}

					if(endLinePos >= (remain + writeLen) )
					{
						remain = remain + writeLen - startLinePos;

						remain = (remain<0)?0:remain;
						memcpy(directorData,&directorData[startLinePos],remain);
						break;
					}
				}// end while( startLinePos < (remain + writeLen))

				startPos += writeLen;

				if(pVFrameHead->control.m.acd == false) //传输完毕
				{
					//成功
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
				else
				{
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //第十二步

		} //end while

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetFileList( )");
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'PullFile(Linker *,struct Command*)'
bool Protocol60_103::PullFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::PullFile()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		//分析命令参数
		if(pCmd->parmLen != 1+4+4+4+100+19+1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PullFile()");
			return false;
		}

		unsigned char fileType = pCmd->pParm[0];//文件类型
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

		//分析文件时间
		string sFileTime(fileTime);
		string::size_type pos1 = sFileTime.find("-");
		string::size_type pos2 = sFileTime.find("-",pos1+1);
		if(pos1 != 4 || pos2 == string::npos )
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PullFile()");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"error command param");
			pLinker->Close();
			return false;
		}
		string fileYear = sFileTime.substr(0,4);
		string fileMonth = sFileTime.substr(pos1+1,pos2-pos1-1);
		int iFileYearMonth = ::atoi(fileYear.c_str())*100 + ::atoi(fileMonth.c_str());

		FileControl::FILETYPE fileControlType;	//文件类型
		switch(fileType)
		{
		case 1:
			fileControlType = FileControl::FT_LOW; //低速录波文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is low-record-file");
			break;
		case 2:
		case 3:
		case 9:
		case 100:
		case 200:
			fileControlType = FileControl::FT_CONT; //连续录波文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is continue-record-wave-file");
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			fileControlType = FileControl::FT_COMTRADE; //标准comtrade文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is comtrade file");
			break;
		case 10:
			fileControlType = FileControl::FT_SHD; //SHD文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SHD file");
			break;
		case 11:
		case 12:
		case 13:
			fileControlType = FileControl::FT_GENERATORTEST; //发电机实验文件文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is GeneratorTest file");
			break;
		case 101:
			fileControlType = FileControl::FT_SOE; //标记事件文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SOE file");
			break;
		case 102:
			fileControlType = FileControl::FT_LOG; //日记文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is LOG file");
			break;
		case 210:
		case 211:
		case 212:
		case 220:
			fileControlType = FileControl::FT_CFG; //配置相关文件
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is configer file");
			break;
		default:
			fileControlType = FileControl::FT_CFG; //未知的文件类型，与配置文件同放在根目录下
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is unkown type file");
			break;
		}

		//加上年月目录(配置相关不创建年月目录)
		char *pYearMonth = NULL;
		char szYearMonth[7]={0};
		if(fileControlType != FileControl::FT_CFG)
		{
			::itoa(iFileYearMonth,szYearMonth,10);
			pYearMonth = &szYearMonth[0];
		}

		unsigned char asdu10_attr[110]={0};	//属性结构
		asdu10_attr[0] = fileType;		//传入文件类型
		memcpy(&asdu10_attr[1],&fileNO,sizeof(unsigned int));	//传入文件号
		memcpy(&asdu10_attr[5],&fileStartPos,sizeof(unsigned int));	//传入文件开始位置
		memcpy(&asdu10_attr[9],inputFileName,100);   //传入文件名称
		asdu10_attr[109] = IsNeedCompress;	//是否压缩

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0XA001,0X67,23,110,1,110,&asdu10_attr[0])};

		//ASDU21（控制方向）―― 读返回的文件名
		GI asdu21_gi[1] ={GI(0XA002,0X67,1,100,1,0,NULL)};

		return GetGeneraFile(pLinker,pCmd,inputFileName,fileTotalLen,fileControlType,pYearMonth,asdu10_gi,1,asdu21_gi,1,IsNeedCompress);

	} //end if( pCmd->from == CD_REMOTE )

	// }}}USR
}
// }}}RME

// {{{RME operation 'PushFile(Linker *,struct Command*)'
bool Protocol60_103::PushFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::PushFile()");

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}// end if( pCmd->from == CD_REMOTE ) //命令来自于远程
	else
	{
		int msgLen = 0; //消息长度

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
		char localFileName[101]={0};		//本地文件名称(含路径)
		string sFileName("");			//文件名(不含路径)
		char ADUFileName[100]={{0}};	//前置机文件名称
		char *ppFileName[1] = {&ADUFileName[0]};
		int  fileNameLen = 100;
		int *pFileNameLen[1] = {&fileNameLen};
		int iAttrNum = 1;

		memcpy(&userName[0],&pCmd->pParm[1],20);
		memcpy(&passward[0],&pCmd->pParm[1+20],10);
		memcpy(&localFileName[0],&pCmd->pParm[1+20+10],100);

		//解析出文件名称
		string sFilePath(localFileName);
		int pos1 = sFilePath.find_last_of("/");
		int pos2 = sFilePath.find_last_of("\\");

		string::size_type pos = max(pos1,pos2);
		if(pos != string::npos)
		{
			sFileName = sFilePath.substr(pos+1,sFilePath.size() -pos -1);
		}
		else
		{
			sFileName = sFilePath;//
		}

		unsigned char asdu10_attr[135]={0};  //属性结构
		asdu10_attr[0] = fileType;
		memcpy(&asdu10_attr[1],&userName[0],20);
		memcpy(&asdu10_attr[21],&passward[0],10);
		memcpy(&asdu10_attr[35],sFileName.c_str(),sFileName.size());

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0XB001,0X67,23,135,1,135,&asdu10_attr[0])};

		//ASDU21（控制方向）―― 读返回的文件名
		GI asdu21_gi[1] ={GI(0XB002,0X67,1,100,1,0,NULL)};

		FileControl fileCtrl;

		//保存中断环境
		struct tagEnVirionment
		{
			unsigned int startPos;
			char fileName[100+1];	//文件名称
			int  fileNameLen;
		};

		unsigned int startPos = 0;	//文件起始传输位置

		if(pCmd->_state.pStoreParm != NULL)	//任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running PushFile");

			struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
			startPos = pEV->startPos;
			fileNameLen = pEV->fileNameLen;
			memcpy(&ADUFileName[0],pEV->fileName,sizeof(pEV->fileName)-1);
		}

		//获取通用分类服务返回的文件名称
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppFileName,pFileNameLen,&iAttrNum);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::PushFile()");
			if(iReturn == 1)
			{
				pLinker->Close();
			}
			return false;
		}

		bool preFCB = false;

		//Asdu107发送的文件数据头的结构
		/*struct _Asdu107FileData
		{
		char fileName[100];	//文件名
		struct CP56Time2a fileTime;	//文件时间
		unsigned int fileLen;	//文件长度
		} fileData;*/

		//memset(&fileData,0,sizeof(fileData));

		/*memcpy(&fileData.fileName[0],&ADUFileName[0],100);
		fileCtrl.GetFileTime(&localFileName[0], &fileData.fileTime);
		fileData.fileLen = fileCtrl.GetFileLen(&localFileName[0]);*/
		int fileLen = fileCtrl.GetFileLen(&localFileName[0]);
		int hasReadLen = 0; //文件已经读取的长度

		//每次可从文件读取的最大文件数据长度
		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;// -sizeof(struct _Asdu107FileData);
		int factReadLen = 0;    //实际读取的数据长度

		if(!fileCtrl.OpenFile(&localFileName[0],FileControl::OM_READ))
		{
			m_errCode = PEC_UNKOWN;
			string sInfo = string("open the file: ") +string(&localFileName[0]) +string(" fail in Protocol60_103::PushFile()");
			mylog.WriteLog2File(LogControl::LERROR,sInfo.c_str());
			//mylog.WriteLog(LogControl::LERROR,pCmd,"Open file failed");
			pLinker->Close();
			return false;
		}

		MemMapMgr memMgr;

		//print
		DWORD pringtTime = GetTickCount();
		char szInfo[255] = {0};

		int count = 0;
		int maxCount = fileLen/readMaxLen; //最大循环次数
		int iInterval = maxCount/20;	//间隔多少次循环之后写一次进度信息(全过程最多写20次进度信息)
		iInterval = (iInterval <= 0)?30:iInterval;

		while(1)
		{
			count++;

			int sendDataLen = 0;	//发送的数据长度
			FIXFRAME fixFrame;
			memset(&fixFrame,0,sizeof(fixFrame));

			char ASDUData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//ASDU数据
			int  ASDUDataLen = 0;

			memcpy(&ASDUData[0],&startPos,4);		//起始传输位置

			if(pCmd->_state._state == CmdState::CS_STEP9) //第9步
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 9 of PushFile:send ASDU107");

				if(pCmd->_state.pStoreParm == NULL)
				{
					struct tagEnVirionment *pEV = new struct tagEnVirionment;
					pEV->fileNameLen = fileNameLen;
					pEV->startPos = startPos;
					memcpy(pEV->fileName,&ADUFileName[0],sizeof(pEV->fileName));

					pCmd->_state.pStoreParm = (void*)pEV;
				}

				factReadLen = fileCtrl.ReadData(startPos,&ASDUData[4],readMaxLen);
				hasReadLen += factReadLen;
				ASDUDataLen = 4 + factReadLen;// + sizeof(fileData);

				//发送ASDU107命令
				auto_ptr<string> pDataPack;
				if(factReadLen <= 0 || hasReadLen >= fileLen )
				{
					pDataPack = auto_ptr<string>(CreateMsg(C_EX107_NA_3,pCmd->destDevId,false,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
				}
				else
				{
					pDataPack = auto_ptr<string>(CreateMsg(C_EX107_NA_3,pCmd->destDevId,true,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send ASDU107 error in Protocol60_103::PushFile()");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			startPos += factReadLen; 

			if(pCmd->_state._state == CmdState::CS_STEP10) //第10步
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 10 of PushFile:wait for M_CON_NA_3");
				//期待采集装置发送M_CON_NA_3(确认帧)
				int iReturn = WantConfirm(pLinker,pCmd);
				if(iReturn != 1)
				{
					mylog.WriteLog2File(LogControl::LWARNING, "wait for M_CON_NA_3 fail in Protocol60_103::PushFile()");

					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置

					result = false;
					break;
				}

				if(factReadLen <= 0 || hasReadLen >= fileLen )
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
				}
				else
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP9);
				}
			}

			/*pringtTime = GetTickCount();
			sprintf(szInfo,"after WantConfirm time:%d",pringtTime);
			mylog.WriteLog2File(LogControl::LDEBUG, szInfo);*/

			//每传iInterval次写一次进度信息
			if(count%iInterval == 0)
			{
				//写进度信息
				pCmd->resultNum = 1;
				PCMDRESULT pResult = new CMDRESULT;
				pResult->resultType = CDRT_PROGRESS;
				pResult->resultLen = 1;
				pResult->pData = new char[2];
				pResult->pData[1] = '\0';

				int iProgress = 100 * (startPos)/(fileLen);
				iProgress = (iProgress>100)?100:iProgress;
				pResult->pData[0] = iProgress;
				pCmd->resultList.push_back(pResult);

				memMgr.WriteTaskResultToMem(*pCmd);

				//清除结果
				delete pResult;
				pResult = NULL;
				pCmd->resultNum = 0;
				pCmd->resultList.clear();
			}

			//数据已经发送完毕
			if(factReadLen <= 0 || hasReadLen >= fileLen )
			{
				result = true;
				break;
			}

		} //end while

		fileCtrl.CloseFile();

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::PushFile()");
	return result ;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReadRltData(Linker *,struct Command*)'
bool Protocol60_103::ReadRltData( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::ReadRltData( )");


#ifdef _PILING

	//测试桩
	PCMDRESULT pResult = new CMDRESULT();
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	pResult->pData[0] = 1;
	pResult->pData[1] = '\0';
	pCmd->resultList.push_back(pResult);

	pCmd->resultNum = 1;

	return true;
#endif

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//分析命令参数
		if(pCmd->parmLen != 2 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char dataType = pCmd->pParm[0];//数据类型
		unsigned char StartType = pCmd->pParm[1];//启动类型

		unsigned char asdu10_attr[2]={0};  //属性结构
		asdu10_attr[0] = dataType;
		asdu10_attr[1] = StartType;

		if(StartType == 1)
		{
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_START,"（启动）") );
		}
		else
		{
			::strcat(pCmd->describe, (CStringRes::GetInstance())->GetStrById(IDS_STOP,"（停止）") );
		}

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X6001,0X67,23,2,1,2,&asdu10_attr[0])};

		//发送ASDU10带执行的写
		int iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iReturn != 1)
		{
			result = false;
		}

		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = result;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);

		pCmd->resultNum = 1;

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::ReadRltData( )");
	return result ;

	// }}}USR
}
// }}}RME

// {{{RME operation 'AutoSend(Linker *,struct Command*)'
bool Protocol60_103::AutoSend( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::AutoSend( )");

	if( pCmd->from != CD_ASDU ) //命令不是来自于ADU
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{

#ifdef _PILING
		string sFileNameTmp("d:/data/1/FaultReport.txt");

		//写执行结果
		pCmd->resultNum = 2;
		PCMDRESULT pResultTmp = new CMDRESULT();
		pResultTmp->resultType = CDRT_TRUFALSE;
		pResultTmp->resultLen = 1;
		pResultTmp->pData = new char[2];
		pResultTmp->pData[0] = 1;      //执行成功
		pResultTmp->pData[1] = '\0';
		pCmd->resultList.push_back(pResultTmp);

		pResultTmp = new CMDRESULT();
		pResultTmp->resultType = CDRT_FILEPATH;
		pResultTmp->resultLen = sFileNameTmp.size();
		pResultTmp->pData = new char[pResultTmp->resultLen+1];
		memset(pResultTmp->pData,0,pResultTmp->resultLen+1);
		memcpy(pResultTmp->pData,sFileNameTmp.c_str(),pResultTmp->resultLen);
		pCmd->resultList.push_back(pResultTmp);

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::AutoSend( )");
		return true;
#endif

		//命令来自于ADU
		int msgLen = 0; //消息长度
		int sendDataLen = 0;    //发送的数据长度

		//分析命令参数
		if(pCmd->parmLen != 1+4+100 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::AutoSend()");
			return false;
		}

		//解析参数
		unsigned char fileType = pCmd->pParm[0];//文件类型
		int fileLen = 0;	//文件长度
		memcpy(&fileLen,&pCmd->pParm[1],4);
		char pSendFileName[101] = {0}; //前台上传的文件名(包含路径)
		memcpy(pSendFileName,&pCmd->pParm[1+4],100);
		string sFileName("");			//文件名(不包含路径)
		string sLocalFileName("");		//本地路径的文件名(包含路径)

		//解析出文件名称
		string sFilePath(pSendFileName);
		int pos1 = sFilePath.find_last_of("/");
		int pos2 = sFilePath.find_last_of("\\");

		string::size_type pos = max(pos1,pos2);
		if(pos != string::npos)
		{
			sFileName = sFilePath.substr(pos+1,sFilePath.size() -pos -1);
		}
		else
		{
			sFileName = sFilePath;
		}

		FileControl::FILETYPE fileControlType;	//文件类型
		FileControl fileCtrl;

		unsigned int startPos = 0;	//文件起始传输位置
		FileControl::OPENMODE fileOpenMod = FileControl::OM_WRITE;//文件打开方式

		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			startPos = *((unsigned int*)(pCmd->_state.pStoreParm));
			fileOpenMod = FileControl::OM_APPEND;
		}

		//modify by kh
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

		//标记事件文件存入硬盘，故障录波报告存入数据库
		if(fileType == 2)
		{
			fileControlType = FileControl::FT_SOE; //标记事件文件

			//获得文件存放目录
			sLocalFileName = fileCtrl.GetFileTypeDirector(devId,fileControlType);

			//加上年月目录(配置相关不创建年月目录)
			SYSTEMTIME sysTime;
			memset(&sysTime,0,sizeof(sysTime));
			GetLocalTime(&sysTime);

			char szYearMonth[7]={0};
			int iYearMonth = sysTime.wYear*100 + sysTime.wMonth;
			sLocalFileName = sLocalFileName + string(::itoa(iYearMonth,szYearMonth,10)) + string("/");
			if(!fileCtrl.IsExist(sLocalFileName.c_str()))
			{
				_mkdir(sLocalFileName.c_str());
			}

			sLocalFileName = sLocalFileName + sFileName;

			if(!fileCtrl.OpenFile(sLocalFileName.c_str(),fileOpenMod))
			{
				m_errCode = PEC_UNKOWN;
				string sInfo = string("open the file: ") +sLocalFileName +string(" fail in Protocol60_103::AutoSend()");
				mylog.WriteLog2File(LogControl::LERROR,sInfo.c_str());
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Open file failed");
				pLinker->Close();
				return false;
			}
		}

		MemMapMgr memMgr;
		int iProgress = 0;

		if(pCmd->_state._state == CmdState::CS_STEP1) //第一步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of AutoSend:pregetfile");
			//准备获取文件前的处理
			if( !PreGetFile(pLinker,pCmd,2,&pSendFileName[0], 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in in Protocol60_103::AutoSend()");
				pCmd->_state.ChangeStateTo(CmdState::CS_FAIL);
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState(); //改变任务状态到下状态
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //第二步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of AutoSend:wait for M_CON_NA_3");
			//期待采集装置发送M_CON_NA_3(确认帧)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"wait for M_CON_NA_3 fail in in Protocol60_103::AutoSend()");
				return false;
			}
			pCmd->_state.ToNextState(); //改变任务状态到下状态
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;
		int count = 0;
		int maxCount = fileLen/readMaxLen; //最大循环次数
		int iInterval = maxCount/20;    //间隔多少次循环之后写一次进度信息(全过程最多写20次进度信息)
		iInterval = (iInterval <= 0)?30:iInterval;

		while(1)
		{
			count++;

			if(pCmd->_state._state == CmdState::CS_STEP3) //第三步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of AutoSend:send C_PL1_NA_3");
				//请求1级用户数据(如果数据没有发送完，则从请求1级用户数据开始循环)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"send C_PL1_NA_3 erorr in in Protocol60_103::AutoSend()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState(); //改变任务状态到下状态
			}

			//期待采集装置发送ASDU104命令
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
			PVFRAMEHEAD pVFrameHead = NULL;	//帧头
			PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
			PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符
			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;	//帧实际的长度

			if(pCmd->_state._state == CmdState::CS_STEP4) //第四步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of AutoSend:wait for ASDU104");
				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LWARNING,"wait for ASDU104 fail in Protocol60_103::AutoSend()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"AutoSend has be interrupted");

					m_errCode = PEC_INTERRUPT;
					if(pCmd->_state.pStoreParm == NULL)
					{
						unsigned int *pPos = new unsigned int;
						pCmd->_state.pStoreParm = (void*)pPos;
					}

					//保存中断时的传输位置
					*((unsigned int*)(pCmd->_state.pStoreParm)) = startPos;
					return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::AutoSend()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//解析ASDU数据
				if(pAsduData == NULL || AsduDataLen <= 0)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"error ASDU data in Protocol60_103::AutoSend()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"error ASDU data");
					pLinker->Close();
					return false;
				}

				memcpy(&startPos,&pAsduData[0],4);
				int writeLen = AsduDataLen - 4;

				if(fileType == 2)	//标记事件
				{
					fileCtrl.Seek(startPos);
					fileCtrl.WriteData(&pAsduData[4],writeLen);
				}
				else		//故障报告
				{
				}

				startPos += writeLen;

				if(pVFrameHead->control.m.acd == false) //文件传输完毕
				{
					//成功
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
				else
				{
					if(pCmd->_state.pStoreParm == NULL)
					{
						unsigned int *pPos = new unsigned int;
						pCmd->_state.pStoreParm = (void*)pPos;
					}

					//保存中断时的传输位置
					*((unsigned int*)(pCmd->_state.pStoreParm)) = startPos;
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP3);
				}

				if(count%iInterval == 0)
				{
					//写进度信息
					pCmd->resultNum = 1;
					PCMDRESULT pResult = new CMDRESULT;
					pResult->resultType = CDRT_PROGRESS;
					pResult->resultLen = 1;
					pResult->pData = new char[2];
					pResult->pData[1] = '\0';

					if(fileLen > 0)
					{
						iProgress = startPos/fileLen;
					}
					else
					{
						iProgress += 1;
					}
					iProgress = (iProgress>100)?0:iProgress;
					pResult->pData[0] = iProgress;
					pCmd->resultList.push_back(pResult);
					memMgr.WriteTaskResultToMem(*pCmd);

					//清除结果
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();
				}//end if(count%iInterval == 0)

			}//end if(pCmd->_state._state == CmdState::CS_STEP4) //第四步

		} //end while

		if(fileType == 2)   //标记事件
		{
			fileCtrl.CloseFile();
		}

		//写执行结果
		pCmd->resultNum = 2;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 1;      //执行成功
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);

		pResult = new CMDRESULT;
		pResult->resultType = CDRT_FILEPATH;
		pResult->resultLen = sLocalFileName.size();
		pResult->pData = new char[pResult->resultLen+1];
		memset(pResult->pData,0,pResult->resultLen+1);
		memcpy(pResult->pData,sLocalFileName.c_str(),pResult->resultLen);
		pCmd->resultList.push_back(pResult);

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::AutoSend( )");
		return true;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol60_103::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::exuteCmd( )");

	int index = GetPro2CmdIndex(cmd);   //获取命令对应的映射索引
	if( index < 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command in Protocol60_103::ExecuteCmd()");
		return false;
	}

	if( pro2CmdMap[index].exuteCmd > 0)
	{
		strcpy(cmd.describe,pro2CmdMap[index].cmdName);
		return pro2CmdMap[index].exuteCmd(this,pLinker,cmd); //执行命令函数
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'InitConnect(Linker *)'
bool Protocol60_103::InitConnect( Linker * pLinker )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol60_103::InitConnect( )");

	int msgLen = 0; //消息长度
	int sendDataLen = 0;	//发送的数据长度

	FIXFRAME fixFrame;	//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	/****************C_RCU_NA_3(复位通信单元)-->*********/
	//发送C_RCU_NA_3（复位通信单元）
	auto_ptr<string> pQuest( CreateMsg(C_RCU_NA_3,0,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送回答帧
		if(sendDataLen != msgLen)
		{
			mylog.WriteLog2File(LogControl::LERROR, "Send C_RCU_NA_3 error in Protocol60_103::InitConnect( )");
			pLinker->Close();
			return false;
		}
	}
	else
	{
		mylog.WriteLog2File(LogControl::LERROR, "Create msg error in Protocol60_103::InitConnect( )");
		pLinker->Close();
		return false;
	}

	//期待采集装置发送M_CON_NA_3(确认，ACD=1)
	if(WantConfirm(pLinker,NULL) != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "Wait for M_CON_NA_3 faile in Protocol60_103::InitConnect( )");
		return false;
	}

	//请求1级用户数据
	if(!SendLevelOne(pLinker,0))
	{
		mylog.WriteLog2File(LogControl::LERROR, "SendLevelOne() return error in Protocol60_103::InitConnect( )");
		return false;
	}

	//期待M_IRC_NA_3（ASDU5，COT=复位通信单元）
	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//保存可变帧数据
	PVFRAMEHEAD pVFrameHead = NULL;	//帧头
	PASDUID pASDUID = NULL;		//103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	unsigned char parmLen =0;

	int framePacketLen=0;   //帧实际的长度

	memset(&framePacket[0],0,sizeof(framePacket));

	if(false==AnalyseVolatileFrame(pLinker,&framePacket[0],
		&framePacketLen,
		&pVFrameHead,
		&pASDUID,
		&pASDUInfId,
		&pAsduData,
		&AsduDataLen,
		NULL,
		false))
	{
		mylog.WriteLog2File(LogControl::LERROR, "get volatile frame error in Protocol60_103::InitConnect( )");
		return false;
	}

	//是否是期待的命令
	if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_5)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU5,but it isn't in Protocol60_103::InitConnect( )");
		return false;
	}

	//判断传送原因是否是复位通信单元（COT=0x04）
	if(pASDUID->COT != 0X04) //不是复位通信单元
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The command isn't the response for M_IRC_NA_3 in Protocol60_103::InitConnect( ).");
		return false;
	}

	/******************复位帧计数位-->*****************/

	//发送C_RFB_NA_3（复位帧计数位）
	string *pTemp = pQuest.release();
	if(pTemp != NULL)
	{
		delete pTemp;
		pTemp = NULL;
	}
	pQuest = auto_ptr<string>( CreateMsg(C_RFB_NA_3,0,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送回答帧
		if(sendDataLen != msgLen)
		{
			mylog.WriteLog2File(LogControl::LERROR, "send C_RFB_NA_3 error in Protocol60_103::InitConnect( )");
			pLinker->Close();
			return false;
		}
	}
	else
	{
		mylog.WriteLog2File(LogControl::LERROR, "Create msg error in Protocol60_103::InitConnect( )");
		pLinker->Close();
		return false;
	}

	//期待采集装置发送M_CON_NA_3 （确认，ACD=1）
	if(WantConfirm(pLinker,NULL) != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "Wait for M_CON_NA_3 faile in Protocol60_103::InitConnect( )");
		return false;
	}

	//请求1级用户数据
	if(!SendLevelOne(pLinker,0))
	{
		mylog.WriteLog2File(LogControl::LERROR, "SendLevelOne return error in Protocol60_103::InitConnect( )");
		return false;
	}

	//期待M_IRF_NA_3（ASDU5，COT=复位帧计数位）
	memset(&framePacket[0],0,sizeof(framePacket));  //保存可变帧数据
	pVFrameHead = NULL;	//帧头
	pASDUID = NULL;		//103协议应用服务数据单元标识符
	pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

	*pAsduData=NULL;
	AsduDataLen=0;
	parmLen =0;

	framePacketLen=0;   //帧实际的长度

	if(false==AnalyseVolatileFrame(pLinker,&framePacket[0],
		&framePacketLen,
		&pVFrameHead,
		&pASDUID,
		&pASDUInfId,
		&pAsduData,
		&AsduDataLen,
		NULL,
		false))
	{
		mylog.WriteLog2File(LogControl::LERROR, "get volatile frame error in Protocol60_103::InitConnect( )");
		return false;
	}

	//是否是期待的命令
	if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_5)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU5,but it isn't in Protocol60_103::InitConnect( )");
		return false;
	}

	//判断传送原因是否是复位帧计数位（COT=0x03）
	if(pASDUID->COT != 0X03) //不是复位帧计数位
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The command isn't the response for M_IRF_NA_3 in Protocol60_103::InitConnect( ).");
		return false;
	}

	mylog.WriteLog2File(LogControl::LINFO,"Leave Protocol60_103::InitConnect( )");
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetUdpData(Linker *,struct Command&)'
bool Protocol60_103::GetUdpData( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	const int PACKET_LEN = 64*1024;	//包长度

	char framePacket[PACKET_LEN] = {0};	//保存可变帧数据
	int framePacketLen=0;   //帧实际的长度
	memset(&framePacket[0],0,sizeof(framePacket));

	struct sockaddr clientAddr;
	memset(&clientAddr,0,sizeof(struct sockaddr));

	framePacketLen = pLinker->ReceiveDataFrom(framePacket,PACKET_LEN,2000,clientAddr);

	if( framePacketLen <= 0)
		return false;

	PUDPDATAPARM pUdpDataParm = new UDPDATAPARM;
	pUdpDataParm->pProto = this;
	pUdpDataParm->newAddrin = new struct sockaddr_in;
	::memcpy(pUdpDataParm->newAddrin,&clientAddr,sizeof(struct sockaddr_in) );

	pUdpDataParm->datalen = framePacketLen;
	pUdpDataParm->pData = new char[framePacketLen];
	memset(pUdpDataParm->pData,0,framePacketLen);
	memcpy(pUdpDataParm->pData,framePacket,framePacketLen);

	HANDLE hDelUdpDataTread = CreateThread(NULL,0,&DelUdpDataThread,pUdpDataParm,0,NULL);
	::CloseHandle(hDelUdpDataTread);

	return false;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
bool Protocol60_103::StopRecWave( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StopRecWave( )");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;

	}
	else //命令来自于UI、定时器等
	{
		int msgLen = 0; //消息长度
		int sendDataLen = 0;	//发送的数据长度

		unsigned char asdu10_attr[4]={0};//属性结构
		asdu10_attr[0] = 2;	//故障录波
		asdu10_attr[1] = 2;	//设定调试状态
		asdu10_attr[2] = 0;
		asdu10_attr[3] = 1;	//返回运行状态的时间(1分钟)

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X1101,0X67,23,4,1,4,&asdu10_attr[0])};

		int iResult = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iResult == 1)
		{
			return true;
		}
		else if(iResult == 0)
		{
			return true;
		}
		else if(iResult == -1)
		{
			mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::StopRecWave( )");
			pLinker->Close();
			return true;
		}

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::StopRecWave( )");
	return true;
}

int Protocol60_103::SendAsdu10WithExeWrite( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber)
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::SendAsdu10WithExeWrite()");

	int msgLen=0;
	int sendDataLen =0;
	FIXFRAME fixFrame;	//固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));
	int iReturn = 0;

	if(pCmd->_state._state == CmdState::CS_STEP1) //第一步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetGIAttrData:send asdu10.");
		auto_ptr<string> pCmdAsdu10(CreateGIMsg(C_GD_NA_3,pCmd->destDevId,0X2A,CINF_250,asdu10_GIs,asdu10_GINumber,&msgLen));
		if( pCmdAsdu10.get() != NULL)
		{
			sendDataLen = SendData(pLinker,pCmdAsdu10->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送asdu10带执行的写的报文 
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LERROR, "send C_GD_NA_3 error in Protocol60_103::SendAsdu10WithExeWrite()");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"send C_GD_NA_3 error");
				return -1;
			}
		}
		else
		{
			m_errCode = PEC_UNKOWN;
			mylog.WriteLog2File(LogControl::LERROR, "create C_GD_NA_3 msg error in Protocol60_103::SendAsdu10WithExeWrite()");
			return -1;
		}

		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP2) //第二步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetGIAttrData:wait for M_CON_NA_3.");
		//期待采集装置发送M_CON_NA_3(确认帧)
		iReturn = WantConfirm(pLinker,pCmd);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"No get M_CON_NA_3 in Protocol60_103::SendAsdu10WithExeWrite().");
			return iReturn;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP3) //第三步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetGIAttrData:send C_PL1_NA_3.");
		//请求1级用户数据
		if(!SendLevelOne(pLinker,pCmd->destDevId))
		{
			mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::SendAsdu10WithExeWrite().");
			return -1;
		}
		pCmd->_state.ToNextState();
	}

	//期待Asdu10带执行写确认
	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};//可变帧数据
	PVFRAMEHEAD pVFrameHead = NULL;	//帧头
	PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;	//帧实际的长度

	if(pCmd->_state._state == CmdState::CS_STEP4) //第四步
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetGIAttrData:wait for asdu10 with answer.");

		//取前台的回答帧
		int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
			&framePacketLen,
			&pVFrameHead,
			&pASDUID,
			&pASDUInfId,
			&pAsduData,
			&AsduDataLen,
			NULL,
			SystemCfger::GetInstance().timeOut);

		if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
		{
			mylog.WriteLog2File(LogControl::LERROR,"Wait for asdu10 with answer error in Protocol60_103::SendAsdu10WithExeWrite().");
			pLinker->Close();
			return -1;
		}
		if( iReturn == 3) //中断帧
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Get an interrupt frame in Protocol60_103::SendAsdu10WithExeWrite().");
			return 0;
		}

		//是否是期待的命令
		if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_10)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU10,but it isn't in Protocol60_103::SendAsdu10WithExeWrite()");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU10,but it isn't");
			return -1;
		}

		//判断返回
		if(pASDUID->COT != 0X28) //否定的确认
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"The answer is negative in Protocol60_103::SendAsdu10WithExeWrite().");

			m_errCode = PEC_ERRCMD_NODATA;
			pCmd->resultNum = 1;
			PCMDRESULT pComdResult = new CMDRESULT;
			pComdResult->resultType = CDRT_TRUFALSE;	//返回的结果是真假类型
			pComdResult->resultLen = 1;					//结果长度1字节
			pComdResult->pData = new char[2];
			pComdResult->pData[0] = 0;					//false
			pComdResult->pData[1] = '\0';
			pCmd->resultList.push_back(pComdResult);
			return 0;
		}

		pCmd->_state.ToNextState();
	}

	return 1;
}

bool Protocol60_103::GetContData( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetContData()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		//分析命令参数
		if((pCmd->parmLen != 1+4+4+4+12+24+sizeof(unsigned short) && 
			pCmd->parmLen != 1+4+4+1+12+24 + sizeof(DataControl::DataQue*))
			|| pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		int uTimeOut = 0;//超时时间
		int iOldWaitTime = SystemCfger::GetInstance().timeOut;

		unsigned int uiStartTime = 0;	//开始时间(time_t类型)
		memcpy(&uiStartTime,&pCmd->pParm[1],4);

		DataControl::DataQue *pDataQue = NULL;
		if(pCmd->from == CD_REMOTE || pCmd->to == CD_REMOTE)
		{
			memcpy(&pDataQue,&pCmd->pParm[1+4+4+4+12+24],sizeof(DataControl::DataQue*));
		}
		else
		{
			memcpy(&uTimeOut,&pCmd->pParm[1+4+4+4+12+24],sizeof(unsigned short));
			uTimeOut = uTimeOut * 1000;
		}

		if( uTimeOut== 0 || uTimeOut > 240*1000)
		{
			uTimeOut = iOldWaitTime;
		}

		unsigned char uSendParmLen = (unsigned char)(pCmd->parmLen - sizeof(unsigned short));

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X4001,0X67,23,uSendParmLen,1,uSendParmLen,(unsigned char*)pCmd->pParm)};

		//ASDU21（控制方向）―― 读返回的文件名
		GI asdu21_gi[1] ={GI(0X4002,0X67,1,100,1,0,NULL)};

# if defined(_MSC_VER)  &&  (_MSC_VER >= 1400)//VC 2005
		struct tm tmStartTime = *_localtime32((const __time32_t *)&uiStartTime);
#else
		struct tm tmStartTime = *localtime((time_t*)&uiStartTime);
#endif

		int iYearMonth = (tmStartTime.tm_year+1900)*100 + tmStartTime.tm_mon+1;
		char szYearMonth[7]={0};
		::itoa(iYearMonth,szYearMonth,10);

		if(pCmd->from == CD_REMOTE || pCmd->to == CD_REMOTE)
		{
			//来自远程的命令，数据写入内存中
			return GetGeneraFile(pLinker,pCmd,pDataQue,asdu10_gi,1,asdu21_gi,1);
		}
		else
		{
			SystemCfger::GetInstance().timeOut = uTimeOut;
			bool rt = GetGeneraFile(pLinker,pCmd,NULL,0,FileControl::FT_CONT,szYearMonth,asdu10_gi,1,asdu21_gi,1);
			SystemCfger::GetInstance().timeOut = iOldWaitTime;
			return rt;
		}
	}

}

bool Protocol60_103::GetGeneraFile( Linker * pLinker,struct Command *pCmd,char *FilePartName,int fileTotalLen,FileControl::FILETYPE fileControlType,char *YearMonth, GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber,char IsNeedCompress)
{
	FileControl fileCtrl;

	//获得文件存放目录
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
	string filePath = fileCtrl.GetFileTypeDirector(devId,fileControlType);

	//加上年月目录(配置相关不创建年月目录)
	if(YearMonth != NULL)
	{
		filePath = filePath + string(YearMonth) + string("/");
		if(!fileCtrl.IsExist(filePath.c_str()))
		{
			_mkdir(filePath.c_str());
		}
	}

	char fileName[5][100+1]={{0},{0},{0},{0},{0}};	//文件名称
	char *ppFileName[5] = {&fileName[0][0],&fileName[1][0],&fileName[2][0],&fileName[3][0],&fileName[4][0]};
	int  fileNameLen[5][1] = {{100},{100},{100},{100},{100}};
	int *pFileNameLen[5] = {&fileNameLen[0][0],&fileNameLen[1][0],&fileNameLen[2][0],&fileNameLen[3][0],&fileNameLen[4][0]};
	int iAttrNum = 5;

	int msgLen = 0; //消息长度
	int sendDataLen = 0;	//发送的数据长度

	string	sDownInfFile="";//保存下载信息的临时文件（保存断点续传信息）

	if(FilePartName != NULL)
		sDownInfFile = filePath + string(FilePartName)+string(".down");

	//保存中断环境
	struct tagEnVirionment
	{
		char FinishFileName[400+1];//下载完成的文件名称
		unsigned int startPos;
		char fileName[100+1];//当前处理文件名称
		char IsNeedCompress;//是否要处理压缩
	};
	struct tagEnVirionment *pEV = NULL;

	unsigned int startPos = 0;	//文件起始传输位置
	FileControl::OPENMODE openMode = FileControl::OM_WRITE;

	if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetGeneraFile");
		pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
		startPos = pEV->startPos;
		openMode = FileControl::OM_APPEND;
	}
	else
	{
		pEV = new struct tagEnVirionment;
		memset(pEV,0,sizeof(struct tagEnVirionment));

		//检查保存下载信息的临时文件是否存在
		if( fileCtrl.IsExist( sDownInfFile.c_str() ) )
		{
			//读取下载信息
			fileCtrl.OpenFile(sDownInfFile.c_str(),FileControl::OM_READ);
			fileCtrl.ReadData((char *)pEV,sizeof(struct tagEnVirionment));
			fileCtrl.CloseFile();

			IsNeedCompress = pEV->IsNeedCompress;
			startPos = pEV->startPos ;
			IsNeedCompress = pEV->IsNeedCompress;
		}
		else
		{
			pEV->IsNeedCompress = IsNeedCompress;
		}
	}

	//获取通用分类服务返回的文件名称
	int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_GIs,asdu10_GINumber,asdu21_GIs,asdu21_GINumber,ppFileName,pFileNameLen,&iAttrNum);
	if(iReturn != 1 || iAttrNum <= 0)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return faile in Protocol60_103::GetGeneraFile()");
		if(iReturn == -1)
		{
			pLinker->Close();
		}
		return false;
	}

	MemMapMgr memMgr;
	PCMDRESULT pResult = NULL;
	vector< PCMDRESULT > fileList;//保存接收到的文件列表

	char szInf[512] = {0};
	bool bHasError = false;

	//循环调所有文件
	for(int i=0;i<iAttrNum && !bHasError ; i++)
	{
		sprintf(szInf,"Begin get file : %s in Protocol60_103::GetGeneraFile()",&fileName[i][0]);
		mylog.WriteLog2File(LogControl::LDEBUG,szInf);

		if( ::strstr( pEV->FinishFileName,&fileName[i][0]) != NULL)
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"The file has be download.");
			continue;
		}
		else if( ::strstr( pEV->fileName,&fileName[i][0]) != NULL)
		{
			//此文件已下载过
			startPos = pEV->startPos ;
			openMode = FileControl::OM_APPEND;
			mylog.WriteLog2File(LogControl::LDEBUG,"The file has be download but not complete.");
		}
		else
		{
			startPos = 0;
			openMode = FileControl::OM_WRITE;
			pEV->startPos = startPos;
		}
		
		string writeFileName;   //保存数据的文件（包括路径）
		string sSourceFileName;	//原文件名
		string sZipFileName;	//压缩的文件名

		if(FilePartName == NULL)
		{
			string tmpFile(&fileName[i][0]);
			string::size_type pos = tmpFile.find_last_of("/");
			if(pos != string::npos)
			{
				sSourceFileName = filePath + tmpFile.substr(pos+1,tmpFile.size() - pos -1);
			}
			else
			{
				if(tmpFile == "")
				{
					sSourceFileName = filePath + string("tmp.file");
				}
				else
				{
					sSourceFileName = filePath + tmpFile;
				}
			}
		}
		else
		{
			sSourceFileName = filePath + string(FilePartName);
		}

		//要处理压缩文件
		if( IsNeedCompress == 1)
		{
			sZipFileName = sSourceFileName + string(".SHZip");
			writeFileName = sZipFileName;
		}
		else
		{
			writeFileName = sSourceFileName;
		}

		if(!fileCtrl.OpenFile(writeFileName.c_str(),openMode))
		{
			m_errCode = PEC_UNKOWN;
			string sInfo = string("open the file: ") +string(&fileName[0][0]) +string(" fail in Protocol60_103::GetGeneraFile()");
			mylog.WriteLog2File(LogControl::LERROR,sInfo.c_str());
			//mylog.WriteLog(LogControl::LERROR,pCmd,"Open file failed");
			pLinker->Close();
			bHasError = true;
			break;
			//return false;
		}

		int iProgress = 0;

		if(pCmd->_state._state == CmdState::CS_STEP9) //第九步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetGeneraFile:pregetfile");

			pEV->startPos = startPos;
			memcpy(pEV->fileName,&fileName[i][0],sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//准备获取文件前的处理
			if( !PreGetFile(pLinker,pCmd,2,&fileName[i][0], 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return faile in Protocol60_103::GetGeneraFile()");
				pLinker->Close();
				bHasError = true;
				break;
				//return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP10) //第10步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetGeneraFile:wait for M_CON_NA_3");
			//期待采集装置发送M_CON_NA_3(确认帧)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"Wait for M_CON_NA_3 faile in Protocol60_103::GetGeneraFile()");
				bHasError = true;
				break;
				//return false;
			}
			pCmd->_state.ToNextState();
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;
		int count = 0;
		int maxCount = fileTotalLen/readMaxLen; //最大循环次数
		int iInterval = maxCount/20;    //间隔多少次循环之后写一次进度信息(全过程最多写20次进度信息)
		iInterval = (iInterval <= 0)?30:iInterval;

		while( !bHasError )
		{
			count++;

			if(pCmd->_state._state == CmdState::CS_STEP11) //第11步
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 11 of GetGeneraFile:send C_PL1_NA_3");
				//请求1级用户数据(如果数据没有发送完，则从请求1级用户数据开始循环)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LWARNING,"Send C_PL1_NA_3 error in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}
				pCmd->_state.ToNextState();
			}

			//期待采集装置发送ASDU104命令
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //保存可变帧数据
			PVFRAMEHEAD pVFrameHead = NULL;	//帧头
			PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
			PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;   //帧实际的长度

			if(pCmd->_state._state == CmdState::CS_STEP12) //第12步
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 12 of GetGeneraFile:wait for ASDU104");
				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LWARNING,"Wait for ASDU104 fail in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					pEV->startPos = startPos; //保存中断时的传输位置
					bHasError = true;
					break;
					//return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetGeneraFile has be interrupted in Protocol60_103::GetGeneraFile()");

					m_errCode = PEC_INTERRUPT;
					pEV->startPos = startPos; //保存中断时的传输位置
					bHasError = true;
					break;
					//return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::GetGeneraFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}

				//解析ASDU数据
				if(pAsduData == NULL || AsduDataLen <= 0)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"error ASDU data in Protocol60_103::GetGeneraFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"error ASDU data");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}

				memcpy(&startPos,&pAsduData[0],4);
				int writeLen = AsduDataLen - 4;

				fileCtrl.Seek(startPos);
				fileCtrl.WriteData(&pAsduData[4],writeLen);
				startPos += writeLen;

				if(pVFrameHead->control.m.acd == false) //传输完毕
				{
					fileCtrl.CloseFile();

					//要处理压缩文件
					if( IsNeedCompress == 1)
					{
						sZipFileName = sSourceFileName + string(".SHZip");
						writeFileName = sZipFileName;

						//解压文件
						sprintf(szInf,
							"Begin uncompress the file :%s to the file :%s in Protocol60_103::GetRecWaveFile()",
							sZipFileName.c_str(),sSourceFileName.c_str());
						mylog.WriteLog2File(LogControl::LDEBUG,szInf);

						if(!fileCtrl.UnCompressFile(sZipFileName.c_str(),sSourceFileName.c_str()))
						{
							sprintf(szInf,
								"Uncompress the file :%s to the file :%s fail in Protocol60_103::GetRecWaveFile()",
								sZipFileName.c_str(),sSourceFileName.c_str());
							mylog.WriteLog2File(LogControl::LERROR,szInf);

							m_errCode = PEC_OPERAT_FILE_FAIL;
							pLinker->Close();
							bHasError = true;
							break;
							//return false;
						}
						DeleteFile(sZipFileName.c_str());
						mylog.WriteLog2File(LogControl::LDEBUG,"Uncompress the file success");
					}

					if( i <= 3)
					{
						::strcat(pEV->FinishFileName,&fileName[i][0]);
					}
					memset(pEV->fileName,0,sizeof(pEV->fileName));

					//成功
					if( i<iAttrNum )//还有文件要召唤
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_STEP9);
					}
					else
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					}
					break;
				}
				else
				{
					pEV->startPos = startPos; //保存中断时的传输位置
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

				//每传iInterval次写一次进度信息
				if(pCmd->from == CD_UI && count%iInterval == 0)
				{
					pCmd->resultNum = 1;
					PCMDRESULT pResult = new CMDRESULT;
					pResult->resultType = CDRT_PROGRESS;
					pResult->resultLen = 1;
					pResult->pData = new char[2];
					pResult->pData[1] = '\0';

					if(fileTotalLen == 0)
					{
						//文件长度未知,则给出虚假的进度信息
						if(iProgress < 95)
						{
							iProgress +=1;
						}
					}
					else
					{
						iProgress = 100*startPos/fileTotalLen;  //计算进度
					}
					iProgress = (iProgress>100)?100:iProgress;
					pResult->pData[0] = iProgress;
					pCmd->resultList.push_back(pResult);
					memMgr.WriteTaskResultToMem(*pCmd);

					//清除结果
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();

				}//end if(pCmd->from == CD_UI && count%iInterval == 0)

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //第12步

		} //end while

		fileCtrl.CloseFile();

		//保存文件路径名
		pResult = new CMDRESULT;
		pResult->resultType = CDRT_FILEPATH;
		pResult->resultLen = writeFileName.size();
		pResult->pData = new char[writeFileName.size()+1];
		pResult->pData[writeFileName.size()] = 0 ;
		memcpy(pResult->pData,writeFileName.c_str(),writeFileName.size());

		fileList.push_back(pResult);
	}//end for() //循环调所有文件

	if(bHasError)//失败
	{
		fileCtrl.OpenFile(sDownInfFile.c_str(),FileControl::OM_WRITE);
		fileCtrl.WriteData((char *)pEV,sizeof(struct tagEnVirionment));
		fileCtrl.CloseFile();

		vector< PCMDRESULT >::iterator it = fileList.begin();
		for(;it != fileList.end();it++)
		{
			PCMDRESULT pTmpResult = (*it);
			delete pTmpResult;
			pTmpResult = NULL;
		}
		fileList.clear();

		pCmd->resultNum = 0;
		pCmd->resultList.clear();
		return false;
	}
	else//成功
	{
		//清除下载信息文件
		string sCmd = string("del ");
		sCmd.append(sDownInfFile);
		while(1)
		{
			string::size_type pos = sCmd.find("/");
			if(pos == string::npos)
				break;
			sCmd = sCmd.replace(pos,1,"\\");
		}
		::system(sCmd.c_str());
	}

	pCmd->resultNum = 1 + fileList.size();
	pResult = new CMDRESULT;
	pResult->resultType = CDRT_TRUFALSE;
	pResult->resultLen = 1;
	pResult->pData = new char[2];
	pResult->pData[0] = 1 ; //执行成功
	pResult->pData[1] = 0 ;

	pCmd->resultList.insert(pCmd->resultList.begin(), fileList.begin(),fileList.end());
	pCmd->resultList.push_back(pResult);

	return true;
}

bool Protocol60_103::GetGeneraFile( Linker * pLinker,struct Command *pCmd,DataControl::DataQue *pDataQue,GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber)
{
	char fileName[5][100+1]={{0},{0},{0},{0},{0}};   //文件名称
	char *ppFileName[5] = {&fileName[0][0],&fileName[1][0],&fileName[2][0],&fileName[3][0],&fileName[4][0]};
	int  fileNameLen[5] = {100,100,100,100,100};
	int *pFileNameLen[5] = {&fileNameLen[0],&fileNameLen[1],&fileNameLen[2],&fileNameLen[3],&fileNameLen[4]};
	int iAttrNum =5;

	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	//保存中断环境
	struct tagEnVirionment
	{
		unsigned int startPos;
		char fileName[100+1];	//文件名称
		int  fileNameLen;
	};

	unsigned int startPos = 0;	//文件起始传输位置

	if(pCmd->_state.pStoreParm != NULL)	//任务曾经被执行过
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetGeneraFile");

		struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
		startPos = pEV->startPos;
		fileNameLen[0] = pEV->fileNameLen;
		memcpy(&fileName[0][0],pEV->fileName,sizeof(pEV->fileName));
	}

	//获取通用分类服务返回的文件名称
	int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_GIs,asdu10_GINumber,asdu21_GIs,asdu21_GINumber,ppFileName,pFileNameLen,&iAttrNum);
	if(iReturn != 1 || iAttrNum <= 0)
	{
		mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::GetGeneraFile()");
		if(iReturn == -1)
		{
			pLinker->Close();
		}
		return false;
	}

	char szInf[256] = {0};

	//循环调用所有的文件
	for(int i=0;i < iAttrNum; i++)
	{
		sprintf(szInf,"Begin get file : %s in Protocol60_103::GetGeneraFile()",&fileName[i][0]);
		mylog.WriteLog2File(LogControl::LDEBUG,szInf);

		startPos = 0;

		if(pCmd->_state._state == CmdState::CS_STEP9) //第九步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetGeneraFile:pregetfile");

			struct tagEnVirionment *pEV = new struct tagEnVirionment;
			pEV->fileNameLen = fileNameLen[0];
			pEV->startPos = startPos;
			memcpy(pEV->fileName,&fileName[0][0],sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//准备获取文件前的处理
			if( !PreGetFile(pLinker,pCmd,2,&fileName[i][0], 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in Protocol60_103::GetGeneraFile()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP10) //第10步
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetGeneraFile:wait for M_CON_NA_3");
			//期待采集装置发送M_CON_NA_3(确认帧)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"Wait for M_CON_NA_3 fail in Protocol60_103::GetGeneraFile()");
				return false;
			}
			pCmd->_state.ToNextState();
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP11) //第11步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 11 of GetGeneraFile:send C_PL1_NA_3");
				//请求1级用户数据(如果数据没有发送完，则从请求1级用户数据开始循环)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//期待采集装置发送ASDU104命令
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //保存可变帧数据
			PVFRAMEHEAD pVFrameHead = NULL;	//帧头
			PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
			PASDUINFID pASDUInfId = NULL;	//103协议应用服务数据单元信息体标识符

			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;   //帧实际的长度

			if(pCmd->_state._state == CmdState::CS_STEP12) //第12步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 12 of GetGeneraFile:wait for ASDU104");
				//取前台的回答帧
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //不是可变帧也不是中断帧
				{
					mylog.WriteLog2File(LogControl::LWARNING,"Wait for ASDU104 fail in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //中断帧
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetGeneraFile has be interrupted");

					m_errCode = PEC_INTERRUPT;
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置
					return false;
				}

				//是否是期待的命令
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::GetGeneraFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//解析ASDU数据
				if(pAsduData == NULL || AsduDataLen <= 0)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"error ASDU data in Protocol60_103::GetGeneraFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"error ASDU data");
					pLinker->Close();
					return false;
				}

				memcpy(&startPos,&pAsduData[0],4);
				int writeLen = AsduDataLen - 4;

				bool bIsStop =false;
				pDataQue->push(&pAsduData[4],writeLen,&bIsStop);
				startPos += writeLen; 

				if(pDataQue->IsCancle())
				{
					pLinker->Close();
					return true;
				}

				if(pVFrameHead->control.m.acd == false) //传输完毕
				{
					//成功
					pDataQue->Cancle();
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
				else
				{
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //保存中断时的传输位置
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //第12步

		} //end while

	}//end for() //循环调用所有的文件

	return true;
}

bool Protocol60_103::ReadZeroOffset( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::ReadZeroOffset( )");

#ifdef _PILING

	//测试桩
	return true;
#endif

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//分析命令参数
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char dataType = pCmd->pParm[0];//数据类型

		unsigned char asdu10_attr[1]={0};  //属性结构
		asdu10_attr[0] = dataType;

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X5001,0X67,3,1,1,1,&asdu10_attr[0])};

		//发送ASDU10带执行的写
		int iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::ReadZeroOffset( )");
			result = false;
		}

		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = result;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);

		pCmd->resultNum = 1;

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::ReadZeroOffset( )");
	return result ;
}

bool Protocol60_103::StartGnExperiment( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartGnExperiment( )");

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//分析命令参数
		if(pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char asdu10_attr[PRO103_ASDUDATA_MAXSIZE +1] = {0}; //属性结构
		memcpy(asdu10_attr, pCmd->pParm, pCmd->parmLen-4);

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0XE001,0X67,23,(unsigned char)pCmd->parmLen,1,pCmd->parmLen,&asdu10_attr[0])};

		//发送ASDU10带执行的写
		int iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::ReadZeroOffset( )");
			result = false;
		}

		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = result;
		pResult->pData[1] = '\0';
		pCmd->resultList.push_back(pResult);

		pCmd->resultNum = 1;

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::StartGnExperiment( )");
	return result ;
}

bool Protocol60_103::StopGnExperiment( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StopGnExperiment( )");

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		int iWaitTime = 13*1000;	//超时时间
		int iOldWaitTime = SystemCfger::GetInstance().timeOut;
		if(pCmd->parmLen == sizeof(unsigned int) )
		{
			memcpy(&iWaitTime,pCmd->pParm,sizeof(int));
			if(iWaitTime <= 0 || iWaitTime > 1800 )
			{
				iWaitTime = iOldWaitTime;
			}
			{
				iWaitTime = iWaitTime * 1000;
			}
		}

		unsigned char temp[1] = {0};
		GI asdu10_gi[1] ={GI(0XE101,0X67,3,1,1,1,temp)};

		//ASDU21（控制方向）―― 读文件名
		GI asdu21_gi[1] ={GI(0XE102,0X67,23,12,1,0,NULL)};

		//获取通用分类服务返回的属性结构数据
		char fileName[100] = {0};
		char	*pfile[1] = {(char*)&fileName};
		int  fileNameLen = 100;
		int *pDataLen[1] = {&fileNameLen};
//		memset(&timeData,0,fileNameLen);
		int iAttrNum = 1;

		unsigned char RII = 0;
		SystemCfger::GetInstance().timeOut = iWaitTime;	//重设超时时间
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,pfile,pDataLen,&iAttrNum,&RII);
		SystemCfger::GetInstance().timeOut = iOldWaitTime;	//恢复超时时间
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::StopGnExperiment()");

			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//写结果
		pCmd->resultNum = 1;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_FILENAME;
		pResult->resultLen = 100;
		pResult->pData = new char[100];
		memcpy(pResult->pData,fileName,pResult->resultLen);
		pCmd->resultList.push_back(pResult);

	} //end if( pCmd->from == CD_REMOTE )

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::StopGnExperiment()");
	return result;
}

bool Protocol60_103::GetDeviceState( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetDeviceState( )");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		int msgLen = 0; //消息长度
		int sendDataLen = 0;//发送的数据长度

		//分析参数
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetDeviceState()");
			return false;
		}

		unsigned char type = pCmd->pParm[0];//类型

		unsigned char asdu10_attr[1]={0};  //属性结构
		asdu10_attr[0] = type; //类型

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X1001,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21(控制方向)--读返回的自检信息
		GI asdu21_gi[1] ={GI(0X1002,0X67,3,1,1,0,NULL)};

		char state[5] = {0};
		char *ppState[1] = {&state[0]};
		int  iDataLen = 5;
		int *pDataLen[1] = {&iDataLen};
		int iAttrNum = 1;

		PCMDRESULT pCmdResult = new CMDRESULT;

		//获取通用分类服务返回的属性结构数据
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppState,pDataLen,&iAttrNum);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::GetDeviceState()");

			//返回失败结果
			pCmdResult->resultType = CDRT_TRUFALSE;
			pCmdResult->resultLen = 1;
			pCmdResult->pData = new char[2];
			pCmdResult->pData[0] = 0;   //false
			pCmdResult->pData[1] = '\0';

			pCmd->resultNum = 1;
			pCmd->resultList.push_back(pCmdResult);

			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//将自检信息放入结果中
		pCmdResult->resultType = CDRT_CHAR;
		pCmdResult->resultLen = iDataLen;
		pCmdResult->pData = new char[pCmdResult->resultLen+1];
		memcpy(pCmdResult->pData,&state[0],pCmdResult->resultLen);
		pCmdResult->pData[pCmdResult->resultLen] = '\0';

		pCmd->resultNum = 1;
		pCmd->resultList.push_back(pCmdResult);

		mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetDeviceState()");
		return true;

	} //end if( pCmd->from == CD_REMOTE )
}

bool Protocol60_103::SetDeviceState( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::SetDeviceState( )");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		int msgLen = 0; //消息长度
		int sendDataLen = 0;    //发送的数据长度

		//分析命令参数
		if(pCmd->parmLen != 1+1+2 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetDeviceState()");
			return false;
		}

		unsigned char asdu10_attr[4]={0};  //属性结构
		asdu10_attr[0] = pCmd->pParm[0];//类型
		asdu10_attr[1] = pCmd->pParm[1];//设定状态
		memcpy(&asdu10_attr[2],&pCmd->pParm[2],2);

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X1101,0X67,23,4,1,4,&asdu10_attr[0])};

		int iResult = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iResult == 1)
		{
			return true;
		}
		else if(iResult == 0)
		{
			return true;
		}
		else if(iResult == -1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"SendAsdu10WithExeWrite return fail in Protocol60_103::SetDeviceState()");
			pLinker->Close();
			return true;
		}

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::SetDeviceState( )");
	return true;
}

bool Protocol60_103::OperateDevice( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::OperateDevice( )");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //命令来自于UI、定时器等
	{
		int msgLen = 0; //消息长度
		int sendDataLen = 0;	//发送的数据长度

		//分析命令参数
		if(pCmd->parmLen != 1+4 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::OperateDevice()");
			return false;
		}

		unsigned char asdu10_attr[5]={0};  //属性结构
		asdu10_attr[0] = pCmd->pParm[0];//类型
		memcpy(&asdu10_attr[1],&pCmd->pParm[1],4);//类型参数

		switch(asdu10_attr[0])
		{
		case 1:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_QUY_DEVSTAT,"（告警复归）"));
			break;
		case 2:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_START_REVERT,"（启动复归）"));
			break;
		case 3:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_DEV_CHECK_SELF,"（设备自检）"));
			break;
		default:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_UNKOWN_OPER,"（未知的操作）"));
			break;
		}

		//asdu10带执行的写的报文
		GI asdu10_gi[1] ={GI(0X9001,0X67,23,5,1,5,&asdu10_attr[0])};

		int iResult = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_gi,1);
		if(iResult == 1)
		{
			return true;
		}
		else if(iResult == 0)
		{
			return true;
		}
		else if(iResult == -1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"SendAsdu10WithExeWrite return fail in Protocol60_103::OperateDevice()");
			pLinker->Close();
			return false;
		}

	} //end if( pCmd->from == CD_REMOTE )

	//pLinker->Close();
	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::OperateDevice( )");
	return true;
}

bool Protocol60_103::GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetSplitRecWaveFile( )");

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//分析命令参数
		if(pCmd->parmLen != 100+1+24*sizeof(unsigned int) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetSplitRecWaveFile()");
			return false;
		}

		unsigned char temp[1] = {0};
		GI asdu10_gi[1] ={GI(0XE201,0X67,23,(unsigned char)pCmd->parmLen,1,pCmd->parmLen,(unsigned char*)pCmd->pParm)};

		//ASDU21（控制方向）―― 读文件名
		GI asdu21_gi[1] ={GI(0XE202,0X67,23,100,1,0,NULL)};

		//获取通用分类服务返回的属性结构数据
		char fileName[100] = {0};
		char	*pfile[1] = {(char*)&fileName};
		int  fileNameLen = 100;
		int *pDataLen[1] = {&fileNameLen};
//		memset(&timeData,0,fileNameLen);
		int iAttrNum = 1;

		unsigned char RII = 0;
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,pfile,pDataLen,&iAttrNum,&RII);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::GetSplitRecWaveFile()");

			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//写结果
		pCmd->resultNum = 1;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_FILENAME;
		pResult->resultLen = 100;
		pResult->pData = new char[100];
		memcpy(pResult->pData,fileName,pResult->resultLen);
		pCmd->resultList.push_back(pResult);

		char szLogInf[256] ={0};
		sprintf(szLogInf,"Get the splite file name:%s in Protocol60_103::GetSplitRecWaveFile()",fileName);
		mylog.WriteLog2File(LogControl::LDEBUG, szLogInf);

	} //end if( pCmd->from == CD_REMOTE )

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol60_103::GetSplitRecWaveFile()");
	return result;
}

void Protocol60_103::GetCmdDesc(struct Command & cmd)
{
	int index = GetPro2CmdIndex(cmd);   //获取命令对应的映射索引
	if( index < 0 )
	{
		return;
	}

	strcpy(cmd.describe,pro2CmdMap[index].cmdName);	
}

string Protocol60_103::GetCmdDesc(unsigned char funCode)
{
	int i = 0;
	int mapCount = (sizeof(pro2CmdMap)/sizeof(PRO2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro2CmdMap[i].cmdFunCode == funCode )
		{
			break;
		}
	}

	if( i < mapCount)
	{
		return string(pro2CmdMap[i].cmdName);
	}
	else
	{
		return string("");
	}
}

DWORD WINAPI DelUdpDataThread(void *pParm)
{
	mylog.WriteLog2File(LogControl::LINFO,"Get UDP data with Protocol60_103");

	Protocol60_103::PUDPDATAPARM pUdpDataParm = (Protocol60_103::PUDPDATAPARM)pParm;

	PVFRAMEHEAD pVFrameHead = NULL;	//帧头
	PASDUID pASDUID = NULL;	//103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;//信息体标识符
	PVFRAMEEND pVFrameEnd = NULL;
	unsigned char checksum; //校验和

	char *pAsduData=NULL;
	int  AsduDataLen=0;

	//分析客户端的IP地址，从而得到发送UDP信息的DAU编号
	char ipAddr[16]={0};
	memcpy(ipAddr,inet_ntoa(pUdpDataParm->newAddrin->sin_addr),strlen(inet_ntoa(pUdpDataParm->newAddrin->sin_addr)));
	//PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr,1);
	PDAUINFO pDauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr);
	struct Command cmd;
	memset(&cmd,0,sizeof(cmd));

	if(pDauInfo != NULL)
	{
		cmd.destDevId = pDauInfo->DAUId;
	}
	else
	{
		//pDauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr,2);
		pDauInfo = SystemCfger::GetInstance().GetDAUInfo(ipAddr);
		if(pDauInfo != NULL)
		{
			cmd.destDevId = pDauInfo->DAUId;
		}
	}

	cmd.from = CD_ASDU;
	cmd.msgId = -1;
	cmd.resultNum = 0 ;
	
	int vFrameHeadLen = sizeof(VFRAMEHEAD); //帧头长度
	int ASDUIdLen   = sizeof(ASDUID);	//ASDU id 长度
	int ASDUInfLen   = sizeof(ASDUINFID);	//ASDU 信息体 长度

	pVFrameHead =(PVFRAMEHEAD) &pUdpDataParm->pData[0];
	pASDUID = (PASDUID)&pUdpDataParm->pData[sizeof(VFRAMEHEAD)];
	pASDUInfId = (PASDUINFID)&pUdpDataParm->pData[sizeof(VFRAMEHEAD) + sizeof(ASDUID)];

	while(1)
	{

		//校验帧的有效性
		//两个启动字符是否相等
		if(pVFrameHead->HEAD.startChar1 != pVFrameHead->HEAD.startChar2 
			|| pVFrameHead->HEAD.startChar1 != PRO103_VOLATILEFRAME_STARTCHAR)
		{
			//不是asdu帧
			mylog.WriteLog2File(LogControl::LWARNING,"It isn't asdu frame in DelUdpDataThread");
			break;
		}

		AsduDataLen = (pVFrameHead->HEAD.lengthLow + ((pVFrameHead->HEAD.lengthHigh & 0x0F) <<8))
			- vFrameHeadLen + sizeof(VFRAMEHEAD::tagHead) - ASDUIdLen - ASDUInfLen;
		if(AsduDataLen<0)
		{
			mylog.WriteLog2File(LogControl::LERROR,"error packet length in DelUdpDataThread()");
			break;
		}

		pAsduData = &pUdpDataParm->pData[vFrameHeadLen + ASDUIdLen + ASDUInfLen];
		pVFrameEnd = (PVFRAMEEND)&pUdpDataParm->pData[vFrameHeadLen + ASDUIdLen + ASDUInfLen + AsduDataLen];

		//结束字符是否正确
		if(pVFrameEnd->endChar != PRO103_ENDCHAR)
		{
			mylog.WriteLog2File(LogControl::LERROR,"The endchar is error in DelUdpDataThread()");
			break;
		}

		//校验和
		if(false == pUdpDataParm->pProto->doCheck(pUdpDataParm->pData,pUdpDataParm->datalen,false,&checksum))
		{
			mylog.WriteLog2File(LogControl::LERROR,"get an error when computer the checksum in DelUdpDataThread()");
			break;
		}
		if(checksum != pVFrameEnd->CS)
		{
			mylog.WriteLog2File(LogControl::LERROR,"The checksum is error in DelUdpDataThread()");
			break;
		}

		//是否是ASDU10包
		if(pVFrameHead->control.m.fun != ASDU_8 ||  pASDUID->TYP != 0X0A)
		{
			mylog.WriteLog2File(LogControl::LERROR,"The udp packet isn't the ASDU10 in DelUdpDataThread()");
			break;
		}

#ifdef _PILING
	Task *pTask = NULL;
#endif
		unsigned char type = 0;
		unsigned short gin = 0;

		Protocol60_103::PSHQDMESSAGE pSHQDMessage = NULL;

		unsigned char unItemNum = pAsduData[1];
		pAsduData += 2;

		for(int i=0;i < unItemNum; i++)
		{
			memcpy(&gin,pAsduData,2);
			pAsduData += 2;
			pAsduData += 1;

			GDD gdd;
			memcpy(&gdd,pAsduData,3);
			pAsduData += 3;

			PCMDRESULT pResult = new CMDRESULT;

			switch(gin)
			{
			case 0XD001:	//故障录波启动信息
				mylog.WriteLog2File(LogControl::LDEBUG,"Get starting info in DelUdpDataThread()");
				cmd.funCode = CMD_STARTINFO;
				cmd.resultNum += 1 ;
				pResult->resultType = CDRT_STARTRECMSG;
				pResult->resultLen = gdd.dataLen * gdd.dataNum -1;
				pResult->pData = new char[pResult->resultLen+1];
				memset(pResult->pData,0,pResult->resultLen+1);
				memcpy(pResult->pData,&pAsduData[1],pResult->resultLen);
				pAsduData += pResult->resultLen + 1;

				cmd.resultList.push_back(pResult);
				break;

			case 0XD002://运行信息(日志)与标志事件
				type = pAsduData[0];
				if( type == 1)//标志事件
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Get mark event info  in DelUdpDataThread()");
					cmd.funCode = CMD_AUTOSEND_MARKEVENT;
					pResult->resultType = CDRT_MARKEVENT;
				}
				else //运行信息
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Get UDP with the log of running in DelUdpDataThread()");
					cmd.funCode = CMD_RUNINFO;
					pResult->resultType = CDRT_CHAR;
				}

				cmd.resultNum += 1 ;
				pResult->resultLen = gdd.dataLen * gdd.dataNum -1;
				pResult->pData = new char[pResult->resultLen+1];
				memset(pResult->pData,0,pResult->resultLen+1);
				memcpy(pResult->pData,&pAsduData[1],pResult->resultLen);
				pAsduData += pResult->resultLen +1;

				cmd.resultList.push_back(pResult);
				break;
			default:
				mylog.WriteLog2File(LogControl::LDEBUG,"Get UDP with unkown type info in DelUdpDataThread()");
				cmd.funCode = 0xff; //unkown function
				pAsduData += gdd.dataLen * gdd.dataNum;
				delete pResult;
				pResult  = NULL;
				continue;
			}

		} //end for(int i=0;i < unItemNum; i++)

		break;
	}//end while(1)

	if(cmd.resultNum > 0)
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Write udp info into memery in DelUdpDataThread()");
		MemMapMgr memMgr;
		memMgr.WriteTaskResultToMem(cmd);
	}

	delete pUdpDataParm->pData;
	pUdpDataParm->pData = NULL;
	delete pUdpDataParm->newAddrin;
	pUdpDataParm->newAddrin = NULL;
	delete pUdpDataParm;
	pUdpDataParm = NULL;

	mylog.WriteLog2File(LogControl::LINFO,"Leave DelUdpDataThread()");
	return 0;
}

// }}}USR
// }}}RME

// }}}RME
