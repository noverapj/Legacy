
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioCheckBoxEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioCheckBoxEX.h"
#include "../utils/GlobalRef.h"
#include "../wxRichTextEditorFrame.h"
#include "ioCheckBoxEX.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioCheckBoxEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
PropertyControllerioButtonEX override
*/
void	PropertyControllerioCheckBoxEX::SetupPrev( wxioPropertyGrid* grid, wxPGProperty* category )
{
	SetupImage( this, grid, category, "Check", true );
	PropertyControllerioImageButtonEX::SetupPrev( grid, category );

	grid->AppendIn( category, new wxStringProperty( "LabelOffsetX" ) );
	grid->AppendIn( category, new wxStringProperty( "LabelOffsetY" ) );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioCheckBoxEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioCheckBoxEX* check = dynamic_cast<ioCheckBoxEX*>( wnd );
	if ( !grid || !check ) {
		return;
	}

	PropertyControllerioImageButtonEX::Sync( grid, wnd );

	SYNC_IMAGE( grid, check, Check )

	grid->SetValue<int>( "LabelOffsetX", check->GetLabelOffsetX() );
	grid->SetValue<int>( "LabelOffsetY", check->GetLabelOffsetY() );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioCheckBoxEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	PropertyControllerioImageButtonEX::OnChange( grid, wnd, event );
	
	ioCheckBoxEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioCheckBoxEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !guimgr ) {
		return;
	}

	CHECK_IMAGE( grid, outwnd, Check )

	if ( keyname == "LabelOffsetX" ) {
		outwnd->SetLabelOffsetX( event.GetValue().GetInteger() );
	} else if ( keyname == "LabelOffsetY" ) {
		outwnd->SetLabelOffsetY( event.GetValue().GetInteger() );
	}
}

//----------------------------------------------------------------------------------
/*
FrameImageSelectorUser override
*/
void	PropertyControllerioCheckBoxEX::OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata )
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioCheckBoxEX* wnd = dynamic_cast<ioCheckBoxEX*>( wxRichTextEditorFrame::GetSingleton().GetSelectioWnd() );
	if ( !wnd || !guimgr ) {
		return;
	}
	
	if ( false ) {}
	SET_IMAGESETNAME( wnd, Check )

	PropertyControllerioImageButtonEX::OnSelectImageSetNames( imagesetname, imagename, userdata );
}