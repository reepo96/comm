#if !defined (PQDEFXXXXXX_H)
#define PQDEFXXXXXX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dfrdef.h"
/*******************************�������********************************/

#pragma pack(1)
//���������Զ����� 
/*
���� 1  ͨ����·�����յ���һ֡�Զ��������ݣ� �������ݲ�����   
���ɸ���������� ������غ������� ��ȡʣ���������� �����д�빲���ڴ�
*/
const   unsigned char   CMD_PQAUTOSEND = 0X63;


const   unsigned char   CMD_PQ_AUTORTDATA = 0X64;   //��������ʵʱ����

const   unsigned char   CMD_PQ_AUTOTRENDDATA = 0X65; //����������������

const   unsigned char   CMD_PQ_AUTOFILEINF = 0X66;	 //�������������ļ������Ϣ

const   unsigned char   CMD_PQ_AUTOEVENT = 0X67;	 //���������¼���Ϣ

//��ȡ����������̬¼���ļ�
/*
���� 1  ��ʼ�¼��ļ��� 40�ֽ�
���� 2  �����¼��ļ��� 40�ֽ�	
���� 3  ��ʼλ��       4�ֽ�
*/
const   unsigned char   CMD_GETPQWAVEFILE = 0X68;

//��ȡ���������¼���Ϣ
/*
���� 1  ���������¼��� 4�ֽ�
*/
const   unsigned char   CMD_GETPQEVENT = 0X69;


//��ȡ�������������ļ�
/*
...
*/
const   unsigned char   CMD_GETPQTRHFILE = 0X6A;

//��ȡϵͳ������־
/*
���� 1  ��ʼʱ��        CP56Time2a
���� 2  ����ʱ��		CP56Time2a
*/
const   unsigned char   CMD_GETRUNNINGLOG = 0X6B;

//�����������ܼ���
/*
*/
const   unsigned char   CMD_PQTEST = 0X6C;

const   unsigned char   CMD_PQRESUMSET = 0X70;		//�ָ���������

const   unsigned char   CMD_PQ_AUTOT10CYCDATA = 0X72; //��������10�ܲ���������

const   unsigned char   CMD_PQ_10CYCDATA_EXP = 0X73; //��������10�ܲ��������ݵ���

const   unsigned char   CMD_PQ_GET_10CYCDATA_FILE = 0X74; //ȡ��������10�ܲ����������ļ�

const   unsigned char   CMD_PQ_AUTOTSTATINF = 0X76; //��������ͳ�������Ϣ
const   unsigned char   CMD_PQ_GETSTAT_FILE = 0X77; //��ȡͳ���ļ�

//��˫���Զ����ļ�Ŀ¼�б�
/*
���� 1  Ŀ¼����        2�ֽ�
���� 2  ��ʼʱ��        CP56Time2a
���� 3  ����ʱ��		CP56Time2a
*/
//const   unsigned char   CMD_GETSHFILELIST = 0X6C;

//��˫���Զ��������ļ�

//const   unsigned char   CMD_GETSHFILE = 0X6D;
/******************************�ṹ����***********************************/
//�¼��ṹ
struct EventInf
{
unsigned int id;

        // �¼����࣬1����̬�¼���2����̬�¼�
        //##ModelId=4C43A50F02A2
        short category;

        /*�¼�����:
			1 Ƶ��Խ���ޡ�2 Ƶ��Խ���ޡ�3 ��ѹԽ���ޡ�4 ��ѹԽ���ޡ�5 ���಻ƽ���Խ�ޡ�
			6�ܻ�����Խ�ޡ�7 г��Խ�ޡ�8 ��ʱ����Խ�ޣ�9 ��ʱ����Խ��  10 ��ѹ����Խ��
			������̬�У�11 ��ѹ�ݽ���12��ѹ������13��ѹ��ʱ�ж�
		*/
        //##ModelId=4C43A53B02DD
        short type;

        // ״̬��1���¼���ʼ��2���¼�����
        //##ModelId=4C43A5810302
        short state;

        // �����¼�ͨ����/��·��
        //##ModelId=4C43A5BC038E
        short channel;

		int  chTyp; //ͨ����/��·������ 1 ��ѹ�� 2 ���� add by kh

        //##ModelId=4C43A681012B
        struct SHTimeStructDAU starttime;

        //##ModelId=4C43A6AA020A
        struct SHTimeStructDAU endtime;

        // ����ʱ��
        //##ModelId=4C43A6E301C1
        double duration;

        // �¼�����
        //##ModelId=4C43A84E013E
        char describe[64];

        // ��ʼ�¼���Ӧ���ļ���
        //##ModelId=4C43A92303E5
        char startfile[64];

        // �����¼���Ӧ���ļ���
        //##ModelId=4C43A94C03D6
        char endfile[64];
};

struct PQTrendFileInf
{
	unsigned short filetype;    //�����ļ������� 
	char fileName[100];			//�ļ�����
	int  fileSize;				//�ļ���С
	struct CP56Time2a ctime;    //�ļ�ʱ��
	struct CP56Time2a etime;    //�ļ�ʱ��
};

struct CommCmdWaveFileInf
{
	char	fileName[40];	//������չ�����ļ���
	unsigned int 	faultTime;		//����ʱ��
	unsigned int nFileLen;	//�ļ�����
};

//˫�ϱ��ͷ
struct  SHAuotSendHead
{
	char shflag[2];
	unsigned char typ;
	unsigned short packNo;
};

enum LOG_TYPE { RUN = 1, DEBUG = 2};
enum LOG_LEVEL { LNONE = 0, LERROR = 1, LWARNING = 2, LINFO = 3, LDEBUG = 4 , LFONLY = 5/* ֻд�ļ�����ʾ*/};
#define  LOGSTRLEN   128		//������־��Ϣ����

typedef struct RunLogInf
{
	LOG_TYPE   type;	//��־����
	LOG_LEVEL  level;	//�ȼ�
	time_t   time;		//��־��¼ʱ��
	unsigned int length;//����
	char    LogStr[LOGSTRLEN];//��־��Ϣ
}RunLogInf;

typedef struct
{
	unsigned int  FrmType;		//֡���ͱ�ʶ��ʵʱ����֡Ϊ0x33333331
	unsigned int  FrmLen;		//֡���ȣ�������֡ͷ��Ч���
	unsigned int  FrmIdx;		//֡��ţ���1��ʼ��ÿ��1֡��1
	unsigned short  AnChCnt;		//ģ��ͨ������
	unsigned short  BinChCnt;	//������ͨ��������
	unsigned short  ULinCnt;		//��ѹ��·����
	unsigned short  ILinCnt;		//������·����	
	unsigned short  SmpRate;		//�����ʣ���ÿ�ܲ�����
	unsigned short  SizePerSample; //ÿ���������С
	unsigned short  PointNum;	//������11.25�ܲ���
	unsigned short  Rsev[1];		//Ԥ��
}FRM_HEAD_RT_SEND;



typedef struct tagContSearchParm
{
	ST_UINT8 Type;	//���ҵ���������:1 �������� 2 ��������
	ST_INT32 StartTime;	//��ʼʱ��
	ST_INT32 Interval;  //�����,����ͼʱ��
	ST_INT32 LastTime; //���ݳ���(��),����ͼʱΪ����
	ST_CHAR  FileName[64];//���ҵ��ļ���(ͨ������¼�����ʱ��)
	ST_UINT8 AnChFlag[12]; //Ҫ��ȡ��ģ��ͨ����־��ͨ����Ӧbit��1��ȡ������ȡ
	ST_UINT8 BinChFlag[24]; //Ҫ��ȡ�Ŀ���ͨ����־��ͨ����Ӧbit��1��ȡ������ȡ
}CONTSEARCHPARM;

typedef struct sys_net_cfg
{   
	unsigned char nic_num; 
	unsigned char ipaddr[4];
	unsigned char netmask[4];
	unsigned char gateway[4];
	int port;
	unsigned char ipaddr2[4];
	unsigned char netmask2[4];
	unsigned char gateway2[4];
	int port2;
}SYS_NetCfg;

#pragma pack()
#endif 