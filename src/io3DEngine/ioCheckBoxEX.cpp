//----------------------------------------------------------------------------------
/*
Filename	: ioCheckBoxEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioCheckBoxEX.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioCheckBoxEX::ioCheckBoxEX( ioGUIManager* mgr )
	: ioImageButtonWndEX( IWEXT_CHECKBOX, mgr )
	, m_check( 0 )
	, m_lableoffsetx( 0 )
	, m_lableoffsety( 0 )
	, m_checked( false )
{
}

ioCheckBoxEX::ioCheckBoxEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioImageButtonWndEX( type, mgr )
	, m_check( 0 )
	, m_lableoffsetx( 0 )
	, m_lableoffsety( 0 )
	, m_checked( false )
{
}

ioCheckBoxEX::~ioCheckBoxEX()
{
	SAFEDELETE( m_check );
}

//----------------------------------------------------------------------------------
/*
ioWnd Override
*/
void	ioCheckBoxEX::OnRender()
{
	ioImageButtonWndEX::OnRender();

	int x = GetDerivedPosX();
	int y = GetDerivedPosY();

	if ( IsChecked() )
	{
		DrawCheck( x, y );
	}

	//DrawBack( x, y );

	//if ( IsDisabled() )
	//{
	//	DrawDisabled( x, y );		
	//}
	//else if ( IsExDisabled() )
	//{
	//	if ( IsPushed() ) {
	//		DrawExDisabledPushed( x, y );
	//	} else if ( IsOvered() ) {
	//		DrawExDisabledOvered( x, y );
	//	} else {
	//		DrawExDisabledNormal( x, y );
	//	}
	//}
	//else
	//{
	//	if ( IsChecked() )
	//	{
	//		ioImageButtonWndEX::OnRender();
	//		DrawCheck( x, y );
	//	}
	//	else
	//	{
	//		if ( IsPushed() ) {
	//			DrawPushed( x, y );
	//		} else if ( IsOvered() ) {
	//			DrawOvered( x, y );
	//		} else {
	//			DrawNormal( x, y );
	//		}
	//	}
	//}

	PrintLabel( x, y );
}

void	ioCheckBoxEX::SetWindowAlpha( int alpha )
{
	ioImageButtonWndEX::SetWindowAlpha( alpha );

	if ( m_check ) {
		m_check->SetAlpha( alpha );
	}

	if ( m_label ) {
		m_label->SetAlpha( alpha );
	}
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioCheckBoxEX::iwm_lbuttonup( const ioMouse& mouse )
{
	m_checked = !m_checked;

	ioImageButtonWndEX::iwm_lbuttonup( mouse );
}

//----------------------------------------------------------------------------------
/*
Virtual
*/
void	ioCheckBoxEX::DrawCheck( int x, int y )
{
	if ( m_check ) {
		m_check->Render( x, y );
	}
}

void	ioCheckBoxEX::PrintLabel( int x, int y )
{
	if ( m_label )
	{
		m_label->GetRenderPosForWndEX( x, y, GetWidth(), GetHeight(), x, y );
		m_label->PrintNormal( x + m_lableoffsetx, y + m_lableoffsety );
	}
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioCheckBoxEX::Copy( ioWnd* wnd )
{
	ioImageButtonWndEX::Copy( wnd );

	ioCheckBoxEX* cwnd = dynamic_cast<ioCheckBoxEX*>(wnd);
	if ( !cwnd ) {
		return;
	}

	CLONEIMAGE( cwnd, m_check );

	cwnd->SetLabelOffsetX( GetLabelOffsetX() );
	cwnd->SetLabelOffsetY( GetLabelOffsetY() );
}

void	ioCheckBoxEX::_SetSaveData( ioXMLElement& extxml )
{
	ioImageButtonWndEX::_SetSaveData( extxml );

	SetImageDataToXML( m_check, extxml, "CheckImage" );

	extxml.SetIntAttribute( "LabelOffsetX", GetLabelOffsetX() );
	extxml.SetIntAttribute( "LabelOffsetY", GetLabelOffsetY() );
}

void	ioCheckBoxEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioImageButtonWndEX::_ParseExtraInfo( extxml );

	SetImageDataFromXML( &m_check, extxml, "CheckImage", m_guimgr );

	SetLabelOffsetX( extxml.GetIntAttribute( "LabelOffsetX" ) );
	SetLabelOffsetY( extxml.GetIntAttribute( "LabelOffsetY" ) );

	RecalcSize();
}