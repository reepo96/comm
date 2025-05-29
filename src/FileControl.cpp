// {{{RME classifier 'Logical View::FileControl'

#if defined( PRAGMA ) && ! defined( PRAGMA_IMPLEMENTED )
#pragma implementation "FileControl.h"
#endif

#include <RTSystem/Communication.h>
#include <FileControl.h>

// {{{RME tool 'OT::Cpp' property 'ImplementationPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2008,
* All rights reserved.
* 
* 文件名称：FileControl.cpp
* 文件标识：
* 摘    要：文件操作类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <time.h>
using namespace std;

#include "Command.h"
#include "../ZipLib/zlib.h"

#ifdef _DEBUG
#define new MYDEBUG_NEW
#endif

#define CHUNK 4096 //16384

// }}}USR
// }}}RME

// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
FileControl::FileControl( void )
	: m_pFile( NULL )
{
}
// }}}RME

// {{{RME operation '~FileControl()'
FileControl::~FileControl( void )
{
	// {{{USR
	        if(m_pFile != NULL)
	    {
	        CloseFile();
	    }
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileDirector(unsigned int,const char*,FILETYPE,const char*)'
string FileControl::GetFileDirector( unsigned int devId, const char * fileName, FILETYPE fileType, const char * fileYearMonth )
{
	// {{{USR
	    //用户设定的目录
	    string fileDirector(SystemCfger::GetInstance().DataFilePath );
	    char ch = fileDirector.at(fileDirector.size()-1);
	    if(ch != '/' && ch != '\\')
	    {
	        fileDirector.append("/");
	    }

	    char szDevId[4]={0};	//设备编号目录
		char szStation[10] = {0};//站编号

		/*
		PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo( devId,1);
		if( pDauInf == NULL)
		{
			pDauInf = SystemCfger::GetInstance().GetDAUInfo( devId,2);
		}
		
		int	iStation = 0;
		if( pDauInf != NULL)
		{
			iStation = pDauInf->StationId;
		}
		*/

		/*if(iStation != 0)
		{
			fileDirector.append(::itoa(iStation,szStation,10));
			fileDirector.append("/");
		}*/

	    fileDirector.append(::itoa( devId ,szDevId,10));
	    fileDirector.append("/");

	    //根据不同的文件类型查找不同的文件目录
	    switch(fileType)
	    {
	    case FT_SHD:
	        fileDirector.append("SHD");
	        break;
	    case FT_COMTRADE:
	        fileDirector.append("Comtrade");
	        break;
	    case FT_SOE:
	        fileDirector.append("SOE");
	        break;
	    case FT_LOG:
	        fileDirector.append("LOG");
	        break;
	    case FT_CONT:
	        fileDirector.append("Cont");
	        break;
	    case FT_CCOMTRADE:
	        fileDirector.append("CComtrade");
	        break;
		case FT_LOW:
	        fileDirector.append("LOW");
	        break;
		case FT_COMTRADE_DIV:
			fileDirector.append("ComtradeDiv");
			break;
		case FT_REPORT:
			fileDirector.append("Report");
			break;
	    case FT_CFG:
	        break;
	    default:
	        return "";
	    }

	    if( fileType != FT_CFG)
	    {
	        fileDirector.append("/");
	    }
	    else
	    {
	        //查找配置文件是否存在
	        string tmp(fileDirector);
	        tmp.append(fileName);
	        if(this->IsExist(tmp.c_str()))
	        {
	            return fileDirector;
	        }
	        else
	        {
	            return "";
	        }
	    }

	    if(fileYearMonth != NULL)
	    {
	        //年月已知，查找此年月目录
	        fileDirector.append(fileYearMonth);
	        fileDirector.append("/");

	        string tmp(fileDirector);
	        tmp.append(fileName);
	        if(this->IsExist(tmp.c_str()))
	        {
	            return fileDirector;
	        }
	        else
	        {
	            return "";
	        }
	    }
	    else
	    {
	        //年月未知，查找目录下所有年月目录
	        WIN32_FIND_DATA findData;
	        memset(&findData,0,sizeof(findData));

	        string findFile(fileDirector);
	        findFile.append("*.*");
	        HANDLE hFind = FindFirstFile(findFile.c_str() ,&findData);

	        if(hFind != INVALID_HANDLE_VALUE)
	        {
	            do           
	            {
	                //是目录
	                if(findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY&&
	                    ::strcmp(findData.cFileName,".") != 0 &&
	                    strcmp(findData.cFileName,"..") != 0 )
	                {
	                    string tmpDirector(fileDirector);
	                    tmpDirector.append(findData.cFileName);
	                    tmpDirector.append("/");

	                    string tmp(tmpDirector);
	                    tmp.append(fileName);
	                    if(this->IsExist(tmp.c_str()))
	                    {
	                        fileDirector.append(findData.cFileName);
	                        fileDirector.append("/");

							::FindClose(hFind);
							hFind = NULL;

	                        return fileDirector; //文件在此目录中
	                    }
	                }
	            } while( FindNextFile(hFind,&findData));

				FindClose(hFind);
				hFind = NULL;
	        }
	    }

	    return ""; //没有找到文件

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileTypeDirector(unsigned int,FILETYPE)'
string FileControl::GetFileTypeDirector( unsigned int devId, FILETYPE fileType )
{
	// {{{USR
	    //用户设定的目录
	    string fileDirector(SystemCfger::GetInstance().DataFilePath );
	    char ch = fileDirector.at(fileDirector.size()-1);
	    if(ch != '/' && ch != '\\')
	    {
	        fileDirector.append("/");
	    }

	    //设备编号目录
	    char szDevId[4]={0};
		char szStation[10] = {0};//站编号

		/*
		PDAUINFO pDauInf = SystemCfger::GetInstance().GetDAUInfo( devId,1);
		if( pDauInf == NULL)
		{
			pDauInf = SystemCfger::GetInstance().GetDAUInfo( devId,2);
		}
		
		int	iStation = 0;
		if( pDauInf != NULL)
		{
			iStation = pDauInf->StationId;
		}
		*/
		/*if(iStation != 0)
		{
			fileDirector.append(::itoa(iStation,szStation,10));
			fileDirector.append("/");
			if(!IsExist(fileDirector.c_str()))
			{
				if(-1 == _mkdir(fileDirector.c_str()))
				{
					return "";
				}
			}
		}*/

	    fileDirector.append(::itoa( devId,szDevId,10));
	    fileDirector.append("/");
		if(!IsExist(fileDirector.c_str()))
		{
			if(-1 == _mkdir(fileDirector.c_str()))
			{
				return "";
			}
		}

	    //根据不同的文件类型查找不同的文件目录
	    switch(fileType)
	    {
	    case FT_SHD:
	        fileDirector.append("SHD");
	        break;
	    case FT_COMTRADE:
	        fileDirector.append("Comtrade");
	        break;
	    case FT_SOE:
	        fileDirector.append("SOE");
	        break;
	    case FT_LOG:
	        fileDirector.append("LOG");
	        break;
	    case FT_CONT:
	        fileDirector.append("Cont");
	        break;
	    case FT_CCOMTRADE:
	        fileDirector.append("CComtrade");
	        break;
		case FT_GENERATORTEST:
			fileDirector.append("GeneratorTest");
			break;
		case FT_LOW:
			fileDirector.append("LOW");
			break;
		case FT_COMTRADE_DIV:
			fileDirector.append("ComtradeDiv");
			break;
		case FT_REPORT:
			fileDirector.append("Report");
			break;
		case FT_PQCOMTRADE:
			fileDirector.append("PQComtrade");
			break;
		case FT_PQTRH:
			fileDirector.append("PQTRH");
			break;
		case FT_PQEVT:
			fileDirector.append("PQEVT");
			break;
		case FT_PQSTAT:
			fileDirector.append("PQStat");
			break;
	    case FT_CFG:
	        break;
	    default:
	        return "";
	    }

	    if( fileType != FT_CFG)
	    {
	        fileDirector.append("/");
	    }

	        if(!IsExist(fileDirector.c_str()))
	        {
	            if(-1 == _mkdir(fileDirector.c_str()))
	            {
	                return "";
	            }
	        }
	    
	    return fileDirector; 

	// }}}USR
}
// }}}RME

// {{{RME operation 'IsExist(const char*)'
bool FileControl::IsExist( const char * fileName )
{
	// {{{USR
	        if( _access(fileName,0) != -1)
	        {
	            return true;
	        }
	        else
	        {
	            return false;
	        }
	// }}}USR
}
// }}}RME

// {{{RME operation 'OpenFile(const char*,OPENMODE)'
bool FileControl::OpenFile( const char * fileName, OPENMODE openMode )
{
	// {{{USR
	    if(m_pFile != NULL)
	    {
	        CloseFile();
	    }

	        switch(openMode)
	        {
	        case OM_READ:
	            m_pFile = fopen(fileName,"rb");
	            break;
	        case OM_WRITE:
	            m_pFile = fopen(fileName,"wb");
	            break;
	        case OM_APPEND:
	            m_pFile = fopen(fileName,"a+b");
	            break;
	        default:
	            m_pFile = fopen(fileName,"rb");
	            break;
	        }
	   
	    if(m_pFile == NULL)
	    {
			DWORD dwErr = GetLastError();
			mylog.FormatLogInf(LogControl::LERROR,
				"Open file:%s fail,errorcode:%d",fileName,dwErr);
	        return false;
	    }
	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'CloseFile()'
bool FileControl::CloseFile( void )
{
	// {{{USR
	    if(m_pFile != NULL)
	    {
	        fclose(m_pFile);
	        m_pFile = NULL;
	    }
	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'ReadData(int,char*,int)'
int FileControl::ReadData( int startPos, char * dest, int readLen )
{
	// {{{USR

	    if(m_pFile != NULL)
	    {
	        fseek(m_pFile,startPos,SEEK_SET);
	        return (int)fread(dest,sizeof(char),readLen,m_pFile);
	    }

	    return 0;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileLen(const char*)'
unsigned int FileControl::GetFileLen( const char * fileName )
{
	// {{{USR
	    FILE *pFile = fopen(fileName,"rb");
	    if(pFile == NULL)
	    {
	        return 0;
	    }

	    fpos_t pos;
	    fseek(pFile,0,SEEK_END);
	    if(fgetpos(pFile,&pos) != 0)
	    {
	        fclose(pFile);
	        return 0;
	    }

	    fclose(pFile);

	    return (unsigned int)pos;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFaultTime(const char*,struct CP56Time2a*)'
bool FileControl::GetRecWaveFaultTime( const char * fileName, struct CP56Time2a * time )
{
	// {{{USR
	    FILE *pFile = fopen(fileName,"r");
	    if(pFile == NULL)
	    {
	        return false;
	    }

	    //分析CFG文件
	    const int lineSize = 512;
	    char line[lineSize] = {0};

	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    memset(line,0,lineSize);
	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    //分析模拟量、数字量的数量
	    string data(line);
	    string::size_type pos1 = data.find(",");
	    string::size_type pos2 = data.find_last_of(",");
	    if(pos1 == string::npos || pos2 == string::npos || (pos1+2) >= pos2)
	    {
	        fclose(pFile);
	        return false;
	    }

	    string sAnalogCount = data.substr(pos1+1,pos2-pos1-2);          //模拟量的数量
	    string sDigitalCount = data.substr(pos2+1,data.size()-2-pos2-2);  //数字量的数量
	    int iAnalogCount = ::atoi(sAnalogCount.c_str());
	    int iDigitalCount = ::atoi(sDigitalCount.c_str());
	    for(int i=0;i < (iAnalogCount+iDigitalCount); i++)
	    {
	        if(fgets(line,lineSize-1,pFile) == NULL)
	        {
	            fclose(pFile);
	            return false;
	        }
	    }

	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    //分析采样率数量
	    memset(line,0,lineSize);
	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    int iRateCount = ::atoi(line);
	    for(int j=0;j < iRateCount; j++)
	    {
	        if(fgets(line,lineSize-1,pFile) == NULL)
	        {
	            fclose(pFile);
	            return false;
	        }
	    }

	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    //故障时间所在的行
	    memset(line,0,lineSize);
	    if(fgets(line,lineSize-1,pFile) == NULL)
	    {
	        fclose(pFile);
	        return false;
	    }

	    int year,month,day,hour,minute,second,millisecond;
	    string sTime(line);

	    //get day
	    string::size_type timePos1 = sTime.find("/");
	    if(timePos1 <=0)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    string sTmp = sTime.substr(0,timePos1);
	    day = atoi( sTmp.c_str() );
	    
	    //get month
	    string::size_type timePos2 = sTime.find("/",timePos1+1);
	    if(timePos2 <= timePos1+1)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    sTmp = sTime.substr(timePos1+1,timePos2-timePos1-1);
	    month = atoi(sTmp.c_str());

	    //get year
	    timePos1 = sTime.find(",",timePos2+1);
	    if(timePos1 <= timePos2+1)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    sTmp = sTime.substr(timePos2+1,timePos1-timePos2-1);
	    year = atoi(sTmp.c_str());

	    //get hour
	    timePos2 = sTime.find(":",timePos1+1);
	    if(timePos2 <= timePos1+1)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    sTmp = sTime.substr(timePos1+1,timePos2-timePos1-1);
	    hour = atoi(sTmp.c_str());

	    //get minute
	    timePos1 = sTime.find(":",timePos2+1);
	    if(timePos1 <= timePos2+1)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    sTmp = sTime.substr(timePos2+1,timePos1-timePos2-1);
	    minute = atoi(sTmp.c_str());

	    //get second
	    timePos2 = sTime.find(".",timePos1+1);
	    if(timePos2 <= timePos1+1)
	    {
	        fclose(pFile);
	        return false;        
	    }
	    sTmp = sTime.substr(timePos1+1,timePos2-timePos1-1);
	    second = atoi(sTmp.c_str());

	     //get millisecond    
	    sTmp = sTime.substr(timePos2+1,sTime.size()-timePos2-1);
	    millisecond = atoi(sTmp.c_str())/1000;

	    struct tm t;
	    memset(&t,0,sizeof(t));
	    t.tm_hour = hour;
	    t.tm_isdst = 0;
	    t.tm_mday = day;
	    t.tm_min = minute;
	    t.tm_mon = month-1;
	    t.tm_sec = 0;
	    t.tm_year = year-1900;
	        

	    time_t tt= mktime(&t);
	    struct tm *newtime = localtime(&tt);

	    time->day = (unsigned char)day;
	    time->hours = (unsigned char)hour;
	    time->milliseconds = (unsigned short)(second*1000 + millisecond);
	    time->minute = (unsigned char)minute;
	    time->month = (unsigned char)month;
	    time->week = (unsigned char)newtime->tm_wday;

	    time->year = (unsigned char)(year%100);

	    return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'Seek(unsigned int)'
bool FileControl::Seek( unsigned int newPos )
{
	// {{{USR
	        if(m_pFile == NULL)
	        {
	            return false;
	        }

	        if(0==fseek(m_pFile,newPos,SEEK_SET))
	        {
	            return true;
	        }
	        else
	        {
	            return false;
	        }

	// }}}USR
}
// }}}RME

// {{{RME operation 'WriteData(const char*,int)'
int FileControl::WriteData( const char * pData, int dataLen )
{
	// {{{USR
	if(m_pFile == NULL)
	{
		return 0;
	}
	return (int)fwrite(pData,sizeof(char),dataLen,m_pFile);

	// }}}USR
}
// }}}RME

// {{{RME operation 'ReadData(char*,int)'
int FileControl::ReadData( char * dest, int readLen )
{
	// {{{USR
	if(m_pFile == NULL)
	{
		return 0;
	}
	return (int)fread(dest,sizeof(char),readLen,m_pFile);

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveFileList(unsigned int,struct CP56Time2a,struct CP56Time2a,vector<PCMDFILELIST>&)'
bool FileControl::GetRecWaveFileList( unsigned int devId, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< PCMDFILELIST > & fileList )
{
	// {{{USR

		int startYearMonth = 0; //开始时间年月
		int endYearMonth = 0 ;  //结束时间年月
		
		startYearMonth = (startTime.year +2000)*100 + startTime.month;
		endYearMonth = (endTime.year +2000)*100 + endTime.month;
		
		unsigned long month_days = 0;   //月份包含的天数
		//start time
		if(startTime.month <= 7 )
		{
			month_days = (startTime.month/2)*31 + (startTime.month - startTime.month/2 -1)*30;
		}
		else
		{
			month_days = ((startTime.month+1)/2)*31 + (startTime.month - ((startTime.month+1)/2) -1)*30;
		}
		unsigned long startMinute =(startTime.year*365 +month_days +startTime.day)*24*60 +
			startTime.hours*60 +startTime.minute;
		
		//end time
		if(endTime.month <= 7 )
		{
			month_days = (endTime.month/2)*31 + (endTime.month - endTime.month/2 -1)*30;
		}
		else
		{
			month_days = ((endTime.month+1)/2)*31 + (endTime.month - ((endTime.month+1)/2) -1)*30;
		}
		unsigned long endMinute =(endTime.year*365 +month_days +endTime.day)*24*60 +
			endTime.hours*60 +endTime.minute;

		//获得故障录波comtrade文件所在根目录
		string waveFileRootDir = this->GetFileTypeDirector(devId,FT_COMTRADE);
		if(waveFileRootDir.compare("")==0)
		{
			return false;
		}
		
		//先查找根目录下所有的子目录（子目录按照年月yyyymm创建的）
		string findSubDir = waveFileRootDir + string("*.*");
		
		WIN32_FIND_DATA findSubData;
		memset(&findSubData,0,sizeof(findSubData));
		HANDLE hSubFind = FindFirstFile(findSubDir.c_str() ,&findSubData);
		
		if(hSubFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				string subDir(findSubData.cFileName);
				int subVal = -1; //子目录名转换成数字
				if(subDir.size() == 6)
				{
					subVal = ::atoi(subDir.c_str());
				}
				
				//查找在查询时间范围内的子目录
				if(subVal >= startYearMonth && subVal <= endYearMonth)
				{
					string findFile = waveFileRootDir +subDir +string("/*.cfg");
					
					WIN32_FIND_DATA findData;
					memset(&findData,0,sizeof(findData));
					HANDLE hFind = FindFirstFile(findFile.c_str() ,&findData);
					
					if(hFind != INVALID_HANDLE_VALUE)
					{
						do           
						{
							SYSTEMTIME sysTime;
							FILETIME localFileTime;
							memset(&sysTime,0,sizeof(sysTime));
							FileTimeToLocalFileTime(&findData.ftLastWriteTime,&localFileTime);
							::FileTimeToSystemTime(&localFileTime,&sysTime);
							
							if(sysTime.wMonth <= 7 )
							{
								month_days = (sysTime.wMonth/2)*31 + (sysTime.wMonth - sysTime.wMonth/2 -1)*30;
							}
							else
							{
								month_days = ((sysTime.wMonth+1)/2)*31 + (sysTime.wMonth - ((sysTime.wMonth+1)/2) -1)*30;
							}
							unsigned long fileMinute =((sysTime.wYear%100)*365 +month_days +sysTime.wDay)*24*60 +
								sysTime.wHour*60 +sysTime.wMinute;
							
							//符合时间范围的文件
							if(fileMinute >= startMinute && fileMinute <= endMinute)
							{
								PCMDFILELIST pFileList = new CMDFILELIST;
								memset(pFileList,0,sizeof(CMDFILELIST));

								//去掉后缀名
								string sTmp(findData.cFileName);
								string::size_type pos = sTmp.find(".");
								string sTmp2;
								if(pos != string::npos)
								{
									sTmp2 = sTmp.substr(0,pos);
								}
								else
								{
									sTmp2 = sTmp;
								}
								
								memcpy(pFileList->fileName,sTmp2.c_str(),sTmp2.size());
								
								this->GetRecWaveFaultTime(pFileList->fileName,&pFileList->faultTime);
								
								fileList.push_back(pFileList);
							}
						} while( FindNextFile(hFind,&findData));
					}//end if(hFind != INVALID_HANDLE_VALUE)            	    
					
					FindClose(hFind);
					hFind = NULL;
					
				}//end if(subVal >= startYearMonth && subVal <= endYearMonth)
			}while( FindNextFile(hSubFind,&findSubData));
		}//end if(hSubFind != INVALID_HANDLE_VALUE)
		
		FindClose(hSubFind);
		hSubFind = NULL;
		
		return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetRecWaveReport(PCMDREPORT)'
bool FileControl::GetRecWaveReport( PCMDREPORT recWaveReport )
{
	// {{{USR

	    //TODO:
	    return false;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileList(char*,struct CP56Time2a,struct CP56Time2a,vector<PCMDGENERALFILEINF>&)'
bool FileControl::GetFileList( char * filePath, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< PCMDGENERALFILEINF > & fileList )
{
	// {{{USR

	vector< GENERALFILEPATHINF > filePathList;

	GetFileList(filePath,2,startTime,endTime,filePathList);

	vector< GENERALFILEPATHINF >::iterator it = filePathList.begin();
	for(;it != filePathList.end();it++)
	{
		PCMDGENERALFILEINF pFileInfo = new CMDGENERALFILEINF;
		memset(pFileInfo,0,sizeof(CMDGENERALFILEINF));

		memcpy(pFileInfo->fileName,(*it).fileName,sizeof(pFileInfo->fileName));
		pFileInfo->fileLen = (*it).fileLen;
		pFileInfo->fileTime = (*it).fileTime;

		fileList.push_back(pFileInfo);
	}
	filePathList.clear();

	return true;
	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileList(char*,int,struct CP56Time2a,struct CP56Time2a,vector<GENERALFILEPATHINF>&)'
bool FileControl::GetFileList( char * filePath, int type, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< GENERALFILEPATHINF > & fileList )
{
	// {{{USR

	SYSTEMTIME systStartTime,systEndTime;

	CP56Time2aToSystemTime(startTime,systStartTime);
	CP56Time2aToSystemTime(endTime,systEndTime);
	if(endTime.IV)
	{
		systEndTime.wYear = 2117;
	}

	FILETIME ftStartTime,ftEndTime;

	SystemTimeToFileTime(&systStartTime,&ftStartTime);
	SystemTimeToFileTime(&systEndTime,&ftEndTime);

	WIN32_FIND_DATA findData;
	memset(&findData,0,sizeof(findData));
	HANDLE hFind = FindFirstFile(filePath ,&findData);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do           
		{
			if(strcmp(findData.cFileName,".") == 0 || strcmp(findData.cFileName,"..") == 0)
			{
				continue;
			}

			SYSTEMTIME sysTime;
			FILETIME localFileTime;
			memset(&sysTime,0,sizeof(sysTime));
			FileTimeToLocalFileTime(&findData.ftLastWriteTime,&localFileTime);
			::FileTimeToSystemTime(&localFileTime,&sysTime);

			GENERALFILEPATHINF filePathInf;
			memset(&filePathInf,0,sizeof(filePathInf));

			if(( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY &&
				(type == 0 || type == 1) )
			{
				//如果是目录且要求查询目录				
				filePathInf.isDir = 1;			
			}
			else if( (type == 0 || type == 2) && CompareFileTime(&ftStartTime,&localFileTime) <=0 
				&& CompareFileTime(&ftEndTime,&localFileTime) >= 0 )
			{
				//如果是文件，则查找符合时间范围的文件
				filePathInf.isDir = 0;
			}
			else
			{
				if( FindNextFile(hFind,&findData))
					continue;
			}

			memcpy(filePathInf.fileName,findData.cFileName,sizeof(filePathInf.fileName));
			filePathInf.fileLen = findData.nFileSizeLow;
			SystemTimeToCP56Time2a(&sysTime,&filePathInf.fileTime);

			fileList.push_back(filePathInf);
			
		} while( FindNextFile(hFind,&findData));
	}//end if(hFind != INVALID_HANDLE_VALUE)            	    

	FindClose(hFind);
	hFind = NULL;

	return true;

	// }}}USR
}
// }}}RME

// {{{RME operation 'GetFileTime(const char*,struct CP56Time2a*)'
bool FileControl::GetFileTime( const char * filePartName, struct CP56Time2a * time,int iType )
{
	// {{{USR
	    HANDLE hFile = CreateFile(filePartName,GENERIC_READ,0,NULL,
	        OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	    if(hFile == INVALID_HANDLE_VALUE)
	    {
	        return false;
	    }

	    FILETIME fileTime;
	    SYSTEMTIME sysTime;

		switch(iType)
		{
		case 1:
			if(FALSE == ::GetFileTime(hFile,&fileTime,NULL,NULL))
			{
				::CloseHandle(hFile);
				return false;
			}
			break;
		case 2:
			if(FALSE == ::GetFileTime(hFile,NULL,&fileTime,NULL))
			{
				::CloseHandle(hFile);
				return false;
			}
			break;
		case 3:
			if(FALSE == ::GetFileTime(hFile,NULL,NULL,&fileTime))
			{
				::CloseHandle(hFile);
				return false;
			}
			break;
		default:
			if(FALSE == ::GetFileTime(hFile,&fileTime,NULL,NULL))
			{
				::CloseHandle(hFile);
				return false;
			}
			break;
		}

	    if(FALSE == FileTimeToSystemTime(&fileTime,&sysTime))
	    {
			::CloseHandle(hFile);
			hFile = NULL;
	        return false;
	    }

		FILETIME localFileTime;       
		FileTimeToLocalFileTime(&fileTime,&localFileTime);
		::FileTimeToSystemTime(&localFileTime,&sysTime);

		SystemTimeToCP56Time2a(&sysTime,time);

	    ::CloseHandle(hFile);
		hFile = NULL;
	    return true;

	// }}}USR
}
// }}}RME

// {{{RME tool 'OT::Cpp' property 'ImplementationEnding'
// {{{USR

bool FileControl::CreateEmptyFile(const char *pFilePath,const struct CP56Time2a* pFileTime)
{
	//修改文件时间
	HANDLE hFile = CreateFile(pFilePath,GENERIC_READ|GENERIC_WRITE,0,NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	SYSTEMTIME fileSysTime;				//新建文件的系统时间
	memset(&fileSysTime,0,sizeof(fileSysTime));

	FILETIME fileTime;					//新建文件的文件结构时间
	memset(&fileTime,0,sizeof(fileTime));

	//设定文件时间
	fileSysTime.wYear = 2000 + pFileTime->year;
	fileSysTime.wMonth = pFileTime->month;
	fileSysTime.wDay = pFileTime->day;
	fileSysTime.wHour = pFileTime->hours;
	fileSysTime.wMinute = pFileTime->minute;
	fileSysTime.wSecond = pFileTime->milliseconds/1000;

	SystemTimeToFileTime(&fileSysTime,&fileTime);

	SetFileTime(hFile,&fileTime,&fileTime,&fileTime);
	::CloseHandle(hFile);
	hFile = NULL;

	return true;
}

void FileControl::CP56Time2aToSystemTime(struct CP56Time2a& srcTime,SYSTEMTIME& destSysTime)
{
	if(srcTime.IV == false)
	{
		destSysTime.wDay = srcTime.day;
		destSysTime.wHour = srcTime.hours;
		destSysTime.wSecond = srcTime.milliseconds/1000;
		destSysTime.wMilliseconds = srcTime.milliseconds%1000;
		destSysTime.wMinute = srcTime.minute;
		destSysTime.wMonth = srcTime.month;
		destSysTime.wDayOfWeek = srcTime.week;
		destSysTime.wYear = srcTime.year+2000;
	}
	else
	{
		destSysTime.wDay = 01;
		destSysTime.wHour = 0;
		destSysTime.wSecond = 0;
		destSysTime.wMilliseconds = 0;
		destSysTime.wMinute = 0;
		destSysTime.wMonth = 01;
		destSysTime.wDayOfWeek = 1;
		destSysTime.wYear = 1970;
	}
}

void FileControl::SystemTimeToCP56Time2a(const LPSYSTEMTIME pSrcSysTime,struct CP56Time2a* pDestTime)
{
	pDestTime->IV = false;
	pDestTime->day = (unsigned char)pSrcSysTime->wDay;
	pDestTime->hours = (unsigned char)pSrcSysTime->wHour;
	pDestTime->milliseconds = (unsigned short)(pSrcSysTime->wSecond*1000 + pSrcSysTime->wMilliseconds);
	pDestTime->minute = (unsigned char)pSrcSysTime->wMinute;
	pDestTime->month = (unsigned char)pSrcSysTime->wMonth;
	pDestTime->week = (unsigned char)pSrcSysTime->wDayOfWeek;

	pDestTime->year = (unsigned char)(pSrcSysTime->wYear%100);
}

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success,Z_MEM_ERROR if memory could not be
   allocated for processing,Z_STREAM_ERROR if an invalid compression
   level is supplied,Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match,or Z_ERRNO if there is
   an error reading or writing the files. */
int def(FILE *source,FILE *dest,int level)
{
    int ret,flush;
    unsigned have;
    z_stream strm;
	char in[CHUNK] = {0};
	char out[CHUNK] = {0};

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

	strm.data_type = Z_BINARY;
    ret = deflateInit(&strm,level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do {
		strm.avail_in = ::fread(in,1,CHUNK,source);
		strm.total_in = strm.avail_in;
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source)?Z_FINISH:Z_NO_FLUSH;
        strm.next_in = (unsigned char*)(&in[0]);

        /* run deflate() on input until output buffer not full,finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char*)(&out[0]);
            ret = deflate(&strm,flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out,1,have,dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success,Z_MEM_ERROR if memory could not be
   allocated for processing,Z_DATA_ERROR if the deflate data is
   invalid or incomplete,Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match,or Z_ERRNO if there
   is an error reading or writing the files. */
int inf(FILE *source,FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    char in[CHUNK];
    char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in,1,CHUNK,source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = (unsigned char*)(&in[0]);

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = (unsigned char*)(&out[0]);
            ret = inflate(&strm,Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out,1,have,dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret)
{
    fputs("zpipe: ",stderr);
    switch (ret) {
    case Z_ERRNO:
        if (ferror(stdin))
            fputs("error reading stdin\n",stderr);
        if (ferror(stdout))
            fputs("error writing stdout\n",stderr);
        break;
    case Z_STREAM_ERROR:
        fputs("invalid compression level\n",stderr);
        break;
    case Z_DATA_ERROR:
        fputs("invalid or incomplete deflate data\n",stderr);
        break;
    case Z_MEM_ERROR:
        fputs("out of memory\n",stderr);
        break;
    case Z_VERSION_ERROR:
        fputs("zlib version mismatch!\n",stderr);
    }
}

bool FileControl::CompressFile(const char *pSourceFileName,const char *pOutFileName)
{
	int ret = 0;

	FILE *pInFile = ::fopen(pSourceFileName,"rb");
	if( pInFile == NULL)
		return false;

	FILE *pOutFile = ::fopen(pOutFileName,"wb+");
	if(pOutFile == NULL)
		return false;

	ret = def(pInFile,pOutFile,1);

	::fclose(pInFile);
	pInFile = NULL;
	::fclose(pOutFile);
	pOutFile = NULL;

	if (ret != Z_OK)
		false;

	return true;
}

bool FileControl::UnCompressFile(const char *pSourceFileName,const char *pOutFileName)
{
	int ret = 0;

	FILE *pInFile = ::fopen(pSourceFileName,"rb");
	if( pInFile == NULL)
		return false;

	FILE *pOutFile = ::fopen(pOutFileName,"wb+");
	if(pOutFile == NULL)
		return false;

	ret = inf(pInFile,pOutFile);

	::fclose(pInFile);
	pInFile = NULL;
	::fflush(pOutFile);
	::fclose(pOutFile);
	pOutFile = NULL;

	if (ret != Z_OK)
		false;

	return true;
}

// }}}USR
// }}}RME

// }}}RME
