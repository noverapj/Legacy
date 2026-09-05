
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioScrolls.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioScrolls.h"
#include "../utils/GlobalRef.h"
#include "ioWnd.h"
#include "ioButton.h"
#include "ioScroll.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioScroll
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioScroll::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWnd::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "Scroll" ) );
	{
		grid->AppendIn( categoryWnd, new wxIntProperty( "PageRange" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "StartTime" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "CurTime" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioScroll::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioScroll* scr = dynamic_cast<ioScroll*>( wnd );
	if ( !grid || !scr ) {
		return;
	}

	PropertyControllerioWnd::Sync( grid, wnd );

	grid->SetValue<int>( "PageRange", scr->GetPageRange() );
	grid->SetValue<int>( "StartTime", scr->GetStartTime() );
	grid->SetValue<int>( "CurTime", scr->GetCurTime() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioScroll::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWnd::OnChange( grid, wnd, event );

	ioScroll* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioScroll>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "PageRange" ) {
		outwnd->SetPageRange( event.GetValue().GetInteger() );
	} else if ( keyname == "StartTime" ) {
		outwnd->SetStartTime( event.GetValue().GetInteger() );
	} else if ( keyname == "CurTime" ) {
		outwnd->SetCurTime( event.GetValue().GetInteger() );
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioSlide
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
