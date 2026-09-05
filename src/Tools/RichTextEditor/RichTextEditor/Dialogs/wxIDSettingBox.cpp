
//----------------------------------------------------------------------------------
/*
Filename	: wxIDSettingBox.cpp
Author		: megamegy
Date		: 2013.10.14
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "wxIDSettingBox.h"
#include "../Utils/GlobalRef.h"
#include "../wxRichTextEditorFrame.h"

using namespace GlobalRef;

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
wxIDSettingBox
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
wxIDSettingBox::wxIDSettingBox( wxWindow* parent, const WndIDVec& ids )
	: IDSettingBox( parent )
	, m_settingdata( ids )
{
	assert( m_parent );

	Centre( wxBOTH );

	RefreshList();
}

wxIDSettingBox::~wxIDSettingBox()
{
}

//----------------------------------------------------------------------------------
/*
기타
*/
void	wxIDSettingBox::RefreshList()
{
	m_wndListBox->Clear();

	WndIDVec::iterator iter = m_settingdata.begin();
	WndIDVec::iterator eiter = m_settingdata.end();
	for ( ; iter != eiter; ++iter ) {
		m_wndListBox->AppendString( Format( "%d", *iter ) );
	}
}

//----------------------------------------------------------------------------------
/*
IDSettingBox override
*/
void	wxIDSettingBox::OnClickUp( wxCommandEvent& event )
{
	event.Skip();

	int idx = m_wndListBox->GetSelection();
	if ( idx < 0 || idx >= static_cast<int>( m_settingdata.size() ) ) {
		return;
	}
	if ( idx - 1 < 0 ) {
		return;
	}
	std::swap( m_settingdata[ idx ], m_settingdata[ idx - 1 ] );

	RefreshList();
}

void	wxIDSettingBox::OnClickDown( wxCommandEvent& event )
{
	event.Skip();

	int idx = m_wndListBox->GetSelection();
	if ( idx < 0 || idx >= static_cast<int>( m_settingdata.size() ) ) {
		return;
	}
	if ( idx + 1 >= static_cast<int>( m_settingdata.size() ) ) {
		return;
	}
	std::swap( m_settingdata[ idx ], m_settingdata[ idx + 1 ] );

	RefreshList();
}

void	wxIDSettingBox::OnClickSubmit( wxCommandEvent& event )
{
	wxString strID = m_idtextbox->GetValue();
	if ( strID != wxEmptyString )
	{
		WndIDVec::iterator iter = m_settingdata.begin();
		WndIDVec::iterator eiter = m_settingdata.end();
		for ( int i = 0; iter != eiter; ++iter, ++i ) 
		{
			ioWnd* wnd = wxRichTextEditorFrame::GetSingleton().GetInputProcessor().GetEditData().GetWndFromID( *iter );
			assert( wnd );

			if ( wnd ) {
				wnd->SetMemberName( wxString::Format( "%s[%d]", strID.c_str(), i ).c_str() );
			}
		}
	}

	event.Skip();

	Destroy();
}

void	wxIDSettingBox::OnClickCancel( wxCommandEvent& event )
{
	event.Skip();

	Destroy();
}

void	wxIDSettingBox::OnSelectWnd( wxCommandEvent& event )
{
	event.Skip();
}