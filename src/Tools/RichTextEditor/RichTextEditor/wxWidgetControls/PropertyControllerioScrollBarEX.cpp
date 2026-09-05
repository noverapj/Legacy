
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioScrollBarEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioScrollBarEX.h"
#include "../utils/GlobalRef.h"
#include "ioScrollBarEX.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioScrollBarEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioScrollBarEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}
	
	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "ScrollBar" ) );
	{
		wxArrayString sitem;
		sitem.Add( "Vert" );
		sitem.Add( "Horz" );

		wxArrayInt nitem;
		nitem.Add( ioScrollBarEX::ST_VERT );
		nitem.Add( ioScrollBarEX::ST_HORZ );

		grid->AppendIn( categoryWnd, new wxEnumProperty( "ScrollType", *wxPGProperty::sm_wxPG_LABEL, sitem, nitem ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "ScrollRange" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );

	PropertyControllerioFrameWndEX::Setup( grid );
}

void	PropertyControllerioScrollBarEX::SetupEventTab( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}
	
	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "ScrollBar" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnClickDecrease" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnClickIncrease" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnScroll" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, st_uniquebackcolor );

	PropertyControllerioFrameWndEX::SetupEventTab( grid );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioScrollBarEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioScrollBarEX* scrollwnd = dynamic_cast<ioScrollBarEX*>( wnd );
	if ( !grid || !scrollwnd ) {
		return;
	}

	PropertyControllerioFrameWndEX::Sync( grid, scrollwnd );
	
	grid->SetValue<int>( "ScrollType", scrollwnd->GetScrollType() );
	grid->SetValue<int>( "ScrollRange", scrollwnd->GetScrollRange() );
}

void	PropertyControllerioScrollBarEX::SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioScrollBarEX* wndex = dynamic_cast<ioScrollBarEX*>( wnd );
	if ( !grid || !wndex ) {
		return;
	}

	PropertyControllerioFrameWndEX::Sync( grid, wndex );

	grid->SetValue<const char*>( "OnClickDecrease", wndex->GetConnectFunctionName( WEVT_SB_CLICK_DECREASE ) );
	grid->SetValue<const char*>( "OnClickIncrease", wndex->GetConnectFunctionName( WEVT_SB_CLICK_INCREASE ) );
	grid->SetValue<const char*>( "OnScroll", wndex->GetConnectFunctionName( WEVT_SB_SCROLL ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioScrollBarEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioFrameWndEX::OnChange( grid, wnd, event );
	
	ioScrollBarEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioScrollBarEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "ScrollType" ) {
		outwnd->SetScrollType( static_cast<ioScrollBarEX::SCROLL_TYPE>( event.GetValue().GetInteger() ) );
	} else if ( keyname == "ScrollRange" ) {
		outwnd->SetScrollRange( event.GetValue().GetInteger() );
	}
}

void	PropertyControllerioScrollBarEX::OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioFrameWndEX::OnChangeEventTab( grid, wnd, event );

	ioScrollBarEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioScrollBarEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( false ) {}
	SET_CONNECTFUNCTION_NAME( "OnClickDecrease", WEVT_SB_CLICK_DECREASE )
	SET_CONNECTFUNCTION_NAME( "OnClickIncrease", WEVT_SB_CLICK_INCREASE )
	SET_CONNECTFUNCTION_NAME( "OnScroll", WEVT_SB_SCROLL )
}