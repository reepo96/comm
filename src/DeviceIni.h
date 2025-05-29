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
	//ͨ������ϵ������
	float m_AnaChMultipliers[96];

	//����id
	int m_iStationID;

	//�豸ID
	int m_nDeviceID;
	//���涨ֵĿ¼
	string m_strSysPath;

	//��ֵ�ļ�ͷ
	SH_DFR_PARM m_theSHConstHead;

	//��¼�ɼ��豸����·
	INT m_nLineCount;
	std::vector<SH_ILIN_PARM> m_vSHLine;

	//��¼�ɼ��豸�ı�ѹ����
	INT m_nTransCount;
	std::vector<SH_TRN_PARM> m_vSHTransformer;

	//��¼�ɼ��豸�ķ������
	INT m_nGeneratorCount;
	std::vector<SH_GN_PARM> m_vSHGn;

	//��¼�ɼ��豸��ͨ����
	INT m_nChannelCount;
	std::vector<SH_ANCH_PARM> m_vSHAnaCh;

	//��¼�ɼ��豸�Ŀ�������
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

	//���ͨ������ϵ��
	//iAnaChNum:ͨ����(1~96)
	float GetAnaChMultiplier(unsigned short usAnaChNum);

	/*************************************************
	    Function:       IsEnabled
	    Description:    �豸��ĳ�������Ƿ񼤻��
	    Calls:
	    Called By:
	    Input:          iType���������0-������¼��1-������¼

	    Output:
	    Return:         ���ܼ����true��û�м����false
	*************************************************/
	bool IsEnabled(int iType);
};

#endif // !defined(AFX_DEVICE_H__EAAAFEF2_0C83_476B_99AA_4D86AAD1D377__INCLUDED_)
