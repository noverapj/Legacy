
#include "stdafx.h"

#include "ioMouse.h"
#include "ioXMLElement.h"
#include "ioXMLDocument.h"

#include "ioWndType.h"
#include "ioWnd.h"
#include "ioEdit.h"
#include "ioButton.h"

#include "ioUIRenderElement.h"
#include "ioUIRenderFrame.h"
#include "ioUIRenderImage.h"
#include "ioUITitle.h"

#include "ioGUIManager.h"
#include "ErrorReport.h"
#include "ioStringConverter.h"
#include "ioFrameTimer.h"
#include "ioUI3DEffectRender.h"

using namespace std;

#define DEFAULT_LAYER       100
#define Z_MODAL_LAYER_TOOLTIP	5
#define Z_MODAL_LAYER		4
#define Z_TOPMOST_LAYER		3
#define Z_NORMAL_LAYER		2
#define Z_BACKMOST_LAYER	1

struct WindowLess
{
	bool operator()( const ioWnd *pLeft, const ioWnd *pRight ) const
	{
		if( pLeft->GetZOrderLayer() < pRight->GetZOrderLayer() )
		{
			return true;
		}
		else if( pLeft->GetZOrderLayer() == pRight->GetZOrderLayer() )
		{
			if( pLeft->GetLayer() < pRight->GetLayer() )
				return true;
		}

		return false;
	}
};

float ioWnd::m_fScreenRateX = 1.0f;
float ioWnd::m_fScreenRateY = 1.0f;

ioWnd* ioWnd::m_pTopModalWnd = NULL;
DWORD  ioWnd::m_dwSelectedID = 0;

ioWnd::ioWnd()
{
	m_dwID					= 0;
    m_dwStyle				= 0; 
	m_dwExStyle				= 0;
	m_dwExStyleParam		= 0;
	m_dwInnerUniqueIndex	= 0;

	m_dwLayer       = DEFAULT_LAYER;

    SetRectEmpty( &m_rcPos );
	m_pHitRect = NULL;

	m_pParent		= NULL;
	m_pPreModalWnd	= NULL;
	m_pNextModalWnd = NULL;

	m_bNowGoToTopReserving = false;
	
	m_pUITitle = NULL;

	m_bShow			= false;
	m_bClicked		= false;
	m_bOver			= false;
	m_bRightClicked = false;

	m_bCheckParentOutSide = false;

	m_XPosAlign = UI_LEFT_ALIGN;
	m_YPosAlign = UI_TOP_ALIGN;
	m_iDefXPos = 0;
	m_iDefYPos = 0;
	m_iDefWidth  = 0;
	m_iDefHeight = 0;
	m_bWidthParent  = false;
	m_bHeightParent = false;

	m_dwWndAniTime = 0;
	m_pSelectedFrm          = NULL;
	m_pMultiplyFrm			= NULL;
	m_pSelectedTitleFrm     = NULL;

	m_idname			= "";
	m_membername		= "";
	m_classname			= "";
	m_subclassname		= "";
	m_stWndName			= "";

	m_exceptfromid = false;

	m_bFocusParentType = false;
	m_bBind = false;

	m_bTemplate = false;
}

ioWnd::~ioWnd()
{
	EraseInModalList();

	UIElementList::iterator iter=m_ElementList.begin();
	for( ; iter!=m_ElementList.end() ; ++iter )
	{
		delete *iter;
	}
	m_ElementList.clear();
	m_NamedImageList.clear();
	m_TooltipList.clear();

	SAFEDELETE( m_pUITitle );
	SAFEDELETE( m_pHitRect );

	DeleteAllChild();

	SAFEDELETE( m_pSelectedFrm );
	SAFEDELETE( m_pMultiplyFrm );
	SAFEDELETE( m_pSelectedTitleFrm );

	for( auto iterEffect = m_pUI3DEffect.begin() ; iterEffect != m_pUI3DEffect.end() ; ++iterEffect )
		SAFEDELETE( *iterEffect );

	while(!m_pTextList.empty())
	{
		std::list<IoUIText*>::iterator it = m_pTextList.begin();
		IoUIText* P = (IoUIText*)(*it);
		SAFEDELETE(P);
		m_pTextList.erase(it);
	}

	ioTemplateList::iterator iterTP;
	for( iterTP = m_TemplateList.begin() ; iterTP != m_TemplateList.end() ; ++iterTP )
		SAFEDELETE( *iterTP );

	m_TemplateList.clear();
}


void ioWnd::SetWndName( const ioHashString &szWndName )
{
	m_stWndName = szWndName;
}


const ioHashString& ioWnd::GetWndName()
{
	return m_stWndName;
}

void ioWnd::ClearRenderImageList()
{
	UIElementList::iterator iter=m_ElementList.begin();
	for( ; iter!=m_ElementList.end() ; ++iter )
	{
		delete *iter;
	}
	m_ElementList.clear();
}

void ioWnd::ClearElementImageList()
{
	UIElementList::iterator iter=m_ElementList.begin();
	for( ; iter!=m_ElementList.end() ; ++iter )
	{
		delete *iter;
	}
	m_ElementList.clear();
	m_NamedImageList.clear();	
		
	SAFEDELETE( m_pSelectedFrm );
	SAFEDELETE( m_pMultiplyFrm );
	SAFEDELETE( m_pSelectedTitleFrm );
}

void ioWnd::SetScreenRate( float fRateX, float fRateY )
{
	m_fScreenRateX = fRateX;
	m_fScreenRateY = fRateY;
}

ioUITitle* ioWnd::CreateUITitle()
{
	SAFEDELETE( m_pUITitle );
	m_pUITitle = new ioUITitle;

	return m_pUITitle;
}

IoUIText* ioWnd::CreateUIText()
{
	IoUIText* pText = new IoUIText;
	m_pTextList.push_back(pText);
	return pText;
}

void ioWnd::SetTitleText( const char *szTitle, const char *szDelim, int iGap )
{
	if( m_pUITitle )
	{
		m_pUITitle->SetFontGap( iGap );
		m_pUITitle->SetText( szTitle, szDelim );
	}
}

void ioWnd::SetTitleColor( DWORD dwTextColor, DWORD dwBkColor )
{
	if( m_pUITitle )
	{
		m_pUITitle->SetAllColor( dwTextColor, dwBkColor );
	}
}

void ioWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "Selected" )
		{
			SAFEDELETE( m_pSelectedFrm );
			m_pSelectedFrm = pFrame;
		}
		else if( szType == "MultiplyFrm" )
		{
			SAFEDELETE( m_pMultiplyFrm );
			m_pMultiplyFrm = pFrame;
		}
		else if( szType == "SelectedTitle" )
		{
			SAFEDELETE( m_pSelectedTitleFrm );
			m_pSelectedTitleFrm = pFrame;
		}
		else
			m_ElementList.push_back( pFrame );
	}
}

void ioWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( pImage )
	{
		m_ElementList.push_back( pImage );
	}	
}

void ioWnd::AddTooltipHelp( const ioHashString &szTipHelp, const ioHashString &szTextAlign, const ioHashString &szTextColor )
{
	if( szTipHelp.IsEmpty() )
		return;
	
	Tooltip kTooltip;
	kTooltip.m_szHelp		= szTipHelp;
	kTooltip.m_TextAlign	= ioStringConverter::ParseTextAlignType( szTextAlign );
	kTooltip.m_dwTextColor  = ioStringConverter::ParseColor( szTextColor );	
	m_TooltipList.push_back( kTooltip );
}

void ioWnd::ParseExtraInfo( ioXMLElement &xElement )
{
}

void ioWnd::AddNamedRenderImage( const ioHashString &szName, ioUIRenderImage *pImage )
{
	NamedRenderImageMap::iterator iter = m_NamedImageList.find( szName );
	if( iter == m_NamedImageList.end() )
	{
		m_NamedImageList.insert( NamedRenderImageMap::value_type( szName, pImage ) );
		m_ElementList.push_back( pImage );
	}
	else
	{
		RemoveNamedRenderImage( szName );
		AddNamedRenderImage( szName, pImage );
	}
}

void ioWnd::RemoveNamedRenderImage( const ioHashString &szName )
{
	NamedRenderImageMap::iterator iter = m_NamedImageList.find( szName );
	if( iter == m_NamedImageList.end() )	return;

	UIElementList::iterator iElem;
	for( iElem=m_ElementList.begin() ; iElem!=m_ElementList.end() ; ++iElem )
	{
		if( *iElem == iter->second )
		{
			delete *iElem;
			m_ElementList.erase( iElem );
			break;
		}
	}

	m_NamedImageList.erase( iter );
}

ioUIRenderImage* ioWnd::GetNamedRenderImage( const ioHashString &szName ) const
{
	NamedRenderImageMap::const_iterator iter = m_NamedImageList.find( szName );
	if( iter != m_NamedImageList.end() )
		return iter->second;

	return NULL;
}

void ioWnd::SetWindowAlpha( int iAlpha )
{
	UIElementList::iterator iElem;
	for( iElem=m_ElementList.begin() ; iElem!=m_ElementList.end() ; ++iElem )
	{
		(*iElem)->SetAlpha( iAlpha );
	}

	if( m_pSelectedFrm )
		m_pSelectedFrm->SetAlpha( iAlpha );

	if( m_pMultiplyFrm )
		m_pMultiplyFrm->SetAlpha( iAlpha );

	if( m_pSelectedTitleFrm )
		m_pSelectedTitleFrm->SetAlpha( iAlpha );

	for( ioWndList::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter )
	{
		ioWnd* pWnd = *iter;
		if( pWnd )
			pWnd->SetWindowAlpha( iAlpha );
	}

	ioUITitle* pTitle = GetTitle();
	if( pTitle )	
		pTitle->SetAlpha( iAlpha );
}

void ioWnd::HeadInsertToModalList()
{
	if( !HasWndStyle( IWS_MODAL ) )
		return;

	m_pPreModalWnd  = m_pTopModalWnd;
	m_pNextModalWnd = NULL;

	if( m_pTopModalWnd )
		m_pTopModalWnd->m_pNextModalWnd = this;

	m_pTopModalWnd = this;
}

void ioWnd::EraseInModalList()
{
	if( !HasWndStyle( IWS_MODAL ) )
		return;

	if( m_pPreModalWnd )
		m_pPreModalWnd->m_pNextModalWnd = m_pNextModalWnd;

	if( m_pNextModalWnd )
		m_pNextModalWnd->m_pPreModalWnd = m_pPreModalWnd;

	if( m_pTopModalWnd == this )
	{
		m_pTopModalWnd = m_pPreModalWnd;
	}

	m_pPreModalWnd  = NULL;
	m_pNextModalWnd = NULL;
}

void ioWnd::ShowWnd()
{
	HeadInsertToModalList();

	if( !HasExWndStyle( IWS_EX_SHOW_NO_TOP ) )
		GoToTop();

	if( !IsShow() )
	{
		if( HasExWndStyle( IWS_EX_SHOW_HIDE_ACTION ) ) 
			m_dwWndAniTime = FRAMEGETTIME();
	}
	m_bShow = true;
	iwm_show();
}

void ioWnd::HideWnd()
{
	EraseInModalList();

	if( IsShow() )
	{
		if( HasExWndStyle( IWS_EX_SHOW_HIDE_ACTION ) ) 
			m_dwWndAniTime = FRAMEGETTIME();
	}

	m_bShow = false;
	iwm_hide();

	if( m_pParent )
	{
		m_pParent->iwm_command( this, IOWN_HIDE, 0 );
	}

	HideChildKillFocus();
}

void	ioWnd::ShowChildWnd()
{
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter )
	{
		(*iter)->ShowWnd();
		(*iter)->ShowChildWnd();
	}
}

void	ioWnd::HideChildWnd()
{
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter )
	{
		(*iter)->HideWnd();
		(*iter)->HideChildWnd();
	}
}

void ioWnd::SetActive()
{
	m_dwStyle &= ~IWS_INACTIVE;
}

void ioWnd::SetInActive()
{
	m_dwStyle |= IWS_INACTIVE;
}

void ioWnd::AddWndStyle( DWORD dwStyle )
{
	m_dwStyle |= dwStyle;
}

void ioWnd::RemoveWndStyle( DWORD dwStyle )
{
	m_dwStyle &= ~dwStyle;
}

void ioWnd::ShowChildWnd( DWORD dwID, bool bMustShow )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
	{
		if( bMustShow || !pChild->IsShow() )
		{
			pChild->ShowWnd();
		}
	}
}

void ioWnd::HideChildWnd( DWORD dwID, bool bMustHide )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
	{
		if( bMustHide || pChild->IsShow() )
		{
			pChild->HideWnd();
		}
	}
}

ioWnd* ioWnd::HideChildByEsc() 
{
	if( !HasChild() ) return NULL;

	ioWnd *pWnd = NULL;
	if( m_pTopModalWnd )
	{
		pWnd = m_pTopModalWnd;
		if( m_pTopModalWnd->iwm_esc() )		
			return pWnd;
		return NULL;
	}

	ioWndList::reverse_iterator rIter;
	for( rIter = m_ChildList.rbegin() ; rIter != m_ChildList.rend() ; ++rIter)
	{
		ioWnd *pChildWnd = *rIter;
		if( pChildWnd == NULL )	continue;
		if( !pChildWnd->IsShow() )	continue;
		if( pChildWnd->iwm_esc() )
			return pChildWnd;
	}	
	return NULL;
}

ioWnd* ioWnd::CommandChildBySpaceBar()
{
	if( !HasChild() ) return NULL;

	ioWnd *pWnd = NULL;
	if( m_pTopModalWnd )
	{
		pWnd = m_pTopModalWnd;
		if( m_pTopModalWnd->iwm_spacebar() )		
			return pWnd;
		return NULL;
	}

	ioWndList::reverse_iterator rIter;
	for( rIter = m_ChildList.rbegin() ; rIter != m_ChildList.rend() ; ++rIter)
	{
		ioWnd *pChildWnd = *rIter;
		if( pChildWnd == NULL )	continue;
		if( !pChildWnd->IsShow() )	continue;
		if( pChildWnd->iwm_spacebar() )
			return pChildWnd;
	}	
	return NULL;
}

void ioWnd::HideChildKillFocus()
{
	if( ioEdit::m_pKeyFocusEdit == this )
		((ioEdit*)this)->KillKeyFocus();

	if( ioEdit::m_pKeyFocusEdit == NULL ) return;
	if( !HasChild() ) return;

	ioWndList::reverse_iterator rIter;
	for( rIter = m_ChildList.rbegin() ; rIter != m_ChildList.rend() ; ++rIter)
	{
		ioWnd *pChildWnd = *rIter;

		if( pChildWnd == NULL )	continue;
		if( ioEdit::m_pKeyFocusEdit == NULL )
			break;

		pChildWnd->HideChildKillFocus();
	}
}

void ioWnd::SetChildActive( DWORD dwID )
{
	ioWnd *pChild = FindChildWnd( dwID );

	if( pChild )
		pChild->SetActive();
}

void ioWnd::SetChildInActive( DWORD dwID )
{
	ioWnd *pChild = FindChildWnd( dwID );

	if( pChild )
		pChild->SetInActive();
}

void ioWnd::SetChildWndStyleAdd( DWORD dwID, DWORD dwStyle )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
		pChild->AddWndStyle( dwStyle );
}

void ioWnd::SetChildWndStyleRemove( DWORD dwID, DWORD dwStyle )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
		pChild->RemoveWndStyle( dwStyle );
}

void ioWnd::SetCheckParentOutSide( bool bCheck )
{
	ioWnd *pParent = GetParent();
	if( pParent )
	{
		m_bCheckParentOutSide = bCheck;
		pParent->SetCheckParentOutSide( bCheck );
	}
}

void ioWnd::SetWndPos( int iXPos, int iYPos )
{
	int iWidth = GetWidth();
	int iHeight = GetHeight();

	m_rcPos.left	= iXPos;
	m_rcPos.top		= iYPos;
	m_rcPos.right	= iXPos + iWidth;
	m_rcPos.bottom  = iYPos + iHeight;
}

void ioWnd::SetScreenWndPos( int iXPos, int iYPos )
{
	if( m_pParent )
	{
		iXPos -= m_pParent->GetDerivedPosX();
		iYPos -= m_pParent->GetDerivedPosY();
	}

	SetWndPos( iXPos, iYPos );
}

void ioWnd::SetSize( int iWidth, int iHeight )
{
	int iPrevWidth = GetWidth();
	int iPrevHeight= GetHeight();

	m_rcPos.right  = GetXPos() + iWidth;
	m_rcPos.bottom = GetYPos() + iHeight;

	CheckFrameReSize( iWidth, iHeight, iPrevWidth, iPrevHeight );
}

void ioWnd::CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight )
{
	UIElementList::iterator iter;
	for( iter=m_ElementList.begin() ; iter!=m_ElementList.end() ; ++iter )
	{
		ioUIRenderFrame *pFrame = ToUIFrame( *iter );
		if( pFrame && pFrame->IsAutoReSize() )
		{
			pFrame->SetSize( iWidth, iHeight );
		}
	}

	if( m_pSelectedFrm && m_pSelectedFrm->IsAutoReSize() )
	{
		m_pSelectedFrm->SetSize( iWidth, iHeight );
	}

	if( m_pMultiplyFrm && m_pMultiplyFrm->IsAutoReSize() )
	{
		if( iPrevWidth > 0 && iPrevHeight > 0 )
		{
			// 비율에 맞게 조정
			float fWidthRate = (float)m_pMultiplyFrm->GetWidth() / (float)iPrevWidth;
			float fHeightRate= (float)m_pMultiplyFrm->GetHeight() / (float)iPrevHeight ;
			m_pMultiplyFrm->SetSize( (float)iWidth * fWidthRate, (float)iHeight * fHeightRate );
		}
	}

	if( m_pSelectedTitleFrm && m_pSelectedTitleFrm->IsAutoReSize() )
	{
		// Width만 늘린다.
		int iWidthOffSet = m_pSelectedTitleFrm->GetPosX();
		m_pSelectedTitleFrm->SetSize( iWidth - ( iWidthOffSet * 2 ), m_pSelectedTitleFrm->GetHeight() );
	}
}

void ioWnd::SetWndRect( const RECT &rcRect )
{
	SetWndPos( rcRect.left, rcRect.top );
	SetSize( rcRect.right - rcRect.left, rcRect.bottom - rcRect.top );
}

ioWnd::UIPosAlign ioWnd::ParseUIPosAlign( const ioHashString &szAlign )
{
	if( szAlign.IsEmpty() )
		return UI_NONE_ALIGN;

	if( szAlign == "Left" )
		return UI_LEFT_ALIGN;
	if( szAlign == "Right" )
		return UI_RIGHT_ALIGN;
	if( szAlign == "Center" )
		return UI_CENTER_ALIGN;
	if( szAlign == "Rate" )
		return UI_RATE_ALIGN;
	if( szAlign == "Top" )
		return UI_TOP_ALIGN;
	if( szAlign == "Bottom" )
		return UI_BOTTOM_ALIGN;

	return UI_NONE_ALIGN;
}

void ioWnd::SetDefWindowPos( ioXMLElement &rkElement )
{
	ioHashString szBuf;	

	szBuf = rkElement.GetStringAttribute( "XAlign" );
	m_XPosAlign = ParseUIPosAlign( szBuf );

	szBuf = rkElement.GetStringAttribute( "YAlign" );
	m_YPosAlign = ParseUIPosAlign( szBuf );

	m_iDefXPos = rkElement.GetIntAttribute( "X" );
	m_iDefYPos = rkElement.GetIntAttribute( "Y" );

	szBuf = rkElement.GetStringAttribute( "Width" );
	if( szBuf == "Parent" )
	{
		m_iDefWidth = 0;
		m_bWidthParent = true;
	}
	else
	{
		m_iDefWidth = rkElement.GetIntAttribute( "Width" );
		m_bWidthParent = false;
	}

	szBuf = rkElement.GetStringAttribute( "Height" );
	if( szBuf == "Parent" )
	{
		m_iDefHeight = 0;
		m_bHeightParent = true;
	}
	else
	{
		m_iDefHeight = rkElement.GetIntAttribute( "Height" );
		m_bHeightParent = false;
	}

	m_bCheckParentOutSide = rkElement.GetBoolAttribute( "CheckParentOutSide" );
}

void ioWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	RECT rcRect;
	::SetRect( &rcRect, 0, 0, 0, 0 );

	if( !GetParent() || m_bWidthParent )
		rcRect.right = iParentWidth;
	else
		rcRect.right = m_iDefWidth;

	if( !GetParent() || m_bHeightParent )
		rcRect.bottom = iParentHeight;
	else
		rcRect.bottom = m_iDefHeight;

	rcRect.left = m_iDefXPos;
	switch( m_XPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.left += ( iParentWidth - rcRect.right ) / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.left *= m_fScreenRateX;
		break;
	case UI_RIGHT_ALIGN:
		rcRect.left += iParentWidth - rcRect.right;
		break;
	}

	rcRect.top = m_iDefYPos;
	switch( m_YPosAlign )
	{
	case UI_CENTER_ALIGN:
		rcRect.top += ( iParentHeight - rcRect.bottom ) / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.top *= m_fScreenRateY;
		break;
	case UI_BOTTOM_ALIGN:
		rcRect.top += iParentHeight - rcRect.bottom;
		break;
	}

	rcRect.right += rcRect.left;
	rcRect.bottom += rcRect.top;

	SetWndRect( rcRect );

	if( bChildMove && HasChild() )
	{
		iParentWidth  = GetWidth();
		iParentHeight = GetHeight();

		m_bNowGoToTopReserving = true;

		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			(*iter)->SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
}

void ioWnd::SetDragRect( const RECT &rcDrag )
{
	if( !m_pHitRect )
		m_pHitRect = new RECT;

	*m_pHitRect = rcDrag;
}

int ioWnd::GetDerivedPosX() const
{
	if( m_pParent )
		return m_pParent->GetDerivedPosX() + GetXPos();

	return GetXPos();
}

int ioWnd::GetDerivedPosY() const
{
	if( m_pParent )
		return m_pParent->GetDerivedPosY() + GetYPos();

	return GetYPos();
}

DWORD ioWnd::GetHighParentID() const
{
	if( m_pParent )
	{
		if( m_pParent->GetID() != ID_DESKTOP )
			return m_pParent->GetHighParentID();
	}
	return GetID();
}

void ioWnd::GetTooltipHelp( ioWnd::TooltipList &rkTooltipList )
{
	if( m_TooltipList.empty() )
		return;

	rkTooltipList.resize( m_TooltipList.size() );
	std::copy( m_TooltipList.begin(), m_TooltipList.end(), rkTooltipList.begin() );
}

bool ioWnd::IsNeedProcess() const
{
	if( m_dwStyle & IWS_PROCESS )
		return true;

	return false;
}

bool ioWnd::IsInWndRect( const RECT& rt )
{
	RECT rc;
	rc.left	= GetDerivedPosX();
	rc.top = GetDerivedPosY();
	rc.right = rc.left + GetWidth();
	rc.bottom = rc.top + GetHeight();
		
	RECT tmprt;
	if ( IntersectRect( &tmprt, &rc, &rt ) == TRUE )
	{
		if ( EqualRect( &tmprt, &rt ) == TRUE ) {
			return true;
		}
		if ( EqualRect( &tmprt, &rc ) == TRUE ) {
			return true;
		}
	}
	return false;
}

bool ioWnd::IsInWndRect( const POINT &vPos )
{
	RECT rcDerived;
	rcDerived.left	= GetDerivedPosX();
	rcDerived.top   = GetDerivedPosY();
	rcDerived.right	= rcDerived.left + GetWidth();
	rcDerived.bottom= rcDerived.top + GetHeight();

	if( HasWndStyle( IWS_ISINRECT_CENTER )  )
	{
		if( m_XPosAlign == UI_CENTER_ALIGN )
		{
			int iWidth = GetWidth();
			if( iWidth != 0 )
				iWidth = iWidth >> 1;
			rcDerived.left -= iWidth;
			rcDerived.right -= iWidth;
		}

		if( m_YPosAlign == UI_CENTER_ALIGN )
		{
			int iHeight = GetHeight();
			if( iHeight != 0 )
				iHeight = iHeight >> 1;
			rcDerived.top -= iHeight;
			rcDerived.bottom -= iHeight;
		}
	}

	if( PtInRect( &rcDerived, vPos ) )
		return true;

	return false;
}

bool ioWnd::IsInHitRect( const POINT &vPos )
{
	if( !m_pHitRect )	return false;

	RECT rcDerived;
	rcDerived.left	= GetDerivedPosX() + m_pHitRect->left;
	rcDerived.top	= GetDerivedPosY() + m_pHitRect->top;
	rcDerived.right = rcDerived.left + ( m_pHitRect->right - m_pHitRect->left );
	rcDerived.bottom= rcDerived.top  + ( m_pHitRect->bottom - m_pHitRect->top );

	if( PtInRect( &rcDerived, vPos ) )
		return true;

	return false;
}

int ioWnd::GetZOrderLayer() const
{
	if( HasWndStyle( IWS_MODAL_TOOLTIP ) )
		return Z_MODAL_LAYER_TOOLTIP;

	if( HasWndStyle( IWS_MODAL ) )
		return Z_MODAL_LAYER;

	if( HasWndStyle( IWS_TOPMOST ) )
		return Z_TOPMOST_LAYER;

	if( !HasWndStyle( IWS_BACKMOST ) )
		return Z_NORMAL_LAYER;

	return Z_BACKMOST_LAYER;
}

ioWnd* ioWnd::GetChild( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumChildren() ) )
	{
		ioWndList::iterator iter = m_ChildList.begin();
		std::advance( iter, iIndex );

		return *iter;
	}

	return NULL;
}

ioWnd* ioWnd::FindChildWnd( DWORD dwID )
{
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		if( (*iter)->GetID() == dwID )
			return *iter;
	}

	return NULL;
}


ioWnd* ioWnd::FindChildWnd( const ioHashString& szWndName )
{
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		if( (*iter)->GetWndName() == szWndName )
			return *iter;
	}

	return NULL;
}

//! 실시간 사용 금지!!
ioWnd* ioWnd::FindChildWndEX( DWORD dwID, bool all )
{
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		if( !all && (*iter)->IsExceptFromID() ) {
			continue;
		}

		if( (*iter)->GetID() == dwID )
			return *iter;

		ioWnd* child = (*iter)->FindChildWndEX( dwID, all );
		if ( child ) {
			return child;
		}
	}

	return NULL;
}

void ioWnd::MoveZorderLayer( DWORD dwWndID, DWORD dwMoveID )
{
	if ( IsEXType() ) return;

	if( dwWndID == 0 || dwMoveID == 0 ) return;

	DWORD dwPrevID = 0;
	ioWnd *pMoveWnd = NULL;
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		if( (*iter)->GetID() == dwMoveID )
		{
			if( dwPrevID == dwWndID )
			{
				return;     // 순서가 정확하면 패스
			}

			pMoveWnd = *iter;
			m_ChildList.erase( iter );
			break;
		}
		dwPrevID = (*iter)->GetID();
	}


	if( pMoveWnd )
	{
		bool bInsert = false;
		ioWndList::iterator iter;
		for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
		{
			if( (*iter)->GetID() == dwWndID )
			{
				bInsert = true;
			}
			else if( bInsert )
			{			
				m_ChildList.insert( iter, pMoveWnd );
				return;
			}
		}

		if( !bInsert )
			m_ChildList.push_back( pMoveWnd );
	}
}

void ioWnd::MoveZorderLayer2( DWORD dwWndID, DWORD dwMoveID )
{
	if( dwWndID == 0 || dwMoveID == 0 ) return;

	DWORD dwPrevID = 0;
	ioWnd *pMoveWnd = NULL;
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		if( (*iter)->GetID() == dwMoveID )
		{
			if( dwPrevID == dwWndID )
			{
				return;     // 순서가 정확하면 패스
			}

			pMoveWnd = *iter;
			m_ChildList.erase( iter );
			break;
		}
		dwPrevID = (*iter)->GetID();
	}


	if( pMoveWnd )
	{
		ioWndList::iterator iter;
		for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
		{
			if( (*iter)->GetID() == dwWndID )
			{
				m_ChildList.insert( iter, pMoveWnd );
				return;
			}
		}

		m_ChildList.push_back( pMoveWnd );
	}
}

void ioWnd::MoveZorderTop( DWORD id )
{
	if ( id == 0 ) return;

	ioWnd* movewnd = 0;
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetID() == id )
		{
			movewnd = *iter;
			m_ChildList.erase( iter );

			if ( movewnd ) {
				m_ChildList.push_back( movewnd );
			}

			break;
		}
	}
}

bool ioWnd::IsCanMouseReaction() const
{
	if( !IsShow() )	return false;
	if( m_dwStyle & IWS_INACTIVE )	return false;
	if( m_dwStyle & IWS_NO_MOUSE_RESPONSE )	return false;

	return true;
}

void ioWnd::GoToTop()
{
	if( m_dwStyle & IWS_BACKMOST )
		return;

	if( m_pParent )
	{
		m_pParent->ReserveChildGoToTop( this );
	}
}

void ioWnd::GoToTopWnd()
{
	if( !IsShow() )
		return;

	HeadInsertToModalList();
	//HeadInsertToSpecialList();

	GoToTop();
}

void ioWnd::ReserveChildGoToTop( ioWnd *pChild )
{
	if( !pChild )	return;

	if( m_bNowGoToTopReserving )
	{
		m_GoToTopReservedChild.push_back( pChild );
	}
	else
	{
		MoveToTop( pChild );
		GoToTop();
	}
}

void ioWnd::ExecuteReservedChildGoToTop()
{
	if( m_GoToTopReservedChild.empty() )
		return;

	ioWndList::iterator iter = m_GoToTopReservedChild.begin();
	for( ; iter!=m_GoToTopReservedChild.end() ; ++iter )
	{
		MoveToTop( *iter );
	}

	m_GoToTopReservedChild.clear();
}

void ioWnd::MoveToTop( ioWnd *pChild )
{
	if ( IsEXType() ) return;

	if( pChild )
	{
		if ( pChild->IsEXType() ) return;

		ioWndList::iterator iter = std::find( m_ChildList.begin(),
											  m_ChildList.end(),
											  pChild );

		if( iter != m_ChildList.end() )
		{
			m_ChildList.remove( pChild );
			m_ChildList.push_back( pChild );

			pChild->iwm_gototop();			
			if( m_ChildList.size() > 1 )
			{
				stable_sort( m_ChildList.begin(), m_ChildList.end(), WindowLess() );
			}
		}
	}
}

void ioWnd::AddChild( ioWnd *pChild )
{
	if( pChild )
	{
		pChild->SetParent( this );
		m_ChildList.push_back( pChild );
	}
}

void ioWnd::DeleteChild( DWORD dwID )
{
	ioWnd *pChild = FindChildWnd( dwID );
	if( pChild )
	{
		pChild->iwm_destroy();

		m_GoToTopReservedChild.remove( pChild );
		m_ChildList.remove( pChild );
		SAFEDELETE( pChild );
	}
}

void ioWnd::DeleteAllChild()
{
	if( !HasChild() )	return;

	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		ioWnd *pWnd = *iter;
		if( GetID() == ID_DESKTOP )
		{				
            if( pWnd )
			{
				ioWndList::iterator iNext = iter;
				iNext++;
			}
		}

		if ( *iter ) {
			(*iter)->iwm_destroy();
		}

		SAFEDELETE( *iter );
	}

	m_ChildList.clear();

	m_GoToTopReservedChild.clear();
}

void ioWnd::CheckRadioButton( DWORD dwMinID, DWORD dwMaxID, DWORD dwCheckID )
{
	if( !HasChild() )	return;

	ioWnd *pChild = NULL;
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		pChild = *iter;

		if( pChild->GetWndType() != IWT_RADIO_BTN )
			continue;

		DWORD dwID = pChild->GetID();
		if( COMPARE( dwID, dwMinID, dwMaxID+1 ) )
		{
			ioRadioButton *pButton = (ioRadioButton *)pChild;
			if( dwID == dwCheckID )
			{
				pButton->SetClick( true	);
				pButton->ReSetPosition(dwCheckID);
			}
			else
			{
				pButton->SetClick( false );
 				pButton->ReSetPosition(dwCheckID);
			}
		}
	}
}

bool ioWnd::OnRenderShowAndHide()
{
	if( !HasExWndStyle( IWS_EX_SHOW_HIDE_ACTION ) )
		return false;

	DWORD dwCheckTime = 0;
	switch( GetWndExStyleParam() )
	{
	case EXWP_SCALE_TOP:
	case EXWP_SCALE_DOWN:
		dwCheckTime = 50;
		break;
	default:
		dwCheckTime = 50;
		break;
	}
	
	switch( GetWndExStyleParam() )
	{
	case EXWP_SCALE_TOP:
		return OnRenderShowAndHideByTop( dwCheckTime );
		break;
	case EXWP_SCALE_DOWN:
		return OnRenderShowAndHideByDown( dwCheckTime );
		break;
	}

	return OnRenderShowAndHideByDefault( dwCheckTime );
}

bool ioWnd::OnRenderShowAndHideByDefault( DWORD dwCheckTime )
{
	if( m_dwWndAniTime == 0 )
		return false;

	if( !m_pSelectedFrm )
		return false;

	if( FRAMEGETTIME() - m_dwWndAniTime > dwCheckTime )
	{
		m_dwWndAniTime = 0;
		m_pSelectedFrm->SetSize( GetWidth(), GetHeight() );
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

	int iWidth = GetWidth() * fTimeRate;
	int iHeight= GetHeight() * fTimeRate;
	int iXPos  = GetDerivedPosX() + ( ( GetWidth() - iWidth ) / 2 );
	int iYPos  = GetDerivedPosY() + ( ( GetHeight() - iHeight ) / 2 );
	m_pSelectedFrm->SetSize( iWidth, iHeight );
	m_pSelectedFrm->Render( iXPos, iYPos );

	return true;
}

bool ioWnd::OnRenderShowAndHideByTop( DWORD dwCheckTime )
{
	if( m_dwWndAniTime == 0 )
		return false;

	if( FRAMEGETTIME() - m_dwWndAniTime > dwCheckTime )
	{
		m_dwWndAniTime = 0;	
		m_pSelectedFrm->SetSize( GetWidth(), GetHeight() );		
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
	m_pSelectedFrm->SetSize( GetWidth(), iHeight );
	m_pSelectedFrm->Render( GetXPos(), GetYPos() );

	return true;
}

bool ioWnd::OnRenderShowAndHideByDown( DWORD dwCheckTime )
{
	return false;
}

void ioWnd::RenderWnd()
{
	if( OnRenderShowAndHide() ) return;
	if( !IsShow() )	return;

	RenderText( IoUIText::URT_PRE_RENDER );
	RenderEffect( ioUI3DEffectRender::eERT_PRERENDER );

	OnRender();

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			
			ioWnd *pWnd = *iter;
			DWORD dwID = 0;
			dwID = pWnd->GetID();
			
			if( pWnd )
			{
				pWnd->RenderWnd();
			}
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
	RenderEffect( ioUI3DEffectRender::eERT_NORMALRENDER );
	OnRenderAfterChild();
	RenderEffect( ioUI3DEffectRender::eERT_AFTERRENDER );
}

void ioWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pSelectedFrm )
		m_pSelectedFrm->Render( iXPos, iYPos );
	if( m_pMultiplyFrm )
		m_pMultiplyFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
	if( m_pSelectedTitleFrm)
		m_pSelectedTitleFrm->Render( iXPos, iYPos );
	
	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );

	if( m_pUITitle )
		m_pUITitle->PrintNormal( iXPos, iYPos );

	RenderText( IoUIText::URT_NORMAL_RENDER );
}

void ioWnd::RenderImageList( int iXPos, int iYPos, UIRenderType eRenderType )
{
	UIElementList::iterator iter;
	for( iter=m_ElementList.begin() ; iter!=m_ElementList.end() ; ++iter )
	{
		if ( *iter ) 
			(*iter)->Render( iXPos, iYPos, eRenderType );
	}
}

void ioWnd::ProcessWnd( float fTimePerSec, ioMouse& mouse )
{
	if( IsNeedProcess() )
	{
		OnProcess( fTimePerSec );
		OnMouseProcess( mouse );
	}

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWnd *pChild = NULL;
		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			pChild = *iter;
			if( pChild )
			{
				ErrorReport::SetPosition( 2100, 1 );
				ErrorReport::SetAndSoOnInfo( pChild->GetID());
				pChild->ProcessWnd( fTimePerSec, mouse );
				ErrorReport::SetPosition( 2100, 2 );
				pChild->OnMouseProcess( mouse );
				ErrorReport::SetPosition( 2100, 3 );
			}
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
}

void ioWnd::OnProcess( float fTimePerSec )
{
	ioEffectVec::iterator iterEffect;
	for( iterEffect = m_pUI3DEffect.begin() ; iterEffect != m_pUI3DEffect.end() ; ++iterEffect )
		( *iterEffect )->UpdateEffectList( fTimePerSec * FLOAT1000 );
}

void ioWnd::iwm_create()
{
	if ( m_bCheckParentOutSide ) {
		SetCheckParentOutSide( m_bCheckParentOutSide );
	}
}

void ioWnd::iwm_destroy()
{
}

void ioWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	m_bClicked	= true;
}

void ioWnd::iwm_lbuttonup( const ioMouse& mouse )
{
	m_bClicked	= false;
	if( IsBind() )
	{
		UpdateBindFunction(IOWN_BTNUP);		
	}
}

void ioWnd::iwm_lbuttonDBLCLK( const ioMouse& mouse )
{
}

void ioWnd::iwm_rbuttondown( const ioMouse& mouse )
{
	m_bRightClicked	= true;	
}

void ioWnd::iwm_rbuttonup( const ioMouse& mouse )
{
	m_bRightClicked	= false;
}

void ioWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( pWnd->IsBind() )
	{
		pWnd->UpdateBindFunction( cmd );
	}
}

void ioWnd::iwm_mouseover( const ioMouse& mouse )
{
	m_bOver = true;

	if( m_pParent )
	{
		m_pParent->iwm_command( this, IOWN_OVERED, 0 );
	}
}

void ioWnd::iwm_mousemove( const ioMouse& mouse )
{
}

void ioWnd::iwm_mouseleave( const ioMouse& mouse )
{
	m_bOver		= false;
	m_bClicked	= false;

	if( m_pParent )
	{
		m_pParent->iwm_command( this, IOWN_LEAVED, 0 );
	}
}

void ioWnd::iwm_vscroll(DWORD dwID, int curPos )
{
}

void ioWnd::iwm_wheel( int zDelta )
{
	if( m_pParent )
		m_pParent->iwm_wheel( zDelta );
}

void ioWnd::iwm_show()
{
}

void ioWnd::iwm_hide()
{
}

bool ioWnd::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		HideWnd();
		return true;
	}
	return false;
}

bool ioWnd::iwm_spacebar()
{
	return false;
}

void ioWnd::iwm_gototop()
{

}

IWDropEffect ioWnd::iwm_dragenter( ioDragItem *pItem, const ioMouse& mouse )
{
	if( m_dwStyle & IWS_DRAG_DROP )
		return IW_DROP_ENABLE;

	return IW_DROP_NONE;
}

IWDropEffect ioWnd::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	if( m_dwStyle & IWS_DRAG_DROP )
		return IW_DROP_ENABLE;

	return IW_DROP_NONE;
}

void ioWnd::iwm_dragleave()
{
}

bool ioWnd::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	return false;
}

void ioWnd::iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped )
{
}

DWORD ioWnd::GetWndType() const
{
	return IWT_WND;
}

void ioWnd::OnRenderAfterChild()
{
	RenderText( IoUIText::URT_AFTER_RENDER );
}

void ioWnd::ChildWndShowIDList()
{
	ioWndList::iterator iter = m_ChildList.begin();
	for( ; iter!=m_ChildList.end() ; ++iter )
	{
		ioWnd *pItem = *iter;
		if( pItem && pItem->IsShow() )
			LOG.PrintTimeAndLog( 0, "Show ID: %u - %d.%d.%d.%d", pItem->GetID(), pItem->GetXPos(), pItem->GetYPos(), pItem->GetWidth(), pItem->GetHeight() );
	}
}

//************************************************************************************************************************
//************************************************************************************************************************
//************************************************************************************************************************
// Tool을 위한 인터페이스들
// Save Util
std::string		ioWnd::ConvertToAlignString( ioWnd::UIPosAlign align, const std::string& def )
{
	if ( align >= ioWnd::UI_NONE_ALIGN || align < 0 ) {
		return def;
	}

	static const char*	g_enginewndalign[ ioWnd::UI_NONE_ALIGN ] =
	{
		"Left"
		,"Right"
		,"Center"
		,"Rate"
		,"Top"
		,"Bottom"
	};

	return g_enginewndalign[ align ];
}
std::string		ioWnd::ConvertToStyleString( TextStyle textstyle )
{
	static const int TEXT_STYLE_NUM = 15;
	if ( textstyle >= TEXT_STYLE_NUM || textstyle < 0 ) {
		return "Normal";
	}

	static const char*	g_enginetextstyle[ TEXT_STYLE_NUM ] =
	{
		"Normal"
		,"Shadow"
		,"Outline"
		,"Outline_2x"
		,"Outline_full"
		,"Outline_full_2x"
		,"Outline_full_3x"
		,"Outline_full_4x"
		,"Bold"
		,"Bold_shadow"
		,"Bold_outline"
		,"Bold_outline_full"
		,"Bold_outline_full_2x"
		,"Bold_outline_full_3x"
		,"Bold_outline_full_4x"
	};

	return g_enginetextstyle[ textstyle ];
}
std::string		ioWnd::ConvertToHorzAlignString( TextAlignType type )
{
	static const int HORZALIGN_NUM = 3;
	if ( type >= HORZALIGN_NUM || type < 0 ) {
		return "Left";
	}

	static const char*	g_enginehorzalign[ HORZALIGN_NUM ] =
	{
		"Left"
		,"Center"
		,"Right"
	};

	return g_enginehorzalign[ type ];
}
std::string		ioWnd::ConvertToVertAlignString( TextVertAlign type )
{
	static const int VERTALIGN_NUM = 3;
	if ( type >= VERTALIGN_NUM || type < 0 ) {
		return "Top";
	}

	static const char*	g_enginevertalign[ VERTALIGN_NUM ] =
	{
		"Top"
		,"Center"
		,"Bottom"
	};

	return g_enginevertalign[ type ];
}
std::string		ioWnd::ConvertToColorString( DWORD color )
{
	char buff[ 64 ];
	std::string ret;	
	
	sprintf( buff, "%02x", ( ( color & 0xFF000000 ) >> 24 ) );
	ret += buff;
	sprintf( buff, "%02x", ( ( color & 0x00FF0000 ) >> 16 ) );
	ret += buff;
	sprintf( buff, "%02x", ( ( color & 0x0000FF00 ) >> 8 ) );
	ret += buff;
	sprintf( buff, "%02x", ( color & 0x000000FF ) );
	ret += buff;

	return ret;
}

void	ioWnd::SetIDEX( DWORD& dwID, bool onlychild ) // // 확장 ID 셋팅 함수 : 자식까지 전부 unique하게 아이디를 지정해주고... 그 최대값을 저장해준다.
{
	if ( !onlychild ) {
		m_dwID = dwID++;
	}

	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter ) {
		(*iter)->SetIDEX( dwID, false );
	}
}

// Save
bool	ioWnd::Save( const char* filename )
{
	if ( !filename ) {
		return false;
	}

	ioXMLDocument doc;
	doc.CreateDocument( "Window", "1.0", "euc-kr", "" );
	Save( doc.GetRootElement() );
	return doc.SaveXML( filename );
}
bool	ioWnd::Save( ioXMLElement& myxml )
{
	// save attribute
	myxml.SetStringAttribute( "ClassName", GetClassNameEX() );
	myxml.SetStringAttribute( "SubClassName", GetSubClassName() );
	myxml.SetIntAttribute( "Id", GetID() );
	myxml.SetStringAttribute( "IDName", GetIDName() );
	myxml.SetStringAttribute( "MemberName", GetMemberName() );

	// save propertys
	SetSaveData( myxml );

	SaveChild( myxml );
	return true;
}
void	ioWnd::SetSaveData( ioXMLElement& myxml )
{
	SaveSize( myxml );
	SaveStyle( myxml );
	SaveExStyle( myxml );
	SaveTitle( myxml );	
	SaveToolTip( myxml );
	SaveDragRect( myxml );
	SaveImagery( myxml );
}
void	ioWnd::SaveChild( ioXMLElement& xml )
{
	int numchild = GetNumChildren();
	for ( int i = 0; i < numchild; ++i )
	{
		ioWnd* child = GetChild( i );
		if ( !child ) {
			continue;
		}
		child->Save( xml.CreateChild( "Window" ) );
	}
}
void	ioWnd::SaveSize( ioXMLElement& xml )
{
	ioXMLElement xmlsize = xml.CreateChild( "Size" );
	
	xmlsize.SetIntAttribute( "X", GetPosX() );
	xmlsize.SetIntAttribute( "Y", GetPosY() );

	if ( m_bWidthParent ) {
		xmlsize.SetStringAttribute( "Width", "Parent" );
	} else {
		xmlsize.SetIntAttribute( "Width", GetWidth() );
	}

	if ( m_bHeightParent ) {
		xmlsize.SetStringAttribute( "Height", "Parent" );
	} else {
		xmlsize.SetIntAttribute( "Height", GetHeight() );
	}

	xmlsize.SetBoolAttribute( "CheckParentOutSide", IsCheckParentOutSide() );
	xmlsize.SetStringAttribute( "XAlign", ConvertToAlignString( m_XPosAlign, "Left" ).c_str() );
	xmlsize.SetStringAttribute( "YAlign", ConvertToAlignString( m_YPosAlign, "Top" ).c_str() );
}

void	ioWnd::SaveStyle( ioXMLElement& xml )
{
	if ( !m_dwStyle ) {
		return;
	}

#define CHECK_STYLE(x) if(m_dwStyle&x){if(styletxt!=""){styletxt+="|";}styletxt+=#x;}
	
	std::string styletxt = "";

	CHECK_STYLE( IWS_START_HIDE );
	CHECK_STYLE( IWS_INACTIVE );
	CHECK_STYLE( IWS_BACKMOST );
	CHECK_STYLE( IWS_CANMOVE );
	CHECK_STYLE( IWS_MODAL );
	CHECK_STYLE( IWS_EXIT_ESC );
	CHECK_STYLE( IWS_PROCESS );
	CHECK_STYLE( IWS_DRAG_DROP );
	CHECK_STYLE( IWS_NO_MOUSE_RESPONSE );
	CHECK_STYLE( IWS_CHILD_ONLY_RESPONSE );
	CHECK_STYLE( IWS_TOPMOST );
	CHECK_STYLE( IWS_EXACTIVE );

	ioXMLElement xmlstyle = xml.CreateChild( "Style" );
	xmlstyle.SetText( styletxt.c_str() );
}

void	ioWnd::SaveExStyle( ioXMLElement& xml )
{
	if ( !m_dwExStyle ) {
		return;
	}
#define CHECK_EXSTYLE(x) if(m_dwExStyle&x){if(exstyletxt!=""){exstyletxt+="|";}exstyletxt+=#x;}
	
	std::string exstyletxt = "";

	CHECK_EXSTYLE( IWS_EX_EDIT_PW );
	CHECK_EXSTYLE( IWS_EX_EDIT_RIGHT );
	CHECK_EXSTYLE( IWS_EX_EDIT_CENTER );
	CHECK_EXSTYLE( IWS_EX_MULTI_EDIT );
	CHECK_EXSTYLE( IWS_EX_NOTIFY_MOVED );
	CHECK_EXSTYLE( IWS_EX_OVER_SCREEN );
	CHECK_EXSTYLE( IWS_EX_PUSH_SCREEN );
	CHECK_EXSTYLE( IWS_EX_HSCROLL );
	CHECK_EXSTYLE( IWS_EX_VSCROLL );
	CHECK_EXSTYLE( IWS_EX_PUSH_DOUBLE );
	CHECK_EXSTYLE( IWS_EX_EDIT_NUM );
	CHECK_EXSTYLE( IWS_EX_EDIT_CHECK_FOCUS );
	CHECK_EXSTYLE( IWS_EX_PUSHED_OVER_SCREEN );
	CHECK_EXSTYLE( IWS_EX_SHOW_NO_TOP );
	CHECK_EXSTYLE( IWS_EX_SHOW_HIDE_ACTION );
	CHECK_EXSTYLE( IWS_EX_NO_REND_EDIT_NUM );

	ioXMLElement xmlexstyle = xml.CreateChild( "ExStyle" );
	xmlexstyle.SetText( exstyletxt.c_str() );
}

void	ioWnd::SaveTitle( ioXMLElement& xml )
{
	if ( !m_pUITitle || !m_pUITitle->GetText() || strcmp( m_pUITitle->GetText(), "" ) == 0 ) {
		return;
	}

	ioXMLElement xmltitle = xml.CreateChild( "Title" );

	ioXMLElement xmlformat = xmltitle.CreateChild( "Format" );
	{
		xmlformat.SetIntAttribute( "X", m_pUITitle->GetTextXOffSet() );
		xmlformat.SetIntAttribute( "Y", m_pUITitle->GetTextYOffSet() );
		xmlformat.SetIntAttribute( "Size", m_pUITitle->GetSize() );
		xmlformat.SetIntAttribute( "VertGap", m_pUITitle->GetVertGap() );
		xmlformat.SetStringAttribute( "Style", ConvertToStyleString( m_pUITitle->GetTextStyle() ).c_str() );
		xmlformat.SetStringAttribute( "DisableStyle", ConvertToStyleString( m_pUITitle->GetDisableTextStyle() ).c_str() );
		xmlformat.SetStringAttribute( "HorzAlign", ConvertToHorzAlignString( m_pUITitle->GetHorzAlign() ).c_str() );
		xmlformat.SetStringAttribute( "VertAlign", ConvertToVertAlignString( m_pUITitle->GetVertAlign() ).c_str() );
	}

	ioXMLElement xmlcolor = xmltitle.CreateChild( "Color" );
	{
		xmlcolor.SetStringAttribute( "Type", "Normal" );
		xmlcolor.SetStringAttribute( "Text", ConvertToColorString( m_pUITitle->GetNormalTitleColor().dwText ).c_str() );
		xmlcolor.SetStringAttribute( "Back", ConvertToColorString( m_pUITitle->GetNormalTitleColor().dwBk ).c_str() );
	}
	xmlcolor = xmltitle.CreateChild( "Color" );
	{
		xmlcolor.SetStringAttribute( "Type", "Over" );
		xmlcolor.SetStringAttribute( "Text", ConvertToColorString( m_pUITitle->GetOverTitleColor().dwText ).c_str() );
		xmlcolor.SetStringAttribute( "Back", ConvertToColorString( m_pUITitle->GetOverTitleColor().dwBk ).c_str() );
	}
	xmlcolor = xmltitle.CreateChild( "Color" );
	{
		xmlcolor.SetStringAttribute( "Type", "Push" );
		xmlcolor.SetStringAttribute( "Text", ConvertToColorString( m_pUITitle->GetPushTitleColor().dwText ).c_str() );
		xmlcolor.SetStringAttribute( "Back", ConvertToColorString( m_pUITitle->GetPushTitleColor().dwBk ).c_str() );
	}
	xmlcolor = xmltitle.CreateChild( "Color" );
	{
		xmlcolor.SetStringAttribute( "Type", "Disable" );
		xmlcolor.SetStringAttribute( "Text", ConvertToColorString( m_pUITitle->GetDisableTitleColor().dwText ).c_str() );
		xmlcolor.SetStringAttribute( "Back", ConvertToColorString( m_pUITitle->GetDisableTitleColor().dwBk ).c_str() );
	}
	xmlcolor = xmltitle.CreateChild( "Color" );
	{
		xmlcolor.SetStringAttribute( "Type", "NoneSelected" );
		xmlcolor.SetStringAttribute( "Text", ConvertToColorString( m_pUITitle->GetNoneSelectedTitleColor().dwText ).c_str() );
		xmlcolor.SetStringAttribute( "Back", ConvertToColorString( m_pUITitle->GetNoneSelectedTitleColor().dwBk ).c_str() );
	}

	ioXMLElement xmltext = xmltitle.CreateChild( "Text" );
	{
		ioHashString text;
		m_pUITitle->GetFirstText( text );
		xmltext.SetText( text.c_str() );
	}
}

void	ioWnd::SaveToolTip( ioXMLElement& xml )
{
	if ( m_TooltipList.empty() ) {
		return;
	}

	ioXMLElement xmltooltip = xml.CreateChild( "Tooltip" );

	TooltipList::iterator iter = m_TooltipList.begin();
	TooltipList::iterator eiter = m_TooltipList.end();
	for ( ; iter != eiter; ++iter )
	{
		ioXMLElement xmlhelp = xmltooltip.CreateChild( "Help" );
		xmlhelp.SetStringAttribute( "Text", iter->m_szHelp.c_str() );
		xmlhelp.SetStringAttribute( "Align", ConvertToHorzAlignString( iter->m_TextAlign ).c_str() );
		xmlhelp.SetStringAttribute( "TextColor", ConvertToColorString( iter->m_dwTextColor ).c_str() );
	}
}

void	ioWnd::SaveDragRect( ioXMLElement& xml )
{
	if ( !m_pHitRect ) {
		return;
	}

	ioXMLElement xmldragrect = xml.CreateChild( "DragRect" );

	xmldragrect.SetIntAttribute( "Left", m_pHitRect->left );
	xmldragrect.SetIntAttribute( "Top", m_pHitRect->top );
	xmldragrect.SetIntAttribute( "Right", m_pHitRect->right );
	xmldragrect.SetIntAttribute( "Bottom", m_pHitRect->bottom );
}

void	ioWnd::SetFrame( ioXMLElement& xml, ioUIRenderFrame* frame, const char* type )
{
	if ( !frame ) {
		return;
	}

	ioXMLElement xmldefframe = xml.CreateChild( "DefaultFrameImagery" );
	xmldefframe.SetStringAttribute( "Type", type );
	xmldefframe.SetStringAttribute( "Templete", frame->GetName().c_str() );
	xmldefframe.SetStringAttribute( "Color", ioWnd::ConvertToColorString( frame->GetColor() ).c_str() );

	ioXMLElement xmlarea = xmldefframe.CreateChild( "Area" );
	xmlarea.SetIntAttribute( "X", frame->GetRenderRect().left );
	xmlarea.SetIntAttribute( "Y", frame->GetRenderRect().top );
	xmlarea.SetIntAttribute( "Width", frame->GetRenderRect().right - frame->GetRenderRect().left );
	xmlarea.SetIntAttribute( "Height", frame->GetRenderRect().bottom - frame->GetRenderRect().top );
	xmlarea.SetStringAttribute( "AutoResize", ( frame->IsAutoReSize() ) ? "True" : "False" );
}

void	ioWnd::SetImagery( ioXMLElement& xml, ioUIRenderImage* img, const char* type )
{
	if ( !img ) {
		return;
	}

	ioXMLElement xmlimage = xml.CreateChild( "Image" );
	xmlimage.SetStringAttribute( "Type", type );
	xmlimage.SetStringAttribute( "Color", ioWnd::ConvertToColorString( img->GetColor() ).c_str() );


	xmlimage.SetStringAttribute( "File", img->GetImageFileName().c_str() );
	xmlimage.SetStringAttribute( "Imageset", img->GetImageSetName().c_str() );
	xmlimage.SetStringAttribute( "Image", img->GetImageName().c_str() );
			
	ioXMLElement xmlarea = xmlimage.CreateChild( "Area" );
	xmlarea.SetIntAttribute( "X", img->GetRenderRect().left );
	xmlarea.SetIntAttribute( "Y", img->GetRenderRect().top );
	xmlarea.SetIntAttribute( "Width", img->GetRenderRect().right - img->GetRenderRect().left );
	xmlarea.SetIntAttribute( "Height", img->GetRenderRect().bottom - img->GetRenderRect().top );

	xmlarea.SetStringAttribute( "HorzAlign", img->GetHorzFormatting().c_str() );
	xmlarea.SetStringAttribute( "VertAlign", img->GetVertFormatting().c_str() );
	xmlarea.SetStringAttribute( "Reverse", img->GetReverseFormattingStr().c_str() );
	xmlarea.SetStringAttribute( "RenderType", img->GetFixedRenderType().c_str() );
}

ioXMLElement ioWnd::SaveImagery( ioXMLElement& xml )
{
	ioXMLElement xmlimagery = xml.CreateChild( "Imagery" );

	SetFrame( xmlimagery, m_pSelectedFrm, "Selected" );
	SetFrame( xmlimagery, m_pMultiplyFrm, "MultiplyFrm" );
	SetFrame( xmlimagery, m_pSelectedTitleFrm, "SelectedTitle" );

	UIElementList::iterator iter = m_ElementList.begin();
	UIElementList::iterator eiter = m_ElementList.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetType() == ioUIRenderElement::UIE_FRAME )
		{
			ioUIRenderFrame* frame = dynamic_cast<ioUIRenderFrame*>( *iter );
			if ( !frame ) {
				continue;
			}
			SetFrame( xmlimagery, frame, frame->GetName().c_str() );
		}
		else if ( (*iter)->GetType() == ioUIRenderElement::UIE_IMAGE )
		{
			ioUIRenderImage* img = dynamic_cast<ioUIRenderImage*>( *iter );
			if ( !img ) {
				continue;
			}

			if ( img->IsFromFile() )
			{
				if ( img->GetImageFileName() == "" ) {
					continue;
				}
			}
			else
			{
				if ( img->GetImageSetName() == "" || img->GetImageName() == "" ) {
					continue;
				}
			}
			SetImagery( xmlimagery, img, "Image" );
		}
	}

	return xmlimagery;
}

bool ioWnd::IsParent( DWORD id )
{
	if ( !GetParent() ) {
		return false;
	}
	if ( GetParent()->GetID() == id ) {
		return true;
	}
	return GetParent()->IsParent( id );
}

ioWnd* ioWnd::Clone( bool ischilds )
{
	ioWnd* ret = _Clone();

	Copy( ret );

	if ( ischilds ) {
		CopyChilds( ret );
	}

	return ret;
}

void	ioWnd::CopyChilds( ioWnd* wnd )
{
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter ) {
		wnd->AddChild( (*iter)->Clone( true ) );
	}
}

void	ioWnd::Copy( ioWnd* wnd )
{
	if ( !wnd ) {
		return;
	}

	{
		UIElementList::iterator iter = m_ElementList.begin();
		UIElementList::iterator eiter = m_ElementList.end();
		for ( ; iter != eiter; ++iter ) {
			wnd->m_ElementList.push_back( (*iter)->Clone() );
		}
	}

	wnd->m_pUITitle = 0;
	if ( m_pUITitle )
	{
		wnd->m_pUITitle = new ioUITitle();
		*(wnd->m_pUITitle) = *m_pUITitle;
	}

	CLONEFRAME( wnd, m_pSelectedFrm );
	CLONEFRAME( wnd, m_pMultiplyFrm );
	CLONEFRAME( wnd, m_pSelectedTitleFrm );

	wnd->m_dwStyle = m_dwStyle;
	wnd->m_dwExStyle = m_dwExStyle;

	wnd->m_rcPos = m_rcPos;
	wnd->m_pHitRect = 0;
	if ( m_pHitRect ) 
	{
		wnd->m_pHitRect = new RECT();
		*(wnd->m_pHitRect) = *m_pHitRect;
	}

	wnd->m_bShow = m_bShow;
	wnd->m_bClicked = false;
	wnd->m_bRightClicked = false;
	wnd->m_bOver = false;
	wnd->m_bCheckParentOutSide = m_bCheckParentOutSide;
	
	wnd->m_XPosAlign = m_XPosAlign;
	wnd->m_YPosAlign = m_YPosAlign;
	wnd->m_iDefXPos = m_iDefXPos;
	wnd->m_iDefYPos = m_iDefYPos;
	wnd->m_iDefWidth = m_iDefWidth;
	wnd->m_iDefHeight = m_iDefHeight;
	wnd->m_bWidthParent = m_bWidthParent;
	wnd->m_bHeightParent = m_bHeightParent;
	wnd->m_dwWndAniTime = m_dwWndAniTime;

	{
		TooltipList::iterator iter = m_TooltipList.begin();
		TooltipList::iterator eiter = m_TooltipList.end();
		for ( ; iter != eiter; ++iter ) {
			wnd->m_TooltipList.push_back( *iter );
		}
	}

	wnd->m_dwID = m_dwID;

	// ret->m_pParent = 0; ??
	// ret->m_pPreModalWnd = m_pPreModalWnd;
	// ret->m_pNextModalWnd = m_pNextModalWnd;
	//typedef std::map< ioHashString, ioUIRenderImage* > NamedRenderImageMap;
	//NamedRenderImageMap m_NamedImageList; ??
	//ioWndList   m_GoToTopReservedChild;
	//m_bNowGoToTopReserving; ??
}

void	ioWnd::Layout()
{
	if( !m_pParent )
		return;

	int parentx      = m_pParent->GetXPos();
	int parenty      = m_pParent->GetYPos();
	int parentwidth  = m_pParent->GetWidth();
	int parentheight = m_pParent->GetHeight();

	RECT rcRect = { 0, 0, 0, 0 };
	rcRect.left = m_iDefXPos;

	switch( m_XPosAlign )
	{
	case UI_LEFT_ALIGN:
		rcRect.left = parentx;
		break;
	case UI_CENTER_ALIGN:
		rcRect.left = parentwidth / 2 - GetWidth() / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.left = parentwidth * m_fScreenRateX;
		break;
	case UI_RIGHT_ALIGN:
		rcRect.left = parentx + parentwidth - GetWidth();
		break;
	}

	rcRect.top = m_iDefYPos;
	switch( m_YPosAlign )
	{
	case UI_TOP_ALIGN:
		rcRect.top = parenty;
		break;
	case UI_CENTER_ALIGN:
		rcRect.top = parentheight / 2 - GetHeight() / 2;
		break;
	case UI_RATE_ALIGN:
		rcRect.top = parenty * m_fScreenRateY;
		break;
	case UI_BOTTOM_ALIGN:
		rcRect.top = parenty + parentheight - GetHeight();
		break;
	}

	rcRect.right  = rcRect.left + GetWidth();
	rcRect.bottom = rcRect.top + GetHeight();
	SetWndRect( rcRect );

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			(*iter)->Layout();
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
}
void	ioWnd::SetClassName( const char* name ) 
{ 
	if ( !name || !strcmp( name, "" ) ) { 
		m_classname = "";
	} else {
		m_classname = name;
	}
}
const char*	ioWnd::GetClassNameEX() const 
{
	if ( m_classname == "" ) {
		return GetDefClassName();
	}
	return m_classname.c_str();
}
void ioWnd::SetSubClassName( const char* name )
{
	if ( name ) {
		m_subclassname = name;
	}
}
const char* ioWnd::GetSubClassName() const
{
	return m_subclassname.c_str();
}
int	ioWnd::UpChild( DWORD id )
{	
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	ioWndList::iterator prev = iter;
	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetID() == id )
		{
			if ( iter == prev ) {
				break;
			}

			ioWnd* swap = *iter;
			*iter = *prev;
			*prev = swap;			
			return (*iter)->GetID();
		}
		prev = iter;
	}	
	return -1;
}
int	ioWnd::DownChild( DWORD id )
{
	ioWndList::iterator iter = m_ChildList.begin();
	ioWndList::iterator eiter = m_ChildList.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetID() == id )
		{
			ioWndList::iterator tmp = iter;
			++tmp;
			if ( tmp == eiter ) {
				break;
			}
			
			ioWnd* swap = *iter;
			*iter = *tmp;
			*tmp = swap;
			return (*iter)->GetID();
		}
	}
	return -1;
}
// tooltip
void ioWnd::AddTooltip( const char* text, int align, DWORD color ) 
{
	Tooltip kTooltip;
	kTooltip.m_szHelp = text;
	kTooltip.m_TextAlign = static_cast<TextAlignType>(align);
	kTooltip.m_dwTextColor = color;
	m_TooltipList.push_back( kTooltip );
}
void ioWnd::SetTooltip( const char* text, int align, DWORD color )
{
	if ( static_cast<int>(m_TooltipList.size()) <= 0 ) {
		AddTooltip( text, align, color );
	} else {
		m_TooltipList[ 0 ].m_szHelp = text;
		m_TooltipList[ 0 ].m_TextAlign = static_cast<TextAlignType>(align);
		m_TooltipList[ 0 ].m_dwTextColor = color;
	}
}
void ioWnd::SetTooltipText( const char* text )
{
	if ( static_cast<int>(m_TooltipList.size()) <= 0 ) {
		AddTooltip( text, TAT_LEFT, 0xffffffff );
	} else {
		m_TooltipList[ 0 ].m_szHelp = text;
	}
}
void ioWnd::SetTooltipAlign( int align )
{
	if ( static_cast<int>(m_TooltipList.size()) <= 0 ) {
		AddTooltip( "", align, 0xffffffff );
	} else {
		m_TooltipList[ 0 ].m_TextAlign = static_cast<TextAlignType>(align);
	}
}
void ioWnd::SetTooltipColor( DWORD color )
{
	if ( static_cast<int>(m_TooltipList.size()) <= 0 ) {
		AddTooltip( "", TAT_LEFT, color );
	} else {
		m_TooltipList[ 0 ].m_dwTextColor = color;
	}
}

// RenderElement ( common )
void ioWnd::GetRenderElementIterator( int i, int type, UIElementList::iterator& iter )
{
	iter = m_ElementList.begin();
	UIElementList::iterator eiter = m_ElementList.end();
	for ( int cnt = 0; iter != eiter; ++iter )
	{
		if ( (*iter)->GetType() == type )
		{
			if ( cnt == i ) {
				break;
			}
			++cnt;
		}
	}
}
ioUIRenderElement** ioWnd::GetRenderElement( int i, int type )
{
	if ( i < 0 || i >= static_cast<int>( m_ElementList.size() ) ) {
		return 0;
	}

	UIElementList::iterator iter;
	GetRenderElementIterator( i, type, iter );
	if ( iter == m_ElementList.end() ) {
		return 0;
	}

	return &(*iter);
}
int ioWnd::GetNumRenderElement( int type )
{
	int ret = 0;
	UIElementList::iterator iter = m_ElementList.begin();
	UIElementList::iterator eiter = m_ElementList.end();
	for ( ; iter != eiter; ++iter )
	{
		if ( (*iter)->GetType() == type ) {
			++ret;
		}
	}
	return ret;
}
void ioWnd::RemoveRenderElementList( int i, int type )
{
	if ( i < 0 || static_cast<int>(m_ElementList.size()) < i ) {
		return;
	}

	UIElementList::iterator iter;
	GetRenderElementIterator( i, type, iter );
	if ( iter == m_ElementList.end() ) {
		return;
	}

	m_ElementList.erase( iter );
}
// SelectedFrame
void _SetFrameRenderRectX( ioUIRenderElement* frame, int x )
{
	if ( frame )
	{
		RECT rt = frame->GetRenderRect();		
		rt.right = ( rt.right - rt.left ) + x;
		rt.left = x;
		frame->SetRenderRect( rt );
	}
}
void _SetFrameRenderRectY( ioUIRenderElement* frame, int y )
{
	if ( frame )
	{
		RECT rt = frame->GetRenderRect();
		rt.bottom = ( rt.bottom - rt.top ) + y;
		rt.top = y;
		frame->SetRenderRect( rt );
	}
}
void _SetRenderRectWidth( ioUIRenderElement* frame, int w )
{
	if ( frame )
	{
		RECT rt = frame->GetRenderRect();
		rt.right = rt.left + w;
		frame->SetRenderRect( rt );
	}
}
void _SetRenderRectHeight( ioUIRenderElement* frame, int h )
{
	if ( frame )
	{
		RECT rt = frame->GetRenderRect();
		rt.bottom = rt.top + h;
		frame->SetRenderRect( rt );
	}
}
// Renderframe ( virtual / common )
static const char* g_iowndfixedframe[ 3 ] = {
	"Selected",
	"MultiplyFrm",
	"SelectedTitle"
};
int ioWnd::GetNumFixedRenderFrame() const
{
	return countof( g_iowndfixedframe );
}
const char* ioWnd::GetFixedRenderFrameType( int index )
{
	if ( index < 0 || index >= countof( g_iowndfixedframe ) ) {
		return 0;
	}
	return g_iowndfixedframe[ index ];
}
void ioWnd::GenerateFrameList( int i )
{
	int num = GetNumRenderElement( ioUIRenderElement::UIE_FRAME );
	if ( num <= i ) {
		for ( int loop = num - 1; loop < i; ++loop ) {
			m_ElementList.push_back( new ioUIRenderFrame() );
		}
	}
}
ioUIRenderElement** ioWnd::GetRenderFrame( const char* type )
{
	if ( strcmp( type, "Selected" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pSelectedFrm);
	} else if ( strcmp( type, "MultiplyFrm" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pMultiplyFrm);
	} else if ( strcmp( type, "SelectedTitle" ) == 0 ) {
		return reinterpret_cast<ioUIRenderElement**>(&m_pSelectedTitleFrm);
	}
	return 0;
}
ioUIRenderElement** ioWnd::GetRenderFrame( const char* type, int i )
{
	ioUIRenderElement** img = GetRenderFrame( type );
	if ( !img ) {
		img = GetRenderElement( i, ioUIRenderElement::UIE_FRAME );
	}
	return img;
}
void ioWnd::SetFrameRenderRectX( const char* type, int i, int x )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		_SetFrameRenderRectX( *frame, x );
	}
}

void ioWnd::SetFrameRenderRectY( const char* type, int i, int y )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		_SetFrameRenderRectY( *frame, y );
	}
}

void ioWnd::SetFrameRenderRectWidth( const char* type, int i, int w )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		_SetRenderRectWidth( *frame, w );
	}
}

void ioWnd::SetFrameRenderRectHeight( const char* type, int i, int h )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		_SetRenderRectHeight( *frame, h );
	}
}

void ioWnd::SetFrameRenderColor( const char* type, int i, DWORD color )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		(*frame)->SetColor( color );
	}
}

void ioWnd::SetFrameRenderAutoResize( const char* type, int i, bool b )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		dynamic_cast<ioUIRenderFrame*>(*frame)->SetAutoReSize( b );
	}
}

void ioWnd::SetFrameRenderTemplete( const char* type, int i, const char* templete, ioGUIManager* mgr )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( !frame ) 
	{
		ioUIRenderElement* newframe = mgr->CloneFrame( templete );
		if ( newframe ) 
		{
			GenerateFrameList( i - 1 );
			m_ElementList.push_back( newframe );
		}
		else 
		{
			GenerateFrameList( i );
		}
		return;
	}

	DWORD color = 0xffffffff;
	RECT rt = { 0, 0, 0, 0 };	
	bool autoresize = true;
	if ( *frame )
	{
		color = (*frame)->GetColor();
		rt = (*frame)->GetRenderRect();
		autoresize = dynamic_cast<ioUIRenderFrame*>(*frame)->IsAutoReSize();
		delete *frame;
	}

	*frame = mgr->CloneFrame( templete );	
	if ( *frame )
	{	
		(*frame)->SetColor( color );
		(*frame)->SetRenderRect( rt );
		dynamic_cast<ioUIRenderFrame*>(*frame)->SetName( templete );
		dynamic_cast<ioUIRenderFrame*>(*frame)->SetAutoReSize( autoresize );
	}
	else
	{
		*frame = new ioUIRenderFrame();
	}
}
int ioWnd::GetFrameRenderRectX( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return (*frame)->GetRenderRect().left;
	}
	return 0;
}
int ioWnd::GetFrameRenderRectY( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return (*frame)->GetRenderRect().top;
	}
	return 0;
}
int ioWnd::GetFrameRenderRectWidth( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return (*frame)->GetRenderRect().right - (*frame)->GetRenderRect().left;
	}
	return 0;
}
int ioWnd::GetFrameRenderRectHeight( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return (*frame)->GetRenderRect().bottom - (*frame)->GetRenderRect().top;
	}
	return 0;
}
DWORD ioWnd::GetFrameRenderColor( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return (*frame)->GetColor();
	}
	return 0;
}
bool ioWnd::GetFrameRenderAutoResize( const char* type, int i )
{
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return dynamic_cast<ioUIRenderFrame*>(*frame)->IsAutoReSize();
	}
	return true;
}
const char* ioWnd::GetFrameRenderTemplete( const char* type, int i )
{
	static const char* ret = "";
	ioUIRenderElement** frame = GetRenderFrame( type, i );
	if ( frame && *frame ) {
		return dynamic_cast<ioUIRenderFrame*>(*frame)->GetName().c_str();
	}
	return ret;
}
// Renderimage ( virtual / common )
int ioWnd::GetNumFixedRenderImage() const
{
	return 0;
}
const char* ioWnd::GetFixedRenderImageType( int index )
{
	return NULL;
}
ioUIRenderElement** ioWnd::GetRenderImage( const char* type )
{
	return 0;
}
ioUIRenderElement**	ioWnd::GetRenderImage( const char* type, int i )
{
	ioUIRenderElement** img = GetRenderImage( type );
	if ( !img ) {
		img = GetRenderElement( i, ioUIRenderElement::UIE_IMAGE );
	}
	return img;
}
void ioWnd::GenerateImageList( int i )
{
	int num = GetNumRenderElement( ioUIRenderElement::UIE_IMAGE );
	if ( num <= i ) {
		for ( int loop = num - 1; loop < i; ++loop ) {
			m_ElementList.push_back( new ioUIRenderImage() );
		}
	}
}
// Renderimage ( Set )
void ioWnd::SetRenderImageRectX( const char* type, int i, int x )
{
	SetRenderImageRectX( GetRenderImage( type, i ), x );
}
void ioWnd::SetRenderImageRectY( const char* type, int i, int y )
{
	SetRenderImageRectY( GetRenderImage( type, i ), y );
}
void ioWnd::SetRenderImageRectWidth( const char* type, int i, int w )
{
	SetRenderImageRectWidth( GetRenderImage( type, i ), w );
}
void ioWnd::SetRenderImageRectHeight( const char* type, int i, int h )
{
	SetRenderImageRectHeight( GetRenderImage( type, i ), h );
}
void ioWnd::SetRenderImageColor( const char* type, int i, DWORD color )
{
	SetRenderImageColor( GetRenderImage( type, i ), color );
}
void ioWnd::SetRenderImageFilename( const char* type, int i, const char* filename, ioGUIManager* mgr )
{
	ioUIRenderElement** ppimg = GetRenderImage( type );
	if ( !ppimg ) {
		ppimg = GetRenderElement( i, ioUIRenderElement::UIE_IMAGE );
	}
	SetRenderImageFilename( ppimg, i, filename, mgr );
}
void ioWnd::SetRenderImageImageName( const char* type, int i, const char* setname, const char* name, ioGUIManager* mgr )
{
	ioUIRenderElement** ppimg = GetRenderImage( type );
	if ( !ppimg ) {
		ppimg = GetRenderElement( i, ioUIRenderElement::UIE_IMAGE );
	}
	SetRenderImageImageName( ppimg, i, setname, name, mgr );
}
// Renderimage ( Get )
int ioWnd::GetRenderImageRectX( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageRectX( GetRenderImage( type, i ) );
}
int ioWnd::GetRenderImageRectY( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageRectY( GetRenderImage( type, i ) );
}
int ioWnd::GetRenderImageRectWidth( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageRectWidth( GetRenderImage( type, i ) );
}
int ioWnd::GetRenderImageRectHeight( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageRectHeight( GetRenderImage( type, i ) );
}
DWORD ioWnd::GetRenderImageColor( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageColor( GetRenderImage( type, i ) );
}
bool ioWnd::GetRenderImageIsFromFile( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageIsFromFile( GetRenderImage( type, i ) );
}
const char* ioWnd::GetRenderImageFilename( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageFilename( GetRenderImage( type, i ) );
}
const char* ioWnd::GetRenderImageImageSetName( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageImageSetName( GetRenderImage( type, i ) );
}
const char* ioWnd::GetRenderImageImageName( const char* type, int i )
{
	bool exist = false;
	return GetRenderImageImageName( GetRenderImage( type, i ) );
}
void ioWnd::SetRenderImageRectX( ioUIRenderElement** img, int x )
{
	if ( !img || !*img ) {
		return;
	}

	RECT rt = (*img)->GetRenderRect();
	rt.right = ( rt.right - rt.left ) + x;
	rt.left = x;
	(*img)->SetRenderRect( rt );
}
void ioWnd::SetRenderImageRectY( ioUIRenderElement** img, int y )
{
	if ( !img || !*img ) {
		return;
	}

	RECT rt = (*img)->GetRenderRect();
	rt.bottom = ( rt.bottom - rt.top ) + y;
	rt.top = y;
	(*img)->SetRenderRect( rt );
}
void ioWnd::SetRenderImageRectWidth( ioUIRenderElement** img, int w )
{
	if ( !img || !*img ) {
		return;
	}

	RECT rt = (*img)->GetRenderRect();
	rt.right = rt.left + w;
	(*img)->SetRenderRect( rt );
}
void ioWnd::SetRenderImageRectHeight( ioUIRenderElement** img, int h )
{
	if ( !img || !*img ) {
		return;
	}

	RECT rt = (*img)->GetRenderRect();
	rt.bottom = rt.top + h;
	(*img)->SetRenderRect( rt );
}
void ioWnd::SetRenderImageColor( ioUIRenderElement** img, DWORD color )
{
	if ( !img || !*img ) {
		return;
	}

	(*img)->SetColor( color );
}
void ioWnd::_SetRenderImageImage( ioUIRenderElement** img, int i, const char* filename, const char* setname, const char* name, ioGUIManager* mgr )
{
	if ( !img ) 
	{
		ioUIRenderElement* newimg = mgr->CreateImage( setname, name );		
		if ( newimg )
		{
			GenerateImageList( i - 1 ); 
			m_ElementList.push_back( newimg );
		}
		else 
		{
			GenerateImageList( i );
		}
		return;
	}

	DWORD color = 0xffffff;
	RECT rt = { 0, 0, 0, 0 };
	if ( *img )
	{
		color = (*img)->GetColor();
		rt = (*img)->GetRenderRect();
		delete *img;
	}
	
	ioUIRenderImage* newimg = mgr->CreateImage( setname, name );
	if ( newimg )
	{
		newimg->SetColor( color );
		newimg->SetRenderRect( rt );
		newimg->SetImageSetName( setname );
		newimg->SetImageName( name );
		newimg->SetImageFileName( filename );
		(*img) = newimg;
	}
	else
	{
		*img = new ioUIRenderImage();
	}
}
void ioWnd::SetRenderImageFilename( ioUIRenderElement** img, int i, const char* filename, ioGUIManager* mgr )
{
	const char* setimagename = NULL;
	const char* imagename = NULL;
	StringVector vparam = ioStringConverter::Split( std::string(filename), "#", 2 );
	if( vparam.size() == 2 ){
		if( strcmp( vparam[0].c_str(), "" ) != 0 ){
			setimagename = vparam[0].c_str();
		}
		if( strcmp( vparam[1].c_str(), "" ) != 0 ){
			imagename = vparam[1].c_str();
		}
	}

	_SetRenderImageImage( img, i, filename, setimagename, imagename, mgr );
}
void ioWnd::SetRenderImageImageName( ioUIRenderElement** img, int i, const char* setname, const char* name, ioGUIManager* mgr )
{
	std::string filename;
	filename += setname;
	filename += "#";
	filename += name;

	_SetRenderImageImage( img, i, filename.c_str(), setname, name, mgr );
}
int ioWnd::GetRenderImageRectX( ioUIRenderElement** img )
{	
	if ( img && *img ) {
		return (*img)->GetRenderRect().left;
	}
	return 0;
}
int ioWnd::GetRenderImageRectY( ioUIRenderElement** img )
{
	if ( img && *img ) {
		return (*img)->GetRenderRect().top;
	}
	return 0;
}
int ioWnd::GetRenderImageRectWidth( ioUIRenderElement** img )
{
	if ( img && *img ) {
		return (*img)->GetRenderRect().right - (*img)->GetRenderRect().left;
	}
	return 0;
}
int ioWnd::GetRenderImageRectHeight( ioUIRenderElement** img )
{
	if ( img && *img ) {
		return (*img)->GetRenderRect().bottom - (*img)->GetRenderRect().top;
	}
	return 0;
}
DWORD ioWnd::GetRenderImageColor( ioUIRenderElement** img )
{
	if ( img && *img ) {
		return (*img)->GetColor();
	}
	return 0;
}
bool ioWnd::GetRenderImageIsFromFile( ioUIRenderElement** img )
{
	if ( img && *img ) 
	{
		ioUIRenderImage* tmp = dynamic_cast<ioUIRenderImage*>( *img );
		if ( tmp ) {
			return tmp->IsFromFile();
		}
	}
	return true;
}
const char* ioWnd::GetRenderImageFilename( ioUIRenderElement** img )
{
	static const char* ret = "";
	if ( img && *img ) 
	{	
		ioUIRenderImage* tmp = dynamic_cast<ioUIRenderImage*>( *img );
		if ( tmp ) {
			return tmp->GetImageFileName().c_str();
		
		}
	}
	return ret;
}
const char* ioWnd::GetRenderImageImageSetName( ioUIRenderElement** img )
{
	static const char* ret = "";
	if ( img && *img )
	{
		ioUIRenderImage* tmp = dynamic_cast<ioUIRenderImage*>( *img );
		if ( tmp ) {
			return tmp->GetImageSetName().c_str();
		}
	}
	return ret;
}
const char* ioWnd::GetRenderImageImageName( ioUIRenderElement** img )
{
	static const char* ret = "";
	if ( img && *img )
	{
		ioUIRenderImage* tmp = dynamic_cast<ioUIRenderImage*>( *img );
		if ( tmp ) {
			return tmp->GetImageName().c_str();
		}
	}
	return ret;
}

void ioWnd::AddEffect( const ioHashString &szType, ioUI3DEffectRender *pUI3DRender )
{
	if( pUI3DRender )
		m_pUI3DEffect.push_back( pUI3DRender );
	else
	{
		LOG.PrintTimeAndLog( 0, "AddEffect - pUI3DRender null" );
	}
}

ioUI3DEffectRender* ioWnd::GetEffect( const ioHashString szType )
{
	for(ioEffectVec::iterator it = m_pUI3DEffect.begin(); it != m_pUI3DEffect.end(); ++it)
	{
		ioUI3DEffectRender* p = (*it);
		if(p->GetType() == szType)
		{
			return p;
		}
	}

	return NULL;
}

void ioWnd::RenderEffect( int iRenderType )
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioEffectVec::iterator iterEffect;
	for( iterEffect = m_pUI3DEffect.begin() ; iterEffect != m_pUI3DEffect.end() ; ++iterEffect )
		( *iterEffect )->Render( iRenderType, iXPos, iYPos );
}

void ioWnd::SetBindFunction( FUNC func, DWORD dwMessage )
{
	m_bBind = true;

	BINDSTRUCT sData;
	sData.func = func;

	m_BindMap[dwMessage] = sData;

	/*auto it = m_BindMap.find(dwMessage);
	if( it == m_BindMap.end() )
	{
		m_BindMap.insert( std::map<DWORD,BINDSTRUCT>::value_type(dwMessage,sData));
	}
	else
		m_BindMap[dwMessage] = sData;*/
}

void ioWnd::SetBindFunction( FUNC func, DWORD dwMessage, BINDSTRUCT sData )
{
	m_bBind = true;
	sData.func = func;

	m_BindMap[dwMessage] = sData;

	/*auto it = m_BindMap.find(dwMessage);
	if( it == m_BindMap.end() )
	{
		m_BindMap.insert( std::map<DWORD,BINDSTRUCT>::value_type(dwMessage,sData));
	}
	else
		m_BindMap[dwMessage] = sData;*/
}

void ioWnd::UpdateBindFunction( DWORD dwMessage )
{
	auto it = m_BindMap.find(dwMessage);
	if( it != m_BindMap.end() )
	{
		BINDSTRUCT sData = it->second;
		sData.func(sData);
	}
}

const char* ioWnd::GetTitleText()
{
	if( m_pUITitle )
		return m_pUITitle->GetText();

	return "";
}

IoUIText* ioWnd::GetText( ioHashString name )
{
	std::list<IoUIText*>::iterator it = m_pTextList.begin();

	for(; it != m_pTextList.end(); ++it)
	{
		if((*it)->GetName() == name)
		{
			return *it;
		}
	}

	return NULL;
}

void ioWnd::RenderText( int iRenderPos )
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	std::list<IoUIText*>::iterator it = m_pTextList.begin();
	for(; it != m_pTextList.end(); ++it)
	{
		if( (*it)->GetVisible() && (*it)->GetRenderPos() == iRenderPos )
		{
			(*it)->PrintNormal(iXPos, iYPos);
		}
	}
}

void ioWnd::AddTemplate( ioXMLElement *pElement )
{
	m_TemplateList.push_back( pElement );
}

void ioWnd::DeleteTemplateChild()
{
	if( !HasChild() )	return;

	std::vector< DWORD > cIDs;
	ioWndList::iterator iter;
	for( iter = m_ChildList.begin() ; iter!=m_ChildList.end() ; ++iter )
	{
		ioWnd *pWnd = *iter;
		if( pWnd->IsTemplateChild() )
			cIDs.push_back( pWnd->GetID() );
	}

	int iCount = cIDs.size();
	for( int i = 0 ; i < iCount ; i++ )
		DeleteChild( cIDs[i] );
}

ioXMLElement* ioWnd::FindTemplate( ioHashString &szClassName, DWORD dwFindID )
{
	ioTemplateList::iterator iterTP;
	for( iterTP = m_TemplateList.begin() ; iterTP != m_TemplateList.end() ; ++iterTP )
	{
		ioXMLElement *pElement = *iterTP;
		ioHashString szTemplateClassName = pElement->GetStringAttribute( "ClassName" );
		DWORD        dwTemplateID        = pElement->GetIntAttribute( "Id" );

		if ( szTemplateClassName == szClassName && dwFindID == -1 )
			return pElement;

		if ( szTemplateClassName == szClassName && dwFindID == dwTemplateID )
			return pElement;

	}
	return NULL;
}

//************************************************************************************************************************