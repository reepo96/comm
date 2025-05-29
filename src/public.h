#ifndef public_H
#define public_H

#pragma warning(disable : 4786)

//#define NULL 0
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
#include <windef.h>
#include <assert.h>

#include <string>
using namespace std;

#include "SystemCfger.h"
#include "LogControl.h"
#include <CommDataType.h>

#include "debugnew.h"

#ifndef MYDEBUG_NEW
#define MYDEBUG_NEW new(__FILE__,__LINE__)
#endif

#define COMM_RESULT_FILEMAP_SIZE    (6*1024*1024)   //��������ļ���С

//�����������
extern const string COMM_REMOTEQUE_NAME;    //Զ�����������
extern const string COMM_ASDUQUE_NAME;        //ASDU���������
extern const string COMM_TIMERQUE_NAME;     //��ʱ���������
extern const string COMM_TIMER_RESULTQUE_NAME;     //��ʱ����ִ�н��������
extern const string COMM_INSIDE_RESULTQUE_NAME;    //ͨѶ�ڲ�ִ�н��������

//���干���ڴ��ļ���
extern const char* COMM_COMMAND_FILEMAP ;        //����ӳ���ļ���
extern const char* COMM_RESULT_FILEMAP ;      //�����ӳ���ļ���

//�����¼�
const char LISTEN_EXIT_EVENTNAME[] = "LISTEN_EXIT_EVENT";       //�˳��������¼���
const char CMDRESULT_FILEMAP_EVENT[] = "CMDRESULT_FILEMAP_EVENT";       //�����������ڴ��¼���

//���ݿ�������
typedef int (_stdcall *DBINIT)(char ip[],char dbname[],char username[],char password[]);
typedef int (_stdcall *ACCESSDBINIT)(const char *pDBFile,const char *pUsername,const char *pPassword,const char *pSecurityFile);
typedef int (_stdcall *ADDLOG)(LogStruct *LogInfo);
typedef int (_stdcall *GETALLDAU)(DAUInfoStruct **DAUInfo);
typedef int (_stdcall *GETONEDAU)(int DAUID,DAUInfoStruct **DAUInfo);
typedef int (_stdcall *GETALLCFG)(SystemCfg **ppSysCfg);
typedef int (_stdcall *RELEASEMEMERY)(void *pVariant);
typedef int (_stdcall *GETTOBESENDRPT2RMT)(const char *rmtIpAddr,RptStruct **RptInfo);
typedef int (_stdcall *ADDSENDFAULTINF)(SendFaultInfo *pSendFaultInf);
typedef int (_stdcall *GETRPTBYSQL)(char *pSql,RptStruct **RptInfo);
typedef int (_stdcall *GETRPTBYID)(unsigned char dauId, int rptId, RptStruct **RptInfo);
typedef int (_stdcall *GETALLTIMERCALLINF)(TimerCall **ppTimerCallInf);
typedef int (_stdcall *GETMAXFILENO)(unsigned char aduId,unsigned char fileType);
typedef int (_stdcall *ADDCALLFILEINF)(PCallFileInf pFileInf);
typedef int (_stdcall *GETNOCALLFILEBYTYPE)(unsigned char dauId,unsigned char type,CallFileInf **ppFileInf);
typedef int (_stdcall *UPDATECALLFILEINFBYID)(unsigned char DAUID, char *fileName, bool IsCallData);
typedef int (_stdcall *GETMARKEDEVTBYSQL)(char *pSql,MarkedEvtStruct **MarkedEvtInfo);

extern DBINIT DbInit;
extern ACCESSDBINIT AccessDBInit;
extern ADDLOG AddLog;
extern GETALLDAU GetAllDau;
extern GETONEDAU GetOneDau;
extern GETALLCFG GetAllCfg;
extern RELEASEMEMERY releaseMemery;
extern GETTOBESENDRPT2RMT getToBeSendRpt2Rmt;
extern ADDSENDFAULTINF addSendFaultInf;
extern GETRPTBYSQL getRptBySql;
extern GETRPTBYID getRptById;
extern GETALLTIMERCALLINF getAllTimerCallInf;
extern GETMAXFILENO getMaxFileNo;
extern ADDCALLFILEINF addCallFileInf;
extern GETNOCALLFILEBYTYPE getNoCallFileByType;
extern UPDATECALLFILEINFBYID updateCallFileInfById;
extern GETMARKEDEVTBYSQL getMarkedEvtBySql;

extern LogControl mylog;     //��־

extern LPCRITICAL_SECTION 	g_pSection4NewMem; //���ڷ����ڴ�ͬ�����ٽ���

#endif