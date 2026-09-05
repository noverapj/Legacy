
//----------------------------------------------------------------------------------
/*
Filename	: PropertyControllerioWnd.cpp
Author		: megamegy
Date		: 2013.10.22
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyControllerioWnd.h"
#include "../utils/GlobalRef.h"
#include "ioWndEX.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderImageSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
wxRenderImageEditor* wxRenderImageSummonerPropertyEX::m_editor = 0;

bool	wxRenderImageSummonerPropertyEX::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
{
	m_editor = new wxRenderImageEditor( "RenderImageEditor" );
	m_editor->SyncFromEngine();
	m_editor->Show( true );
	return true;
}

wxClassInfo*	wxRenderImageSummonerPropertyEX::GetClassInfo() const
{
	return wxLongStringProperty::GetClassInfo();
}

const wxPGEditor*	wxRenderImageSummonerPropertyEX::DoGetEditorClass() const
{
	return wxLongStringProperty::DoGetEditorClass();
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderFrameSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
wxRenderFrameEditor* wxRenderFrameSummonerPropertyEX::m_editor = 0;

bool	wxRenderFrameSummonerPropertyEX::OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
{
	m_editor = new wxRenderFrameEditor( "RenderFrameEditor" );
	m_editor->SyncFromEngine();
	m_editor->Show( true );
	return true;
}

wxClassInfo*	wxRenderFrameSummonerPropertyEX::GetClassInfo() const
{
	return wxLongStringProperty::GetClassInfo();
}

const wxPGEditor*	wxRenderFrameSummonerPropertyEX::DoGetEditorClass() const
{
	return wxLongStringProperty::DoGetEditorClass();
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
PropertyControllerioWnd
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

wxColour	PropertyControllerioWnd::st_textdefcolor = *wxBLACK;
wxColour	PropertyControllerioWnd::st_backdefcolor = wxColour( 245, 250, 205 );
wxColour	PropertyControllerioWnd::st_uniquebackcolor = wxColour( 200, 250, 250 );

using namespace GlobalRef;

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
PropertyControllerioWnd::PropertyControllerioWnd()
{
}

PropertyControllerioWnd::~PropertyControllerioWnd()
{
}

//----------------------------------------------------------------------------------
/*
Setup
*/
void	PropertyControllerioWnd::Setup( wxioPropertyGrid* grid )
{
	if ( !grid ) {
		return;
	}

	wxPGProperty* categoryWnd = grid->Append( new wxPropertyCategory( "Wnd" ) );		
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

		wxPGProperty* pgImagery = grid->AppendIn( categoryWnd, new wxPropertyCategory( "Imagery" ) );
		{
			grid->AppendIn( pgImagery, new wxRenderFrameSummonerPropertyEX( "FrameEditor" ) );
			grid->AppendIn( pgImagery, new wxRenderImageSummonerPropertyEX( "ImageEditor" ) );
		}
		grid->SetPropertyBackgroundColour( pgImagery, st_uniquebackcolor );

		wxPGProperty* pgtitle = grid->AppendIn( categoryWnd, new wxPropertyCategory( "Title" ) );
		{
			grid->AppendIn( pgtitle, new wxStringProperty( "Text" ) );
			grid->AppendIn( pgtitle, new wxIntProperty( "OffsetX" ) );
			grid->AppendIn( pgtitle, new wxIntProperty( "OffsetY" ) );
			grid->AppendIn( pgtitle, new wxIntProperty( "TextSize" ) );
			grid->AppendIn( pgtitle, new wxIntProperty( "TextVertGap" ) );
			grid->AppendIn( pgtitle, new wxEnumProperty( "TextHorzAlign", *wxPGProperty::sm_wxPG_LABEL, GetHorzAlignString(), GetHorzAlignInt() ) );
			grid->AppendIn( pgtitle, new wxEnumProperty( "TextVertAlign", *wxPGProperty::sm_wxPG_LABEL, GetVertAlignString(), GetVertAlignInt() ) );
			grid->AppendIn( pgtitle, new wxEnumProperty( "TextStyle", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );
			grid->AppendIn( pgtitle, new wxEnumProperty( "TextDisableStyle", *wxPGProperty::sm_wxPG_LABEL, GetTextStyleString(), GetTextStyleInt() ) );
			wxPGProperty* pgnormalcolor = grid->AppendIn( pgtitle, new wxPropertyCategory( "ColorNormal" ) );
			{
				grid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalTextColor", wxPG_LABEL, st_textdefcolor ) );
				grid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalBKColor", wxPG_LABEL, st_textdefcolor ) );
				//grid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalTypeColor", wxPG_LABEL, st_textdefcolor ) );
			}
			grid->Collapse( pgnormalcolor );
			wxPGProperty* pgovercolor = grid->AppendIn( pgtitle, new wxPropertyCategory( "ColorOver" ) );
			{
				grid->AppendIn( pgovercolor, new wxColourProperty( "OverTextColor", wxPG_LABEL, st_textdefcolor ) );
				grid->AppendIn( pgovercolor, new wxColourProperty( "OverBKColor", wxPG_LABEL, st_textdefcolor ) );
			}
			grid->Collapse( pgovercolor );
			wxPGProperty* pgpushcolor = grid->AppendIn( pgtitle, new wxPropertyCategory( "ColorPush" ) );
			{
				grid->AppendIn( pgpushcolor, new wxColourProperty( "PushTextColor", wxPG_LABEL, st_textdefcolor ) );
				grid->AppendIn( pgpushcolor, new wxColourProperty( "PushBKColor", wxPG_LABEL, st_textdefcolor ) );
			}
			grid->Collapse( pgpushcolor );
			wxPGProperty* pgdacolor = grid->AppendIn( pgtitle, new wxPropertyCategory( "ColorDisable" ) );
			{
				grid->AppendIn( pgdacolor, new wxColourProperty( "DisableTextColor", wxPG_LABEL, st_textdefcolor ) );
				grid->AppendIn( pgdacolor, new wxColourProperty( "DisableBKColor", wxPG_LABEL, st_textdefcolor ) );
			}
			grid->Collapse( pgdacolor );
			wxPGProperty* pgnccolor = grid->AppendIn( pgtitle, new wxPropertyCategory( "ColorNoneSelected" ) );
			{
				grid->AppendIn( pgnccolor, new wxColourProperty( "NoneSelectedTextColor", wxPG_LABEL, st_textdefcolor ) );
				grid->AppendIn( pgnccolor, new wxColourProperty( "NoneSelectedBKColor", wxPG_LABEL, st_textdefcolor ) );
			}
			grid->Collapse( pgnccolor );
		}
		grid->Collapse( pgtitle );
		grid->SetPropertyBackgroundColour( pgtitle, st_uniquebackcolor );

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

		//// ToolTip 총 TOOLTIP_SURPPORT_NUM개까지 지원
		//for ( int i = 0; i < TOOLTIP_SURPPORT_NUM; ++i )
		//{
		//	wxPGProperty* pgtooltip = grid->AppendIn( categoryWnd, new wxPropertyCategory( Format( "ToolTip%d", i ) ) );
		//	{
		//		grid->AppendIn( pgtooltip, new wxStringProperty( Format( "ToolTipText%d", i ) ) );
		//		grid->AppendIn( pgtooltip, new wxEnumProperty( Format( "ToolTipAlign%d", i ), *wxPGProperty::sm_wxPG_LABEL, horzitems, horzitemsv ) );
		//		grid->AppendIn( pgtooltip, new wxColourProperty( Format( "ToolTipTextColor%d", i ), wxPG_LABEL, *wxBLACK ) );
		//	}
		//	grid->Collapse( pgtooltip );
		//	grid->SetPropertyBackgroundColour( pgtooltip, colordef );
		//}
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

//----------------------------------------------------------------------------------
/*
Sync
*/
void	PropertyControllerioWnd::Sync( wxioPropertyGrid* grid, ioWnd* wnd )
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
	// title
	grid->SetValue<const char*>( "Text", wnd->GetTitleTextEX() );
	grid->SetValue<int>( "OffsetX", wnd->GetTitleOffsetX() );
	grid->SetValue<int>( "OffsetY", wnd->GetTitleOffsetY() );
	grid->SetValue<int>( "TextSize", wnd->GetTitleTextSize() );
	grid->SetValue<int>( "TextVertGap", wnd->GetTitleVertGap() );
	grid->SetValue<int>( "TextHorzAlign", wnd->GetTitleTextHorzAlign() );
	grid->SetValue<int>( "TextVertAlign", wnd->GetTitleTextVertAlign() );
	grid->SetValue<int>( "TextStyle", wnd->GetTitleTextStyle() );
	grid->SetValue<int>( "TextDisableStyle", wnd->GetTitleTextDisableStyle() );
	grid->SetColorValue( "NormalTextColor", ConvertToToolColor( wnd->GetTitleNormalTextColor() ) );
	grid->SetColorValue( "NormalBKColor", ConvertToToolColor( wnd->GetTitleNormalBKColor() ) );
	grid->SetColorValue( "OverTextColor", ConvertToToolColor( wnd->GetTitleOverTextColor() ) );
	grid->SetColorValue( "OverBKColor", ConvertToToolColor( wnd->GetTitleOverBKColor() ) );
	grid->SetColorValue( "PushTextColor", ConvertToToolColor( wnd->GetTitlePushTextColor() ) );
	grid->SetColorValue( "PushBKColor", ConvertToToolColor( wnd->GetTitlePushBKColor() ) );
	grid->SetColorValue( "DisableTextColor", ConvertToToolColor( wnd->GetTitleDisableTextColor() ) );
	grid->SetColorValue( "DisableBKColor", ConvertToToolColor( wnd->GetTitleDisableBKColor() ) );
	grid->SetColorValue( "NoneSelectedTextColor", ConvertToToolColor( wnd->GetTitleNoneSelectedTextColor() ) );
	grid->SetColorValue( "NoneSelectedBKColor", ConvertToToolColor( wnd->GetTitleNoneSelectedBKColor() ) );
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

//----------------------------------------------------------------------------------
/*
OnChange
*/
void	PropertyControllerioWnd::OnChange( wxioPropertyGrid* grid, ioWnd* wnd, wxPropertyGridEvent& event )
{
	event.Skip();

	ioWnd* outwnd = 0;
	wxString keyname;
	wxAny value;
	if ( !GetDatasFor<ioWnd>( wnd, &outwnd, keyname, value, event ) ) {
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
	// title
	COMPARE_AND_SET( "Text", SetTitleTextEX, event.GetValue().GetString().c_str() )
	COMPARE_AND_SET( "OffsetX", SetTitleOffsetX, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "OffsetY", SetTitleOffsetY, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextSize", SetTitleTextSize, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextVertGap", SetTitleVertGap, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextHorzAlign", SetTitleTextHorzAlign, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextVertAlign", SetTitleTextVertAlign, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextStyle", SetTitleTextStyle, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "TextDisableStyle", SetTitleTextDisableStyle, event.GetValue().GetInteger() )
	COMPARE_AND_SET( "NormalTextColor", SetTitleNormalTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "NormalBKColor", SetTitleNormalBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "OverTextColor", SetTitleOverTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "OverBKColor", SetTitleOverBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "PushTextColor", SetTitlePushTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "PushBKColor", SetTitlePushBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "DisableTextColor", SetTitleDisableTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "DisableBKColor", SetTitleDisableBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "NoneSelectedTextColor", SetTitleNoneSelectedTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
	COMPARE_AND_SET( "NoneSelectedBKColor", SetTitleNoneSelectedBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
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

//----------------------------------------------------------------------------------
/*
Util
*/
void	PropertyControllerioWnd::AppendInBoolPropertyCheckbox( wxioPropertyGrid* grid, wxPGProperty* child, const char* str )
{
	wxBoolProperty* boolproperty = new wxBoolProperty( str );
	boolproperty->SetAttribute( "UseCheckbox", 1 );
	grid->AppendIn( child, boolproperty );
}