// {{{RME classifier 'Logical View::Protocol104'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol104.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�Protocol104.cpp
* �ļ���ʶ��
* ժ    Ҫ��104Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

#include "Linker.h"
#include "Task.h"
#include "StringRes.h"

PRO104UFRAME2CMD pro104UFrame2Cmd[] = {
	{PRO104_TESTFR_ACT,CMD_104_CONFIRM_TESTFR,ConfirmTestFR,(CStringRes::GetInstance())->GetStrById(IDS_RESP_TESTFRAME,"Ӧ�����֡")},
	{PRO104_STOPDT_ACT,CMD_104_STOPDT,StopDT,(CStringRes::GetInstance())->GetStrById(IDS_STOP_DT,"ֹͣ���ݴ���")},
	{PRO104_STARTDT_ACT,CMD_104_STARTDT,StartDT,(CStringRes::GetInstance())->GetStrById(IDS_START_DT,"�������ݴ���")},
	{0XFF,CMD_104_SEND_TESTFR,SendTestFR,(CStringRes::GetInstance())->GetStrById(IDS_SEND_TESTFRAME,"���Ͳ���֡")}
};

ASDUSTRUCT104 asduStruct104[] = {
	{STRNO_GLIB,0x81,255,0,"ȫ�ֹ���"},
	{STRNO_GEN,0x81,254,0,"ͨ�ù���"},
	{STRNO_ALL_GROUP_TITLE,0x81,254,240,"��ȫ����ı���"},
	{STRNO_GROUP_ALL_ENTRY_VAL,0x81,254,241,"һ�����ȫ����Ŀ��ֵ������"},
	{STRNO_ONE_ENTRY_DIR,0x81,254,243,"��������Ŀ��Ŀ¼"},
	{STRNO_ONE_ENTRY_VAL,0x81,254,244,"��������Ŀ��ֵ������"}
};

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol104::~Protocol104( void )
{
}
// }}}RME

// {{{RME operation 'Protocol104()'
Protocol104::Protocol104( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR
	:P104_K(156),P104_W(100),P104_T1(15),P104_T2(5),P104_T3(25)
	// }}}USR
	// }}}RME
{
	// {{{USR
	Init();
	// }}}USR
}
// }}}RME

void Protocol104::Init()
{
	m_iLenOfApdulen = 1;

	m_iHasSendNum = 0;
	m_iHasReceivNum = 0;
	m_iHasAckNum = 0;
	m_iNoAckNum = 0;

	m_dwBeginT1Time = 0;
	m_dwBeginT2Time = 0;
	m_dwBeginT3Time = 0;

	m_bCanSendData = false;
}

int Protocol104::SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd)
{
	m_dwBeginT2Time = 0;
	int count = 0;

	//δ��ȷ�ϵ�֡�����˲���K����ֹͣ���ͣ��ȴ��Է�ȷ��
	while( ((m_iHasSendNum>m_iHasAckNum) && ((m_iHasSendNum - m_iHasAckNum) >= P104_K) ) ||
			((m_iHasSendNum<m_iHasAckNum) && ((m_iHasSendNum+0x7FFF-m_iHasAckNum) >= P104_K) ) )
	{
		count++;
		if( m_dwBeginT2Time == 0)
		{
			m_dwBeginT2Time = time(NULL);//t2��ʱ��ʼ
		}
		else
		{
			unsigned long dwNowTime = time(NULL);
			if( (dwNowTime - m_dwBeginT2Time)/1000 >= P104_T2)
			{
				//��ʱδȷ��
				m_errCode = PEC_TIME_OUT;
				pLinker->Close();
				return -1;
			}
		}

		Task *pNewTask = Decode(pLinker);
		if(pNewTask != NULL)
		{
			delete pNewTask;
			pNewTask = NULL;
		}
		if( pLinker->IsConnect() == false )
		{
			mylog.WriteLog2File(LogControl::LERROR,"Protocol104::SendIFrame( ):Wait for comfirm fail");
			return -1;
		}
	}

	string sSendData = CreateIFrame(pAsdu,iAsduLen);
	if(sSendData.size() != SendData(pLinker,sSendData.c_str(),sSendData.size(),P104_T1*1000))
	{
		pLinker->Close();
		mylog.WriteLog2File(LogControl::LERROR,"Protocol104::SendIFrame( ):Send ack frame error");
		return -1;
	}
	m_iHasSendNum++;
	return 1;
}

// {{{RME operation 'Decode(Linker*)'
Task * Protocol104::Decode( Linker * pLinker )
{
char	szFrameData[4096+1] = {0};
	int		iFrameLen = 4096;
	APCI4Prot104	apci(m_iLenOfApdulen);
	PASDUID104		pAsduId = NULL;
	PASDUINFID104	pAsduInfoId = NULL;
	char		*pAsduData = NULL;
	int			iAsduDataLen = 0;
	int iResult = 0;
	unsigned int nWaitTime = 50;

	while(1)
	{
		iResult = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,nWaitTime);
		if(iResult == -1) //�д���
		{
			pLinker->Close();
			return NULL;
		}
		else if(iResult == 0) //û������
		{
			return NULL;
		}

		if(apci.GetFrameType() == APCI4Prot104::FT_S)
		{
			//s��ʽ֡�������������
			nWaitTime = 1;
			continue;
		}
		else if(apci.GetFrameType() == APCI4Prot104::FT_U)
		{
			return AnalyseTask(&apci,pLinker);
		}
	} //end while(1)

	return NULL;

}
// }}}RME

// {{{RME operation 'AnalyseTask(APCI4Prot104*,Linker*)'
Task * Protocol104::AnalyseTask( APCI4Prot104 * pAPCI, Linker * pLinker )
{
	// {{{USR
		if(pAPCI == NULL || pAPCI->GetFrameType() != APCI4Prot104::FT_U)
		{
			return NULL;
		}

		int i;
	    int mapCount = (sizeof(pro104UFrame2Cmd)/sizeof(PRO104UFRAME2CMD));
		for(i=0;i<mapCount;i++)
		{
			if(pro104UFrame2Cmd[i].control1 == pAPCI->controlArea.control1)
			{
				break;
			}
		}

		if( i >= mapCount)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			return NULL;
		}

		//��������
		Task *pTask = new Task();
	    pTask->Linker = pLinker;
	    pTask->cmd.destDevId = 0;
	    pTask->cmd.from = CD_REMOTE;
	    pTask->cmd.to = CD_NEURON;
		pTask->cmd.funCode = pro104UFrame2Cmd[i].cmdFunCode;
		strcpy(pTask->cmd.describe,pro104UFrame2Cmd[i].cmdName);

		return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol104::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	int i;
	int mapCount = (sizeof(pro104UFrame2Cmd)/sizeof(PRO104UFRAME2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro104UFrame2Cmd[i].cmdFunCode == cmd.funCode )
		{
			break;
		}
	}

	if( i < mapCount)
	{
		return pro104UFrame2Cmd[i].exuteCmd(this,pLinker,cmd);
	}

	return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseFrame(Linker *,char*,int*,APCI4Prot104*,PASDUID104*,PASDUINFID104*,char**,int*,unsigned int)'
int Protocol104::AnalyseFrame( Linker * pLinker, char * framePacket, int * frameLen, APCI4Prot104 * pAPCI, PASDUID104 * pOutASDUID, PASDUINFID104 * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, unsigned int uiTimeOut )
{
	// {{{USR
	    assert(pLinker != NULL);
	    assert(framePacket != NULL);
	    assert(frameLen != NULL);
		assert(pAPCI != NULL);
		char szLogInfo[255] ={0};
	    
	    int iAPCILen = P104_STARTCHAR_LEN + m_iLenOfApdulen + P104_CONTROLAREA_LEN;//APCI�ĳ���
	    int  readLen=0;
		int ASDUIdLen   = ASDUID104_LEN;   //ASDU id ����
		int ASDUInfLen   = ASDUINFID104_LEN;   //ASDU ��Ϣ�� ����
		
		//�ȶ�APCI������
		readLen = ReceiveData(pLinker,framePacket,iAPCILen,uiTimeOut );
		if(readLen < 0 )
		{
			mylog.WriteLog2File(LogControl::LERROR,"read frame head error in Protocol104::AnalyseFrame()");
			return -1;
		}
		else if(readLen != iAPCILen )
		{
			//mylog.WriteLog2File(LogControl::LDEBUG,"Time out when read frame head in Protocol104::AnalyseFrame()");
			return 0;
		}

		*frameLen = iAPCILen;

		//������APCIͷ
		if( !pAPCI->Decode(framePacket,iAPCILen) )
		{
			m_errCode = PEC_ERR_FRAME;
			sprintf(szLogInfo,"decode the APCI error(len:%d) in Protocol104::AnalyseFrame()",iAPCILen);
			mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
			mylog.WriteData2Log(LogControl::LERROR,"FrameH:",framePacket,iAPCILen);

			readLen = ReceiveData(pLinker,framePacket,1024,100 );
			if( readLen > 0)
			{
				mylog.WriteData2Log(LogControl::LERROR,"FrameD:",framePacket,readLen);
			}
			return -1;
		}

		//���ݲ�ͬ��֡���ͽ��д���
		int iSendNum = 0;
		switch(pAPCI->GetFrameType())
		{
		case APCI4Prot104::FT_I:	//I��ʽ֡
			iSendNum = pAPCI->GetSendNum();
			if(m_iHasReceivNum != iSendNum)
			{
				//�������кŲ���ȷ
				sprintf(szLogInfo,"Error receivNum,iSendNum=%d,m_iHasReceivNum=%d",iSendNum,m_iHasReceivNum);
				mylog.WriteLog2File(LogControl::LERROR,szLogInfo);
				return -1;
			}
			else
			{
				m_iHasReceivNum++;
				m_iNoAckNum++;
			}

			//�õ�ȷ�ϵ����к�
			m_iHasAckNum = pAPCI->GetReceivNum();

			sprintf(szLogInfo,"Got I frame,m_iNoAckNum=%d,m_iHasReceivNum=%d,m_iHasAckNum=%d",m_iNoAckNum,m_iHasReceivNum,m_iHasAckNum);
			mylog.WriteLog2File(LogControl::LDETAIL,szLogInfo);

			break;

		case APCI4Prot104::FT_S:	//S��ʽ֡
			//�õ�ȷ�ϵ����к�
			m_iHasAckNum = pAPCI->GetReceivNum();

			sprintf(szLogInfo,"Got S frame,m_iNoAckNum=%d,m_iHasReceivNum=%d,m_iHasAckNum=%d",m_iNoAckNum,m_iHasReceivNum,m_iHasAckNum);
			mylog.WriteLog2File(LogControl::LDETAIL,szLogInfo);

			break;
		case APCI4Prot104::FT_U:	//U��ʽ֡
			sprintf(szLogInfo,"Got U frame from linkhandle:%d,m_iNoAckNum=%d,m_iHasReceivNum=%d,m_iHasAckNum=%d",
				pLinker->GetHandle(),m_iNoAckNum,m_iHasReceivNum,m_iHasAckNum);
			mylog.WriteLog2File(LogControl::LDEBUG,szLogInfo);
			break;
		default:
			sprintf(szLogInfo,"Got unkown frame,m_iNoAckNum=%d,m_iHasReceivNum=%d,m_iHasAckNum=%d",m_iNoAckNum,m_iHasReceivNum,m_iHasAckNum);
			mylog.WriteLog2File(LogControl::LWARNING,szLogInfo);
			return -1;
		}

		 *frameLen = P104_STARTCHAR_LEN + m_iLenOfApdulen + pAPCI->iApduLen;

		//����I��ʽ֡����û��ASDU
		int iASDULen = pAPCI->iApduLen - P104_CONTROLAREA_LEN;
		if(pAPCI->GetFrameType() != APCI4Prot104::FT_I || iASDULen == 0 )
		{
			m_dwBeginT3Time = 0;//t3��ʱ����
			m_dwBeginT1Time = 0;//t1��ʱ����

			return 1;
		}

		//��������֡����
		PASDUID104 pASDUID = (PASDUID104)&framePacket[iAPCILen];	//ASDU id
	    PASDUINFID104 pASDUInfID = (PASDUINFID104)&framePacket[iAPCILen + ASDUID104_LEN];	//ASDU ��Ϣ��
		int AsduDataLen = iASDULen - ASDUID104_LEN - ASDUINFID104_LEN;//ASDU��ϢԪ�صĴ�С

		readLen = ReceiveData(pLinker,&framePacket[iAPCILen],iASDULen,0);
		if(readLen != iASDULen )
		{
			mylog.WriteLog2File(LogControl::LERROR,"read ASDU error in Protocol104::AnalyseFrame()");
			return -1;
		}

		if(AsduDataLen<0)
		{
			m_errCode = PEC_ERR_FRAME;
			mylog.WriteLog2File(LogControl::LERROR,"error packet length");
			return -1;
		}
		//���û��Ӧ��Է���I֡��������W����һS֡��Է�ȷ��
		if( m_iNoAckNum >= P104_W )
		{
			string sFram_s = CreateSFrame();
			if( sFram_s.length() != SendData(pLinker,sFram_s.c_str(),sFram_s.length(),5000))
			{
				mylog.WriteLog2File(LogControl::LERROR,"Send S frame fail in Protocol104::AnalyseFrame()");
				return -1;
			}
			m_iNoAckNum = 0;	//û��Ӧ��Է���I֡������
		}
		m_dwBeginT3Time = 0;//t3��ʱ����
		m_dwBeginT1Time = 0;//t1��ʱ����

		*pAsduDataLen = AsduDataLen;
		char *pAsduData = &framePacket[iAPCILen + ASDUIdLen + ASDUInfLen];
	    
		if(pOutASDUID != NULL)
			*pOutASDUID = pASDUID;
		if(pOutASDUInfID != NULL)
			*pOutASDUInfID = pASDUInfID;
		if(pOutAsduData != NULL)
			*pOutAsduData = pAsduData;

		return 1;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
bool Protocol104::GetRecWaveFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
bool Protocol104::GetRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'StartDT(Linker *,struct Command*)'
bool Protocol104::StartDT( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	m_bCanSendData = true;

	mylog.WriteLog2File(LogControl::LDEBUG,"Begin answer for STARTDT");

	//����STARTDTȷ��֡
	string sSendData = CReateUFrame(1);
	int iSendLen = SendData(pLinker,sSendData.c_str(),sSendData.length(),SystemCfger::GetInstance().timeOut);
	if(iSendLen == sSendData.length())
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Answer for STARTDT success");
		return true;
	}

	mylog.WriteLog2File(LogControl::LDEBUG,"Answer for STARTDT fail");
	return false;
	// }}}USR
}
// }}}RME

// {{{RME operation 'StopDT(Linker *,struct Command*)'
bool Protocol104::StopDT( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	m_bCanSendData = false;

	mylog.WriteLog2File(LogControl::LDEBUG,"��ʼ��ӦSTOPDT����");

	//����STOPDTȷ��֡
	string sSendData = CReateUFrame(3);	
	int iSendLen = SendData(pLinker,sSendData.c_str(),sSendData.length(),SystemCfger::GetInstance().timeOut);
	if(iSendLen == sSendData.length())
	{
		return true;
	}

	mylog.WriteLog2File(LogControl::LDEBUG,"��ʼ��ӦSTOPDT����ʧ��");
	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'SendTestFR(Linker *,struct Command*)'
bool Protocol104::SendTestFR( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LDEBUG,"Enter Protocol104::SendTestFR(");

	//���Ͳ���֡
	string sSendData = CReateUFrame(4);	
	int iSendLen = SendData(pLinker,sSendData.c_str(),sSendData.length(),SystemCfger::GetInstance().timeOut);
	if(iSendLen != sSendData.length())
	{
		//����ʧ�ܣ���ر�����
		mylog.WriteLog2File(LogControl::LERROR,"SendTestFR fail");
		pLinker->Close();
		return false;
	}

	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ConfirmTestFR(Linker *,struct Command*)'
bool Protocol104::ConfirmTestFR( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	mylog.WriteLog2File(LogControl::LDEBUG,"Enter Protocol104::ConfirmTestFR()");

	m_dwBeginT3Time = 0;//t3��ʱ����
	m_dwBeginT1Time = 0;//t1��ʱ����

	//���Ͳ���ȷ��֡
	string sSendData = CReateUFrame(5);	
	int iSendLen = SendData(pLinker,sSendData.c_str(),sSendData.length(),SystemCfger::GetInstance().timeOut);
	if(iSendLen != sSendData.length())
	{
		mylog.WriteLog2File(LogControl::LERROR,"ConfirmTestFR fail");
		pLinker->Close();
		return false;
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'CreateIFrame(const char*,int)'
string Protocol104::CreateIFrame( const char * pAsdu, int iAsduLen )
{
	// {{{USR
	APCI4Prot104 apci(m_iLenOfApdulen);
	apci.SetFrameType(APCI4Prot104::FT_I);
	apci.startChar = 0x68;
	apci.iApduLen = P104_CONTROLAREA_LEN + iAsduLen;
	apci.SetSendNum(m_iHasSendNum);
	apci.SetReceivNum(m_iHasReceivNum);

	//printf("m_iHasSendNum=%d\r\n",m_iHasSendNum);

	string sFrameData = apci.Encode();
	int iAPCILen = apci.GetApciLen();
	sFrameData.resize(iAPCILen+iAsduLen);
	memcpy(&(sFrameData.operator [](iAPCILen)),pAsdu,iAsduLen);

	return sFrameData;
	// }}}USR
}
// }}}RME

// {{{RME operation 'CreateSFrame()'
string Protocol104::CreateSFrame( void )
{
	// {{{USR
	APCI4Prot104 apci(m_iLenOfApdulen);
	apci.SetFrameType(APCI4Prot104::FT_S);
	apci.startChar = 0x68;
	apci.iApduLen = P104_CONTROLAREA_LEN;
	apci.controlArea.control1 = 1;
	apci.SetReceivNum(m_iHasReceivNum);

	return apci.Encode();
	// }}}USR
}
// }}}RME

// {{{RME operation 'CReateUFrame(int)'
string Protocol104::CReateUFrame( int iType )
{
	// {{{USR
	APCI4Prot104 apci(m_iLenOfApdulen);
	apci.startChar = 0x68;
	apci.iApduLen = P104_CONTROLAREA_LEN;
	apci.controlArea.control1 = 3 + ( 1<<(2+iType) ) ;
	apci.controlArea.control2 = 0;
	apci.controlArea.control3 = 0;
	apci.controlArea.control4 = 0;

	return apci.Encode();
	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle(Linker*)'
Task * Protocol104::DoIdle( Linker * pLinker )
{
	// {{{USR
	if(m_dwBeginT3Time == 0 && m_dwBeginT1Time == 0 )
	{
		m_dwBeginT3Time = ::GetTickCount();//t3��ʼ��ʱ
		return NULL;
	}
	else if(m_dwBeginT3Time > 0 && m_dwBeginT1Time == 0 )
	{
		//t3�Ѿ���ʱ�������t3�Ƿ��Ѿ���ʱ
		DWORD dwNowTime = GetTickCount();
		if( ((dwNowTime - m_dwBeginT3Time)/1000) < P104_T3)
		{
			//t3δ��ʱ
			return NULL;
		}

		//t3�Ѿ���ʱ�����Ͳ���֡

		//��������
		Task *pTask = new Task();
	    pTask->Linker = pLinker;
		pTask->cmd.destDevId = pLinker->GetDevId();
	    pTask->cmd.from = CD_NEURON;
	    pTask->cmd.to = CD_ASDU;
		pTask->cmd.funCode = CMD_104_SEND_TESTFR;
		strcpy(pTask->cmd.describe,(CStringRes::GetInstance())->GetStrById(IDS_SEND_TESTFRAME,"���Ͳ���֡"));

		m_dwBeginT3Time = 0;	//t3��ʱ����
		m_dwBeginT1Time = GetTickCount(); //t1��ʱ��ʼ

		return pTask;
	}
	else if(m_dwBeginT1Time > 0)
	{
		//t1�Ѿ���ʱ�������1�Ƿ��Ѿ���ʱ
		DWORD dwNowTime = GetTickCount();
		if( ((dwNowTime - m_dwBeginT1Time)/1000) < P104_T1)
		{
			//t1δ��ʱ
			return NULL;
		}

		//t1�Ѿ���ʱ����ر�����
		m_dwBeginT3Time = 0;
		m_dwBeginT1Time = 0;
		pLinker->Close();
		return NULL;
	}

	return NULL;
	// }}}USR
}
// }}}RME

// {{{RME operation 'EncodeAsdu(unsigned char,int,unsigned char,unsigned short,const char*,int)'
string Protocol104::EncodeAsdu( unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen )
{
	// {{{USR
	int indexmsg=0;
	ASDUID104 asduId;
	memset(&asduId,0,ASDUID104_LEN);
	ASDUINFID104 asduInfId;
	memset(&asduInfId,0,ASDUINFID104_LEN);

	int strCount = (sizeof(asduStruct104)/sizeof(ASDUSTRUCT104));

	//Ѱ����Ϣ��
	for(indexmsg=0;indexmsg<strCount;indexmsg++)
	{
		if(asduStruct104[indexmsg].no == asduStructNo)
		{
			break;
		}
	}

	if(indexmsg>=strCount)
	{
		mylog.WriteLog2File(LogControl::LERROR,"struct no not found");
		return "";
	}

	asduId.TYP = type;
	asduId.VSQ = asduStruct104[indexmsg].VSQ;
	asduId.COT = cot;
	asduId.COMADDR = addr;
	asduInfId.FUN = asduStruct104[indexmsg].FUN;
	asduInfId.INF = asduStruct104[indexmsg].INF;

	string sResult("");
	sResult.resize(ASDUID104_LEN  +ASDUINFID104_LEN + iAsduDataLen);
	memcpy(&(sResult.operator [](0)),&asduId,ASDUID104_LEN);
	memcpy(&(sResult.operator [](ASDUID104_LEN)),&asduInfId,ASDUINFID104_LEN);
	memcpy(&(sResult.operator [](ASDUID104_LEN + ASDUINFID104_LEN)),pAsduData,iAsduDataLen);

	return sResult;

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

	//���Ͳ���֡
	bool SendTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->SendTestFR(pLinker,&cmd);
	}

	//Ӧ�����֡
	bool ConfirmTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->ConfirmTestFR(pLinker,&cmd);;
	}

	//�������ݴ���
	bool StartDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->StartDT(pLinker,&cmd);;
	}

	//ֹͣ���ݴ���
	bool StopDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd)
	{
		return pProtocol->StopDT(pLinker,&cmd);;
	}

// }}}USR
// }}}RME

// }}}RME
