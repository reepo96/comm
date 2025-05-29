// {{{RME classifier 'Logical View::Protocol104_ZJ'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Protocol104_ZJ.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104_ZJ.h>
#include <DataControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�Protocol104_ZJ.cpp
* �ļ���ʶ��
* ժ    Ҫ���㽭104Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��4��
*
**************************************************************************/

#include "Linker.h"
#include "Task.h"
#include "FileControl.h"
#include "DataControl.h"

#include "StringRes.h"

PRO104ZJFRAME2CMD pro104ZJFR2CmdMap[] = {
	{PRO104_ASDU15,CMD_GETWAVEFILELIST,GetRecWaveFileList,(CStringRes::GetInstance())->GetStrById(IDS_CALL_REC_FILELIST,"��¼���ļ��б�")},
	{PRO104_ASDU13,CMD_GETWAVEFILE,GetRecWaveFile,(CStringRes::GetInstance())->GetStrById(IDS_CALL_REC_FILE,"�ٻ�¼���ļ�")},
	{PRO104_ASDU113,CMD_GETREPORTFILE,GetReportFile,(CStringRes::GetInstance())->GetStrById(99997,"��¼�����ļ�")},
	{PRO104_ASDU17,CMD_104ZJ_GET_MARKEVN_LIST,GetMarkEventList,(CStringRes::GetInstance())->GetStrById(IDS_GET_EVENT_LIST,"���¼��б�")},
	{0,CMD_AUTOSEND_LINKSTATE,AutoSendDevState,(CStringRes::GetInstance())->GetStrById(IDS_AUTO_SEND_DEVSTAT,"�Զ��ϴ��豸״̬")},
	{0,CMD_AUTOSEND_MARKEVENT,AutoSendMarkEvent,(CStringRes::GetInstance())->GetStrById(IDS_AUTO_SEND_EVENT,"�Զ��ϴ�����¼�")},
	{0,CMD_CREATEREPORTFILE,CreateReportFile,(CStringRes::GetInstance())->GetStrById(IDS_CREATE_RPT,"���ɹ��ϼ��ļ�")},
	{0,CMD_AUTOSEND_REPORTFILE,AutoSendReportFile,(CStringRes::GetInstance())->GetStrById(IDS_AUTO_SEND_RPT,"�Զ��ϴ����ϼ��ļ�")}
};

DWORD WINAPI ReadWavFileData(void *pParm); //��¼���ļ������߳�

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Protocol104_ZJ::~Protocol104_ZJ( void )
{
}
// }}}RME

// {{{RME operation 'Protocol104_ZJ()'
Protocol104_ZJ::Protocol104_ZJ( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_iLenOfApdulen = 2;
	m_dwBeginIdleTime = 0;
	m_bCanSendData = true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseFrame(Linker *,char*,int*,APCI4Prot104*,PASDUID104*,PASDUINFID104*,char**,int*,unsigned int)'
int Protocol104_ZJ::AnalyseFrame( Linker * pLinker, char * framePacket, int * frameLen, APCI4Prot104 * pAPCI, PASDUID104 * pOutASDUID, PASDUINFID104 * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, unsigned int uiTimeOut )
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

		//��Э�鲻���Ϲ淶�����Բ�����������
		pAPCI->NoAnalysCtrlArea();

		//������APCIͷ
		if( !pAPCI->Decode(framePacket,iAPCILen) )
		{
			m_errCode = PEC_ERR_FRAME;
			mylog.WriteLog2File(LogControl::LERROR,"decode the APCI error in Protocol104::AnalyseFrame()");
			return -1;
		}

		if(pAPCI->iApduLen <= 4)
		{
			return 1;
		}

		*frameLen = P104_STARTCHAR_LEN + m_iLenOfApdulen + pAPCI->iApduLen;
		int iASDULen = pAPCI->iApduLen - P104_CONTROLAREA_LEN;

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

// {{{RME operation 'Decode(Linker*)'
Task * Protocol104_ZJ::Decode( Linker * pLinker )
{
	// {{{USR
	char	szFrameData[PRO104ZJ_FRAME_MAXSIZE+1] = {0};
	int		iFrameLen = PRO104ZJ_FRAME_MAXSIZE;
	APCI4Prot104	apci(2);
	PASDUID104		pAsduId = NULL;
	PASDUINFID104	pAsduInfoId = NULL;
	char		*pAsduData = NULL;
	int			iAsduDataLen = 0;
	int iResult = 0;

	while(1)
	{
		iResult = AnalyseFrame(pLinker,szFrameData,&iFrameLen,&apci,&pAsduId,&pAsduInfoId,&pAsduData,&iAsduDataLen,50);
		if(iResult == -1) //�д���
		{
			pLinker->Close();
			return NULL;
		}
		else if(iResult == 0) //û������
		{
			return NULL;
		}

		if(apci.GetFrameType() == APCI4Prot104::FT_I)
		{
			Task *pTask =  AnalyseTask(pAsduId,pAsduData,iAsduDataLen,pLinker);
			if(pTask == NULL)
			{
				//�Ǳ�ϵͳ��֧�ֵ�����,�򽫴���ԭ����Ϊ44��ԭ֡�ͻ�
				apci.SetSendNum(m_iHasSendNum);//���÷������к�
				apci.SetReceivNum(m_iHasReceivNum);//���ý������к�
				memcpy(szFrameData,apci.Encode().c_str(),apci.GetApciLen());

				pAsduId->COT = 44;//δ֪�����ͱ�ʶ

				if(iFrameLen == pLinker->SendData(szFrameData,iFrameLen,0))
				{
					m_iHasSendNum++; //�������кż�1
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
			//s��ʽ֡�������������
			continue;
		}
	} //end while(1)
	// }}}USR
}
// }}}RME

// {{{RME operation 'AnalyseTask(PASDUID104,char*,int,Linker*)'
Task * Protocol104_ZJ::AnalyseTask( PASDUID104 pAsduId, char * pAsduData, int iAsduDataLen, Linker * pLinker )
{
	// {{{USR
		if(pAsduId == NULL || pAsduData == NULL)
		{
			return NULL;
		}

		int i;
	    int mapCount = (sizeof(pro104ZJFR2CmdMap)/sizeof(PRO104ZJFRAME2CMD));
		for(i=0;i<mapCount;i++)
		{
			if(pro104ZJFR2CmdMap[i].type == pAsduId->TYP)
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
		pTask->cmd.destDevId = (pAsduId->COMADDR&0XFF00)>>8;
	    pTask->cmd.from = CD_REMOTE;
	    pTask->cmd.to = CD_NEURON;
		pTask->cmd.funCode = pro104ZJFR2CmdMap[i].cmdFunCode;
		strcpy(pTask->cmd.describe,pro104ZJFR2CmdMap[i].cmdName);

		if(pTask->cmd.funCode == CMD_GETWAVEFILELIST || pTask->cmd.funCode == CMD_104ZJ_GET_MARKEVN_LIST)
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

			char *pTmp = pTask->cmd.pParm;

			if(pTask->cmd.funCode == CMD_GETWAVEFILELIST || pTask->cmd.funCode == CMD_104ZJ_GET_MARKEVN_LIST)
			{
				if(pAsduId->COT == 0x14)
				{
					pTask->cmd.pParm[0] = 1;	//ֻ�����й��ϵ�¼���ļ��б�
				}
				else
				{
					pTask->cmd.pParm[0] = 2;	//�������е�¼���ļ��б�
				}

				pTmp++;
			}

			memcpy(pTmp,pAsduData+1,pTask->cmd.parmLen);
		}

		return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
bool Protocol104_ZJ::ExecuteCmd( Linker * pLinker, struct Command & cmd )
{
	// {{{USR
	int i;
	int mapCount = (sizeof(pro104ZJFR2CmdMap)/sizeof(PRO104ZJFRAME2CMD));
	for(i=0;i<mapCount;i++)
	{
		if(pro104ZJFR2CmdMap[i].cmdFunCode == cmd.funCode )
		{
			break;
		}
	}

	if( i >= mapCount)
	{
		return Protocol104::ExecuteCmd(pLinker,cmd);
	}

	return pro104ZJFR2CmdMap[i].exuteCmd(this,pLinker,cmd);
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
bool Protocol104_ZJ::GetRecWaveFileList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//�ٻ�¼���ļ��б�
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol104_ZJ::GetRecWaveFileList()");

	//�����������
	if(pCmd->parmLen != 1 + sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol103::GetRecWaveFileList()");
		return false;
	}

	unsigned char ucFlag = 1;	//��־:1-ֻ�����й��ϵ�¼���ļ��б�,2-�������е�¼���ļ��б�
	unsigned char COT = 0;
	struct CP56Time2a startTime;    //��ѯ��ʼʱ��
	struct CP56Time2a endTime;    //��ѯ����ʱ��

	memcpy(&ucFlag,pCmd->pParm,1);
	memcpy(&startTime,&pCmd->pParm[1],sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[sizeof(startTime)+1],sizeof(endTime));

	if(ucFlag == 1)
	{
		COT = 0x14;
	}
	else
	{
		COT = 0x09;
	}

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	bool result = true; //���صĽ��

	vector< DataControl::FILEINFO > fileList;
	DataControl dataCtrl;

	if(false == dataCtrl.GetRecWaveFileList(pCmd->destDevId,ucFlag,startTime,endTime,fileList))
	{
		m_errCode = PEC_UNKOWN;
		mylog.WriteLog2File(LogControl::LERROR,"Protocol104_ZJ::GetRecWaveFileList():GetRecWaveFileList fail");
		pLinker->Close();   //�ر�����
		return false;
	}

	//����ASDU_16
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[0];
	pData += 4 + sizeof(struct CP56Time2a)*2;

	unsigned short iItemNum = 0;	//��Ŀ��
	int	iItemLen = sizeof(DataControl::FILEINFO);//��Ŀ����
	int iMaxItemNum = (PRO104ZJ_ASDUDATA_MAXSIZE -4 -sizeof(struct CP56Time2a)*2)/iItemLen;//ÿ������ܷ��͵���Ŀ��

	iMaxItemNum = (iMaxItemNum>0XFFFF)?0XFFFF:iMaxItemNum;

	memcpy(&szAsduData[4],&startTime,sizeof(startTime));
	memcpy(&szAsduData[4+sizeof(startTime)],&endTime,sizeof(endTime));

	vector<DataControl::FILEINFO>::iterator it = fileList.begin();
	while(it!= fileList.end())
	{
		DataControl::FILEINFO fileInfo = (*it);
		it++;
		iItemNum++;

		//û�г���֡��󳤶��һ�������
		if( iItemNum <= iMaxItemNum && it != fileList.end() )
		{
			memcpy(pData,&fileInfo,iItemLen);
			pData += iItemLen;
		}
		else//����֡��󳤶Ȼ����ݷ�����
		{
			if(iItemNum > iMaxItemNum )
			{
				//֡����
				szAsduData[1] = 1; //��������
				it--;
				iItemNum--;
			}
			else
			{
				//֡δ�����������Ѿ����
				szAsduData[1] = 0; //�����Ѿ����
				memcpy(pData,&fileInfo,iItemLen);
				pData += iItemLen;
			}

			int iAsduDataLen = pData - szAsduData;

			memcpy(&szAsduData[2],&iItemNum,sizeof(iItemNum));

			iItemNum = 0;

			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU16,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
			{
				return false;
			}

			memset(szAsduData,0,sizeof(szAsduData));
			pData = &szAsduData[2];
		}
	}

	fileList.clear();

	return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
bool Protocol104_ZJ::GetRecWaveFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//�ٻ�¼���ļ�
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol104_ZJ::GetRecWaveFile( )");

	//�����������
	if(pCmd->parmLen != 44 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::GetRecWaveFile()");
		return false;
	}

	unsigned int   startPos = 0;                    //��ʼ����λ��
	memcpy(&startPos,&pCmd->pParm[40],4);
		
	bool bComplete = false;
	bool bResult = true;

	DataControl::DataQue *pDataQue = DataControl::CreateDataQue(1024*1024*3);

	typedef struct tagWaveFileData
	{
		DataControl::DataQue *pDataQue;
		char fileName[41];
		unsigned int fileStartTransPos;//�ļ���ʼ����λ��
		unsigned int devId;
		bool dataEnd;	//�����Ƿ����
		bool isStop;	//�Ƿ�ֹͣ������
		bool hasError;	//�Ƿ��д���
	}WAVEFILEDATA;

	WAVEFILEDATA fileData;
	memset(&fileData,0,sizeof(fileData));

	fileData.pDataQue = pDataQue;
	memcpy(fileData.fileName ,pCmd->pParm,40);    //������չ�����ļ���
	fileData.fileStartTransPos = startPos;
	fileData.devId = pCmd->destDevId;

	//�������ļ����ݵ��߳�
	HANDLE hThread = CreateThread(NULL,0,&ReadWavFileData,&fileData,0,NULL);
	::CloseHandle(hThread);

	//����ASDU_14
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[6];
	int iCanReadDataLen = sizeof(szAsduData) - 6;//�ɶ��������ݳ���
	int iRemainDataLen = 0 ;//��ʣ������ݳ���

	//��ʼ����λ��
	memcpy(&szAsduData[2],&startPos,4);

	while(1)
	{
		//��������
		int iReadLen = pDataQue->pop(pData,iCanReadDataLen);

		pData += iReadLen;
		iCanReadDataLen -= iReadLen;
		startPos += iReadLen;

		if(iCanReadDataLen <= 0 || //��д��
			fileData.hasError == true || //�д���
			(fileData.dataEnd == true && pDataQue->IsEmpty() )) //������
		{
			if(fileData.hasError == true || (fileData.dataEnd == true && pDataQue->IsEmpty() ))
			{
				szAsduData[1] = 0;//������־
				bComplete = true;
			}
			else
			{
				szAsduData[1] = 1;//������־
			}

			int iAsduDataLen = pData - szAsduData;
			unsigned char COT = 0X14;
			if(fileData.hasError == true)
				COT = 0X15;

			//����ASDU_14
			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU14,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
			{
				bResult =  false;
				fileData.isStop = true;
				break;
			}

			//���ݷ�����
			if(bComplete)
			{
				bResult = true;
				break;
			}

			memcpy(&szAsduData[2],&startPos,4); //��ʼ����λ��
			pData = &szAsduData[6];				//�ļ�����λ��
			iCanReadDataLen = sizeof(szAsduData) - 6;//�ɶ��������ݳ���

			continue;
		}//end if(iCanReadDataLen <= 0 || //��д��

	}

	if(pDataQue != NULL)
	{
		delete pDataQue;
		pDataQue = NULL;
	}

	return bResult;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
bool Protocol104_ZJ::GetReportFile( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//��ȡ��Ҫ¼������
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol104_ZJ::GetReportFile()");
	return false;

	//�����������
	/*if(pCmd->parmLen != 44 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::GetReportFile()");
		return false;
	}

	char	szFilePartName[41] ={0};
	unsigned int   startPos = 0;			//��ʼ����λ��
	memcpy(szFilePartName,pCmd->pParm,40);
	memcpy(&startPos,&pCmd->pParm[40],4);

	char *pReportData = NULL;
	int iReportDataLen = 0;

	//�õ��ļ���������չ�����ļ�·��
	FileControl fileCtrl;
	string fileDir = fileCtrl.GetFileDirector(pCmd->destDevId,szFilePartName,FileControl::FT_COMTRADE,NULL);
	fileDir.append(szFilePartName);

	unsigned short TrgTypeNum = 0;//����ԭ��
	DataControl dataCtrl;
	if(!dataCtrl.GetReportByFileName_ZJ(pCmd->destDevId,fileDir.c_str(),&pReportData,&iReportDataLen,TrgTypeNum) 
		|| pReportData == NULL)
	{
		if(pReportData != NULL)
		{
			delete []pReportData;
			pReportData = NULL;
		}
	}

	bool bResult = SendReportFile(pLinker,pCmd,1,pReportData,iReportDataLen,startPos);

	if(pReportData != NULL)
	{
		delete []pReportData;
		pReportData = NULL;
	}

	return bResult;*/
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetMarkEventList(Linker *,struct Command*)'
bool Protocol104_ZJ::GetMarkEventList( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//��ȡ����¼��б�
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol104_ZJ::GetMarkEventList()");

	if(pCmd->parmLen != 1+sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::GetMarkEventList()");
		return false;
	}

	unsigned char type = 1;	//1��ֻ�����й��ϵ��¼��б�2�����������¼�
	struct CP56Time2a startTime,endTime;
	unsigned char COT = 0x14;

	//��ѯ����
	type = *(pCmd->pParm);
	if(type == 1)
	{
		COT = 0x14;
	}
	else
	{
		COT = 0x09;
	}

	//��ʼʱ��
	memcpy(&startTime,&pCmd->pParm[1],sizeof(struct CP56Time2a));

	//����ʱ��
	memcpy(&endTime,&pCmd->pParm[1+sizeof(struct CP56Time2a)],sizeof(struct CP56Time2a));

	DataControl dataCtrl;
	vector< MarkedEvtStruct> markedEventList;
	dataCtrl.GetMarkecEventList((int)pCmd->destDevId,startTime,endTime,markedEventList);

	//����ASDU_18
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[0];
	pData += 4 + sizeof(struct CP56Time2a)*2;

	unsigned short iItemNum = 0;	//��Ŀ��
	int	iItemLen = sizeof(MARKEVNT);//��Ŀ����
	int iMaxItemNum = (PRO104ZJ_ASDUDATA_MAXSIZE -4 -sizeof(struct CP56Time2a)*2)/iItemLen;//ÿ������ܷ��͵���Ŀ��

	iMaxItemNum = (iMaxItemNum>0XFFFF)?0XFFFF:iMaxItemNum;

	memcpy(&szAsduData[4],&startTime,sizeof(startTime));
	memcpy(&szAsduData[4+sizeof(startTime)],&endTime,sizeof(endTime));

	vector< MarkedEvtStruct>::iterator it = markedEventList.begin();
	while(it!= markedEventList.end())
	{
		//MarkedEvtStruct dbMarkEvt = (*it);

		//TODO:MarkedEvtStruct to MARKEVNT
		MARKEVNT markEvnt;

		it++;
		iItemNum++;

		//û�г���֡��󳤶��һ�������
		if( iItemNum <= iMaxItemNum && it != markedEventList.end() )
		{
			memcpy(pData,&markEvnt,iItemLen);
			pData += iItemLen;
		}
		else//����֡��󳤶Ȼ����ݷ�����
		{
			if(iItemNum > iMaxItemNum )
			{
				//֡����
				szAsduData[1] = 1; //��������
				it--;
				iItemNum--;
			}
			else
			{
				//֡δ�����������Ѿ����
				szAsduData[1] = 0; //�����Ѿ����
				memcpy(pData,&markEvnt,iItemLen);
				pData += iItemLen;
			}

			int iAsduDataLen = pData - szAsduData;

			memcpy(&szAsduData[2],&iItemNum,sizeof(iItemNum));

			iItemNum = 0;

			if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU18,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
			{
				return false;
			}

			memset(szAsduData,0,sizeof(szAsduData));
			pData = &szAsduData[2];
		}
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'AutoSendDevState(Linker *,struct Command*)'
bool Protocol104_ZJ::AutoSendDevState( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//�����������
	if( pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::AutoSendDevState()");
		return false;
	}

	int iItemNum = pCmd->parmLen/sizeof(DAVICESTATE);
	char *pTmp = pCmd->pParm;
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[0];
	int iAsduDataLen = 1+sizeof(struct CP56Time2a)+1;
	bool bResult = true;

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime);

	struct CP56Time2a time;
	memset(&time,0,sizeof(time));

	time.year = sysTime.wYear%100;
	time.month = sysTime.wMonth;
	time.day = sysTime.wDay;
	time.hours = sysTime.wHour;
	time.minute = sysTime.wMinute;
	time.milliseconds = sysTime.wSecond*1000 + sysTime.wMilliseconds;

	for(int i=0;i<iItemNum;i++)
	{
		DAVICESTATE DevState;
		memcpy(&DevState,pTmp,sizeof(DAVICESTATE));
		pTmp += sizeof(DAVICESTATE);

		if(DevState.state == 1 || DevState.state == 2)
		{
			*pData = 2;//����
		}
		else
		{
			*pData = 1;//�Ͽ�
		}
		pData += 1;
		memcpy(&pData,&time,sizeof(time));
		pData += sizeof(time);

		//����ASDU_1
		string sAsdu = this->EncodeAsdu(PRO104_ASDU1,STRNO_GLIB,1,DevState.devId,szAsduData,iAsduDataLen);
		if(sAsdu == "" || sAsdu.size() < 4)
		{
			bResult =  false;
			break;
		}

		sAsdu.operator [](ASDUID104_LEN) = (unsigned char)253;
		sAsdu.operator [](ASDUID104_LEN+1) = (unsigned char)DevState.devId;

		int iResult = SendIFrame(pLinker,sAsdu.c_str(),sAsdu.size(),pCmd);

		if( -1 == iResult )
		{
			pLinker->Close();
			mylog.WriteLog2File(LogControl::LERROR, "Protocol104_ZJ::AutoSendDevState( ):Send ASDU_14 error");
			bResult = false;
			break;
		}
		else if(iResult == 0)
		{
			bResult = false;
			break;
		}
	}


	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'AutoSendMarkEvent(Linker *,struct Command*)'
bool Protocol104_ZJ::AutoSendMarkEvent( Linker * pLinker, struct Command * pCmd )
{
	// {{{USR
	//�����������
	if( pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::AutoSendMarkEvent()");
		return false;
	}

	//����ASDU_18
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[0];
	pData += 4 + sizeof(struct CP56Time2a)*2;

	szAsduData[1] = 0; //�����Ѿ����

	MARKEVENT markEvntCommon;
	MARKEVNT markEvntZJ;
	memset(&markEvntZJ,0,sizeof(markEvntZJ));

	memcpy(&markEvntCommon, pCmd->pParm,sizeof(markEvntCommon));
	memcpy(markEvntZJ.desc,markEvntCommon.Describe,sizeof(markEvntZJ.desc));
	markEvntZJ.devAddr = markEvntCommon.DAUID;
	markEvntZJ.fileLen = markEvntCommon.FileLen;
	memcpy(&markEvntZJ.fileName,markEvntCommon.FileName,sizeof(markEvntZJ.fileName));
	memcpy(&markEvntZJ.startTime,&markEvntCommon.StartTime,sizeof(markEvntZJ.startTime));

	long timeLen = markEvntCommon.EndTime.Millisecond - markEvntCommon.StartTime.Millisecond;
	if(timeLen < 0)
	{
		timeLen = 60*1000 + timeLen;
	}
	markEvntZJ.timeLen = (unsigned short)timeLen;
	memcpy(pData,&markEvntZJ,sizeof(markEvntZJ));
	pData += sizeof(markEvntZJ);

	int iAsduDataLen = pData - szAsduData;
	szAsduData[2] = 1;

	return SendAsduFrame(pLinker,pCmd,PRO104_ASDU18,STRNO_GLIB,0x01,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen);

	// }}}USR
}
// }}}RME

// {{{RME operation 'DoIdle(Linker*)'
Task * Protocol104_ZJ::DoIdle( Linker * pLinker )
{
	// {{{USR
	if(m_dwBeginIdleTime <= 0 )
	{
		m_dwBeginIdleTime = ::GetTickCount();//��ʼ��ʱ
		return Protocol104::DoIdle(pLinker);
	}
	else
	{
		//��ʱ
		DWORD dwNowTime = GetTickCount();
		if( ((dwNowTime - m_dwBeginIdleTime)/1000) < 15*60*1000)
		{
			//δ��ʱ
			return Protocol104::DoIdle(pLinker);
		}

		//ʱ�䵽������״̬
		Task *pTask = new Task();
	    pTask->Linker = pLinker;
	    pTask->cmd.destDevId = 0xFF; //�����豸
	    pTask->cmd.from = CD_NEURON;
	    pTask->cmd.to = CD_REMOTE;
		pTask->cmd.funCode = CMD_QUERY_LINKSTATE;
		strcpy(pTask->cmd.describe,(CStringRes::GetInstance())->GetStrById(IDS_QUY_DEVSTAT,"��ѯ��·״̬"));

		return pTask;
	}

	return NULL;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

int Protocol104_ZJ::SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd)
{
	if(!m_bCanSendData)
	{
		this->m_errCode = PEC_STARTDT_NOACT;
		return 0;
	}

	m_dwBeginT2Time = 0;
	int count = 0;

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

bool Protocol104_ZJ::SendAsduFrame(Linker * pLinker,struct Command * pCmd,unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen )
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
		mylog.WriteLog2File(LogControl::LERROR, "Protocol104_ZJ::SendAsduFrame( ):Send ASDU error");
		return false;
	}
	else if(iResult == 0)
	{
		return false;
	}

	return true;
}

bool Protocol104_ZJ::CreateReportFile( Linker * pLinker, struct Command * pCmd )
{
	//��ȡ��Ҫ¼������
	mylog.WriteLog2File(LogControl::LINFO,"Enter Protocol104_ZJ::CreateReportFile()");
	return false;

	//�����������
	/*if(pCmd->parmLen != 512 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol104_ZJ::CreateReportFile()");
		return false;
	}

	char	szFilePath[512] ={0};//������չ�����ļ�·��
	memcpy(szFilePath,pCmd->pParm,sizeof(szFilePath));

	char *pReportData = NULL;
	int iReportDataLen = 0;

	unsigned short TrgTypeNum = 0;//����ԭ��
	DataControl dataCtrl;
	if(!dataCtrl.GetReportByFileName_ZJ(pCmd->destDevId,szFilePath,&pReportData,&iReportDataLen,TrgTypeNum) 
		|| pReportData == NULL)
	{
		mylog.WriteLog2File(LogControl::LERROR,"Create fault report file failed in Protocol104_ZJ::CreateReportFile()");
		return false;
	}

	//�����Զ��ϴ����ϼ򱨣��Ŵ����Զ��ϴ�����
	if( SystemCfger::GetInstance().IsCandAutoSendRpt(TrgTypeNum) )
	{
		//��������
		Task *pTask = new Task();
		pTask->Linker = NULL;
		pTask->cmd.destDevId = pCmd->destDevId;
		pTask->cmd.from = CD_NEURON;
		pTask->cmd.to = CD_REMOTE;
		pTask->cmd.funCode = CMD_AUTOSEND_REPORTFILE;

		pTask->cmd.parmLen = (unsigned short)iReportDataLen;
		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);
		memcpy(pTask->cmd.pParm,pReportData,pTask->cmd.parmLen);

		//�������͵�Զ�����������
		if(!(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pTask))
		{
			delete pTask;
		}
	}

	delete []pReportData;
	pReportData = NULL;

	return true;*/
}

bool Protocol104_ZJ::AutoSendReportFile( Linker * pLinker, struct Command * pCmd )
{
	if(pCmd->pParm == NULL)
	{
		return false;
	}

	return SendReportFile(pLinker,pCmd,2,pCmd->pParm,pCmd->parmLen,0);
}

bool Protocol104_ZJ::SendReportFile( Linker * pLinker, struct Command * pCmd,int type,const char *pReportData,int DataLen,unsigned int startPos)
{
	unsigned char COT = 0X14;
	char szAsduData[PRO104ZJ_ASDUDATA_MAXSIZE] = {0};
	char *pData = &szAsduData[6];
	int iCanReadDataLen = sizeof(szAsduData) - 6;//�ɶ��������ݳ���
	int iRemainDataLen = 0 ;//��ʣ������ݳ���
	bool bResult = true;

	int iReportDataLen = DataLen;

	if(type == 1)
	{
		 COT = 0X14;
	}
	else
	{
		 COT = 0X01;//�Զ��ϴ�
	}

	if(pReportData == NULL)
	{
		szAsduData[1] = 0;
		COT = 0x15;

		//����ASDU_114
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU114,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,2))
		{
			return false;
		}
		return false;
	}

	//��ʼ����λ��
	memcpy(&szAsduData[2],&startPos,4);

	iRemainDataLen = iReportDataLen-startPos;
	while(1)
	{
		//��������
		int iReadLen = min(iRemainDataLen,iCanReadDataLen);

		memcpy(pData,&pReportData[iReportDataLen - iRemainDataLen],iReadLen);

		pData += iReadLen;
		iCanReadDataLen -= iReadLen;
		startPos += iReadLen;
		iRemainDataLen -= iReadLen;

		if(iRemainDataLen > 0) //����δ��
		{
			szAsduData[1] = 1;//������־
		}
		else
		{
			szAsduData[1] = 0;//������־
		}

		int iAsduDataLen = pData - szAsduData;

		//����ASDU_114
		if(!SendAsduFrame(pLinker,pCmd,PRO104_ASDU114,STRNO_GLIB,COT,(pCmd->destDevId & 0XFF),szAsduData,iAsduDataLen))
		{
			bResult =  false;
			break;
		}

		//���ݷ�����
		if(iRemainDataLen <= 0)
		{
			bResult = true;
			break;
		}

		memcpy(&szAsduData[2],&startPos,4); //��ʼ����λ��
		pData = &szAsduData[6];				//�ļ�����λ��
		iCanReadDataLen = sizeof(szAsduData) - 6;//�ɶ��������ݳ���

	}//end while(1)

	return bResult;
}


	//�ٻ�¼���ļ��б�
    bool GetRecWaveFileList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFileList(pLinker,&cmd);
    }

	//�ٻ�¼���ļ�
    bool GetRecWaveFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetRecWaveFile(pLinker,&cmd);
    }

    //��Ҫ¼������
    bool GetReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
    {
        return pProtocol->GetReportFile(pLinker,&cmd);
    }

	//���¼��б�
    bool GetMarkEventList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->GetReportFile(pLinker,&cmd);
	}

	//�Զ��ϴ��豸״̬
	bool AutoSendDevState(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->AutoSendDevState(pLinker,&cmd);
	}

	bool AutoSendMarkEvent(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->AutoSendMarkEvent(pLinker,&cmd);
	}

	//�������ϼ��ļ�
    bool CreateReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->CreateReportFile(pLinker,&cmd);
	}

	//�Զ��ϴ����ϼ��ļ�
    bool AutoSendReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd)
	{
		return pProtocol->AutoSendReportFile(pLinker,&cmd);
	}


	//��¼���ļ������߳�
	DWORD WINAPI ReadWavFileData(void *pParm)
	{
		typedef struct tagWaveFileData
		{
			DataControl::DataQue *pDataQue;
			char fileName[41];
			unsigned int fileStartTransPos;//�ļ���ʼ����λ��
			unsigned int devId;
			bool dataEnd;	//�����Ƿ����
			bool isStop;	//�Ƿ�ֹͣ������
			bool hasError;	//�Ƿ��д���
		}*PWAVEFILEDATA;

		PWAVEFILEDATA pWaveFileData = (PWAVEFILEDATA)pParm;

		assert(pWaveFileData != NULL);
		assert(pWaveFileData->pDataQue != NULL);

		DataControl::DataQue *pDataQue = pWaveFileData->pDataQue;

		FileControl fileCtrl;

		string partFileName(pWaveFileData->fileName);
		string comtradeDir = fileCtrl.GetFileDirector(pWaveFileData->devId,partFileName.append(".cfg").c_str(),FileControl::FT_COMTRADE,NULL);

		string cfgFileName = comtradeDir + string(pWaveFileData->fileName) + string(".cfg");
		string headFileName = comtradeDir + string(pWaveFileData->fileName) + string(".HDR");
		string dateFileName = comtradeDir + string(pWaveFileData->fileName) + string(".DAT");
		string infFileName = comtradeDir + string(pWaveFileData->fileName) + string(".inf");

		//�������ļ��������
		if( !fileCtrl.IsExist( cfgFileName.c_str()) || !fileCtrl.IsExist( dateFileName.c_str()))
		{
			pWaveFileData->hasError = true;
			return 0;
		}

		//��ȡ¼������ʱ��
		struct CP56Time2a cpTime;
		memset(&cpTime,0,sizeof(struct CP56Time2a));
		fileCtrl.GetRecWaveFaultTime(cfgFileName.c_str(),&cpTime);

		unsigned int uiStartTransPos = pWaveFileData->fileStartTransPos;//�ļ���ʼ����λ��

		char szHead[47] = {0};
		memcpy(szHead,pWaveFileData->fileName,40);
		memcpy(&szHead[40],&cpTime,7);

		char szFileData[1024*8] = {0};	//�����ļ�����
		string files[4] ={headFileName,cfgFileName,dateFileName,infFileName};

		//��������ͷ
		if(uiStartTransPos <= 47)
		{
			pDataQue->push(&szHead[uiStartTransPos],(47-uiStartTransPos),&pWaveFileData->isStop);
		}
		else
		{
			uiStartTransPos = uiStartTransPos -47;
		}

		//����4���ļ�
		for(int i=0;i<4;i++)
		{
			if( !fileCtrl.IsExist( files[i].c_str()))
			{
				continue;
			}

			szHead[0] = i+1;	//�ļ�����
			unsigned int uiFileLen = fileCtrl.GetFileLen(files[i].c_str());//�ļ�����
			memcpy(&szHead[1],&uiFileLen,4);

			//����ͷ
			if(uiStartTransPos <= 5)
			{
				pDataQue->push(&szHead[uiStartTransPos],(5-uiStartTransPos),&pWaveFileData->isStop);
				uiStartTransPos = 0;

				if(pWaveFileData->isStop)
					return 0;
			}
			else
			{
				uiStartTransPos = uiStartTransPos -5;
			}

			//�����ļ�����
			if(uiStartTransPos >= uiFileLen)
			{
				uiStartTransPos = uiStartTransPos - uiFileLen;
				continue;
			}
			else
			{
				//���ļ�
				if(!fileCtrl.OpenFile(files[i].c_str(),FileControl::OM_READ))
				{
					pWaveFileData->hasError = true;
					return 0;
				}

				fileCtrl.Seek(uiStartTransPos);

				//���ļ�����
				while(1)
				{
					int iReadLen = fileCtrl.ReadData(szFileData,sizeof(szFileData));
					pDataQue->push(szFileData,iReadLen,&pWaveFileData->isStop);

					if(iReadLen < sizeof(szFileData) )
					{
						break;//�ļ�����
					}

					if(pWaveFileData->isStop)
					{
						fileCtrl.CloseFile();
						return 0;
					}
				}//end while(1)

				fileCtrl.CloseFile();
			}//end if(uiStartTransPos >= uiFileLen) -- else
		}//end for(int i=0;i<4;i++)

		pWaveFileData->dataEnd = true;

		return 0;
	}

// }}}USR
// }}}RME

// }}}RME
