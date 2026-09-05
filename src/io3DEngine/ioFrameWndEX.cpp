//----------------------------------------------------------------------------------
/*
Filename	: ioFrameWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioFrameWndEX.h"
#include "ioUIRenderElement.h"
#include "ioUIRenderFrame.h"
#include "ioGUIManager.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioFrameWndEX::ioFrameWndEX( ioGUIManager* mgr )
	: ioWndEX( IWEXT_FRAME )
	, m_guimgr( mgr )
	, m_frame( 0 )
	, m_nomagicalign( false )
{
}

ioFrameWndEX::ioFrameWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioWndEX( type )
	, m_guimgr( mgr )
	, m_frame( 0 )
	, m_nomagicalign( false )
{
}

ioFrameWndEX::~ioFrameWndEX()
{
	SAFEDELETE( m_frame );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioFrameWndEX::SetSize( int width, int height )
{
	ioWndEX::SetSize( width, height );
	ioWndEX::SetRenderElementSize( m_frame, width, height );
}

 void	ioFrameWndEX::OnRender()
 {
	 if ( m_frame )
	 {
		 int wx = GetDerivedPosX();
		 int wy = GetDerivedPosY();

		 m_frame->Render( wx, wy, GetRenderType(), GetTextureFilterOption(), m_nomagicalign, GetGrayRenderType() );
	 }
 }

 void	ioFrameWndEX::CheckFrameReSize( int width, int height, int prevwidth, int prevheight )
 {
	ioWndEX::CheckFrameReSize( width, height, prevwidth, prevheight );
	ioWndEX::SetRenderElementSize( m_frame, width, height );
 }

void	ioFrameWndEX::SetWindowAlpha( int alpha )
{
	ioWndEX::SetWindowAlpha( alpha );

	if ( m_frame ) {
		m_frame->SetAlpha( alpha );
	}
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioFrameWndEX::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioFrameWndEX* framewnd = dynamic_cast<ioFrameWndEX*>(wnd);
	if ( !framewnd ) {
		return;
	}

	CLONEFRAME( framewnd, m_frame );

	framewnd->m_nomagicalign = m_nomagicalign;
}

void	ioFrameWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );

	if ( m_frame )
	{
		ioXMLElement framexml = extxml.CreateChild( "FrameInfo" );

		framexml.SetStringAttribute( "Template", GetFrameTemplateName() );
		framexml.SetIntAttribute( "RenderType", GetRenderType() );
		framexml.SetIntAttribute( "GrayRenderType", GetGrayRenderType() );
		framexml.SetIntAttribute( "TextureFilterOption", GetTextureFilterOption() );
		framexml.SetBoolAttribute( "NoMagicAlign", GetNoMagicAlign() );
		framexml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( GetColor() ).c_str() );
	}
}

void	ioFrameWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioWndEX::_ParseExtraInfo( extxml );

	ioXMLElement elem = extxml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "FrameInfo" ) == 0 )
		{
			SetFrameTemplateName( elem.GetStringAttribute( "Template" ), m_guimgr );
			SetRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "RenderType" ) ) );
			SetGrayRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "GrayRenderType" ) ) );
			SetTextureFilterOption( static_cast<TextureFilterOption>( elem.GetIntAttribute( "TextureFilterOption" ) ) );
			SetNoMagicAlign( elem.GetBoolAttribute( "NoMagicAlign" ) );
			SetColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "Color" ) ) );
			RecalcSize();
			break;
		}
		elem = elem.NextSibling();
	}
}