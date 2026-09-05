
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioRadioWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioRadioWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioTabWndEX.h"
#include "../wxRichTextEditorFrame.h"
#include "PropertyControllerioButtonEX.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioRadioWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioRadioWndEX::SetupEventTab( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}
	
	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "RadioWnd" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnRadio" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, st_uniquebackcolor );

	PropertyControllerioWndEX::SetupEventTab( grid );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioRadioWndEX::SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioRadioWndEX* wndex = dynamic_cast<ioRadioWndEX*>( wnd );
	if ( !grid || !wndex ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, wndex );

	grid->SetValue<const char*>( "OnRadio", wndex->GetConnectFunctionName( WEVT_RADIOCHECK ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioRadioWndEX::OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChangeEventTab( grid, wnd, event );

	ioRadioWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioRadioWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( false ) {}
	SET_CONNECTFUNCTION_NAME( "OnRadio", WEVT_RADIOCHECK )
}