// Device.cpp: implementation of the CDevice class.
//
//////////////////////////////////////////////////////////////////////
#include "public.h"
#include "DeviceIni.h"
#include  <io.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeviceIni::CDeviceIni():
		m_nDeviceID(-1),
		m_strSysPath(""),
		m_nLineCount(0),
		m_nTransCount(0),
		m_nGeneratorCount(0),
		m_nChannelCount(0),
		m_nSwitchCount(0)
{
	memset(&m_AnaChMultipliers[0],0,sizeof(m_AnaChMultipliers));
	m_iStationID = 0;
}

CDeviceIni::~CDeviceIni()
{

}

bool CDeviceIni::InitDevice(int iStationID,int nDeviceID, string strSysPath)
{
	//入口参数检查
	if ( nDeviceID == -1 || strSysPath.empty() )
	{
		return false;
	}

	//得到保存定值文件的路径
	char szPath[256]={0};

	/*if(iStationID > 0 )
	{
		sprintf(szPath,"%s/%d/%d/SHDevice.dat",strSysPath.c_str(), iStationID,nDeviceID );
	}
	else
	{*/
		sprintf(szPath,"%s\\%d\\SHDevice_1.dat",strSysPath.c_str(), nDeviceID );
		if( _access(szPath,0) == -1)
		{
			sprintf(szPath,"%s\\%d\\SHDevice_2.dat",strSysPath.c_str(), nDeviceID );
		}

	//}

	//根据设备ID初始化操作定值文件的类
	SHDevice shDevice( szPath);
	if ( 0 == shDevice.CheckDone() )
	{
		mylog.WriteLog2File(LogControl::LERROR,"Read const file fail!!!");
		return false;
	}

	ClearDeviceInfo();
	m_iStationID = iStationID;
	m_nDeviceID = nDeviceID;
	m_strSysPath = strSysPath;

	m_theSHConstHead = *(shDevice.GetConstHead());

	//得到线路信息
	SH_ILIN_PARM *pLine = shDevice.GetLines( m_nLineCount );
	if ( pLine != NULL )
	{
		for ( int i=0; i<m_nLineCount; i++ )
		{
			m_vSHLine.push_back( *(pLine+i) );
		}
	}
	//得到开关量信息
	SH_BINCH_PARM *pSwitch = shDevice.GetSwitches( m_nSwitchCount );
	if ( pSwitch != NULL )
	{
		for ( int i=0; i<m_nSwitchCount; i++ )
		{
			m_vSHSwCh.push_back( *(pSwitch+i) );
		}
	}
	//得到通道信息
	SH_ANCH_PARM *pChannel = shDevice.GetChannels( m_nChannelCount );
	if ( pChannel != NULL )
	{
		for (int i=0; i<m_nChannelCount; i++ )
		{
			if(i < 96)
				m_AnaChMultipliers[i] = (pChannel+i)->AnCfg.Scaler;
			
			m_vSHAnaCh.push_back( *(pChannel+i) );
		}
	}
	//得到变压器信息
	SH_TRN_PARM *pTrans = shDevice.GetTransformers( m_nTransCount );
	if ( pTrans != NULL )
	{
		for (int i=0; i<m_nTransCount; i++ )
		{
			m_vSHTransformer.push_back( *(pTrans+i) );
		}
	}
	//得到发电机信息
	SH_GN_PARM *pGenerator = shDevice.GetGenerators( m_nGeneratorCount );
	if ( pGenerator != NULL )
	{
		for (int i=0; i<m_nGeneratorCount; i++ )
		{
			m_vSHGn.push_back( *(pGenerator+i) );
		}
	}
	return true;
}

vector<SH_ILIN_PARM>& CDeviceIni::GetLineInfo()
{	
	//线路信息
	return m_vSHLine;
}

vector<SH_ANCH_PARM>& CDeviceIni::GetChannelInfo()
{
	//通道信息
	return m_vSHAnaCh;
}

vector<SH_BINCH_PARM>& CDeviceIni::GetSwitchInfo()
{	
	//开关量信息
	return m_vSHSwCh;
}

vector<SH_TRN_PARM>& CDeviceIni::GetTransformerInfo()
{
	//变压器信息
	return m_vSHTransformer;
}

vector<SH_GN_PARM>& CDeviceIni::GetGeneratorInfo()
{
	//发电机信息
	return m_vSHGn;
}

int CDeviceIni::GetID()
{
	//设备ID
	return m_nDeviceID;
}

void CDeviceIni::ClearDeviceInfo()
{
	m_nDeviceID = -1;
	m_iStationID = 0;

	//记录线路
	if ( !m_vSHLine.empty() )
	{
		m_vSHLine.clear();
		m_nLineCount = 0;
	}

	//记录变压器
	if ( !m_vSHTransformer.empty() )
	{
		m_vSHTransformer.clear();
		m_nTransCount = 0;
	}

	//记录发电机
	if ( !m_vSHGn.empty() )
	{
		m_vSHGn.clear();
		m_nGeneratorCount = 0;
	}

	//记录通道
	if ( !m_vSHAnaCh.empty() )
	{
		m_vSHAnaCh.clear();
		m_nChannelCount = 0;
	}

	//记录开关量
	if ( !m_vSHSwCh.empty() )
	{
		m_vSHSwCh.clear();
		m_nSwitchCount = 0;
	}
}

bool CDeviceIni::RefreshDevice()
{
	//重新装载设备信息
	return InitDevice(m_iStationID,m_nDeviceID, m_strSysPath);
}

SH_DFR_PARM & CDeviceIni::GetHead()
{
	return m_theSHConstHead;
}

float CDeviceIni::GetAnaChMultiplier(unsigned short usAnaChNum)
{
	if(usAnaChNum == 0 || usAnaChNum> 96)
	{
		return 0.00;
	}
	return m_AnaChMultipliers[usAnaChNum-1];
}

bool CDeviceIni::IsEnabled(int iType)
{
	int iEnabled = 0;
	if( iType == 0)//触发记录
	{
		iEnabled = m_theSHConstHead.DfrCfg.TrnsEna;
	}
	else
	{
		iEnabled = m_theSHConstHead.DfrCfg.ContEna;
	}

	if(iEnabled > 0 )
	{
		return true;
	}
	else
	{
		return false;
	}
}
