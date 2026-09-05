
//----------------------------------------------------------------------------------
/*
Filename	: wxFrameSelector.h
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#ifndef _WXFRAMESELECTOR_H_
#define _WXFRAMESELECTOR_H_

#include "../wxMainFrame.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxFrameSelector
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class ioWnd;
class FrameImageSelectorUser;
class wxFrameSelector : public FrameSelector
{
public:

	//----------------------------------------------------------------------------------
	/*
	»ý¼º / ¼Ò¸ê
	*/
	wxFrameSelector( const wxString& title, FrameImageSelectorUser* parent, void* userdata );
	virtual ~wxFrameSelector();

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
	virtual void				OnSelectFrame( wxCommandEvent& event );

	//----------------------------------------------------------------------------------
	/*
	control
	*/
	void						Select( const wxString& templatename );

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

#endif // _WXFRAMESELECTOR_H_