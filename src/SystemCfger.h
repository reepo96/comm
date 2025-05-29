// {{{RME classifier 'Logical View::SystemCfger'

#ifndef SystemCfger_H
#define SystemCfger_H

#ifdef PRAGMA
#pragma interface "SystemCfger.h"
#endif

#include <RTSystem/Communication.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�SystemCfger.h
* �ļ���ʶ��
* ժ    Ҫ��ϵͳ�����࣬���ڱ���ϵͳ��������Ϣ
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/
#include "public.h"
#include <map>
#include <vector>
#include "DeviceIniMgr.h"
#include <CommDataType.h>

using namespace std;

#ifdef _MY_UNITTEST
    class SystemCfger_UT;
#endif

//�������ͣ�socket���ӣ�com���ӣ�moden����
typedef enum cfg_link_type{CFG_SOCKET_LINK,CFG_COM_LINK,CFG_MODEN_LINK} CFG_LINK_TYPE;

//Э������
typedef enum protocol_type{PRO_60_103,PRO_103_HB,PRO_104_GB,PRO_104_ZJ,PRO_61850,PRO_104_SH} PROTOCOL_TYPE;

//DAU��ص�һЩ������Ϣ
typedef struct tagDAUInfo
{
    char DAUName[51];           //�ɼ�װ������
	char DAUType[52];			//�ɼ�װ���ͺ�
    bool    isRebuild;          //��������Ƿ��ѱ��ؽ�
	int		StationId;			//���վ���
    unsigned int DAUId;        //�豸���
	unsigned int BoardId;
	CFG_LINK_TYPE linkType;     //���ADU�����ӷ�ʽ
	short BoardType;        //�ɼ��幦������  1 ��ʾ��̬�ɼ��� 2 ��ʾ��̬�ɼ�

    char    addr[16];           //ip��ַ
    int     tcpPort;            //tcp�˿ں�
    int     udpPort;            //udp�˿ں�

    char    comName[5];         //������
    int     baud;               //������
    char    parity;             //��żУ��
    int   dataBit;            //����λ
    int   stop;                 //ֹͣλ

    char    phoneNum[20];       //�绰����
	int ProtocalType;
	bool IsActive;
}DAUINFO,*PDAUINFO;

typedef struct tagCfgTimerCall
{
    unsigned char DAUID;       //�ɼ�װ��ID	
    struct tm	startTime;		//��ʼʱ��
    int		intervalTime;		//���ʱ��(��λ����)
    bool	isCallAll;			//�Ƿ��ٻ���������
    bool	isCallEventData;	//�Ƿ��ٻ���־�¼�����
    bool	isCallFaultData;	//�Ƿ��ٻ���������
    bool	isCallLogData;		//�Ƿ��ٻ���־����
    bool	isCallLowRecData;	//�Ƿ��ٻ�����¼������
    bool	isCallQualityData;	//�Ƿ��ٻ�������������
    bool	isCallDynaData;		//�Ƿ��ٻ���̬��¼����
    bool	isCallConstData;	//�Ƿ��ٻ���������    
    time_t  ttStartTime;		//��ʼʱ��,��ͬ��startTime������һ�ֱ�����ʽ
}CFGTIMERCALL,*PCFGTIMERCALL;

typedef struct tagReportHead *PREPORTHEAD;

// }}}USR
// }}}RME

class SystemCfger
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    friend class SystemCfger_UT;
#endif
		//keyΪBoardId 
		map< unsigned int, PDAUINFO > DAUInfoMap;

		int iMaxAutoConnDevNum;//ͨѶģ���������Զ������豸������
		int iMaxConnDevNum;		//��������豸��

		map< unsigned char,PCFGTIMERCALL> DauTimerCallMap;//�豸��ʱ�ٻ���Ϣ

		int  logLevel;      //��־����,0-NONE 1-ERROR 2-WARNING 3-INFO 4-DEBUG
		int  logFileMaxSize;    //log�ļ���󳤶ȣ���λM
		int  udpPortForRltData; //����DAUʵʱ���ݵ�UDP�˿�

		CDeviceIniMgr	m_IniMgr;//Ini��ֵ�ļ��Ĺ���

		PROTOCOL_TYPE	rmtProtocolType; //Զ��ͨѶЭ������

		char modemComName[ 5 ];//modemʹ�õĴ���
		int modemBaud;// modem�ڲ�����
		char modemParity;//modem����żУ��
		int modemDataBit;// modem������λ
		int modemStopBit;//modem��ֹͣλ

		map< unsigned int, PDAUINFO > ConstantDAUInfoMap;//��̬�豸��Ϣ

		int maxLenOfAutoAnaWaveFile;	//�Զ���������¼���ļ�����󳤶�(��λ��M�ֽ�)

		//�Զ��ϴ����ϼ�����
		struct tagAutoSendRptCfg
		{
			bool PSV_UpperLimit;	//����ԭ���������ѹ����ʱ�Զ��ϴ����ϼ�
			bool PSV_LowerLimit;	//����ԭ���������ѹ����ʱ�Զ��ϴ����ϼ�
			bool DCB_UpperLimit;	//����ԭ����ֱ��ͻ������ʱ�Զ��ϴ����ϼ�
			bool DCB_LowerLimit;	//����ԭ����ֱ��ͻ������ʱ�Զ��ϴ����ϼ�
			bool F_UpperLimit;//����ԭ����Ƶ������ʱ�Զ��ϴ����ϼ�
			bool F_LowerLimit;//����ԭ����Ƶ������ʱ�Զ��´����ϼ�
			bool F_ChangeRate;//����ԭ����Ƶ�ʱ仯��ʱ�Զ��ϴ����ϼ�
			bool C_UpperLimit;//����ԭ���ǵ�������ʱ�Զ��ϴ����ϼ�
			bool C_LowerLimit;//����ԭ���ǵ�������ʱ�Զ��ϴ����ϼ�
			bool C_Break;//����ԭ���ǵ���ͻ��ʱ�Զ��ϴ����ϼ�
			bool C_OverLimit;//����ԭ���ǵ���Խ��ʱ�Զ��ϴ����ϼ�
			bool Reverse_power;//����ԭ�����湦�������ϴ����ϼ�
			bool ZSACV;//����ԭ������������ѹ�ϴ����ϼ�
			bool NSACV;//����ԭ���Ǹ�������ѹ�ϴ����ϼ�
			bool Shock;//����ԭ�������ϴ����ϼ�
			bool SWCH;//����ԭ���ǿ��ر�λ�ϴ����ϼ�
			bool V_Break;//����ԭ���ǵ�ѹͻ��ʱ�Զ��ϴ����ϼ�
			bool Fault_InZone;//����ԭ�������ڹ���ʱ�Զ��ϴ����ϼ�
			bool Fault_OutZone;//����ԭ�����������ʱ�Զ��ϴ����ϼ�
		} AutoSendRptCfg;

		typedef enum QDReason
		{
			QD_PSV_UPPERLIMIT = 104,//�����ѹ����
			QD_PSV_LOWERLIMIT = 105,//�����ѹ����
			QD_DCB_UPPERLIMIT = 201,//ֱ��ͻ������?????????????
			QD_DCB_LOWERLIMIT = 202,//ֱ��ͻ������????????????????
			QD_F_UPPERLIMIT = 101,//Ƶ������
			QD_F_LOWERLIMIT = 102,//Ƶ������
			QD_F_CHANGERATE = 103,//Ƶ�ʱ仯��
			QD_C_UPPERLIMIT,//��������????
			QD_C_LOWERLIMIT =202,//��������???
			QD_C_BREAK = 203,//����ͻ��?????
			QD_C_OVERLIMIT = 201,//����Խ��
			QD_REVERSE_POWER =407,//�湦������
			QD_ZSACV,//��������ѹ????
			QD_NSACV,//��������ѹ????
			QD_SHOCK = 109,//��
			QD_SWCH = 206,//���ر�λ
			QD_V_BREAK,//��ѹͻ��

			QD_FAULT_INZONE,//���ڹ���?????????????
			QD_FAULT_OUTZONE,//�������?????????????
		}QDREASON;

		/*
		*��������ԭ����ж��ܷ��Զ��ϴ����ϼ�
		*TrgTypeNum������ԭ���
		*���أ�true-���ԣ�false-������
		*/
		bool IsCandAutoSendRpt(unsigned short TrgTypeNum);

		//��ʼ��ini�ļ��е�����
		void InitIniFileCfg( void );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    map< string,vector<PREPORTHEAD>*> toSendRmtReport;//�����͵�Զ�̵Ĺ��ϼ��б�

		void InitOtherCfg();
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute '_instance'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static SystemCfger _instance;
	// }}}RME

public:
	// {{{RME classAttribute 'rmtLinkType'
	// {{{RME general 'documentation'
	/* {{{USR
	��Զ��ͨѶ�����ӷ�ʽ
	   }}}USR */
	// }}}RME
	CFG_LINK_TYPE rmtLinkType;
	// }}}RME
	// {{{RME classAttribute 'ADULinkType'
	// {{{RME general 'documentation'
	/* {{{USR
	��ADUͨѶ�����ӷ�ʽ
	   }}}USR */
	// }}}RME
	CFG_LINK_TYPE ADULinkType;
	// }}}RME
	// {{{RME classAttribute 'addrForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	��ADUͨѶ��IP��ַ
	   }}}USR */
	// }}}RME
	char addrForADU[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'tcpPortForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	����ADU���ӵĶ˿�
	   }}}USR */
	// }}}RME
	int tcpPortForADU;
	// }}}RME
	// {{{RME classAttribute 'udpPortForADU'
	// {{{RME general 'documentation'
	/* {{{USR
	��ADUͨѶ��udp�˿�
	   }}}USR */
	// }}}RME
	int udpPortForADU;
	// }}}RME

	//ADD BY KH 
	int tcpPortForADUAutoSend;
	// {{{RME classAttribute 'addrForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	��Զ�����ӵĵ�ַ
	   }}}USR */
	// }}}RME
	char addrForRemote[ 16 ];
	// }}}RME
	// {{{RME classAttribute 'tcpPortForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	Զ�̼����˿�
	   }}}USR */
	// }}}RME
	int tcpPortForRemote;
	// }}}RME
	// {{{RME classAttribute 'udpPortForRemote'
	// {{{RME general 'documentation'
	/* {{{USR
	��Զ�̵�udp�˿�
	   }}}USR */
	// }}}RME
	int udpPortForRemote;
	// }}}RME
	// {{{RME classAttribute 'listenNum'
	// {{{RME general 'documentation'
	/* {{{USR
	������
	   }}}USR */
	// }}}RME
	int listenNum;
	// }}}RME
	// {{{RME classAttribute 'TransientDAUInfoMap'
	// {{{RME general 'documentation'
	/* {{{USR
	��̬�豸��Ϣ
	   }}}USR */
	// }}}RME
	map< unsigned int, PDAUINFO > TransientDAUInfoMap;
	// }}}RME
	// {{{RME classAttribute 'comNameForRmt'
	// {{{RME general 'documentation'
	/* {{{USR
	��Զ�����ӵĴ���
	   }}}USR */
	// }}}RME
	char comNameForRmt[ 5 ];
	// }}}RME
	// {{{RME classAttribute 'baud'
	// {{{RME general 'documentation'
	/* {{{USR
	������
	   }}}USR */
	// }}}RME
	int baud;
	// }}}RME
	// {{{RME classAttribute 'parity'
	// {{{RME general 'documentation'
	/* {{{USR
	��żУ��
	   }}}USR */
	// }}}RME
	char parity;
	// }}}RME
	// {{{RME classAttribute 'dataBit'
	// {{{RME general 'documentation'
	/* {{{USR
	����λ
	   }}}USR */
	// }}}RME
	int dataBit;
	// }}}RME
	// {{{RME classAttribute 'stopBit'
	// {{{RME general 'documentation'
	/* {{{USR
	ֹͣλ
	   }}}USR */
	// }}}RME
	int stopBit;
	// }}}RME
	// {{{RME classAttribute 'phoneNumForRmt'
	// {{{RME general 'documentation'
	/* {{{USR
	��Զ�����ӵĵ绰����
	   }}}USR */
	// }}}RME
	char phoneNumForRmt[ 20 ];
	// }}}RME
	// {{{RME classAttribute 'timeOut'
	// {{{RME general 'documentation'
	/* {{{USR
	�趨�ĳ�ʱʱ��(��λ������)
	   }}}USR */
	// }}}RME
	int timeOut;
	// }}}RME
	// {{{RME classAttribute 'DataFilePath'
	// {{{RME general 'documentation'
	/* {{{USR
	�����ļ����Ŀ¼
	   }}}USR */
	// }}}RME
	char DataFilePath[ 255 ];
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateCopyConstructor'
	SystemCfger( const SystemCfger & rtg_arg );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateAssignmentOperator'
	SystemCfger & operator=( const SystemCfger & rtg_arg );
	// }}}RME


protected:
	// {{{RME operation 'SystemCfger()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	SystemCfger( void );
	// }}}RME

public:
	// {{{RME operation '~SystemCfger()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~SystemCfger( void );
	// }}}RME
	// {{{RME operation 'GetInstance()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	static SystemCfger & GetInstance( void );
	// }}}RME
	// {{{RME operation 'Rebuild(int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Rebuild
	    Description:    ���»�ȡ����
	    Calls:          
	    Called By:      
	    Input:          type���ؽ����ͣ�1���ؽ�Զ��������ã�2���ؽ�ADU������ã�3��������ͨѶ�޹ص�����
	                    
	                    
	    Output:                             
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Rebuild( int type );
	// }}}RME
	// {{{RME operation 'Init()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Init
	    Description:    ��ʼ��
	    Calls:          
	    Called By:      
	    Input:          
	                    
	                    
	    Output:                             
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void Init( void );
	// }}}RME

protected:
	// {{{RME operation 'InitDauInfo()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitDauInfo( void );
	// }}}RME
	// {{{RME operation 'InitSysCfg()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitSysCfg( void );
	// }}}RME
	// {{{RME operation 'InitTimerCallInf()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	void InitTimerCallInf( void );
	// }}}RME

public:
	// {{{RME operation 'GetDAUInfo(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDAUInfo
	    Description:    ����DAU��Ż��DAU����Ϣ  ͣ�øú���2010-10-18
	    Calls:          
	    Called By:      
	    Input:          DAUId���豸���
	                    devType:�豸���ͣ�1-��̬�豸,2-��̬�豸
	                    
	    Output:                             
	    Return:         �ɹ�����DAU����Ϣ�����򷵻�NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	PDAUINFO GetDAUInfo( unsigned int DAUId, int devType );
	// }}}RME
	// {{{RME operation 'GetDAUInfo(char[16],int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDAUInfo
	    Description:    ����ip��ַ���DAU����Ϣ  ͣ�øú���2010-10-18
	    Calls:          
	    Called By:      
	    Input:          DAUId���豸���
	                    devType:�豸���ͣ�1-��̬�豸,2-��̬�豸
	                    
	    Output:                             
	    Return:         �ɹ�����DAU����Ϣ�����򷵻�NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	PDAUINFO GetDAUInfo( const char *ipAddr, int devType );
	// }}}RME

protected:
	// {{{RME operation 'RptStruct2CmdReport(RptStruct *,PREPORTHEAD)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       RptStruct2CmdReport
	    Description:    �����ݿ�򱨷�����ʽת��Э��򱨸�ʽ
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	bool RptStruct2CmdReport( RptStruct * RptInfo, PREPORTHEAD pFaultReport );
	// }}}RME

public:
	PDAUINFO GetDAUInfo( const char *ipAdd );
	PDAUINFO GetDAUInfo( unsigned int BoardId);

	/*************************************************
	    Function:       InitDauInfo2
	    Description:    �����豸id��ʼ���豸��Ϣ����idΪ0���ʼȫ����Ϣ
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************/
	void InitDauInfo2(unsigned int paraDevId=0);

	// {{{RME operation 'InitReportList(string)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       InitReportList
	    Description:    ����Զ��ip��ʼ�����ϼ��б�
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void InitReportList( string rmtIP );
	// }}}RME
	// {{{RME operation 'GetReportList(string)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportList
	    Description:    ����Զ��ip��ù��ϼ��б�
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	vector< PREPORTHEAD > * GetReportList( string rmtIP );
	// }}}RME
	// {{{RME operation 'AppendReportList(unsigned int,int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AppendReportList
	    Description:    �����豸id���ļ�id��ӹ��ϱ�����Ϣ
	    Calls:          
	    Called By:      
	    Input:          
	                    
	    Output:         
	    Return:         
	    *************************************************
	   }}}USR */
	// }}}RME
	void AppendReportList( unsigned int devId, int fileId );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* SystemCfger_H */

// }}}RME
