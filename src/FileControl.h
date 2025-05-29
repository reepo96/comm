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
* �ļ����ƣ�FileControl.h
* �ļ���ʶ��
* ժ    Ҫ���ļ�������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
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
	    *�ļ����ͣ�
	    *FT_SHD������¼��
	    *FT_COMTRADE������¼����Ӧ��comtrade�ļ�
	    *FT_SOE������¼�
	    *FT_LOG����־�ļ�
	    *FT_CONT��������¼
	    *FT_CCOMTRADE��������¼��Ӧ��comtrade�ļ�
	    *FT_CFG��¼���������ļ�
		*FT_GENERATORTEST:�����ʵ���ļ�
		*FT_LOW: ����¼���ļ�
		*FT_COMTRADE_DIV:comtrade���ļ�
		*FT_REPORT:��
		*FT_PQSTAT:��������ͳ���ļ�
	    */
	typedef enum FileType {FT_SHD,FT_COMTRADE,FT_SOE,FT_LOG,FT_CONT,FT_CCOMTRADE,FT_CFG,FT_GENERATORTEST,FT_LOW,
		FT_COMTRADE_DIV,FT_REPORT,FT_PQCOMTRADE,FT_PQTRH,FT_PQEVT,FT_PQSTAT} FILETYPE;

	typedef enum OpenMode {OM_READ,OM_WRITE,OM_APPEND} OPENMODE;

	/*************************************************
	    Function:       CreateEmptyFile
	    Description:    �����յ��ļ�
	    Calls:          
	    Called By:      
	    Input:          pFilePath:�ļ�����·��
	                    pFileTime:�ļ�ʱ��
	    Output:         
	    Return:         �ɹ�����true������false
	    *************************************************/
	bool CreateEmptyFile(const char *pFilePath,const struct CP56Time2a* pFileTime);

	/*************************************************
	    Function:       CompressFile
	    Description:    ѹ���ļ�
	    Calls: 
	    Called By:
	    Input:          pSourceFileName:Դ�ļ�
	                    pOutFileName:ѹ���������ļ�
	    Output:
	    Return:         �ɹ�����true������false
	    *************************************************/
	bool CompressFile(const char *pSourceFileName,const char *pOutFileName);

	/*************************************************
	    Function:       UnCompressFile
	    Description:    ��ѹ�ļ�
	    Calls: 
	    Called By:
	    Input:          pSourceFileName:Դ�ļ�
	                    pOutFileName:��ѹ�������ļ�
	    Output:
	    Return:         �ɹ�����true������false
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
	    Description:    �����ļ�����Ŀ¼
	    Calls:          
	    Called By:      
	    Input:          devId:�ļ������豸���
	                    fileName:������·�����ļ���
	                    fileType:�ļ�����
	                    fileYearMonth:�ļ����£����δ֪�ɴ���NULL
	    Output:         
	    Return:         �ɹ������ļ�����Ŀ¼�����򷵻�""
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
	    Description:    ����ļ����Ͷ�Ӧ��Ŀ¼
	    Calls:          
	    Called By:      
	    Input:          devId:�ļ������豸���	                    
	                    fileType:�ļ�����	                    
	    Output:         
	    Return:         �ɹ������ļ����Ͷ�Ӧ��Ŀ¼�����򷵻�""
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
	    Description:    �ж��ļ��Ƿ����
	    Calls:          
	    Called By:      
	    Input:          filePartName:����·�����ļ���
	                    
	    Output:         
	    Return:         ���ڷ���true�����򷵻�false
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
	    Description:    ���ļ�
	    Calls:          
	    Called By:      
	    Input:          filePartName������·�����ļ���
	                    openMode����ģʽ
	    Output:         
	    Return:         ���ڷ���true�����򷵻�false
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
	    Description:    ���ļ�����
	    Calls:          
	    Called By:      
	    Input:          startPos���ļ���ʼλ��
	                    dest�����������������
	                    readLen��Ҫ���ĳ���
	    Output:         
	    Return:         ����ʵ�ʶ������ֽڴ�С������<0����ʾʧ�ܣ�����0��ʾ�ļ��Ѿ�����
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
	    Description:    ��ȡ�ļ��ĳ���
	    Calls:          
	    Called By:      
	    Input:          fileName���ļ���
	                    
	    Output:         
	    Return:         �����ļ���С
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
	    Description:    ����¼���ļ�����ȡ¼������ʱ��
	    Calls:          
	    Called By:      
	    Input:          fileName��¼���ļ���(����·��)
	                    
	    Output:         time�����ص�¼������ʱ��
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ƫ���ļ��α굽ĳһ��λ��
	    Calls:          
	    Called By:      
	    Input:          newPos���µ�ƫ��λ��
	                    
	    Output:         
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ���ļ���д����
	    Calls:          
	    Called By:      
	    Input:          pData��ָ��д���ļ�������
	                    dataLen��д�����ݵĳ���
	    Output:         
	    Return:         ����ʵ��д������ݳ���
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
	    Description:    ���ļ�����
	    Calls:          
	    Called By:      
	    Input:          dest�����������������
	                    readLen��Ҫ���ĳ���
	    Output:         
	    Return:         ����ʵ�ʶ������ֽڴ�С������<0����ʾʧ�ܣ�����0��ʾ�ļ��Ѿ�����
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
	    Description:    ���¼���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          startTime����ѯ��ʼʱ��
	                    endTime����ѯ����ʱ��
	    Output:         fileList�����淵�ص��ļ��б�
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ��ȡ���ص�¼����
	    Calls:          
	    Called By:      
	    Input:          
	                                        
	    Output:         recWaveReport�����ص�¼����
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ��ѯͨ���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          filePath��Ŀ¼����ͨ���
						startTime����ѯ��ʼʱ��
	                    endTime����ѯ����ʱ��
	    Output:         fileList�����淵�ص��ļ��б�
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ��ѯͨ���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          filePath��Ŀ¼����ͨ���
						type����ѯ���ͣ�0-Ŀ¼���ļ���1-Ŀ¼��2-�ļ�
	                    startTime����ѯ��ʼʱ��
	                    endTime����ѯ����ʱ��
	    Output:         fileList�����淵�ص��ļ��б�
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    �����ļ��ļ�����ȡ�ļ�ʱ��
	    Calls:          
	    Called By:      
	    Input:          filePartName���ļ���
						iType��ʱ�����ͣ�1������ʱ�䡢2������ʱ�䡢3���޸�ʱ��
	                                        
	    Output:         time�����ص��ļ�ʱ��
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
