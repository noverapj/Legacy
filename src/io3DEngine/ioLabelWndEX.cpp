//----------------------------------------------------------------------------------
/*
Filename	: ioLabelWndEX.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioLabelWndEX.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioLabelWndEX::ioLabelWndEX()
	: ioWndEX( IWEXT_LABEL )
	, m_label( 0 )
	, m_alignstyle( ASE_RECT )
	, m_labeloffsetx( 0 )
	, m_labeloffsety( 0 )
{
	m_label = new ioUITitle();
}

ioLabelWndEX::ioLabelWndEX( IOWNDEX_TYPE type )
	: ioWndEX( type )
	, m_label( 0 )
	, m_alignstyle( ASE_RECT )
	, m_labeloffsetx( 0 )
	, m_labeloffsety( 0 )
{
	m_label = new ioUITitle();
}

ioLabelWndEX::~ioLabelWndEX()
{
	SAFEDELETE( m_label );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioLabelWndEX::OnRender()
{
	if ( m_label )
	{
		int printx = 0;
		int printy = 0;

		switch ( m_alignstyle )
		{
		case ASE_RECT:
			m_label->GetRenderPosForWndEX( GetDerivedPosX(), GetDerivedPosY(), GetWidth(), GetHeight(), printx, printy );
			break;

		case ASE_POINT:
			printx = GetDerivedPosX();
			printy = GetDerivedPosY();
			break;
		}

		m_label->PrintNormal( printx + m_labeloffsetx, printy + m_labeloffsety );
	}
}

void	ioLabelWndEX::SetWindowAlpha( int alpha )
{
	ioWndEX::SetWindowAlpha( alpha );

	if ( m_label ) {
		m_label->SetAlpha( alpha );
	}
}

 //----------------------------------------------------------------------------------
/*
Controls
*/
void	ioLabelWndEX::SetLabelTextColor( DWORD color )
{
	if ( m_label ) {
		m_label->SetNormalTitleTextColor( color );
	}
}

DWORD	ioLabelWndEX::GetLabelTextColor() const
{
	if ( m_label ) {
		return m_label->GetNormalTextColor();
	}
	return 0;
}

void	ioLabelWndEX::SetLabelTextBKColor( DWORD color )
{
	if ( m_label ) {
		m_label->SetNormalTitleBKColor( color );
	}
}

DWORD	ioLabelWndEX::GetLabelTextBKColor() const
{
	if ( m_label ) {
		return m_label->GetNormalTitleColor().dwBk;
	}
	return 0;
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioLabelWndEX::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioLabelWndEX* labelwnd = dynamic_cast<ioLabelWndEX*>(wnd);
	if ( !labelwnd ) {
		return;
	}

	if ( m_label ) {
		*(labelwnd->m_label) = *m_label;
	}

	labelwnd->m_alignstyle = m_alignstyle;
	labelwnd->m_labeloffsetx = m_labeloffsetx;
	labelwnd->m_labeloffsety = m_labeloffsety;
}

void	ioLabelWndEX::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );

	extxml.SetIntAttribute( "AlignStyle", m_alignstyle );
	extxml.SetIntAttribute( "LabelOffsetX", m_labeloffsetx );
	extxml.SetIntAttribute( "LabelOffsetY", m_labeloffsety );

	if ( m_label )
	{
		if ( m_label->GetText() || strcmp( m_label->GetText(), "" ) != 0 ) 
		{
			ioXMLElement framexml = extxml.CreateChild( "LabelInfo" );

			framexml.SetStringAttribute( "Text", GetLabelText() );
			framexml.SetIntAttribute( "Size", GetLabelTextSize() );
			framexml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( GetLabelTextColor() ).c_str() );
			framexml.SetStringAttribute( "BKColor", ioWnd::ConvertToColorString( GetLabelTextBKColor() ).c_str() );
			framexml.SetIntAttribute( "Alpha", GetLabelTextAlpha() );
			framexml.SetIntAttribute( "Style", GetLabelTextStyle() );
			framexml.SetIntAttribute( "DisableStyle", GetLabelTextDisableStyle() );
			framexml.SetIntAttribute( "VertAlign", GetLabelVertAlign() );
			framexml.SetIntAttribute( "HorzAlign", GetLabelHorzAlign() );
			framexml.SetIntAttribute( "Gap", GetLabelTextGap() );
			framexml.SetIntAttribute( "PrintTextStyle", GetLabelPrintTextStyle() );
			framexml.SetFloatAttribute( "PrintWidthCutSize", GetLabelPrintWidthCutSize() );
		}
	}
}

void	ioLabelWndEX::_ParseExtraInfo( ioXMLElement& extxml )
{
	ioWndEX::_ParseExtraInfo( extxml );

	SetAlignStyle( static_cast< ALIGN_STYLE_EX >( extxml.GetIntAttribute( "AlignStyle" ) ) );
	SetLabelOffsetX( extxml.GetIntAttribute( "LabelOffsetX" ) );
	SetLabelOffsetY( extxml.GetIntAttribute( "LabelOffsetY" ) );

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
				SetLabelTextColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "Color" ) ) );
				SetLabelTextBKColor( ioStringConverter::ParseColor( elem.GetStringAttribute( "BKColor" ) ) );
				SetLabelTextStyle( static_cast<TextStyle>( elem.GetIntAttribute( "Style" ) ) );
				SetLabelTextDisableStyle( static_cast<TextStyle>( elem.GetIntAttribute( "DisableStyle" ) ) );
				SetLabelTextSize( elem.GetIntAttribute( "Size" ) );
				SetLabelTextAlpha( elem.GetIntAttribute( "Alpha" ) );
				SetLabelTextGap( elem.GetIntAttribute( "Gap" ) );
				SetLabelPrintTextStyle( static_cast<ioUITitle::PrintTextStyle>( elem.GetIntAttribute( "PrintTextStyle" ) ) );
				SetLabelPrintWidthCutSize( elem.GetFloatAttribute( "PrintWidthCutSize" ) );
			}
			break;
		}
		elem = elem.NextSibling();
	}
}