

#include "stdafx.h"

#include "ioUIRenderImage.h"
#include "ioProgressBar.h"

#include "ioStringConverter.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"

ioProgressBar::ioProgressBar()
{
	m_iCurPos = 0;
	m_iMaxPos = 100;

	m_FillAlign = PA_LEFT;

	m_pBack  = NULL;
	m_pGraph = NULL;

	m_dwGraphColor = 0xFFFFFFFF;
}

ioProgressBar::~ioProgressBar()
{
	SAFEDELETE( m_pBack );
	SAFEDELETE( m_pGraph );
}

void ioProgressBar::SetCurPosition( int iCurPos )
{
	m_iCurPos = max( 0, min( iCurPos, m_iMaxPos ) );
}

void ioProgressBar::SetMaxPosition( int iMaxPos )
{
	m_iMaxPos = max( 1, iMaxPos );
}

void ioProgressBar::SetProgressAlign( ProgressAlign eAlign )
{
	m_FillAlign = eAlign;
}

void ioProgressBar::SetProgressAlign( const ioHashString &szAlign )
{
	ProgressAlign eAlign = PA_LEFT;
	if( szAlign == "Left" )
		eAlign = PA_LEFT;
	else if( szAlign == "Right" )
		eAlign = PA_RIGHT;

	SetProgressAlign( eAlign );
}

void ioProgressBar::SetGraphColor( DWORD dwColor )
{
	m_dwGraphColor = dwColor;
}

void ioProgressBar::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Back" )
	{
		SAFEDELETE( m_pBack );
		m_pBack = pImage;
	}
	else if( szType == "Graph" )
	{
		SAFEDELETE( m_pGraph );
		m_pGraph = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioProgressBar::SetSaveData( ioXMLElement& xml )
{
	ioWnd::SetSaveData( xml );

	// custom
	ioXMLElement elem = xml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "Imagery" ) == 0 )
		{
			SetImagery( elem, m_pGraph, "Graph" );
			SetImagery( elem, m_pBack, "Back" );
			break;
		}
		elem = elem.NextSibling();
	}

	// ExtraInfo
	ioXMLElement xmlextra = xml.CreateChild( "ExtraInfo" );
	xmlextra.SetStringAttribute( "GraphColor", ioWnd::ConvertToColorString( m_dwGraphColor ).c_str() );	
	xmlextra.SetIntAttribute( "MaxPos", m_iMaxPos );
	if ( m_FillAlign == PA_RIGHT ) {
		xmlextra.SetStringAttribute( "Align", "Right" );
	} else {
		xmlextra.SetStringAttribute( "Align", "Left" );
	}
}
void	ioProgressBar::Copy( ioWnd* wnd )
{
	ioWnd::Copy( wnd );

	ioProgressBar* pgwnd = dynamic_cast<ioProgressBar*>( wnd );
	if ( pgwnd )
	{
		pgwnd->m_iCurPos = m_iCurPos;
		pgwnd->m_iMaxPos = m_iMaxPos;
		pgwnd->m_FillAlign = m_FillAlign;
		CLONEIMAGE( pgwnd, m_pBack );
		CLONEIMAGE( pgwnd, m_pGraph );
	}
}
static const char* g_ioprogressbarfixedimage[ 2 ] = {
	"Back",
	"Graph",
};
int ioProgressBar::GetNumFixedRenderImage() const
{
	return countof( g_ioprogressbarfixedimage );
}
const char* ioProgressBar::GetFixedRenderImageType( int index )
{
	if ( index < 0 || index >= GetNumFixedRenderImage() ) {
		return NULL;
	}
	return g_ioprogressbarfixedimage[ index ];
}
ioUIRenderElement** ioProgressBar::GetRenderImage( const char* type )
{
	if ( strcmp( type, "Back" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pBack);
	} else if ( strcmp( type, "Graph" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pGraph);
	}
	return 0;
}
//************************************************************************************************************************

void ioProgressBar::ParseExtraInfo( ioXMLElement &xElement )
{
	DWORD dwGraphColor = ioStringConverter::ParseColor( xElement.GetStringAttribute( "GraphColor" ) );
	SetGraphColor( dwGraphColor );

	SetProgressAlign( xElement.GetStringAttribute( "Align" ) );
	SetMaxPosition( xElement.GetIntAttribute( "MaxPos" ) );
}

bool ioProgressBar::IsCanMouseReaction() const
{
	return false;
}

DWORD ioProgressBar::GetWndType() const
{
	return IWT_PROGRESS;
}

void ioProgressBar::OnRender()
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_pBack )
	{
		m_pBack->Render( iXPos, iYPos );
	}

	if( m_pGraph )
	{
		float fRate = (float)m_iCurPos/(float)m_iMaxPos;
		fRate = max( 0.0f, min( fRate, 1.0f ) );

		int iWidthStart, iWidthEnd;
		if( m_FillAlign == PA_LEFT )
		{
			iWidthStart = 0;
			iWidthEnd = m_pGraph->GetWidth() * fRate;
		}
		else	// PA_RIGHT
		{
			iWidthStart = m_pGraph->GetWidth() * ( 1.0f - fRate );
			iWidthEnd = m_pGraph->GetWidth();
		}

		if( iWidthStart < iWidthEnd )
		{
			m_pGraph->RenderWidthCut( iXPos,
									  iYPos,
									  iWidthStart,
									  iWidthEnd );
		}
	}
}