

#include "stdafx.h"

#include "ioResource.h"
#include "ioFont.h"
#include "ioFontManager.h"

#include "../ioFreeType/ioFreeType.h"

ioFont::ioFont( const ioHashString &name ) : ioResource( name )
{
	m_pFTFace = NULL;
	m_pFallbackFace1 = NULL;
	m_pFallbackFace2 = NULL;
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

void ioFont::SetFallbackFaces( ioFTFace *pFace1, ioFTFace *pFace2 )
{
	if( m_pFallbackFace1 && m_pFallbackFace1 != pFace1 )
	{
		ioFTManager::GetManager()->DestroyFace( m_pFallbackFace1 );
	}

	if( m_pFallbackFace2 && m_pFallbackFace2 != pFace2 )
	{
		ioFTManager::GetManager()->DestroyFace( m_pFallbackFace2 );
	}

	m_pFallbackFace1 = pFace1;
	m_pFallbackFace2 = pFace2;

	if( m_pFallbackFace1 )
	{
		m_pFallbackFace1->SetCharSize( m_iFontSize );
		m_pFallbackFace1->SetWhiteSpaceSize( m_iWhiteSpaceSize );
		m_pFallbackFace1->SetCodePage( ioText::GetCodePage() );
	}

	if( m_pFallbackFace2 )
	{
		m_pFallbackFace2->SetCharSize( m_iFontSize );
		m_pFallbackFace2->SetWhiteSpaceSize( m_iWhiteSpaceSize );
		m_pFallbackFace2->SetCodePage( ioText::GetCodePage() );
	}
}

void ioFont::SetFontSize( int iSize )
{
	if( m_pFTFace )
	{
		m_pFTFace->SetCharSize( iSize );
	}

	if( m_pFallbackFace1 )
	{
		m_pFallbackFace1->SetCharSize( iSize );
	}

	if( m_pFallbackFace2 )
	{
		m_pFallbackFace2->SetCharSize( iSize );
	}

	m_iFontSize = iSize;
}

void ioFont::SetWhiteSpaceSize( int iSize )
{
	if( m_pFTFace )
	{
		m_pFTFace->SetWhiteSpaceSize( iSize );
	}

	if( m_pFallbackFace1 )
	{
		m_pFallbackFace1->SetWhiteSpaceSize( iSize );
	}

	if( m_pFallbackFace2 )
	{
		m_pFallbackFace2->SetWhiteSpaceSize( iSize );
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

	if( m_pFallbackFace1 )
	{
		ioFTManager::GetManager()->DestroyFace( m_pFallbackFace1 );
		m_pFallbackFace1 = NULL;
	}

	if( m_pFallbackFace2 )
	{
		ioFTManager::GetManager()->DestroyFace( m_pFallbackFace2 );
		m_pFallbackFace2 = NULL;
	}
}

const GlyphImg* ioFont::GetGlyphImg( WORD wCode ) const
{
	if( m_pFTFace )
		return m_pFTFace->GetGlyphImg( wCode );

	return NULL;
}

const GlyphImg* ioFont::GetGlyphImgWide( wchar_t wChar ) const
{
	const GlyphImg *pImg = NULL;

	if( m_pFTFace )
		pImg = m_pFTFace->GetGlyphImgWide( wChar );

	if( !pImg && m_pFallbackFace1 )
		pImg = m_pFallbackFace1->GetGlyphImgWide( wChar );

	if( !pImg && m_pFallbackFace2 )
		pImg = m_pFallbackFace2->GetGlyphImgWide( wChar );

	return pImg;
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

