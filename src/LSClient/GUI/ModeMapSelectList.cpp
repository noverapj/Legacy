

#include "StdAfx.h"

#include "ModeMapSelectList.h"
#include "GlobalWnd.h"

ModeMapSelectList::ModeMapSelectList() : m_pOpenBtn( NULL )
{
	m_pDarkLine	 = NULL;
	m_pLightLine = NULL;
	m_pOverLine	 = NULL;
	m_pManualMark= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iCutWidth = 0;
	m_iHeightGap= 0;	
	m_iCurPos   = 0;	
	m_iSelectIndex   = -1;
	m_bSafetyLevelRoom = false;
	m_iOverListIndex = -2;
	m_iOverMarkListIndex = -2;
	m_bOverSafetyLevel = false;
	m_iManualMarkX = 0;
	m_iManualMarkWidth = 0;
	m_pModeHelpTooltip  = NULL;
	m_bModeSelect   = false;	
	m_iMaxPageList  = 0;
	m_iBackupWidth = 0;
	m_bAI = false;
}

ModeMapSelectList::~ModeMapSelectList()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
	SAFEDELETE( m_pManualMark );
}

void ModeMapSelectList::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iManualMarkX = xElement.GetIntAttribute_e( "ManualMarkX" );
	m_iManualMarkWidth = xElement.GetIntAttribute_e( "ManualMarkWidth" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
	m_iMaxPageList = xElement.GetIntAttribute_e( "MaxPageList" );
}

void ModeMapSelectList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ModeMapSelectList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void ModeMapSelectList::iwm_hide()
{
	SetCheckParentOutSide( false );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	
}

void ModeMapSelectList::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void ModeMapSelectList::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void ModeMapSelectList::OpenList( ioWnd *pOpenBtn, bool bModeSelect, int iSelectIndex, bool bSafetyLevelRoom, int iStartXPos, int iStartYPos, bool bAI )
{
	if( IsShow() )
	{
		HideWnd();
		return;
	}
	m_vEnableSelectList.clear();

	m_pOpenBtn			= pOpenBtn;
	m_bModeSelect       = bModeSelect;
	m_iStartXPos		= iStartXPos;
	m_iStartYPos		= iStartYPos;	
	m_iSelectIndex		= iSelectIndex;
	m_bSafetyLevelRoom	= bSafetyLevelRoom;
	m_bAI = bAI;
	ShowWnd();
}

void ModeMapSelectList::AddList( int iIndex, bool bSafetyLevelRoom, const ioHashString &rkName, int iHelpIndex, int iManualIndex, ioUIRenderImage *pMarkRes, int iAddWidth )
{
	SelectList kList;
	kList.m_iIndex			 = iIndex;
	kList.m_iHelpIndex       = iHelpIndex;
	kList.m_iManualIndex     = iManualIndex;
	kList.m_szTitle			 = rkName;
	kList.m_bSafetyLevelRoom = bSafetyLevelRoom;
	kList.m_pMarkRes         = pMarkRes;
	if( m_iSelectIndex == iIndex && 
		m_bSafetyLevelRoom == bSafetyLevelRoom )
	{
		kList.m_bCheck = true;
	}
	m_vEnableSelectList.push_back( kList );

	if( m_iBackupWidth == 0 )
		m_iBackupWidth = GetWidth();
	if( m_bAI )		
		SetSize( m_iBackupWidth + 1 + iAddWidth, ( min( m_iMaxPageList, (int)m_vEnableSelectList.size() ) * m_iHeightGap ) + m_iAddHeight );
	else
		SetSize( m_iBackupWidth + iAddWidth, ( min( m_iMaxPageList, (int)m_vEnableSelectList.size() ) * m_iHeightGap ) + m_iAddHeight );
	SetWndPos( m_iStartXPos, m_iStartYPos - GetHeight() );

	m_iCurPos = 0;
	if( (int)m_vEnableSelectList.size() <= m_iMaxPageList )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_vEnableSelectList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}
}

bool ModeMapSelectList::GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos )
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;

	int iListSize = m_vEnableSelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		if( m_vEnableSelectList[iListPos].m_iIndex == iSelectIndex )
		{
			rfXPos = iXPos + GetWidth() / 2;
			rfYPos = iYPos + m_iListYPos + iIndex * m_iHeightGap;
			return true;
		}
	}


	// 없으면 스크롤
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( pScroll && pScroll->IsShow() )
	{
		rfXPos = pScroll->GetDerivedPosX() + ( pScroll->GetWidth() / 2 );
		rfYPos = pScroll->GetDerivedPosY();
		return true;
	}
	return false;
}

void ModeMapSelectList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex  = -2;
	m_iOverMarkListIndex = -2;
	m_bOverSafetyLevel= false;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != this && 
			pPreClick != m_pOpenBtn &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	int iManualMarkWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;
	if( m_bModeSelect )
		iManualMarkWidth = m_iManualMarkWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_vEnableSelectList.size();
	for(int i = 0;i < iListSize;i++)
	{
		if( i >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + i;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + i * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - ( iCutWidth + iManualMarkWidth ) ) - ( m_iListXPos * 2 ) );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;
		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			SelectList &kList = m_vEnableSelectList[iListPos];
			m_iOverListIndex  = kList.m_iIndex;
			m_bOverSafetyLevel= kList.m_bSafetyLevelRoom;
			if( pMouse->IsLBtnUp() )
			{
				DWORD dwParam = MAKELONG( kList.m_bSafetyLevelRoom, kList.m_iIndex );
				GetParent()->iwm_command( m_pOpenBtn, GetID(), dwParam );
				HideWnd();
			}
			break;
		}

		if( m_bModeSelect )
		{
			// 리스트 도움말
			rcNameRect.left = iXPos + m_iListXPos + ( m_iManualMarkX - iCutWidth );
			rcNameRect.top  = iYPos + m_iListYPos + i * m_iHeightGap;
			rcNameRect.right  = rcNameRect.left + m_iManualMarkWidth;
			rcNameRect.bottom = rcNameRect.top  + m_iHeightGap;
			if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
			{
				SelectList &kList = m_vEnableSelectList[iListPos];
				m_iOverMarkListIndex  = kList.m_iIndex;
				if( pMouse->IsLBtnUp() )
				{
					// 공략 창
					if( kList.m_iManualIndex != -1 )
					{
						HideWnd();
						ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
						if( pManualWnd )
							pManualWnd->ShowWndByCurManual( kList.m_iManualIndex );
					}				
				}
				break;
			}
		}
	}

	ProcessModeHelpTooltip();
}

void ModeMapSelectList::ProcessModeHelpTooltip()
{
	if( !IsShow() ) return;

	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{		
		if( m_iOverMarkListIndex != -2 )
		{
			int iListSize = m_vEnableSelectList.size();
			for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
			{
				SelectList &kList = m_vEnableSelectList[iIndex];
				if( m_iOverMarkListIndex == kList.m_iIndex )
				{
					pModeHelpTooltip->SetModeHelpTooltip( this, kList.m_szTitle, kList.m_iHelpIndex );
					break;
				}
			}			
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}

void ModeMapSelectList::OnRender()
{
	ioWnd::OnRender();
	
	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_vEnableSelectList.size();
	for(int i = 0;i < iListSize;i++)
	{
		if( i >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + i;
		if( iListPos >= iListSize )
			break;

		const SelectList &rkSelectList = m_vEnableSelectList[iListPos];
		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == rkSelectList.m_iIndex && m_bOverSafetyLevel == rkSelectList.m_bSafetyLevelRoom )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( i * m_iHeightGap ) + 3, FONT_SIZE_12, rkSelectList.m_szTitle.c_str() );
		
		if( rkSelectList.m_pMarkRes )
		{
			int iTitleSize = g_FontMgr.GetTextWidth( rkSelectList.m_szTitle.c_str(), TS_NORMAL, FONT_SIZE_12 );
			rkSelectList.m_pMarkRes->Render( iXPos + m_iListXPos + 7 + iTitleSize, iYPos + m_iListYPos + ( i * m_iHeightGap ) + 5 );
		}

		if( m_bModeSelect && m_pManualMark )
		{
			if( m_iOverMarkListIndex == rkSelectList.m_iIndex )
				m_pManualMark->SetColor( 206, 78, 0 );
			else
				m_pManualMark->SetColor( 128, 128, 128 );

			m_pManualMark->Render( iXPos + m_iListXPos + ( m_iManualMarkX - iCutWidth ) + 3, iYPos + m_iListYPos + ( i * m_iHeightGap ) + 3, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
CreateModeSelectList::CreateModeSelectList() : m_pOpenBtn( NULL )
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;
	m_pManualMark = NULL;

	m_iMaxPageList = 10;
	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iHeightGap= 0;
	m_iCutWidth = 0;
	m_iOverListIndex = -2;
	m_iOverMarkListIndex = -2;
	m_iCurPos   = 0;
	m_iManualMarkX = 0;
	m_iManualMarkWidth = 0;
	m_pModeHelpTooltip  = NULL;
}

CreateModeSelectList::~CreateModeSelectList()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
	SAFEDELETE( m_pManualMark );
}

void CreateModeSelectList::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
	m_iManualMarkX = xElement.GetIntAttribute_e( "ManualMarkX" );
	m_iManualMarkWidth = xElement.GetIntAttribute_e( "ManualMarkWidth" );
	m_iMaxPageList = xElement.GetIntAttribute_e( "MaxPageList" );
}

void CreateModeSelectList::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CreateModeSelectList::iwm_show()
{
	SetCheckParentOutSide( true );
}

void CreateModeSelectList::iwm_hide()
{
	SetCheckParentOutSide( false );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{
		pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
	}	
}

void CreateModeSelectList::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void CreateModeSelectList::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void CreateModeSelectList::OpenList( ioWnd *pOpenBtn )
{
	if( IsShow() )
	{
		HideWnd();
		return;
	}
	m_vEnableSelectList.clear();

	m_pOpenBtn			= pOpenBtn;	
	ShowWnd();
}

void CreateModeSelectList::AddList( int iIndex, const ioHashString &rkName, int iHelpIndex, int iManualIndex, ioUIRenderImage *pMarkRes )
{
	SelectList kList;
	kList.m_iIndex			 = iIndex;
	kList.m_iHelpIndex       = iHelpIndex;
	kList.m_iManualIndex     = iManualIndex;
	kList.m_szTitle			 = rkName;
	kList.m_pMarkRes		 = pMarkRes;
	m_vEnableSelectList.push_back( kList );

	SetSize( GetWidth(), ( min( m_iMaxPageList, (int)m_vEnableSelectList.size() ) * m_iHeightGap ) + m_iAddHeight );

	m_iCurPos = 0;
	if( (int)m_vEnableSelectList.size() <= m_iMaxPageList )
	{
		HideChildWnd( ID_VERT_SCROLL );
	}
	else
	{
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_vEnableSelectList.size() );
			pScroll->SetScrollPos( m_iCurPos );
			pScroll->ShowWnd();
		}
	}
}

bool CreateModeSelectList::GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos )
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;

	int iListSize = m_vEnableSelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		if( m_vEnableSelectList[iListPos].m_iIndex == iSelectIndex )
		{
			rfXPos = iXPos + GetWidth() / 2;
			rfYPos = iYPos + m_iListYPos + iIndex * m_iHeightGap;
			return true;
		}
	}

	// 없으면 스크롤
	ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
	if( pScroll && pScroll->IsShow() )
	{
		rfXPos = pScroll->GetDerivedPosX() + ( pScroll->GetWidth() / 2 );
		rfYPos = pScroll->GetDerivedPosY();
		return true;
	}
	return false;
}

void CreateModeSelectList::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_iOverListIndex  = -2;
	m_iOverMarkListIndex  = -2;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != m_pOpenBtn &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	int iManualMarkWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;
	if( m_pManualMark )
		iManualMarkWidth = m_iManualMarkWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_vEnableSelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		// 리스트
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - ( m_iListXPos * 2 ) ) - ( iCutWidth + iManualMarkWidth ) );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;
		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			SelectList &kList = m_vEnableSelectList[iListPos];
			m_iOverListIndex  = kList.m_iIndex;
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( m_pOpenBtn, GetID(), m_iOverListIndex );
				HideWnd();
			}
			break;
		}

		// 리스트 도움말
		rcNameRect.left = iXPos + m_iListXPos + ( m_iManualMarkX - iCutWidth );
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + m_iManualMarkWidth;
		rcNameRect.bottom = rcNameRect.top  + m_iHeightGap;
		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			SelectList &kList = m_vEnableSelectList[iListPos];
			m_iOverMarkListIndex  = kList.m_iIndex;
			if( pMouse->IsLBtnUp() )
			{
				// 공략 창
				if( kList.m_iManualIndex != -1 )
				{
					if( GetParent() )
						GetParent()->HideWnd();
					HideWnd();

					ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
					if( pManualWnd )
						pManualWnd->ShowWndByCurManual( kList.m_iManualIndex );
				}				
			}
			break;
		}
	}
	ProcessModeHelpTooltip();
}

void CreateModeSelectList::ProcessModeHelpTooltip()
{
	if( !IsShow() ) return;

	if( m_pModeHelpTooltip == NULL )
		m_pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( FindChildWnd( ID_MODE_HELP_TOOLTIP ) );

	ModeHelpTooltip *pModeHelpTooltip = dynamic_cast< ModeHelpTooltip * >( m_pModeHelpTooltip );	
	if( pModeHelpTooltip )
	{		
		if( m_iOverMarkListIndex != -2 )
		{
			int iListSize = m_vEnableSelectList.size();
			for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
			{
				SelectList &kList = m_vEnableSelectList[iIndex];
				if( m_iOverMarkListIndex == kList.m_iIndex )
				{
					pModeHelpTooltip->SetModeHelpTooltip( this, kList.m_szTitle, kList.m_iHelpIndex );
					break;
				}
			}			
		}
		else
		{
			pModeHelpTooltip->SetModeHelpTooltip( NULL, "", -1 );
		}
	}	
}

void CreateModeSelectList::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_vEnableSelectList.size() > m_iMaxPageList )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_vEnableSelectList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= m_iMaxPageList )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_iOverListIndex == m_vEnableSelectList[iListPos].m_iIndex )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintText( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, m_vEnableSelectList[iListPos].m_szTitle.c_str() );

		if( m_vEnableSelectList[iListPos].m_pMarkRes )
		{
			int iTitleSize = g_FontMgr.GetTextWidth( m_vEnableSelectList[iListPos].m_szTitle.c_str(), TS_NORMAL, FONT_SIZE_12 );
			m_vEnableSelectList[iListPos].m_pMarkRes->Render( iXPos + m_iListXPos + 7 + iTitleSize, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 5 );
		}

		if( m_pManualMark )
		{
			if( m_iOverMarkListIndex == m_vEnableSelectList[iListPos].m_iIndex )
				m_pManualMark->SetColor( 206, 78, 0 );
			else
				m_pManualMark->SetColor( 128, 128, 128 );
			m_pManualMark->Render( iXPos + m_iListXPos + ( m_iManualMarkX - iCutWidth ) + 3, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, UI_RENDER_NORMAL_ADD_COLOR );
		}
	}
}

