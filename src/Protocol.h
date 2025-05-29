// {{{RME classifier 'Logical View::Protocol'

#ifndef Protocol_H
#define Protocol_H

#ifdef PRAGMA
#pragma interface "Protocol.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol.h
* 文件标识：
* 摘    要：所有协议的父类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

struct Task;
class Linker;

#include "public.h"
#include "Command.h"
//#include "LogControl.h"

//协议处理错误代码
typedef enum proErrorCode{PEC_UNKOWN,PEC_NOERROR,PEC_ERRCMD_NODATA,PEC_INTERRUPT,PEC_ERR_ANSWER,
PEC_ERR_FRAME,PEC_STARTDT_NOACT,PEC_RMT_STOP_TRANSFER,PEC_TIME_OUT,PEC_OPERAT_FILE_FAIL} PROERRORCODE;

//定义ASDU信息序号
#define ASDU_INF_R_ALLGROUP_TITLE		240	//读所有被定义组的标题
#define ASDU_INF_R_ALLENTRY_VAL			241	//读一个组的全部条目的值或属性
#define ASDU_INF_R_ONEENTRY_DIR			243	//读单个条目的目录
#define ASDU_INF_R_ONEENTRY_VAL			244	//读单个条目的值或属性
#define ASDU_INF_GEN_DATA				245	//通用分类数据


//定义ASDU描述的类别
#define ASDU_KOD_ACT_VAL				1	//实际值
#define ASDU_KOD_DIMENSION				9	//量纲
#define ASDU_KOD_DESCRIPTION			10	//描述

//定义ASDU描述的类别(国标104扩展)
#define ASDU_KOD_RATING_104GB			101	//额定值
#define ASDU_KOD_OBJTYPE_104GB			101	//检索对象类别

//定义通用分类数据描述
typedef struct _GDD
{
    unsigned char dataType; //数据类型
    unsigned char dataLen;  //数据长度
    unsigned char dataNum;  //数据数目
}GDD,*PGDD;

typedef struct _GI
{
public:
	union
    {
        struct
        {
            unsigned char groupNo;//组号
			unsigned char entryNo;//条目号
        }g;

        unsigned short GIN;//通用分类标识序号
    };

    unsigned char KOD;      //描述类别
    GDD           gdd;      //通用分类数据描述
    unsigned short dataLen; //数据长度
    unsigned char *data;    //数据

    _GI(unsigned short gin,unsigned char kod,unsigned char gddDataType,unsigned char gddDataLen,unsigned char gddDataNum,
        unsigned short giDataLen,unsigned char *pGiData)
    {
        this->GIN = gin;
        this->KOD = kod;
        this->gdd.dataType = gddDataType;
        this->gdd.dataLen = gddDataLen;
        this->gdd.dataNum = gddDataNum;
        this->dataLen = giDataLen;
        this->data = pGiData;
    }

	_GI(unsigned char groupNo,unsigned char entryNo,unsigned char kod,unsigned char gddDataType,unsigned char gddDataLen,unsigned char gddDataNum,
        unsigned short giDataLen,unsigned char *pGiData)
    {
		this->g.groupNo = groupNo;
		this->g.entryNo = entryNo;
        this->KOD = kod;
        this->gdd.dataType = gddDataType;
        this->gdd.dataLen = gddDataLen;
        this->gdd.dataNum = gddDataNum;
        this->dataLen = giDataLen;
        this->data = pGiData;
    }
}GI,*PGI;

//描述元素的数目
typedef struct _NDE
{
	unsigned char NO:6;		//数目
	unsigned char COUNT:1;	//计数器位
	unsigned char CONT:1;	//后续位

	_NDE()
	{
		NO = 0;
		COUNT = 0;
		CONT = 0;
	}
}NDE,*PNDE;

//通用分类数据集数目
typedef struct _NGD
{
	union
    {
		struct
		{
			unsigned char NO:6;		//数目
			unsigned char COUNT:1;	//计数器位
			unsigned char CONT:1;	//后续位
		}s;
		unsigned char ngd;
	};

	_NGD()
	{
		s.NO = 0;
		s.COUNT = 0;
		s.CONT = 0;
	}
}NGD,*PNGD;


// }}}USR
// }}}RME

class Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	/*********命令码与命令的对应关系-->***********************/
	typedef struct tagFunCode2Cmd
	{
		unsigned char cmdFunCode;	//本系统内部使用的命令代码
		bool (*exuteCmd)(Protocol * ,Linker * ,struct Command& );  //用于执行命令的函数
		const char*	cmdName; //消息名称
	}FUNCODE2CMD,*PFUNCODE2CMD;

	//获取命令描述信息
	virtual void GetCmdDesc(struct Command & cmd)
	{
	}

	virtual int DealAutoSend(Linker *pLinker)
	{
		return 0;
	}

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    //LogControl log;
	    PROERRORCODE    m_errCode;

		void CP56Time2aTotm(struct CP56Time2a& srcTime,struct tm& tmDestTime);

		void tmToCP56Time2a(struct tm& tmSrcTime,struct CP56Time2a& destTime);
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol( void );
	// }}}RME
	// {{{RME operation 'Protocol()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol( void );
	// }}}RME

	virtual void Init() {}

	// {{{RME operation 'Decode(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Decode
	    Description:    解析通过通讯传入的数据，生成一个任务对象
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual Task * Decode( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExecuteCmd
	    Description:    执行命令
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    cmd：要执行的命令
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	// }}}RME
	// {{{RME operation 'InitConnect(Linker *)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       InitConnect
	    Description:    初始化连接
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool InitConnect( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'GetUdpData(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool GetUdpData( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'SendData(Linker *,const char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       SendData
	      Description:    发送数据
	      Calls:          
	      Called By:      
	      Input:          pLinker：链路对象
	                          pDataBuf：指向要发送的数据
	                      iDataLen：要发送数据的长度
	                          uiTimeOut：超时时间（单位：毫秒），为0则永不超时
	      Output:         
	      Return:         实际发送数据的长度
	    *************************************************	
	   }}}USR */
	// }}}RME
	int SendData( Linker * pLinker, const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'ReceiveData(Linker *,char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ReceiveData
	      Description:    接收数据
	      Calls:          
	      Called By:      
	      Input:          pLinker：链路对象
	                          pDataBuf：保存接收到的数据
	                      iBufferLen：期望接收数据的长度
	                      uiTimeOut：超时时间（单位：毫秒），为0则永不超时
	      Output:         
	      Return:         实际接收到的数据长度
	    *************************************************
	   }}}USR */
	// }}}RME
	int ReceiveData( Linker * pLinker, char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
	// }}}RME

public:
	// {{{RME operation 'GetLastErrCode()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrCode
	      Description:    获得最近发生的一次错误代码
	      Calls:          
	      Called By:      
	      Input:          
	                      	                      
	      Output:         
	      Return:         错误代码
	    *************************************************
	   }}}USR */
	// }}}RME
	PROERRORCODE GetLastErrCode( void );
	// }}}RME
	// {{{RME operation 'DoIdle(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    处理空闲
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         处理空闲的任务
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual Task * DoIdle( Linker * pLinker );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol_H */

// }}}RME
