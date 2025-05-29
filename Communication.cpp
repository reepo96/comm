// Communication.cpp : Defines the entry point for the console application.
//
#pragma warning(disable : 4786)

#include "stdafx.h"
#include "debugnew.h"
#include "SystemCtrl.h"
#include <signal.h>
#include "SocketLinker.h"
#include <direct.h>


#ifdef _DEBUG
	DebugNewTracer myNewTracer;
#endif

bool	g_bIsExitSys = false;

SystemCtrl *g_pSystemCtrlObj = NULL;//系统控制对象

//处理中止信号函数
void SysExit(int iSignal)
{
	printf("Begin exit\r\n");
	g_bIsExitSys = true;
}

//捕捉关闭控制台事件
BOOL WINAPI ConsoleHandler(DWORD CEvent);

int main(int argc, char* argv[])
{
	printf("Begin run\r\n");

	//通过创建一个事件来确保只能启动一个通讯进程
	HANDLE  hStartEvent = CreateEvent(NULL,FALSE,TRUE,"SH2000D_COMMUNICATION_START");
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		printf("Communication process is running,no run any more\r\n");
		::CloseHandle(hStartEvent);
		::Sleep(1000);
		return 0;
	}

	//修改当前路径为通讯模块可执行文件所在路径
	char szCurDir[_MAX_PATH];
	GetModuleFileName( GetModuleHandle(NULL), szCurDir, _MAX_PATH );
	char *pPostfix = strrchr(szCurDir, '\\');
	*pPostfix = '\0';
	_chdir(szCurDir);

	//signal(SIGINT,SysExit);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);

	SystemCtrl sysCtrl;
    sysCtrl.Run();

	g_pSystemCtrlObj = NULL;
	::CloseHandle(hStartEvent);
	return 0;
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch(CEvent)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		g_bIsExitSys = true;
		printf("Begin exit\r\n");
		Sleep(3000);
	}
	return TRUE;
}

