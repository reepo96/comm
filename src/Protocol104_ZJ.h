// {{{RME classifier 'Logical View::Protocol104_ZJ'

#ifndef Protocol104_ZJ_H
#define Protocol104_ZJ_H

#ifdef PRAGMA
#pragma interface "Protocol104_ZJ.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol104.h>
class DataControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
#include "public.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol104_ZJ.h
* 文件标识：
* 摘    要：浙江104协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

#define		PRO104ZJ_FRAME_MAXSIZE	4090			//帧最大长度
#define		PRO104ZJ_ASDUDATA_MAXSIZE	4078		//帧ASDU DATA(信息元素集部分)最大长度

#define		PRO104ZJ_FILE_LEN	40		//文件名长度

// }}}USR
// }}}RME

class Protocol104_ZJ : public Protocol104
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	typedef struct tagMarkEvnt
	{
		unsigned char groupNo;
		unsigned char entryNo;
		unsigned char devAddr;
		char desc[40];
		struct CP56Time2a startTime;
		unsigned short timeLen;
		unsigned int fileLen;
		char fileName[40];
	}MARKEVNT,*PMARKEVNT;

	/**************************************************
	    Function:       CreateReportFile
	    Description:    创建故障简报
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/	
	bool CreateReportFile( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       AutoSendReportFile
	    Description:    自动上传故障简报
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/	
	bool AutoSendReportFile( Linker * pLinker, struct Command * pCmd );
				
	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	DWORD	m_dwBeginIdleTime;

	/**************************************************
	    Function:       SendReportFile
	    Description:    发送故障简报
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
						type：类型 1-主动召唤，2-自动上传
						pReportData：简报数据
						DataLen：简报数据长度
						startPos：起始传输位置

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/	
	bool SendReportFile( Linker * pLinker, struct Command * pCmd,int type,const char *pReportData,int DataLen,unsigned int startPos =0);

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

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104_ZJ( void );
	// }}}RME
	// {{{RME operation 'Protocol104_ZJ()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104_ZJ( void );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseFrame(Linker *,char*,int*,APCI4Prot104*,PASDUID104*,PASDUINFID104*,char**,int*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseFrame
	    Description:    将网络数据流解析成一个帧
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    uiTimeOut：超时时间，单位毫秒，0表示永不超时
	    Output:         framePacket：保存返回帧的数据包
	                    frameLen：数据包长度
	                    pAPCI：APCI部分的数据
	                    pOutASDUID：ASDUID部分的数据
						pOutASDUInfID:ASDUInfID部分的数据
	                    pOutAsduData：信息体部分的数据
	                    pAsduDataLen：信息体长度
	    Return:         -1:网络错误或协议帧错误;  0:链路没有数据   1:成功解析出协议帧
	    *************************************************
	   }}}USR */
	// }}}RME
	int AnalyseFrame( Linker * pLinker, char * framePacket, int * frameLen, APCI4Prot104 * pAPCI, PASDUID104 * pOutASDUID, PASDUINFID104 * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, unsigned int uiTimeOut = 0 );
	// }}}RME

public:
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
	// {{{RME operation 'AnalyseTask(PASDUID104,char*,int,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( PASDUID104 pAsduId, char * pAsduData, int iAsduDataLen, Linker * pLinker );
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
	// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    召唤录波文件列表
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFile
	    Description:    召唤录波文件
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportFile
	    Description:    获取简要录波报告
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetReportFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetMarkEventList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetMarkEventList
	    Description:    调事件列表
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetMarkEventList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSendDevState(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSendDevState
	    Description:    自动上传设备状态
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AutoSendDevState( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSendMarkEvent(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSendMarkEvent
	    Description:    自动上传标记事件
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool AutoSendMarkEvent( Linker * pLinker, struct Command * pCmd );
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
	Task * DoIdle( Linker * pLinker );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

	//调事件列表
	const   unsigned char   CMD_104ZJ_GET_MARKEVN_LIST = 0X30;

	/*********协议帧与命令的对应关系-->***********************/
	typedef struct tagPro104ZJFrame2Cmd
	{
		unsigned char type;        //应用服务单元类型标识
		unsigned char cmdFunCode;    //本系统内部使用的命令代码
		bool (*exuteCmd)(Protocol104_ZJ*,Linker * ,struct Command&);  //用于执行命令的函数
		const char *	cmdName; //消息名称
	}PRO104ZJFRAME2CMD,*PPRO104ZJFRAME2CMD;

	extern PRO104ZJFRAME2CMD pro104ZJFR2CmdMap[8];

	//召唤录波文件列表
    bool GetRecWaveFileList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//召唤录波文件
    bool GetRecWaveFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

    //简要录波报告
    bool GetReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//调事件列表
    bool GetMarkEventList(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//自动上传设备状态
    bool AutoSendDevState(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//自动上传标记事件
    bool AutoSendMarkEvent(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//创建故障简报文件
    bool CreateReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

	//自动上传故障简报文件
    bool AutoSendReportFile(Protocol104_ZJ *pProtocol,Linker * pLinker,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_ZJ_H */

// }}}RME
