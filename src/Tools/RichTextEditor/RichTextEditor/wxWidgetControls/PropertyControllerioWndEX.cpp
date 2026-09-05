
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioWndEX.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioWndEX.h"
#include "../utils/GlobalRef.h"
#include "ioWndEX.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioWndEX::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "Common" ) );
	{
		wxPGProperty* pgwndid = grid->AppendIn( categoryWnd, new wxIntProperty( "WndID" ) );
		if ( pgwndid ) {
			pgwndid->Enable( false );
		}
		grid->AppendIn( categoryWnd, new wxStringProperty( "ID" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "Member" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "ClassName" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "SubClassName" ) );

		wxPGProperty* pgsize = grid->AppendIn( categoryWnd, new wxPropertyCategory( "Size" ) );		
		{
			grid->AppendIn( pgsize, new wxIntProperty( "X" ) );
			grid->AppendIn( pgsize, new wxIntProperty( "Y" ) );
			grid->AppendIn( pgsize, new wxIntProperty( "Width" ) );
			grid->AppendIn( pgsize, new wxIntProperty( "Height" ) );
			grid->AppendIn( pgsize, new wxBoolProperty( "CheckParentOutSide" ) );
		}
		grid->SetPropertyBackgroundColour( pgsize, st_backdefcolor );
		
		wxPGProperty* pgalign = grid->AppendIn( categoryWnd, new wxPropertyCategory( "Align" ) );
		{
			wxArrayString items;
			wxArrayInt itemsv;
			
			for ( int i = 0; i < ioWnd::UI_TOP_ALIGN; ++i )
			{
				items.Add( g_enginewndalign[ i ] );
				itemsv.Add( i );
			}
			grid->AppendIn( pgalign, new wxEnumProperty( "HorzAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );

			items.Clear();
			itemsv.Clear();
			for ( int i = ioWnd::UI_CENTER_ALIGN; i < ioWnd::UI_NONE_ALIGN; ++i )
			{
				items.Add( g_enginewndalign[ i ] );
				itemsv.Add( i );
			}
			grid->AppendIn( pgalign, new wxEnumProperty( "VertAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );
		}
		grid->SetPropertyBackgroundColour( pgalign, st_backdefcolor );
		
		wxPGProperty* pgstyle = grid->AppendIn( categoryWnd, new wxPropertyCategory( "Style" ) );
		{
			for ( int i = 0; i < IWS_NUM; ++i ) {
				AppendInBoolPropertyCheckbox( grid, pgstyle, g_enginewindowstyletext[ i ] );
			}
		}
		grid->Collapse( pgstyle );
		grid->SetPropertyBackgroundColour( pgstyle, st_backdefcolor );

		wxPGProperty* pgstyleex = grid->AppendIn( categoryWnd, new wxPropertyCategory( "StyleEX" ) );
		{
			for ( int i = 0; i < IWS_EX_NUM; ++i ) {
				AppendInBoolPropertyCheckbox( grid, pgstyleex, g_enginewindowexstyletext[ i ] );
			}
		}
		grid->Collapse( pgstyleex );
		grid->SetPropertyBackgroundColour( pgstyleex, st_backdefcolor );

		wxPGProperty* pgtooltip = grid->AppendIn( categoryWnd, new wxPropertyCategory( "ToolTip" ) );
		{
			grid->AppendIn( pgtooltip, new wxStringProperty( "ToolTipText" ) );
			grid->AppendIn( pgtooltip, new wxEnumProperty( "ToolTipAlign", *wxPGProperty::sm_wxPG_LABEL, GetHorzAlignString(), GetHorzAlignInt() ) );
			grid->AppendIn( pgtooltip, new wxColourProperty( "ToolTipTextColor", wxPG_LABEL, st_textdefcolor ) );
		}
		grid->Collapse( pgtooltip );
		grid->SetPropertyBackgroundColour( pgtooltip, st_backdefcolor );

		wxPGProperty* pgdragrect = grid->AppendIn( categoryWnd, new wxPropertyCategory( "DragRect" ) );
		{
			grid->AppendIn( pgdragrect, new wxIntProperty( "DR_Left" ) );
			grid->AppendIn( pgdragrect, new wxIntProperty( "DR_Top" ) );
			grid->AppendIn( pgdragrect, new wxIntProperty( "DR_Right" ) );
			grid->AppendIn( pgdragrect, new wxIntProperty( "DR_Bottom" ) );
		}
		grid->Collapse( pgdragrect );
		grid->SetPropertyBackgroundColour( pgdragrect, st_backdefcolor );
	}
}

void	PropertyControllerioWndEX::SetupEventTab( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "WndEX" ) );
	{
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnCreate" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnDestroy" ) );		
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnShow" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnHide" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnESC" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnSpaceBar" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnLButtonDown" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnLButtonUp" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnLButtonDBClick" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnRButtonDown" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnRButtonUp" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnMouseOver" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnMouseMove" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnMouseLeave" ) );
		grid->AppendIn( categoryWnd, new wxStringProperty( "OnMouseWheel" ) );
	}
	grid->SetPropertyBackgroundColour( categoryWnd, st_backdefcolor );
}

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioWndEX::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
{
	if ( !grid || !wnd ) {
		return;
	}

	// ID
	grid->SetValue<int>( "WndID", wnd->GetID() );
	grid->SetValue<const char*>( "ID", wnd->GetIDName() );
	grid->SetValue<const char*>( "Member", wnd->GetMemberName() );
	grid->SetValue<const char*>( "ClassName", wnd->GetClassNameEX() );
	grid->SetValue<const char*>( "SubClassName", wnd->GetSubClassName() );
	// size
	grid->SetValue<int>( "X", wnd->GetPosX() );
	grid->SetValue<int>( "Y", wnd->GetPosY() );
	grid->SetValue<int>( "Width", wnd->GetDefWidth() );
	grid->SetValue<int>( "Height", wnd->GetDefHeight() );
	grid->SetValue<bool>( "CheckParentOutSide", wnd->IsCheckParentOutSide() );
	grid->SetValue<long>( "HorzAlign", wnd->GetHorzAlign() );
	grid->SetValue<long>( "VertAlign", wnd->GetVertAlign() );
	// style
#define STYLE_TO_BOOLEAN(x) (((wnd->GetStyle()&x)!=0x00)?true:false)
	grid->SetValue<bool>( "IWS_START_HIDE", STYLE_TO_BOOLEAN( IWS_START_HIDE ) );
	grid->SetValue<bool>( "IWS_INACTIVE", STYLE_TO_BOOLEAN( IWS_INACTIVE ) );
	grid->SetValue<bool>( "IWS_BACKMOST", STYLE_TO_BOOLEAN( IWS_BACKMOST ) );
	grid->SetValue<bool>( "IWS_CANMOVE", STYLE_TO_BOOLEAN( IWS_CANMOVE ) );
	grid->SetValue<bool>( "IWS_MODAL", STYLE_TO_BOOLEAN( IWS_MODAL ) );
	grid->SetValue<bool>( "IWS_EXIT_ESC", STYLE_TO_BOOLEAN( IWS_EXIT_ESC ) );
	grid->SetValue<bool>( "IWS_PROCESS", STYLE_TO_BOOLEAN( IWS_PROCESS ) );
	grid->SetValue<bool>( "IWS_DRAG_DROP", STYLE_TO_BOOLEAN( IWS_DRAG_DROP ) );
	grid->SetValue<bool>( "IWS_NO_MOUSE_RESPONSE", STYLE_TO_BOOLEAN( IWS_NO_MOUSE_RESPONSE ) );
	grid->SetValue<bool>( "IWS_CHILD_ONLY_RESPONSE", STYLE_TO_BOOLEAN( IWS_CHILD_ONLY_RESPONSE ) );
	grid->SetValue<bool>( "IWS_TOPMOST", STYLE_TO_BOOLEAN( IWS_TOPMOST ) );
	grid->SetValue<bool>( "IWS_EXACTIVE", STYLE_TO_BOOLEAN( IWS_EXACTIVE ) );
	// ex style
#define EXSTYLE_TO_BOOLEAN(x) (((wnd->GetExStyle()&x)!=0x00)?true:false)
	grid->SetValue<bool>( "IWS_EX_EDIT_PW", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_PW ) );
	grid->SetValue<bool>( "IWS_EX_EDIT_RIGHT", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_RIGHT ) );
	grid->SetValue<bool>( "IWS_EX_EDIT_CENTER", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_CENTER ) );
	grid->SetValue<bool>( "IWS_EX_MULTI_EDIT", EXSTYLE_TO_BOOLEAN( IWS_EX_MULTI_EDIT ) );
	grid->SetValue<bool>( "IWS_EX_NOTIFY_MOVED", EXSTYLE_TO_BOOLEAN( IWS_EX_NOTIFY_MOVED ) );
	grid->SetValue<bool>( "IWS_EX_OVER_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_OVER_SCREEN ) );
	grid->SetValue<bool>( "IWS_EX_PUSH_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSH_SCREEN ) );
	grid->SetValue<bool>( "IWS_EX_HSCROLL", EXSTYLE_TO_BOOLEAN( IWS_EX_HSCROLL ) );
	grid->SetValue<bool>( "IWS_EX_VSCROLL", EXSTYLE_TO_BOOLEAN( IWS_EX_VSCROLL ) );
	grid->SetValue<bool>( "IWS_EX_PUSH_DOUBLE", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSH_DOUBLE ) );
	grid->SetValue<bool>( "IWS_EX_EDIT_NUM", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_NUM ) );
	grid->SetValue<bool>( "IWS_EX_EDIT_CHECK_FOCUS", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_CHECK_FOCUS ) );
	grid->SetValue<bool>( "IWS_EX_PUSHED_OVER_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSHED_OVER_SCREEN ) );
	grid->SetValue<bool>( "IWS_EX_SHOW_NO_TOP", EXSTYLE_TO_BOOLEAN( IWS_EX_SHOW_NO_TOP ) );
	grid->SetValue<bool>( "IWS_EX_SHOW_HIDE_ACTION", EXSTYLE_TO_BOOLEAN( IWS_EX_SHOW_HIDE_ACTION ) );
	grid->SetValue<bool>( "IWS_EX_NO_REND_EDIT_NUM", EXSTYLE_TO_BOOLEAN( IWS_EX_NO_REND_EDIT_NUM ) );
	// tooltip
	grid->SetValue<const char*>( "ToolTipText", wnd->GetTooltipText() );
	grid->SetValue<int>( "ToolTipAlign", wnd->GetTooltipAlign() );
	grid->SetColorValue( "ToolTipTextColor", ConvertToToolColor( wnd->GetTooltipColor() ) );
	// rect
	grid->SetValue<int>( "DR_Left", wnd->GetDragRectLeft() );
	grid->SetValue<int>( "DR_Top", wnd->GetDragRectTop() );
	grid->SetValue<int>( "DR_Right", wnd->GetDragRectRight() );
	grid->SetValue<int>( "DR_Bottom", wnd->GetDragRectBottom() );
}

void	PropertyControllerioWndEX::SyncEventTab( wxioPropertyGrid* grid, ioWnd* wnd )
{
	ioWndEX* wndex = dynamic_cast<ioWndEX*>( wnd );
	if ( !grid || !wndex ) {
		return;
	}

	grid->SetValue<const char*>( "OnCreate", wndex->GetConnectFunctionName( WEVT_CREATE ) );
	grid->SetValue<const char*>( "OnDestroy", wndex->GetConnectFunctionName( WEVT_DESTROY ) );
	grid->SetValue<const char*>( "OnShow", wndex->GetConnectFunctionName( WEVT_SHOW ) );
	grid->SetValue<const char*>( "OnHide", wndex->GetConnectFunctionName( WEVT_HIDE ) );
	grid->SetValue<const char*>( "OnESC", wndex->GetConnectFunctionName( WEVT_ESC ) );
	grid->SetValue<const char*>( "OnSpaceBar", wndex->GetConnectFunctionName( WEVT_SPACEBAR ) );
	grid->SetValue<const char*>( "OnLButtonDown", wndex->GetConnectFunctionName( WEVT_LBUTTONDOWN ) );
	grid->SetValue<const char*>( "OnLButtonUp", wndex->GetConnectFunctionName( WEVT_LBUTTONUP ) );
	grid->SetValue<const char*>( "OnLButtonDBClick", wndex->GetConnectFunctionName( WEVT_LBUTTONDBCLICK ) );
	grid->SetValue<const char*>( "OnRButtonDown", wndex->GetConnectFunctionName( WEVT_RBUTTONDOWN ) );
	grid->SetValue<const char*>( "OnRButtonUp", wndex->GetConnectFunctionName( WEVT_RBUTTONUP ) );
	grid->SetValue<const char*>( "OnMouseOver", wndex->GetConnectFunctionName( WEVT_MOUSEOVER ) );
	grid->SetValue<const char*>( "OnMouseMove", wndex->GetConnectFunctionName( WEVT_MOUSEMOVE ) );
	grid->SetValue<const char*>( "OnMouseLeave", wndex->GetConnectFunctionName( WEVT_MOUSELEAVE ) );	
	grid->SetValue<const char*>( "OnMouseWheel", wndex->GetConnectFunctionName( WEVT_WHEEL ) );
}

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioWndEX::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	ioWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

#define COMPARE_AND_SET( comp, func, getfunc ) else if(keyname==comp){outwnd->func(getfunc);return;}
#define COMPARE_AND_ADDSTYLE( style, func, getfunc ) else if(keyname==#style){outwnd->func(style,getfunc);return;}

	if ( false ) {}
	// ID
	COMPARE_AND_SET( "ID", SetIDName, event.GetValue().GetString().c_str() )
	COMPARE_AND_SET( "Member", SetMemberName, event.GetValue().GetString().c_str() )
	COMPARE_AND_SET( "ClassName", SetClassName, event.GetValue().GetString().c_str() )
	COMPARE_AND_SET( "SubClassName", SetSubClassName, event.GetValue().GetString().c_str() )
	// size
	COMPARE_AND_SET( "X", SetPosX, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "Y", SetPosY, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "Width", SetWidth, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "Height", SetHeight, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "CheckParentOutSide", SetCheckParentOutSide, event.GetValue().GetBool() )
	COMPARE_AND_SET( "HorzAlign", SetHorzAlign, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "VertAlign", SetVertAlign, event.GetValue().GetInteger() )
	// style
	if ( false ) {}
	COMPARE_AND_ADDSTYLE( IWS_START_HIDE, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_INACTIVE, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_BACKMOST, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_CANMOVE, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_MODAL, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EXIT_ESC, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_PROCESS, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_DRAG_DROP, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_NO_MOUSE_RESPONSE, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_CHILD_ONLY_RESPONSE, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_TOPMOST, AddStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EXACTIVE, AddStyle, event.GetValue().GetBool() )
	// ex style
	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_PW, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_RIGHT, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_CENTER, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_MULTI_EDIT, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_NOTIFY_MOVED, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_OVER_SCREEN, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_PUSH_SCREEN, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_HSCROLL, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_VSCROLL, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_PUSH_DOUBLE, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_NUM, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_CHECK_FOCUS, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_PUSHED_OVER_SCREEN, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_SHOW_NO_TOP, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_SHOW_HIDE_ACTION, AddExStyle, event.GetValue().GetBool() )
	COMPARE_AND_ADDSTYLE( IWS_EX_NO_REND_EDIT_NUM, AddExStyle, event.GetValue().GetBool() )
	// tooltip
	if ( keyname == "ToolTipText" ) {
		wnd->SetTooltipText( event.GetValue().GetString().c_str() );
	} else if ( keyname == "ToolTipAlign" ) {
		wnd->SetTooltipAlign( event.GetValue().GetInteger() );
	} else if ( keyname == "ToolTipTextColor" ) {
		wnd->SetTooltipColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
	}
	// rect
	if ( false ) {}
	COMPARE_AND_SET( "DR_Left", SetDragRectLeft, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "DR_Top", SetDragRectTop, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "DR_Right", SetDragRectRight, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "DR_Bottom", SetDragRecBottom, event.GetValue().GetInteger() )
}

void	PropertyControllerioWndEX::OnChangeEventTab( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	ioWndEX* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioWndEX>( wnd, &outwnd, keyname, value, event ) ) {
		return;
	}

	if ( false ) {}
	SET_CONNECTFUNCTION_NAME( "OnCreate", WEVT_CREATE )
	SET_CONNECTFUNCTION_NAME( "OnDestroy", WEVT_DESTROY )
	SET_CONNECTFUNCTION_NAME( "OnShow", WEVT_SHOW )
	SET_CONNECTFUNCTION_NAME( "OnHide", WEVT_HIDE )
	SET_CONNECTFUNCTION_NAME( "OnESC", WEVT_ESC )
	SET_CONNECTFUNCTION_NAME( "OnSpaceBar", WEVT_SPACEBAR )
	SET_CONNECTFUNCTION_NAME( "OnLButtonDown", WEVT_LBUTTONDOWN )
	SET_CONNECTFUNCTION_NAME( "OnLButtonUp", WEVT_LBUTTONUP )
	SET_CONNECTFUNCTION_NAME( "OnLButtonDBClick", WEVT_LBUTTONDBCLICK )
	SET_CONNECTFUNCTION_NAME( "OnRButtonDown", WEVT_RBUTTONDOWN )
	SET_CONNECTFUNCTION_NAME( "OnRButtonUp", WEVT_RBUTTONUP )
	SET_CONNECTFUNCTION_NAME( "OnMouseOver", WEVT_MOUSEOVER )
	SET_CONNECTFUNCTION_NAME( "OnMouseLeave", WEVT_MOUSELEAVE )
	SET_CONNECTFUNCTION_NAME( "OnMouseMove", WEVT_MOUSEMOVE )
	SET_CONNECTFUNCTION_NAME( "OnMouseWheel", WEVT_WHEEL )
}