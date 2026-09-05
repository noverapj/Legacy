
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioTabWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioTabWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioTabWndEX.h"
#include "../wxRichTextEditorFrame.h"
#include "PropertyControllerioButtonEX.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioTabWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioTabWndEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	PropertyControllerioWndEX::Setup( grid );

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "TabWnd" ) );
	{
		PropertyControllerioImageButtonEX::SetupImage( this, grid, categoryWnd, "Dot", true );
		grid->AppendIn( categoryWnd, new wxIntProperty( "DotGap" ) );
		grid->AppendIn( categoryWnd, new wxIntProperty( "DotHeight" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, PropertyControllerioWnd::st_uniquebackcolor );
}

void	PropertyControllerioTabWndEX::SetupEventTab( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}
	
	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "TabWnd" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnTab" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, st_uniquebackcolor );

	PropertyControllerioWndEX::SetupEventTab( grid );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioTabWndEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioTabWndEX* twnd = dynamic_cast<ioTabWndEX*>( wnd );
	if ( !grid || !twnd ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, twnd );

	SYNC_IMAGE( grid, twnd, Dot )

	grid->SetValue<int>( "DotGap", twnd->GetDotGap() );
	grid->SetValue<int>( "DotHeight", twnd->GetDotHeight() );
}

void	PropertyControllerioTabWndEX::SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioTabWndEX* wndex = dynamic_cast<ioTabWndEX*>( wnd );
	if ( !grid || !wndex ) {
		return;
	}

	PropertyControllerioWndEX::Sync( grid, wndex );

	grid->SetValue<const char*>( "OnTab", wndex->GetConnectFunctionName( WEVT_TABCHECK ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioTabWndEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChange( grid, wnd, event );
	
	ioTabWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioTabWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	CHECK_IMAGE( grid, outwnd, Dot )

	if ( keyname == "DotGap" ) {
		outwnd->SetDotGap( event.GetValue().GetInteger() );
	} else if ( keyname == "DotHeight" ) {
		outwnd->SetDotHeight( event.GetValue().GetInteger() );
	}
}

void	PropertyControllerioTabWndEX::OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioWndEX::OnChangeEventTab( grid, wnd, event );

	ioTabWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioTabWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( false ) {}
	SET_CONNECTFUNCTION_NAME( "OnTab", WEVT_TABCHECK )
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioTabWndEX::OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioTabWndEX* wnd = dynamic_cast<ioTabWndEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}

	if ( false ) {}
	SET_IMAGESETNAME( wnd, Dot )
	else {
		return;
	}

	wxRichTextEditorFrame::GetSingleton().SyncProperty();
}