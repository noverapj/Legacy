///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxMainFrame__
#define __wxMainFrame__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include "wxWidgetControls/wxioPropertyGrid.h"
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/scrolwin.h>
#include <wx/listbox.h>

///////////////////////////////////////////////////////////////////////////

#define ID_TOOLBOX_WND 1000
#define ID_TOOLBOX_BUTTON 1001
#define ID_TOOLBOX_RADIO 1002
#define ID_TOOLBOX_CHECK 1003
#define ID_TOOLBOX_EDIT 1004
#define ID_TOOLBOX_PROGRESSBAR 1005
#define ID_TOOLBOX_SLIDE 1006
#define ID_TOOLBOX_ACTIVESCREENBTN 1007
#define ID_TOOLBOX_SCROLL 1008
#define ID_TOOLBOX_WNDEX 1009
#define ID_TOOLBOX_FRAMEWND 1010
#define ID_TOOLBOX_IMAGEWND 1011
#define ID_TOOLBOX_LABELWND 1012
#define ID_TOOLBOX_FRAMEBUTTON 1013
#define ID_TOOLBOX_IMAGEBUTTON 1014
#define ID_TOOLBOX_CHECKBOX 1015
#define ID_TOOLBOX_FLASHPLAYER 1016
#define ID_TOOLBOX_RADIOWND 1017
#define ID_TOOLBOX_RADIOFRAMEBTN 1018
#define ID_TOOLBOX_RADIOIMAGEBTN 1019
#define ID_TOOLBOX_TABFRAMEBUTTON 1020
#define ID_TOOLBOX_TABWND 1021
#define ID_TOOLBOX_SCROLLBAR 1022
#define ID_TOOLBOX_RICHLABEL 1023
#define wxID_TOOl_ALIGN_LEFT 1024
#define wxID_TOOl_ALIGN_RIGHT 1025
#define wxID_TOOl_ALIGN_TOP 1026
#define wxID_TOOl_ALIGN_BOTTOM 1027
#define wxID_TOOl_CORRECTGAP_LR 1028
#define wxID_TOOl_CORRECTGAP_TB 1029
#define wxID_TOOl_CORRECTSIZE_LR 1030
#define wxID_TOOl_CORRECTSIZE_TB 1031
#define wxID_TOOl_CORRECTSIZE_ALL 1032
#define wxID_TAB_PROPERTY 1033
#define wxID_TAB_EVENT 1034
#define wxID_TOOLBAR_NEW 1035
#define wxID_TOOLBAR_SAVE 1036
#define wxID_TOOLBAR_SAVEAS 1037
#define wxID_TOOLBAR_LOAD 1038
#define wxID_TOOLBAR_SUMMONIDSETTINGBOX 1039

///////////////////////////////////////////////////////////////////////////////
/// Class wxMainFrame
///////////////////////////////////////////////////////////////////////////////
class wxMainFrame : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel6;
		wxTreeCtrl* m_InheritTree;
		wxNotebook* m_controltab;
		wxPanel* m_panel14;
		wxStaticBitmap* m_toolbox_wnd;
		wxStaticText* m_staticText1;
		wxStaticBitmap* m_bitmap11;
		wxStaticText* m_staticText2;
		wxStaticBitmap* m_bitmap111;
		wxStaticText* m_staticText3;
		wxStaticBitmap* m_bitmap112;
		wxStaticText* m_staticText4;
		wxStaticBitmap* m_bitmap113;
		wxStaticText* m_staticText5;
		wxStaticBitmap* m_bitmap1131;
		wxStaticText* m_staticText51;
		wxStaticBitmap* m_bitmap11311;
		wxStaticText* m_staticText511;
		wxStaticBitmap* m_bitmap113111;
		wxStaticText* m_staticText5111;
		wxStaticBitmap* m_bitmap1131111;
		wxStaticText* m_staticText51111;
		wxPanel* m_panel15;
		wxStaticBitmap* m_toolbox_wnd1;
		wxStaticText* m_staticText11;
		wxStaticBitmap* m_bitmap114;
		wxStaticText* m_staticText21;
		wxStaticBitmap* m_bitmap115;
		wxStaticText* m_staticText22;
		wxStaticBitmap* m_bitmap116;
		wxStaticText* m_staticText23;
		wxStaticBitmap* m_bitmap1161;
		wxStaticText* m_staticText231;
		wxStaticBitmap* m_bitmap11611;
		wxStaticText* m_staticText2311;
		wxStaticBitmap* m_bitmap116111;
		wxStaticText* m_staticText23111;
		wxStaticBitmap* m_bitmap1161111;
		wxStaticText* m_staticText231111;
		wxStaticBitmap* m_bitmap11611112;
		wxStaticText* m_staticText2311112;
		wxStaticBitmap* m_bitmap11611114;
		wxStaticText* m_staticText2311114;
		wxStaticBitmap* m_bitmap11611115;
		wxStaticText* m_staticText2311115;
		wxStaticBitmap* m_bitmap11611111;
		wxStaticText* m_staticText2311111;
		wxStaticBitmap* m_bitmap11611113;
		wxStaticText* m_staticText2311113;
		wxStaticBitmap* m_bitmap116111131;
		wxStaticText* m_staticText23111131;
		wxStaticBitmap* m_bitmap1161111311;
		wxStaticText* m_staticText231111311;
		wxPanel* m_displayer;
		wxBoxSizer* m_propertySizer;
		wxNotebook* m_notebook2;
		wxPanel* m_panel141;
		wxBoxSizer* bSizer37;
		wxioPropertyGrid* m_propertygrid;
		wxPanel* m_panel151;
		wxBoxSizer* bSizer38;
		wxioPropertyGrid* m_propertygridevent;
		wxPanel* m_panel4;
		wxTextCtrl* m_textCtrl1;
		wxButton* m_button1;
		wxButton* m_button5;
		wxMenuBar* m_menubar2;
		wxMenu* m_menu2;
		wxMenu* m_menu31;
		wxMenu* m_menu3;
		wxToolBar* m_toolBar1;
		wxStatusBar* m_statusbar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnInheritTreeKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnInheritTreeKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnInheritTreeBoxSelect( wxTreeEvent& event ) { event.Skip(); }
		virtual void OnToolBoxControlDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnClickToolAlignLeft( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolAlignRight( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolAlignTop( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolAlignBottom( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolCorrectGapLeftRight( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolCorrectGapTopBottom( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolCorrectSizeLeftRight( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolCorrectSizeTopBottom( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickToolCorrectSizeAll( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDisplayerKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnDisplayerKeyUp( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnDisplayerLButtonDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnDisplayerLButtonUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnDisplayerMouseMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnDisplayerPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnChangePropertyTab( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnCmd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCmd2( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoad( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSummonIDSettingBox( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		wxToolBar* m_toolbar_wndcontorl;
		
		wxMainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1900,1100 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~wxMainFrame();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class RenderImageEditor
///////////////////////////////////////////////////////////////////////////////
class RenderImageEditor : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel6;
		wxButton* m_button2;
		wxButton* m_button3;
		wxScrolledWindow* m_AddPanel;
		wxBoxSizer* m_AddSizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemove( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		RenderImageEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 620,849 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~RenderImageEditor();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ImageSelector
///////////////////////////////////////////////////////////////////////////////
class ImageSelector : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel7;
		wxPanel* m_canvas;
		wxPanel* m_panel10;
		wxButton* m_button5;
		wxButton* m_button6;
		wxPanel* m_panel8;
		wxListBox* m_imagesetlistbox;
		wxListBox* m_imagelistbox;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLButtonDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLButtonUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCanvasPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectImageSet( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectImage( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ImageSelector( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Image Selector"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1063,863 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~ImageSelector();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FrameSelector
///////////////////////////////////////////////////////////////////////////////
class FrameSelector : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_canvas;
		wxPanel* m_panel21;
		wxButton* m_button11;
		wxButton* m_button12;
		wxPanel* m_panel20;
		wxListBox* m_framelistbox;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnLButtonDown( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnLButtonUp( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnMouseMove( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCanvasPaint( wxPaintEvent& event ) { event.Skip(); }
		virtual void OnSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectFrame( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FrameSelector( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("FrameSelector"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 776,601 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~FrameSelector();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class IDSettingBox
///////////////////////////////////////////////////////////////////////////////
class IDSettingBox : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel17;
		wxButton* m_button11;
		wxButton* m_button12;
		wxListBox* m_wndListBox;
		wxTextCtrl* m_idtextbox;
		wxButton* m_button9;
		wxButton* m_button10;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClickUp( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickDown( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSelectWnd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickSubmit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClickCancel( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		IDSettingBox( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("ID Setting Box"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 413,592 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~IDSettingBox();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class RichLabelEditor
///////////////////////////////////////////////////////////////////////////////
class RichLabelEditor : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel17;
		wxListBox* m_blocklistbox;
		wxButton* m_button14;
		wxButton* m_button15;
		wxioPropertyGrid* m_propertygrid;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClickBlock( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAdd( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRemove( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		RichLabelEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 636,571 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~RichLabelEditor();
	
};

#endif //__wxMainFrame__
