/*************************************************************************
* Copyright (c) 2006,
* All rights reserved.
* 
* 文件名称：SHType.h
* 文件标识：基本类型定义
* 摘    要：
* 

**************************************************************************/

#ifndef __SH_TYPEDEF_H
#define __SH_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef PI
#define PI			3.1415926535897932384626433832795
#endif

#ifndef SQRT_2
#define SQRT_2		1.4142135623730950488016887242097
#endif

#ifndef SQRT_3
#define SQRT_3		1.7320508075688772935274463415059
#endif

#undef FAR
#undef NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD, INT32U;
typedef int                 BOOL;
typedef unsigned char       BYTE, INT8U;
typedef unsigned short      WORD, INT16U;
typedef float               FLOAT, FLOAT32;
typedef double              DOUBLE, FLOAT64;
typedef FLOAT               *PFLOAT;
typedef BYTE				BOOLEAN;           

typedef char CHAR;
typedef signed char INT8;
typedef short SHORT, INT16;

#ifndef INT32
//typedef long LONG, INT32;
typedef int INT32; 
#endif
//#define INT32 long
typedef long LONG;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

//how to define INT24 and INT128?

/* Types use for passing & returning polymorphic values */

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))

typedef DWORD   COLORREF;
typedef DWORD   *LPCOLORREF;

#pragma pack(1)
struct SHTimeStruct
{ 
	WORD	Year;		//年
	BYTE	Month;		//月
	BYTE	Day;		//日
	BYTE	Hour;		//时
	BYTE	Minute;		//分
	BYTE	Second;		//秒
	WORD	Millisecond;//毫秒0.001s
	WORD	Microsecond;//微秒0.001*0.001
};

struct SHRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
};

struct SHPOINT
{
    LONG  x;
    LONG  y;
};

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif /* __SH_TYPEDEF_H */
