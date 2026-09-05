

#include "StdAfx.h"
#include "QuaternionCompression.h"

DWORD QComp::CompSmallThree( D3DXQUATERNION qtRot )
{
	D3DXQuaternionNormalize( &qtRot, &qtRot );

	int i=0;
	int iMaxIdx = 0;
	float fMaxAmt = fabs( qtRot.x );

	for( i=1 ; i<4 ; i++ )
	{
		float fCurAmt = fabs( qtRot[i]);

		if( fMaxAmt < fCurAmt )
		{
			iMaxIdx = i;
			fMaxAmt = fCurAmt;
		}
	}

	DWORD dwRot = ( iMaxIdx << 30 );	// 31, 30 비트에 인덱스 저장
	if( qtRot[iMaxIdx] < 0.0f )			// 가장 큰 값을 양수로 변환
	{
		qtRot = -qtRot;
	}

	int k=0;
	for( i=0 ; i<4 ; i++ )
	{
		if( i != iMaxIdx )
		{
			int iValue = (int)( ( ( qtRot[i] + 1.0f ) * FLOAT05 ) * 1023.0f );
			iValue = max( 0, min( iValue, 1023 ) );

			dwRot |= (DWORD)(iValue << ( 10 * ( 2 - k ) ) );
			k++;
		}
	}

	return dwRot;
}

D3DXQUATERNION QComp::DecompSmallThree( DWORD dwRot )
{
	DWORD dwMaxIdx = ( dwRot & 0xc0000000 ) >> 30;	// 31, 30비트

	float fVal[3];
	for( int i=0 ; i<3 ; i++ )
	{
		int iBitShift = 10 * ( 2 - i );
		DWORD dwValue = ( dwRot & ( 0x3ff << iBitShift ) ) >> iBitShift;

		fVal[i] = 2.0f * ( (float)dwValue / 1023.0f ) - 1.0f;
	}

	float fValueSqSum = fVal[0]*fVal[0] + fVal[1]*fVal[1] + fVal[2]*fVal[2];
	float fExtraVal = sqrt( 1.0f - fValueSqSum );

	DWORD j,k;
	D3DXQUATERNION qtRot;

	for( j=0, k=0 ; j<4 ; j++ )
	{
		if( j!= dwMaxIdx )
		{
			qtRot[j] = fVal[k++];
		}
		else
		{
			qtRot[j] = fExtraVal;
		}
	}

	return qtRot;
}

void QComp::Comp8Bytes( D3DXQUATERNION qtRot, DWORD& dwHigh, DWORD& dwLow )
{
	D3DXQuaternionNormalize( &qtRot, &qtRot );

	if( qtRot.w < 0.0f )
		qtRot = -qtRot;

	float fValue = 0.0f;
	DWORD dwValue = 0;

	fValue  = ( qtRot.x + 1.0f ) * FLOAT05;
	dwValue = (DWORD)(int)(fValue * 65535.0f);
	dwValue = max( 0, min( dwValue, 65535 ) );
	dwHigh  = dwValue << 16;
	
	fValue  = ( qtRot.y + 1.0f ) * FLOAT05;
	dwValue = (DWORD)(int)(fValue * 65535.0f);
	dwValue = max( 0, min( dwValue, 65535 ) );
	dwHigh  |= dwValue;

	fValue  = ( qtRot.z + 1.0f ) * FLOAT05;
	dwValue = (DWORD)(int)(fValue * 65535.0f);
	dwValue = max( 0, min( dwValue, 65535 ) );
	dwLow = dwValue << 16;

	fValue  = qtRot.w;	// 0.0f ~ 1.0f
	dwValue = (DWORD)(int)(fValue * 65535.0f);
	dwValue = max( 0, min( dwValue, 65535 ) );
	dwLow |= dwValue;
}

static float sInvMax = 1.0f / 65535.0f;

D3DXQUATERNION QComp::Decomp8Bytes( DWORD dwHigh, DWORD dwLow )
{
	D3DXQUATERNION qtRot;

	DWORD dwValue = dwHigh >> 16;
	qtRot.x = ( ( (float)dwValue * sInvMax ) * 2.0f ) - 1.0f;

	dwValue = dwHigh & 0xffff;
	qtRot.y = ( ( (float)dwValue * sInvMax ) * 2.0f ) - 1.0f;

	dwValue = dwLow >> 16;
	qtRot.z = ( ( (float)dwValue * sInvMax ) * 2.0f ) - 1.0f;

	dwValue = dwLow & 0xffff;
	qtRot.w = (float)dwValue * sInvMax;

	return qtRot;
}

