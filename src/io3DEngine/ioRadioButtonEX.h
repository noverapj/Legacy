//----------------------------------------------------------------------------------
/*
Filename	: ioRadioButtonEX.h
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#ifndef _IORADIOBUTTONEX_H_
#define _IORADIOBUTTONEX_H_

#include "ioWnd.h"
#include "ioButtonWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRadioWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioRadioWndEX : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRadioWndEX();
	ioRadioWndEX( IOWNDEX_TYPE type );
	virtual ~ioRadioWndEX();

	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void			iwm_show();

	//----------------------------------------------------------------------------------
	/*
	callback
	*/
	void					OnTabClicked( ioButtonWndEX* btn );
	virtual void			OnTabClickedActive( ioButtonWndEX* btn, int idx );
	virtual void			OnTabClickedUnActive( ioButtonWndEX* btn );
	
	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void					SetCheck( int index, bool forced );
	int						GetCheck() const { return m_checkindex; }
	virtual bool			IsControlType( IOWNDEX_TYPE type ) const; // 해당 type이 내가 컨트롤 하는 type인지 확인해준다.

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( RadioWndEX, ioRadioWndEX )
	virtual ioWnd*			_Clone() { return new ioRadioWndEX(); }

protected:

	int						m_checkindex;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRadioFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioRadioFrameButtonEX : public ioFrameButtonWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRadioFrameButtonEX( ioGUIManager* mgr );
	ioRadioFrameButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioRadioFrameButtonEX() {}

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void			SetButtonEXType( BUTTONEX_TYPE type ) {}
	
	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void			iwm_lbuttondown( const ioMouse& mouse );	

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( RadioFrameButtonEX, ioRadioFrameButtonEX )
	virtual ioWnd*			_Clone() { return new ioRadioFrameButtonEX( m_guimgr ); }
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRadioImageButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioRadioImageButtonEX : public ioImageButtonWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioRadioImageButtonEX( ioGUIManager* mgr );
	ioRadioImageButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr );
	virtual ~ioRadioImageButtonEX() {}

	//----------------------------------------------------------------------------------
	/*
	ioWnd Override
	*/
	virtual void			SetButtonEXType( BUTTONEX_TYPE type ) {}
	
	//----------------------------------------------------------------------------------
	/*
	ioWnd Event Override
	*/
	virtual void			iwm_create();
	virtual void			iwm_lbuttondown( const ioMouse& mouse );

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( RadioImageButtonEX, ioRadioImageButtonEX )
	virtual ioWnd*			_Clone() { return new ioRadioImageButtonEX( m_guimgr ); }
};


#endif // _IORADIOBUTTONEX_H_