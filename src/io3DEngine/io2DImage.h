

#ifndef _io2DImage_h_
#define _io2DImage_h_

class ioRc2DImage;

class __EX io2DImage
{
private:
	ioRc2DImage	*m_pRc2DImage;
	RECT	m_SrcRect;

	static DWORD m_Screen15LeftRed[32][32];
	static DWORD m_Screen15LeftGreen[32][32];
	static DWORD m_Screen15LeftBlue[32][32];

	static DWORD m_Screen15RightRed[32][32];
	static DWORD m_Screen15RightGreen[32][32];
	static DWORD m_Screen15RightBlue[32][32];

	static DWORD m_Gray15LeftRGB[ 32768 ];
	static DWORD m_Gray15RightRGB[ 32768 ];

public:
	static void InitColorTable();

public:
	void DrawDest( WORD *pDest, int xPos, int yPos, int iPitch );
	void ScreenDrawDest( WORD *pDest, int xPos, int yPos, int iPitch );
	void GrayDrawDest( WORD *pDest, int xPos, int yPos, int iPitch );
	void AlphaDrawDest( WORD *pDest, int xPos, int yPos, int iPitch, int iOpacity );

private:
	void __fastcall Screen15Copy4Byte( WORD *pDest, const WORD *pSrc, int iPixelNum );
	void __fastcall Gray15Copy4Byte( WORD *pDest, const WORD *pSrc, int iPixelNum );

public:
	void SetSrcRect( int iStartX, int iStartY, int iWidth, int iHeight );
	void SetSrcRectWidth( int iStartX, int iWidth );
	void SetSrcRectHeight( int iStartY, int iHeight );
	void RestoreSrcRect();

	const ioHashString& GetName() const;
	int GetWidth() const;
	int GetHeight() const;

private:
	const WORD* GetImgHPtr( int yPos );
	const WORD* GetImgWPtr( const WORD *pSrc, int xPos, bool &bColorKey, int &iNum );

public:
	io2DImage( const ioHashString &name );
	~io2DImage();
};

#endif