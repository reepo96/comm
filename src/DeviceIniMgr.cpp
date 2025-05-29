// DeviceManager.cpp: implementation of the CDeviceManager class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "DeviceIniMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceIniMgr::CDeviceIniMgr():			
			m_nDeviceNumber(0)
{

}

CDeviceIniMgr::~CDeviceIniMgr()
{
	ReleaseDevice();
}

bool CDeviceIniMgr::AddDevice(int nStationID,int nDeviceID, string strSysPath)
{
	//初始化设备
	CDeviceIni dev;
	if ( !dev.InitDevice( nStationID, nDeviceID, strSysPath ) )
	{
		return false;
	}

	m_DeviceMap.insert(map<int,CDeviceIni>::value_type(nDeviceID,dev));

	//设备数加1
	m_nDeviceNumber++;

	return true;
}

void CDeviceIniMgr::ReleaseDevice()
{
	//释放资源
	m_DeviceMap.clear();
}

bool CDeviceIniMgr::DeleteDevice(int nDeviceID)
{
	map<int,CDeviceIni>::iterator it = m_DeviceMap.find(nDeviceID);
	if(it != m_DeviceMap.end())
	{
		m_DeviceMap.erase(it);
	}
	return true;
}

CDeviceIni * CDeviceIniMgr::GetDevice(int nDeviceID)
{
	//查找设备
	map<int,CDeviceIni>::iterator it = m_DeviceMap.find(nDeviceID);
	if(it != m_DeviceMap.end())
	{
		return &( (*it).second );
	}

	return NULL;
}

bool CDeviceIniMgr::RefreshDevice(int nDeviceID)
{
	//刷新设备
	CDeviceIni *pDevice = GetDevice(nDeviceID);
	if (pDevice == NULL)
	{
		return false;
	}
	return pDevice->RefreshDevice();
}

int CDeviceIniMgr::GetDeviceNumber()
{
	return m_nDeviceNumber;
}

bool CDeviceIniMgr::IsEnabled(int nDeviceID,int iType)
{
	CDeviceIni *pDevice = GetDevice(nDeviceID);
	if (pDevice == NULL)
	{
		return false;
	}

	return pDevice->IsEnabled(iType);
}
