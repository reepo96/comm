// {{{RME classifier 'Logical View::Protocol104'

#ifndef Protocol104_H
#define Protocol104_H

#ifdef PRAGMA
#pragma interface "Protocol104.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Protocol104.h
* 文件标识：
* 摘    要：104协议
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2008年3月4日
*
**************************************************************************/

#include "public.h"
#include "APCI4Prot104.h"
#include "Command.h"

//104类型标示
#define	PRO104_ASDU6	6
#define	PRO104_ASDU7	7
#define	PRO104_ASDU8	8
#define	PRO104_ASDU20	20
#define	PRO104_ASDU21	21
#define	PRO104_ASDU120	120
#define	PRO104_ASDU122	122
#define	PRO104_ASDU124	124
#define	PRO104_ASDU101	101
#define	PRO104_ASDU103	103
#define	PRO104_ASDU104	104
#define	PRO104_ASDU105	105
#define	PRO104_ASDU221	221

#define	PRO104_ASDU100	100
#define	PRO104_ASDU10	10
#define	PRO104_ASDU11	11
#define	PRO104_ASDU121	121
#define	PRO104_ASDU123	123
#define	PRO104_ASDU102	102
#define	PRO104_ASDU104	104
#define	PRO104_ASDU106	106
#define	PRO104_ASDU108	108
#define	PRO104_ASDU220	220

#define	PRO104_ASDU13	13
#define	PRO104_ASDU15	15
#define	PRO104_ASDU17	17
#define	PRO104_ASDU113	113
#define	PRO104_ASDU1	1
#define	PRO104_ASDU14	14
#define	PRO104_ASDU16	16
#define	PRO104_ASDU18	18
#define	PRO104_ASDU114	114

//定义104协议通讯参数
/*const unsigned short P104_K = 256;		//参数k
const unsigned short P104_W = 168;		//参数w
const unsigned int P104_T1 = 15;		//参数t1
const unsigned int P104_T2 = 5;			//参数t2
const unsigned int P104_T3 = 25;		//参数t3*/

//104应用服务数据单元标识符
typedef struct tagASDUID104
{
    unsigned char TYP;      //类型标识
    unsigned char VSQ;    //信息元素的数目    
    unsigned char COT;      //传送原因
    unsigned short COMADDR;  //ASDU公共地址
}ASDUID104,*PASDUID104;

//应用服务数据单元信息体标识符
typedef struct tagASDUInfID104
{
    unsigned char FUN;      //功能类型
    unsigned char INF;      //信息序号
}ASDUINFID104,*PASDUINFID104;

const int ASDUID104_LEN = sizeof(ASDUID104);
const int ASDUINFID104_LEN = sizeof(ASDUINFID104);

#define	STRNO_GLIB 1
#define	STRNO_GEN 2
#define	STRNO_ALL_GROUP_TITLE 3
#define	STRNO_GROUP_ALL_ENTRY_VAL 4
#define	STRNO_ONE_ENTRY_DIR 5
#define	STRNO_ONE_ENTRY_VAL 6

typedef struct tagAsduStruct104
{
	int no;

	unsigned char VSQ;
	unsigned char FUN;
	unsigned char INF;
	char		  desc[64];
}ASDUSTRUCT104;

extern ASDUSTRUCT104 asduStruct104[6];

// }}}USR
// }}}RME

class Protocol104 : public Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
		int		m_iLenOfApdulen;		//APDU长度所占字节数（标准为1字节）

		const unsigned short P104_K ;		//参数k
		const unsigned short P104_W;		//参数w
		const unsigned int P104_T1 ;		//参数t1
		const unsigned int P104_T2;			//参数t2
		const unsigned int P104_T3 ;		//参数t3

		unsigned short	m_iHasSendNum:15;		//已发送的序列号
		unsigned short	m_iHasReceivNum:15;		//已接收的序列号
		unsigned short	m_iHasAckNum:15;		//已得到应答的序列号
		unsigned short	m_iNoAckNum;			//没有回答的包数

		DWORD	m_dwBeginT1Time;		//t1时间开始计时
		DWORD	m_dwBeginT2Time;		//t2时间开始计时
		DWORD	m_dwBeginT3Time;		//t3时间开始计时

		bool	m_bCanSendData;			//是否可以传输数据
				
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol104( void );
	// }}}RME
	// {{{RME operation 'Protocol104()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Protocol104( void );
	
		/*************************************************
	    Function:       SendIFrame
	    Description:    发送I格式帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
						pAsdu：指向ASDU部分
						iAsduLen：ASDU部分的长度
						pCmd：当前执行的命令
	                    
	    Output:         
	    Return:         -1：发生错误，0：停止发送，1:发送成功
	    *************************************************/
	virtual int SendIFrame(Linker * pLinker,const char *pAsdu,int iAsduLen,struct Command * pCmd);

	void Init();
	
	// }}}RME
	// {{{RME operation 'Decode(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       Decode
	    Description:    解析通过通讯传入的数据，生成一个任务对象
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         成功返回任务，失败返回NULL
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual Task * Decode( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseTask(APCI4Prot104*,Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Task * AnalyseTask( APCI4Prot104 * pAPCI, Linker * pLinker );
	// }}}RME

public:
	// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExecuteCmd
	    Description:    执行命令
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    cmd：要执行的命令
	    Output:         
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'AnalyseFrame(Linker *,char*,int*,APCI4Prot104*,PASDUID104*,PASDUINFID104*,char**,int*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseFrame
	    Description:    将网络数据流解析成一个帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    uiTimeOut：超时时间，单位毫秒，0表示永不超时
	    Output:         framePacket：保存返回帧的数据包
	                    frameLen：数据包长度
	                    pAPCI：APCI部分的数据
	                    pOutASDUID：ASDUID部分的数据
						pOutASDUInfID:ASDUInfID部分的数据
	                    pOutAsduData：信息体部分的数据
	                    pAsduDataLen：信息体长度
	    Return:         -1:网络错误或协议帧错误;  0:链路没有数据   1:成功解析出协议帧
	    *************************************************
	   }}}USR */
	// }}}RME
	int AnalyseFrame( Linker * pLinker, char * framePacket, int * frameLen, APCI4Prot104 * pAPCI, PASDUID104 * pOutASDUID, PASDUINFID104 * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, unsigned int uiTimeOut = 0 );
	// }}}RME

public:
	// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    召唤录波文件列表
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFile
	    Description:    召唤录波文件
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartDT(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartDT
	    Description:    接收到开始传输数据命令
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartDT( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StopDT(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StopDT
	    Description:    接收到停止传输数据命令
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StopDT( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SendTestFR(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendTestFR
	    Description:    发送测试帧
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool SendTestFR( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ConfirmTestFR(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ConfirmTestFR
	    Description:    收到测试帧，应答它
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    pCmd：要执行的命令                    
	                    
	    Output:                             
	    Return:         成功返回true，失败返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ConfirmTestFR( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'CreateIFrame(const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	string CreateIFrame( const char * pAsdu, int iAsduLen );
	// }}}RME
	// {{{RME operation 'CreateSFrame()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	string CreateSFrame( void );
	// }}}RME
	// {{{RME operation 'CReateUFrame(int)'
	// {{{RME general 'documentation'
	/* {{{USR
	创建U格式帧
	iType：0-STARTDT生效；1-STARTDT确认；2-STOPDT生效；3-STOPDT确认；4-TESTFR生效；5-TESTFR确认
	   }}}USR */
	// }}}RME
	string CReateUFrame( int iType );
	// }}}RME

public:
	// {{{RME operation 'DoIdle(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    处理空闲
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         处理空闲的任务
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DoIdle( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'EncodeAsdu(unsigned char,int,unsigned char,unsigned short,const char*,int)'
	// {{{RME general 'documentation'
	/* {{{USR
		*************************************************
	    Function:       EncodeAsdu
	    Description:    将ASDU编码成字符流
	    Calls:          
	    Called By:      
	    Input:          type：类型标识
	                    asduStructNo：序号
						cot：传送原因
						addr：地址
						pAsduData：ASDU数据
						iAsduDataLen：数据长度
	    Output:         
	                    
	    Return:         字符流
	    *************************************************
	   }}}USR */
	// }}}RME
	string EncodeAsdu( unsigned char type, int asduStructNo, unsigned char cot, unsigned short addr, const char * pAsduData, int iAsduDataLen );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

	#define	PRO104_TESTFR_ACT	0X43	//测试激活
	#define	PRO104_STOPDT_ACT	0X13	//停止数据传输激活
	#define	PRO104_STARTDT_ACT	0X07	//启动数据传输激活
		
	/****-------->定义系统内部使用的命令代码******/
	//发送测试帧
	const   unsigned char   CMD_104_SEND_TESTFR = 0X25;

	//应答测试帧
	const   unsigned char   CMD_104_CONFIRM_TESTFR = 0X26;

	//启动数据传输
	const   unsigned char   CMD_104_STARTDT = 0X27;

	//停止数据传输
	const   unsigned char   CMD_104_STOPDT = 0X28;

	/*********协议U格式帧与命令的对应关系-->***********************/
	typedef struct tagPro104UFrame2Cmd
	{
		unsigned char control1;      //U格式帧第一控制域
		unsigned char cmdFunCode;    //本系统内部使用的命令代码
		bool (*exuteCmd)(Protocol104*,Linker * ,struct Command&);  //用于执行命令的函数
		const char*	cmdName; //消息名称
	}PRO104UFRAME2CMD,*PPRO104UFRAME2CMD;

	extern PRO104UFRAME2CMD pro104UFrame2Cmd[4];

	//发送测试帧
	bool SendTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//应答测试帧
	bool ConfirmTestFR(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//启动数据传输
	bool StartDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

	//停止数据传输
	bool StopDT(Protocol104 *pProtocol,Linker *pLinker ,struct Command& cmd);

// }}}USR
// }}}RME

#endif /* Protocol104_H */

// }}}RME
