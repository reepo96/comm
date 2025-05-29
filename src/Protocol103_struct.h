#ifndef Protocol103_struct_H
#define Protocol103_struct_H

#include "Command.h"

class Protocol103;
class Linker;

//定义数据流方向
#define PROHB103_DIRECT_M		0	//监视方向
#define PROHB103_DIRECT_C		1	//控制方向

//定义控制方向贞计数有效位
#define PROHB103_FCV_IV		0	//无效
#define PROHB103_FCV_V		1	//有效

//定义监视方向数据流控制位
#define PROHB103_DFC_V		0	//可以接受数据
#define PROHB103_DFC_IV		1	//不能接受数据

//定义数据包的类型
#define PROHB103_FT_CONST			0	//固定长度数据包
#define PROHB103_FT_NOTCONST		1	//可变长度

//数据单元数据元素或数据体的个数可变
#define PROHB103_INFOOBJECT_CHNUM		0

//数据单元数据元素的寻址方式
#define PROHB103_FIND_INFOELEM		0	//通过信息元素组寻址
#define PROHB103_FIND_INFOOBJ			1	//通过信息体组寻址

//应用服务数据单元数据的级别
#define PROHB103_PRI_NON				0	//不是数据
#define PROHB103_PRI_1				1	//1级
#define PROHB103_PRI_2				2	//2级

//定义功能类型
#define PROHB103_FUN_GLB		255		//全局功能类型
#define PROHB103_FUN_GEN		254		//通用分类功能类型
#define PROHB103_FUN_OTHER		128		//其他功能类型

//应用服务数据单元数据如何响应总查询
#define PROHB103_GI_NON				0	//不响应
#define PROHB103_GI_ALL				1	//数据改变传送
#define PROHB103_GI_ON				2	//数据从OFF 变成 ON时传送

//控制方向的GOM命令
#define PROHB103_GOM_NON				0	//
#define PROHB103_GOM_ALL				1	//
#define PROHB103_GOM_ON				2	//

//链路功能码
//控制方向
#define CFUNID_0	0
#define CFUNID_1	1
#define CFUNID_2	2
#define CFUNID_3	3
#define CFUNID_4	4
#define CFUNID_5	5
#define CFUNID_6	6
#define CFUNID_7	7
#define CFUNID_8	8
#define CFUNID_9	9
#define CFUNID_10	10
#define CFUNID_11	11
#define CFUNID_12	12
#define CFUNID_13	13
#define CFUNID_14	14
#define CFUNID_15	15
//监视方向
#define MFUNID_0	0
#define MFUNID_1	1
#define MFUNID_2	2
#define MFUNID_3	3
#define MFUNID_4	4
#define MFUNID_5	5
#define MFUNID_6	6
#define MFUNID_7	7
#define MFUNID_8	8
#define MFUNID_9	9
#define MFUNID_10	10
#define MFUNID_11	11
#define MFUNID_12	12
#define MFUNID_13	13
#define MFUNID_14	14
#define MFUNID_15	15

//应用服务单元类别标识码
#define ASDU_1			1
#define ASDU_2			2
#define ASDU_3			3
#define ASDU_4			4
#define ASDU_5			5
#define ASDU_6			6
#define ASDU_7			7
#define ASDU_8			8
#define ASDU_9			9
#define ASDU_10			10
#define ASDU_11			11
#define ASDU_20			20
#define ASDU_21			21
#define ASDU_22			22
#define ASDU_23			23
#define ASDU_24			24
#define ASDU_25			25
#define ASDU_26			26
#define ASDU_27			27
#define ASDU_28			28
#define ASDU_29			29
#define ASDU_30			30
#define ASDU_31			31

//华北扩展
#define ASDU_12			12
#define ASDU_13			13
#define ASDU_14			14
#define ASDU_15			15
#define ASDU_16			16
#define ASDU_17			17
#define ASDU_18			18

#define ASDU_42			42

#define ASDU_101		101
#define ASDU_102		102
#define ASDU_103		103
#define ASDU_104		104
#define ASDU_105		105
#define ASDU_106		106
#define ASDU_107		107
#define ASDU_109		109
#define ASDU_111		111


//控制方向固定长度包
#define C_RFB_NA_3	1
#define C_RCU_NA_3	2
#define C_PL1_NA_3	3
#define C_PL2_NA_3	4
#define C_RLK_NA_3	5

//监视方向固定长度包
#define M_CON_NA_3	11
#define M_BY_NA_3	12
#define M_NV_NA_3	13
#define M_LKRGOOD_NA_3	14
#define M_LKRNOTWORK_NA_3	15
#define M_LKRNOTEXPORT_NA_3	16

//监视方向可变长度数据包
#define M_TTM_TA_3	21
#define M_TMR_TA_3	22
#define M_MEI_NA_3	23
#define M_TME_TA_3	24
#define M_IRC_NA_3	25
#define M_SYN_TA_3	26
#define M_TGI_NA_3	27
#define M_MEII_NA_3	28
#define M_GD_TA_3	29

//控制方向可变长度数据包
#define C_SYN_TA_3	51
#define C_IGI_NA_3	52
#define C_GD_NA_3	53
#define C_GRC_NA_3	54
#define C_GC_NA_3	55

//华北扩展监视方向可变长度数据包
#define M_EX12_NA_3		101
#define M_EX14_NA_3		102
#define M_EX16_NA_3		103
#define M_EX18_NA_3		104
#define M_EX42_NA_3		105
#define M_EX102_NA_3	106
#define M_EX104_NA_3	107
#define M_EX106_NA_3	108

//华北扩展控制方向可变长度数据包
#define C_EX13_NA_3		121
#define C_EX15_NA_3		122
#define C_EX17_NA_3		123

#define C_EX101_NA_3	124
#define C_EX103_NA_3	125

#define C_EX105_NA_3	126
#define C_EX107_NA_3	127
#define C_EX109_NA_3	128
#define C_EX111_NA_3	129


/////////////////ASDU////////////////////
//监控方向的系统功能
#define MINF_NOTUSER	0	//表示信息体标示符包含在信息数据中
#define MINF_0	1
#define MINF_00	2
#define MINF_2	3
#define MINF_3	4
#define MINF_4	5
#define MINF_5	6

//监视方向的状态信号
#define MINF_16	11
#define MINF_17	12
#define MINF_18	13
#define MINF_19	14
#define MINF_20	15
#define MINF_21	16
#define MINF_22	17
#define MINF_23	18
#define MINF_24	19
#define MINF_25	20
#define MINF_26	21
#define MINF_27	22
#define MINF_28	23
#define MINF_29	24
#define MINF_30	25

//监视方向的监视信号
#define MINF_32	31
#define MINF_33	32
#define MINF_35	33
#define MINF_36	34
#define MINF_37	35
#define MINF_38	36
#define MINF_39	37
#define MINF_46	38
#define MINF_47	39

//监视方向的接地故障信号
#define MINF_48	40
#define MINF_49	41
#define MINF_50	42
#define MINF_51	43
#define MINF_52	44

//监视方向的故障信号
#define MINF_64	50
#define MINF_65	51
#define MINF_66	52
#define MINF_67	53
#define MINF_68	54
#define MINF_69	55
#define MINF_70	56
#define MINF_71	57
#define MINF_72	58
#define MINF_73	59
#define MINF_74	60
#define MINF_75	61
#define MINF_76	62
#define MINF_77	63
#define MINF_78	64
#define MINF_79	65
#define MINF_80	66
#define MINF_81	67
#define MINF_82	68
#define MINF_83	69
#define MINF_84	70
#define MINF_85	71
#define MINF_86	72
#define MINF_87	73
#define MINF_88	74
#define MINF_89	75
#define MINF_90	76
#define MINF_91	77
#define MINF_92	78
#define MINF_93	79

//监视方向的自动重合闸信号
#define MINF_128	91
#define MINF_129	92
#define MINF_130	93

//监视方向的被测值
#define MINF_144	101
#define MINF_145	102
#define MINF_146	103
#define MINF_147	104
#define MINF_148	105

//监视方向的通用分类功能
#define MINF_240	111
#define MINF_241	112
#define MINF_242	113
#define MINF_243	114
#define MINF_244	115
#define MINF_245	116
#define MINF_249	117
#define MINF_250	118
#define MINF_251	119

//控制方向的系统功能
#define CINF_NOTUSER	200
#define CINF_0		201
#define CINF_00		202

//控制方向的一般命令
#define CINF_16		211
#define CINF_17		212
#define CINF_18		213
#define CINF_19		214
#define CINF_23		215
#define CINF_24		216
#define CINF_25		217
#define CINF_26		218

//控制方向的通用分类功能
#define CINF_240	221
#define CINF_241	222
#define CINF_242	223

#define CINF_243	224
#define CINF_244	225
#define CINF_245	226
#define CINF_248	227
#define CINF_249	228
#define CINF_250	229
#define CINF_251	230


/*********************103规约消息定义结构-->************************************/
typedef struct msg103
{
	//消息号
	int msgno;

	//控制域
	//控制域功能码
	unsigned char funid:4;
	//在控制方向,表示贞计数有效位
	//在监视方向,标识数据流控制位
	unsigned char user:1;
	//固定长度
	bool isconst:1;
	//消息方向
	unsigned char direct:1;
	//贞数据包格式
	unsigned char ft:1;

	//应用服务单元
	//类型标识
	unsigned char typ;
	//数目
	unsigned char num:7;
	//寻址方式
	unsigned char sq:1;

	//消息号名称
	char *msgnoname;
	//消息名称
	char *msgname;
}smsg103;


extern smsg103 msg103_array[];
extern int MSG103_COUNT;

/***************************103规约消息定义结构<--******************************/

/***********************103规约应用服务数据单元的信息序号结构-->**********************************/
typedef struct inf103
{
	//信息序号编号
	int infno;

	//消息号
	int msgno;

	//消息方向
	unsigned char direct:1;
	//在监视方向表示数据的级别\在控制方向表示GOM命令
	unsigned char user1:2;
	//在监视方向表示如何响应总查询
	unsigned char user2:2;
	//备用
	unsigned char res:3;

	unsigned char fun;
	//信息序号
	unsigned char inf;

	//信息序号编号名称
	char *infnoname;
	//信息序号名称
	char *infname;
}sinf103;

//103规约应用服务数据单元的信息序号
//定义的信息序号在0-255范围内,用一个字节表示
//如果信息序号不在以下定义的范围内,可使用fun<<8 + inf直接组合成信息序号
#define FUNINF(fun,inf)		((fun<<8) + inf)
#define FUN(index)			(index>>8)
#define INF(index)			(index & 0xff)

extern sinf103 inf103_array[];
extern int INF103_COUNT;

/************************103规约应用服务数据单元的信息序号结构<--*********************************/


/**********由协议功能到命令的对应关系-->***********************/
typedef struct tagPro2Cmd
{
    unsigned char funid:4;     //控制域功能码
    unsigned char direct:1;    //消息方向,0X0：控制方向；0X01：监视方向
    char          res:3;
    unsigned char type;        //应用服务单元类型标识
    unsigned char COT;         //传送原因
    unsigned short GIN;        //通用分类标识序号
    CMDDIR from;
    CMDDIR to;
    unsigned char cmdFunCode;    //本系统内部使用的命令代码
    bool (*exuteCmd)(Protocol103*,Linker * ,struct Command&);  //用于执行命令的函数
    const char*	cmdName; //消息名称
}PRO2CMD,*PPRO2CMD;

//查询录波器运行状态信息
bool GetDauInf(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//召唤录波文件
bool GetRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//召唤分通道录波文件
bool GetSplitRecWaveFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//召唤录波文件列表
bool GetRecWaveFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//召唤通用文件列表
bool GetFileList(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//简要录波报告
bool GetReportFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//对时（时间同步）
bool SetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//获取时间
bool GetTime(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//停止录波
bool StopRecWave(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//手动启动录波
bool StartRecByManual(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//定时启动录波
bool StartRecByTimer(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//读动态纪录数据
bool GetContData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//获取文件
bool PullFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//下传文件
bool PushFile(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//读实时数据
bool ReadRltData(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//读实零漂刻度
bool ReadZeroOffset(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//开始发电机实验
bool StartGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//停止发电机实验
bool StopGnExperiment(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//主动上传
bool AutoSend(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//读装置状态
bool GetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//设置装置状态
bool SetDeviceState(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

//硬件操作
bool OperateDevice(Protocol103 *pProtocol,Linker * pLinker,struct Command& cmd);

extern PRO2CMD pro2CmdMap[22];

/**********由协议功能到命令的对应关系<--***********************/

#endif