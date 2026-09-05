

#ifndef _QuaternionCompression_h_
#define _QuaternionCompression_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX QComp
{
public:
	static DWORD CompSmallThree( D3DXQUATERNION qtRot );
    static D3DXQUATERNION DecompSmallThree( DWORD dwRot );

	static void Comp8Bytes( D3DXQUATERNION qtRot, DWORD& dwHigh, DWORD& dwLow );
	static D3DXQUATERNION Decomp8Bytes( DWORD dwHigh, DWORD dwLow );
};

#endif