

#ifndef _ioFont_h_
#define _ioFont_h_

#include "ioResource.h"

class ioFTFace;
struct GlyphImg;

class ioFont : public ioResource
{
protected:
	ioFTFace *m_pFTFace;

	// cross-script fallback faces (Noto CJK, Noto Thai). owned by ioFont.
	ioFTFace *m_pFallbackFace1;
	ioFTFace *m_pFallbackFace2;

	int m_iFontSize;
	int m_iWhiteSpaceSize;

public:
	void SetFTFace( ioFTFace *pFace );
	void SetFallbackFaces( ioFTFace *pFace1, ioFTFace *pFace2 );
	void SetFontSize( int iSize );
	void SetWhiteSpaceSize( int iSize );

protected:
	void DestroyCurFace();

public:
	const GlyphImg* GetGlyphImg( WORD wCode ) const;
	const GlyphImg* GetGlyphImgWide( wchar_t wChar ) const;

	int GetAdvance( WORD wCode ) const;
	int GetImgWidth( WORD wCode ) const;

	int GetBearingX( WORD wCode ) const;
	int GetBearingY( WORD wCode ) const;

public:
	int GetMaxHeight() const;
	int GetMaxBearingY() const;
	
	inline int GetFontSize() const { return m_iFontSize; }
	inline int GetWhiteSpaceSize() const { return m_iWhiteSpaceSize; }

public:
	ioFont( const ioHashString &name );
	virtual ~ioFont();
};

#endif
