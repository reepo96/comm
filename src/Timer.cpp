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
* 文件名称：Timer.cpp
* 文件标识：
* 摘    要：定时器
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include <time.h>

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

const unsigned char FAULTREC_FILETYPE = 4;	//故障录波文件类型
const unsigned char RUNLOG_FILETYPE = 102;	//运行日记文件类型
const unsigned char EVENT_FILETYPE = 101;	//标记事件文件类型
const unsigned char DSM_FILETYPE = 1;	//低速录波文件类型
const unsigned char DCR_FILETYPE = 2;	//动态记录文件类型
const unsigned char TRH_FILETYPE = 3;	//趋势数据文件类型

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

			//到达开始时间或达到间隔时间
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
		if(pTimerCall->isCallAll) //自动召唤所有数据
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

		if(pTimerCall->isCallEventData)//自动召唤标志事件数据
		{
			GetFileList(pTimerCall->DAUID,92);
		}
		if(pTimerCall->isCallFaultData)//自动召唤故障数据
		{
			//GetFileList(pTimerCall->DAUID,90);
			GetRecWaveFileList(pTimerCall->DAUID);
		}
		if(pTimerCall->isCallLogData)//自动召唤日志数据
		{
			GetFileList(pTimerCall->DAUID,91);
		}
		if(pTimerCall->isCallLowRecData)//自动召唤低速录波数据
		{
			GetFileList(pTimerCall->DAUID,93);
		}
		if(pTimerCall->isCallQualityData)//自动召唤电能质量数据
		{
			//GetFileList(pTimerCall->DAUID,94);
		}
		if(pTimerCall->isCallDynaData)//自动召唤动态记录数据
		{
			GetFileList(pTimerCall->DAUID,95);
		}
		if(pTimerCall->isCallConstData)//自动召唤趋势数据
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

		pTask->cmd.destDevId = devId;	//设备编号
		pTask->cmd.from = CD_TIMER;	//命令来源
		pTask->cmd.to = CD_ASDU;	//命令目的地
		pTask->cmd.funCode = CMD_GETFILELIST;	//命令码
		pTask->cmd.parmLen = 1 +sizeof(int);	//参数总长度

		//拷贝参数数据
		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

		pTask->cmd.pParm[0] = dirType;

		//最近的文件数
		int fileNum = 0;    //所有文件
		memcpy(&pTask->cmd.pParm[1],&fileNum,sizeof(int));   //参数数据

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

		pTask->cmd.destDevId = devId;	//设备编号
		pTask->cmd.from = CD_TIMER;	//命令来源
		pTask->cmd.to = CD_ASDU;	//命令目的地
		pTask->cmd.funCode = CMD_PULLFILE;	//命令码
		pTask->cmd.parmLen = 1 +3*4 +100 +19 ;	//参数总长度

		pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
		memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

		//拷贝参数数据
		pTask->cmd.pParm[0] = fileType;
		memcpy(&pTask->cmd.pParm[1],&fileNo,4); //文件号

		int fileStartPos = 0;
		memcpy(&pTask->cmd.pParm[1+4],&fileStartPos,4); //文件号
		memcpy(&pTask->cmd.pParm[1+4+4],&fileLen,4); //文件长度
		memcpy(&pTask->cmd.pParm[1+4+4+4],&fileName,strlen(fileName)); //文件名称
		memcpy(&pTask->cmd.pParm[1+4+4+4+100],&fileTime,min(strlen(fileTime),19)); //文件时间

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
				//查询目录命令
				if(pTask->cmd.funCode == CMD_GETFILELIST)
				{
					unsigned char dirType = pTask->cmd.pParm[0];//目录类型

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
								//case 90://故障录波目录
								//    fileType = FAULTREC_FILETYPE;
								//    break;
							case 91://运行日记目录
								fileType = RUNLOG_FILETYPE;
								break;
							case 92://标记事件目录
								fileType = EVENT_FILETYPE;
								break;
							case 93://低速录波目录
								fileType = DSM_FILETYPE;
								break;
							case 95://动态记录目录
								fileType = DCR_FILETYPE;
								break;
							case 96://趋势数据目录
								fileType = TRH_FILETYPE;
								break;
							default:
								fileType = 0; //unkow type
								break;
							}

							//当前返回的文件号小于数据库中的最大文件号，则认为该文件
							//不是新文件，不处理它。未知的文件类型也不处理
							if(fileType == 0 || (maxFileNo >0 && pCmdDirFileInf->fileNo <= maxFileNo) )
							{
								delete pCmdResult->pData;
								pCmdResult->pData = NULL;

								delete pCmdResult;
								pCmdResult = NULL;
								continue;
							}

							//将查询上来的文件信息保存到数据库中
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

							//释放内存
							delete pCmdResult->pData;
							pCmdResult->pData = NULL;

							delete pCmdResult;
							pCmdResult = NULL;

							//从数据库查找还没有召唤的文件
							CallFileInf *pFileInf = NULL;
							getNoCallFileByType(pTask->cmd.destDevId,fileType,&pFileInf);

							while(pFileInf != NULL)
							{
								//发送命令，召唤文件内容
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
				else if(pTask->cmd.funCode == CMD_GETWAVEFILELIST)//召唤故障录波文件列表
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
				else if(pTask->cmd.funCode == CMD_GETWAVEFILE)//召唤故障录波文件
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
				else if(pTask->cmd.funCode == CMD_PULLFILE) //召唤文件命令
				{
					if(pTask->cmd.parmLen != 1 +3*4 +100 +19 )
					{
						continue;
					}

					int fileNO = 0;       //文件号
					char fileName[100]={0};     //文件名称
					memcpy(&fileNO,&pTask->cmd.pParm[1],4);
					memcpy(fileName,&pTask->cmd.pParm[1+3*4],100);

					vector<PCMDRESULT>::iterator it = pTask->cmd.resultList.begin();
					if(it != pTask->cmd.resultList.end())
					{
						PCMDRESULT pCmdResult = (*it);
						if(pCmdResult->resultType == CDRT_TRUFALSE)
						{
							if(pCmdResult->pData[0] == 1) //执行成功
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
					}//end else if(pTask->cmd.funCode == CMD_PULLFILE) //召唤文件命令

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

	pTask->cmd.destDevId = devId;	//设备编号
	pTask->cmd.from = CD_TIMER;	//命令来源
	pTask->cmd.to = CD_ASDU;	//命令目的地
	pTask->cmd.funCode = CMD_GETWAVEFILELIST;	//命令码
	pTask->cmd.parmLen = 1+sizeof(startTime) +sizeof(endTime);//参数总长度

	//拷贝参数数据
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

	pTask->cmd.destDevId = devId;//设备编号
	pTask->cmd.from = CD_TIMER;//命令来源
	pTask->cmd.to = CD_ASDU;//命令目的地
	pTask->cmd.funCode = CMD_GETWAVEFILE;//命令码
	pTask->cmd.parmLen = 45;//参数总长度

	size_t iMinLen = min(::strlen(fileName),40);

	pTask->cmd.pParm = new char[pTask->cmd.parmLen+1];
	memset(pTask->cmd.pParm,0,pTask->cmd.parmLen+1);

	//拷贝参数数据
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
