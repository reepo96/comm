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

#include "P61850Linker.h"
#include "Protocol61850.h"

P61850Linker::P61850Linker(void): m_isListenLinker( false )
{
}

P61850Linker::~P61850Linker(void)
{
}

void P61850Linker::Init( void * parm, LINKERTYPE linkerType )
{
	m_CommType = CT_SOCKET;
	//��ʼ�����Ӳ���
	memset(&strLinkParm,0,sizeof(struct tagLinkParm));
	memcpy(&strLinkParm,parm,sizeof(struct tagLinkParm));

	//���ø��ຯ��
	Linker::Init(parm,linkerType);

	if(m_pProtocol != NULL)
	{
		delete m_pProtocol;
		m_pProtocol = NULL;
	}

	m_ProtocolType = PRO_61850;
	m_pProtocol = new Protocol61850(strLinkParm.cfgFile,strLinkParm.devName,strLinkParm.ARName);
}

bool P61850Linker::CreatTask( void )
{
	Task* pTask = m_pProtocol->Decode(this);
	return false;
}

bool P61850Linker::ExecuteCmd( struct Command & cmd )
{
	assert(m_pProtocol != NULL);
	return m_pProtocol->ExecuteCmd(this,cmd);
}

bool P61850Linker::Connect( void )
{
	m_isExitLink = false;
	m_isConnect = false;
	m_isInitComm = false;

	char szLogInf[256] = {0};

	Protocol61850 *pTmpProtocol = (Protocol61850*)m_pProtocol;
	//����Զ��
	if( pTmpProtocol->Connect() == false)
	{
		mylog.WriteLog2File(LogControl::LERROR,"Connect to client error in P61850Linker::Connect()");
		return false;
	}

	m_isConnect = true;

	//Э�����ӳ�ʼ��
	if(!m_pProtocol->InitConnect(this))
	{
		this->Close();
		mylog.WriteLog2File(LogControl::LERROR,"the protocol init connect with TCP/IP error");
		return false;
	}

	m_isInitComm = true;

	return true;
}

bool P61850Linker::BindUdpLink( void )
{
	return true;
}

bool P61850Linker::GetUdpData( struct Command & cmd )
{
	return m_pProtocol->GetUdpData(this,cmd);
}

bool P61850Linker::Close( void )
{
	Protocol61850 *pTmpProtocol = (Protocol61850*)m_pProtocol;
	pTmpProtocol->Close();
	m_isConnect = false;
	m_isInitComm = false;
	return true;
}

string P61850Linker::GetConnectAddr( void )
{
	return string(strLinkParm.cfgFile);
}