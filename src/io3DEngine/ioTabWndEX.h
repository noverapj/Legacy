//----------------------------------------------------------------------------------
/*
Filename	: ioTabWndEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IOTABWNDEX_H_
#define _IOTABWNDEX_H_

#include "ioWndEX.h"
#include "ioRadioButtonEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioTabWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioTabWndEX : public ioRadioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioTabWndEX( ioGUIManager* mgr );
	ioTabWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioTabWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void			AddChild( ioWnd* child );
	virtual void			DeleteChild( DWORD id );
	virtual void			OnRender();
	virtual void			SetWindowAlpha( int alpha );	

	//----------------------------------------------------------------------------------
	/*
	callback
	*/
	void					OnResizeTabBtn( ioButtonWndEX* btn );
	virtual void			OnTabClickedActive( ioButtonWndEX* btn, int idx );
	virtual void			OnTabClickedUnActive( ioButtonWndEX* btn );	

	//----------------------------------------------------------------------------------
	/*
	control
	*/
	virtual bool			IsControlType( IOWNDEX_TYPE type ) const; // 해당 type이 내가 컨트롤 하는 type인지 확인해준다.
	void					LayoutTabButton();
	void					SetDotGap( int gap ) { m_dotgap = gap; LayoutTabButton(); }
	int						GetDotGap() const { return m_dotgap; }
	void					SetDotHeight( int h ) { m_dotheight = h; LayoutTabButton(); }
	int						GetDotHeight() const { return m_dotheight; }
	DECLARE_IMAGE_INTERFACES( Dot, m_dot )
 
	//----------------------------------------------------------------------------------
	/*
	불러오기 가상함수
	*/
	virtual void			_ParseExtraInfo( ioXMLElement& extxml );	

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( TabWndEX, ioTabWndEX )
	virtual ioWnd*			_Clone() { return new ioTabWndEX( m_guimgr ); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );

protected:

	ioGUIManager*			m_guimgr;
	ioUIRenderImage*		m_dot;	

	int						m_dotgap;
	int						m_dotheight;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioTabFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioTabFrameButtonEX : public ioRadioFrameButtonEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioTabFrameButtonEX( ioGUIManager* mgr );
	ioTabFrameButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioTabFrameButtonEX() {}

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void			SetPosX( int x );
	virtual void			SetPosY( int y );
	virtual void			SetWidth( int w );
	virtual void			SetHeight( int h );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( TabFrameButtonEX, ioTabFrameButtonEX )
	virtual ioWnd*			_Clone() { return new ioTabFrameButtonEX( m_guimgr ); }
};

#endif // _IOTABWNDEX_H_