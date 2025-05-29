// {{{RME classifier 'Logical View::Linker'

#ifndef Linker_H
#define Linker_H

#ifdef PRAGMA
#pragma interface "Linker.h"
#endif

#include <RTSystem/Communication.h>
class MsgQueue;
class QueRegister;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：Linker.h
* 文件标识：
* 摘    要：链接的父类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/

#include "public.h"
#include "QueRegister.h"

#include <Protocol.h>

//通讯错误代码
typedef enum linkErrorCode{LEC_NOERROR,LEC_LINKERR,LEC_LINK_TIMEOUT,LEC_USER_TERMINATE,LEC_LINK_UNKOWN_ERR,\
        LEC_PRO_UNKOWN_ERR,LEC_PRO_ERRCMD_NODATA,LEC_PRO_INTERRPUT,LEC_PRO_ERR_ANSWER,LEC_PRO_ERR_FRAME,\
		LEC_PRO_STARTDT_NOACT,LEC_PRO_RMT_STOP_TRANSFER,LEC_PRO_TIME_OUT,LEC_PRO_OPERAT_FILE_FAIL} LINKERRORCODE;

// }}}USR
// }}}RME

class Linker
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    //单元测试代码
	    static bool m_bConnectFail; //仅用于单元测试，指示linker的connect函数成功或失败

	    virtual string WhoAmI();
#endif

	    typedef enum LinkerType{LK_REMOTE,LK_ASDU,LK_UNKNOW} LINKERTYPE; //连接类型

		typedef enum CommType{CT_UNKNOW,CT_SOCKET,CT_COM,CT_MODEM} COMMTYPE; //通讯类型

	    /*************************************************
	    Function:       Listen
	    Description:    监听网络连接
	    Calls:          
	    Called By:      
	    Input:          DisposeFun:处理函数

	    Output:
	    Return:
	    *************************************************/	   
		virtual void Listen( DWORD (WINAPI DisposeFun)(void *pParm) = 0 );

		COMMTYPE GetCommType()
		{
			return m_CommType;
		}

		//获取命令描述信息
		void GetCmdDesc(struct Command & cmd);

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR
	    PROTOCOL_TYPE m_ProtocolType;
	    LINKERTYPE   m_LinkerType;
		COMMTYPE	 m_CommType;

	    LINKERRORCODE m_errCode;
		int			m_iUserCount;

		unsigned int	m_nDevId;//链接关联的设备ID
	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

protected:
	// {{{RME classAttribute 'm_Link'
	// {{{RME general 'documentation'
	/* {{{USR
	通讯连接的句柄
	   }}}USR */
	// }}}RME
	HANDLE m_Link;
	// }}}RME
	// {{{RME classAttribute 'm_isConnect'
	// {{{RME general 'documentation'
	/* {{{USR
	是否是连接的
	   }}}USR */
	// }}}RME
	bool m_isConnect;
	// }}}RME
	// {{{RME classAttribute 'm_isInitComm'
	// {{{RME general 'documentation'
	/* {{{USR
	通讯是否已经初始化（在通讯连接后要进行通讯初始化）
	   }}}USR */
	// }}}RME
	bool m_isInitComm;
	// }}}RME
	// {{{RME classAttribute 'm_isServerLink'
	// {{{RME general 'documentation'
	/* {{{USR
	是否是服务器端的连接（通常是指socket通讯的服务端）
	   }}}USR */
	// }}}RME
	bool m_isServerLink;
	// }}}RME
	// {{{RME classAttribute 'm_isExitLink'
	// {{{RME general 'documentation'
	/* {{{USR
	是否要退出连接
	   }}}USR */
	// }}}RME
	bool m_isExitLink;
	// }}}RME
	// {{{RME associationEnd 'm_pProtocol'
	Protocol * m_pProtocol;
	// }}}RME
	// {{{RME associationEnd 'm_pMsgQue'
	MsgQueue * m_pMsgQue;
	// }}}RME

	bool m_isPreDisConnect;	//是否要预断开链接

public:
		//     *************************************************
		//     Function:       PreClose
		//     Description:    预关闭链接（其它线程试图关闭另一线程的链接时调用此借口）
		//     Calls:
		//     Called By:
		//     Input:
		// 
		//     Output:
		//     Return:
		// *************************************************
		void PreClose() { m_isPreDisConnect = true; }

		bool IsPreDisConnect() { return m_isPreDisConnect;}

		virtual int GetHandle() { return (int)m_Link;}

		void SetDevId(unsigned int nDevId) { m_nDevId = nDevId;}

		unsigned int GetDevId() { return m_nDevId;}

	// {{{RME operation 'Linker()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	Linker( void );
	// }}}RME
	// {{{RME operation '~Linker()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual ~Linker( void );
	// }}}RME
	// {{{RME operation 'Init(void*,LINKERTYPE)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Init
	      Description:    初始化连接
	      Calls:          
	      Called By:      
	      Input:          parm:通讯连接所需参数
	                      linkerType:连接类型(连接远程或连接DAU)
	      Output:         
	      Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual void Init( void * parm, LINKERTYPE linkerType );
	// }}}RME
	// {{{RME operation 'ExecuteCmd(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ExecuteCmd
	      Description:    执行通讯任务
	      Calls:          
	      Called By:      
	      Input:          cmd：保存通讯任务信息
	                      
	      Output:         
	      Return:         执行成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( struct Command & cmd );
	// }}}RME
	// {{{RME operation 'Connect()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Connect
	      Description:    连接到服务器
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         连接成功返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool Connect( void );
	// }}}RME
	// {{{RME operation 'BindUdpLink()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool BindUdpLink( void );
	// }}}RME
	// {{{RME operation 'SendData(const char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       SendData
	      Description:    发送数据
	      Calls:          
	      Called By:      
	      Input:          pDataBuf：指向要发送的数据
	                      iDataLen：要发送数据的长度
	      Output:         
	      Return:         实际发送数据的长度
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual int SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'CreatTask()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       CreatTask
	      Description:    监听到数据后调用此函数创建任务
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         创建成功返回true，失败返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool CreatTask( void );
	// }}}RME
	// {{{RME operation 'GetUdpData(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	virtual bool GetUdpData( struct Command & cmd );
	// }}}RME
	// {{{RME operation 'ReceiveData(char*,int,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ReceiveData
	      Description:    接收数据
	      Calls:          
	      Called By:      
	      Input:          pDataBuf：保存接收到的数据
	                      iBufferLen：期望接收数据的长度
	                      uiTimeOut：超时时间（单位：毫秒），为0则永不超时
	      Output:         
	      Return:         实际接收到的数据长度
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual int ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
	// }}}RME
	// {{{RME operation 'ReceiveDataFrom(char*,int,unsigned int,struct sockaddr&)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       ReceiveData
	      Description:    接收数据
	      Calls:          
	      Called By:      
	      Input:          pDataBuf：保存接收到的数据
	                      iBufferLen：期望接收数据的长度
	                      uiTimeOut：超时时间（单位：毫秒），为0则永不超时
	      Output:         
	      Return:         实际接收到的数据长度
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual int ReceiveDataFrom( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut, struct sockaddr & clientAddr );
	// }}}RME
	// {{{RME operation 'Close()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       Close
	      Description:    关闭连接
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         成功关闭返回true，否则返回false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool Close( void );
	// }}}RME
	// {{{RME operation 'GetConnectAddr()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetConnectAddr
	    Description:    获得链接目的的地址：IP地址，电话号码或者串口名
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         返回链接目的的地址
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual string GetConnectAddr( void );
	// }}}RME
	// {{{RME operation 'IsConnect()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsConnect
	    Description:    判断是否链接的
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         已经连接返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool IsConnect( void );
	// }}}RME
	// {{{RME operation 'IsInitComm()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsInitComm
	    Description:    通讯是否已经初始化（在通讯连接后要进行通讯初始化）
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         已经初始化返回true，否则返回false
	    *************************************************		
	   }}}USR */
	// }}}RME
	bool IsInitComm( void );
	// }}}RME
	// {{{RME operation 'IsServerLink()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       IsServerLink
	    Description:    是否是服务器端的连接（通常是指socket通讯的服务端）
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         是返回true，否则返回false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool IsServerLink( void );
	// }}}RME
	// {{{RME operation 'GetLinkType()'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	LINKERTYPE GetLinkType( void );
	// }}}RME
	// {{{RME operation 'ExitLink()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExitLink
	    Description:    退出连接
	    Calls:          
	    Called By:      
	    Input:          

	    Output:         
	    Return:         
	    *************************************************		
	   }}}USR */
	// }}}RME
	void ExitLink( void );
	// }}}RME
	// {{{RME operation 'GetLastErrCode()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrCode
	      Description:    获得最近发生的一次错误代码
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         错误代码
	    *************************************************
	   }}}USR */
	// }}}RME
	LINKERRORCODE GetLastErrCode( void );
	// }}}RME
	// {{{RME operation 'GetLastErrDesc()'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	      Function:       GetLastErrDesc
	      Description:    获得最近发生的一次错误描述
	      Calls:          
	      Called By:      
	      Input:          
	                      
	                      
	      Output:         
	      Return:         错误描述
	    *************************************************
	   }}}USR */
	// }}}RME
	const char * GetLastErrDesc( void );
	// }}}RME
	// {{{RME operation 'DoIdle()'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DoIdle
	    Description:    处理空闲
	    Calls:          
	    Called By:      
	    Input:          pLinker：通讯连接
	                    
	    Output:         
	    Return:         
	    *************************************************	
	   }}}USR */
	// }}}RME
	void DoIdle( void );
	// }}}RME

	//处理主动上传数据
	void DealAutoSend()
	{
		if( m_pProtocol )
		{
			m_pProtocol->DealAutoSend(this);
		}
	}

	void AddRef(){ m_iUserCount++;}

	void SaftDelete()
	{
		m_iUserCount--;
		if( m_iUserCount <= 0 )
		{
			delete this;
		}
	}
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Linker_H */

// }}}RME
