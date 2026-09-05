//----------------------------------------------------------------------------------
/*
Filename	: ioLabelWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOLABELWNDEX_H_
#define _IOLABELWNDEX_H_

#include "ioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
title 관련 Set / Get 인터페이스를 반복적으로 정의하기 위한 매크로
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
#define DECLARE_IOUITITLE_INTERFACES( member ) \
	void SetLabelVertAlign( TextVertAlign align ) { if ( member ) member->SetVertAlign( align ); }\
	TextVertAlign GetLabelVertAlign() const { if ( member ) return member->GetVertAlign(); return TVA_TOP; }\
	void SetLabelHorzAlign( TextAlignType align ) { if ( member ) member->SetHorzAlign( align ); }\
	TextAlignType GetLabelHorzAlign() const { if ( member ) return member->GetHorzAlign(); return TAT_CENTER; }\
	void SetLabelTextStyle( TextStyle style ) { if ( member ) member->SetStyle( style ); }\
	TextStyle GetLabelTextStyle() const { if ( member ) return member->GetTextStyle(); return TS_NORMAL; }\
	void SetLabelTextSize( int size ) { if ( member ) member->SetSize( size ); }\
	int	GetLabelTextSize() const { if ( member ) return member->GetSize(); return 0; }\
	void SetLabelTextAlpha( int alpha ) { if ( member ) member->SetAlpha( alpha ); }\
	int GetLabelTextAlpha() const { if ( member ) return member->GetAlpha(); return 0; }\
	void SetLabelTextDisableStyle( TextStyle style ) { if ( member ) member->SetDisableStyle( style ); }\
	TextStyle GetLabelTextDisableStyle() const { if ( member ) return member->GetDisableTextStyle(); return TS_NORMAL; }\
	void SetLabelTextGap( int gap ) { if ( member ) member->SetVertGap( gap ); }\
	int GetLabelTextGap() const { if ( member ) return member->GetVertGap(); return 0; }\
	void SetLabelPrintTextStyle( ioUITitle::PrintTextStyle style ) { if ( member ) member->SetPrintTextStyle( style ); } \
	ioUITitle::PrintTextStyle GetLabelPrintTextStyle() const { if ( member ) return member->GetPrintTextStyle(); return ioUITitle::PTS_NORMAL; }\
	void SetLabelPrintWidthCutSize( float size ) { if ( member ) member->SetWidthCutSize( size ); }\
	float GetLabelPrintWidthCutSize() const { if ( member ) return member->GetWidthCutSize(); return 0; }\
	void SetLabelText( const char* text, const char* delims = "#" ) { if ( member ) member->SetText( text, delims ); }\
	const char* GetLabelText() const { if ( member ) return member->GetText(); return 0; }


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioLabelWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioLabelWndEX : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioLabelWndEX();
	ioLabelWndEX( IOWNDEX_TYPE type );
	virtual ~ioLabelWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void				OnRender();
	virtual void				SetWindowAlpha( int alpha );

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	DECLARE_IOUITITLE_INTERFACES( m_label )
	//
	void						SetAlignStyle( ALIGN_STYLE_EX style ) { m_alignstyle = style; }
	ALIGN_STYLE_EX				GetAlignStyle() const { return m_alignstyle; }
	//
	void						SetLabelOffsetX( int x ) { m_labeloffsetx = x; }
	int							GetLabelOffsetX() const { return m_labeloffsetx; }
	void						SetLabelOffsetY( int y ) { m_labeloffsety = y; }
	int							GetLabelOffsetY() const { return m_labeloffsety; }
	//
	void						SetLabelTextColor( DWORD color );
	DWORD						GetLabelTextColor() const;
	//
	void						SetLabelTextBKColor( DWORD color );
	DWORD						GetLabelTextBKColor() const;

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void				_ParseExtraInfo( ioXMLElement& extxml );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( LabelWnd, ioLabelWndEX )
	virtual ioWnd*				_Clone() { return new ioLabelWndEX(); }
	virtual void				Copy( ioWnd* wnd );
	virtual void				_SetSaveData( ioXMLElement& extxml );

protected:

	ioUITitle*					m_label;

	ALIGN_STYLE_EX				m_alignstyle;
	int							m_labeloffsetx;
	int							m_labeloffsety;
};


#endif // _IOLABELWNDEX_H_