// {{{RME classifier 'Logical View::FileControl'

#ifndef FileControl_H
#define FileControl_H

#ifdef PRAGMA
#pragma interface "FileControl.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：FileControl.h
* 文件标识：
* 摘    要：文件操作类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"
#include "Command.h"
#include <stdio.h>
#include <direct.h>
#include <vector>
using namespace std;

// }}}USR
// }}}RME

class FileControl
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	    /*
	    *文件类型：
	    *FT_SHD：触发录波
	    *FT_COMTRADE：触发录波对应的comtrade文件
	    *FT_SOE：标记事件
	    *FT_LOG：日志文件
	    *FT_CONT：连续记录
	    *FT_CCOMTRADE：连续记录对应的comtrade文件
	    *FT_CFG：录波器配置文件
		*FT_GENERATORTEST:发电机实验文件
		*FT_LOW: 低速录波文件
		*FT_COMTRADE_DIV:comtrade分文件
		*FT_REPORT:简报
		*FT_PQSTAT:电能质量统计文件
	    */
	typedef enum FileType {FT_SHD,FT_COMTRADE,FT_SOE,FT_LOG,FT_CONT,FT_CCOMTRADE,FT_CFG,FT_GENERATORTEST,FT_LOW,
		FT_COMTRADE_DIV,FT_REPORT,FT_PQCOMTRADE,FT_PQTRH,FT_PQEVT,FT_PQSTAT} FILETYPE;

	typedef enum OpenMode {OM_READ,OM_WRITE,OM_APPEND} OPENMODE;

	/*************************************************
	    Function:       CreateEmptyFile
	    Description:    创建空的文件
	    Calls:          
	    Called By:      
	    Input:          pFilePath:文件完整路径
	                    pFileTime:文件时间
	    Output:         
	    Return:         成功返回true，否则false
	    *************************************************/
	bool CreateEmptyFile(const char *pFilePath,const struct CP56Time2a* pFileTime);

	/*************************************************
	    Function:       CompressFile
	    Description:    压缩文件
	    Calls: 
	    Called By:
	    Input:          pSourceFileName:源文件
	                    pOutFileName:压缩后的输出文件
	    Output:
	    Return:         成功返回true，否则false
	    *************************************************/
	bool CompressFile(const char *pSourceFileName,const char *pOutFileName);

	/*************************************************
	    Function:       UnCompressFile
	    Description:    解压文件
	    Calls: 
	    Called By:
	    Input:          pSourceFileName:源文件
	                    pOutFileName:解压后的输出文件
	    Output:
	    Return:         成功返回true，否则false
	    *************************************************/
	bool UnCompressFile(const char *pSourceFileName,const char *pOutFileName);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	void CP56Time2aToSystemTime(struct CP56Time2a& srcTime,SYSTEMTIME& destSysTime);

	void SystemTimeToCP56Time2a(const LPSYSTEMTIME pSrcSysTime,struct CP56Time2a* pDestTime);
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'm_pFile'
	// {{{RME general 'documentation'
	/* {{{USR
		
	   }}}USR */
	// }}}RME
	FILE * m_pFile;
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	FileControl( void );
	// }}}RME
	// {{{RME operation '~FileControl()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~FileControl( void );
	// }}}RME
	// {{{RME operation 'GetFileDirector(unsigned int,const char*,FILETYPE,const char*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileDirector
	    Description:    返回文件所在目录
	    Calls:          
	    Called By:      
	    Input:          devId:文件所属设备编号
	                    fileName:不包含路径的文件名
	                    fileType:文件类型
	                    fileYearMonth:文件年月，如果未知可传入NULL
	    Output:         
	    Return:         成功返回文件所在目录，否则返回""
	    *************************************************
	   }}}USR */
	// }}}RME
	string GetFileDirector( unsigned int devId, const char * fileName, FILETYPE fileType, const char * fileYearMonth = NULL );
	// }}}RME
	// {{{RME operation 'GetFileTypeDirector(unsigned int,FILETYPE)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileTypeDirector
	    Description:    获得文件类型对应的目录
	    Calls:          
	    Called By:      
	    Input:          devId:文件所属设备编号	                    
	                    fileType:文件类型	                    
	    Output:         
	    Return:         成功返回文件类型对应的目录，否则返回""
	    *************************************************
	   }}}USR */
	// }}}RME
	string GetFileTypeDirector( unsigned int devId, FILETYPE fileType );
	// }}}RME
	// {{{RME operation 'IsExist(const char*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsExist
	    Description:    判断文件是否存在
	    Calls:          
	    Called By:      
	    Input:          filePartName:包含路径的文件名
	                    
	    Output:         
	    Return:         存在返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool IsExist( const char * fileName );
	// }}}RME
	// {{{RME operation 'OpenFile(const char*,OPENMODE)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       OpenFile
	    Description:    打开文件
	    Calls:          
	    Called By:      
	    Input:          filePartName：包含路径的文件名
	                    openMode：打开模式
	    Output:         
	    Return:         存在返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool OpenFile( const char * fileName, OPENMODE openMode );
	// }}}RME
	// {{{RME operation 'CloseFile()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	bool CloseFile( void );
	// }}}RME
	// {{{RME operation 'ReadData(int,char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ReadData
	    Description:    读文件数据
	    Calls:          
	    Called By:      
	    Input:          startPos：文件起始位置
	                    dest：保存读出来的数据
	                    readLen：要读的长度
	    Output:         
	    Return:         返回实际读出的字节大小。返回<0，表示失败；返回0表示文件已经读完
	    *************************************************	
	   }}}USR */
	// }}}RME
	int ReadData( int startPos, char * dest, int readLen );
	// }}}RME
	// {{{RME operation 'GetFileLen(const char*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileLen
	    Description:    获取文件的长度
	    Calls:          
	    Called By:      
	    Input:          fileName：文件名
	                    
	    Output:         
	    Return:         返回文件大小
	    *************************************************	
	   }}}USR */
	// }}}RME
	unsigned int GetFileLen( const char * fileName );
	// }}}RME
	// {{{RME operation 'GetRecWaveFaultTime(const char*,struct CP56Time2a*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFaultTime
	    Description:    根据录波文件名获取录波故障时间
	    Calls:          
	    Called By:      
	    Input:          fileName：录波文件名(包含路径)
	                    
	    Output:         time：返回的录波故障时间
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool GetRecWaveFaultTime( const char * fileName, struct CP56Time2a * time );
	// }}}RME
	// {{{RME operation 'Seek(unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Seek
	    Description:    偏移文件游标到某一个位置
	    Calls:          
	    Called By:      
	    Input:          newPos：新的偏移位置
	                    
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool Seek( unsigned int newPos );
	// }}}RME
	// {{{RME operation 'WriteData(const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       WriteData
	    Description:    往文件中写数据
	    Calls:          
	    Called By:      
	    Input:          pData：指向写入文件的数据
	                    dataLen：写入数据的长度
	    Output:         
	    Return:         返回实际写入的数据长度
	    *************************************************
	   }}}USR */
	// }}}RME
	int WriteData( const char * pData, int dataLen );
	// }}}RME
	// {{{RME operation 'ReadData(char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       ReadData
	    Description:    读文件数据
	    Calls:          
	    Called By:      
	    Input:          dest：保存读出来的数据
	                    readLen：要读的长度
	    Output:         
	    Return:         返回实际读出的字节大小。返回<0，表示失败；返回0表示文件已经读完
	    *************************************************
	   }}}USR */
	// }}}RME
	int ReadData( char * dest, int readLen );
	// }}}RME
	// {{{RME operation 'GetRecWaveFileList(unsigned int,struct CP56Time2a,struct CP56Time2a,vector<PCMDFILELIST>&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    获得录波文件列表
	    Calls:          
	    Called By:      
	    Input:          startTime：查询开始时间
	                    endTime：查询结束时间
	    Output:         fileList：保存返回的文件列表
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetRecWaveFileList( unsigned int devId, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< PCMDFILELIST > & fileList );
	// }}}RME
	// {{{RME operation 'GetRecWaveReport(PCMDREPORT)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveReport
	    Description:    获取本地的录波简报
	    Calls:          
	    Called By:      
	    Input:          
	                                        
	    Output:         recWaveReport：返回的录波简报
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetRecWaveReport( PCMDREPORT recWaveReport );
	// }}}RME
	// {{{RME operation 'GetFileList(char*,struct CP56Time2a,struct CP56Time2a,vector<PCMDGENERALFILEINF>&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileList
	    Description:    查询通用文件列表
	    Calls:          
	    Called By:      
	    Input:          filePath：目录名及通配符
						startTime：查询开始时间
	                    endTime：查询结束时间
	    Output:         fileList：保存返回的文件列表
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetFileList( char * filePath, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< PCMDGENERALFILEINF > & fileList );
	// }}}RME
	// {{{RME operation 'GetFileList(char*,int,struct CP56Time2a,struct CP56Time2a,vector<GENERALFILEPATHINF>&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileList
	    Description:    查询通用文件列表
	    Calls:          
	    Called By:      
	    Input:          filePath：目录名及通配符
						type：查询类型：0-目录及文件，1-目录，2-文件
	                    startTime：查询开始时间
	                    endTime：查询结束时间
	    Output:         fileList：保存返回的文件列表
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetFileList( char * filePath, int type, struct CP56Time2a startTime, struct CP56Time2a endTime, vector< GENERALFILEPATHINF > & fileList );
	// }}}RME
	// {{{RME operation 'GetFileTime(const char*,struct CP56Time2a*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFileTime
	    Description:    根据文件文件名读取文件时间
	    Calls:          
	    Called By:      
	    Input:          filePartName：文件名
						iType：时间类型，1：创建时间、2：访问时间、3：修改时间
	                                        
	    Output:         time：返回的文件时间
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool GetFileTime( const char * filePartName, struct CP56Time2a * time,int iType=1 );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* FileControl_H */

// }}}RME
