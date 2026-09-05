
//----------------------------------------------------------------------------------
/*
Filename	: wxCustomApp.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxCustomApp.h"
#include "wxRichTextEditorFrame.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxCustomApp
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

IMPLEMENT_APP( wxCustomApp )

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxCustomApp::wxCustomApp()
	: wxApp()
	, m_frame( 0 )
{
}

wxCustomApp::~wxCustomApp()
{
	m_frame = 0; // static singleton 이라서 delete 하면 안된다!
}

//----------------------------------------------------------------------------------
/*
wxApp override
*/
bool	wxCustomApp::OnInit()
{
	m_frame = new wxRichTextEditorFrame( wxT( "RichTextEditor" ) );
    m_frame->Show( true );

    return true;
}

void	wxCustomApp::OnIdle( wxIdleEvent& evt )
{
	if ( m_frame )
	{
		m_frame->Render();
		evt.RequestMore();
	}
}