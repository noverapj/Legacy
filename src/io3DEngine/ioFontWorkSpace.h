

#ifndef _ioFontWorkSpace_h_
#define _ioFontWorkSpace_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioFont;
class ioTexture;

#define TEX_WIDTH	512
#define TEX_HEIGHT	512
#define DRAW_COLOR_TABLE 16

class WorkTexture
{
public:
	ioTexture *m_pTex;
	WorkTexture *m_pNext;

private:
	WORD *m_pImage;
	int m_iTop;
	int m_iBottom;

public:
	inline WORD* GetImageBuf() const { return m_pImage; }

public:
	void NeedUpdate( int iTop, int iBottom );
	void UpdateTexture();
	void ClearImageBuffer();

public:
	WorkTexture();
	~WorkTexture();
};

class ioTextPiece
{
public:
	ioHashString m_Text;
	ioFont	*m_pFont;

public:
	DWORD   m_dwTextColor;
	DWORD	m_dwBkColor;
	TextStyle m_TextStyle;
	int		m_iFontGap;

public:
	WorkTexture *m_pTex;
	
	int	m_iWidth;
	int m_iHeight;
	int	m_iFirstBearingX;

public:
	int	m_iUpdateCnt;

public:
	struct PieceQuad
	{
		int m_iStartU;
		int m_iStartV;
		int m_iWidth;
	};
	typedef std::vector< PieceQuad > PieceQuadList;
	PieceQuadList m_vRenderQuad;

public:
	bool operator<( const ioTextPiece &rhs ) const;

public:
	ioTextPiece();
	ioTextPiece( ioFont *pFont, const char *szText );
	~ioTextPiece();
};

class ioFontWorkSpace
{
private:
	int		m_iStartU, m_iStartV;
	int		m_iTop, m_iBottom;
	int		m_iNeedHeight;

	WORD	*m_pLineBuf;
	int		m_iLineBufWidth;
	int		m_iLineBufHeight;

	struct LineCopyInfo
	{
		const GlyphImg *pImg;
		int iStartX;
	};
	typedef std::vector< LineCopyInfo > LineCopyList;
	LineCopyList m_LineCopyList;

	class PieceSort : public std::binary_function< const ioTextPiece*, const ioTextPiece*, bool >
	{
	public:
		inline bool operator()( const ioTextPiece *pLeft, const ioTextPiece *pRight ) const
		{
			return *pLeft < *pRight;
		}
	};

	typedef std::set< ioTextPiece*, PieceSort > ioTextPieceSet;
	typedef std::list< ioTextPiece* > ioTextPieceList;

	ioTextPieceSet	m_PieceList;
	ioTextPieceList m_ReserveList;

	WorkTexture	*m_pCurTex;
	WorkTexture *m_pPivotTex;

private:	// Current Text State..
	DWORD m_dwTextColor;
	DWORD m_dwBkColor;
	TextStyle m_TextStyle;
	int   m_iFontGap;

	//UJ 130903, 최적화: short 단위로 access
	union WORDColor
	{
		struct 
		{
			WORD b : 4;
			WORD g : 4;
			WORD r : 4;
			WORD a : 4;
		} pixel;
		unsigned short argb;
	};

	int m_AlphaColorTable[DRAW_COLOR_TABLE][DRAW_COLOR_TABLE];
	WORDColor m_aDrawColor[DRAW_COLOR_TABLE];

private:
	static int m_iTexCreateCnt;

public:
	void SetTextColor( DWORD dwColor ) { m_dwTextColor = dwColor; }
	void SetBkColor( DWORD dwColor ) { m_dwBkColor = dwColor; }
	void SetTextStyle( TextStyle eStyle ) { m_TextStyle = eStyle; }
	void SetFontGap( int iFontGap ) { m_iFontGap = iFontGap; }

public:
	ioTextPiece* GetTextPiece( ioFont *pFont, const char *szText );
	POINT GetStyleAddPixel( TextStyle eStyle );
	POINT GetStyleAddTailPixel( TextStyle eStyle );

public:
	int CalculateTextWidth( const char *szText,
							const ioFont *pFont,
							TextStyle eStyle,
							int iFontGap );

	int CalculateTextWidth( const char *szText, const ioFont *pFont );

	int CalculateTextHeight( const char *szText, const ioFont *pFont, TextStyle eStyle );

	void Flush();

private:
	WorkTexture* CreateNewTexture();
	ioTexture* CreateD3DTexture();

private:
	void WriteTexture( ioTextPiece *pPiece );
	void RemovePage( WorkTexture *pCurTex );

	void WriteLineToImageBuffer( int iSrcX,
								 int iWidth, int iHeight,
								 int iDestX, int iDestY );

	void SetLineDrawColor( DWORD dwColor, bool bFullColor = false );
	void WriteLineByOutLine( int x, int y, ioTextPiece *pPiece, int iLineGap, bool bBold = false );
	void WriteLineByStyle( int x, int y, ioTextPiece *pPiece );
	void WriteLineBuffer( int iX, int iY, ioTextPiece *pPiece, bool bDirect = false );

private:
	void CheckLineBuffer( int iNeedWidth, int iNeedHeight );
	void ClearLineBuffer();

private:
	inline bool IsLineOver( int iNewCharW ) const
	{
		if( m_iStartU + iNewCharW >= TEX_WIDTH )
			return true;

		return false;
	}
	inline bool IsPageOver( int iNeedHeight ) const
	{
		if( m_iStartV + iNeedHeight >= TEX_HEIGHT )
			return true;

		return false;
	}

public:	// for Test
	void RenderWorkTexture( int iXPos, int iYPos, float fScale );

public:
	ioFontWorkSpace();
	~ioFontWorkSpace();
};

#endif
