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
* �ļ����ƣ�Protocol103.cpp
* �ļ���ʶ��
* ժ    Ҫ������103Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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

	//�Ȼ�ȡ�����ַ�
	readLen = ReceiveData(pLinker,(char *)&startChar,1,50);
	if(readLen != 1)
	{
		//mylog.WriteLog2File(LogControl::LDEBUG,"no task to be receive");
		return NULL;
	}

	switch(startChar)
	{
	case PRO103_VOLATILEFRAME_STARTCHAR: //�ɱ䳤֡����
		return DecodeVolatileFrame(pLinker);
	case PRO103_FIXFRAME_STARTCHAR: //�̶���֡����
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

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //����ɱ�֡����
	PVFRAMEHEAD pVFrameHead = NULL;                 //֡ͷ
	PASDUID pASDUID = NULL;      //103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;   //103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��
	PVFRAMEEND pVFrameEnd = NULL;    //֡β
	char *pAsduData=NULL;
	int  AsduDataLen=0;

	int framePacketLen=0;   //֡ʵ�ʵĳ���

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

	//���Э����������ӳ������
	int index = -1;
	if(pASDUID->TYP == 0X0A) //ASDU10Ҫ�������⴦��
	{
		if(AsduDataLen < 4)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"The asdu data is too short");
			return NULL;
		}

		unsigned short gin = 0;
		memcpy(&gin,&pAsduData[2],2);   //��ȡgin

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

	//��������
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

	//��ù̶�����֡
	if(false == AnalyseFixFrame(pLinker,&fixFrame,true,1000) )
	{
		mylog.WriteLog2File(LogControl::LERROR, "DecodeFixFrame fail");
		return NULL;
	}

	//���Э����������ӳ������
	int index = GetPro2CmdIndex(fixFrame.control.c.prm,fixFrame.control.c.fun,0,0,true);
	if(index < 0 )
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command");
		return NULL;
	}

	//��������
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

		//��ȥ��ͷVFRAMEHEAD::tagHead�Ͱ�βVFRAMEEND����������ݺ�
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
			int sendDataLen = SendData(pLinker,pCmdAsdu1->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�����ж�֡ 
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LINFO, "Send interrupt frame failed");
				return false;
			}

			pCmd->_state.ChangeStateTo(CmdState::CS_PAUSE);

			//������ȡ������
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

	//�Ȼ�ȡ�����ַ�
	readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
	if(readLen != 1)
	{
		mylog.WriteLog2File(LogControl::LERROR,"Get start char error in Protocol103::GetFrame().");
		pLinker->Close();
		return -1;  //ʧ��
	}

	switch(startChar)
	{
	case PRO103_VOLATILEFRAME_STARTCHAR: //�ɱ䳤֡����
		mylog.WriteLog2File(LogControl::LDETAIL, "Begin get volatile frame");

		if(vFramePacket == NULL || vFrameLen == NULL || pAsduDataLen == NULL
			|| pOutVFrameHead == NULL || pOutASDUID == NULL)
		{
			mylog.WriteLog2File(LogControl::LERROR,"Error parameter in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //ʧ��
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

					//������ȡ������
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
				return 3;   //�����ж�֡
			}
			else
			{
				return 2;   //������ͨ�ɱ�֡
			}
		}
		else
		{
			mylog.WriteLog2File(LogControl::LERROR,"AnalyseVolatileFrame() return error in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //ʧ��
		}
	case PRO103_FIXFRAME_STARTCHAR: //�̶���֡����
		mylog.WriteLog2File(LogControl::LDETAIL, "Begin get fix frame");

		if(pFixFrame == NULL)
		{
			FIXFRAME fixFrame;
			memset(&fixFrame,0,sizeof(FIXFRAME));
			fixFrame.startChar = startChar;

			if(AnalyseFixFrame(pLinker,&fixFrame,true,uiTimeOut))
			{
				m_errCode = PEC_ERRCMD_NODATA;
				return 1;  //������
			}
			else
			{
				mylog.WriteLog2File(LogControl::LERROR,"AnalyseFixFrame() return error in Protocol103::GetFrame().");
				pLinker->Close();
				return -1;  //ʧ��
			}
		}//end if(pFixFrame == NULL)

		if(AnalyseFixFrame(pLinker,pFixFrame,true,uiTimeOut))
		{
			return 1;   //���ع̶�֡
		}
		else
		{
			mylog.WriteLog2File(LogControl::LERROR,"AnalyseFixFrame() return error in Protocol103::GetFrame().");
			pLinker->Close();
			return -1;  //ʧ��
		}
	default:
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"unknow frame type");
		pLinker->Close();
		return -1;  //ʧ��
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
	unsigned char checksum; //У���
	unsigned char startChar=0;  //�����ַ�

	if( hasReadStartChar == false ) //��û�ж�ȡ�����ַ������ȶ�ȡ
	{
		//�Ȼ�ȡ�����ַ�
		readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
		if(readLen != 1)
		{
			mylog.WriteLog2File(LogControl::LERROR,"receive data error");
			return false;
		}

		if( startChar != PRO103_FIXFRAME_STARTCHAR) //�����ڴ��Ĺ̶���֡
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want a fix frame,but the frame isn't");
			return false;
		}
	}

	pFixFrame->startChar = PRO103_FIXFRAME_STARTCHAR;

	//��������֡������
	readLen = ReceiveData(pLinker,&pFrame[1],sizeof(FIXFRAME) -1,uiTimeOut );
	if(readLen != sizeof(FIXFRAME) -1 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"read fix frame head error when using protocol 103");
		return false;
	}

	//��ӡ֡��Ϣ
	if(SystemCfger::GetInstance().logLevel >= 5)
	{
		char szInf[250] = {0};
		sprintf(szInf,"Get a fix frame -->\r StartChar : %X\r FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r",
			pFixFrame->startChar,pFixFrame->control.m.fun,pFixFrame->address,pFixFrame->CS,pFixFrame->endChar);
		mylog.WriteLog2File(LogControl::LDETAIL, szInf);
	}

	//У��֡����Ч��

	//�����ַ��Ƿ���ȷ
	if(pFixFrame->endChar != PRO103_ENDCHAR)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The endchar is error");
		return false;
	}

	//У���
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

	PVFRAMEHEAD pVFrameHead =(PVFRAMEHEAD) &framePacket[0];                 //֡ͷ
	PASDUID pASDUID = (PASDUID)&framePacket[sizeof(VFRAMEHEAD)];      //ASDU id
	PASDUINFID pASDUInfID = (PASDUINFID)&framePacket[sizeof(VFRAMEHEAD) + sizeof(ASDUID)];        //ASDU ��Ϣ��

	int  readLen=0;
	unsigned char checksum; //У���
	unsigned char startChar=0;  //�����ַ�

	if( hasReadStartChar == false ) //��û�ж�ȡ�����ַ������ȶ�ȡ
	{
		//�Ȼ�ȡ�����ַ�
		readLen = ReceiveData(pLinker,(char *)&startChar,1,uiTimeOut);
		if(readLen != 1)
		{
			mylog.WriteLog2File(LogControl::LERROR,"receive data error");
			return false;
		}

		if( startChar != PRO103_VOLATILEFRAME_STARTCHAR) //�����ڴ��Ŀɱ䳤֡
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want a volatile frame,but the frame isn't");
			return false;
		}
	}

	framePacket[0] = PRO103_VOLATILEFRAME_STARTCHAR;

	int vFrameHeadLen = sizeof(VFRAMEHEAD); //֡ͷ����
	int ASDUIdLen   = sizeof(ASDUID);   //ASDU id ����
	int ASDUInfLen   = sizeof(ASDUINFID);   //ASDU ��Ϣ�� ����
	int vFrameEndLen = sizeof(VFRAMEEND);  //֡β����

	//�ȶ�֡ͷ��ASDU id������
	readLen = ReceiveData(pLinker,&framePacket[1],vFrameHeadLen + ASDUIdLen + -1,uiTimeOut );
	if(readLen != vFrameHeadLen + ASDUIdLen + -1 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"read volatile frame head error when using protocol 103");
		return false;
	}

	int AsduDataLen = 0;//ASDU��ϢԪ�صĴ�С

	if(pASDUID->TYP == ASDU_14 || pASDUID->TYP == ASDU_16 || pASDUID->TYP == ASDU_104) //û����Ϣ���ʶ��
	{
		pASDUInfID = NULL;
		ASDUInfLen = 0;

		//����ASDU��ϢԪ�صĴ�С(����-������-��ַ-Ӧ�÷������ݵ�Ԫͷ)
		AsduDataLen = (pVFrameHead->HEAD.lengthLow + ((pVFrameHead->HEAD.lengthHigh & 0x0F) <<8))
			- vFrameHeadLen + sizeof(VFRAMEHEAD::tagHead) - ASDUIdLen;
	}
	else
	{
		//��ASDU ��Ϣ��
		readLen = ReceiveData(pLinker,&framePacket[vFrameHeadLen + ASDUIdLen],ASDUInfLen,uiTimeOut);
		if(readLen != ASDUInfLen )
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"read volatile frame head error when using protocol 103");
			return false;
		}

		//����ASDU��ϢԪ�صĴ�С(����-������-��ַ-Ӧ�÷������ݵ�Ԫͷ)
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

	//��ȡASDU��ϢԪ�ؼ�103Э��ɱ�֡֡β
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

	//��ӡ֡��Ϣ
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

	//У��֡����Ч��

	//���������ַ��Ƿ����
	if(pVFrameHead->HEAD.startChar1 != pVFrameHead->HEAD.startChar2)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The startchar1 don't equals startchar2");
		return false;
	}

	//�����ַ��Ƿ���ȷ
	if(pVFrameEnd->endChar != PRO103_ENDCHAR)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The endchar is error");
		return false;
	}

	//У���
	int packetLen = vFrameHeadLen + ASDUIdLen + ASDUInfLen + AsduDataLen + vFrameEndLen;   //���ܳ���

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
	//�ش�ȷ��֡
	int msgLen = 0;
	auto_ptr<string> pAswer( CreateMsg(M_CON_NA_3, devId,true,0,0,NULL,0,&msgLen) );
	if( pAswer->c_str() != NULL)
	{
		int sendDataLen = SendData(pLinker,pAswer->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //���ͻش�֡ 
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
	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));
	unsigned int startPos = 0;      //�ļ���ʼ����λ��

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //����ɱ�֡����
	PVFRAMEHEAD pVFrameHead = NULL;                 //֡ͷ
	PASDUID pASDUID = NULL;      //103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;   //103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��
	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;   //֡ʵ�ʵĳ���

	//ȡǰ̨�Ļش�֡
	int iReturn = GetFrame(pLinker,pCmd,&fixFrame,framePacket,
		&framePacketLen,
		&pVFrameHead,
		&pASDUID,
		&pASDUInfId,
		&pAsduData,
		&AsduDataLen,
		NULL,
		SystemCfger::GetInstance().timeOut);

	if( iReturn != 1 && iReturn != 3) //���ǹ̶�֡Ҳ�����ж�֡
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the frame M_CON_NA_3,but get a volatile frame");
		pLinker->Close();
		return -1;
	}
	if( iReturn == 3) //�ж�֡
	{
		return 0;
	}

	//�Ƿ����ڴ�������
	if( fixFrame.control.m.fun  != MFUNID_0 && fixFrame.control.m.fun  != MFUNID_9)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command M_CON_NA_3,but it isn't");
		pLinker->Close();
		return -1;
	}

	if(fixFrame.control.m.fun  != MFUNID_9)//���صĲ���M_NV_NA_3
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
	//����1���û�����
	int msgLen = 0;
	auto_ptr<string> pQuest( CreateMsg(C_PL1_NA_3,devId,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		int sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //���ͻش�֡ 
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
	//�ڴ�C_PL1_NA_3(�ٻ�1������)
	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	if(false == AnalyseFixFrame(pLinker,&fixFrame,false,SystemCfger::GetInstance().timeOut) ) //��ù̶�����֡
	{
		mylog.WriteLog2File(LogControl::LERROR,"get C_PL1_NA_3 frame error");
		return false;
	}

	//�Ƿ����ڴ�������
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
		//�̶�֡���ݡ���Ϣ���򡱼������������롱Ϊ��������ӳ��
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
		//�ɱ�֡���ݡ���Ϣ���򡱡���Ӧ�÷���Ԫ���ͱ�ʶ����������ԭ�򡱼���ͨ�÷����ʶ��š�Ϊ��������ӳ��
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
	//�ٻ�¼���ļ�
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol103::GetRecWaveFile( )");

	//�����������
	if(pCmd->parmLen != 44 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::GetRecWaveFile()");
		return false;
	}

	char pFileName[41] = {0};
	memcpy(pFileName,pCmd->pParm,40);    //������չ�����ļ���
	unsigned int   startPos = 0;                    //��ʼ����λ��
	unsigned int   preStartPos = 7;                 //ǰһ֡����ʼ����λ��
	int            FCB = -1;

	unsigned int   cfgFileLen = 0;                //�����ļ�����
	unsigned int   hdrFileLen = 0;                //head�ļ�����
	unsigned int   datFileLen = 0;                //data�ļ�����
	unsigned int   cfgAndHdrLen = 0;              //hdr��cfg�ļ�����
	unsigned int   sendDataTotalLen = 0;          //���������ܳ���
	memcpy(&startPos,&pCmd->pParm[40],4);

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

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

	bool             bFileIsExit=false;  //�ļ��Ƿ����
	auto_ptr<string> pHdrAndCfgData ;   //���������ļ�ͷ��HDR��CFG�������ļ�������

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) 
	{
		//����������Զ�̣���¼���ļ����͵�Զ��
		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run step 1 of GetRecWaveFile:send M_CON_NA_3");
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG, "Has fault report and pause GetRecWaveFile");
				//���¹��ϣ���ͣ������
				return false;
			}
			//�ش�ȷ��֡
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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

		//�Ƚ�HDR��CFG�ļ������ݶ���
		if( bFileIsExit)
		{
			cfgFileLen = fileCtrl.GetFileLen(cfgFileName.c_str());                //�����ļ�����
			hdrFileLen = fileCtrl.GetFileLen(headFileName.c_str());;                //head�ļ�����
			datFileLen = fileCtrl.GetFileLen(dateFileName.c_str());;                //data�ļ�����
			sendDataTotalLen = cfgFileLen +hdrFileLen +datFileLen +40 +7 + 3*(1+4);          //���������ܳ���
			cfgAndHdrLen = hdrFileLen + cfgFileLen +(40 +7 +3*(1+4) );

			//���������ļ�ͷ��HDR��CFG�������ļ�������
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
			fileCtrl.GetRecWaveFaultTime(cfgFileName.c_str(),&cpTime);    //��ȡ¼������ʱ��

			memcpy(pData,pFileName,40);      //�����ļ���
			memcpy(&pData[40],&cpTime,7);      //���͹���ʱ��
			pData[47] = 1;          //hdr�ļ�����
			memcpy(&pData[48],(void*)&hdrFileLen,4);          //hdr�ļ�����

			//�ȶ�ȡhead�ļ�����
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

			//CFG����
			*pData = 2; //cfg�ļ�����
			pData++;
			memcpy(pData,(void*)&cfgFileLen,4);//cfg�ļ�����
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

			//dat�ļ���ͷ
			*pData = 3; //dat�ļ�����
			pData++;
			memcpy(pData,(void*)&datFileLen,4);//dat�ļ�����

		}//end if( fileCtrl.IsExist( cfgFileName.c_str() )) 

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
			{
				mylog.WriteLog2File(LogControl::LDEBUG, "Run step 2 of GetRecWaveFile:wait for C_PL1_NA_3");
				//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//����ASDU14û����Ϣ���ʶ��������ÿ�ο��Է��͵����ݿ��ԴﵽPRO103_ASDUDATA_MAXSIZE -4(��ʼ����λ��)
			int needReadLen = (PRO103_ASDUDATA_MAXSIZE -4); //��Ҫ��ȡ�����ݳ���

			unsigned int fileStartPos = 0;       //��ȡ�ļ�����ʼλ��
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //���͵�����
			int  sendDataLen =0 ;                   //�������ݵĳ��ȣ���ASDUDATA�ĳ���
			bool isSendAllData = false;            //���������Ƿ��Ѿ��������

			if(!bFileIsExit) //�ļ�������
			{
				isSendAllData = true;
			}
			else //�ļ�����
			{
				memcpy(&sendData[0],&startPos,4); 
				sendDataLen += 4;

				//��ʼ����λ����HDR��CFG�ļ���
				if( startPos < cfgAndHdrLen )
				{
					unsigned int endPos = startPos + (PRO103_ASDUDATA_MAXSIZE -4);
					if( endPos > cfgAndHdrLen) //Խ���߽�
					{
						memcpy(&sendData[4],
							&pHdrAndCfgData->operator [](startPos),
							(cfgAndHdrLen - startPos) );
						sendDataLen += cfgAndHdrLen - startPos;
						needReadLen = PRO103_ASDUDATA_MAXSIZE - sendDataLen;
						startPos = cfgAndHdrLen;    //���DAT�ļ��ж�ȡ�������δ�����Ŀռ�
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

				//��ʼ����λ�ý��뵽DAT�ļ���
				if( startPos >= cfgAndHdrLen)
				{
					//��DAT�ļ�
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
						isSendAllData = true; //���ݷ������
					}

					sendDataLen += factReadLen;
					startPos += factReadLen;

					fileCtrl.CloseFile();
				}//end if( startPos >= cfgAndHdrLen)

			}//end if(!bFileIsExit) //�ļ�������

			auto_ptr<string> pDataPack;    //Ҫ���͵����ݰ�

			if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG, "Has fault report and pause GetRecWaveFile");
					m_errCode = PEC_INTERRUPT;
					//���¹��ϣ���ͣ������
					*((int*)(pCmd->_state.pStoreParm)) = startPos;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG, "Run step 3 of GetRecWaveFile:send ASDU14");

				//��������	              
				if( isSendAllData) //�����Ѿ�������
				{
					pDataPack = auto_ptr<string>(CreateMsg(M_EX14_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}
				else //����δ������
				{
					pDataPack = auto_ptr<string>(CreateMsg(M_EX14_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"Send data error");
					pLinker->Close();
					return false;
				}

				pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
				if( isSendAllData) //�����Ѿ�������
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
			}

			}//end while(1)

		}
		else
		{
			//����������UI����ʱ����
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
	int index = GetPro2CmdIndex(cmd);   //��ȡ�����Ӧ��ӳ������
	if( index < 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command");
		//mylog.WriteLog(LogControl::LWARNING,&cmd,"Invalid command");
		return false;
	}

	if( pro2CmdMap[index].exuteCmd > 0)
	{
		return pro2CmdMap[index].exuteCmd(this,pLinker,cmd); //ִ�������
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
	string *pResult = NULL;     //���ص���Ϣ��
	char pMsgStr[PRO103_VFRAME_MAXSIZE+1] = {0};

	int indexmsg;
	int indexinf;
	int	packetlen;

	CONTOLAREA103 controlArea;
	memset(&controlArea,0,sizeof(controlArea));

	//Ѱ����Ϣ��
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

	//���ɿ�������Ϣ
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

	//�̶�����
	if(msg103_array[indexmsg].isconst==true)
	{
		//pMsgStr = new char[sizeof(FIXFRAME)];
		//memset(pMsgStr,0,sizeof(FIXFRAME));
		PFIXFRAME	pFixFrame = (PFIXFRAME)pMsgStr;


		packetlen=sizeof(FIXFRAME);

		//��ʼ�ַ�
		pFixFrame->startChar = PRO103_FIXFRAME_STARTCHAR;

		//������
		pFixFrame->control.control = controlArea.control;

		//վ��ַ
		pFixFrame->address = (deviceId & 0XFF);

		//У��
		this->doCheck(pMsgStr,packetlen,true,&(pFixFrame->CS));

		//�����ַ�16h
		pFixFrame->endChar = PRO103_ENDCHAR;

		//��ӡ֡��Ϣ
		if(SystemCfger::GetInstance().logLevel >= 5)
		{
			char szInf[250] = {0};
			sprintf(szInf,"Create a fix frame -->\r StartChar : %X\r FunCode : %X\r addr : %X\r CS : %X\r EndCHar : %X\r",
				pFixFrame->startChar,pFixFrame->control.m.fun,pFixFrame->address,pFixFrame->CS,pFixFrame->endChar);
			mylog.WriteLog2File(LogControl::LDETAIL, szInf);
		}

	}
	//�ɱ䳤��
	else
	{
		//����ṹָ��λ��
		PVFRAMEHEAD	pVFrameHead = NULL;
		PASDUID	pAsduID = NULL;
		PASDUINFID	pAsduInfID = NULL;
		PVFRAMEEND	pVFrameEnd = NULL;

		if( MINF_NOTUSER == infoNo ) //��Ϣ���ʾ����������Ϣ������
		{
			packetlen = sizeof(VFRAMEHEAD) + sizeof(ASDUID) + ASDUDataLen	+ sizeof(VFRAMEEND);
			//pMsgStr = new char[packetlen];
			//memset(pMsgStr,0,packetlen);

			pVFrameHead = (PVFRAMEHEAD)pMsgStr;
			pAsduID = (PASDUID)( pMsgStr + sizeof(VFRAMEHEAD) );
			pVFrameEnd = (PVFRAMEEND)( pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) + ASDUDataLen );

			//��ϢԪ�ؼ�
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

			//Ѱ����Ϣ���
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
				//��Ϣ���ʶ
				pAsduInfID->FUN = inf103_array[indexinf].fun;
				pAsduInfID->INF = inf103_array[indexinf].inf;
			}

			//��ϢԪ�ؼ�
			memcpy( (pMsgStr + sizeof(VFRAMEHEAD) + sizeof(ASDUID) + sizeof(ASDUINFID) ),ASDUData,ASDUDataLen);
		}


		if((unsigned int)packetlen > PRO103_VFRAME_MAXSIZE)
		{
			mylog.WriteLog2File(LogControl::LERROR,"data len too big");
			return 	NULL;
		}

		//��ʼ�ַ�
		pVFrameHead->HEAD.startChar1 = pVFrameHead->HEAD.startChar2 = PRO103_VOLATILEFRAME_STARTCHAR;

		//��·����
		int len = packetlen - sizeof(VFRAMEHEAD::tagHead) - sizeof(VFRAMEEND);
		pVFrameHead->HEAD.lengthLow = len & 0X000000FF;
		pVFrameHead->HEAD.lengthHigh = (len & 0X00000F00) >> 8;

		//������
		pVFrameHead->control.control = controlArea.control;

		//���ݵ�Ԫ��ʶ
		pAsduID->TYP = msg103_array[indexmsg].typ;
		pAsduID->S = msg103_array[indexmsg].sq;
		pAsduID->VSQ= msg103_array[indexmsg].num;

		//ԭ����
		pAsduID->COT = COT;

		//��·��ַ
		pVFrameHead->address = (deviceId & 0XFF);
		//ASDU��ַ
		pAsduID->COMADDR = (deviceId & 0XFF);

		//У��
		this->doCheck(pMsgStr,packetlen,false,&(pVFrameEnd->CS ));

		//�����ַ�16h
		pVFrameEnd->endChar= PRO103_ENDCHAR;

		//��ӡ֡��Ϣ
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
	//�ٻ�¼���ļ��б�
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetRecWaveFileList()");

	//�����������
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::GetRecWaveFileList()");
		return false;
	}

	struct CP56Time2a startTime;    //��ѯ��ʼʱ��
	struct CP56Time2a endTime;    //��ѯ����ʱ��
	memcpy(&startTime,pCmd->pParm,sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[sizeof(startTime)],sizeof(endTime));

	int            FCB = -1;	   

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	//bool      isNeedSendPrePack = false;  //�Ƿ��ط�ǰһ֡�����ݣ�FCV=1ʱ��FCBδ�䣬��Ҫ�ط���

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	//int count = -1;//ѭ������
	bool result = true; //���صĽ��

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		//ȡ���ļ��б�
		vector<PCMDFILELIST> fileList;
		if(false == fileCtrl.GetRecWaveFileList(pCmd->destDevId,startTime,endTime,fileList))
		{
			m_errCode = PEC_UNKOWN;
			mylog.WriteLog2File(LogControl::LERROR,"GetRecWaveFileList fail");
			pLinker->Close();   //�ر�����
			return false;
		}

		vector<PCMDFILELIST>::iterator it = fileList.begin();

		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Has fault report need to sent to remot and pause GetRecWaveFileList");
				//��ͣ����
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetRecWaveFileList:send M_CON_NA_3");
			//�ش�ȷ��֡
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		int hasSendItems = 0;   //�Ѿ������͵ĸ���
		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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
			if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetRecWaveFileList:wait for C_PL1_NA_3");
				//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//����ASDU16û����Ϣ���ʶ��������ÿ�ο��Է��͵����ݿ��ԴﵽPRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //���͵�����
			int  sendDataLen =0 ;                   //�������ݵĳ��ȣ���ASDUDATA�ĳ���
			bool isSendAllData = false;             //���������Ƿ��Ѿ��������

			//�ܹ������͵��ļ��б�Ԫ�ظ���
			unsigned short maxFileItemSend = (PRO103_ASDUDATA_MAXSIZE -16)/sizeof(CMDFILELIST);

			//ʵ�ʱ����͵��ļ��б�Ԫ�ظ���
			unsigned short factFileItemSend = 0;

			if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Has fault report need to sent to remot and pause GetRecWaveFileList");
					//���¹��ϣ���ͣ������
					m_errCode = PEC_INTERRUPT;
					*((int*)(pCmd->_state.pStoreParm)) = hasSendItems;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetRecWaveFileList:send ASDU16");

				for(int i=0;i<maxFileItemSend;i++)
				{
					if( it != fileList.end())
					{
						//���ļ�������Ϣ������Ҫ���͵�����buffer��
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

				//����ASDU16
				if( factFileItemSend > 0)
				{
					//���ñ�֡�ļ���
					memcpy(&sendData[0],&factFileItemSend,2);
					sendDataLen +=2;

					//���ÿ�ʼ������ʱ��
					memcpy(&sendData[2],&startTime,sizeof(startTime) );
					memcpy(&sendData[2+sizeof(startTime)],&endTime,sizeof(endTime) );
					sendDataLen += 2*sizeof(startTime);

					//��������
					auto_ptr<string> pDataPack ;    //Ҫ���͵����ݰ�

					if( isSendAllData) //�����Ѿ�������
					{
						pDataPack = auto_ptr<string>(CreateMsg(M_EX16_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}
					else //����δ������
					{
						pDataPack = auto_ptr<string>(CreateMsg(M_EX16_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}

					sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
					if(sendDataLen != msgLen)
					{
						mylog.WriteLog2File(LogControl::LERROR, "send data error");
						pLinker->Close();
						result = false;
						break;
					}

					pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
					if( isSendAllData) //�����Ѿ�������
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
						break;
					}

				} // if( factFileItemSend > 0)
			}//end if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
		}//end while(1)

		//�ͷ��ڴ�
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
		//����������UI����ʱ����
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
	//��ȡ��Ҫ¼������    
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetReportFile()");

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		//Զ���Ѿ�����C_PL2_NA_3���ٻ�2�����ݣ�
		CMDREPORT report;
		memset(&report,0,sizeof(CMDREPORT));

		//��ȡ���ص�¼����
		vector<PREPORTHEAD> *pReportList = SystemCfger::GetInstance().GetReportList( pLinker->GetConnectAddr() );

		auto_ptr<string> pAswer ;

		if(pCmd->_state._state == CmdState::CS_STEP1)
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetReportFile:send M_NV_NA_3");

			if(pReportList != NULL && pReportList->size()>0) //������¼���򱨣��ش�M_NV_NA_3������Ҫ�����ݣ�ACD=1��
			{
				pAswer = auto_ptr<string>( CreateMsg(M_NV_NA_3, pCmd->destDevId,true,0,0,NULL,0,&msgLen) );
			}
			else    //������¼���򱨣��ش�M_NV_NA_3������Ҫ�����ݣ�ACD=0��
			{
				pAswer = auto_ptr<string>( CreateMsg(M_NV_NA_3, pCmd->destDevId,false,0,0,NULL,0,&msgLen) );
			}

			if( pAswer->c_str() != NULL)
			{
				sendDataLen = SendData(pLinker,pAswer->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //���ͻش�֡ 
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
				//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			char sendData[(PRO103_ASDUDATA_MAXSIZE-2) +1] = {0};    //���͵�����(�ų���Ϣ���ʶ��)
			int  sendDataLen =0 ;                   //�������ݵĳ��ȣ���ASDUDATA�ĳ���

			PREPORTHEAD pReport = (*it);                
			memcpy(&sendData[0],&(pReport->report),sizeof(CMDREPORT));
			sendDataLen = sizeof(CMDREPORT);                

			if(pCmd->_state._state == CmdState::CS_STEP3)
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetReportFile:send ASDU12");
				//����ASDU12
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

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
					pLinker->Close();
					return false;
				}
			}//end if(pCmd->_state._state == CmdState::CS_STEP3)
		}//end for(;it != pReportList->end(); it++)

		//�������ݿⲢ����б�
		it = pReportList->begin();
		for(;it != pReportList->end(); it++)
		{
			PREPORTHEAD pReport = (*it);

			//�������ݿ�
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

	}// end if( pCmd->from == CD_REMOTE ) //����������Զ��
	else
	{
		//����������UI����ʱ����
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		//�����������
		if(pCmd->parmLen != sizeof(struct CP56Time2a) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
			return false;
		}

		struct CP56Time2a time;    //ʱ��
		memcpy(&time,pCmd->pParm,sizeof(time));

		//����ϵͳʱ��
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
		//����������UI����ʱ����
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

	ASDUData[0] = 0 ;           //������Ϣ��ʶ��(δ��)
	ASDUData[1] = GINumber;     //ͨ�÷������ݼ���Ŀ
	ASDULen = 2;

	//ͨ�÷�����Ŀ
	for(int i=0; i<(int)GINumber; i++)
	{
		memcpy(&ASDUData[ASDULen],&(GIs[i].GIN),2); //GIN
		ASDULen += 2;
		ASDUData[ASDULen] = GIs[i].KOD ;   //�������
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataType ;//ͨ�÷�������:��������
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataLen ;//ͨ�÷�������:���ݿ��
		ASDULen += 1;
		ASDUData[ASDULen] = GIs[i].gdd.dataNum ;//ͨ�÷�������:������Ŀ
		ASDULen += 1;

		//���������Խṹ������
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
	//�ٻ�һ���ļ��б�
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol103::GetFileList()");

	//�����������
	if(pCmd->parmLen != sizeof(struct CP56Time2a)*2 +100 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
		return false;
	}

	struct CP56Time2a startTime;    //��ѯ��ʼʱ��
	struct CP56Time2a endTime;    //��ѯ����ʱ��
	memcpy(&startTime,pCmd->pParm,sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[sizeof(startTime)],sizeof(endTime));

	char fileName[101] = {0};                   //Ŀ¼����ͨ���
	memcpy(fileName,&pCmd->pParm[sizeof(startTime)*2],100);	    

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	auto_ptr<string> pDataPack ;    //Ҫ���͵����ݰ�
	//bool      isNeedSendPrePack = false;  //�Ƿ��ط�ǰһ֡�����ݣ�FCV=1ʱ��FCBδ�䣬��Ҫ�ط���

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	bool result = true; //���صĽ��

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause GetFileList");
				//���¹��ϣ���ͣ������
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetFileList:send M_CON_NA_3");
			//�ش�ȷ��֡
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		//ȡ���ļ��б�
		vector<PCMDGENERALFILEINF> fileList;
		if(false == fileCtrl.GetFileList(fileName,startTime,endTime,fileList))
		{
			mylog.WriteLog2File(LogControl::LERROR,"GetRecWaveFileList fail");
			pLinker->Close();   //�ر�����
			return false;
		}

		int hasSendItems = 0;   //�Ѿ������͵ĸ���
		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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
			if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetFileList:wait C_PL1_NA_3");
				//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//����ASDU102û����Ϣ���ʶ��������ÿ�ο��Է��͵����ݿ��ԴﵽPRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //���͵�����
			int  sendDataLen =0 ;                   //�������ݵĳ��ȣ���ASDUDATA�ĳ���
			bool isSendAllData = false;             //���������Ƿ��Ѿ��������

			//�ܹ������͵��ļ��б�Ԫ�ظ���
			unsigned short maxFileItemSend = (PRO103_ASDUDATA_MAXSIZE -16)/sizeof(CMDGENERALFILEINF);

			//ʵ�ʱ����͵��ļ��б�Ԫ�ظ���
			unsigned short factFileItemSend = 0;

			if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			{
				if(SendInterruptFrame(pLinker,pCmd))
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause GetFileList");
					//���¹��ϣ���ͣ������
					*((int*)(pCmd->_state.pStoreParm)) = hasSendItems;
					return false;
				}

				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetFileList:send ASDU102");

				for(int i=0;i<maxFileItemSend;i++)
				{
					if( it != fileList.end())
					{
						//���ļ�������Ϣ������Ҫ���͵�����buffer��
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

				//����ASDU102

				//���ñ�֡�ļ���
				memcpy(&sendData[0],&factFileItemSend,2);
				sendDataLen +=2;

				//���ÿ�ʼ������ʱ��
				memcpy(&sendData[2],&startTime,sizeof(startTime) );
				memcpy(&sendData[2+sizeof(startTime)],&endTime,sizeof(endTime) );
				sendDataLen += 2*sizeof(startTime);

				//��������
				if( isSendAllData) //�����Ѿ�������
				{
					string *pTemp = pDataPack.release();
					if(pTemp != NULL)
					{
						delete pTemp;
						pTemp = NULL;
					}
					pDataPack = auto_ptr<string>(CreateMsg(M_EX102_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}
				else //����δ������
				{
					string *pTemp = pDataPack.release();
					if(pTemp != NULL)
					{
						delete pTemp;
						pTemp = NULL;
					}
					pDataPack = auto_ptr<string>(CreateMsg(M_EX102_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
				if(sendDataLen != msgLen)
				{
					mylog.WriteLog2File(LogControl::LERROR, "send data error");
					pLinker->Close();
					result = false;
					break;
				}

				pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
				if( isSendAllData) //�����Ѿ�������
				{
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
			}//end if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
		} //end while(1)

		//�ͷ��ڴ�
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
		//����������UI����ʱ����
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

	//�����������
	if(pCmd->parmLen != 104 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::SetTime()");
		return false;
	}

	char fileName[100] = {0};    //�ļ���������Ŀ¼��
	int startPos = 0;           //��ʼ����λ��
	memcpy(&fileName,pCmd->pParm,sizeof(fileName));
	memcpy(&startPos,&pCmd->pParm[sizeof(fileName)],sizeof(int));

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	bool result = true; //���صĽ��

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			if(SendInterruptFrame(pLinker,pCmd))
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause PullFile");
				//���¹��ϣ���ͣ������
				return false;
			}

			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of PullFile:send M_CON_NA_3");
			//�ش�ȷ��֡
			if(!SendConfirm(pLinker,pCmd->destDevId,true))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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

		//ASDU104���͵��ļ�����ͷ�Ľṹ
		struct _Asdu104FileHead
		{
			char fileName[100];             //�ļ���
			struct CP56Time2a fileTime;     //�ļ�ʱ��
			unsigned int fileLen;           //�ļ�����
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

		//ÿ�οɴ��ļ���ȡ������ļ����ݳ���
		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4 -sizeof(struct _Asdu104FileHead);
		int factReadLen = 0;    //ʵ�ʶ�ȡ�����ݳ���
		unsigned int hasReadLen = startPos; //�ļ��Ѿ���ȡ�ĳ���

		while(1)
		{
			if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of PullFile:wait for C_PL1_NA_3");
				//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
				if(!WantLevelOne(pLinker))
				{
					pLinker->Close();
					result = false;
					break;
				}
				pCmd->_state.ToNextState();
			}

			//����ASDU104û����Ϣ���ʶ��������ÿ�ο��Է��͵����ݿ��ԴﵽPRO103_ASDUDATA_MAXSIZE
			char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};    //���͵�����
			int  sendDataLen =0 ;                   //�������ݵĳ��ȣ���ASDUDATA�ĳ���
			bool isSendAllData = false;             //���������Ƿ��Ѿ��������
			auto_ptr<string> pDataPack ;    //Ҫ���͵����ݰ�

			//����ASDU104
			if(!fileIsExist) //�ļ�������
			{
				memcpy(sendData,fileName,100);
				sendDataLen = sizeof(fileName) + sizeof(struct CP56Time2a) + sizeof(int);
				pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
				isSendAllData = true;
			}
			else
			{
				memcpy(&sendData[0],&startPos,4);                             //��ʼ����λ��
				memcpy(&sendData[4],&fileHead,sizeof(fileHead));              //�ļ�����ͷ

				if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
				{
					if(SendInterruptFrame(pLinker,pCmd))
					{
						mylog.WriteLog2File(LogControl::LDEBUG,"Send fault report and pause PullFile");
						//���¹��ϣ���ͣ������
						*((int*)(pCmd->_state.pStoreParm)) = startPos;
						return false;
					}

					mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of PullFile:send ASDU104");

					factReadLen = fileCtrl.ReadData(startPos,&sendData[4+sizeof(fileHead)],readMaxLen);
					sendDataLen = 4 + sizeof(fileHead) + factReadLen;
					hasReadLen += factReadLen;
					startPos += factReadLen;

					//�ļ��Ѿ�����
					if(factReadLen <= 0 || hasReadLen >= fileHead.fileLen)
					{
						isSendAllData = true;
					}

					//��������
					if( isSendAllData) //�����Ѿ�������
					{
						pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,false,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}
					else //����δ������
					{
						pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,true,0,MINF_NOTUSER,&sendData[0],sendDataLen,&msgLen));
					}

					sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
					if(sendDataLen != msgLen)
					{
						mylog.WriteLog2File(LogControl::LERROR, "send data error");
						pLinker->Close();
						result = false;
						break;
					}

					pCmd->_state.ChangeStateTo(CmdState::CS_STEP2);
					if( isSendAllData) //�����Ѿ�������
					{
						pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
						break;
					}

				}//end if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			}//end if(!fileIsExist) //�ļ�������

		}//end while(1)

		fileCtrl.CloseFile();
		return result;
	}
	else
	{
		//����������UI����ʱ����
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
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	FIXFRAME fixFrame;                  //�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	/*********************��λͨ�ŵ�Ԫ-->******************/

	//�ڴ�Զ�̷���C_RCU_NA_3(��λͨ�ŵ�Ԫ)
	if(false == AnalyseFixFrame(pLinker,&fixFrame,false) ) //��ù̶�����֡
	{
		mylog.WriteLog2File(LogControl::LERROR,"get fix frame error");
		return false;
	}

	//�Ƿ����ڴ�������
	if( fixFrame.control.c.fun != CFUNID_0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"want the command C_RCU_NA_3,but it isn't");
		return false;
	}

	//����M_CON_NA_3��ȷ�ϣ�ACD=1��
	if(!SendConfirm(pLinker,0,true))
	{
		return false;
	}

	//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
	if(!WantLevelOne(pLinker))
	{
		return false;
	}

	char sendData[(PRO103_ASDUDATA_MAXSIZE-2) +1] = {0};    //���͵�����(�ų���Ϣ���ʶ��)

	sendData[0] = 0X03;      //���ݼ���
	sendDataLen = 1;

	//����M_IRC_NA_3��ASDU5��COT=��λͨ�ŵ�Ԫ��
	auto_ptr<string> pDataPack(CreateMsg(M_IRC_NA_3, 0,false,0X04,MINF_242,&sendData[0],sendDataLen,&msgLen));
	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
	if(sendDataLen != msgLen)
	{
		mylog.WriteLog2File(LogControl::LERROR, "send data error");
		return false;
	}

	/*********************��λ֡����λ-->******************/

	//�ڴ�Զ�̷���C_RFB_NA_3����λ֡����λ��
	if(false == AnalyseFixFrame(pLinker,&fixFrame,false) ) //��ù̶�����֡
	{
		mylog.WriteLog2File(LogControl::LERROR,"get fix frame error");
		return false;
	}

	//�Ƿ����ڴ�������
	if( fixFrame.control.c.fun != CFUNID_7 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"want the command C_RFB_NA_3,but it isn't");
		return false;
	}

	//����M_CON_NA_3��ȷ�ϣ�ACD=1��
	if(!SendConfirm(pLinker,0,true))
	{
		return false;
	}

	//�ڴ�Զ�̷���C_PL1_NA_3(�ٻ�1������)
	if(!WantLevelOne(pLinker))
	{
		return false;
	}

	//����M_IRF_NA_3��ASDU5��COT=��λ֡����λ��
	memset(&sendData[0],0,sizeof(sendData));
	sendData[0] = 0X03;      //���ݼ���
	sendDataLen = 1;

	string *pTemp = pDataPack.release();
	if(pTemp != NULL)
	{
		delete pTemp;
		pTemp = NULL;
	}
	pDataPack = auto_ptr<string>(CreateMsg(M_IRC_NA_3, 0,false,0X03,MINF_242,&sendData[0],sendDataLen,&msgLen));
	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
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

    //��ѯ¼��������״̬��Ϣ
    bool GetDauInf(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetDauInf(pLinker,&cmd);
    }

    //�ٻ�¼���ļ�
    bool GetRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFile(pLinker,&cmd);
    }

	//�ٻ���ͨ��¼���ļ�
    bool GetSplitRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetSplitRecWaveFile(pLinker,&cmd);
    }

    //�ٻ�¼���ļ��б�
    bool GetRecWaveFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFileList(pLinker,&cmd);
    }

    //��Ҫ¼������
    bool GetReportFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetReportFile(pLinker,&cmd);
    }

    //�ٻ�ͨ���ļ��б�
    bool GetFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetFileList(pLinker,&cmd);
    }

    //��ʱ��ʱ��ͬ����
    bool SetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->SetTime(pLinker,&cmd);
    }

    //��ȡʱ��
    bool GetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
		return pProtocol->GetTime(pLinker,&cmd);
    }

    //ֹͣ¼��
    bool StopRecWave(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StopRecWave(pLinker,&cmd);
    }

    //�ֶ�����¼��
    bool StartRecByManual(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StartRecByManual(pLinker,&cmd);
    }

    //��ʱ����¼��
    bool StartRecByTimer(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->StartRecByTimer(pLinker,&cmd);
    }

    //����̬��¼����
    bool GetContData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetContData(pLinker,&cmd);
    }

    //��ȡ�ļ�
    bool PullFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->PullFile(pLinker,&cmd);
    }

    //�´��ļ�
    bool PushFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->PushFile(pLinker,&cmd);
    }

    //��ʵʱ����
    bool ReadRltData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->ReadRltData(pLinker,&cmd);
    }

    //��ʵ��Ư�̶�
    bool ReadZeroOffset(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->ReadZeroOffset(pLinker,&cmd);
    }

	//��ʼ�����ʵ��
	bool StartGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->StartGnExperiment(pLinker, &cmd);
	}

	//ֹͣ�����ʵ��
	bool StopGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->StopGnExperiment(pLinker, &cmd);
	}

    //�����ϴ�
    bool AutoSend(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->AutoSend(pLinker,&cmd);
    }

    //��װ��״̬
    bool GetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetDeviceState(pLinker,&cmd);
    }

    //����װ��״̬
    bool SetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->SetDeviceState(pLinker,&cmd);
    }

    //Ӳ������
    bool OperateDevice(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->OperateDevice(pLinker,&cmd);
    }

// }}}USR
// }}}RME

// }}}RME
