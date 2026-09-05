#include "stdafx.h"
#include "PacUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacUtil::CPacUtil()
{

}

CPacUtil::~CPacUtil()
{

}

void CPacUtil::ReplaceChar( OUT char *pSource,IN const int iSourceSize,IN char chOld,IN char chNew )
{
	int iMax = strlen(pSource);
	iMax = min( iMax, iSourceSize );

	for ( int i=0; i<iMax; i++ )
	{
		if(pSource[i] == chOld)
		{
			pSource[i] = chNew;
		}
	}
}