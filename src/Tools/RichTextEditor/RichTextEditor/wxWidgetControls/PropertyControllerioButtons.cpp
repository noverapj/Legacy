
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioButtons.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioButtons.h"
#include "../utils/GlobalRef.h"
#include "ioWnd.h"
#include "ioButton.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioButtons
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioButton::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWnd::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "Button" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "Help" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OverSound" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "ClickSound" ) );		
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_backdefcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioButton::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioButton* btn = dynamic_cast<ioButton*>( wnd );
	if ( !grid || !btn ) {
		return;
	}

	PropertyControllerioWnd::Sync( grid, wnd );

	grid->SetValue<const char*>( "Help", btn->GetHelp() );
	grid->SetValue<const char*>( "OverSound", btn->GetOverSound() );
	grid->SetValue<const char*>( "ClickSound", btn->GetClickSound() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioButton::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWnd::OnChange( grid, wnd, event );

	ioButton* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioButton>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}	

	if ( keyname == "Help" ) {
		outwnd->SetHelp( event.GetValue().GetString().c_str() );
	} else if ( keyname == "OverSound" ) {
		outwnd->SetOverSound( event.GetValue().GetString().c_str() );
	} else if ( keyname == "ClickSound" ) {
		outwnd->SetClickSound( event.GetValue().GetString().c_str() );
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioCheckBox
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioCheckBox::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioButton::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "CheckButton" ) );
	{
		grid->AppendIn( categoryWnd, new wxColourProperty( "UnCheckTextColor", wxPG_LABEL, PropertyControllerioWnd::st_textdefcolor ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "UnCheckBackColor", wxPG_LABEL, PropertyControllerioWnd::st_textdefcolor ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioCheckBox::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioCheckButton* cwnd = dynamic_cast<ioCheckButton*>( wnd );
	if ( !grid || !cwnd ) {
		return;
	}

	grid->SetColorValue( "UnCheckTextColor", ConvertToToolColor( cwnd->GetUnCheckTextColor() ) );
	grid->SetColorValue( "UnCheckBackColor", ConvertToToolColor( cwnd->GetUnCheckBackColor() ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioCheckBox::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioButton::OnChange( grid, wnd, event );
	
	ioCheckButton* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioCheckButton>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( keyname == "UnCheckTextColor" ) {
		outwnd->SetUnCheckTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "UnCheckBackColor" ) {
		outwnd->SetUnCheckBackColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}
}