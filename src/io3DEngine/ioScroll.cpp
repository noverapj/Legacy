

#include "stdafx.h"

#include "ioWndType.h"
#include "ioWnd.h"
#include "ioUIRenderImage.h"


#include "ioScroll.h"
#include "ioMouse.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "ioFrameTimer.h"

#define DEFAULT_SLIDE_SIZE            30
#define SLIDEBAR_OFFSET               10
ioSlide::ioSlide()
{
	m_pSlideIcon = NULL;
}

ioSlide::~ioSlide()
{
	SAFEDELETE( m_pSlideIcon );
}

void ioSlide::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon_Slide" )
	{
		SAFEDELETE( m_pSlideIcon );
		m_pSlideIcon = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioSlide::SetSaveData( ioXMLElement& xml )
{
	ioWnd::SetSaveData( xml );

	// custom
	ioXMLElement elem = xml.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if ( strcmp( elem.GetTagName(), "Imagery" ) == 0 )
		{
			SetImagery( elem, m_pSlideIcon, "Icon_Slide" );
			break;
		}
		elem = elem.NextSibling();
	}
}
void	ioSlide::Copy( ioWnd* wnd )
{
	ioWnd::Copy( wnd );

	ioSlide* slwnd = dynamic_cast<ioSlide*>( wnd );
	if ( slwnd ) {
		CLONEIMAGE( slwnd, m_pSlideIcon );
	}
}
static const char* g_ioslidefixedimage[ 1 ] = {
	"Icon_Slide"
};
int ioSlide::GetNumFixedRenderImage() const
{
	return countof( g_ioslidefixedimage );
}
const char* ioSlide::GetFixedRenderImageType( int index )
{
	if ( index < 0 || index >= GetNumFixedRenderImage() ) {
		return NULL;
	}
	return g_ioslidefixedimage[ index ];
}
ioUIRenderElement** ioSlide::GetRenderImage( const char* type )
{
	if ( strcmp( type, "Icon_Slide" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pSlideIcon);
	}
	return 0;
}
//************************************************************************************************************************

void ioSlide::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );
	if( m_pParent )
		m_pParent->iwm_command( this, IOBN_BTNDOWN, 0 );
}

void ioSlide::iwm_lbuttonup( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttonup( mouse );
	if( m_pParent )
		m_pParent->iwm_command( this, IOBN_BTNUP, 0 );
}

void ioSlide::ReSizeSlide( int iWidth, int iHeight )
{
	ioWnd::SetSize( iWidth, iHeight );
	RECT rcDragRect = { 0, 0, iWidth, iHeight };
	ioWnd::SetDragRect( rcDragRect );
}

void ioSlide::OnRender() 
{
	if( ioWnd::HasWndStyle( IWS_INACTIVE ) )
	{
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();
		
		RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );
		
		iXPos += ( GetWidth() / 2 ) + 1;
		iYPos += ( GetHeight() / 2 ) + 1;
		
		if( m_pSlideIcon )
		{
			m_pSlideIcon->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
			m_pSlideIcon->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pSlideIcon->Render( iXPos, iYPos );
		}
	}
	else
	{
		ioWnd::OnRender();
		
		int iXPos, iYPos;
		iXPos = GetDerivedPosX() + ( GetWidth() / 2 ) + 1;
		iYPos = GetDerivedPosY() + ( GetHeight() / 2 ) + 1;
		
		if( m_pSlideIcon )
		{
			m_pSlideIcon->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
			m_pSlideIcon->SetVertFormatting( ioUIRenderImage::VF_CENTER );
			m_pSlideIcon->Render( iXPos, iYPos );
		}
	}	
}

ioScroll::ioScroll()
{
	m_iMinPos		= 0;
	m_iMaxPos		= 1;
	m_iCurPos		= 0;
	m_iPageRange    = 0;
	m_dwStartTime	= 0;
	m_dwCurTime		= 0;
	m_dwScrollTime  = 0.0f;
	m_dwCheckTime   = 0;

	m_pScrollUpNDown = NULL;
}

ioScroll::~ioScroll()
{
}

DWORD ioScroll::GetWndType() const
{
	return IWT_SCROLL;
}

void ioScroll::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iPageRange  = xElement.GetIntAttribute( "PageRange" );
	m_dwStartTime = xElement.GetIntAttribute( "StartTime" );
	m_dwCurTime	  = xElement.GetIntAttribute( "CurTime" );
}

void ioScroll::iwm_create()
{
	if( HasExWndStyle( IWS_EX_HSCROLL ) )
	{
		m_rcSlide.left	= SLIDEBAR_OFFSET;
		m_rcSlide.right = GetWidth() - SLIDEBAR_OFFSET;
		m_rcSlide.top	= 0;
		m_rcSlide.bottom= GetHeight();
	}
	else if( HasExWndStyle( IWS_EX_VSCROLL ) )
	{
		m_rcSlide.left	= 0;
		m_rcSlide.right = GetWidth();
		m_rcSlide.top	= SLIDEBAR_OFFSET;
		m_rcSlide.bottom= GetHeight() - SLIDEBAR_OFFSET;
	}	
}

void ioScroll::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_SCROLL_UP:
		if( cmd == IOBN_BTNDOWN )
		{
			SetScrollPos( m_iCurPos - 1 );
			m_pScrollUpNDown = pWnd;
			m_dwScrollTime   = FRAMEGETTIME();
			m_dwCheckTime    = m_dwStartTime;

			if( m_pParent )
				m_pParent->iwm_command( this, IOBN_BTNDOWN, ID_SCROLL_UP );
		}
		break;
	case ID_SCROLL_DOWN:
		if( cmd == IOBN_BTNDOWN )
		{
			SetScrollPos( m_iCurPos + 1 );
			m_pScrollUpNDown = pWnd;
			m_dwScrollTime   = FRAMEGETTIME();
			m_dwCheckTime    = m_dwStartTime;

			if( m_pParent )
				m_pParent->iwm_command( this, IOBN_BTNDOWN, ID_SCROLL_DOWN );
		}
		break;
	case ID_SCROLL_THUMB:
		if( cmd == IOWN_MOVED )
		{
			SetScrollSlide( pWnd );
		}
		else if( cmd == IOBN_BTNDOWN )
		{
			if( m_pParent )
				m_pParent->iwm_command( this, cmd, ID_SCROLL_THUMB );
		}
		break;
	}
}

void ioScroll::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWnd::iwm_lbuttondown( mouse );

	
	POINT pt = { mouse.GetMousePos().x, mouse.GetMousePos().y };
	RECT rcDerived;
	rcDerived.left	= GetDerivedPosX() + m_rcSlide.left;
	rcDerived.top	= GetDerivedPosY() + m_rcSlide.top;
	rcDerived.right = rcDerived.left + ( m_rcSlide.right - m_rcSlide.left );
	rcDerived.bottom= rcDerived.top  + ( m_rcSlide.bottom - m_rcSlide.top );
	if( PtInRect( &rcDerived, pt ) )
	{
		// 슬라이드바 이동
		if( HasExWndStyle( IWS_EX_HSCROLL ) )
		{
			int iThumbSize = m_rcSlide.right - m_rcSlide.left;		
			float fThumbGap  = (float)iThumbSize / max( m_iMaxPos, 1 );			
			float fPos = (float)max( 0, ( mouse.GetMousePos().x - rcDerived.left ) ) / max( fThumbGap, 1.0f );

			int iNewPos = fPos + FLOAT05;
			if( m_iCurPos < iNewPos )
				SetNextPage();
			else if( iNewPos < m_iCurPos )
				SetPrePage();

			//SetScrollPos( iNewPos );
		}
		else if( HasExWndStyle( IWS_EX_VSCROLL ) )
		{
			int iThumbSize = m_rcSlide.bottom - m_rcSlide.top;
			float fThumbGap  = (float)iThumbSize / max( m_iMaxPos, 1 );

			float fPos = (float)max( 0, ( mouse.GetMousePos().y - rcDerived.top ) ) / max( fThumbGap, 1.0f );

			int iNewPos = fPos + FLOAT05;
			if( m_iCurPos < iNewPos )
				SetNextPage();
			else if( iNewPos < m_iCurPos )
				SetPrePage();
			
			//SetScrollPos( iNewPos );
		}
	}	
	if( m_pParent )
		m_pParent->iwm_command( this, IOBN_BTNDOWN, 0 );
}

void ioScroll::SetActive()
{
	ioWnd::SetActive();

	SetChildActive( ID_SCROLL_UP );
	SetChildActive( ID_SCROLL_DOWN );
	SetChildActive( ID_SCROLL_THUMB );
}

void ioScroll::SetInActive()
{
	ioWnd::SetInActive();

	SetChildInActive( ID_SCROLL_UP );
	SetChildInActive( ID_SCROLL_DOWN );
	SetChildInActive( ID_SCROLL_THUMB );
}

void ioScroll::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	if( bCreate )
	{
		ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
	}
}

void ioScroll::SetScrollPos( int iPos )
{
	iPos = max( m_iMinPos, min( iPos, m_iMaxPos ) );

	m_iCurPos = iPos;

	float fRate = GetScrollRate();
	if( HasExWndStyle( IWS_EX_HSCROLL ) )
	{
		int iThumbSize = m_rcSlide.right - m_rcSlide.left;
		
		ioWnd *pThumb = FindChildWnd( ID_SCROLL_THUMB );
		if( pThumb )
		{
			int iTempX     = ( iThumbSize - pThumb->GetWidth() ) * fRate;
			pThumb->SetWndPos( m_rcSlide.left + iTempX, m_rcSlide.top );
			
			if( m_pParent )
				m_pParent->iwm_vscroll( m_dwID, m_iCurPos );
		}
	}
	else if( HasExWndStyle( IWS_EX_VSCROLL ) )
	{
		int iThumbSize = m_rcSlide.bottom - m_rcSlide.top;
				
		ioWnd *pThumb = FindChildWnd( ID_SCROLL_THUMB );
		if( pThumb )
		{
			int iTempY     = ( iThumbSize - pThumb->GetHeight() ) * fRate;
			pThumb->SetWndPos( m_rcSlide.left, m_rcSlide.top + iTempY );
			
			if( m_pParent )
				m_pParent->iwm_vscroll( m_dwID, m_iCurPos );
		}
	}	
}

void ioScroll::SetScrollSlide( ioWnd *pSlide )
{
	int iPos = m_iCurPos;

	if( ioWnd::HasWndStyle( IWS_INACTIVE ) )
	{
		float fRate = GetScrollRate();
		if( HasExWndStyle( IWS_EX_HSCROLL ) )
		{
			int iThumbSize = m_rcSlide.right - m_rcSlide.left;

			if( pSlide )
			{
				int iTempX     = ( iThumbSize - pSlide->GetWidth() ) * fRate;
				pSlide->SetWndPos( m_rcSlide.left + iTempX, m_rcSlide.top );
			}
		}
		else if( HasExWndStyle( IWS_EX_VSCROLL ) )
		{
			int iThumbSize = m_rcSlide.bottom - m_rcSlide.top;

			if( pSlide )
			{
				int iTempY     = ( iThumbSize - pSlide->GetHeight() ) * fRate;
				pSlide->SetWndPos( m_rcSlide.left, m_rcSlide.top + iTempY );
			}
		}	
	}
	else
	{
		if( HasExWndStyle( IWS_EX_HSCROLL ) )
		{
			int iThumbSize = m_rcSlide.right - m_rcSlide.left;
			int iXPos      = pSlide->GetXPos();
			int iYPos      = m_rcSlide.top;

			// 
			if( iXPos < m_rcSlide.left )
				iXPos = m_rcSlide.left;
			else if( iXPos + pSlide->GetWidth() >= m_rcSlide.right )
				iXPos = m_rcSlide.right - pSlide->GetWidth();
			pSlide->SetWndPos( iXPos, iYPos );

			if( iThumbSize - pSlide->GetWidth() <= 0 ) return;

			iXPos = pSlide->GetXPos();
			float fRatio = (float)( iXPos - m_rcSlide.left ) / 
				(float)( iThumbSize - pSlide->GetWidth() );
			iPos = fRatio * m_iMaxPos;
			iPos = max( m_iMinPos, min( iPos, m_iMaxPos ) );	
		}
		else if( HasExWndStyle( IWS_EX_VSCROLL ) )
		{
			int iThumbSize = m_rcSlide.bottom - m_rcSlide.top;
			int iXPos      = m_rcSlide.left;
			int iYPos      = pSlide->GetYPos();

			// 
			if( iYPos < m_rcSlide.top )
				iYPos = m_rcSlide.top;
			else if( iYPos + pSlide->GetHeight() >= m_rcSlide.bottom )
				iYPos = m_rcSlide.bottom - pSlide->GetHeight();
			pSlide->SetWndPos( iXPos, iYPos );

			if( iThumbSize - pSlide->GetHeight() <= 0 ) return;

			iYPos = pSlide->GetYPos();
			float fRatio = (float)( iYPos - m_rcSlide.top ) / 
				(float)( iThumbSize - pSlide->GetHeight() );
			iPos = fRatio * m_iMaxPos;
			iPos = max( m_iMinPos, min( iPos, m_iMaxPos ) );		
		}

		if( iPos == m_iCurPos )	return;

		m_iCurPos = iPos;
		if( m_pParent )
			m_pParent->iwm_vscroll( m_dwID, m_iCurPos );
	}
}

float ioScroll::GetScrollRate() const
{
	float fRate = 0.0f;
	
	if( m_iMaxPos > 0 )
	{
		fRate = (float)m_iCurPos / (float)m_iMaxPos;
	}

	return fRate;
}

void ioScroll::SetScrollRange( int iMinPos, int iMaxPos )
{
	m_iMinPos = iMinPos;
	m_iMaxPos = ( iMaxPos - iMinPos ) - m_iPageRange;
	if( iMaxPos <= m_iPageRange )
		m_iMaxPos = 0;
	else
		m_iMaxPos = max( 1, m_iMaxPos );

	if( m_iMaxPos == 0 )
	{
		SetInActive();
	}
	else
	{
		SetActive();
	}		
	
	m_iCurPos = max( m_iMinPos, min( m_iCurPos, m_iMaxPos ) );

	if( HasExWndStyle( IWS_EX_HSCROLL ) )
	{
		int iThumbSize = m_rcSlide.right - m_rcSlide.left;
		
		ioSlide *pThumb = (ioSlide*)FindChildWnd( ID_SCROLL_THUMB );
		if( pThumb )
		{
			int iRealSize = iThumbSize;
			if( m_iMaxPos > 0 )
				iRealSize = ( iThumbSize / iMaxPos ) * m_iPageRange;
			if( iRealSize < DEFAULT_SLIDE_SIZE )
				iRealSize = DEFAULT_SLIDE_SIZE;
			pThumb->ReSizeSlide( iRealSize, pThumb->GetHeight() );
		}
		
		ioWnd *pDownBtn = FindChildWnd( ID_SCROLL_DOWN );
		if( pDownBtn )
			pDownBtn->SetWndPos( GetWidth() - pDownBtn->GetWidth(), 0 );
	}
	else if( HasExWndStyle( IWS_EX_VSCROLL ) )
	{
		int iThumbSize = m_rcSlide.bottom - m_rcSlide.top;
		
		ioSlide *pThumb = (ioSlide*)FindChildWnd( ID_SCROLL_THUMB );
		if( pThumb )
		{
			int iRealSize = iThumbSize;
			if( m_iMaxPos > 0 )
				iRealSize = ( iThumbSize / iMaxPos ) * m_iPageRange;
			if( iRealSize < DEFAULT_SLIDE_SIZE )
				iRealSize = DEFAULT_SLIDE_SIZE;
			pThumb->ReSizeSlide( pThumb->GetWidth(), iRealSize );
		}

		ioWnd *pDownBtn = FindChildWnd( ID_SCROLL_DOWN );
		if( pDownBtn )
			pDownBtn->SetWndPos( 0, GetHeight() - pDownBtn->GetHeight() );
	}	
}

void ioScroll::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_pScrollUpNDown && 
		m_pScrollUpNDown->IsClicked() )
	{
		if( FRAMEGETTIME() - m_dwScrollTime >= m_dwCheckTime )	
		{
			iwm_command( m_pScrollUpNDown, IOBN_BTNDOWN, 0 );
			m_dwCheckTime = m_dwCurTime;
		}
	}	
}

void ioScroll::SetPrePage()
{
	SetScrollPos( m_iCurPos - m_iPageRange );
}

void ioScroll::SetNextPage()
{
	SetScrollPos( m_iCurPos + m_iPageRange );
}

//************************************************************************************************************************
// Tool을 위한 인터페이스들
void	ioScroll::SetSaveData( ioXMLElement& xml )
{
	ioWnd::SetSaveData( xml );

	// ExtraInfo
	ioXMLElement xmlextra = xml.CreateChild( "ExtraInfo" );
	xmlextra.SetIntAttribute( "PageRange", m_iPageRange );
	xmlextra.SetIntAttribute( "StartTime", m_dwStartTime );
	xmlextra.SetIntAttribute( "CurTime", m_dwCurTime );
}
void	ioScroll::Copy( ioWnd* wnd )
{
	ioWnd::Copy( wnd );

	ioScroll* scwnd = dynamic_cast<ioScroll*>( wnd );
	if ( scwnd )
	{
		scwnd->m_iMinPos = m_iMinPos;
		scwnd->m_iMaxPos = m_iMinPos;
		scwnd->m_iCurPos = m_iCurPos;
		scwnd->m_iPageRange = m_iPageRange;
		scwnd->m_rcSlide = m_rcSlide;
		scwnd->m_dwStartTime = m_dwStartTime;
		scwnd->m_dwCurTime = m_dwCurTime;
		scwnd->m_dwCheckTime = m_dwCheckTime;
		scwnd->m_dwScrollTime = m_dwScrollTime;
		if ( m_pScrollUpNDown ) {
			scwnd->m_pScrollUpNDown = m_pScrollUpNDown->Clone( true );
		}
	}
}
//************************************************************************************************************************