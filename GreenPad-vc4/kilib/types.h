#ifndef _KILIB_TYPES_H_
#define _KILIB_TYPES_H_
#include "stdafx.h"
//=========================================================================
//@{ @pkg ki.Types //@}
//=========================================================================

// �ϐ��̃T�C�Y�𖾎��I�Ɏw������Ƃ��Ɏg�����O
typedef unsigned char  byte;
typedef unsigned short dbyte;
typedef unsigned long  qbyte;
typedef wchar_t unicode;

// unsigned ���Ė���ł̖ʓ|
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

// �z��̗v�f��
#define countof(_array) (sizeof(_array)/sizeof(_array[0]))

// �傫�����A��������
template<typename T> inline T Min(T x,T y) { return (x<y ? x : y); }
template<typename T> inline T Max(T x,T y) { return (y<x ? x : y); }

// �Â�C++�����n�ł��Afor�Ŏg���ϐ��̃X�R�[�v�������I�ɐ���
#if defined(_MSC_VER) || defined(__DMC__)
#define for if(0);else for 
#endif

template<typename T> inline T NZero(T x) { return (x==0 ? 1 : x); }

// �R�s�[�֎~�I�u�W�F�N�g
#define NOCOPY(T) T( const T& ); T& operator=( const T& )



#endif // _KILIB_TYPES_H_