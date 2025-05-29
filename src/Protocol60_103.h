// {{{RME classifier 'Logical View::Protocol60_103'

#ifndef Protocol60_103_H
#define Protocol60_103_H

#ifdef PRAGMA
#pragma interface "Protocol60_103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol103.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol60_103.h
* 文件标识：
* 摘    要：解析60_103协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"
#include "DataControl.h"
#include "FileControl.h"

// }}}USR
// }}}RME

class Protocol60_103 : public Protocol103
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	friend DWORD WINAPI DelUdpDataThread(void *pParm);

	typedef struct SHQDFlagWord
	{
		unsigned char SerialNoOfInput;
		bool    IsSimulate:1;   //输入量类型，1：模拟量，0：开关量
		unsigned char DeviceType:4; //所属设备类型，1：母线电压；2：电流线路；3：变压器；4：发电机；5：高频；6：直流
									//7：保护；8：断路器
		unsigned char ActType:3;    //动作类型，000：无效；001：越上限；010： 越下限；011：变化率；100：信号；101：开关闭合；111：开关断开
		unsigned short StartVal:5;   //启动量，0000：有效值；0001：正序；0010：负序；0011：零序；0101：振荡；0110：无功功率
									//1000：频率；1001：差流；1010：过激磁；1011：非周期量；1100：穿越电流；1101：逆功率
		unsigned short Reserve:11;
	}SHQDFLAGWORD;

	typedef struct SHQDMessage
	{
		unsigned char State;        //状态，1：启动；2：录波；3：结束
		unsigned int DeviceId;      //设备id
		unsigned int MarkId;		//事件序号
		SHTimeStructDAU StartTime;
		SHQDFLAGWORD StartFlag;  //启动原因
		unsigned int ChiefDevNo;    //一次设备编号
		float CurrentVal;			//采样值
		unsigned int HasRecDataLen; //已记录数据长度，单位字节
		SHTimeStruct EndTime;	   //越限结束时间
		char FileName[100];        //文件名或文件号

		int GetMarkType()
		{
			return StartFlag.StartVal;
		}

		string GetMarkTypeDesc()
		{
			char szDesc[64] = {0};
			string sMarkTypeDesc("");

			if(StartFlag.IsSimulate == false)//开关
			{
				switch(StartFlag.ActType)
				{
				case 5:
					return string("开关闭合");
				case 7:
					return string("开关断开");
				default:
					return string("");
				}
			}
			else //模拟量
			{
				switch(StartFlag.StartVal)
				{
				case 0:
					sMarkTypeDesc.append("有效值");
					break;
				case 1:
					sMarkTypeDesc.append("正序");
					break;
				case 2:
					sMarkTypeDesc.append("负序");
					break;
				case 3:
					sMarkTypeDesc.append("零序");
					break;
				case 4:
					sMarkTypeDesc.append("开关");
					break;
				case 5:
					sMarkTypeDesc.append("振荡");
					break;
				case 6:
					sMarkTypeDesc.append("无功功率");
					break;
				case 7:
					sMarkTypeDesc.append("有功功率");
					break;
				case 8:
					sMarkTypeDesc.append("频率");
					break;
				case 9:
					sMarkTypeDesc.append("差流");
					break;
				case 10:
					sMarkTypeDesc.append("过激磁");
					break;
				case 11:
					sMarkTypeDesc.append("非周期量");
					break;
				case 12:
					sMarkTypeDesc.append("穿越电流");
					break;
				case 13:
					sMarkTypeDesc.append("逆功率");
					break;
				case 14:
					sMarkTypeDesc.append("功率因素");
					break;
				case 15:
					sMarkTypeDesc.append("总功率");
					break;
				case 16:
					sMarkTypeDesc.append("突变");
					break;
				default:
					if(StartFlag.StartVal >16 && StartFlag.StartVal <=31)
					{
						char szTmp[10]={0};
						sprintf(szTmp,"%d次谐波",StartFlag.StartVal - 16);
						sMarkTypeDesc.append(szTmp);
					}
					else
					{
						sMarkTypeDesc.append("");
					}
					break;
				}//end switch(StartFlag.StartVal)

				switch(StartFlag.ActType)
				{
				case 0:
					sMarkTypeDesc.append("");
					break;
				case 1:
					sMarkTypeDesc.append("越上限");
					break;
				case 2:
					sMarkTypeDesc.append("越下限");
					break;
				case 3:
					sMarkTypeDesc.append("变化率");
					break;
				case 4:
					sMarkTypeDesc.append("信号");
					break;
				case 5:
					sMarkTypeDesc.append("开关闭合");
					break;
				case 7:
					sMarkTypeDesc.append("开关断开");
					break;
				default:
					sMarkTypeDesc.append("");
					break;
				}//end switch(StartFlag.ActType)

				return sMarkTypeDesc;
			}//end 模拟量

			return string("");
		}

	}SHQDMESSAGE,*PSHQDMESSAGE;

	//处理UDP信息的结构信息
	typedef struct tagUdpDataParm
	{
		Protocol60_103 *pProto;
		struct sockaddr_in *newAddrin;
		int datalen;
		char *pData;
	}UDPDATAPARM,*PUDPDATAPARM;

	/*************************************************
	Function:		GetCmdDesc
	Description:	获取命令描述信息
	Calls:
	Called By:
	Input:			cmd：要执行的命令

	Output:
	Return:
	*************************************************/
	void GetCmdDesc(struct Command & cmd);

	static string GetCmdDesc(unsigned char funCode);

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

	/*************************************************
	Function:		StartGnExperiment
	Description:	开始发电机实验
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool StartGnExperiment( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		StopGnExperiment
	Description:	停止发电机实验
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool StopGnExperiment( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		GetDeviceState
	Description:	读装置状态
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	bool GetDeviceState( Linker * pLinker, struct Command * pCmd );

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

	/*************************************************
	Function:		GetSplitRecWaveFile
	Description:	召唤分通道录波文件
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令

	Output:
	Return:			成功返回true，失败返回false
	*************************************************/
	virtual bool GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	map<unsigned int,MARKEVENT> m_MarkEventMap;//保存自动上传的标记事件列表

	/*************************************************
	Function:		SendAsdu10WithExeWrite
	Description:	发送ASDU10带执行的写。
	Calls:
	Called By:
	Input:			pLinker：通讯连接
	pCmd：			要执行的命令
					asdu10_GIs：asdu10命令所使用的GI结构数组
					asdu10_GINumber：asdu10_GIs数组个数
	Output:

	Return:			-1：异常，通常是通讯上的异常，需要关闭连接
					0：获取失败，发送错误的指令，或者没有数据
					1：获取成功
	*************************************************/
	int SendAsdu10WithExeWrite( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber);

	/*************************************************
	Function:		GetGeneraFile
	Description:	获取通用文件。
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令
					FilePartName：不含路径的文件名，如果为NULL,则需分析前台返回的文件名
					fileTotalLen:文件总长度
					fileControlType：文件类型
					YearMonth：文件存放的年月目录(可以为NULL)
					asdu10_GIs：asdu10命令所使用的GI结构数组
					asdu10_GINumber：asdu10_GIs数组个数
					asdu21_GIs：asdu21命令所使用的GI结构数组
					asdu21_GINumber：asdu21_GIs数组个数
					IsNeedCompress:是否需要压缩，0：不要、1：要
	Output:

	Return:			false：获取失败
					true：获取成功
	*************************************************/
	bool GetGeneraFile( Linker * pLinker,struct Command *pCmd,char *FilePartName,int fileTotalLen,FileControl::FILETYPE fileControlType,char *YearMonth, GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber,char IsNeedCompress=0);

	/*************************************************
	Function:		GetGeneraFile
	Description:	获取通用文件(数据写入内存)
	Calls:
	Called By:
	Input:			pLinker：通讯连接
					pCmd：要执行的命令
					pDataQue：保存数据的队列
					asdu10_GIs：asdu10命令所使用的GI结构数组
					asdu10_GINumber：asdu10_GIs数组个数
					asdu21_GIs：asdu21命令所使用的GI结构数组
					asdu21_GINumber：asdu21_GIs数组个数
	Output:

	Return:			false：获取失败
					true：获取成功
	*************************************************/
	bool GetGeneraFile( Linker * pLinker,struct Command *pCmd,DataControl::DataQue *pDataQue,GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber);

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	Protocol60_103( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol60_103( void );
	// }}}RME
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
	bool GetDauInf( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'PreGetFile(Linker *,struct Command*,int,char*,int,unsigned int,unsigned char)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PreGetFile
	    Description:    获取文件前期处理工作
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    type：获取的文件类型：1-录波文件；2-一般文件
	                    pFileName：文件名
	                    startPos：文件起始位置
	                    recWaveType：录波文件类型。只有在type=1的情况下有效
	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool PreGetFile( Linker * pLinker, struct Command * pCmd, int type, char * pFileName, int fileNameLen, unsigned int startPos, unsigned char recWaveType = 0 );
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
	bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
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
	bool GetTime( Linker * pLinker, struct Command * pCmd );
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
	// {{{RME operation 'StartRec(Linker *,struct Command*,GI[])'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       StartRec
	    Description:    启动录波
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    GIs：GI结构
	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool StartRec( Linker * pLinker, struct Command * pCmd, GI GIs[] );
	// }}}RME
	// {{{RME operation 'GetGIAttrData(Linker *,struct Command *,GI[],unsigned char,GI[],unsigned char,char*,int*,int*,unsigned char*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetGIAttrData
	    Description:    获得通用分类服务返回的属性结构数据。
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令
	                    asdu10_GIs：asdu10命令所使用的GI结构数组
	                    asdu10_GINumber：asdu10_GIs数组个数
	                    asdu21_GIs：asdu21命令所使用的GI结构数组
	                    asdu21_GINumber：asdu21_GIs数组个数
	    Output:         outAttrData：输出的属性结构数据数组
	                    outAttrLen：输出的属性结构数据长度数组
						outAttrNum：输出属性个数
	                    outRII：输出的RII
	    Return:         -1：异常，通常是通讯上的异常，需要关闭连接
	                        0：获取失败，发送错误的指令，或者没有数据
	                        1：获取成功
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetGIAttrData( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber, GI asdu21_GIs[], unsigned char asdu21_GINumber, char * outAttrData[], int * outAttrLen[], int * outAttrNum, unsigned char * outRII = NULL );
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
	bool GetFileList( Linker * pLinker, struct Command * pCmd );
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
	    Description:    读实时数据，实时显示用
	    Calls:
	    Called By:
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令

	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool ReadRltData( Linker * pLinker, struct Command * pCmd );
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
	bool AutoSend( Linker * pLinker, struct Command * pCmd );
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
	bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
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
	bool GetUdpData( Linker * pLinker, struct Command & cmd );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol60_103_H */

// }}}RME
