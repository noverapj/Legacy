#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include "../Setting.h"
#include "../TextColorTable.h"
#include "../PlayEntityHelpStruct.h"
#include "../NetworkWrappingFunc.h"

#include "../ioMyInfo.h"
#include "../ioApplication.h"
#include "../ioPlayStage.h"
#include "../ioBaseChar.h"
#include "../ioFishingManager.h"
#include "../ioMyLevelMgr.h"

#include "../ioEtcItem.h"
#include "../ioEtcItemManager.h"

#include "../WndID.h"

#include "ioSP2GUIManager.h"
#include "NewShopWnd.h"
#include "GlobalWnd.h"
#include "ioFishingMessageBox.h"
#include "FishingInventoryWnd.h"

#include <strsafe.h>
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/Safesprintf.h"

#include "../ioDateHelp.h"



//////////////////////////////////////////////////////////////////////////
FishingIventoryCategoryTab::FishingIventoryCategoryTab()
{

}

FishingIventoryCategoryTab::~FishingIventoryCategoryTab()
{

}

void FishingIventoryCategoryTab::ClearTab(int iIndex)
{
	if (iIndex == -1)
	{
		for(int i = 0; i < (int)m_Setting.size(); i++)
			m_Setting[i].Clear();
	}
	else
		m_Setting[iIndex].Clear();
	HideWnd();
}

void FishingIventoryCategoryTab::SetTab( const TabUISettingVec& rkTabVec ) 
{
	m_Setting = rkTabVec;
	ShowWnd();
}

const TabUISetting& FishingIventoryCategoryTab::GetTab(int iIndex)
{
	return m_Setting[iIndex]; 
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FishingItemInfoWnd::FishingItemInfoWnd()
{
	m_iItemType = -1;
	m_dwEtcItemCode = 0;
	m_iSlotNum = -1;
	m_pIcon = NULL;
	m_pBack = NULL;
	m_pSelect = NULL;
	m_pUse = NULL;
	m_bOwn = true;
	m_bSelected = false;
	m_bUse = false;
}

FishingItemInfoWnd::~FishingItemInfoWnd()
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pBack);
	SAFEDELETE(m_pSelect);
	SAFEDELETE(m_pUse);
}

void FishingItemInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "SlotBack" )
	{
		SAFEDELETE(m_pBack);
		m_pBack = pImage;
	}
	else if( szType == "SelectSlot" )
	{
		SAFEDELETE(m_pSelect);
		m_pSelect = pImage;
	}
	else if( szType == "UseFishingGear" )
	{
		SAFEDELETE(m_pUse);
		m_pUse = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FishingItemInfoWnd::SetInfo( int iItemCode, int iSlotNum, DWORD dwGearType, DWORD dwEtcItemCode )
{
	m_iItemType = iItemCode;
	m_iSlotNum = iSlotNum;
	m_dwGearType = dwGearType;
	m_dwEtcItemCode = dwEtcItemCode;

	if( m_iItemType < 0 )
	{
		SAFEDELETE( m_pIcon );
		SetInActive();
	}
	else
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName(g_FishingMgr.GetFishingItemIconName(m_iItemType, (FishingGearType)m_dwGearType));

		if( m_dwGearType == (DWORD)FGT_NONE )
		{
			SetActive();
			m_bOwn = true;
			return;
		}

		if( g_MyInfo.GetFishingGearOwnByTabType( iSlotNum - 1, (FishingGearType)m_dwGearType ) )
		{
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			int iArray = m_iSlotNum - 1;
			int iItemEtcCode = g_MyInfo.GetSelectFishingGearEtcCode( iArray, (FishingGearType)m_dwGearType );
			if( pUserEtcItem && pUserEtcItem->CheckDurationOfFishingGear( iItemEtcCode ) )
				m_bOwn = true;
			else
				m_bOwn = false;
		}
		else
			m_bOwn = false;
		SetActive();
		if( (FishingGearType)dwGearType != FGT_NONE )
		{
			if( g_MyInfo.CheckUseFishingGear( iSlotNum - 1, (FishingGearType)m_dwGearType ) )
				m_bUse = true;
			else
				m_bUse = false;
		}
	}
}

bool FishingItemInfoWnd::CheckEmptySlot()
{
	if( m_iItemType == -1 )
		return true;
	return false;
}

void FishingItemInfoWnd::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		if( m_pBack )
		{
			m_pBack->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( 49, 70, 91 );
		g_FontMgr.PrintText( iXPos + 21, iYPos + 14, FONT_SIZE_12, STR(1), m_iSlotNum );
	}
	else if( m_pIcon )
	{
		UIRenderType eNewType = UI_RENDER_NORMAL;
		if( !m_bOwn )
			eNewType = UI_RENDER_GRAY;

		if( m_pSelect && m_bSelected )
			m_pSelect->Render( iXPos + 21, iYPos + 21, UI_RENDER_NORMAL, TFO_BILINEAR );

		m_pIcon->SetScale( 0.65f );
		m_pIcon->Render( iXPos + 21, iYPos + 21, eNewType, TFO_BILINEAR );

		if( IsOver() )
		{
			if( m_bOwn )
				m_pIcon->Render( iXPos + 21, iYPos + 21, UI_RENDER_SCREEN, TFO_BILINEAR );
			else
				m_pIcon->Render( iXPos + 21, iYPos + 21, UI_RENDER_GRAY_LIGHT, TFO_BILINEAR );
		}

		if( m_pUse && m_bUse )
		{
			m_pUse->Render( iXPos + 23, iYPos + 26, UI_RENDER_NORMAL, TFO_BILINEAR );

			// 선택한 낚시 장비 설명
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 23, iYPos + 26 + 3, FONT_SIZE_11, STR(2) );
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
FishingItemPageBtn::FishingItemPageBtn()
{
	m_pActiveImage = NULL;
	m_pInActiveImage = NULL;
}

FishingItemPageBtn::~FishingItemPageBtn()
{
	SAFEDELETE(m_pActiveImage);
	SAFEDELETE(m_pInActiveImage);
}

void FishingItemPageBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ActiveImage" )
	{
		SAFEDELETE(m_pActiveImage);
		m_pActiveImage = pImage;
	}
	else if( szType == "InActiveImage" )
	{
		SAFEDELETE(m_pInActiveImage);
		m_pInActiveImage = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FishingItemPageBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		if(m_pInActiveImage)
		{
			m_pInActiveImage->Render( iXPos+6, iYPos+9, UI_RENDER_NORMAL );
		}
	}
	else
	{
		if(m_pActiveImage)
		{
			m_pActiveImage->Render( iXPos+6, iYPos+9, UI_RENDER_NORMAL );

			if( IsOver() )
			{
				m_pActiveImage->Render( iXPos+6, iYPos+9, UI_RENDER_SCREEN );
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
FishingExpWnd::FishingExpWnd()
{
	m_pExpGauge = NULL;

	m_iCurLevel = 0;
	m_iCurExp = 0;
}

FishingExpWnd::~FishingExpWnd()
{
	SAFEDELETE(m_pExpGauge);
}

void FishingExpWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Exp_Gauge" )
	{
		SAFEDELETE(m_pExpGauge);
		m_pExpGauge = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FishingExpWnd::SetInfo( int iLevel, int iExp )
{
	m_iCurLevel = iLevel;
	m_iCurExp = iExp;
}

void FishingExpWnd::SetSelectItemInfo( ioHashString info )
{
	m_InfoText = info;
}

void FishingExpWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	float fExpRate = 0.0f;
	int iMaxExp = g_LevelMgr.GetNextFishingLevelUpExp( m_iCurLevel );
	if( iMaxExp != 0 )
		fExpRate = (float)m_iCurExp / (float)iMaxExp;

	if( m_pExpGauge )
	{
		int iWidth = m_pExpGauge->GetWidth();
		iWidth *= fExpRate;
		m_pExpGauge->RenderWidthCut( iXPos, iYPos, 0, iWidth );
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos - 73, iYPos - 4, FONT_SIZE_12, STR(1), m_iCurLevel );

	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iXPos + 211, iYPos - 4, FONT_SIZE_12, STR(2), (int)( fExpRate * FLOAT100 ) );

	if( !m_InfoText.IsEmpty() )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos - 71, iYPos + 17, FONT_SIZE_12, m_InfoText.c_str() );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
FishingScreenBtn::FishingScreenBtn()
{
	m_bScreen = false;
	m_bFishing = false;
	m_dwScreenStartTime = 0;
}

FishingScreenBtn::~FishingScreenBtn()
{
}

void FishingScreenBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwScreenTimer = xElement.GetIntAttribute_e( "ScreenTime" );
}

void FishingScreenBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ProcessScreenEffect();
}

void FishingScreenBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_bScreen && !m_bFishing )
	{
		ioButton::OnDrawOvered( iXPos, iYPos );
	}
	else
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}
}

void FishingScreenBtn::SetFishingState( bool bFishing )
{
	m_bFishing = bFishing;

	if( !m_bFishing )
		SetTitleText( STR(1) );
	else
		SetTitleText( STR(2) );
}

void FishingScreenBtn::ProcessScreenEffect()
{
	if( m_dwScreenStartTime == 0 )
		m_dwScreenStartTime = FRAMEGETTIME();

	if( m_bScreen )
	{
		if( FRAMEGETTIME() - m_dwScreenStartTime > m_dwScreenTimer )
		{
			m_dwScreenStartTime = FRAMEGETTIME();
			m_bScreen      = false;
		}
	}	
	else
	{
		if( FRAMEGETTIME() - m_dwScreenStartTime > m_dwScreenTimer )
		{
			m_dwScreenStartTime = FRAMEGETTIME();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
FishingGearEquipBtn::FishingGearEquipBtn()
{
}

FishingGearEquipBtn::~FishingGearEquipBtn()
{
}

void FishingGearEquipBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
}

void FishingGearEquipBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void FishingGearEquipBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
}

/////////////////////////////////////////////////////////////////////////////////////
FishingBaitListWnd::FishingBaitListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_pOpenBtn  = NULL;
	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iOverListIndex = -2;
}

FishingBaitListWnd::~FishingBaitListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);

	m_vBaitList.clear();
}

void FishingBaitListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
}

void FishingBaitListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

bool FishingBaitListWnd::IsEmptyList()
{
	CheckBaitList();

	return m_vBaitList.empty();
}

void FishingBaitListWnd::CheckBaitList()
{
	m_vBaitList.clear();

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
		return;

	ioUserEtcItem::ETCITEMSLOT kBaitSlot;

	if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_BAIT, kBaitSlot ) )
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_FISHING_BAIT );
		if( pEtcItem )
		{
			BaitList kList;
			kList.m_iIndex = pEtcItem->GetType();
			if( !pEtcItem->GetExtraName().IsEmpty() )
				kList.m_szTitle= pEtcItem->GetExtraName();
			else
				kList.m_szTitle= pEtcItem->GetName();
			kList.m_iCount = kBaitSlot.GetUse();
			m_vBaitList.push_back( kList );
		}
	}

	if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_BAIT, kBaitSlot ) )
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_FISHING_MOON_BAIT );
		if( pEtcItem )
		{
			BaitList kList;
			kList.m_iIndex = pEtcItem->GetType();
			if( !pEtcItem->GetExtraName().IsEmpty() )
				kList.m_szTitle= pEtcItem->GetExtraName();
			else
				kList.m_szTitle= pEtcItem->GetName();
			kList.m_iCount = kBaitSlot.GetUse();
			m_vBaitList.push_back( kList );
		}
	}

	if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT, kBaitSlot ) )
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT );
		if( pEtcItem )
		{
			BaitList kList;
			kList.m_iIndex = pEtcItem->GetType();
			if( !pEtcItem->GetExtraName().IsEmpty() )
				kList.m_szTitle= pEtcItem->GetExtraName();
			else
				kList.m_szTitle= pEtcItem->GetName();
			kList.m_iCount = kBaitSlot.GetUse();
			m_vBaitList.push_back( kList );
		}
	}
}

void FishingBaitListWnd::OpenList( ioWnd *pOpenBtn )
{
	if( IsShow() || !pOpenBtn )
	{
		HideWnd();
		return;
	}

	m_pOpenBtn			= pOpenBtn;
	m_iStartXPos		= m_pOpenBtn->GetXPos();
	m_iStartYPos		= m_pOpenBtn->GetYPos();	
	ShowWnd();
}

void FishingBaitListWnd::iwm_show()
{
	SetCheckParentOutSide( true );

	CheckBaitList();

	if( !m_pOpenBtn )
		return;

	// 사이즈 및 결정
	SetSize( GetWidth(), m_iAddHeight + ( (DWORD)m_vBaitList.size() * m_iHeightGap ) );
	SetWndPos( m_iStartXPos, m_iStartYPos - GetHeight() );
}

void FishingBaitListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void FishingBaitListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	m_iOverListIndex  = -2;

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick )
	{
		if( pPreClick != m_pOpenBtn && pPreClick != this )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	ioLSMouse *pMouse = g_App.GetMouse();

	int iListSize = m_vBaitList.size();
	for(int i = 0;i < iListSize;i++)
	{
		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + i * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( GetWidth() - ( m_iListXPos * 2 ) );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			BaitList &kList = m_vBaitList[i];
			m_iOverListIndex  = kList.m_iIndex;
			if( pMouse->IsLBtnUp() )
			{
				SetParentInfo( m_iOverListIndex );
				HideWnd();
			}
			break;
		}
	}
}

void FishingBaitListWnd::SetParentInfo( int iType )
{
	if( !GetParent() )
	{
		LOG.PrintTimeAndLog( 0, "%s Parent == NULL.", __FUNCTION__ );
		return;
	}

	FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*> ( GetParent() );
	if( !pWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s Parent Wnd == NULL.", __FUNCTION__ );
		return;
	}

	pWnd->SetBaitType( iType );
	HideWnd();
}

void FishingBaitListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	bool bDarkRender = false;
	int iListSize = m_vBaitList.size();
	for(int i = 0;i < iListSize;i++)
	{
		const BaitList &rkSelectList = m_vBaitList[i];
		bDarkRender = !bDarkRender;

		if( m_iOverListIndex == rkSelectList.m_iIndex )
		{
			m_pOverLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( i * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		if (rkSelectList.m_iIndex == ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT)
			g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( i * m_iHeightGap ) + 3, FONT_SIZE_12, 96.0f, "%s ∞개", rkSelectList.m_szTitle.c_str() );
		else
			g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( i * m_iHeightGap ) + 3, FONT_SIZE_12, 96.0f, STR(1), rkSelectList.m_szTitle.c_str(), rkSelectList.m_iCount );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////
FishingInventoryWnd::FishingInventoryWnd()
{
	m_pPlayStage = NULL;

	m_iCurFishInvenPage = 0;
	m_iCurFishInvenPageCnt = 1;

	m_iCurFishingGearInvenPage = 0;
	m_iCurFishingGearInvenPageCnt = 1;

	m_iBaitCnt = 0;

	m_dwCurTabID = ID_TAB_FISHING_ROD_BTN;
	m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;

	m_bchangeFishInvenPage = false;

	m_bLockEquipGearItem = false;

	m_dwSetFishingGroundTime = 0;
}

FishingInventoryWnd::~FishingInventoryWnd()
{
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	if( IsShow() )
	{
		int iCurRodNum = g_FishingMgr.GetFishingGearNumByItemCode( g_MyInfo.GetCurFishingRod() );
		int iCurBaitNum = g_FishingMgr.GetFishingGearNumByItemCode( g_MyInfo.GetCurFishingBait() );
		g_GUIMgr.EtcValueSaveInt( "FishingInventoryWnd", "fishing_rod", iCurRodNum );
		g_GUIMgr.EtcValueSaveInt( "FishingInventoryWnd", "fishing_bait", iCurBaitNum );
	}

	g_GUIMgr.GUIPosSaveInt( "FishingInventoryWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "FishingInventoryWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void FishingInventoryWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "FishingInventoryWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "FishingInventoryWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	m_dwCurTabID = ID_TAB_FISHING_ROD_BTN;
	m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;
}

void FishingInventoryWnd::iwm_show()
{
	ChangeTab( ID_TAB_FISHING_ROD_BTN, "" );
	m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;

	CheckPageData( true );
	UpdatePage( true );
	UpdateBait();
	UpdateExp();

	FishingItemInfoWnd* pItem = NULL;
	// 선택 초기화
	for( DWORD dwBtnID = ID_GEAR_ITEM1_BTN; dwBtnID <= ID_GEAR_ITEM6_BTN; dwBtnID++ )
	{
		pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
		if( pItem )
			pItem->OnSelect( false );
	}

	pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( ID_GEAR_ITEM1_BTN ) );
	if( pItem )
	{
		pItem->OnSelect( true );
		m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;
	}

	// 탭or버튼에 따른 하단 텍스트 표시
	UpdateSelecItemInfo();

	g_GUIMgr.HideWnd( SKILL_LIST_WND );
	g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );

#if defined( USE_GA )
	// PLAY_PLAZA_FISHING_START
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Play"
		, "Start"
		, ""
		, 1
		, "%2FPLAY%2FPLAZA%2FFISHING%2FSTART" );
#endif
}

void FishingInventoryWnd::iwm_hide()
{
	if( m_pPlayStage )
	{
		ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
		if( pOwner )
		{
			pOwner->SetFishingEndState();
		}
	}
	int iCurRodNum = g_FishingMgr.GetFishingGearNumByItemCode( g_MyInfo.GetCurFishingRod() );
	int iCurBaitNum = g_FishingMgr.GetFishingGearNumByItemCode( g_MyInfo.GetCurFishingBait() );
	g_GUIMgr.EtcValueSaveInt( "FishingInventoryWnd", "fishing_rod", iCurRodNum );
	g_GUIMgr.EtcValueSaveInt( "FishingInventoryWnd", "fishing_bait", iCurBaitNum );

	g_GUIMgr.ShowWnd( SKILL_LIST_WND );
	g_GUIMgr.ShowWnd( MINI_SOLDIER_SELECT_WND );	
}

void FishingInventoryWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_ITEM1_BTN:
	case ID_ITEM2_BTN:
	case ID_ITEM3_BTN: 
	case ID_ITEM4_BTN:
	case ID_ITEM5_BTN:
	case ID_ITEM6_BTN:
	case ID_ITEM7_BTN:
	case ID_ITEM8_BTN:
	case ID_ITEM9_BTN:
	case ID_ITEM10_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				UpdatePage();
				OnSetIconPush( dwID, false, 0 );
			}
		}
		break;
	case ID_GEAR_ITEM1_BTN:
	case ID_GEAR_ITEM2_BTN:
	case ID_GEAR_ITEM3_BTN: 
	case ID_GEAR_ITEM4_BTN:
	case ID_GEAR_ITEM5_BTN:
	case ID_GEAR_ITEM6_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				OnGearItemBtnUp( pWnd, dwID );
			}
		}
		break;
	case ID_ITEM_SELL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 첫번째 것 판매
			if( g_MyInfo.GetCurFishingItemCnt() > 0 )
			{
				m_iCurFishInvenPage = 0;
				UpdatePage();

				OnSetIconPush( ID_ITEM1_BTN, false, 0 );
			}
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
		}
		break;
	case ID_ITEM_FISHING_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 선택된 낚시대나 미끼가 없을 경우
			if( g_MyInfo.GetCurFishingRod() == -1 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			else if( g_MyInfo.GetCurFishingBait() == -1 )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			else
				CheckFishingBtn();
		}
		break;
	case FISHING_MESSAGE_BOX:
		if( cmd == IOBN_BTNUP )
		{
			WORD wResult = LOWORD( param );
			if( wResult == IDYES )
			{
				SendItemSell( HIWORD( param ) );
			}
			else if( wResult == IDRETRY )
			{
				CheckNextItem( HIWORD( param ) );
			}
			else if( wResult == IDCANCEL )
			{
				HideWnd();
			}
		}
		break;
	case ID_PAGE_UP:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				m_bchangeFishInvenPage = true;
				m_iCurFishInvenPage--;
				UpdatePage();
			}
		}
		break;
	case ID_PAGE_DOWN:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				m_bchangeFishInvenPage = true;
				m_iCurFishInvenPage++;
				UpdatePage();
			}
		}
		break;
	case ID_GEAR_PAGE_UP:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				m_iCurFishingGearInvenPage--;
				UpdatePage();
				UpdateSelecItemInfo();
			}
		}
		break;
	case ID_GEAR_PAGE_DOWN:
		if( cmd == IOBN_BTNUP )
		{
			if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
			{
				m_iCurFishingGearInvenPage++;
				UpdatePage();
				UpdateSelecItemInfo();
			}
		}
		break;
	case ID_BAIT_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			FishingBaitListWnd *pListWnd = dynamic_cast<FishingBaitListWnd*>( FindChildWnd(ID_BAIT_LIST) );
			if( pListWnd )
			{
				if( pListWnd->IsEmptyList() )
				{
					NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
					if( pNewShopWnd )
					{
						pNewShopWnd->ShowWnd();
						pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_FISHING_BAIT, 0 );
					}
				}
				else
				{
					pListWnd->OpenList( pWnd );
				}
			}
		}
		break;
	case ID_TAB_FISHING_ROD_BTN:
	case ID_TAB_FISHING_BAIT_BTN:
	case ID_TAB_FISHING_GROUND_BTN:
		{
			if( cmd == IOBN_BTNDOWN )
			{
				if( dwID != ID_TAB_FISHING_GROUND_BTN )
					SetChildActive( ID_EQUIP_FISHING_GEAR );

				m_iCurFishingGearInvenPage = 0;
				ChangeTab( dwID, "" );
				CheckPageData( false );
				UpdatePage();

				FishingItemInfoWnd* pItem2 = NULL;
				// 선택 초기화
				for( DWORD dwBtnID = ID_GEAR_ITEM1_BTN; dwBtnID <= ID_GEAR_ITEM6_BTN; dwBtnID++ )
				{
					pItem2 = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
					if( pItem2 )
						pItem2->OnSelect( false );
				}

				pItem2 = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( ID_GEAR_ITEM1_BTN ) );
				if( pItem2 )
				{
					pItem2->OnSelect( true );
					m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;
				}

				// 탭or버튼에 따른 하단 텍스트 표시
				UpdateSelecItemInfo();

				// 내 용병이 낚시 상태이면 장착 및 사용 버튼 비활성화
				ChangeButtonStateByFishingStart();

				// 낚시터 설치/해제 딜레이 시간 남았으면 장착 버튼 비활성화
				if( dwID == ID_TAB_FISHING_GROUND_BTN && m_dwSetFishingGroundTime > 0 )
					SetChildInActive( ID_EQUIP_FISHING_GEAR );
			}
		}
		break;
	case ID_EQUIP_FISHING_GEAR:
		{
			if( cmd == IOBN_BTNUP )
			{
				OnEquipFishingGearItem();
			}
		}
		break;
	case ID_BUY_FISHING_GEAR:
		{
			if( cmd == IOBN_BTNUP )
			{
				FishingItemInfoWnd* pSelectedItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( m_dwFocusItemButtonID ) );
				if( pSelectedItem )
				{
					int iArray = pSelectedItem->GetSlotNum() - 1;
					int iItemEtcCode = g_MyInfo.GetSelectFishingGearEtcCode( iArray, (FishingGearType)( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN ) );

					NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
					if( pNewShopWnd )
					{
						pNewShopWnd->ShowWnd();
						pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, iItemEtcCode, 0 );
					}
				}
			}
		}
		break;
	}
}

void FishingInventoryWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( !m_pPlayStage ) return;

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( pOwner )
	{
		FishingScreenBtn *pBtn = dynamic_cast<FishingScreenBtn*> ( FindChildWnd( ID_ITEM_FISHING_BTN ) );
		if( !pBtn ) return;

		if( pOwner->GetState() != CS_FISHING )
			pBtn->SetFishingState( false );
		else
			pBtn->SetFishingState( true );
	}

	DWORD dwCurTime = FRAMEGETTIME();
	// 1.5초 지난 후 장착 버튼 비활성화 해제
	if( m_dwSetFishingGroundTime > 0 && m_dwSetFishingGroundTime + 2000 < dwCurTime )
	{
		m_dwSetFishingGroundTime = 0;
		SetChildActive( ID_EQUIP_FISHING_GEAR );
	}

	UpdateDefaultPos();
	ProcessToolTip();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void FishingInventoryWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void FishingInventoryWnd::InitCheckUsingFishingGround( bool bOnlySelect )
{
	if( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
	{
		// 선택 초기화
		for( DWORD dwAllGearItemBtnID = ID_GEAR_ITEM1_BTN; dwAllGearItemBtnID <= ID_GEAR_ITEM6_BTN; dwAllGearItemBtnID++ )
		{
			if( bOnlySelect )
			{
				if( m_dwFocusItemButtonID != dwAllGearItemBtnID )
					continue;
			}
			FishingItemInfoWnd* pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwAllGearItemBtnID ) );
			if( pItem )
				pItem->SetUsing( false );

			if( m_dwFocusItemButtonID == dwAllGearItemBtnID )
				CheckGearEquipBtnByState( false );
		}
	}
}

void FishingInventoryWnd::InitCheckDieFishingGround( DWORD dwEtcCode )
{
	if( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
	{
		// 선택 초기화
		for( DWORD dwAllGearItemBtnID = ID_GEAR_ITEM1_BTN; dwAllGearItemBtnID <= ID_GEAR_ITEM6_BTN; dwAllGearItemBtnID++ )
		{
			FishingItemInfoWnd* pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwAllGearItemBtnID ) );
			if( pItem && pItem->GetEtcItemCode() == dwEtcCode )
			{
				pItem->SetUsing( false );
				if( m_dwFocusItemButtonID == dwAllGearItemBtnID )
					CheckGearEquipBtnByState( false );
			}
		}
	}
}

void FishingInventoryWnd::CheckPageData( bool bFirst )
{
	int iCurMaxArray = g_MyInfo.GetCurMaxFishingInventory();
	int iCurItemCnt = g_MyInfo.GetCurFishingItemCnt();

	if( iCurMaxArray <= MAX_BTN )
	{
		m_iCurFishInvenPageCnt = 1;
	}
	else
	{
		m_iCurFishInvenPageCnt = iCurMaxArray / MAX_BTN;

		if( iCurMaxArray%MAX_BTN )
			++m_iCurFishInvenPageCnt;
	};

	if( bFirst )
	{
		// 첫 페이지 보여줄 경우
		m_iCurFishInvenPage = 0;
	}
	else
	{
		// 현재 페이지 고수
	}


	iCurItemCnt = g_MyInfo.GetFishingGearCountByTabType( (FishingGearType)( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN ) );

	if( iCurMaxArray <= MAX_GEAR_BTN )
	{
		m_iCurFishingGearInvenPageCnt = 1;
	}
	else
	{
		m_iCurFishingGearInvenPageCnt = iCurItemCnt / MAX_GEAR_BTN;

		if( iCurItemCnt%MAX_GEAR_BTN )
			++m_iCurFishingGearInvenPageCnt;
	};

	if( bFirst )
	{
		// 첫 페이지 보여줄 경우
		m_iCurFishingGearInvenPage = 0;
	}
	else
	{
		// 현재 페이지 고수
	}
	/*
	// 마지막 아이템 있는 페이지 보여줄 경우
	if( iCurItemCnt <= MAX_BTN )
	{
		m_iCurPage = 0;
	}
	else
	{
		m_iCurPage = iCurItemCnt / MAX_BTN;

		if( iCurItemCnt%MAX_BTN == 0 )
			--m_iCurPage;
	}

	if( m_iCurPage > 0 && m_iCurPage >= m_iCurPageCnt )
		m_iCurPage = m_iCurPageCnt - 1;
	*/
}

void FishingInventoryWnd::UpdatePage( bool bFirst, bool bBuy )
{
	int iCurMaxArray = g_MyInfo.GetCurMaxFishingInventory();
	int iStartArray = m_iCurFishInvenPage * MAX_BTN;
	if( !m_bchangeFishInvenPage ) 
		iStartArray = m_iCurFishingGearInvenPage * MAX_GEAR_BTN;

	if( bBuy )
	{
		FishingItemInfoWnd* pItem = NULL;
		// 선택 초기화
		for( DWORD dwBtnID = ID_GEAR_ITEM1_BTN; dwBtnID <= ID_GEAR_ITEM6_BTN; dwBtnID++ )
		{
			pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
			if( pItem )
				pItem->OnSelect( false );
		}

		pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( ID_GEAR_ITEM1_BTN ) );
		if( pItem )
		{
			pItem->OnSelect( true );
			m_dwFocusItemButtonID = ID_GEAR_ITEM1_BTN;
		}
	}

	g_MyInfo.AlignFishingGearByOwn();

	ioBaseChar* pOwner = m_pPlayStage->GetOwnerChar();

	if( bFirst && pOwner && pOwner->GetState() != CS_FISHING )
	{
		iStartArray = 0;
		int iCurFishingRod = g_GUIMgr.EtcValueLoadInt( "FishingInventoryWnd", "fishing_rod" );
		int iCurFishingBait = g_GUIMgr.EtcValueLoadInt( "FishingInventoryWnd", "fishing_bait" );

		if( iCurFishingRod != 0 )
		{
			iCurFishingRod = g_FishingMgr.GetFishingGearCodeByTypeNum( FGT_ROD, iCurFishingRod );
			if( !g_MyInfo.GetFishingGearOwnByEtcCode( iCurFishingRod, FGT_ROD ) )
				iCurFishingRod = g_MyInfo.GetFishingRodByFirstOrder();
		}
		if( iCurFishingBait != 0 )
		{
			iCurFishingBait = g_FishingMgr.GetFishingGearCodeByTypeNum( FGT_BAIT, iCurFishingBait );
			if( !g_MyInfo.GetFishingGearOwnByEtcCode( iCurFishingBait, FGT_BAIT ) )
				iCurFishingBait = g_MyInfo.GetFishingBaitByFirstOrder();
		}

		g_MyInfo.SetFirstFishingRod_Bait( iCurFishingRod, iCurFishingBait );
	}

	for( int i = 0 ; i < ( MAX_BTN + MAX_GEAR_BTN ); i++ )
	{
		int iCurArray = iStartArray + i;

		UpdateBtns( iCurArray, ID_ITEM1_BTN + i, bFirst );
	}

	if( m_bchangeFishInvenPage )
		m_bchangeFishInvenPage = false;

	// Page UP, DOWN 버튼
	if( m_iCurFishInvenPage == 0 )
		SetChildInActive( ID_PAGE_UP );
	else
		SetChildActive( ID_PAGE_UP );

	if( m_iCurFishInvenPage == (m_iCurFishInvenPageCnt-1) )
		SetChildInActive( ID_PAGE_DOWN );
	else
		SetChildActive( ID_PAGE_DOWN );

	if( m_iCurFishingGearInvenPage == 0 )
		SetChildInActive( ID_GEAR_PAGE_UP );
	else
		SetChildActive( ID_GEAR_PAGE_UP );

	if( m_iCurFishingGearInvenPage == (m_iCurFishingGearInvenPageCnt-1) )
		SetChildInActive( ID_GEAR_PAGE_DOWN );
	else
		SetChildActive( ID_GEAR_PAGE_DOWN );
}

void FishingInventoryWnd::UpdateBtns( int iArray , DWORD dwBtnID, bool bFirst )
{
	FishingItemInfoWnd* pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
	if( !pItem )
		return;

	if( m_bchangeFishInvenPage && dwBtnID >= (ID_ITEM1_BTN+MAX_BTN) && !bFirst )
		return;

	int iItemType = -1;
	DWORD dwGearType = ( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN );
	if( dwBtnID >= ID_ITEM1_BTN && dwBtnID < (ID_ITEM1_BTN+MAX_BTN) )
	{
		if( !m_bchangeFishInvenPage && !bFirst )
			return;
		iItemType = g_MyInfo.GetFishingItemType( iArray );
		dwGearType = (DWORD)FGT_NONE;
	}
	else if( m_dwCurTabID == ID_TAB_FISHING_ROD_BTN )
	{
		iItemType = g_MyInfo.GetFishingRodItemType( iArray - MAX_BTN );
		iArray -= MAX_BTN;
	}
	else if( m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
	{
		iItemType = g_MyInfo.GetFishingBaitItemType( iArray - MAX_BTN );
		iArray -= MAX_BTN;
	}
	else if( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
	{
		iItemType = g_MyInfo.GetFishingGroundItemType( iArray - MAX_BTN );
		iArray -= MAX_BTN;
	}

	DWORD dwEtcCode = g_MyInfo.GetSelectFishingGearEtcCode( iArray, (FishingGearType)dwGearType );

	pItem->SetInfo( iItemType, iArray+1, dwGearType, dwEtcCode );
}

void FishingInventoryWnd::UpdateExp()
{
	FishingExpWnd* pExp = dynamic_cast<FishingExpWnd*> ( FindChildWnd( ID_FISHING_EXP_WND ) );
	if( !pExp ) return;

	pExp->SetInfo( g_MyInfo.GetFishingLevel(), g_MyInfo.GetFishingExpert() );
}

void FishingInventoryWnd::UpdateSelecItemInfo()
{
	FishingExpWnd* pExp = dynamic_cast<FishingExpWnd*> ( FindChildWnd( ID_FISHING_EXP_WND ) );
	if( !pExp ) return;

	char szText[MAX_PATH] = "";
	m_SelectGearInfo.Clear();

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	FishingItemInfoWnd* pSelectedItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( m_dwFocusItemButtonID ) );
	if( pUserEtcItem && pSelectedItem )
	{
		int iArray = pSelectedItem->GetSlotNum() - 1;
		int iItemEtcCode = g_MyInfo.GetSelectFishingGearEtcCode( iArray, (FishingGearType)( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN ) );
		if( iItemEtcCode != -1 )
		{
			DWORD sInfo1Color;
			DWORD sInfo2Color;
			ioHashString sInfo1;
			ioHashString sInfo2;
			pUserEtcItem->GetUseInfo( iItemEtcCode, sInfo1, sInfo2, sInfo1Color, sInfo2Color, true );
			ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( iItemEtcCode );

			if( !pEtcItem )
			{
				m_SelectGearInfo = STR(2);
				SetChildInActive( ID_EQUIP_FISHING_GEAR );
				return;
			}
				
			ioHashString sEtcItemName = pEtcItem->GetName();

			FishingGearEquipBtn* pFishingGearEquipBtn = dynamic_cast<FishingGearEquipBtn*> ( FindChildWnd( ID_EQUIP_FISHING_GEAR ) );
			FishingGearEquipBtn* pFishingGearBuyBtn = dynamic_cast<FishingGearEquipBtn*> ( FindChildWnd( ID_BUY_FISHING_GEAR ) );
			if( !sInfo1.IsEmpty() )
			{
				m_SelectGearInfo = sEtcItemName + "(" + sInfo1 + ")";

				if( pUserEtcItem->IsCanUse( iItemEtcCode ) )
				{
					if( pFishingGearEquipBtn )
						pFishingGearEquipBtn->ShowWnd();
					if( pFishingGearBuyBtn )
						pFishingGearBuyBtn->HideWnd();

					if( !m_bLockEquipGearItem && m_dwSetFishingGroundTime == 0 )
						SetChildActive( ID_EQUIP_FISHING_GEAR );
				}
				else
				{
					if( pFishingGearEquipBtn )
						pFishingGearEquipBtn->HideWnd();
					if( pFishingGearBuyBtn )
						pFishingGearBuyBtn->ShowWnd();
				}
			}
			else
			{
				m_SelectGearInfo = sEtcItemName + "(" + STR(1) + ")";
				if( pFishingGearEquipBtn )
					pFishingGearEquipBtn->HideWnd();
				if( pFishingGearBuyBtn )
					pFishingGearBuyBtn->ShowWnd();
			}

			CheckGearEquipBtnByState( pSelectedItem->CheckUsing() );
		}
	}
	else
	{
		m_SelectGearInfo = STR(1);
		SetChildInActive( ID_EQUIP_FISHING_GEAR );
	}
}

void FishingInventoryWnd::OnSetIconPush( DWORD dwID, bool bNextSel, int iStartArray )
{
	if( bNextSel )
	{
		if( g_MyInfo.GetFishingItemType( iStartArray ) == -1 )
		{
			iStartArray = 0;
		}
		// 판매 창으로 전달
		CheckMessageBox( iStartArray );
	}
	else
	{
		int iCurArray = dwID - ID_ITEM1_BTN;
		iStartArray = ( m_iCurFishInvenPage * MAX_BTN ) + iCurArray;
		if( g_MyInfo.GetFishingItemType( iStartArray ) != -1 )
		{
			ioFishingMessageBox *pFishMessageBox = dynamic_cast<ioFishingMessageBox*>(g_GUIMgr.FindWnd(FISHING_MESSAGE_BOX));
			if( pFishMessageBox && pFishMessageBox->IsShow() )
			{
				pFishMessageBox->StopAutoSell();
			}
			// 판매 창으로 전달
			CheckMessageBox( iStartArray );
		}
	}
}

void FishingInventoryWnd::SendItemSell( int iSellItemArray )
{
	int iItemType = g_MyInfo.GetFishingItemType( iSellItemArray );
	int iIndexArray = g_MyInfo.GetFishingItemArray( iSellItemArray );

	if( iItemType != -1 )
	{
		SP2Packet kPacket( CTPK_FISHING );
		kPacket << FISHING_SELL;
		kPacket << iIndexArray;
		TCPNetwork::SendToServer( kPacket );

		g_App.SetMouseBusy( true );
	}
}

void FishingInventoryWnd::CheckNextItem( int iSellItemArray )
{
	int iCurTotalItem = g_MyInfo.GetCurFishingItemCnt();
	if( iCurTotalItem > 0 )
	{
		OnSetIconPush( ID_ITEM1_BTN + (iSellItemArray % MAX_BTN), true, iSellItemArray );
	}
	else
	{
		g_GUIMgr.HideWnd( FISHING_MESSAGE_BOX );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void FishingInventoryWnd::UpdateItemData( int iType, ioHashString szIcon, ioHashString szGrade )
{
	m_bchangeFishInvenPage = true;

	// 갱신된 리스트로 페이지 갱신
	CheckPageData( false );
	UpdatePage();
	UpdateExp();
	UpdateBait();

	switch( iType )
	{
	case SELL_TYPE:
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( !pOwner ) return;

			pOwner->SetFishingGradeEmoticon( szIcon, szGrade );
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EMOTICON_SYNC );
				kPacket << pOwner->GetCharName();
				kPacket << EMOTICON_FISHING_GRADE;
				kPacket << szIcon;
				kPacket << szGrade;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		break;
	}
}

void FishingInventoryWnd::CheckMessageBox( int iStartArray )
{
	g_GUIMgr.SetFishingMsgListBox( this, iStartArray );
}

void FishingInventoryWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ONE_NUM_X	= 329,
		ONE_NUM_Y	= 63,

		TWO_NUM_X	= 325,
		TWO_NUM_Y	= 63,
	};

	enum { NAME_X_OFFSET = 20, NAME_Y_OFFSET = 142, CNT_X_OFFSET = 104, CNT_Y_OFFSET = 142, };

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintTextWidthCut( iXPos + NAME_X_OFFSET, iYPos + NAME_Y_OFFSET, FONT_SIZE_13, 48.0f, m_BaitCntText.c_str() );

	// 낚시 결과물 슬롯 페이지
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( 42, 60, 77 );
	g_FontMgr.PrintText( iXPos + 498, iYPos + 70, FONT_SIZE_14, STR(2), m_iCurFishInvenPage + 1 );

	// 낚시 장비 슬롯 페이지
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( 42, 60, 77 );
	g_FontMgr.PrintText( iXPos + 14, iYPos + 70, FONT_SIZE_14, STR(2), m_iCurFishingGearInvenPage + 1 );

	// 선택한 낚시 장비 설명
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( 42, 60, 77 );
	g_FontMgr.PrintText( iXPos + 104, iYPos + 137, FONT_SIZE_12, m_SelectGearInfo.c_str() );
}

void FishingInventoryWnd::ProcessToolTip()
{
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];

	DWORD dwCurOverID = 0;
	if( g_GUIMgr.GetPreOverWnd() )
	{
		if( g_GUIMgr.GetPreOverWnd()->GetParent() == this )
			dwCurOverID = g_GUIMgr.GetPreOverWnd()->GetID();
	}
	
	if( COMPARE( dwCurOverID, ID_ITEM1_BTN, ID_ITEM1_BTN+MAX_BTN+MAX_GEAR_BTN ) )
	{
		int iStartArray = m_iCurFishInvenPage * MAX_BTN;
		int iCurArray = dwCurOverID - ID_ITEM1_BTN;

		int iItemType = -1;
		DWORD dwGearType = ( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN );
		if( dwCurOverID >= ID_ITEM1_BTN && dwCurOverID < (ID_ITEM1_BTN+MAX_BTN) )
		{
			iItemType = g_MyInfo.GetFishingItemType( iStartArray + iCurArray );
			dwGearType = (DWORD)FGT_NONE;
		}
		else if( m_dwCurTabID == ID_TAB_FISHING_ROD_BTN )
		{
			iStartArray = m_iCurFishingGearInvenPage * MAX_GEAR_BTN;
			iItemType = g_MyInfo.GetFishingRodItemType( ( iStartArray + iCurArray ) - MAX_BTN );
		}
		else if( m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
		{
			iStartArray = m_iCurFishingGearInvenPage * MAX_GEAR_BTN;
			iItemType = g_MyInfo.GetFishingBaitItemType( ( iStartArray + iCurArray ) - MAX_BTN );
		}
		else if( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
		{
			iStartArray = m_iCurFishingGearInvenPage * MAX_GEAR_BTN;
			iItemType = g_MyInfo.GetFishingGroundItemType( ( iStartArray + iCurArray ) - MAX_BTN );
		}

		if( iItemType == -1 )
			return;

		FishingItemInfoWnd *pInfoWnd = dynamic_cast<FishingItemInfoWnd*>( FindChildWnd( dwCurOverID ) );
		if( !pInfoWnd )
			return;

		ioHashString szName = g_FishingMgr.GetFishingItemName( iItemType, (FishingGearType)dwGearType );
		if( szName.IsEmpty() )
			return;

		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, szName.c_str() );
	}
	else if( ID_FISHING_EXP_WND == dwCurOverID )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1), g_MyInfo.GetFishingExpert(), g_LevelMgr.GetNextFishingLevelUpExp( g_MyInfo.GetFishingLevel() ) );
	}

	GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter );
	}
}

void FishingInventoryWnd::SetUpFishingGround( ioBaseChar* pChar, DWORD dwItemType )
{
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwItemType );
	if( !pEtcItem )
		return;

	ioEtcItemBlock *pEtcBlock = dynamic_cast<ioEtcItemBlock *>(pEtcItem);
	if( !pEtcBlock ) return;

	DWORD dwItemCode = pEtcBlock->GetType();
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem ) return;

	// 권한 아이템 보유여부 체크
	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	//if( P2PNetwork::IsNetworkPlaying() && !pUserEtcItem->GetEtcItem( dwItemCode, kItemSlot ) )
	//{
	//	char szMsg[MAX_PATH*2]=""; 
	//	SafeSprintf( szMsg, sizeof( szMsg ), STR(1), pEtcItem->GetName().c_str() );
	//	g_ChatMgr.SetSystemMsg( szMsg );
	//	return;
	//}

	pChar->CreateFishingGround();

	if( m_pPlayStage->GetModeType() == MT_TRAINING && m_pPlayStage->GetBlockWorld() != NULL )
	{
		g_ChatMgr.SetSystemMsg( "길드 본부 에서는 사용 할 수 없습니다." );
		return;
	}

	// 광장/길드본부/훈련소/개인본부 체크
	bool bTraining = false;
	bool bMyRoom = false;
	if( m_pPlayStage->GetModeType() == MT_TRAINING )
		bTraining = true;

	if( m_pPlayStage->GetModeType() == MT_MYROOM && ioMyRoomMode::GetMyRoomType() == MRT_PRACTICE )
		bMyRoom = true;
	else if( m_pPlayStage->GetModeType() == MT_HEADQUARTERS )
		bMyRoom = true;
	else if( m_pPlayStage->GetModeType() == MT_HOUSE )
		bMyRoom = true;

	if( P2PNetwork::IsNetworkPlaying() && !bTraining && !bMyRoom )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(2) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return;
	}

	// 갯수제한 체크
	if( bTraining && !g_PushStructListMgr.CheckMaxStructCntForRoom() )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(3) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return;
	}
	else if( bMyRoom && !g_PushStructListMgr.CheckMaxStructCntForMyRoom() )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(4) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return;
	}

	if( !g_PushStructListMgr.CheckMaxStructCntForEtcCode( pChar, dwItemCode ) )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(5), pEtcItem->GetName().c_str(),
			g_PushStructListMgr.GetBlockEtcMaxCnt() );
		g_ChatMgr.SetSystemMsg( szMsg );
		return;
	}

	// 기간 체크
	if( kItemSlot.m_iValue1 != 0 && kItemSlot.m_iValue2 != 0 )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kItemSlot.GetYear(), kItemSlot.GetMonth(), kItemSlot.GetDay(), kItemSlot.GetHour(), kItemSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec < 3 )
		{
			char szMsg[MAX_PATH*2]=""; 
			SafeSprintf( szMsg, sizeof( szMsg ), STR(6), pEtcItem->GetName().c_str() );
			g_ChatMgr.SetSystemMsg( szMsg );
			return;
		}
	}

	ioObjectItem *pItem = pChar->GetObject();
	// 캐릭터 상태체크
	if( (pChar->GetState() != CS_DELAY && pChar->GetState() != CS_SPECIAL_MOTION) ||
		pChar->IsCatchMode() || pChar->IsPrisonerMode() || pChar->IsObejctEquipState() ||
		pChar->HasBuff( BT_CHANGE_MESH ) || pChar->HasBuff( BT_CHANGE_SKIN_COLOR ) ||
		pChar->CheckPoweredObjectItem( pItem ) )
	{
		char szMsg[MAX_PATH*2]=""; 
		SafeSprintf( szMsg, sizeof( szMsg ), STR(7) );
		g_ChatMgr.SetSystemMsg( szMsg );
		return;
	}

	pChar->SetCreateStructState( pEtcBlock, false );
}

void FishingInventoryWnd::OnGearItemBtnUp( ioWnd* pWnd, DWORD dwBtnID )
{
	FishingItemInfoWnd* pItem = NULL;
	
	// 선택 초기화
	for( DWORD dwAllGearItemBtnID = ID_GEAR_ITEM1_BTN; dwAllGearItemBtnID <= ID_GEAR_ITEM6_BTN; dwAllGearItemBtnID++ )
	{
		pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwAllGearItemBtnID ) );
		if( pItem )
			pItem->OnSelect( false );
	}

	pItem = dynamic_cast<FishingItemInfoWnd*> ( pWnd );
	if( pItem )
	{
		pItem->OnSelect( true );
		m_dwFocusItemButtonID = dwBtnID;

		// 탭or버튼에 따른 하단 텍스트 표시
		UpdateSelecItemInfo();
	}
}

void FishingInventoryWnd::OnEquipFishingGearItem()
{
	FishingItemInfoWnd* pItem3 = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( m_dwFocusItemButtonID ) );
	ioBaseChar* pMyChar = m_pPlayStage->GetOwnerChar();
	int iArray = pItem3->GetSlotNum() - 1;
	int iItemType = g_MyInfo.GetSelectFishingGearEtcCode( iArray, (FishingGearType)( m_dwCurTabID - ID_TAB_FISHING_ROD_BTN ) );

	if( pItem3 && pItem3->CheckOwnItem() && !pItem3->CheckUsing() && pMyChar && pMyChar->GetState() != CS_FISHING )
	{
		// 낚시터 설치하려고 할때 CS_DELAY 상태가 아니면 설치되지 않게 하자
		if ( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
		{
			if ( pMyChar->GetState() != CS_DELAY )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
			
			if( !m_pPlayStage || ( m_pPlayStage->GetPlazaModeType() != PT_BATTLE && m_pPlayStage->GetPlazaModeType() != PT_COMMUNITY ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}
		}
		// 사용 표시 초기화
		if ( m_dwCurTabID != ID_TAB_FISHING_GROUND_BTN )
		{
			for( DWORD dwBtnID = ID_GEAR_ITEM1_BTN; dwBtnID <= ID_GEAR_ITEM6_BTN; dwBtnID++ )
			{
				FishingItemInfoWnd* pItem4 = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
				if( pItem4 )
					pItem4->SetUsing( false );
			}
		}

		// 사용 표시
		pItem3->SetUsing( true );

		if( m_dwCurTabID == ID_TAB_FISHING_ROD_BTN )
			g_MyInfo.SetCurFishingRod( iItemType );
		else if( m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
			g_MyInfo.SetCurFishingBait( iItemType );
		else if ( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
			SetUpFishingGround( pMyChar, (DWORD)iItemType );
	}
	else if( pItem3 && pItem3->CheckOwnItem() && pItem3->CheckUsing() && pMyChar && pMyChar->GetState() != CS_FISHING )
	{
		pItem3->SetUsing( false );

		if( m_dwCurTabID == ID_TAB_FISHING_ROD_BTN )
			g_MyInfo.SetCurFishingRod( -1 );
		else if( m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
			g_MyInfo.SetCurFishingBait( -1 );
		else if ( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
			g_PushStructListMgr.DeleteSelectFishingGround( pMyChar, (DWORD)iItemType );
	}
	else if ( pMyChar && pMyChar->GetState() == CS_FISHING )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );

	UpdateSelecItemInfo();

	if ( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
	{
		m_dwSetFishingGroundTime = FRAMEGETTIME();
		SetChildInActive( ID_EQUIP_FISHING_GEAR );
	}
}

bool FishingInventoryWnd::IsMaxSlot()
{
	return g_MyInfo.IsFishingItemMax();
}

void FishingInventoryWnd::SetBaitType( int iType )
{
	if( !m_pPlayStage ) return;

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner ) return;

	if( g_MyInfo.SetCurFishingBait( iType ) )
	{
		if( pOwner->GetState() == CS_FISHING )
			pOwner->SetFishingEndState();

		UpdateBait();
	}
}

void FishingInventoryWnd::CheckFishingBtn()
{
	if( !m_pPlayStage ) return;
	
	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner ) return;

	if( pOwner->GetState() == CS_FISHING )
	{
		pOwner->SetFishingEndState();
	}
	else
	{
		if( pOwner->EnableFishing( true ) )
		{
			pOwner->SetFishingState( true );
		}
	}
}

void FishingInventoryWnd::UpdateBait()
{
	ioWnd *pBaitWnd = FindChildWnd(ID_BAIT_SELECT_BTN);
	if( !pBaitWnd )
		return;

	int iBaitType = g_MyInfo.GetCurFishingBait();
	int iBaitCnt = g_MyInfo.GetCurFishingBaitCnt();

	char szTitle[MAX_PATH] = "";
	if( iBaitType == 0 )
	{
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1) );
	}
	else
	{
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( iBaitType );
		if( pItem )
		{
			if (iBaitType == ioEtcItem::EIT_ETC_PCROOM_FISHING_BAIT)
				SafeSprintf( szTitle, sizeof( szTitle ), "%s ∞개", pItem->GetExtraName().c_str() );
			else if( !pItem->GetExtraName().IsEmpty() )
				SafeSprintf( szTitle, sizeof( szTitle ), STR(2), pItem->GetExtraName().c_str(), iBaitCnt );
			else
				SafeSprintf( szTitle, sizeof( szTitle ), STR(3), pItem->GetName().c_str(), iBaitCnt );
		}
		else
		{
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4) );
		}
	}

	ioUITitle *pTitle = pBaitWnd->GetTitle();
	if( pTitle )
	{
		pTitle->SetWidthCutSize( 96.0f );
		pTitle->SetText( szTitle, "" );
	}
}

void FishingInventoryWnd::UpdateDefaultPos()
{
	if(  g_GUIMgr.IsDragMove( m_dwID )  || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			if( COMPARE( GetYPos(), 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = GetYPos();
		}
	}
}

void FishingInventoryWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	int iTempDefYPos = m_iDefYPos;
	m_iDefYPos -= ( Setting::Height() - 600 ) * 0.22f;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
	}
}

void FishingInventoryWnd::ChangeTab( DWORD dwID, const char *szFind  )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_FISHING_ROD_BTN, ID_TAB_FISHING_GROUND_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_TAB_FISHING_ROD_BTN; i < ID_TAB_FISHING_GROUND_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}
}

void FishingInventoryWnd::ChangeSelectedUsingState()
{
	FishingItemInfoWnd* pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( m_dwFocusItemButtonID ) );
	if( pItem )
	{
		bool bCheckUsing = pItem->CheckUsing();
		if( bCheckUsing )
			pItem->SetUsing( false );
		else if( !bCheckUsing )
			pItem->SetUsing( true );

		CheckGearEquipBtnByState( pItem->CheckUsing() );
	}
}

void FishingInventoryWnd::CheckGearEquipBtnByState( bool bCheckUsing )
{
	FishingGearEquipBtn* pFishingGearEquipBtn = dynamic_cast<FishingGearEquipBtn*> ( FindChildWnd( ID_EQUIP_FISHING_GEAR ) );
	if( pFishingGearEquipBtn )
	{
		if( m_dwCurTabID == ID_TAB_FISHING_ROD_BTN || m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
		{
			if( !bCheckUsing )
				pFishingGearEquipBtn->SetTitleText( STR(1) );
			else
				pFishingGearEquipBtn->SetTitleText( STR(2) );
		}
		else if ( m_dwCurTabID == ID_TAB_FISHING_GROUND_BTN )
		{
			if( !bCheckUsing )
				pFishingGearEquipBtn->SetTitleText( STR(3) );
			else
				pFishingGearEquipBtn->SetTitleText( STR(4) );
		}
	}
}

void FishingInventoryWnd::ChangeUsingFishingBait()
{
	UpdatePage();
	if ( m_dwCurTabID == ID_TAB_FISHING_BAIT_BTN )
	{
		for( DWORD dwBtnID = ID_GEAR_ITEM1_BTN; dwBtnID <= ID_GEAR_ITEM6_BTN; dwBtnID++ )
		{
			FishingItemInfoWnd* pItem = dynamic_cast<FishingItemInfoWnd*> ( FindChildWnd( dwBtnID ) );
			if( pItem )
			{
				if( pItem->GetEtcItemCode() == g_MyInfo.GetCurFishingBait() )
				{
					pItem->SetUsing( true );
					pItem->OnSelect( true );
					m_dwFocusItemButtonID = dwBtnID;
				}
				else
				{
					if( pItem->CheckUsing() )
						pItem->SetOwnItem( false );
					pItem->SetUsing( false );
					pItem->OnSelect( false );
				}
			}
		}
	}
	UpdateSelecItemInfo();
}

void FishingInventoryWnd::ChangeButtonStateByFishingStart()
{
	ioBaseChar* pMyChar = m_pPlayStage->GetOwnerChar();
	if( !pMyChar )
		return;
	
	FishingGearEquipBtn* pFishingGearEquipBtn = dynamic_cast<FishingGearEquipBtn*> ( FindChildWnd( ID_EQUIP_FISHING_GEAR ) );
	if( pFishingGearEquipBtn )
	{
		if( pMyChar->GetState() == CS_FISHING )
		{
			pFishingGearEquipBtn->SetInActive();
			m_bLockEquipGearItem = true;
		}
		else if( pMyChar->GetState() != CS_FISHING )
		{
			pFishingGearEquipBtn->SetActive();
			m_bLockEquipGearItem = false;
		}
	}
}