#ifndef _ioTextUtil_h_
#define _ioTextUtil_h_

namespace ioText
{
	__EX int  GetCodePage();
	__EX void SetCodePage( int iCodePage );
	__EX bool IsLeadByte( unsigned char byChar );
}

#endif // _ioTextUtil_h_
