// {{{RME classifier 'Logical View::Protocol60_103'

#ifndef Protocol60_103_H
#define Protocol60_103_H

#ifdef PRAGMA
#pragma interface "Protocol60_103.h"
#endif

#include <RTSystem/Communication.h>
#include <Protocol103.h>
class FileControl;

// {{{RME tool 'OT::Cpp' property 'HeaderPreface'
// {{{USR
/*************************************************************************
* Copyright (c) 2007,
* All rights reserved.
* 
* �ļ����ƣ�Protocol60_103.h
* �ļ���ʶ��
* ժ    Ҫ������60_103Э��
* 
* ��ǰ�汾��1.0
* ��    �ߣ�libo
* ������ڣ�2007��7��4��
*
**************************************************************************/

#include "public.h"
#include "DataControl.h"
#include "FileControl.h"

// }}}USR
// }}}RME

class Protocol60_103 : public Protocol103
{
public:
	// {{{RME tool 'OT::Cpp' property 'PublicDeclarations'
	// {{{USR

	friend DWORD WINAPI DelUdpDataThread(void *pParm);

	typedef struct SHQDFlagWord
	{
		unsigned char SerialNoOfInput;
		bool    IsSimulate:1;   //���������ͣ�1��ģ������0��������
		unsigned char DeviceType:4; //�����豸���ͣ�1��ĸ�ߵ�ѹ��2��������·��3����ѹ����4���������5����Ƶ��6��ֱ��
									//7��������8����·��
		unsigned char ActType:3;    //�������ͣ�000����Ч��001��Խ���ޣ�010�� Խ���ޣ�011���仯�ʣ�100���źţ�101�����رպϣ�111�����ضϿ�
		unsigned short StartVal:5;   //��������0000����Чֵ��0001������0010������0011������0101���񵴣�0110���޹�����
									//1000��Ƶ�ʣ�1001��������1010�������ţ�1011������������1100����Խ������1101���湦��
		unsigned short Reserve:11;
	}SHQDFLAGWORD;

	typedef struct SHQDMessage
	{
		unsigned char State;        //״̬��1��������2��¼����3������
		unsigned int DeviceId;      //�豸id
		unsigned int MarkId;		//�¼����
		SHTimeStructDAU StartTime;
		SHQDFLAGWORD StartFlag;  //����ԭ��
		unsigned int ChiefDevNo;    //һ���豸���
		float CurrentVal;			//����ֵ
		unsigned int HasRecDataLen; //�Ѽ�¼���ݳ��ȣ���λ�ֽ�
		SHTimeStruct EndTime;	   //Խ�޽���ʱ��
		char FileName[100];        //�ļ������ļ���

		int GetMarkType()
		{
			return StartFlag.StartVal;
		}

		string GetMarkTypeDesc()
		{
			char szDesc[64] = {0};
			string sMarkTypeDesc("");

			if(StartFlag.IsSimulate == false)//����
			{
				switch(StartFlag.ActType)
				{
				case 5:
					return string("���رպ�");
				case 7:
					return string("���ضϿ�");
				default:
					return string("");
				}
			}
			else //ģ����
			{
				switch(StartFlag.StartVal)
				{
				case 0:
					sMarkTypeDesc.append("��Чֵ");
					break;
				case 1:
					sMarkTypeDesc.append("����");
					break;
				case 2:
					sMarkTypeDesc.append("����");
					break;
				case 3:
					sMarkTypeDesc.append("����");
					break;
				case 4:
					sMarkTypeDesc.append("����");
					break;
				case 5:
					sMarkTypeDesc.append("��");
					break;
				case 6:
					sMarkTypeDesc.append("�޹�����");
					break;
				case 7:
					sMarkTypeDesc.append("�й�����");
					break;
				case 8:
					sMarkTypeDesc.append("Ƶ��");
					break;
				case 9:
					sMarkTypeDesc.append("����");
					break;
				case 10:
					sMarkTypeDesc.append("������");
					break;
				case 11:
					sMarkTypeDesc.append("��������");
					break;
				case 12:
					sMarkTypeDesc.append("��Խ����");
					break;
				case 13:
					sMarkTypeDesc.append("�湦��");
					break;
				case 14:
					sMarkTypeDesc.append("��������");
					break;
				case 15:
					sMarkTypeDesc.append("�ܹ���");
					break;
				case 16:
					sMarkTypeDesc.append("ͻ��");
					break;
				default:
					if(StartFlag.StartVal >16 && StartFlag.StartVal <=31)
					{
						char szTmp[10]={0};
						sprintf(szTmp,"%d��г��",StartFlag.StartVal - 16);
						sMarkTypeDesc.append(szTmp);
					}
					else
					{
						sMarkTypeDesc.append("");
					}
					break;
				}//end switch(StartFlag.StartVal)

				switch(StartFlag.ActType)
				{
				case 0:
					sMarkTypeDesc.append("");
					break;
				case 1:
					sMarkTypeDesc.append("Խ����");
					break;
				case 2:
					sMarkTypeDesc.append("Խ����");
					break;
				case 3:
					sMarkTypeDesc.append("�仯��");
					break;
				case 4:
					sMarkTypeDesc.append("�ź�");
					break;
				case 5:
					sMarkTypeDesc.append("���رպ�");
					break;
				case 7:
					sMarkTypeDesc.append("���ضϿ�");
					break;
				default:
					sMarkTypeDesc.append("");
					break;
				}//end switch(StartFlag.ActType)

				return sMarkTypeDesc;
			}//end ģ����

			return string("");
		}

	}SHQDMESSAGE,*PSHQDMESSAGE;

	//����UDP��Ϣ�Ľṹ��Ϣ
	typedef struct tagUdpDataParm
	{
		Protocol60_103 *pProto;
		struct sockaddr_in *newAddrin;
		int datalen;
		char *pData;
	}UDPDATAPARM,*PUDPDATAPARM;

	/*************************************************
	Function:		GetCmdDesc
	Description:	��ȡ����������Ϣ
	Calls:
	Called By:
	Input:			cmd��Ҫִ�е�����

	Output:
	Return:
	*************************************************/
	void GetCmdDesc(struct Command & cmd);

	static string GetCmdDesc(unsigned char funCode);

	/*************************************************
	Function:		StopRecWave
	Description:	ֹͣ¼��
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool StopRecWave( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		GetContData
	Description:	����̬��¼����
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool GetContData( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		ReadZeroOffset
	Description:	��ʵ��Ư�̶�
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool ReadZeroOffset( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		StartGnExperiment
	Description:	��ʼ�����ʵ��
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool StartGnExperiment( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		StopGnExperiment
	Description:	ֹͣ�����ʵ��
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool StopGnExperiment( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		GetDeviceState
	Description:	��װ��״̬
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool GetDeviceState( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		SetDeviceState
	Description:	����װ��״̬
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:         �ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool SetDeviceState( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		OperateDevice
	Description:	Ӳ������
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	bool OperateDevice( Linker * pLinker, struct Command * pCmd );

	/*************************************************
	Function:		GetSplitRecWaveFile
	Description:	�ٻ���ͨ��¼���ļ�
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����

	Output:
	Return:			�ɹ�����true��ʧ�ܷ���false
	*************************************************/
	virtual bool GetSplitRecWaveFile( Linker * pLinker, struct Command * pCmd );

	// }}}USR
	// }}}RME

protected:
	// {{{RME tool 'OT::Cpp' property 'ProtectedDeclarations'
	// {{{USR

	map<unsigned int,MARKEVENT> m_MarkEventMap;//�����Զ��ϴ��ı���¼��б�

	/*************************************************
	Function:		SendAsdu10WithExeWrite
	Description:	����ASDU10��ִ�е�д��
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
	pCmd��			Ҫִ�е�����
					asdu10_GIs��asdu10������ʹ�õ�GI�ṹ����
					asdu10_GINumber��asdu10_GIs�������
	Output:

	Return:			-1���쳣��ͨ����ͨѶ�ϵ��쳣����Ҫ�ر�����
					0����ȡʧ�ܣ����ʹ����ָ�����û������
					1����ȡ�ɹ�
	*************************************************/
	int SendAsdu10WithExeWrite( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber);

	/*************************************************
	Function:		GetGeneraFile
	Description:	��ȡͨ���ļ���
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����
					FilePartName������·�����ļ��������ΪNULL,�������ǰ̨���ص��ļ���
					fileTotalLen:�ļ��ܳ���
					fileControlType���ļ�����
					YearMonth���ļ���ŵ�����Ŀ¼(����ΪNULL)
					asdu10_GIs��asdu10������ʹ�õ�GI�ṹ����
					asdu10_GINumber��asdu10_GIs�������
					asdu21_GIs��asdu21������ʹ�õ�GI�ṹ����
					asdu21_GINumber��asdu21_GIs�������
					IsNeedCompress:�Ƿ���Ҫѹ����0����Ҫ��1��Ҫ
	Output:

	Return:			false����ȡʧ��
					true����ȡ�ɹ�
	*************************************************/
	bool GetGeneraFile( Linker * pLinker,struct Command *pCmd,char *FilePartName,int fileTotalLen,FileControl::FILETYPE fileControlType,char *YearMonth, GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber,char IsNeedCompress=0);

	/*************************************************
	Function:		GetGeneraFile
	Description:	��ȡͨ���ļ�(����д���ڴ�)
	Calls:
	Called By:
	Input:			pLinker��ͨѶ����
					pCmd��Ҫִ�е�����
					pDataQue���������ݵĶ���
					asdu10_GIs��asdu10������ʹ�õ�GI�ṹ����
					asdu10_GINumber��asdu10_GIs�������
					asdu21_GIs��asdu21������ʹ�õ�GI�ṹ����
					asdu21_GINumber��asdu21_GIs�������
	Output:

	Return:			false����ȡʧ��
					true����ȡ�ɹ�
	*************************************************/
	bool GetGeneraFile( Linker * pLinker,struct Command *pCmd,DataControl::DataQue *pDataQue,GI asdu10_GIs[], unsigned char asdu10_GINumber,GI asdu21_GIs[], unsigned char asdu21_GINumber);

	// }}}USR
	// }}}RME

private:
	// {{{RME tool 'OT::Cpp' property 'PrivateDeclarations'
	// {{{USR

	// }}}USR
	// }}}RME

public:
	// {{{RME tool 'OT::Cpp' property 'GenerateDefaultConstructor'
	Protocol60_103( void );
	// }}}RME
	// {{{RME tool 'OT::Cpp' property 'GenerateDestructor'
	virtual ~Protocol60_103( void );
	// }}}RME
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
	bool GetDauInf( Linker * pLinker, struct Command * pCmd );
	// }}}RME

protected:
	// {{{RME operation 'PreGetFile(Linker *,struct Command*,int,char*,int,unsigned int,unsigned char)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       PreGetFile
	    Description:    ��ȡ�ļ�ǰ�ڴ�����
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    type����ȡ���ļ����ͣ�1-¼���ļ���2-һ���ļ�
	                    pFileName���ļ���
	                    startPos���ļ���ʼλ��
	                    recWaveType��¼���ļ����͡�ֻ����type=1���������Ч
	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool PreGetFile( Linker * pLinker, struct Command * pCmd, int type, char * pFileName, int fileNameLen, unsigned int startPos, unsigned char recWaveType = 0 );
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
	bool GetRecWaveFile( Linker * pLinker, struct Command * pCmd );
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
	bool GetTime( Linker * pLinker, struct Command * pCmd );
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
	// {{{RME operation 'StartRec(Linker *,struct Command*,GI[])'
	// {{{RME general 'documentation'
	/* {{{USR
	*************************************************
	    Function:       StartRec
	    Description:    ����¼��
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    GIs��GI�ṹ
	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	virtual bool StartRec( Linker * pLinker, struct Command * pCmd, GI GIs[] );
	// }}}RME
	// {{{RME operation 'GetGIAttrData(Linker *,struct Command *,GI[],unsigned char,GI[],unsigned char,char*,int*,int*,unsigned char*)'
	// {{{RME general 'documentation'
	/* {{{USR
	    *************************************************
	    Function:       GetGIAttrData
	    Description:    ���ͨ�÷�����񷵻ص����Խṹ���ݡ�
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����
	                    asdu10_GIs��asdu10������ʹ�õ�GI�ṹ����
	                    asdu10_GINumber��asdu10_GIs�������
	                    asdu21_GIs��asdu21������ʹ�õ�GI�ṹ����
	                    asdu21_GINumber��asdu21_GIs�������
	    Output:         outAttrData����������Խṹ��������
	                    outAttrLen����������Խṹ���ݳ�������
						outAttrNum��������Ը���
	                    outRII�������RII
	    Return:         -1���쳣��ͨ����ͨѶ�ϵ��쳣����Ҫ�ر�����
	                        0����ȡʧ�ܣ����ʹ����ָ�����û������
	                        1����ȡ�ɹ�
	    *************************************************
	   }}}USR */
	// }}}RME
	int GetGIAttrData( Linker * pLinker, struct Command * pCmd, GI asdu10_GIs[], unsigned char asdu10_GINumber, GI asdu21_GIs[], unsigned char asdu21_GINumber, char * outAttrData[], int * outAttrLen[], int * outAttrNum, unsigned char * outRII = NULL );
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
	bool GetFileList( Linker * pLinker, struct Command * pCmd );
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
	    Description:    ��ʵʱ���ݣ�ʵʱ��ʾ��
	    Calls:
	    Called By:
	    Input:          pLinker��ͨѶ����
	                    pCmd��Ҫִ�е�����

	    Output:
	    Return:         �ɹ�����true��ʧ�ܷ���false
	    *************************************************
	   }}}USR */
	// }}}RME
	bool ReadRltData( Linker * pLinker, struct Command * pCmd );
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
	bool AutoSend( Linker * pLinker, struct Command * pCmd );
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
	bool ExecuteCmd( Linker * pLinker, struct Command & cmd );
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
	// {{{RME operation 'GetUdpData(Linker *,struct Command&)'
	// {{{RME general 'documentation'
	/* {{{USR

	   }}}USR */
	// }}}RME
	bool GetUdpData( Linker * pLinker, struct Command & cmd );
	// }}}RME
};

// {{{RME tool 'OT::Cpp' property 'HeaderEnding'
// {{{USR

// }}}USR
// }}}RME

#endif /* Protocol60_103_H */

// }}}RME
