// DeviceManager.h: interface for the CDeviceManager class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICEMANAGER_H__4C3B5452_659A_49B9_88FB_B6078C71E7B7__INCLUDED_)
#define AFX_DEVICEMANAGER_H__4C3B5452_659A_49B9_88FB_B6078C71E7B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DeviceIni.h"
#include <map>
using namespace std;

//�豸��ֵ�ļ�����
class CDeviceIniMgr 
{
public:
	
// Constructor
public:
	CDeviceIniMgr();
	virtual ~CDeviceIniMgr();

// Attribute
private:
	map<int,CDeviceIni> m_DeviceMap;

public:

// Operate
private:
	void ReleaseDevice(void);
	int m_nDeviceNumber;
	
public:
	int GetDeviceNumber(void);
	bool RefreshDevice(int nDeviceID);
	CDeviceIni* GetDevice(int nDeviceID);
	bool DeleteDevice(int nDeviceID);
	bool AddDevice(int nStationID,int nDeviceID,string strSysPath);

	/*************************************************
	    Function:       IsEnabled
	    Description:    �豸��ĳ�������Ƿ񼤻��
	    Calls:
	    Called By:
	    Input:          nDeviceID���豸���
						iType���������0-������¼��1-������¼

	    Output:
	    Return:         ���ܼ����true��û�м����false
	*************************************************/
	bool IsEnabled(int nDeviceID,int iType);
};

#endif // !defined(AFX_DEVICEMANAGER_H__4C3B5452_659A_49B9_88FB_B6078C71E7B7__INCLUDED_)
