// {{{RME classifier 'Logical View::Protocol103'

#ifndef Protocol103_H
#define Protocol103_H

#ifdef PRAGMA
#pragma interface "Protocol103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol103.h
* 文件标识：
* 摘    要：解析103协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"
#include "Command.h"

#ifdef _MY_UNITTEST
    class Protocol103_UT;
#endif

class Protocol;

const unsigned char PRO103_VOLATILEFRAME_STARTCHAR = 0X68;  //可变帧的启动字符
const unsigned char PRO103_FIXFRAME_STARTCHAR = 0X10;       //固定帧的启动字符
const unsigned char PRO103_ENDCHAR = 0X16;                  //103协议的结束字符
const int PRO103_VFRAME_MAXSIZE = 1400;//4101;              //103协议可变帧最大长度
const int PRO103_ASDUDATA_MAXSIZE = 1388;//4089;            //103协议可变帧ASDU数据部分(即信息体部分)最大长度

const int PRO103_ASDU_FILENAMELEN = 40;                     //103协议在ASDU数据文件名类型条目中文件名的长度

//控制域的定义
typedef struct tagContolArea103
{
    union
    {
        //控制方向
        struct
        {
            unsigned char fun:4;
            bool fcv:1;
            bool fcb:1;
            bool prm:1;
            char res:1;
        }c;

        //监视方向
        struct
        {
            unsigned char fun:4;
            bool dfc:1;
            bool acd:1;
            bool prm:1;
            char res:1;
        }m;

        unsigned char control;
    };
}CONTOLAREA103;

/***定义103协议可变帧-->***/

//103协议可变帧帧头
typedef struct tagVFrameHead
{
    struct tagHead
    {
        unsigned char startChar1;    //启动字符
        unsigned char lengthLow;    //报文长度=控制域1+地址1+链路用户数据x
        unsigned char lengthHigh:4;
        unsigned char reserve:4;    //保留
        unsigned char startChar2;    //启动字符
    }HEAD;

    CONTOLAREA103 control;    //控制域
    unsigned char address;    //链路地址域
}VFRAMEHEAD,*PVFRAMEHEAD;

//103应用服务数据单元标识符
typedef struct tagASDUID
{
    unsigned char TYP;      //类型标识
    unsigned char VSQ:7;    //信息元素的数目
    unsigned char S:1;      //寻址方式
    unsigned char COT;      //传送原因
    unsigned char COMADDR;  //ASDU公共地址
}ASDUID,*PASDUID;

//103应用服务数据单元信息体标识符
typedef struct tagASDUInfID
{
    unsigned char FUN;      //功能类型
    unsigned char INF;      //信息序号
}ASDUINFID,*PASDUINFID;

const int ASDUID_LEN = sizeof(ASDUID);
const int ASDUINFID_LEN = sizeof(ASDUINFID);

//103协议可变帧帧尾
typedef struct tagVFrameEnd
{
    unsigned char CS;      //帧校验和，计算从控制域起，到此之前
    unsigned char endChar;    //结束字符
}VFRAMEEND,*PVFRAMEEND;	   

/***定义103协议可变帧<--***/


//定义103协议固定帧
typedef struct tagFixFrame
{
    unsigned char startChar;    //启动字符
    CONTOLAREA103 control;    //控制域
    unsigned char address;    //链路地址域
    unsigned char CS;      //帧校验和
    unsigned char endChar;    //结束字符        
}FIXFRAME,*PFIXFRAME;

// }}}USR
// }}}RME

class Protocol103 : public Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    friend class Protocol103_UT;
#endif

		/*************************************************
		Function:       StopRecWave
		Description:    停止录波
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool StopRecWave( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetContData
		Description:    读动态纪录数据
		Calls:          
		Called By:      
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool GetContData( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       ReadZeroOffset
		Description:    读实零漂刻度
		Calls:          
		Called By:      
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/	    
		virtual bool ReadZeroOffset( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       StartGnExperiment
		Description:    开始发电机实验
		Calls:          
		Called By:      
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool StartGnExperiment( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       StopGnExperiment
		Description:    停止发电机实验
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool StopGnExperiment( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetDeviceState
		Description:    读装置状态
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool GetDeviceState( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       SetDeviceState
		Description:    设置装置状态
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool SetDeviceState( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       OperateDevice
		Description:    硬件操作
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool OperateDevice( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetSplitRecWaveFile
		Description:    召唤分通道录波文件
		Calls:
		Called By:
		Input:          pLinker：通讯连接
		pCmd：要执行的命令

		Output:
		Return:         成功返回true，失败返回false
		*************************************************/
		virtual bool GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	Protocol103( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol103( void );
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
	Task * Decode( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'DecodeVolatileFrame(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DecodeVolatileFrame
	    Description:    解码可变帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DecodeVolatileFrame( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'DecodeFixFrame(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DecodeFixFrame
	    Description:    解码固定帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DecodeFixFrame( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'doCheck(char*,int,bool,unsigned char *)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       doCheck
	    Description:    计算数据包的校验和
	    Calls:          
	    Called By:      
	    Input:          packet：包数据
	                    packetlen：包长度
	                    isFixFrame：是否是固定帧
	    Output:         result：校验和结果
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool doCheck( char * packet, int packetlen, bool isFixFrame, unsigned char * result );
	// }}}RME
	// {{{RME operation 'SendInterruptFrame(Linker *,struct Command *)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       SendInterruptFrame
	    Description:    有新的故障报告时发送中断帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接	                    
	                    pCmd：被中断相关命令
	    Output:         
	                    
	    Return:         true：中断帧发送成功；false：没有新的故障报告或中断帧发送失败
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendInterruptFrame( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetFrame(Linker *,struct Command *,PFIXFRAME,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFrame
	    Description:    将网络数据流解析成一个协议帧，帧的类型（固定、可变）未知
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接	                    
	                    uiTimeOut：超时时间，单位毫秒，0表示永不超时
	    Output:         pFixFrame：保存返回的固定帧数据包
	                        vFramePacket：保存返回的可变帧数据包
	                    vFrameLen：可变帧数据包长度
	                    pOutVFrameHead：VFRAMEHEAD部分的数据
	                    pOutASDUID：ASDUID部分的数据
	                    pOutAsduData：信息体部分的数据
	                    pAsduDataLen：信息体长度
	                    pOutVFrameEnd：VFRAMEEND部分的数据
	    Return:         -1：失败；  1：返回固定帧； 2：返回普通可变帧；  3：故障中断帧
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetFrame( Linker * pLinker, struct Command * pCmd, PFIXFRAME pFixFrame, char * vFramePacket, int * vFrameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'AnalyseFixFrame(Linker *,PFIXFRAME,bool,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseFixFrame
	    Description:    将网络数据流解析成一个固定长帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    hasReadStartChar：帧的第一个启动字符是否已经被读出
	                    uiTimeOut：超时时间，单位毫秒，0表示永不超时
	    Output:         pFixFrame：保存返回的固定帧
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AnalyseFixFrame( Linker * pLinker, PFIXFRAME pFixFrame, bool hasReadStartChar, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'AnalyseVolatileFrame(Linker *,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,bool,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseVolatileFrame
	    Description:    将网络数据流解析成一个可变帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    hasReadStartChar：帧的第一个启动字符是否已经被读出
	                    uiTimeOut：超时时间，单位毫秒，0表示永不超时
	    Output:         framePacket：保存返回的可变帧数据包
	                    frameLen：数据包长度
	                    pOutVFrameHead：VFRAMEHEAD部分的数据
	                    pOutASDUID：ASDUID部分的数据
	                    pOutAsduData：信息体部分的数据
	                    pAsduDataLen：信息体长度
	                    pOutVFrameEnd：VFRAMEEND部分的数据
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AnalyseVolatileFrame( Linker * pLinker, char * framePacket, int * frameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, bool hasReadStartChar, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'SendConfirm(Linker*,unsigned int,bool)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendConfirm
	    Description:    发送确认帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                        devId：设备编号
	                    acd：true-发送正确的确认；false-发送否定的确认
	    Output:         
	                    
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendConfirm( Linker * pLinker, unsigned int devId, bool acd );
	// }}}RME
	// {{{RME operation 'WantConfirm(Linker*,struct Command *)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       WantConfirm
	    Description:    期待确认帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	                    
	    Return:         -1：异常，通常是通讯上的异常；
	                    0：获得否定的确认；
	                    1：获得正确的确认
	    *************************************************
	   }}}USR */
	// }}}RME
	int WantConfirm( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SendLevelOne(Linker*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       SendLevelOne
	    Description:    召唤1级数据
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                        devId：设备编号	                    
	    Output:         
	                    
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendLevelOne( Linker * pLinker, unsigned int devId );
	// }}}RME
	// {{{RME operation 'WantLevelOne(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       WantLevelOne
	    Description:    期待召唤1级数据帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	                    
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool WantLevelOne( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'GetPro2CmdIndex(unsigned char,unsigned char,unsigned char,unsigned short,bool)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetPro2CmdIndex
	    Description:    获取由协议包到命令的映射索引号
	    Calls:          
	    Called By:      
	    Input:          direct：方向
	                    func：功能码
	                    cot：原因
	                    gin：组号
	                    isFixFrame：是否是固定帧
	    Output:         
	                    
	    Return:         成功返回索引号，失败返回-1
	    *************************************************	
	   }}}USR */
	// }}}RME
	int GetPro2CmdIndex( unsigned char direct, unsigned char func, unsigned char cot, unsigned short gin, bool isFixFrame );
	// }}}RME

public:
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
	virtual bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
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
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'GetPro2CmdIndex(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetPro2CmdIndex
	    Description:    由命令获取命令与协议功能映射索引号
	    Calls:          
	    Called By:      
	    Input:          cmd：命令                    
	                    
	    Output:                             
	    Return:         成功返回索引号，失败返回-1
	    *************************************************	
	   }}}USR */
	// }}}RME
	int GetPro2CmdIndex( struct Command & cmd );
	// }}}RME
	// {{{RME operation 'CreateMsg(int,unsigned int,bool,unsigned char,int,char*,int,int*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateMsg
	    Description:    根据消息号创建消息字符流
	    Calls:          
	    Called By:      
	    Input:          msgNo：消息号
	                    deviceId：设备编号
	                    fcbOrAcd：acd
	                    COT：原因
	                    infoNo：信息号
	                    ASDUData：信息体数据
	                    ASDUDataLen：信息体数据长度
	    Output:         msgLen：消息字符流长度
	    Return:         成功返回消息字符流，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	string * CreateMsg( int msgNo, unsigned int deviceId, bool fcbOrAcd, unsigned char COT, int infoNo, char * ASDUData, int ASDUDataLen, int * msgLen );
	// }}}RME

public:
	// {{{RME operation 'GetDauInf(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDauInf
	    Description:    查询录波器运行状态信息
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetDauInf( Linker * pLinker, struct Command * pCmd );
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
	virtual bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
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
	virtual bool GetReportFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SetTime(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SetTime
	    Description:    对时（时间同步）
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool SetTime( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetTime(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetTime
	    Description:    获取时间
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetTime( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartRecByManual(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartRecByManual
	    Description:    手动启动录波
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartRecByManual( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartRecByTimer(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartRecByTimer
	    Description:    定时启动录波
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartRecByTimer( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'CreateGIMsg(int,unsigned int,unsigned char,int,GI[],unsigned char,int*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateMsg
	    Description:    创建通用分类消息字符流
	    Calls:          
	    Called By:      
	    Input:          msgNo：消息号
	                    deviceId：设备编号
	                    fcbOrAcd：acd
	                    COT：原因
	                    infoNo：信息号
	                    GIs：GI信息                    
	    Output:         msgLen：消息字符流长度
	    Return:         成功返回消息字符流，失败返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	string * CreateGIMsg( int msgNo, unsigned int deviceId, unsigned char COT, int infoNo, GI GIs[], unsigned char GINumber, int * msgLen );
	// }}}RME

public:
	// {{{RME operation 'GetFileList(Linker *,struct Command*)'
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
	virtual bool GetFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'PullFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PullFile
	    Description:    获取文件
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool PullFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'PushFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PushFile
	    Description:    下传文件
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool PushFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ReadRltData(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ReadRltData
	    Description:    读实时数据
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ReadRltData( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSend(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSend
	    Description:    主动上传
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool AutoSend( Linker * pLinker, struct Command * pCmd );
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
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol103_H */

// }}}RME
