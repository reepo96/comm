// Device.h: interface for the CDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICE_H__EAAAFEF2_0C83_476B_99AA_4D86AAD1D377__INCLUDED_)
#define AFX_DEVICE_H__EAAAFEF2_0C83_476B_99AA_4D86AAD1D377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SHDevice.h"
#include <vector>
#include <string>
using namespace std;

class CDeviceIni  
{
public:
	CDeviceIni();
	virtual ~CDeviceIni();

// Attribute
private:
	//通道比例系数数组
	float m_AnaChMultipliers[96];

	//变电机id
	int m_iStationID;

	//设备ID
	int m_nDeviceID;
	//保存定值目录
	string m_strSysPath;

	//定值文件头
	SH_DFR_PARM m_theSHConstHead;

	//记录采集设备的线路
	INT m_nLineCount;
	std::vector<SH_ILIN_PARM> m_vSHLine;

	//记录采集设备的变压器数
	INT m_nTransCount;
	std::vector<SH_TRN_PARM> m_vSHTransformer;

	//记录采集设备的发电机数
	INT m_nGeneratorCount;
	std::vector<SH_GN_PARM> m_vSHGn;

	//记录采集设备的通道数
	INT m_nChannelCount;
	std::vector<SH_ANCH_PARM> m_vSHAnaCh;

	//记录采集设备的开关量数
	INT m_nSwitchCount;
	std::vector<SH_BINCH_PARM> m_vSHSwCh;
public:

// Operate
private:
	void ClearDeviceInfo(void);
	
public:
	SH_DFR_PARM & GetHead(void);
	bool RefreshDevice(void);
	int GetID(void);
	std::vector<SH_GN_PARM>& GetGeneratorInfo();
	std::vector<SH_TRN_PARM>& GetTransformerInfo();
	std::vector<SH_BINCH_PARM>& GetSwitchInfo();
	std::vector<SH_ANCH_PARM>& GetChannelInfo();
	std::vector<SH_ILIN_PARM>& GetLineInfo();
	bool InitDevice(int iStationID,int nDeviceID, string strSysPaht);

	//获得通道比例系数
	//iAnaChNum:通道号(1~96)
	float GetAnaChMultiplier(unsigned short usAnaChNum);

	/*************************************************
	    Function:       IsEnabled
	    Description:    设备的某个功能是否激活的
	    Calls:
	    Called By:
	    Input:          iType：功能类别：0-触发记录；1-连续记录

	    Output:
	    Return:         功能激活返回true，没有激活返回false
	*************************************************/
	bool IsEnabled(int iType);
};

#endif // !defined(AFX_DEVICE_H__EAAAFEF2_0C83_476B_99AA_4D86AAD1D377__INCLUDED_)
