

#ifndef _ioFTFaceImpl_h_
#define _ioFTFaceImpl_h_

class ioFTFaceImpl : public ioFTFace
{
protected:
	FT_Face		m_FTFace;
	std::string m_FaceName;
	ioFTEncode  m_Encoding;

	int		m_iHorzResolution;
	int		m_iVertResolution;

	typedef std::map< WORD, GlyphImg* > GlyphMap;
	GlyphMap m_GlyphMap;

	typedef std::map< wchar_t, GlyphImg* > WideGlyphMap;
	WideGlyphMap m_WideGlyphMap;

	int m_iMaxGlyphWidth;
	int m_iMaxGlyphHeight;
	int m_iMaxGlyphBearingY;

	// 국가 코드 페이지
	int m_iCodePage;

public:
	void InitFTFace( const std::string &szName, FT_Face pFace );

protected:
	void FreeFTFace();
	void UpdateGlyphGlobalInfo();

public:
	virtual void SetCharSize( int iSize );
	virtual void SetDeviceResolution( int iHorz, int iVert );

	virtual void SetWhiteSpaceSize( int iSize );
	virtual void SetCodePage( int iCodePage );

public:
	virtual const GlyphImg* GetGlyphImg( WORD wCode );
	virtual const GlyphImg* GetGlyphImgWide( wchar_t wChar );

	virtual int GetGlyphAdvance( WORD wCode );
	virtual int GetGlyphImgWidth( WORD wCode );
	virtual int GetGlyphImgHeight( WORD wCode );

	virtual int GetGlyphBearingX( WORD wCode );
	virtual int GetGlyphBearingY( WORD wCode );

	virtual int GetMaxGlyphWidth() const;
	virtual int GetMaxGlyphHeight() const;
	virtual int GetMaxBearingY() const;

public:
	virtual const char* GetName() const;
	virtual int GetNumGlyphs() const;
	virtual int GetNumFaces() const;

	virtual ioFTEncode GetEncode() const;
	virtual const char* GetEncodeText() const;

protected:
	FT_GlyphSlot FindGlyphSlot( WORD wCode );
	FT_GlyphSlot FindGlyphSlotWide( wchar_t wChar );
	const GlyphImg* AddNewGlyphImg( WORD wCode );
	const GlyphImg* AddNewWideGlyphImg( wchar_t wChar );
	GlyphImg* PackGlyphBitmap( FT_GlyphSlot ftGlyph );

	bool IsValidGlyph( FT_GlyphSlot ftGlyph );

protected:
	WORD ConvertToCharCode( wchar_t wChar );

public:
	ioFTFaceImpl();
	virtual ~ioFTFaceImpl();
};

#endif
