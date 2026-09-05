

#include "stdafx.h"

#include "ioResource.h"
#include "ioRcFont.h"

ioRcFont::ioRcFont( const ioHashString &name ) : ioResource( name )
{
	m_iMaxHeight = 0;
	m_iCodeCnt	 = 0;

	m_pCode   = NULL;
	m_pImage  = NULL;
	m_pChInfo = NULL;
	m_pPalette = NULL;
}

ioRcFont::~ioRcFont()
{
	SAFEDELETEARRAY( m_pCode );
	SAFEDELETEARRAY( m_pImage );
	SAFEDELETEARRAY( m_pChInfo );
	SAFEDELETEARRAY( m_pPalette );
}

void ioRcFont::SetCode( int iCnt, WORD *pCode )
{
	m_iCodeCnt = iCnt;
	m_pCode = pCode;
}

void ioRcFont::SetImage( BYTE *pImage )
{
	SAFEDELETEARRAY( m_pImage );
	m_pImage = pImage;
}

void ioRcFont::SetChInfo( ChInfo *pInfo )
{
	SAFEDELETEARRAY( m_pChInfo );
	m_pChInfo = pInfo;
}

void ioRcFont::SetPalette( WORD *pPalette )
{
	SAFEDELETEARRAY( m_pPalette );
	m_pPalette = pPalette;
}