
//----------------------------------------------------------------------------------
/*
Filename	: ioRichLabel.cpp
Author		: megamegy
Date		: 2013.10.16
Desc		: 
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioRichLabel.h"
#include "ioFontManager.h"
#include "ioStringConverter.h"
#include "ioXMLElement.h"

//----------------------------------------------------------------------------------
void	ChangeCharSet( char *szText, char cDst, char cSrc )
{
	int iLen = strlen( szText );
	for(int i = 0;i < iLen;i++)
	{
		if( szText[i] == cDst )
			szText[i] = cSrc;
	}
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioRichLabel
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioRichLabel::ioRichLabel()
	: ioWndEX( IWEXT_RICHLABEL )
	, m_horztype( TAT_LEFT )
	, m_verttype( TVA_BOTTOM )
	, m_macroscopic_verttype( TVA_TOP )
	, m_linefeedtype( LINEFEED_TYPE_NORMAL )
	, m_vertgap( 0.0f )
	, m_textheight( 0.0f )
	, m_curralpharate( MAX_ALPHA_RATE )
{
	m_inputinfo.Reset();
}

ioRichLabel::ioRichLabel( IOWNDEX_TYPE type )
	: ioWndEX( type )
	, m_horztype( TAT_LEFT )
	, m_verttype( TVA_BOTTOM )
	, m_macroscopic_verttype( TVA_TOP )
	, m_linefeedtype( LINEFEED_TYPE_NORMAL )
	, m_vertgap( 0.0f )
	, m_textheight( 0.0f )
{
	m_inputinfo.Reset();
}

ioRichLabel::~ioRichLabel()
{
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioRichLabel::CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight )
{
	ioWndEX::CheckFrameReSize( iWidth, iHeight, iPrevWidth, iPrevHeight );

	RecalcTextLine();
}

void	ioRichLabel::ParseExtraInfo( ioXMLElement& xElement )
{	
	SetText( xElement );
}

void	ioRichLabel::SetWindowAlpha( int iAlpha )
{
	ioWnd::SetWindowAlpha( iAlpha );
	m_curralpharate = iAlpha;
}

//----------------------------------------------------------------------------------
/*
Text
*/
std::wstring	Str2WStr( const std::string& str )
{
	wchar_t wbuff[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, wbuff, MAX_PATH );
	return std::wstring( wbuff );
}

std::string		WStr2Str( const std::wstring& wstr )
{ 
	char buff[ MAX_PATH ];
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, buff, MAX_PATH, 0, FALSE );
	return std::string( buff );
}

void	ioRichLabel::AddText( int size, TextStyle style, DWORD color, const char* txt, bool submitinputinfo )
{
	if ( !txt ) {
		return;
	}

	AddText( size, style, color, std::string( txt ), submitinputinfo );
}

void	ioRichLabel::AddText( int size, TextStyle style, DWORD color, const std::string& txt, bool submitinputinfo )
{
	if ( m_lines.empty() ) {
		m_lines.push_back( Line() );
	}

	std::string buff( txt );
	while ( !buff.empty() )
	{
		size_t f = buff.find( "\n" );
		if ( f >= buff.size() )
		{
			AddTextDetail( size, style, color, Str2WStr( buff ) );
			break;
		}
		
		std::string tmp = buff.substr( 0, f );
		buff.erase( 0, f + 1 );
		AddTextDetail( size, style, color, Str2WStr( tmp ) );
		m_lines.push_back( Line() );
	}

	if ( submitinputinfo ) {
		m_inputinfo.m_txtblock.push_back( TextBlock( txt, size, style, color ) );
	}
	
	m_textheight = 0.0f;
	LineVec::iterator iter = m_lines.begin();
	LineVec::iterator eiter = m_lines.end();
	for ( ; iter != eiter; ++iter ) {
		m_textheight += ( iter->m_maxheight + m_vertgap );
	}
}

void	ioRichLabel::AddText( Line& line, int size, TextStyle style, DWORD color, const std::string& txt )
{
	float scale = size / static_cast<float>( g_FontMgr.GetFontSize() );

	line.m_printer.SetBkColor( 0, 0, 0 );
	line.m_printer.SetTextStyle( style );
	line.m_printer.SetTextColor( color );
	//line.m_printer.AddTextPieceWithoutXOffset( size, txt.c_str() );
	line.m_printer.AddTextPiece( scale, txt.c_str() );
	line.m_maxheight = std::max< float >( line.m_maxheight, g_FontMgr.GetTextHeight( txt.c_str(), style, scale ) + ( ( size / 10 ) * 2 ) ); // 약 크기가 10당 2의 offset을 주어야 height가 맞는듯 하다.	
}

void	ioRichLabel::AddTextDetail( int size, TextStyle style, DWORD color, const std::wstring& txt )
{
	std::wstring txtbuf = txt;

	int idx = m_lines.size() - 1;

	float scale = size / static_cast<float>( g_FontMgr.GetFontSize() );

	m_lines[ idx ].m_printer.UpdateFullWidth();
	float width = m_lines[ idx ].m_printer.GetFullWidth();
	int rcwidth = GetWidth();
	float space = rcwidth - width;

	float txtwidth = g_FontMgr.GetTextWidth( WStr2Str( txtbuf ).c_str(), style, scale );
	if ( txtwidth <= space )
	{
		AddText( m_lines[ idx ], size, style, color, WStr2Str( txtbuf ) );
		return;
	}

	int numtxt = 1;
	float charwidth = ( txtbuf.length() == 0 ) ? 0.0f : ( txtwidth / txtbuf.length() );
	if ( charwidth < space ) {
		numtxt = ( charwidth == 0.0f ) ? 1 : static_cast<int>( space / charwidth );
	}

	std::wstring ftxt = txtbuf.substr( 0, numtxt );
	txtbuf.erase( 0, numtxt );
	AddText( m_lines[ idx ], size, style, color, WStr2Str( ftxt ) );

	if ( txtbuf.empty() ) {
		return;
	}

	m_lines.push_back( Line() );
	AddTextDetail( size, style, color, txtbuf );
}

void	ioRichLabel::RemoveText( int i )
{
	if ( i < 0 ) return;
	if ( i > static_cast<int>( m_inputinfo.m_txtblock.size() ) ) return;

	int cnt = 0;
	for ( TextBlockVec::iterator iter = m_inputinfo.m_txtblock.begin(); iter != m_inputinfo.m_txtblock.end(); ++iter, ++cnt ) 
	{
		if ( cnt == i )
		{
			m_inputinfo.m_txtblock.erase( iter );
			break;
		}
	}

	m_lines.clear();

	RecalcTextLine();
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	ioRichLabel::Clear()
{
	m_horztype = TAT_LEFT;
	m_verttype = TVA_BOTTOM;
	m_macroscopic_verttype = TVA_TOP;
	m_linefeedtype = LINEFEED_TYPE_NORMAL;
	m_vertgap = 0.0f;
	m_lines.clear();
	m_inputinfo.Reset();
}

void	ioRichLabel::ClearText()
{
	m_lines.clear();
	m_inputinfo.Reset();
}

void	ioRichLabel::SetText( ioXMLElement& xElement )
{
	Clear();

	SetHorzAlignType( ioStringConverter::ParseTextAlignType( xElement.GetStringAttribute( "HorzAlign" ) ) );
	SetVertAlignType( ioStringConverter::ParseTextVertAlign( xElement.GetStringAttribute( "VertAlign" ) ) );
	SetMSVertAlignType( ioStringConverter::ParseTextVertAlign( xElement.GetStringAttribute( "MSVertAlign" ) ) );
	m_vertgap = xElement.GetIntAttribute( "VertGap" );
	
	// 텍스트를 추가한다.
	ioXMLElement child = xElement.FirstChild();
	while ( !child.IsEmpty() )
	{			
		if ( strcmp( child.GetTagName(), "TextBlock" ) == 0 ) 
		{			
			char txt[ MAX_PATH ];
			strcpy_s( txt, child.GetStringAttribute( "Text" ) );
			ChangeCharSet( txt, '#', '\n' );

			TextStyle style = ioStringConverter::ParseTextStyle( child.GetStringAttribute( "Style" ) );
			DWORD color = ioStringConverter::ParseColor( child.GetStringAttribute( "Color" ) );			

			AddText( child.GetIntAttribute( "Size" ), style, color, txt, true );
		}
		else if ( strcmp( child.GetTagName(), "Format" ) == 0 )
		{
			SetText( child.GetStringAttribute( "Param" ) );
			break;
		}

		child = child.NextSibling();
	}
}

void	ioRichLabel::SetText( const std::string& param )
{
	Clear();

	std::string buff = param;

	const char* piecemark = "%#%";
	int pmlen = strlen( piecemark );
	const char* blockmark = "$#$";
	int bmlen = strlen( blockmark );
	while ( !buff.empty() )
	{
		size_t pos = buff.find( blockmark );
		if ( buff.empty() || pos >= buff.size() ) {
			break;
		}

		std::string block = buff.substr( 0, pos );
		buff.erase( 0, pos + bmlen );

		// scale
		size_t ppos = block.find( piecemark );
		if ( block.size() <= ppos ) {
			continue;
		}
		//float scale = dynamic_cast<float>( atoi( block.substr( 0, ppos ).c_str() ) ) / dynamic_cast<float>( g_FontMgr.GetFontSize() );
		int size = atoi( block.substr( 0, ppos ).c_str() );
		block.erase( 0, ppos + pmlen );

		// style
		ppos = block.find( piecemark );
		if ( block.size() <= ppos ) {
			continue;
		}
		TextStyle style = ioStringConverter::ParseTextStyle( block.substr( 0, ppos ).c_str() );
		block.erase( 0, ppos + pmlen );

		// color
		ppos = block.find( piecemark );
		if ( block.size() <= ppos ) {
			continue;
		}
		DWORD color = ioStringConverter::ParseColor( block.substr( 0, ppos ).c_str() );
		block.erase( 0, ppos + pmlen );

		// text
		ppos = block.find( piecemark );
		if ( block.size() <= ppos ) {
			continue;
		}
		char txt[ MAX_PATH ];
		strcpy_s( txt, block.substr( 0, ppos ).c_str() );
		ChangeCharSet( txt, '#', '\n' );

		AddText( size, style, color, txt, false );
	}

	m_inputinfo.m_param = param;
}

void	ioRichLabel::RecalcTextLine()
{
	if ( m_inputinfo.IsParamType() )
	{
		SetText( m_inputinfo.m_param );
	}
	else
	{
		m_lines.clear();
		TextBlockVec::iterator iter = m_inputinfo.m_txtblock.begin();
		TextBlockVec::iterator eiter = m_inputinfo.m_txtblock.end();
		for ( ; iter != eiter; ++iter ) {
			AddText( iter->m_size, iter->m_style, iter->m_color, iter->m_text, false );
		}
	}
}

ioRichLabel::TextBlock*	ioRichLabel::GetInputInfo( int i ) 
{
	if ( i < 0 ) return 0;
	if ( i >= GetNumInputInfo() ) return 0;

	return &( m_inputinfo.m_txtblock[ i ] ); 
}

const char*		ioRichLabel::GetInputInfoText( int i )
{
	if ( !GetInputInfo( i ) ) return 0;
	
	return GetInputInfo( i )->m_text.c_str(); 
}

int		ioRichLabel::GetInputInfoSize( int i )
{
	if ( !GetInputInfo( i ) ) return 0;
	
	return GetInputInfo( i )->m_size;
}

TextStyle	ioRichLabel::GetInputInfoStyle( int i )
{
	if ( !GetInputInfo( i ) ) return TS_NORMAL;

	return GetInputInfo( i )->m_style;
}

DWORD	ioRichLabel::GetInputInfoColor( int i )
{
	if ( !GetInputInfo( i ) ) return 0xFFFFFFFF;

	return GetInputInfo( i )->m_color;
}

void	ioRichLabel::SetInputInfoText( int i, const char* text )
{
	if ( !text ) return;
	if ( !GetInputInfo( i ) ) return;

	char txt[ MAX_PATH ];
	strcpy_s( txt, text );
	ChangeCharSet( txt, '#', '\n' );

	GetInputInfo( i )->m_text = txt;
}

void	ioRichLabel::SetInputInfoSize( int i, int size )
{
	if ( !GetInputInfo( i ) ) return;

	GetInputInfo( i )->m_size = size;
}

void	ioRichLabel::SetInputInfoStyle( int i, TextStyle style )
{
	if ( !GetInputInfo( i ) ) return;

	GetInputInfo( i )->m_style = style;
}

void	ioRichLabel::SetInputInfoColor( int i, DWORD color )
{
	if ( !GetInputInfo( i ) ) return;

	GetInputInfo( i )->m_color = color;
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioRichLabel::OnRender()
{
	int x = GetDerivedPosX();
	int y = GetDerivedPosY();

	switch ( m_macroscopic_verttype )
	{
	case TVA_CENTER:
		y -= ( m_textheight * FLOAT05 );
		break;

	case TVA_BOTTOM:
		y -= ( m_textheight );
		break;
	}

	TextVertAlign prevva = g_FontMgr.GetVertAlign();	
	g_FontMgr.SetVertAlign( m_verttype );
	
	LineVec::iterator iter = m_lines.begin();
	LineVec::iterator eiter = m_lines.end();
	for ( ; iter != eiter; ++iter )
	{
		float ry = y;
		switch ( m_verttype )
		{
		case TVA_CENTER:
			ry += ( iter->m_maxheight * FLOAT05 );
			break;
		case TVA_BOTTOM:
			ry += iter->m_maxheight;
			break;
		}

		float rx = x;
		switch ( m_horztype )
		{
		case TAT_CENTER:
			{
				float dist = GetWidth() - iter->m_printer.GetFullWidth();
				rx += dist * FLOAT05;
			}
			break;
		case TAT_RIGHT:
			{
				float dist = GetWidth() - iter->m_printer.GetFullWidth();
				rx += dist;
			}
			break;
		}
				
		iter->m_printer.PrintFullText( rx, ry, TAT_LEFT, m_curralpharate );
		y += ( iter->m_maxheight + m_vertgap );
	}

	g_FontMgr.SetVertAlign( prevva );
}

//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioRichLabel::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );
	
	extxml.SetStringAttribute( "HorzAlign", ioStringConverter::ParseTextAlignType( GetHorzAlignType() ).c_str() );
	extxml.SetStringAttribute( "VertAlign", ioStringConverter::ParseTextVertAlign( GetVertAlignType() ).c_str() );
	extxml.SetStringAttribute( "MSVertAlign", ioStringConverter::ParseTextVertAlign( GetMSVertAlignType() ).c_str() );
	extxml.SetIntAttribute( "VertGap", m_vertgap );

	if ( m_inputinfo.IsParamType() )
	{
		ioXMLElement formatxml = extxml.CreateChild( "Format" );
		formatxml.SetStringAttribute( "Param", m_inputinfo.m_param.c_str() );
	}
	else
	{
		TextBlockVec::iterator iter = m_inputinfo.m_txtblock.begin();
		TextBlockVec::iterator eiter = m_inputinfo.m_txtblock.end();
		for ( ; iter != eiter; ++iter )
		{
			ioXMLElement txtblockxml = extxml.CreateChild( "TextBlock" );

			char txt[ MAX_PATH ];
			strcpy_s( txt, iter->m_text.c_str() );
			ChangeCharSet( txt, '\n', '#' );

			txtblockxml.SetStringAttribute( "Text", txt );
			txtblockxml.SetIntAttribute( "Size", iter->m_size );
			txtblockxml.SetStringAttribute( "Style", ConvertToStyleString( iter->m_style ).c_str() );
			txtblockxml.SetStringAttribute( "Color", ioStringConverter::ParseColor( iter->m_color ).c_str() );
		}
	}
}

void	ioRichLabel::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioRichLabel* wndex = dynamic_cast<ioRichLabel*>( wnd );
	if ( wndex )
	{
		wndex->m_horztype = m_horztype;
		wndex->m_verttype = m_verttype;
		wndex->m_macroscopic_verttype = m_macroscopic_verttype;
		wndex->m_linefeedtype = m_linefeedtype;
		wndex->m_vertgap = m_vertgap;
		wndex->m_textheight = m_textheight;
		wndex->m_inputinfo = m_inputinfo;
		wndex->RecalcTextLine();
	}
}