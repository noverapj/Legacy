#pragma once
#include "StdAfx.h"
#include "ioPullDownEventListener.h"
#include "ioPullDownItemFactory.h"
#include "ioPullDownList.h"

#include "../ioPowerUpManager.h"

ioPullDownList::ioPullDownList() : m_pOpenBtn( NULL )
{
	m_ePullDownStyle		= PDS_DOWN;

	m_pDarkLine				= NULL;
	m_pLightLine			= NULL;
	m_pOverLine				= NULL;

	m_iMaxPageList			= 10;
	m_iBlankWidth			= 0;
	m_iBlankHeight			= 0;
	m_iListHeight			= 0;

	m_iOverSelectIndex		= PullDownEvent::NONE_SELECT;
	m_iImageOverIndex		= PullDownEvent::NONE_SELECT;
	m_iCurPos				= 0;
}

ioPullDownList::~ioPullDownList()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);

	DestroyPullDown();
	DestroyPullDownImage();
}

void ioPullDownList::DestroyPullDown()
{	
	for( vPullDownItem::iterator iter = m_vPullDownItem.begin(); iter != m_vPullDownItem.end(); ++iter )
	{
		ioPullDownItem* pPullDown = *iter;
		SAFEDELETE( pPullDown );
	}
	m_vPullDownItem.clear();
}

void ioPullDownList::DestroyPullDownImage()
{
	for( vPullDownInnerImage::iterator iter = m_vImageList.begin(); iter != m_vImageList.end(); ++iter )
	{
		PullDownInnerImage& rkImage = *iter;
		if( !rkImage.m_pImage )
			continue;

		if( !rkImage.m_bManagedImageByItem )
		{
			SAFEDELETE( rkImage.m_pImage );			
		}
	}
	m_vImageList.clear();
}

void ioPullDownList::ParseExtraInfo( ioXMLElement &xElement )
{	
	ioXMLElement elem = xElement.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if( strcmp( elem.GetTagName(), "PullDown" ) == 0 )
		{
			ParsePullDown( elem );
		}
		else if( strcmp( elem.GetTagName(), "PullDownItem" ) == 0 )
		{
			ParsePullDownItem( elem );
		}
		else if( strcmp( elem.GetTagName(), "PowerUpPullDownItem" ) == 0 )
		{
			g_PowerUpManager.SetMenuList( elem );
		}

		elem = elem.NextSibling();
	}
}

void ioPullDownList::ParsePullDown( ioXMLElement &xElement )
{
	m_iMaxPageList		= xElement.GetIntAttribute( "MaxPageList" );							//풀다운 박스 Element 최대 갯수, 최대 갯수를 넘어가면 스크롤바가 Show됨
	m_iBlankWidth		= xElement.GetIntAttribute( "BlankWidth" );								//PullDownItem의 시작 X 위치
	m_iBlankHeight		= xElement.GetIntAttribute( "BlankHeight" );							//PullDownItem의 시작 Y 위치	
	m_iListHeight		= xElement.GetIntAttribute( "ListHeight" );								//PullDownItem의 높이값	
	m_ePullDownStyle	= static_cast<PullDownStyle>( xElement.GetIntAttribute( "Style" ) );	//풀다운 펼침 스타일 지정	
}

void ioPullDownList::ParsePullDownItem( ioXMLElement &xElement )
{
	ioXMLElement elem = xElement.FirstChild();
	while ( !elem.IsEmpty() )
	{
		if( strcmp( elem.GetTagName(), "Image" ) == 0 )
		{
			ParseImage( elem );
		}

		elem = elem.NextSibling();
	}
}

void ioPullDownList::ParseImage( ioXMLElement &xElement )
{
	//풀다운 아이템에 등록될 이미지 정보
	PullDownInnerImage ImageSet;
	ImageSet.m_pImage				= g_UIImageSetMgr.CreateImageByFullName( xElement.GetStringAttribute( "Name" ) );
	ImageSet.m_iXPos				= xElement.GetIntAttribute( "X" );
	ImageSet.m_iYPos				= xElement.GetIntAttribute( "Y" );
	ImageSet.m_dwColor				= ioStringConverter::ParseColor( xElement.GetStringAttribute( "Color" ) );
	ImageSet.m_dwOverColor			= ioStringConverter::ParseColor( xElement.GetStringAttribute( "OverColor" ) );
	ImageSet.m_eRenderType			= ioStringConverter::ParseRenderType( xElement.GetStringAttribute( "RenderType" ) );
	ImageSet.m_bManagedImageByItem	= false;

	m_vImageList.push_back( ImageSet );
}

void ioPullDownList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ioPullDownList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void ioPullDownList::iwm_hide()
{
	SetCheckParentOutSide( false );

	//풀 다운 아이템 마우스 리브
	OnPullDownLeave( m_iOverSelectIndex, m_iImageOverIndex );	
	m_iOverSelectIndex	= PullDownEvent::NONE_SELECT;
	m_iImageOverIndex	= PullDownEvent::NONE_SELECT;
}

void ioPullDownList::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
	}
	else
	{
		if( g_App.IsMouseBusy() )
			return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL )
			return;

		if( !pScroll->IsShow() )
			return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void ioPullDownList::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL )
		return;

	if( !pScroll->IsShow() )
		return;

	m_iCurPos = curPos;
	g_PullDownFactory.Layout( this, m_ePullDownStyle );
}

void ioPullDownList::OpenList( ioWnd *pOpenBtn )
{
	if( IsShow() )
	{
		HideWnd();
		return;
	}

	DestroyPullDown();

	m_pOpenBtn = pOpenBtn;
	ShowWnd();
}

void ioPullDownList::AddList( ioPullDownItem* pItem )
{
	if( !pItem )
	{
		LOG.PrintTimeAndLog( 0, "%s - pItem == NULL!!", __FUNCTION__ );
		return;
	}

	pItem->m_iIndex = static_cast<int>( m_vPullDownItem.size() );
	m_vPullDownItem.push_back( pItem );

	m_iCurPos = 0;

	if( (int)m_vPullDownItem.size() <= m_iMaxPageList )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			if ( (int)m_vPullDownItem.size() > m_iMaxPageList )
				pScroll->SetScrollRange( 0, m_vPullDownItem.size() - m_iMaxPageList + 1 );
			else
				pScroll->SetScrollRange( 0, m_vPullDownItem.size() );

			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}

	g_PullDownFactory.Layout( this, m_ePullDownStyle );
}

ioPullDownItem* ioPullDownList::GetPullDown( int nIdx ) const
{
	if( m_vPullDownItem.empty() )
		return NULL;

	int iSize = static_cast<int>( m_vPullDownItem.size() );
	if( COMPARE( nIdx , 0, iSize ) )
		return m_vPullDownItem[nIdx];

	return NULL;
}

int ioPullDownList::GetPullDownCount() const
{
	return static_cast<int>( m_vPullDownItem.size() );
}

//선택한 리스트 윈도우(콤보박스)의 Index에 맞는 X,Y 좌표를 돌려줌
bool ioPullDownList::GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos )
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	for( int i = m_iCurPos; i < m_iCurPos + m_iMaxPageList; ++i )
	{
		ioPullDownItem* pPullDown = GetPullDown( i );
		if( !pPullDown )
			continue;

		if( pPullDown->m_iIndex == iSelectIndex )
		{
			rfXPos = iXPos + GetWidth() / 2;
			rfYPos = iYPos + m_iBlankHeight + i * m_iListHeight;
			return true;
		}
	}

	// 없으면 스크롤
	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( pScroll && pScroll->IsShow() )
	{
		rfXPos = pScroll->GetDerivedPosX() + ( pScroll->GetWidth() / 2 );
		rfYPos = pScroll->GetDerivedPosY();
		return true;
	}

	return false;
}

int ioPullDownList::GetScrollBarCutWidth()
{
	ioWnd* pWnd = FindChildWnd( ID_VERT_SCROLL );
	if( pWnd && (int)m_vPullDownItem.size() > m_iMaxPageList )
	{
		return pWnd->GetWidth();
	}

	return 0;
}

ioPullDownItem* ioPullDownList::GetOverPullDown()
{
	if( m_iOverSelectIndex == PullDownEvent::NONE_SELECT )
		return NULL;
	
	int iListSize = m_vPullDownItem.size();
	for(int i = 0 ; i < iListSize ; ++i )
	{
		ioPullDownItem* pPullDown = m_vPullDownItem[i];
		if( pPullDown && pPullDown->m_iIndex == m_iOverSelectIndex )
			return pPullDown;
	}

	return NULL;
}

void ioPullDownList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if ( pPreClick->GetID() == BASIC_TOOLTIP_MGR)
			return;
		if( pPreClick != this		&& 
			pPreClick != m_pOpenBtn && 
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	//부모 윈도우가 하이드 되면 자신도 하이드
	if( GetParent() && !GetParent()->IsShow() )
		HideWnd();

	//풀 다운 아이템의 마우스 이벤트를  체크
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
	{
		LOG.PrintTimeAndLog(0, "%s - pMouse == NULL!!", __FUNCTION__ );
		return;
	}

	for( int i = m_iCurPos; i < m_iCurPos + m_iMaxPageList; ++i )
	{
		ioPullDownItem* pPullDown = GetPullDown( i );
		if( !pPullDown )
			continue;

		int iOverIdx	= pPullDown->OnSelectOver( pMouse->GetMousePos() );
		int iImgOverIdx = pPullDown->OnImageOver( pMouse->GetMousePos() );

		if(	iOverIdx == PullDownEvent::NONE_SELECT )
			continue;

		//풀 다운 아이템에 대한 이벤트
		if( CheckPullDownItem( pMouse, iOverIdx, iImgOverIdx ) )
			return;
		
		//풀 다운 아이템에 등록 된 이미지에 대한 이벤트
		if( CheckPullDownImage( pMouse, iOverIdx, iImgOverIdx ) )
			return;
	}

	RECT rect;
	rect.left	= GetDerivedPosX();
	rect.top	= GetDerivedPosY();
	rect.right	= GetDerivedPosX() + GetWidth();
	rect.bottom	= GetDerivedPosY() + GetHeight();

	if( !::PtInRect( &rect, pMouse->GetMousePos() ) )
	{
		if( m_iOverSelectIndex != PullDownEvent::NONE_SELECT || m_iImageOverIndex != PullDownEvent::NONE_SELECT )
		{
			//풀 다운 아이템 마우스 리브
			OnPullDownLeave( m_iOverSelectIndex, m_iImageOverIndex );
			m_iOverSelectIndex = PullDownEvent::NONE_SELECT;
			m_iImageOverIndex  = PullDownEvent::NONE_SELECT;
		}
	}
}

bool ioPullDownList::CheckPullDownItem( ioLSMouse *pMouse, int iOverIdx, int iOverImageIdx )
{
	//풀다운 아이템 마우스 클릭
	if( m_iOverSelectIndex == iOverIdx && iOverImageIdx == PullDownEvent::NONE_SELECT && pMouse->IsLBtnUp() )
	{			
		OnPullDownClick( iOverIdx, PullDownEvent::NONE_SELECT );
		HideWnd();
		return true;
	}
	//풀다운 아이템 마우스 오버
	else if( m_iOverSelectIndex != iOverIdx )
	{			
		OnPullDownOver( iOverIdx, PullDownEvent::NONE_SELECT );

		m_iOverSelectIndex = iOverIdx;
		m_iImageOverIndex  = PullDownEvent::NONE_SELECT;
		return true;
	}

	return false;
}

bool ioPullDownList::CheckPullDownImage( ioLSMouse *pMouse, int iOverIdx, int iOverImageIdx )
{
	//이미지 마우스 리브
	if( m_iImageOverIndex != PullDownEvent::NONE_SELECT && iOverImageIdx == PullDownEvent::NONE_SELECT )
	{		
		OnPullDownLeave( m_iOverSelectIndex, m_iImageOverIndex );
		m_iImageOverIndex = PullDownEvent::NONE_SELECT;
		return true;
	}
	else
	{
		//이미지 클릭
		if( m_iImageOverIndex != PullDownEvent::NONE_SELECT && m_iImageOverIndex == iOverImageIdx && pMouse->IsLBtnUp() )
		{			
			OnPullDownClick( iOverIdx, iOverImageIdx );
			HideWnd();
			return true;
		}
		//이미지 마우스 오버
		else if( m_iImageOverIndex == PullDownEvent::NONE_SELECT && m_iImageOverIndex != iOverImageIdx )
		{			
			OnPullDownOver( iOverIdx, iOverImageIdx );
			m_iImageOverIndex  = iOverImageIdx;
			m_iOverSelectIndex = iOverIdx;
			return true;
		}
	}

	return false;
}

void ioPullDownList::OnPullDownOver( int iOverIdx, int iOverImageIdx )
{
	PullDownEventListener* pListener = dynamic_cast<PullDownEventListener*>( GetParent() );
	if( !pListener )
	{
		LOG.PrintTimeAndLog(0, "%s - pListener == NULL !!!", __FUNCTION__ );
		return;
	}

	//오버 이벤트 객체 생성
	if( iOverIdx != PullDownEvent::NONE_SELECT && iOverImageIdx == PullDownEvent::NONE_SELECT )
	{
		OnOverPullDownEvent Event( this );
		Event.iOverIdx = iOverIdx;
		pListener->iwm_pulldown_event( Event );
	}
	else if( iOverImageIdx != PullDownEvent::NONE_SELECT )
	{
		OnOverPullDownEvent Event( this );
		Event.iOverIdx		= iOverIdx;
		Event.iOverImageIdx = iOverImageIdx;
		pListener->iwm_pulldown_event( Event );
	}
}

void ioPullDownList::OnPullDownClick( int iClickIdx, int iClickImageIdx )
{
	PullDownEventListener* pListener = dynamic_cast<PullDownEventListener*>( GetParent() );
	if( !pListener )
	{
		LOG.PrintTimeAndLog(0, "%s - pListener == NULL !!!", __FUNCTION__ );
		return;
	}

	//클릭 이벤트 객체 생성
	if( iClickIdx != PullDownEvent::NONE_SELECT && iClickImageIdx == PullDownEvent::NONE_SELECT )
	{
		ClickPullDownEvent Event( this );
		Event.iClickIdx	= iClickIdx;
		pListener->iwm_pulldown_event( Event );
	}
	else if( iClickImageIdx != PullDownEvent::NONE_SELECT )
	{
		ClickPullDownEvent Event( this );
		Event.iClickIdx		 = iClickIdx;
		Event.iClickImageIdx = iClickImageIdx;
		pListener->iwm_pulldown_event( Event );
	}
}

void ioPullDownList::OnPullDownLeave( int iLeaveIdx, int iLeaveImageIdx )
{
	PullDownEventListener* pListener = dynamic_cast<PullDownEventListener*>( GetParent() );
	if( !pListener )
	{
		LOG.PrintTimeAndLog(0, "%s - pListener == NULL !!!", __FUNCTION__ );
		return;
	}

	//마우스 리브 이벤트 객체 생성
	if( iLeaveIdx != PullDownEvent::NONE_SELECT && iLeaveImageIdx == PullDownEvent::NONE_SELECT )
	{
		OnLeavePullDownEvent Event( this );
		Event.iLeaveIdx	= iLeaveIdx;
		pListener->iwm_pulldown_event( Event );
	}	
	else  if( iLeaveImageIdx != PullDownEvent::NONE_SELECT )
	{
		OnLeavePullDownEvent Event( this );
		Event.iLeaveIdx	= iLeaveIdx;
		Event.iLeaveImageIdx = iLeaveImageIdx;
		pListener->iwm_pulldown_event( Event );
	}
}

void ioPullDownList::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	for( int i = m_iCurPos; i < m_iCurPos + m_iMaxPageList; ++i )
	{
		ioPullDownItem* pPullDown = GetPullDown( i );
		if( !pPullDown )
			continue;

		pPullDown->ReSize();
		pPullDown->OnRender( m_iOverSelectIndex, m_iImageOverIndex );
		pPullDown->OnImageListRender( m_iOverSelectIndex, m_iImageOverIndex );
	}
}