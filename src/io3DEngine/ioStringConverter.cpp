

#include "stdafx.h"

#include "ioStringConverter.h"

using namespace std;

char ioStringConverter::m_ConvertBuf[MAX_PATH];
std::vector< float > ioStringConverter::m_vTempFloatList(100);
StringVector ioStringConverter::m_vSplitResults;

std::string ioStringConverter::toString( float val )
{
	sprintf( m_ConvertBuf, "%.2f", val );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( int val )
{
	wsprintf( m_ConvertBuf, "%d", val );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( unsigned int val )
{
	wsprintf( m_ConvertBuf, "%u", val );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( long val )
{
	wsprintf( m_ConvertBuf, "%d", val );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( unsigned long val )
{
	wsprintf( m_ConvertBuf, "%u", val );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( bool val, bool bYesNo )
{
	if( !bYesNo )
	{
		if( val )
			return std::string( "true" );
		else
			return std::string( "false" );
	}
	else
	{
		if( val )
			return std::string( "yes" );
		else
			return std::string( "no" );
	}
}

std::string ioStringConverter::toString( const D3DXVECTOR2 &val )
{
	sprintf( m_ConvertBuf, "%.2f %.2f", val.x, val.y );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const Vector2Vec &val )
{
	ZeroMemory( m_ConvertBuf, MAX_PATH );

	char szTemp[MAX_PATH];

	int iCount = val.size();
	for( int i=0 ; i<iCount ; i++ )
	{
		sprintf( szTemp, "%.2f %.2f", val[i].x, val[i].y );

		if( i > 0 )
		{
			strcat( m_ConvertBuf, " " );
		}

		strcat( m_ConvertBuf, szTemp );
	}

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const D3DXVECTOR3 &val )
{
	sprintf( m_ConvertBuf, "%.2f %.2f %.2f", val.x, val.y, val.z );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const Vector3Vec &val )
{
	char szBuf[512];
	ZeroMemory( szBuf, 512 );

	char szTemp[MAX_PATH];

	int iCount = val.size();
	for( int i=0 ; i<iCount ; i++ )
	{
		sprintf( szTemp, "%.2f %.2f %.2f", val[i].x, val[i].y, val[i].z );

		if( i > 0 )
		{
			strcat( szBuf, " " );
		}

		strcat( szBuf, szTemp );
	}

	return std::string(szBuf);
}

std::string ioStringConverter::toString( const D3DXQUATERNION &val )
{
	sprintf( m_ConvertBuf, "%.2f %.2f %.2f %.2f", val.x, val.y, val.z, val.w );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const D3DXVECTOR4 &val )
{
	sprintf( m_ConvertBuf, "%.2f %.2f %.2f %.2f", val.x, val.y, val.z, val.w );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const D3DCOLORVALUE &val )
{
	sprintf( m_ConvertBuf, "%.2f %.2f %.2f %.2f", val.r, val.g, val.b, val.a );

	return std::string(m_ConvertBuf);
}

std::string ioStringConverter::toString( const vector<D3DXVECTOR4> &val )
{
	char szBuf[512];
	ZeroMemory( szBuf, 512 );

	char szTemp[MAX_PATH];

	int iCount = val.size();
	for( int i=0 ; i<iCount ; i++ )
	{
		sprintf( szTemp, "%.2f %.2f %.2f %.2f", val[i].x, val[i].y, val[i].z, val[i].w );

		if( i > 0 )
		{
			strcat( szBuf, " " );
		}

		strcat( szBuf, szTemp );
	}

	return std::string(szBuf);
}

const char* ioStringConverter::toString( ErasePolicy ePolicy )
{
	switch( ePolicy )
	{
	case EP_AUTOMATIC:
		return "Automatic";
	case EP_MANUAL:
		return "Manual";
	case EP_NO_ERASE:
		return "NoErase";
	}

	return "Automatic";
}

float ioStringConverter::ParseFloat( const char *szVal )
{
	return atof( szVal );
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

D3DXVECTOR2 ioStringConverter::ParseVector2( const char *szVal )
{
	D3DXVECTOR2 vVec;

	sscanf( szVal, "%f %f", &vVec.x, &vVec.y );

	return vVec;
}

void ioStringConverter::ParseVector2Vec( const char *szVal, Vector2Vec &vList )
{
	char szBuf[512];
	strcpy( szBuf, szVal );

	char *pCur;
	pCur = strtok( szBuf, " " );
	if( !pCur )	return;

	m_vTempFloatList.erase( m_vTempFloatList.begin(), m_vTempFloatList.end() );
	m_vTempFloatList.push_back( atof( pCur ) );

	while( pCur )
	{
		pCur = strtok( NULL, " " );
		if( pCur )
		{
			m_vTempFloatList.push_back( atof(pCur) );
		}
	}

	if( m_vTempFloatList.size() % 2 == 1 )
	{
		m_vTempFloatList.push_back( 0.0f );
	}

	vList.erase( vList.begin(), vList.end() );

	D3DXVECTOR2 vVec2;
	vector<float>::iterator iter;
	for( iter=m_vTempFloatList.begin(); iter!=m_vTempFloatList.end() ; )
	{
		vVec2.x = *iter;
		++iter;
		vVec2.y = *iter;

		vList.push_back( vVec2 );
		++iter;
	}
}

D3DXVECTOR3 ioStringConverter::ParseVector3( const char *szVal )
{
	D3DXVECTOR3 vVec;

	sscanf( szVal, "%f %f %f", &vVec.x, &vVec.y, &vVec.z );

	return vVec;
}

void ioStringConverter::ParseVector3Vec( const char *szVal, Vector3Vec &vList )
{
	char szBuf[512];
	strcpy( szBuf, szVal );

	char *pCur;
	pCur = strtok( szBuf, " " );
	if( !pCur )	return;

	m_vTempFloatList.erase( m_vTempFloatList.begin(), m_vTempFloatList.end() );
	m_vTempFloatList.push_back( atof( pCur ) );

	while( pCur )
	{
		pCur = strtok( NULL, " " );
		if( pCur )
		{
			m_vTempFloatList.push_back( atof(pCur) );
		}
	}

	int iModCnt = m_vTempFloatList.size() % 3;
	if( iModCnt > 0 )
	{
		iModCnt = 3 - iModCnt;

		for( int i=0 ; i<iModCnt ; i++ )
		{
			m_vTempFloatList.push_back( 0.0f );
		}
	}

	vList.erase( vList.begin(), vList.end() );

	D3DXVECTOR3 vVec3;
	vector<float>::iterator iter;
	for( iter = m_vTempFloatList.begin(); iter!=m_vTempFloatList.end() ; )
	{
		vVec3.x = *iter;
		++iter;
		vVec3.y = *iter;
		++iter;
		vVec3.z = *iter;
		++iter;

		vList.push_back( vVec3 );
	}
}

D3DXQUATERNION ioStringConverter::ParseQuaternion( const char *szVal )
{
	D3DXQUATERNION quater;
	sscanf( szVal, "%f %f %f %f", &quater.x, &quater.y, &quater.z, &quater.w );

	return quater;
}

D3DXVECTOR4 ioStringConverter::ParseVector4( const char *szVal )
{
	D3DXVECTOR4 vVec;
	sscanf( szVal, "%f %f %f %f", &vVec.x, &vVec.y, &vVec.z, &vVec.w );

	return vVec;
}

D3DCOLORVALUE ioStringConverter::ParseColorValue( const char *szVal )
{
	D3DCOLORVALUE vColor;
	sscanf( szVal, "%f %f %f %f", &vColor.r, &vColor.g, &vColor.b, &vColor.a );

	return vColor;
}

void ioStringConverter::ParseVector4Vec( const char *szVal,
										 vector<D3DXVECTOR4> &vList )
{
	char szBuf[512];
	strcpy( szBuf, szVal );

	char *pCur;
	pCur = strtok( szBuf, " " );
	if( !pCur )	return;

	m_vTempFloatList.erase( m_vTempFloatList.begin(), m_vTempFloatList.end() );
	m_vTempFloatList.push_back( atof( pCur ) );

	while( pCur )
	{
		pCur = strtok( NULL, " " );
		if( pCur )
		{
			m_vTempFloatList.push_back( atof(pCur) );
		}
	}

	int iModCnt = m_vTempFloatList.size() % 4;
	if( iModCnt > 0 )
	{
		iModCnt = 4 - iModCnt;
		for( int i=0 ; i<iModCnt ; i++ )
		{
			m_vTempFloatList.push_back( 0.0f );
		}
	}

	vList.erase( vList.begin(), vList.end() );

	D3DXVECTOR4 vVec4;
	vector<float>::iterator iter;
	for( iter = m_vTempFloatList.begin() ; iter!=m_vTempFloatList.end() ; )
	{
		vVec4.x = *iter;
		++iter;
		vVec4.y = *iter;
		++iter;
		vVec4.z = *iter;
		++iter;
		vVec4.w = *iter;
		++iter;

		vList.push_back( vVec4 );
	}
}

TextAlignType ioStringConverter::ParseTextAlignType( const ioHashString &szType )
{
	TextAlignType eAlign = TAT_LEFT;
	if( szType == "Left" )
		eAlign = TAT_LEFT;
	else if( szType == "Center" )
		eAlign = TAT_CENTER;
	else if( szType == "Right" )
		eAlign = TAT_RIGHT;

	return eAlign;
}

ioHashString ioStringConverter::ParseTextAlignType( TextAlignType type )
{
	switch ( type )
	{
	case TAT_LEFT: return "Left";
	case TAT_CENTER: return "Center";
	case TAT_RIGHT: return "Right";
	}
	return "Left";
}

TextVertAlign ioStringConverter::ParseTextVertAlign( const ioHashString &szType )
{
	TextVertAlign eAlign = TVA_TOP;
	if( szType == "Top" )
		eAlign = TVA_TOP;
	else if( szType == "Center" )
		eAlign = TVA_CENTER;
	else if( szType == "Bottom" )
		eAlign = TVA_BOTTOM;

	return eAlign;
}

ioHashString ioStringConverter::ParseTextVertAlign( TextVertAlign type )
{
	switch ( type )
	{
	case TVA_TOP: return "Top";
	case TVA_CENTER: return "Center";
	case TVA_BOTTOM: return "Bottom";
	}
	return "Top";
}

TextStyle ioStringConverter::ParseTextStyle( const ioHashString &szStyle )
{
	TextStyle eStyle = TS_NORMAL;
	if( szStyle == "Normal" )
		eStyle = TS_NORMAL;
	else if( szStyle == "Shadow" )
		eStyle = TS_SHADOW;
	else if( szStyle == "Outline" )
		eStyle = TS_OUTLINE;
	else if( szStyle == "Outline_2x" )
		eStyle = TS_OUTLINE_2X;
	else if( szStyle == "Outline_full" )
		eStyle = TS_OUTLINE_FULL;
	else if( szStyle == "Outline_full_2x" )
		eStyle = TS_OUTLINE_FULL_2X;
	else if( szStyle == "Outline_full_3x" )
		eStyle = TS_OUTLINE_FULL_3X;
	else if( szStyle == "Outline_full_4x" )
		eStyle = TS_OUTLINE_FULL_4X;
	else if( szStyle == "Bold" )
		eStyle = TS_BOLD;
	else if( szStyle == "Bold_shadow" )
		eStyle = TS_BOLD_SHADOW;
	else if( szStyle == "Bold_outline" )
		eStyle = TS_BOLD_OUTLINE;
	else if( szStyle == "Bold_outline_full" )
		eStyle = TS_BOLD_OUTLINE_FULL;
	else if( szStyle == "Bold_outline_full_2x" )
		eStyle = TS_BOLD_OUTLINE_FULL_2X;
	else if( szStyle == "Bold_outline_full_3x" )
		eStyle = TS_BOLD_OUTLINE_FULL_3X;
	else if( szStyle == "Bold_outline_full_4x" )
		eStyle = TS_BOLD_OUTLINE_FULL_4X;

	return eStyle;
}

UIRenderType ioStringConverter::ParseRenderType( const ioHashString &szType )
{
	if( szType == "Normal" )
		return UI_RENDER_NORMAL;
	else if( szType == "Screen" )
		return UI_RENDER_SCREEN;
	else if( szType == "Gray" )
		return UI_RENDER_GRAY;
	else if( szType == "Add" )
		return UI_RENDER_ADD;
	else if( szType == "AddColor" )
		return UI_RENDER_NORMAL_ADD_COLOR;
	else if( szType == "ColorAlpha" )
		return UI_RENDER_COLOR_ALPHA;
	else if( szType == "Lighten" )
		return UI_RENDER_LIGHTEN;
	else if( szType == "Darken" )
		return UI_RENDER_DARKEN;
	else if( szType == "Multiply" )
		return UI_RENDER_MULTIPLY;
	else if( szType == "GrayLight" )
		return UI_RENDER_GRAY_LIGHT;
	else if( szType == "AddAlpha" )
		return UI_RENDER_ADD_ALPHA;
	return UI_RENDER_NORMAL;
}

ErasePolicy ioStringConverter::ParseErasePolicy( const ioHashString &szType )
{
	ErasePolicy ePolicy = EP_AUTOMATIC;
	if( szType == "Automatic" )
		ePolicy = EP_AUTOMATIC;
	else if( szType == "Manual" )
		ePolicy = EP_MANUAL;
	else if( szType == "NoErase" )
		ePolicy = EP_NO_ERASE;

	return ePolicy;
}

FXRenderType ioStringConverter::ParseFXRenderType( const ioHashString &szType )
{
	FXRenderType eType = FRT_ADD;
	if( szType == "add" )
	{
		eType = FRT_ADD;
	}
	else if( szType == "src_alpha" )
	{
		eType = FRT_SRCALPHA;
	}
	else if( szType == "screen" )
	{
		eType = FRT_SCREEN;
	}
	else if( szType == "lighten" )
	{
		eType = FRT_LIGHTEN;
	}
	else if( szType == "darken" )
	{
		eType = FRT_DARKEN;
	}
	else if( szType == "rev_subtract" )
	{
		eType = FRT_REV_SUBTRACT;
	}
	else if( szType == "subtract" )
	{
		eType = FRT_SUBTRACT;
	}

	return eType;
}

TextureFilterOption ioStringConverter::ParseTextureFilterOption( const ioHashString &szType )
{
	if( szType == "none" )
		return TFO_NEAREST;
	else if( szType == "bilinear" )
		return TFO_BILINEAR;
	else if( szType == "trilinear" )
		return TFO_TRILINEAR;
	else if( szType == "anisotropic" )
		return TFO_ANISOTROPIC;

	return TFO_BILINEAR;
}

DWORD ioStringConverter::ParseColor( const ioHashString &szColor )
{
	DWORD dwColor = 0xff000000;
	sscanf( szColor.c_str(), "%8X", &dwColor );
	
	if( dwColor == 0 )
		dwColor = 0xffffffff;

	return dwColor;
}

ioHashString ioStringConverter::ParseColor( DWORD color )
{	
	char buff[ 64 ];
	std::string ret;	
	
	sprintf( buff, "%02x", ( ( color & 0xFF000000 ) >> 24 ) );
	ret += buff;
	sprintf( buff, "%02x", ( ( color & 0x00FF0000 ) >> 16 ) );
	ret += buff;
	sprintf( buff, "%02x", ( ( color & 0x0000FF00 ) >> 8 ) );
	ret += buff;
	sprintf( buff, "%02x", ( color & 0x000000FF ) );
	ret += buff;

	return ret.c_str();
}

std::string ioStringConverter::SplitFirstOnly( const std::string &param,
											   const std::string &delims )
{
	size_t iPos = param.find_first_of( delims, 0 );
	return param.substr( 0, iPos );
}

const StringVector& ioStringConverter::Split( const std::string &param, const std::string &delims, int iMaxSplit )
{
	size_t iStart, iPos;
	iStart = 0;

	int iNumSplits = 0;
	m_vSplitResults.erase( m_vSplitResults.begin(), m_vSplitResults.end() );

	do
	{
		iPos = param.find_first_of( delims, iStart );
		if( iPos == iStart )
		{
			iStart = iPos + 1;
		}
		else if( iPos == param.npos || ( iMaxSplit && iNumSplits == iMaxSplit ) )
		{
			m_vSplitResults.push_back( param.substr(iStart) );
			break;
		}
		else
		{
			m_vSplitResults.push_back( param.substr( iStart, iPos - iStart ) );
			iStart = iPos + 1;
		}

		iStart = param.find_first_not_of( delims, iStart );
		++iNumSplits;
	}while( iPos != param.npos );

	return m_vSplitResults;
}

void ioStringConverter::Split( OUT StringVector& szSplit, const std::string &param, const std::string &delims, int iMaxSplit )
{
	size_t iStart, iPos;
	iStart = 0;

	int iNumSplits = 0;
	szSplit.clear();

	do
	{
		iPos = param.find_first_of( delims, iStart );
		if( iPos == iStart )
		{
			iStart = iPos + 1;
		}
		else if( iPos == param.npos || ( iMaxSplit && iNumSplits == iMaxSplit ) )
		{
			szSplit.push_back( param.substr(iStart) );
			break;
		}
		else
		{
			szSplit.push_back( param.substr( iStart, iPos - iStart ) );
			iStart = iPos + 1;
		}

		iStart = param.find_first_not_of( delims, iStart );
		++iNumSplits;

	}while( iPos != param.npos );
}

std::string ioStringConverter::AddTailToFileName( const char *szFileName, const char *szAddString )
{
	const StringVector &vParams = Split( std::string( szFileName ), "." );
	if( vParams.size() != 2 )
		return std::string( szFileName );

	std::string szResult = vParams[0] + szAddString + "." + vParams[1];
	return szResult;
}

void ioStringConverter::toLowerCase( std::string &str )
{
	std::transform( str.begin(), str.end(), str.begin(), tolower );
}

void ioStringConverter::toUpperCase( std::string &str )
{
	std::transform( str.begin(), str.end(), str.begin(), toupper );
}
