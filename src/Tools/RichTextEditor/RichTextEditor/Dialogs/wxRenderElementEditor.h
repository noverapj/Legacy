
//----------------------------------------------------------------------------------
/*
Filename	: wxRenderElementEditor.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXRENDERELEMENTEDITOR_H_
#define _WXRENDERELEMENTEDITOR_H_

#include "../wxMainFrame.h"
#include <wx/CheckBox.h>
#include <list>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
Frame / Image Selector를 사용하기 위해선 해당 class를 상속받아야만 한다.
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class FrameImageSelectorUser
{
public:

	FrameImageSelectorUser() {}
	virtual ~FrameImageSelectorUser() {}

	virtual void	OnSelectFrameName( const char* name, void* userdata ) = 0;
	virtual void	OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata ) = 0;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxImageSetRendererSummonerPropertyEX : 이미지렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxImageSelector;
class WXDLLIMPEXP_PROPGRID wxImageSetRendererSummonerPropertyEX : public wxLongStringProperty
{
	 WX_PG_DECLARE_PROPERTY_CLASS( wxImageSetRendererSummonerPropertyEX )

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
    wxImageSetRendererSummonerPropertyEX( FrameImageSelectorUser* parent, void* userdata, const wxString& label, const wxString& key, wxioPropertyGrid* grid )
		: wxLongStringProperty( label )
		, m_userdata( userdata )
		, m_key( key )
		, m_grid( grid )
	{
		m_parent = parent;
	}
	virtual ~wxImageSetRendererSummonerPropertyEX() {}

	virtual bool					OnButtonClick( wxPropertyGrid* propgrid, wxString& value );

protected:

	static FrameImageSelectorUser*	m_parent;
	static wxImageSelector*			m_editor;
	void*							m_userdata;
	wxString						m_key;
	wxioPropertyGrid*				m_grid;
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxFrameRendererSummonerPropertyEX : 프레임렌더 편집창을 위한 property
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxFrameSelector;
class WXDLLIMPEXP_PROPGRID wxFrameRendererSummonerPropertyEX : public wxLongStringProperty
{
	 WX_PG_DECLARE_PROPERTY_CLASS( wxFrameRendererSummonerPropertyEX )

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
    wxFrameRendererSummonerPropertyEX( FrameImageSelectorUser* parent, void* userdata, const wxString& label, const wxString& key, wxioPropertyGrid* grid )
		: wxLongStringProperty( label )
		, m_userdata( userdata )
		, m_key( key )
		, m_grid( grid )
	{
		m_parent = parent;
	}
	virtual ~wxFrameRendererSummonerPropertyEX() {}

	virtual bool					OnButtonClick( wxPropertyGrid* propgrid, wxString& value );

protected:

	static FrameImageSelectorUser*	m_parent;
	static wxFrameSelector*			m_editor;
	void*							m_userdata;
	wxString						m_key;
	wxioPropertyGrid*				m_grid;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderElementEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioWnd;
class wxRenderElementEditor : public RenderImageEditor
{
public:

	//----------------------------------------------------------------------------------
	/*
	RenderElementImageInfo
	*/
	struct RenderElementInfo
	{
		RenderElementInfo( wxBoxSizer* sizer, wxCheckBox* check, wxButton* copybtn, wxButton* pastebtn, wxioPropertyGrid* grid, bool fixed, const std::string& type )
			: m_sizer( sizer )
			, m_check( check )
			, m_copybtn( copybtn )
			, m_pastebtn( pastebtn )
			, m_grid( grid )
			, m_catagerypg( 0 )
			, m_fixed( fixed )
			, m_type ( type )
		{}

		wxBoxSizer*			m_sizer;
		wxCheckBox*			m_check;
		wxButton*			m_copybtn;
		wxButton*			m_pastebtn;
		wxioPropertyGrid*	m_grid;
		wxPGProperty*		m_catagerypg;

		bool				m_fixed;
		std::string			m_type;
	};

	//----------------------------------------------------------------------------------
	/*	
	*/
	typedef std::list< RenderElementInfo >		RenderElementUIList;

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRenderElementEditor( const wxString& title, const std::string& cname, int renderelemtype );
	virtual ~wxRenderElementEditor();

	//----------------------------------------------------------------------------------
	/*
	RenderImageEditor override
	*/
	virtual void			OnAdd( wxCommandEvent& event );
	virtual void			OnRemove( wxCommandEvent& event );

	//----------------------------------------------------------------------------------
	/*
	이벤트
	*/
	void					Select( wxObject* obj );
	void					OnSelect( wxCommandEvent& event );
	void					OnCopy( wxCommandEvent& event );
	void					OnPaste( wxCommandEvent& event );
	void					OnPropertyChanged( wxPropertyGridEvent& event );
	void					OnPropertySelect( wxPropertyGridEvent& event );

	//----------------------------------------------------------------------------------
	/*
	RenderImageUIList 컨트롤
	*/
	void					SyncFromEngine();
	virtual void			_SyncFromEngine( RenderElementInfo* info, ioWnd* wnd, int index ) {}
	int						GetSelectRenderElementUI();
	RenderElementInfo*		GetRenderElementUI( int index );
	RenderElementInfo*		AddRenderElementUI( bool fixed, const std::string& type );
	void					RemoveRenderElementUI( int index );
	void					ClearRenderElementUI();
	void					ConvertUI( int index, bool fixed, const std::string& type );

protected:
	
	//----------------------------------------------------------------------------------
	/*
	Copy / Paste
	*/
	virtual void			Copy( wxioPropertyGrid* grid ) {}
	virtual void			Paste( wxioPropertyGrid* grid ) {}

	virtual void			_AddRenderElementUI( wxioPropertyGrid* grid, wxPGProperty* categoryWnd ) {}
	virtual void			SetToEngine( wxioPropertyGrid* grid, wxPropertyGridEvent& event, int index, const std::string& type ) {}
	virtual void			SetToEngine( wxioPropertyGrid* grid, int index, const std::string& type ) {}

protected:

	int						m_renderelemtype;

	RenderElementUIList		m_renderelementUIs;
	std::string				m_catageryname;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderImageEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxRenderImageEditor : public wxRenderElementEditor, public FrameImageSelectorUser
{
	struct CopyData
	{
		std::string			m_filename;
		std::string			m_imagesetname;
		std::string			m_imagename;
		int					m_x;
		int					m_y;
		int					m_width;
		int					m_height;
		DWORD				m_color;
	};

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRenderImageEditor( const wxString& title );
	virtual ~wxRenderImageEditor();

	//----------------------------------------------------------------------------------
	/*
	RenderImageEditor override
	*/
	virtual void			OnAdd( wxCommandEvent& event );
	virtual void			OnRemove( wxCommandEvent& event );
	
	//----------------------------------------------------------------------------------
	/*
	RenderImageUIList 컨트롤
	*/
	virtual void			_SyncFromEngine( RenderElementInfo* info, ioWnd* wnd, int index );

	//----------------------------------------------------------------------------------
	/*
	FrameImageSelectorUser override
	*/
	virtual void			OnSelectFrameName( const char* name, void* userdata ) {}
	virtual void			OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata );

protected:

	//----------------------------------------------------------------------------------
	/*
	Copy / Paste
	*/
	virtual void			Copy( wxioPropertyGrid* grid );
	virtual void			Paste( wxioPropertyGrid* grid );

	virtual void			_AddRenderElementUI( wxioPropertyGrid* grid, wxPGProperty* categoryWnd );
	virtual void			SetToEngine( wxioPropertyGrid* grid, wxPropertyGridEvent& event, int index, const std::string& type );
	virtual void			SetToEngine( wxioPropertyGrid* grid, int index, const std::string& type );

protected:

	CopyData				m_copydata;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxRenderFrameEditor
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class wxRenderFrameEditor : public wxRenderElementEditor, public FrameImageSelectorUser
{
	struct CopyData
	{
		int					m_x;
		int					m_y;
		int					m_width;
		int					m_height;
		DWORD				m_color;
		bool				m_autoresize;
		std::string			m_templete;
	};

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	wxRenderFrameEditor( const wxString& title );
	virtual ~wxRenderFrameEditor();

	//----------------------------------------------------------------------------------
	/*
	RenderImageEditor override
	*/
	virtual void			OnAdd( wxCommandEvent& event );
	virtual void			OnRemove( wxCommandEvent& event );
	
	//----------------------------------------------------------------------------------
	/*
	RenderImageUIList 컨트롤
	*/
	virtual void			_SyncFromEngine( RenderElementInfo* info, ioWnd* wnd, int index );

	//----------------------------------------------------------------------------------
	/*
	FrameImageSelectorUser override
	*/
	virtual void			OnSelectFrameName( const char* name, void* userdata );
	virtual void			OnSelectImageSetNames( const char* imagesetname, const char* imagename, void* userdata ) {}

protected:

	//----------------------------------------------------------------------------------
	/*
	Copy / Paste
	*/
	virtual void			Copy( wxioPropertyGrid* grid );
	virtual void			Paste( wxioPropertyGrid* grid );

	virtual void			_AddRenderElementUI( wxioPropertyGrid* grid, wxPGProperty* categoryWnd );
	virtual void			SetToEngine( wxioPropertyGrid* grid, wxPropertyGridEvent& event, int index, const std::string& type );
	virtual void			SetToEngine( wxioPropertyGrid* grid, int index, const std::string& type );

protected:

	CopyData				m_copydata;
};

#endif // _WXRENDERELEMENTEDITOR_H_