//----------------------------------------------------------------------------------
/*
Filename	: ioRadioButtonEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioRadioButtonEX.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
/*
持失 / 社瑚
*/
ioRadioWndEX::ioRadioWndEX()
	: ioWndEX( IWEXT_RADIOWND )
	, m_checkindex( -1 )
{
}

ioRadioWndEX::ioRadioWndEX( IOWNDEX_TYPE type )
	: ioWndEX( type )
	, m_checkindex( -1 )
{
}

ioRadioWndEX::~ioRadioWndEX()
{
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioRadioWndEX::iwm_show()
{
	ioWndEX::iwm_show();

	SetCheck( 0, true );
}

//----------------------------------------------------------------------------------
/*
callback
*/
void	ioRadioWndEX::OnTabClicked( ioButtonWndEX* btn )
{
	if ( !btn ) {
		return;
	}

	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( int i = 0; iter != eiter; ++iter )
	{
		ioButtonWndEX* exwnd = dynamic_cast<ioButtonWndEX*>( *iter );
		if ( exwnd )
		{
			if ( exwnd->GetID() == btn->GetID() && IsControlType( btn->GetEXType() ) ) 
			{
				m_checkindex = i;
				OnTabClickedActive( exwnd, m_checkindex );				
			}
			else 
			{
				OnTabClickedUnActive( exwnd );
			}
			++i;
		}
	}
}

void	ioRadioWndEX::OnTabClickedActive( ioButtonWndEX* btn, int idx )
{
	if ( btn )
	{
		btn->SetToggle( true );

		SendEvent<ioWndEXRadioEvent>( WEVT_RADIOCHECK, ioWndEXRadioEvent( btn, idx ) );
	}
}

void	ioRadioWndEX::OnTabClickedUnActive( ioButtonWndEX* btn )
{
	if ( btn ) {
		btn->SetToggle( false );
	}
}

//----------------------------------------------------------------------------------
/*
Control
*/
bool	ioRadioWndEX::IsControlType( IOWNDEX_TYPE type ) const
{
	switch ( type )
	{
	case IWEXT_RADIOFRAMEBUTTON:
	case IWEXT_RADIOIMAGEBUTTON:
		return true;
	}
	return false;
}

void	ioRadioWndEX::SetCheck( int index, bool forced )
{
	if ( !forced ) {
		if ( m_checkindex == index ) {
			return;
		}
	}

	m_checkindex = index;

	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( int cnt = 0; iter != eiter; ++iter )
	{
		ioButtonWndEX* exwnd = dynamic_cast<ioButtonWndEX*>( *iter );
		if ( exwnd )
		{
			if ( cnt == index && IsControlType( exwnd->GetEXType() ) )
			{
				OnTabClicked( exwnd );
				break;
			}
			++cnt;
		}
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRadioFrameButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
持失 / 社瑚
*/
ioRadioFrameButtonEX::ioRadioFrameButtonEX( ioGUIManager* mgr )
	: ioFrameButtonWndEX( IWEXT_RADIOFRAMEBUTTON, mgr ) 
{
	m_btntype = BEXT_RADIO;
}

ioRadioFrameButtonEX::ioRadioFrameButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr ) 
	: ioFrameButtonWndEX( type, mgr )
{
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioRadioFrameButtonEX::iwm_lbuttondown( const ioMouse& mouse ) 
{
	ioFrameButtonWndEX::iwm_lbuttondown( mouse );

	ioRadioWndEX* tabwnd = dynamic_cast<ioRadioWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnTabClicked( this );
	}
}


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRadioImageButtonEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
持失 / 社瑚
*/
ioRadioImageButtonEX::ioRadioImageButtonEX( ioGUIManager* mgr )
	: ioImageButtonWndEX( IWEXT_RADIOIMAGEBUTTON, mgr ) 
{
	m_btntype = BEXT_RADIO;
}

ioRadioImageButtonEX::ioRadioImageButtonEX( IOWNDEX_TYPE type, ioGUIManager* mgr ) 
	: ioImageButtonWndEX( type, mgr )
{
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioRadioImageButtonEX::iwm_create()
{
	ioImageButtonWndEX::iwm_create();

	SetCheckParentOutSide( true );
}

void	ioRadioImageButtonEX::iwm_lbuttondown( const ioMouse& mouse ) 
{
	ioImageButtonWndEX::iwm_lbuttondown( mouse );

	ioRadioWndEX* tabwnd = dynamic_cast<ioRadioWndEX*>( GetParent() );
	if ( tabwnd ) {
		tabwnd->OnTabClicked( this );
	}
}