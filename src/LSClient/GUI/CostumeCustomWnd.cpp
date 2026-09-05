
#include "stdafx.h"

#include "CostumeCustomWnd.h"

#include "ItemCustomWnd.h"
#include "ioUI3DRender.h"
#include "FileListWnd.h"
#include "ChangeTradeStateWnd.h"

#include "../ioDecorationPrice.h"
#include <shlobj.h>

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 아이템 정보)
CostumeCustomInfoWnd::CostumeCustomInfoWnd()
{
	m_ItemInfo.Init();
	m_nCostumeIndex = 0;

	m_pEmptyImg = NULL;
}

CostumeCustomInfoWnd::~CostumeCustomInfoWnd()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void CostumeCustomInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_MALE1_BTN:
	case ID_MALE2_BTN:
	case ID_FEMALE_BTN:
	case ID_CUSTOM_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void CostumeCustomInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pEmptyImg );
		m_pEmptyImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void CostumeCustomInfoWnd::SetBlank()
{
	m_ItemInfo.Init();
	m_nCostumeIndex = 0;

	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_MALE1_BTN );
	HideChildWnd( ID_MALE2_BTN );
	HideChildWnd( ID_FEMALE_BTN );

	ClearDesc();
}

void CostumeCustomInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool CostumeCustomInfoWnd::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_INFO_BTN ) )
		return true;
	return false;
}

bool CostumeCustomInfoWnd::IsIconOver()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_ICON_BTN ) )
		return true;
	return false;
}

void CostumeCustomInfoWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{	
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos );

		// 기타 이미지 추가.
		if( m_pEmptyImg )
		{
			m_pEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
		}
	}
	else
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void CostumeCustomInfoWnd::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET           = 52,
		Y_OFFSET           = 105,
		Y_LINE_GAP         = 16,
		Y_OFFSET_SECOND    = -8,
	};

	iXPos += X_OFFSET;
	iYPos += Y_OFFSET;

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "없음" );
		m_szDescArray[0].PrintFullTextWidthCut( iXPos, iYPos, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		int iDescCnt = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			iDescCnt++;
		}

		if( iDescCnt == 3 )
			iYPos += Y_OFFSET_SECOND;

		int iCnt  = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + ( Y_LINE_GAP * iCnt ), TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
			iCnt++;
		}
	}
}

void CostumeCustomInfoWnd::SetInfo( InventoryItemInfo &rkInfo )
{
	SetBlank();

	ioHashString szName, szIcon;

	SetActive();

	HideChildWnd( ID_CUSTOM_INFO_BTN );
	m_ItemInfo = rkInfo;
	if( !SetCostumeInfo() )
	{
		SetBlank();
		return;
	}
}

InventoryItemInfo& CostumeCustomInfoWnd::GetInfo()
{
	return m_ItemInfo;
}

int CostumeCustomInfoWnd::GetMarkType()
{
	ItemCustomIconBtn *pBtn = dynamic_cast< ItemCustomIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn ) 
		return TRADE_MARK_TYPE_ENABLE;

	return pBtn->GetMarkType();
}

bool CostumeCustomInfoWnd::SetCostumeInfo()
{
	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
		return false;

	if( !pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	ItemCustomIconBtn *pBtn = dynamic_cast< ItemCustomIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetEquip( kSlot.m_bEquip );
		pBtn->SetCostume( true );
	}

	m_nCostumeIndex = kSlot.m_nIndex;

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
		ShowChildWnd( ID_CUSTOM_INFO_BTN );
	else
		HideChildWnd( ID_CUSTOM_INFO_BTN );

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", kSlot.m_Name.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "영구사용" );
	
	// Limit
	CostumeInfo sCostumeInfo;
	g_CostumeInfoMgr.GetCostumeInfo( kSlot.m_nCostumeCode, sCostumeInfo );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( sCostumeInfo.m_nEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s 전용", g_MyInfo.GetClassName( sCostumeInfo.m_nEnableClass ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "제한없음" );

	// 클래스 제한이 있는 아이템은 해당 클래스에 여캐가 있어야지만 여성 가능
	if( sCostumeInfo.m_nEnableClass > 0 )
	{
		if( g_MyInfo.IsSlotItem( (sCostumeInfo.m_nEnableClass * 100000) + UID_KINDRED, RDT_HUMAN_WOMAN ) )
		{
			ShowChildWnd( ID_MALE2_BTN );
			ShowChildWnd( ID_FEMALE_BTN );
		}
		else
			ShowChildWnd( ID_MALE1_BTN );
	}
	else
	{
		ShowChildWnd( ID_MALE2_BTN );
		ShowChildWnd( ID_FEMALE_BTN );
	}

	return true;
}

void CostumeCustomInfoWnd::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool CostumeCustomInfoWnd::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 장비 선택 창)
CostumeCustomSelectWnd::CostumeCustomSelectWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

CostumeCustomSelectWnd::~CostumeCustomSelectWnd()
{
	m_vInventoryItemInfoList.clear();
}

void CostumeCustomSelectWnd::iwm_show()
{
	g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
	g_GUIMgr.HideWnd( ITEM_CUSTOM_SELECT_WND );

	ChangeTab( ID_TAB_1_BTN );

	HideChildWnd( ID_TAB_2_BTN );
}

void CostumeCustomSelectWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
}

void CostumeCustomSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void CostumeCustomSelectWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioMovingWnd::iwm_wheel( zDelta );
	}
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void CostumeCustomSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
	case ID_TAB_4_BTN:
	case ID_TAB_5_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeTab( dwID );
		}
		break;
	case ID_ITEM_INFO_WND_1_BTN:
	case ID_ITEM_INFO_WND_2_BTN:
	case ID_ITEM_INFO_WND_3_BTN:
	case ID_ITEM_INFO_WND_4_BTN:
	case ID_ITEM_INFO_WND_5_BTN:
	case ID_ITEM_INFO_WND_6_BTN:
	case ID_ITEM_INFO_WND_7_BTN:
	case ID_ITEM_INFO_WND_8_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( param == CostumeCustomInfoWnd::ID_CUSTOM_INFO_BTN )
				OnCostumeCustomInfoPush( dwID );
			else
			{
				CostumeCustomInfoWnd *pItem = dynamic_cast<CostumeCustomInfoWnd*>(FindChildWnd(dwID));
				if( pItem )
				{
					CostumeCustomMakeWnd *pMakeWnd = dynamic_cast<CostumeCustomMakeWnd*>(g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_WND ));
					if( pMakeWnd )
					{
						if( param == CostumeCustomInfoWnd::ID_MALE1_BTN || param == CostumeCustomInfoWnd::ID_MALE2_BTN )
							pMakeWnd->SetCostumeInfo( pItem->GetInfo(), true );
						else if( param == CostumeCustomInfoWnd::ID_FEMALE_BTN )
							pMakeWnd->SetCostumeInfo( pItem->GetInfo(), false );	
					}				
				}
			}
		}
		break;
	}
}

void CostumeCustomSelectWnd::OnCostumeCustomInfoPush( DWORD dwID )
{
	CostumeCustomInfoWnd *pItem = dynamic_cast<CostumeCustomInfoWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMagicCode = pItem->GetMagicCode();
				
		ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
		if( !pCostume ) 
			return;

		CostumeSlot kSlot;
		if( !pCostume->GetCostumeSlot( iMagicCode, kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "존재하지 않는 장비입니다." );
			return;
		}

		if( kSlot.m_bEquip )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "착용 중인 코스튬은 스킨을 할 수 없습니다." );
			return;
		}
		
		CostumeEquipInfo sCostumeEquipInfo;
		sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;
		sCostumeEquipInfo.m_Costume_Male_Custom = kSlot.m_dwMaleCustom;
		sCostumeEquipInfo.m_Costume_Female_Custom = kSlot.m_dwFemaleCustom;

		CostumeCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast< CostumeCustomGlobalDetailViewWnd * >( g_GUIMgr.FindWnd( COSTUME_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
		if( pDetailViewWnd )
			pDetailViewWnd->CreateCharacter( sCostumeEquipInfo );		
	}
}

void CostumeCustomSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		CostumeCustomInfoWnd *pItem = dynamic_cast<CostumeCustomInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			InventoryItemInfo kInfo;
			pItem->SetInfo( kInfo );
			continue;
		}

		pItem->SetInfo( m_vInventoryItemInfoList[iCurArray] );
	}
}

void CostumeCustomSelectWnd::ChangeTab( DWORD dwID )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
	m_dwCurTabID = dwID;

	SetCostumeInfoVec( m_dwCurTabID );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void CostumeCustomSelectWnd::SetCostumeInfoVec( DWORD dwTabID )
{
	m_vInventoryItemInfoList.clear();

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;

	int nSize = pCostume->GetCostumeSlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		CostumeSlot kSlot;
		if( !pCostume->GetCostumeSlotArray( i, kSlot ) )
			continue;

		if( kSlot.m_PeriodType != PCPT_MORTMAIN )
			continue;

		int nGroup = (kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
		if( !IsRightTab( dwTabID, nGroup ) )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_COSTUME;
		kInfo.m_iMagicCode = kSlot.m_nIndex;
		kInfo.m_bEquip = kSlot.m_bEquip;

		int nSlot = kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE;
		int nValue = kSlot.m_iValue1;

		if( !kSlot.m_bEquip )
			nValue += 100;

		kInfo.m_iPriorityOrder = nSlot * 10000 + nValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_nCostumeCode;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwTabID == ID_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
	{
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );
	}
}

void CostumeCustomSelectWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		TEXT_X		= 17,
		TEXT_Y		= 474,

		LINE_GAP	= 18,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "★" );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "보유하고 있는" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 영구 코스튬만 스킨 변경이 가능합니다" );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "★" );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "아이템 하나로" );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 남성용" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, ", " );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "여성용" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 코스튬 중 한가지 선택 후 변경이 가능합니다 " );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+18, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "★ 변경한 스킨은 다른 스킨으로 덮어쓸 수 있습니다" );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+36, TAT_LEFT );
	kPrinter.ClearList();
}

void CostumeCustomSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessCustomBtnTooltip();
}

void CostumeCustomSelectWnd::ProcessCustomBtnTooltip()
{
	CostumeSlot kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for( int i=ID_ITEM_INFO_WND_1_BTN; i<=ID_ITEM_INFO_WND_8_BTN; i++ )
	{
		CostumeCustomInfoWnd *pItemWnd = dynamic_cast<CostumeCustomInfoWnd*>(FindChildWnd( i ));
		if( !pItemWnd || !pItemWnd->IsShow() || !pItemWnd->IsCustomToolTip() ) 
			continue;

		ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
		if( !pCostume ) 
			continue;

		int iMagicCode = pItemWnd->GetMagicCode();
		if( !pCostume->GetCostumeSlot( iMagicCode, kSlot ) ) 
			continue;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "코스튬 스킨 - " );

		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "남성" );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "여성 " );
		}		
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "남성 " );
		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "여성 " );
		}
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "이 적용되어있습니다" );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "＊아이콘을 " );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "클릭하면 자세히" );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, " 볼 수 있습니다" );
	}

	CostumeEquipInfo sCostumeEquipInfo;
	sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;
	sCostumeEquipInfo.m_Costume_Male_Custom = kSlot.m_dwMaleCustom;
	sCostumeEquipInfo.m_Costume_Female_Custom = kSlot.m_dwFemaleCustom;

	CostumeCustomThumbnailTooltip *pThumbnailTooltip = dynamic_cast<CostumeCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( COSTUME_CUSTOM_SELECT_TOOLTIP ));
	if( pThumbnailTooltip )
	{
		pThumbnailTooltip->SetTooltipText( kPrinter, sCostumeEquipInfo );
		pThumbnailTooltip->IsShow();
	}

	for( int i=0; i<GLOBAL_TEXT_TOOLTIP_LINE; i++ )
		kPrinter[i].ClearList();

	// 장착 툴팁
	int nEquipCharArray = -1;
	for( int i=ID_ITEM_INFO_WND_1_BTN; i<=ID_ITEM_INFO_WND_8_BTN; i++ )
	{
		CostumeCustomInfoWnd *pItemWnd = dynamic_cast<CostumeCustomInfoWnd*>(FindChildWnd( i ));
		if( !pItemWnd || !pItemWnd->IsShow() || !pItemWnd->IsIconOver() ) 
			continue;

		nEquipCharArray = g_MyInfo.FindEquipedCostumeChar( pItemWnd->GetMagicCode() );
		break;
	}

	if( nEquipCharArray != -1 )
	{
		const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( nEquipCharArray );
		if( rkCharInfo.m_class_type > 0 )
		{
			ioHashString szClassName = g_MyInfo.GetClassName( rkCharInfo.m_class_type, g_MyInfo.GetCharPowerUpGradeByClassType( rkCharInfo.m_class_type ) );

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			if( rkCharInfo.m_sex == 1 )
			{
				kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s - 남성", szClassName.c_str() );
			}
			else
			{
				kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, "%s - 여성", szClassName.c_str() );
			}
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "이 착용 중입니다." );
		}
	}

	GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_INFO_TOOLTIP_WND ));
	if( pTooltip )
		pTooltip->SetTooltipText( kPrinter );
}

int CostumeCustomSelectWnd::GetTabType( DWORD dwTabID )
{
	int iArray = ( dwTabID - ID_TAB_1_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	switch( dwTabID )
	{
	case ID_TAB_2_BTN:
		return 1;
	case ID_TAB_3_BTN:
		return 2;
	case ID_TAB_4_BTN:
		return 3;
	case ID_TAB_5_BTN:
		return 4;
	}

	return -1;
}

bool CostumeCustomSelectWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void CostumeCustomSelectWnd::Update()
{
	UpdatePage( m_iCurPage );
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 선택 창)
CostumeCustomMakeWnd::CostumeCustomMakeWnd()
{
	m_bMale = true;
	m_TexSelectCount = 0;
	m_eFileUplaodState = FUS_NONE;
	m_dwUploadDelayTime = 0;
	m_dwUploadUniqueIndex = 0;

	m_dwHelpID = 0;
}

CostumeCustomMakeWnd::~CostumeCustomMakeWnd()
{
}

void CostumeCustomMakeWnd::iwm_show()
{
	g_GUIMgr.HideWnd( COSTUME_CUSTOM_SELECT_WND );
}

void CostumeCustomMakeWnd::AfterViewLSCTexture( int iSelectCount )
{
	CostumeCustomMake3DViewWnd *pAfter3D = dynamic_cast< CostumeCustomMake3DViewWnd * >( FindChildWnd( ID_AFTER_VIEW ) );
	if( pAfter3D )
		pAfter3D->ChangeLSCTexture( m_OriginalTexList, m_AfterTexList );

	m_TexSelectCount = iSelectCount;

	char szBtnTitle[MAX_PATH] = "";
	SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), "파일 올리기 (%d/%d)", m_TexSelectCount, (int)m_OriginalTexList.size() );
	ioWnd *pUploadBtn = FindChildWnd( ID_UPLOAD_FILE );
	if( pUploadBtn )
	{
		pUploadBtn->SetTitleText( szBtnTitle );
		pUploadBtn->SetActive();
	}
}

void CostumeCustomMakeWnd::SetCostumeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
		return;

	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;
	m_OriginalTexList.clear();
	m_AfterTexList.clear();
	m_TexSelectCount = 0;

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
		return;

	if( kSlot.m_bEquip )
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			if( g_MyInfo.IsCharRentalToCostume( m_ItemInfo.m_iMagicCode ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#장비 스킨이 불가능합니다." );
				return;
			}
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "착용 중인 장비는 스킨이 불가능합니다." );
			return;
		}
	}

	m_szItemName = kSlot.m_Name;
	m_ItemNameMent.ClearList();
	m_ItemNameMent.SetTextStyle( TS_NORMAL );
	m_ItemNameMent.SetBkColor( 0, 0, 0 );	
	m_ItemNameMent.SetTextColor( TCT_DEFAULT_GRAY );
	m_ItemNameMent.AddTextPiece( FONT_SIZE_17, "%s", kSlot.m_Name.c_str() );
	if( m_bMale )
	{
		m_ItemNameMent.SetTextColor( TCT_DEFAULT_BLUE );
		m_ItemNameMent.AddTextPiece( FONT_SIZE_17, " (남성)" );
	}
	else
	{
		m_ItemNameMent.SetTextColor( TCT_DEFAULT_RED );
		m_ItemNameMent.AddTextPiece( FONT_SIZE_17, " (여성)" );
	}	

	// Before 세팅
	CostumeCustomMake3DViewWnd *pBefore3D = dynamic_cast< CostumeCustomMake3DViewWnd * >( FindChildWnd( ID_BEFORE_VIEW ) );
	if( pBefore3D )
	{
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		if( m_bMale )
		{
			kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
			kCharInfo.m_hair       = 1;
		}
		else
		{
			kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
			kCharInfo.m_hair       = 4;
		}

		CostumeEquipInfo sCostumeEquipInfo;
		sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;

		pBefore3D->CreateCharacter( kCharInfo, sCostumeEquipInfo, m_OriginalTexList );		
	}

	if( (int)m_OriginalTexList.empty() )
	{
		if( m_bMale )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "[%s] (남성)은#이미지없이 이펙트만으로 제작되어서#스킨 변경이 불가능합니다", m_szItemName.c_str() );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "[%s] (여성)은#이미지없이 이펙트만으로 제작되어서#스킨 변경이 불가능합니다", m_szItemName.c_str() );
		}
		return;
	}

	// After 세팅
	CostumeCustomMake3DViewWnd *pAfter3D = dynamic_cast< CostumeCustomMake3DViewWnd * >( FindChildWnd( ID_AFTER_VIEW ) );
	if( pAfter3D )
	{
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		if( m_bMale )
		{
			kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
			kCharInfo.m_hair       = 1;
		}
		else
		{
			kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
			kCharInfo.m_hair       = 4;
		}

		CostumeEquipInfo sCostumeEquipInfo;
		sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;
		sCostumeEquipInfo.m_Costume_Male_Custom = kSlot.m_dwMaleCustom;
		sCostumeEquipInfo.m_Costume_Female_Custom = kSlot.m_dwFemaleCustom;

		pAfter3D->CreateCharacter( kCharInfo, sCostumeEquipInfo, m_AfterTexList );
	}

	// 
	m_TextureCountMent.ClearList();
	m_TextureCountMent.SetTextStyle( TS_NORMAL );
	m_TextureCountMent.SetBkColor( 0, 0, 0 );	
	m_TextureCountMent.SetTextColor( TCT_DEFAULT_ORANGE );
	m_TextureCountMent.AddTextPiece( FONT_SIZE_13, "(총 %d개의 텍스쳐 파일(jpg)이 필요합니다)", (int)m_OriginalTexList.size() );

	char szBtnTitle[MAX_PATH] = "";
	SafeSprintf( szBtnTitle, sizeof(szBtnTitle), "파일 올리기 (0/%d)", (int)m_OriginalTexList.size() );
	ioWnd *pUploadBtn = FindChildWnd( ID_UPLOAD_FILE );
	if( pUploadBtn )
	{
		pUploadBtn->SetTitleText( szBtnTitle );
		pUploadBtn->SetActive();
	}

	ShowWnd();
}

void CostumeCustomMakeWnd::UploadTexture()
{
	if( m_OriginalTexList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "변경 가능한 텍스쳐가 없습니다." );
		return;
	}

	//
	ItemCustomMakeUploadWnd *pUploadWnd = dynamic_cast<ItemCustomMakeUploadWnd*>(g_GUIMgr.FindWnd(ITEM_CUSTOM_MAKE_UPLOAD_WND));
	if( pUploadWnd )
	{
		pUploadWnd->SetUpload( GetID(), m_ItemNameMent, m_TextureCountMent, m_OriginalTexList );
		pUploadWnd->ShowWnd();
	}
}

void CostumeCustomMakeWnd::DownloadTexture()
{
	if( m_OriginalTexList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "변경 가능한 텍스쳐가 없습니다." );
		return;
	}

	char szDesktopPath[MAX_PATH] = "";
	SHGetSpecialFolderPath( g_App.GetHWnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );

	char szSavePath[MAX_PATH] = "";
	SafeSprintf( szSavePath, sizeof( szSavePath ), "%s\\로스트사가 장비 원본 텍스쳐", szDesktopPath );
	CreateDirectory( szSavePath, NULL );

	SYSTEMTIME st;
	GetLocalTime( &st );
	ioHashString kFirstFileName;
    for(int i = 0;i < (int)m_OriginalTexList.size();i++)
	{
		CustomTexture &rkData = m_OriginalTexList[i];

		bool bTexCreate = false;
		ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( rkData.m_szTextureName.c_str() );
		if( !pDDSTex )
		{
			bTexCreate = true;
			pDDSTex = g_TextureMgr.CreateTexture( rkData.m_szTextureName.c_str(), D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
		}
		else if( pDDSTex )
		{
			// 포멧 고퀄로 변경
			g_TextureMgr.ChangeTextureQuality( pDDSTex, TQ_HIGH );
		}

		if( !pDDSTex )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "(%s) 텍스쳐를 저장하던 중에 오류가 발생했습니다(1)", rkData.m_szTextureName.c_str() );
			return;
		}

		//
		char szFileName[2048] = "";
		StringCbPrintf( szFileName, sizeof(szFileName),	"%s_%d_%02d%02d%02d_%02d%02d%02d.jpg", m_szItemName.c_str(), rkData.m_dwCustomSeq,
                        st.wYear - 2000, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

		if( kFirstFileName.IsEmpty() )
			kFirstFileName = szFileName;
		//
		char szFullPath[2048] ="";		
		StringCbPrintf( szFullPath, sizeof(szFullPath),	"%s\\%s", szSavePath, szFileName );
		
		//
		if( !Help::CustomTextureToJPG( pDDSTex, szFullPath ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "(%s) 텍스쳐를 저장하던 중에 오류가 발생했습니다(2)", rkData.m_szTextureName.c_str() );

			if( bTexCreate )
			{
				g_TextureMgr.DestroyByName( rkData.m_szTextureName.c_str(), true );
			}
			return;
		}

		if( bTexCreate )
		{
			g_TextureMgr.DestroyByName( rkData.m_szTextureName.c_str(), true );
		}
		else
		{
			// 원래 퀄리티로 변경
			g_TextureMgr.ChangeTextureQuality( pDDSTex, Setting::GetTextureQuality() );
		}
	}

	ItemCustomMakeDownloadWnd *pDownComplete = dynamic_cast< ItemCustomMakeDownloadWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_DOWNLOAD_WND ) );
	if( pDownComplete )
	{
		pDownComplete->SetDownload( szSavePath, kFirstFileName, m_OriginalTexList.size() );
		pDownComplete->ShowWnd();
	}
}

void CostumeCustomMakeWnd::ShowDetailView()
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
		return;

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
		return;

	// 자세히보기 세팅
	CostumeCustomMakeDetailViewWnd *pDetailView = dynamic_cast< CostumeCustomMakeDetailViewWnd * >( g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_DETAIL_VIEW_WND ) );
	if( pDetailView )
	{
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		if( m_bMale )
			kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		else
			kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷

		CostumeEquipInfo sCostumeEquipInfo;
		sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;

		pDetailView->CreateCharacter( kCharInfo, sCostumeEquipInfo, m_AfterTexList );
		pDetailView->ShowWnd();
	}
}

void CostumeCustomMakeWnd::StartCustomFileMake( DWORD dwUniqueIndex )
{
	if( !IsShow() )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "변경된 데이터가 존재하지 않습니다 (1)" );
		return;
	}

	m_dwUploadUniqueIndex = dwUniqueIndex;

	int i = 0;
	// 업로드할 파일 검사 및 경로 생성
	m_FileUploadDataList.clear();
	m_FileUploadCompleteList.clear();
	for(i = 0;i < (int)m_AfterTexList.size();i++)
	{
		CustomTexture &rkData = m_AfterTexList[i];

		char szExt[MAX_PATH] = "";
		_splitpath( rkData.m_szTextureName.c_str(), NULL, NULL, NULL, szExt );

		if( strcmp( szExt, ".lsc" ) != 0 )
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "변경된 데이터가 존재하지 않습니다#(%s)", rkData.m_szTextureName.c_str() );
			return;
		}
		
		FileUploadData kUpload;
		sprintf( kUpload.m_szDstFilePath, "%s%s", g_CustomTextureMgr.GetAppDirectory().c_str(), rkData.m_szTextureName.c_str() );
		sprintf( kUpload.m_szWriteFilePath, "%d_%d.lsc", m_dwUploadUniqueIndex, rkData.m_dwCustomSeq );

		// 이미지 사이즈
		bool bTexCreate = false;
		ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( rkData.m_szTextureName );
		if( !pDDSTex )
		{
			bTexCreate = true;
			pDDSTex = g_TextureMgr.CreateTexture( rkData.m_szTextureName, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
		}
		else if( pDDSTex )
		{
			// 포멧 고퀄로 변경
			g_TextureMgr.ChangeTextureQuality( pDDSTex, TQ_HIGH );
		}

		if( !pDDSTex )
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "(%s) 텍스쳐를 불러오던 중에 오류가 발생했습니다", rkData.m_szTextureName.c_str() );
			return;
		}
		kUpload.m_iSrcWidth  = pDDSTex->GetWidth();
		kUpload.m_iSrcHeight = pDDSTex->GetHeight();

		if( bTexCreate )
			g_TextureMgr.DestroyByName( rkData.m_szTextureName, true );
		else
			g_TextureMgr.ChangeTextureQuality( pDDSTex, Setting::GetTextureQuality() );

		m_FileUploadDataList.push_back( kUpload );
	}

	m_eFileUplaodState = FUS_FILE_UPLOAD_DALAY;
	m_dwUploadDelayTime= REALGETTIME();

	FileWriteUploadWnd *pFileUploadWnd = dynamic_cast< FileWriteUploadWnd * >( g_GUIMgr.FindWnd( FILE_WRITE_SERVER_PROCESS ) );
	if( pFileUploadWnd )
	{
		pFileUploadWnd->HideWnd();
		pFileUploadWnd->InitUploadData( m_FileUploadDataList.size() );
	}
}

bool CostumeCustomMakeWnd::iwm_esc()
{
	if( m_eFileUplaodState != FUS_NONE || g_App.IsMouseBusy() )
	{
		return false;
	}
	return ioWnd::iwm_esc();
}

void CostumeCustomMakeWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwHelpID = xElement.GetIntAttribute_e( "HelpID" );
}

void CostumeCustomMakeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 업로드 중이면 블럭 조치
			if( m_eFileUplaodState == FUS_NONE )
				HideWnd();
		}
		break;
	case ID_HELP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 업로드 중이면 블럭 조치
			if( m_eFileUplaodState == FUS_NONE )
			{
				HideWnd();

				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( m_dwHelpID );
			}
		}
		break;
	case ID_SELECT_ITEM:
		if( cmd == IOBN_BTNUP )
		{	
			// 업로드 중이면 블럭 조치
			if( m_eFileUplaodState == FUS_NONE )
			{
				HideWnd();
				g_GUIMgr.ShowWnd( COSTUME_CUSTOM_SELECT_WND );
			}
		}
		break;
	case ID_UPLOAD_FILE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_eFileUplaodState == FUS_NONE )
				UploadTexture();
		}
		break;
	case ID_DOWNLOAD_FILE:
		if( cmd == IOBN_BTNUP )
		{
			if( m_eFileUplaodState == FUS_NONE )
				DownloadTexture();		
		}
		break;
	case ID_DETAIL_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			if( m_eFileUplaodState == FUS_NONE )
				ShowDetailView(); 
		}
		break;
	case ID_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( param == 0 && m_eFileUplaodState == FUS_NONE ) // user click
			{
				/*
				// 코스튬 관련 (현재 사용하지 않는 기능 협의)
				ioUserEtcItem::ETCITEMSLOT kEtcItem;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( !pUserEtcItem || !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_CUSTOM_COSTUME_SKIN, kEtcItem ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "[장비스킨]#아이템을 보유하고 있지 않습니다" );
				}
				else if( m_TexSelectCount == 0 )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택된 텍스쳐가 없습니다" );
				else
				{
					CostumeCustomChangeConfirmWnd *pChangeConfirmWnd = dynamic_cast< CostumeCustomChangeConfirmWnd * >( g_GUIMgr.FindWnd( COSTUME_CUSTOM_CHANGE_CONFIRM_WND ) );
					if( pChangeConfirmWnd )
						pChangeConfirmWnd->SetCostumeCustomChangeInfo( m_ItemInfo, m_bMale );
				}
				*/
			}
			else  // g_FileWriteClient 응답
			{
				EndFileUpload( param );
			}			
		}
		break;
	}
}

void CostumeCustomMakeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	g_FontMgr.PrintText( iXPos + 92, iYPos + 378, FONT_SIZE_12, "다운받거나 직접 제작한" );
	g_FontMgr.PrintText( iXPos + 92, iYPos + 393, FONT_SIZE_12, "텍스쳐 파일을 올립니다" );

	g_FontMgr.PrintText( iXPos + 245, iYPos + 378, FONT_SIZE_12, "텍스쳐 파일 원본을 받아서" );
	g_FontMgr.PrintText( iXPos + 245, iYPos + 393, FONT_SIZE_12, "수정합니다" );

	g_FontMgr.PrintText( iXPos + 398, iYPos + 378, FONT_SIZE_12, "텍스쳐 파일을 확대해서" );
	g_FontMgr.PrintText( iXPos + 398, iYPos + 393, FONT_SIZE_12, "자세히 확인합니다" );

	m_ItemNameMent.PrintFullText( iXPos + 246, iYPos + 294, TAT_CENTER );
	m_TextureCountMent.PrintFullText( iXPos + 246, iYPos + 315, TAT_CENTER );
}

void CostumeCustomMakeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessUploadDelay();
}

void CostumeCustomMakeWnd::ProcessUploadDelay()
{
	// g_FileWriteClient.Start()에서 소켓을 연결하게 되는데 패킷을 받을때 호출하면 크래쉬가 발생함. 그래서 딜레이를 주어 처리
	if( m_eFileUplaodState != FUS_FILE_UPLOAD_DALAY )
		return;
	if( m_FileUploadDataList.empty() )
		return;

	enum { DELAY_TIME = 100, };
	if( REALGETTIME() - m_dwUploadDelayTime < DELAY_TIME )
		return;

	m_dwUploadDelayTime = 0;
	FileUploadData &rkUpload = m_FileUploadDataList[0];
	if( g_FileWriteClient.Start( rkUpload.m_szDstFilePath, rkUpload.m_szWriteFilePath, 1000000, rkUpload.m_iSrcWidth, rkUpload.m_iSrcHeight, GetID(), ID_CHANGE_BTN ) )
	{
		m_eFileUplaodState = FUS_FILE_UPLOAD;

		FileWriteUploadWnd *pFileUploadWnd = dynamic_cast< FileWriteUploadWnd * >( g_GUIMgr.FindWnd( FILE_WRITE_SERVER_PROCESS ) );
		if( pFileUploadWnd && !pFileUploadWnd->IsShow() )
		{
			pFileUploadWnd->ShowWnd();
		}
	}
}

void CostumeCustomMakeWnd::EndFileUpload( DWORD param )
{
	if( param == ioFileWriteClient::RETURN_WND_PARAM_FAIL )
	{
		m_eFileUplaodState = FUS_NONE;
		g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 업로드에 실패했습니다#잠시 후 다시 시도해주세요" );
	}
	else if( param == ioFileWriteClient::RETURN_WND_PARAM_COMPLETE )
	{
		if( !IsShow() )
		{
			m_eFileUplaodState = FUS_NONE;
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 업로드가 중단 되었습니다" );
		}
		else
		{
			// 다음 파일 업로드 시작
			m_FileUploadCompleteList.push_back( m_FileUploadDataList[0] );
			m_FileUploadDataList.erase( m_FileUploadDataList.begin() );
			if( !m_FileUploadDataList.empty() )
			{
				FileWriteUploadWnd *pFileUploadWnd = dynamic_cast< FileWriteUploadWnd * >( g_GUIMgr.FindWnd( FILE_WRITE_SERVER_PROCESS ) );
				if( pFileUploadWnd && pFileUploadWnd->IsShow() )
				{
					pFileUploadWnd->UploadRemainFile( m_FileUploadDataList.size() );
				}
				m_eFileUplaodState = FUS_FILE_UPLOAD_DALAY;
				m_dwUploadDelayTime= REALGETTIME();
			}
			else
			{
				// 업로드 완료

				// 장비값 및 변경 적용 - 서버 동기화
				if( m_ItemInfo.m_iMagicCode > 0 )
				{
					CostumeSlot kSlot;
					ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
					if( pCostume && pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
					{
						if( m_bMale )
							kSlot.m_dwMaleCustom = m_dwUploadUniqueIndex;
						else
							kSlot.m_dwFemaleCustom = m_dwUploadUniqueIndex;

						pCostume->SetCostumeSlot( kSlot );

						SP2Packet kPacket( CTPK_ETCITEM_USE );
						kPacket << ioEtcItem::EIT_ETC_CUSTOM_COSTUME_SKIN;
						kPacket << kSlot.m_nIndex;
						kPacket << kSlot.m_dwMaleCustom;
						kPacket << kSlot.m_dwFemaleCustom;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}	
					else
					{
						m_eFileUplaodState = FUS_NONE;
						g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨을 적용할 대상을 찾지 못했습니다" );
					}
				}
				else
				{
					m_eFileUplaodState = FUS_NONE;
					g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨을 적용할 대상을 찾지 못했습니다" );
				}
			}
		}
	}
}

void CostumeCustomMakeWnd::CompleteCustomFileMake()
{
	g_MyInfo.UpdateEquipCostume( m_ItemInfo.m_iMagicCode );

	HideWnd();
	g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );

	CostumeCustomMakeCompleteWnd *pCompleteWnd = dynamic_cast< CostumeCustomMakeCompleteWnd * >( g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_COMPLETE_WND ) );
	if( pCompleteWnd )
		pCompleteWnd->SetCostumeCustomMakeInfo( m_ItemInfo, m_bMale );
	
	m_eFileUplaodState = FUS_NONE;
	for(int i = 0;i < (int)m_FileUploadCompleteList.size();i++)
	{
		FileUploadData &rkData = m_FileUploadCompleteList[i];
		::DeleteFile( rkData.m_szDstFilePath );
	}
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 미리보기 창)
CostumeCustomMake3DViewWnd::CostumeCustomMake3DViewWnd()
{
	m_pStateFrm   = NULL;
	m_pRectSrc    = NULL;
	m_pMouseLeft  = NULL;	
	m_pMouseRight = NULL;
	m_pMouseMiddle= NULL;
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;
}

CostumeCustomMake3DViewWnd::~CostumeCustomMake3DViewWnd()
{
	SAFEDELETE( m_pStateFrm );
	SAFEDELETE( m_pRectSrc );
	SAFEDELETE( m_pMouseLeft );	
	SAFEDELETE( m_pMouseRight );
	SAFEDELETE( m_pMouseMiddle );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void CostumeCustomMake3DViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "StateFrm" )
	{
		SAFEDELETE( m_pStateFrm );
		m_pStateFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CostumeCustomMake3DViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RectSrc" )
	{
		SAFEDELETE( m_pRectSrc );
		m_pRectSrc = pImage;
	}
	else if( szType == "MouseLeft" )
	{
		SAFEDELETE( m_pMouseLeft );
		m_pMouseLeft = pImage;
	}
	else if( szType == "MouseRight" )
	{
		SAFEDELETE( m_pMouseRight );
		m_pMouseRight = pImage;
	}
	else if( szType == "MouseMiddle" )
	{
		SAFEDELETE( m_pMouseMiddle );
		m_pMouseMiddle = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CostumeCustomMake3DViewWnd::CreateCharacter( const CHARACTER &rkInfo, const CostumeEquipInfo &sCostumeEquipInfo, CustomTextureList &rkReturn )
{		
	CostumeInfo sCostumeInfo;
	if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeEquipInfo.m_CostumeCode, sCostumeInfo ) )
		return;
	
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( rkInfo, "ResultChar", false );
	if( !pChar )
		return;

	pChar->SetPosition( 0.0f, -80.0f, 950.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );

	float fAspect = (float)(GetWidth() - 2)/(float)(GetHeight() - 2);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 20.0f, fAspect );

	int nSubType = (sCostumeEquipInfo.m_CostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
	// UI코스튬 관련 (임시 아이템 코드 설정 - 코스튬 적용을 위해)
	DWORD dwTempItemCode = 0;
	switch ( nSubType )
	{
		/*
	case ioItem::IT_WEAPON:
		dwTempItemCode = 1;
		break;
		*/
	case ioItem::IT_ARMOR:
		dwTempItemCode = 100001;
		break;
	case ioItem::IT_HELMET:
		dwTempItemCode = 200001;
		break;
	case ioItem::IT_CLOAK:
		dwTempItemCode = 300001;
		break;
	}

	pChar->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );
	pChar->RotateY( -15.0f );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	m_aMyChar->SetMannequins( true );

	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{	
		switch( nSubType )
		{
			/*
		case ioItem::IT_WEAPON:
			{
				ioHashString szNewMeshName = Help::ConvertName( pWeaponItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( pWeaponItem->GetEquipMesh(NULL), true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

				m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
				m_pUI3DRender->SetCameraPos( -110.0f, 20.0f, 615.0f );
			}
			break;
			*/
		case ioItem::IT_ARMOR:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szBodyMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szBodyMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

				szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHandMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHandMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

				m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
				m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 600.0f );
			}
			break;
		case ioItem::IT_HELMET:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHelmetMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHelmetMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

				m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
				pChar->SetPosition( 0.0f, -130.0f, 950.0f );
				m_pUI3DRender->SetCameraPos( -110.0f, -20.0f, 710.0f );
			}
			break;
		case ioItem::IT_CLOAK:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szCloakMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szCloakMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

				m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
				m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 1300.0f );
			}
			break;
		}			
	}
}

void CostumeCustomMake3DViewWnd::ChangeLSCTexture( CustomTextureList &rkOriTexList, CustomTextureList &rkAftTexList )
{
	if( rkOriTexList.size() != rkAftTexList.size() )
		return;
	
	if( m_aMyChar )
	{
		int iMaxTex = rkAftTexList.size();
		for(int i = 0;i < iMaxTex;i++)
		{
			CustomTexture &rkOriData = rkOriTexList[i];
			CustomTexture &rkAftData = rkAftTexList[i];

			char szFileName[MAX_PATH] = "";
			_splitpath( rkOriData.m_szTextureName.c_str(), NULL, NULL, szFileName, NULL );
			char szLSCName[MAX_PATH] = "";
			sprintf( szLSCName, "%s.lsc", szFileName );

			if( g_TextureMgr.GetTextureByName( szLSCName ) )
				g_TextureMgr.DestroyByName( szLSCName, true );

			Help::CustomEntityGroupTexture( m_aMyChar->GetGroup(), rkAftData.m_szTextureName, szLSCName, rkAftData.m_dwCustomSeq );
			rkAftData.m_szTextureName = szLSCName;
		}
	}
}

void CostumeCustomMake3DViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	if( GetParent() )
	{
		if( !GetParent()->IsShow() ) 
			return;
	}

	if( m_aMyChar )
	{
		if( m_pUI3DRender )
		{
			if( ioWnd::m_pTopModalWnd == this || GetParent() == ioWnd::m_pTopModalWnd )
			{
				ioWnd *pClickWnd = g_GUIMgr.GetPreClickWnd();
				if( !pClickWnd || pClickWnd->GetWndType() != IWT_BUTTON )
					m_pUI3DRender->UpdateCameraByMouse( 20.0f, 20.0f, 4.0f );
			}
		}

		m_aMyChar->Update( fTimePerSec );
	}
}

void CostumeCustomMake3DViewWnd::OnRectRender( int iXPos, int iYPos )
{
	if( !m_pRectSrc ) 
		return;

	for( int i=0; i<RECT_HEIGHT_CNT; i++ )
	{
		for( int j=0; j<RECT_WIDTH_CNT; j++ )
			m_pRectSrc->Render( iXPos + (j * RECT_GAP_SIZE), iYPos );

		iYPos += RECT_GAP_SIZE;
	}
}

void CostumeCustomMake3DViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRectRender( iXPos, iYPos );

	if( m_pMouseLeft )
	{
		m_pMouseLeft->SetScale( 0.66f );
		m_pMouseLeft->Render( iXPos + 143, iYPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 146, iYPos + 39, FONT_SIZE_10, "이동" );
	}
	if( m_pMouseRight )
	{
		m_pMouseRight->SetScale( 0.66f );
		m_pMouseRight->Render( iXPos + 165, iYPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 168, iYPos + 39, FONT_SIZE_10, "회전" );
	}
	if( m_pMouseMiddle )
	{
		m_pMouseMiddle->SetScale( 0.66f );
		m_pMouseMiddle->Render( iXPos + 187, iYPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 190, iYPos + 39, FONT_SIZE_10, "확대" );
	}

	if( m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 1;
		rcD3D.y1 = iYPos + 1;
		rcD3D.x2 = rcD3D.x1 + (GetWidth()-2);
		rcD3D.y2 = rcD3D.y1 + (GetHeight()-2);
		m_pUI3DRender->RenderViewPort( &rcD3D, true );

//		g_FontMgr.SetTextStyle( TS_NORMAL );
//		g_FontMgr.SetAlignType( TAT_CENTER );
//		g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
//		g_FontMgr.PrintText( iXPos + (GetWidth()/2), iYPos + (GetHeight()/2), FONT_SIZE_12, "%.2f - %.2f - %.2f", m_pUI3DRender->GetCameraPosition().x, m_pUI3DRender->GetCameraPosition().y, m_pUI3DRender->GetCameraPosition().z );
	}

	if( m_pStateFrm )
	{
		m_pStateFrm->Render( iXPos, iYPos );

		switch( GetID() )
		{
		case CostumeCustomMakeWnd::ID_BEFORE_VIEW:
			{
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, "원본" );
			}
			break;
		case CostumeCustomMakeWnd::ID_AFTER_VIEW:
			{
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, "변경" );
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 자세히 보기 창)
CostumeCustomMakeDetailViewWnd::CostumeCustomMakeDetailViewWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;

	m_pTitleBar   = NULL;
	m_pBackground = NULL;
	m_pMouseLeft  = NULL;	
	m_pMouseRight = NULL;
	m_pMouseMiddle= NULL;
}

CostumeCustomMakeDetailViewWnd::~CostumeCustomMakeDetailViewWnd()
{
	SAFEDELETE( m_pTitleBar );
	SAFEDELETE( m_pBackground );
	SAFEDELETE( m_pMouseLeft );
	SAFEDELETE( m_pMouseRight );
	SAFEDELETE( m_pMouseMiddle );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void CostumeCustomMakeDetailViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "TitleBar" )
	{
		SAFEDELETE( m_pTitleBar );
		m_pTitleBar = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CostumeCustomMakeDetailViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Background" )
	{
		SAFEDELETE( m_pBackground );
		m_pBackground = pImage;
	}
	else if( szType == "MouseLeft" )
	{
		SAFEDELETE( m_pMouseLeft );
		m_pMouseLeft = pImage;
	}
	else if( szType == "MouseRight" )
	{
		SAFEDELETE( m_pMouseRight );
		m_pMouseRight = pImage;
	}
	else if( szType == "MouseMiddle" )
	{
		SAFEDELETE( m_pMouseMiddle );
		m_pMouseMiddle = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CostumeCustomMakeDetailViewWnd::CreateCharacter( const CHARACTER &rkInfo, const CostumeEquipInfo &sCostumeEquipInfo, CustomTextureList &rkTexList )
{
	CostumeInfo sCostumeInfo;
	if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeEquipInfo.m_CostumeCode, sCostumeInfo ) )
		return;

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( rkInfo, "ResultChar" );
	if( !pChar )
		return;

	pChar->SetPosition( 0.0f, -80.0f, 950.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );

	float fAspect = (float)(VIEW_WIDTH)/(float)(VIEW_HEIGHT);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 45.0f, fAspect );

	int nSubType = (sCostumeEquipInfo.m_CostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
	// UI코스튬 관련 (임시 아이템 코드 설정 - 코스튬 적용을 위해)
	DWORD dwTempItemCode = 0;
	switch ( nSubType )
	{
		/*
	case ioItem::IT_WEAPON:
		dwTempItemCode = 1;
		break;
		*/
	case ioItem::IT_ARMOR:
		dwTempItemCode = 100001;
		break;
	case ioItem::IT_HELMET:
		dwTempItemCode = 200001;
		break;
	case ioItem::IT_CLOAK:
		dwTempItemCode = 300001;
		break;
	}

	pChar->EquipItem( dwTempItemCode, 0, 0, sCostumeEquipInfo.m_CostumeCode );
	pChar->RotateY( -15.0f );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	m_aMyChar->SetMannequins( false );
	m_aMyChar->AttachEffect( "ui_grid.txt", NULL );

	CustomTextureList kTempList;
	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{	
		if( m_aMyChar->IsMale() )
			SafeSprintf( m_szSkinName, sizeof(m_szSkinName), "%s (남성)", sCostumeInfo.m_szName.c_str() );
		else
			SafeSprintf( m_szSkinName, sizeof(m_szSkinName), "%s (여성)", sCostumeInfo.m_szName.c_str() );

		switch( nSubType )
		{
			/*
		case ioItem::IT_WEAPON:
			{
				const ioWeaponItem *pWeaponItem = ToWeaponItemConst( pItem );
				ioHashString szNewMeshName = Help::ConvertName( pWeaponItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
					{
						szNewMeshName = Help::ConvertName( pWeaponItem->GetEquipMesh(NULL), true );
					}
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

				m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
			}
			break;
			*/
		case ioItem::IT_ARMOR:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szBodyMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szBodyMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

				szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHandMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHandMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

				m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
			}
			break;
		case ioItem::IT_HELMET:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHelmetMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szHelmetMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

				m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
			}
			break;
		case ioItem::IT_CLOAK:
			{
				ioHashString szNewMeshName = Help::ConvertName( sCostumeInfo.m_szCloakMesh, m_aMyChar->IsMale() );
				if( !m_aMyChar->IsMale() )
				{
					char szFullName[MAX_PATH]="";
					StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
					if( !g_ResourceLoader.IsFileExist( szFullName ) )
						szNewMeshName = Help::ConvertName( sCostumeInfo.m_szCloakMesh, true );
				}
				Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

				m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
			}
			break;
		}			
	}

	if( kTempList.size() == rkTexList.size() )
	{
		for(int i = 0;i < (int)rkTexList.size();i++)
			Help::CustomEntityGroupTexture( m_aMyChar->GetGroup(), kTempList[i].m_szTextureName, rkTexList[i].m_szTextureName, kTempList[i].m_dwCustomSeq );
	}
}

void CostumeCustomMakeDetailViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void CostumeCustomMakeDetailViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( GetParent() )
	{
		if( !GetParent()->IsShow() ) return;
	}

	if( m_aMyChar )
	{
		if( m_pUI3DRender )
		{
			if( ioWnd::m_pTopModalWnd == this )
			{
				m_pUI3DRender->UpdateCameraByMouse( 20.0f, 20.0f, 4.0f );
			}
		}

		m_aMyChar->Update( fTimePerSec );
	}
}

void CostumeCustomMakeDetailViewWnd::OnRenderBackground( int iXPos, int iYPos )
{
	if( !m_pBackground ) return;

	int iWidthCnt = 750 / m_pBackground->GetWidth();
	for(int i = 0;i < iWidthCnt;i++)
	{
		m_pBackground->Render( iXPos + 2 + (m_pBackground->GetWidth() * i), iYPos + 6 );
	}
}

void CostumeCustomMakeDetailViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBackground( iXPos, iYPos );

	if( m_pTitleBar )
		m_pTitleBar->Render( iXPos, iYPos );

	if( m_pMouseLeft )
		m_pMouseLeft->Render( iXPos + 22, iYPos + 59 );
	if( m_pMouseRight )
		m_pMouseRight->Render( iXPos + 54, iYPos + 59 );
	if( m_pMouseMiddle )
		m_pMouseMiddle->Render( iXPos + 86, iYPos + 59 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_18, "스킨 파일 자세히보기 : " );

	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( 255, 188, 26 );
	kPrinter.AddTextPiece( FONT_SIZE_18, "%s", m_szSkinName );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 36, iYPos + 106, FONT_SIZE_12, "이동" );
	g_FontMgr.PrintText( iXPos + 68, iYPos + 106, FONT_SIZE_12, "회전" );
	g_FontMgr.PrintText( iXPos + 100, iYPos + 106, FONT_SIZE_12, "확대" );

	if( m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 2;
		rcD3D.y1 = iYPos + 39;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
	}
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 현재 사용하지 않음...)
CostumeCustomMakeConfirmWnd::CostumeCustomMakeConfirmWnd()
{
	m_pExclamationMark = NULL;
}

CostumeCustomMakeConfirmWnd::~CostumeCustomMakeConfirmWnd()
{
	SAFEDELETE( m_pExclamationMark );
}

void CostumeCustomMakeConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ExclamationMark" )
	{
		SAFEDELETE( m_pExclamationMark );
		m_pExclamationMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

bool CostumeCustomMakeConfirmWnd::SetCostumeCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( pCostume && pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
	{
		m_szName = kSlot.m_Name;

		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ItemCustomIconBtn *pIconBtn = dynamic_cast<ItemCustomIconBtn*>(FindChildWnd(ID_ICON_BTN));
		if( pIconBtn )
		{
			pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1 );
			pIconBtn->SetCostume( true );
		}
	}
	else
	{
		HideWnd();
		return false;
	}

	if( !IsShow() )
		ShowWnd();

	return true;
}

void CostumeCustomMakeConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MAKE_BTN:
		if( cmd == IOBN_BTNUP )
		{			
			CostumeCustomMakeWnd *pMakeWnd = dynamic_cast<CostumeCustomMakeWnd*>(g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_WND ));
			if( pMakeWnd )
				pMakeWnd->SetCostumeInfo( m_ItemInfo, m_bMale );

			HideWnd();
		}
		break;
	case ID_CANCEL_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void CostumeCustomMakeConfirmWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		NAME_X		= 103,
		NAME_Y		= 149,

		TEXT_X		= 103,
		TEXT_Y		= 179,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, "%s", m_szName.c_str() );
	kPrinter.PrintFullText( iXPos+NAME_X, iYPos+NAME_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "장비의 스킨을 변경하면" );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "앞으로 거래소에서" );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+20, TAT_CENTER );

	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, "거래가 불가능" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "합니다." );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+40, TAT_CENTER );

	kPrinter.ClearList();
}

void CostumeCustomMakeConfirmWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		MARK_X		= 103,
		MARK_Y		= 98,
	};

	if( m_pExclamationMark )
		m_pExclamationMark->Render( iXPos+MARK_X, iYPos+MARK_Y );
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 완료)
CostumeCustomMakeCompleteWnd::CostumeCustomMakeCompleteWnd()
{
	m_pIconBack	  = NULL;
	m_pIcon		  = NULL;
	m_pCustomIcon = NULL;

	m_bMale = true;
}

CostumeCustomMakeCompleteWnd::~CostumeCustomMakeCompleteWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pCustomIcon );
}

void CostumeCustomMakeCompleteWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "CustomIcon" )
	{
		SAFEDELETE( m_pCustomIcon );
		m_pCustomIcon = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

bool CostumeCustomMakeCompleteWnd::SetCostumeCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( pCostume && pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
	{
		m_szName = kSlot.m_Name;

		SAFEDELETE( m_pIcon );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
	}
	else
	{
		HideWnd();
		return false;
	}

	if( !IsShow() )
		ShowWnd();

	return true;
}

void CostumeCustomMakeCompleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			// 아이템 보기
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->ShowCostumeTabDirect( -1 );

			HideWnd();
		}
		break;
	}
}

void CostumeCustomMakeCompleteWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void CostumeCustomMakeCompleteWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		int iNewXPos = iXPos + 103;
		int iNewYPos = iYPos + 98;
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );

		if( m_pIcon )
			m_pIcon->Render( iNewXPos, iNewYPos );

		if( m_pCustomIcon )
			m_pCustomIcon->Render( iNewXPos + 7, iNewYPos - 43 );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 149, FONT_SIZE_17, "%s", m_szName.c_str() );

	if( m_bMale )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 170, FONT_SIZE_17, "남성" );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 170, FONT_SIZE_17, "여성" );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 199, FONT_SIZE_13, "스킨 변경이 완료되었습니다" );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 217, FONT_SIZE_13, "변경한 스킨은 다른 스킨으로" );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 235, FONT_SIZE_13, "덮어쓰거나 삭제할 수 있습니다" );
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 변경 확인 윈도우)
CostumeCustomChangeConfirmWnd::CostumeCustomChangeConfirmWnd()
{
}

CostumeCustomChangeConfirmWnd::~CostumeCustomChangeConfirmWnd()
{
}

bool CostumeCustomChangeConfirmWnd::SetCostumeCustomChangeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( pCostume && pCostume->GetCostumeSlot( m_ItemInfo.m_iMagicCode, kSlot ) )
		m_szName = kSlot.m_Name;
	else
	{
		HideWnd();
		return false;
	}

	if( !IsShow() )
		ShowWnd();

	return true;
}

void CostumeCustomChangeConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{	
			HideWnd();
			/*
			// 코스튬 관련 (현재 사용하지 않는 기능 협의)
			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_CUSTOM_COSTUME_SKIN_UNIQUE_INDEX );
			TCPNetwork::SendToServer( kPacket );
			*/
		}
		break;
	case ID_CANCEL:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void CostumeCustomChangeConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 53, FONT_SIZE_17, "%s", m_szName.c_str() );
	if( m_bMale )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, "남성" );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, "여성" );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 101, FONT_SIZE_13, "스킨을 변경하시겠습니까?" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 128, FONT_SIZE_13, "＊부적절한 이미지를 사용한 경우" );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 146, FONT_SIZE_13, "사전 통보없이 관리자에 의해" );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, "제재 및 변경 될 수 있습니다" );
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 삭제 창)
CostumeCustomDeleteWnd::CostumeCustomDeleteWnd()
{
	m_iSlotIndex = -1;
	m_pPreEdit   = NULL;
}

CostumeCustomDeleteWnd::~CostumeCustomDeleteWnd()
{

}

void CostumeCustomDeleteWnd::SetSlotIndex( int iSlotIndex )
{
	m_iSlotIndex = iSlotIndex;

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( m_iSlotIndex, kSlot ) )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "존재하지 않는 코스튬입니다" );
	else
	{
		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			SetChildActive( ID_MALE_DELETE );
			SetChildActive( ID_FEMALE_DELETE );

			if( !IsShow() )
				ShowWnd();
		}
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			SetChildActive( ID_MALE_DELETE );
			SetChildInActive( ID_FEMALE_DELETE );

			if( !IsShow() )
				ShowWnd();

		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			SetChildInActive( ID_MALE_DELETE );
			SetChildActive( ID_FEMALE_DELETE );

			if( !IsShow() )
				ShowWnd();
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "스킨이 적용되지 않은 코스튬입니다" );
		}		
	}
}

void CostumeCustomDeleteWnd::iwm_show()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_DELETE_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		m_pPreEdit = ioEdit::m_pKeyFocusEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );
		pEdit->KillKeyFocus();
	}
}

void CostumeCustomDeleteWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void CostumeCustomDeleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_MALE_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			ioEdit *pEdit = dynamic_cast< ioEdit * >( FindChildWnd( ID_DELETE_EDIT ) );
			if( !pEdit ) return;
			if( strcmp( pEdit->GetText(), "코스튬스킨삭제" ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "[코스튬스킨삭제]를 입력하고 삭제를 누르세요" );
				return;
			}
			/*
			// 코스튬 관련 (현재 사용하지 않는 기능 협의)
			TCPNetwork::MouseBusy( true );

			SP2Packet kPacket( CTPK_CUSTOM_COSTUME_SKIN_DELETE );
			kPacket << m_iSlotIndex;
			kPacket << SDT_MALE;
			TCPNetwork::SendToServer( kPacket );
			*/
			HideWnd();
		}
		break;
	case ID_FEMALE_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			ioEdit *pEdit = dynamic_cast< ioEdit * >( FindChildWnd( ID_DELETE_EDIT ) );
			if( !pEdit ) return;
			if( strcmp( pEdit->GetText(), "코스튬스킨삭제" ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "[코스튬스킨삭제]를 입력하고 삭제를 누르세요" );
				return;
			}
			/*
			// 코스튬 관련 (현재 사용하지 않는 기능 협의)
			TCPNetwork::MouseBusy( true );

			SP2Packet kPacket( CTPK_CUSTOM_COSTUME_SKIN_DELETE );
			kPacket << m_iSlotIndex;
			kPacket << SDT_FEMALE;
			TCPNetwork::SendToServer( kPacket );
			*/
			HideWnd();
		}
		break;
	case ID_DELETE_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = dynamic_cast<ioEdit*>(pWnd);
				pEdit->SetKeyFocus();
			}
		}
		break;
	}
}

void CostumeCustomDeleteWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;
}

void CostumeCustomDeleteWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 53, FONT_SIZE_17, "코스스킨을" );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, "삭제하시겠습니까?" );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_12, "[코스튬스킨삭제]를 입력하세요" );
	if( FindChildWnd( ID_DELETE_EDIT ) != ioEdit::m_pKeyFocusEdit )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 201, FONT_SIZE_12, "코스튬스킨삭제 입력하세요" );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "*적용된 스킨이 삭제되면" );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 98, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, "원본 스킨" );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "으로 돌아옵니다" );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 115, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "*" );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, "남성" );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "과" );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 여성" );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, " 스킨 모두 적용시" );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 132, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "따로 따로 삭제가 가능합니다" );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 149, TAT_CENTER );
	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨 ??)
CostumeCustomGlobalDetailViewWnd::CostumeCustomGlobalDetailViewWnd()
{
	m_pUI3DRender = NULL;
	m_CustomChar1 = NULL;
	m_CustomChar2 = NULL;

	m_pTitleBar   = NULL;
	m_pBackground = NULL;
	m_pMouseLeft  = NULL;	
	m_pMouseRight = NULL;
	m_pMouseMiddle= NULL;
}

CostumeCustomGlobalDetailViewWnd::~CostumeCustomGlobalDetailViewWnd()
{
	SAFEDELETE( m_pTitleBar );
	SAFEDELETE( m_pBackground );
	SAFEDELETE( m_pMouseLeft );
	SAFEDELETE( m_pMouseRight );
	SAFEDELETE( m_pMouseMiddle );
	SAFEDELETE( m_CustomChar1 );
	SAFEDELETE( m_CustomChar2 );
	SAFEDELETE( m_pUI3DRender );
}

void CostumeCustomGlobalDetailViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "TitleBar" )
	{
		SAFEDELETE( m_pTitleBar );
		m_pTitleBar = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CostumeCustomGlobalDetailViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Background" )
	{
		SAFEDELETE( m_pBackground );
		m_pBackground = pImage;
	}
	else if( szType == "MouseLeft" )
	{
		SAFEDELETE( m_pMouseLeft );
		m_pMouseLeft = pImage;
	}
	else if( szType == "MouseRight" )
	{
		SAFEDELETE( m_pMouseRight );
		m_pMouseRight = pImage;
	}
	else if( szType == "MouseMiddle" )
	{
		SAFEDELETE( m_pMouseMiddle );
		m_pMouseMiddle = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CostumeCustomGlobalDetailViewWnd::CreateCharacter( const CostumeEquipInfo &sCostumeEquipInfo )
{
	CostumeInfo sCostumeInfo;
	if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeEquipInfo.m_CostumeCode, sCostumeInfo ) )
		return;

	HideWnd();

	SAFEDELETE( m_CustomChar1 );
	SAFEDELETE( m_CustomChar2 );
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );
	
	int nSubType = (sCostumeEquipInfo.m_CostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
	DWORD dwTempItemCode = 0;
	switch ( nSubType )
	{
		/*
	case ioItem::IT_WEAPON:
		dwTempItemCode = 1;
		break;
		*/
	case ioItem::IT_ARMOR:
		dwTempItemCode = 100001;
		break;
	case ioItem::IT_HELMET:
		dwTempItemCode = 200001;
		break;
	case ioItem::IT_CLOAK:
		dwTempItemCode = 300001;
		break;
	}

	if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 && sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		// 남성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		
		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( -50.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );
		m_CustomChar1->RotateY( -11.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid_2people.txt", NULL );

		// 여성
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷

		m_CustomChar2 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar2 )
			return;

		m_CustomChar2->SetPosition( 50.0f, -80.0f, 953.0f );
		m_CustomChar2->SetYaw( FLOAT10 );

		m_CustomChar2->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );
		m_CustomChar2->RotateY( -11.0f );
		m_CustomChar2->SetMannequins( false );
	}
	else if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 )
	{
		// 남성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		
		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( 0.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );
		m_CustomChar1->RotateY( -15.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid.txt", NULL );
	}
	else if( sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		// 여성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		
		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( 0.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );
		m_CustomChar1->RotateY( -15.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid.txt", NULL );
	}


	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );
	float fAspect = (float)(VIEW_WIDTH)/(float)(VIEW_HEIGHT);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 45.0f, fAspect );	
		
	if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 && sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
		SafeSprintf( m_szSkinName, sizeof(m_szSkinName), "%s (남성,여성)", sCostumeInfo.m_szName.c_str() );
	else if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 )
		SafeSprintf( m_szSkinName, sizeof(m_szSkinName), "%s (남성)", sCostumeInfo.m_szName.c_str() );
	else if( sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
		SafeSprintf( m_szSkinName, sizeof(m_szSkinName), "%s (여성)", sCostumeInfo.m_szName.c_str() );

	switch( nSubType )
	{
		/*
	case ioItem::IT_WEAPON:
		{
			if( m_CustomChar1 )
				m_CustomChar1->SetLoopAni( "custom_pose_01.ani" );
			if( m_CustomChar2 )
				m_CustomChar2->SetLoopAni( "custom_pose_01.ani" );				
		}
		break;
		*/
	case ioItem::IT_ARMOR:
		{
			if( m_CustomChar2 )
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_01.ani" );
				if( m_CustomChar2 )
					m_CustomChar2->SetLoopAni( "custom_pose_01.ani" );
			}
			else
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_02.ani" );
			}
		}
		break;
	case ioItem::IT_HELMET:
		{
			if( m_CustomChar2 )
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_01.ani" );
				if( m_CustomChar2 )
					m_CustomChar2->SetLoopAni( "custom_pose_01.ani" );
			}
			else
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_02.ani" );
			}
		}
		break;
	case ioItem::IT_CLOAK:
		{
			if( m_CustomChar2 )
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_01.ani" );
				if( m_CustomChar2 )
					m_CustomChar2->SetLoopAni( "custom_pose_01.ani" );
			}
			else
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_02.ani" );
			}
		}
		break;
	}		
	ShowWnd();
}

void CostumeCustomGlobalDetailViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	}
}

void CostumeCustomGlobalDetailViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( GetParent() )
	{
		if( !GetParent()->IsShow() ) return;
	}

	if( m_pUI3DRender )
	{
		if( ioWnd::m_pTopModalWnd == this )
		{
			m_pUI3DRender->UpdateCameraByMouse( 20.0f, 20.0f, 4.0f );
		}
	}

	if( m_CustomChar1 )
		m_CustomChar1->Update( fTimePerSec );
	if( m_CustomChar2 )
		m_CustomChar2->Update( fTimePerSec );
}

void CostumeCustomGlobalDetailViewWnd::OnRenderBackground( int iXPos, int iYPos )
{
	if( !m_pBackground ) return;

	int iWidthCnt = 750 / m_pBackground->GetWidth();
	for(int i = 0;i < iWidthCnt;i++)
	{
		m_pBackground->Render( iXPos + 2 + (m_pBackground->GetWidth() * i), iYPos + 6 );
	}
}

void CostumeCustomGlobalDetailViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRenderBackground( iXPos, iYPos );

	if( m_pTitleBar )
		m_pTitleBar->Render( iXPos, iYPos );

	if( m_pMouseLeft )
		m_pMouseLeft->Render( iXPos + 22, iYPos + 59 );
	if( m_pMouseRight )
		m_pMouseRight->Render( iXPos + 54, iYPos + 59 );
	if( m_pMouseMiddle )
		m_pMouseMiddle->Render( iXPos + 86, iYPos + 59 );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_18, "스킨 파일 자세히보기 : " );

	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( 255, 188, 26 );
	kPrinter.AddTextPiece( FONT_SIZE_18, "%s", m_szSkinName );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 36, iYPos + 106, FONT_SIZE_12, "이동" );
	g_FontMgr.PrintText( iXPos + 68, iYPos + 106, FONT_SIZE_12, "회전" );
	g_FontMgr.PrintText( iXPos + 100, iYPos + 106, FONT_SIZE_12, "확대" );

	if( m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 2;
		rcD3D.y1 = iYPos + 39;
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
	}
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨한것 미리보기 캐릭터)
CostumeCustomThumbnailWnd::CostumeCustomThumbnailWnd()
{
	m_pBlueStateFrm   = NULL;
	m_pRedStateFrm   = NULL;
	m_pRectSrc    = NULL;
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;
	m_dwRotateCheckTime = 0;
}

CostumeCustomThumbnailWnd::~CostumeCustomThumbnailWnd()
{
	SAFEDELETE( m_pBlueStateFrm );
	SAFEDELETE( m_pRedStateFrm );
	SAFEDELETE( m_pRectSrc );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void CostumeCustomThumbnailWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BlueStateFrm" )
	{
		SAFEDELETE( m_pBlueStateFrm );
		m_pBlueStateFrm = pFrame;
	}
	else if( szType == "RedStateFrm" )
	{
		SAFEDELETE( m_pRedStateFrm );
		m_pRedStateFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void CostumeCustomThumbnailWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "RectSrc" )
	{
		SAFEDELETE( m_pRectSrc );
		m_pRectSrc = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CostumeCustomThumbnailWnd::CreateCharacter( const CostumeEquipInfo &sCostumeEquipInfo, bool bMale )
{
	if( IsShow() )
		return;

	CostumeInfo sCostumeInfo;
	if ( !g_CostumeInfoMgr.GetCostumeInfo( sCostumeEquipInfo.m_CostumeCode, sCostumeInfo ) )
		return;

	CHARACTER kCharInfo;
	kCharInfo.m_class_type = 1;
	kCharInfo.m_skin_color = 9;		//회색
	kCharInfo.m_underwear  = 1;     //기본속옷
	if( bMale )
	{
		kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		kCharInfo.m_hair       = 1;
	}
	else
	{
		kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_hair       = 4;
	}
	
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );

	int nSubType = (sCostumeEquipInfo.m_CostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
	DWORD dwTempItemCode = 0;
	switch ( nSubType )
	{
		/*
	case ioItem::IT_WEAPON:
		dwTempItemCode = 1;
		break;
		*/
	case ioItem::IT_ARMOR:
		dwTempItemCode = 100001;
		break;
	case ioItem::IT_HELMET:
		dwTempItemCode = 200001;
		break;
	case ioItem::IT_CLOAK:
		dwTempItemCode = 300001;
		break;
	}

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
	if( !pChar )
		return;

	pChar->SetPosition( 0.0f, -80.0f, 950.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );

	float fAspect = (float)(GetWidth() - 2)/(float)(GetHeight() - 2);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 20.0f, fAspect );

	pChar->EquipItem( dwTempItemCode, sCostumeEquipInfo.m_Costume_Male_Custom, sCostumeEquipInfo.m_Costume_Female_Custom, sCostumeEquipInfo.m_CostumeCode );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;
	
	m_aMyChar->SetMannequins( true );
	m_aMyChar->SetLoopAni( "custom_pose_01.ani" );

	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{			
		switch( nSubType )
		{
		case ioItem::IT_WEAPON:
			{
				m_pUI3DRender->SetCameraPos( -110.0f, 20.0f, 665.0f );
			}
			break;
		case ioItem::IT_ARMOR:
			{
				m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 650.0f );
			}
			break;
		case ioItem::IT_HELMET:
			{
				pChar->SetPosition( 0.0f, -130.0f, 950.0f );
				m_pUI3DRender->SetCameraPos( -110.0f, -20.0f, 710.0f );
			}
			break;
		case ioItem::IT_CLOAK:
			{
				m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 1350.0f );
			}
			break;
		}		
	}

	m_dwRotateCheckTime = FRAMEGETTIME();

	ShowWnd();
}

void CostumeCustomThumbnailWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
	if( GetParent() )
	{
		if( !GetParent()->IsShow() ) return;
	}

	if( m_aMyChar )
	{
		if( FRAMEGETTIME() - m_dwRotateCheckTime > 2000 )
		{
			m_aMyChar->RotateY( 80.0f * fTimePerSec );
		}
		m_aMyChar->Update( fTimePerSec );
	}
}

void CostumeCustomThumbnailWnd::OnRectRender( int iXPos, int iYPos )
{
	if( !m_pRectSrc ) return;

	int iWidthCnt = GetWidth() / RECT_GAP_SIZE;
	for(int i = 0;i < RECT_HEIGHT_CNT;i++)
	{
		for(int j = 0;j < iWidthCnt;j++)
		{
			m_pRectSrc->Render( iXPos + (j * RECT_GAP_SIZE), iYPos );
		}
		iYPos += RECT_GAP_SIZE;
	}
}

void CostumeCustomThumbnailWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	OnRectRender( iXPos, iYPos );

	if( m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 1;
		rcD3D.y1 = iYPos + 1;
		rcD3D.x2 = rcD3D.x1 + (GetWidth()-2);
		rcD3D.y2 = rcD3D.y1 + (GetHeight()-2);
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
	}

	if( m_aMyChar )
	{
		if( m_aMyChar->IsMale() )
		{
			if( m_pBlueStateFrm )
			{
				m_pBlueStateFrm->Render( iXPos, iYPos );

				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, "남성" );
			}
		}
		else
		{
			if( m_pRedStateFrm )
			{
				m_pRedStateFrm->Render( iXPos, iYPos );

				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, "여성" );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// UI코스튬 관련 (코스튬 스킨한것 미리보기 툴팁)
CostumeCustomThumbnailTooltip::CostumeCustomThumbnailTooltip()
{
}

CostumeCustomThumbnailTooltip::~CostumeCustomThumbnailTooltip()
{
}

void CostumeCustomThumbnailTooltip::ClearList()
{
	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; ++i )
	{
		m_Printer[i].ClearList();
	}
	m_CostumeEquipInfo.Init();
}

void CostumeCustomThumbnailTooltip::iwm_hide()
{
	HideChildWnd( ID_CUSTOM_1 );	
	HideChildWnd( ID_CUSTOM_2 );
}

void CostumeCustomThumbnailTooltip::SetTooltipText( ioComplexStringPrinter *rkPrinter, const CostumeEquipInfo &sCostumeEquipInfo )
{
	ClearList();

	m_CostumeEquipInfo = sCostumeEquipInfo;
	if( m_CostumeEquipInfo.m_Costume_Male_Custom == 0 && m_CostumeEquipInfo.m_Costume_Female_Custom == 0 )
	{
		HideWnd();
		return;
	}

	int i = 0;
	int iMaxLine = 0;
	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		if( rkPrinter[i].IsEmpty() )
			break;

		rkPrinter[i].CopyFullText( m_Printer[i] );
		iMaxLine++;
	}

	if( iMaxLine == 0 )
	{
		HideWnd();
		return;
	}

	ShowWnd();

	int iMaxWidth  = THUMBNAIL_WIDTH;
	if( m_CostumeEquipInfo.m_Costume_Male_Custom > 0 && m_CostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		iMaxWidth += THUMBNAIL_WIDTH + 4;
	}
	else
	{
		iMaxWidth = THUMBNAIL_WIDTH + 42;
	}

	int iMaxHeight = MIN_HEIGHT + 8;
	iMaxHeight += ( TEXT_LINE_GAP * (iMaxLine - 1) );

	SetSize( iMaxWidth, THUMBNAIL_HEIGHT + iMaxHeight );
	ProcessPosition();

	if( m_CostumeEquipInfo.m_Costume_Male_Custom > 0 && m_CostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		CostumeCustomThumbnailWnd *pCustom1 = dynamic_cast< CostumeCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 182, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_CostumeEquipInfo, true );
		}

		CostumeCustomThumbnailWnd *pCustom2 = dynamic_cast< CostumeCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_2 ) );
		if( pCustom2 )
			pCustom2->CreateCharacter( m_CostumeEquipInfo, false );
	}
	else if( m_CostumeEquipInfo.m_Costume_Male_Custom > 0 )
	{
		CostumeCustomThumbnailWnd *pCustom1 = dynamic_cast< CostumeCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 217, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_CostumeEquipInfo, true );
		}
	}
	else if( m_CostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		CostumeCustomThumbnailWnd *pCustom1 = dynamic_cast< CostumeCustomThumbnailWnd* >( FindChildWnd( ID_CUSTOM_1 ) );
		if( pCustom1 )
		{
			pCustom1->SetSize( 217, pCustom1->GetHeight() );
			pCustom1->CreateCharacter( m_CostumeEquipInfo, false );
		}
	}
}

void CostumeCustomThumbnailTooltip::ProcessPosition()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( pMouse )
	{
		int iOffSetXY = 8;
		int iXPos = pMouse->GetMousePos().x;
		int iYPos = pMouse->GetMousePos().y;

		if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
			iXPos -= ( GetWidth() + iOffSetXY );
		else 
			iXPos += iOffSetXY;

		if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
			iYPos -= ( GetHeight() + iOffSetXY );
		else 
			iYPos += iOffSetXY;

		if( GetParent() )
		{
			iXPos -= GetParent()->GetDerivedPosX();
			iYPos -= GetParent()->GetDerivedPosY();
		}

		SetWndPos( iXPos, iYPos );
		pMouse->SetToolTipCursor();
	}
}

void CostumeCustomThumbnailTooltip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	//
	ProcessPosition();
}

void CostumeCustomThumbnailTooltip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;;

	for( int i=0; i < GLOBAL_TEXT_TOOLTIP_LINE; i++ )
	{
		if( m_Printer[i].IsEmpty() )
			break;

		m_Printer[i].PrintFullText( iXPos, iYPos + TEXT_LINE_GAP*i, TAT_LEFT );
	}
}