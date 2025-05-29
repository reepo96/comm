#ifndef PROTOCAL104_SH_H_HEADER_INCLUDED_B39C559D
#define PROTOCAL104_SH_H_HEADER_INCLUDED_B39C559D
#include <RTSystem/Communication.h>
#include <Protocol104.h>
#include <FileControl.h>
#include <public.h>
#include <map>
using namespace std;
#define PROSH104_FRAME_MAXSIZE   2048
#define AutoSendDataBuffSize  600*1024
class MemMapMgr;
//##ModelId=4C62414C0232
class Protocal104_SH : public Protocol104
{
public:

	char RES_COMTRADE_DIR[100];	//comtrade文件目录
	char RES_COMTRADE_DIV_DIR[100];	//comtrade分文件目录
	char RES_SHD_DIR[100];		//shd文件目录
	char RES_MARKENV_DIR[100];	//标记事件文件目录
	char RES_CONFIG_DIR[100];	//配置相关文件目录
	char RES_REPORT_DIR[100];	//简报文件目录
	char RES_PROGRAM_DIR[100];	//升级程序文件目录

	char RES_LOG_DIR[100];
	char RES_PQTREND_DIR[100];
	char RES_PQEVENT_DIR[100];
	char RES_PQCOMTRADE_DIR[100];

	//key:boardId   value: bit0 bRcdRtEnable;bit1 bZeroOffsetEnable; bit2 bPQRtEnable; bit3 bPQTrendEnable

	static std::map<unsigned int ,unsigned char> m_DevAutoSendEnableMap;

	static CRITICAL_SECTION	*m_pMapSectionSH104;
	//
	char* m_szSrcAutoSendDataBuff;
	char* m_szDestAutoSendDataBuff;

	bool	m_bAutoDataHasTrs;//自动数据已上传
	
public:
	
	static bool CallProcessAutoSendData(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetRunLog(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetSHFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetSHFileList(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallPushFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallOperateDevice(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallSetDeviceTestingState(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallSetDeviceRunningState(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetDauInf(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetTime(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallSetTime(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetNetCfg(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallSetNetCfg(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallStartRecByManual(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallReadZeroOffset(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallReadRltData(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetContData(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetRecWaveFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetRecWaveFileList(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetPQEvent(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetPQWaveFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetPQTrendFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);

	//电能性能检验
	static bool CallPQTest(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);

	static bool CallPQResumSet(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallPQ10CycDataExp(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetPQ10CycDataFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetPQStatFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallGetAuditFile(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallSendAuditAlarm(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);
	static bool CallUserLogin(Protocal104_SH* pProtocol,Linker* pLinker, struct Command& pCmd);

	Task* AnalyseTask( PASDUID104 pAsduId, char * pAsduData, int iAsduDataLen, Linker * pLinker );
	Task * Decode( Linker * pLinker );
	  
	bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	bool InitConnect( Linker * pLinker );
	bool GetCmdRst(Linker *pLinker, int asduNo,int inf,string &szRstData );
	bool GetCmdRst(Linker *pLinker, struct  Command *pCmd,int asduNo,int inf);
	bool ParserOneFrameRstData(const char* pCommData, int validatelen, char * szLeftbuff, int& szLeftBuffLen,struct  Command *pCmd,MemMapMgr& memMgr,string& sz_DBFilelist);
	bool ProcessComtradeFileCmdRst(Linker *pLinker,struct  Command *pCmd, int asduNo ,FileControl::FILETYPE fileControlType);
	bool ProcessGernalFileCmdRst(Linker* pLinker, struct Command* pCmd, int asduNo,string fileName, int fileSize,string YearMonth, FileControl::FILETYPE fileControlType);
	int AdjustRTDataStyle(int devId, char * szData, int srclen);
	int PQRTToSchdeclueRtData(int devId, char* szData ,int srclen);
	static bool WriteAutoData2Mem( unsigned int devBoradId,unsigned char funcode,CMDRESULTTYPE resultType,const char *data,int data_len );
	bool ProcessAutoSendData(Linker *pLinker,struct Command* pCmd);
	int DealAutoSend(Linker *pLinker);
	unsigned char GetDevId(unsigned int boardId);
	string ParseFileNameForGetTime(const string pFileName, bool bPQ, bool bRemote);

	bool ProcessContComtradeFileCmdRst(Linker *pLinker,struct  Command *pCmd, int asduNo ,
		const char* fileName, FileControl::FILETYPE fileControlType,const char* fileTime=NULL);

    bool GetWaveFileExitInDB(string & szDBFileList,struct CP56Time2a & t0,struct CP56Time2a & t1);
	//##ModelId=4C6241FB038A
    Protocal104_SH();
	~Protocal104_SH();

    //##ModelId=4C6244B2000F
    //Task* DoIdle(Linker * pLinker);

    //##ModelId=4C62462D003E
    int AnalyseFrame(Linker* pLinker, char* framePacket, int* frameLen, APCI4Prot104* pAPCI, PASDUID104* pOutASDUID, PASDUINFID104* pOutASDUInfID, char** pOutAsduData, int* pAsduDataLen, unsigned int uiTimeOut = 0);
	//电能质量趋势文件
	bool GetPQTrendFile(Linker *pLinker, struct Command *pCmd);
	//电能质量录波文件
    //##ModelId=4C624C04035B
    bool GetPQRecWaveFile(Linker* pLinker, struct Command* pCmd);
	//电能质量事件
    //##ModelId=4C624DE300CB
    bool GetPQEvent(Linker* pLinker, struct Command* pCmd);

	//电能质量性能检验
	bool PQTest(Linker *pLinker, struct Command *pCmd);

	//恢复出厂设置
	bool PQResumSet(Linker *pLinker, struct Command *pCmd);

	bool PQ10CycDataExp(Linker *pLinker, struct Command *pCmd);

	bool GetPQ10CycDataFile(Linker *pLinker, struct Command *pCmd);

	bool GetPQStatFile(Linker *pLinker, struct Command *pCmd);

	bool GetAuditFile(Linker *pLinker, struct Command *pCmd);

	bool SendAuditAlarm(Linker *pLinker, struct Command *pCmd);

	bool UserLogin(Linker *pLinker, struct Command *pCmd);

	//稳态数据
    //##ModelId=4C624DF702FD
    bool GetContData(Linker* pLinker, struct Command* pCmd);
	//故障录录波文件
    //##ModelId=4C624E0C00BB
    bool GetRecWaveFile(Linker* pLinker, struct Command* pCmd);
	//故障录波文件列表
    //##ModelId=4C6252B8009C
    bool GetRecWaveFileList(Linker* pLinker, struct Command* pCmd);

	//读实时数据
	bool Protocal104_SH::ReadRcdRtData(Linker* pLinker, struct Command* pCmd);
	//读零漂数据
    //##ModelId=4C6252E9004E
    bool ReadRcdOffsetData(Linker* pLinker, struct Command* pCmd);
	//手动启动录波
    //##ModelId=4C62531F02CE
    bool StartRecordByManune(Linker* pLinker, struct Command* pCmd);
	//设置装置时间
    //##ModelId=4C6253830157
    bool SetDeviceTime(Linker* pLinker, struct Command* pCmd);
	//读装置时间
    //##ModelId=4C62539101D4
    bool GetDeviceTime(Linker* pLinker, struct Command* pCmd);

	bool GetNetCfg(Linker* pLinker, struct Command* pCmd);
	bool SetNetCfg(Linker* pLinker, struct Command* pCmd);

	//读设备状态
    //##ModelId=4C6253A40177
    bool GetDeviceState(Linker* pLinker, struct Command* pCmd);
	//设定装置为运行状态
    //##ModelId=4C6253C0030D
    bool SetDeviceRunningState(Linker* pLinker, struct Command* pCmd);
	//设定装置为调试状态
	bool SetDeviceTestingState(Linker* pLinker, struct Command* pCmd);
	//获取自 检信息
    //##ModelId=4C6253D901C5
    bool GetDAUInfo(Linker* pLinker, struct Command* pCmd);
	//操作设备硬件
    //##ModelId=4C62540B03B9
    bool OperateHardWare(Linker* pLinker, struct Command* pCmd);
	//获取通用文件列表
    //##ModelId=4C625440036B
    bool GetGeneralFilelist(Linker* pLinker, struct Command* pCmd);
	//获取通用文件
    //##ModelId=4C6254580148
    bool GetGeneralFile(Linker* pLinker, struct Command* pCmd);
	//获取装置运行日志
    //##ModelId=4C62546502EE
    bool GetRunLog(Linker* pLinker, struct Command* pCmd);
	//下发文件到装置
    //##ModelId=4C62547600FA
    bool SHPushFile(Linker* pLinker, struct Command* pCmd);

    /*****************************
    召唤双合自定义文件列表
    一共3个参数
    参数1 开始时间
    参数2 结束时间
    参数3 文件类型
    *****************************/
    //##ModelId=4C625484004E
    bool GetSHFileList(Linker* pLinker, struct Command* pCmd);

    /********************************
    获取双合自定义类型文化
    一共2个参数
    参数1 文件类型
    参数2 文件名称
    ********************************/
    //##ModelId=4C625508000F
    bool GetSHFile(Linker* pLinker, struct Command* pCmd);

};

	/*********协议帧与命令的对应关系-->***********************/
	typedef struct tagPro104SHFrame2Cmd
	{
		unsigned char cmdFunCode;    //本系统内部使用的命令代码
		bool (*exuteCmd)(Protocal104_SH*,Linker * ,struct Command&);  //用于执行命令的函数
		char           cmdName[64]; //消息名称
	}PRO104SHFRAME2CMD,*PPRO104SHFRAME2CMD;

	extern PRO104SHFRAME2CMD pro104SHFR2CmdMap[30];


#endif /* PROTOCAL104_SH_H_HEADER_INCLUDED_B39C559D */
