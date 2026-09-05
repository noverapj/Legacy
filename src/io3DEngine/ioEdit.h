

#ifndef _ioEdit_h_
#define _ioEdit_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioStringConverter.h"

class ioWnd;
class ioXMLElement;

#define MAX_EDIT_TEXT	260
#define MAX_MULTI_LINE	2

class __EX ioEdit : public ioWnd
{
protected:
	int		m_iMaxLen;
	int     m_iMaxLine;
	char	m_szText[ MAX_EDIT_TEXT ];

	int     m_iPrevLeftCut;
	int     m_iLeftCutPos;
	ioHashStringVec m_vSplitText;
	std::string     m_szMultiText;

	ioHashString m_FontName;
	DWORD	m_dwTextColor;
	DWORD	m_dwBkColor;
	float   m_fScale;

	TextAlignType	m_AlignType;
	TextStyle		m_TextStyle;

	bool	m_bKeyFocus;
	bool	m_bReadOnly;

	int     m_iCaretXPos;

public:
	static ioEdit	*m_pKeyFocusEdit;

protected:
	static DWORD m_dwPreCaretCtrlTime;
	static DWORD m_dwCaretTwinkleGap;
	static bool  m_bNowCaretShow;

protected:	
	StringVector m_vMultiText;

	int m_iCurCaretLine;

	int m_iCurPage;	
	int m_iPrevCaretPos;
	int m_iSpaceBetweenLine;
	int m_iEndLineMoveAmount;

public:

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	DECL_NAME_INTERFACE_FOR_TOOL( Edit, ioEdit )
	//************************************************************************************************************************

	static void CheckCaretTwinkle( DWORD dwCurTime );
	static void ForceCaretTwinkle();

	static void SetCaretTwinkleGap( DWORD dwGap );
	static bool IsNowCaretShow();

public:
	void SetMaxLength( int iLen );
	void SetReadOnly();
	void SetWriteCan() { m_bReadOnly = false; }

protected:
	void SetTextSplit();
	bool CheckHanByte( char *szText ) const;
	bool CheckConstHanByte( const char *szText ) const;
	void GetTextLeftCut( char *pSrc, char *pDst, char *pCut, float fWidth );
	void GetTextRightCut( char *pSrc, char *pDst, char *pCut, float fWidth );
	void RenderMultiLine();

public:
	int GetCurCaretLine();
	int GetMaxLine();
	int GetMaxLength();

	int GetCurPage();
	int GetMaxPage();
	void PrevPage();
	void NextPage();

	void SpliteTextWidth( std::string&szSource, std::string&szLeftDest, std::string&szRightDest );
	void DevideMultiLineText( int iCurLine );
	void SetCaretLine( int iLine );
	void SetMultiText( StringVector& vMultiText );
	inline const StringVector& GetMultiText() const { return m_vMultiText; }
	int GetMultiLineTextLength();
	int GetMultiLineCnt();
	std::string& GetMultiLineText( int iCurLine );

	void ClearMultiText();

protected:
	void MultiLineEnter();
	void MultiLineBackSpace();
	void MultiLineDel();
	void MultiLineLeft();
	void MultiLineRight();
	void MultiLineUp();
	void MultiLineDown();
	void MultiLineProcess();

public:
	void SetCaretViewPage();	
	bool CheckMaxLength();

public:
	inline bool IsKeyFocus() const { return m_bKeyFocus; }

	virtual void SetKeyFocus();
	virtual void KillKeyFocus();

public:
	void SetText( const char *szText );
	const char* GetText() const;

	void ClearString();

	void SetFontName( const char *szFontName ) { m_FontName = szFontName; }
	void SetTextColor( DWORD dwTextColor, DWORD dwTextBkColor );
	void SetTextAlignType( TextAlignType tType ){ m_AlignType = tType; }
	void SetTextStyle( TextStyle tStyle ){ m_TextStyle = tStyle; }
	void SetFontScale( float fScale ){ m_fScale = fScale; }

	inline int GetCaretXPos() const { return m_iCaretXPos; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_create();
	virtual void iwm_lbuttondown( const ioMouse& mouse );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual DWORD GetWndType() const;

public:
	ioEdit();
	virtual ~ioEdit();
};

#endif