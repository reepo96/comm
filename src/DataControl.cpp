// {{{RME classifier 'Logical View::DataControl'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "DataControl.h"
#endif

#include <RTSystem/Communication.h>
#include <DataControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：DataControl.cpp
* 文件标识：
* 摘    要：数据控制类，用于数据的管理
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月12日
*
**************************************************************************/

#include "FileControl.h"
//#include "FaultAnalyzer.h"
//#include "ReportManager.h"
#include "LogControl.h"
//#include "analysecenter.h"
//#include "BasicAlgorithm.h"
#include <algorithm>
using namespace std;

extern LogControl mylog;

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
DataControl::DataControl( void )
{
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
DataControl::~DataControl( void )
{
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(int,int,struct CP56Time2a,struct CP56Time2a,vector<FILEINFO>&)'
bool DataControl::GetRecWaveFileList( int devId, int type, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< FILEINFO > & fileList )
{
	// {{{USR
	char szStartTime[25] = {0};
	char szEndTime[25] = {0};
	char szDevIdCondition[64]={0};
	char szSql[255] = {0};
	char szFilePath[256] = {0};
	FileControl fileCtrl;

	sprintf(szStartTime,"%d-%02d-%02d %02d:%02d:%02d",startTime.year+2000,startTime.month,startTime.day,
		startTime.hours,startTime.minute,startTime.milliseconds/1000);

	sprintf(szEndTime,"%d-%02d-%02d %02d:%02d:%02d",endTime.year+2000,endTime.month,endTime.day,
		endTime.hours,endTime.minute,endTime.milliseconds/1000);

	if(devId != -1)
	{
		sprintf(szDevIdCondition,"SHRpt.DAUID=%d and",devId);
	}

	if(type == 1)//只查询有故障的录波文件列表
	{
		sprintf(szSql,"select SHRpt.*,SHDAU.DAUName from SHRpt,SHDAU where SHRpt.DAUID=SHDAU.DAUID and %s FaultTime>='%s' and FaultTime<='%s' and FaultType<13",
			szDevIdCondition,szStartTime,szEndTime);
	}
	else//查询所有的录波文件列表
	{
		sprintf(szSql,"select * from SHRpt where %s FaultTime>='%s' and FaultTime<='%s'",
			szDevIdCondition,szStartTime,szEndTime);
	}

	if(getRptBySql >0 )
	{
		RptStruct *RptInfo = NULL;
		getRptBySql(szSql,&RptInfo);//从数据库查找

		while(RptInfo != NULL)
		{
			FILEINFO fileInfo;
			memset(&fileInfo,0,sizeof(fileInfo));

			fileInfo.devID = RptInfo->DAUID;
			StringTime2CP56Time2a(RptInfo->FaultTime,fileInfo.faultTime);

			//文件名
			string sFileName(RptInfo->RptFileName);
			string::size_type pos = sFileName.find(".cfg");
			if(pos == string::npos)
			{
				pos = sFileName.find(".CFG");
			}

			if(pos != string::npos)
			{
				sFileName = sFileName.substr(0,pos);
			}
			memcpy(fileInfo.fileName,sFileName.c_str(),min(sizeof(fileInfo.fileName),sFileName.size()));

			//文件长度
			string sFilePath(RptInfo->FilePath);
			sFilePath.append(sFileName);

			unsigned int fileLen = fileCtrl.GetFileLen(sFilePath.append(".cfg").c_str());
			fileLen += fileCtrl.GetFileLen(sFilePath.append(".dat").c_str());
			fileLen += fileCtrl.GetFileLen(sFilePath.append(".hdr").c_str());
			fileLen += fileCtrl.GetFileLen(sFilePath.append(".inf").c_str());

			fileInfo.fileLen = fileLen;
			fileList.push_back(fileInfo);

			RptStruct *pTmpRptInfo = RptInfo;
	        RptInfo = pTmpRptInfo->next;
	        releaseMemery((void*)pTmpRptInfo);
	        pTmpRptInfo = NULL;
		}
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetMarkecEventList(int,struct CP56Time2a,struct CP56Time2a,vector<MarkedEvtStruct>&)'
bool DataControl::GetMarkecEventList( int devId, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< MarkedEvtStruct > & markedEventList )
{
	// {{{USR
	char szStartTime[25] = {0};
	char szEndTime[25] = {0};
	char szDevIdCondition[64]={0};
	char szSql[255] = {0};
	char szFilePath[256] = {0};
	FileControl fileCtrl;

	sprintf(szStartTime,"%d-%02d-%02d %02d:%02d:%02d",startTime.year+2000,startTime.month,startTime.day,
		startTime.hours,startTime.minute,startTime.milliseconds/1000);

	sprintf(szEndTime,"%d-%02d-%02d %02d:%02d:%02d",endTime.year+2000,endTime.month,endTime.day,
		endTime.hours,endTime.minute,endTime.milliseconds/1000);

	if(devId != -1)
	{
		sprintf(szDevIdCondition,"DAUID=%d and",devId);
	}

	sprintf(szSql,"select * from SHMarkedEvt where '%s' MarkedEvtTime>='%s' and MarkedEvtTime<='%s'",
			szDevIdCondition,szStartTime,szEndTime);

	if(getMarkedEvtBySql >0 )
	{
		MarkedEvtStruct *MarkEvt = NULL;
		getMarkedEvtBySql(szSql,&MarkEvt);//从数据库查找

		while(MarkEvt != NULL)
		{
			MarkedEvtStruct mark;
			memcpy(&mark,MarkEvt,sizeof(mark));
			markedEventList.push_back(mark);

			MarkedEvtStruct *pTmpMark = MarkEvt;
	        MarkEvt = pTmpMark->next;
	        releaseMemery((void*)pTmpMark);
	        pTmpMark = NULL;
		}
	}

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetReportByFileName_ZJ(const int,const string,char **,int*,unsigned short&)'
/*bool DataControl::GetReportByFileName_ZJ( const int devId, const string sFilePartName, char * * ppReportData, int * piReportDataLen, unsigned short & TrgTypeNum )
{
	// {{{USR
	FileControl fileCtrl;

	REPORTHEAD cmdReportHead;
	memset(&cmdReportHead,0,sizeof(cmdReportHead));

	FaultAnalyzer analyzer;

	try
	{
		if(!analyzer.SHAnalyzeComtradeFile((char*)sFilePartName.c_str(),0))
		{
			mylog.WriteLog2File(LogControl::LERROR,"Analyze comtrade file failed in DataControl::GetReportByFileName_ZJ()");
			return false;
		}
	}
	catch(...)
	{
		mylog.WriteLog2File(LogControl::LERROR,"Happen unknow error when Analyze comtrade file failed in DataControl::GetReportByFileName_ZJ()");
		return false;
	}

	//启动原因
	TrgTypeNum = analyzer.rptManager->rptHead.TrgTypeNum;

	cmdReportHead.devID = devId;
	AnalyzResult2CmdReport(analyzer,&cmdReportHead);

	//写故障简报(xml)
	FILE *fp;
	string sXmlReportFile = sFilePartName;
	sXmlReportFile.append("故障报告.xml");

	fp = fopen(sXmlReportFile.c_str(),"w");
	CAnalyReportManager analyRptMgr;
	analyRptMgr.WriteXmlType1(fp,analyzer.rptManager->rptHead,analyzer.rptManager->rpt);
	fclose(fp);

	//写故障简报(rpt)
	string sTxtReportFile = sFilePartName;
	sTxtReportFile.append("故障报告.txt");
	analyRptMgr.WriteReportTXT(sTxtReportFile.c_str(),analyzer.rptManager->rptHead,analyzer.rptManager->rpt);

	ZJREPORT reportHead;
	memset(&reportHead,0,sizeof(reportHead));

	//文件名
	memcpy(reportHead.recWaveFileName,sFilePartName.c_str(),
		min(sizeof(reportHead.recWaveFileName),strlen(sFilePartName.c_str())));

	//间隔名
	memcpy(reportHead.BASName,cmdReportHead.report.BASName,
		min(sizeof(reportHead.recWaveFileName),sizeof(cmdReportHead.report.BASName)) );

	memcpy(&reportHead.FPT,&cmdReportHead.report.FPT,sizeof(reportHead.FPT));
	memcpy(&reportHead.JPT,&cmdReportHead.report.JPT,sizeof(reportHead.JPT));

	memcpy(reportHead.SCL,cmdReportHead.report.SCL,sizeof(reportHead.SCL));

	int iDPI_Time = (int)analyzer.rptManager->rpt->ReclosureTime; //重合闸时间
	if( iDPI_Time )
	{
		reportHead.DPI = 2;
	}
	else
	{
		reportHead.DPI = 1;
		iDPI_Time = 0;
	}

	reportHead.DPI_TimeL = iDPI_Time&0xFF;
	reportHead.DPI_TimeH = (iDPI_Time&0xFF00)>>8;

	//故障时间
	memcpy(&reportHead.faultTime,&cmdReportHead.report.faultTime,sizeof(reportHead.faultTime));

	//打包
	unsigned int uiXMLFileLen = fileCtrl.GetFileLen(sXmlReportFile.c_str());
	unsigned int uiTxtFileLen = fileCtrl.GetFileLen(sTxtReportFile.c_str());

	*piReportDataLen = sizeof(reportHead) + (1+4)*2 +uiXMLFileLen +uiTxtFileLen;
	*ppReportData = new char[*piReportDataLen];
	unsigned char *pDataTmp =(unsigned char *)(*ppReportData);

	memcpy(pDataTmp,&reportHead,sizeof(reportHead));
	pDataTmp += sizeof(reportHead);

	*pDataTmp = 241;	//xml file type
	pDataTmp++;
	memcpy(pDataTmp,&uiXMLFileLen,4);
	pDataTmp += 4;

	//get file data
	if(!fileCtrl.OpenFile(sXmlReportFile.c_str(),FileControl::OM_READ))
	{
		delete [](*ppReportData);
		*ppReportData = NULL;
		return false;
	}
	fileCtrl.ReadData((char*)pDataTmp,uiXMLFileLen);
	fileCtrl.CloseFile();
	pDataTmp += uiXMLFileLen;

	//delete file
	string delCmd("del ");
	system((delCmd + sXmlReportFile).c_str());

	*pDataTmp = 10;	//txt file type
	pDataTmp++;
	memcpy(pDataTmp,&uiTxtFileLen,4);
	pDataTmp += 4;

	//get file data
	if(!fileCtrl.OpenFile(sTxtReportFile.c_str(),FileControl::OM_READ))
	{
		delete [](*ppReportData);
		*ppReportData = NULL;
		return false;
	}
	fileCtrl.ReadData((char*)pDataTmp,uiTxtFileLen);
	fileCtrl.CloseFile();
	uiXMLFileLen += uiTxtFileLen;

	//delete file
	system((delCmd + sTxtReportFile).c_str());

	return true;
	// }}}USR
}*/
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR
void DataControl::StringTime2CP56Time2a(const char *pStringTime,struct CP56Time2a& destTime)
{
	int year = 0 ;
	int month = 0;
	int day = 0;
	int hours = 0;
	int minute =0;
	int second =0;
	int millisecond =0;
	string sFaultTime(pStringTime); //格式：yyyy-mm-dd HH:MM:ss.sss?
	string::size_type pos1 = sFaultTime.find_first_of("-");
	string::size_type pos2 = sFaultTime.find_last_of("-");
	if(pos1 != string::npos && pos2 != string::npos && pos2 > pos1)
	{
		string sTmp = sFaultTime.substr(0,pos1);
		year = atoi(sTmp.c_str());

		sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
		month = atoi(sTmp.c_str());

		pos1 = pos2;
		pos2 = sFaultTime.find(" ",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			day = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			hours = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			minute = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(".",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			second = atoi(sTmp.c_str());

			sTmp = sFaultTime.substr(pos2+1,sFaultTime.size() -pos2 -1);
			millisecond = atoi(sTmp.c_str());
		}

		destTime.year = year%200;
		destTime.month = month;
		destTime.day = day;
		destTime.hours = hours;
		destTime.minute = minute;
		destTime.milliseconds = second*1000 + millisecond;
	}
}

bool DataControl::RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport )
{
	if(RptInfo == NULL || pFaultReport == NULL)
		return false;

	pFaultReport->devID = RptInfo->DAUID;
	pFaultReport->rptID = RptInfo->RptID;

	//故障相别及类型
	pFaultReport->report.FPT.isOutArea = false;
	pFaultReport->report.FPT.isValid = true;

	switch(RptInfo->FaultType)
	{
	case 0://A相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 1://B相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 2://B相接地
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 3://BC两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 4://AB两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 5://AC两相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 6://BC两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 7://AB两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 8://AC两相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 9://三相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 888://区外故障
		pFaultReport->report.FPT.isOutArea = true;
		break;	
	default:
		pFaultReport->report.FPT.isValid = false;
		break;
	}

	//跳闸相别
	pFaultReport->report.JPT.isValid = true;
	switch(RptInfo->TripPhase)
	{
	case 0://A相
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 1://B相
		pFaultReport->report.JPT.B_Jump = true;
		break;
	case 2://C相
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 3://BC相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 4://AB相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 5://CA相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 6://ABC相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;	
	default:
		pFaultReport->report.JPT.isValid = false;
	}

	//故障距离
	float scl = (float)atof(RptInfo->Locate);
	memcpy(&pFaultReport->report.SCL[0],&scl,4);
	
	//故障时间
	int year = 0 ;
	int month = 0;
	int day = 0;
	int hours = 0;
	int minute =0;
	int second =0;
	int millisecond =0;
	string sFaultTime(RptInfo->FaultTime); //格式：yyyy-mm-dd HH:MM:ss.sss?
	string::size_type pos1 = sFaultTime.find_first_of("-");
	string::size_type pos2 = sFaultTime.find_last_of("-");
	if(pos1 != string::npos && pos2 != string::npos && pos2 > pos1)
	{
		string sTmp = sFaultTime.substr(0,pos1);
		year = atoi(sTmp.c_str());

		sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
		month = atoi(sTmp.c_str());

		pos1 = pos2;
		pos2 = sFaultTime.find(" ",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			day = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			hours = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(":",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			minute = atoi(sTmp.c_str());
		}

		pos1 = pos2;
		pos2 = sFaultTime.find_first_of(".",pos1+1);
		if(pos2 != string::npos && pos2 > pos1)
		{
			sTmp = sFaultTime.substr(pos1+1,pos2-pos1-1);
			second = atoi(sTmp.c_str());

			sTmp = sFaultTime.substr(pos2+1,sFaultTime.size() -pos2 -1);
			millisecond = atoi(sTmp.c_str());
		}

		pFaultReport->report.faultTime.year = year%200;
		pFaultReport->report.faultTime.month = month;
		pFaultReport->report.faultTime.day = day;
		pFaultReport->report.faultTime.hours = hours;
		pFaultReport->report.faultTime.minute = minute;
		pFaultReport->report.faultTime.milliseconds = second*1000 + millisecond;
	}

	strcpy(pFaultReport->report.recWaveFileName,RptInfo->RptFileName);
	strcpy(pFaultReport->report.BASName,RptInfo->FaultComponent);

	return true;
}

/*bool DataControl::AnalyzResult2CmdReport( FaultAnalyzer& analyzer, PREPORTHEAD pFaultReport )
{
	if(pFaultReport == NULL)
		return false;

	pFaultReport->rptID = analyzer.rptManager->rpt->wLineID;

	//故障相别及类型
	pFaultReport->report.FPT.isOutArea = false;
	pFaultReport->report.FPT.isValid = true;

	switch(analyzer.rptManager->rpt->FaultType)
	{
	case 0://A相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 1://B相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 2://B相接地
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 3://BC两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 4://AB两相接地
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 5://AC两相接地
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 6://BC两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 7://AB两相短路
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 8://AC两相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 9://三相短路
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 888://区外故障
		pFaultReport->report.FPT.isOutArea = true;
		break;	
	default:
		pFaultReport->report.FPT.isValid = false;
		break;
	}

	//跳闸相别
	pFaultReport->report.JPT.isValid = true;
	switch(analyzer.rptManager->rpt->TripPhase)
	{
	case 0://A相
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 1://B相
		pFaultReport->report.JPT.B_Jump = true;
		break;
	case 2://C相
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 3://BC相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 4://AB相
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 5://CA相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 6://ABC相
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;	
	default:
		pFaultReport->report.JPT.isValid = false;
	}

	//故障距离
	float scl = analyzer.rptManager->rpt->FltDisKm;
	memcpy(&pFaultReport->report.SCL[0],&scl,4);
	
	//故障时间
	pFaultReport->report.faultTime.year = analyzer.rptManager->rpt->FaultTime.Year%100;
	pFaultReport->report.faultTime.month = analyzer.rptManager->rpt->FaultTime.Month;
	pFaultReport->report.faultTime.day = analyzer.rptManager->rpt->FaultTime.Day;
	pFaultReport->report.faultTime.hours = analyzer.rptManager->rpt->FaultTime.Hour;
	pFaultReport->report.faultTime.minute = analyzer.rptManager->rpt->FaultTime.Minute;
	pFaultReport->report.faultTime.milliseconds = analyzer.rptManager->rpt->FaultTime.Second*1000
		+ analyzer.rptManager->rpt->FaultTime.Millisecond;

	memcpy(pFaultReport->report.recWaveFileName,analyzer.rptManager->rptHead.FileName,sizeof(pFaultReport->report.recWaveFileName));
	memcpy(pFaultReport->report.BASName,analyzer.rptManager->rpt->LinNam,sizeof(pFaultReport->report.BASName));

	return true;
}
*/
DataControl::DataQue* DataControl::CreateDataQue(int iSize)
 {
	 DataQue *pQue = new DataQue(iSize);
	 return pQue;
 }

DataControl::DataQue::DataQue(int iSize)
{
	InitializeCriticalSection(&criticalSection);

	startPos = 0;	//有效数据起始位置
	endPos = 0 ;	//有效数据结束位置
	bufLen = iSize;
	bIsCancle = false;
	pData = new char[iSize];
}

DataControl::DataQue::~DataQue()
{
	DeleteCriticalSection(&criticalSection);

	if(pData != NULL)
	{
		delete pData;
		pData = NULL;
	}
}

void DataControl::DataQue::push(const char* pSrcData,int iDataLen,bool *pIsStop)
{
	int iRemainBufSize = 0;	//buffer还剩余可写的空间
	int iRemainDataLen = iDataLen;//剩余未写的数据

	while(iRemainBufSize == 0 && *pIsStop == false && !bIsCancle)
	{
		EnterCriticalSection(&criticalSection);

		if(endPos >= startPos)
		{
			iRemainBufSize = bufLen - endPos;
			if(iRemainBufSize == 0 && startPos != 0)
			{
				LeaveCriticalSection(&criticalSection);

				//endpos已在结尾处，从头来过
				endPos = 0;
				continue;
			}
		}
		else
		{
			iRemainBufSize = startPos - endPos -1;
		}

		//共享内存是满的
		if(iRemainBufSize == 0)
		{
			LeaveCriticalSection(&criticalSection);

			::Sleep(100);
			continue;
		}

		//写入队列
		int iWritLen = min(iRemainBufSize,iRemainDataLen);
		memcpy(&pData[endPos],&pSrcData[iDataLen - iRemainDataLen],iWritLen);

		endPos += iWritLen;
		iRemainDataLen -= iWritLen;
		iRemainBufSize = 0;

		LeaveCriticalSection(&criticalSection);

		if(iRemainDataLen <= 0)
			break;

	}//end while(uiRemainBufSize == 0 && *pIsStop == false)
	
}

int DataControl::DataQue::pop(char *pDestBuf,int iBufLen)
{
	//同步控制
	EnterCriticalSection(&criticalSection);

	int iRemainDataLen = 0;

	//计算文件数据缓冲区中还剩余的数据量
	if(endPos>= startPos)
	{
		iRemainDataLen = endPos - startPos;
	}
	else
	{
		iRemainDataLen = bufLen - startPos;
	}

	int iReadLen = min(iBufLen,iRemainDataLen);

	memcpy(pDestBuf,&pData[startPos],iReadLen);
	
	startPos += iReadLen;

	//到达末尾处
	if(startPos == bufLen && startPos != endPos)
	{
		startPos = 0;
	}

	//离开临界区
	LeaveCriticalSection(&criticalSection);

	return iReadLen;
}

bool DataControl::DataQue::IsEmpty()
{
	if(startPos == endPos)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool DataControl::DataQue::IsFull()
{
	if(endPos == startPos-1 || (startPos==0 && endPos==bufLen) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//故障简报管理类

//##ModelId=479C018B02B1
DataControl::CAnalyReportManager::CAnalyReportManager()
{
}

//##ModelId=479C018B02BF
DataControl::CAnalyReportManager::~CAnalyReportManager()
{

}

//##ModelId=479C018B02A1
void DataControl::CAnalyReportManager::GetFaultTypeName(WORD wFaultType, CHAR *pstrName)
{
	//故障类型
	switch(wFaultType)
	{
	case 0:
		strcpy(pstrName, "A相接地");
		break;
	case 1:
		strcpy(pstrName, "B相接地");
		break;
	case 2:
		strcpy(pstrName, "C相接地");
		break;
	case 3:
		strcpy(pstrName, "BC两相接地");
		break;
	case 4:
		strcpy(pstrName, "AB两相接地");
		break;
	case 5:
		strcpy(pstrName, "CA两相接地");
		break;
	case 6:
		strcpy(pstrName, "BC两相短路");
		break;
	case 7:
		strcpy(pstrName, "AB两相短路");
		break;
	case 8:
		strcpy(pstrName, "CA两相短路");
		break;
	case 9:
		strcpy(pstrName, "三相短路");
		break;
	case 10:
		strcpy(pstrName, "变压器△侧CA短路");
		break;
	case 11:
		strcpy(pstrName, "变压器△侧AB短路");
		break;
	case 12:
		strcpy(pstrName, "变压器△侧BC短路");
		break;
	default:
		strcpy(pstrName, "");
	}

}

//##ModelId=479C018B0292
void DataControl::CAnalyReportManager::GetTripPhaseName(WORD wTripPhase, CHAR *pstrName)
{
	switch(wTripPhase)
	{
	case 0:
		strcpy(pstrName, "A相");
		break;
	case 1:
		strcpy(pstrName, "B相");
		break;
	case 2:
		strcpy(pstrName, "C相");
		break;
	case 3:
		strcpy(pstrName, "BC两相");
		break;
	case 4:
		strcpy(pstrName, "AB两相");
		break;
	case 5:
		strcpy(pstrName, "CA两相");
		break;
	case 6:
		strcpy(pstrName, "ABC三相");
		break;
	case 7:
		strcpy(pstrName, "变压器△侧CA相");
		break;
	case 8:
		strcpy(pstrName, "变压器△侧AB相");
		break;
	case 9:
		strcpy(pstrName, "变压器△侧BC相");
		break;
	default:
		strcpy(pstrName, "");
	}
}


/*void DataControl::CAnalyReportManager::WriteXmlType1(FILE *fp, ReportFileHead rptHead, Report * rpt)
{
	int i=0, y=0,wCurrentLine;

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
	//站
	fprintf(fp, "<RecorderReport Desc=\"录波简报\"><Fault value=\"故障描述\"><SubStation value=\"%s\"/>",rptHead.StationName);

	//录波器 Recorder
	fprintf(fp, "<Recorder value=\"%s\"/>",rptHead.RecordDeviceName);

	//录波起始时间	StartTime
	fprintf(fp, "<StartTime value=\"%04u-%02d-%02d %02d:%02d:%02d.%03d\" desc=\"录波起始时间\"/>",
		rptHead.FileStartTime.Year, rptHead.FileStartTime.Month
		, rptHead.FileStartTime.Day, rptHead.FileStartTime.Hour
		, rptHead.FileStartTime.Minute, rptHead.FileStartTime.Second
		, rptHead.FileStartTime.Millisecond);

	//相对时间(毫秒) RET
	BasicAlgorithm alg;
	double tempTime = alg.SHGetRelativeTime(rptHead.FileStartTime, rptHead.TriggerTime);
	fprintf(fp, "<StartTime value=\"%.0f\" desc=\"相对时间(毫秒)\"/>",tempTime);

	wCurrentLine=0;
	//输出故障线路
	while (i < rptHead.ReportCount)
	{
		if (wCurrentLine == rpt[i].wLineID)
		{
			//	fprintf(fp, "\n[ 本线路第%d次故障 ]\t\n" , ++y);
		}
		else
		{
			y=1;
			wCurrentLine=rpt[i].wLineID;
		}
		WriteLineFaultXml1(fp,rpt[i]);
		++i;
	}

}
*/
/*void DataControl::CAnalyReportManager::WriteLineFaultXml1(FILE *fp,Report rpt)
{
	INT32 i;
	CHAR tempString[28];
	char str[255]={0};

	if (rpt.FaultType == NOTFAULTTYPE)	//无故障显示启动报告
	{//区内无故障
		fprintf(fp, "<Range value=\"FALSE\" desc=\"故障范围(是否区内)\"/>");
	}
	else
	{	//故障线路
		fprintf(fp, "<Bay value=\"%s\" desc=\"故障间隔\"/>",rpt.LinNam);

		//故障类型 FPT
		fprintf(fp, "<FPT value=\"%d\" desc=\"故障类型\">",rpt.FaultType);

		//相
		GetFaultTypeName(rpt.FaultType, tempString);
		fprintf(fp, "<Phrase value=\"%s\" desc=\"相别\"/>",tempString);

		//接地
		char *pTmp = (rpt.FaultType>5)?"FALSE":"TRUE";
		fprintf(fp, "<Grounded value=\"%s\" desc=\"是否接地\"/></FPT>",pTmp);

		//跳闸相别 JPT
		GetTripPhaseName(rpt.TripPhase, tempString);
		if (rpt.TripPhase < 10)	//有跳闸
		{	
			fprintf(fp, "<JPT value=\"%d\" desc=\"跳闸相别\">",rpt.TripPhase);

			//相
			fprintf(fp, "<Phrase value=\"%s\"/></JPT>",tempString);
		}

		//故障位置
		fprintf(fp, "<Position value=\"%.3f\" desc=\"故障位置(测距)\"/>",rpt.FltDisKm);

		//区内有故障
		fprintf(fp, "<Range value=\"TRUE\" desc=\"故障范围(是否区内)\"/>");

		//故障零时刻
		sprintf(str,"%04u-%02d-%02d %02d:%02d:%02d.%03d",rpt.FaultTime.Year
			, rpt.FaultTime.Month, rpt.FaultTime.Day, rpt.FaultTime.Hour
			, rpt.FaultTime.Minute, rpt.FaultTime.Second, rpt.FaultTime.Millisecond);
		fprintf(fp, "<RET value=\"2007-03-15 13:28:52.653\" desc=\"故障零时刻\"/>",str);

		//保护动作时刻(毫秒)
		fprintf(fp, "<RelayActTime value=\"%.0f\" desc=\"保护动作时刻(毫秒)\"/>",rpt.ProtectTime);

		//断路器分时刻(毫秒)(无)
		for(i=0; i<3; i++)
		{
			if (rpt.AgainTripTime[i] != 0)
			{
				sprintf(str,"%d",rpt.AgainTripTime[i]);
				break;
			}
		}
		fprintf(fp, "<BreakerJumpTime value=\"%s\" desc=\"断路器分时刻(毫秒)\"/>",str);
		
		//重合闸时间(毫秒)"
		if (rpt.AgainFaultTime != 0)//重合闸动作
		{
			fprintf(fp, "<CHZTime value=\"%.0f\" desc=\"重合闸时间(毫秒)\"/>",rpt.AgainFaultTime);
		}

		//断路器合时刻
		for(i=0; i<3; i++)
		{
			if (rpt.ShutTime[i] != 0)
			{
				sprintf(str,"%d",rpt.ShutTime[i]);
				break;
			}
		}
		fprintf(fp, "<BreakerShutTime value=\"%s\" desc=\"断路器合时刻(毫秒)\"/>",str);

		
		if (rpt.AgainFaultType!=NOTFAULTTYPE && rpt.AgainTripPhase!=NOTFAULTTYPE)//再次故障输出
		{
			//再次保护动作时刻(毫秒)fprintf(fp, "再次故障切除时间: %.0f ms\n", rpt.AgainProtectTime);
			fprintf(fp, "<RelayActAgainTime value=\"%.0f\" desc=\"保护再次动作时刻(毫秒)\"/>",rpt.AgainProtectTime);

			//断路器再次分时刻(毫秒)"
			for(i=0; i<3; i++)
			{
				if (rpt.AgainTripTime[i] != 0)
				{
					sprintf(str,"%d",rpt.AgainTripTime[i]);
					break;
				}
			}
			fprintf(fp, "<BreakerJumpAgainTime value=\"%s\" desc=\"断路器再次分时刻(毫秒)\"/>",str);
		}

		//故障持续时间(毫秒)" (无)FaultEndTime
		fprintf(fp, "<Duration value=\"%.0f\" desc=\"故障持续时间(毫秒)\"/>",rpt.FaultEndTime);

		//重合是否成功
		pTmp = rpt.ReclosureResult == 0?"FALSE":"TRUE";
		fprintf(fp, "<Result value=\"%s\" desc=\"重合是否成功\"/>",pTmp);
	}

	//FaultValue desc="故障值"

	double tempValueI[4];
	double tempValueV[4];
	for (i=0; i<4; i++)
	{
		tempValueI[i] = rpt.ChData.FaultData[4+i][4] * rpt.ChData.PSRatio[4+i];//电流
	}
	sort(tempValueI, tempValueI+4);
	for (i=0; i<4; i++)
	{
		tempValueV[i] = rpt.ChData.FaultData[i][4] * rpt.ChData.PSRatio[i];//电压
	}
	sort(tempValueV, tempValueV+4);
	//故障值一次
	fprintf(fp, "<FaultValue desc=\"故障值\"><Primary desc=\"一次\">");

	fprintf(fp, "<I value=\"%.4f\" desc=\"最大故障电流\"/>",tempValueI[3]);
	fprintf(fp, "<U value=\"%.4f\" desc=\"最低故障电压\"/></Primary>",tempValueV[0]);

	//故障值二次	
	for (i=0; i<4; i++)
	{
		tempValueI[i] = rpt.ChData.FaultData[4+i][4];//电流
	}
	sort(tempValueI, tempValueI+4);
	for (i=0; i<4; i++)
	{
		tempValueV[i] = rpt.ChData.FaultData[i][4];//电压
	}
	fprintf(fp, "<Secondary desc=\"二次\">");

	fprintf(fp, "<I value=\"%.4f\" desc=\"最大故障电流\"/>",tempValueI[3]);
	fprintf(fp, "<U value=\"%.4f\" desc=\"最低故障电压\"/></Secondary></FaultValue>",tempValueV[0]);

	//故障前一周波
	char *cs[4] = {"Ia","Ib","Ic","I0"};
	char charTemp[255];

	fprintf(fp, "<CycBefore desc=\"故障前一周波\">");

	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[4+i][0],
			rpt.ChData.FaultData[4+i][1]);

	}
	cs[0] = "Ua";cs[1] = "Ub";cs[2] = "Uc";cs[3] = "U0";
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[i][0],
			rpt.ChData.FaultData[i][1]);
	}
	fprintf(fp, "</CycBefore>");

	//故障后一周波
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";

	fprintf(fp, "<Cyc1Afte desc=\"故障后一周波\">");

	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[4+i][4],
			rpt.ChData.FaultData[4+i][5]);
	}
	cs[0] = "Ua";cs[1] = "Ub";cs[2] = "Uc";cs[3] = "U0";
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[i][4],
			rpt.ChData.FaultData[i][5]);
	}
	fprintf(fp, "</Cyc1Afte>");

	//故障后两周波
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";
	fprintf(fp, "<Cyc2Afte desc=\"故障后两周波\">");

	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[4+i][6],
			rpt.ChData.FaultData[4+i][7]);
	}
	cs[0] = "Ua";cs[1] = "Ub";cs[2] = "Uc";cs[3] = "U0";
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.FaultData[i][6],
			rpt.ChData.FaultData[i][7]);
	}
	fprintf(fp, "</Cyc2Afte>");

	//重合闸 AgainCyc1After
	if (rpt.AgainFaultTime == 0)
	{
		return;
	}

	//故障后一周波(再次故障) AgainCyc1After
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";

	fprintf(fp, "<AgainCyc1After desc=\"故障后一周波(再次故障)\">");
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.ReclosureData[4+i][4],
			rpt.ChData.ReclosureData[4+i][5]);

	}
	cs[0] = "Ua";cs[1] = "Ub";cs[2] = "Uc";cs[3] = "U0";
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.ReclosureData[i][4],
			rpt.ChData.ReclosureData[i][5]);
	}
	fprintf(fp, "</AgainCyc1After>");

	//故障后两周波(再次故障)
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";
	fprintf(fp, "<AgainCyc2After desc=\"故障后两周波(再次故障)\">");

	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.ReclosureData[4+i][6],
			rpt.ChData.ReclosureData[4+i][7]);
	}
	cs[0] = "Ua";cs[1] = "Ub";cs[2] = "Uc";cs[3] = "U0";
	for (i=0; i<4; i++)
	{
		strcpy(charTemp, cs[i]);
		fprintf(fp, "<%s value=\"%.2f\" angle=\"%.2f\"/>",charTemp,rpt.ChData.ReclosureData[i][6],
			rpt.ChData.ReclosureData[i][7]);
	}
	fprintf(fp, "</AgainCyc2After>");
}
*/
//##ModelId=479C018B0282
/*BOOL DataControl::CAnalyReportManager::WriteLineFault(FILE *fp, Report rpt)
{
	INT32 i, j;
	CHAR tempString[28];

	if (rpt.FaultType == NOTFAULTTYPE)	//无故障显示启动报告
	{
		fprintf(fp, "\n故障类型: 区内无故障\n");
	}
	else
	{
		{//故障线路
			fprintf(fp, "\n故障线路: %s", rpt.LinNam );	
		}
		GetFaultTypeName(rpt.FaultType, tempString);
		{
			//故障时间
			fprintf(fp, "\n故障时间: %04u-%02d-%02d %02d:%02d:%02d.%03d", rpt.FaultTime.Year
				, rpt.FaultTime.Month, rpt.FaultTime.Day, rpt.FaultTime.Hour
				, rpt.FaultTime.Minute, rpt.FaultTime.Second, rpt.FaultTime.Millisecond);
		}
		{//故障相别
			fprintf(fp, "\n故障相别: %s", tempString );
		}		
		{//故障距离
			fprintf(fp, "\n故障距离: %.3f\n", rpt.FltDisKm);
		}
		GetTripPhaseName(rpt.TripPhase, tempString);
		if (rpt.TripPhase < 10)	//有跳闸
		{
			{
				//跳闸相别
				fprintf(fp, "跳闸相别: %s\n", tempString);
			}

		}
		{
			//故障切除时间
			fprintf(fp, "故障切除时间: %.0f ms\n", rpt.FaultEndTime);
		}
		if (rpt.AgainFaultTime != 0)
		{
			//重合闸时间
			{
				fprintf(fp, "重合闸时间: %.0f ms\n", rpt.AgainFaultTime);
			}

		}
		if (rpt.AgainFaultType!=NOTFAULTTYPE && rpt.AgainTripPhase!=NOTFAULTTYPE)//再次故障输出
		{
			GetFaultTypeName(rpt.AgainFaultType, tempString);
			if (rpt.AgainFaultType != NOTFAULTTYPE)
			{
				{
					fprintf(fp, "再次故障相别: %s\n", tempString);
				}
			}
			GetTripPhaseName(rpt.AgainTripPhase, tempString);
			if (rpt.AgainTripPhase != NOTFAULTTYPE)
			{
				{
					//再次跳闸相别
					fprintf(fp, "再次跳闸相别: %s\n", tempString);
				}
			}

			if (rpt.AgainFaultEndTime)
			{
				{
					//故障切除时间
					fprintf(fp, "再次故障切除时间: %.0f ms\n", rpt.AgainFaultEndTime);//AgainProtectTime
				}
			}
		}
	}	
	fprintf(fp, "\n%s前2周波有效值: \n线路名称:\t\t\t", rpt.FaultType==8888 ? "启动" : "故障");	
	fprintf(fp, "%s(V)\t\t\t\t%s(A)", rpt.VoltageNam, rpt.LinNam);	//输出线路名称
	fprintf(fp, "\n相别:\t\t%9s%9s%9s%9s%9s%9s%9s%9s\n"
		, rpt.ChNam[0], rpt.ChNam[1], rpt.ChNam[2], rpt.ChNam[3]	//输出通道名称
	, rpt.ChNam[4], rpt.ChNam[5], rpt.ChNam[6], rpt.ChNam[7]);
	for (i=0; i<2; i++)	//数值输出
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.FaultData[j][2 * i]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "%s后5周波有效值:\n", rpt.FaultType==NOTFAULTTYPE ? "启动" : "故障");

	for (i=0; i<5; i++)	//后5周波有效值
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.FaultData[j][2 * (i + 2)]);
		}
		fprintf(fp, "\n");
	}

	//重合闸
	if(rpt.AgainFaultTime == 0.00)
	{
		return TRUE;
	}

	fprintf(fp, "\n\n重合闸前2周波有效值:\n");	//重合闸数值输出
	for (i=0; i<2; i++)	//重合闸前2周波有效值
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.ReclosureData[j][2 * i]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "重合闸后5周波有效值:\n");
	
	for (i=0; i<5; i++)	//重合闸后5周波有效值
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.ReclosureData[j][2 * (i + 2)]);
		}
		fprintf(fp, "\n");
	}
	return TRUE;
}
*/
//##ModelId=479C018B0255
/*BOOL DataControl::CAnalyReportManager::WriteReportTXT(const CHAR * pstrFileName ,ReportFileHead rptHead, Report * rpt)
{
	if (rpt == NULL)
	{
		return 0;
	}
	BOOL bReturn = FALSE;
	WORD i, y, wCurrentLine;
	FILE *fp;
	
	if (rpt ==NULL )
	{
		return bReturn;
	}
	//打开文件
	if (rptHead.ReportCount == 0 || (fp = fopen(pstrFileName, "w")) == NULL)
	{
		return bReturn;
	}

	i=0;
	wCurrentLine=0;


	//输出总故障信息
	{
		fprintf(fp, "变电站编号: [%u_%u]\n", rptHead.StationID, rptHead.RecordDeviceID);
		fprintf(fp, "变电站名称: %s %s\n", rptHead.StationName, rptHead.RecordDeviceName);
		fprintf(fp, "录波文件名: %s\n", rptHead.FileName );
		fprintf(fp, "录波起始时间: %04u-%02d-%02d %02d:%02d:%02d.%03d\n", rptHead.FileStartTime.Year
			, rptHead.FileStartTime.Month, rptHead.FileStartTime.Day, rptHead.FileStartTime.Hour
			, rptHead.FileStartTime.Minute, rptHead.FileStartTime.Second, rptHead.FileStartTime.Millisecond);
	}
	//输出故障线路
	while (i < rptHead.ReportCount)
	{
		if (wCurrentLine == rpt[i].wLineID)
		{
			fprintf(fp, "\n[ 本线路第%d次故障 ]\t\n" , ++y);
		}
		else
		{
			y=1;
			wCurrentLine=rpt[i].wLineID;
		}
		WriteLineFault(fp, rpt[i]);
		++i;
	}	
	//输出装置信息
	{
		fprintf(fp, "\n\n\n\t\t\tSH2000D型录波器   深圳市双合电脑系统有限公司制造");
		fprintf(fp, "\n 地址：深圳市罗湖区莲塘第一工业区117栋   电话：0755—25708154，25708167   传真：0755—25708167");
	}
	fclose(fp);
	return bReturn = 1;
}*/

// }}}USR
// }}}RME

// }}}RME
