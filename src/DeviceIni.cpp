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
	//��ڲ������
	if ( nDeviceID == -1 || strSysPath.empty() )
	{
		return false;
	}

	//�õ����涨ֵ�ļ���·��
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

	//�����豸ID��ʼ��������ֵ�ļ�����
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

	//�õ���·��Ϣ
	SH_ILIN_PARM *pLine = shDevice.GetLines( m_nLineCount );
	if ( pLine != NULL )
	{
		for ( int i=0; i<m_nLineCount; i++ )
		{
			m_vSHLine.push_back( *(pLine+i) );
		}
	}
	//�õ���������Ϣ
	SH_BINCH_PARM *pSwitch = shDevice.GetSwitches( m_nSwitchCount );
	if ( pSwitch != NULL )
	{
		for ( int i=0; i<m_nSwitchCount; i++ )
		{
			m_vSHSwCh.push_back( *(pSwitch+i) );
		}
	}
	//�õ�ͨ����Ϣ
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
	//�õ���ѹ����Ϣ
	SH_TRN_PARM *pTrans = shDevice.GetTransformers( m_nTransCount );
	if ( pTrans != NULL )
	{
		for (int i=0; i<m_nTransCount; i++ )
		{
			m_vSHTransformer.push_back( *(pTrans+i) );
		}
	}
	//�õ��������Ϣ
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
	//��·��Ϣ
	return m_vSHLine;
}

vector<SH_ANCH_PARM>& CDeviceIni::GetChannelInfo()
{
	//ͨ����Ϣ
	return m_vSHAnaCh;
}

vector<SH_BINCH_PARM>& CDeviceIni::GetSwitchInfo()
{	
	//��������Ϣ
	return m_vSHSwCh;
}

vector<SH_TRN_PARM>& CDeviceIni::GetTransformerInfo()
{
	//��ѹ����Ϣ
	return m_vSHTransformer;
}

vector<SH_GN_PARM>& CDeviceIni::GetGeneratorInfo()
{
	//�������Ϣ
	return m_vSHGn;
}

int CDeviceIni::GetID()
{
	//�豸ID
	return m_nDeviceID;
}

void CDeviceIni::ClearDeviceInfo()
{
	m_nDeviceID = -1;
	m_iStationID = 0;

	//��¼��·
	if ( !m_vSHLine.empty() )
	{
		m_vSHLine.clear();
		m_nLineCount = 0;
	}

	//��¼��ѹ��
	if ( !m_vSHTransformer.empty() )
	{
		m_vSHTransformer.clear();
		m_nTransCount = 0;
	}

	//��¼�����
	if ( !m_vSHGn.empty() )
	{
		m_vSHGn.clear();
		m_nGeneratorCount = 0;
	}

	//��¼ͨ��
	if ( !m_vSHAnaCh.empty() )
	{
		m_vSHAnaCh.clear();
		m_nChannelCount = 0;
	}

	//��¼������
	if ( !m_vSHSwCh.empty() )
	{
		m_vSHSwCh.clear();
		m_nSwitchCount = 0;
	}
}

bool CDeviceIni::RefreshDevice()
{
	//����װ���豸��Ϣ
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
	if( iType == 0)//������¼
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
