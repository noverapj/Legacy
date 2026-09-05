///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "wxMainFrame.h"

///////////////////////////////////////////////////////////////////////////

wxMainFrame::wxMainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 1800,1024 ), wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_panel6 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel6->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel6, wxID_ANY, wxT("Inherit") ), wxVERTICAL );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	m_InheritTree = new wxTreeCtrl( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxHSCROLL|wxVSCROLL );
	bSizer17->Add( m_InheritTree, 1, wxEXPAND, 5 );
	
	sbSizer3->Add( bSizer17, 4, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	sbSizer3->Add( bSizer18, 1, wxEXPAND, 5 );
	
	m_panel6->SetSizer( sbSizer3 );
	m_panel6->Layout();
	sbSizer3->Fit( m_panel6 );
	bSizer16->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	bSizer1->Add( bSizer16, 4, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_controltab = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel14 = new wxPanel( m_controltab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel14->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel14, wxID_ANY, wxT("Default Controls") ), wxVERTICAL );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_toolbox_wnd = new wxStaticBitmap( m_panel14, ID_TOOLBOX_WND, wxBitmap( wxT("ToolResources/UITool/Images/Wnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_toolbox_wnd, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( m_panel14, wxID_ANY, wxT("Wnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer12->Add( m_staticText1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer12, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121;
	bSizer121 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_BUTTON, wxBitmap( wxT("ToolResources/UITool/Images/button.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121->Add( m_bitmap11, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( m_panel14, wxID_ANY, wxT("Button"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer121->Add( m_staticText2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer121, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1211;
	bSizer1211 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap111 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_RADIO, wxBitmap( wxT("ToolResources/UITool/Images/Radio.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1211->Add( m_bitmap111, 0, wxALL, 5 );
	
	m_staticText3 = new wxStaticText( m_panel14, wxID_ANY, wxT("RadioButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer1211->Add( m_staticText3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer1211, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1212;
	bSizer1212 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap112 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_CHECK, wxBitmap( wxT("ToolResources/UITool/Images/check.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1212->Add( m_bitmap112, 0, wxALL, 5 );
	
	m_staticText4 = new wxStaticText( m_panel14, wxID_ANY, wxT("CheckBox"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer1212->Add( m_staticText4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer1212, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1213;
	bSizer1213 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap113 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_EDIT, wxBitmap( wxT("ToolResources/UITool/Images/Edit.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1213->Add( m_bitmap113, 0, wxALL, 5 );
	
	m_staticText5 = new wxStaticText( m_panel14, wxID_ANY, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer1213->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer1213, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12131;
	bSizer12131 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1131 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_PROGRESSBAR, wxBitmap( wxT("ToolResources/UITool/Images/progressbar.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12131->Add( m_bitmap1131, 0, wxALL, 5 );
	
	m_staticText51 = new wxStaticText( m_panel14, wxID_ANY, wxT("ProgressBar"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	bSizer12131->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer12131, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121311;
	bSizer121311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11311 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_SLIDE, wxBitmap( wxT("ToolResources/UITool/Images/slide.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121311->Add( m_bitmap11311, 0, wxALL, 5 );
	
	m_staticText511 = new wxStaticText( m_panel14, wxID_ANY, wxT("Slide"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText511->Wrap( -1 );
	bSizer121311->Add( m_staticText511, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer121311, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1213111;
	bSizer1213111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap113111 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_ACTIVESCREENBTN, wxBitmap( wxT("ToolResources/UITool/Images/activebutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1213111->Add( m_bitmap113111, 0, wxALL, 5 );
	
	m_staticText5111 = new wxStaticText( m_panel14, wxID_ANY, wxT("ActiveScreenButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5111->Wrap( -1 );
	bSizer1213111->Add( m_staticText5111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer1213111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12131111;
	bSizer12131111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1131111 = new wxStaticBitmap( m_panel14, ID_TOOLBOX_SCROLL, wxBitmap( wxT("ToolResources/UITool/Images/scroll.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12131111->Add( m_bitmap1131111, 0, wxALL, 5 );
	
	m_staticText51111 = new wxStaticText( m_panel14, wxID_ANY, wxT("Scroll"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51111->Wrap( -1 );
	bSizer12131111->Add( m_staticText51111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer1->Add( bSizer12131111, 0, wxEXPAND, 5 );
	
	m_panel14->SetSizer( sbSizer1 );
	m_panel14->Layout();
	sbSizer1->Fit( m_panel14 );
	m_controltab->AddPage( m_panel14, wxT("Control"), false );
	m_panel15 = new wxPanel( m_controltab, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel15->SetBackgroundColour( wxColour( 248, 156, 218 ) );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel15, wxID_ANY, wxT("Extension Controls") ), wxVERTICAL );
	
	wxBoxSizer* bSizer122;
	bSizer122 = new wxBoxSizer( wxHORIZONTAL );
	
	m_toolbox_wnd1 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_WNDEX, wxBitmap( wxT("ToolResources/UITool/Images/Wnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer122->Add( m_toolbox_wnd1, 0, wxALL, 5 );
	
	m_staticText11 = new wxStaticText( m_panel15, wxID_ANY, wxT("WndEX"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer122->Add( m_staticText11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer122, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1214;
	bSizer1214 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap114 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_FRAMEWND, wxBitmap( wxT("ToolResources/UITool/Images/framewnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1214->Add( m_bitmap114, 0, wxALL, 5 );
	
	m_staticText21 = new wxStaticText( m_panel15, wxID_ANY, wxT("FrameWnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	bSizer1214->Add( m_staticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer1214, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1215;
	bSizer1215 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap115 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_IMAGEWND, wxBitmap( wxT("ToolResources/UITool/Images/imagewnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1215->Add( m_bitmap115, 0, wxALL, 5 );
	
	m_staticText22 = new wxStaticText( m_panel15, wxID_ANY, wxT("ImageWnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	bSizer1215->Add( m_staticText22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer1215, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1216;
	bSizer1216 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap116 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_LABELWND, wxBitmap( wxT("ToolResources/UITool/Images/labelwnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1216->Add( m_bitmap116, 0, wxALL, 5 );
	
	m_staticText23 = new wxStaticText( m_panel15, wxID_ANY, wxT("LableWnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer1216->Add( m_staticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer1216, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12161;
	bSizer12161 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1161 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_FRAMEBUTTON, wxBitmap( wxT("ToolResources/UITool/Images/framebutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12161->Add( m_bitmap1161, 0, wxALL, 5 );
	
	m_staticText231 = new wxStaticText( m_panel15, wxID_ANY, wxT("FrameButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	bSizer12161->Add( m_staticText231, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer12161, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611;
	bSizer121611 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_IMAGEBUTTON, wxBitmap( wxT("ToolResources/UITool/Images/imagebutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611->Add( m_bitmap11611, 0, wxALL, 5 );
	
	m_staticText2311 = new wxStaticText( m_panel15, wxID_ANY, wxT("ImageButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311->Wrap( -1 );
	bSizer121611->Add( m_staticText2311, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1216111;
	bSizer1216111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap116111 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_CHECKBOX, wxBitmap( wxT("ToolResources/UITool/Images/check.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1216111->Add( m_bitmap116111, 0, wxALL, 5 );
	
	m_staticText23111 = new wxStaticText( m_panel15, wxID_ANY, wxT("CheckBoxEX"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23111->Wrap( -1 );
	bSizer1216111->Add( m_staticText23111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer1216111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12161111;
	bSizer12161111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1161111 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_FLASHPLAYER, wxBitmap( wxT("ToolResources/UITool/Images/flashplayer.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12161111->Add( m_bitmap1161111, 0, wxALL, 5 );
	
	m_staticText231111 = new wxStaticText( m_panel15, wxID_ANY, wxT("FlashPlayer"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231111->Wrap( -1 );
	bSizer12161111->Add( m_staticText231111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer12161111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611112;
	bSizer121611112 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611112 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_RADIOWND, wxBitmap( wxT("ToolResources/UITool/Images/radiobox.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611112->Add( m_bitmap11611112, 0, wxALL, 5 );
	
	m_staticText2311112 = new wxStaticText( m_panel15, wxID_ANY, wxT("RadioWnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311112->Wrap( -1 );
	bSizer121611112->Add( m_staticText2311112, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611112, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611114;
	bSizer121611114 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611114 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_RADIOFRAMEBTN, wxBitmap( wxT("ToolResources/UITool/Images/radiobutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611114->Add( m_bitmap11611114, 0, wxALL, 5 );
	
	m_staticText2311114 = new wxStaticText( m_panel15, wxID_ANY, wxT("RadioFrameButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311114->Wrap( -1 );
	bSizer121611114->Add( m_staticText2311114, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611114, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611115;
	bSizer121611115 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611115 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_RADIOIMAGEBTN, wxBitmap( wxT("ToolResources/UITool/Images/radiobutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611115->Add( m_bitmap11611115, 0, wxALL, 5 );
	
	m_staticText2311115 = new wxStaticText( m_panel15, wxID_ANY, wxT("RadioImageButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311115->Wrap( -1 );
	bSizer121611115->Add( m_staticText2311115, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611115, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611111;
	bSizer121611111 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611111 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_TABFRAMEBUTTON, wxBitmap( wxT("ToolResources/UITool/Images/tabframebutton.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611111->Add( m_bitmap11611111, 0, wxALL, 5 );
	
	m_staticText2311111 = new wxStaticText( m_panel15, wxID_ANY, wxT("TabFrameButton"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311111->Wrap( -1 );
	bSizer121611111->Add( m_staticText2311111, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611111, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer121611113;
	bSizer121611113 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap11611113 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_TABWND, wxBitmap( wxT("ToolResources/UITool/Images/tabwnd.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer121611113->Add( m_bitmap11611113, 0, wxALL, 5 );
	
	m_staticText2311113 = new wxStaticText( m_panel15, wxID_ANY, wxT("TabWnd"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2311113->Wrap( -1 );
	bSizer121611113->Add( m_staticText2311113, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer121611113, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer1216111131;
	bSizer1216111131 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap116111131 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_SCROLLBAR, wxBitmap( wxT("ToolResources/UITool/Images/scrollbar.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1216111131->Add( m_bitmap116111131, 0, wxALL, 5 );
	
	m_staticText23111131 = new wxStaticText( m_panel15, wxID_ANY, wxT("ScrollBar"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23111131->Wrap( -1 );
	bSizer1216111131->Add( m_staticText23111131, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer1216111131, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12161111311;
	bSizer12161111311 = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap1161111311 = new wxStaticBitmap( m_panel15, ID_TOOLBOX_RICHLABEL, wxBitmap( wxT("ToolResources/UITool/Images/richlabel.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12161111311->Add( m_bitmap1161111311, 0, wxALL, 5 );
	
	m_staticText231111311 = new wxStaticText( m_panel15, wxID_ANY, wxT("RichLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231111311->Wrap( -1 );
	bSizer12161111311->Add( m_staticText231111311, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	sbSizer10->Add( bSizer12161111311, 0, wxEXPAND, 5 );
	
	m_panel15->SetSizer( sbSizer10 );
	m_panel15->Layout();
	sbSizer10->Fit( m_panel15 );
	m_controltab->AddPage( m_panel15, wxT("Control EX"), true );
	
	bSizer11->Add( m_controltab, 1, wxEXPAND | wxALL, 5 );
	
	bSizer1->Add( bSizer11, 3, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );
	
	m_toolbar_wndcontorl = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	m_toolbar_wndcontorl->SetToolSeparation( 7 );
	m_toolbar_wndcontorl->AddSeparator(); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_ALIGN_LEFT, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/alignleft.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("왼쪽 맞춤"), wxT("쪽 맞춤") ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_ALIGN_RIGHT, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/alignright.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("오른쪽 맞춤"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_ALIGN_TOP, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/aligntop.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("위쪽 맞춤"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_ALIGN_BOTTOM, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/alignbottom.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("아래쪽 맞춤"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddSeparator(); 
	m_toolbar_wndcontorl->AddSeparator(); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_CORRECTGAP_LR, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/correctgaplr.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("옆으로"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_CORRECTGAP_TB, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/correctgaptb.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("아래로"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddSeparator(); 
	m_toolbar_wndcontorl->AddSeparator(); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_CORRECTSIZE_LR, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/correctsizelr.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("같은 너비로"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_CORRECTSIZE_TB, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/correctsizetb.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("같은 높이로"), wxEmptyString ); 
	m_toolbar_wndcontorl->AddTool( wxID_TOOl_CORRECTSIZE_ALL, wxEmptyString, wxBitmap( wxT("ToolResources/UITool/Images/correctsizeall.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxT("같은 크기로"), wxEmptyString ); 
	m_toolbar_wndcontorl->Realize();
	
	bSizer101->Add( m_toolbar_wndcontorl, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer2->SetMinSize( wxSize( -1,800 ) ); 
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	bSizer4->SetMinSize( wxSize( 1024,-1 ) ); 
	m_displayer = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL|wxWANTS_CHARS );
	m_displayer->SetBackgroundColour( wxColour( 44, 107, 87 ) );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_displayer->SetSizer( bSizer8 );
	m_displayer->Layout();
	bSizer8->Fit( m_displayer );
	bSizer4->Add( m_displayer, 1, wxALL|wxEXPAND, 5 );
	
	bSizer2->Add( bSizer4, 0, wxEXPAND, 5 );
	
	m_propertySizer = new wxBoxSizer( wxVERTICAL );
	
	m_notebook2 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel141 = new wxPanel( m_notebook2, wxID_TAB_PROPERTY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	m_propertygrid = new wxioPropertyGrid( m_panel141 );
	bSizer37->Add( m_propertygrid, 1, wxALL|wxEXPAND, 5 );
	
	m_panel141->SetSizer( bSizer37 );
	m_panel141->Layout();
	bSizer37->Fit( m_panel141 );
	m_notebook2->AddPage( m_panel141, wxT("Property"), true );
	m_panel151 = new wxPanel( m_notebook2, wxID_TAB_EVENT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer38 = new wxBoxSizer( wxVERTICAL );
	
	m_propertygridevent = new wxioPropertyGrid( m_panel151 );
	bSizer38->Add( m_propertygridevent, 1, wxALL|wxEXPAND, 5 );
	
	m_panel151->SetSizer( bSizer38 );
	m_panel151->Layout();
	bSizer38->Fit( m_panel151 );
	m_notebook2->AddPage( m_panel151, wxT("Event"), false );
	
	m_propertySizer->Add( m_notebook2, 1, wxEXPAND | wxALL, 5 );
	
	bSizer2->Add( m_propertySizer, 1, wxEXPAND, 5 );
	
	bSizer101->Add( bSizer2, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrl1 = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_textCtrl1, 10, wxALIGN_CENTER|wxALL, 5 );
	
	m_button1 = new wxButton( m_panel4, wxID_ANY, wxT("Submit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button1, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_button5 = new wxButton( m_panel4, wxID_ANY, wxT("Submit2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_button5, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_panel4->SetSizer( bSizer7 );
	m_panel4->Layout();
	bSizer7->Fit( m_panel4 );
	bSizer3->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	bSizer101->Add( bSizer3, 2, wxEXPAND, 5 );
	
	bSizer1->Add( bSizer101, 18, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	m_menubar2 = new wxMenuBar( 0 );
	m_menu2 = new wxMenu();
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("New") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( m_menuItem5 );
	
	wxMenuItem* m_separator1;
	m_separator1 = m_menu2->AppendSeparator();
	
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Save") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( m_menuItem1 );
	
	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Save As") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( m_menuItem4 );
	
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem( m_menu2, wxID_ANY, wxString( wxT("Load") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu2->Append( m_menuItem2 );
	
	m_menubar2->Append( m_menu2, wxT("File") ); 
	
	m_menu31 = new wxMenu();
	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem( m_menu31, wxID_ANY, wxString( wxT("IDSettingBox") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu31->Append( m_menuItem6 );
	
	m_menubar2->Append( m_menu31, wxT("Utility") ); 
	
	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem( m_menu3, wxID_ANY, wxString( wxT("...") ) , wxEmptyString, wxITEM_NORMAL );
	m_menu3->Append( m_menuItem3 );
	
	m_menubar2->Append( m_menu3, wxT("About") ); 
	
	this->SetMenuBar( m_menubar2 );
	
	m_toolBar1 = this->CreateToolBar( wxTB_HORIZONTAL, wxID_ANY );
	m_toolBar1->SetToolBitmapSize( wxSize( 25,20 ) );
	m_toolBar1->AddTool( wxID_TOOLBAR_NEW, wxT("New"), wxBitmap( wxT("ToolResources/UITool/Images/new.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar1->AddSeparator(); 
	m_toolBar1->AddTool( wxID_TOOLBAR_SAVE, wxT("tool"), wxBitmap( wxT("ToolResources/UITool/Images/save.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar1->AddTool( wxID_TOOLBAR_SAVEAS, wxT("tool"), wxBitmap( wxT("ToolResources/UITool/Images/asave.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar1->AddTool( wxID_TOOLBAR_LOAD, wxT("tool"), wxBitmap( wxT("ToolResources/UITool/Images/load.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar1->AddSeparator(); 
	m_toolBar1->AddTool( wxID_TOOLBAR_SUMMONIDSETTINGBOX, wxT("tool"), wxBitmap( wxT("ToolResources/UITool/Images/summonidsettingbox.bmp"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString ); 
	m_toolBar1->Realize();
	
	m_statusbar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_InheritTree->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( wxMainFrame::OnInheritTreeKeyDown ), NULL, this );
	m_InheritTree->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainFrame::OnInheritTreeKeyUp ), NULL, this );
	m_InheritTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( wxMainFrame::OnInheritTreeBoxSelect ), NULL, this );
	m_toolbox_wnd->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap112->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap113->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1131->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11311->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap113111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1131111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_toolbox_wnd1->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap114->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap115->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611112->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611114->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611115->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611111->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611113->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116111131->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161111311->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	this->Connect( wxID_TOOl_ALIGN_LEFT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignLeft ) );
	this->Connect( wxID_TOOl_ALIGN_RIGHT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignRight ) );
	this->Connect( wxID_TOOl_ALIGN_TOP, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignTop ) );
	this->Connect( wxID_TOOl_ALIGN_BOTTOM, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignBottom ) );
	this->Connect( wxID_TOOl_CORRECTGAP_LR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectGapLeftRight ) );
	this->Connect( wxID_TOOl_CORRECTGAP_TB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectGapTopBottom ) );
	this->Connect( wxID_TOOl_CORRECTSIZE_LR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeLeftRight ) );
	this->Connect( wxID_TOOl_CORRECTSIZE_TB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeTopBottom ) );
	this->Connect( wxID_TOOl_CORRECTSIZE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeAll ) );
	m_displayer->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( wxMainFrame::OnDisplayerKeyDown ), NULL, this );
	m_displayer->Connect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainFrame::OnDisplayerKeyUp ), NULL, this );
	m_displayer->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnDisplayerLButtonDown ), NULL, this );
	m_displayer->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( wxMainFrame::OnDisplayerLButtonUp ), NULL, this );
	m_displayer->Connect( wxEVT_MOTION, wxMouseEventHandler( wxMainFrame::OnDisplayerMouseMove ), NULL, this );
	m_displayer->Connect( wxEVT_PAINT, wxPaintEventHandler( wxMainFrame::OnDisplayerPaint ), NULL, this );
	m_notebook2->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( wxMainFrame::OnChangePropertyTab ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMainFrame::OnCmd ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMainFrame::OnCmd2 ), NULL, this );
	this->Connect( m_menuItem5->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnNew ) );
	this->Connect( m_menuItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSave ) );
	this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSaveAs ) );
	this->Connect( m_menuItem2->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnLoad ) );
	this->Connect( m_menuItem6->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSummonIDSettingBox ) );
	this->Connect( wxID_TOOLBAR_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnNew ) );
	this->Connect( wxID_TOOLBAR_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSave ) );
	this->Connect( wxID_TOOLBAR_SAVEAS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSaveAs ) );
	this->Connect( wxID_TOOLBAR_LOAD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnLoad ) );
	this->Connect( wxID_TOOLBAR_SUMMONIDSETTINGBOX, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSummonIDSettingBox ) );
}

wxMainFrame::~wxMainFrame()
{
	// Disconnect Events
	m_InheritTree->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( wxMainFrame::OnInheritTreeKeyDown ), NULL, this );
	m_InheritTree->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainFrame::OnInheritTreeKeyUp ), NULL, this );
	m_InheritTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( wxMainFrame::OnInheritTreeBoxSelect ), NULL, this );
	m_toolbox_wnd->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap112->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap113->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1131->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11311->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap113111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1131111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_toolbox_wnd1->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap114->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap115->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611112->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611114->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611115->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611111->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap11611113->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap116111131->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	m_bitmap1161111311->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnToolBoxControlDown ), NULL, this );
	this->Disconnect( wxID_TOOl_ALIGN_LEFT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignLeft ) );
	this->Disconnect( wxID_TOOl_ALIGN_RIGHT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignRight ) );
	this->Disconnect( wxID_TOOl_ALIGN_TOP, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignTop ) );
	this->Disconnect( wxID_TOOl_ALIGN_BOTTOM, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolAlignBottom ) );
	this->Disconnect( wxID_TOOl_CORRECTGAP_LR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectGapLeftRight ) );
	this->Disconnect( wxID_TOOl_CORRECTGAP_TB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectGapTopBottom ) );
	this->Disconnect( wxID_TOOl_CORRECTSIZE_LR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeLeftRight ) );
	this->Disconnect( wxID_TOOl_CORRECTSIZE_TB, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeTopBottom ) );
	this->Disconnect( wxID_TOOl_CORRECTSIZE_ALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnClickToolCorrectSizeAll ) );
	m_displayer->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( wxMainFrame::OnDisplayerKeyDown ), NULL, this );
	m_displayer->Disconnect( wxEVT_KEY_UP, wxKeyEventHandler( wxMainFrame::OnDisplayerKeyUp ), NULL, this );
	m_displayer->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxMainFrame::OnDisplayerLButtonDown ), NULL, this );
	m_displayer->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( wxMainFrame::OnDisplayerLButtonUp ), NULL, this );
	m_displayer->Disconnect( wxEVT_MOTION, wxMouseEventHandler( wxMainFrame::OnDisplayerMouseMove ), NULL, this );
	m_displayer->Disconnect( wxEVT_PAINT, wxPaintEventHandler( wxMainFrame::OnDisplayerPaint ), NULL, this );
	m_notebook2->Disconnect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( wxMainFrame::OnChangePropertyTab ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMainFrame::OnCmd ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( wxMainFrame::OnCmd2 ), NULL, this );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnNew ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSave ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSaveAs ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnLoad ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( wxMainFrame::OnSummonIDSettingBox ) );
	this->Disconnect( wxID_TOOLBAR_NEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnNew ) );
	this->Disconnect( wxID_TOOLBAR_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSave ) );
	this->Disconnect( wxID_TOOLBAR_SAVEAS, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSaveAs ) );
	this->Disconnect( wxID_TOOLBAR_LOAD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnLoad ) );
	this->Disconnect( wxID_TOOLBAR_SUMMONIDSETTINGBOX, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( wxMainFrame::OnSummonIDSettingBox ) );
	
}

RenderImageEditor::RenderImageEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	m_panel6 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( m_panel6, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_button2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_button3 = new wxButton( m_panel6, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_button3, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_panel6->SetSizer( bSizer21 );
	m_panel6->Layout();
	bSizer21->Fit( m_panel6 );
	bSizer19->Add( m_panel6, 1, wxEXPAND | wxALL, 5 );
	
	m_AddPanel = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_AddPanel->SetScrollRate( 5, 5 );
	m_AddSizer = new wxBoxSizer( wxVERTICAL );
	
	m_AddPanel->SetSizer( m_AddSizer );
	m_AddPanel->Layout();
	m_AddSizer->Fit( m_AddPanel );
	bSizer19->Add( m_AddPanel, 15, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RenderImageEditor::OnAdd ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RenderImageEditor::OnRemove ), NULL, this );
}

RenderImageEditor::~RenderImageEditor()
{
	// Disconnect Events
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RenderImageEditor::OnAdd ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RenderImageEditor::OnRemove ), NULL, this );
	
}

ImageSelector::ImageSelector( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );
	
	m_panel7 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxVERTICAL );
	
	m_canvas = new wxPanel( m_panel7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_canvas->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_GRAYTEXT ) );
	
	bSizer29->Add( m_canvas, 12, wxEXPAND | wxALL, 5 );
	
	m_panel10 = new wxPanel( m_panel7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel10->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );
	
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new wxButton( m_panel10, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30->Add( m_button5, 1, wxALL|wxEXPAND, 5 );
	
	m_button6 = new wxButton( m_panel10, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer30->Add( m_button6, 1, wxALL|wxEXPAND, 5 );
	
	m_panel10->SetSizer( bSizer30 );
	m_panel10->Layout();
	bSizer30->Fit( m_panel10 );
	bSizer29->Add( m_panel10, 1, wxEXPAND | wxALL, 5 );
	
	m_panel7->SetSizer( bSizer29 );
	m_panel7->Layout();
	bSizer29->Fit( m_panel7 );
	bSizer26->Add( m_panel7, 3, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxVERTICAL );
	
	m_panel8 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel8->SetBackgroundColour( wxColour( 97, 138, 184 ) );
	
	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel8, wxID_ANY, wxT("ImageSet") ), wxVERTICAL );
	
	m_imagesetlistbox = new wxListBox( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SORT ); 
	sbSizer3->Add( m_imagesetlistbox, 1, wxALL|wxEXPAND, 5 );
	
	bSizer28->Add( sbSizer3, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel8, wxID_ANY, wxT("Image") ), wxVERTICAL );
	
	m_imagelistbox = new wxListBox( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SORT ); 
	sbSizer4->Add( m_imagelistbox, 1, wxALL|wxEXPAND, 5 );
	
	bSizer28->Add( sbSizer4, 1, wxEXPAND, 5 );
	
	m_panel8->SetSizer( bSizer28 );
	m_panel8->Layout();
	bSizer28->Fit( m_panel8 );
	bSizer27->Add( m_panel8, 1, wxEXPAND | wxALL, 5 );
	
	bSizer26->Add( bSizer27, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer26 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_canvas->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ImageSelector::OnLButtonDown ), NULL, this );
	m_canvas->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( ImageSelector::OnLButtonUp ), NULL, this );
	m_canvas->Connect( wxEVT_MOTION, wxMouseEventHandler( ImageSelector::OnMouseMove ), NULL, this );
	m_canvas->Connect( wxEVT_PAINT, wxPaintEventHandler( ImageSelector::OnCanvasPaint ), NULL, this );
	m_button5->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImageSelector::OnSelect ), NULL, this );
	m_button6->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImageSelector::OnCancel ), NULL, this );
	m_imagesetlistbox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ImageSelector::OnSelectImageSet ), NULL, this );
	m_imagelistbox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ImageSelector::OnSelectImage ), NULL, this );
}

ImageSelector::~ImageSelector()
{
	// Disconnect Events
	m_canvas->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ImageSelector::OnLButtonDown ), NULL, this );
	m_canvas->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( ImageSelector::OnLButtonUp ), NULL, this );
	m_canvas->Disconnect( wxEVT_MOTION, wxMouseEventHandler( ImageSelector::OnMouseMove ), NULL, this );
	m_canvas->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ImageSelector::OnCanvasPaint ), NULL, this );
	m_button5->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImageSelector::OnSelect ), NULL, this );
	m_button6->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ImageSelector::OnCancel ), NULL, this );
	m_imagesetlistbox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ImageSelector::OnSelectImageSet ), NULL, this );
	m_imagelistbox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( ImageSelector::OnSelectImage ), NULL, this );
	
}

FrameSelector::FrameSelector( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxVERTICAL );
	
	m_canvas = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bSizer42->Add( m_canvas, 8, wxEXPAND | wxALL, 5 );
	
	m_panel21 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_panel21->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button11 = new wxButton( m_panel21, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_button11, 1, wxALL|wxEXPAND, 5 );
	
	m_button12 = new wxButton( m_panel21, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer43->Add( m_button12, 1, wxALL|wxEXPAND, 5 );
	
	m_panel21->SetSizer( bSizer43 );
	m_panel21->Layout();
	bSizer43->Fit( m_panel21 );
	bSizer42->Add( m_panel21, 1, wxEXPAND | wxALL, 5 );
	
	bSizer41->Add( bSizer42, 2, wxEXPAND, 5 );
	
	m_panel20 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( m_panel20, wxID_ANY, wxT("Frames") ), wxVERTICAL );
	
	m_framelistbox = new wxListBox( m_panel20, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SORT ); 
	sbSizer9->Add( m_framelistbox, 1, wxALL|wxEXPAND, 5 );
	
	m_panel20->SetSizer( sbSizer9 );
	m_panel20->Layout();
	sbSizer9->Fit( m_panel20 );
	bSizer41->Add( m_panel20, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer41 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_canvas->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FrameSelector::OnLButtonDown ), NULL, this );
	m_canvas->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( FrameSelector::OnLButtonUp ), NULL, this );
	m_canvas->Connect( wxEVT_MOTION, wxMouseEventHandler( FrameSelector::OnMouseMove ), NULL, this );
	m_canvas->Connect( wxEVT_PAINT, wxPaintEventHandler( FrameSelector::OnCanvasPaint ), NULL, this );
	m_button11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FrameSelector::OnSelect ), NULL, this );
	m_button12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FrameSelector::OnCancel ), NULL, this );
	m_framelistbox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( FrameSelector::OnSelectFrame ), NULL, this );
}

FrameSelector::~FrameSelector()
{
	// Disconnect Events
	m_canvas->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( FrameSelector::OnLButtonDown ), NULL, this );
	m_canvas->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( FrameSelector::OnLButtonUp ), NULL, this );
	m_canvas->Disconnect( wxEVT_MOTION, wxMouseEventHandler( FrameSelector::OnMouseMove ), NULL, this );
	m_canvas->Disconnect( wxEVT_PAINT, wxPaintEventHandler( FrameSelector::OnCanvasPaint ), NULL, this );
	m_button11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FrameSelector::OnSelect ), NULL, this );
	m_button12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FrameSelector::OnCancel ), NULL, this );
	m_framelistbox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( FrameSelector::OnSelectFrame ), NULL, this );
	
}

IDSettingBox::IDSettingBox( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer50;
	bSizer50 = new wxBoxSizer( wxVERTICAL );
	
	m_panel17 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button11 = new wxButton( m_panel17, wxID_ANY, wxT("Up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_button11, 1, wxALL|wxEXPAND, 5 );
	
	m_button12 = new wxButton( m_panel17, wxID_ANY, wxT("Down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_button12, 1, wxALL|wxEXPAND, 5 );
	
	bSizer51->Add( bSizer53, 0, wxEXPAND, 5 );
	
	m_wndListBox = new wxListBox( m_panel17, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer51->Add( m_wndListBox, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxHORIZONTAL );
	
	m_idtextbox = new wxTextCtrl( m_panel17, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_idtextbox, 3, wxALL|wxEXPAND, 5 );
	
	m_button9 = new wxButton( m_panel17, wxID_ANY, wxT("Submit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_button9, 1, wxALL, 5 );
	
	m_button10 = new wxButton( m_panel17, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer52->Add( m_button10, 1, wxALL, 5 );
	
	bSizer51->Add( bSizer52, 0, wxEXPAND, 5 );
	
	m_panel17->SetSizer( bSizer51 );
	m_panel17->Layout();
	bSizer51->Fit( m_panel17 );
	bSizer50->Add( m_panel17, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer50 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_button11->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickUp ), NULL, this );
	m_button12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickDown ), NULL, this );
	m_wndListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IDSettingBox::OnSelectWnd ), NULL, this );
	m_button9->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickSubmit ), NULL, this );
	m_button10->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickCancel ), NULL, this );
}

IDSettingBox::~IDSettingBox()
{
	// Disconnect Events
	m_button11->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickUp ), NULL, this );
	m_button12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickDown ), NULL, this );
	m_wndListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IDSettingBox::OnSelectWnd ), NULL, this );
	m_button9->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickSubmit ), NULL, this );
	m_button10->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IDSettingBox::OnClickCancel ), NULL, this );
	
}

RichLabelEditor::RichLabelEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer56;
	bSizer56 = new wxBoxSizer( wxVERTICAL );
	
	m_panel17 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer60;
	bSizer60 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer57;
	bSizer57 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer601;
	bSizer601 = new wxBoxSizer( wxVERTICAL );
	
	m_blocklistbox = new wxListBox( m_panel17, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer601->Add( m_blocklistbox, 9, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button14 = new wxButton( m_panel17, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_button14, 1, wxALL|wxEXPAND, 5 );
	
	m_button15 = new wxButton( m_panel17, wxID_ANY, wxT("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_button15, 1, wxALL|wxEXPAND, 5 );
	
	bSizer601->Add( bSizer61, 1, wxEXPAND, 5 );
	
	bSizer57->Add( bSizer601, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer58;
	bSizer58 = new wxBoxSizer( wxVERTICAL );
	
	m_propertygrid = new wxioPropertyGrid( m_panel17 );
	bSizer58->Add( m_propertygrid, 9, wxALL|wxEXPAND, 5 );
	
	bSizer57->Add( bSizer58, 1, wxEXPAND, 5 );
	
	bSizer60->Add( bSizer57, 1, wxEXPAND, 5 );
	
	m_panel17->SetSizer( bSizer60 );
	m_panel17->Layout();
	bSizer60->Fit( m_panel17 );
	bSizer56->Add( m_panel17, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer56 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_blocklistbox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( RichLabelEditor::OnClickBlock ), NULL, this );
	m_button14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RichLabelEditor::OnAdd ), NULL, this );
	m_button15->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RichLabelEditor::OnRemove ), NULL, this );
}

RichLabelEditor::~RichLabelEditor()
{
	// Disconnect Events
	m_blocklistbox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( RichLabelEditor::OnClickBlock ), NULL, this );
	m_button14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RichLabelEditor::OnAdd ), NULL, this );
	m_button15->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( RichLabelEditor::OnRemove ), NULL, this );
	
}
