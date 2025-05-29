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

SystemCtrl *g_pSystemCtrlObj = NULL;//ϵͳ���ƶ���

//������ֹ�źź���
void SysExit(int iSignal)
{
	printf("Begin exit\r\n");
	g_bIsExitSys = true;
}

//��׽�رտ���̨�¼�
BOOL WINAPI ConsoleHandler(DWORD CEvent);

int main(int argc, char* argv[])
{
	printf("Begin run\r\n");

	//ͨ������һ���¼���ȷ��ֻ������һ��ͨѶ����
	HANDLE  hStartEvent = CreateEvent(NULL,FALSE,TRUE,"SH2000D_COMMUNICATION_START");
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		printf("Communication process is running,no run any more\r\n");
		::CloseHandle(hStartEvent);
		::Sleep(1000);
		return 0;
	}

	//�޸ĵ�ǰ·��ΪͨѶģ���ִ���ļ�����·��
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

