

#ifndef _ioStringConverter_h_
#define _ioStringConverter_h_

#include "io3DCommon.h"

typedef std::vector< std::string > StringVector;

class __EX ioStringConverter
{
private:
	static char	m_ConvertBuf[MAX_PATH];
	static std::vector< float > m_vTempFloatList;

	static StringVector m_vSplitResults;

public:
	static std::string toString( float val );

	static std::string toString( int val );

	static std::string toString( unsigned int val );

	static std::string toString( long val );

	static std::string toString( unsigned long val );

	// if bYesNo is true => yes/no, else true/false
	static std::string toString( bool val, bool bYesNo = false );

	// Format : "x y"
	static std::string toString( const D3DXVECTOR2 &val );

	// Format : "x y x y x y..."
	static std::string toString( const Vector2Vec &val );

	// Format : "x y z"
	static std::string toString( const D3DXVECTOR3 &val );

	static std::string toString( const Vector3Vec &val );

	static std::string toString( const D3DXVECTOR4 &val );

	// Format : "r g b a"
	static std::string toString( const D3DCOLORVALUE &val );

	// Foramt : "x y z w"
	static std::string toString( const D3DXQUATERNION &val );

	// Format : "f x y z f x y z f x y z..."
	static std::string toString( const std::vector<D3DXVECTOR4> &val );

	static const char* toString( ErasePolicy ePolicy );

public:
	static float ParseFloat( const char *szVal );

	static int ParseInt( const char *szVal );

	static unsigned int ParseUnsignedInt( const char *szVal );

	static long ParseLong( const char *szVal );

	static unsigned long ParseUnsignedLong( const char *szVal );

	static bool ParseBool( const char *szVal );

	static D3DXVECTOR2 ParseVector2( const char *szVal );

	static void ParseVector2Vec( const char *szVal, Vector2Vec &vList );

	static D3DXVECTOR3 ParseVector3( const char *szVal );

	static void ParseVector3Vec( const char *szVal, Vector3Vec &vList );

	static D3DXQUATERNION ParseQuaternion( const char *szVal );

	static D3DXVECTOR4 ParseVector4( const char *szVal );
	
	static D3DCOLORVALUE ParseColorValue( const char *szVal );

	static void ParseVector4Vec( const char *szVal, std::vector<D3DXVECTOR4> &vList );

	static TextAlignType ParseTextAlignType( const ioHashString &szType );
	static ioHashString ParseTextAlignType( TextAlignType type );
	static TextVertAlign ParseTextVertAlign( const ioHashString &szType );
	static ioHashString ParseTextVertAlign( TextVertAlign type );
	static ErasePolicy ParseErasePolicy( const ioHashString &szType );
	static TextStyle ParseTextStyle( const ioHashString &szStyle );
	static ioHashString ParseTextStyle( TextStyle style );
	static UIRenderType ParseRenderType( const ioHashString &szType );

	static FXRenderType ParseFXRenderType( const ioHashString &szType );
	static TextureFilterOption ParseTextureFilterOption( const ioHashString &szType );

	static DWORD ParseColor( const ioHashString &szColor );
	static ioHashString ParseColor( DWORD color );

public:
	static std::string SplitFirstOnly( const std::string &param, const std::string &delims );

	static const StringVector& Split( const std::string &param, const std::string &delims, int iMaxSplit = 0 );
	static void Split( OUT StringVector& szSplit, const std::string &param, const std::string &delims, int iMaxSplit = 0 );

	static std::string AddTailToFileName( const char *szFileName, const char *szAddString );

	static void toLowerCase( std::string &str );
	static void toUpperCase( std::string &str );
};

#endif
