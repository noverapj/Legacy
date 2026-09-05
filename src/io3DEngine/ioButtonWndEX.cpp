//----------------------------------------------------------------------------------
/*
Filename	: ioButtonWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioButtonWndEX.h"
#include "ioSoundManager.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioButtonWndEX::ioButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioWndEX( type )
	, m_btntype( BEXT_NORMAL )
	, m_guimgr( mgr )
	, m_normal( 0 )
	, m_over( 0 )
	, m_overadd( 0 )
	, m_push( 0 )
	, m_back( 0 )
	, m_disable( 0 )
	, m_exdisable_normal( 0 )
	, m_exdisable_push( 0 )
	, m_toggled( false )
{
	m_label = new ioUITitle();
}

ioButtonWndEX::~ioButtonWndEX()
{
	SAFEDELETE( m_label );

	SAFEDELETE( m_normal );
	SAFEDELETE( m_over );
	SAFEDELETE( m_overadd );
	SAFEDELETE( m_push );
	SAFEDELETE( m_back );
	SAFEDELETE( m_disable );
	SAFEDELETE( m_exdisable_normal );
	SAFEDELETE( m_exdisable_push );
}

//----------------------------------------------------------------------------------
/*
ioWnd Override
*/
void	ioButtonWndEX::OnRender()
{
	int x = GetDerivedPosX();
	int y = GetDerivedPosY();

	DrawBack( x, y );

	if ( IsDisabled() )
	{
		DrawDisabled( x, y );
		PrintDisableTitle( x, y );
	}
	else if ( IsExDisabled() )
	{
		if ( IsPushed() ) {
			DrawExDisabledPushed( x, y );
		} else if ( IsOvered() ) {
			DrawExDisabledOvered( x, y );
		} else {
			DrawExDisabledNormal( x, y );
		}
		PrintDisableTitle( x, y );
	}
	else
	{
		if ( IsPushed() || m_toggled )
		{
			DrawPushed( x, y );
			PrintPushTitle( x, y );
		}
		else if ( IsOvered() )
		{
			DrawOvered( x, y );
			PrintOverTitle( x, y );
		}
		else
		{
			DrawNormal( x, y );
			PrintNormalTitle( x, y );
		}
	}
}

void	ioButtonWndEX::SetSize( int width, int height )
{
	ioWndEX::SetSize( width, height );

	ioWndEX::SetRenderElementSize( m_normal, width, height );
	ioWndEX::SetRenderElementSize( m_over, width, height );
	ioWndEX::SetRenderElementSize( m_overadd, width, height );
	ioWndEX::SetRenderElementSize( m_push, width, height );
	ioWndEX::SetRenderElementSize( m_back, width, height );
	ioWndEX::SetRenderElementSize( m_disable, width, height );
	ioWndEX::SetRenderElementSize( m_exdisable_normal, width, height );
	ioWndEX::SetRenderElementSize( m_exdisable_push, width, height );
}

 void	ioButtonWndEX::CheckFrameReSize( int width, int height, int prevwidth, int prevheight )
 {
	ioWndEX::CheckFrameReSize( width, height, prevwidth, prevheight );

	ioWndEX::SetRenderElementSize( m_normal, width, height );
	ioWndEX::SetRenderElementSize( m_over, width, height );
	ioWndEX::SetRenderElementSize( m_overadd, width, height );
	ioWndEX::SetRenderElementSize( m_push, width, height );
	ioWndEX::SetRenderElementSize( m_back, width, height );
	ioWndEX::SetRenderElementSize( m_disable, width, height );
	ioWndEX::SetRenderElementSize( m_exdisable_normal, width, height );
	ioWndEX::SetRenderElementSize( m_exdisable_push, width, height );
 }

void	ioButtonWndEX::SetWindowAlpha( int alpha )
{
	ioWndEX::SetWindowAlpha( alpha );

	if ( m_normal ) {
		m_normal->SetAlpha( alpha );
	}
	if ( m_over ) {
		m_over->SetAlpha( alpha );
	}
	if ( m_overadd ) {
		m_overadd->SetAlpha( alpha );
	}
	if ( m_push ) {
		m_push->SetAlpha( alpha );
	}
	if ( m_back ) {
		m_back->SetAlpha( alpha );
	}
	if ( m_disable ) {
		m_disable->SetAlpha( alpha );
	}
	if ( m_exdisable_normal ) {
		m_exdisable_normal->SetAlpha( alpha );
	}
	if ( m_exdisable_push ) {
		m_exdisable_push->SetAlpha( alpha );
	}

	if ( m_label ) {
		m_label->SetAlpha( alpha );
	}
}

//----------------------------------------------------------------------------------
/*
ioWnd Event Override
*/
void	ioButtonWndEX::iwm_create()
{
	ioWndEX::iwm_create();

	SetCheckParentOutSide( true );
}

void	ioButtonWndEX::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWndEX::iwm_lbuttondown( mouse );
	
	switch ( m_btntype )
	{
	case BEXT_NORMAL:
		break;

	case BEXT_TOGGLE:
		m_toggled = !m_toggled;
		break;

	case BEXT_RADIO:
		{
			if ( !m_toggled ) {
				m_toggled = true;
			}
		}
		break;
	}

	if ( !m_soundclick.IsEmpty() ) {
		g_SoundMgr.PlaySound( m_soundclick );
	}
}

void	ioButtonWndEX::iwm_mouseover( const ioMouse& mouse )
{
	ioWndEX::iwm_mouseover( mouse );

	if ( !m_soundover.IsEmpty() ) {
		g_SoundMgr.PlaySound( m_soundover );
	}
}

//----------------------------------------------------------------------------------
/*
Draw virtual
*/	
void	ioButtonWndEX::DrawNormal( int x, int y )
{
	if ( m_normal ) {
		m_normal->Render( x, y );
	}
}

void	ioButtonWndEX::DrawOvered( int x, int y )
{
	if ( HasExWndStyle( IWS_EX_OVER_SCREEN ) )
	{
		if ( m_over )
		{
			m_over->Render( x, y );
			m_over->Render( x, y, UI_RENDER_SCREEN );
		}
	}
	else
	{
		if ( m_over ) {
			m_over->Render( x, y );
		}
	}

	DrawOveredAdd( x, y );
}

void	ioButtonWndEX::DrawOveredAdd( int x, int y )
{
	if ( m_overadd ) {
		m_overadd->Render( x, y );
	}
}

void	ioButtonWndEX::DrawPushed( int x, int y )
{
	if ( HasExWndStyle( IWS_EX_PUSH_SCREEN ) )
	{
		if ( m_push )
		{
			m_push->Render( x, y );
			m_push->Render( x, y, UI_RENDER_SCREEN );
		}
	}
	else if ( HasExWndStyle( IWS_EX_PUSH_DOUBLE ) )
	{
		if( m_push )
		{
			m_push->Render( x, y );
			m_push->Render( x, y );
		}
	}
	else if ( HasExWndStyle( IWS_EX_PUSHED_OVER_SCREEN ) )
	{
		if ( m_push )
		{
			m_push->Render( x, y );

			if ( m_bOver ) {
				m_push->Render( x, y, UI_RENDER_SCREEN );
			}
		}
	}
	else
	{
		if ( m_push ) {
			m_push->Render( x, y );
		}
	}
}

void	ioButtonWndEX::DrawDisabled( int x, int y )
{
	if ( m_disable ) {
		m_disable->Render( x, y );
	} else {
		DrawNormal( x, y );
	}
}

void	ioButtonWndEX::DrawExDisabledNormal( int x, int y )
{
	if ( m_exdisable_normal ) {
		m_exdisable_normal->Render( x, y );
	} else {
		DrawNormal( x, y );
	}
}

void	ioButtonWndEX::DrawExDisabledOvered( int x, int y )
{
	if ( HasExWndStyle( IWS_EX_OVER_SCREEN ) )
	{
		if ( m_exdisable_normal )
		{
			m_exdisable_normal->Render( x, y );
			m_exdisable_normal->Render( x, y, UI_RENDER_SCREEN );
		}
	}
	else
	{
		if ( m_exdisable_normal ) {
			m_exdisable_normal->Render( x, y );
		}
	}

	DrawOveredAdd( x, y );
}

void	ioButtonWndEX::DrawExDisabledPushed( int x, int y )
{
	if ( HasExWndStyle( IWS_EX_PUSH_SCREEN ) )
	{
		if ( m_exdisable_push )
		{
			m_exdisable_push->Render( x, y );
			m_exdisable_push->Render( x, y, UI_RENDER_SCREEN );
		}
	}
	else if ( HasExWndStyle( IWS_EX_PUSH_DOUBLE ) )
	{
		if ( m_exdisable_push )
		{
			m_exdisable_push->Render( x, y );
			m_exdisable_push->Render( x, y );
		}
	}
	else if ( HasExWndStyle( IWS_EX_PUSHED_OVER_SCREEN ) )
	{
		if ( m_exdisable_push )
		{
			m_exdisable_push->Render( x, y );

			if ( m_bOver ) {
				m_exdisable_push->Render( x, y, UI_RENDER_SCREEN );
			}
		}
	}
	else
	{
		if ( m_exdisable_push ) {
			m_exdisable_push->Render( x, y );
		} else {
			DrawPushed( x, y );
		}
	}
}

void	ioButtonWndEX::DrawBack( int x, int y )
{
	if ( m_back ) {
		m_back->Render( x, y );
	}
}

//----------------------------------------------------------------------------------
/*
Text Print
*/
void	ioButtonWndEX::PrintNormalTitle( int x, int y )
{
	if ( m_label )
	{
		m_label->GetRenderPosForWndEX( x, y, GetWidth(), GetHeight(), x, y );
		m_label->PrintNormal( x, y );
	}
}

void	ioButtonWndEX::PrintOverTitle( int x, int y )
{
	if ( m_label )
	{
		m_label->GetRenderPosForWndEX( x, y, GetWidth(), GetHeight(), x, y );
		m_label->PrintOver( x, y );
	}
}

void	ioButtonWndEX::PrintPushTitle( int x, int y )
{
	if ( m_label )
	{
		m_label->GetRenderPosForWndEX( x, y, GetWidth(), GetHeight(), x, y );
		m_label->PrintPush( x, y );
	}
}

void	ioButtonWndEX::PrintDisableTitle( int x, int y )
{
	if ( m_label )
	{
		m_label->GetRenderPosForWndEX( x, y, GetWidth(), GetHeight(), x, y );

		if ( m_label->GetTextStyle() != m_label->GetDisableTextStyle() ) 
		{
			if ( m_label->GetDisableTextStyle() == TS_NORMAL ) {
				m_label->PrintDisable( x + 2, y + 2 );
			} else {
				m_label->PrintDisable( x, y );
			}
		}
		else
		{
			m_label->PrintDisable( x, y );	
		}
	}
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioButtonWndEX::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioButtonWndEX* bwnd = dynamic_cast<ioButtonWndEX*>(wnd);
	if ( !bwnd ) {
		return;
	}

	if ( m_label ) {
		*(bwnd->m_label) = *m_label;
	}

	bwnd->m_btntype = m_btntype;
}

void	ioButtonWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );

	extxml.SetIntAttribute( "ButtonType", GetButtonEXType() );

	if ( m_label )
	{
		if ( m_label->GetText() || strcmp( m_label->GetText(), "" ) != 0 )
		{
			ioXMLElement framexml = extxml.CreateChild( "LabelInfo" );

			framexml.SetStringAttribute( "Text", GetLabelText() );
			framexml.SetIntAttribute( "Size", GetLabelTextSize() );		
			framexml.SetIntAttribute( "Alpha", GetLabelTextAlpha() );
			framexml.SetIntAttribute( "Style", GetLabelTextStyle() );
			framexml.SetIntAttribute( "DisableStyle", GetLabelTextDisableStyle() );
			framexml.SetIntAttribute( "VertAlign", GetLabelVertAlign() );
			framexml.SetIntAttribute( "HorzAlign", GetLabelHorzAlign() );
			framexml.SetIntAttribute( "Gap", GetLabelTextGap() );
			framexml.SetIntAttribute( "PrintTextStyle", GetLabelPrintTextStyle() );
			framexml.SetFloatAttribute( "PrintWidthCutSize", GetLabelPrintWidthCutSize() );
			// color
			framexml.SetStringAttribute( "NormalColor", ioWnd::ConvertToColorString( GetLabelNormalTextColor() ).c_str() );
			framexml.SetStringAttribute( "NormalBKColor", ioWnd::ConvertToColorString( GetLabelNormalTextBKColor() ).c_str() );
			framexml.SetStringAttribute( "OverColor", ioWnd::ConvertToColorString( GetLabelOverTextColor() ).c_str() );
			framexml.SetStringAttribute( "OverBKColor", ioWnd::ConvertToColorString( GetLabelOverTextBKColor() ).c_str() );
			framexml.SetStringAttribute( "PushColor", ioWnd::ConvertToColorString( GetLabelPushTextColor() ).c_str() );
			framexml.SetStringAttribute( "PushBKColor", ioWnd::ConvertToColorString( GetLabelPushTextBKColor() ).c_str() );
			framexml.SetStringAttribute( "DisableColor", ioWnd::ConvertToColorString( GetLabelDisableTextColor() ).c_str() );
			framexml.SetStringAttribute( "DisableBKColor", ioWnd::ConvertToColorString( GetLabelDisableTextBKColor() ).c_str() );
		}
	}
}

void	ioButtonWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	ioWndEX::_ParseExtraInfo( extxml );

	SetButtonEXType( static_cast<BUTTONEX_TYPE>( extxml.GetIntAttribute( "ButtonType" ) ) );

	ioXMLElement elem = extxml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "LabelInfo" ) == 0 )
		{
			const char* text = elem.GetStringAttribute( "Text" );
			if ( text || strcmp( text, "" ) != 0 )
			{
				SetLabelText( text, "" );
				SetLabelVertAlign( static_cast<TextVertAlign>( elem.GetIntAttribute( "VertAlign" ) ) );
				SetLabelHorzAlign( static_cast<TextAlignType>( elem.GetIntAttribute( "HorzAlign" ) ) );
				SetLabelTextStyle( static_cast<TextStyle>( elem.GetIntAttribute( "Style" ) ) );
				SetLabelTextDisableStyle( static_cast<TextStyle>( elem.GetIntAttribute( "DisableStyle" ) ) );
				SetLabelTextSize( elem.GetIntAttribute( "Size" ) );
				SetLabelTextAlpha( elem.GetIntAttribute( "Alpha" ) );
				SetLabelTextGap( elem.GetIntAttribute( "Gap" ) );
				SetLabelPrintTextStyle( static_cast<ioUITitle::PrintTextStyle>( elem.GetIntAttribute( "PrintTextStyle" ) ) );
				SetLabelPrintWidthCutSize( elem.GetFloatAttribute( "PrintWidthCutSize" ) );
				// color state
				SetLabelNormalTextColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "NormalColor" ) ) );
				SetLabelNormalTextBKColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "NormalBKColor" ) ) );
				SetLabelOverTextColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "OverColor" ) ) );
				SetLabelOverTextBKColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "OverBKColor" ) ) );
				SetLabelPushTextColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "PushColor" ) ) );
				SetLabelPushTextBKColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "PushBKColor" ) ) );
				SetLabelDisableTextColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "DisableColor" ) ) );
				SetLabelDisableTextBKColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "DisableBKColor" ) ) );
			}
			break;			
		}
		elem = elem.NextSibling();
	}
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioFrameButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioFrameButtonWndEX::ioFrameButtonWndEX( ioGUIManager* mgr )
	: ioButtonWndEX( IWEXT_FRAMEBUTTON, mgr )
{
}

ioFrameButtonWndEX::ioFrameButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioButtonWndEX( type, mgr )
{
}

ioFrameButtonWndEX::~ioFrameButtonWndEX()
{
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioFrameButtonWndEX::Copy( ioWnd* wnd )
{
	ioButtonWndEX::Copy( wnd );

	ioFrameButtonWndEX* fbwnd = dynamic_cast<ioFrameButtonWndEX*>(wnd);
	if ( !fbwnd ) {
		return;
	}

	CLONEFRAME( fbwnd, m_normal );
	CLONEFRAME( fbwnd, m_over );
	CLONEFRAME( fbwnd, m_overadd );
	CLONEFRAME( fbwnd, m_push );
	CLONEFRAME( fbwnd, m_back );
	CLONEFRAME( fbwnd, m_disable );
	CLONEFRAME( fbwnd, m_exdisable_normal );
	CLONEFRAME( fbwnd, m_exdisable_push );
}

void	ioFrameButtonWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioButtonWndEX::_SetSaveData( extxml );

	SetFrameDataToXML( m_normal, extxml, "NormalFrame" );
	SetFrameDataToXML( m_over, extxml, "OverFrame" );
	SetFrameDataToXML( m_overadd, extxml, "OverAddFrame" );
	SetFrameDataToXML( m_push, extxml, "PushFrame" );
	SetFrameDataToXML( m_back, extxml, "BackFrame" );
	SetFrameDataToXML( m_disable, extxml, "DisableFrame" );
	SetFrameDataToXML( m_exdisable_normal, extxml, "ExDisableNormalFrame" );
	SetFrameDataToXML( m_exdisable_push, extxml, "ExDisablePushFrame" );
}

void	ioFrameButtonWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioButtonWndEX::_ParseExtraInfo( extxml );

	SetFrameDataFromXML( &m_normal, extxml, "NormalFrame", m_guimgr );
	SetFrameDataFromXML( &m_over, extxml, "OverFrame", m_guimgr );
	SetFrameDataFromXML( &m_overadd, extxml, "OverAddFrame", m_guimgr );
	SetFrameDataFromXML( &m_push, extxml, "PushFrame", m_guimgr );
	SetFrameDataFromXML( &m_back, extxml, "BackFrame", m_guimgr );
	SetFrameDataFromXML( &m_disable, extxml, "DisableFrame", m_guimgr );
	SetFrameDataFromXML( &m_exdisable_normal, extxml, "ExDisableNormalFrame", m_guimgr );
	SetFrameDataFromXML( &m_exdisable_push, extxml, "ExDisablePushFrame", m_guimgr );

	RecalcSize();
}

//----------------------------------------------------------------------------------
/*
편의
*/
void	ioFrameButtonWndEX::SetFrameDataToXML( ioUIRenderElement* elem, ioXMLElement& extxml, const char* tagname )
{
	ioUIRenderFrame* frame = dynamic_cast<ioUIRenderFrame*>( elem );
	if ( frame && tagname )
	{
		ioXMLElement framexml = extxml.CreateChild( tagname );

		framexml.SetStringAttribute( "Template", frame->GetName().c_str() );
		framexml.SetIntAttribute( "RenderType", frame->GetRenderType() );
		framexml.SetIntAttribute( "GrayRenderType", frame->GetGrayRenderType() );
		framexml.SetIntAttribute( "TextureFilterOption", frame->GetTextureFilterOption() );		
		framexml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( frame->GetColor() ).c_str() );
	}
}

void	ioFrameButtonWndEX::SetFrameDataFromXML( ioUIRenderElement** element, ioXMLElement& extxml, const char* tagname, ioGUIManager* mgr )
{
	if ( !mgr || !tagname || !element ) {
		return;
	}

	ioXMLElement elem = extxml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), tagname ) == 0 )
		{			
			ioWndEX::ChangeFrame( reinterpret_cast<ioUIRenderFrame**>(element), elem.GetStringAttribute( "Template" ), mgr );
			if ( *element )
			{
				( *element )->SetRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "RenderType" ) ) );
				( *element )->SetGrayRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "GrayRenderType" ) ) );
				( *element )->SetTextureFilterOption( static_cast<TextureFilterOption>( elem.GetIntAttribute( "TextureFilterOption" ) ) );			
				( *element )->SetColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "Color" ) ) );				
			}
			break;
		}
		elem = elem.NextSibling();
	}
}




//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioImageButtonWndEX
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioImageButtonWndEX::ioImageButtonWndEX( ioGUIManager* mgr )
	: ioButtonWndEX( IWEXT_IMAGEBUTTON, mgr )
{
}

ioImageButtonWndEX::ioImageButtonWndEX( IOWNDEX_TYPE type, ioGUIManager* mgr )
	: ioButtonWndEX( type, mgr )
{
}

ioImageButtonWndEX::~ioImageButtonWndEX()
{
}

//----------------------------------------------------------------------------------
/*
Controls
*/
void	ioImageButtonWndEX::SetImageSetAll( const char* imagesetname, const char* imagename, ioGUIManager* mgr )
{
	SetImageSetNormal( imagesetname, imagename, mgr );
	SetImageSetOver( imagesetname, imagename, mgr );
	SetImageSetOverAdd( imagesetname, imagename, mgr );
	SetImageSetPush( imagesetname, imagename, mgr );
	SetImageSetBack( imagesetname, imagename, mgr );
	SetImageSetDisable( imagesetname, imagename, mgr );
	SetImageSetExDisableNormal( imagesetname, imagename, mgr );
	SetImageSetExDisablePush( imagesetname, imagename, mgr );
}

void	ioImageButtonWndEX::SetImageSetAll( const char* fullname )
{
	SetImageSetNormal( fullname );
	SetImageSetOver( fullname );
	SetImageSetOverAdd( fullname );
	SetImageSetPush( fullname );
	SetImageSetBack( fullname );
	SetImageSetDisable( fullname );
	SetImageSetExDisableNormal( fullname );
	SetImageSetExDisablePush( fullname );
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioImageButtonWndEX::Copy( ioWnd* wnd )
{
	ioButtonWndEX::Copy( wnd );

	ioImageButtonWndEX* ibwnd = dynamic_cast<ioImageButtonWndEX*>(wnd);
	if ( !ibwnd ) {
		return;
	}

	CLONEIMAGE( ibwnd, m_normal );
	CLONEIMAGE( ibwnd, m_over );
	CLONEIMAGE( ibwnd, m_overadd );
	CLONEIMAGE( ibwnd, m_push );
	CLONEIMAGE( ibwnd, m_back );
	CLONEIMAGE( ibwnd, m_disable );
	CLONEIMAGE( ibwnd, m_exdisable_normal );
	CLONEIMAGE( ibwnd, m_exdisable_push );
}

void	ioImageButtonWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioButtonWndEX::_SetSaveData( extxml );

	SetImageDataToXML( m_normal, extxml, "NormalImage" );
	SetImageDataToXML( m_over, extxml, "OverImage" );
	SetImageDataToXML( m_overadd, extxml, "OverAddImage" );
	SetImageDataToXML( m_push, extxml, "PushImage" );
	SetImageDataToXML( m_back, extxml, "BackImage" );
	SetImageDataToXML( m_disable, extxml, "DisableImage" );
	SetImageDataToXML( m_exdisable_normal, extxml, "ExDisableNormalImage" );
	SetImageDataToXML( m_exdisable_push, extxml, "ExDisablePushImage" );
}

void	ioImageButtonWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	if ( !m_guimgr ) {
		return;
	}

	ioButtonWndEX::_ParseExtraInfo( extxml );

	SetImageDataFromXML( &m_normal, extxml, "NormalImage", m_guimgr );
	SetImageDataFromXML( &m_over, extxml, "OverImage", m_guimgr );
	SetImageDataFromXML( &m_overadd, extxml, "OverAddImage", m_guimgr );
	SetImageDataFromXML( &m_push, extxml, "PushImage", m_guimgr );
	SetImageDataFromXML( &m_back, extxml, "BackImage", m_guimgr );
	SetImageDataFromXML( &m_disable, extxml, "DisableImage", m_guimgr );
	SetImageDataFromXML( &m_exdisable_normal, extxml, "ExDisableNormalImage", m_guimgr );
	SetImageDataFromXML( &m_exdisable_push, extxml, "ExDisablePushImage", m_guimgr );

	RecalcSize();
}

//----------------------------------------------------------------------------------
/*
편의
*/
void	ioImageButtonWndEX::SetImageDataToXML( ioUIRenderElement* elem, ioXMLElement& extxml, const char* tagname )
{
	ioUIRenderImage* image = dynamic_cast<ioUIRenderImage*>( elem );
	if ( image && tagname )
	{
		ioXMLElement framexml = extxml.CreateChild( tagname );		
		framexml.SetStringAttribute( "FileName", image->GetImageFileName().c_str() );
		framexml.SetStringAttribute( "ImageSetName",image->GetImageSetName().c_str() );
		framexml.SetStringAttribute( "ImageName", image->GetImageName().c_str() );
		
		framexml.SetIntAttribute( "OffsetX", image->GetEXOffsetX() );
		framexml.SetIntAttribute( "OffsetY", image->GetEXOffsetY() );
		framexml.SetIntAttribute( "RenderType", image->GetRenderType() );
		framexml.SetIntAttribute( "GrayRenderType", image->GetGrayRenderType() );
		framexml.SetIntAttribute( "TextureFilterOption", image->GetTextureFilterOption() );
		framexml.SetIntAttribute( "ReserveFormat", image->GetReverseFormatting() );
		framexml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( image->GetColor() ).c_str() );
	}
}

void	ioImageButtonWndEX::SetImageDataFromXML( ioUIRenderElement** element, ioXMLElement& extxml, const char* tagname, ioGUIManager* mgr )
{
	if ( !mgr || !tagname || !element ) {
		return;
	}

	ioXMLElement elem = extxml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), tagname ) == 0 )
		{
			const char* filename = elem.GetStringAttribute( "FileName" );
			if ( filename && strcmp( filename, "" ) != 0 ) {
				ioWndEX::ChangeImageByFileName( reinterpret_cast<ioUIRenderImage**>(element), filename, mgr );
			}
			else {
				const char* imgsetname = elem.GetStringAttribute( "ImageSetName" );
				const char* imgname = elem.GetStringAttribute( "ImageName" );
				if( imgsetname && imgname && strcmp( imgsetname, "" ) != 0 && strcmp( imgname, "" ) != 0  ) {
					ioWndEX::ChangeImageByImageSet( reinterpret_cast<ioUIRenderImage**>(element), elem.GetStringAttribute( "ImageSetName" ), elem.GetStringAttribute( "ImageName" ), mgr );
				}
			}

			if ( *element )
			{
				dynamic_cast<ioUIRenderImage*>( *element )->SetEXOffsetX( elem.GetIntAttribute( "OffsetX" ) );
				dynamic_cast<ioUIRenderImage*>( *element )->SetEXOffsetY( elem.GetIntAttribute( "OffsetY" ) );
				( *element )->SetRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "RenderType" ) ) );
				( *element )->SetGrayRenderType( static_cast<UIRenderType>( elem.GetIntAttribute( "GrayRenderType" ) ) );
				( *element )->SetTextureFilterOption( static_cast<TextureFilterOption>( elem.GetIntAttribute( "TextureFilterOption" ) ) );
				dynamic_cast<ioUIRenderImage*>( *element )->SetReverseFormatting( static_cast<ioUIRenderImage::ReverseFormatting>( elem.GetIntAttribute( "ReserveFormat" ) ) );
				( *element )->SetColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "Color" ) ) );
				dynamic_cast<ioUIRenderImage*>( *element )->SetUseOffset( false );
				dynamic_cast<ioUIRenderImage*>( *element )->SetAutoSize( true );
			}
			break;
		}
		elem = elem.NextSibling();
	}
}