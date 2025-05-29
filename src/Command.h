// {{{RME classifier 'Logical View::Command'

#ifndef Command_H
#define Command_H

#ifdef PRAGMA
#pragma interface "Command.h"
#endif

#include <RTSystem/Communication.h>
#include <CmdState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Command.h
* 文件标识：
* 摘    要：本系统内部使用的命令结构
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#pragma warning( disable : 4786 )

#include "public.h"

#include "dfrdef.h"

//#include "SHType.h"
#include <vector>
using namespace std;

//命令来自于或者发往哪个方向
typedef enum CmdDir{CD_REMOTE,CD_UI,CD_TIMER,CD_ASDU/*暂态态设备*/,CD_CONST_DAU/*稳态设备*/,CD_NEURON} CMDDIR;

//CP56Time2a时间格式
struct CP56Time2a
{
    unsigned short milliseconds;    //毫秒
    unsigned char minute:6;         //分钟
    bool          res1:1;           //备用
    bool          IV:1;             //非法标志
    unsigned char hours:5;          //小时
    char          res2:2;
    bool          SU:1;             //夏令时标志
    unsigned char day:5;            //日期
    unsigned char week:3;           //星期(1-7)
    unsigned char month:4;
    unsigned char res3:4;
    unsigned char year:7;
    bool          res4:1;
};


//数据类型，即传给命令的参数，或者命令执行后输出的数据的类型
//CDT_TIME：时间类型，使用CP56Time2a结构；CDT_CHAR：一般的字符类型；CDT_INT：4字节的无符整数
//CDT_SHTIME：时间类型，使用SHTimeStruct结构
typedef enum CmdDataType{CDT_TIME,CDT_INT,CDT_CHAR,CDT_SHTIME} CMDDATATYPE;

//命令参数的格式
typedef struct tagCmdParam
{
    CMDDATATYPE paramType;
    unsigned char paramLen;
    unsigned char data[256];
}CMDPARAM,*PCMDPARAM;

/**********命令执行结果定义-->********************/

//结果数据类型
//CDRT_TRUFALSE：真假类型，表示命令执行是否成功；
//CDRT_CHAR：一般的字符类型；
//CDRT_FILELIST：文件名列表；
//CDRT_TIME：时间类型
//CDRT_REPORT：录波简报结果；
//CDRT_INT：4字节的无符整数；
//CDRT_PROGRESS：进度信息（0~100）
//CDRT_DIRFILEINF：CMDDIRFILEINF结构的数据；
//CDRT_DESC：描述信息（通常是错误描述信息）
//CDRT_POINTDATA：采样点数据格式；
//CDRT_FILEPATH：文件路径；
//CDRT_STARTRECMSG：故障录波启动信息
//CDRT_MARKEVENT:标记事件信息
//CDRT_FILENAME:文件名；
//CDRT_PQ_TRENDDATA：电能质量趋势数据； 
//CDRT_PQ_EVENT：电能质量事件；
//CDRT_PQ_FILEINF：电能质量完结信息
typedef enum CmdResultType{CDRT_TRUFALSE,CDRT_CHAR,CDRT_FILELIST,CDRT_TIME,CDRT_REPORT,CDRT_INT,CDRT_PROGRESS,\
CDRT_DIRFILEINF,CDRT_DESC,CDRT_POINTDATA,CDRT_FILEPATH,CDRT_STARTRECMSG,CDRT_MARKEVENT,CDRT_FILENAME,\
CDRT_PQ_TRENDDATA,CDRT_PQ_EVENT,CDRT_PQ_FILEINF,CDRT_PQ_10CYCDATA,CDRT_PQ_STATINF,CDRT_IP,CDRT_USER_INFO} CMDRESULTTYPE;

struct SHTimeStruct
{ 
	WORD	Year;		//年
	BYTE	Month;		//月
	BYTE	Day;		//日
	BYTE	Hour;		//时
	BYTE	Minute;		//分
	BYTE	Second;		//秒
	WORD	Millisecond;//毫秒0.001s
	WORD	Microsecond;//微秒0.001*0.001
};

//命令执行结果格式
typedef struct tagCmdResult
{
    CMDRESULTTYPE resultType;   //结果数据类型
    unsigned int resultLen;   //结果长度
    char *pData;                //结果数据	
}CMDRESULT,*PCMDRESULT;

typedef struct check
{
	/*
	unsigned int 	time_sec;		//本次自检信息生产的时间(秒)
	unsigned int 	time_usec;		//本次自检信息生产的时间，秒以下(微秒)
	unsigned int	dsp_ver;		//DSP版本
	unsigned int	ram_ver;		//RAM版本
	unsigned int	mm_ver;			//暂态监控版本
	unsigned int	md_ver;			//稳态监控版本
	unsigned int	dsp_err_no;		//dsp出错类型码
	unsigned int	ram_err_no;		//ram出错类型码
	unsigned int	mm_err_no;		//暂态出错类型码
	unsigned int	md_err_no;		//稳态出错类型码	
	unsigned int	intio_state;	//输入接口的状态，低16位有效
	unsigned int	outio_state;	//输出接口的状态，低16位有效
	unsigned int	gps_err_no;		//GPS错误码
	unsigned int	reservation1;	//保留
	unsigned int	reservation2;	//保留
	*/
	unsigned int 	time_sec;		//本次自检信息生产的时间(秒)
	unsigned int 	time_usec;	//本次自检信息生产的时间，秒以下(微秒)
	unsigned int	dsp_ver;		//DSP版本
	unsigned int	ram_ver;		//RAM版本
	unsigned int	 Module_ver[16];	//监控板各模块版本，为0表示无此模块：
								// Module_ver[0]：暂态模块版本
								// Module_ver[1]：稳态模块版本
								// Module_ver[2]：电能质量模块版本
								// Module_ver[3]：PMU模块版本
								// Module_ver[4]：通讯模块版本
								//其它：预留
	unsigned int	dsp_err_no;		//dsp出错类型码，见2.2.1节
	unsigned int	ram_err_no;		//ram出错类型码
	unsigned int	m_err_no;		//监控出错类型码	
	unsigned int	intio_state;	//输入接口的状态，低16位有效
	unsigned int	outio_state;	//输出接口的状态，低16位有效
	unsigned int	gps_err_no;		//GPS错误码，见2.2.1节
	unsigned int	reservation1;	//保留
	unsigned int	reservation2;	//保留

}CHECK;

//录波文件名列表格式
typedef struct tagCmdFileList
{
    unsigned char devID;        //文件录波装置地址
    char fileName[40];          //文件名称（不含扩展名）
    struct CP56Time2a faultTime;    //文件故障时间
}CMDFILELIST,*PCMDFILELIST;

//通用文件信息格式
typedef struct tagCmdGeneralFileInf
{
    char fileName[100];         //文件名称（可以包含目录）
    struct CP56Time2a fileTime; //文件时间
    unsigned int fileLen;		//文件长度
}CMDGENERALFILEINF,*PCMDGENERALFILEINF;

//通用文件/目录信息格式
typedef struct tagGeneralFilePathInf
{
    char fileName[100];         //文件/目录名称
    struct CP56Time2a fileTime; //文件时间
	unsigned char isDir;		//是否是目录，1是目录，0不是
    unsigned int fileLen;		//文件长度
}GENERALFILEPATHINF,*PGENERALFILEPATHINF;

//目录文件信息格式
typedef struct tagCmdDirFileInf
{
    int fileNo;         //文件号
    char fileTime[25]; //文件时间,yyyy-mm-dd hh:MM:ss格式
    char fileName[100]; //文件名称
    int fileLen;        //文件长度
}CMDDIRFILEINF,*PCMDDIRFILEINF;

//设备状态
typedef struct tagDeviceState
{
	unsigned int devId;
	unsigned char state;// 0：未知状态， 1：空闲状态， 2：工作状态， 3：无效状态
}DAVICESTATE,*PDAVICESTATE;

//录波简报格式
typedef struct tagCmdReport
{
	struct tagFPT
	{
		bool    A_Fault:1;      //A相故障
		bool    B_Fault:1;      //B相故障
		bool    C_Fault:1;      //C相故障
		bool    isEarthing:1;   //是否是接地故障
		bool    isOutArea:1;    //是否是区外故障，true：区外故障；false：区内故障
		char    res:2;          //保留
		bool    isValid:1;      //有效位
	} FPT;//故障相别及类型

	struct tagJPT
	{
		bool    A_Jump:1;      //A相跳闸
		bool    B_Jump:1;      //B相跳闸
		bool    C_Jump:1;      //C相跳闸
		char    res:4;          //保留
		bool    isValid:1;      //有效位
	} JPT;//跳闸相别

	unsigned char SCL[4];       //故障距离

	struct CP56Time2a faultTime;    //故障时间
	char recWaveFileName[40];       //不含扩展名的录波文件名
	char BASName[40];               //故障间隔名
	char res[100];                  //保留
}CMDREPORT,*PCMDREPORT;

//录波简报头
typedef struct tagReportHead
{
    unsigned char devID;        //文件录波装置地址
    int            rptID;       //录波文件id
    CMDREPORT      report;      //录波简报
}REPORTHEAD,*PREPORTHEAD;

//定义包消息结构
typedef struct tagPackInfo
{
	char flag[2]; //标志
	char msgType; //消息类型(1：实时显示，2：零漂数据)
	unsigned short packNo; //包序号(每秒增一)
	unsigned short frameNo; //帧号（每个包第一帧号为1：还有后续包，0为最后一帧）
}PACKINFO,*PPACKINFO;

//原始采样点数据头
typedef struct tagOrigPointDataHead
{
    unsigned int  ucSendCountCycle; //要发送的周波总数
	unsigned int  ucPerCyclePoint;  //每周波采样点数
	unsigned int wPerSampleSize; //每个采样点占的字节数
	unsigned int ucTotalSw;        //开关量通道总数
	unsigned int ucTotalAn;        //模拟通道总数
	unsigned int wReserve; //备用字节,暂时置零
}ORIGPOINTDATAHEAD,*PORIGPOINTDATAHEAD;


typedef struct tagRtDataHead
{
    unsigned long  dwSendCountCycle;	//要发送的周波总数
    unsigned long  dwPerCyclePoint; 	//每周波采样点数
    unsigned long  dwTotalAn;      	//模拟通道总数
    unsigned long  dwTotalSw;      	//开关量通道总数，
}RTDATAHEAD,*PRTDATAHEAD;

//采样点数据头
typedef struct tagPointDataHead
{
	struct SHTimeStruct time;   //时间
	unsigned char ChannelNumber;//通道数
	unsigned char SwitchNumber; //开关组数，以16个开关量为1组
	unsigned int Chan[3];       //需要实时显示的模拟通号,1取，0不取
	float samplrate;    //采样率
	unsigned int point;         //point
}POINTDATAHEAD,*PPOINTDATAHEAD;

//标记事件
typedef struct tagMarkEvent
{
	char State;		//状态：1——启动、3——结束
	unsigned char DAUID;	//设备编号
	unsigned int MarkId;			//事件序号
	struct SHTimeStruct StartTime;	//事件开始时间
	struct SHTimeStruct EndTime;	//事件结束时间
	char FaultComponent[20];		//故障设备
	int MarkEvtType;				//事件类型
	float	CurrentVal;				//采样值
	char Describe[40];				//描述
	unsigned int FileLen;			//文件大小
	char FileName[100];				//文件名
}MARKEVENT,*PMARKEVENT;

/**********命令执行结果定义<--********************/

//定义命令功能代码

//查询录波器运行状态信息
const   unsigned char   CMD_GETDAUINF = 0X01;

//本地设置ADU的时间，或者远程设置本地的时间
//此命令要传入1个参数：
//  1：时间，CDT_TIME类型
const   unsigned char   CMD_SETTIME = 0X02;

//从ADU获取时间
//此命令要传入1个参数：
//  1：时间类型（CDT_CHAR类型）：
//          1——MONITOR时间
//          2——DCR时间
//          3——时钟芯片时间
//          4——系统时间。（默认）
const   unsigned char   CMD_GETTIME = 0X03;


//获取故障简报数据
//此命令不需参数：
const   unsigned char   CMD_GETREPORTFILE = 0X05;

//获得录波文件列表
//此命令要传入2个参数：
//  1：文件开始时间，CDT_TIME类型
//  2：文件结束时间，CDT_TIME类型
const   unsigned char   CMD_GETWAVEFILELIST = 0X06;

//获得通用文件列表
//对于远程此命令要传入3个参数：
//  1：文件开始时间，CDT_TIME类型
//  2：文件结束时间，CDT_TIME类型
//  3：一般文件名称（目录名及通配符(100个字节)）
//对于DAU此命令要传入2个参数：
//  1：目录类型，CDT_CHAR类型
//          90：故障录波目录
//          91：运行日记目录
//          92：标记事件目录
//          93：低速录波目录
//          94：电能质量目录
//          95：动态记录目录
//          96：趋势数据目录
//  2：文件数：最近的文件数，0表示目录下所有的文件
const   unsigned char   CMD_GETFILELIST = 0X07;

//获得录波文件
//远程调用此命令要传入2个参数：
//  1：文件名称（不含扩展名，40字节）
//  2：起始传输位置，CDT_INT类型
//
//SHNeuron调用此命令要传入3个参数：
//  1：录波文件类型（1字节）：1-故障录波 2-低速录波
//  2：文件名称（不含扩展名，40字节）
//  3：起始传输位置，CDT_INT类型
const   unsigned char   CMD_GETWAVEFILE = 0X08;

//自动上传录波文件给远程
//此命令要传入1个参数：
//  1：文件名称（不含扩展名）
const   unsigned char   CMD_SENDWAVEFILE = 0X09;

//手动启动录波
//此命令需1参数：
//  参数1：启动类型（1字节）：1-故障录波 2-低速录波
const   unsigned char   CMD_STARTREC_MANUAL = 0X0A;

//定时启动录波
//此命令需4参数：
//  参数1：启动类型（1字节）：1-故障录波 2-低速录波
//  参数2：启动开始时间：SHTimeStruct结构
//  参数3：间格（1字节）：故障录波时的单位为秒，必须大于5秒；低速录波时的单位为分，大于设定低速记录时间长度
//  参数4：启动次数（2字节）
const   unsigned char   CMD_STARTREC_TIMER = 0X0B;

//获取通用文件
//此命令需6参数：
//  参数1：文件类型（1字节）：
//                      1：SHW格式
//                      2：SHD格式
//                      3：SWR格式
//                      4：TRM格式
//                      5：COMTRAD_CFG格式
//                      6：COMTRAD_DAT格式
//                      7：COMTRAD_HDR格式
//                      8：故障报告文件
//                      100：故障摘要文件
//                      101：标记事件文件
//                      102：运行日记文件
//                      200：DSP故障录波文件
//                      210：全部配置文件
//                      211：线路配置文件
//                      212：运行配置文件
//  参数2：文件号（4字节）：文件类型为1-80时，文件号为该文件的唯一标识号；
//                          类型为100-110时，文件号表示当前文件最近项数，0则表示全部内容
//  参数3：文件开始位置（4字节）：文件类型为1-80时有效
//  参数4：文件长度：长度未知，则传入0
//  参数5：文件名称：100个字节未使用字节补0
//  参数6：文件时间：字符串,yyyy-mm-dd hh:MM:ss格式
const   unsigned char   CMD_PULLFILE = 0X0C;

//下传文件
//此命令需4参数：
//  参数1：文件类型（1字节）：
//                      20：全部配置文件
//                      21：线路配置文件
//                      22：运行配置文件
//  参数2：用户名称（20字节）
//  参数3：密码（10字节）
//  参数4：文件名称（100字节）
const   unsigned char   CMD_PUSHFILE = 0X0D;


//主动上传
//此命令需2参数：
//  参数1：类型（1字节）：
//                      1：故障录波报告
//                      2：标记事件文件
//  参数2：文件名称（100字节）
const   unsigned char   CMD_AUTOSEND = 0X0E;

//读实时数据
//此命令需2参数：
//  参数1：类型（1字节）：
//                      1：采样数据
//                      2：计算数据
//  参数2：启动标志（1字节）：
//                      1：启动
//                      2：停止
const   unsigned char   CMD_READ_RLT_DATA = 0X0F;

//故障录波启动信息(DAU自动上传信息的功能)
const   unsigned char   CMD_STARTINFO = 0X10;

//运行状态信息(DAU自动上传信息的功能)
const   unsigned char   CMD_RUNINFO = 0X11;

//停止录波
const   unsigned char   CMD_STOPREC = 0X12;

//读动态纪录数据
//此命令需3参数：
//  参数1：类型（1字节）：
//                      1：动态纪录数据
//                      2：趋势纪录数据
//  参数2：开始时间（4字节）
//  参数3：数据间隔时间（4字节）
const   unsigned char   CMD_GET_CONT_DATA = 0X13;

//读零漂、刻度
//此命令需1参数：
//  参数1：类型（1字节）：
//                      1：采样数据
//                      2：计算数据
const   unsigned char   CMD_READ_ZEROOFFSET = 0X14;

//读装置状态
//此命令需1参数：
//  参数1：类型（1字节）：
//                      1：全部功能
//                      2：故障录波
//                      4：低速录波
//                      8：动态记录
//                      16：PMU
//                      32：电能质量
const   unsigned char   CMD_GET_DEVICE_STATE = 0X15;

//设置装置状态
//此命令需3参数：
//  参数1：类型（1字节）：
//                      1：全部功能
//                      2：故障录波
//                      4：低速录波
//                      8：动态记录
//                      16：PMU
//                      32：电能质量
//  参数2：状态（1字节）：
//                      1：运行状态
//                      2：调试状态（对应类型功能停止）
//  参数3：（2字节）：从调试状态返回运行状态的时间，单位分钟。“状态”参数为2时起作用
const   unsigned char   CMD_SET_DEVICE_STATE = 0X16;

//硬件操作
//此命令需2参数：
//  参数1：类型（1字节）：
//                      1：告警复归
//                      2：启动复归
//                      3：自检命令(在调试状态下可用)
//  参数2：类型参数（4字节）
const   unsigned char   CMD_OPERATE_DEVICE = 0X17;

//自动上传标记事件(DAU自动上传信息的功能)
const   unsigned char   CMD_AUTOSEND_MARKEVENT = 0X18;

//创建故障简报
const   unsigned char   CMD_CREATEREPORTFILE = 0X19;
//参数：不含扩展名的文件路径（512字节）

//自动上传故障简报
const   unsigned char   CMD_AUTOSEND_REPORTFILE = 0X1A;
//参数：故障简报文件数据

//获取故障录波分通道文件
const   unsigned char   CMD_GET_SPLIT_WAVEFILE = 0X1B;

//调取监控板网络配置
const   unsigned char   CMD_GET_NET_CFG = 0X1C;

//设置监控板网络配置
const   unsigned char   CMD_SET_NET_CFG = 0X1D;

//调取审计文件
const   unsigned char   CMD_GET_AUDIT_FILE = 0X1E;

//发送审计告警
const   unsigned char   CMD_SEND_AUDIT_ALARM = 0X1F;

//用户登录
const   unsigned char   CMD_USER_LOGIN = 0X20;


//通讯模块退出
//无参数
const   unsigned char   CMD_EXIT = 0X50;

//重建远程通讯相关配置
//无参数
const   unsigned char   CMD_REBUILD_RMT = 0X51;

//重建DAU通讯相关配置
//无参数
const   unsigned char   CMD_REBUILD_DAU = 0X52;

//重建其它与实际通讯无关配置
//无参数
const   unsigned char   CMD_REBUILD_OTHER = 0X53;

//查询某个链路的状态
//无参数
const   unsigned char   CMD_QUERY_LINKSTATE = 0X54;

//自动上传链路的状态
const   unsigned char   CMD_AUTOSEND_LINKSTATE = 0X55;

//连接设备
const   unsigned char   CMD_CONNECT_DEVICE = 0X56;

//断开设备
const   unsigned char   CMD_DISCONNECT_DEVICE = 0X57;

//后台生成新的故障简报，则需通知通讯模块去获取新的故障简报
//此命令需1参数：
//  参数1：文件号（int 4字节）
const   unsigned char   CMD_NEW_FAULTREPORT = 0X60;

//开始发电机实验
const   unsigned char   CMD_STARTGNEXPERIMENT = 0X61;

//停止发电机实验
const   unsigned char   CMD_STOPGNEXPERIMENT = 0X62;
// }}}USR
// }}}RME

struct Command
{
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	public:
	unsigned char	processType;	//发送命令的进程的类型
	unsigned int	processId;		//发送命令的进程id
	bool			isSplitCmd;		//是否是分命令(一个命令被分成两个命令)

	//克隆命令
	void Clone(struct Command *pNewCmd);
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
	// {{{RME classAttribute 'msgId'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int msgId;
	// }}}RME
	// {{{RME classAttribute 'from'
	// {{{RME general 'documentation'
	/* {{{USR
	命令来自于哪个方向：Remote,UI,Timer,ASDU
	   }}}USR */
	// }}}RME
	CMDDIR from;
	// }}}RME
	// {{{RME classAttribute 'to'
	// {{{RME general 'documentation'
	/* {{{USR
	命令发到哪个方向：Remote,UI,Timer,ASDU
	   }}}USR */
	// }}}RME
	CMDDIR to;
	// }}}RME
	// {{{RME classAttribute 'destDevId'
	// {{{RME general 'documentation'
	/* {{{USR
	目标设备id：变电站编号(中间2字节)+设备编号(低1字节),最高1字节暂不使用
	   }}}USR */
	// }}}RME
	unsigned int destDevId;
	// }}}RME
	// {{{RME classAttribute 'funCode'
	// {{{RME general 'documentation'
	/* {{{USR
	功能代码
	   }}}USR */
	// }}}RME
	unsigned char funCode;
	// }}}RME
	// {{{RME classAttribute 'parmLen'
	// {{{RME general 'documentation'
	/* {{{USR
	命令参数总长度
	   }}}USR */
	// }}}RME
	unsigned short parmLen;
	// }}}RME
	// {{{RME classAttribute 'pParm'
	// {{{RME general 'documentation'
	/* {{{USR
	命令参数数据	    
	   }}}USR */
	// }}}RME
	char * pParm;
	// }}}RME
	// {{{RME classAttribute 'resultNum'
	// {{{RME general 'documentation'
	/* {{{USR
	返回结果结构的个数
	   }}}USR */
	// }}}RME
	unsigned short resultNum;
	// }}}RME
	// {{{RME classAttribute 'resultList'
	// {{{RME general 'documentation'
	/* {{{USR
	结果列表
	   }}}USR */
	// }}}RME
	vector< PCMDRESULT > resultList;
	// }}}RME
	// {{{RME classAttribute 'describe'
	// {{{RME general 'documentation'
	/* {{{USR
		命令描述
	   }}}USR */
	// }}}RME
	char describe[ 64 ];
	// }}}RME
	// {{{RME associationEnd '_state'
	CmdState _state;
	// }}}RME
	// {{{RME operation 'Command()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Command( void );
	// }}}RME
	// {{{RME operation '~Command()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~Command( void );
	// }}}RME
	// {{{RME operation 'operator==(const Command &)'
	// {{{RME general 'documentation'
	/* {{{USR
	    判断两个命令是否相同的
	   }}}USR */
	// }}}RME
	bool operator==( const Command & rtg_arg );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Command_H */

// }}}RME
