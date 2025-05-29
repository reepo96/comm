// {{{RME classifier 'Logical View::Timer'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "Timer.h"
#endif

#include <RTSystem/Communication.h>
#include <Timer.h>
#include <MsgQueue.h>
#include <QueRegister.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* �ļ����ƣ�Timer.cpp
* �ļ���ʶ��
* ժ    Ҫ����ʱ��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include <time.h>

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

const unsigned char FAULTREC_FILETYPE = 4;	//����¼���ļ�����
const unsigned char RUNLOG_FILETYPE = 102;	//�����ռ��ļ�����
const unsigned char EVENT_FILETYPE = 101;	//����¼��ļ�����
const unsigned char DSM_FILETYPE = 1;	//����¼���ļ�����
const unsigned char DCR_FILETYPE = 2;	//��̬��¼�ļ�����
const unsigned char TRH_FILETYPE = 3;	//���������ļ�����

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
Timer::~Timer( void )
{
}
// }}}RME

// {{{RME operation 'Timer()'
Timer::Timer( void )
	// {{{RME tool 'OT::Cpp' property 'ConstructorInitializer'
	// {{{USR

	// }}}USR
	// }}}RME
{
	// {{{USR
	m_iMsgId = 0;
	m_pMsgQue = NULL;
	m_pMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMERQUE_NAME);

	m_pTimerResultMsgQue = NULL;
	m_pTimerResultMsgQue = QueRegister::GetInstance().RegisterQue(COMM_TIMER_RESULTQUE_NAME);
	// }}}USR
}
// }}}RME

// {{{RME operation 'Run()'
void Timer::Run( void )
{
	// {{{USR
	HANDLE hTread = CreateThread(NULL,0,&GetTimerTaskResult,this,0,NULL);
	::CloseHandle(hTread);

	while(1)
	{
		time_t currentTime =  time(NULL);

		map< unsigned char,PCFGTIMERCALL>& TimerCallMap = SystemCfger::GetInstance().DauTimerCallMap;
		map< unsigned char,PCFGTIMERCALL>::iterator it = TimerCallMap.begin();
		for(;it != TimerCallMap.end();it++)
		{
			PCFGTIMERCALL pTimerCall = (*it).second;
			if(pTimerCall->ttStartTime <= 0 || pTimerCall->intervalTime <= 0)
				continue;

			//���￪ʼʱ���ﵽ���ʱ��
			if( (currentTime - pTimerCall->ttStartTime)%pTimerCall->intervalTime == 0 )
			{
				SendTask(pTimerCall);
			}
		}

		::Sleep(1000);
	}
	// }}}USR
}
// }}}RME

// {{{RME operation 'SendTask(PCFGTIMERCALL)'
void Timer::SendTask( PCFGTIMERCALL pTimerCall )
{
	// {{{USR
		if(pTimerCall->isCallAll) //�Զ��ٻ���������
		{
			/*GetFileList(pTimerCall->DAUID,90);
			GetFileList(pTimerCall->DAUID,91);
			GetFileList(pTimerCall->DAUID,92);
			GetFileList(pTimerCall->DAUID,93);
			GetFileList(pTimerCall->DAUID,95);
			GetFileList(pTimerCall->DAUID,96);*/

			GetRecWaveFileList(pTimerCall->DAUID);
			return;
		}

		if(pTimerCall->isCallEventData)//�Զ��ٻ���־�¼�����
		{
			GetFileList(pTimerCall->DAUID,92);
		}
		if(pTimerCall->isCallFaultData)//�Զ��ٻ���������
		{
			//GetFileList(pTimerCall->DAUID,90);
			GetRecWaveFileList(pTimerCall->DAUID);
		}
		if(pTimerCall->isCallLogData)//�Զ��ٻ���־����
		{
			GetFileList(pTimerCall->DAUID,91);
		}
		if(pTimerCall->isCallLowRecData)//�Զ��ٻ�����¼������
		{
			GetFileList(pTimerCall->DAUID,93);
		}
		if(pTimerCall->isCallQualityData)//�Զ��ٻ�������������
		{
			//GetFileList(pTimerCall->DAUID,94);
		}
		if(pTimerCall->isCallDynaData)//�Զ��ٻ���̬��¼����
		{
			GetFileList(pTimerCall->DAUID,95);
		}
		if(pTimerCall->isCallConstData)//�Զ��ٻ���������
		{
			GetFileList(pTimerCall->DAUID,96);
		}

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileList(unsigned char,unsigned char)'
int Timer::GetFileList( unsigned char devId, unsigned char dirType )
{
	// {{{USR
		char szInfo[125]={0};
		sprintf(szInfo,"Send command:GetFileList by timer,dest DAU id=%d,directory type=%d",devId,dirType);
		mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

		Task* pTask = new Task();
		pTask->Linker = NULL;

		m_iMsgId++;
		pTask->cmd.msgId = m_iMsgId;

		pTask->cmd.destDevId = devId;	//�豸���
		pTask->cmd.from = CD_TIMER;	//������Դ
		pTask->cmd.to = CD_ASDU;	//����Ŀ�ĵ�
		pTask->cmd.funCode = CMD_GETFILELIST;	//������
		pTask->cmd.parmLen = 1 +sizeof(int);	//�����ܳ���

		//������������
		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

		pTask->cmd.pParm[0] = dirType;

		//������ļ���
		int fileNum = 0;    //�����ļ�
		memcpy(&pTask->cmd.pParm[1],&fileNum,sizeof(int));   //��������

		if(!m_pMsgQue->SendMsg(pTask))
		{
			delete pTask;
			pTask = NULL;
			return -1;
		}

		return m_iMsgId;
	// }}}USR
}
// }}}RME

// {{{RME operation 'PullFile(unsigned char,unsigned char,int,int,char *,char *)'
int Timer::PullFile( unsigned char devId, unsigned char fileType, int fileNo, int fileLen, char * fileName, char * fileTime )
{
	// {{{USR
		char szInfo[512]={0};
		sprintf(szInfo,"Send command:PullFile by timer,dest DAU id=%d,fileType=%d,fileNo=%d,fileName=%s,fileTime=%s",
			devId,fileType,fileNo,fileLen,fileName,fileTime);
		mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

		Task* pTask = new Task();
		pTask->Linker = NULL;

		m_iMsgId++;
		pTask->cmd.msgId = m_iMsgId;

		pTask->cmd.destDevId = devId;	//�豸���
		pTask->cmd.from = CD_TIMER;	//������Դ
		pTask->cmd.to = CD_ASDU;	//����Ŀ�ĵ�
		pTask->cmd.funCode = CMD_PULLFILE;	//������
		pTask->cmd.parmLen = 1 +3*4 +100 +19 ;	//�����ܳ���

		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

		//������������
		pTask->cmd.pParm[0] = fileType;
		memcpy(&pTask->cmd.pParm[1],&fileNo,4); //�ļ���

		int fileStartPos = 0;
		memcpy(&pTask->cmd.pParm[1+4],&fileStartPos,4); //�ļ���
		memcpy(&pTask->cmd.pParm[1+4+4],&fileLen,4); //�ļ�����
		memcpy(&pTask->cmd.pParm[1+4+4+4],&fileName,strlen(fileName)); //�ļ�����
		memcpy(&pTask->cmd.pParm[1+4+4+4+100],&fileTime,min(strlen(fileTime),19)); //�ļ�ʱ��

		if(!m_pMsgQue->SendMsg(pTask))
		{
			delete pTask;
			pTask = NULL;
			return -1;
		}

		return m_iMsgId;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetTaskResult()'
void Timer::GetTaskResult( void )
{
	// {{{USR
		if( m_pTimerResultMsgQue == NULL)
		{
			return;
		}

		while(1)
		{
			Task* pTask = m_pTimerResultMsgQue->ReceiveMsg();
			if(pTask != NULL)
			{
				//��ѯĿ¼����
				if(pTask->cmd.funCode == CMD_GETFILELIST)
				{
					unsigned char dirType = pTask->cmd.pParm[0];//Ŀ¼����

					int maxFileNo = getMaxFileNo(pTask->cmd.destDevId,dirType);

					vector<PCMDRESULT>::iterator it = pTask->cmd.resultList.begin();
					for(;it != pTask->cmd.resultList.end();it++)
					{
						PCMDRESULT pCmdResult = (*it);
						if(pCmdResult->resultType == CDRT_DIRFILEINF)
						{
							PCMDDIRFILEINF pCmdDirFileInf = (PCMDDIRFILEINF)pCmdResult->pData;

							unsigned char fileType = 0;

							switch(dirType)
							{
								//case 90://����¼��Ŀ¼
								//    fileType = FAULTREC_FILETYPE;
								//    break;
							case 91://�����ռ�Ŀ¼
								fileType = RUNLOG_FILETYPE;
								break;
							case 92://����¼�Ŀ¼
								fileType = EVENT_FILETYPE;
								break;
							case 93://����¼��Ŀ¼
								fileType = DSM_FILETYPE;
								break;
							case 95://��̬��¼Ŀ¼
								fileType = DCR_FILETYPE;
								break;
							case 96://��������Ŀ¼
								fileType = TRH_FILETYPE;
								break;
							default:
								fileType = 0; //unkow type
								break;
							}

							//��ǰ���ص��ļ���С�����ݿ��е�����ļ��ţ�����Ϊ���ļ�
							//�������ļ�������������δ֪���ļ�����Ҳ������
							if(fileType == 0 || (maxFileNo >0 && pCmdDirFileInf->fileNo <= maxFileNo) )
							{
								delete pCmdResult->pData;
								pCmdResult->pData = NULL;

								delete pCmdResult;
								pCmdResult = NULL;
								continue;
							}

							//����ѯ�������ļ���Ϣ���浽���ݿ���
							CallFileInf callFileInf;
							memset(&callFileInf,0,sizeof(callFileInf));
							callFileInf.DAUID = pTask->cmd.destDevId;
							callFileInf.FileLen = pCmdDirFileInf->fileLen;
							memcpy(callFileInf.fileName,pCmdDirFileInf->fileName,sizeof(callFileInf.fileName));
							callFileInf.fileNo = pCmdDirFileInf->fileNo;
							memcpy(callFileInf.FileTime,pCmdDirFileInf->fileTime,sizeof(callFileInf.FileTime));
							callFileInf.fileType = fileType;
							callFileInf.IsCallData = false;

							SYSTEMTIME systemTime;
							GetSystemTime(&systemTime);
							sprintf(callFileInf.time,"%d-%d-%d %d:%d:%d",systemTime.wYear,systemTime.wMonth,
								systemTime.wDay,systemTime.wHour,systemTime.wMinute,systemTime.wSecond);

							addCallFileInf(&callFileInf);

							//�ͷ��ڴ�
							delete pCmdResult->pData;
							pCmdResult->pData = NULL;

							delete pCmdResult;
							pCmdResult = NULL;

							//�����ݿ���һ�û���ٻ����ļ�
							CallFileInf *pFileInf = NULL;
							getNoCallFileByType(pTask->cmd.destDevId,fileType,&pFileInf);

							while(pFileInf != NULL)
							{
								//��������ٻ��ļ�����
								PullFile(pFileInf->DAUID,pFileInf->fileType,pFileInf->fileNo,pFileInf->FileLen,
									pFileInf->fileName,pFileInf->FileTime);

								CallFileInf *pTmpCallFileInf = pFileInf;
								pFileInf = pTmpCallFileInf->next;
								releaseMemery((void*)pTmpCallFileInf);
								pTmpCallFileInf = NULL;
							}

						}
						else
						{
							delete pCmdResult->pData;
							pCmdResult->pData = NULL;

							delete pCmdResult;
							pCmdResult = NULL;

						} //end if(pCmdResult->resultType == CDRT_DIRFILEINF)
					}//end for(;it != pTask->cmd.resultList.end();it++)

					pTask->cmd.resultList.clear();
				} //end if(pTask->cmd.funCode == CMD_GETFILELIST)
				else if(pTask->cmd.funCode == CMD_GETWAVEFILELIST)//�ٻ�����¼���ļ��б�
				{
					vector<PCMDRESULT>::iterator it = pTask->cmd.resultList.begin();
					for(;it != pTask->cmd.resultList.end();it++)
					{
						PCMDRESULT pCmdResult = (*it);
						if(pCmdResult->resultType == CDRT_FILELIST && pCmdResult->resultLen == sizeof(CMDFILELIST))
						{
							CMDFILELIST fileList;
							memset(&fileList,0,sizeof(fileList));
							memcpy(&fileList,pCmdResult->pData,pCmdResult->resultLen);

							GetRecWaveFile(fileList.devID,fileList.fileName);
							::Sleep(100);
						}

						delete pCmdResult->pData;
						pCmdResult->pData = NULL;

						delete pCmdResult;
						pCmdResult = NULL;
					}
					pTask->cmd.resultList.clear();
					pTask->cmd.resultNum = 0;
				}//end if(pTask->cmd.funCode == CMD_GETWAVEFILELIST)
				else if(pTask->cmd.funCode == CMD_GETWAVEFILE)//�ٻ�����¼���ļ�
				{
					vector<PCMDRESULT>::iterator it = pTask->cmd.resultList.begin();
					for(;it != pTask->cmd.resultList.end();it++)
					{
						PCMDRESULT pCmdResult = (*it);
						delete pCmdResult->pData;
						pCmdResult->pData = NULL;

						delete pCmdResult;
						pCmdResult = NULL;
					}
					pTask->cmd.resultList.clear();
					pTask->cmd.resultNum = 0;
				}//end if(pTask->cmd.funCode == CMD_GETWAVEFILE)
				else if(pTask->cmd.funCode == CMD_PULLFILE) //�ٻ��ļ�����
				{
					if(pTask->cmd.parmLen != 1 +3*4 +100 +19 )
					{
						continue;
					}

					int fileNO = 0;       //�ļ���
					char fileName[100]={0};     //�ļ�����
					memcpy(&fileNO,&pTask->cmd.pParm[1],4);
					memcpy(fileName,&pTask->cmd.pParm[1+3*4],100);

					vector<PCMDRESULT>::iterator it = pTask->cmd.resultList.begin();
					if(it != pTask->cmd.resultList.end())
					{
						PCMDRESULT pCmdResult = (*it);
						if(pCmdResult->resultType == CDRT_TRUFALSE)
						{
							if(pCmdResult->pData[0] == 1) //ִ�гɹ�
							{
								updateCallFileInfById(pTask->cmd.destDevId,fileName,true);
							}
						}//end if(pCmdResult->resultType == CDRT_TRUFALSE)

						delete pCmdResult->pData;
						pCmdResult->pData = NULL;

						delete pCmdResult;
						pCmdResult = NULL;

						pTask->cmd.resultList.clear();

					}//end if(it != pTask->cmd.resultList.end())
					}//end else if(pTask->cmd.funCode == CMD_PULLFILE) //�ٻ��ļ�����

					delete pTask;
					pTask = NULL;
				}//end if(pTask != NULL)

				::Sleep(1000);
			}//end while(1)

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

int Timer::GetRecWaveFileList( unsigned char devId)
{
	struct CP56Time2a startTime,endTime;
	memset(&startTime,0,sizeof(startTime));
	memset(&endTime,0,sizeof(endTime));

	startTime.year = 01;
	startTime.month = 01;

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime);
	endTime.year = sysTime.wYear%100;
	endTime.month = sysTime.wMonth;
	endTime.day = sysTime.wDay;
	endTime.hours = sysTime.wHour;
	endTime.minute = 59;

	Task* pTask = new Task();
	pTask->Linker = NULL;

	m_iMsgId++;
	pTask->cmd.msgId = m_iMsgId;

	pTask->cmd.destDevId = devId;	//�豸���
	pTask->cmd.from = CD_TIMER;	//������Դ
	pTask->cmd.to = CD_ASDU;	//����Ŀ�ĵ�
	pTask->cmd.funCode = CMD_GETWAVEFILELIST;	//������
	pTask->cmd.parmLen = 1+sizeof(startTime) +sizeof(endTime);//�����ܳ���

	//������������
	pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
	memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

	memcpy(&pTask->cmd.pParm[1],&startTime,sizeof(startTime));
	memcpy(&pTask->cmd.pParm[1+sizeof(startTime)],&endTime,sizeof(endTime));

	if(!m_pMsgQue->SendMsg(pTask))
	{
		delete pTask;
		pTask = NULL;
		return -1;
	}

	return m_iMsgId;
}

int Timer::GetRecWaveFile(const unsigned char devId,const char* fileName,int startPos)
{
	if(fileName == NULL)
		return -1;

	Task* pTask = new Task();
	pTask->Linker = NULL;

	m_iMsgId++;
	pTask->cmd.msgId = m_iMsgId;

	pTask->cmd.destDevId = devId;//�豸���
	pTask->cmd.from = CD_TIMER;//������Դ
	pTask->cmd.to = CD_ASDU;//����Ŀ�ĵ�
	pTask->cmd.funCode = CMD_GETWAVEFILE;//������
	pTask->cmd.parmLen = 45;//�����ܳ���

	size_t iMinLen = min(::strlen(fileName),40);

	pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
	memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

	//������������
	pTask->cmd.pParm[0] = 1;
    memcpy(&pTask->cmd.pParm[1],fileName,iMinLen);
	memcpy(&pTask->cmd.pParm[41],&startPos,4);

	if(!m_pMsgQue->SendMsg(pTask))
	{
		delete pTask;
		pTask = NULL;
		return -1;
	}

	return m_iMsgId;
}

DWORD WINAPI GetTimerTaskResult(void *pParm)
{
	Timer *pTimer = (Timer*)pParm;
	pTimer->GetTaskResult();

	return 0;
}
// }}}USR
// }}}RME

// }}}RME
