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
	//��ʼ���豸
	CDeviceIni dev;
	if ( !dev.InitDevice( nStationID, nDeviceID, strSysPath ) )
	{
		return false;
	}

	m_DeviceMap.insert(map<int,CDeviceIni>::value_type(nDeviceID,dev));

	//�豸����1
	m_nDeviceNumber++;

	return true;
}

void CDeviceIniMgr::ReleaseDevice()
{
	//�ͷ���Դ
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
	//�����豸
	map<int,CDeviceIni>::iterator it = m_DeviceMap.find(nDeviceID);
	if(it != m_DeviceMap.end())
	{
		return &( (*it).second );
	}

	return NULL;
}

bool CDeviceIniMgr::RefreshDevice(int nDeviceID)
{
	//ˢ���豸
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
