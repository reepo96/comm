// {{{RME classifier 'Logical View::Protocol104_GB'

#ifndef Protocol104_GB_H
#define Protocol104_GB_H

#ifdef PRAGMA
#pragma interface "Protocol104_GB.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol104_GB.h
* 文件标识：
* 摘    要：国标104协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

#define		PRO104GB_FRAME_MAXSIZE	4090			//帧最大长度
#define		PRO104GB_ASDUDATA_MAXSIZE	4078		//帧ASDU DATA(信息元素集部分)最大长度

#define		PRO104GB_FILEPATH_LEN	100		//文件(目录)名长度

// }}}USR
// }}}RME

class Protocol104_GB : public Protocol104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	/*************************************************
	    Function:       StopDataTransfer
	    Description:    终止数据传输
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************/	   
	bool StopDataTransfer( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	    Function:       CpyGI2SendBuf
	    Description:    将GI结构拷贝到发送buffer中
	    Calls:          
	    Called By:      
	    Input:          ppDestBuf：目标buffer
	                    iBufLen：目标buffer长度
						gi:GI结构
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************/	   
	bool CpyGI2SendBuf( char **ppDestBuf, int iBufLen,GI& gi);

	/*************************************************
	    Function:       SendAsduFrame
	    Description:    发送ASDU帧
	    Calls:          
	    Called By:      
	    Input:          type：类型标识
	                    asduStructNo：序号
						cot：传送原因
						addr：地址
						pAsduData：ASDU数据
						iAsduDataLen：数据长度
	    Output:         
	                    
	    Return:         成功返回true，失败返回false
	    ************************************************/	  
	bool SendAsduFrame( Linker * pLinker,struct Command * pCmd,unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen );


	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	/*************************************************
	    Function:       SendIFrame
	    Description:    发送I格式帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
						pAsdu：指向ASDU部分
						iAsduLen：ASDU部分的长度
						pCmd：当前执行的命令
	                    
	    Output:         
	    Return:         -1：发生错误，0：停止发送，1:发送成功
	    *************************************************/
		int SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd);

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104_GB( void );
	// }}}RME
	// {{{RME operation 'Protocol104_GB()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104_GB( void );
	// }}}RME
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

protected:
	// {{{RME operation 'AnalyseTask(PASDUID104,PASDUINFID104,char*,int,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( PASDUID104 pAsduId, PASDUINFID104 pInfId, char * pAsduData, int iAsduDataLen, Linker * pLinker );
	// }}}RME

public:
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
	// {{{RME operation 'CommParamArrange(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       CommParamArrange
	    Description:    通讯参数协商
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool CommParamArrange( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GenCommand(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GenCommand
	    Description:    通用分类命令
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GenCommand( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'GetMarkEvn(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetMarkEvn
	    Description:    读历史事件
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetMarkEvn( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetSearchType(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetSearchType
	    Description:    查询数据检索类型
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetSearchType( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetAllDataObjDesc(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetAllDataObjDesc
	    Description:    查询所有数据检索对象描述
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetAllDataObjDesc( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetDataObjAttr(Linker *,struct Command*,unsigned char,unsigned char)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetDataObjAttr
	    Description:    查询一个数据检索对象的属性
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
							groupNo:组号
						entryNo:条目号
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetDataObjAttr( Linker * pLinker, struct Command * pCmd, unsigned char groupNo, unsigned char entryNo );
	// }}}RME

public:
	// {{{RME operation 'GetSerialRecorder(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GetSerialRecorder
	    Description:    调取连续记录波形
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetSerialRecorder( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'QuerySerialData(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       QuerySerialData
	    Description:    连续记录检索
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool QuerySerialData( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GeneralFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       GeneralFileList
	    Description:    通用文件列表
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GeneralFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ReadGeneralFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       ReadGeneralFile
	    Description:    通用文件读
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool ReadGeneralFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'WriteGeneralFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       WriteGeneralFile
	    Description:    通用文件写
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool WriteGeneralFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR
	
	//通用分类命令
	const   unsigned char   CMD_104GB_GEN_COMMAND = 0X30;

	//调取连续记录波形
	const   unsigned char   CMD_104GB_GET_SERIALRECORDER = 0X31;

	//连续记录检索
	const   unsigned char   CMD_104GB_QUERY_SERIALDATA = 0X32;

	//通用文件列表
	const   unsigned char   CMD_104GB_GENERAL_FILE_LIST = 0X33;

	//通用文件读
	const   unsigned char   CMD_104GB_GENERAL_READ_FILE = 0X34;

	//通用文件写
	const   unsigned char   CMD_104GB_GENERAL_WRITE_FILE = 0X35;

	//通讯参数协商
	const   unsigned char   CMD_104GB_COMM_PARAM_ARRANGE = 0X36;

	//终止数据传输
	const   unsigned char   CMD_104GB_COMM_STOP_TRANSFER = 0X37;

	/*********协议帧与命令的对应关系-->***********************/
	typedef struct tagPro104GBFrame2Cmd
	{
		unsigned char type;        //应用服务单元类型标识		
		unsigned char cmdFunCode;    //本系统内部使用的命令代码
		bool (*exuteCmd)(Protocol104_GB*,Linker * ,struct Command&);  //用于执行命令的函数
		const char *	cmdName; //消息名称
	}PRO104GBFRAME2CMD,*PPRO104GBFRAME2CMD;

	extern PRO104GBFRAME2CMD pro104GBFR2CmdMap[8];

	//通用分类命令
	bool GenCommand(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//调取连续记录波形
	bool GetSerialRecorder(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//连续记录检索
	bool QuerySerialData(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//通用文件列表
	bool GeneralFileList(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//通用文件读
	bool ReadGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//通用文件写
	bool WriteGeneralFile(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//通讯参数协商
	bool CommParamArrange(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

	//终止数据传输
	bool StopDataTransfer(Protocol104_GB *pProtocol,Linker *pLinker ,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_GB_H */

// }}}RME
