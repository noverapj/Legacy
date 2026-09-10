#include "StdAfx.h"

#include "ADImagePanelWnd.h"

ADImagePanelWnd::ADImagePanelWnd()
{
	m_pBorder			= NULL;

	m_ADState			= ADS_DELAY;
	m_SlideMove			= SM_NEXT;

	m_pCurrBanner		= NULL;
	m_pPrevBanner		= NULL;

	m_dwDelayStartTime	= 0;
	m_dwDelayTime		= 0;

	m_dPagingStartTime	= 0;
	m_dPagingTime		= 0;

	m_fCurrRate			= FLOAT1;

	m_iCurrIndex		= 0;
}

ADImagePanelWnd::~ADImagePanelWnd()
{
	SAFEDELETE( m_pBorder );
}

void ADImagePanelWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "Border" )
	{
		SAFEDELETE( m_pBorder );
		m_pBorder = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ADImagePanelWnd::iwm_show()
{
	HideChildWnd( ID_PREV_BTN );
	HideChildWnd( ID_NEXT_BTN );

	m_dwDelayStartTime	= FRAMEGETTIME();
	m_fCurrRate			= 0.0f;
	m_iCurrIndex		= 0;

	m_pCurrBanner		= NULL;
	m_pPrevBanner		= NULL;
	m_SlideMove			= SM_NEXT;

	ParseAD();
	ChangeAD();
	ParseDirectADButton();

	m_ADState			= ADS_DELAY;
}

void ADImagePanelWnd::iwm_hide()
{
	m_pCurrBanner		= NULL;
	m_pPrevBanner		= NULL;
}

void ADImagePanelWnd::iwm_mouseover( const ioMouse& mouse )
{
	ioButton::iwm_mouseover( mouse );

	if( !IsShow() )
		return;

	ShowChildWnd( ID_PREV_BTN );
	ShowChildWnd( ID_NEXT_BTN );
}

void ADImagePanelWnd::iwm_mouseleave( const ioMouse& mouse )
{
	ioButton::iwm_mouseleave( mouse );

	if( !IsShow() )
		return;

	if( !IsInWndRect( mouse.GetMousePos() ) )
	{
		HideChildWnd( ID_PREV_BTN );
		HideChildWnd( ID_NEXT_BTN );
	}
}

void ADImagePanelWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_PREV_BTN:
		{
			if( cmd == IOBN_BTNUP && m_ADState == ADS_DELAY )
			{				
				CheckPrevMoveADIndex();
				ChangeAD();
				CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
			}
		}
		break;
	case ID_NEXT_BTN:
		{
			if( cmd == IOBN_BTNUP && m_ADState == ADS_DELAY )
			{				
				CheckNextMoveADIndex();
				ChangeAD();
				CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
			}
		}
		break;
	case ID_AD_DIRECT_BTN01:
	case ID_AD_DIRECT_BTN02:
	case ID_AD_DIRECT_BTN03:
	case ID_AD_DIRECT_BTN04:
	case ID_AD_DIRECT_BTN05:
	case ID_AD_DIRECT_BTN06:
	case ID_AD_DIRECT_BTN07:
	case ID_AD_DIRECT_BTN08:
	case ID_AD_DIRECT_BTN09:
	case ID_AD_DIRECT_BTN10:
		{
			if( cmd == IOBN_BTNUP && m_ADState == ADS_DELAY )
			{				
				CheckDirectMoveADIndex( dwID - ID_AD_DIRECT_BTN01 );
				ChangeAD();
				CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
			}
		}
		break;
	}
}

void ADImagePanelWnd::ParseAD()
{
	m_ADIndexList.clear();

	//유효한 광고 찾기
	for( int i = 0; i < g_ShopManager.GetImageADCount(); ++i )
	{
		const ADImage* pCurrAD = g_ShopManager.GetImageAD( i );
		if( pCurrAD )
		{
			m_ADIndexList.push_back( i );
		}
	}
}

ioUIRenderImage* ADImagePanelWnd::GetImage( int index )
{
	if( index >= (int)m_ADIndexList.size() )
	{
		index = 0;
	}
	else if( index < 0 )
	{
		index = (int)m_ADIndexList.size() - 1;
	}

	const ADImage* pCurrAD = g_ShopManager.GetImageAD( m_ADIndexList[index] );
	if( pCurrAD )
	{
		return pCurrAD->m_pImage;
	}

	return NULL;
}

void ADImagePanelWnd::ChangeAD()
{
	if( !COMPARE( m_iCurrIndex, 0, (int)m_ADIndexList.size() ) )
		return;

	m_fCurrRate			= 0.0f;
	m_ADState			= ADS_PAGING;
	m_dPagingStartTime	= FRAMEGETTIME();

	const ADImage* pCurrAD = g_ShopManager.GetImageAD( m_ADIndexList[m_iCurrIndex] );
	if( pCurrAD )
	{
		if( m_pCurrBanner )
		{
			m_pPrevBanner = m_pCurrBanner;
		}
		else
		{
			switch( m_SlideMove )
			{
			case SM_NEXT:
				m_pPrevBanner = GetImage( m_iCurrIndex + 1 );
				break;
			case SM_PREV:
				m_pPrevBanner = GetImage( m_iCurrIndex - 1 );
				break;
			}
		}

		m_pCurrBanner	= pCurrAD->m_pImage;

		m_dwDelayTime	= pCurrAD->m_dwHoldTime;
		m_dPagingTime	= pCurrAD->m_dPagingTime;
	}
	else
	{
		m_dwDelayTime	= 0;
		m_dPagingTime	= 0;
	}
}

void ADImagePanelWnd::CheckPrevMoveADIndex()
{
	m_SlideMove = SM_PREV;
	m_iCurrIndex--;
	if( !COMPARE( m_iCurrIndex, 0, (int)m_ADIndexList.size() ) )
	{
		m_iCurrIndex = (int)m_ADIndexList.size() - 1;
	}	
}

void ADImagePanelWnd::CheckNextMoveADIndex()
{
	m_SlideMove = SM_NEXT;
	m_iCurrIndex++;
	if( !COMPARE( m_iCurrIndex, 0, (int)m_ADIndexList.size() ) )
	{
		m_iCurrIndex = 0;
	}
}

void ADImagePanelWnd::CheckDirectMoveADIndex( int iTargetIndex )
{
	if( m_iCurrIndex < iTargetIndex )
	{
		m_SlideMove		= SM_NEXT;
		m_iCurrIndex	= iTargetIndex;
	}
	else if( m_iCurrIndex > iTargetIndex )
	{
		m_SlideMove		= SM_PREV;
		m_iCurrIndex	= iTargetIndex;
	}
}

void ADImagePanelWnd::ParseDirectADButton()
{
	int i		= 0;
	int iXPos	= 692 - (int)m_ADIndexList.size() * 22;

	for( int iButtonID = ID_AD_DIRECT_BTN01; iButtonID < ID_AD_DIRECT_BTN_MAX; ++iButtonID, ++i )
	{
		ioRadioButton* pRadio = dynamic_cast<ioRadioButton*>( FindChildWnd( iButtonID ) );
		if( pRadio )
		{
			if( COMPARE( i, 0, (int)m_ADIndexList.size() ) )
			{
				const ADImage* pCurrAD = g_ShopManager.GetImageAD( m_ADIndexList[i] );
				if( pCurrAD )
				{
					pRadio->SetWndPos( iXPos, 200 );
					pRadio->ShowWnd();

					iXPos += pRadio->GetWidth() + 4;
				}
				else
				{
					pRadio->HideWnd();
				}
			}
			else
			{
				pRadio->HideWnd();
			}
		}
	}

	CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
}


bool ADImagePanelWnd::IsSliding()
{ 
	if( m_ADState == ADS_PAGING )
		return true;

	return false;
}

void ADImagePanelWnd::MoveDirectLink()
{
	NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( !pNewShopWnd )
		return;

	const ADImage* pCurrAD = g_ShopManager.GetImageAD( m_ADIndexList[m_iCurrIndex] );
	if( !pCurrAD )
		return;
	
	//링크 정보 체크
	if( pCurrAD->m_Link.IsNoneLink() )
		return;
		
	if( pCurrAD->m_Link.m_eADProductType == ADPT_DECO )
	{
		pNewShopWnd->SetReserveDecoSex( pCurrAD->m_Link.m_iValue1 );
		pNewShopWnd->SetDirectSmallTab( pCurrAD->m_Link.m_iTab, NewShopWnd::ID_SMALL_TAB_1_BTN + pCurrAD->m_Link.m_iSubTab, pCurrAD->m_Link.m_iValue2, pCurrAD->m_Link.m_iValue3 );
	}
	else
	{
		pNewShopWnd->SetDirectSmallTab( pCurrAD->m_Link.m_iTab, NewShopWnd::ID_SMALL_TAB_1_BTN + pCurrAD->m_Link.m_iSubTab, pCurrAD->m_Link.m_iValue1, pCurrAD->m_Link.m_iValue2 );
	}
}


void ADImagePanelWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_ADState )
	{
	case ADS_DELAY:
		if( m_dwDelayStartTime + m_dwDelayTime < dwCurTime )
		{
			switch( m_SlideMove )
			{
			case SM_PREV:
				CheckPrevMoveADIndex();
				ChangeAD();
				CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
				break;
			case SM_NEXT:			
				CheckNextMoveADIndex();
				ChangeAD();
				CheckRadioButton( ID_AD_DIRECT_BTN01, ID_AD_DIRECT_BTN10, ID_AD_DIRECT_BTN01 + m_iCurrIndex );
				break;
			}
		}
		break;
	case ADS_PAGING:
		{
			DWORD dwMoveTime = dwCurTime - m_dPagingStartTime;
			m_fCurrRate = (float)dwMoveTime / m_dPagingTime;
			m_fCurrRate = min( m_fCurrRate, FLOAT1 );

			if( m_fCurrRate == FLOAT1 )
			{				
				ParseDirectADButton();

				m_ADState			= ADS_DELAY;
				m_dwDelayStartTime	= dwCurTime;
			}			
		}
		break;
	}
}

void ADImagePanelWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_ADState )
	{
	case ADS_DELAY:	
		{
			RECT RenderRect = { 0, 0, 705, 224 };
			if( m_pCurrBanner )
				m_pCurrBanner->RenderRect( iXPos + 2, iYPos + 2, UI_RENDER_NORMAL, RenderRect );
		}
		break;
	case ADS_PAGING:
		{
			switch( m_SlideMove )
			{
			case SM_PREV:
				{
					OnPrevPaigingRender( iXPos + 2, iYPos + 2 );
				}
				break;
			case SM_NEXT:
				{
					OnNextPaigingRender( iXPos + 2, iYPos + 2 );
				}
				break;
			}
		}
		break;
	}

	if( m_pBorder )
		m_pBorder->Render( iXPos, iYPos );
}

void ADImagePanelWnd::OnPrevPaigingRender( int iXPos, int iYPos )
{	
	if( m_fCurrRate == FLOAT1 )
	{
		RECT RenderRect = { 0, 0, 705, 224 };
		if( m_pCurrBanner )
			m_pCurrBanner->RenderRect( iXPos, iYPos, UI_RENDER_NORMAL, RenderRect );
	}
	else
	{
		int iCurrWidth = 705 * sin( D3DX_PI/2 * m_fCurrRate );
		int iNextWidth = 705 * sin( D3DX_PI/2 * ( FLOAT1 -  m_fCurrRate ) );

		RECT RenderPrevRect = { iNextWidth, 0, 705, 224 };
		if( m_pCurrBanner )
			m_pCurrBanner->RenderRect( iXPos - iNextWidth, iYPos, UI_RENDER_NORMAL, RenderPrevRect );

		RECT RenderRect = { 0, 0, iNextWidth, 224 };
		if( m_pPrevBanner )
			m_pPrevBanner->RenderRect( iXPos + (705 - iNextWidth), iYPos, UI_RENDER_NORMAL, RenderRect );
	}
}

void ADImagePanelWnd::OnNextPaigingRender( int iXPos, int iYPos )
{
	if( m_fCurrRate == FLOAT1 )
	{
		RECT RenderRect = { 0, 0, 705, 224 };
		if( m_pPrevBanner )
			m_pPrevBanner->RenderRect( iXPos, iYPos, UI_RENDER_NORMAL, RenderRect );
	}
	else
	{		
		int iNextWidth = 705 * sin( D3DX_PI/2 * m_fCurrRate );

		RECT RenderPrevRect = { iNextWidth, 0, 705, 224 };
		if( m_pPrevBanner )
			m_pPrevBanner->RenderRect( iXPos - iNextWidth, iYPos, UI_RENDER_NORMAL, RenderPrevRect );
		
		RECT RenderRect = { 0, 0, iNextWidth, 224 };
		if( m_pCurrBanner )
			m_pCurrBanner->RenderRect( iXPos + (705 - iNextWidth), iYPos, UI_RENDER_NORMAL, RenderRect );
	}	
}