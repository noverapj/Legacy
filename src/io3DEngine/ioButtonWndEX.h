//----------------------------------------------------------------------------------
/*
Filename	: ioButtonWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOBUTTONWNDEX_H_
#define _IOBUTTONWNDEX_H_

#include "ioWndEX.h"
#include "ioFrameWndEX.h"
#include "ioImageWndEX.h"
#include "ioLabelWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioButtonWndEX : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioButtonWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void				OnRender();
	virtual void				SetSize( int width, int height );
	virtual void				CheckFrameReSize( int width, int height, int prevwidth, int prevheight );
	virtual DWORD				GetWndType() const { return IWT_BUTTON; }
	virtual void				SetWindowAlpha( int alpha );

	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void				iwm_create();
	virtual void				iwm_lbuttondown( const ioMouse& mouse );
	virtual void				iwm_mouseover( const ioMouse& mouse );

	//----------------------------------------------------------------------------------
	/*
	Draw virtual
	*/	
	virtual void				DrawNormal( int x, int y );
	virtual void				DrawOvered( int x, int y );
	virtual void				DrawOveredAdd( int x, int y );
	virtual void				DrawPushed( int x, int y );
	virtual void				DrawDisabled( int x, int y );
	virtual void				DrawExDisabledNormal( int x, int y );
	virtual void				DrawExDisabledOvered( int x, int y );
	virtual void				DrawExDisabledPushed( int x, int y );
	virtual void				DrawBack( int x, int y );
	
	//----------------------------------------------------------------------------------
	/*
	Text Print
	*/
	virtual void				PrintNormalTitle( int x, int y );
	virtual void				PrintOverTitle( int x, int y );
	virtual void				PrintPushTitle( int x, int y );
	virtual void				PrintDisableTitle( int x, int y );

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	void						SetHelpText( const char* text ) { if ( text ) m_helptext = text; }
	const char*					GetHelpText() const { return m_helptext.c_str(); }
	//
	void						SetSoundOver( const char* str ) { if ( str ) m_soundover = str; }
	const char*					GetSoundOver() const { return m_soundover.c_str(); }
	void						SetSoundClick( const char* str ) { if ( str ) m_soundclick = str; }
	const char*					GetSoundClick() const { return m_soundclick.c_str(); }
	//
	DECLARE_IOUITITLE_INTERFACES( m_label )
	//
	void						SetLabelNormalTextColor( DWORD color ) { if ( m_label ) m_label->SetNormalTitleTextColor( color ); }
	DWORD						GetLabelNormalTextColor() const { if ( m_label ) return m_label->GetNormalTitleColor().dwText; return 0xFFFFFFFF; }
	void						SetLabelOverTextColor( DWORD color ) { if ( m_label ) m_label->SetOverTitleTextColor( color ); }
	DWORD						GetLabelOverTextColor() const { if ( m_label ) return m_label->GetOverTitleColor().dwText; return 0xFFFFFFFF; }
	void						SetLabelPushTextColor( DWORD color ) { if ( m_label ) m_label->SetPushTitleTextColor( color ); }
	DWORD						GetLabelPushTextColor() const { if ( m_label ) return m_label->GetPushTitleColor().dwText; return 0xFFFFFFFF; }
	void						SetLabelDisableTextColor( DWORD color ) { if ( m_label ) m_label->SetDisableTitleTextColor( color ); }
	DWORD						GetLabelDisableTextColor() const { if ( m_label ) return m_label->GetDisableTitleColor().dwText; return 0xFFFFFFFF; }
	//
	void						SetLabelNormalTextBKColor( DWORD color ) { if ( m_label ) m_label->SetNormalTitleBKColor( color ); }
	DWORD						GetLabelNormalTextBKColor() const { if ( m_label ) return m_label->GetNormalTitleColor().dwBk; return 0xFFFFFFFF; }
	void						SetLabelOverTextBKColor( DWORD color ) { if ( m_label ) m_label->SetOverTitleBKColor( color ); }
	DWORD						GetLabelOverTextBKColor() const { if ( m_label ) return m_label->GetOverTitleColor().dwBk; return 0xFFFFFFFF; }
	void						SetLabelPushTextBKColor( DWORD color ) { if ( m_label ) m_label->SetPushTitleBKColor( color ); }
	DWORD						GetLabelPushTextBKColor() const { if ( m_label ) return m_label->GetPushTitleColor().dwBk; return 0xFFFFFFFF; }
	void						SetLabelDisableTextBKColor( DWORD color ) { if ( m_label ) m_label->SetDisableTitleBKColor( color ); }
	DWORD						GetLabelDisableTextBKColor() const { if ( m_label ) return m_label->GetDisableTitleColor().dwBk; return 0xFFFFFFFF; }
	//
	virtual void				SetButtonEXType( BUTTONEX_TYPE type ) { m_btntype = type; }
	BUTTONEX_TYPE				GetButtonEXType() const { return m_btntype; }
	void						SetToggle( bool toggle ) { m_toggled = toggle; }
	bool						IsToggled() const { return m_toggled; }

	//----------------------------------------------------------------------------------
	/*
	Get State
	*/
	inline bool					IsOvered() const { if ( m_bOver && !m_bClicked ) return true; return false; }
	inline bool					IsPushed() const { return m_bClicked; }
	inline bool					IsDisabled() const { if ( m_dwStyle & IWS_INACTIVE ) return true; return false; }
	inline bool					IsExDisabled() const { if ( m_dwStyle & IWS_EXACTIVE ) return true; return false; }

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void				_ParseExtraInfo( ioXMLElement& extxml );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	virtual void				Copy( ioWnd* wnd );
	virtual void				_SetSaveData( ioXMLElement& extxml );
	
protected:

	BUTTONEX_TYPE				m_btntype;

	ioGUIManager*				m_guimgr;

	ioUITitle*					m_label;

	// 자주 사용됨
	ioUIRenderElement*			m_normal;
	ioUIRenderElement*			m_over;
	ioUIRenderElement*			m_overadd;
	ioUIRenderElement*			m_push;
	// 자주 사용 안됨
	ioUIRenderElement*			m_back;
	ioUIRenderElement*			m_disable;
	ioUIRenderElement*			m_exdisable_normal;
	ioUIRenderElement*			m_exdisable_push;

	bool						m_toggled;

	ioHashString				m_helptext;
	ioHashString				m_soundover;
	ioHashString				m_soundclick;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioFrameButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioFrameButtonWndEX : public ioButtonWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioFrameButtonWndEX( ioGUIManager* mgr );
	ioFrameButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioFrameButtonWndEX();

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	DECLARE_FRAME_INTERFACES( Normal, m_normal )
	DECLARE_FRAME_INTERFACES( Over, m_over )
	DECLARE_FRAME_INTERFACES( OverAdd, m_overadd )
	DECLARE_FRAME_INTERFACES( Push, m_push )
	DECLARE_FRAME_INTERFACES( Back, m_back )
	DECLARE_FRAME_INTERFACES( Disable, m_disable )
	DECLARE_FRAME_INTERFACES( ExDisableNormal, m_exdisable_normal )
	DECLARE_FRAME_INTERFACES( ExDisablePush, m_exdisable_push )

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( FrameButton, ioFrameButtonWndEX )
	virtual ioWnd*			_Clone() { return new ioFrameButtonWndEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	편의
	*/
	static void				SetFrameDataToXML( ioUIRenderElement* elem, ioXMLElement& extxml, const char* tagname );
	static void				SetFrameDataFromXML( ioUIRenderElement** element, ioXMLElement& extxml, const char* tagname, ioGUIManager* mgr );
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioImageButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioImageButtonWndEX : public ioButtonWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioImageButtonWndEX( ioGUIManager* mgr );
	ioImageButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioImageButtonWndEX();

	//----------------------------------------------------------------------------------
	/*
	Controls
	*/
	void					SetImageSetAll( const char* imagesetname, const char* imagename, ioGUIManager* mgr );
	void					SetImageSetAll( const char* fullname );
	//
	DECLARE_IMAGE_INTERFACES( Normal, m_normal )
	DECLARE_IMAGE_INTERFACES( Over, m_over )
	DECLARE_IMAGE_INTERFACES( OverAdd, m_overadd )
	DECLARE_IMAGE_INTERFACES( Push, m_push )
	DECLARE_IMAGE_INTERFACES( Back, m_back )
	DECLARE_IMAGE_INTERFACES( Disable, m_disable )
	DECLARE_IMAGE_INTERFACES( ExDisableNormal, m_exdisable_normal )
	DECLARE_IMAGE_INTERFACES( ExDisablePush, m_exdisable_push )

	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( ImageButton, ioImageButtonWndEX )
	virtual ioWnd*			_Clone() { return new ioImageButtonWndEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	편의
	*/
	static void				SetImageDataToXML( ioUIRenderElement* elem, ioXMLElement& extxml, const char* tagname );
	static void				SetImageDataFromXML( ioUIRenderElement** element, ioXMLElement& extxml, const char* tagname, ioGUIManager* mgr );
};

#endif // _IOBUTTONWNDEX_H_