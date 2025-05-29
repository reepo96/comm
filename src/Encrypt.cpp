
#include <string.h>
#include "Encrypt.h"
#include "sm4.h"

const char *ENCRYPT_KEY = "sh_PQ_key_93";

bool SHEncrypt(const char *src,char * out_str)
{
	if( src == NULL || out_str==NULL)
		return false;

	int iStrLen = strlen(src);
	if(iStrLen == 0)
		return false;

	iStrLen = 16*((iStrLen+15)/16);

	unsigned char auth_key[16] ={0};
	unsigned char *outputdata = new unsigned char[iStrLen];
	unsigned char *inputdata = new unsigned char[iStrLen];

	memset(outputdata,0,iStrLen);
	memset(inputdata,0,iStrLen);
	
	strcpy((char*)auth_key,ENCRYPT_KEY);
	sm4_context context;
	sm4_setkey_enc(&context,auth_key);
	
	for(int i=0;i<strlen(src);i++)
	{
		inputdata[i] = src[i];
	}
	
	sm4_crypt_ecb(&context,SM4_ENCRYPT,iStrLen,inputdata,outputdata);
	for(int i=0;i<iStrLen;i++)
	{
		sprintf(&out_str[i*2],"%02X",outputdata[i]);
	}

	delete []outputdata;
	delete []inputdata;
	return true;
}

int SHEncryptNoAsc(const char *src,int src_len,char ** out_str)
{
	if( src == NULL || out_str==NULL)
		return -1;

	if(src_len <= 0)
		return -1;

	int iStrLen = 16*((src_len+15)/16);

	unsigned char auth_key[16] ={0};
	*out_str = new char[iStrLen];
	unsigned char *inputdata = new unsigned char[iStrLen];

	memset(*out_str,0,iStrLen);
	memset(inputdata,0,iStrLen);
	
	strcpy((char*)auth_key,ENCRYPT_KEY);
	sm4_context context;
	sm4_setkey_enc(&context,auth_key);
	
	memcpy(inputdata,src,src_len);
	
	sm4_crypt_ecb(&context,SM4_ENCRYPT,iStrLen,inputdata,(unsigned char *)(*out_str));
	
	delete []inputdata;
	return iStrLen;
}

bool SHEncrypt(const char *key,const char *src,char * out_str)
{
	if(key==NULL || src == NULL || out_str==NULL)
		return false;

	int iStrLen = strlen(src);
	if(iStrLen == 0)
		return false;

	iStrLen = 16*((iStrLen+15)/16);

	unsigned char auth_key[16] ={0};
	unsigned char *outputdata = new unsigned char[iStrLen];
	unsigned char *inputdata = new unsigned char[iStrLen];

	memset(outputdata,0,iStrLen);
	memset(inputdata,0,iStrLen);
	
	strncpy((char*)auth_key,key,sizeof(auth_key));
	sm4_context context;
	sm4_setkey_enc(&context,auth_key);
	
	for(int i=0;i<strlen(src);i++)
	{
		inputdata[i] = src[i];
	}
	
	sm4_crypt_ecb(&context,SM4_ENCRYPT,iStrLen,inputdata,outputdata);
	for(int i=0;i<iStrLen;i++)
	{
		sprintf(&out_str[i*2],"%02X",outputdata[i]);
	}

	delete []outputdata;
	delete []inputdata;
	return true;
}

bool SHDecrypt(const char *src,char* out_str)
{
	if( src == NULL || out_str==NULL)
		return false;

	int iStrLen = strlen(src);
	if(iStrLen == 0)
		return false;
		
	unsigned char auth_key[16] ={0};
	unsigned char *outputdata = new unsigned char[iStrLen/2+8];
	unsigned char *inputdata = new unsigned char[iStrLen/2+8];
	char szShortVal[3] = {0};

	memset(outputdata,0,iStrLen/2+8);
	memset(inputdata,0,iStrLen/2+8);

	strcpy((char*)auth_key,ENCRYPT_KEY);
	sm4_context context;
	sm4_setkey_dec(&context,auth_key);
	
	for(int i=0;i<iStrLen/2 ;i++)
	{
		strncpy(szShortVal,(const char*)&src[i*2],2);
		sscanf(szShortVal,"%02X",&inputdata[i]);
	}
	
	sm4_crypt_ecb(&context,SM4_DECRYPT,iStrLen/2,inputdata,outputdata);
	sprintf(out_str,"%s",outputdata);

	delete []outputdata;
	delete []inputdata;
}

int SHDecryptNoAsc(const char *src,int src_len,char ** out_str)
{
	if( src == NULL || out_str==NULL || src_len<1)
		return -1;

	int iStrLen = 16*((src_len+15)/16);

	unsigned char auth_key[16] ={0};
	*out_str = new char[iStrLen+8];
	unsigned char *inputdata = new unsigned char[iStrLen+8];

	memset(*out_str,0,iStrLen+8);
	memset(inputdata,0,iStrLen+8);

	strcpy((char*)auth_key,ENCRYPT_KEY);
	sm4_context context;
	sm4_setkey_dec(&context,auth_key);
	
	memcpy(inputdata,src,src_len);
	
	sm4_crypt_ecb(&context,SM4_DECRYPT,iStrLen,inputdata,(unsigned char *)(*out_str));

	delete []inputdata;
	return iStrLen;
}
