// {{{RME classifier 'Logical View::Protocol103'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol103.h>
#include <FileControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/***********************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：Protocol103.cpp
* 文件标识：
* 摘    要：解析103协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
**************************************************************/

#include <string>
#include <vector>
#include <memory>

#include "Linker.h"
#include "Task.h"

#include "Protocol103_struct.h"

using namespace std;

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
Protocol103::Protocol103( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol103::~Protocol103( void )
{
}
// }}}RME

// {{{RME operation 'Decode(Linker*)'
Task * Protocol103::Decode( Linker * pLinker )
{
	// {{{USR
	assert(pLinker != NULL);

	unsigned char startChar=0;
	int  readLen=0;

	//先获取启动字符
	readLen = ReceiveData(pLinker,(char *)&startChar,1,50);
	if(readLen != 1)
	{
		//mylog.WriteLog2File(LogControl::LDEBUG,"no task to be receive");
		return NULL;
	}

	switch(startChar)
	{
	case PRO103_VOLATILEFRAME_STARTCHAR: //可变长帧命令
		return DecodeVolatileFrame(pLinker);
	case PRO103_FIXFRAME_STARTCHAR: //固定长帧命令
		return DecodeFixFrame(pLinker);
	default:
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LINFO,"unknow command");
		return NULL;
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'DecodeVolatileFrame(Linker*)'
Task * Protocol103::DecodeVolatileFrame( Linker * pLinker )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol103::DecodeVolatileFrame( )");

	assert(pLinker != NULL);

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //保存可变帧数据
	PVFRAMEHEAD pVFrameHead = NULL;                 //帧头
	PASDUID pASDUID = NULL;      //103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;   //103协议应用服务数据单元信息体标识符
	PVFRAMEEND pVFrameEnd = NULL;    //帧尾
	char *pAsduData=NULL;
	int  AsduDataLen=0;

	int framePacketLen=0;   //帧实际的长度

	if(false==AnalyseVolatileFrame(pLinker,&framePacket[0],
		&framePacketLen,
		&pVFrameHead,
		&pASDUID,
		&pASDUInfId,
		&pAsduData,
		&AsduDataLen,
		&pVFrameEnd,
		true,
		1000))
	{
		return NULL;
	}

	//获得协议包和命令的映射索引
	int index = -1;
	if(pASDUID->TYP == 0X0A) //ASDU10要进行特殊处理
	{
		if(AsduDataLen < 4)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"The asdu data is too short");
			return NULL;
		}

		unsigned short gin = 0;
		memcpy(&gin,&pAsduData[2],2);   //获取gin

		index = GetPro2CmdIndex(pVFrameHead->control.c.prm,pASDUID->TYP,pASDUID->COT,gin,false);
	}
	else
	{
		index = GetPro2CmdIndex(pVFrameHead->control.c.prm,pASDUID->TYP,0,0,false);
	}

	if(index < 0 )
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command");
		return NULL;
	}

	//构建任务
	Task *pTask = new Task();
	pTask->Linker = pLinker;
	pTask->cmd.destDevId = pVFrameHead->address;
	pTask->cmd.from = CD_REMOTE;
	pTask->cmd.to = CD_NEURON;
	pTask->cmd.funCode = pro2CmdMap[index].cmdFunCode;

	pTask->cmd.parmLen = (unsigned short)AsduDataLen;
	if(pTask->cmd.parmLen > 0)
	{
		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);
		memcpy(pTask->cmd.pParm,pAsduData,pTask->cmd.parmLen);
	}

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol103::DecodeVolatileFrame( )");
	return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'DecodeFixFrame(Linker*)'
Task * Protocol103::DecodeFixFrame( Linker * pLinker )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol103::DecodeFixFrame( )");

	assert(pLinker != NULL);

	FIXFRAME fixFrame;
	memset(&fixFrame,0,sizeof(fixFrame));
	fixFrame.startChar = PRO103_FIXFRAME_STARTCHAR;

	//获得固定长的帧
	if(false == AnalyseFixFrame(pLinker,&fixFrame,true,1000) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "DecodeFixFrame fail");
		return NULL;
	}

	//获得协议包和命令的映射索引
	int index = GetPro2CmdIndex(fixFrame.control.c.prm,fixFrame.control.c.fun,0,0,true);
	if(index < 0 )
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command");
		return NULL;
	}

	//构建任务
	Task *pTask = new Task();
	pTask->Linker = pLinker;
	pTask->cmd.destDevId = fixFrame.address ;
	pTask->cmd.from = CD_REMOTE;
	pTask->cmd.to = CD_NEURON;
	pTask->cmd.funCode = pro2CmdMap[index].cmdFunCode;
	pTask->cmd.parmLen = 0;

	mylog.WriteLog2File(LogControl::LINFO, "Leave Protocol103::DecodeFixFrame( )");
	return pTask;

	// }}}USR
}
// }}}RME

// {{{RME operation 'doCheck(char*,int,bool,unsigned char *)'
bool Protocol103::doCheck( char * packet, int packetlen, bool isFixFrame, unsigned char * result )
{
	// {{{USR
	assert(packet != NULL);
	assert(result != NULL);

	unsigned int i;
	unsigned char check=0;
	if(true == isFixFrame)
	{
		if(packetlen!=sizeof(FIXFRAME))
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"the packetlen don't equal the sizeof the fix frame struct length.");
			return false;
		}

		for(i=1;i<3;i++)
		{
			check += (unsigned char)packet[i];
		}
	}
	else
	{
		if( (unsigned int)packetlen < sizeof(VFRAMEHEAD) + sizeof(ASDUID) +sizeof(ASDUINFID) + sizeof(VFRAMEEND) )
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"the packetlen too short.");
			return false;
		}

		//除去包头VFRAMEHEAD::tagHead和包尾VFRAMEEND，计算包数据和
		for(i=sizeof(VFRAMEHEAD::tagHead);i<packetlen - sizeof(VFRAMEEND) ;i++)
		{
			check += (unsigned char)packet[i];
		}
	}

	*result = check;
	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendInterruptFrame(Linker *,struct Command *)'
bool Protocol103::SendInterruptFrame( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	assert(pLinker != NULL);
	assert(pCmd != NULL);

	string addr = pLinker->GetConnectAddr();
	vector< PREPORTHEAD > *pReportList = SystemCfger::GetInstance().GetReportList(addr);
	if(pReportList != NULL && pReportList->size()>0 )
	{
		mylog.WriteLog2File(LogControl::LINFO, "Has new fault reports need be sent to remote");

		int msgLen = 0;
		auto_ptr<string> pCmdAsdu1(CreateMsg(M_TTM_TA_3,pCmd->destDevId,true,0X01,MINF_84,NULL,0,&msgLen));
		if(pCmdAsdu1.get() != NULL)
		{
			int sendDataLen = SendData(pLinker,pCmdAsdu1->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送中断帧 
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LINFO, "Send interrupt frame failed");
				return false;
			}

			pCmd->_state.ChangeStateTo(CmdState::CS_PAUSE);

			//构建获取简报任务
			Task *pTask = new Task();
			pTask->Linker = pLinker;
			pTask->cmd.destDevId = pCmd->destDevId;
			pTask->cmd.from = CD_REMOTE;
			pTask->cmd.to = CD_NEURON;
			pTask->cmd.funCode = CMD_GETREPORTFILE;
			pTask->cmd.msgId = -1;
			pTask->cmd._state.preState = CmdState::CS_STEP2;

			if(!QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME)->SendMsg(pTask))
			{
				delete pTask;
				pTask = NULL;
				return false;
			}

			return true;
		}
	}

	return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFrame(Linker *,struct Command *,PFIXFRAME,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,unsigned int)'
int Protocol103::GetFrame( Linker * pLinker, struct Command * pCmd, PFIXFRAME pFixFrame, char * vFramePacket, int * vFrameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, unsigned int uiTimeOut )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LDETAIL, "Enter Protocol103::GetFrame()");

	assert(pLinker != NULL);

	unsigned char startChar=0;
	int  readLen=0;

	//先获取启动字符
	readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
	if(readLen != 1)
	{
		mylog.WriteLog2File(LogControl::LERROR,"Get start char error in Protocol103::GetFrame().");
		pLinker->Close();
		return -1;  //失败
	}

	switch(startChar)
	{
	case PRO103_VOLATILEFRAME_STARTCHAR: //可变长帧命令
		mylog.WriteLog2File(LogControl::LDETAIL, "Begin get volatile frame");

		if(vFramePacket == NULL || vFrameLen == NULL || pAsduDataLen == NULL
			|| pOutVFrameHead == NULL || pOutASDUID == NULL)
		{
			mylog.WriteLog2File(LogControl::LERROR,"Error parameter in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //失败
		}

		if(AnalyseVolatileFrame(pLinker,vFramePacket,vFrameLen,
			pOutVFrameHead,
			pOutASDUID,
			pOutASDUInfID,
			pOutAsduData,
			pAsduDataLen,
			pOutVFrameEnd,
			true,uiTimeOut))
		{
			if((*pOutVFrameHead)->control.c.fun == ASDU_8 && (*pOutASDUID)->TYP == 1 && (*pOutASDUID)->COT == 1)
			{
				if(pCmd != NULL)
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_PAUSE);

					//构建获取简报任务
					Task *pTask = new Task();
					pTask->Linker = pLinker;
					pTask->cmd.destDevId = pCmd->destDevId;
					pTask->cmd.from = CD_ASDU;
					pTask->cmd.to = CD_NEURON;
					pTask->cmd.funCode = CMD_GETREPORTFILE;
					pTask->cmd.msgId = -1;

					if(!QueRegister::GetInstance().RegisterQue(COMM_ASDUQUE_NAME)->SendMsg(pTask))
					{
						delete pTask;
						pTask = NULL;
						return -1;
					}
				}

				mylog.WriteLog2File(LogControl::LDEBUG, "Get an interrupted frame");
				return 3;   //故障中断帧
			}
			else
			{
				return 2;   //返回普通可变帧
			}
		}
		else
		{
			mylog.WriteLog2File(LogControl::LERROR,"AnalyseVolatileFrame() return error in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //失败
		}
	case PRO103_FIXFRAME_STARTCHAR: //固定长帧命令
		mylog.WriteLog2File(LogControl::LDETAIL, "Begin get fix frame");

		if(pFixFrame == NULL)
		{
			FIXFRAME fixFrame;
			memset(&fixFrame,0,sizeof(FIXFRAME));
			fixFrame.startChar = startChar;

			if(AnalyseFixFrame(pLinker,&fixFrame,true,uiTimeOut))
			{
				m_errCode = PEC_ERRCMD_NODATA;
				return 1;  //无数据
			}
			else
			{
				mylog.WriteLog2File(LogControl::LERROR,"AnalyseFixFrame() return error in Protocol103::GetFrame().");
				pLinker->Close();
				return -1;  //失败
			}
		}//end if(pFixFrame == NULL)

		if(AnalyseFixFrame(pLinker,pFixFrame,true,uiTimeOut))
		{
			return 1;   //返回固定帧
		}
		else
		{
			mylog.WriteLog2File(LogControl::LERROR,"AnalyseFixFrame() return error in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //失败
		}
	default:
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"unknow frame type");
		pLinker->Close();
		return -1;  //失败
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseFixFrame(Linker *,PFIXFRAME,bool,unsigned int)'
bool Protocol103::AnalyseFixFrame( Linker * pLinker, PFIXFRAME pFixFrame, bool hasReadStartChar, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pLinker != NULL);
	assert(pFixFrame != NULL);

	pFixFrame->startChar = PRO103_FIXFRAME_STARTCHAR;
	char *pFrame = (char*)pFixFrame;

	int  readLen=0;
	unsigned char checksum; //校验和
	unsigned char startChar=0;  //启动字符

	if( hasReadStartChar == false ) //还没有读取启动字符，则先读取
	{
		//先获取启动字符
		readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
		if(readLen != 1)
		{
			mylog.WriteLog2File(LogControl::LERROR,"receive data error");
			return false;
		}

		if( startChar != PRO103_FIXFRAME_STARTCHAR) //不是期待的固定长帧
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want a fix frame,but the frame isn't");
			return false;
		}
	}

	pFixFrame->startChar = PRO103_FIXFRAME_STARTCHAR;

	//读出整个帧的数据
	readLen = ReceiveData(pLinker,&pFrame[1],sizeof(FIXFRAME) -1,uiTimeOut );
	if(readLen != sizeof(FIXFRAME) -1 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"read fix frame head error when using protocol 103");
		return false;
	}

	//打印帧信息
	if(SystemCfger::GetInstance().logLevel >= 5)
	{
		char szInf[250] = {0};
		sprintf(szInf,"Get a fix frame -->\r StartChar : %X\r FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r",
			pFixFrame->startChar,pFixFrame->control.m.fun,pFixFrame->address,pFixFrame->CS,pFixFrame->endChar);
		mylog.WriteLog2File(LogControl::LDETAIL, szInf);
	}

	//校验帧的有效性

	//结束字符是否正确
	if(pFixFrame->endChar != PRO103_ENDCHAR)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The endchar is error");
		return false;
	}

	//校验和
	if(false == doCheck(pFrame,sizeof(FIXFRAME),true,&checksum))
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"get an error when computer the checksum");
		return false;
	}

	if(checksum != pFixFrame->CS )
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The checksum is error");
		return false;
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseVolatileFrame(Linker *,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,bool,unsigned int)'
bool Protocol103::AnalyseVolatileFrame( Linker * pLinker, char * framePacket, int * frameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, bool hasReadStartChar, unsigned int uiTimeOut )
{
	// {{{USR
	assert(pLinker != NULL);
	assert(framePacket != NULL);
	assert(frameLen != NULL);
	assert(pAsduDataLen != NULL);

	framePacket[0] = PRO103_VOLATILEFRAME_STARTCHAR;

	PVFRAMEHEAD pVFrameHead =(PVFRAMEHEAD) &framePacket[0];                 //帧头
	PASDUID pASDUID = (PASDUID)&framePacket[sizeof(VFRAMEHEAD)];      //ASDU id
	PASDUINFID pASDUInfID = (PASDUINFID)&framePacket[sizeof(VFRAMEHEAD) + sizeof(ASDUID)];        //ASDU 信息体

	int  readLen=0;
	unsigned char checksum; //校验和
	unsigned char startChar=0;  //启动字符

	if( hasReadStartChar == false ) //还没有读取启动字符，则先读取
	{
		//先获取启动字符
		readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
		if(readLen != 1)
		{
			mylog.WriteLog2File(LogControl::LERROR,"receive data error");
			return false;
		}

		if( startChar != PRO103_VOLATILEFRAME_STARTCHAR) //不是期待的可变长帧
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want a volatile frame,but the frame isn't");
			return false;
		}
	}

	framePacket[0] = PRO103_VOLATILEFRAME_STARTCHAR;

	int vFrameHeadLen = sizeof(VFRAMEHEAD); //帧头长度
	int ASDUIdLen   = sizeof(ASDUID);   //ASDU id 长度
	int ASDUInfLen   = sizeof(ASDUINFID);   //ASDU 信息体 长度
	int vFrameEndLen = sizeof(VFRAMEEND);  //帧尾长度

	//先读帧头和ASDU id的数据
	readLen = ReceiveData(pLinker,&framePacket[1],vFrameHeadLen + ASDUIdLen + -1,uiTimeOut );
	if(readLen != vFrameHeadLen + ASDUIdLen + -1 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"read volatile frame head error when using protocol 103");
		return false;
	}

	int AsduDataLen = 0;//ASDU信息元素的大小

	if(pASDUID->TYP == ASDU_14 || pASDUID->TYP == ASDU_16 || pASDUID->TYP == ASDU_104) //没有信息体表识符
	{
		pASDUInfID = NULL;
		ASDUInfLen = 0;

		//计算ASDU信息元素的大小(长度-控制域-地址-应用服务数据单元头)
		AsduDataLen = (pVFrameHead->HEAD.lengthLow + ((pVFrameHead->HEAD.lengthHigh & 0x0F) <<8))
			- vFrameHeadLen + sizeof(VFRAMEHEAD::tagHead) - ASDUIdLen;
	}
	else
	{
		//读ASDU 信息体
		readLen = ReceiveData(pLinker,&framePacket[vFrameHeadLen + ASDUIdLen],ASDUInfLen,uiTimeOut);
		if(readLen != ASDUInfLen )
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"read volatile frame head error when using protocol 103");
			return false;
		}

		//计算ASDU信息元素的大小(长度-控制域-地址-应用服务数据单元头)
		AsduDataLen = (pVFrameHead->HEAD.lengthLow + ((pVFrameHead->HEAD.lengthHigh & 0x0F) <<8)) 
			- vFrameHeadLen + sizeof(VFRAMEHEAD::tagHead) - ASDUIdLen - ASDUInfLen;
	}

	if(AsduDataLen<0)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"error packet length");
		return false;
	}

	*pAsduDataLen = AsduDataLen;

	//获取ASDU信息元素及103协议可变帧帧尾
	readLen = ReceiveData(pLinker,&framePacket[vFrameHeadLen + ASDUIdLen + ASDUInfLen], AsduDataLen+vFrameEndLen,uiTimeOut );
	if(readLen != AsduDataLen+vFrameEndLen )
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"read volatile frame's data and tail error when using protocol 103");
		return false;
	}

	char *pAsduData = &framePacket[vFrameHeadLen + ASDUIdLen + ASDUInfLen];
	PVFRAMEEND pVFrameEnd = (PVFRAMEEND)&framePacket[vFrameHeadLen + ASDUIdLen + ASDUInfLen + AsduDataLen];

	*frameLen = vFrameHeadLen + ASDUIdLen + ASDUInfLen + AsduDataLen + vFrameEndLen;

	//打印帧信息
	if(SystemCfger::GetInstance().logLevel >= 5)
	{
		char szInf[1000] = {0};
		sprintf(szInf,"Get a Volatile frame -->\r StartChar1 : %X\r LowLen : %X\r HighLen %X\r StartChar2 : %X\r \
					  FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r TYP : %X\r VSQ : %X\r COT : %X\r ASDU_ADDR : %X\r",
					  pVFrameHead->HEAD.startChar1,pVFrameHead->HEAD.lengthLow,pVFrameHead->HEAD.lengthHigh,
					  pVFrameHead->HEAD.startChar2,pVFrameHead->control.m.fun,pVFrameHead->address,pVFrameEnd->CS,
					  pVFrameEnd->endChar,pASDUID->TYP,( pASDUID->VSQ + (pASDUID->S<<7) ),pASDUID->COT,pASDUID->COMADDR);
		if(pASDUInfID != NULL)
		{
			sprintf(szInf,"%s FUN : %X\r INF :%X\r ",szInf,pASDUInfID->FUN,pASDUInfID->INF);
		}

		mylog.WriteLog2File(LogControl::LDETAIL, szInf);
	}

	//校验帧的有效性

	//两个启动字符是否相等
	if(pVFrameHead->HEAD.startChar1 != pVFrameHead->HEAD.startChar2)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The startchar1 don't equals startchar2");
		return false;
	}

	//结束字符是否正确
	if(pVFrameEnd->endChar != PRO103_ENDCHAR)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The endchar is error");
		return false;
	}

	//校验和
	int packetLen = vFrameHeadLen + ASDUIdLen + ASDUInfLen + AsduDataLen + vFrameEndLen;   //包总长度

	if(false == doCheck(&framePacket[0],packetLen,false,&checksum))
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"get an error when computer the checksum");
		return false;
	}

	if(checksum != pVFrameEnd->CS)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The checksum is error");
		return false;
	}

	if(pOutVFrameHead != NULL)
		*pOutVFrameHead = pVFrameHead;
	if(pOutASDUID != NULL)
		*pOutASDUID = pASDUID;
	if(pOutASDUInfID != NULL)
		*pOutASDUInfID = pASDUInfID;
	if(pOutAsduData != NULL)
		*pOutAsduData = pAsduData;
	if(pOutVFrameEnd != NULL)
		*pOutVFrameEnd = pVFrameEnd;

	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'SendConfirm(Linker*,unsigned int,bool)'
bool Protocol103::SendConfirm( Linker * pLinker, unsigned int devId, bool acd )
{
	// {{{USR
	//回答确认帧
	int msgLen = 0;
	auto_ptr<string> pAswer( CreateMsg(M_CON_NA_3, devId,true,0,0,NULL,0,&msgLen) );
	if( pAswer->c_str() != NULL)
	{
		int sendDataLen = SendData(pLinker,pAswer->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送回答帧 
		if(sendDataLen != msgLen)
		{
			mylog.WriteLog2File(LogControl::LERROR, "send M_CON_NA_3 error");
			return false;
		}
	}
	else
	{
		m_errCode = PEC_UNKOWN;
		mylog.WriteLog2File(LogControl::LERROR,"create M_CON_NA_3 msg error");
		return false;
	}
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'WantConfirm(Linker*,struct Command *)'
int Protocol103::WantConfirm( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));
	unsigned int startPos = 0;      //文件起始传输位置

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //保存可变帧数据
	PVFRAMEHEAD pVFrameHead = NULL;                 //帧头
	PASDUID pASDUID = NULL;      //103协议应用服务数据单元标识符
	PASDUINFID pASDUInfId = NULL;   //103协议应用服务数据单元信息体标识符
	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;   //帧实际的长度

	//取前台的回答帧
	int iReturn = GetFrame(pLinker,pCmd,&fixFrame,framePacket,
		&framePacketLen,
		&pVFrameHead,
		&pASDUID,
		&pASDUInfId,
		&pAsduData,
		&AsduDataLen,
		NULL,
		SystemCfger::GetInstance().timeOut);

	if( iReturn != 1 && iReturn != 3) //不是固定帧也不是中断帧
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the frame M_CON_NA_3,but get a volatile frame");
		pLinker->Close();
		return -1;
	}
	if( iReturn == 3) //中断帧
	{
		return 0;
	}

	//是否是期待的命令
	if( fixFrame.control.m.fun  != MFUNID_0 && fixFrame.control.m.fun  != MFUNID_9)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command M_CON_NA_3,but it isn't");
		pLinker->Close();
		return -1;
	}

	if(fixFrame.control.m.fun  != MFUNID_9)//返回的不是M_NV_NA_3
	{
		return 1;
	}
	else
	{
		m_errCode = PEC_ERRCMD_NODATA;
		return 0;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendLevelOne(Linker*,unsigned int)'
bool Protocol103::SendLevelOne( Linker * pLinker, unsigned int devId )
{
	// {{{USR
	//请求1级用户数据
	int msgLen = 0;
	auto_ptr<string> pQuest( CreateMsg(C_PL1_NA_3,devId,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		int sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送回答帧 
		if(sendDataLen != msgLen)
		{
			mylog.WriteLog2File(LogControl::LERROR, "send C_PL1_NA_3 error");
			return false;
		}
	}
	else
	{
		m_errCode = PEC_UNKOWN;
		mylog.WriteLog2File(LogControl::LERROR, "create C_PL1_NA_3 msg error");
		return false;
	}
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'WantLevelOne(Linker*)'
bool Protocol103::WantLevelOne( Linker * pLinker )
{
	// {{{USR
	//期待C_PL1_NA_3(召唤1级数据)
	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	if(false == AnalyseFixFrame(pLinker,&fixFrame,false,SystemCfger::GetInstance().timeOut) ) //获得固定长的帧
	{
		mylog.WriteLog2File(LogControl::LERROR,"get C_PL1_NA_3 frame error");
		return false;
	}

	//是否是期待的命令
	if( fixFrame.control.c.fun != CFUNID_10 )
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command C_PL1_NA_3,but it isn't");
		return false;
	}
	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetPro2CmdIndex(unsigned char,unsigned char,unsigned char,unsigned short,bool)'
int Protocol103::GetPro2CmdIndex( unsigned char direct, unsigned char func, unsigned char cot, unsigned short gin, bool isFixFrame )
{
	// {{{USR
	int i;
	int mapCount = (sizeof(pro2CmdMap)/sizeof(PRO2CMD));
	if(true == isFixFrame)
	{
		//固定帧根据“消息方向”及“控制域功能码”为条件检索映射
		for(i=0;i<mapCount;i++)
		{
			if(pro2CmdMap[i].direct == direct &&
				pro2CmdMap[i].funid == func)
			{
				return i;
			}
		}
	}
	else
	{
		//可变帧根据“消息方向”、“应用服务单元类型标识”、“传送原因”及“通用分类标识序号”为条件检索映射
		for(i=0;i<mapCount;i++)
		{
			if(pro2CmdMap[i].direct == direct &&
				pro2CmdMap[i].type == func &&
				pro2CmdMap[i].COT == cot &&
				pro2CmdMap[i].GIN == gin )
			{
				return i;
			}
		}
	}

	m_errCode = PEC_ERRCMD_NODATA;
	return -1;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
bool Protocol103::GetRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//召唤录波文件
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol103::GetRecWaveFile( )");

	//分析命令参数
	if(pCmd->parmLen != 44 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::GetRecWaveFile()");
		return false;
	}

	char pFileName[41] = {0};
	memcpy(pFileName,pCmd->pParm,40);    //不含扩展名的文件名
	unsigned int   startPos = 0;                    //起始传输位置
	unsigned int   preStartPos = 7;                 //前一帧的起始传输位置
	int            FCB = -1;

	unsigned int   cfgFileLen = 0;                //配置文件长度
	unsigned int   hdrFileLen = 0;                //head文件长度
	unsigned int   datFileLen = 0;                //data文件长度
	unsigned int   cfgAndHdrLen = 0;              //hdr和cfg文件长度
	unsigned int   sendDataTotalLen = 0;          //发送数据总长度
	memcpy(&startPos,&pCmd->pParm[40],4);

	FileControl fileCtrl;
	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	int  readLen=0;

	// modify by kh
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
	string partFileName(pFileName);
	string comtradeDir = fileCtrl.GetFileDirector(devId,partFileName.append(".cfg").c_str(),FileControl::FT_COMTRADE,NULL);
	string cfgFileName = comtradeDir + string(pFileName) + string(".cfg");
	string headFileName = comtradeDir + string(pFileName) + string(".HDR");
	string dateFileName = comtradeDir + string(pFileName) + string(".DAT");

	bool             bFileIsExit=false;  //文件是否存在
	auto_ptr<string> pHdrAndCfgData ;   //保存三个文件头及HDR和CFG这两个文件的数据

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) 
	{
		//命令来自于远程，则将录波文件发送到远程
		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run step 1 of GetRecWaveFile:send M_CON_NA_3");
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG, "Has fault report and pause GetRecWaveFile");
				//有新故障，暂停此任务
				return false;
			}
			//回答确认帧
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Resum running GetRecWaveFile");
			startPos = *((int*)(pCmd->_state.pStoreParm));
		}
		else
		{
			unsigned int *pStartPos = new unsigned int;
			*pStartPos = 0;
			pCmd->_state.pStoreParm = (void*)pStartPos;
		}

		if( fileCtrl.IsExist( cfgFileName.c_str() ))
		{
			bFileIsExit = true;
		}

		//先将HDR、CFG文件的数据读出
		if( bFileIsExit)
		{
			cfgFileLen = fileCtrl.GetFileLen(cfgFileName.c_str());                //配置文件长度
			hdrFileLen = fileCtrl.GetFileLen(headFileName.c_str());;                //head文件长度
			datFileLen = fileCtrl.GetFileLen(dateFileName.c_str());;                //data文件长度
			sendDataTotalLen = cfgFileLen +hdrFileLen +datFileLen +40 +7 + 3*(1+4);          //发送数据总长度
			cfgAndHdrLen = hdrFileLen + cfgFileLen +(40 +7 +3*(1+4) );

			//保存三个文件头及HDR和CFG这两个文件的数据
			string *pTemp = pHdrAndCfgData.release();
			if(pTemp != NULL)
			{
				delete pTemp;
				pTemp = NULL;
			}
			pHdrAndCfgData = auto_ptr<string>(new string(""));
			pHdrAndCfgData->resize(cfgAndHdrLen,'\0');

			char *pData = &pHdrAndCfgData->operator[](0);

			struct CP56Time2a cpTime;
			memset(&cpTime,0,sizeof(struct CP56Time2a));
			fileCtrl.GetRecWaveFaultTime(cfgFileName.c_str(),&cpTime);    //获取录波故障时间

			memcpy(pData,pFileName,40);      //发送文件名
			memcpy(&pData[40],&cpTime,7);      //发送故障时间
			pData[47] = 1;          //hdr文件类型
			memcpy(&pData[48],(void*)&hdrFileLen,4);          //hdr文件长度

			//先读取head文件数据
			if(false == fileCtrl.OpenFile(headFileName.c_str(),FileControl::OM_READ))
			{
				m_errCode = PEC_UNKOWN;
				string info = string(headFileName.c_str()) + string(" open failed");
				mylog.WriteLog2File(LogControl::LERROR,info.c_str());
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Open HDR file failed");
				pLinker->Close();
				return false;
			}

			pData += 40 + 7 + (1+4);
			int factReadLen = fileCtrl.ReadData(0,pData,hdrFileLen);
			if( factReadLen != hdrFileLen)
			{
				m_errCode = PEC_UNKOWN;
				mylog.WriteLog2File(LogControl::LERROR,"read hdr file fail");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Read HDR file failed");
				fileCtrl.CloseFile();
				pLinker->Close();
				return false;
			}

			fileCtrl.CloseFile();
			pData += hdrFileLen;

			//CFG数据
			*pData = 2; //cfg文件类型
			pData++;
			memcpy(pData,(void*)&cfgFileLen,4);//cfg文件长度
			pData += 4;

			if(false == fileCtrl.OpenFile(cfgFileName.c_str(),FileControl::OM_READ))
			{
				m_errCode = PEC_UNKOWN;
				string info = string(cfgFileName.c_str()) + string(" open failed");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Open CFG file failed");
				mylog.WriteLog2File(LogControl::LERROR,info.c_str());
				pLinker->Close();
				return false;
			}

			factReadLen = fileCtrl.ReadData(0,pData,cfgFileLen);
			if( factReadLen != cfgFileLen)
			{
				m_errCode = PEC_UNKOWN;
				mylog.WriteLog2File(LogControl::LERROR,"read cfg file fail");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Read CFG file failed");
				fileCtrl.CloseFile();
				pLinker->Close();
				return false;
			}

			fileCtrl.CloseFile();
			pData += cfgFileLen;

			//dat文件的头
			*pData = 3; //dat文件类型
			pData++;
			memcpy(pData,(void*)&datFileLen,4);//dat文件长度

		}//end if( fileCtrl.IsExist( cfgFileName.c_str() )) 

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
			{
				mylog.WriteLog2File(LogControl::LDEBUG, "Run step 2 of GetRecWaveFile:wait for C_PL1_NA_3");
				//期待远程发送C_PL1_NA_3(召唤1级数据)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//由于ASDU14没有信息体标识符，所以每次可以发送的数据可以达到PRO103_ASDUDATA_MAXSIZE -4(起始传输位置)
			int needReadLen = (PRO103_ASDUDATA_MAXSIZE -4); //需要读取的数据长度

			unsigned int fileStartPos = 0;       //读取文件的起始位置
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //发送的数据
			int  sendDataLen =0 ;                   //发送数据的长度，即ASDUDATA的长度
			bool isSendAllData = false;            //所有数据是否已经发送完毕

			if(!bFileIsExit) //文件不存在
			{
				isSendAllData = true;
			}
			else //文件存在
			{
				memcpy(&sendData[0],&startPos,4); 
				sendDataLen += 4;

				//起始传输位置在HDR或CFG文件中
				if( startPos < cfgAndHdrLen )
				{
					unsigned int endPos = startPos + (PRO103_ASDUDATA_MAXSIZE -4);
					if( endPos > cfgAndHdrLen) //越过边界
					{
						memcpy(&sendData[4],
							&pHdrAndCfgData->operator [](startPos),
							(cfgAndHdrLen - startPos) );
						sendDataLen += cfgAndHdrLen - startPos;
						needReadLen = PRO103_ASDUDATA_MAXSIZE - sendDataLen;
						startPos = cfgAndHdrLen;    //需从DAT文件中读取数据填充未填满的空间
					}
					else
					{
						memcpy(&sendData[4],
							&pHdrAndCfgData->operator [](startPos),
							(PRO103_ASDUDATA_MAXSIZE -4) );
						sendDataLen += PRO103_ASDUDATA_MAXSIZE -4;
						needReadLen = 0;
						startPos += PRO103_ASDUDATA_MAXSIZE -4;
					}
				}

				//起始传输位置进入到DAT文件中
				if( startPos >= cfgAndHdrLen)
				{
					//打开DAT文件
					if(false == fileCtrl.OpenFile(dateFileName.c_str(),FileControl::OM_READ))
					{
						m_errCode = PEC_UNKOWN;
						string info = string(dateFileName.c_str()) + string(" open failed");
						mylog.WriteLog2File(LogControl::LERROR,info.c_str());
						//mylog.WriteLog(LogControl::LERROR,pCmd,"Open Dat file failed");
						pLinker->Close();
						return false;
					}

					fileStartPos = startPos - cfgAndHdrLen;

					int factReadLen = fileCtrl.ReadData(fileStartPos,&sendData[PRO103_ASDUDATA_MAXSIZE-needReadLen],needReadLen);
					if( factReadLen < 0)
					{
						m_errCode = PEC_UNKOWN;
						mylog.WriteLog2File(LogControl::LERROR,"read dat file fail");
						//mylog.WriteLog(LogControl::LERROR,pCmd,"Read Dat file failed");
						fileCtrl.CloseFile();
						pLinker->Close();
						return false;
					}

					if( factReadLen < needReadLen || (startPos + factReadLen) >= sendDataTotalLen )
					{
						isSendAllData = true; //数据发送完毕
					}

					sendDataLen += factReadLen;
					startPos += factReadLen;

					fileCtrl.CloseFile();
				}//end if( startPos >= cfgAndHdrLen)

			}//end if(!bFileIsExit) //文件不存在

			auto_ptr<string> pDataPack;    //要发送的数据包

			if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG, "Has fault report and pause GetRecWaveFile");
					m_errCode = PEC_INTERRUPT;
					//有新故障，暂停此任务
					*((int*)(pCmd->_state.pStoreParm)) = startPos;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG, "Run step 3 of GetRecWaveFile:send ASDU14");

				//发送数据	              
				if( isSendAllData) //数据已经发送完
				{
					pDataPack = auto_ptr<string>(CreateMsg(M_EX14_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}
				else //数据未发送完
				{
					pDataPack = auto_ptr<string>(CreateMsg(M_EX14_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"Send data error");
					pLinker->Close();
					return false;
				}

				pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
				if( isSendAllData) //数据已经发送完
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
			}

			}//end while(1)

		}
		else
		{
			//命令来自于UI、定时器等
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
			//pLinker->Close();
			return false;
		}

		return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol103::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	int index = GetPro2CmdIndex(cmd);   //获取命令对应的映射索引
	if( index < 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command");
		//mylog.WriteLog(LogControl::LWARNING,&cmd,"Invalid command");
		return false;
	}

	if( pro2CmdMap[index].exuteCmd > 0)
	{
		return pro2CmdMap[index].exuteCmd(this,pLinker,cmd); //执行命令函数
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetPro2CmdIndex(struct Command&)'
int Protocol103::GetPro2CmdIndex( struct Command & cmd )
{
	// {{{USR
	int i;
	int mapCount = (sizeof(pro2CmdMap)/sizeof(PRO2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro2CmdMap[i].cmdFunCode == cmd.funCode )
		{
			return i;
		}
	}

	return -1;
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreateMsg(int,unsigned int,bool,unsigned char,int,char*,int,int*)'
string * Protocol103::CreateMsg( int msgNo, unsigned int deviceId, bool fcbOrAcd, unsigned char COT, int infoNo, char * ASDUData, int ASDUDataLen, int * msgLen )
{
	// {{{USR
	string *pResult = NULL;     //返回的消息串
	char pMsgStr[PRO103_VFRAME_MAXSIZE+1] = {0};

	int indexmsg;
	int indexinf;
	int	packetlen;

	CONTOLAREA103 controlArea;
	memset(&controlArea,0,sizeof(controlArea));

	//寻找消息号
	for(indexmsg=0;indexmsg<MSG103_COUNT;indexmsg++)
	{
		if(msg103_array[indexmsg].msgno == msgNo)
		{
			break;
		}
	}

	if(indexmsg>=MSG103_COUNT)
	{
		mylog.WriteLog2File(LogControl::LERROR,"msgno not found");
		return NULL;
	}

	//生成控制域信息
	if(PROHB103_DIRECT_M == msg103_array[indexmsg].direct)
	{
		controlArea.m.acd=fcbOrAcd;
		controlArea.m.prm=PROHB103_DIRECT_M;
		controlArea.m.fun=msg103_array[indexmsg].funid;
		controlArea.m.dfc=msg103_array[indexmsg].user;
	}
	else
	{
		controlArea.c.fcb=fcbOrAcd;
		controlArea.c.prm=PROHB103_DIRECT_C;
		controlArea.c.fun=msg103_array[indexmsg].funid;
		controlArea.c.fcv=msg103_array[indexmsg].user;
	}

	//固定长度
	if(msg103_array[indexmsg].isconst==true)
	{
		//pMsgStr = new char[sizeof(FIXFRAME)];
		//memset(pMsgStr,0,sizeof(FIXFRAME));
		PFIXFRAME	pFixFrame = (PFIXFRAME)pMsgStr;


		packetlen=sizeof(FIXFRAME);

		//起始字符
		pFixFrame->startChar = PRO103_FIXFRAME_STARTCHAR;

		//控制域
		pFixFrame->control.control = controlArea.control;

		//站地址
		pFixFrame->address = (deviceId & 0XFF);

		//校验
		this->doCheck(pMsgStr,packetlen,true,&(pFixFrame->CS));

		//结束字符16h
		pFixFrame->endChar = PRO103_ENDCHAR;

		//打印帧信息
		if(SystemCfger::GetInstance().logLevel >= 5)
		{
			char szInf[250] = {0};
			sprintf(szInf,"Create a fix frame -->\r StartChar : %X\r FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r",
				pFixFrame->startChar,pFixFrame->control.m.fun,pFixFrame->address,pFixFrame->CS,pFixFrame->endChar);
			mylog.WriteLog2File(LogControl::LDETAIL, szInf);
		}

	}
	//可变长度
	else
	{
		//定义结构指针位置
		PVFRAMEHEAD	pVFrameHead = NULL;
		PASDUID	pAsduID = NULL;
		PASDUINFID	pAsduInfID = NULL;
		PVFRAMEEND	pVFrameEnd = NULL;

		if( MINF_NOTUSER == infoNo ) //信息体标示符包含在信息数据中
		{
			packetlen = sizeof(VFRAMEHEAD) + sizeof(ASDUID) + ASDUDataLen	+ sizeof(VFRAMEEND);
			//pMsgStr = new char[packetlen];
			//memset(pMsgStr,0,packetlen);

			pVFrameHead = (PVFRAMEHEAD)pMsgStr;
			pAsduID = (PASDUID)( pMsgStr + sizeof(VFRAMEHEAD) );
			pVFrameEnd = (PVFRAMEEND)( pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) + ASDUDataLen );

			//信息元素集
			memcpy( (pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) ),ASDUData,ASDUDataLen);
		}
		else
		{
			packetlen = sizeof(VFRAMEHEAD) + sizeof(ASDUID) + sizeof(ASDUINFID) + ASDUDataLen	+ sizeof(VFRAMEEND);
			//pMsgStr = new char[packetlen];
			//memset(pMsgStr,0,packetlen);

			pVFrameHead = (PVFRAMEHEAD)pMsgStr;
			pAsduID = (PASDUID)( pMsgStr + sizeof(VFRAMEHEAD) );
			pAsduInfID = (PASDUINFID)( pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID));
			pVFrameEnd = (PVFRAMEEND)( pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) + sizeof(ASDUINFID) + ASDUDataLen );

			//寻找信息序号
			for(indexinf=0;indexinf<INF103_COUNT;indexinf++)
			{
				if(inf103_array[indexinf].infno == infoNo)
				{
					break;
				}
			}

			if(indexinf>= INF103_COUNT)
			{
				pAsduInfID->FUN = FUN(infoNo);
				pAsduInfID->INF = INF(infoNo);
			}
			else
			{
				//信息体标识
				pAsduInfID->FUN = inf103_array[indexinf].fun;
				pAsduInfID->INF = inf103_array[indexinf].inf;
			}

			//信息元素集
			memcpy( (pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) + sizeof(ASDUINFID) ),ASDUData,ASDUDataLen);
		}


		if((unsigned int)packetlen > PRO103_VFRAME_MAXSIZE)
		{
			mylog.WriteLog2File(LogControl::LERROR,"data len too big");
			return 	NULL;
		}

		//起始字符
		pVFrameHead->HEAD.startChar1 = pVFrameHead->HEAD.startChar2 = PRO103_VOLATILEFRAME_STARTCHAR;

		//链路长度
		int len = packetlen - sizeof(VFRAMEHEAD::tagHead) - sizeof(VFRAMEEND);
		pVFrameHead->HEAD.lengthLow = len & 0X000000FF;
		pVFrameHead->HEAD.lengthHigh = (len & 0X00000F00) >> 8;

		//控制域
		pVFrameHead->control.control = controlArea.control;

		//数据单元标识
		pAsduID->TYP = msg103_array[indexmsg].typ;
		pAsduID->S = msg103_array[indexmsg].sq;
		pAsduID->VSQ= msg103_array[indexmsg].num;

		//原因码
		pAsduID->COT = COT;

		//链路地址
		pVFrameHead->address = (deviceId & 0XFF);
		//ASDU地址
		pAsduID->COMADDR = (deviceId & 0XFF);

		//校验
		this->doCheck(pMsgStr,packetlen,false,&(pVFrameEnd->CS ));

		//结束字符16h
		pVFrameEnd->endChar= PRO103_ENDCHAR;

		//打印帧信息
		if(SystemCfger::GetInstance().logLevel >= 5)
		{
			char szInf[1000] = {0};
			sprintf(szInf,"Create a Volatile frame -->\r StartChar1 : %X\r LowLen : %X\r HighLen %X\r StartChar2 : %X\r \
						  FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r TYP : %X\r VSQ : %X\r COT : %X\r ASDU_ADDR : %X\r",
						  pVFrameHead->HEAD.startChar1,pVFrameHead->HEAD.lengthLow,pVFrameHead->HEAD.lengthHigh,
						  pVFrameHead->HEAD.startChar2,pVFrameHead->control.m.fun,pVFrameHead->address,pVFrameEnd->CS,
						  pVFrameEnd->endChar,pAsduID->TYP,( pAsduID->VSQ + (pAsduID->S<<7) ),pAsduID->COT,pAsduID->COMADDR);
			if(pAsduInfID != NULL)
			{
				sprintf(szInf,"%s FUN : %X\r INF :%X\r ",szInf,pAsduInfID->FUN,pAsduInfID->INF);                          
			}

			mylog.WriteLog2File(LogControl::LDETAIL, szInf);
		}

	}

	if(NULL != msgLen)
	{
		*msgLen = packetlen;
	}

	pResult = new string("");
	pResult->resize(packetlen+1);
	memset(&(pResult->operator [](0)),0,pResult->size());
	memcpy(&(pResult->operator [](0)),pMsgStr,packetlen);
	//pResult = new string(pMsgStr);

	return pResult;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetDauInf(Linker *,struct Command*)'
bool Protocol103::GetDauInf( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    m_errCode = PEC_ERRCMD_NODATA;
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol 103 don't support the command");
	    return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
bool Protocol103::GetRecWaveFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//召唤录波文件列表
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetRecWaveFileList()");

	//分析命令参数
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::GetRecWaveFileList()");
		return false;
	}

	struct CP56Time2a startTime;    //查询开始时间
	struct CP56Time2a endTime;    //查询结束时间
	memcpy(&startTime,pCmd->pParm,sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[sizeof(startTime)],sizeof(endTime));

	int            FCB = -1;	   

	FileControl fileCtrl;
	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	//bool      isNeedSendPrePack = false;  //是否重发前一帧的数据（FCV=1时，FCB未变，都要重发）

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	//int count = -1;//循环计数
	bool result = true; //返回的结果

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		//取得文件列表
		vector<PCMDFILELIST> fileList;
		if(false == fileCtrl.GetRecWaveFileList(pCmd->destDevId,startTime,endTime,fileList))
		{
			m_errCode = PEC_UNKOWN;
			mylog.WriteLog2File(LogControl::LERROR,"GetRecWaveFileList fail");
			pLinker->Close();   //关闭连接
			return false;
		}

		vector<PCMDFILELIST>::iterator it = fileList.begin();

		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Has fault report need to sent to remot and pause GetRecWaveFileList");
				//暂停任务
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetRecWaveFileList:send M_CON_NA_3");
			//回答确认帧
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		int hasSendItems = 0;   //已经被发送的个数
		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetRecWaveFileList");
			hasSendItems = *((int*)(pCmd->_state.pStoreParm));
		}
		else
		{
			int *pHasSend = new int;
			*pHasSend = 0;
			pCmd->_state.pStoreParm = (void*)pHasSend;
		}

		for(int i=0;i<hasSendItems;i++)
		{
			it++;
		}

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetRecWaveFileList:wait for C_PL1_NA_3");
				//期待远程发送C_PL1_NA_3(召唤1级数据)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//由于ASDU16没有信息体标识符，所以每次可以发送的数据可以达到PRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //发送的数据
			int  sendDataLen =0 ;                   //发送数据的长度，即ASDUDATA的长度
			bool isSendAllData = false;             //所有数据是否已经发送完毕

			//能够被发送的文件列表元素个数
			unsigned short maxFileItemSend = (PRO103_ASDUDATA_MAXSIZE -16)/sizeof(CMDFILELIST);

			//实际被发送的文件列表元素个数
			unsigned short factFileItemSend = 0;

			if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Has fault report need to sent to remot and pause GetRecWaveFileList");
					//有新故障，暂停此任务
					m_errCode = PEC_INTERRUPT;
					*((int*)(pCmd->_state.pStoreParm)) = hasSendItems;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetRecWaveFileList:send ASDU16");

				for(int i=0;i<maxFileItemSend;i++)
				{
					if( it != fileList.end())
					{
						//将文件名等信息拷贝到要发送的数据buffer中
						memcpy(&sendData[16 + factFileItemSend*sizeof(CMDFILELIST)],(*it),sizeof(CMDFILELIST));
						sendDataLen += sizeof(CMDFILELIST);

						it++;
						factFileItemSend++;
						hasSendItems++;
					}
					else
					{
						isSendAllData = true;
						break;
					}
				}

				//发送ASDU16
				if( factFileItemSend > 0)
				{
					//设置本帧文件数
					memcpy(&sendData[0],&factFileItemSend,2);
					sendDataLen +=2;

					//设置开始、结束时间
					memcpy(&sendData[2],&startTime,sizeof(startTime) );
					memcpy(&sendData[2+sizeof(startTime)],&endTime,sizeof(endTime) );
					sendDataLen += 2*sizeof(startTime);

					//发送数据
					auto_ptr<string> pDataPack ;    //要发送的数据包

					if( isSendAllData) //数据已经发送完
					{
						pDataPack = auto_ptr<string>(CreateMsg(M_EX16_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}
					else //数据未发送完
					{
						pDataPack = auto_ptr<string>(CreateMsg(M_EX16_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}

					sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
					if(sendDataLen != msgLen)
					{
						mylog.WriteLog2File(LogControl::LERROR, "send data error");
						pLinker->Close();
						result = false;
						break;
					}

					pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
					if( isSendAllData) //数据已经发送完
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
						break;
					}

				} // if( factFileItemSend > 0)
			}//end if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
		}//end while(1)

		//释放内存
		vector<PCMDFILELIST>::iterator delIt = fileList.begin();
		for(;delIt != fileList.end();delIt++)
		{
			PCMDFILELIST pTmp = (*delIt);
			delete pTmp;
			pTmp = NULL;
		}
		fileList.clear();

		return result;

	}
	else
	{
		//命令来自于UI、定时器等
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
		//pLinker->Close();
		return false;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
bool Protocol103::GetReportFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//获取简要录波报告    
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetReportFile()");

	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		//远程已经发送C_PL2_NA_3（召唤2级数据）
		CMDREPORT report;
		memset(&report,0,sizeof(CMDREPORT));

		//获取本地的录波简报
		vector<PREPORTHEAD> *pReportList = SystemCfger::GetInstance().GetReportList( pLinker->GetConnectAddr() );

		auto_ptr<string> pAswer ;

		if(pCmd->_state._state == CmdState::CS_STEP1)
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetReportFile:send M_NV_NA_3");

			if(pReportList != NULL && pReportList->size()>0) //本地有录波简报，回答M_NV_NA_3（无所要求数据，ACD=1）
			{
				pAswer = auto_ptr<string>( CreateMsg(M_NV_NA_3, pCmd->destDevId,true,0,0,NULL,0,&msgLen) );
			}
			else    //本地无录波简报，回答M_NV_NA_3（无所要求数据，ACD=0）
			{
				pAswer = auto_ptr<string>( CreateMsg(M_NV_NA_3, pCmd->destDevId,false,0,0,NULL,0,&msgLen) );
			}

			if( pAswer->c_str() != NULL)
			{
				sendDataLen = SendData(pLinker,pAswer->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送回答帧 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
					pLinker->Close();
					return false;
				}
			}
			else
			{
				mylog.WriteLog2File(LogControl::LERROR, "create msg error");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pReportList == NULL || pReportList->size() == 0)
		{
			return true;
		}

		vector<PREPORTHEAD>::iterator it = pReportList->begin();
		int count = 0;
		for(;it != pReportList->end(); it++)
		{
			count++;

			if(pCmd->_state._state == CmdState::CS_STEP2)
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetReportFile:wait for C_PL1_NA_3");
				//期待远程发送C_PL1_NA_3(召唤1级数据)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			char sendData[(PRO103_ASDUDATA_MAXSIZE-2) +1] = {0};    //发送的数据(排除信息体标识符)
			int  sendDataLen =0 ;                   //发送数据的长度，即ASDUDATA的长度

			PREPORTHEAD pReport = (*it);                
			memcpy(&sendData[0],&(pReport->report),sizeof(CMDREPORT));
			sendDataLen = sizeof(CMDREPORT);                

			if(pCmd->_state._state == CmdState::CS_STEP3)
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetReportFile:send ASDU12");
				//发送ASDU12
				auto_ptr<string> pDataPack;
				if(count < (int)pReportList->size())
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
					pDataPack = auto_ptr<string>( CreateMsg(M_EX12_NA_3, pCmd->destDevId,true,1,MINF_242,&sendData[0],sendDataLen,&msgLen));
				}
				else
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					pDataPack = auto_ptr<string>( CreateMsg(M_EX12_NA_3, pCmd->destDevId,false,1,MINF_242,&sendData[0],sendDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
					pLinker->Close();
					return false;
				}
			}//end if(pCmd->_state._state == CmdState::CS_STEP3)
		}//end for(;it != pReportList->end(); it++)

		//更新数据库并清除列表
		it = pReportList->begin();
		for(;it != pReportList->end(); it++)
		{
			PREPORTHEAD pReport = (*it);

			//更新数据库
			if(addSendFaultInf > 0)
			{
				SendFaultInfo sendFaultInf;
				memset(&sendFaultInf,0,sizeof(sendFaultInf));
				sendFaultInf.DAUID = pReport->devID;
				sendFaultInf.RptID = pReport->rptID;
				strncpy(sendFaultInf.rmtIp,pLinker->GetConnectAddr().c_str(),sizeof(sendFaultInf.rmtIp)-1);

				addSendFaultInf(&sendFaultInf);
			}

			delete pReport;
			pReport = NULL;
		}
		pReportList->clear();

		return true;

	}// end if( pCmd->from == CD_REMOTE ) //命令来自于远程
	else
	{
		//命令来自于UI、定时器等
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
		//pLinker->Close();
		return false;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'SetTime(Linker *,struct Command*)'
bool Protocol103::SetTime( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::SetTime()");

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		//分析命令参数
		if(pCmd->parmLen != sizeof(struct CP56Time2a) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
			return false;
		}

		struct CP56Time2a time;    //时间
		memcpy(&time,pCmd->pParm,sizeof(time));

		//设置系统时间
		SYSTEMTIME systemTime;
		memset(&systemTime,0,sizeof(SYSTEMTIME));
		systemTime.wYear = 2000 + time.year;
		systemTime.wMonth = time.month;
		systemTime.wDay = time.day;
		systemTime.wHour = time.hours ;
		systemTime.wMinute = time.minute;
		systemTime.wSecond = time.milliseconds/1000;
		systemTime.wMilliseconds = time.milliseconds%1000;

		SetSystemTime(&systemTime);

		mylog.WriteLog2File(LogControl::LINFO,"Leave Protocol103::SetTime()");
		return true;

	}//end if( pCmd->from == CD_REMOTE )
	else
	{
		//命令来自于UI、定时器等
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
		//pLinker->Close();
		return false;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetTime(Linker *,struct Command*)'
bool Protocol103::GetTime( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol don't support the command:GetTime");
	    return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'StartRecByManual(Linker *,struct Command*)'
bool Protocol103::StartRecByManual( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	   
	mylog.WriteLog2File(LogControl::LERROR, "The protocol 103 don't support the command:StartRecByManual");
	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'StartRecByTimer(Linker *,struct Command*)'
bool Protocol103::StartRecByTimer( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	   
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol 103 don't support the command:StartRecByTimer");
	    return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'CreateGIMsg(int,unsigned int,unsigned char,int,GI[],unsigned char,int*)'
string * Protocol103::CreateGIMsg( int msgNo, unsigned int deviceId, unsigned char COT, int infoNo, GI GIs[], unsigned char GINumber, int * msgLen )
{
	// {{{USR
	    
	char ASDUData[PRO103_ASDUDATA_MAXSIZE] = {0};
	int  ASDULen    = 0;

	ASDUData[0] = 0 ;           //返回信息标识符(未用)
	ASDUData[1] = GINumber;     //通用分类数据集数目
	ASDULen = 2;

	//通用分类条目
	for(int i=0; i<(int)GINumber; i++)
	{
		memcpy(&ASDUData[ASDULen],&(GIs[i].GIN),2); //GIN
		ASDULen += 2;
		ASDUData[ASDULen] = GIs[i].KOD ;   //描述类别
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataType ;//通用分类数据:数据类型
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataLen ;//通用分类数据:数据宽度
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataNum ;//通用分类数据:数据数目
		ASDULen += 1;

		//拷贝“属性结构”数据
		if(GIs[i].dataLen >0 && GIs[i].data != NULL)
		{
			if( ASDULen + GIs[i].dataLen > PRO103_ASDUDATA_MAXSIZE)
			{
				return NULL;
			}

			memcpy(&ASDUData[ASDULen],GIs[i].data,GIs[i].dataLen);
			ASDULen += GIs[i].dataLen;
		}
	}

	return CreateMsg(msgNo,deviceId,false,COT,infoNo,&ASDUData[0],ASDULen,msgLen);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileList(Linker *,struct Command*)'
bool Protocol103::GetFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//召唤一般文件列表
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetFileList()");

	//分析命令参数
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 +100 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
		return false;
	}

	struct CP56Time2a startTime;    //查询开始时间
	struct CP56Time2a endTime;    //查询结束时间
	memcpy(&startTime,pCmd->pParm,sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[sizeof(startTime)],sizeof(endTime));

	char fileName[101] = {0};                   //目录名及通配符
	memcpy(fileName,&pCmd->pParm[sizeof(startTime)*2],100);	    

	FileControl fileCtrl;
	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	auto_ptr<string> pDataPack ;    //要发送的数据包
	//bool      isNeedSendPrePack = false;  //是否重发前一帧的数据（FCV=1时，FCB未变，都要重发）

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	bool result = true; //返回的结果

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause GetFileList");
				//有新故障，暂停此任务
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetFileList:send M_CON_NA_3");
			//回答确认帧
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		//取得文件列表
		vector<PCMDGENERALFILEINF> fileList;
		if(false == fileCtrl.GetFileList(fileName,startTime,endTime,fileList))
		{
			mylog.WriteLog2File(LogControl::LERROR,"GetRecWaveFileList fail");
			pLinker->Close();   //关闭连接
			return false;
		}

		int hasSendItems = 0;   //已经被发送的个数
		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetFileList");
			hasSendItems = *((int*)(pCmd->_state.pStoreParm));
		}
		else
		{
			int *pHasSend = new int;
			*pHasSend = 0;
			pCmd->_state.pStoreParm = (void*)pHasSend;
		}

		vector<PCMDGENERALFILEINF>::iterator it = fileList.begin();

		for(int i=0;i<hasSendItems;i++)
		{
			it++;
		}

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetFileList:wait C_PL1_NA_3");
				//期待远程发送C_PL1_NA_3(召唤1级数据)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//由于ASDU102没有信息体标识符，所以每次可以发送的数据可以达到PRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //发送的数据
			int  sendDataLen =0 ;                   //发送数据的长度，即ASDUDATA的长度
			bool isSendAllData = false;             //所有数据是否已经发送完毕

			//能够被发送的文件列表元素个数
			unsigned short maxFileItemSend = (PRO103_ASDUDATA_MAXSIZE -16)/sizeof(CMDGENERALFILEINF);

			//实际被发送的文件列表元素个数
			unsigned short factFileItemSend = 0;

			if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause GetFileList");
					//有新故障，暂停此任务
					*((int*)(pCmd->_state.pStoreParm)) = hasSendItems;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetFileList:send ASDU102");

				for(int i=0;i<maxFileItemSend;i++)
				{
					if( it != fileList.end())
					{
						//将文件名等信息拷贝到要发送的数据buffer中
						memcpy(&sendData[16 + factFileItemSend*sizeof(CMDGENERALFILEINF)],(*it),sizeof(CMDGENERALFILEINF));
						sendDataLen += sizeof(CMDGENERALFILEINF);

						it++;
						factFileItemSend++;
						hasSendItems++;
					}
					else
					{
						isSendAllData = true;
						break;
					}
				}

				//发送ASDU102

				//设置本帧文件数
				memcpy(&sendData[0],&factFileItemSend,2);
				sendDataLen +=2;

				//设置开始、结束时间
				memcpy(&sendData[2],&startTime,sizeof(startTime) );
				memcpy(&sendData[2+sizeof(startTime)],&endTime,sizeof(endTime) );
				sendDataLen += 2*sizeof(startTime);

				//发送数据
				if( isSendAllData) //数据已经发送完
				{
					string *pTemp = pDataPack.release();
					if(pTemp != NULL)
					{
						delete pTemp;
						pTemp = NULL;
					}
					pDataPack = auto_ptr<string>(CreateMsg(M_EX102_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}
				else //数据未发送完
				{
					string *pTemp = pDataPack.release();
					if(pTemp != NULL)
					{
						delete pTemp;
						pTemp = NULL;
					}
					pDataPack = auto_ptr<string>(CreateMsg(M_EX102_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					pLinker->Close();
					result = false;
					break;
				}

				pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
				if( isSendAllData) //数据已经发送完
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
			}//end if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
		} //end while(1)

		//释放内存
		vector<PCMDGENERALFILEINF>::iterator delIt = fileList.begin();
		for(;delIt != fileList.end();delIt++)
		{
			PCMDGENERALFILEINF pTmp = (*delIt);
			delete pTmp;
			pTmp = NULL;
		}
		fileList.clear();
		return result;
	}
	else
	{
		//命令来自于UI、定时器等
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
		//pLinker->Close();
		return false;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'PullFile(Linker *,struct Command*)'
bool Protocol103::PullFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::PullFile()");

	//分析命令参数
	if(pCmd->parmLen != 104 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
		return false;
	}

	char fileName[100] = {0};    //文件名（包含目录）
	int startPos = 0;           //起始传输位置
	memcpy(&fileName,pCmd->pParm,sizeof(fileName));
	memcpy(&startPos,&pCmd->pParm[sizeof(fileName)],sizeof(int));

	FileControl fileCtrl;
	int msgLen = 0; //消息长度

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	bool result = true; //返回的结果

	if( pCmd->from == CD_REMOTE ) //命令来自于远程
	{
		if(pCmd->_state._state == CmdState::CS_STEP1) //第1步
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause PullFile");
				//有新故障，暂停此任务
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of PullFile:send M_CON_NA_3");
			//回答确认帧
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state.pStoreParm != NULL)  //任务曾经被执行过
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running PullFile");
			startPos = *((int*)(pCmd->_state.pStoreParm));
		}
		else
		{
			int *pStartPos = new int;
			*pStartPos = 0;
			pCmd->_state.pStoreParm = (void*)pStartPos;
		}

		//ASDU104发送的文件数据头的结构
		struct _Asdu104FileHead
		{
			char fileName[100];             //文件名
			struct CP56Time2a fileTime;     //文件时间
			unsigned int fileLen;           //文件长度
		} fileHead;
		memset(&fileHead,0,sizeof(fileHead));

		bool fileIsExist = fileCtrl.IsExist(fileName);
		if(fileIsExist)
		{
			memcpy(&fileHead.fileName[0],&fileName[0],100);
			fileCtrl.GetFileTime(&fileName[0], &fileHead.fileTime);
			fileHead.fileLen = fileCtrl.GetFileLen(&fileName[0]);

			if(!fileCtrl.OpenFile(&fileName[0],FileControl::OM_READ))
			{
				string sInfo = string("open the file: ") +string(&fileName[0]) +string(" fail");
				mylog.WriteLog2File(LogControl::LERROR,sInfo.c_str());
				//mylog.WriteLog(LogControl::LERROR,pCmd,"Open file failed");
				pLinker->Close();
				return false;
			}
		}

		//每次可从文件读取的最大文件数据长度
		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4 -sizeof(struct _Asdu104FileHead);
		int factReadLen = 0;    //实际读取的数据长度
		unsigned int hasReadLen = startPos; //文件已经读取的长度

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //第2步
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of PullFile:wait for C_PL1_NA_3");
				//期待远程发送C_PL1_NA_3(召唤1级数据)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//由于ASDU104没有信息体标识符，所以每次可以发送的数据可以达到PRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //发送的数据
			int  sendDataLen =0 ;                   //发送数据的长度，即ASDUDATA的长度
			bool isSendAllData = false;             //所有数据是否已经发送完毕
			auto_ptr<string> pDataPack ;    //要发送的数据包

			//发送ASDU104
			if(!fileIsExist) //文件不存在
			{
				memcpy(sendData,fileName,100);
				sendDataLen = sizeof(fileName) + sizeof(struct CP56Time2a) + sizeof(int);
				pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				isSendAllData = true;
			}
			else
			{
				memcpy(&sendData[0],&startPos,4);                             //起始传输位置
				memcpy(&sendData[4],&fileHead,sizeof(fileHead));              //文件数据头

				if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
				{
					if(SendInterruptFrame(pLinker,pCmd))
					{
						mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause PullFile");
						//有新故障，暂停此任务
						*((int*)(pCmd->_state.pStoreParm)) = startPos;
						return false;
					}

					mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of PullFile:send ASDU104");

					factReadLen = fileCtrl.ReadData(startPos,&sendData[4+sizeof(fileHead)],readMaxLen);
					sendDataLen = 4 + sizeof(fileHead) + factReadLen;
					hasReadLen += factReadLen;
					startPos += factReadLen;

					//文件已经读完
					if(factReadLen <= 0 || hasReadLen >= fileHead.fileLen)
					{
						isSendAllData = true;
					}

					//发送数据
					if( isSendAllData) //数据已经发送完
					{
						pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}
					else //数据未发送完
					{
						pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}

					sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
					if(sendDataLen != msgLen)
					{
						mylog.WriteLog2File(LogControl::LERROR, "send data error");
						pLinker->Close();
						result = false;
						break;
					}

					pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
					if( isSendAllData) //数据已经发送完
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
						break;
					}

				}//end if(pCmd->_state._state == CmdState::CS_STEP3) //第3步
			}//end if(!fileIsExist) //文件不存在

		}//end while(1)

		fileCtrl.CloseFile();
		return result;
	}
	else
	{
		//命令来自于UI、定时器等
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol103 only surport the communication between the remote and the SHNeuron");
		//pLinker->Close();
		return false;
	}

	// }}}USR
}
// }}}RME

// {{{RME operation 'PushFile(Linker *,struct Command*)'
bool Protocol103::PushFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol don't support the command:PushFile");
	    return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ReadRltData(Linker *,struct Command*)'
bool Protocol103::ReadRltData( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol don't support the command:ReadRltData");
	    return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'AutoSend(Linker *,struct Command*)'
bool Protocol103::AutoSend( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    mylog.WriteLog2File(LogControl::LERROR, "The protocol don't support the command:AutoSend");
	    return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'InitConnect(Linker *)'
bool Protocol103::InitConnect( Linker * pLinker )
{
	// {{{USR
	int msgLen = 0; //消息长度
	int sendDataLen = 0;    //发送的数据长度

	FIXFRAME fixFrame;                  //固定长帧
	memset(&fixFrame,0,sizeof(fixFrame));

	/*********************复位通信单元-->******************/

	//期待远程发送C_RCU_NA_3(复位通信单元)
	if(false == AnalyseFixFrame(pLinker,&fixFrame,false) ) //获得固定长的帧
	{
		mylog.WriteLog2File(LogControl::LERROR,"get fix frame error");
		return false;
	}

	//是否是期待的命令
	if( fixFrame.control.c.fun != CFUNID_0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"want the command C_RCU_NA_3,but it isn't");
		return false;
	}

	//发送M_CON_NA_3（确认，ACD=1）
	if(!SendConfirm(pLinker,0,true))
	{
		return false;
	}

	//期待远程发送C_PL1_NA_3(召唤1级数据)
	if(!WantLevelOne(pLinker))
	{
		return false;
	}

	char sendData[(PRO103_ASDUDATA_MAXSIZE-2) +1] = {0};    //发送的数据(排除信息体标识符)

	sendData[0] = 0X03;      //兼容级别
	sendDataLen = 1;

	//发送M_IRC_NA_3（ASDU5，COT=复位通信单元）
	auto_ptr<string> pDataPack(CreateMsg(M_IRC_NA_3, 0,false,0X04,MINF_242,&sendData[0],sendDataLen,&msgLen));
	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
	if(sendDataLen != msgLen)
	{
		mylog.WriteLog2File(LogControl::LERROR, "send data error");
		return false;
	}

	/*********************复位帧计数位-->******************/

	//期待远程发送C_RFB_NA_3（复位帧计数位）
	if(false == AnalyseFixFrame(pLinker,&fixFrame,false) ) //获得固定长的帧
	{
		mylog.WriteLog2File(LogControl::LERROR,"get fix frame error");
		return false;
	}

	//是否是期待的命令
	if( fixFrame.control.c.fun != CFUNID_7 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"want the command C_RFB_NA_3,but it isn't");
		return false;
	}

	//发送M_CON_NA_3（确认，ACD=1）
	if(!SendConfirm(pLinker,0,true))
	{
		return false;
	}

	//期待远程发送C_PL1_NA_3(召唤1级数据)
	if(!WantLevelOne(pLinker))
	{
		return false;
	}

	//发送M_IRF_NA_3（ASDU5，COT=复位帧计数位）
	memset(&sendData[0],0,sizeof(sendData));
	sendData[0] = 0X03;      //兼容级别
	sendDataLen = 1;

	string *pTemp = pDataPack.release();
	if(pTemp != NULL)
	{
		delete pTemp;
		pTemp = NULL;
	}
	pDataPack = auto_ptr<string>(CreateMsg(M_IRC_NA_3, 0,false,0X03,MINF_242,&sendData[0],sendDataLen,&msgLen));
	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //发送数据 
	if(sendDataLen != msgLen)
	{
		mylog.WriteLog2File(LogControl::LERROR, "send data error");
		return false;
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
    bool Protocol103::StopRecWave( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

    bool Protocol103::GetContData( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

    bool Protocol103::ReadZeroOffset( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

	bool Protocol103::StartGnExperiment( Linker * pLinker, struct Command * pCmd )
	{
		return false;
	}

	bool Protocol103::StopGnExperiment( Linker * pLinker, struct Command * pCmd )
	{
		return false;
	}

    bool Protocol103::GetDeviceState( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

    bool Protocol103::SetDeviceState( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

    bool Protocol103::OperateDevice( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

	bool Protocol103::GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd )
    {
        return false;
    }

    //查询录波器运行状态信息
    bool GetDauInf(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetDauInf(pLinker,&cmd);
    }

    //召唤录波文件
    bool GetRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFile(pLinker,&cmd);
    }

	//召唤分通道录波文件
    bool GetSplitRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetSplitRecWaveFile(pLinker,&cmd);
    }

    //召唤录波文件列表
    bool GetRecWaveFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFileList(pLinker,&cmd);
    }

    //简要录波报告
    bool GetReportFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetReportFile(pLinker,&cmd);
    }

    //召唤通用文件列表
    bool GetFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetFileList(pLinker,&cmd);
    }

    //对时（时间同步）
    bool SetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->SetTime(pLinker,&cmd);
    }

    //获取时间
    bool GetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
		return pProtocol->GetTime(pLinker,&cmd);
    }

    //停止录波
    bool StopRecWave(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StopRecWave(pLinker,&cmd);
    }

    //手动启动录波
    bool StartRecByManual(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StartRecByManual(pLinker,&cmd);
    }

    //定时启动录波
    bool StartRecByTimer(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StartRecByTimer(pLinker,&cmd);
    }

    //读动态纪录数据
    bool GetContData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetContData(pLinker,&cmd);
    }

    //获取文件
    bool PullFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->PullFile(pLinker,&cmd);
    }

    //下传文件
    bool PushFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->PushFile(pLinker,&cmd);
    }

    //读实时数据
    bool ReadRltData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->ReadRltData(pLinker,&cmd);
    }

    //读实零漂刻度
    bool ReadZeroOffset(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->ReadZeroOffset(pLinker,&cmd);
    }

	//开始发电机实验
	bool StartGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->StartGnExperiment(pLinker, &cmd);
	}

	//停止发电机实验
	bool StopGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->StopGnExperiment(pLinker, &cmd);
	}

    //主动上传
    bool AutoSend(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->AutoSend(pLinker,&cmd);
    }

    //读装置状态
    bool GetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetDeviceState(pLinker,&cmd);
    }

    //设置装置状态
    bool SetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->SetDeviceState(pLinker,&cmd);
    }

    //硬件操作
    bool OperateDevice(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->OperateDevice(pLinker,&cmd);
    }

// }}}USR
// }}}RME

// }}}RME
