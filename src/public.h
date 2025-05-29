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

#define COMM_RESULT_FILEMAP_SIZE    (6*1024*1024)   //结果共享文件大小

//定义队列名称
extern const string COMM_REMOTEQUE_NAME;    //远程任务队列名
extern const string COMM_ASDUQUE_NAME;        //ASDU任务队列名
extern const string COMM_TIMERQUE_NAME;     //定时任务队列名
extern const string COMM_TIMER_RESULTQUE_NAME;     //定时任务执行结果队列名
extern const string COMM_INSIDE_RESULTQUE_NAME;    //通讯内部执行结果队列名

//定义共享内存文件名
extern const char* COMM_COMMAND_FILEMAP ;        //命令映射文件名
extern const char* COMM_RESULT_FILEMAP ;      //命令返回映射文件名

//定义事件
const char LISTEN_EXIT_EVENTNAME[] = "LISTEN_EXIT_EVENT";       //退出监听的事件名
const char CMDRESULT_FILEMAP_EVENT[] = "CMDRESULT_FILEMAP_EVENT";       //命令结果共享内存事件名

//数据库操作相关
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

extern LogControl mylog;     //日志

extern LPCRITICAL_SECTION 	g_pSection4NewMem; //用于分配内存同步的临界区

#endif