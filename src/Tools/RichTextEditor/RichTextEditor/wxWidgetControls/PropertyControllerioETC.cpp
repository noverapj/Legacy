
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioETC.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioETC.h"
#include "../utils/GlobalRef.h"
#include "ioWnd.h"
#include "ioProgressBar.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioProgressBar
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioProgressBar::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWnd::Setup( grid );

	wxArrayString items;
	wxArrayInt itemsv;
	items.Add( "Left" );
	items.Add( "Right" );
	itemsv.Add( 0 );
	itemsv.Add( 1 );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "ProgressBar" ) );
	{
		grid->AppendIn( categoryWnd, new wxColourProperty( "GraphColor", wxPG_LABEL, PropertyControllerioWnd::st_textdefcolor ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "MaxPos" ) );
		grid->AppendIn( categoryWnd, new wxEnumProperty( "ProgressAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioProgressBar::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioProgressBar* pro = dynamic_cast<ioProgressBar*>( wnd );
	if ( !grid || !pro ) {
		return;
	}

	PropertyControllerioWnd::Sync( grid, wnd );

	grid->SetColorValue( "GraphColor", ConvertToToolColor( pro->GetGraphColor() ) );
	grid->SetValue<int>( "MaxPos", pro->GetMaxPos() );
	grid->SetValue<int>( "ProgressAlign", pro->GetProgressAlign() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioProgressBar::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWnd::OnChange( grid, wnd, event );

	ioProgressBar* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioProgressBar>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "GraphColor" ) {
		outwnd->SetGraphColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "MaxPos" ) {
		outwnd->SetMaxPosition( event.GetValue().GetInteger() );
	} else if ( keyname == "ProgressAlign" ) {
		outwnd->SetProgressAlign( static_cast<ioProgressBar::ProgressAlign>( event.GetValue().GetInteger() ) );
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioEdit
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------