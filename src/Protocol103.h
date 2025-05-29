// {{{RME classifier 'Logical View::Protocol103'

#ifndef Protocol103_H
#define Protocol103_H

#ifdef PRAGMA
#pragma interface "Protocol103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol103.h
* �ļ���ʶ��
* ժ    Ҫ������103Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"
#include "Command.h"

#ifdef _MY_UNITTEST
    class Protocol103_UT;
#endif

class Protocol;

const unsigned char PRO103_VOLATILEFRAME_STARTCHAR = 0X68;  //�ɱ�֡�������ַ�
const unsigned char PRO103_FIXFRAME_STARTCHAR = 0X10;       //�̶�֡�������ַ�
const unsigned char PRO103_ENDCHAR = 0X16;                  //103Э��Ľ����ַ�
const int PRO103_VFRAME_MAXSIZE = 1400;//4101;              //103Э��ɱ�֡��󳤶�
const int PRO103_ASDUDATA_MAXSIZE = 1388;//4089;            //103Э��ɱ�֡ASDU���ݲ���(����Ϣ�岿��)��󳤶�

const int PRO103_ASDU_FILENAMELEN = 40;                     //103Э����ASDU�����ļ���������Ŀ���ļ����ĳ���

//������Ķ���
typedef struct tagContolArea103
{
    union
    {
        //���Ʒ���
        struct
        {
            unsigned char fun:4;
            bool fcv:1;
            bool fcb:1;
            bool prm:1;
            char res:1;
        }c;

        //���ӷ���
        struct
        {
            unsigned char fun:4;
            bool dfc:1;
            bool acd:1;
            bool prm:1;
            char res:1;
        }m;

        unsigned char control;
    };
}CONTOLAREA103;

/***����103Э��ɱ�֡-->***/

//103Э��ɱ�֡֡ͷ
typedef struct tagVFrameHead
{
    struct tagHead
    {
        unsigned char startChar1;    //�����ַ�
        unsigned char lengthLow;    //���ĳ���=������1+��ַ1+��·�û�����x
        unsigned char lengthHigh:4;
        unsigned char reserve:4;    //����
        unsigned char startChar2;    //�����ַ�
    }HEAD;

    CONTOLAREA103 control;    //������
    unsigned char address;    //��·��ַ��
}VFRAMEHEAD,*PVFRAMEHEAD;

//103Ӧ�÷������ݵ�Ԫ��ʶ��
typedef struct tagASDUID
{
    unsigned char TYP;      //���ͱ�ʶ
    unsigned char VSQ:7;    //��ϢԪ�ص���Ŀ
    unsigned char S:1;      //Ѱַ��ʽ
    unsigned char COT;      //����ԭ��
    unsigned char COMADDR;  //ASDU������ַ
}ASDUID,*PASDUID;

//103Ӧ�÷������ݵ�Ԫ��Ϣ���ʶ��
typedef struct tagASDUInfID
{
    unsigned char FUN;      //��������
    unsigned char INF;      //��Ϣ���
}ASDUINFID,*PASDUINFID;

const int ASDUID_LEN = sizeof(ASDUID);
const int ASDUINFID_LEN = sizeof(ASDUINFID);

//103Э��ɱ�֡֡β
typedef struct tagVFrameEnd
{
    unsigned char CS;      //֡У��ͣ�����ӿ������𣬵���֮ǰ
    unsigned char endChar;    //�����ַ�
}VFRAMEEND,*PVFRAMEEND;	   

/***����103Э��ɱ�֡<--***/


//����103Э��̶�֡
typedef struct tagFixFrame
{
    unsigned char startChar;    //�����ַ�
    CONTOLAREA103 control;    //������
    unsigned char address;    //��·��ַ��
    unsigned char CS;      //֡У���
    unsigned char endChar;    //�����ַ�        
}FIXFRAME,*PFIXFRAME;

// }}}USR
// }}}RME

class Protocol103 : public Protocol
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR
#ifdef _MY_UNITTEST
	    friend class Protocol103_UT;
#endif

		/*************************************************
		Function:       StopRecWave
		Description:    ֹͣ¼��
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool StopRecWave( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetContData
		Description:    ����̬��¼����
		Calls:          
		Called By:      
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool GetContData( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       ReadZeroOffset
		Description:    ��ʵ��Ư�̶�
		Calls:          
		Called By:      
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/	    
		virtual bool ReadZeroOffset( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       StartGnExperiment
		Description:    ��ʼ�����ʵ��
		Calls:          
		Called By:      
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool StartGnExperiment( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       StopGnExperiment
		Description:    ֹͣ�����ʵ��
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool StopGnExperiment( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetDeviceState
		Description:    ��װ��״̬
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool GetDeviceState( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       SetDeviceState
		Description:    ����װ��״̬
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool SetDeviceState( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       OperateDevice
		Description:    Ӳ������
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool OperateDevice( Linker * pLinker, struct Command * pCmd );

		/*************************************************
		Function:       GetSplitRecWaveFile
		Description:    �ٻ���ͨ��¼���ļ�
		Calls:
		Called By:
		Input:          pLinker��ͨѶ����
		pCmd��Ҫִ�е�����

		Output:
		Return:         �ɹ�����true��ʧ�ܷ���false
		*************************************************/
		virtual bool GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	Protocol103( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol103( void );
	// }}}RME
	// {{{RME operation 'Decode(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       Decode
	    Description:    ����ͨ��ͨѶ��������ݣ�����һ���������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
	    *************************************************
	   }}}USR */
	// }}}RME
	Task * Decode( Linker * pLinker );
	// }}}RME

protected:
	// {{{RME operation 'DecodeVolatileFrame(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DecodeVolatileFrame
	    Description:    ����ɱ�֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DecodeVolatileFrame( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'DecodeFixFrame(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       DecodeFixFrame
	    Description:    ����̶�֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	    Return:         �ɹ���������ʧ�ܷ���NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	Task * DecodeFixFrame( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'doCheck(char*,int,bool,unsigned char *)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       doCheck
	    Description:    �������ݰ���У���
	    Calls:          
	    Called By:      
	    Input:          packet��������
	                    packetlen��������
	                    isFixFrame���Ƿ��ǹ̶�֡
	    Output:         result��У��ͽ��
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool doCheck( char * packet, int packetlen, bool isFixFrame, unsigned char * result );
	// }}}RME
	// {{{RME operation 'SendInterruptFrame(Linker *,struct Command *)'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       SendInterruptFrame
	    Description:    ���µĹ��ϱ���ʱ�����ж�֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����	                    
	                    pCmd�����ж��������
	    Output:         
	                    
	    Return:         true���ж�֡���ͳɹ���false��û���µĹ��ϱ�����ж�֡����ʧ��
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendInterruptFrame( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetFrame(Linker *,struct Command *,PFIXFRAME,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetFrame
	    Description:    ������������������һ��Э��֡��֡�����ͣ��̶����ɱ䣩δ֪
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����	                    
	                    uiTimeOut����ʱʱ�䣬��λ���룬0��ʾ������ʱ
	    Output:         pFixFrame�����淵�صĹ̶�֡���ݰ�
	                        vFramePacket�����淵�صĿɱ�֡���ݰ�
	                    vFrameLen���ɱ�֡���ݰ�����
	                    pOutVFrameHead��VFRAMEHEAD���ֵ�����
	                    pOutASDUID��ASDUID���ֵ�����
	                    pOutAsduData����Ϣ�岿�ֵ�����
	                    pAsduDataLen����Ϣ�峤��
	                    pOutVFrameEnd��VFRAMEEND���ֵ�����
	    Return:         -1��ʧ�ܣ�  1�����ع̶�֡�� 2��������ͨ�ɱ�֡��  3�������ж�֡
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetFrame( Linker * pLinker, struct Command * pCmd, PFIXFRAME pFixFrame, char * vFramePacket, int * vFrameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'AnalyseFixFrame(Linker *,PFIXFRAME,bool,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseFixFrame
	    Description:    ������������������һ���̶���֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    hasReadStartChar��֡�ĵ�һ�������ַ��Ƿ��Ѿ�������
	                    uiTimeOut����ʱʱ�䣬��λ���룬0��ʾ������ʱ
	    Output:         pFixFrame�����淵�صĹ̶�֡
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AnalyseFixFrame( Linker * pLinker, PFIXFRAME pFixFrame, bool hasReadStartChar, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'AnalyseVolatileFrame(Linker *,char*,int*,PVFRAMEHEAD*,PASDUID*,PASDUINFID*,char**,int*,PVFRAMEEND*,bool,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AnalyseVolatileFrame
	    Description:    ������������������һ���ɱ�֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    hasReadStartChar��֡�ĵ�һ�������ַ��Ƿ��Ѿ�������
	                    uiTimeOut����ʱʱ�䣬��λ���룬0��ʾ������ʱ
	    Output:         framePacket�����淵�صĿɱ�֡���ݰ�
	                    frameLen�����ݰ�����
	                    pOutVFrameHead��VFRAMEHEAD���ֵ�����
	                    pOutASDUID��ASDUID���ֵ�����
	                    pOutAsduData����Ϣ�岿�ֵ�����
	                    pAsduDataLen����Ϣ�峤��
	                    pOutVFrameEnd��VFRAMEEND���ֵ�����
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	bool AnalyseVolatileFrame( Linker * pLinker, char * framePacket, int * frameLen, PVFRAMEHEAD * pOutVFrameHead, PASDUID * pOutASDUID, PASDUINFID * pOutASDUInfID, char * * pOutAsduData, int * pAsduDataLen, PVFRAMEEND * pOutVFrameEnd, bool hasReadStartChar, unsigned int uiTimeOut = 0 );
	// }}}RME
	// {{{RME operation 'SendConfirm(Linker*,unsigned int,bool)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SendConfirm
	    Description:    ����ȷ��֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                        devId���豸���
	                    acd��true-������ȷ��ȷ�ϣ�false-���ͷ񶨵�ȷ��
	    Output:         
	                    
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendConfirm( Linker * pLinker, unsigned int devId, bool acd );
	// }}}RME
	// {{{RME operation 'WantConfirm(Linker*,struct Command *)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       WantConfirm
	    Description:    �ڴ�ȷ��֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	                    
	    Return:         -1���쳣��ͨ����ͨѶ�ϵ��쳣��
	                    0����÷񶨵�ȷ�ϣ�
	                    1�������ȷ��ȷ��
	    *************************************************
	   }}}USR */
	// }}}RME
	int WantConfirm( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SendLevelOne(Linker*,unsigned int)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       SendLevelOne
	    Description:    �ٻ�1������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                        devId���豸���	                    
	    Output:         
	                    
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool SendLevelOne( Linker * pLinker, unsigned int devId );
	// }}}RME
	// {{{RME operation 'WantLevelOne(Linker*)'
	// {{{RME general 'documentation'
	/* {{{USR
	        *************************************************
	    Function:       WantLevelOne
	    Description:    �ڴ��ٻ�1������֡
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	    Output:         
	                    
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool WantLevelOne( Linker * pLinker );
	// }}}RME
	// {{{RME operation 'GetPro2CmdIndex(unsigned char,unsigned char,unsigned char,unsigned short,bool)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetPro2CmdIndex
	    Description:    ��ȡ��Э����������ӳ��������
	    Calls:          
	    Called By:      
	    Input:          direct������
	                    func��������
	                    cot��ԭ��
	                    gin�����
	                    isFixFrame���Ƿ��ǹ̶�֡
	    Output:         
	                    
	    Return:         �ɹ����������ţ�ʧ�ܷ���-1
	    *************************************************	
	   }}}USR */
	// }}}RME
	int GetPro2CmdIndex( unsigned char direct, unsigned char func, unsigned char cot, unsigned short gin, bool isFixFrame );
	// }}}RME

public:
	// {{{RME operation 'GetRecWaveFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFile
	    Description:    �ٻ�¼���ļ�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ExecuteCmd(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ExecuteCmd
	    Description:    ִ������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
	// }}}RME

protected:
	// {{{RME operation 'GetPro2CmdIndex(struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetPro2CmdIndex
	    Description:    �������ȡ������Э�鹦��ӳ��������
	    Calls:          
	    Called By:      
	    Input:          cmd������                    
	                    
	    Output:                             
	    Return:         �ɹ����������ţ�ʧ�ܷ���-1
	    *************************************************	
	   }}}USR */
	// }}}RME
	int GetPro2CmdIndex( struct Command & cmd );
	// }}}RME
	// {{{RME operation 'CreateMsg(int,unsigned int,bool,unsigned char,int,char*,int,int*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateMsg
	    Description:    ������Ϣ�Ŵ�����Ϣ�ַ���
	    Calls:          
	    Called By:      
	    Input:          msgNo����Ϣ��
	                    deviceId���豸���
	                    fcbOrAcd��acd
	                    COT��ԭ��
	                    infoNo����Ϣ��
	                    ASDUData����Ϣ������
	                    ASDUDataLen����Ϣ�����ݳ���
	    Output:         msgLen����Ϣ�ַ�������
	    Return:         �ɹ�������Ϣ�ַ�����ʧ�ܷ���NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	string * CreateMsg( int msgNo, unsigned int deviceId, bool fcbOrAcd, unsigned char COT, int infoNo, char * ASDUData, int ASDUDataLen, int * msgLen );
	// }}}RME

public:
	// {{{RME operation 'GetDauInf(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetDauInf
	    Description:    ��ѯ¼��������״̬��Ϣ
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetDauInf( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetRecWaveFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    �ٻ�¼���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetRecWaveFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetReportFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetReportFile
	    Description:    ��ȡ��Ҫ¼������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetReportFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'SetTime(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       SetTime
	    Description:    ��ʱ��ʱ��ͬ����
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool SetTime( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'GetTime(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetTime
	    Description:    ��ȡʱ��
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetTime( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartRecByManual(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartRecByManual
	    Description:    �ֶ�����¼��
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartRecByManual( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'StartRecByTimer(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       StartRecByTimer
	    Description:    ��ʱ����¼��
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool StartRecByTimer( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'CreateGIMsg(int,unsigned int,unsigned char,int,GI[],unsigned char,int*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       CreateMsg
	    Description:    ����ͨ�÷�����Ϣ�ַ���
	    Calls:          
	    Called By:      
	    Input:          msgNo����Ϣ��
	                    deviceId���豸���
	                    fcbOrAcd��acd
	                    COT��ԭ��
	                    infoNo����Ϣ��
	                    GIs��GI��Ϣ                    
	    Output:         msgLen����Ϣ�ַ�������
	    Return:         �ɹ�������Ϣ�ַ�����ʧ�ܷ���NULL
	    *************************************************	
	   }}}USR */
	// }}}RME
	string * CreateGIMsg( int msgNo, unsigned int deviceId, unsigned char COT, int infoNo, GI GIs[], unsigned char GINumber, int * msgLen );
	// }}}RME

public:
	// {{{RME operation 'GetFileList(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetRecWaveFileList
	    Description:    �ٻ�¼���ļ��б�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool GetFileList( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'PullFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PullFile
	    Description:    ��ȡ�ļ�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool PullFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'PushFile(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PushFile
	    Description:    �´��ļ�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool PushFile( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'ReadRltData(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       ReadRltData
	    Description:    ��ʵʱ����
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool ReadRltData( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'AutoSend(Linker *,struct Command*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       AutoSend
	    Description:    �����ϴ�
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************	
	   }}}USR */
	// }}}RME
	virtual bool AutoSend( Linker * pLinker, struct Command * pCmd );
	// }}}RME
	// {{{RME operation 'InitConnect(Linker *)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       InitConnect
	    Description:    ��ʼ������
	    Calls:          
	    Called By:      
	    Input:          pLinker��ͨѶ����
	                    
	                    
	    Output:                             
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool InitConnect( Linker * pLinker );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol103_H */

// }}}RME
