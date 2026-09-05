//----------------------------------------------------------------------------------
/*
Filename	: ioCheckBoxEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOCHECKBOXEX_H_
#define _IOCHECKBOXEX_H_

#include "ioWnd.h"
#include "ioButtonWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioCheckBoxEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioCheckBoxEX : public ioImageButtonWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioCheckBoxEX( ioGUIManager* mgr );
	ioCheckBoxEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioCheckBoxEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void			OnRender();
	virtual DWORD			GetWndType() const { return IWT_CHECK_BOX; }
	virtual void			SetWindowAlpha( int alpha );

	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void			iwm_lbuttonup( const ioMouse& mouse );

	//----------------------------------------------------------------------------------
	/*
	Virtual
	*/
	virtual void			DrawCheck( int x, int y );
	virtual void			PrintLabel( int x, int y );
	
	//----------------------------------------------------------------------------------
	/*
	ioImageButtonWndEX Text Print
	*/
	virtual void			PrintNormalTitle( int x, int y ) {}
	virtual void			PrintOverTitle( int x, int y ) {}
	virtual void			PrintPushTitle( int x, int y ) {}
	virtual void			PrintDisableTitle( int x, int y ) {}

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void					SetCheck( bool check ) { m_checked = check; }
	bool					IsChecked() const { return m_checked; }
	//
	void					SetLabelOffsetX( int x ) { m_lableoffsetx = x; }
	int						GetLabelOffsetX() const { return m_lableoffsetx; }
	void					SetLabelOffsetY( int y ) { m_lableoffsety = y; }
	int						GetLabelOffsetY() const { return m_lableoffsety; }
	//
	DECLARE_IMAGE_INTERFACES( Check, m_check )
	
	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml );
	
	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( CheckBoxEX, ioCheckBoxEX )
	virtual ioWnd*			_Clone() { return new ioCheckBoxEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );

protected:

	ioUIRenderElement*		m_check;

	int						m_lableoffsetx;
	int						m_lableoffsety;

	bool					m_checked;
};


#endif // _IOCHECKBOXEX_H_