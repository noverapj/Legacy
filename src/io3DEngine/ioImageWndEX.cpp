//----------------------------------------------------------------------------------
/*
Filename	: ioImageWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioImageWndEX.h"
#include "ioUIRenderElement.h"
#include "ioGUIManager.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"
#include "ioFrameTimer.h"

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioImageWndEX::ioImageWndEX( ioGUIManager* mgr )
	: ioWndEX( IWEXT_IMAGE )
	, m_guimgr( mgr )
	, m_image( 0 )
	, m_nomagicalign( false )
{
}

ioImageWndEX::ioImageWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioWndEX( type )
	, m_guimgr( mgr )
	, m_image( 0 )
	, m_nomagicalign( false )
{
}

ioImageWndEX::~ioImageWndEX()
{
	SAFEDELETE( m_image );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioImageWndEX::SetSize( int width, int height )
{
	if ( m_image )
	{
		m_image->SetAutoSize( true );
		m_image->SetUseOffset( false );
	}

	ioWndEX::SetSize( width, height );
	ioWndEX::SetRenderElementSize( m_image, width, height );	
}

void	ioImageWndEX::OnRender()
{
	if ( m_image )
	{
		int wx = GetDerivedPosX();
		int wy = GetDerivedPosY();

		m_image->Render( wx, wy, GetRenderType(), GetTextureFilterOption(), m_nomagicalign, GetGrayRenderType() );
	}
}

void	ioImageWndEX::CheckFrameReSize( int width, int height, int prevwidth, int prevheight )
{
	ioWndEX::CheckFrameReSize( width, height, prevwidth, prevheight );
	ioWndEX::SetRenderElementSize( m_image, width, height );
}

void	ioImageWndEX::SetWindowAlpha( int alpha )
{
	ioWndEX::SetWindowAlpha( alpha );

	if ( m_image ) {
		m_image->SetAlpha( alpha );
	}
}

bool ioImageWndEX::OnRenderShowAndHideByTop( DWORD dwCheckTime )
{
	if( m_dwWndAniTime == 0 )
		return false;

	if( !m_image )
		return false;

	if( FRAMEGETTIME() - m_dwWndAniTime > dwCheckTime )
	{
		m_dwWndAniTime = 0;
		m_image->SetSize( GetWidth(), GetHeight() );
		return false;
	}

	// 확대 & 축소를 20% ~ 100% 사이만 진행 시킨다.
	DWORD dwGapTime = FRAMEGETTIME() - m_dwWndAniTime;
	float fTimeRate = ( (float)dwGapTime / dwCheckTime ) * 0.8f;
	if( IsShow() )
	{
		fTimeRate = min( 0.8f, fTimeRate ) + 0.2f;
	}
	else
	{
		fTimeRate = max( 0.0f, 0.8f - fTimeRate ) + 0.2f;
	}
	
	int iHeight = GetHeight() * fTimeRate;
	m_image->SetSize( GetWidth(), iHeight );
	m_image->Render( GetDerivedPosX(), GetDerivedPosY() );

	return true;
}

//----------------------------------------------------------------------------------
/*
Controls
*/
//void	ioImageWndEX::SetImageSet( const char* fullname )
//{
//	if ( !fullname ) {
//		return;
//	}
//
//	DWORD color = 0xFFFFFFFF;
//	if ( m_image ) {
//		color = GetColor();
//	}
//	SAFEDELETE( m_image );
//
//	m_image = g_UIImageSetMgr.CreateImageByFullName( fullname );
//
//	if ( m_image ) 
//	{
//		RECT rt = m_image->GetRenderRect();
//		rt.left = 0;
//		rt.top = 0;
//		rt.right = GetWidth();
//		rt.bottom = GetHeight();
//		m_image->SetRenderRect( rt );
//
//		m_image->SetColor( color );
//
//		RecalcSize();
//	}
//}

void	ioImageWndEX::SetImage( ioUIRenderImage* image )
{
	if ( !image ) {
		return;
	}

	SAFEDELETE( m_image );
	m_image = image;

	RecalcSize();
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioImageWndEX::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioImageWndEX* imagewnd = dynamic_cast<ioImageWndEX*>(wnd);
	if ( !imagewnd ) {
		return;
	}

	CLONEIMAGE( imagewnd, m_image );

	imagewnd->m_nomagicalign = m_nomagicalign;
}

void	ioImageWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );

	if ( m_image )
	{
		ioXMLElement framexml = extxml.CreateChild( "ImageInfo" );
		framexml.SetStringAttribute( "FileName", GetFileName() );
		framexml.SetStringAttribute( "ImageSetName", GetImageSetName() );
		framexml.SetStringAttribute( "ImageName", GetImageName() );

		framexml.SetIntAttribute( "OffsetX", GetOffsetX() );
		framexml.SetIntAttribute( "OffsetY", GetOffsetY() );
		framexml.SetIntAttribute( "RenderType", GetRenderType() );
		framexml.SetIntAttribute( "GrayRenderType", GetGrayRenderType() );
		framexml.SetIntAttribute( "TextureFilterOption", GetTextureFilterOption() );
		framexml.SetIntAttribute( "ReserveFormat", GetReverseFormatting() );
		framexml.SetBoolAttribute( "NoMagicAlign", GetNoMagicAlign() );
		framexml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( GetColor() ).c_str() );
	}
}

void	ioImageWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioWndEX::_ParseExtraInfo( extxml );

	ioXMLElement elem = extxml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "ImageInfo" ) == 0 )
		{
			const char* filename = elem.GetStringAttribute( "FileName" );
			if ( filename && strcmp( filename, "" ) != 0 ) {
				SetFileName( filename, m_guimgr );
			}
			else {
				const char* imgsetname = elem.GetStringAttribute( "ImageSetName" );
				const char* imgname = elem.GetStringAttribute( "ImageName" );
				if( imgsetname && imgname && strcmp( imgsetname, "" ) != 0 && strcmp( imgname, "" ) != 0  ) {
					SetImageSet( imgsetname, imgname, m_guimgr );
				}
			}

			SetOffsetX( elem.GetIntAttribute( "OffsetX" ) );
			SetOffsetY( elem.GetIntAttribute( "OffsetY" ) );
			SetRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "RenderType" ) ) );
			SetGrayRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "GrayRenderType" ) ) );
			SetTextureFilterOption( static_cast<TextureFilterOption>( elem.GetIntAttribute( "TextureFilterOption" ) ) );
			SetReverseFormatting( static_cast<ioUIRenderImage::ReverseFormatting>( elem.GetIntAttribute( "ReserveFormat" ) ) );
			SetNoMagicAlign( elem.GetBoolAttribute( "NoMagicAlign" ) );
			SetColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "Color" ) ) );
			RecalcSize();
			break;
		}
		elem = elem.NextSibling();
	}
}