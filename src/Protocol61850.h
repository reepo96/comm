/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol61850.h
* 文件标识：
* 摘    要：解析61850协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2009年5月9日
*
**************************************************************************/

#ifndef Protocol61850_H
#define Protocol61850_H

#include "protocol.h"
#include "public.h"
#include "Command.h"
#include "shglobal.h"
#include "acsi.h"
#include "dfr_rserver.h"
#include "req.h"
#include "FileControl.h"

class Protocol61850 :public Protocol
{
public:
	Protocol61850(const char *pCfgFile,const char *pDeviceName,const char *pARName);
public:
	~Protocol61850(void);

protected:
	const char	*m_pDeviceName;		//设备名称
	//文件路径
	char RES_COMTRADE_DIR[100];	//comtrade文件目录
	char RES_COMTRADE_DIV_DIR[100];	//comtrade分文件目录
	char RES_SHD_DIR[100];		//shd文件目录
	char RES_MARKENV_DIR[100];	//标记事件文件目录
	char RES_CONFIG_DIR[100];	//配置相关文件目录
	char RES_REPORT_DIR[100];	//简报文件目录
	char RES_PROGRAM_DIR[100];	//升级程序文件目录
	char RES_LOG_DIR[100];
	char RES_PQTREND_DIR[100];
	char RES_PQEVENT_DIR[100];

	//启动录波控制属性
	char RES_STARTREC_CTRL[64];
	//设备状态(运行、调试)
	char RES_DEVSTATE_CTRL[64];
	//设备操作(告警复归、启动复归、自检)
	char RES_DEVOPER_CTRL[64]; 
	int i;

protected:
	SHDFR_B_RemoteServer	*m_pIEC61850_Adaptor;//61850协议适配器
	bool					m_bAdaptorHasIni;	//61850协议适配器已经初始化

	bool	m_bCanSendRltData;			//能否发送实时数据给主界面
	bool	m_bCanSendZeroOffsetData;	//能否发送零漂数据给主界面
	static map<unsigned int ,bool>	m_RltEnableList;		//可发送实时数据的设备列表
	static map<unsigned int ,bool>	m_ZeroOffsetEnableList; //可发送零漂数据的设备列表
	static CRITICAL_SECTION	*m_pMapSection;
public:

	bool Connect();
	bool Close();

	/*************************************************
	    Function:		Decode
	    Description:	解析通过通讯传入的数据，生成一个任务对象（在本协议仅作侦测链接是否正常）
	    Calls:
	    Called By:
	    Input:			pLinker：通讯连接

	    Output:
	    Return:			成功返回任务，失败返回NULL
	    *************************************************/
	Task * Decode( Linker * pLinker );

	/**************************************************
	    Function:		ExecuteCmd
	    Description:	执行命令
	    Calls:
	    Called By:
	    Input:			pLinker：通讯连接
						pCmd：要执行的命令

	    Output:
	    Return:			成功返回true，失败返回false
	    *************************************************/
	bool ExecuteCmd( Linker * pLinker, struct Command & cmd );

	/**************************************************
	    Function:       GetDauInf
	    Description:    查询录波器运行状态信息
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    **************************************************/
	bool GetDauInf( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       GetRecWaveFile
	    Description:    召唤录波文件
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       GetRecWaveFileList
	    Description:    召唤录波文件列表
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       SetTime
	    Description:    对时（时间同步）
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool SetTime( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       GetTime
	    Description:    获取时间
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool GetTime( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       StartRecByManual
	    Description:    手动启动录波
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool StartRecByManual( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		StopRecWave
	Description:	停止录波
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool StopRecWave( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       GetFileList
	    Description:    文件列表
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool GetFileList( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       PullFile
	    Description:    获取文件
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool PullFile( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       PushFile
	    Description:    下传文件
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool PushFile( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       ReadRltData
	    Description:    读实时数据，实时显示用
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool ReadRltData( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		ReadZeroOffset
	Description:	读实零漂刻度
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool ReadZeroOffset( Linker * pLinker, struct Command * pCmd );

	/**************************************************
	    Function:       AutoRltData
	    Description:    处理实时数据的回调函数
	    Calls:
	    Called By:
	    Input:
	                    

	    Output:
	    Return:
	    *************************************************/
	static void AutoRltData(SHDFR_B_RemoteServer *pServer, void* data, int data_len );

	/**************************************************
	    Function:       AutoMarkEvt
	    Description:    处理自动上传的标志事件
	    Calls:
	    Called By:
	    Input:          
	                    

	    Output:
	    Return:
	    *************************************************/
	static void AutoMarkEvt(SHDFR_B_RemoteServer *pServer, void* data, int data_len );

	/**************************************************
	    Function:       AutoStartInf
	    Description:    处理自动上传的启动信息
	    Calls:
	    Called By:
	    Input:          
	                    

	    Output:
	    Return:
	    *************************************************/
	static void AutoStartInf(SHDFR_B_RemoteServer *pServer, void* data, int data_len );

	/*************************************************
	Function:		GetContData
	Description:	读动态记录数据
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool GetContData( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		SetDeviceState
	Description:	设置装置状态
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:         成功返回true，失败返回false
	*************************************************/
	bool SetDeviceState( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		OperateDevice
	Description:	硬件操作
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool OperateDevice( Linker * pLinker, struct Command * pCmd );

protected:
	/**************************************************
	    Function:       GetFileFromDirNode
	    Description:    从目录节点中获取文件信息
	    Calls:
	    Called By:
	    Input:          pCmd：要执行的命令
						pDirTree：树
	                    pDirNode：目录节点
						bNeedCpyExt：是否需要拷贝扩展名
						pFileExtNameList：要获取的文件扩展名列表，NULL表示所有文件

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************/
	bool GetFileFromDirNode( struct Command * pCmd,FileDirTree *pDirTree,FileDirNode *pDirNode,bool bNeedCpyExt,vector<const char *> *pFileExtNameList );

	/**************************************************
	    Function:       FindFileInStr
	    Description:    查找文件节点中的录波文件名是否在所有录波文件名组成的串中
	    Calls:
	    Called By:
	    Input:          devID：设备id
						pDirNode：目录节点
	                    pAllFileNames：目录节点
						bNeedCpyExt：所有文件名组成的串
	    Output:			pNoInFileList：存放不存在的文件节点
	    Return:         无
	    *************************************************/
	void FindFileInStr(unsigned char devID, FileDirNode *pDirNode,const char *pAllFileNames,vector< CMDFILELIST > *pNoInFileList );

	/**************************************************
	    Function:       GetRmtFileDirAndFileType
	    Description:    根据文件类型(字符)，返回对应远程目录及此文件类型的枚举值
	    Calls:
	    Called By:
	    Input:          ucType：字符表示的文件类型						

	    Output:			fileControlType：返回的文件类型的枚举值
	    Return:			对应远程目录，没有则返回""
	    *************************************************/
	string GetRmtFileDirAndFileType(const unsigned char ucType , FileControl::FILETYPE & fileControlType );

	/**************************************************
	    Function:       GetFile
	    Description:    调取文件
	    Calls:
	    Called By:
	    Input:          pCmd：要执行的命令
						locFile：本地文件名
	                    rmtFile：远程文件名

	    Output:
	    Return:
	    *************************************************/
	int GetFile(struct Command * pCmd, const char *locFile,const char *rmtFile );

	/**************************************************
	    Function:       PutFile
	    Description:    下传文件
	    Calls:
	    Called By:
	    Input:          pCmd：要执行的命令
						locFile：本地文件名
	                    rmtFile：远程文件名

	    Output:
	    Return:
	    *************************************************/
	int PutFile(struct Command * pCmd, const char *locFile,const char *rmtFile );

	/**************************************************
	    Function:       PutOrGetFileInf
	    Description:    下传/调取文件过程信息
	    Calls:
	    Called By:
	    Input:          pCmd：要执行的命令
						iType：类型，1-下传文件，2-调取文件
						req：请求对象
	                    pFileAttr：文件属性

	    Output:
	    Return:成功返回0，其它值为错误
	    *************************************************/
	int PutOrGetFileInf(struct Command * pCmd, int iType,const RequestPtr *pReqPtr );

	/**************************************************
	    Function:       AnaErrNo
	    Description:    分析61850协议适配器相关函数返回值是否表示错误
	    Calls:
	    Called By:
	    Input:          errorno：错误号
						pLinker：连接对象
	    Output:
	    Return:true：有错误，false：无错误
	    *************************************************/
	bool IsError( int errorno,Linker * pLinker );

	/**************************************************
	    Function:       WriteAutoData2Mem
	    Description:    写自动上传数据到共享内存中
	    Calls:
	    Called By:
	    Input:          pRmtIpAddr：远程ip
						funcode：功能码
						resultType：结果类型
	    Output:
	    Return:true：有错误，false：无错误
	    *************************************************/
	static void WriteAutoData2Mem( const char *pRmtIpAddr,unsigned char funcode,CMDRESULTTYPE resultType,const char *data,int data_len );

public:
	//查询录波器运行状态信息
	static bool CallGetDauInf(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//召唤录波文件
	static bool CallGetRecWaveFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//召唤录波文件列表
	static bool CallGetRecWaveFileList(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//召唤通用文件列表
	static bool CallGetFileList(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//对时（时间同步）
	static bool CallSetTime(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//获取时间
	static bool CallGetTime(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//停止录波
	static bool CallStopRecWave(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//手动启动录波
	static bool CallStartRecByManual(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//读动态纪录数据
	static bool CallGetContData(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//获取文件
	static bool CallPullFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//下传文件
	static bool CallPushFile(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//读实时数据
	static bool CallReadRltData(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//读实零漂刻度
	static bool CallReadZeroOffset(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//设置设备状态
	static bool CallSetDeviceState(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);

	//告警复归
	static bool CallOperateDevice(Protocol *pProtocol,Linker * pLinker,struct Command& cmd);
};

#endif /* Protocol61850_H */