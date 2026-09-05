
//----------------------------------------------------------------------------------
/*
Filename	: wxFrameSelector.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxFrameSelector.h"
#include "../Utils/GlobalRef.h"
#include "ioGUIManager.h"
#include "../wxRichTextEditorFrame.h"
#include "wxRenderElementEditor.h"
#include "ioRenderSystem.h"
#include "ioTextureManager.h"
#include "ioRenderTexture.h"
#include "wxImageSelector.h"
#include "ioVertexFormat.h"
#include "ioUIRenderer.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxFrameSelector
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
wxFrameSelector::wxFrameSelector( const wxString& title, FrameImageSelectorUser* parent, void* userdata )
	: FrameSelector( NULL, wxID_ANY, title )
	, m_parent( parent )
	, m_lbuttondown( false )
	, m_userdata( userdata )
{
	assert( m_parent );

	Centre( wxBOTH );

	int numframe = wxRichTextEditorFrame::GetSingleton().GetGUIManager()->GetNumFrame();
	for ( int i = 0; i < numframe; ++i ) {
		m_framelistbox->AppendString( wxRichTextEditorFrame::GetSingleton().GetGUIManager()->GetFrameName( i ) );
	}

	m_center.x = 0;
	m_center.y = 0;
}

wxFrameSelector::~wxFrameSelector()
{
}

//----------------------------------------------------------------------------------
/*
FrameSelector override
*/
void	wxFrameSelector::OnLButtonDown( wxMouseEvent& event )
{
	m_lbuttondown = true;

	m_dragpos.x = event.GetX();
	m_dragpos.y = event.GetY();

	m_canvas->CaptureMouse();
	m_canvas->SetFocus();

	event.Skip();
}

void	wxFrameSelector::OnLButtonUp( wxMouseEvent& event )
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

void	wxFrameSelector::OnMouseMove( wxMouseEvent& event )
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

void	wxFrameSelector::OnCanvasPaint( wxPaintEvent& event )
{	
	ioGUIManager* guimgr = wxRichTextEditorFrame::GetSingleton().GetGUIManager();
	ioRenderSystem* renderer = wxRichTextEditorFrame::GetSingleton().GetRenderSystem();
	ioTextureManager* texturemgr = wxRichTextEditorFrame::GetSingleton().GetTextureManager();
	if ( guimgr && renderer && texturemgr )
	{	
		wxSize size = m_canvas->GetSize();

		D3DVIEWPORT9 viewportold = renderer->GetViewPort();
		D3DVIEWPORT9 viewportnew;
		viewportnew.X = 0.0f;
		viewportnew.Y = 0.0f;
		viewportnew.Width = size.GetWidth();
		viewportnew.Height = size.GetHeight();
		viewportnew.MinZ = 0.0f;
		viewportnew.MaxZ = 1.0f;

		ioRenderTexture* texture = texturemgr->CreateRTT( "None", size.GetWidth(), size.GetHeight(), D3DFMT_A8R8G8B8, D3DFMT_D16, false );
		if ( texture )
		{
			int framewidth = size.GetWidth() * 0.33333f;
			int frameheight = size.GetHeight() * 0.25f;

			renderer->SetRenderTarget( texture );
			renderer->SetViewPort( viewportnew );

			renderer->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff007f7f );
			guimgr->RenderTestFrame( m_framelistbox->GetStringSelection().c_str(), size.GetWidth() * 0.5f - framewidth * 0.5f, size.GetHeight() * 0.5f - frameheight * 0.5f, framewidth, frameheight );
			wxRichTextEditorFrame::GetSingleton().GetUIRenderer()->Render( renderer );

			renderer->SetViewPort( viewportold );
			renderer->RestoreRenderTarget();

			wxImageSelector::Render( m_canvas, texture->GetTexture(), m_center );

			texture->Release();
			texturemgr->DestroyRTTIfNoRef( texture );
		}
	}

	event.Skip();
}

void	wxFrameSelector::OnSelect( wxCommandEvent& event )
{
	m_parent->OnSelectFrameName( m_framelistbox->GetStringSelection(), m_userdata );

	Destroy();

	event.Skip();
}

void	wxFrameSelector::OnCancel( wxCommandEvent& event )
{
	Destroy();

	event.Skip();
}

void	wxFrameSelector::OnSelectFrame( wxCommandEvent& event )
{
	m_center.x = 0;
	m_center.y = 0;

	Refresh();

	event.Skip();
}

//----------------------------------------------------------------------------------
/*
control
*/
void	wxFrameSelector::Select( const wxString& templatename )
{
	int num = m_framelistbox->GetCount();
	for ( int i = 0; i < num; ++i )
	{
		if ( m_framelistbox->GetString( i ) == templatename )
		{
			m_framelistbox->Select( i );
			m_framelistbox->SetSelection( i );
			break;
		}
	}

	Refresh();
}