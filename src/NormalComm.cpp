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
* �ļ����ƣ�SystemCfger.cpp
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�����࣬���ڱ���ϵͳ��������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
			//�������ʧ�ܣ���������û�з��ؽ���������ʧ�ܵķ��ؽ��
			if(pTask->cmd.funCode == 0X26)
			{
				return true;
			}
			if(pTask->cmd.resultNum <= 0&&pTask->cmd.funCode != 0X26)//104����Ӧ�� 0x26
			{
				pTask->cmd.resultNum = 1;
				PCMDRESULT pResult = new CMDRESULT();
				pResult->resultType = CDRT_TRUFALSE;
				pResult->resultLen = 1;
				pResult->pData = new char[1];
				pResult->pData[0] = 0 ; //false

				pTask->cmd.resultList.push_back(pResult);
			}

			//��Ӵ�������
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
			//���������û�з��ؽ��������ӳɹ��ķ��ؽ��
			if(pTask->cmd.resultNum <= 0&&pTask->cmd.funCode != 0X26) //104����Ӧ��0x26
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
