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
* �ļ����ƣ�DataControl.h
* �ļ���ʶ��
* ժ    Ҫ�����ݿ����࣬�������ݵĹ���
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2008��3��12��
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

	//¼���ļ���Ϣ
	typedef struct tagFileInfo
	{
		unsigned char devID;        //�ļ�¼��װ�õ�ַ
		char fileName[40];          //�ļ����ƣ�������չ����
		struct CP56Time2a faultTime;//�ļ�����ʱ��
		unsigned int fileLen;		//�ļ���С
	}FILEINFO,*PFILEINFO;

	//�㽭¼���򱨸�ʽ
	typedef struct tagZJReport
	{
		char recWaveFileName[40];       //������չ����¼���ļ���
		char BASName[40];               //���ϼ����

		struct tagFPT
		{
			bool    A_Fault:1;      //A�����
			bool    B_Fault:1;      //B�����
			bool    C_Fault:1;      //C�����
			bool    isEarthing:1;   //�Ƿ��ǽӵع���
			bool    isOutArea:1;    //�Ƿ���������ϣ�true��������ϣ�false�����ڹ���
			char    res:2;          //����
			bool    isValid:1;      //��Чλ
		} FPT;//�����������

		struct tagJPT
		{
			bool    A_Jump:1;      //A����բ
			bool    B_Jump:1;      //B����բ
			bool    C_Jump:1;      //C����բ
			char    res:4;          //����
			bool    isValid:1;      //��Чλ
		} JPT;//��բ���

		unsigned char SCL[4];       //���Ͼ���
		unsigned char DPI;			//�غ�բ
		unsigned char DPI_TimeL;	//�غ�բʱ�������ֽ�
		unsigned char DPI_TimeH;	//�غ�բʱ�������ֽ�

		struct CP56Time2a faultTime;    //����ʱ��
	}ZJREPORT,*PZJREPORT;


	//�������ݶ���
	class DataQue
	{
	protected:
		CRITICAL_SECTION criticalSection;

		int startPos;	//��Ч������ʼλ��
		int endPos;		//��Ч���ݽ���λ��
		int bufLen;

		char *pData;
		bool bIsCancle; //�Ƿ��˳�����

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

		//���������ֱ���ɹ�������*pIsStop = true
		void push(const char* pSrcData,int iDataLen,bool *pIsStop);

		//���ݳ�����
		//����ʵ�ʳ����еĸ���
		int pop(char *pDestBuf,int iBufLen);

		bool IsEmpty();
		bool IsFull();
	};

	static DataQue* CreateDataQue(int iSize);

	//���ϼ򱨹�����
	class CAnalyReportManager
	{
	public:
		//*********************************************
		//Function:   BOOL WriteReportTXT( CHAR * pstrFileName )
		//Description:���ɹ��ϱ����ı��ļ�.TXT
		//Input:      CHAR * pstrFileName���ļ���
		//Output:     ��
		//Return:     BOOL���ɹ�ΪTrue��ʧ��ΪFalse
		//*********************************************
		//##ModelId=479C018B0255
		//BOOL WriteReportTXT( const CHAR * pstrFileName ,ReportFileHead rptHead, Report * rpt);

	private:
		//void WriteLineFaultXml1(FILE *fp,Report rpt);
		//*********************************************
		//Function:   BOOL WriteLineFault( FILE * fp, Report rpt )
		//Description:дһ�����ϼ�¼���ļ�fp��
		//Input:      FILE * fp���ļ�ָ�룬Report
		//rpt�����ϱ����¼
		//Output:     ��
		//Return:     BOOL���ɹ�ΪTrue��ʧ��ΪFalse
		//*********************************************
		//##ModelId=479C018B0282
		//BOOL WriteLineFault( FILE * fp, Report rpt );
		//*********************************************
		//Function:   void GetTripPhaseName( WORD wTripPhase,
		//CHAR * pstrName )
		//Description:�õ���բ����Ӧ������
		//Input:      WORD wTripPhase����բ���
		//Output:     CHAR * pstrName����բ�������
		//Return:     ��
		//*********************************************
		//##ModelId=479C018B0292
		void GetTripPhaseName( WORD wTripPhase, CHAR * pstrName );
		//*********************************************
		//Function:   void GetFaultTypeName( WORD wFaultType,
		//CHAR * pstrName )
		//Description:�õ��������Ͷ�Ӧ������
		//Input:      WORD wFaultType����������
		//Output:     CHAR * pstrName��������������
		//Return:     ��
		//*********************************************
		//##ModelId=479C018B02A1
		void GetFaultTypeName( WORD wFaultType, CHAR * pstrName );
	public:
		//void WriteXmlType1(FILE *fp, ReportFileHead rptHead, Report * rpt);
		//*********************************************
		//Function:   CAnalyReportManager();
		//Description:���캯������ʼ��
		//Input:      �� 
		//Output:     ��
		//Return:     ��
		//*********************************************
		//##ModelId=479C018B02B1
		CAnalyReportManager();
		//*********************************************
		//Function:   ~CAnalyReportManager();
		//Description:��������
		//Input:      �� 
		//Output:     ��
		//Return:     ��
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
	    Description:    ���ַ���ʱ��ת����CP56Time2aʱ��
	    Calls:
	    Called By:
	    Input:          pStringTime:�ַ���ʱ�䣬��ʽyyyy-mm-dd HH:MM:ss.sss
						
	    Output:         destTime��ת�����ʱ��
	    Return:
	    *************************************************/
	void StringTime2CP56Time2a(const char *pStringTime,struct CP56Time2a& destTime);

	/*************************************************
	    Function:       RptStruct2CmdReport
	    Description:    �����ݿ�򱨷�����ʽת��Э��򱨸�ʽ
	    Calls:
	    Called By:
	    Input:

	    Output:
	    Return
	    *************************************************/
	bool RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport );

	/*************************************************
	    Function:       AnalyzResult2CmdReport
	    Description:    �ӹ��Ϸ������ת��Э��򱨸�ʽ
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
	    Description:    ���¼���ļ��б�
	    Calls:
	    Called By:
	    Input:          devId:�豸id��-1��ʾ�����豸
						startTime����ѯ��ʼʱ��
	                    endTime����ѯ����ʱ��
						type:1-ֻ��ѯ�й��ϵ�¼���ļ��б�,2-��ѯ���е�¼���ļ��б�
	    Output:         fileList�����淵�ص��ļ��б�
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ��ñ���¼��б��б�
	    Calls:
	    Called By:
	    Input:          devId:�豸id��-1��ʾ�����豸
						startTime����ѯ��ʼʱ��
	                    endTime����ѯ����ʱ��
	    Output:         fileList�����淵�صı���¼��б�
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
	    Description:    ����¼���ļ�����ȡ���ϼ�(�㽭)
	    Calls:
	    Called By:
	    Input:          devId���豸id
						sFileName:������չ����¼���ļ�·����
						ppReportData�����ϼ�����
						TrgTypeNum������ԭ��
	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
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
