//----------------------------------------------------------------------------------
/*
Filename	: ioScrollBarEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOSCROLLBAREX_H_
#define _IOSCROLLBAREX_H_

#include "ioFrameWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioScrollBarEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioFrameButtonWndEX;
class __EX ioScrollBarEX : public ioFrameWndEX
{
public:

	enum SCROLL_TYPE
	{		
		ST_VERT,
		ST_HORZ,
	};

	enum SCROLL_CHILD_ID
	{		
		SCI_DECBTN	= 1,
		SCI_INCBTN	= 2,
		SCI_SCROLL	= 3,
	};

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioScrollBarEX( ioGUIManager* mgr );
	ioScrollBarEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioScrollBarEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void			OnRender();
	virtual void			OnProcess( float fTimePerSec );
	virtual void			AddChild( ioWnd* wnd );

	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void			iwm_create();
	virtual void			iwm_destroy();

	//----------------------------------------------------------------------------------
	/*
	event func
	*/
	void					OnClickDecrease( ioWndEX* wnd, ioWndEXMouseEvent& event );
	void					OnClickIncrease( ioWndEX* wnd, ioWndEXMouseEvent& event );
	void					OnLDownScrollBtn( ioWndEX* wnd, ioWndEXMouseEvent& event );

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void					SetScrollType( SCROLL_TYPE type ) { m_scrolltype = type; }
	SCROLL_TYPE				GetScrollType() const { return m_scrolltype; }
	void					SetScrollPos( float pos );
	float					GetScrollPos() const { return m_scrollpos; }
	void					SetScrollRange( int range );
	int						GetScrollRange() const { return m_scrollrange; }
	// scroll btn
	void					SetScroll( const char* templatename );
	void					SetScrollWidth( int w );
	void					SetScrollHeight( int h );
	ioFrameWndEX*			GetScrollImg() { return m_scroll; }
	ioFrameButtonWndEX*		GetDecreaseBtn() { return m_decreasebtn; }
	ioFrameButtonWndEX*		GetIncreaseBtn() { return m_increasebtn; }
	// dec btn
	void					SetDecBtnTemplateNameNormal( const char* templatename );
	void					SetDecBtnTemplateNameOver( const char* templatename );
	void					SetDecBtnTemplateNameOverAdd( const char* templatename );
	void					SetDecBtnTemplateNamePush( const char* templatename );
	void					SetDecBtnWidth( int w );
	void					SetDecBtnHeight( int h );
	void					SetDecBtnLabelText( const char* txt );
	void					SetDecBtnLabelGap( int gap );
	void					SetDecBtnLabelVertAlign( TextVertAlign align );
	void					SetDecBtnLabelHorzAlign( TextAlignType align );
	// inc btn
	void					SetIncBtnTemplateNameNormal( const char* templatename );
	void					SetIncBtnTemplateNameOver( const char* templatename );
	void					SetIncBtnTemplateNameOverAdd( const char* templatename );
	void					SetIncBtnTemplateNamePush( const char* templatename );
	void					SetIncBtnWidth( int w );
	void					SetIncBtnHeight( int h );
	void					SetIncBtnLabelText( const char* txt );
	void					SetIncBtnLabelGap( int gap );
	void					SetIncBtnLabelVertAlign( TextVertAlign align );
	void					SetIncBtnLabelHorzAlign( TextAlignType align );

	//----------------------------------------------------------------------------------
	/*
	ioWnd 재정의 ParseExtraInfo
	*/
	virtual void			ParseExtraInfo( ioXMLElement &extxml );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( ScrollBarEX, ioScrollBarEX )
	virtual void			SetIDEX( DWORD& dwID, bool onlychild );
	virtual ioWnd*			_Clone() { return new ioScrollBarEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );

protected:

	SCROLL_TYPE				m_scrolltype;
	float					m_scrollpos;
	int						m_scrollrange;
	bool					m_scrolling;
	POINT					m_scrolledcursorpos;

	ioFrameWndEX*			m_scroll;
	ioFrameButtonWndEX*		m_decreasebtn;
	ioFrameButtonWndEX*		m_increasebtn;	
};

#endif // _IOSCROLLBAREX_H_