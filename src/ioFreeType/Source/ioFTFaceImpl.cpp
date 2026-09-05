

#include "stdafx.h"

#include "ioFTFace.h"
#include "ioFTFaceImpl.h"

ioFTFaceImpl::ioFTFaceImpl()
{
	m_FTFace = NULL;
	m_Encoding = ENCODING_NONE;

	m_iHorzResolution = 72;
	m_iVertResolution = 72;

	// 기본값 한국 코드 페이지
	// 각 국가별로 한번 배포 된 뒤 수정이 되지 않으므로 코드내에서 직접 바꿔주도록 한다.
	// Indonesia		1252
	// North America	1252
	// Taiwan			950
	// Thailand			874
	// Europe			1252
	// South America	1250
	// South Korea		949

	m_iCodePage	= 949;
}

ioFTFaceImpl::~ioFTFaceImpl()
{
	FreeFTFace();
}

void ioFTFaceImpl::InitFTFace( const std::string &szName, FT_Face pFace )
{
	FreeFTFace();

	m_FTFace = pFace;
	m_FaceName = szName;

	if( pFace->charmap )
	{
		m_Encoding = (ioFTEncode)pFace->charmap->encoding;
	}
	else
	{
		if( pFace->num_charmaps > 0 )	// 윤고딕240에는 unicodemap이 없다. 쐣
		{
			m_Encoding = (ioFTEncode)pFace->charmaps[0]->encoding;
			FT_Select_Charmap( pFace, (FT_Encoding)m_Encoding );
		}
		else
		{
			m_Encoding = ENCODING_NONE;
		}
	}
}

void ioFTFaceImpl::FreeFTFace()
{
	GlyphMap::iterator iter, iEnd;
	iEnd = m_GlyphMap.end();
	for( iter=m_GlyphMap.begin() ; iter!=iEnd ; ++iter )
	{
		delete iter->second;
	}
	m_GlyphMap.clear();

	if( m_FTFace )
	{
		FT_Done_Face( m_FTFace );
		m_FTFace = NULL;
	}
}

void ioFTFaceImpl::UpdateGlyphGlobalInfo()
{
	if( !m_FTFace )	return;

	float fWidthRate  = (float)m_FTFace->size->metrics.x_ppem / (float)m_FTFace->units_per_EM;
	float fHeightRate = (float)m_FTFace->size->metrics.y_ppem / (float)m_FTFace->units_per_EM;

	if( FT_IS_SCALABLE( m_FTFace ) )
	{
		float fBoxWidth  = (float)( m_FTFace->bbox.xMax - m_FTFace->bbox.xMin );
		float fBoxHeight = (float)( m_FTFace->bbox.yMax - m_FTFace->bbox.yMin );

		m_iMaxGlyphWidth  = (int)( fBoxWidth * fWidthRate + 0.99999f );
		m_iMaxGlyphHeight = (int)( fBoxHeight * fHeightRate  + 0.99999f );
	}
	else
	{
		m_iMaxGlyphWidth  = m_FTFace->size->metrics.max_advance >> 6;
		m_iMaxGlyphHeight = m_FTFace->size->metrics.height >> 6;
	}

	int iMaxBearingY = max( m_FTFace->ascender, m_FTFace->bbox.yMax );
	m_iMaxGlyphBearingY = (int)( (float)iMaxBearingY * fHeightRate + 0.99999f );
}

int ioFTFaceImpl::GetMaxGlyphWidth() const
{
	return m_iMaxGlyphWidth;
}

int ioFTFaceImpl::GetMaxGlyphHeight() const
{
	return m_iMaxGlyphHeight;
}

int ioFTFaceImpl::GetMaxBearingY() const
{
	return m_iMaxGlyphBearingY;
}

void ioFTFaceImpl::SetWhiteSpaceSize( int iSize )
{
	WORD wWhiteSpace = ' ';
	
	GlyphImg *pWhite = NULL;
	GlyphMap::iterator iter = m_GlyphMap.find( wWhiteSpace );
	if( iter == m_GlyphMap.end() )
	{
		pWhite = new GlyphImg;
		pWhite->iBearingX = 0;
		pWhite->iBearingY = m_iMaxGlyphBearingY;
		pWhite->iAdvance  = iSize;
		pWhite->iWidth    = iSize;
		pWhite->iHeight   = m_iMaxGlyphHeight;
		pWhite->pBuf = NULL;

		m_GlyphMap.insert( GlyphMap::value_type( wWhiteSpace, pWhite ) );
	}
	else
	{
		pWhite = iter->second;
		pWhite->iAdvance  = iSize;
		pWhite->iWidth    = iSize;
	}
}

void ioFTFaceImpl::SetDeviceResolution( int iHorz, int iVert )
{
	m_iHorzResolution = iHorz;
	m_iVertResolution = iVert;
}

void ioFTFaceImpl::SetCharSize( int iSize )
{
	if( !m_FTFace )	return;

	FT_Error ftError = FT_Set_Char_Size( m_FTFace,
										 iSize * 64,
										 iSize * 64,
										 m_iHorzResolution,
										 m_iVertResolution );

	UpdateGlyphGlobalInfo();

	if( ftError )
	{
		LOG.PrintTimeAndLog( 0, "ioFTFace::SetCharSize - Failed(%d)", ftError );
	}
}

const GlyphImg* ioFTFaceImpl::GetGlyphImg( WORD wCode )
{
	GlyphMap::iterator iter = m_GlyphMap.find( wCode );
	if( iter != m_GlyphMap.end() )
		return iter->second;

	return AddNewGlyphImg( wCode );
}

int ioFTFaceImpl::GetGlyphAdvance( WORD wCode )
{
	const GlyphImg *pImg = GetGlyphImg( wCode );
	if( pImg )
		return pImg->iAdvance;

	return 0;
}

int ioFTFaceImpl::GetGlyphImgWidth( WORD wCode )
{
	const GlyphImg *pImg = GetGlyphImg( wCode );
	if( pImg )
		return pImg->iWidth;

	return 0;
}

int ioFTFaceImpl::GetGlyphImgHeight( WORD wCode )
{
	const GlyphImg *pImg = GetGlyphImg( wCode );
	if( pImg )
		return pImg->iHeight;

	return 0;
}

int ioFTFaceImpl::GetGlyphBearingX( WORD wCode )
{
	const GlyphImg *pImg = GetGlyphImg( wCode );
	if( pImg )
		return pImg->iBearingX;

	return 0;
}

int ioFTFaceImpl::GetGlyphBearingY( WORD wCode )
{
	const GlyphImg *pImg = GetGlyphImg( wCode );
	if( pImg )
		return pImg->iBearingY;

	return 0;
}

WORD ioFTFaceImpl::ConvertToCharCode( wchar_t wChar )
{
	WORD wCode = (WORD)wChar;

	if( m_Encoding == ENCODING_WANSUNG )
	{
		if( !(wChar >= ' ' && wChar < 127 ) )	// Not Ascii
		{
			WideCharToMultiByte( 949, // 949  : korean
								 0,
								 &wChar, 1,
								 (LPSTR)&wCode, sizeof(wCode),
								 "*",
								 NULL );

			wCode = ( ( (wCode&0xff00) >> 8 ) | ( (wCode&0xff) << 8 ) );
		}
	}

	return wCode;
}

const GlyphImg* ioFTFaceImpl::AddNewGlyphImg( WORD wCode )
{
	FT_GlyphSlot ftGlyph = FindGlyphSlot( wCode );
	if( !IsValidGlyph( ftGlyph ) )
		return NULL;

	GlyphImg *pNewGlyph = new GlyphImg;

	pNewGlyph->iBearingX = ftGlyph->metrics.horiBearingX >> 6;
	pNewGlyph->iBearingY = ftGlyph->metrics.horiBearingY >> 6;
	pNewGlyph->iAdvance  = ftGlyph->advance.x >> 6;

	FT_Bitmap *pFtBitmap = &ftGlyph->bitmap;
	int iWidth  = pFtBitmap->width;
	int iHeight = pFtBitmap->rows;

	pNewGlyph->iWidth  = iWidth;
	pNewGlyph->iHeight = iHeight;

	int iEvenWidth = iWidth / 2 * 2;	// 홀수자리를 하나 때버림
	int iAllocSize = ( iWidth + 1 ) / 2 * iHeight;
	pNewGlyph->pBuf = new BYTE[ iAllocSize ];

	int iPitch = pFtBitmap->pitch;
	char cPixelMode = pFtBitmap->pixel_mode;
	unsigned char *pSrc = pFtBitmap->buffer;

	BYTE *pBuf = (BYTE*)pNewGlyph->pBuf;
	if( cPixelMode == FT_PIXEL_MODE_GRAY )
	{
		for( int i=0 ; i<iHeight ; i++ )
		{
			for( int j=0 ; j<iEvenWidth ; j+=2 )
			{
				*pBuf++ = ( ( pSrc[j] >> 4 ) << 4 ) | ( pSrc[j+1] >> 4 );
			}

			if( iWidth & 1 )
			{
				*pBuf++ = ( pSrc[j] >> 4 ) << 4;
			}

			pSrc += iPitch;
		}
	}
	else	// FT_PIXEL_MODE_MONO
	{
		unsigned char uByte;
		for( int i=0 ; i<iHeight ; i++ )
		{
			for( int j=0 ; j<iEvenWidth ; j+=2 )
			{
				BYTE bCode = 0;

				uByte = pSrc[ j>>3 ] << (j%8);

#if defined( SRC_OVERSEAS )
				if( IsDBCSLeadByteEx( m_iCodePage, uByte ) )
#else
				if( IsDBCSLeadByte( uByte ) )
#endif
					bCode = 0xf0;

				uByte = pSrc[ (j+1)>>3 ] << ( (j+1)%8 );

#if defined( SRC_OVERSEAS )
				if( IsDBCSLeadByteEx( m_iCodePage, uByte ) )
#else
				if( IsDBCSLeadByte( uByte ) )
#endif
					bCode |= 0x0f;

				*pBuf++ = bCode;
			}

			if( iWidth & 1 )
			{
				uByte = pSrc[ j>>3 ] << (j%8);
#if defined( SRC_OVERSEAS )
				if( IsDBCSLeadByteEx( m_iCodePage, uByte ) )
#else
				if( IsDBCSLeadByte( uByte ) )
#endif
				{
					*pBuf++ = 0xf0;
				}
			}

			pSrc += iPitch;
		}
	}

	m_GlyphMap.insert( GlyphMap::value_type( wCode, pNewGlyph ) );

	return pNewGlyph;
}

FT_GlyphSlot ioFTFaceImpl::FindGlyphSlot( WORD wCode )
{
	if( m_FTFace )
	{

// 해외버전에서는 single byte check 와 double byte check를 분류하도록 한다.
#if defined( SRC_OVERSEAS )

		BYTE szBuf[3];
		wchar_t wWideChar;

#if defined( SINGLE_BYTE_CHECK )
		// single byte
		// 문자표를 확인하여 체크할 문자열 코드 범위를 세팅해준다.
		if( wCode >= 0x00 && wCode <= 0xFF )
		{
			szBuf[0] = wCode;
			szBuf[1] = 0;
			MultiByteToWideChar( m_iCodePage, 0, (LPCSTR)szBuf, 1, &wWideChar, 1 );
		}
		else
		{
			szBuf[0] = wCode;
			szBuf[1] = 0;
			MultiByteToWideChar( CP_ACP, 0, (LPCSTR)szBuf, 1, &wWideChar, 1 );
		}		
#else
		// double bytes
		if( IsDBCSLeadByteEx( m_iCodePage, ( wCode >> 8 ) & 0xff ) )
		{
			szBuf[1] = wCode & 0xff;
			szBuf[0] = (wCode >> 8) & 0xff;
			szBuf[2] = 0;
			MultiByteToWideChar( m_iCodePage, 0, (LPCSTR)szBuf, 2, &wWideChar, 1 );
		}
		else
		{
			szBuf[0] = wCode & 0xff;
			szBuf[1] = 0;
			szBuf[2] = 0;
			MultiByteToWideChar( m_iCodePage, 0, (LPCSTR)szBuf, 1, &wWideChar, 1 );
		}
#endif

// 국내 버전에서는 기존 코드와 동일하게 가도록 한다.
#else
		char szBuf[3];
		wchar_t wWideChar;
		if( wCode > 0xff )
		{
			szBuf[0] = ( wCode & 0xff00 ) >> 8;
			szBuf[1] = wCode & 0xff;
			szBuf[2] = '\0';
			MultiByteToWideChar( CP_ACP, 0, szBuf, 2, &wWideChar, 1 );
		}
		else
		{
			szBuf[0] = (BYTE)wCode;
			szBuf[1] = '\0';
			MultiByteToWideChar( CP_ACP, 0, szBuf, 1, &wWideChar, 1 );
		}

		wWideChar = ConvertToCharCode( wWideChar );
#endif
		FT_Error ftError = FT_Load_Char( m_FTFace, wWideChar, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL );
		if( ftError == 0 )
		{
			return m_FTFace->glyph;
		}
	}

	return NULL;

}

bool ioFTFaceImpl::IsValidGlyph( FT_GlyphSlot ftGlyph )
{
	if( !ftGlyph )
		return false;

	if( ftGlyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY &&
		ftGlyph->bitmap.pixel_mode != FT_PIXEL_MODE_MONO )
	{
		LOG.PrintTimeAndLog( 0, "ioFTFaceImpl::IsValidGlyph - Unsupported PixelFormat(%d)",
								ftGlyph->bitmap.pixel_mode );
		return false;
	}

	return true;
}

const char* ioFTFaceImpl::GetName() const
{
	return m_FaceName.c_str();
}

int ioFTFaceImpl::GetNumGlyphs() const
{
	if( m_FTFace )
		return m_FTFace->num_glyphs;

	return 0;
}

int ioFTFaceImpl::GetNumFaces() const
{
	if( m_FTFace )
		return m_FTFace->num_faces;

	return 0;
}

ioFTEncode ioFTFaceImpl::GetEncode() const
{
	return m_Encoding;
}

const char* ioFTFaceImpl::GetEncodeText() const
{
	switch( m_Encoding )
	{
	case ENCODING_MS_SYMBOL:
		return "Microsoft Symbol encoding";
	case ENCODING_UNICODE:
		return "Unicode character set";
	case ENCODING_SJIS:
		return "Japanese SJIS encoding";
	case ENCODING_GB2312:
		return "Cimplified Chinese";
	case ENCODING_BIG5:
		return "Traditional Chilese";
	case ENCODING_WANSUNG:
		return "Korean Wansung";
	case ENCODING_JOHAB:
		return "Korean Johab";
	case ENCODING_ADOBE_STANDARD:
		return "Adobe Standard encoding";
	case ENCODING_ADOBE_EXPERT:
		return "Adobe Expert encoding";
	case ENCODING_ADOBE_CUSTOM:
		return "Adobe custom encoding";
	case ENCODING_ADOBE_LATIN_1:
		return "Latin-1 encoding";
	case ENCODING_OLD_LATIN_2:
		return "Latin-2 (Not Used)";
	case ENCODING_APPLE_ROMAN:
		return "8-bit Apple roman encoding";
	}

	return "Unknown";
}
