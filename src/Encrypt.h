// SHTool.h: interface for the SHTool class.
//
//////////////////////////////////////////////////////////////////////
//加解密

#if !defined(AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_)
#define AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <string>

//加密
//src：要加密的字符串
//out_str:输出加密后的字符串,由调用者保证out_str内存空间至少加密字符2倍字节
bool SHEncrypt(const char *src,char * out_str);

//对字符串加密，加密输出字符串不转换成Asc码
//src：要加密的字符串
//src_len：加密字符串长度
//out_str:输出加密后的字符串,函数内分配空间，调用者使用完后负责释放内存
//输出：成功则返回加密后字符串长度，否则返回-1
int SHEncryptNoAsc(const char *src,int src_len,char ** out_str);

//用key对字符串加密
//key：加密key，不能大于16字节
//src：要加密的字符串
//out_str:输出加密后的字符串,由调用者保证out_str内存空间至少加密字符2倍字节
bool SHEncrypt(const char *key,const char *src,char * out_str);

//解密
//src：要解密的字符串
//out_str:输出解密后的字符串,由调用者保证out_str内存空间至少解密字符串1/2字节
bool SHDecrypt(const char *src,char* out_str);

//对字符串解密，解密输入输出字符串不和Asc码互转
//src：要解密的字符串
//src_len：解密字符串长度
//out_str:输出解密后的字符串,函数内分配空间，调用者使用完后负责释放内存
//输出：成功则返回解密后字符串长度，否则返回-1
int SHDecryptNoAsc(const char *src,int src_len,char ** out_str);


#endif // !defined(AFX_ENCRYPT_H__583DFFCC_5C07_4958_8609_E600EBCF1699__INCLUDED_)

