// {{{RME classifier 'Logical View::Command'

#ifndef Command_H
#define Command_H

#ifdef PRAGMA
#pragma interface "Command.h"
#endif

#include <RTSystem/Communication.h>
#include <CmdState.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Command.h
* �ļ���ʶ��
* ժ    Ҫ����ϵͳ�ڲ�ʹ�õ�����ṹ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#pragma warning( disable : 4786 )

#include "public.h"

#include "dfrdef.h"

//#include "SHType.h"
#include <vector>
using namespace std;

//���������ڻ��߷����ĸ�����
typedef enum CmdDir{CD_REMOTE,CD_UI,CD_TIMER,CD_ASDU/*��̬̬�豸*/,CD_CONST_DAU/*��̬�豸*/,CD_NEURON} CMDDIR;

//CP56Time2aʱ���ʽ
struct CP56Time2a
{
    unsigned short milliseconds;    //����
    unsigned char minute:6;         //����
    bool          res1:1;           //����
    bool          IV:1;             //�Ƿ���־
    unsigned char hours:5;          //Сʱ
    char          res2:2;
    bool          SU:1;             //����ʱ��־
    unsigned char day:5;            //����
    unsigned char week:3;           //����(1-7)
    unsigned char month:4;
    unsigned char res3:4;
    unsigned char year:7;
    bool          res4:1;
};


//�������ͣ�����������Ĳ�������������ִ�к���������ݵ�����
//CDT_TIME��ʱ�����ͣ�ʹ��CP56Time2a�ṹ��CDT_CHAR��һ����ַ����ͣ�CDT_INT��4�ֽڵ��޷�����
//CDT_SHTIME��ʱ�����ͣ�ʹ��SHTimeStruct�ṹ
typedef enum CmdDataType{CDT_TIME,CDT_INT,CDT_CHAR,CDT_SHTIME} CMDDATATYPE;

//��������ĸ�ʽ
typedef struct tagCmdParam
{
    CMDDATATYPE paramType;
    unsigned char paramLen;
    unsigned char data[256];
}CMDPARAM,*PCMDPARAM;

/**********����ִ�н������-->********************/

//�����������
//CDRT_TRUFALSE��������ͣ���ʾ����ִ���Ƿ�ɹ���
//CDRT_CHAR��һ����ַ����ͣ�
//CDRT_FILELIST���ļ����б�
//CDRT_TIME��ʱ������
//CDRT_REPORT��¼���򱨽����
//CDRT_INT��4�ֽڵ��޷�������
//CDRT_PROGRESS��������Ϣ��0~100��
//CDRT_DIRFILEINF��CMDDIRFILEINF�ṹ�����ݣ�
//CDRT_DESC��������Ϣ��ͨ���Ǵ���������Ϣ��
//CDRT_POINTDATA�����������ݸ�ʽ��
//CDRT_FILEPATH���ļ�·����
//CDRT_STARTRECMSG������¼��������Ϣ
//CDRT_MARKEVENT:����¼���Ϣ
//CDRT_FILENAME:�ļ�����
//CDRT_PQ_TRENDDATA�����������������ݣ� 
//CDRT_PQ_EVENT�����������¼���
//CDRT_PQ_FILEINF���������������Ϣ
typedef enum CmdResultType{CDRT_TRUFALSE,CDRT_CHAR,CDRT_FILELIST,CDRT_TIME,CDRT_REPORT,CDRT_INT,CDRT_PROGRESS,\
CDRT_DIRFILEINF,CDRT_DESC,CDRT_POINTDATA,CDRT_FILEPATH,CDRT_STARTRECMSG,CDRT_MARKEVENT,CDRT_FILENAME,\
CDRT_PQ_TRENDDATA,CDRT_PQ_EVENT,CDRT_PQ_FILEINF,CDRT_PQ_10CYCDATA,CDRT_PQ_STATINF,CDRT_IP,CDRT_USER_INFO} CMDRESULTTYPE;

struct SHTimeStruct
{ 
	WORD	Year;		//��
	BYTE	Month;		//��
	BYTE	Day;		//��
	BYTE	Hour;		//ʱ
	BYTE	Minute;		//��
	BYTE	Second;		//��
	WORD	Millisecond;//����0.001s
	WORD	Microsecond;//΢��0.001*0.001
};

//����ִ�н����ʽ
typedef struct tagCmdResult
{
    CMDRESULTTYPE resultType;   //�����������
    unsigned int resultLen;   //�������
    char *pData;                //�������	
}CMDRESULT,*PCMDRESULT;

typedef struct check
{
	/*
	unsigned int 	time_sec;		//�����Լ���Ϣ������ʱ��(��)
	unsigned int 	time_usec;		//�����Լ���Ϣ������ʱ�䣬������(΢��)
	unsigned int	dsp_ver;		//DSP�汾
	unsigned int	ram_ver;		//RAM�汾
	unsigned int	mm_ver;			//��̬��ذ汾
	unsigned int	md_ver;			//��̬��ذ汾
	unsigned int	dsp_err_no;		//dsp����������
	unsigned int	ram_err_no;		//ram����������
	unsigned int	mm_err_no;		//��̬����������
	unsigned int	md_err_no;		//��̬����������	
	unsigned int	intio_state;	//����ӿڵ�״̬����16λ��Ч
	unsigned int	outio_state;	//����ӿڵ�״̬����16λ��Ч
	unsigned int	gps_err_no;		//GPS������
	unsigned int	reservation1;	//����
	unsigned int	reservation2;	//����
	*/
	unsigned int 	time_sec;		//�����Լ���Ϣ������ʱ��(��)
	unsigned int 	time_usec;	//�����Լ���Ϣ������ʱ�䣬������(΢��)
	unsigned int	dsp_ver;		//DSP�汾
	unsigned int	ram_ver;		//RAM�汾
	unsigned int	 Module_ver[16];	//��ذ��ģ��汾��Ϊ0��ʾ�޴�ģ�飺
								// Module_ver[0]����̬ģ��汾
								// Module_ver[1]����̬ģ��汾
								// Module_ver[2]����������ģ��汾
								// Module_ver[3]��PMUģ��汾
								// Module_ver[4]��ͨѶģ��汾
								//������Ԥ��
	unsigned int	dsp_err_no;		//dsp���������룬��2.2.1��
	unsigned int	ram_err_no;		//ram����������
	unsigned int	m_err_no;		//��س���������	
	unsigned int	intio_state;	//����ӿڵ�״̬����16λ��Ч
	unsigned int	outio_state;	//����ӿڵ�״̬����16λ��Ч
	unsigned int	gps_err_no;		//GPS�����룬��2.2.1��
	unsigned int	reservation1;	//����
	unsigned int	reservation2;	//����

}CHECK;

//¼���ļ����б��ʽ
typedef struct tagCmdFileList
{
    unsigned char devID;        //�ļ�¼��װ�õ�ַ
    char fileName[40];          //�ļ����ƣ�������չ����
    struct CP56Time2a faultTime;    //�ļ�����ʱ��
}CMDFILELIST,*PCMDFILELIST;

//ͨ���ļ���Ϣ��ʽ
typedef struct tagCmdGeneralFileInf
{
    char fileName[100];         //�ļ����ƣ����԰���Ŀ¼��
    struct CP56Time2a fileTime; //�ļ�ʱ��
    unsigned int fileLen;		//�ļ�����
}CMDGENERALFILEINF,*PCMDGENERALFILEINF;

//ͨ���ļ�/Ŀ¼��Ϣ��ʽ
typedef struct tagGeneralFilePathInf
{
    char fileName[100];         //�ļ�/Ŀ¼����
    struct CP56Time2a fileTime; //�ļ�ʱ��
	unsigned char isDir;		//�Ƿ���Ŀ¼��1��Ŀ¼��0����
    unsigned int fileLen;		//�ļ�����
}GENERALFILEPATHINF,*PGENERALFILEPATHINF;

//Ŀ¼�ļ���Ϣ��ʽ
typedef struct tagCmdDirFileInf
{
    int fileNo;         //�ļ���
    char fileTime[25]; //�ļ�ʱ��,yyyy-mm-dd hh:MM:ss��ʽ
    char fileName[100]; //�ļ�����
    int fileLen;        //�ļ�����
}CMDDIRFILEINF,*PCMDDIRFILEINF;

//�豸״̬
typedef struct tagDeviceState
{
	unsigned int devId;
	unsigned char state;// 0��δ֪״̬�� 1������״̬�� 2������״̬�� 3����Ч״̬
}DAVICESTATE,*PDAVICESTATE;

//¼���򱨸�ʽ
typedef struct tagCmdReport
{
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

	struct CP56Time2a faultTime;    //����ʱ��
	char recWaveFileName[40];       //������չ����¼���ļ���
	char BASName[40];               //���ϼ����
	char res[100];                  //����
}CMDREPORT,*PCMDREPORT;

//¼����ͷ
typedef struct tagReportHead
{
    unsigned char devID;        //�ļ�¼��װ�õ�ַ
    int            rptID;       //¼���ļ�id
    CMDREPORT      report;      //¼����
}REPORTHEAD,*PREPORTHEAD;

//�������Ϣ�ṹ
typedef struct tagPackInfo
{
	char flag[2]; //��־
	char msgType; //��Ϣ����(1��ʵʱ��ʾ��2����Ư����)
	unsigned short packNo; //�����(ÿ����һ)
	unsigned short frameNo; //֡�ţ�ÿ������һ֡��Ϊ1�����к�������0Ϊ���һ֡��
}PACKINFO,*PPACKINFO;

//ԭʼ����������ͷ
typedef struct tagOrigPointDataHead
{
    unsigned int  ucSendCountCycle; //Ҫ���͵��ܲ�����
	unsigned int  ucPerCyclePoint;  //ÿ�ܲ���������
	unsigned int wPerSampleSize; //ÿ��������ռ���ֽ���
	unsigned int ucTotalSw;        //������ͨ������
	unsigned int ucTotalAn;        //ģ��ͨ������
	unsigned int wReserve; //�����ֽ�,��ʱ����
}ORIGPOINTDATAHEAD,*PORIGPOINTDATAHEAD;


typedef struct tagRtDataHead
{
    unsigned long  dwSendCountCycle;	//Ҫ���͵��ܲ�����
    unsigned long  dwPerCyclePoint; 	//ÿ�ܲ���������
    unsigned long  dwTotalAn;      	//ģ��ͨ������
    unsigned long  dwTotalSw;      	//������ͨ��������
}RTDATAHEAD,*PRTDATAHEAD;

//����������ͷ
typedef struct tagPointDataHead
{
	struct SHTimeStruct time;   //ʱ��
	unsigned char ChannelNumber;//ͨ����
	unsigned char SwitchNumber; //������������16��������Ϊ1��
	unsigned int Chan[3];       //��Ҫʵʱ��ʾ��ģ��ͨ��,1ȡ��0��ȡ
	float samplrate;    //������
	unsigned int point;         //point
}POINTDATAHEAD,*PPOINTDATAHEAD;

//����¼�
typedef struct tagMarkEvent
{
	char State;		//״̬��1����������3��������
	unsigned char DAUID;	//�豸���
	unsigned int MarkId;			//�¼����
	struct SHTimeStruct StartTime;	//�¼���ʼʱ��
	struct SHTimeStruct EndTime;	//�¼�����ʱ��
	char FaultComponent[20];		//�����豸
	int MarkEvtType;				//�¼�����
	float	CurrentVal;				//����ֵ
	char Describe[40];				//����
	unsigned int FileLen;			//�ļ���С
	char FileName[100];				//�ļ���
}MARKEVENT,*PMARKEVENT;

/**********����ִ�н������<--********************/

//��������ܴ���

//��ѯ¼��������״̬��Ϣ
const   unsigned char   CMD_GETDAUINF = 0X01;

//��������ADU��ʱ�䣬����Զ�����ñ��ص�ʱ��
//������Ҫ����1��������
//  1��ʱ�䣬CDT_TIME����
const   unsigned char   CMD_SETTIME = 0X02;

//��ADU��ȡʱ��
//������Ҫ����1��������
//  1��ʱ�����ͣ�CDT_CHAR���ͣ���
//          1����MONITORʱ��
//          2����DCRʱ��
//          3����ʱ��оƬʱ��
//          4����ϵͳʱ�䡣��Ĭ�ϣ�
const   unsigned char   CMD_GETTIME = 0X03;


//��ȡ���ϼ�����
//������������
const   unsigned char   CMD_GETREPORTFILE = 0X05;

//���¼���ļ��б�
//������Ҫ����2��������
//  1���ļ���ʼʱ�䣬CDT_TIME����
//  2���ļ�����ʱ�䣬CDT_TIME����
const   unsigned char   CMD_GETWAVEFILELIST = 0X06;

//���ͨ���ļ��б�
//����Զ�̴�����Ҫ����3��������
//  1���ļ���ʼʱ�䣬CDT_TIME����
//  2���ļ�����ʱ�䣬CDT_TIME����
//  3��һ���ļ����ƣ�Ŀ¼����ͨ���(100���ֽ�)��
//����DAU������Ҫ����2��������
//  1��Ŀ¼���ͣ�CDT_CHAR����
//          90������¼��Ŀ¼
//          91�������ռ�Ŀ¼
//          92������¼�Ŀ¼
//          93������¼��Ŀ¼
//          94����������Ŀ¼
//          95����̬��¼Ŀ¼
//          96����������Ŀ¼
//  2���ļ�����������ļ�����0��ʾĿ¼�����е��ļ�
const   unsigned char   CMD_GETFILELIST = 0X07;

//���¼���ļ�
//Զ�̵��ô�����Ҫ����2��������
//  1���ļ����ƣ�������չ����40�ֽڣ�
//  2����ʼ����λ�ã�CDT_INT����
//
//SHNeuron���ô�����Ҫ����3��������
//  1��¼���ļ����ͣ�1�ֽڣ���1-����¼�� 2-����¼��
//  2���ļ����ƣ�������չ����40�ֽڣ�
//  3����ʼ����λ�ã�CDT_INT����
const   unsigned char   CMD_GETWAVEFILE = 0X08;

//�Զ��ϴ�¼���ļ���Զ��
//������Ҫ����1��������
//  1���ļ����ƣ�������չ����
const   unsigned char   CMD_SENDWAVEFILE = 0X09;

//�ֶ�����¼��
//��������1������
//  ����1���������ͣ�1�ֽڣ���1-����¼�� 2-����¼��
const   unsigned char   CMD_STARTREC_MANUAL = 0X0A;

//��ʱ����¼��
//��������4������
//  ����1���������ͣ�1�ֽڣ���1-����¼�� 2-����¼��
//  ����2��������ʼʱ�䣺SHTimeStruct�ṹ
//  ����3�����1�ֽڣ�������¼��ʱ�ĵ�λΪ�룬�������5�룻����¼��ʱ�ĵ�λΪ�֣������趨���ټ�¼ʱ�䳤��
//  ����4������������2�ֽڣ�
const   unsigned char   CMD_STARTREC_TIMER = 0X0B;

//��ȡͨ���ļ�
//��������6������
//  ����1���ļ����ͣ�1�ֽڣ���
//                      1��SHW��ʽ
//                      2��SHD��ʽ
//                      3��SWR��ʽ
//                      4��TRM��ʽ
//                      5��COMTRAD_CFG��ʽ
//                      6��COMTRAD_DAT��ʽ
//                      7��COMTRAD_HDR��ʽ
//                      8�����ϱ����ļ�
//                      100������ժҪ�ļ�
//                      101������¼��ļ�
//                      102�������ռ��ļ�
//                      200��DSP����¼���ļ�
//                      210��ȫ�������ļ�
//                      211����·�����ļ�
//                      212�����������ļ�
//  ����2���ļ��ţ�4�ֽڣ����ļ�����Ϊ1-80ʱ���ļ���Ϊ���ļ���Ψһ��ʶ�ţ�
//                          ����Ϊ100-110ʱ���ļ��ű�ʾ��ǰ�ļ����������0���ʾȫ������
//  ����3���ļ���ʼλ�ã�4�ֽڣ����ļ�����Ϊ1-80ʱ��Ч
//  ����4���ļ����ȣ�����δ֪������0
//  ����5���ļ����ƣ�100���ֽ�δʹ���ֽڲ�0
//  ����6���ļ�ʱ�䣺�ַ���,yyyy-mm-dd hh:MM:ss��ʽ
const   unsigned char   CMD_PULLFILE = 0X0C;

//�´��ļ�
//��������4������
//  ����1���ļ����ͣ�1�ֽڣ���
//                      20��ȫ�������ļ�
//                      21����·�����ļ�
//                      22�����������ļ�
//  ����2���û����ƣ�20�ֽڣ�
//  ����3�����루10�ֽڣ�
//  ����4���ļ����ƣ�100�ֽڣ�
const   unsigned char   CMD_PUSHFILE = 0X0D;


//�����ϴ�
//��������2������
//  ����1�����ͣ�1�ֽڣ���
//                      1������¼������
//                      2������¼��ļ�
//  ����2���ļ����ƣ�100�ֽڣ�
const   unsigned char   CMD_AUTOSEND = 0X0E;

//��ʵʱ����
//��������2������
//  ����1�����ͣ�1�ֽڣ���
//                      1����������
//                      2����������
//  ����2��������־��1�ֽڣ���
//                      1������
//                      2��ֹͣ
const   unsigned char   CMD_READ_RLT_DATA = 0X0F;

//����¼��������Ϣ(DAU�Զ��ϴ���Ϣ�Ĺ���)
const   unsigned char   CMD_STARTINFO = 0X10;

//����״̬��Ϣ(DAU�Զ��ϴ���Ϣ�Ĺ���)
const   unsigned char   CMD_RUNINFO = 0X11;

//ֹͣ¼��
const   unsigned char   CMD_STOPREC = 0X12;

//����̬��¼����
//��������3������
//  ����1�����ͣ�1�ֽڣ���
//                      1����̬��¼����
//                      2�����Ƽ�¼����
//  ����2����ʼʱ�䣨4�ֽڣ�
//  ����3�����ݼ��ʱ�䣨4�ֽڣ�
const   unsigned char   CMD_GET_CONT_DATA = 0X13;

//����Ư���̶�
//��������1������
//  ����1�����ͣ�1�ֽڣ���
//                      1����������
//                      2����������
const   unsigned char   CMD_READ_ZEROOFFSET = 0X14;

//��װ��״̬
//��������1������
//  ����1�����ͣ�1�ֽڣ���
//                      1��ȫ������
//                      2������¼��
//                      4������¼��
//                      8����̬��¼
//                      16��PMU
//                      32����������
const   unsigned char   CMD_GET_DEVICE_STATE = 0X15;

//����װ��״̬
//��������3������
//  ����1�����ͣ�1�ֽڣ���
//                      1��ȫ������
//                      2������¼��
//                      4������¼��
//                      8����̬��¼
//                      16��PMU
//                      32����������
//  ����2��״̬��1�ֽڣ���
//                      1������״̬
//                      2������״̬����Ӧ���͹���ֹͣ��
//  ����3����2�ֽڣ����ӵ���״̬��������״̬��ʱ�䣬��λ���ӡ���״̬������Ϊ2ʱ������
const   unsigned char   CMD_SET_DEVICE_STATE = 0X16;

//Ӳ������
//��������2������
//  ����1�����ͣ�1�ֽڣ���
//                      1���澯����
//                      2����������
//                      3���Լ�����(�ڵ���״̬�¿���)
//  ����2�����Ͳ�����4�ֽڣ�
const   unsigned char   CMD_OPERATE_DEVICE = 0X17;

//�Զ��ϴ�����¼�(DAU�Զ��ϴ���Ϣ�Ĺ���)
const   unsigned char   CMD_AUTOSEND_MARKEVENT = 0X18;

//�������ϼ�
const   unsigned char   CMD_CREATEREPORTFILE = 0X19;
//������������չ�����ļ�·����512�ֽڣ�

//�Զ��ϴ����ϼ�
const   unsigned char   CMD_AUTOSEND_REPORTFILE = 0X1A;
//���������ϼ��ļ�����

//��ȡ����¼����ͨ���ļ�
const   unsigned char   CMD_GET_SPLIT_WAVEFILE = 0X1B;

//��ȡ��ذ���������
const   unsigned char   CMD_GET_NET_CFG = 0X1C;

//���ü�ذ���������
const   unsigned char   CMD_SET_NET_CFG = 0X1D;

//��ȡ����ļ�
const   unsigned char   CMD_GET_AUDIT_FILE = 0X1E;

//������Ƹ澯
const   unsigned char   CMD_SEND_AUDIT_ALARM = 0X1F;

//�û���¼
const   unsigned char   CMD_USER_LOGIN = 0X20;


//ͨѶģ���˳�
//�޲���
const   unsigned char   CMD_EXIT = 0X50;

//�ؽ�Զ��ͨѶ�������
//�޲���
const   unsigned char   CMD_REBUILD_RMT = 0X51;

//�ؽ�DAUͨѶ�������
//�޲���
const   unsigned char   CMD_REBUILD_DAU = 0X52;

//�ؽ�������ʵ��ͨѶ�޹�����
//�޲���
const   unsigned char   CMD_REBUILD_OTHER = 0X53;

//��ѯĳ����·��״̬
//�޲���
const   unsigned char   CMD_QUERY_LINKSTATE = 0X54;

//�Զ��ϴ���·��״̬
const   unsigned char   CMD_AUTOSEND_LINKSTATE = 0X55;

//�����豸
const   unsigned char   CMD_CONNECT_DEVICE = 0X56;

//�Ͽ��豸
const   unsigned char   CMD_DISCONNECT_DEVICE = 0X57;

//��̨�����µĹ��ϼ򱨣�����֪ͨͨѶģ��ȥ��ȡ�µĹ��ϼ�
//��������1������
//  ����1���ļ��ţ�int 4�ֽڣ�
const   unsigned char   CMD_NEW_FAULTREPORT = 0X60;

//��ʼ�����ʵ��
const   unsigned char   CMD_STARTGNEXPERIMENT = 0X61;

//ֹͣ�����ʵ��
const   unsigned char   CMD_STOPGNEXPERIMENT = 0X62;
// }}}USR
// }}}RME

struct Command
{
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
	public:
	unsigned char	processType;	//��������Ľ��̵�����
	unsigned int	processId;		//��������Ľ���id
	bool			isSplitCmd;		//�Ƿ��Ƿ�����(һ������ֳ���������)

	//��¡����
	void Clone(struct Command *pNewCmd);
	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME classAttribute 'msgId'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	int msgId;
	// }}}RME
	// {{{RME classAttribute 'from'
	// {{{RME general 'documentation'
	/* {{{USR
	�����������ĸ�����Remote,UI,Timer,ASDU
	   }}}USR */
	// }}}RME
	CMDDIR from;
	// }}}RME
	// {{{RME classAttribute 'to'
	// {{{RME general 'documentation'
	/* {{{USR
	������ĸ�����Remote,UI,Timer,ASDU
	   }}}USR */
	// }}}RME
	CMDDIR to;
	// }}}RME
	// {{{RME classAttribute 'destDevId'
	// {{{RME general 'documentation'
	/* {{{USR
	Ŀ���豸id�����վ���(�м�2�ֽ�)+�豸���(��1�ֽ�),���1�ֽ��ݲ�ʹ��
	   }}}USR */
	// }}}RME
	unsigned int destDevId;
	// }}}RME
	// {{{RME classAttribute 'funCode'
	// {{{RME general 'documentation'
	/* {{{USR
	���ܴ���
	   }}}USR */
	// }}}RME
	unsigned char funCode;
	// }}}RME
	// {{{RME classAttribute 'parmLen'
	// {{{RME general 'documentation'
	/* {{{USR
	��������ܳ���
	   }}}USR */
	// }}}RME
	unsigned short parmLen;
	// }}}RME
	// {{{RME classAttribute 'pParm'
	// {{{RME general 'documentation'
	/* {{{USR
	�����������	    
	   }}}USR */
	// }}}RME
	char * pParm;
	// }}}RME
	// {{{RME classAttribute 'resultNum'
	// {{{RME general 'documentation'
	/* {{{USR
	���ؽ���ṹ�ĸ���
	   }}}USR */
	// }}}RME
	unsigned short resultNum;
	// }}}RME
	// {{{RME classAttribute 'resultList'
	// {{{RME general 'documentation'
	/* {{{USR
	����б�
	   }}}USR */
	// }}}RME
	vector< PCMDRESULT > resultList;
	// }}}RME
	// {{{RME classAttribute 'describe'
	// {{{RME general 'documentation'
	/* {{{USR
		��������
	   }}}USR */
	// }}}RME
	char describe[ 64 ];
	// }}}RME
	// {{{RME associationEnd '_state'
	CmdState _state;
	// }}}RME
	// {{{RME operation 'Command()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Command( void );
	// }}}RME
	// {{{RME operation '~Command()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~Command( void );
	// }}}RME
	// {{{RME operation 'operator==(const Command &)'
	// {{{RME general 'documentation'
	/* {{{USR
	    �ж����������Ƿ���ͬ��
	   }}}USR */
	// }}}RME
	bool operator==( const Command & rtg_arg );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Command_H */

// }}}RME
