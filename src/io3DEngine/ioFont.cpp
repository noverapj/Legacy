

#include "stdafx.h"

#include "ioResource.h"
#include "ioFont.h"
#include "ioFontManager.h"

#include "../ioFreeType/ioFreeType.h"

ioFont::ioFont( const ioHashString &name ) : ioResource( name )
{
	m_pFTFace = NULL;
	m_iFontSize = 24;
	m_iWhiteSpaceSize = 9;
}

ioFont::~ioFont()
{
	DestroyCurFace();
}

void ioFont::SetFTFace( ioFTFace *pFace )
{
	DestroyCurFace();

	m_pFTFace = pFace;
}

void ioFont::SetFontSize( int iSize )
{
	if( m_pFTFace )
	{
		m_pFTFace->SetCharSize( iSize );
	}

	m_iFontSize = iSize;
}

void ioFont::SetWhiteSpaceSize( int iSize )
{
	if( m_pFTFace )
	{
		m_pFTFace->SetWhiteSpaceSize( iSize );
	}
}

void ioFont::DestroyCurFace()
{
	if( m_pFTFace )
	{
		ioFTManager::GetManager()->DestroyFace( m_pFTFace );
		g_FontMgr.RemoveMem( GetName() );
		m_pFTFace = NULL;
	}
}

const GlyphImg* ioFont::GetGlyphImg( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphImg( wCode );

	return NULL;
}

int ioFont::GetAdvance( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphAdvance( wCode );

	return 0;
}

int ioFont::GetImgWidth( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphImgWidth( wCode );

	return 0;
}

int ioFont::GetBearingX( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphBearingX( wCode );

	return 0;
}

int ioFont::GetBearingY( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphBearingY( wCode );

	return 0;
}

int ioFont::GetMaxHeight() const
{
	if( m_pFTFace )
		return m_pFTFace->GetMaxGlyphHeight();

	return 0;
}

int ioFont::GetMaxBearingY() const
{
	if( m_pFTFace )
		return m_pFTFace->GetMaxBearingY();

	return 0;
}

