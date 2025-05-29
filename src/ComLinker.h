// {{{RME classifier 'Logical View::ComLinker'

#ifndef ComLinker_H
#define ComLinker_H

#ifdef PRAGMA
#pragma interface "ComLinker.h"
#endif

#include <RTSystem/Communication.h>
#include <Linker.h>

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR

#pragma warning( disable : 4786 )

#include "public.h"
#include "ComParm.h"

/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：ComLinker.h
* 文件标识：
* 摘    要：串口连接类的头文件
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2007年7月4日
*
**************************************************************************/


// }}}USR
// }}}RME

class ComLinker : public Linker
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    //单元测试代码
	    string WhoAmI();
#endif

	    /*************************************************
	    Function:       Listen
	    Description:    监听网络连接
	    Calls:          
	    Called By:      
	    Input:          DisposeFun：处理函数

	    Output:         
	    Return:         
	    *************************************************/	   
	    void Listen( DWORD (WINAPI DisposeFun)(void *pParm) = 0 );

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

protected:
	// {{{RME classAttribute 'strLinkParm'
	// {{{RME general 'documentation'
	/* {{{USR
	连接参数
	   }}}USR */
	// }}}RME
	ComParm strLinkParm;
	// }}}RME
	// {{{RME classAttribute 'm_hComDev'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	HANDLE m_hComDev;
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	ComLinker( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~ComLinker( void );
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
	void Init( void * parm, LINKERTYPE linkerType );
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
	bool ExecuteCmd( struct Command & cmd );
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
	bool Connect( void );
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
	int SendData( const char * pDataBuf, int iDataLen, unsigned int uiTimeOut );
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
	bool CreatTask( void );
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
	int ReceiveData( char * pDataBuf, int iBufferLen, unsigned int uiTimeOut );
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
	bool Close( void );
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
	string GetConnectAddr( void );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* ComLinker_H */

// }}}RME
