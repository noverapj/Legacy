
//----------------------------------------------------------------------------------
/*
Filename	: wxImageSelector.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXIMAGESELECTOR_H_
#define _WXIMAGESELECTOR_H_

#include "../wxMainFrame.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxImageSelector
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioWnd;
class FrameImageSelectorUser;
class wxImageSelector : public ImageSelector
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	wxImageSelector( const wxString& title, FrameImageSelectorUser* parent, void* userdata );
	virtual ~wxImageSelector();

	//----------------------------------------------------------------------------------
	/*
	ImageSelector override
	*/
	virtual void				OnLButtonDown( wxMouseEvent& event );
	virtual void				OnLButtonUp( wxMouseEvent& event );
	virtual void				OnMouseMove( wxMouseEvent& event );
	virtual void				OnCanvasPaint( wxPaintEvent& event );
	virtual void				OnSelect( wxCommandEvent& event );
	virtual void				OnCancel( wxCommandEvent& event );
	virtual void				OnSelectImageSet( wxCommandEvent& event );
	virtual void				OnSelectImage( wxCommandEvent& event );

	//----------------------------------------------------------------------------------
	/*
	control
	*/
	void						Select( const wxString& imageset, const wxString& image );
	void						ListupImageNames();

public:

	//----------------------------------------------------------------------------------
	/*
	render
	*/
	static void					Render( wxWindow* win, IDirect3DTexture9* texture, const POINT& offset );
	static HRESULT				LoadRGBAFromDXT( LPDIRECT3DTEXTURE9 dst, LPDIRECT3DTEXTURE9 src );

protected:

	//----------------------------------------------------------------------------------
	/*
	render
	*/
	void						Render( IDirect3DTexture9* texture );

protected:

	FrameImageSelectorUser*		m_parent;

	POINT						m_center;
	POINT						m_dragpos;
	bool						m_lbuttondown;

	void*						m_userdata;
};

#endif // _WXIMAGESELECTOR_H_