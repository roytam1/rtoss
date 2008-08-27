#ifndef	VCOUT_H
#define	VCOUT_H


#include	<iostream>
using namespace std;

#include	<windows.h>
//#include	<mbctype.h>		// for _ismbblead()
#include	"Utilities.h"	// for _ismbblead()


class vc_debugstring_buf : public streambuf {
	char	utf8[4];	// UTF-8文字の保持
	int utflen,utfpos;
public:
	vc_debugstring_buf() : knj(0) {}
	virtual int overflow(int c=EOF) {
		if (utflen == 1 && utfpos) {
			// UTF-8文字の最後
			utf8[++utfpos] = c;
			::OutputDebugString(utf8);
			utflen = utfpos = 0;
		}
		else if ( _ismbblead(c) ) {
			utflen = _mbbc(c) - 1;
			utfpos = 0;
			utf8[utfpos] = c;
		}
		else if ( _ismbbtail(c) ) {
			--utflen;
			utf8[++utfpos] = c;
		}
		else {
			utf8[0] = (char)c;
			utf8[1] = '\0';
			::OutputDebugString(utf8);
		}
		return	c;
	}
};

class vc_debugstring_stream : public ostream {
	vc_debugstring_buf	buf;
public:
	vc_debugstring_stream() : ostream(&buf) {}
};

//extern	vc_debugstring_stream	vcout;
static	vc_debugstring_stream	vcout;

#endif	// VCOUT_H
