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
* �ļ����ƣ�DataControl.cpp
* �ļ���ʶ��
* ժ    Ҫ�����ݿ����࣬�������ݵĹ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��12��
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

	if(type == 1)//ֻ��ѯ�й��ϵ�¼���ļ��б�
	{
		sprintf(szSql,"select SHRpt.*,SHDAU.DAUName from SHRpt,SHDAU where SHRpt.DAUID=SHDAU.DAUID and %s FaultTime>='%s' and FaultTime<='%s' and FaultType<13",
			szDevIdCondition,szStartTime,szEndTime);
	}
	else//��ѯ���е�¼���ļ��б�
	{
		sprintf(szSql,"select * from SHRpt where %s FaultTime>='%s' and FaultTime<='%s'",
			szDevIdCondition,szStartTime,szEndTime);
	}

	if(getRptBySql >0 )
	{
		RptStruct *RptInfo = NULL;
		getRptBySql(szSql,&RptInfo);//�����ݿ����

		while(RptInfo != NULL)
		{
			FILEINFO fileInfo;
			memset(&fileInfo,0,sizeof(fileInfo));

			fileInfo.devID = RptInfo->DAUID;
			StringTime2CP56Time2a(RptInfo->FaultTime,fileInfo.faultTime);

			//�ļ���
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

			//�ļ�����
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
		getMarkedEvtBySql(szSql,&MarkEvt);//�����ݿ����

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

	//����ԭ��
	TrgTypeNum = analyzer.rptManager->rptHead.TrgTypeNum;

	cmdReportHead.devID = devId;
	AnalyzResult2CmdReport(analyzer,&cmdReportHead);

	//д���ϼ�(xml)
	FILE *fp;
	string sXmlReportFile = sFilePartName;
	sXmlReportFile.append("���ϱ���.xml");

	fp = fopen(sXmlReportFile.c_str(),"w");
	CAnalyReportManager analyRptMgr;
	analyRptMgr.WriteXmlType1(fp,analyzer.rptManager->rptHead,analyzer.rptManager->rpt);
	fclose(fp);

	//д���ϼ�(rpt)
	string sTxtReportFile = sFilePartName;
	sTxtReportFile.append("���ϱ���.txt");
	analyRptMgr.WriteReportTXT(sTxtReportFile.c_str(),analyzer.rptManager->rptHead,analyzer.rptManager->rpt);

	ZJREPORT reportHead;
	memset(&reportHead,0,sizeof(reportHead));

	//�ļ���
	memcpy(reportHead.recWaveFileName,sFilePartName.c_str(),
		min(sizeof(reportHead.recWaveFileName),strlen(sFilePartName.c_str())));

	//�����
	memcpy(reportHead.BASName,cmdReportHead.report.BASName,
		min(sizeof(reportHead.recWaveFileName),sizeof(cmdReportHead.report.BASName)) );

	memcpy(&reportHead.FPT,&cmdReportHead.report.FPT,sizeof(reportHead.FPT));
	memcpy(&reportHead.JPT,&cmdReportHead.report.JPT,sizeof(reportHead.JPT));

	memcpy(reportHead.SCL,cmdReportHead.report.SCL,sizeof(reportHead.SCL));

	int iDPI_Time = (int)analyzer.rptManager->rpt->ReclosureTime; //�غ�բʱ��
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

	//����ʱ��
	memcpy(&reportHead.faultTime,&cmdReportHead.report.faultTime,sizeof(reportHead.faultTime));

	//���
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
	string sFaultTime(pStringTime); //��ʽ��yyyy-mm-dd HH:MM:ss.sss?
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

	//�����������
	pFaultReport->report.FPT.isOutArea = false;
	pFaultReport->report.FPT.isValid = true;

	switch(RptInfo->FaultType)
	{
	case 0://A��ӵ�
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 1://B��ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 2://B��ӵ�
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 3://BC����ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 4://AB����ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 5://AC����ӵ�
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 6://BC�����·
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 7://AB�����·
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 8://AC�����·
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 9://�����·
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 888://�������
		pFaultReport->report.FPT.isOutArea = true;
		break;	
	default:
		pFaultReport->report.FPT.isValid = false;
		break;
	}

	//��բ���
	pFaultReport->report.JPT.isValid = true;
	switch(RptInfo->TripPhase)
	{
	case 0://A��
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 1://B��
		pFaultReport->report.JPT.B_Jump = true;
		break;
	case 2://C��
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 3://BC��
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 4://AB��
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 5://CA��
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 6://ABC��
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;	
	default:
		pFaultReport->report.JPT.isValid = false;
	}

	//���Ͼ���
	float scl = (float)atof(RptInfo->Locate);
	memcpy(&pFaultReport->report.SCL[0],&scl,4);
	
	//����ʱ��
	int year = 0 ;
	int month = 0;
	int day = 0;
	int hours = 0;
	int minute =0;
	int second =0;
	int millisecond =0;
	string sFaultTime(RptInfo->FaultTime); //��ʽ��yyyy-mm-dd HH:MM:ss.sss?
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

	//�����������
	pFaultReport->report.FPT.isOutArea = false;
	pFaultReport->report.FPT.isValid = true;

	switch(analyzer.rptManager->rpt->FaultType)
	{
	case 0://A��ӵ�
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 1://B��ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 2://B��ӵ�
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 3://BC����ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 4://AB����ӵ�
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 5://AC����ӵ�
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = true;
		break;
	case 6://BC�����·
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 7://AB�����·
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 8://AC�����·
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 9://�����·
		pFaultReport->report.FPT.A_Fault = true;
		pFaultReport->report.FPT.B_Fault = true;
		pFaultReport->report.FPT.C_Fault = true;
		pFaultReport->report.FPT.isEarthing = false;
		break;
	case 888://�������
		pFaultReport->report.FPT.isOutArea = true;
		break;	
	default:
		pFaultReport->report.FPT.isValid = false;
		break;
	}

	//��բ���
	pFaultReport->report.JPT.isValid = true;
	switch(analyzer.rptManager->rpt->TripPhase)
	{
	case 0://A��
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 1://B��
		pFaultReport->report.JPT.B_Jump = true;
		break;
	case 2://C��
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 3://BC��
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 4://AB��
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.A_Jump = true;
		break;
	case 5://CA��
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;
	case 6://ABC��
		pFaultReport->report.JPT.A_Jump = true;
		pFaultReport->report.JPT.B_Jump = true;
		pFaultReport->report.JPT.C_Jump = true;
		break;	
	default:
		pFaultReport->report.JPT.isValid = false;
	}

	//���Ͼ���
	float scl = analyzer.rptManager->rpt->FltDisKm;
	memcpy(&pFaultReport->report.SCL[0],&scl,4);
	
	//����ʱ��
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

	startPos = 0;	//��Ч������ʼλ��
	endPos = 0 ;	//��Ч���ݽ���λ��
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
	int iRemainBufSize = 0;	//buffer��ʣ���д�Ŀռ�
	int iRemainDataLen = iDataLen;//ʣ��δд������

	while(iRemainBufSize == 0 && *pIsStop == false && !bIsCancle)
	{
		EnterCriticalSection(&criticalSection);

		if(endPos >= startPos)
		{
			iRemainBufSize = bufLen - endPos;
			if(iRemainBufSize == 0 && startPos != 0)
			{
				LeaveCriticalSection(&criticalSection);

				//endpos���ڽ�β������ͷ����
				endPos = 0;
				continue;
			}
		}
		else
		{
			iRemainBufSize = startPos - endPos -1;
		}

		//�����ڴ�������
		if(iRemainBufSize == 0)
		{
			LeaveCriticalSection(&criticalSection);

			::Sleep(100);
			continue;
		}

		//д�����
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
	//ͬ������
	EnterCriticalSection(&criticalSection);

	int iRemainDataLen = 0;

	//�����ļ����ݻ������л�ʣ���������
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

	//����ĩβ��
	if(startPos == bufLen && startPos != endPos)
	{
		startPos = 0;
	}

	//�뿪�ٽ���
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


//���ϼ򱨹�����

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
	//��������
	switch(wFaultType)
	{
	case 0:
		strcpy(pstrName, "A��ӵ�");
		break;
	case 1:
		strcpy(pstrName, "B��ӵ�");
		break;
	case 2:
		strcpy(pstrName, "C��ӵ�");
		break;
	case 3:
		strcpy(pstrName, "BC����ӵ�");
		break;
	case 4:
		strcpy(pstrName, "AB����ӵ�");
		break;
	case 5:
		strcpy(pstrName, "CA����ӵ�");
		break;
	case 6:
		strcpy(pstrName, "BC�����·");
		break;
	case 7:
		strcpy(pstrName, "AB�����·");
		break;
	case 8:
		strcpy(pstrName, "CA�����·");
		break;
	case 9:
		strcpy(pstrName, "�����·");
		break;
	case 10:
		strcpy(pstrName, "��ѹ������CA��·");
		break;
	case 11:
		strcpy(pstrName, "��ѹ������AB��·");
		break;
	case 12:
		strcpy(pstrName, "��ѹ������BC��·");
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
		strcpy(pstrName, "A��");
		break;
	case 1:
		strcpy(pstrName, "B��");
		break;
	case 2:
		strcpy(pstrName, "C��");
		break;
	case 3:
		strcpy(pstrName, "BC����");
		break;
	case 4:
		strcpy(pstrName, "AB����");
		break;
	case 5:
		strcpy(pstrName, "CA����");
		break;
	case 6:
		strcpy(pstrName, "ABC����");
		break;
	case 7:
		strcpy(pstrName, "��ѹ������CA��");
		break;
	case 8:
		strcpy(pstrName, "��ѹ������AB��");
		break;
	case 9:
		strcpy(pstrName, "��ѹ������BC��");
		break;
	default:
		strcpy(pstrName, "");
	}
}


/*void DataControl::CAnalyReportManager::WriteXmlType1(FILE *fp, ReportFileHead rptHead, Report * rpt)
{
	int i=0, y=0,wCurrentLine;

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
	//վ
	fprintf(fp, "<RecorderReport Desc=\"¼����\"><Fault value=\"��������\"><SubStation value=\"%s\"/>",rptHead.StationName);

	//¼���� Recorder
	fprintf(fp, "<Recorder value=\"%s\"/>",rptHead.RecordDeviceName);

	//¼����ʼʱ��	StartTime
	fprintf(fp, "<StartTime value=\"%04u-%02d-%02d %02d:%02d:%02d.%03d\" desc=\"¼����ʼʱ��\"/>",
		rptHead.FileStartTime.Year, rptHead.FileStartTime.Month
		, rptHead.FileStartTime.Day, rptHead.FileStartTime.Hour
		, rptHead.FileStartTime.Minute, rptHead.FileStartTime.Second
		, rptHead.FileStartTime.Millisecond);

	//���ʱ��(����) RET
	BasicAlgorithm alg;
	double tempTime = alg.SHGetRelativeTime(rptHead.FileStartTime, rptHead.TriggerTime);
	fprintf(fp, "<StartTime value=\"%.0f\" desc=\"���ʱ��(����)\"/>",tempTime);

	wCurrentLine=0;
	//���������·
	while (i < rptHead.ReportCount)
	{
		if (wCurrentLine == rpt[i].wLineID)
		{
			//	fprintf(fp, "\n[ ����·��%d�ι��� ]\t\n" , ++y);
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

	if (rpt.FaultType == NOTFAULTTYPE)	//�޹�����ʾ��������
	{//�����޹���
		fprintf(fp, "<Range value=\"FALSE\" desc=\"���Ϸ�Χ(�Ƿ�����)\"/>");
	}
	else
	{	//������·
		fprintf(fp, "<Bay value=\"%s\" desc=\"���ϼ��\"/>",rpt.LinNam);

		//�������� FPT
		fprintf(fp, "<FPT value=\"%d\" desc=\"��������\">",rpt.FaultType);

		//��
		GetFaultTypeName(rpt.FaultType, tempString);
		fprintf(fp, "<Phrase value=\"%s\" desc=\"���\"/>",tempString);

		//�ӵ�
		char *pTmp = (rpt.FaultType>5)?"FALSE":"TRUE";
		fprintf(fp, "<Grounded value=\"%s\" desc=\"�Ƿ�ӵ�\"/></FPT>",pTmp);

		//��բ��� JPT
		GetTripPhaseName(rpt.TripPhase, tempString);
		if (rpt.TripPhase < 10)	//����բ
		{	
			fprintf(fp, "<JPT value=\"%d\" desc=\"��բ���\">",rpt.TripPhase);

			//��
			fprintf(fp, "<Phrase value=\"%s\"/></JPT>",tempString);
		}

		//����λ��
		fprintf(fp, "<Position value=\"%.3f\" desc=\"����λ��(���)\"/>",rpt.FltDisKm);

		//�����й���
		fprintf(fp, "<Range value=\"TRUE\" desc=\"���Ϸ�Χ(�Ƿ�����)\"/>");

		//������ʱ��
		sprintf(str,"%04u-%02d-%02d %02d:%02d:%02d.%03d",rpt.FaultTime.Year
			, rpt.FaultTime.Month, rpt.FaultTime.Day, rpt.FaultTime.Hour
			, rpt.FaultTime.Minute, rpt.FaultTime.Second, rpt.FaultTime.Millisecond);
		fprintf(fp, "<RET value=\"2007-03-15 13:28:52.653\" desc=\"������ʱ��\"/>",str);

		//��������ʱ��(����)
		fprintf(fp, "<RelayActTime value=\"%.0f\" desc=\"��������ʱ��(����)\"/>",rpt.ProtectTime);

		//��·����ʱ��(����)(��)
		for(i=0; i<3; i++)
		{
			if (rpt.AgainTripTime[i] != 0)
			{
				sprintf(str,"%d",rpt.AgainTripTime[i]);
				break;
			}
		}
		fprintf(fp, "<BreakerJumpTime value=\"%s\" desc=\"��·����ʱ��(����)\"/>",str);
		
		//�غ�բʱ��(����)"
		if (rpt.AgainFaultTime != 0)//�غ�բ����
		{
			fprintf(fp, "<CHZTime value=\"%.0f\" desc=\"�غ�բʱ��(����)\"/>",rpt.AgainFaultTime);
		}

		//��·����ʱ��
		for(i=0; i<3; i++)
		{
			if (rpt.ShutTime[i] != 0)
			{
				sprintf(str,"%d",rpt.ShutTime[i]);
				break;
			}
		}
		fprintf(fp, "<BreakerShutTime value=\"%s\" desc=\"��·����ʱ��(����)\"/>",str);

		
		if (rpt.AgainFaultType!=NOTFAULTTYPE && rpt.AgainTripPhase!=NOTFAULTTYPE)//�ٴι������
		{
			//�ٴα�������ʱ��(����)fprintf(fp, "�ٴι����г�ʱ��: %.0f ms\n", rpt.AgainProtectTime);
			fprintf(fp, "<RelayActAgainTime value=\"%.0f\" desc=\"�����ٴζ���ʱ��(����)\"/>",rpt.AgainProtectTime);

			//��·���ٴη�ʱ��(����)"
			for(i=0; i<3; i++)
			{
				if (rpt.AgainTripTime[i] != 0)
				{
					sprintf(str,"%d",rpt.AgainTripTime[i]);
					break;
				}
			}
			fprintf(fp, "<BreakerJumpAgainTime value=\"%s\" desc=\"��·���ٴη�ʱ��(����)\"/>",str);
		}

		//���ϳ���ʱ��(����)" (��)FaultEndTime
		fprintf(fp, "<Duration value=\"%.0f\" desc=\"���ϳ���ʱ��(����)\"/>",rpt.FaultEndTime);

		//�غ��Ƿ�ɹ�
		pTmp = rpt.ReclosureResult == 0?"FALSE":"TRUE";
		fprintf(fp, "<Result value=\"%s\" desc=\"�غ��Ƿ�ɹ�\"/>",pTmp);
	}

	//FaultValue desc="����ֵ"

	double tempValueI[4];
	double tempValueV[4];
	for (i=0; i<4; i++)
	{
		tempValueI[i] = rpt.ChData.FaultData[4+i][4] * rpt.ChData.PSRatio[4+i];//����
	}
	sort(tempValueI, tempValueI+4);
	for (i=0; i<4; i++)
	{
		tempValueV[i] = rpt.ChData.FaultData[i][4] * rpt.ChData.PSRatio[i];//��ѹ
	}
	sort(tempValueV, tempValueV+4);
	//����ֵһ��
	fprintf(fp, "<FaultValue desc=\"����ֵ\"><Primary desc=\"һ��\">");

	fprintf(fp, "<I value=\"%.4f\" desc=\"�����ϵ���\"/>",tempValueI[3]);
	fprintf(fp, "<U value=\"%.4f\" desc=\"��͹��ϵ�ѹ\"/></Primary>",tempValueV[0]);

	//����ֵ����	
	for (i=0; i<4; i++)
	{
		tempValueI[i] = rpt.ChData.FaultData[4+i][4];//����
	}
	sort(tempValueI, tempValueI+4);
	for (i=0; i<4; i++)
	{
		tempValueV[i] = rpt.ChData.FaultData[i][4];//��ѹ
	}
	fprintf(fp, "<Secondary desc=\"����\">");

	fprintf(fp, "<I value=\"%.4f\" desc=\"�����ϵ���\"/>",tempValueI[3]);
	fprintf(fp, "<U value=\"%.4f\" desc=\"��͹��ϵ�ѹ\"/></Secondary></FaultValue>",tempValueV[0]);

	//����ǰһ�ܲ�
	char *cs[4] = {"Ia","Ib","Ic","I0"};
	char charTemp[255];

	fprintf(fp, "<CycBefore desc=\"����ǰһ�ܲ�\">");

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

	//���Ϻ�һ�ܲ�
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";

	fprintf(fp, "<Cyc1Afte desc=\"���Ϻ�һ�ܲ�\">");

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

	//���Ϻ����ܲ�
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";
	fprintf(fp, "<Cyc2Afte desc=\"���Ϻ����ܲ�\">");

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

	//�غ�բ AgainCyc1After
	if (rpt.AgainFaultTime == 0)
	{
		return;
	}

	//���Ϻ�һ�ܲ�(�ٴι���) AgainCyc1After
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";

	fprintf(fp, "<AgainCyc1After desc=\"���Ϻ�һ�ܲ�(�ٴι���)\">");
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

	//���Ϻ����ܲ�(�ٴι���)
	cs[0] = "Ia";cs[1] = "Ib";cs[2] = "Ic";cs[3] = "I0";
	fprintf(fp, "<AgainCyc2After desc=\"���Ϻ����ܲ�(�ٴι���)\">");

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

	if (rpt.FaultType == NOTFAULTTYPE)	//�޹�����ʾ��������
	{
		fprintf(fp, "\n��������: �����޹���\n");
	}
	else
	{
		{//������·
			fprintf(fp, "\n������·: %s", rpt.LinNam );	
		}
		GetFaultTypeName(rpt.FaultType, tempString);
		{
			//����ʱ��
			fprintf(fp, "\n����ʱ��: %04u-%02d-%02d %02d:%02d:%02d.%03d", rpt.FaultTime.Year
				, rpt.FaultTime.Month, rpt.FaultTime.Day, rpt.FaultTime.Hour
				, rpt.FaultTime.Minute, rpt.FaultTime.Second, rpt.FaultTime.Millisecond);
		}
		{//�������
			fprintf(fp, "\n�������: %s", tempString );
		}		
		{//���Ͼ���
			fprintf(fp, "\n���Ͼ���: %.3f\n", rpt.FltDisKm);
		}
		GetTripPhaseName(rpt.TripPhase, tempString);
		if (rpt.TripPhase < 10)	//����բ
		{
			{
				//��բ���
				fprintf(fp, "��բ���: %s\n", tempString);
			}

		}
		{
			//�����г�ʱ��
			fprintf(fp, "�����г�ʱ��: %.0f ms\n", rpt.FaultEndTime);
		}
		if (rpt.AgainFaultTime != 0)
		{
			//�غ�բʱ��
			{
				fprintf(fp, "�غ�բʱ��: %.0f ms\n", rpt.AgainFaultTime);
			}

		}
		if (rpt.AgainFaultType!=NOTFAULTTYPE && rpt.AgainTripPhase!=NOTFAULTTYPE)//�ٴι������
		{
			GetFaultTypeName(rpt.AgainFaultType, tempString);
			if (rpt.AgainFaultType != NOTFAULTTYPE)
			{
				{
					fprintf(fp, "�ٴι������: %s\n", tempString);
				}
			}
			GetTripPhaseName(rpt.AgainTripPhase, tempString);
			if (rpt.AgainTripPhase != NOTFAULTTYPE)
			{
				{
					//�ٴ���բ���
					fprintf(fp, "�ٴ���բ���: %s\n", tempString);
				}
			}

			if (rpt.AgainFaultEndTime)
			{
				{
					//�����г�ʱ��
					fprintf(fp, "�ٴι����г�ʱ��: %.0f ms\n", rpt.AgainFaultEndTime);//AgainProtectTime
				}
			}
		}
	}	
	fprintf(fp, "\n%sǰ2�ܲ���Чֵ: \n��·����:\t\t\t", rpt.FaultType==8888 ? "����" : "����");	
	fprintf(fp, "%s(V)\t\t\t\t%s(A)", rpt.VoltageNam, rpt.LinNam);	//�����·����
	fprintf(fp, "\n���:\t\t%9s%9s%9s%9s%9s%9s%9s%9s\n"
		, rpt.ChNam[0], rpt.ChNam[1], rpt.ChNam[2], rpt.ChNam[3]	//���ͨ������
	, rpt.ChNam[4], rpt.ChNam[5], rpt.ChNam[6], rpt.ChNam[7]);
	for (i=0; i<2; i++)	//��ֵ���
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.FaultData[j][2 * i]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "%s��5�ܲ���Чֵ:\n", rpt.FaultType==NOTFAULTTYPE ? "����" : "����");

	for (i=0; i<5; i++)	//��5�ܲ���Чֵ
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.FaultData[j][2 * (i + 2)]);
		}
		fprintf(fp, "\n");
	}

	//�غ�բ
	if(rpt.AgainFaultTime == 0.00)
	{
		return TRUE;
	}

	fprintf(fp, "\n\n�غ�բǰ2�ܲ���Чֵ:\n");	//�غ�բ��ֵ���
	for (i=0; i<2; i++)	//�غ�բǰ2�ܲ���Чֵ
	{
		fprintf(fp, "\t\t");
		for (j=0; j<8; j++)
		{
			fprintf(fp, "%9.2f", rpt.ChData.ReclosureData[j][2 * i]);
		}
		fprintf(fp, "\n");
	}

	fprintf(fp, "�غ�բ��5�ܲ���Чֵ:\n");
	
	for (i=0; i<5; i++)	//�غ�բ��5�ܲ���Чֵ
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
	//���ļ�
	if (rptHead.ReportCount == 0 || (fp = fopen(pstrFileName, "w")) == NULL)
	{
		return bReturn;
	}

	i=0;
	wCurrentLine=0;


	//����ܹ�����Ϣ
	{
		fprintf(fp, "���վ���: [%u_%u]\n", rptHead.StationID, rptHead.RecordDeviceID);
		fprintf(fp, "���վ����: %s %s\n", rptHead.StationName, rptHead.RecordDeviceName);
		fprintf(fp, "¼���ļ���: %s\n", rptHead.FileName );
		fprintf(fp, "¼����ʼʱ��: %04u-%02d-%02d %02d:%02d:%02d.%03d\n", rptHead.FileStartTime.Year
			, rptHead.FileStartTime.Month, rptHead.FileStartTime.Day, rptHead.FileStartTime.Hour
			, rptHead.FileStartTime.Minute, rptHead.FileStartTime.Second, rptHead.FileStartTime.Millisecond);
	}
	//���������·
	while (i < rptHead.ReportCount)
	{
		if (wCurrentLine == rpt[i].wLineID)
		{
			fprintf(fp, "\n[ ����·��%d�ι��� ]\t\n" , ++y);
		}
		else
		{
			y=1;
			wCurrentLine=rpt[i].wLineID;
		}
		WriteLineFault(fp, rpt[i]);
		++i;
	}	
	//���װ����Ϣ
	{
		fprintf(fp, "\n\n\n\t\t\tSH2000D��¼����   ������˫�ϵ���ϵͳ���޹�˾����");
		fprintf(fp, "\n ��ַ���������޺���������һ��ҵ��117��   �绰��0755��25708154��25708167   ���棺0755��25708167");
	}
	fclose(fp);
	return bReturn = 1;
}*/

// }}}USR
// }}}RME

// }}}RME
