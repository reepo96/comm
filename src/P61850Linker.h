/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�P61850Linker.h
* �ļ���ʶ��
* ժ    Ҫ��61850Э����ص�����������
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2009��5��7��
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
	struct tagLinkParm strLinkParm; //���Ӳ���
	bool m_isListenLinker;	//�����ڼ��������ӣ�������ͨ������

public:

	/*************************************************
	      Function:       Init
	      Description:    ��ʼ������
	      Calls:          
	      Called By:      
	      Input:          parm:ͨѶ�����������
	                      linkerType:��������(����Զ�̻�����DAU)
	      Output:         
	      Return:         
	    *************************************************/
	void Init( void * parm, LINKERTYPE linkerType );

	/*************************************************
	      Function:		CreatTask
	      Description:	���������ݺ���ô˺�����������(������������������Ƿ�����)
	      Calls:
	      Called By:
	      Input:

	      Output:
	      Return:		�����ɹ�����true��ʧ�ܷ���false
	    *************************************************/
	bool CreatTask( void );

	/*************************************************
	      Function:       ExecuteCmd
	      Description:    ִ��ͨѶ����
	      Calls:          
	      Called By:      
	      Input:          cmd������ͨѶ������Ϣ
	                      
	      Output:         
	      Return:         ִ�гɹ�����true�����򷵻�false
	    *************************************************/
	virtual bool ExecuteCmd( struct Command & cmd );

	/*************************************************
	      Function:       Connect
	      Description:    ���ӵ�������
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         ���ӳɹ�����true�����򷵻�false
	    *************************************************/
	bool Connect( void );

	bool BindUdpLink( void );

	bool GetUdpData( struct Command & cmd );

	/*************************************************
	      Function:       Close
	      Description:    �ر�����
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         �ɹ��رշ���true�����򷵻�false
	    *************************************************/
	bool Close( void );

	/*************************************************
	      Function:       GetConnectAddr
	      Description:    �������Ŀ�ĵĵ�ַ��IP��ַ
	      Calls:          
	      Called By:      
	      Input:          
	                      
	      Output:         
	      Return:         ��������Ŀ�ĵĵ�ַ
	    *************************************************/
	virtual string GetConnectAddr( void );
};
