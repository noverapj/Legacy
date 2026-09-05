
//----------------------------------------------------------------------------------
/*
Filename	: wxRichTextEditorFrame.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxRichTextEditorFrame.h"
#include "wx/imaglist.h"
#include "Utils/GlobalRef.h"
#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/advprops.h"
#include "wxRenderElementEditor.h"
#include "ioButton.h"
#include "ioProgressBar.h"
#include "ioScroll.h"
#include "wxWidgetControls/PropertyControllerioWnd.h"
#include "wxWidgetControls/PropertyControllerioScrolls.h"
#include "wxWidgetControls/PropertyControllerioETC.h"
#include "wxWidgetControls/PropertyControllerioButtons.h"

using namespace GlobalRef;

static const wxColour colordef = wxColour( 245, 250, 205 );
static const wxColour colorblue = wxColour( 200, 250, 250 );
static const wxColour colortxtdef = wxColour( 0, 0, 0 );

////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
///*
//wxRenderImageSummonerPropertyEX : 이미지렌더 편집창을 위한 property
//*/
////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
//class WXDLLIMPEXP_PROPGRID wxRenderImageSummonerPropertyEX : public wxLongStringProperty
//{
//	 WX_PG_DECLARE_PROPERTY_CLASS(wxRenderImageSummonerPropertyEX)
//public:
//    wxRenderImageSummonerPropertyEX( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString )
//		: wxLongStringProperty( label, name, value )		
//	{}
//	virtual ~wxRenderImageSummonerPropertyEX() {}
//	virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
//	{
//		m_editor = new wxRenderImageEditor( "RenderImageEditor" );
//		m_editor->SyncFromEngine();
//		m_editor->Show( true );
//		return true;
//	}
//protected:
//	static wxRenderImageEditor*		m_editor;
//};
//wxRenderImageEditor* wxRenderImageSummonerPropertyEX::m_editor = 0;
//wxClassInfo* wxRenderImageSummonerPropertyEX::GetClassInfo() const {
//	return wxLongStringProperty::GetClassInfo();
//}
//const wxPGEditor* wxRenderImageSummonerPropertyEX::DoGetEditorClass() const{
//	return wxLongStringProperty::DoGetEditorClass();
//}
//
//class WXDLLIMPEXP_PROPGRID wxRenderFrameSummonerPropertyEX : public wxLongStringProperty
//{
//	 WX_PG_DECLARE_PROPERTY_CLASS(wxRenderFrameSummonerPropertyEX)
//public:
//    wxRenderFrameSummonerPropertyEX( const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxString& value = wxEmptyString )
//		: wxLongStringProperty( label, name, value )		
//	{}
//	virtual ~wxRenderFrameSummonerPropertyEX() {}
//	virtual bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value )
//	{
//		m_editor = new wxRenderFrameEditor( "RenderFrameEditor" );
//		m_editor->SyncFromEngine();
//		m_editor->Show( true );
//		return true;
//	}
//protected:
//	static wxRenderFrameEditor*		m_editor;
//};
//wxRenderFrameEditor* wxRenderFrameSummonerPropertyEX::m_editor = 0;
//wxClassInfo* wxRenderFrameSummonerPropertyEX::GetClassInfo() const {
//	return wxLongStringProperty::GetClassInfo();
//}
//const wxPGEditor* wxRenderFrameSummonerPropertyEX::DoGetEditorClass() const{
//	return wxLongStringProperty::DoGetEditorClass();
//}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRichTextEditorFrame
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//void	AppendInBoolPropertyCheckbox( wxioPropertyGrid* pg, wxPGProperty* child, const char* str )
//{
//	wxBoolProperty* boolproperty = new wxBoolProperty( str );
//	boolproperty->SetAttribute( "UseCheckbox", 1 );
//	pg->AppendIn( child, boolproperty );
//}
//
////----------------------------------------------------------------------------------
///*
//PropertyWnd
//*/
//void	wxRichTextEditorFrame::SetupWndPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	wxArrayString horzitems;
//	wxArrayInt horzitemsv;
//	for ( int i = 0; i < GlobalRef::HORZALIGN_NUM; ++i )
//	{
//		horzitems.Add( GlobalRef::g_enginehorzalign[ i ] );
//		horzitemsv.Add( i );
//	}
//
//	wxPGProperty* categoryWnd = propertygrid->Append( new wxPropertyCategory( "Wnd" ) );		
//	{
//		wxPGProperty* pgsize = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "Size" ) );		
//		{
//			propertygrid->AppendIn( pgsize, new wxIntProperty( "X" ) );
//			propertygrid->AppendIn( pgsize, new wxIntProperty( "Y" ) );
//			propertygrid->AppendIn( pgsize, new wxIntProperty( "Width" ) );
//			propertygrid->AppendIn( pgsize, new wxIntProperty( "Height" ) );
//		}		
//		propertygrid->SetPropertyBackgroundColour( pgsize, colordef );
//		
//		wxPGProperty* pgalign = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "Align" ) );
//		{
//			wxArrayString items;
//			wxArrayInt itemsv;
//			
//			for ( int i = 0; i < ioWnd::UI_TOP_ALIGN; ++i )
//			{
//				items.Add( GlobalRef::g_enginewndalign[ i ] );
//				itemsv.Add( i );
//			}
//			propertygrid->AppendIn( pgalign, new wxEnumProperty( "HorzAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );
//
//			items.Clear();
//			itemsv.Clear();
//			for ( int i = ioWnd::UI_CENTER_ALIGN; i < ioWnd::UI_NONE_ALIGN; ++i )
//			{
//				items.Add( GlobalRef::g_enginewndalign[ i ] );
//				itemsv.Add( i );
//			}
//			propertygrid->AppendIn( pgalign, new wxEnumProperty( "VertAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );
//		}
//		propertygrid->SetPropertyBackgroundColour( pgalign, colordef );
//
//		wxPGProperty* pgImagery = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "Imagery" ) );
//		{
//			propertygrid->AppendIn( pgImagery, new wxRenderFrameSummonerPropertyEX( "FrameEditor" ) );
//			propertygrid->AppendIn( pgImagery, new wxRenderImageSummonerPropertyEX( "ImageEditor" ) );
//		}
//		propertygrid->SetPropertyBackgroundColour( pgImagery, colorblue );
//
//		wxPGProperty* pgtitle = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "Title" ) );
//		{
//			wxArrayString vertitems;
//			wxArrayInt vertitemsv;
//			for ( int i = 0; i < GlobalRef::VERTALIGN_NUM; ++i )
//			{
//				vertitems.Add( GlobalRef::g_enginevertalign[ i ] );
//				vertitemsv.Add( i );
//			}
//
//			wxArrayString tsitems;
//			wxArrayInt tsitemsv;
//			for ( int i = 0; i < GlobalRef::TEXT_STYLE_NUM; ++i )
//			{
//				tsitems.Add( GlobalRef::g_enginetextstyle[ i ] );
//				tsitemsv.Add( i );
//			}
//
//			propertygrid->AppendIn( pgtitle, new wxStringProperty( "Text" ) );
//			propertygrid->AppendIn( pgtitle, new wxIntProperty( "OffsetX" ) );
//			propertygrid->AppendIn( pgtitle, new wxIntProperty( "OffsetY" ) );
//			propertygrid->AppendIn( pgtitle, new wxIntProperty( "TextSize" ) );
//			propertygrid->AppendIn( pgtitle, new wxIntProperty( "TextVertGap" ) );
//			propertygrid->AppendIn( pgtitle, new wxEnumProperty( "TextHorzAlign", *wxPGProperty::sm_wxPG_LABEL, horzitems, horzitemsv ) );
//			propertygrid->AppendIn( pgtitle, new wxEnumProperty( "TextVertAlign", *wxPGProperty::sm_wxPG_LABEL, vertitems, vertitemsv ) );
//			propertygrid->AppendIn( pgtitle, new wxEnumProperty( "TextStyle", *wxPGProperty::sm_wxPG_LABEL, tsitems, tsitemsv ) );
//			propertygrid->AppendIn( pgtitle, new wxEnumProperty( "TextDisableStyle", *wxPGProperty::sm_wxPG_LABEL, tsitems, tsitemsv ) );
//			wxPGProperty* pgnormalcolor = propertygrid->AppendIn( pgtitle, new wxPropertyCategory( "ColorNormal" ) );
//			{
//				propertygrid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalTextColor", wxPG_LABEL, colortxtdef ) );
//				propertygrid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalBKColor", wxPG_LABEL, colortxtdef ) );
//				//propertygrid->AppendIn( pgnormalcolor, new wxColourProperty( "NormalTypeColor", wxPG_LABEL, colortxtdef ) );
//			}
//			propertygrid->Collapse( pgnormalcolor );
//			wxPGProperty* pgovercolor = propertygrid->AppendIn( pgtitle, new wxPropertyCategory( "ColorOver" ) );
//			{
//				propertygrid->AppendIn( pgovercolor, new wxColourProperty( "OverTextColor", wxPG_LABEL, colortxtdef ) );
//				propertygrid->AppendIn( pgovercolor, new wxColourProperty( "OverBKColor", wxPG_LABEL, colortxtdef ) );
//			}
//			propertygrid->Collapse( pgovercolor );
//			wxPGProperty* pgpushcolor = propertygrid->AppendIn( pgtitle, new wxPropertyCategory( "ColorPush" ) );
//			{
//				propertygrid->AppendIn( pgpushcolor, new wxColourProperty( "PushTextColor", wxPG_LABEL, colortxtdef ) );
//				propertygrid->AppendIn( pgpushcolor, new wxColourProperty( "PushBKColor", wxPG_LABEL, colortxtdef ) );
//			}
//			propertygrid->Collapse( pgpushcolor );
//			wxPGProperty* pgdacolor = propertygrid->AppendIn( pgtitle, new wxPropertyCategory( "ColorDisable" ) );
//			{
//				propertygrid->AppendIn( pgdacolor, new wxColourProperty( "DisableTextColor", wxPG_LABEL, colortxtdef ) );
//				propertygrid->AppendIn( pgdacolor, new wxColourProperty( "DisableBKColor", wxPG_LABEL, colortxtdef ) );
//			}
//			propertygrid->Collapse( pgdacolor );
//			wxPGProperty* pgnccolor = propertygrid->AppendIn( pgtitle, new wxPropertyCategory( "ColorNoneSelected" ) );
//			{
//				propertygrid->AppendIn( pgnccolor, new wxColourProperty( "NoneSelectedTextColor", wxPG_LABEL, colortxtdef ) );
//				propertygrid->AppendIn( pgnccolor, new wxColourProperty( "NoneSelectedBKColor", wxPG_LABEL, colortxtdef ) );
//			}
//			propertygrid->Collapse( pgnccolor );
//		}
//		propertygrid->Collapse( pgtitle );
//		propertygrid->SetPropertyBackgroundColour( pgtitle, colorblue );
//
//		wxPGProperty* pgstyle = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "Style" ) );
//		{
//			for ( int i = 0; i < GlobalRef::IWS_NUM; ++i ) {
//				AppendInBoolPropertyCheckbox( propertygrid, pgstyle, GlobalRef::g_enginewindowstyletext[ i ] );
//			}
//		}
//		propertygrid->Collapse( pgstyle );
//		propertygrid->SetPropertyBackgroundColour( pgstyle, colordef );
//
//		wxPGProperty* pgstyleex = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "StyleEX" ) );
//		{
//			for ( int i = 0; i < GlobalRef::IWS_EX_NUM; ++i ) {
//				AppendInBoolPropertyCheckbox( propertygrid, pgstyleex, GlobalRef::g_enginewindowexstyletext[ i ] );
//			}
//		}
//		propertygrid->Collapse( pgstyleex );
//		propertygrid->SetPropertyBackgroundColour( pgstyleex, colordef );
//
//		//// ToolTip 총 TOOLTIP_SURPPORT_NUM개까지 지원
//		//for ( int i = 0; i < TOOLTIP_SURPPORT_NUM; ++i )
//		//{
//		//	wxPGProperty* pgtooltip = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( Format( "ToolTip%d", i ) ) );
//		//	{
//		//		propertygrid->AppendIn( pgtooltip, new wxStringProperty( Format( "ToolTipText%d", i ) ) );
//		//		propertygrid->AppendIn( pgtooltip, new wxEnumProperty( Format( "ToolTipAlign%d", i ), *wxPGProperty::sm_wxPG_LABEL, horzitems, horzitemsv ) );
//		//		propertygrid->AppendIn( pgtooltip, new wxColourProperty( Format( "ToolTipTextColor%d", i ), wxPG_LABEL, colortxtdef ) );
//		//	}
//		//	propertygrid->Collapse( pgtooltip );
//		//	propertygrid->SetPropertyBackgroundColour( pgtooltip, colordef );
//		//}
//		wxPGProperty* pgtooltip = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "ToolTip" ) );
//		{
//			propertygrid->AppendIn( pgtooltip, new wxStringProperty( "ToolTipText" ) );
//			propertygrid->AppendIn( pgtooltip, new wxEnumProperty( "ToolTipAlign", *wxPGProperty::sm_wxPG_LABEL, horzitems, horzitemsv ) );
//			propertygrid->AppendIn( pgtooltip, new wxColourProperty( "ToolTipTextColor", wxPG_LABEL, colortxtdef ) );
//		}
//		propertygrid->Collapse( pgtooltip );
//		propertygrid->SetPropertyBackgroundColour( pgtooltip, colordef );
//
//		wxPGProperty* pgdragrect = propertygrid->AppendIn( categoryWnd, new wxPropertyCategory( "DragRect" ) );
//		{
//			propertygrid->AppendIn( pgdragrect, new wxIntProperty( "DR_Left" ) );
//			propertygrid->AppendIn( pgdragrect, new wxIntProperty( "DR_Top" ) );
//			propertygrid->AppendIn( pgdragrect, new wxIntProperty( "DR_Right" ) );
//			propertygrid->AppendIn( pgdragrect, new wxIntProperty( "DR_Bottom" ) );
//		}
//		propertygrid->Collapse( pgdragrect );
//		propertygrid->SetPropertyBackgroundColour( pgdragrect, colordef );
//	}
//}
//
//void	wxRichTextEditorFrame::SetupButtonPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	wxPGProperty* categoryWnd = propertygrid->Append( new wxPropertyCategory( "Button" ) );
//	{
//		propertygrid->AppendIn( categoryWnd, new wxStringProperty( "Help" ) );
//		propertygrid->AppendIn( categoryWnd, new wxStringProperty( "OverSound" ) );
//		propertygrid->AppendIn( categoryWnd, new wxStringProperty( "ClickSound" ) );		
//	}
//	propertygrid->SetPropertyBackgroundColour( categoryWnd, colordef );
//}
//
//void	wxRichTextEditorFrame::SetupRadioButtonPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	SetupButtonPropertyAttribute( m_propertygrid );
//}
//
//void	wxRichTextEditorFrame::SetupCheckBoxPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	SetupButtonPropertyAttribute( m_propertygrid );
//
//	wxPGProperty* categoryWnd = propertygrid->Append( new wxPropertyCategory( "CheckButton" ) );
//	{
//		propertygrid->AppendIn( categoryWnd, new wxColourProperty( "UnCheckTextColor", wxPG_LABEL, colortxtdef ) );
//		propertygrid->AppendIn( categoryWnd, new wxColourProperty( "UnCheckBackColor", wxPG_LABEL, colortxtdef ) );
//	}
//	propertygrid->SetPropertyBackgroundColour( categoryWnd, colorblue );
//}
//
//void	wxRichTextEditorFrame::SetupEditPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//}
//
//void	wxRichTextEditorFrame::SetupProgressBarPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	wxArrayString items;
//	wxArrayInt itemsv;
//	items.Add( "Left" );
//	items.Add( "Right" );
//	itemsv.Add( 0 );
//	itemsv.Add( 1 );
//
//	wxPGProperty* categoryWnd = propertygrid->Append( new wxPropertyCategory( "ProgressBar" ) );
//	{
//		propertygrid->AppendIn( categoryWnd, new wxColourProperty( "GraphColor", wxPG_LABEL, colortxtdef ) );
//		propertygrid->AppendIn( categoryWnd, new wxIntProperty( "MaxPos" ) );
//		propertygrid->AppendIn( categoryWnd, new wxEnumProperty( "ProgressAlign", *wxPGProperty::sm_wxPG_LABEL, items, itemsv ) );
//	}
//	propertygrid->SetPropertyBackgroundColour( categoryWnd, colorblue );
//}
//
//void	wxRichTextEditorFrame::SetupSlidePropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//}
//
//void	wxRichTextEditorFrame::SetupActiveScreenPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	SetupButtonPropertyAttribute( m_propertygrid );
//}
//
//void	wxRichTextEditorFrame::SetupScrollPropertyAttribute( wxioPropertyGrid* propertygrid )
//{
//	wxPGProperty* categoryWnd = propertygrid->Append( new wxPropertyCategory( "Scroll" ) );
//	{
//		propertygrid->AppendIn( categoryWnd, new wxIntProperty( "PageRange" ) );
//		propertygrid->AppendIn( categoryWnd, new wxIntProperty( "StartTime" ) );
//		propertygrid->AppendIn( categoryWnd, new wxIntProperty( "CurTime" ) );
//	}
//	propertygrid->SetPropertyBackgroundColour( categoryWnd, colorblue );
//}

//----------------------------------------------------------------------------------
/*
wxWindow / wxFrame Override
*/

//----------------------------------------------------------------------------------
/*
wxMainFrame Override
*/
// Frame
//void	wxRichTextEditorFrame::OnInheritTreeBoxSelect( wxTreeEvent& event )
//{	
//	m_inputprocessor.OnInheritTreeBoxSelect( event );
//
//	m_propertygrid->Clear();
//
//	int type = m_inputprocessor.GetEditData().GetType( event.GetItem() );
//	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) ) 
//	{
//		m_pgcontrollers[ type ]->Setup( m_propertygrid );
//
//		SyncProperty();
//	}	
//	
//	event.Skip();
//}

//----------------------------------------------------------------------------------
/*
PropertyGrid Event
*/
//void	wxRichTextEditorFrame::OnPropertyGridSelected( wxPropertyGridEvent& event )
//{
//}

//void	wxRichTextEditorFrame::OnPropertyGridChanged( wxPropertyGridEvent& event )
//{
//	int type = m_inputprocessor.GetEditData().GetSelectType();
//	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) )
//	{
//		m_pgcontrollers[ type ]->OnChange( m_propertygrid, m_inputprocessor.GetSelectWnd(), event );
//
//		SyncProperty();
//	}	
//
//	event.Skip();
//}

//template < typename T >
//bool	GetDatasFor( EditData& edata, T** wnd, wxString& keyname, wxAny& value, wxPropertyGridEvent& event )
//{
//	*wnd = dynamic_cast<T*>( edata.GetSelectWnd() );
//	if ( !(*wnd) ) {
//		return false;
//	}
//
//	wxPGProperty* property = event.GetProperty();
//	if ( !property ) {
//		return false;
//	}
//
//	keyname = property->GetName();
//	value = property->GetValue();
//	return true;
//}
//
//void	wxRichTextEditorFrame::ProcessWndTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ioWnd* wnd = 0;
//	wxString keyname;
//	wxAny value;
//	if ( !GetDatasFor<ioWnd>( m_inputprocessor.GetEditData(), &wnd, keyname, value, event ) ) {
//		return;
//	}
//
//#define COMPARE_AND_SET( comp, func, getfunc ) else if(keyname==comp){wnd->func(getfunc);return;}
//#define COMPARE_AND_ADDSTYLE( style, func, getfunc ) else if(keyname==#style){wnd->func(style,getfunc);return;}
//
//	if ( false ) {}
//	// size
//	COMPARE_AND_SET( "X", SetPosX, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "Y", SetPosY, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "Width", SetWidth, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "Height", SetHeight, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "HorzAlign", SetHorzAlign, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "VertAlign", SetVertAlign, event.GetValue().GetInteger() )
//	// title
//	COMPARE_AND_SET( "Text", SetTitleTextEX, event.GetValue().GetString().c_str() )
//	COMPARE_AND_SET( "OffsetX", SetTitleOffsetX, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "OffsetY", SetTitleOffsetY, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextSize", SetTitleTextSize, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextVertGap", SetTitleVertGap, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextHorzAlign", SetTitleTextHorzAlign, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextVertAlign", SetTitleTextVertAlign, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextStyle", SetTitleTextStyle, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "TextDisableStyle", SetTitleTextDisableStyle, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "NormalTextColor", SetTitleNormalTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "NormalBKColor", SetTitleNormalBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "OverTextColor", SetTitleOverTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "OverBKColor", SetTitleOverBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "PushTextColor", SetTitlePushTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "PushBKColor", SetTitlePushBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "DisableTextColor", SetTitleDisableTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "DisableBKColor", SetTitleDisableBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "NoneSelectedTextColor", SetTitleNoneSelectedTextColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	COMPARE_AND_SET( "NoneSelectedBKColor", SetTitleNoneSelectedBKColor, ConvertToEngineColor( value.As<wxColour>().m_pixel ) )
//	// style
//	if ( false ) {}
//	COMPARE_AND_ADDSTYLE( IWS_START_HIDE, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_INACTIVE, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_BACKMOST, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_CANMOVE, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_MODAL, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EXIT_ESC, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_PROCESS, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_DRAG_DROP, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_NO_MOUSE_RESPONSE, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_CHILD_ONLY_RESPONSE, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_TOPMOST, AddStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EXACTIVE, AddStyle, event.GetValue().GetBool() )
//	// ex style
//	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_PW, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_RIGHT, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_CENTER, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_MULTI_EDIT, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_NOTIFY_MOVED, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_OVER_SCREEN, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_PUSH_SCREEN, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_HSCROLL, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_VSCROLL, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_PUSH_DOUBLE, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_NUM, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_EDIT_CHECK_FOCUS, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_PUSHED_OVER_SCREEN, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_SHOW_NO_TOP, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_SHOW_HIDE_ACTION, AddExStyle, event.GetValue().GetBool() )
//	COMPARE_AND_ADDSTYLE( IWS_EX_NO_REND_EDIT_NUM, AddExStyle, event.GetValue().GetBool() )
//	// tooltip
//	if ( keyname == "ToolTipText" ) {
//		wnd->SetTooltipText( event.GetValue().GetString().c_str() );
//	} else if ( keyname == "ToolTipAlign" ) {
//		wnd->SetTooltipAlign( event.GetValue().GetInteger() );
//	} else if ( keyname == "ToolTipTextColor" ) {
//		wnd->SetTooltipColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
//	}
//	// rect
//	if ( false ) {}
//	COMPARE_AND_SET( "DR_Left", SetDragRectLeft, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "DR_Top", SetDragRectTop, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "DR_Right", SetDragRectRight, event.GetValue().GetInteger() )
//	COMPARE_AND_SET( "DR_Bottom", SetDragRecBottom, event.GetValue().GetInteger() )
//}
//
//void	wxRichTextEditorFrame::ProcessButtonTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ioButton* wnd = 0;
//	wxString keyname;
//	wxAny value;
//	if ( !GetDatasFor<ioButton>( m_inputprocessor.GetEditData(), &wnd, keyname, value, event ) ) {
//		return;
//	}
//
//	if ( keyname == "Help" ) {
//		wnd->SetHelp( event.GetValue().GetString().c_str() );
//	} else if ( keyname == "OverSound" ) {
//		wnd->SetOverSound( event.GetValue().GetString().c_str() );
//	} else if ( keyname == "ClickSound" ) {
//		wnd->SetClickSound( event.GetValue().GetString().c_str() );
//	}
//}
//
//void	wxRichTextEditorFrame::ProcessRadioButtonTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ProcessButtonTypePropertyGridChanged( event );
//}
//
//void	wxRichTextEditorFrame::ProcessCheckBoxTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ProcessButtonTypePropertyGridChanged( event );
//	
//	ioCheckButton* wnd = 0;
//	wxString keyname;
//	wxAny value;
//	if ( !GetDatasFor<ioCheckButton>( m_inputprocessor.GetEditData(), &wnd, keyname, value, event ) ) {
//		return;
//	}
//
//	if ( keyname == "UnCheckTextColor" ) {
//		wnd->SetUnCheckTextColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
//	} else if ( keyname == "UnCheckBackColor" ) {
//		wnd->SetUnCheckBackColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
//	}
//}
//
//void	wxRichTextEditorFrame::ProcessEditTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//}
//
//void	wxRichTextEditorFrame::ProcessProgressBarTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ioProgressBar* wnd = 0;
//	wxString keyname;
//	wxAny value;
//	if ( !GetDatasFor<ioProgressBar>( m_inputprocessor.GetEditData(), &wnd, keyname, value, event ) ) {
//		return;
//	}
//
//	if ( keyname == "GraphColor" ) {
//		wnd->SetGraphColor( ConvertToEngineColor( value.As<wxColour>().m_pixel ) );
//	} else if ( keyname == "MaxPos" ) {
//		wnd->SetMaxPosition( event.GetValue().GetInteger() );
//	} else if ( keyname == "ProgressAlign" ) {
//		wnd->SetProgressAlign( static_cast<ioProgressBar::ProgressAlign>( event.GetValue().GetInteger() ) );
//	}
//}
//
//void	wxRichTextEditorFrame::ProcessSlideTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//}
//
//void	wxRichTextEditorFrame::ProcessActiveScreenTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ProcessButtonTypePropertyGridChanged( event );
//}
//
//void	wxRichTextEditorFrame::ProcessScrollTypePropertyGridChanged( wxPropertyGridEvent& event )
//{
//	ioScroll* wnd = 0;
//	wxString keyname;
//	wxAny value;
//	if ( !GetDatasFor<ioScroll>( m_inputprocessor.GetEditData(), &wnd, keyname, value, event ) ) {
//		return;
//	}
//
//	if ( keyname == "PageRange" ) {
//		wnd->SetPageRange( event.GetValue().GetInteger() );
//	} else if ( keyname == "StartTime" ) {
//		wnd->SetStartTime( event.GetValue().GetInteger() );
//	} else if ( keyname == "CurTime" ) {
//		wnd->SetCurTime( event.GetValue().GetInteger() );
//	}
//}

////----------------------------------------------------------------------------------
///*
//Sync Engine To Tool
//*/
//void	wxRichTextEditorFrame::SyncProperty()
//{
//	int type = m_inputprocessor.GetEditData().GetSelectType();
//	if ( type >= 0 && type < static_cast<int>( m_pgcontrollers.size() ) ) {
//		m_pgcontrollers[ type ]->Sync( m_propertygrid, m_inputprocessor.GetSelectWnd() );
//	}
//
//	m_propertygrid->ClearIgnoreSet();
//}

//void	wxRichTextEditorFrame::SyncWndProperty()
//{
//	ioWnd* wnd = m_inputprocessor.GetSelectWnd();
//	if ( !wnd ) {
//		return;
//	}
//
//	// size
//	m_propertygrid->SetValue<int>( "X", wnd->GetPosX() );
//	m_propertygrid->SetValue<int>( "Y", wnd->GetPosY() );
//	m_propertygrid->SetValue<int>( "Width", wnd->GetDefWidth() );
//	m_propertygrid->SetValue<int>( "Height", wnd->GetDefHeight() );
//	m_propertygrid->SetValue<long>( "HorzAlign", wnd->GetHorzAlign() );
//	m_propertygrid->SetValue<long>( "VertAlign", wnd->GetVertAlign() );
//	// title
//	m_propertygrid->SetValue<const char*>( "Text", wnd->GetTitleTextEX() );
//	m_propertygrid->SetValue<int>( "OffsetX", wnd->GetTitleOffsetX() );
//	m_propertygrid->SetValue<int>( "OffsetY", wnd->GetTitleOffsetY() );
//	m_propertygrid->SetValue<int>( "TextSize", wnd->GetTitleTextSize() );
//	m_propertygrid->SetValue<int>( "TextVertGap", wnd->GetTitleVertGap() );
//	m_propertygrid->SetValue<int>( "TextHorzAlign", wnd->GetTitleTextHorzAlign() );
//	m_propertygrid->SetValue<int>( "TextVertAlign", wnd->GetTitleTextVertAlign() );
//	m_propertygrid->SetValue<int>( "TextStyle", wnd->GetTitleTextStyle() );
//	m_propertygrid->SetValue<int>( "TextDisableStyle", wnd->GetTitleTextDisableStyle() );
//	m_propertygrid->SetColorValue( "NormalTextColor", ConvertToToolColor( wnd->GetTitleNormalTextColor() ) );
//	m_propertygrid->SetColorValue( "NormalBKColor", ConvertToToolColor( wnd->GetTitleNormalBKColor() ) );
//	m_propertygrid->SetColorValue( "OverTextColor", ConvertToToolColor( wnd->GetTitleOverTextColor() ) );
//	m_propertygrid->SetColorValue( "OverBKColor", ConvertToToolColor( wnd->GetTitleOverBKColor() ) );
//	m_propertygrid->SetColorValue( "PushTextColor", ConvertToToolColor( wnd->GetTitlePushTextColor() ) );
//	m_propertygrid->SetColorValue( "PushBKColor", ConvertToToolColor( wnd->GetTitlePushBKColor() ) );
//	m_propertygrid->SetColorValue( "DisableTextColor", ConvertToToolColor( wnd->GetTitleDisableTextColor() ) );
//	m_propertygrid->SetColorValue( "DisableBKColor", ConvertToToolColor( wnd->GetTitleDisableBKColor() ) );
//	m_propertygrid->SetColorValue( "NoneSelectedTextColor", ConvertToToolColor( wnd->GetTitleNoneSelectedTextColor() ) );
//	m_propertygrid->SetColorValue( "NoneSelectedBKColor", ConvertToToolColor( wnd->GetTitleNoneSelectedBKColor() ) );
//	// style
//#define STYLE_TO_BOOLEAN(x) (((wnd->GetStyle()&x)!=0x00)?true:false)
//	m_propertygrid->SetValue<bool>( "IWS_START_HIDE", STYLE_TO_BOOLEAN( IWS_START_HIDE ) );
//	m_propertygrid->SetValue<bool>( "IWS_INACTIVE", STYLE_TO_BOOLEAN( IWS_INACTIVE ) );
//	m_propertygrid->SetValue<bool>( "IWS_BACKMOST", STYLE_TO_BOOLEAN( IWS_BACKMOST ) );
//	m_propertygrid->SetValue<bool>( "IWS_CANMOVE", STYLE_TO_BOOLEAN( IWS_CANMOVE ) );
//	m_propertygrid->SetValue<bool>( "IWS_MODAL", STYLE_TO_BOOLEAN( IWS_MODAL ) );
//	m_propertygrid->SetValue<bool>( "IWS_EXIT_ESC", STYLE_TO_BOOLEAN( IWS_EXIT_ESC ) );
//	m_propertygrid->SetValue<bool>( "IWS_PROCESS", STYLE_TO_BOOLEAN( IWS_PROCESS ) );
//	m_propertygrid->SetValue<bool>( "IWS_DRAG_DROP", STYLE_TO_BOOLEAN( IWS_DRAG_DROP ) );
//	m_propertygrid->SetValue<bool>( "IWS_NO_MOUSE_RESPONSE", STYLE_TO_BOOLEAN( IWS_NO_MOUSE_RESPONSE ) );
//	m_propertygrid->SetValue<bool>( "IWS_CHILD_ONLY_RESPONSE", STYLE_TO_BOOLEAN( IWS_CHILD_ONLY_RESPONSE ) );
//	m_propertygrid->SetValue<bool>( "IWS_TOPMOST", STYLE_TO_BOOLEAN( IWS_TOPMOST ) );
//	m_propertygrid->SetValue<bool>( "IWS_EXACTIVE", STYLE_TO_BOOLEAN( IWS_EXACTIVE ) );
//	// ex style
//#define EXSTYLE_TO_BOOLEAN(x) (((wnd->GetExStyle()&x)!=0x00)?true:false)
//	m_propertygrid->SetValue<bool>( "IWS_EX_EDIT_PW", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_PW ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_EDIT_RIGHT", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_RIGHT ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_EDIT_CENTER", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_CENTER ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_MULTI_EDIT", EXSTYLE_TO_BOOLEAN( IWS_EX_MULTI_EDIT ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_NOTIFY_MOVED", EXSTYLE_TO_BOOLEAN( IWS_EX_NOTIFY_MOVED ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_OVER_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_OVER_SCREEN ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_PUSH_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSH_SCREEN ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_HSCROLL", EXSTYLE_TO_BOOLEAN( IWS_EX_HSCROLL ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_VSCROLL", EXSTYLE_TO_BOOLEAN( IWS_EX_VSCROLL ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_PUSH_DOUBLE", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSH_DOUBLE ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_EDIT_NUM", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_NUM ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_EDIT_CHECK_FOCUS", EXSTYLE_TO_BOOLEAN( IWS_EX_EDIT_CHECK_FOCUS ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_PUSHED_OVER_SCREEN", EXSTYLE_TO_BOOLEAN( IWS_EX_PUSHED_OVER_SCREEN ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_SHOW_NO_TOP", EXSTYLE_TO_BOOLEAN( IWS_EX_SHOW_NO_TOP ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_SHOW_HIDE_ACTION", EXSTYLE_TO_BOOLEAN( IWS_EX_SHOW_HIDE_ACTION ) );
//	m_propertygrid->SetValue<bool>( "IWS_EX_NO_REND_EDIT_NUM", EXSTYLE_TO_BOOLEAN( IWS_EX_NO_REND_EDIT_NUM ) );
//	// tooltip
//	m_propertygrid->SetValue<const char*>( "ToolTipText", wnd->GetTooltipText() );
//	m_propertygrid->SetValue<int>( "ToolTipAlign", wnd->GetTooltipAlign() );
//	m_propertygrid->SetColorValue( "ToolTipTextColor", ConvertToToolColor( wnd->GetTooltipColor() ) );
//	// rect
//	m_propertygrid->SetValue<int>( "DR_Left", wnd->GetDragRectLeft() );
//	m_propertygrid->SetValue<int>( "DR_Top", wnd->GetDragRectTop() );
//	m_propertygrid->SetValue<int>( "DR_Right", wnd->GetDragRectRight() );
//	m_propertygrid->SetValue<int>( "DR_Bottom", wnd->GetDragRectBottom() );
//}
//
//void	wxRichTextEditorFrame::SyncButtonProperty()
//{
//	ioButton* wnd = dynamic_cast<ioButton*>( m_inputprocessor.GetSelectWnd() );
//	if ( !wnd ) {
//		return;
//	}
//
//	m_propertygrid->SetValue<const char*>( "Help", wnd->GetHelp() );
//	m_propertygrid->SetValue<const char*>( "OverSound", wnd->GetOverSound() );
//	m_propertygrid->SetValue<const char*>( "ClickSound", wnd->GetClickSound() );
//}
//
//void	wxRichTextEditorFrame::SyncRadioButtonProperty()
//{
//	SyncButtonProperty();
//}
//
//void	wxRichTextEditorFrame::SyncCheckBoxProperty()
//{
//	SyncButtonProperty();
//
//	ioCheckButton* wnd = dynamic_cast<ioCheckButton*>( m_inputprocessor.GetSelectWnd() );
//	if ( !wnd ) {
//		return;
//	}
//
//	m_propertygrid->SetColorValue( "UnCheckTextColor", ConvertToToolColor( wnd->GetUnCheckTextColor() ) );
//	m_propertygrid->SetColorValue( "UnCheckBackColor", ConvertToToolColor( wnd->GetUnCheckBackColor() ) );
//}
//
//void	wxRichTextEditorFrame::SyncEditProperty()
//{
//}
//
//void	wxRichTextEditorFrame::SyncProgressBarProperty()
//{
//	ioProgressBar* wnd = dynamic_cast<ioProgressBar*>( m_inputprocessor.GetSelectWnd() );
//	if ( !wnd ) {
//		return;
//	}
//
//	m_propertygrid->SetColorValue( "GraphColor", ConvertToToolColor( wnd->GetGraphColor() ) );
//	m_propertygrid->SetValue<int>( "MaxPos", wnd->GetMaxPos() );
//	m_propertygrid->SetValue<int>( "ProgressAlign", wnd->GetProgressAlign() );
//}
//
//void	wxRichTextEditorFrame::SyncSlideProperty()
//{
//}
//
//void	wxRichTextEditorFrame::SyncActiveScreenBtnProperty()
//{
//	SyncButtonProperty();
//}
//
//void	wxRichTextEditorFrame::SyncScrollProperty()
//{
//	ioScroll* wnd = dynamic_cast<ioScroll*>( m_inputprocessor.GetSelectWnd() );
//	if ( !wnd ) {
//		return;
//	}
//
//	m_propertygrid->SetValue<int>( "PageRange", wnd->GetPageRange() );
//	m_propertygrid->SetValue<int>( "StartTime", wnd->GetStartTime() );
//	m_propertygrid->SetValue<int>( "CurTime", wnd->GetCurTime() );
//}