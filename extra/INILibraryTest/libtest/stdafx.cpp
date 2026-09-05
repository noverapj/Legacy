// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// libtest.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.

namespace ioStringConverter
{

StringVector Split( const string &param, const string &delims, int iMaxSplit )
{
	StringVector ret;
	int iNumSplits = 0;

	size_t iStart, iPos;
	iStart = 0;

	do
	{
		iPos = param.find_first_of( delims, iStart );
		if( iPos == iStart )
		{
			iStart = iPos + 1;
		}
		else if( iPos == param.npos || ( iMaxSplit && iNumSplits == iMaxSplit ) )
		{
			ret.push_back( param.substr(iStart) );
			break;
		}
		else
		{
			ret.push_back( param.substr( iStart, iPos - iStart ) );
			iStart = iPos + 1;
		}

		iStart = param.find_first_not_of( delims, iStart );
		++iNumSplits;
	}while( iPos != param.npos );

	return ret;
}

void toLowerCase( std::string &str )
{
	std::transform( str.begin(), str.end(), str.begin(), tolower );
}

}