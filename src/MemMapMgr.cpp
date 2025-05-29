// {{{RME classifier 'Logical View::MemMapMgr'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "MemMapMgr.h"
#endif

#include <RTSystem/Communication.h>
#include <MemMapMgr.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：MemMapMgr.cpp
* 文件标识：
* 摘    要：共享内存处理类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "Command.h"
#include "SocketParm.h"
#include "SocketLinker.h"
#include "ModemLinker.h"
#include "ComLinker.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

const char* COMM_CMD_FILEMAP_EVENT = "comm_cmd_filemap_event";
// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
MemMapMgr::MemMapMgr( void )
	: m_hCmdMapFile( NULL )
	, m_pCmdMapData( NULL )
	, m_hResMapFile( NULL )
	, m_pResMapData( NULL )
{
}
// }}}RME

// {{{RME operation '~MemMapMgr()'
MemMapMgr::~MemMapMgr( void )
{
	// {{{USR
	    CloseCmdMapFile();
	    CloseResMapFile();

	// }}}USR
}
// }}}RME

// {{{RME operation 'OpenCmdMapFile()'
void MemMapMgr::OpenCmdMapFile( void )
{
	// {{{USR
	    if(m_hCmdMapFile != NULL)
	    {
	        CloseCmdMapFile();
	    }

	    //打开内存文件
	    m_hCmdMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,COMM_COMMAND_FILEMAP);

	    //映射视图
	    if(m_hCmdMapFile != NULL)
	    {
	        m_pCmdMapData = (unsigned char*)MapViewOfFile(m_hCmdMapFile,FILE_MAP_ALL_ACCESS,0,0,0);
	    }
	    
	// }}}USR
}
// }}}RME

// {{{RME operation 'CloseCmdMapFile()'
void MemMapMgr::CloseCmdMapFile( void )
{
	// {{{USR
	    //释放资源
	    if(m_pCmdMapData != NULL)
	    {
	        ::UnmapViewOfFile((LPCVOID)m_pCmdMapData);
	        m_pCmdMapData = NULL;
	    }
	    if(m_hCmdMapFile != NULL)
	    {
	        ::CloseHandle(m_hCmdMapFile);
	        m_hCmdMapFile = NULL;
	    }
	    
	// }}}USR
}
// }}}RME

// {{{RME operation 'OpenResMapFile()'
void MemMapMgr::OpenResMapFile( void )
{
	// {{{USR
	    if(m_hResMapFile != NULL)
	    {
	        CloseResMapFile();
	    }

	    //打开内存文件
	    m_hResMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,COMM_RESULT_FILEMAP);

	    //映射视图
	    if(m_hResMapFile != NULL)
	    {
	        m_pResMapData = (unsigned char*)MapViewOfFile(m_hResMapFile,FILE_MAP_ALL_ACCESS,0,0,0);
	    }
	    
	// }}}USR
}
// }}}RME

// {{{RME operation 'CloseResMapFile()'
void MemMapMgr::CloseResMapFile( void )
{
	// {{{USR
	    //释放资源
	    if(m_pResMapData != NULL)
	    {
	        ::UnmapViewOfFile((LPCVOID)m_pResMapData);
	        m_pResMapData = NULL;
	    }
	    if(m_hResMapFile != NULL)
	    {
	        ::CloseHandle(m_hResMapFile);
	        m_hResMapFile = NULL;
	    }
	    
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetTaskFromMem()'
Task * MemMapMgr::GetTaskFromMem( void )
{
	// {{{USR
		const int COMMAND_HEADLEN = 18;	//命令结构头长度(除去pParm的长度)
	    Task *pTask = NULL;

		static DWORD dwTime = 0;
		DWORD dwPreTime = 0;
		dwPreTime = dwTime;
		dwTime = GetTickCount();

		//打开内存文件
		if(m_hCmdMapFile == NULL)
		{
			OpenCmdMapFile();
		}

		if( m_hCmdMapFile == NULL)
	    {
	        return NULL;
	    }

		HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,COMM_CMD_FILEMAP_EVENT);
		if(hEvent != NULL)
		{
			if(WAIT_OBJECT_0 != WaitForSingleObject(hEvent,2000))
			{
				::CloseHandle(hEvent);
				hEvent = NULL;
				return NULL;
			}
		}

		//映射视图
		if( m_pCmdMapData == NULL)
	    {
	        CloseCmdMapFile();
			SetEvent(hEvent);
			::CloseHandle(hEvent);
			hEvent = NULL;
	        return NULL;
	    }

	    unsigned char *pData = m_pCmdMapData; 

		unsigned char cmdNum = *pData;  //取得命令个数
		pData++;

		unsigned int uiTotalLen = 0;	//总长度
		memcpy(&uiTotalLen,pData,4);
		pData += 4;

		unsigned char *pDataHead = pData;

		if(cmdNum > 0)  //有命令
	    {
			pTask = new Task();
			pTask->Linker = NULL;

			//取得第一个命令
			pTask->cmd.processType = *pData;
			pData++;
			memcpy(&pTask->cmd.processId,pData,4);
			pData += 4;
			memcpy(&pTask->cmd.msgId,pData,4);
			pData += 4;

			pTask->cmd.from = CD_UI;
			pData += 1;
			pTask->cmd.to = (CMDDIR)(*pData);
			pData += 1;
			memcpy(&pTask->cmd.destDevId,pData,4);
			pData += 4;
			pTask->cmd.funCode = *pData;
			pData += 1;

			//char szInfo[256] = {0};
			//sprintf(szInfo,"Find %d commands,and now get the command with funCode:%x",cmdNum,pTask->cmd.funCode);
			//mylog.WriteLog2File(LogControl::LDEBUG,szInfo);

			memcpy(&(pTask->cmd.parmLen),pData,2);
			pData += 2;

			if(pTask->cmd.parmLen > 0)
			{
				pTask->cmd.pParm = new char[pTask->cmd.parmLen +1];
				memset(pTask->cmd.pParm,0,pTask->cmd.parmLen +1);
				memcpy(pTask->cmd.pParm,pData,pTask->cmd.parmLen);

				pData += pTask->cmd.parmLen;
			}

			//删除消费掉的命令
			cmdNum -= 1;
			*m_pCmdMapData = cmdNum;

			memmove(pDataHead,pData,(uiTotalLen - (pData-m_pCmdMapData) ) );

			uiTotalLen -= (COMMAND_HEADLEN + pTask->cmd.parmLen);
			memcpy(m_pCmdMapData+1,&uiTotalLen,4);
		} //end if(cmdNum > 0)

		/*if(pTask != NULL)
		{
			printf("find %d commands in share memery,get command fun-code:%X,use time:%d\r\n",(int)cmdNum+1,(int)pTask->cmd.funCode,dwTime-dwPreTime);
		}
		else
		{
			printf("No find any comand,use time:%d\r\n",dwTime-dwPreTime);
		}*/

		if(hEvent != NULL)
		{
			::SetEvent(hEvent);
			::CloseHandle(hEvent);
			hEvent = NULL;
		}

		return pTask;
	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteTaskToMem(Task*)'
bool MemMapMgr::WriteTaskToMem( Task * task )
{
	// {{{USR
	    return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteTaskResultToMem(const struct Command&)'
bool MemMapMgr::WriteTaskResultToMem( const struct Command & cmd )
{
	// {{{USR
		//命令不是来自UI也不是来自DAU，则不用写结果入共享内存
	    if(cmd.from != CD_UI && cmd.from != CD_ASDU && cmd.from != CD_CONST_DAU)
	    {
			printf("MemMapMgr return 111!\n");
	        return true;
	    }

	        //打开内存文件
		if(m_hResMapFile == NULL)
		{
			OpenResMapFile();
		}

	    if( m_hResMapFile == NULL)
	    {
			printf("MemMapMgr return 222!\n");
	        return NULL;
	    }

		HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,CMDRESULT_FILEMAP_EVENT);

		if(hEvent != NULL)
		{
			if(WAIT_OBJECT_0 != WaitForSingleObject(hEvent,10000))
			{
				::SetEvent(hEvent);
				::CloseHandle(hEvent);
				hEvent = NULL;
				printf("MemMapMgr return 333!\n");
				return NULL;
			}
		}

	        //映射视图
		if( m_pResMapData == NULL)
	    {
	        CloseResMapFile();
			::SetEvent(hEvent);
			::CloseHandle(hEvent);
			hEvent = NULL;
			printf("MemMapMgr return 4444!\n");
	        return NULL;
	    }

	    unsigned char *pData = m_pResMapData;

	    unsigned char *pTmp = pData;

		unsigned int uiMaxMemSize = 0;

		unsigned int nImportDataOffSet = 12;	//重要数据结果的偏移
		memcpy(pData,&nImportDataOffSet,sizeof(unsigned int));
		pData += sizeof(unsigned int);

		unsigned int uiMainUIOffSet = 2048;
		memcpy(pData,&uiMainUIOffSet,sizeof(unsigned int));	//主界面结果偏移量
		pData += sizeof(unsigned int);

		unsigned int uiOtherOffSet = 5.5*1024*1024;
		memcpy(pData,&uiOtherOffSet,sizeof(unsigned int));	//其它进程结果偏移量
		pData += sizeof(unsigned int);

		if(cmd.processType == 0 || cmd.msgId <= 0)
		{
			//查询通讯状态的结果用独立的区域，避免通讯模块和界面之间发送的数据量过大而使状态信息不能发送
			if( cmd.funCode == CMD_QUERY_LINKSTATE )
			{
				pData = pTmp + nImportDataOffSet;
				uiMaxMemSize = uiMainUIOffSet -1;
			}
			else
			{
				pData = pTmp + uiMainUIOffSet;
				uiMaxMemSize = uiOtherOffSet -1;
			}
		}
		else
		{
			pData = pTmp + uiOtherOffSet;
			uiMaxMemSize = COMM_RESULT_FILEMAP_SIZE;
		}
	    
	    unsigned char cmdReturnNum = *pData;  //取得命令返回个数
	    pData++;

	    unsigned int resultTotalLen =0;    //命令返回结果总长度
	    memcpy(&resultTotalLen,pData,sizeof(unsigned int));
	    pData += sizeof(unsigned int);

	    pData += resultTotalLen;            //跳转到末尾

	    if(cmd.resultNum > 0)
	    {
	        cmdReturnNum += 1;              //命令返回个数加1

			memcpy(pData,&cmd.processId,4);      //拷贝进程id
	        pData += 4;
	        resultTotalLen += 4;

	        memcpy(pData,&cmd.msgId,4);      //拷贝消息号
	        pData += 4;
	        resultTotalLen += 4;

	        *pData = cmd.from;              //命令来源
	        pData += 1;
	        resultTotalLen += 1;

	        *pData = cmd.funCode;         //功能代码
	        pData += 1;
	        resultTotalLen += 1;

			memcpy(pData,&cmd.destDevId,4);	//设备编号
	        pData += 4;
	        resultTotalLen += 4;

			memcpy(pData,&cmd.resultNum,sizeof(unsigned short));//结果个数
	        pData += sizeof(unsigned short);
	        resultTotalLen += sizeof(unsigned short);

			vector< PCMDRESULT >::const_iterator it = cmd.resultList.begin();
	        for(;it != cmd.resultList.end(); it++)
	        {
	            *pData = (*it)->resultType; //结果数据类型
	            pData += 1;

				memcpy(pData,&((*it)->resultLen),sizeof(unsigned int));   //结果长度
				pData += sizeof(unsigned int);

				//拷贝结果
				if((pData - m_pResMapData + (*it)->resultLen + 24) > uiMaxMemSize)
				{
					mylog.WriteLog2File(LogControl::LERROR, "The result share memery is full,cann't write result any more");
					if(hEvent != NULL)
					{
						::SetEvent(hEvent);
						::CloseHandle(hEvent);
						hEvent = NULL;
					}
					printf("MemMapMgr return 5555!\n");
					return false;
				}

				memcpy(pData,(*it)->pData,(*it)->resultLen);
				pData += (*it)->resultLen;

				//计算结果总长度
				resultTotalLen += 1 + sizeof(unsigned int) + (*it)->resultLen;
	        }

			if(cmd.processType == 0 || cmd.msgId <= 0)
			{
				if( cmd.funCode == CMD_QUERY_LINKSTATE )
				{
					pData = pTmp + nImportDataOffSet;
				}
				else
				{
					pData = pTmp + uiMainUIOffSet;
				}
			}
			else
			{
				pData = pTmp + uiOtherOffSet;
			}

	        *pData = cmdReturnNum;   //写回命令返回个数
	        pData++;

	        memcpy(pData,&resultTotalLen,sizeof(unsigned int)); //写回结果总长度
		}

#ifdef _DEBUG
		
		if(m_pResMapData)
		PrintShareMemoryInfo(m_pResMapData);
#endif
	
		if(hEvent != NULL)
		{
			::SetEvent(hEvent);
			::CloseHandle(hEvent);
			hEvent = NULL;
		}
	
		return true;
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
void MemMapMgr::PrintShareMemoryInfo(unsigned char *  pMem)
{
	PCMDRESULT pResult = new CMDRESULT();
	if(NULL==pResult)
	{
			return ;
	}
	unsigned  int szRstLen = 256;
	memset(pResult,0,sizeof(CMDRESULT));
	pResult->pData= NULL;
	pResult->pData = new char[szRstLen] ;
	if(NULL==pResult->pData)
	{
		delete pResult;
		return ;
	}
	memset(pResult->pData,0,szRstLen);
	unsigned char *pData =pMem; 
	unsigned char *pTmp = pData;

	unsigned int uiMainUIOffSet = 8;
	unsigned int uiOtherOffSet = 5*1024*1024;
	memcpy(pData,&uiMainUIOffSet,sizeof(unsigned int));
	pData += sizeof(unsigned int);
	memcpy(pData,&uiOtherOffSet,sizeof(unsigned int));
	pData += sizeof(unsigned int);

	//if(m_processType == PT_MAIN_UI )
	{
		pData = pTmp + uiMainUIOffSet;
	}
	//else
	{
		//	pData = pTmp + uiOtherOffSet;
	}

	unsigned char cmdReturnNum = *pData;  //取得命令返回个数
	pData++;

	unsigned int resultTotalLen =0;    //命令返回结果总长度
	memcpy(&resultTotalLen,pData,sizeof(unsigned int));
	pData += sizeof(unsigned int);

	if(cmdReturnNum == 0 || resultTotalLen == 0)
	{
		if(pResult)
		{
			if(pResult->pData)
			{
				delete []pResult->pData;
				pResult->pData = NULL;
			}
			delete pResult;
			pResult = NULL;
		}
		return ;
	}

	for(unsigned char m =0; m < cmdReturnNum; m++)
	{
		unsigned int processId = 0;	//进程id
		memcpy(&processId,pData,4);
		pData += 4;

		int msgId = 0 ;     //消息号
		memcpy(&msgId,pData,4);
		pData += 4;

		//   CommTaskMgr::CMDDIR cmdFrom = (CommTaskMgr::CMDDIR)(*pData);  //命令来源
		pData++;

		unsigned char funCode = *pData;     //功能代码
		pData++;

		unsigned int devId = 0;     //设备板编号
		memcpy(&devId,pData,4);
		pData += 4;

		//结果个数
		unsigned short resultNum = 0;
		memcpy(&resultNum,pData,sizeof(unsigned short) );
		pData += sizeof(unsigned short);

		char szInfo[256]={0};
		for(unsigned short i=0; i<resultNum; i++)
		{

			pResult->resultType = CMDRESULTTYPE(*pData);
			pData++;

			memcpy(&(pResult->resultLen),pData,4);
			pData += sizeof(unsigned int);

			if(funCode==CMD_GETWAVEFILE )
			{
				if(pResult->resultLen>szRstLen)
				{
					delete []pResult->pData;
					pResult->pData = NULL;
					pResult->pData = new char[pResult->resultLen+1];
					memset(pResult->pData,0,pResult->resultLen+1);
					szRstLen = pResult->resultLen;
				}
				memcpy(pResult->pData,pData,pResult->resultLen);
			}
			pData += pResult->resultLen;


			if(funCode==CMD_GETWAVEFILE )
			{

				if(pResult->resultType == CDRT_TRUFALSE)
				{
					if(pResult->pData[0])
					{
						sprintf(szInfo,"MemMapMgr Get WaveFile succ !!!\n");
					}
					else
					{
						sprintf(szInfo,"MemMapMgr Get WaveFile fail !!!\n");
					}

				}
				else if(pResult->resultType  == CDRT_FILEPATH)
				{
					sprintf(szInfo,"MemMapMgr Get file  %s  succ !!!\n", pResult->pData);
				}
				else if(pResult->resultType == CDRT_DESC)
				{
					sprintf(szInfo,"MemMapMgr Desc:: %s  !!!\n", pResult->pData);
				}
				else if(pResult->resultType == CDRT_PROGRESS)
				{
					//sprintf(szInfo,"MemMapMgr Progress:: %d ...\n", pResult->pData[0]);
				}
				if(strlen(szInfo)>0)
				{
					//printf(szInfo);
					//OutputDebugString(szInfo);
				}
			}	
		}
		
	}

	if(pResult)
	{
		if(pResult->pData)
		{
			delete []pResult->pData;
			pResult->pData = NULL;
		}
		delete pResult;
		pResult = NULL;
	}
	
}