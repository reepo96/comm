/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* 文件名称：P61850Linker.h
* 文件标识：
* 摘    要：61850协议相关的虚拟连接类
* 
* 当前版本：1.0
* 作    者：libo
* 完成日期：2009年5月7日
*
**************************************************************************/

#pragma once
#include "linker.h"
#include "public.h"
#include "SocketParm.h"
#include "LogControl.h"

class P61850Linker :
	public Linker
{
public:
	P61850Linker(void);
public:
	virtual ~P61850Linker(void);

public:
	struct tagLinkParm
	{
		char cfgFile[64];
		char devName[128];
		char ARName[128];

		tagLinkParm()
		{
			memset(cfgFile,0,sizeof(cfgFile));
			memset(devName,0,sizeof(devName));
			memset(ARName,0,sizeof(ARName));
		}
	};

protected:
	struct tagLinkParm strLinkParm; //连接参数
	bool m_isListenLinker;	//是用于监听的链接，还是普通的链接

public:

	/*************************************************
	      Function:       Init
	      Description:    初始化连接
	      Calls:          
	      Called By:      
	      Input:          parm:通讯连接所需参数
	                      linkerType:连接类型(连接远程或连接DAU)
	      Output:         
	      Return:         
	    *************************************************/
	void Init( void * parm, LINKERTYPE linkerType );

	/*************************************************
	      Function:		CreatTask
	      Description:	监听到数据后调用此函数创建任务(本例中用于侦测链接是否正常)
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:		创建成功返回true，失败返回false
	    *************************************************/
	bool CreatTask( void );

	/*************************************************
	      Function:       ExecuteCmd
	      Description:    执行通讯任务
	      Calls:          
	      Called By:      
	      Input:          cmd：保存通讯任务信息
	                      
	      Output:         
	      Return:         执行成功返回true，否则返回false
	    *************************************************/
	virtual bool ExecuteCmd( struct Command & cmd );

	/*************************************************
	      Function:       Connect
	      Description:    连接到服务器
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         连接成功返回true，否则返回false
	    *************************************************/
	bool Connect( void );

	bool BindUdpLink( void );

	bool GetUdpData( struct Command & cmd );

	/*************************************************
	      Function:       Close
	      Description:    关闭连接
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         成功关闭返回true，否则返回false
	    *************************************************/
	bool Close( void );

	/*************************************************
	      Function:       GetConnectAddr
	      Description:    获得链接目的的地址：IP地址
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         返回链接目的的地址
	    *************************************************/
	virtual string GetConnectAddr( void );
};
