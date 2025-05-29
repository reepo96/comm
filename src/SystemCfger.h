// {{{RME classifier 'Logical View::SystemCfger'

#ifndef SystemCfger_H
#define SystemCfger_H

#ifdef PRAGMA
#pragma interface "SystemCfger.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：SystemCfger.h
* 文件标识：
* 摘    要：系统配置类，用于保存系统的配置信息
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/
#include "public.h"
#include <map>
#include <vector>
#include "DeviceIniMgr.h"
#include <CommDataType.h>

using namespace std;

#ifdef _MY_UNITTEST
    class SystemCfger_UT;
#endif

//连接类型：socket连接；com连接；moden连接
typedef enum cfg_link_type{CFG_SOCKET_LINK,CFG_COM_LINK,CFG_MODEN_LINK} CFG_LINK_TYPE;

//协议类型
typedef enum protocol_type{PRO_60_103,PRO_103_HB,PRO_104_GB,PRO_104_ZJ,PRO_61850,PRO_104_SH} PROTOCOL_TYPE;

//DAU相关的一些配置信息
typedef struct tagDAUInfo
{
    char DAUName[51];           //采集装置名称
	char DAUType[52];			//采集装置型号
    bool    isRebuild;          //相关配置是否已被重建
	int		StationId;			//变电站编号
    unsigned int DAUId;        //设备编号
	unsigned int BoardId;
	CFG_LINK_TYPE linkType;     //与此ADU的链接方式
	short BoardType;        //采集板功能类型  1 表示暂态采集， 2 表示稳态采集

    char    addr[16];           //ip地址
    int     tcpPort;            //tcp端口号
    int     udpPort;            //udp端口号

    char    comName[5];         //串口名
    int     baud;               //波特率
    char    parity;             //奇偶校验
    int   dataBit;            //数据位
    int   stop;                 //停止位

    char    phoneNum[20];       //电话号码
	int ProtocalType;
	bool IsActive;
}DAUINFO,*PDAUINFO;

typedef struct tagCfgTimerCall
{
    unsigned char DAUID;       //采集装置ID	
    struct tm	startTime;		//开始时间
    int		intervalTime;		//间隔时间(单位：秒)
    bool	isCallAll;			//是否召唤所有数据
    bool	isCallEventData;	//是否召唤标志事件数据
    bool	isCallFaultData;	//是否召唤故障数据
    bool	isCallLogData;		//是否召唤日志数据
    bool	isCallLowRecData;	//是否召唤低速录波数据
    bool	isCallQualityData;	//是否召唤电能质量数据
    bool	isCallDynaData;		//是否召唤动态记录数据
    bool	isCallConstData;	//是否召唤趋势数据    
    time_t  ttStartTime;		//开始时间,等同于startTime，是另一种表现形式
}CFGTIMERCALL,*PCFGTIMERCALL;

typedef struct tagReportHead *PREPORTHEAD;

// }}}USR
// }}}RME

class SystemCfger
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    friend class SystemCfger_UT;
#endif
		//key为BoardId 
		map< unsigned int, PDAUINFO > DAUInfoMap;

		int iMaxAutoConnDevNum;//通讯模块启动后自动连接设备最多个数
		int iMaxConnDevNum;		//最大连接设备数

		map< unsigned char,PCFGTIMERCALL> DauTimerCallMap;//设备定时召唤信息

		int  logLevel;      //日志级别,0-NONE 1-ERROR 2-WARNING 3-INFO 4-DEBUG
		int  logFileMaxSize;    //log文件最大长度，单位M
		int  udpPortForRltData; //接收DAU实时数据的UDP端口

		CDeviceIniMgr	m_IniMgr;//Ini定值文件的管理

		PROTOCOL_TYPE	rmtProtocolType; //远程通讯协议类型

		char modemComName[ 5 ];//modem使用的串口
		int modemBaud;// modem口波特率
		char modemParity;//modem口奇偶校验
		int modemDataBit;// modem口数据位
		int modemStopBit;//modem口停止位

		map< unsigned int, PDAUINFO > ConstantDAUInfoMap;//稳态设备信息

		int maxLenOfAutoAnaWaveFile;	//自动分析故障录波文件的最大长度(单位：M字节)

		//自动上传故障简报条件
		struct tagAutoSendRptCfg
		{
			bool PSV_UpperLimit;	//故障原因是正序电压上限时自动上传故障简报
			bool PSV_LowerLimit;	//故障原因是正序电压下限时自动上传故障简报
			bool DCB_UpperLimit;	//故障原因是直流突变上限时自动上传故障简报
			bool DCB_LowerLimit;	//故障原因是直流突变下限时自动上传故障简报
			bool F_UpperLimit;//故障原因是频率上限时自动上传故障简报
			bool F_LowerLimit;//故障原因是频率下限时自动下传故障简报
			bool F_ChangeRate;//故障原因是频率变化率时自动上传故障简报
			bool C_UpperLimit;//故障原因是电流上限时自动上传故障简报
			bool C_LowerLimit;//故障原因是电流下限时自动上传故障简报
			bool C_Break;//故障原因是电流突变时自动上传故障简报
			bool C_OverLimit;//故障原因是电流越限时自动上传故障简报
			bool Reverse_power;//故障原因是逆功率启动上传故障简报
			bool ZSACV;//故障原因是零序交流电压上传故障简报
			bool NSACV;//故障原因是负序交流电压上传故障简报
			bool Shock;//故障原因是震荡上传故障简报
			bool SWCH;//故障原因是开关变位上传故障简报
			bool V_Break;//故障原因是电压突变时自动上传故障简报
			bool Fault_InZone;//故障原因是区内故障时自动上传故障简报
			bool Fault_OutZone;//故障原因是区外故障时自动上传故障简报
		} AutoSendRptCfg;

		typedef enum QDReason
		{
			QD_PSV_UPPERLIMIT = 104,//正序电压上限
			QD_PSV_LOWERLIMIT = 105,//正序电压下限
			QD_DCB_UPPERLIMIT = 201,//直流突变上限?????????????
			QD_DCB_LOWERLIMIT = 202,//直流突变上限????????????????
			QD_F_UPPERLIMIT = 101,//频率上限
			QD_F_LOWERLIMIT = 102,//频率下限
			QD_F_CHANGERATE = 103,//频率变化率
			QD_C_UPPERLIMIT,//电流上限????
			QD_C_LOWERLIMIT =202,//电流下限???
			QD_C_BREAK = 203,//电流突变?????
			QD_C_OVERLIMIT = 201,//电流越限
			QD_REVERSE_POWER =407,//逆功率启动
			QD_ZSACV,//零序交流电压????
			QD_NSACV,//负序交流电压????
			QD_SHOCK = 109,//震荡
			QD_SWCH = 206,//开关变位
			QD_V_BREAK,//电压突变

			QD_FAULT_INZONE,//区内故障?????????????
			QD_FAULT_OUTZONE,//区外故障?????????????
		}QDREASON;

		/*
		*根据启动原因号判断能否自动上传故障简报
		*TrgTypeNum：启动原因号
		*返回：true-可以，false-不可以
		*/
		bool IsCandAutoSendRpt(unsigned short TrgTypeNum);

		//初始化ini文件中的配置
		void InitIniFileCfg( void );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    map< string,vector<PREPORTHEAD>*> toSendRmtReport;//待发送到远程的故障简报列表

		void InitOtherCfg();
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute '_instance'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static SystemCfger _instance;
	// }}}RME

public:
	// {{{RME classAttribute 'rmtLinkType'
	// {{{RME general 'documentation'
	/* {{{USR
	与远程通讯的连接方式
	   }}}USR */
	// }}}RME
	CFG_LINK_TYPE rmtLinkType;
	// }}}RME
	// {{{RME classAttribute 'ADULinkType'
	// {{{RME general 'documentation'
	/* {{{USR
	与ADU通讯的连接方式
	   }}}USR */
	// }}}RME
	CFG_LINK_TYPE ADULinkType;
	// }}}RME
	// {{{RME classAttribute 'addrForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	与ADU通讯的IP地址
	   }}}USR */
	// }}}RME
	char addrForADU[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'tcpPortForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	监听ADU连接的端口
	   }}}USR */
	// }}}RME
	int tcpPortForADU;
	// }}}RME
	// {{{RME classAttribute 'udpPortForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	与ADU通讯的udp端口
	   }}}USR */
	// }}}RME
	int udpPortForADU;
	// }}}RME

	//ADD BY KH 
	int tcpPortForADUAutoSend;
	// {{{RME classAttribute 'addrForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	与远程连接的地址
	   }}}USR */
	// }}}RME
	char addrForRemote[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'tcpPortForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	远程监听端口
	   }}}USR */
	// }}}RME
	int tcpPortForRemote;
	// }}}RME
	// {{{RME classAttribute 'udpPortForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	与远程的udp端口
	   }}}USR */
	// }}}RME
	int udpPortForRemote;
	// }}}RME
	// {{{RME classAttribute 'listenNum'
	// {{{RME general 'documentation'
	/* {{{USR
	监听数
	   }}}USR */
	// }}}RME
	int listenNum;
	// }}}RME
	// {{{RME classAttribute 'TransientDAUInfoMap'
	// {{{RME general 'documentation'
	/* {{{USR
	暂态设备信息
	   }}}USR */
	// }}}RME
	map< unsigned int, PDAUINFO > TransientDAUInfoMap;
	// }}}RME
	// {{{RME classAttribute 'comNameForRmt'
	// {{{RME general 'documentation'
	/* {{{USR
	与远程连接的串口
	   }}}USR */
	// }}}RME
	char comNameForRmt[ 5 ];
	// }}}RME
	// {{{RME classAttribute 'baud'
	// {{{RME general 'documentation'
	/* {{{USR
	波特率
	   }}}USR */
	// }}}RME
	int baud;
	// }}}RME
	// {{{RME classAttribute 'parity'
	// {{{RME general 'documentation'
	/* {{{USR
	奇偶校验
	   }}}USR */
	// }}}RME
	char parity;
	// }}}RME
	// {{{RME classAttribute 'dataBit'
	// {{{RME general 'documentation'
	/* {{{USR
	数据位
	   }}}USR */
	// }}}RME
	int dataBit;
	// }}}RME
	// {{{RME classAttribute 'stopBit'
	// {{{RME general 'documentation'
	/* {{{USR
	停止位
	   }}}USR */
	// }}}RME
	int stopBit;
	// }}}RME
	// {{{RME classAttribute 'phoneNumForRmt'
	// {{{RME general 'documentation'
	/* {{{USR
	与远程连接的电话号码
	   }}}USR */
	// }}}RME
	char phoneNumForRmt[ 20 ];
	// }}}RME
	// {{{RME classAttribute 'timeOut'
	// {{{RME general 'documentation'
	/* {{{USR
	设定的超时时间(单位：毫秒)
	   }}}USR */
	// }}}RME
	int timeOut;
	// }}}RME
	// {{{RME classAttribute 'DataFilePath'
	// {{{RME general 'documentation'
	/* {{{USR
	数据文件存放目录
	   }}}USR */
	// }}}RME
	char DataFilePath[ 255 ];
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateCopyConstructor'
	SystemCfger( const SystemCfger & rtg_arg );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateAssignmentOperator'
	SystemCfger & operator=( const SystemCfger & rtg_arg );
	// }}}RME


protected:
	// {{{RME operation 'SystemCfger()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	SystemCfger( void );
	// }}}RME

public:
	// {{{RME operation '~SystemCfger()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~SystemCfger( void );
	// }}}RME
	// {{{RME operation 'GetInstance()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static SystemCfger & GetInstance( void );
	// }}}RME
	// {{{RME operation 'Rebuild(int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Rebuild
	    Description:    重新获取配置
	    Calls:          
	    Called By:      
	    Input:          type：重建类型，1：重建远程相关配置；2：重建ADU相关配置；3：其它与通讯无关的配置
	                    
	                    
	    Output:                             
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Rebuild( int type );
	// }}}RME
	// {{{RME operation 'Init()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Init
	    Description:    初始化
	    Calls:          
	    Called By:      
	    Input:          
	                    
	                    
	    Output:                             
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Init( void );
	// }}}RME

protected:
	// {{{RME operation 'InitDauInfo()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitDauInfo( void );
	// }}}RME
	// {{{RME operation 'InitSysCfg()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitSysCfg( void );
	// }}}RME
	// {{{RME operation 'InitTimerCallInf()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitTimerCallInf( void );
	// }}}RME

public:
	// {{{RME operation 'GetDAUInfo(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDAUInfo
	    Description:    根据DAU编号获得DAU的信息  停用该函数2010-10-18
	    Calls:          
	    Called By:      
	    Input:          DAUId：设备编号
	                    devType:设备类型，1-暂态设备,2-稳态设备
	                    
	    Output:                             
	    Return:         成功返回DAU的信息，否则返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	PDAUINFO GetDAUInfo( unsigned int DAUId, int devType );
	// }}}RME
	// {{{RME operation 'GetDAUInfo(char[16],int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDAUInfo
	    Description:    根据ip地址获得DAU的信息  停用该函数2010-10-18
	    Calls:          
	    Called By:      
	    Input:          DAUId：设备编号
	                    devType:设备类型，1-暂态设备,2-稳态设备
	                    
	    Output:                             
	    Return:         成功返回DAU的信息，否则返回NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	PDAUINFO GetDAUInfo( const char *ipAddr, int devType );
	// }}}RME

protected:
	// {{{RME operation 'RptStruct2CmdReport(RptStruct *,PREPORTHEAD)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       RptStruct2CmdReport
	    Description:    从数据库简报分析格式转到协议简报格式
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	bool RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport );
	// }}}RME

public:
	PDAUINFO GetDAUInfo( const char *ipAdd );
	PDAUINFO GetDAUInfo( unsigned int BoardId);

	/*************************************************
	    Function:       InitDauInfo2
	    Description:    根据设备id初始化设备信息，若id为0则初始全部信息
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************/
	void InitDauInfo2(unsigned int paraDevId=0);

	// {{{RME operation 'InitReportList(string)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       InitReportList
	    Description:    根据远程ip初始化故障简报列表
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void InitReportList( string rmtIP );
	// }}}RME
	// {{{RME operation 'GetReportList(string)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportList
	    Description:    根据远程ip获得故障简报列表
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	vector< PREPORTHEAD > * GetReportList( string rmtIP );
	// }}}RME
	// {{{RME operation 'AppendReportList(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AppendReportList
	    Description:    根据设备id和文件id添加故障报告信息
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void AppendReportList( unsigned int devId, int fileId );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SystemCfger_H */

// }}}RME
