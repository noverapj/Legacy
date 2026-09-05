
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioFlashPlayer.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioFlashPlayer.h"
#include "../utils/GlobalRef.h"
#include "ioFlashPlayer.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioFlashPlayer
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioFlashPlayer::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "FlashPlayer" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "SWF_FileName" ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "AlphaMode" ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "Loop" ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "Color", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( categoryWnd, new wxColourProperty( "BKColor", wxPG_LABEL, *wxWHITE ) );
		grid->AppendIn( categoryWnd, new wxBoolProperty( "PlayTest" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioFlashPlayer::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioFlashPlayer* fplayer = dynamic_cast<ioFlashPlayer*>( wnd );
	if ( !grid || !fplayer ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, fplayer );

	grid->SetValue<const char*>( "SWF_FileName", fplayer->GetSWFFileName() );
	grid->SetValue<bool>( "AlphaMode", fplayer->IsFlashAlphaMode() );
	grid->SetValue<bool>( "Loop", fplayer->IsFlashLoop() );
	grid->SetColorValue( "Color", ConvertToToolColor( fplayer->GetFrontColor() ) );
	grid->SetColorValue( "BKColor", ConvertToToolColor( fplayer->GetFlashBKColor() ) );
	grid->SetValue<bool>( "PlayTest", fplayer->IsPlaying() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioFlashPlayer::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioFlashPlayer* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioFlashPlayer>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	if ( keyname == "SWF_FileName" ) 
	{
		outwnd->Load( event.GetValue().GetString().c_str() );
		outwnd->Stop();
		grid->SetValue<bool>( "PlayTest", false );
	} 
	else if ( keyname == "AlphaMode" ) {
		outwnd->SetFlashAlphaMode( event.GetValue().GetBool() );
	} else if ( keyname == "Loop" ) {
		outwnd->SetFlashLoop( event.GetValue().GetBool() );
	} else if ( keyname == "Color" ) {
		outwnd->SetFrontColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	} else if ( keyname == "BKColor" ) {
		outwnd->SetFlashBKColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}
	else if ( keyname == "PlayTest" ) 
	{
		//if ( event.GetValue().GetBool() ) {
		//	outwnd->Play();
		//} else {
		//	outwnd->Stop();
		//}
	}
}