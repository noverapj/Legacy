

#ifndef _ioUITitle_h_
#define _ioUITitle_h_

#include "io3DCommon.h"

class __EX ioUITitle
{
public:
	enum PrintTextStyle
	{
		PTS_NORMAL         = 0,
		//PTS_NORMAL_ALPHA   = 1,
		PTS_WIDTHCUT       = 2,
		//PTS_WIDTHCUT_ALPHA = 3,
		//PTS_LEFTCUT        = 4,
	};

protected:
	int m_iXOffset;
	int m_iYOffset;

	int m_iSize;
	int m_iVertGap;
	int m_iFontGap;
	int m_iAlphaRate;
	float m_fWidthCutSize;
	
	TextStyle      m_TextStyle;
	TextStyle      m_DisableTextStyle;
	TextAlignType  m_TextHorzAlign;
	TextVertAlign  m_TextVertAlign;
	PrintTextStyle m_PrintTextStyle;

	ioHashStringVec m_vTextList;

	struct TitleColor
	{
		DWORD dwText;
		DWORD dwBk;
		
		TitleColor()
		{
			dwText = 0xff000000;
			dwBk = 0xff000000;
		}
	};

	TitleColor m_Normal;
	TitleColor m_Over;
	TitleColor m_Push;
	TitleColor m_Disable;
	TitleColor m_NoneSelected;

	bool m_bVisible;

public:
	void SetSize( int iSize ) { m_iSize = iSize; }
	void SetAlpha( int iAlphaRate ){ m_iAlphaRate = min( MAX_ALPHA_RATE, max( 0, iAlphaRate ) ); }
	void SetVertGap( int iGap ) { m_iVertGap = iGap; }
	void SetOffset( int iXOffset, int iYOffset ) { m_iXOffset = iXOffset, m_iYOffset = iYOffset; }
	void SetOffsetX( int x ) { m_iXOffset = x; }
	void SetOffsetY( int y ) { m_iYOffset = y; }

	void SetStyle( TextStyle eStyle ) { m_TextStyle = eStyle; }
	void SetStyle( const ioHashString &szStyle );

	void SetDisableStyle( TextStyle eStyle ) { m_DisableTextStyle = eStyle; }
	void SetDisableStyle( const ioHashString &szStyle );

	void SetHorzAlign( TextAlignType eAlign ) { m_TextHorzAlign = eAlign; }
	void SetHorzAlign( const ioHashString &szAlign );

	void SetVertAlign( TextVertAlign eAlign ) { m_TextVertAlign = eAlign; }
	void SetVertAlign( const ioHashString &szAlign );

	void SetColor( const ioHashString &szType, DWORD dwTextColor, DWORD dwBkColor );
	void SetColor( const ioHashString &szType, const ioHashString &szText, const ioHashString &szBk );
	void SetAllColor( DWORD dwTextColor, DWORD dwBkColor );

	void SetFontGap( int iGap );
	void SetText( const char *szText, const char *szDelims );

	void SetPrintTextStyle( PrintTextStyle ePrintStyle ) { m_PrintTextStyle = ePrintStyle; }
	void SetWidthCutSize(float fWidthCutSize) { m_fWidthCutSize = fWidthCutSize; }
	void SetWidthCutSize(const ioHashString &szAlign);

public:
	void PrintNormal( int iXPos, int iYPos );
	void PrintOver( int iXPos, int iYPos );
	void PrintPush( int iXPos, int iYPos );
	void PrintDisable( int iXPos, int iYPos );
	void PrintNoneSelected( int iXPos, int iYPos );
	void Print( int iXPos, int iYPos, DWORD dwTextColor, DWORD dwBkColor, TextStyle eTextStyle );

public:
	// 윈도우 기반에서 정렬을 위해 현재 정렬상태에 따른 렌더위치를 알려주는 함수
	void GetRenderPosForWndEX( int x, int y, int width, int height, int& outx, int& outy );

	//! 툴을 위한 인터페이스 ( 다른곳에서 사용하지 마세요 )
	const char* GetText() { static ioHashString ret; ret = ""; GetFirstText( ret ); return ret.c_str(); }

	inline int GetAlpha() const { return m_iAlphaRate; }
	inline int GetSize() const { return m_iSize; }
	inline int GetTextXOffSet() const { return m_iXOffset; }
	inline int GetTextYOffSet() const { return m_iYOffset; }
	inline int GetVertGap() const { return m_iVertGap; }

	inline PrintTextStyle GetPrintTextStyle() const { return m_PrintTextStyle; }
	inline float GetWidthCutSize() const { return m_fWidthCutSize; }

	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool GetVisible() { return m_bVisible; }

	inline TextStyle GetTextStyle() const { return m_TextStyle; }
	inline TextStyle GetDisableTextStyle() const { return m_DisableTextStyle; }

	inline TextAlignType GetHorzAlign() const { return m_TextHorzAlign; }
	inline TextVertAlign GetVertAlign() const { return m_TextVertAlign; }

public:
	inline DWORD GetNormalTextColor() const { return m_Normal.dwText; }
	inline void SetNormalTitleTextColor( DWORD c ) { m_Normal.dwText = c; }
	inline void SetNormalTitleBKColor( DWORD c ) { m_Normal.dwBk = c; }
	inline const TitleColor& GetNormalTitleColor() const { return m_Normal; }

	inline void SetOverTitleTextColor( DWORD c ) { m_Over.dwText = c; }
	inline void SetOverTitleBKColor( DWORD c ) { m_Over.dwBk = c; }
	inline const TitleColor& GetOverTitleColor() const { return m_Over; }

	inline void SetPushTitleTextColor( DWORD c ) { m_Push.dwText = c; }
	inline void SetPushTitleBKColor( DWORD c ) { m_Push.dwBk = c; }
	inline const TitleColor& GetPushTitleColor() const { return m_Push; }

	inline void SetDisableTitleTextColor( DWORD c ) { m_Disable.dwText = c; }
	inline void SetDisableTitleBKColor( DWORD c ) { m_Disable.dwBk = c; }
	inline const TitleColor& GetDisableTitleColor() const { return m_Disable; }

	inline void SetNoneSelectedTitleTextColor( DWORD c ) { m_NoneSelected.dwText = c; }
	inline void SetNoneSelectedTitleBKColor( DWORD c ) { m_NoneSelected.dwBk = c; }
	inline const TitleColor& GetNoneSelectedTitleColor() const { return m_NoneSelected; }
	
	void GetFirstText( OUT ioHashString &rszText );
public:
	ioUITitle();
	virtual ~ioUITitle();
};

class __EX IoUIText : public ioUITitle
{
public:
	enum
	{
		URT_PRE_RENDER		= 1,
		URT_NORMAL_RENDER	= 2,
		URT_AFTER_RENDER	= 3,
	};

public:
	IoUIText();
	virtual ~IoUIText();

	void SetName( const char *name );
	ioHashString GetName(){return m_szName;};
	int		GetPosX(){return m_iXOffset;};
	int		GetPosY(){return m_iYOffset;};

	ioHashString GetText(){return m_vTextList[0];};

	float	GetScale(){return (float)m_iSize;};

	TextAlignType  GetHorzAlign(){return m_TextHorzAlign;};
	TextVertAlign  GetVertAlign(){return m_TextVertAlign;};

	DWORD GetTextColor() const { return m_Normal.dwText; };
	DWORD GetBackColor() const { return m_Normal.dwBk; };

	void SetText( const char *szText , const char *szDelims = "" ){ ioUITitle::SetText(szText, szDelims); };
	void SetParserRenderPos( const ioHashString &szRenderPos );

	inline int GetRenderPos() const { return m_iRenderPos; }

protected:
	ioHashString m_szName;
	int m_iRenderPos;
};
#endif
