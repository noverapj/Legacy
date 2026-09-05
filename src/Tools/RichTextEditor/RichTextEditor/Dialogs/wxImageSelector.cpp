
//----------------------------------------------------------------------------------
/*
Filename	: wxImageSelector.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxImageSelector.h"
#include "../Utils/GlobalRef.h"
#include "ioGUIManager.h"
#include "../wxRichTextEditorFrame.h"
#include "wxRenderElementEditor.h"
#include "ioRenderSystem.h"
#include "ioTextureManager.h"
#include "ioRenderTexture.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxImageSelector
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
wxImageSelector::wxImageSelector( const wxString& title, FrameImageSelectorUser* parent, void* userdata )
	: ImageSelector( NULL, wxID_ANY, title )
	, m_parent( parent )
	, m_lbuttondown( false )
	, m_userdata( userdata )
{
	assert( m_parent );

	Centre( wxBOTH );

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( guimgr )
	{
		int numset = guimgr->GetNumImageSet();
		for ( int i = 0; i < numset; ++i ) {
			m_imagesetlistbox->AppendString( guimgr->GetImageSetName( i ) );
		}
	}

	m_center.x = 0;
	m_center.y = 0;
}

wxImageSelector::~wxImageSelector()
{
}

//----------------------------------------------------------------------------------
/*
ImageSelector override
*/
void	wxImageSelector::OnLButtonDown( wxMouseEvent& event )
{
	m_lbuttondown = true;

	m_dragpos.x = event.GetX();
	m_dragpos.y = event.GetY();

	m_canvas->CaptureMouse();
	m_canvas->SetFocus();

	//ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	//if ( guimgr ) 
	//{
	//	wxString imagesetname = m_imagesetlistbox->GetStringSelection();
	//	wxString imagename = m_imagelistbox->GetStringSelection();
	//	guimgr->geti
	//	if ( imagename != wxEmptyString )
	//	{
	//		int left = m_center.x + guimgr->GetImageRectLeft( imagesetname.c_str(), imagename.c_str() );
	//		int right = m_center.x + guimgr->GetImageRectRight( imagesetname.c_str(), imagename.c_str() );
	//		int top = m_center.y + guimgr->GetImageRectTop( imagesetname.c_str(), imagename.c_str() );
	//		int bottom = m_center.y + guimgr->GetImageRectBottom( imagesetname.c_str(), imagename.c_str() );
	//	}
	//}

	event.Skip();
}

void	wxImageSelector::OnLButtonUp( wxMouseEvent& event )
{
	if ( m_lbuttondown ) {
		m_canvas->ReleaseMouse();
	}
	m_canvas->SetFocus();

	m_lbuttondown = false;

	m_dragpos.x = event.GetX();
	m_dragpos.y = event.GetY();

	event.Skip();
}

void	wxImageSelector::OnMouseMove( wxMouseEvent& event )
{
	if ( m_lbuttondown ) 
	{
		m_canvas->SetFocus();

		m_center.x += ( event.GetX() - m_dragpos.x );
		m_center.y += ( event.GetY() - m_dragpos.y );

		m_dragpos.x = event.GetX();
		m_dragpos.y = event.GetY();

		Refresh();
	}

	event.Skip();
}

HRESULT	wxImageSelector::LoadRGBAFromDXT( LPDIRECT3DTEXTURE9 dst, LPDIRECT3DTEXTURE9 src )
{
    assert( dst );
    assert( src );

    HRESULT ret = E_FAIL;
    LPDIRECT3DSURFACE9 dstsurf = NULL;
    LPDIRECT3DSURFACE9 srcsurf = NULL;

    if ( SUCCEEDED( dst->GetSurfaceLevel( 0, &dstsurf ) ) )
    {
	    if ( SUCCEEDED( src->GetSurfaceLevel( 0, &srcsurf ) ) )
	    {
			ret = D3DXLoadSurfaceFromSurface (
				dstsurf,
			    NULL,
			    NULL,
			    srcsurf,
			    NULL,
			    NULL,
			    D3DX_DEFAULT,
			    0
		    );
	    }
    }

    if ( dstsurf ) {
		dstsurf->Release ();
	}
    if ( srcsurf ) {
		srcsurf->Release ();
	}

    return ret;
}

void	wxImageSelector::Render( IDirect3DTexture9* texture )
{
	Render( m_canvas, texture, m_center );

	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( guimgr ) 
	{
		wxString imagesetname = m_imagesetlistbox->GetStringSelection();
		wxString imagename = m_imagelistbox->GetStringSelection();
		if ( imagename != wxEmptyString )
		{
			wxPaintDC dc( m_canvas );

			int left = m_center.x + guimgr->GetImageRectLeft( imagesetname.c_str(), imagename.c_str() );
			int right = m_center.x + guimgr->GetImageRectRight( imagesetname.c_str(), imagename.c_str() );
			int top = m_center.y + guimgr->GetImageRectTop( imagesetname.c_str(), imagename.c_str() );
			int bottom = m_center.y + guimgr->GetImageRectBottom( imagesetname.c_str(), imagename.c_str() );

			dc.SetPen( wxPen( wxColour( 255, 0, 255, 100 ), 3, wxPENSTYLE_SHORT_DASH ) );				
			dc.DrawLine( left, top, left, bottom );
			dc.DrawLine( left, top, right, top );
			dc.DrawLine( right, top, right, bottom );
			dc.DrawLine( left, bottom, right, bottom );
			dc.DrawLine( left, top, right, bottom );
			dc.DrawLine( right, top, left, bottom );
		}
	}
}

void	wxImageSelector::Render( wxWindow* win, IDirect3DTexture9* texture, const POINT& offset )
{
	IDirect3DDevice9* device = wxRichTextEditorFrame::GetSingleton().GetD3DDevice();
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( !win || !device || !texture || !guimgr ) {
		return;
	}

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc( 0, &desc );

	bool isrelease = false;
	if ( desc.Format != D3DFMT_A8R8G8B8 || desc.Usage != 0 )
	{
		IDirect3DTexture9* subtexture = 0;
		device->CreateTexture( desc.Width, desc.Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &subtexture, 0 );

		if ( SUCCEEDED( LoadRGBAFromDXT( subtexture, texture ) ) ) 
		{
			texture = subtexture;
			isrelease = true;
		}
	}

	if ( texture )
	{			
		texture->GetLevelDesc( 0, &desc );

		IDirect3DSurface9* surface = 0;
		texture->GetSurfaceLevel( 0, &surface );

		HDC surfaceDC;
		surface->GetDC( &surfaceDC );
			
		wxPaintDC dc( win );
		BitBlt( dc.GetHDC(), offset.x, offset.y, desc.Width, desc.Height, surfaceDC, 0, 0, SRCCOPY );
			
		surface->ReleaseDC( surfaceDC );
		surface->Release();

		if ( isrelease ) {
			texture->Release();
		}
	}
}

void	wxImageSelector::OnCanvasPaint( wxPaintEvent& event )
{		
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( guimgr ) {
		Render( guimgr->GetImageSetTexture( m_imagesetlistbox->GetStringSelection().c_str() ) );
	}

	event.Skip();
}

void	wxImageSelector::OnSelect( wxCommandEvent& event )
{
	m_parent->OnSelectImageSetNames( m_imagesetlistbox->GetStringSelection(), m_imagelistbox->GetStringSelection(), m_userdata );

	Destroy();

	event.Skip();
}

void	wxImageSelector::OnCancel( wxCommandEvent& event )
{
	Destroy();

	event.Skip();
}

void	wxImageSelector::OnSelectImageSet( wxCommandEvent& event )
{
	ListupImageNames();

	m_center.x = 0;
	m_center.y = 0;

	Refresh();

	event.Skip();
}

void	wxImageSelector::OnSelectImage( wxCommandEvent& event )
{
	Refresh();

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
control
*/
void	wxImageSelector::Select( const wxString& imageset, const wxString& image )
{
	int numset = m_imagesetlistbox->GetCount();
	for ( int i = 0; i < numset; ++i )
	{
		if ( m_imagesetlistbox->GetString( i ) == imageset )
		{
			m_imagesetlistbox->Select( i );
			m_imagesetlistbox->SetSelection( i );
			break;
		}
	}

	ListupImageNames();

	int numimage = m_imagelistbox->GetCount();
	for ( int i = 0; i < numimage; ++i )
	{
		if ( m_imagelistbox->GetString( i ) == image )
		{
			m_imagelistbox->Select( i );
			m_imagelistbox->SetSelection( i );
			break;
		}
	}

	Refresh();
}

void	wxImageSelector::ListupImageNames()
{
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	if ( guimgr )
	{
		m_imagelistbox->Clear();
		wxString imagesetname = m_imagesetlistbox->GetStringSelection();
		int num = guimgr->GetNumImage( imagesetname.c_str() );
		for ( int i = 0; i < num; ++i ) {
			m_imagelistbox->AppendString( guimgr->GetImageTagName( imagesetname.c_str(), i ) );
		}
	}
}