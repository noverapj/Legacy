

#ifndef _ioFontManager_h_
#define _ioFontManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResourceManager.h"
#include "ioRenderOperation.h"
#include "ioMemFile.h"

class ioResource;
class ioFont;
class ioFontWorkSpace;
class ioUIRenderer;
class ioTextPiece;

class __EX ioFontManager : public ioResourceManager, public Singleton< ioFontManager >
{
protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	ioFont* CreateFont( const ioHashString &name );
	ioFont* GetFontByName( const ioHashString &name );

protected:
	bool LoadFile( const ioHashString &name, ioFont *pFont );

public:
	static ioFontManager& GetSingleton();

// ------------- 글자 출력을 위한 기능들 ----------------
protected:
	enum 
	{
		FONT_BUFFER_SIZE = 2048,
	};
	ioUIRenderer *m_pUIRenderer;
	ioFontWorkSpace *m_pWorkSpace;
	ioFont	*m_pCurFont;

	TextAlignType	m_AlignType;
	TextVertAlign   m_VertAlignType;

	DWORD	m_dwTextAlpha;
	float	m_fScale;

	char m_szBuffer[FONT_BUFFER_SIZE];
	// 폰트 사용을 위해서 참조용 메모리도 계속 메모리에 유지해야 함.
	ioMemFile m_MemFile;

protected:
	void SetText( float x, float y, float fScale, BYTE bAlpha );
	void SetTextWidthCut( float x, float y, float fScale, float fWidth, BYTE bAlpha );
	void SetTextLeftCut( float x, float y, float fScale, float fWidth, BYTE bAlpha );

public:
	void Initialize( ioUIRenderer *pRenderer );
	void Flush();
	void Reset();

public:
	bool SetFont( const ioHashString &name );
	void SetFontSize( int iSize, int iWhiteSpaceSize );
	int  GetFontSize();

	void SetFontGap( int iGap );

	void SetAlignType( TextAlignType type ) { m_AlignType = type; }
	void SetTextStyle( TextStyle tStyle );

	void SetVertAlign( TextVertAlign type ) { m_VertAlignType = type; }
	TextVertAlign GetVertAlign() const { return m_VertAlignType; }

	void SetTextColor( DWORD dwColor );
	void SetTextColor( int iRed, int iGreen, int iBlue );

	void SetBkColor( DWORD dwColor );
	void SetBkColor( int iRed, int iGreen, int iBlue );

public:
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void PrintText( float x, float y, float fScale, const char *szSource, ... );
#else
	void PrintText( float x, float y, float fScale, const char *szSource );
#endif
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1 );
	void PrintText( float x, float y, float fScale, const char *szSource, DWORD dwArg1 );
	void PrintText( float x, float y, float fScale, const char *szSource, unsigned int uArg1 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1 );
	void PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, const char *szArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, int iArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1, int iArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, const char *szArg2 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2, int iArg3 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, const char *szArg2, const char *szArg3 );
	void PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, int iArg2, int iArg3 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5 );
	void PrintText( float x, float y, float fScale, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5 );
	void PrintText( float x, float y, float fScale, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5 );
	void PrintText( float x, float y, float fScale, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5, float fArg6 );

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, ... );
#else
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource );
#endif
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, int iArg1 );
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, float fArg1 );
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, const char *szArg1 );
	void PrintTextAlpha( float x, float y, float fScale, BYTE bAlpha, const char *szSource, const char *szArg1, const char *szArg2 );

	// PrintTextWidthCut : Text를 width 사이즈만큼 자르고 "..."으로 처리한다.
	// Text마다 사이즈를 구하므로 느리다.
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, ... );
#else
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource );
#endif
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, float fArg1 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, const char *szArg2 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, int iArg2 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, const char *szArg2 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2, int iArg3 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 );
	void PrintTextWidthCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 );

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, ... );
#else
	void PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource );
#endif
	void PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, int iArg1 );
	void PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, float fArg1 );
	void PrintTextWidthCutAlpha( float x, float y, float fScale, float fWidth, BYTE bAlpha, const char *szSource, const char *szArg1 );

	// ioEdit에서 윈도우의 넓이를 넘어서는 스트링을 자른다.
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, ... );
#else
	void PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource );
#endif
	void PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, int iArg1 );
	void PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, float fArg1 );
	void PrintTextLeftCut( float x, float y, float fScale, float fWidth, const char *szSource, const char *szArg1 );

	float GetTextWidth( const char *szText, TextStyle eStyle, float fScale, int iFontGap = 0 );
	// 텍스트를 특정 사이즈로 잘라도 결국엔 특정 사이즈보다 작은 사이즈로 잘리므로 잘린 텍스트의 사이즈를 반환.
	float GetTextWidthCutSize( const char *szText, TextStyle eStyle, float fScale, float fWidth );

	// 방금 출력한 텍스트의 사이즈
	float GetRecentlyPrintTextWidth( float fScale );

	float GetTextHeight( const char *szText, TextStyle eStyle, float fScale, int iFontGap = 0 );
	float GetTextWidth( const char *szText, float fScale );

protected:
	void SetScale( float fScale );
	void PrintByAlign( float x, float y );

protected:
	
	void TextWidthCut( const char *szAdd, float fScale, float fWidth );
	void TextLeftCut( float fScale, float fWidth );

public:	// for debugging
	void RenderTextTexture( int iXPos, int iYPos, float fScale = 1.0f );

public:
	ioFontManager();
	virtual ~ioFontManager();
};

#define g_FontMgr ioFontManager::GetSingleton()

#endif
