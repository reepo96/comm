// {{{RME classifier 'Logical View::NormalComm'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "NormalComm.h"
#endif

#include <RTSystem/Communication.h>
#include <NormalComm.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：SystemCfger.cpp
* 文件标识：
* 摘    要：系统配置类，用于保存系统的配置信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
NormalComm::NormalComm( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
NormalComm::~NormalComm( void )
{
}
// }}}RME

// {{{RME operation 'RunTask(Task*)'
bool NormalComm::RunTask( Task * pTask )
{
	// {{{USR
	    assert(pTask != NULL);
	    assert(pTask->Linker != NULL);

		if(!pTask->Linker->ExecuteCmd(pTask->cmd))
		{
			//如果命令失败，而命令中没有返回结果，则添加失败的返回结果
			if(pTask->cmd.funCode == 0X26)
			{
				return true;
			}
			if(pTask->cmd.resultNum <= 0&&pTask->cmd.funCode != 0X26)//104测试应答 0x26
			{
				pTask->cmd.resultNum = 1;
				PCMDRESULT pResult = new CMDRESULT();
				pResult->resultType = CDRT_TRUFALSE;
				pResult->resultLen = 1;
				pResult->pData = new char[1];
				pResult->pData[0] = 0 ; //false

				pTask->cmd.resultList.push_back(pResult);
			}

			//添加错误描述
			const char *errorDesc = pTask->Linker->GetLastErrDesc();

			pTask->cmd.resultNum += 1;
			PCMDRESULT pResult = new CMDRESULT();
			pResult->resultType = CDRT_DESC;
			pResult->resultLen = strlen(errorDesc);
			pResult->pData = new char[pResult->resultLen+1];
			strcpy(pResult->pData,errorDesc);
			pResult->pData[pResult->resultLen] = 0 ;

			pTask->cmd.resultList.push_back(pResult);

			mylog.FormatLogInf(LogControl::LINFO,"Execute task(0x%x) fail,because:%s",
				pTask->cmd.funCode,pResult->pData);

			return false;
		}
		else
		{
			//如果命令中没有返回结果，则添加成功的返回结果
			if(pTask->cmd.resultNum <= 0&&pTask->cmd.funCode != 0X26) //104测试应答0x26
			{
				pTask->cmd.resultNum = 1;
				PCMDRESULT pResult = new CMDRESULT();
				pResult->resultType = CDRT_TRUFALSE;
				pResult->resultLen = 1;
				pResult->pData = new char[1];
				pResult->pData[0] = 1 ; //true

				pTask->cmd.resultList.push_back(pResult);
			}
			return true;
		}
	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

// }}}USR
// }}}RME

// }}}RME
