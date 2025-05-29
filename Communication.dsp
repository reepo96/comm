# Microsoft Developer Studio Project File - Name="Communication" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Communication - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Communication.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Communication.mak" CFG="Communication - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Communication - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Communication - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/产品开发/SH2000D录波器/SHNeuron故障记录分析中心/开发库/SRC/Communication/COMMUNICATION", RFMDAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Communication - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Deployment"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /W3 /GX /O2 /I "./src/" /I "../../DataBase/DBAdapter_DLL" /I "./include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tapi32.lib ws2_32.lib ../../Deployment/SHDeviceModule.lib ../../Deployment/WatchModule.lib zlib.lib acsi.lib /nologo /subsystem:console /machine:I386 /libpath:"../../Deployment" /libpath:"./lib"

!ELSEIF  "$(CFG)" == "Communication - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Zp1 /W3 /Gm /GX /ZI /Od /I "./src/" /I "./include" /I "./lib" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_LOGFILE" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib tapi32.lib ws2_32.lib ./Const/SHDeviceModule.lib ./WatchDog/WatchModule.lib ./ZipLib/zlib_d.lib acsi.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"./lib"

!ENDIF 

# Begin Target

# Name "Communication - Win32 Release"
# Name "Communication - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\APCI4Prot104.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CmdState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ComLinker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Communication.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CommunicationManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ComParm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DataControl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\debugnew.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevFailState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Device.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DeviceCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DeviceIni.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DeviceIniMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DeviceState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevIdleState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevInvalidState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevSuccessState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevTimeOutState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DevWorkState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FileControl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FileDB.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FTPCFileFatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FtpCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Linker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LogControl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MemMapMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ModemLinker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ModemParm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MsgQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\src\NormalComm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\P61850Linker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol103.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol103_struct.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104_GB.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104_ZJ.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol60.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol60_103.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Protocol61850.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QueRegister.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SocketLinker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SocketParm.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SysExitState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SysIdleState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SysInitState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SystemCfger.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SystemCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SystemState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SysWorkState.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Task.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Timer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\APCI4Prot104.h
# End Source File
# Begin Source File

SOURCE=.\src\CmdState.h
# End Source File
# Begin Source File

SOURCE=.\src\ComLinker.h
# End Source File
# Begin Source File

SOURCE=.\src\Command.h
# End Source File
# Begin Source File

SOURCE=.\src\CommunicationManager.h
# End Source File
# Begin Source File

SOURCE=.\src\ComParm.h
# End Source File
# Begin Source File

SOURCE=.\src\DataControl.h
# End Source File
# Begin Source File

SOURCE=.\src\debugnew.h
# End Source File
# Begin Source File

SOURCE=.\src\DevFailState.h
# End Source File
# Begin Source File

SOURCE=.\src\Device.h
# End Source File
# Begin Source File

SOURCE=.\src\DeviceCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\DeviceIni.h
# End Source File
# Begin Source File

SOURCE=.\src\DeviceIniMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\DeviceState.h
# End Source File
# Begin Source File

SOURCE=.\src\DevIdleState.h
# End Source File
# Begin Source File

SOURCE=.\src\DevInvalidState.h
# End Source File
# Begin Source File

SOURCE=.\src\DevSuccessState.h
# End Source File
# Begin Source File

SOURCE=.\src\DevTimeOutState.h
# End Source File
# Begin Source File

SOURCE=.\src\DevWorkState.h
# End Source File
# Begin Source File

SOURCE=.\src\FileControl.h
# End Source File
# Begin Source File

SOURCE=.\src\FileDB.h
# End Source File
# Begin Source File

SOURCE=.\src\FTPCFileFatcher.h
# End Source File
# Begin Source File

SOURCE=.\src\FtpCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\Linker.h
# End Source File
# Begin Source File

SOURCE=.\src\LogControl.h
# End Source File
# Begin Source File

SOURCE=.\src\MemMapMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\ModemLinker.h
# End Source File
# Begin Source File

SOURCE=.\src\ModemParm.h
# End Source File
# Begin Source File

SOURCE=.\src\MsgQueue.h
# End Source File
# Begin Source File

SOURCE=.\src\NormalComm.h
# End Source File
# Begin Source File

SOURCE=.\src\P61850Linker.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol103.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol103_struct.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104_GB.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol104_ZJ.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol60.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol60_103.h
# End Source File
# Begin Source File

SOURCE=.\src\Protocol61850.h
# End Source File
# Begin Source File

SOURCE=.\src\public.h
# End Source File
# Begin Source File

SOURCE=.\src\QueRegister.h
# End Source File
# Begin Source File

SOURCE=.\src\SocketLinker.h
# End Source File
# Begin Source File

SOURCE=.\src\SocketParm.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\SysExitState.h
# End Source File
# Begin Source File

SOURCE=.\src\SysIdleState.h
# End Source File
# Begin Source File

SOURCE=.\src\SysInitState.h
# End Source File
# Begin Source File

SOURCE=.\src\SystemCfger.h
# End Source File
# Begin Source File

SOURCE=.\src\SystemCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\SystemState.h
# End Source File
# Begin Source File

SOURCE=.\src\SysWorkState.h
# End Source File
# Begin Source File

SOURCE=.\src\Task.h
# End Source File
# Begin Source File

SOURCE=.\src\Timer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
