

#include "stdafx.h"

#include "ioStringConverter.h"

using namespace std;

char ioStringConverter::m_ConvertBuf[MAX_PATH];

ioHashString ioStringConverter::toString( float val )
{
	sprintf_s( m_ConvertBuf, "%.2f", val );

	return ioHashString(m_ConvertBuf);
}

ioHashString ioStringConverter::toString( int val )
{
	wsprintf( m_ConvertBuf, "%d", val );

	return ioHashString(m_ConvertBuf);
}

ioHashString ioStringConverter::toString( unsigned int val )
{
	wsprintf( m_ConvertBuf, "%u", val );

	return ioHashString(m_ConvertBuf);
}

ioHashString ioStringConverter::toString( long val )
{
	wsprintf( m_ConvertBuf, "%d", val );

	return ioHashString(m_ConvertBuf);
}

ioHashString ioStringConverter::toString( unsigned long val )
{
	wsprintf( m_ConvertBuf, "%u", val );

	return ioHashString(m_ConvertBuf);
}

ioHashString ioStringConverter::toString( bool val, bool bYesNo )
{
	if( !bYesNo )
	{
		if( val )
			return ioHashString( "true" );
		else
			return ioHashString( "false" );
	}
	else
	{
		if( val )
			return ioHashString( "yes" );
		else
			return ioHashString( "no" );
	}
}


float ioStringConverter::ParseFloat( const char *szVal )
{
	return (float)atof( szVal );
}

int ioStringConverter::ParseInt( const char *szVal )
{
	return atoi( szVal );
}

unsigned int ioStringConverter::ParseUnsignedInt( const char *szVal )
{
	return atoi( szVal );
}

long ioStringConverter::ParseLong( const char *szVal )
{
	return atol( szVal );
}

unsigned long ioStringConverter::ParseUnsignedLong( const char *szVal )
{
	return atol( szVal );
}

bool ioStringConverter::ParseBool( const char *szVal )
{
	if( !strcmp( szVal, "true" ) )
		return true;

	return false;
}

StringVector ioStringConverter::Split( const string &param, const string &delims, int iMaxSplit )
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

void ioStringConverter::toLowerCase( std::string &str )
{
	std::transform( str.begin(), str.end(), str.begin(), tolower );
}

void ioStringConverter::toUpperCase( std::string &str )
{
	std::transform( str.begin(), str.end(), str.begin(), toupper );
}
