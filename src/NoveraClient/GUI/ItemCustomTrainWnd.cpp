#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3dengine/ioUITitle.h"
#include "../io3DEngine/ioTexture.h"
#include "../io3DEngine/ioTextureManager.h"
#include "../io3Dengine/ioResourceLoader.h"

#include "../WndID.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"

#include "../ioMyLevelMgr.h"
#include "../ioClassPrice.h"

#include "../EtcHelpFunc.h"
#include "../TextColorTable.h"

#include "../ioPlayStage.h"

#include "../ioBaseChar.h"
#include "../ioCreateChar.h"
#include "../ioSkillMaker.h"

#include "../ioGrowthLevel.h"
#include "../ioGrowthUpInfo.h"
#include "../ioGrowthInfoManager.h"

#include "../ioItem.h"
#include "../ioItemMaker.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"

#include "../ioWeaponItem.h"
#include "../ioArmorItem.h"
#include "../ioHelmetItem.h"
#include "../ioCloakItem.h"

#include "../ioUserEtcItem.h"
#include "../ioEtcItemManager.h"
#include "../ioUserExtraItem.h"
#include "../ioExtraItemInfoManager.h"

#include "../ioPresentMgr.h"
#include "../ioDecorationPrice.h"

#include "../NetworkWrappingFunc.h"
#include "../ioMedalItemInfoManager.h"

#include "../ioQuestManager.h"

#include "../ioCustomTextureMgr.h"
#include "../ChatMacroMgr.h"
#include "../ioPlayMode.h"
#include "../ioMyRoomMode.h"

#include "..\Local\ioLocalManager.h"
#include "..\Local\ioLocalParent.h"

#include "ioUI3DRender.h"
#include "MySoldierWnd.h"
#include "ioSP2GUIManager.h"
#include "GlobalWnd.h"
#include "FileListWnd.h"
#include "NewShopWnd.h"
#include "MyInventoryWnd.h"
#include "MyInventoryInfoWnd.h"

#include "ChangeTradeStateWnd.h"
#include "ItemCustomTrainWnd.h"
#include "ItemCustomWnd.h"


#include <shlobj.h>
#include <strsafe.h>
#include "../Local/ioLocalManager.h"


//////////////////////////////////////////////////////////////////////////
ItemCustomTrainIconBtn::ItemCustomTrainIconBtn()
{
	m_pIcon       = NULL;
	m_pIconBack   = NULL;

	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_nGradeType = 0;
}

ItemCustomTrainIconBtn::~ItemCustomTrainIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void ItemCustomTrainIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void ItemCustomTrainIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void ItemCustomTrainIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void ItemCustomTrainIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void ItemCustomTrainIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void ItemCustomTrainIconBtn::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackScale );

		if( m_fBackScale != FLOAT1 )
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void ItemCustomTrainIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		if ( eNewType != UI_RENDER_SCREEN )
			g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType );
	}
}

bool ItemCustomTrainIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale )
{
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		return false;
	}

	m_pIcon = pIcon;
	return true;
}

void ItemCustomTrainIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_pParent )
		return;
}
//////////////////////////////////////////////////////////////////////////
ItemCustomTrainInfoWnd::ItemCustomTrainInfoWnd()
{
	m_pEmptyImg = NULL;

	m_dwItemCode = 0;
}

ItemCustomTrainInfoWnd::~ItemCustomTrainInfoWnd()
{
	SAFEDELETE(m_pEmptyImg);
}

void ItemCustomTrainInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_MALE_BTN:
	case ID_FEMALE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void ItemCustomTrainInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomTrainInfoWnd::OnRender()
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
		ioButton::OnRender();

	if( m_dwItemCode == 0 )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 52, iYPos + 105, FONT_SIZE_13, STR(1) );
		g_FontMgr.PrintText( iXPos + 52, iYPos + 121, FONT_SIZE_13, "-" );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintTextWidthCut( iXPos + 52, iYPos + 105, FONT_SIZE_13,FLOAT90, STR(2), m_szItemName.c_str() );

		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 52, iYPos + 121, FONT_SIZE_13, STR(3) );
	}
}

void ItemCustomTrainInfoWnd::SetInfo( DWORD dwItemCode )
{
	m_dwItemCode = dwItemCode;

	if( m_dwItemCode == 0 )
	{
		SetInActive();
		m_szItemName.Clear();

		HideChildWnd( ID_ICON_BTN );
		HideChildWnd( ID_MALE_BTN );
		HideChildWnd( ID_FEMALE_BTN );
	}
	else
	{
		SetActive();
		m_szItemName = g_CustomTextureMgr.GetTrainExtraItemName( m_dwItemCode );

		ShowChildWnd( ID_ICON_BTN );
		ShowChildWnd( ID_MALE_BTN );
		ShowChildWnd( ID_FEMALE_BTN );

		ItemCustomTrainIconBtn *pBtn = dynamic_cast< ItemCustomTrainIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
		if( !pBtn )
		{
			LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
		}
		else
		{
			ioUIRenderImage *pImg = NULL;
			pImg = g_UIImageSetMgr.CreateImageByFullName( g_CustomTextureMgr.GetTrainExtraItemIcon( m_dwItemCode ) );
			pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
			
			if ( m_dwItemCode > 0)
			{
				const ioItem *pItem = g_ItemMaker.GetItemConst( m_dwItemCode, __FUNCTION__ );
				if( pItem )
					pBtn->SetGradeType( pItem->GetGradeType() );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

ItemCustomTrainSelectWnd::ItemCustomTrainSelectWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

ItemCustomTrainSelectWnd::~ItemCustomTrainSelectWnd()
{
	m_dwItemCodeList.clear();
}

void ItemCustomTrainSelectWnd::iwm_create()
{
}

void ItemCustomTrainSelectWnd::iwm_show()
{
	g_GUIMgr.HideWnd( ITEM_CUSTOM_SELECT_WND );
	g_GUIMgr.HideWnd( COSTUME_CUSTOM_SELECT_WND );

	ChangeTab( ID_TAB_1_BTN );
}

void ItemCustomTrainSelectWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
}

void ItemCustomTrainSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void ItemCustomTrainSelectWnd::iwm_wheel( int zDelta )
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

void ItemCustomTrainSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			ItemCustomTrainInfoWnd *pItem = dynamic_cast<ItemCustomTrainInfoWnd*>(FindChildWnd(dwID));
			if( pItem )
			{
				ItemCustomTrainMakeWnd *pMakeWnd = dynamic_cast<ItemCustomTrainMakeWnd*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_TRAIN_MAKE_WND ));
				if( pMakeWnd )
				{
					if( param == ItemCustomTrainInfoWnd::ID_MALE_BTN )
					{	
						pMakeWnd->SetExtraItemInfo( pItem->GetItemCode(), true );
					}
					else if( param == ItemCustomTrainInfoWnd::ID_FEMALE_BTN )
					{	
						pMakeWnd->SetExtraItemInfo( pItem->GetItemCode(), false );							
					}
				}
			}
		}
		break;
	}
}

void ItemCustomTrainSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_dwItemCodeList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemCustomTrainInfoWnd *pItem = dynamic_cast<ItemCustomTrainInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			pItem->SetInfo( 0 );
			continue;
		}

		pItem->SetInfo( m_dwItemCodeList[iCurArray] );
	}
}

void ItemCustomTrainSelectWnd::ChangeTab( DWORD dwID )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
	m_dwCurTabID = dwID;

	SetExtraItemInfoVec( m_dwCurTabID );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_dwItemCodeList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void ItemCustomTrainSelectWnd::SetExtraItemInfoVec( DWORD dwTabID )
{
	m_dwItemCodeList.clear();

	int iSize = g_CustomTextureMgr.GetMaxTrainExtraItem();
	for(int i = 0;i < iSize;i++)
	{
		DWORD dwItemCode = g_CustomTextureMgr.GetTrainExtraItem( i );

		int iGroup = dwItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightTab( dwTabID, iGroup ) )
			continue;

		m_dwItemCodeList.push_back( dwItemCode );
	}
}

void ItemCustomTrainSelectWnd::OnRender()
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
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(1) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+18, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(11) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(12) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+36, TAT_LEFT );
	kPrinter.ClearList();
}

void ItemCustomTrainSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

int ItemCustomTrainSelectWnd::GetTabType( DWORD dwTabID )
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

bool ItemCustomTrainSelectWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void ItemCustomTrainSelectWnd::Update()
{
	UpdatePage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
ItemCustomTrainMakeWnd::ItemCustomTrainMakeWnd()
{
	m_bMale = true;
	m_TexSelectCount = 0;

	m_dwItemCode = 0;
	m_dwHelpID   = 0;
}

ItemCustomTrainMakeWnd::~ItemCustomTrainMakeWnd()
{
}

void ItemCustomTrainMakeWnd::iwm_create()
{
}

void ItemCustomTrainMakeWnd::iwm_show()
{
	g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
}

void ItemCustomTrainMakeWnd::iwm_hide()
{
}

void ItemCustomTrainMakeWnd::AfterViewLSCTexture( int iSelectCount )
{
	ItemCustomMake3DViewWnd *pAfter3D = dynamic_cast< ItemCustomMake3DViewWnd * >( FindChildWnd( ID_AFTER_VIEW ) );
	if( pAfter3D )
	{
		pAfter3D->ChangeLSCTexture( m_OriginalTexList, m_AfterTexList );
	}

	m_TexSelectCount = iSelectCount;
	char szBtnTitle[MAX_PATH] = "";
	SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(1), m_TexSelectCount, (int)m_OriginalTexList.size() );
	ioWnd *pUploadBtn = FindChildWnd( ID_UPLOAD_FILE );
	if( pUploadBtn )
	{
		pUploadBtn->SetTitleText( szBtnTitle );
		pUploadBtn->SetActive();
	}
}

void ItemCustomTrainMakeWnd::SetExtraItemInfo( DWORD dwItemCode, bool bMale )
{
	if( dwItemCode == 0 )
		return;

	m_dwItemCode = dwItemCode;
	m_bMale		 = bMale;
	m_OriginalTexList.clear();
	m_AfterTexList.clear();
	m_TexSelectCount = 0;

	const ioHashString &rkName = g_CustomTextureMgr.GetTrainExtraItemName( m_dwItemCode );
	
	m_szItemName = rkName;
	m_ItemNameMent.ClearList();
	m_ItemNameMent.SetTextStyle( TS_NORMAL );
	m_ItemNameMent.SetBkColor( 0, 0, 0 );	
	m_ItemNameMent.SetTextColor( TCT_DEFAULT_GRAY );
	m_ItemNameMent.AddTextPiece( FONT_SIZE_17, STR(1), m_szItemName.c_str() );
	if( m_bMale )
	{
		m_ItemNameMent.SetTextColor( TCT_DEFAULT_BLUE );
		m_ItemNameMent.AddTextPiece( FONT_SIZE_17, STR(2) );
	}
	else
	{
		m_ItemNameMent.SetTextColor( TCT_DEFAULT_RED );
		m_ItemNameMent.AddTextPiece( FONT_SIZE_17, STR(3) );
	}	

	// Before 세팅
	ItemCustomMake3DViewWnd *pBefore3D = dynamic_cast< ItemCustomMake3DViewWnd * >( FindChildWnd( ID_BEFORE_VIEW ) );
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

		ITEM_DATA kItemData;
		kItemData.m_item_code = m_dwItemCode;

		pBefore3D->CreateCharacter( kCharInfo, kItemData, m_OriginalTexList );		
	}

	if( (int)m_OriginalTexList.empty() )
	{
		if( m_bMale )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), m_szItemName.c_str() );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), m_szItemName.c_str() );
		}
		return;
	}

	// After 세팅
	ItemCustomMake3DViewWnd *pAfter3D = dynamic_cast< ItemCustomMake3DViewWnd * >( FindChildWnd( ID_AFTER_VIEW ) );
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

		ITEM_DATA kItemData;
		kItemData.m_item_code = m_dwItemCode;

		pAfter3D->CreateCharacter( kCharInfo, kItemData, m_AfterTexList );
	}

	// 
	m_TextureCountMent.ClearList();
	m_TextureCountMent.SetTextStyle( TS_NORMAL );
	m_TextureCountMent.SetBkColor( 0, 0, 0 );	
	m_TextureCountMent.SetTextColor( TCT_DEFAULT_ORANGE );
	m_TextureCountMent.AddTextPiece( FONT_SIZE_13, STR(6), (int)m_OriginalTexList.size() );

	char szBtnTitle[MAX_PATH] = "";
	SafeSprintf( szBtnTitle, sizeof( szBtnTitle ), STR(7), (int)m_OriginalTexList.size() );
	ioWnd *pUploadBtn = FindChildWnd( ID_UPLOAD_FILE );
	if( pUploadBtn )
	{
		pUploadBtn->SetTitleText( szBtnTitle );
		pUploadBtn->SetActive();
	}

	ShowWnd();
}

void ItemCustomTrainMakeWnd::UploadTexture()
{
	if( m_OriginalTexList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
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

void ItemCustomTrainMakeWnd::DownloadTexture()
{
	if( m_OriginalTexList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	char szDesktopPath[MAX_PATH] = "";
	SHGetSpecialFolderPath( g_App.GetHWnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );

	char szSavePath[MAX_PATH] = "";
	SafeSprintf( szSavePath, sizeof( szSavePath ), STR(2), szDesktopPath );
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
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), rkData.m_szTextureName.c_str() );
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
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), rkData.m_szTextureName.c_str() );

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

void ItemCustomTrainMakeWnd::ShowDetailView()
{
	if( m_dwItemCode == 0 ) 
		return;
	
	// 자세히보기 세팅
	ItemCustomMakeDetailViewWnd *pDetailView = dynamic_cast< ItemCustomMakeDetailViewWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_DETAIL_VIEW_WND ) );
	if( pDetailView )
	{
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		if( m_bMale )
		{
			kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		}
		else
		{
			kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		}
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		//kCharInfo.m_underwear  = 14;	//속옷 제거

		ITEM_DATA kItemData;
		kItemData.m_item_code = m_dwItemCode;

		pDetailView->CreateCharacter( kCharInfo, kItemData, m_AfterTexList );
		pDetailView->ShowWnd();
	}
}

bool ItemCustomTrainMakeWnd::iwm_esc()
{
	return ioWnd::iwm_esc();
}

void ItemCustomTrainMakeWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwHelpID = xElement.GetIntAttribute_e( "HelpID" );
}

void ItemCustomTrainMakeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_HELP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_dwHelpID );
		}
		break;
	case ID_SELECT_ITEM:
		if( cmd == IOBN_BTNUP )
		{	
			HideWnd();
			g_GUIMgr.ShowWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
		}
		break;
	case ID_UPLOAD_FILE:
		if( cmd == IOBN_BTNUP )
		{
			UploadTexture();
		}
		break;
	case ID_DOWNLOAD_FILE:
		if( cmd == IOBN_BTNUP )
		{
			DownloadTexture();		
		}
		break;
	case ID_DETAIL_VIEW:
		if( cmd == IOBN_BTNUP )
		{
			ShowDetailView(); 
		}
		break;
	case ID_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( ITEM_CUSTOM_WANT_ETCITEM_WND );			
		}
		break;
	}
}

void ItemCustomTrainMakeWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	g_FontMgr.PrintText( iXPos + 92, iYPos + 378, FONT_SIZE_12, STR(1) );
	g_FontMgr.PrintText( iXPos + 92, iYPos + 393, FONT_SIZE_12, STR(2) );

	g_FontMgr.PrintText( iXPos + 245, iYPos + 378, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 245, iYPos + 393, FONT_SIZE_12, STR(4) );

	g_FontMgr.PrintText( iXPos + 398, iYPos + 378, FONT_SIZE_12, STR(5) );
	g_FontMgr.PrintText( iXPos + 398, iYPos + 393, FONT_SIZE_12, STR(6) );

	m_ItemNameMent.PrintFullText( iXPos + 246, iYPos + 294, TAT_CENTER );
	m_TextureCountMent.PrintFullText( iXPos + 246, iYPos + 315, TAT_CENTER );
}

void ItemCustomTrainMakeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
}
//////////////////////////////////////////////////////////////////////////
ItemCustomWantEtcItemWnd::ItemCustomWantEtcItemWnd()
{
	m_pIconBack	  = NULL;
	m_pIcon		  = NULL;
}

ItemCustomWantEtcItemWnd::~ItemCustomWantEtcItemWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void ItemCustomWantEtcItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "EtcIcon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ItemCustomWantEtcItemWnd::iwm_show()
{
	ioUserEtcItem::ETCITEMSLOT kEtcItem;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem || !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN, kEtcItem ) )
	{
		HideChildWnd( ID_INVENTORY );
		ShowChildWnd( ID_SHOP );
	}
	else
	{
		ShowChildWnd( ID_INVENTORY );
		HideChildWnd( ID_SHOP );
	}
}

void ItemCustomWantEtcItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			// 아이템 보기
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_EXTRA_TAB_BTN, ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN, 0 );
			}
			HideWnd();
			g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_MAKE_WND );
		}
		break;
	case ID_INVENTORY:
		if( cmd == IOBN_BTNUP )
		{
			// 아이템 보기
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd )
			{
				pInvenWnd->ShowEtcTabDirect( ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN );
			}
			HideWnd();
			g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_MAKE_WND );
		}
		break;
	}
}

void ItemCustomWantEtcItemWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
	{
		int iNewXPos = iXPos + 103;
		int iNewYPos = iYPos + 98;
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
		if( m_pIcon )
		{
			m_pIcon->Render( iNewXPos, iNewYPos );
		}
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(1) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 149, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_17, "1" );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(2) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 170, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 199, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 217, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 235, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 253, TAT_CENTER );
	kPrinter.ClearList();
}