

#include "stdafx.h"

#include "ioMath.h"
#include "ioOrientBox.h"
#include "IntersectionUtility.h"

bool Clip( float fDenom, float fNumer, float &fT0, float &fT1 )
{
	if( fDenom > 0.0f )
	{
		if( fNumer > fDenom*fT1 )
			return false;
		if( fNumer > fDenom*fT0 )
			fT0 = fNumer / fDenom;

		return true;
	}
	else if( fDenom < 0.0f )
	{
		if( fNumer > fDenom * fT0 )
			return false;
		if( fNumer > fDenom * fT1 )
			fT1 = fNumer / fDenom;

		return true;
	}

	return (fNumer <= 0.0f);
}