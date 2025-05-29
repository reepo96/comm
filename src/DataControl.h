// {{{RME classifier 'Logical View::DataControl'

#ifndef DataControl_H
#define DataControl_H

#ifdef PRAGMA
#pragma interface "DataControl.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：DataControl.h
* 文件标识：
* 摘    要：数据控制类，用于数据的管理
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月12日
*
**************************************************************************/

#include "public.h"
#include "Command.h"
#include <stdio.h>
#include <direct.h>
/*#include "ReportManager.h"
#include "FaultAnalyzer.h"
#include <ReportFileHead.h>
#include <Report.h>*/
#include <vector>
using namespace std;

// }}}USR
// }}}RME

class DataControl
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	//录波文件信息
	typedef struct tagFileInfo
	{
		unsigned char devID;        //文件录波装置地址
		char fileName[40];          //文件名称（不含扩展名）
		struct CP56Time2a faultTime;//文件故障时间
		unsigned int fileLen;		//文件大小
	}FILEINFO,*PFILEINFO;

	//浙江录波简报格式
	typedef struct tagZJReport
	{
		char recWaveFileName[40];       //不含扩展名的录波文件名
		char BASName[40];               //故障间隔名

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
		unsigned char DPI;			//重合闸
		unsigned char DPI_TimeL;	//重合闸时间毫秒低字节
		unsigned char DPI_TimeH;	//重合闸时间毫秒高字节

		struct CP56Time2a faultTime;    //故障时间
	}ZJREPORT,*PZJREPORT;


	//管理数据队列
	class DataQue
	{
	protected:
		CRITICAL_SECTION criticalSection;

		int startPos;	//有效数据起始位置
		int endPos;		//有效数据结束位置
		int bufLen;

		char *pData;
		bool bIsCancle; //是否退出队列

	public:
		DataQue(int iSize);
		~DataQue();

		void Cancle()
		{
			bIsCancle = true;
		}

		bool IsCancle()
		{
			return bIsCancle;
		}

		//数据入队列直到成功，除非*pIsStop = true
		void push(const char* pSrcData,int iDataLen,bool *pIsStop);

		//数据出队列
		//返回实际出队列的个数
		int pop(char *pDestBuf,int iBufLen);

		bool IsEmpty();
		bool IsFull();
	};

	static DataQue* CreateDataQue(int iSize);

	//故障简报管理类
	class CAnalyReportManager
	{
	public:
		//*********************************************
		//Function:   BOOL WriteReportTXT( CHAR * pstrFileName )
		//Description:生成故障报告文本文件.TXT
		//Input:      CHAR * pstrFileName：文件名
		//Output:     无
		//Return:     BOOL：成功为True，失败为False
		//*********************************************
		//##ModelId=479C018B0255
		//BOOL WriteReportTXT( const CHAR * pstrFileName ,ReportFileHead rptHead, Report * rpt);

	private:
		//void WriteLineFaultXml1(FILE *fp,Report rpt);
		//*********************************************
		//Function:   BOOL WriteLineFault( FILE * fp, Report rpt )
		//Description:写一条故障记录到文件fp中
		//Input:      FILE * fp：文件指针，Report
		//rpt：故障报告记录
		//Output:     无
		//Return:     BOOL：成功为True，失败为False
		//*********************************************
		//##ModelId=479C018B0282
		//BOOL WriteLineFault( FILE * fp, Report rpt );
		//*********************************************
		//Function:   void GetTripPhaseName( WORD wTripPhase,
		//CHAR * pstrName )
		//Description:得到跳闸相别对应的名称
		//Input:      WORD wTripPhase：跳闸相别
		//Output:     CHAR * pstrName：跳闸相别名称
		//Return:     无
		//*********************************************
		//##ModelId=479C018B0292
		void GetTripPhaseName( WORD wTripPhase, CHAR * pstrName );
		//*********************************************
		//Function:   void GetFaultTypeName( WORD wFaultType,
		//CHAR * pstrName )
		//Description:得到故障类型对应的名称
		//Input:      WORD wFaultType：故障类型
		//Output:     CHAR * pstrName：故障类型名称
		//Return:     无
		//*********************************************
		//##ModelId=479C018B02A1
		void GetFaultTypeName( WORD wFaultType, CHAR * pstrName );
	public:
		//void WriteXmlType1(FILE *fp, ReportFileHead rptHead, Report * rpt);
		//*********************************************
		//Function:   CAnalyReportManager();
		//Description:构造函数，初始化
		//Input:      无 
		//Output:     无
		//Return:     无
		//*********************************************
		//##ModelId=479C018B02B1
		CAnalyReportManager();
		//*********************************************
		//Function:   ~CAnalyReportManager();
		//Description:析构函数
		//Input:      无 
		//Output:     无
		//Return:     无
		//*********************************************
		//##ModelId=479C018B02BF
		virtual ~CAnalyReportManager();
	};

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	/*************************************************
	    Function:       StringTime2CP56Time2a
	    Description:    将字符串时间转换成CP56Time2a时间
	    Calls:
	    Called By:
	    Input:          pStringTime:字符串时间，格式yyyy-mm-dd HH:MM:ss.sss
						
	    Output:         destTime：转换后的时间
	    Return:
	    *************************************************/
	void StringTime2CP56Time2a(const char *pStringTime,struct CP56Time2a& destTime);

	/*************************************************
	    Function:       RptStruct2CmdReport
	    Description:    从数据库简报分析格式转到协议简报格式
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return
	    *************************************************/
	bool RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport );

	/*************************************************
	    Function:       AnalyzResult2CmdReport
	    Description:    从故障分析结果转到协议简报格式
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return
	    *************************************************/
	//bool AnalyzResult2CmdReport( FaultAnalyzer& analyzer, PREPORTHEAD pFaultReport );

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	DataControl( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~DataControl( void );
	// }}}RME
	// {{{RME operation 'GetRecWaveFileList(int,int,struct CP56Time2a,struct CP56Time2a,vector<FILEINFO>&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    获得录波文件列表
	    Calls:
	    Called By:
	    Input:          devId:设备id，-1表示所有设备
						startTime：查询开始时间
	                    endTime：查询结束时间
						type:1-只查询有故障的录波文件列表,2-查询所有的录波文件列表
	    Output:         fileList：保存返回的文件列表
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetRecWaveFileList( int devId, int type, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< FILEINFO > & fileList );
	// }}}RME
	// {{{RME operation 'GetMarkecEventList(int,struct CP56Time2a,struct CP56Time2a,vector<MarkedEvtStruct>&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetMarkecEventList
	    Description:    获得标记事件列表列表
	    Calls:
	    Called By:
	    Input:          devId:设备id，-1表示所有设备
						startTime：查询开始时间
	                    endTime：查询结束时间
	    Output:         fileList：保存返回的标记事件列表
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetMarkecEventList( int devId, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< MarkedEvtStruct > & markedEventList );
	// }}}RME
	// {{{RME operation 'GetReportByFileName_ZJ(const int,const string,char **,int*,unsigned short&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportByFileName_ZJ
	    Description:    根据录波文件名获取故障简报(浙江)
	    Calls:
	    Called By:
	    Input:          devId：设备id
						sFileName:不带扩展名的录波文件路径名
						ppReportData：故障简报数据
						TrgTypeNum：启动原因
	    Output:
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	//bool GetReportByFileName_ZJ( const int devId, const string sFilePartName, char * * ppReportData, int * piReportDataLen, unsigned short & TrgTypeNum );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* DataControl_H */

// }}}RME
