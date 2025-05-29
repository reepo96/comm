#if !defined (PQDEFXXXXXX_H)
#define PQDEFXXXXXX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dfrdef.h"
/*******************************命令功能码********************************/

#pragma pack(1)
//电能质量自动上送 
/*
参数 1  通信链路解析收到的一帧自动上送数据， 数据内容不定，   
生成该任务命令后 调用相关函数继续 获取剩余类型数据 ，最后写入共享内存
*/
const   unsigned char   CMD_PQAUTOSEND = 0X63;


const   unsigned char   CMD_PQ_AUTORTDATA = 0X64;   //电能质量实时数据

const   unsigned char   CMD_PQ_AUTOTRENDDATA = 0X65; //电能质量趋势数据

const   unsigned char   CMD_PQ_AUTOFILEINF = 0X66;	 //电能质量趋势文件完结信息

const   unsigned char   CMD_PQ_AUTOEVENT = 0X67;	 //电能质量事件信息

//调取电能质量暂态录波文件
/*
参数 1  开始事件文件名 40字节
参数 2  结束事件文件名 40字节	
参数 3  起始位置       4字节
*/
const   unsigned char   CMD_GETPQWAVEFILE = 0X68;

//调取电能质量事件信息
/*
参数 1  电能质量事件号 4字节
*/
const   unsigned char   CMD_GETPQEVENT = 0X69;


//调取电能质量趋势文件
/*
...
*/
const   unsigned char   CMD_GETPQTRHFILE = 0X6A;

//调取系统运行日志
/*
参数 1  开始时间        CP56Time2a
参数 2  结束时间		CP56Time2a
*/
const   unsigned char   CMD_GETRUNNINGLOG = 0X6B;

//电能质量性能检验
/*
*/
const   unsigned char   CMD_PQTEST = 0X6C;

const   unsigned char   CMD_PQRESUMSET = 0X70;		//恢复出厂设置

const   unsigned char   CMD_PQ_AUTOT10CYCDATA = 0X72; //电能质量10周波周期数据

const   unsigned char   CMD_PQ_10CYCDATA_EXP = 0X73; //电能质量10周波周期数据导出

const   unsigned char   CMD_PQ_GET_10CYCDATA_FILE = 0X74; //取电能质量10周波周期数据文件

const   unsigned char   CMD_PQ_AUTOTSTATINF = 0X76; //电能质量统计完成信息
const   unsigned char   CMD_PQ_GETSTAT_FILE = 0X77; //调取统计文件

//调双合自定义文件目录列表
/*
参数 1  目录类型        2字节
参数 2  开始时间        CP56Time2a
参数 3  结束时间		CP56Time2a
*/
//const   unsigned char   CMD_GETSHFILELIST = 0X6C;

//调双合自定义类型文件

//const   unsigned char   CMD_GETSHFILE = 0X6D;
/******************************结构定义***********************************/
//事件结构
struct EventInf
{
unsigned int id;

        // 事件种类，1：稳态事件，2：暂态事件
        //##ModelId=4C43A50F02A2
        short category;

        /*事件类型:
			1 频率越上限、2 频率越下限、3 电压越上限、4 电压越下限、5 三相不平衡度越限、
			6总畸变率越限、7 谐波越限、8 短时闪变越限，9 长时闪变越限  10 电压波动越限
			对于暂态有：11 电压暂降、12电压暂升、13电压短时中断
		*/
        //##ModelId=4C43A53B02DD
        short type;

        // 状态，1：事件开始，2：事件结束
        //##ModelId=4C43A5810302
        short state;

        // 产生事件通道号/线路号
        //##ModelId=4C43A5BC038E
        short channel;

		int  chTyp; //通道号/线路号类型 1 电压， 2 电流 add by kh

        //##ModelId=4C43A681012B
        struct SHTimeStructDAU starttime;

        //##ModelId=4C43A6AA020A
        struct SHTimeStructDAU endtime;

        // 持续时间
        //##ModelId=4C43A6E301C1
        double duration;

        // 事件描述
        //##ModelId=4C43A84E013E
        char describe[64];

        // 开始事件对应的文件名
        //##ModelId=4C43A92303E5
        char startfile[64];

        // 结束事件对应的文件名
        //##ModelId=4C43A94C03D6
        char endfile[64];
};

struct PQTrendFileInf
{
	unsigned short filetype;    //趋势文件子类型 
	char fileName[100];			//文件名称
	int  fileSize;				//文件大小
	struct CP56Time2a ctime;    //文件时间
	struct CP56Time2a etime;    //文件时间
};

struct CommCmdWaveFileInf
{
	char	fileName[40];	//不带扩展名的文件名
	unsigned int 	faultTime;		//故障时间
	unsigned int nFileLen;	//文件长度
};

//双合标记头
struct  SHAuotSendHead
{
	char shflag[2];
	unsigned char typ;
	unsigned short packNo;
};

enum LOG_TYPE { RUN = 1, DEBUG = 2};
enum LOG_LEVEL { LNONE = 0, LERROR = 1, LWARNING = 2, LINFO = 3, LDEBUG = 4 , LFONLY = 5/* 只写文件不显示*/};
#define  LOGSTRLEN   128		//运行日志信息长度

typedef struct RunLogInf
{
	LOG_TYPE   type;	//日志类型
	LOG_LEVEL  level;	//等级
	time_t   time;		//日志记录时间
	unsigned int length;//长度
	char    LogStr[LOGSTRLEN];//日志信息
}RunLogInf;

typedef struct
{
	unsigned int  FrmType;		//帧类型标识，实时数据帧为0x33333331
	unsigned int  FrmLen;		//帧长度，不包含帧头和效验和
	unsigned int  FrmIdx;		//帧序号，从1开始，每发1帧加1
	unsigned short  AnChCnt;		//模拟通道个数
	unsigned short  BinChCnt;	//开关量通道总数，
	unsigned short  ULinCnt;		//电压线路个数
	unsigned short  ILinCnt;		//电流线路个数	
	unsigned short  SmpRate;		//采样率，即每周波点数
	unsigned short  SizePerSample; //每个采样点大小
	unsigned short  PointNum;	//点数（11.25周波）
	unsigned short  Rsev[1];		//预留
}FRM_HEAD_RT_SEND;



typedef struct tagContSearchParm
{
	ST_UINT8 Type;	//查找的数据类型:1 采样数据 2 趋势数据
	ST_INT32 StartTime;	//开始时间
	ST_INT32 Interval;  //抽点间隔,趋势图时用
	ST_INT32 LastTime; //数据长度(秒),趋势图时为点数
	ST_CHAR  FileName[64];//查找的文件名(通过标记事件查找时用)
	ST_UINT8 AnChFlag[12]; //要抽取的模拟通道标志，通道对应bit置1则取，否则不取
	ST_UINT8 BinChFlag[24]; //要抽取的开关通道标志，通道对应bit置1则取，否则不取
}CONTSEARCHPARM;

typedef struct sys_net_cfg
{   
	unsigned char nic_num; 
	unsigned char ipaddr[4];
	unsigned char netmask[4];
	unsigned char gateway[4];
	int port;
	unsigned char ipaddr2[4];
	unsigned char netmask2[4];
	unsigned char gateway2[4];
	int port2;
}SYS_NetCfg;

#pragma pack()
#endif 