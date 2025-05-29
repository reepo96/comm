// SHTool.h: interface for the SHTool class.
//
//////////////////////////////////////////////////////////////////////
//�ӽ���

#if !defined(AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_)
#define AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string>

//����
//src��Ҫ���ܵ��ַ���
//out_str:������ܺ���ַ���,�ɵ����߱�֤out_str�ڴ�ռ����ټ����ַ�2���ֽ�
bool SHEncrypt(const char *src,char * out_str);

//���ַ������ܣ���������ַ�����ת����Asc��
//src��Ҫ���ܵ��ַ���
//src_len�������ַ�������
//out_str:������ܺ���ַ���,�����ڷ���ռ䣬������ʹ��������ͷ��ڴ�
//������ɹ��򷵻ؼ��ܺ��ַ������ȣ����򷵻�-1
int SHEncryptNoAsc(const char *src,int src_len,char ** out_str);

//��key���ַ�������
//key������key�����ܴ���16�ֽ�
//src��Ҫ���ܵ��ַ���
//out_str:������ܺ���ַ���,�ɵ����߱�֤out_str�ڴ�ռ����ټ����ַ�2���ֽ�
bool SHEncrypt(const char *key,const char *src,char * out_str);

//����
//src��Ҫ���ܵ��ַ���
//out_str:������ܺ���ַ���,�ɵ����߱�֤out_str�ڴ�ռ����ٽ����ַ���1/2�ֽ�
bool SHDecrypt(const char *src,char* out_str);

//���ַ������ܣ�������������ַ�������Asc�뻥ת
//src��Ҫ���ܵ��ַ���
//src_len�������ַ�������
//out_str:������ܺ���ַ���,�����ڷ���ռ䣬������ʹ��������ͷ��ڴ�
//������ɹ��򷵻ؽ��ܺ��ַ������ȣ����򷵻�-1
int SHDecryptNoAsc(const char *src,int src_len,char ** out_str);


#endif // !defined(AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_)

