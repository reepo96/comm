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
* �ļ����ƣ�Protocol60_103.cpp
* �ļ���ʶ��
* ժ    Ҫ������60_103Э��
*
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* �������:2007��7��4��
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

DWORD WINAPI DelUdpDataThread(void *pParm);//����udp���ݵ��߳�

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

	if( pCmd->from == CD_REMOTE ) //����Զ��
	{
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //������UI����ʱ����
	{
		//�����������
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetDauInf()");
			return false;
		}

		unsigned char type = pCmd->pParm[0];	//����

		//asdu10��ִ�е�д�ı���
		unsigned char asdu10_attr[1]={0};	//���Խṹ
		asdu10_attr[0] = type;	//����

		GI asdu10_gi[1] ={GI(0X1201,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21�����Ʒ��򣩨D�D �����ص��Լ���Ϣ
		GI asdu21_gi[1] ={GI(0X1202,0X67,23,0,1,0,NULL)};

		char pIniData[PRO103_ASDUDATA_MAXSIZE+1] ={0};//
		char *ppData[1] ={ &pIniData[0]};
		int  iniDataLen[1] = {PRO103_ASDUDATA_MAXSIZE};
		int	*pIniDataLen[1] = {&iniDataLen[0]};
		int iAttrNum = 1;

		PCMDRESULT pCmdResult = new CMDRESULT;

		//��ȡͨ�÷�����񷵻ص����Խṹ����
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppData,pIniDataLen,&iAttrNum);
		if(iReturn != 1)
		{
			//����ʧ�ܽ��
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

		//���Լ���Ϣ��������
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
	//��ȡ�ļ�ǰ�Ĵ�����
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::PreGetFile()");

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;	//���͵����ݳ���

	FIXFRAME fixFrame;	//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	static bool preFCB = false;

	char ASDUData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//ASDU����
	int  ASDUDataLen = 0;

	if( type == 1 || type == 3) //¼���ļ�
	{
		ASDUData[0] = recWaveType;			//¼������
		memcpy(&ASDUData[1],pFileName,fileNameLen);	//¼���ļ�����������չ����
		memcpy(&ASDUData[41],&startPos,4);	//��ʼ����λ��
		ASDUDataLen = 45;
	}
	else //һ���ļ�
	{
		memcpy(&ASDUData[0],pFileName,fileNameLen);	//�ļ���
		memcpy(&ASDUData[100],&startPos,4);	//��ʼ����λ��
		ASDUDataLen = 104;
	}

	preFCB = !preFCB;
	auto_ptr<string> pDataPack;
	if( type == 1) //¼���ļ�
	{
		//����ASDU13����
		pDataPack = auto_ptr<string>(CreateMsg(C_EX13_NA_3, pCmd->destDevId,preFCB,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
	}
	else //һ���ļ�
	{
		//����ASDU103����
		pDataPack = auto_ptr<string>(CreateMsg(C_EX103_NA_3, pCmd->destDevId,preFCB,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
	}

	sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //��������
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
	//�ٻ�¼���ļ�
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetRecWaveFile()");

	//�����������
	if(pCmd->parmLen != 45 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetRecWaveFile()");
		return false;
	}

	char szLogInf[312] = {0};
	unsigned char recWaveType = pCmd->pParm[0];	//¼������
	char pFileName[41] = {0};//������չ�����ļ���
	memcpy(pFileName,&pCmd->pParm[1],40);
	int  fileNameLen = strlen(pFileName);
	unsigned int   startPos = 0;//��ʼ����λ��
	memcpy(&startPos,&pCmd->pParm[41],4);

	string	sDownInfFile;//����������Ϣ����ʱ�ļ�������ϵ�������Ϣ��

	struct CP56Time2a faultTime;//����ʱ��
	memset(&faultTime,0,sizeof(faultTime));

	SYSTEMTIME fileSysTime;//�½��ļ���ϵͳʱ��
	memset(&fileSysTime,0,sizeof(fileSysTime));

	FILETIME fileTime;//�½��ļ����ļ��ṹʱ��
	memset(&fileTime,0,sizeof(fileTime));

	FileControl fileCtrl;

	//�����жϻ���
	struct tagEnVirionment
	{
		unsigned int startPos;
		int iCurrentComtradeFileType;
		int iCurrentFileLen;
		int iCurrentFileHasWriteLen;
		char sFilePathWithoutExt[512];
		bool bNeedUnCompress;	//dat�ļ���Ҫ��ѹ
	};
	struct tagEnVirionment *pEV = NULL;
	bool	bHasError = false;	//�д���

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

	auto_ptr<string> pDataPack ;//Ҫ���͵����ݰ�

	FIXFRAME fixFrame;//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	if( pCmd->from == CD_REMOTE )
	{
		//����������Զ�̣���¼���ļ����͵�Զ��
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else
	{
		//����������UI����ʱ����
		MemMapMgr memMgr;
		int iCurrentComtradeFileType = 0 ;	//��ǰ¼���ļ����ͣ�0��ʾδ֪
		int iCurrentFileLen = -1;	//��ǰ�ļ�����
		int iCurrentFileHasWriteLen = 0;	//��ǰ�ļ��ѱ�д�ĳ���
		string sFilePathWithoutExt("");	//������չ����¼���ļ�·��
		string sCurrentFileName("");	//��ǰ������ļ���
		char szSaveRemainData[4] = {0};	//�����ϴ�δ���������
		int  iRemainDataLen = 0 ;	//δ��������ݳ���
		bool bCfgHasDown = false;	//cfg�ļ��Ѿ�����
		bool bDatHasDown = false;	//dat�ļ��Ѿ�����
		bool bNeedUnCompress = false;	//dat�ļ���Ҫ��ѹ

		bool bNeedAutoSendReportFile = true;//�Ƿ�Ҫ�Զ��ϴ����ϱ���
		int iMaxFileLen = 0;				//�����ļ�����

		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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

			//��鱣��������Ϣ����ʱ�ļ��Ƿ����
			if( fileCtrl.IsExist( sDownInfFile.c_str() ) )
			{
				//��ȡ������Ϣ
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

		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run the first step of GetRecWaveFile:pregetfile");

			//׼����ȡ�ļ�ǰ�Ĵ���
			if( !PreGetFile(pLinker,pCmd,1,pFileName, fileNameLen, startPos, recWaveType) )
			{
				mylog.WriteLog2File(LogControl::LERROR, "Return error from PreGetFile in GetRecWaveFile()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
		{
			mylog.WriteLog2File(LogControl::LDEBUG, "Run the step of GetRecWaveFile:wait for M_CON_NA_3");
			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LERROR, "Wait M_CON_NA_3 error in GetRecWaveFile()");
				return false;
			}
			pCmd->_state.ToNextState();
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4; //ÿ�οɶ�ȡ��������ݳ���
		int count = 0;	//ѭ������

		while( !bHasError )
		{
			count ++;

			if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			{
				mylog.WriteLog2File(LogControl::LDETAIL, "Run the step 3 of GetRecWaveFile:send question for level 1 data");
				//����1���û�����(�������û�з����꣬�������1���û����ݿ�ʼѭ��)
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

			//�ڴ��ɼ�װ�÷���ASDU14����
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
			PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
			PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
			PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

			char *pAsduData=NULL;
			int  AsduDataLen=0;

			int framePacketLen=0;	//֡ʵ�ʵĳ���

			if(pCmd->_state._state == CmdState::CS_STEP4) //��4��
			{
				mylog.WriteLog2File(LogControl::LDETAIL, "Run the step 4 of GetRecWaveFile:wait for ASDU14");
				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LERROR, "No get ASDU14 frame in GetRecWaveFile().");
					pLinker->Close();
					bHasError = true;
					break;
					//return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG, "GetRecWaveFile is interrupted.");

					//�����жϻ���
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					pEV->iCurrentComtradeFileType = iCurrentComtradeFileType;
					pEV->iCurrentFileLen = iCurrentFileLen;
					pEV->iCurrentFileHasWriteLen = iCurrentFileHasWriteLen;
					bHasError = true;
					break;
					//return false;
				}

				//�Ƿ����ڴ�������
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

				unsigned int fileStartPos = 0;	//д�ļ�����ʼλ��
				unsigned int  writeLen = 0;	//Ҫд���ļ������ݳ���

				while(AsduDataLen > 4  && !bHasError)//AsduDataLen����4�ֽڵ���ʼλ�úʹ���ļ�����
				{
					//�״δ����ص�����
					if(iCurrentComtradeFileType == 0)
					{
						if(startPos != 0 || AsduDataLen < (4 + PRO103_ASDU_FILENAMELEN +7 +1 +4) )
						{
							//���յ������ݲ���Ҫ��
							mylog.WriteLog2File(LogControl::LERROR,"comtrade file data is invalid in GetRecWaveFile()");
							//mylog.WriteLog(LogControl::LERROR,pCmd,"comtrade file data is invalid");
							m_errCode = PEC_ERR_ANSWER;
							pLinker->Close();
							bHasError = true;
							break;
							//return false;
						}

						//��ù���ʱ��
						memcpy(&faultTime,&pAsduData[4 +PRO103_ASDU_FILENAMELEN],sizeof(faultTime));

						//�趨�ļ�ʱ��
						fileSysTime.wYear = 2000 + faultTime.year;
						fileSysTime.wMonth = faultTime.month;
						fileSysTime.wDay = faultTime.day;
						fileSysTime.wHour = faultTime.hours;
						fileSysTime.wMinute = faultTime.minute;
						fileSysTime.wSecond = faultTime.milliseconds/1000;

						SystemTimeToFileTime(&fileSysTime,&fileTime);

						//���һ򴴽��ļ����Ŀ¼
						int year = 2000 + faultTime.year;
						int month = faultTime.month;
						int yearMonth = year*100 + month;
						char szYearMonth[7] = {0};
						comtradeDir.append(::itoa(yearMonth,szYearMonth,10));
						if(!fileCtrl.IsExist(comtradeDir.c_str())) //���Ŀ¼�Ƿ����
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

						//ָ���ļ�·��
						comtradeDir.append("/");
						sFilePathWithoutExt = comtradeDir + string(pFileName);

						memcpy(pEV->sFilePathWithoutExt,
							sFilePathWithoutExt.c_str(),
							sFilePathWithoutExt.size());
						sFilePathWithoutExt = string(pEV->sFilePathWithoutExt);

						//�ļ�����
						iCurrentComtradeFileType = (int)pAsduData[4 +PRO103_ASDU_FILENAMELEN +7];
						//�ļ�����
						memcpy(&iCurrentFileLen,&pAsduData[4 +PRO103_ASDU_FILENAMELEN +7 +1],4);

						startPos = PRO103_ASDU_FILENAMELEN +7 +1 +4;
						pAsduData += PRO103_ASDU_FILENAMELEN +7 +1 +4;

						AsduDataLen = AsduDataLen - (PRO103_ASDU_FILENAMELEN +7 +1 +4);
						if( AsduDataLen <= 4 )//���ݴ�����
						{
							mylog.WriteLog2File(LogControl::LWARNING,"The first data-pack only include the RecWaveFile head in GetRecWaveFile()");
							break;
						}

						//�ļ��Ѿ����ڣ������Զ��ϴ����ϼ�
						string sTmpFile = sFilePathWithoutExt + string(".cfg");
						if(fileCtrl.IsExist( sTmpFile.c_str() ))
						{
							bNeedAutoSendReportFile = false;
						}

					}//end if(iCurrentComtradeFileType == 0)

					//�ļ�����δ֪
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
						//��õ�ǰ�����ļ�����
						switch(iCurrentComtradeFileType)
						{
						case 1: //hdr�ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".HDR");
							break;
						case 2: //cfg�ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".cfg");
							break;
						case 3: //dat�ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".DAT");
							break;
						case 4: //inf�ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".INF");
							break;
						case 5: //she�ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".SHE");
							break;
						case 6: //shc�ļ�(��ֵ�ļ�)
							sCurrentFileName = sFilePathWithoutExt + string(".SHC");
							break;
						case 7: //datѹ���ļ�
							sCurrentFileName = sFilePathWithoutExt + string(".SHZip");
							bNeedUnCompress = true;
							pEV->bNeedUnCompress = bNeedUnCompress;
							break;
						default:
							//���յ������ݲ���Ҫ��
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
							//�޸��ļ�ʱ��
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

					//��ǰ�ļ����Ȼ�δ֪
					if(iCurrentFileLen == -1)
					{
						char szLenData[4] = {0};

						//�ȿ����ϴ�ʣ�������
						memcpy(&szLenData[0],&szSaveRemainData[0],iRemainDataLen);

						//�ٿ������ν��յ�������
						int iNeedCpy = 4 - iRemainDataLen;
						memcpy(&szLenData[iRemainDataLen],&pAsduData[4],iNeedCpy);
						AsduDataLen = AsduDataLen - iNeedCpy;
						pAsduData += iNeedCpy;
						startPos += 4;
						iRemainDataLen = 0;

						//�ļ�����
						memcpy(&iCurrentFileLen,&szLenData[0],4);
						if(iMaxFileLen < iCurrentFileLen)
						{
							iMaxFileLen = iCurrentFileLen;
						}
					}

					//������д���ļ�
					fileCtrl.Seek(iCurrentFileHasWriteLen);
					int iRemain =iCurrentFileLen - iCurrentFileHasWriteLen;
					writeLen = (iRemain - (AsduDataLen-4))>0?(AsduDataLen-4):iRemain;

					fileCtrl.WriteData(&pAsduData[4],writeLen);
					pAsduData += writeLen ;
					startPos += writeLen;
					iCurrentFileHasWriteLen += writeLen;
					AsduDataLen = AsduDataLen - writeLen;

					//��ǰ������ļ��Ѿ�������ϣ������¸��ļ�
					if(AsduDataLen > 4 || iCurrentFileHasWriteLen == iCurrentFileLen)
					{
						sprintf(szLogInf,
							"The current file's downloading completes,remaind asdu-data-len:%d,current file len:%d,has write len:%d in Protocol60_103::GetRecWaveFile()",
							AsduDataLen,iCurrentFileLen,iCurrentFileHasWriteLen);
						mylog.WriteLog2File(LogControl::LDEBUG,szLogInf);

						if(iCurrentComtradeFileType == 2)
						{
							bCfgHasDown = true;//cfg�ļ��Ѿ�����
						}
						else if(iCurrentComtradeFileType == 3 )
						{
							bDatHasDown = true;//dat�ļ��Ѿ�����
						}
						else if(iCurrentComtradeFileType == 7)
						{
							bDatHasDown = true;//dat�ļ��Ѿ�����
							bNeedUnCompress = true;//��Ҫ��ѹ
						}

						//���³�ʼ��
						fileCtrl.CloseFile();
						sCurrentFileName = string("");
						iCurrentFileHasWriteLen = 0;
						iCurrentFileLen = -1 ;

						int iRemain = AsduDataLen -4;

						//�ļ�����
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

						//�ļ�����
						if(iRemain < 4)//ʣ������ݲ����Ա�ʾ�ļ�����
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

				int maxCount = iCurrentFileLen/readMaxLen; //���ѭ������
				int iInterval = maxCount/20;    //������ٴ�ѭ��֮��дһ�ν�����Ϣ(ȫ�������д20�ν�����Ϣ)
				iInterval = (iInterval <= 0)?30:iInterval;

				//д������Ϣ(����ʾdat�ļ�����Ϣ)
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

					//������
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();
				}

				if(pVFrameHead->control.m.acd == false)
				{
					//����ļ����ȴ��ڹ涨�ĳ������Զ����Ϸ���
					//if(SystemCfger::GetInstance().maxLenOfAutoAnaWaveFile *1024*1024 < iMaxFileLen)
					//{
					//	bNeedAutoSendReportFile = false;
					//}
					bNeedAutoSendReportFile = false;//������ͨѶ�����Զ��ϴ����ϼ�

					//��Ҫ�Զ��ϴ����ϼ�
					if(bNeedAutoSendReportFile)
					{
						//��������
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

						//�������͵�Զ�����������
						if(!(QueRegister::GetInstance().RegisterQue(COMM_REMOTEQUE_NAME))->SendMsg(pTask))
						{
							delete pTask;
						}
					}

					//���������Ϣ�ļ�
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

					//�ļ��������
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					if(!bCfgHasDown || !bDatHasDown)
					{
						//cfg�ļ���dat�ļ�û�����أ��򷵻ش���
						mylog.WriteLog2File(LogControl::LWARNING,"Download complete,but the cfg or the dat file don't downloaded,leave Protocol60_103::GetRecWaveFile()");
						return false;
					}
					else
					{
						if( bNeedUnCompress )
						{
							//��ѹ�ļ�
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

			}//end if(pCmd->_state._state == CmdState::CS_STEP4) //��4��

		}//end while(1)

		//����������Ϣ
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

	//�����������
	if(pCmd->parmLen != 1+sizeof(struct CP56Time2a)*2 || pCmd->pParm == NULL)
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetRecWaveFileList()");
		return false;
	}

	struct CP56Time2a startTime;	//��ѯ��ʼʱ��
	struct CP56Time2a endTime;	//��ѯ����ʱ��
	memcpy(&startTime,&pCmd->pParm[1],sizeof(startTime));
	memcpy(&endTime,&pCmd->pParm[1+sizeof(startTime)],sizeof(endTime));

	FileControl fileCtrl;
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	FIXFRAME fixFrame;//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //����������Զ�̣���¼���ļ����͵�Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		char sendData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//���͵�����

		memcpy(&sendData[0],pCmd->pParm,1);	//¼������
		memcpy(&sendData[1],&startTime,sizeof(startTime));//��ѯ��ʼʱ��
		memcpy(&sendData[1+sizeof(startTime)],&endTime,sizeof(endTime));//��ѯ��ֹʱ��

		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetRecWaveFileList:send ASDU15.");

			//����ASDU15����
			auto_ptr<string> pDataPack = auto_ptr<string>(CreateMsg(C_EX15_NA_3, pCmd->destDevId,false,0,CINF_0,&sendData[0],2*sizeof(struct CP56Time2a)+1,&msgLen));
			sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //��������
			if(sendDataLen != msgLen)
			{
				mylog.WriteLog2File(LogControl::LERROR, "send ASDU15 error in Protocol60_103::GetRecWaveFileList()");
				//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetRecWaveFileList:wait for M_CON_NA_3.");

			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
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
			if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetRecWaveFileList:Send C_PL1_NA_3.");
				//����1���û�����
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::GetRecWaveFileList().");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//�ڴ��ɼ�װ�÷���ASDU16����
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
			PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
			PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
			PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

			char *pAsduData=NULL;
			int  AsduDataLen=0;

			int framePacketLen=0;   //֡ʵ�ʵĳ���

			if(pCmd->_state._state == CmdState::CS_STEP4) //��4��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetRecWaveFileList:wait for ASDU16.");
				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LERROR,"No get ASDU16 in GetRecWaveFileList().");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetRecWaveFileList has be interrupted.");
					m_errCode = PEC_INTERRUPT;
					return false;
				}

				//�Ƿ����ڴ�������
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_16)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU16,but it isn't in Protocol60_103::GetRecWaveFileList()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU16,but it isn't");
					pLinker->Close();
					return false;
				}

				//��ASDU���ݽ������������
				unsigned short fileCount =0; //���͹������ļ���
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

				//ȡ���ļ�
				for(int i=0;i<fileCount;i++)
				{
					PCMDRESULT pCmdResult = new CMDRESULT;
					memset(pCmdResult,0,sizeof(CMDRESULT));

					pCmdResult->pData  = new char[sizeof(CMDFILELIST)+1];
					memset(pCmdResult->pData,0,sizeof(CMDFILELIST)+1);

					pCmdResult->resultType = CDRT_FILELIST;//���صĽ�����ļ��б�����
					pCmdResult->resultLen = sizeof(CMDFILELIST);
					memcpy(&pCmdResult->pData[0],&pAsduData[0],sizeof(CMDFILELIST)); //��������

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

			}//end  if(pCmd->_state._state == CmdState::CS_STEP4) //��4��

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
	//��ȡ��Ҫ¼������
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetReportFile()");

#ifdef _PILING
	//����׮
	CMDREPORT report;
	PCMDRESULT pCmdResult = new CMDRESULT();

	memset(&report,0,sizeof(CMDREPORT));
	memcpy(report.BASName,"��·1",strlen("��·1"));
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

	//�������ݷ�������
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
	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;	//���͵����ݳ���

	FIXFRAME fixFrame;	//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else
	{
		//����������UI����ʱ����

		if(pCmd->_state._state == CmdState::CS_STEP1) //��1��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetReportFile:send C_PL2_NA_3.");
			//����C_PL2_NA_3���ٻ�2�����ݣ�
			auto_ptr<string> pQurey( CreateMsg(C_PL2_NA_3, pCmd->destDevId,true,0,0,NULL,0,&msgLen) );
			if( pQurey.get() != NULL)
			{
				sendDataLen = SendData(pLinker,pQurey->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //��������֡
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

		if(pCmd->_state._state == CmdState::CS_STEP2) //��2��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetReportFile:wait for M_NV_NA_3.");
			//�ڴ��ɼ�װ�÷���M_NV_NA_3������Ҫ�����ݣ�ACD=1��
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

		//�Ƿ��м�����
		//if( fixFrame.control.m.acd != true)
		//{
		//û�����ݣ�����
		//pLinker->Close();
		//	return true;
		//}

		if(pCmd->_state._state == CmdState::CS_STEP3) //��3��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetReportFile:send C_PL1_NA_3.");
			//����1���û�����
			if(!SendLevelOne(pLinker,pCmd->destDevId))
			{
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		//�ڴ��ɼ�װ�÷���ASDU12����
		char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
		PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
		PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
		PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

		char *pAsduData=NULL;
		int  AsduDataLen=0;

		int framePacketLen=0;	//֡ʵ�ʵĳ���

		if(pCmd->_state._state == CmdState::CS_STEP4) //��4��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetReportFile:wait for ASDU12.");
			//ȡǰ̨�Ļش�֡
			int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
				&framePacketLen,
				&pVFrameHead,
				&pASDUID,
				&pASDUInfId,
				&pAsduData,
				&AsduDataLen,
				NULL,
				SystemCfger::GetInstance().timeOut);

			if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
			{
				pLinker->Close();
				return false;
			}
			if( iReturn == 3) //�ж�֡
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"GetReportFile has be interrupted.");
				m_errCode = PEC_INTERRUPT;
				return false;
			}

			//�Ƿ����ڴ�������
			if( pVFrameHead->control.m.fun != MFUNID_8 || pASDUID->TYP != ASDU_12)
			{
				m_errCode = PEC_ERR_ANSWER;
				mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU12,but it isn't");
				//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU12,but it isn't");
				pLinker->Close();
				return false;
			}

			//��ȡ������
			CMDREPORT report;
			PCMDRESULT pCmdResult = new CMDRESULT;

			if(AsduDataLen>0)
			{
				memset(&report,0,sizeof(CMDREPORT));
				memcpy(&report,&pAsduData[0],sizeof(CMDREPORT));

				//�������ݷ�������
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
				pCmdResult->pData[0] = 0;	//ʧ��
				pCmdResult->pData[1] = '\0';
			}

			pCmd->resultNum = 1;
			pCmd->resultList.push_back(pCmdResult);

		}//end if(pCmd->_state._state == CmdState::CS_STEP4) //��4��

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
	//��ʱ
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::SetTime()");

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		//pLinker->Close();
		return false;

	}// end if( pCmd->from == CD_REMOTE ) //����������Զ��
	else
	{
		/*
		//�����������
		if(pCmd->parmLen != sizeof(struct CP56Time2a) || pCmd->pParm == NULL)
		{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
		return false;
		}

		//����C_SYN_TA_3��ASDU6������
		memcpy(&sendData[0],pCmd->pParm,sizeof(struct CP56Time2a));
		auto_ptr<string> pDataPack(CreateMsg(C_SYN_TA_3, pCmd->destDevId,false,8,MINF_0,&sendData[0],sizeof(struct CP56Time2a),&msgLen));

		sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //��������
		if(sendDataLen != msgLen)
		{
		mylog.WriteLog2File(LogControl::LERROR, "send data error");
		//mylog.WriteLog(LogControl::LERROR,pCmd,"send data error");
		pLinker->Close();
		return false;
		}*/

		bool result = true;

		//�����������
		if(pCmd->parmLen != 1+1+sizeof(struct SHTimeStruct) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char asdu10_attr[1+1+sizeof(struct SHTimeStruct)]={0};  //���Խṹ
		memcpy(asdu10_attr,pCmd->pParm,pCmd->parmLen);

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0X0101,0X67,23,13,1,13,&asdu10_attr[0])};

		//����ASDU10��ִ�е�д
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//�����������
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
			return false;
		}

		unsigned char timeType = pCmd->pParm[0];	//ʱ������

		//asdu10��ִ�е�д�ı���
		unsigned char asdu10_attr[1]={0};	//���Խṹ
		asdu10_attr[0] = timeType;
		GI asdu10_gi[1] ={GI(0X0201,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21�����Ʒ��򣩨D�D ��ʱ��
		GI asdu21_gi[1] ={GI(0X0202,0X67,23,6,1,0,NULL)};

		FileControl fileCtrl;

		//��ȡͨ�÷�����񷵻ص����Խṹ����
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

		//����ϵͳʱ��
		struct CP56Time2a getTime;
		memset(&getTime,0,sizeof(struct CP56Time2a));
		getTime.year = timeData.Year%100;
		getTime.month = timeData.Month;
		getTime.day = timeData.Day;
		getTime.hours = timeData.Hour;
		getTime.minute = timeData.Minute;
		getTime.milliseconds = timeData.Second*1000 + timeData.Millisecond;

		//д���
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
	//����¼��
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartRecByManual()");

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		//�����������
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetTime()");
			return false;
		}

		unsigned char startType = pCmd->pParm[0];	//��������
		unsigned char operateType = 1;	//��������(1������ʦվ���̨����)
		unsigned short GIN = 0;	//ͨ�÷����ʶ��

		GIN = 0X0301;   //�ֶ�����

		//asdu10��ִ�е�д�ı���
		unsigned char attr[2];  //���Խṹ
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
	//����¼��
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::StartRecByTimer()");

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;

	}
	else //����������UI����ʱ����
	{
		//�����������
		if(pCmd->parmLen != 1+sizeof(struct SHTimeStruct)+1+sizeof(unsigned short) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::StartRecByTimer()");
			return false;
		}

		unsigned char startType = pCmd->pParm[0];	//��������

		struct SHTimeStruct startTime;	//������ʼʱ��
		memset(&startTime,0,sizeof(startTime));
		memcpy(&startTime,&pCmd->pParm[1],sizeof(startTime));

		unsigned char interval = pCmd->pParm[1+sizeof(startTime)];	//���,����¼����λ�룬�������5�롣����¼����λ���ӣ������趨���ټ�¼ʱ�䳤��

		unsigned short startCount =0;	//��������
		memcpy(&startCount,&pCmd->pParm[1+sizeof(startTime)+1],sizeof(startCount));

		unsigned short GIN = 0;	//ͨ�÷����ʶ��
		GIN = 0X0401;   //��ʱ����

		unsigned char attr[10];	//���Խṹ
		attr[0] = startType;	//��������

		memcpy(&attr[1],&startTime.Microsecond,2);	//������ʼʱ���΢��

		//������ʼʱ���Time_tʱ��
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

		attr[7] = interval; //���

		memcpy(&attr[8],&startCount,2);	//��������

		//asdu10��ִ�е�д�ı���
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

	//����ASDU10��ִ�е�д
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
	FIXFRAME fixFrame;	//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));
	int iReturn = 0;

	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};//�ɱ�֡����
	PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
	PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;	//֡ʵ�ʵĳ���

	//����ASDU10��ִ�е�д
	iReturn = SendAsdu10WithExeWrite(pLinker,pCmd,asdu10_GIs,asdu10_GINumber);
	if(iReturn != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "SendAsdu10WithExeWrite return fail in Protocol60_103::GetGIAttrData()");
		return iReturn;
	}

	if(pCmd->_state._state == CmdState::CS_STEP5) //���岽
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 5 of GetGIAttrData:send asdu21.");
		//ASDU21�����Ʒ��򣩨D�D �����ص��ļ���
		auto_ptr<string> pDataPack(CreateGIMsg(C_GC_NA_3,pCmd->destDevId,0X2A,CINF_241,asdu21_GIs,asdu21_GINumber,&msgLen));
		if( pDataPack.get() != NULL)
		{
			sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //����asdu21��ִ�е�д�ı��� 
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

	if(pCmd->_state._state == CmdState::CS_STEP6) //������
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 6 of GetGIAttrData:wait for M_CON_NA_3.");
		//�ڴ��ɼ�װ�÷���M_CON_NA_3 ��ȷ�ϣ�ACD=1��
		iReturn = WantConfirm(pLinker,pCmd);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING, "wait M_CON_NA_3 fail in Protocol60_103::GetGIAttrData()");
			return iReturn;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP7) //���߲�
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 7 of GetGIAttrData:send C_PL1_NA_3.");
		//����1���û�����
		if(!SendLevelOne(pLinker,pCmd->destDevId))
		{
			mylog.WriteLog2File(LogControl::LERROR, "send C_PL1_NA_3 error in Protocol60_103::GetGIAttrData()");
			return -1;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP8) //�ڰ˲�
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 8 of GetGIAttrData:wait for Asdu10.");
		//�ڴ�Asdu10�����ӷ��򣩨D�D ����һ�����ȫ����Ŀֵ 
		memset(&framePacket[0],0,sizeof(framePacket));

		//ȡǰ̨�Ļش�֡
		int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
			&framePacketLen,
			&pVFrameHead,
			&pASDUID,
			&pASDUInfId,
			&pAsduData,
			&AsduDataLen,
			NULL,
			SystemCfger::GetInstance().timeOut);

		if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
		{
			mylog.WriteLog2File(LogControl::LWARNING, "wait for Asdu10 fail in Protocol60_103::GetGIAttrData()");
			pLinker->Close();
			return -1;
		}
		if( iReturn == 3) //�ж�֡
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"GetGIAttrData has be interrupted.");
			return 0;
		}

		//�Ƿ����ڴ�������
		if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_10)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU10,but it isn't in Protocol60_103::GetGIAttrData()");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU10,but it isn't");
			return -1;
		}

		//�жϷ���
		if(pASDUID->COT != 0X2A) //������Ӧ����Ŀֵ
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

		unsigned char unNGD = pAsduData[1];//��Ŀ����

		char *pTmpAsduData = pAsduData;
		pTmpAsduData += 2; //����RII(1�ֽ�)+NGD(1�ֽ�)

		//������������Ŀ
		GDD gdd;
		unsigned short GIN = 0;//ͨ�÷����ʶ���
		char KOD = 0;
		int minAttrDataLen = 0;
		unsigned char i=0;
		int iAttrNum = 0;
		for(;i< unNGD && iAttrNum< *outAttrNum; i++)
		{
			memcpy(&GIN,pTmpAsduData,2);//�õ�gin
			pTmpAsduData += 2;
			KOD = *pTmpAsduData;//�õ�kod
			pTmpAsduData += 1;
			memcpy(&gdd,pTmpAsduData,3); //�õ�gdd
			pTmpAsduData += 3;//����GDD�ṹ(3�ֽ�)

			if( (GIN & 0X00FF) > 1)
			{
				minAttrDataLen = min( gdd.dataLen * gdd.dataNum ,*outAttrLen[iAttrNum]);
				memcpy(outAttrData[iAttrNum],pTmpAsduData,minAttrDataLen); //������Խṹ����
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
	//�ٻ�ͨ���ļ��б�
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::GetFileList( )");

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		//pLinker->Close();
		return false;

	}
	else //����������UI����ʱ����
	{
#ifdef _PILING
		//����׮
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
			pComdResult->resultType = CDRT_DIRFILEINF;    //���ؽ�����������
			pComdResult->resultLen = sizeof(CMDDIRFILEINF); //�������1�ֽ�
			pComdResult->pData = new char[sizeof(CMDDIRFILEINF)+1];
			memset(pComdResult->pData,0,sizeof(CMDDIRFILEINF)+1);

			memcpy(pComdResult->pData,&dirFileInf.fileNo,sizeof(dirFileInf));

			pCmd->resultList.push_back(pComdResult);
		}
		return true;
#endif

		//�����������
		if(pCmd->parmLen != 5 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;	//���͵����ݳ���

		unsigned char dirType = pCmd->pParm[0];//Ŀ¼����
		unsigned int fileNum = 0; //������ļ�����0��ʾĿ¼�����е��ļ�
		memcpy(&fileNum,&pCmd->pParm[1],4);

		//asdu10��ִ�е�д�ı���
		unsigned char asdu10_attr[110]={0};	//���Խṹ
		asdu10_attr[0] = dirType;
		memcpy(&asdu10_attr[1],&fileNum,sizeof(unsigned int));

		GI asdu10_gi[1] ={GI(0XA001,0X67,23,110,1,110,&asdu10_attr[0])};

		//ASDU21�����Ʒ��򣩨D�D �����ص��ļ���
		GI asdu21_gi[1] ={GI(0XA002,0X67,1,100,1,0,NULL)};

		char pFileName[100+1] ={0};	//�ļ�����
		char *ppFileName[1] = {&pFileName[0]};
		int  fileNameLen = 100;
		int *pLen[1] = {&fileNameLen};
		int iAttrNum = 1;

		//��ȡͨ�÷�����񷵻ص��ļ�����
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

		//�����жϻ���
		struct tagEnVirionment
		{
			unsigned int startPos;
			char fileName[100+1];	//�ļ�����
			int  fileNameLen;
		};

		unsigned int startPos = 0;	//�ļ���ʼ����λ��

		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetFileList.");

			struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
			startPos = pEV->startPos;
			fileNameLen = pEV->fileNameLen;
			memcpy(pFileName,pEV->fileName,sizeof(pEV->fileName));
		}

		if(pCmd->_state._state == CmdState::CS_STEP9) //�ھŲ�
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetFileList:pregetfile");

			struct tagEnVirionment *pEV = new struct tagEnVirionment;
			pEV->fileNameLen = fileNameLen;
			pEV->startPos = startPos;
			memcpy(pEV->fileName,pFileName,sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//׼����ȡ�ļ�ǰ�Ĵ���
			if( !PreGetFile(pLinker,pCmd,2,pFileName, 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in Protocol60_103::GetFileList()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState(); //�ı�����״̬����״̬
		}

		if(pCmd->_state._state == CmdState::CS_STEP10) //��ʮ��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetFileList:wait for M_CON_NA_3");
			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,":wait for M_CON_NA_3 fail in Protocol60_103::GetFileList()");
				return false;
			}
			pCmd->_state.ToNextState(); //�ı�����״̬����״̬
		}

		char directorData[PRO103_ASDUDATA_MAXSIZE+512]={0}; //Ŀ¼�ļ�����
		int remain = 0 ;	//��ʣ��������û�з���

		while(1)
		{
			//�������
			memset(&directorData[remain],0,(sizeof(directorData) - remain));

			if(pCmd->_state._state == CmdState::CS_STEP11) //��ʮһ��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 11 of GetFileList:send C_PL1_NA_3");
				//����1���û�����(������1���û����ݿ�ʼѭ��)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,":send C_PL1_NA_3 error in Protocol60_103::GetFileList()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState(); //�ı�����״̬����״̬
			}

			if(pCmd->_state._state == CmdState::CS_STEP12) //��ʮ����
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 12 of GetFileList:wait for ASDU104");
				//�ڴ��ɼ�װ�÷���ASDU104����
				char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
				PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
				PASDUID pASDUID = NULL;      //Ӧ�÷������ݵ�Ԫ��ʶ��
				PASDUINFID pASDUInfId = NULL;   //Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

				char *pAsduData=NULL;
				int  AsduDataLen=0;
				int framePacketLen=0;	//֡ʵ�ʵĳ���

				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LERROR,"wait for ASDU104 error in Protocol60_103::GetFileList()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Get an in interrupt frame in Protocol60_103::GetFileList()");
					m_errCode = PEC_INTERRUPT;
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					return false;
				}

				//�Ƿ����ڴ�������
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::GetFileList()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//����ASDU����
				if(pAsduData == NULL || AsduDataLen <= 0)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LDEBUG,"No find ASDU data in Protocol60_103::GetFileList()");
					return false;
				}

				memcpy(&startPos,&pAsduData[0],4);
				int writeLen = AsduDataLen - 4;
				memcpy(&directorData[remain],&pAsduData[4],writeLen);

				//����Ŀ¼�ļ�����
				char szLine[256]={0};//����1������
				int startLinePos = 0;
				int endLinePos = 0;
				while( startLinePos < (remain + writeLen))
				{
					if(directorData[endLinePos] == ';')
					{
						//ȡ1������
						memcpy(szLine,&directorData[startLinePos],endLinePos-startLinePos);
						startLinePos = endLinePos +2;
						endLinePos++;

						//����������
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
						pComdResult->resultType = CDRT_DIRFILEINF;//���ؽ�����������
						pComdResult->resultLen = sizeof(CMDDIRFILEINF);//�������1�ֽ�
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

				if(pVFrameHead->control.m.acd == false) //�������
				{
					//�ɹ�
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
				else
				{
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //��ʮ����

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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		//�����������
		if(pCmd->parmLen != 1+4+4+4+100+19+1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PullFile()");
			return false;
		}

		unsigned char fileType = pCmd->pParm[0];//�ļ�����
		unsigned int fileNO = 0;//�ļ���
		unsigned int fileStartPos = 0;//�ļ���ʼλ��
		unsigned int fileTotalLen = 0;//�ļ��ܳ���
		char inputFileName[100]={0};//������ļ�����
		char fileTime[25]={0};//�ļ�ʱ��:�ַ���,yyyy-mm-dd hh:MM:ss��ʽ
		char IsNeedCompress = 0;//�Ƿ���Ҫѹ����1Ϊ��Ҫ

		memcpy(&fileNO,&pCmd->pParm[1],4);
		memcpy(&fileStartPos,&pCmd->pParm[1+4],4);
		memcpy(&fileTotalLen,&pCmd->pParm[1+4+4],4);
		memcpy(inputFileName,&pCmd->pParm[1+4*3],100);
		memcpy(fileTime,&pCmd->pParm[1+4*3+100],19);
		IsNeedCompress = pCmd->pParm[1+4*3+100+19];

		//�����ļ�ʱ��
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

		FileControl::FILETYPE fileControlType;	//�ļ�����
		switch(fileType)
		{
		case 1:
			fileControlType = FileControl::FT_LOW; //����¼���ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is low-record-file");
			break;
		case 2:
		case 3:
		case 9:
		case 100:
		case 200:
			fileControlType = FileControl::FT_CONT; //����¼���ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is continue-record-wave-file");
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			fileControlType = FileControl::FT_COMTRADE; //��׼comtrade�ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is comtrade file");
			break;
		case 10:
			fileControlType = FileControl::FT_SHD; //SHD�ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SHD file");
			break;
		case 11:
		case 12:
		case 13:
			fileControlType = FileControl::FT_GENERATORTEST; //�����ʵ���ļ��ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is GeneratorTest file");
			break;
		case 101:
			fileControlType = FileControl::FT_SOE; //����¼��ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is SOE file");
			break;
		case 102:
			fileControlType = FileControl::FT_LOG; //�ռ��ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is LOG file");
			break;
		case 210:
		case 211:
		case 212:
		case 220:
			fileControlType = FileControl::FT_CFG; //��������ļ�
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is configer file");
			break;
		default:
			fileControlType = FileControl::FT_CFG; //δ֪���ļ����ͣ��������ļ�ͬ���ڸ�Ŀ¼��
			mylog.WriteLog2File(LogControl::LDEBUG,"Get file type is unkown type file");
			break;
		}

		//��������Ŀ¼(������ز���������Ŀ¼)
		char *pYearMonth = NULL;
		char szYearMonth[7]={0};
		if(fileControlType != FileControl::FT_CFG)
		{
			::itoa(iFileYearMonth,szYearMonth,10);
			pYearMonth = &szYearMonth[0];
		}

		unsigned char asdu10_attr[110]={0};	//���Խṹ
		asdu10_attr[0] = fileType;		//�����ļ�����
		memcpy(&asdu10_attr[1],&fileNO,sizeof(unsigned int));	//�����ļ���
		memcpy(&asdu10_attr[5],&fileStartPos,sizeof(unsigned int));	//�����ļ���ʼλ��
		memcpy(&asdu10_attr[9],inputFileName,100);   //�����ļ�����
		asdu10_attr[109] = IsNeedCompress;	//�Ƿ�ѹ��

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0XA001,0X67,23,110,1,110,&asdu10_attr[0])};

		//ASDU21�����Ʒ��򣩨D�D �����ص��ļ���
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}// end if( pCmd->from == CD_REMOTE ) //����������Զ��
	else
	{
		int msgLen = 0; //��Ϣ����

		//�����������
		if(pCmd->parmLen != 1+20+10+100 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::PushFile()");
			return false;
		}

		unsigned char fileType = pCmd->pParm[0];	//�ļ�����
		unsigned char userName[20]={0};			//�û���
		unsigned char passward[10]={0};		//����
		char localFileName[101]={0};		//�����ļ�����(��·��)
		string sFileName("");			//�ļ���(����·��)
		char ADUFileName[100]={{0}};	//ǰ�û��ļ�����
		char *ppFileName[1] = {&ADUFileName[0]};
		int  fileNameLen = 100;
		int *pFileNameLen[1] = {&fileNameLen};
		int iAttrNum = 1;

		memcpy(&userName[0],&pCmd->pParm[1],20);
		memcpy(&passward[0],&pCmd->pParm[1+20],10);
		memcpy(&localFileName[0],&pCmd->pParm[1+20+10],100);

		//�������ļ�����
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

		unsigned char asdu10_attr[135]={0};  //���Խṹ
		asdu10_attr[0] = fileType;
		memcpy(&asdu10_attr[1],&userName[0],20);
		memcpy(&asdu10_attr[21],&passward[0],10);
		memcpy(&asdu10_attr[35],sFileName.c_str(),sFileName.size());

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0XB001,0X67,23,135,1,135,&asdu10_attr[0])};

		//ASDU21�����Ʒ��򣩨D�D �����ص��ļ���
		GI asdu21_gi[1] ={GI(0XB002,0X67,1,100,1,0,NULL)};

		FileControl fileCtrl;

		//�����жϻ���
		struct tagEnVirionment
		{
			unsigned int startPos;
			char fileName[100+1];	//�ļ�����
			int  fileNameLen;
		};

		unsigned int startPos = 0;	//�ļ���ʼ����λ��

		if(pCmd->_state.pStoreParm != NULL)	//����������ִ�й�
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Resum running PushFile");

			struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
			startPos = pEV->startPos;
			fileNameLen = pEV->fileNameLen;
			memcpy(&ADUFileName[0],pEV->fileName,sizeof(pEV->fileName)-1);
		}

		//��ȡͨ�÷�����񷵻ص��ļ�����
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

		//Asdu107���͵��ļ�����ͷ�Ľṹ
		/*struct _Asdu107FileData
		{
		char fileName[100];	//�ļ���
		struct CP56Time2a fileTime;	//�ļ�ʱ��
		unsigned int fileLen;	//�ļ�����
		} fileData;*/

		//memset(&fileData,0,sizeof(fileData));

		/*memcpy(&fileData.fileName[0],&ADUFileName[0],100);
		fileCtrl.GetFileTime(&localFileName[0], &fileData.fileTime);
		fileData.fileLen = fileCtrl.GetFileLen(&localFileName[0]);*/
		int fileLen = fileCtrl.GetFileLen(&localFileName[0]);
		int hasReadLen = 0; //�ļ��Ѿ���ȡ�ĳ���

		//ÿ�οɴ��ļ���ȡ������ļ����ݳ���
		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;// -sizeof(struct _Asdu107FileData);
		int factReadLen = 0;    //ʵ�ʶ�ȡ�����ݳ���

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
		int maxCount = fileLen/readMaxLen; //���ѭ������
		int iInterval = maxCount/20;	//������ٴ�ѭ��֮��дһ�ν�����Ϣ(ȫ�������д20�ν�����Ϣ)
		iInterval = (iInterval <= 0)?30:iInterval;

		while(1)
		{
			count++;

			int sendDataLen = 0;	//���͵����ݳ���
			FIXFRAME fixFrame;
			memset(&fixFrame,0,sizeof(fixFrame));

			char ASDUData[PRO103_ASDUDATA_MAXSIZE +1] = {0};	//ASDU����
			int  ASDUDataLen = 0;

			memcpy(&ASDUData[0],&startPos,4);		//��ʼ����λ��

			if(pCmd->_state._state == CmdState::CS_STEP9) //��9��
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

				//����ASDU107����
				auto_ptr<string> pDataPack;
				if(factReadLen <= 0 || hasReadLen >= fileLen )
				{
					pDataPack = auto_ptr<string>(CreateMsg(C_EX107_NA_3,pCmd->destDevId,false,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
				}
				else
				{
					pDataPack = auto_ptr<string>(CreateMsg(C_EX107_NA_3,pCmd->destDevId,true,0,CINF_0,&ASDUData[0],ASDUDataLen,&msgLen));
				}

				sendDataLen = SendData(pLinker,pDataPack->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //�������� 
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

			if(pCmd->_state._state == CmdState::CS_STEP10) //��10��
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 10 of PushFile:wait for M_CON_NA_3");
				//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
				int iReturn = WantConfirm(pLinker,pCmd);
				if(iReturn != 1)
				{
					mylog.WriteLog2File(LogControl::LWARNING, "wait for M_CON_NA_3 fail in Protocol60_103::PushFile()");

					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��

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

			//ÿ��iInterval��дһ�ν�����Ϣ
			if(count%iInterval == 0)
			{
				//д������Ϣ
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

				//������
				delete pResult;
				pResult = NULL;
				pCmd->resultNum = 0;
				pCmd->resultList.clear();
			}

			//�����Ѿ��������
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

	//����׮
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//�����������
		if(pCmd->parmLen != 2 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char dataType = pCmd->pParm[0];//��������
		unsigned char StartType = pCmd->pParm[1];//��������

		unsigned char asdu10_attr[2]={0};  //���Խṹ
		asdu10_attr[0] = dataType;
		asdu10_attr[1] = StartType;

		if(StartType == 1)
		{
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_START,"��������") );
		}
		else
		{
			::strcat(pCmd->describe, (CStringRes::GetInstance())->GetStrById(IDS_STOP,"��ֹͣ��") );
		}

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0X6001,0X67,23,2,1,2,&asdu10_attr[0])};

		//����ASDU10��ִ�е�д
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

	if( pCmd->from != CD_ASDU ) //�����������ADU
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{

#ifdef _PILING
		string sFileNameTmp("d:/data/1/FaultReport.txt");

		//дִ�н��
		pCmd->resultNum = 2;
		PCMDRESULT pResultTmp = new CMDRESULT();
		pResultTmp->resultType = CDRT_TRUFALSE;
		pResultTmp->resultLen = 1;
		pResultTmp->pData = new char[2];
		pResultTmp->pData[0] = 1;      //ִ�гɹ�
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

		//����������ADU
		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;    //���͵����ݳ���

		//�����������
		if(pCmd->parmLen != 1+4+100 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::AutoSend()");
			return false;
		}

		//��������
		unsigned char fileType = pCmd->pParm[0];//�ļ�����
		int fileLen = 0;	//�ļ�����
		memcpy(&fileLen,&pCmd->pParm[1],4);
		char pSendFileName[101] = {0}; //ǰ̨�ϴ����ļ���(����·��)
		memcpy(pSendFileName,&pCmd->pParm[1+4],100);
		string sFileName("");			//�ļ���(������·��)
		string sLocalFileName("");		//����·�����ļ���(����·��)

		//�������ļ�����
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

		FileControl::FILETYPE fileControlType;	//�ļ�����
		FileControl fileCtrl;

		unsigned int startPos = 0;	//�ļ���ʼ����λ��
		FileControl::OPENMODE fileOpenMod = FileControl::OM_WRITE;//�ļ��򿪷�ʽ

		if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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

		//����¼��ļ�����Ӳ�̣�����¼������������ݿ�
		if(fileType == 2)
		{
			fileControlType = FileControl::FT_SOE; //����¼��ļ�

			//����ļ����Ŀ¼
			sLocalFileName = fileCtrl.GetFileTypeDirector(devId,fileControlType);

			//��������Ŀ¼(������ز���������Ŀ¼)
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

		if(pCmd->_state._state == CmdState::CS_STEP1) //��һ��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of AutoSend:pregetfile");
			//׼����ȡ�ļ�ǰ�Ĵ���
			if( !PreGetFile(pLinker,pCmd,2,&pSendFileName[0], 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in in Protocol60_103::AutoSend()");
				pCmd->_state.ChangeStateTo(CmdState::CS_FAIL);
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState(); //�ı�����״̬����״̬
		}

		if(pCmd->_state._state == CmdState::CS_STEP2) //�ڶ���
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of AutoSend:wait for M_CON_NA_3");
			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
			if(WantConfirm(pLinker,pCmd) != 1)
			{
				mylog.WriteLog2File(LogControl::LWARNING,"wait for M_CON_NA_3 fail in in Protocol60_103::AutoSend()");
				return false;
			}
			pCmd->_state.ToNextState(); //�ı�����״̬����״̬
		}

		int readMaxLen = PRO103_ASDUDATA_MAXSIZE -sizeof(ASDUINFID) -4;
		int count = 0;
		int maxCount = fileLen/readMaxLen; //���ѭ������
		int iInterval = maxCount/20;    //������ٴ�ѭ��֮��дһ�ν�����Ϣ(ȫ�������д20�ν�����Ϣ)
		iInterval = (iInterval <= 0)?30:iInterval;

		while(1)
		{
			count++;

			if(pCmd->_state._state == CmdState::CS_STEP3) //������
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of AutoSend:send C_PL1_NA_3");
				//����1���û�����(�������û�з����꣬�������1���û����ݿ�ʼѭ��)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"send C_PL1_NA_3 erorr in in Protocol60_103::AutoSend()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState(); //�ı�����״̬����״̬
			}

			//�ڴ��ɼ�װ�÷���ASDU104����
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
			PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
			PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
			PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��
			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;	//֡ʵ�ʵĳ���

			if(pCmd->_state._state == CmdState::CS_STEP4) //���Ĳ�
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of AutoSend:wait for ASDU104");
				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LWARNING,"wait for ASDU104 fail in Protocol60_103::AutoSend()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"AutoSend has be interrupted");

					m_errCode = PEC_INTERRUPT;
					if(pCmd->_state.pStoreParm == NULL)
					{
						unsigned int *pPos = new unsigned int;
						pCmd->_state.pStoreParm = (void*)pPos;
					}

					//�����ж�ʱ�Ĵ���λ��
					*((unsigned int*)(pCmd->_state.pStoreParm)) = startPos;
					return false;
				}

				//�Ƿ����ڴ�������
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::AutoSend()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//����ASDU����
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

				if(fileType == 2)	//����¼�
				{
					fileCtrl.Seek(startPos);
					fileCtrl.WriteData(&pAsduData[4],writeLen);
				}
				else		//���ϱ���
				{
				}

				startPos += writeLen;

				if(pVFrameHead->control.m.acd == false) //�ļ��������
				{
					//�ɹ�
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

					//�����ж�ʱ�Ĵ���λ��
					*((unsigned int*)(pCmd->_state.pStoreParm)) = startPos;
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP3);
				}

				if(count%iInterval == 0)
				{
					//д������Ϣ
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

					//������
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();
				}//end if(count%iInterval == 0)

			}//end if(pCmd->_state._state == CmdState::CS_STEP4) //���Ĳ�

		} //end while

		if(fileType == 2)   //����¼�
		{
			fileCtrl.CloseFile();
		}

		//дִ�н��
		pCmd->resultNum = 2;
		PCMDRESULT pResult = new CMDRESULT;
		pResult->resultType = CDRT_TRUFALSE;
		pResult->resultLen = 1;
		pResult->pData = new char[2];
		pResult->pData[0] = 1;      //ִ�гɹ�
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

	int index = GetPro2CmdIndex(cmd);   //��ȡ�����Ӧ��ӳ������
	if( index < 0 )
	{
		mylog.WriteLog2File(LogControl::LERROR,"No find the index of map.Invalid command in Protocol60_103::ExecuteCmd()");
		return false;
	}

	if( pro2CmdMap[index].exuteCmd > 0)
	{
		strcpy(cmd.describe,pro2CmdMap[index].cmdName);
		return pro2CmdMap[index].exuteCmd(this,pLinker,cmd); //ִ�������
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

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;	//���͵����ݳ���

	FIXFRAME fixFrame;	//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));

	/****************C_RCU_NA_3(��λͨ�ŵ�Ԫ)-->*********/
	//����C_RCU_NA_3����λͨ�ŵ�Ԫ��
	auto_ptr<string> pQuest( CreateMsg(C_RCU_NA_3,0,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //���ͻش�֡
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

	//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ�ϣ�ACD=1)
	if(WantConfirm(pLinker,NULL) != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "Wait for M_CON_NA_3 faile in Protocol60_103::InitConnect( )");
		return false;
	}

	//����1���û�����
	if(!SendLevelOne(pLinker,0))
	{
		mylog.WriteLog2File(LogControl::LERROR, "SendLevelOne() return error in Protocol60_103::InitConnect( )");
		return false;
	}

	//�ڴ�M_IRC_NA_3��ASDU5��COT=��λͨ�ŵ�Ԫ��
	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};	//����ɱ�֡����
	PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
	PASDUID pASDUID = NULL;		//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	unsigned char parmLen =0;

	int framePacketLen=0;   //֡ʵ�ʵĳ���

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

	//�Ƿ����ڴ�������
	if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_5)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU5,but it isn't in Protocol60_103::InitConnect( )");
		return false;
	}

	//�жϴ���ԭ���Ƿ��Ǹ�λͨ�ŵ�Ԫ��COT=0x04��
	if(pASDUID->COT != 0X04) //���Ǹ�λͨ�ŵ�Ԫ
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"The command isn't the response for M_IRC_NA_3 in Protocol60_103::InitConnect( ).");
		return false;
	}

	/******************��λ֡����λ-->*****************/

	//����C_RFB_NA_3����λ֡����λ��
	string *pTemp = pQuest.release();
	if(pTemp != NULL)
	{
		delete pTemp;
		pTemp = NULL;
	}
	pQuest = auto_ptr<string>( CreateMsg(C_RFB_NA_3,0,false,0,0,NULL,0,&msgLen) );
	if( pQuest->c_str() != NULL)
	{
		sendDataLen = SendData(pLinker,pQuest->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //���ͻش�֡
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

	//�ڴ��ɼ�װ�÷���M_CON_NA_3 ��ȷ�ϣ�ACD=1��
	if(WantConfirm(pLinker,NULL) != 1)
	{
		mylog.WriteLog2File(LogControl::LWARNING, "Wait for M_CON_NA_3 faile in Protocol60_103::InitConnect( )");
		return false;
	}

	//����1���û�����
	if(!SendLevelOne(pLinker,0))
	{
		mylog.WriteLog2File(LogControl::LERROR, "SendLevelOne return error in Protocol60_103::InitConnect( )");
		return false;
	}

	//�ڴ�M_IRF_NA_3��ASDU5��COT=��λ֡����λ��
	memset(&framePacket[0],0,sizeof(framePacket));  //����ɱ�֡����
	pVFrameHead = NULL;	//֡ͷ
	pASDUID = NULL;		//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

	*pAsduData=NULL;
	AsduDataLen=0;
	parmLen =0;

	framePacketLen=0;   //֡ʵ�ʵĳ���

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

	//�Ƿ����ڴ�������
	if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_5)
	{
		m_errCode = PEC_ERR_ANSWER;
		mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU5,but it isn't in Protocol60_103::InitConnect( )");
		return false;
	}

	//�жϴ���ԭ���Ƿ��Ǹ�λ֡����λ��COT=0x03��
	if(pASDUID->COT != 0X03) //���Ǹ�λ֡����λ
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
	const int PACKET_LEN = 64*1024;	//������

	char framePacket[PACKET_LEN] = {0};	//����ɱ�֡����
	int framePacketLen=0;   //֡ʵ�ʵĳ���
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;

	}
	else //����������UI����ʱ����
	{
		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;	//���͵����ݳ���

		unsigned char asdu10_attr[4]={0};//���Խṹ
		asdu10_attr[0] = 2;	//����¼��
		asdu10_attr[1] = 2;	//�趨����״̬
		asdu10_attr[2] = 0;
		asdu10_attr[3] = 1;	//��������״̬��ʱ��(1����)

		//asdu10��ִ�е�д�ı���
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
	FIXFRAME fixFrame;	//�̶���֡
	memset(&fixFrame,0,sizeof(fixFrame));
	int iReturn = 0;

	if(pCmd->_state._state == CmdState::CS_STEP1) //��һ��
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 1 of GetGIAttrData:send asdu10.");
		auto_ptr<string> pCmdAsdu10(CreateGIMsg(C_GD_NA_3,pCmd->destDevId,0X2A,CINF_250,asdu10_GIs,asdu10_GINumber,&msgLen));
		if( pCmdAsdu10.get() != NULL)
		{
			sendDataLen = SendData(pLinker,pCmdAsdu10->c_str(),msgLen,SystemCfger::GetInstance().timeOut); //����asdu10��ִ�е�д�ı��� 
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

	if(pCmd->_state._state == CmdState::CS_STEP2) //�ڶ���
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 2 of GetGIAttrData:wait for M_CON_NA_3.");
		//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
		iReturn = WantConfirm(pLinker,pCmd);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"No get M_CON_NA_3 in Protocol60_103::SendAsdu10WithExeWrite().");
			return iReturn;
		}
		pCmd->_state.ToNextState();
	}

	if(pCmd->_state._state == CmdState::CS_STEP3) //������
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 3 of GetGIAttrData:send C_PL1_NA_3.");
		//����1���û�����
		if(!SendLevelOne(pLinker,pCmd->destDevId))
		{
			mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::SendAsdu10WithExeWrite().");
			return -1;
		}
		pCmd->_state.ToNextState();
	}

	//�ڴ�Asdu10��ִ��дȷ��
	char framePacket[PRO103_VFRAME_MAXSIZE] = {0};//�ɱ�֡����
	PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
	PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

	char *pAsduData=NULL;
	int  AsduDataLen=0;
	int framePacketLen=0;	//֡ʵ�ʵĳ���

	if(pCmd->_state._state == CmdState::CS_STEP4) //���Ĳ�
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Run step 4 of GetGIAttrData:wait for asdu10 with answer.");

		//ȡǰ̨�Ļش�֡
		int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
			&framePacketLen,
			&pVFrameHead,
			&pASDUID,
			&pASDUInfId,
			&pAsduData,
			&AsduDataLen,
			NULL,
			SystemCfger::GetInstance().timeOut);

		if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
		{
			mylog.WriteLog2File(LogControl::LERROR,"Wait for asdu10 with answer error in Protocol60_103::SendAsdu10WithExeWrite().");
			pLinker->Close();
			return -1;
		}
		if( iReturn == 3) //�ж�֡
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Get an interrupt frame in Protocol60_103::SendAsdu10WithExeWrite().");
			return 0;
		}

		//�Ƿ����ڴ�������
		if( pVFrameHead->control.c.fun != CFUNID_8 || pASDUID->TYP != ASDU_10)
		{
			m_errCode = PEC_ERR_ANSWER;
			mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU10,but it isn't in Protocol60_103::SendAsdu10WithExeWrite()");
			//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU10,but it isn't");
			return -1;
		}

		//�жϷ���
		if(pASDUID->COT != 0X28) //�񶨵�ȷ��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"The answer is negative in Protocol60_103::SendAsdu10WithExeWrite().");

			m_errCode = PEC_ERRCMD_NODATA;
			pCmd->resultNum = 1;
			PCMDRESULT pComdResult = new CMDRESULT;
			pComdResult->resultType = CDRT_TRUFALSE;	//���صĽ�����������
			pComdResult->resultLen = 1;					//�������1�ֽ�
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		//�����������
		if((pCmd->parmLen != 1+4+4+4+12+24+sizeof(unsigned short) && 
			pCmd->parmLen != 1+4+4+1+12+24 + sizeof(DataControl::DataQue*))
			|| pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		int uTimeOut = 0;//��ʱʱ��
		int iOldWaitTime = SystemCfger::GetInstance().timeOut;

		unsigned int uiStartTime = 0;	//��ʼʱ��(time_t����)
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

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0X4001,0X67,23,uSendParmLen,1,uSendParmLen,(unsigned char*)pCmd->pParm)};

		//ASDU21�����Ʒ��򣩨D�D �����ص��ļ���
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
			//����Զ�̵��������д���ڴ���
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

	//����ļ����Ŀ¼
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

	//��������Ŀ¼(������ز���������Ŀ¼)
	if(YearMonth != NULL)
	{
		filePath = filePath + string(YearMonth) + string("/");
		if(!fileCtrl.IsExist(filePath.c_str()))
		{
			_mkdir(filePath.c_str());
		}
	}

	char fileName[5][100+1]={{0},{0},{0},{0},{0}};	//�ļ�����
	char *ppFileName[5] = {&fileName[0][0],&fileName[1][0],&fileName[2][0],&fileName[3][0],&fileName[4][0]};
	int  fileNameLen[5][1] = {{100},{100},{100},{100},{100}};
	int *pFileNameLen[5] = {&fileNameLen[0][0],&fileNameLen[1][0],&fileNameLen[2][0],&fileNameLen[3][0],&fileNameLen[4][0]};
	int iAttrNum = 5;

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;	//���͵����ݳ���

	string	sDownInfFile="";//����������Ϣ����ʱ�ļ�������ϵ�������Ϣ��

	if(FilePartName != NULL)
		sDownInfFile = filePath + string(FilePartName)+string(".down");

	//�����жϻ���
	struct tagEnVirionment
	{
		char FinishFileName[400+1];//������ɵ��ļ�����
		unsigned int startPos;
		char fileName[100+1];//��ǰ�����ļ�����
		char IsNeedCompress;//�Ƿ�Ҫ����ѹ��
	};
	struct tagEnVirionment *pEV = NULL;

	unsigned int startPos = 0;	//�ļ���ʼ����λ��
	FileControl::OPENMODE openMode = FileControl::OM_WRITE;

	if(pCmd->_state.pStoreParm != NULL)  //����������ִ�й�
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

		//��鱣��������Ϣ����ʱ�ļ��Ƿ����
		if( fileCtrl.IsExist( sDownInfFile.c_str() ) )
		{
			//��ȡ������Ϣ
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

	//��ȡͨ�÷�����񷵻ص��ļ�����
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
	vector< PCMDRESULT > fileList;//������յ����ļ��б�

	char szInf[512] = {0};
	bool bHasError = false;

	//ѭ���������ļ�
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
			//���ļ������ع�
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
		
		string writeFileName;   //�������ݵ��ļ�������·����
		string sSourceFileName;	//ԭ�ļ���
		string sZipFileName;	//ѹ�����ļ���

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

		//Ҫ����ѹ���ļ�
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

		if(pCmd->_state._state == CmdState::CS_STEP9) //�ھŲ�
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetGeneraFile:pregetfile");

			pEV->startPos = startPos;
			memcpy(pEV->fileName,&fileName[i][0],sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//׼����ȡ�ļ�ǰ�Ĵ���
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

		if(pCmd->_state._state == CmdState::CS_STEP10) //��10��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetGeneraFile:wait for M_CON_NA_3");
			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
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
		int maxCount = fileTotalLen/readMaxLen; //���ѭ������
		int iInterval = maxCount/20;    //������ٴ�ѭ��֮��дһ�ν�����Ϣ(ȫ�������д20�ν�����Ϣ)
		iInterval = (iInterval <= 0)?30:iInterval;

		while( !bHasError )
		{
			count++;

			if(pCmd->_state._state == CmdState::CS_STEP11) //��11��
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 11 of GetGeneraFile:send C_PL1_NA_3");
				//����1���û�����(�������û�з����꣬�������1���û����ݿ�ʼѭ��)
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

			//�ڴ��ɼ�װ�÷���ASDU104����
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //����ɱ�֡����
			PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
			PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
			PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;   //֡ʵ�ʵĳ���

			if(pCmd->_state._state == CmdState::CS_STEP12) //��12��
			{
				mylog.WriteLog2File(LogControl::LDETAIL,"Run step 12 of GetGeneraFile:wait for ASDU104");
				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LWARNING,"Wait for ASDU104 fail in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					bHasError = true;
					break;
					//return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetGeneraFile has be interrupted in Protocol60_103::GetGeneraFile()");

					m_errCode = PEC_INTERRUPT;
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					bHasError = true;
					break;
					//return false;
				}

				//�Ƿ����ڴ�������
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

				//����ASDU����
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

				if(pVFrameHead->control.m.acd == false) //�������
				{
					fileCtrl.CloseFile();

					//Ҫ����ѹ���ļ�
					if( IsNeedCompress == 1)
					{
						sZipFileName = sSourceFileName + string(".SHZip");
						writeFileName = sZipFileName;

						//��ѹ�ļ�
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

					//�ɹ�
					if( i<iAttrNum )//�����ļ�Ҫ�ٻ�
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
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

				//ÿ��iInterval��дһ�ν�����Ϣ
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
						//�ļ�����δ֪,�������ٵĽ�����Ϣ
						if(iProgress < 95)
						{
							iProgress +=1;
						}
					}
					else
					{
						iProgress = 100*startPos/fileTotalLen;  //�������
					}
					iProgress = (iProgress>100)?100:iProgress;
					pResult->pData[0] = iProgress;
					pCmd->resultList.push_back(pResult);
					memMgr.WriteTaskResultToMem(*pCmd);

					//������
					delete pResult;
					pResult = NULL;
					pCmd->resultNum = 0;
					pCmd->resultList.clear();

				}//end if(pCmd->from == CD_UI && count%iInterval == 0)

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //��12��

		} //end while

		fileCtrl.CloseFile();

		//�����ļ�·����
		pResult = new CMDRESULT;
		pResult->resultType = CDRT_FILEPATH;
		pResult->resultLen = writeFileName.size();
		pResult->pData = new char[writeFileName.size()+1];
		pResult->pData[writeFileName.size()] = 0 ;
		memcpy(pResult->pData,writeFileName.c_str(),writeFileName.size());

		fileList.push_back(pResult);
	}//end for() //ѭ���������ļ�

	if(bHasError)//ʧ��
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
	else//�ɹ�
	{
		//���������Ϣ�ļ�
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
	pResult->pData[0] = 1 ; //ִ�гɹ�
	pResult->pData[1] = 0 ;

	pCmd->resultList.insert(pCmd->resultList.begin(), fileList.begin(),fileList.end());
	pCmd->resultList.push_back(pResult);

	return true;
}

bool Protocol60_103::GetGeneraFile( Linker * pLinker,struct Command *pCmd,DataControl::DataQue *pDataQue,GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber)
{
	char fileName[5][100+1]={{0},{0},{0},{0},{0}};   //�ļ�����
	char *ppFileName[5] = {&fileName[0][0],&fileName[1][0],&fileName[2][0],&fileName[3][0],&fileName[4][0]};
	int  fileNameLen[5] = {100,100,100,100,100};
	int *pFileNameLen[5] = {&fileNameLen[0],&fileNameLen[1],&fileNameLen[2],&fileNameLen[3],&fileNameLen[4]};
	int iAttrNum =5;

	int msgLen = 0; //��Ϣ����
	int sendDataLen = 0;    //���͵����ݳ���

	//�����жϻ���
	struct tagEnVirionment
	{
		unsigned int startPos;
		char fileName[100+1];	//�ļ�����
		int  fileNameLen;
	};

	unsigned int startPos = 0;	//�ļ���ʼ����λ��

	if(pCmd->_state.pStoreParm != NULL)	//����������ִ�й�
	{
		mylog.WriteLog2File(LogControl::LDEBUG,"Resum running GetGeneraFile");

		struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
		startPos = pEV->startPos;
		fileNameLen[0] = pEV->fileNameLen;
		memcpy(&fileName[0][0],pEV->fileName,sizeof(pEV->fileName));
	}

	//��ȡͨ�÷�����񷵻ص��ļ�����
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

	//ѭ���������е��ļ�
	for(int i=0;i < iAttrNum; i++)
	{
		sprintf(szInf,"Begin get file : %s in Protocol60_103::GetGeneraFile()",&fileName[i][0]);
		mylog.WriteLog2File(LogControl::LDEBUG,szInf);

		startPos = 0;

		if(pCmd->_state._state == CmdState::CS_STEP9) //�ھŲ�
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 9 of GetGeneraFile:pregetfile");

			struct tagEnVirionment *pEV = new struct tagEnVirionment;
			pEV->fileNameLen = fileNameLen[0];
			pEV->startPos = startPos;
			memcpy(pEV->fileName,&fileName[0][0],sizeof(pEV->fileName));

			pCmd->_state.pStoreParm = (void*)pEV;

			//׼����ȡ�ļ�ǰ�Ĵ���
			if( !PreGetFile(pLinker,pCmd,2,&fileName[i][0], 100, startPos) )
			{
				mylog.WriteLog2File(LogControl::LWARNING,"PreGetFile return fail in Protocol60_103::GetGeneraFile()");
				pLinker->Close();
				return false;
			}
			pCmd->_state.ToNextState();
		}

		if(pCmd->_state._state == CmdState::CS_STEP10) //��10��
		{
			mylog.WriteLog2File(LogControl::LDEBUG,"Run step 10 of GetGeneraFile:wait for M_CON_NA_3");
			//�ڴ��ɼ�װ�÷���M_CON_NA_3(ȷ��֡)
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
			if(pCmd->_state._state == CmdState::CS_STEP11) //��11��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 11 of GetGeneraFile:send C_PL1_NA_3");
				//����1���û�����(�������û�з����꣬�������1���û����ݿ�ʼѭ��)
				if(!SendLevelOne(pLinker,pCmd->destDevId))
				{
					mylog.WriteLog2File(LogControl::LERROR,"Send C_PL1_NA_3 error in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					return false;
				}
				pCmd->_state.ToNextState();
			}

			//�ڴ��ɼ�װ�÷���ASDU104����
			char framePacket[PRO103_VFRAME_MAXSIZE] = {0};  //����ɱ�֡����
			PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
			PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
			PASDUINFID pASDUInfId = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��

			char *pAsduData=NULL;
			int  AsduDataLen=0;
			int framePacketLen=0;   //֡ʵ�ʵĳ���

			if(pCmd->_state._state == CmdState::CS_STEP12) //��12��
			{
				mylog.WriteLog2File(LogControl::LDEBUG,"Run step 12 of GetGeneraFile:wait for ASDU104");
				//ȡǰ̨�Ļش�֡
				int iReturn = GetFrame(pLinker,pCmd,NULL,framePacket,
					&framePacketLen,
					&pVFrameHead,
					&pASDUID,
					&pASDUInfId,
					&pAsduData,
					&AsduDataLen,
					NULL,
					SystemCfger::GetInstance().timeOut);

				if( iReturn != 2 && iReturn != 3) //���ǿɱ�֡Ҳ�����ж�֡
				{
					mylog.WriteLog2File(LogControl::LWARNING,"Wait for ASDU104 fail in Protocol60_103::GetGeneraFile()");
					pLinker->Close();
					return false;
				}
				if( iReturn == 3) //�ж�֡
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"GetGeneraFile has be interrupted");

					m_errCode = PEC_INTERRUPT;
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					return false;
				}

				//�Ƿ����ڴ�������
				if( pVFrameHead->control.m.fun != CFUNID_8 || pASDUID->TYP != ASDU_104)
				{
					m_errCode = PEC_ERR_ANSWER;
					mylog.WriteLog2File(LogControl::LERROR,"want the command ASDU104,but it isn't in Protocol60_103::GetGeneraFile()");
					//mylog.WriteLog(LogControl::LWARNING,pCmd,"want the frame ASDU104,but it isn't");
					pLinker->Close();
					return false;
				}

				//����ASDU����
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

				if(pVFrameHead->control.m.acd == false) //�������
				{
					//�ɹ�
					pDataQue->Cancle();
					pCmd->_state.ChangeStateTo(CmdState::CS_SUCCESS);
					break;
				}
				else
				{
					struct tagEnVirionment *pEV = (struct tagEnVirionment *)(pCmd->_state.pStoreParm);
					pEV->startPos = startPos; //�����ж�ʱ�Ĵ���λ��
					pCmd->_state.ChangeStateTo(CmdState::CS_STEP11);
				}

			}//end if(pCmd->_state._state == CmdState::CS_STEP12) //��12��

		} //end while

	}//end for() //ѭ���������е��ļ�

	return true;
}

bool Protocol60_103::ReadZeroOffset( Linker * pLinker, struct Command * pCmd )
{
	mylog.WriteLog2File(LogControl::LINFO, "Enter Protocol60_103::ReadZeroOffset( )");

#ifdef _PILING

	//����׮
	return true;
#endif

	bool result = true;

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//�����������
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char dataType = pCmd->pParm[0];//��������

		unsigned char asdu10_attr[1]={0};  //���Խṹ
		asdu10_attr[0] = dataType;

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0X5001,0X67,3,1,1,1,&asdu10_attr[0])};

		//����ASDU10��ִ�е�д
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//�����������
		if(pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetFileList()");
			return false;
		}

		unsigned char asdu10_attr[PRO103_ASDUDATA_MAXSIZE +1] = {0}; //���Խṹ
		memcpy(asdu10_attr, pCmd->pParm, pCmd->parmLen-4);

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0XE001,0X67,23,(unsigned char)pCmd->parmLen,1,pCmd->parmLen,&asdu10_attr[0])};

		//����ASDU10��ִ�е�д
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		int iWaitTime = 13*1000;	//��ʱʱ��
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

		//ASDU21�����Ʒ��򣩨D�D ���ļ���
		GI asdu21_gi[1] ={GI(0XE102,0X67,23,12,1,0,NULL)};

		//��ȡͨ�÷�����񷵻ص����Խṹ����
		char fileName[100] = {0};
		char	*pfile[1] = {(char*)&fileName};
		int  fileNameLen = 100;
		int *pDataLen[1] = {&fileNameLen};
//		memset(&timeData,0,fileNameLen);
		int iAttrNum = 1;

		unsigned char RII = 0;
		SystemCfger::GetInstance().timeOut = iWaitTime;	//���賬ʱʱ��
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,pfile,pDataLen,&iAttrNum,&RII);
		SystemCfger::GetInstance().timeOut = iOldWaitTime;	//�ָ���ʱʱ��
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::StopGnExperiment()");

			if(iReturn == -1)
			{
				pLinker->Close();
			}
			return false;
		}

		//д���
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;//���͵����ݳ���

		//��������
		if(pCmd->parmLen != 1 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetDeviceState()");
			return false;
		}

		unsigned char type = pCmd->pParm[0];//����

		unsigned char asdu10_attr[1]={0};  //���Խṹ
		asdu10_attr[0] = type; //����

		//asdu10��ִ�е�д�ı���
		GI asdu10_gi[1] ={GI(0X1001,0X67,3,1,1,1,&asdu10_attr[0])};

		//ASDU21(���Ʒ���)--�����ص��Լ���Ϣ
		GI asdu21_gi[1] ={GI(0X1002,0X67,3,1,1,0,NULL)};

		char state[5] = {0};
		char *ppState[1] = {&state[0]};
		int  iDataLen = 5;
		int *pDataLen[1] = {&iDataLen};
		int iAttrNum = 1;

		PCMDRESULT pCmdResult = new CMDRESULT;

		//��ȡͨ�÷�����񷵻ص����Խṹ����
		int iReturn = GetGIAttrData(pLinker,pCmd,asdu10_gi,1,asdu21_gi,1,ppState,pDataLen,&iAttrNum);
		if(iReturn != 1)
		{
			mylog.WriteLog2File(LogControl::LWARNING,"GetGIAttrData return fail in Protocol60_103::GetDeviceState()");

			//����ʧ�ܽ��
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

		//���Լ���Ϣ��������
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;    //���͵����ݳ���

		//�����������
		if(pCmd->parmLen != 1+1+2 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::SetDeviceState()");
			return false;
		}

		unsigned char asdu10_attr[4]={0};  //���Խṹ
		asdu10_attr[0] = pCmd->pParm[0];//����
		asdu10_attr[1] = pCmd->pParm[1];//�趨״̬
		memcpy(&asdu10_attr[2],&pCmd->pParm[2],2);

		//asdu10��ִ�е�д�ı���
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "The protocol Protocol60_103 only surport the communication between the ADU and the SHNeuron");
		return false;
	}
	else //����������UI����ʱ����
	{
		int msgLen = 0; //��Ϣ����
		int sendDataLen = 0;	//���͵����ݳ���

		//�����������
		if(pCmd->parmLen != 1+4 || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::OperateDevice()");
			return false;
		}

		unsigned char asdu10_attr[5]={0};  //���Խṹ
		asdu10_attr[0] = pCmd->pParm[0];//����
		memcpy(&asdu10_attr[1],&pCmd->pParm[1],4);//���Ͳ���

		switch(asdu10_attr[0])
		{
		case 1:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_QUY_DEVSTAT,"���澯���飩"));
			break;
		case 2:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_START_REVERT,"���������飩"));
			break;
		case 3:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_DEV_CHECK_SELF,"���豸�Լ죩"));
			break;
		default:
			::strcat(pCmd->describe,(CStringRes::GetInstance())->GetStrById(IDS_UNKOWN_OPER,"��δ֪�Ĳ�����"));
			break;
		}

		//asdu10��ִ�е�д�ı���
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

	if( pCmd->from == CD_REMOTE ) //����������Զ��
	{
		m_errCode = PEC_ERRCMD_NODATA;
		mylog.WriteLog2File(LogControl::LERROR, "Don't support the command");
		return false;
	}
	else
	{
		//�����������
		if(pCmd->parmLen != 100+1+24*sizeof(unsigned int) || pCmd->pParm == NULL)
		{
			m_errCode = PEC_ERRCMD_NODATA;
			mylog.WriteLog2File(LogControl::LERROR,"error command param in Protocol60_103::GetSplitRecWaveFile()");
			return false;
		}

		unsigned char temp[1] = {0};
		GI asdu10_gi[1] ={GI(0XE201,0X67,23,(unsigned char)pCmd->parmLen,1,pCmd->parmLen,(unsigned char*)pCmd->pParm)};

		//ASDU21�����Ʒ��򣩨D�D ���ļ���
		GI asdu21_gi[1] ={GI(0XE202,0X67,23,100,1,0,NULL)};

		//��ȡͨ�÷�����񷵻ص����Խṹ����
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

		//д���
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
	int index = GetPro2CmdIndex(cmd);   //��ȡ�����Ӧ��ӳ������
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

	PVFRAMEHEAD pVFrameHead = NULL;	//֡ͷ
	PASDUID pASDUID = NULL;	//103Э��Ӧ�÷������ݵ�Ԫ��ʶ��
	PASDUINFID pASDUInfId = NULL;//��Ϣ���ʶ��
	PVFRAMEEND pVFrameEnd = NULL;
	unsigned char checksum; //У���

	char *pAsduData=NULL;
	int  AsduDataLen=0;

	//�����ͻ��˵�IP��ַ���Ӷ��õ�����UDP��Ϣ��DAU���
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
	
	int vFrameHeadLen = sizeof(VFRAMEHEAD); //֡ͷ����
	int ASDUIdLen   = sizeof(ASDUID);	//ASDU id ����
	int ASDUInfLen   = sizeof(ASDUINFID);	//ASDU ��Ϣ�� ����

	pVFrameHead =(PVFRAMEHEAD) &pUdpDataParm->pData[0];
	pASDUID = (PASDUID)&pUdpDataParm->pData[sizeof(VFRAMEHEAD)];
	pASDUInfId = (PASDUINFID)&pUdpDataParm->pData[sizeof(VFRAMEHEAD) + sizeof(ASDUID)];

	while(1)
	{

		//У��֡����Ч��
		//���������ַ��Ƿ����
		if(pVFrameHead->HEAD.startChar1 != pVFrameHead->HEAD.startChar2 
			|| pVFrameHead->HEAD.startChar1 != PRO103_VOLATILEFRAME_STARTCHAR)
		{
			//����asdu֡
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

		//�����ַ��Ƿ���ȷ
		if(pVFrameEnd->endChar != PRO103_ENDCHAR)
		{
			mylog.WriteLog2File(LogControl::LERROR,"The endchar is error in DelUdpDataThread()");
			break;
		}

		//У���
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

		//�Ƿ���ASDU10��
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
			case 0XD001:	//����¼��������Ϣ
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

			case 0XD002://������Ϣ(��־)���־�¼�
				type = pAsduData[0];
				if( type == 1)//��־�¼�
				{
					mylog.WriteLog2File(LogControl::LDEBUG,"Get mark event info  in DelUdpDataThread()");
					cmd.funCode = CMD_AUTOSEND_MARKEVENT;
					pResult->resultType = CDRT_MARKEVENT;
				}
				else //������Ϣ
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
