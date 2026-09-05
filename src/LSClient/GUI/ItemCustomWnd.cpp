#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/InputBox.h"
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
#include "ItemCustomWnd.h"
#include "ItemCustomTrainWnd.h"
#include "CostumeCustomWnd.h"

#include <shlobj.h>
#include <strsafe.h>
#include "../Local/ioLocalManager.h"


//////////////////////////////////////////////////////////////////////////
ItemCustomIconBtn::ItemCustomIconBtn()
{
	m_pIcon       = NULL;
	m_pIconBack   = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_pCostumeMark = NULL;

	m_pStateOrangeFrm = NULL;
	m_pStateGreenFrm  = NULL;
	m_pStateBlueFrm   = NULL;

	m_iMarkType = TRADE_MARK_TYPE_DISABLE;
	m_bEquip    = false;
	m_iNum = 0;
	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_bIsCostume = false;
	m_nGradeType = 0;
}

ItemCustomIconBtn::~ItemCustomIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pCostumeMark );

	SAFEDELETE( m_pStateOrangeFrm );
	SAFEDELETE( m_pStateGreenFrm );
	SAFEDELETE( m_pStateBlueFrm );
}

void ItemCustomIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void ItemCustomIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else if( szType == "StateBlueFrm" )
		{
			SAFEDELETE( m_pStateBlueFrm );
			m_pStateBlueFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void ItemCustomIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemCustomIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemCustomIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemCustomIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void ItemCustomIconBtn::RenderBack( int iXPos, int iYPos )
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

void ItemCustomIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
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

	// UI코스튬 관련 (코스튬 마크)
	if( m_bIsCostume && m_pCostumeMark )
	{
		m_pCostumeMark->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		if( m_fScale != FLOAT1 )
			m_pCostumeMark->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pCostumeMark->Render( iNewXPos, iNewYPos, eNewType );
	}
}

void ItemCustomIconBtn::RenderOtherInfo( int iXPos, int iYPos )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;


	// Mark
	int iMarkX = iNewXPos;
	int iMarkY = iNewYPos;

	int iTextX = iMarkX;
	int iTextY = iMarkY;

	char szName[MAX_PATH]="";
	if( m_iMarkType == TRADE_MARK_TYPE_ENABLE )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateGreenFrm )
			m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(1) );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
	}
	else if( m_iMarkType == TRADE_MARK_TYPE_WAIT )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateOrangeFrm )
			m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(2) );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
	}
	else if( m_iMarkType == TRADE_MARK_TYPE_DISABLE )
	{
		if( m_bEquip && m_pStateBlueFrm )
		{
			// 장착 중
			iMarkX = iNewXPos-45;
			iMarkY = iNewYPos-41;

			m_pStateBlueFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

			iTextX = iMarkX+27;
			iTextY = iMarkY+Y_TEXT_OFFSET;
			SafeSprintf( szName, sizeof( szName ), STR(3) );

			g_FontMgr.SetTextStyle( TS_BOLD );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
		}
		else
		{
			// 미 장착
		}
	}

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	if( m_pNumText && m_pPlusText && m_iNum > 0 )
	{
		int iCurPos = 0;
		int iDefaultPos = iNewXPos;

		int iCurYPos = iNewYPos+(REINFORCE_Y * m_fScale);

		int iTotalSize = 0;
		if( m_iNum < 10 )
			iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
		else if( m_iNum < 100 )
			iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
		else
			iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

		iCurPos = iDefaultPos - (iTotalSize / 2);

		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (PLUS_SIZE-NUM_GAP);

		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iNum, -NUM_GAP );
	}
}

bool ItemCustomIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int iNum )
{
	m_iNum = iNum;
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_bIsCostume = false;
	m_iMarkType = 0;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		m_iNum = 0;
		return false;
	}

	m_pIcon = pIcon;

	return true;
}

void ItemCustomIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_pParent )
		return;
}
//////////////////////////////////////////////////////////////////////////
ItemCustomInfoWnd::ItemCustomInfoWnd()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	m_pEmptyImg = NULL;
}

ItemCustomInfoWnd::~ItemCustomInfoWnd()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void ItemCustomInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void ItemCustomInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomInfoWnd::SetBlank()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_MALE1_BTN );
	HideChildWnd( ID_MALE2_BTN );
	HideChildWnd( ID_FEMALE_BTN );

	ClearDesc();
}

void ItemCustomInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool ItemCustomInfoWnd::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_INFO_BTN ) )
		return true;
	return false;
}

bool ItemCustomInfoWnd::IsIconOver()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_ICON_BTN ) )
		return true;
	return false;
}

void ItemCustomInfoWnd::OnRender()
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

void ItemCustomInfoWnd::RenderDesc( int iXPos, int iYPos )
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
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(1) );
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

void ItemCustomInfoWnd::SetInfo( InventoryItemInfo &rkInfo )
{
	SetBlank();

	ioHashString szName, szIcon;

	SetActive();

	HideChildWnd( ID_CUSTOM_INFO_BTN );
	m_ItemInfo = rkInfo;
	if( !SetExtraItemInfo() )
	{
		SetBlank();
		return;
	}
}

InventoryItemInfo& ItemCustomInfoWnd::GetInfo()
{
	return m_ItemInfo;
}

int ItemCustomInfoWnd::GetMarkType()
{
	ItemCustomIconBtn *pBtn = dynamic_cast< ItemCustomIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn ) return TRADE_MARK_TYPE_ENABLE;

	return pBtn->GetMarkType();
}

bool ItemCustomInfoWnd::SetExtraItemInfo()
{
	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	if( !pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
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
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );
		pBtn->SetEquip( kSlot.m_bCharEquip );
		switch( kSlot.m_iTradeState )
		{
		case EET_NORMAL:
			pBtn->SetMarkType( TRADE_MARK_TYPE_WAIT );
			break;
		case EET_DISABLE:
			pBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );
			break;
		case EET_ENABLE:
			pBtn->SetMarkType( TRADE_MARK_TYPE_ENABLE );
			break;
		}
	}

	m_iExtraItemIndex = kSlot.m_iIndex;

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
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(1), kSlot.m_Name.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, STR(2) );

	// Limit
	int iLimitLevel		  = kSlot.m_iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iEnableClass	  = 0;
	int iPowerUpItemGrade = ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode );

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if( pItem )
	{
		iEnableClass = pItem->GetEnableClass();

		if ( pBtn )
			pBtn->SetGradeType( pItem->GetGradeType() );
	}

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( iEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(5), g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade )  ) );
		//m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(3), iLimitLevel, g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade )  ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(4), iLimitLevel );

	// 클래스 제한이 있는 아이템은 해당 클래스에 여캐가 있어야지만 여성 가능
	if( iEnableClass > 0 )
	{
		if( g_MyInfo.IsSlotItem( (iEnableClass * 100000) + UID_KINDRED, RDT_HUMAN_WOMAN ) )
		{
			ShowChildWnd( ID_MALE2_BTN );
			ShowChildWnd( ID_FEMALE_BTN );
		}
		else
		{
			ShowChildWnd( ID_MALE1_BTN );
		}
	}
	else
	{
		ShowChildWnd( ID_MALE2_BTN );
		ShowChildWnd( ID_FEMALE_BTN );
	}

	return true;
}

void ItemCustomInfoWnd::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool ItemCustomInfoWnd::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}

//////////////////////////////////////////////////////////////////////////

ItemCustomSelectWnd::ItemCustomSelectWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

ItemCustomSelectWnd::~ItemCustomSelectWnd()
{
	m_vInventoryItemInfoList.clear();
}

void ItemCustomSelectWnd::iwm_create()
{
}

void ItemCustomSelectWnd::iwm_show()
{
	g_GUIMgr.HideWnd( ITEM_CUSTOM_TRAIN_SELECT_WND );
	g_GUIMgr.HideWnd( COSTUME_CUSTOM_SELECT_WND );

	ChangeTab( ID_TAB_1_BTN );
}

void ItemCustomSelectWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
}

void ItemCustomSelectWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void ItemCustomSelectWnd::iwm_wheel( int zDelta )
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

void ItemCustomSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			if( param == ItemCustomInfoWnd::ID_CUSTOM_INFO_BTN )
			{
				OnItemCustomInfoPush( dwID );
			}
			else
			{
				ItemCustomInfoWnd *pItem = dynamic_cast<ItemCustomInfoWnd*>(FindChildWnd(dwID));
				if( pItem )
				{
					ItemCustomMakeWnd *pMakeWnd = dynamic_cast<ItemCustomMakeWnd*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_WND ));
					if( pMakeWnd )
					{
						if( param == ItemCustomInfoWnd::ID_MALE1_BTN || param == ItemCustomInfoWnd::ID_MALE2_BTN )
						{	
							pMakeWnd->SetExtraItemInfo( pItem->GetInfo(), true );
						}
						else if( param == ItemCustomInfoWnd::ID_FEMALE_BTN )
						{	
							pMakeWnd->SetExtraItemInfo( pItem->GetInfo(), false );							
						}
					}				
				}
			}
		}
		break;
	}
}

void ItemCustomSelectWnd::OnItemCustomInfoPush( DWORD dwID )
{
	ItemCustomInfoWnd *pItem = dynamic_cast<ItemCustomInfoWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMagicCode = pItem->GetMagicCode();

		// 장비 스킨 삭제 팝업
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem ) return;

		EXTRAITEMSLOT kSlot;
		if( !pExtraItem->GetExtraItem( iMagicCode, kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}

		if( kSlot.m_bCharEquip )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "착용 중인 장비는 스킨 을 할 수 없습니다." );
			return;
		}
		
		ItemCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast< ItemCustomGlobalDetailViewWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
		if( pDetailViewWnd )
		{
			pDetailViewWnd->CreateCharacter( kSlot.m_iItemCode, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
		}
		
	}
}

void ItemCustomSelectWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		ItemCustomInfoWnd *pItem = dynamic_cast<ItemCustomInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
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

void ItemCustomSelectWnd::ChangeTab( DWORD dwID )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
	m_dwCurTabID = dwID;

	SetExtraItemInfoVec( m_dwCurTabID );

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

void ItemCustomSelectWnd::SetExtraItemInfoVec( DWORD dwTabID )
{
	m_vInventoryItemInfoList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return;

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		int iGroup = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightTab( dwTabID, iGroup ) )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_EXTRAITEM;
		kInfo.m_iMagicCode = kSlot.m_iIndex;
		kInfo.m_bEquip = kSlot.m_bCharEquip;

		int iSlot = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE;
		int iValue = kSlot.m_iReinforce;

		if( !kSlot.m_bCharEquip )
			iValue += 100;

		kInfo.m_iPriorityOrder = iSlot * 10000 + iValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_iItemCode;

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

void ItemCustomSelectWnd::OnRender()
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
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_LEFT );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
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
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+36, TAT_LEFT );
	kPrinter.ClearList();
}

void ItemCustomSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );
	ProcessCustomBtnTooltip();
}

void ItemCustomSelectWnd::ProcessCustomBtnTooltip()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(i = ID_ITEM_INFO_WND_1_BTN;i <= ID_ITEM_INFO_WND_8_BTN;i++)
	{
		ItemCustomInfoWnd *pItemWnd = dynamic_cast<ItemCustomInfoWnd*>(FindChildWnd( i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem ) continue;

		int iMagicCode = pItemWnd->GetMagicCode();
		if( !pExtraItem->GetExtraItem( iMagicCode, kSlot ) ) continue;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1) );

		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(2) );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(3) );
		}		
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(4) );
		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(5) );
		}
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(6) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(7) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(8) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(9) );
	}

	ItemCustomThumbnailTooltip *pThumbnailTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_SELECT_TOOLTIP ));
	if( pThumbnailTooltip )
	{
		pThumbnailTooltip->SetTooltipText( kPrinter, kSlot );
		pThumbnailTooltip->IsShow();
	}

	for(i = 0;i < GLOBAL_TEXT_TOOLTIP_LINE;i++)
	{
		kPrinter[i].ClearList();
	}

	// 장착 툴팁
	int iEquipCharArray = -1;
	for(i = ID_ITEM_INFO_WND_1_BTN;i <= ID_ITEM_INFO_WND_8_BTN;i++)
	{
		ItemCustomInfoWnd *pItemWnd = dynamic_cast<ItemCustomInfoWnd*>(FindChildWnd( i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsIconOver() ) continue;

		iEquipCharArray = g_MyInfo.FindEquipedExtraItemChar( pItemWnd->GetMagicCode() );
		break;
	}

	if( iEquipCharArray != -1 )
	{
		const CHARACTER &rkCharInfo = g_MyInfo.GetCharacter( iEquipCharArray );
		if( rkCharInfo.m_class_type > 0 )
		{
			ioHashString szClassName = g_MyInfo.GetClassName( rkCharInfo.m_class_type, g_MyInfo.GetCharPowerUpGradeByClassType( rkCharInfo.m_class_type ) );

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			if( rkCharInfo.m_sex == 1 )
			{
				kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(10), szClassName.c_str() );
			}
			else
			{
				kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
				kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(11), szClassName.c_str() );
			}
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(12) );
		}
	}

	GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_INFO_TOOLTIP_WND ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter );
	}
}

int ItemCustomSelectWnd::GetTabType( DWORD dwTabID )
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

bool ItemCustomSelectWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

void ItemCustomSelectWnd::Update()
{
	UpdatePage( m_iCurPage );
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMakeUploadWnd::ItemCustomMakeUploadWnd()
{
	m_pBottomFrm = NULL;
	m_pTextLineFrm= NULL;
	m_dwCallWndID = 0;
}

ItemCustomMakeUploadWnd::~ItemCustomMakeUploadWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pTextLineFrm );
}

void ItemCustomMakeUploadWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "TextLineFrm" )
	{
		SAFEDELETE( m_pTextLineFrm );
		m_pTextLineFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ItemCustomMakeUploadWnd::iwm_create()
{
	char szDesktopPath[MAX_PATH] = "";
	SHGetSpecialFolderPath( g_App.GetHWnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );
	m_szFileFindPath = szDesktopPath;
}

void ItemCustomMakeUploadWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_UPLOAD:
		if( cmd == IOBN_BTNUP )
		{
			CompleteUpload();
			HideWnd();
		}
		break;
	case ID_FILE_FIND1:
	case ID_FILE_FIND2:
	case ID_FILE_FIND3:
	case ID_FILE_FIND4:
	case ID_FILE_FIND5:
	case ID_FILE_FIND6:
	case ID_FILE_FIND7:
	case ID_FILE_FIND8:
	case ID_FILE_FIND9:
	case ID_FILE_FIND10:
		if( cmd == IOBN_BTNUP )
		{
			FileListWnd *pFileListWnd = dynamic_cast< FileListWnd * >( g_GUIMgr.FindWnd( FILE_LIST_WND ) );
			if( pFileListWnd )
			{
				if( m_szFileFindPath.IsEmpty() )
				{
					char szDesktopPath[MAX_PATH] = "";
					SHGetSpecialFolderPath( g_App.GetHWnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );
					m_szFileFindPath = szDesktopPath;
				}
				pFileListWnd->SetFileList( m_szFileFindPath.c_str(), "jpg" );
				pFileListWnd->SetReturnInfo( FindChildWnd( dwID ) , FileListWnd::RWT_CUSTOM_TEXTURE_FIND_WND );
				pFileListWnd->ShowWnd();
			}
		}
		break;
	}
}

void ItemCustomMakeUploadWnd::CompleteUpload()
{
	int i = 0;
	int iSelectCount = 0;
	for(i = 0;i < (int)m_TextureList.size();i++)
	{
		TextureData &rkData = m_TextureList[i];

		char szFileName[MAX_PATH] = "";
		_splitpath( rkData.m_CustomTex.m_szTextureName.c_str(), NULL, NULL, szFileName, NULL );

		if( rkData.m_szSelectFile.IsEmpty() )
		{
			char szPath[1024] = "";
			sprintf( szPath, "%s%s_%d_custom.dds", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName, rkData.m_CustomTex.m_dwCustomSeq );				

			bool bTexCreate = false;
			ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( rkData.m_CustomTex.m_szTextureName );
			if( !pDDSTex )
			{
				bTexCreate = true;
				pDDSTex = g_TextureMgr.CreateTexture( rkData.m_CustomTex.m_szTextureName, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
			}
			else if( pDDSTex )
			{
				// 포멧 고퀄로 변경
				g_TextureMgr.ChangeTextureQuality( pDDSTex, TQ_HIGH );
			}

			if( !pDDSTex )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), rkData.m_CustomTex.m_szTextureName.c_str() );
				return;
			}

			if( !Help::CustomTextureToDDS( pDDSTex, szPath ) )
			{				
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), rkData.m_CustomTex.m_szTextureName.c_str() );
				if( bTexCreate )
				{
					g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
				}
				return;
			}

			if( !Help::CompressLSCFile( szPath ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), rkData.m_CustomTex.m_szTextureName.c_str() );
				if( bTexCreate )
				{
					g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
				}
				return;
			}
			
			if( bTexCreate )
				g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
			else
				g_TextureMgr.ChangeTextureQuality( pDDSTex, Setting::GetTextureQuality() );

			char szNewPath[1024] = "";			
			sprintf( szNewPath, "%s%s.lsc", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName );

			char szOldPath[1024] = "";
			sprintf( szOldPath, "%s%s_%d_custom.lsc", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName, rkData.m_CustomTex.m_dwCustomSeq );
			::DeleteFile( szNewPath );
			if( rename( szOldPath, szNewPath ) != 0 ) 
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), rkData.m_CustomTex.m_szTextureName.c_str() );	
				return;
			}

			// 임시 DDS 삭제
			::DeleteFile( szPath );

			// 임시 LSC 삭제	
			::DeleteFile( szOldPath );
		}
		else
		{
			char szPath[1024] = "";
			sprintf( szPath, "%s%s_%d_custom.dds", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName, rkData.m_CustomTex.m_dwCustomSeq );			

			bool bTexCreate = false;
			ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( rkData.m_CustomTex.m_szTextureName );
			if( !pDDSTex )
			{
				bTexCreate = true;
				pDDSTex = g_TextureMgr.CreateTexture( rkData.m_CustomTex.m_szTextureName, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
			}
			else if( pDDSTex )
			{
				// 포멧 고퀄로 변경
				g_TextureMgr.ChangeTextureQuality( pDDSTex, TQ_HIGH );
			}

			if( !pDDSTex )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), rkData.m_CustomTex.m_szTextureName.c_str() );
				return;
			}

			if( !Help::CustomTextureToDDS( pDDSTex, szPath ) )
			{				
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), rkData.m_CustomTex.m_szTextureName.c_str() );
				if( bTexCreate )
				{
					g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
				}
				return;
			}

			if( bTexCreate )
				g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
			else
				g_TextureMgr.ChangeTextureQuality( pDDSTex, Setting::GetTextureQuality() );

			// 파일 두개를 합쳐서 dds로 만든다.
			int iFunResult = Help::CustomTextureJPGMixDDSFromDDS( szPath, rkData.m_szSelectFile, szPath );
			if( iFunResult != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), rkData.m_CustomTex.m_szTextureName.c_str(), iFunResult );
				if( bTexCreate )
				{
					g_TextureMgr.DestroyByName( rkData.m_CustomTex.m_szTextureName, true );
				}
				return;
			}

			// 만들어진 파일 lsc로 변경
			if( !Help::CompressLSCFile( szPath ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8), rkData.m_CustomTex.m_szTextureName.c_str() );
				return;
			}

			// 파일명 변경
			char szNewPath[1024] = "";
			sprintf( szNewPath, "%s%s.lsc", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName );
			
			char szOldPath[1024] = "";
			sprintf( szOldPath, "%s%s_%d_custom.lsc", g_CustomTextureMgr.GetAppDirectory().c_str(), szFileName, rkData.m_CustomTex.m_dwCustomSeq );
			::DeleteFile( szNewPath );
			if( rename( szOldPath, szNewPath ) != 0 ) 
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9), rkData.m_CustomTex.m_szTextureName.c_str() );	
				return;
			}

			// 임시 DDS 삭제
			::DeleteFile( szPath );

			// 임시 LSC 삭제	
			::DeleteFile( szOldPath );

			iSelectCount++;
		}
	}	

	if( m_dwCallWndID == ITEM_CUSTOM_MAKE_WND )
	{
		ItemCustomMakeWnd *pMakeWnd = dynamic_cast< ItemCustomMakeWnd * > ( g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_WND ) );
		if( pMakeWnd )
		{
			pMakeWnd->AfterViewLSCTexture( iSelectCount );
		}
	}
	else if( m_dwCallWndID == ITEM_CUSTOM_TRAIN_MAKE_WND )
	{
		ItemCustomTrainMakeWnd *pMakeWnd = dynamic_cast< ItemCustomTrainMakeWnd * > ( g_GUIMgr.FindWnd( ITEM_CUSTOM_TRAIN_MAKE_WND ) );
		if( pMakeWnd )
		{
			pMakeWnd->AfterViewLSCTexture( iSelectCount );
		}
	}
	else if( m_dwCallWndID == COSTUME_CUSTOM_MAKE_WND )
	{
		CostumeCustomMakeWnd *pMakeWnd = dynamic_cast<CostumeCustomMakeWnd*>( g_GUIMgr.FindWnd( COSTUME_CUSTOM_MAKE_WND ) );
		if( pMakeWnd )
			pMakeWnd->AfterViewLSCTexture( iSelectCount );
	}
}

void ItemCustomMakeUploadWnd::SetUpload( DWORD dwCallWndID, ioComplexStringPrinter &rkItemNameMent, ioComplexStringPrinter &rkTextureCountMent, CustomTextureList &rkOriginalTexList )
{
	m_TextureList.clear();
	m_ItemNameMent.ClearList();
	m_TextureCountMent.ClearList();

	m_dwCallWndID = dwCallWndID;
	rkItemNameMent.CopyFullText( m_ItemNameMent );
	rkTextureCountMent.CopyFullText( m_TextureCountMent );
    	
	int i = 0;
	for(i = 0;i < MAX_FILE;i++)
		HideChildWnd( ID_FILE_FIND1 + i );

	for(i = 0;i < (int)rkOriginalTexList.size();i++)
	{
		TextureData kData;
		kData.m_CustomTex = rkOriginalTexList[i];

		bool bTexCreate = false;
		ioTexture *pDDSTex = g_TextureMgr.GetTextureByName( kData.m_CustomTex.m_szTextureName );
		if( !pDDSTex )
		{
			bTexCreate = true;
			pDDSTex = g_TextureMgr.CreateTexture( kData.m_CustomTex.m_szTextureName, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH );
		}
		else if( pDDSTex )
		{
			// 포멧 고퀄로 변경
			g_TextureMgr.ChangeTextureQuality( pDDSTex, TQ_HIGH );
		}

		if( !pDDSTex )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), kData.m_CustomTex.m_szTextureName.c_str() );
			return;
		}
		
		kData.m_iWidth = pDDSTex->GetWidth();
		kData.m_iHeight= pDDSTex->GetHeight();
		m_TextureList.push_back( kData );

		if( bTexCreate )
			g_TextureMgr.DestroyByName( kData.m_CustomTex.m_szTextureName, true );
		else
			g_TextureMgr.ChangeTextureQuality( pDDSTex, Setting::GetTextureQuality() );
		ShowChildWnd( ID_FILE_FIND1 + i );
	}

	// 윈도우 사이즈
	SetSize( GetWidth(), DEFAULT_HEIGHT + ( LINE_ADD_HEIGHT * (int)m_TextureList.size() ) );

	// 버튼 배치
	ioWnd *pUpload = FindChildWnd( ID_UPLOAD );
	if( pUpload )
	{
		pUpload->SetWndPos( pUpload->GetXPos(), GetHeight() - 43 );
	}

	ioWnd *pCancel = FindChildWnd( ID_CANCEL );
	if( pCancel )
	{
		pCancel->SetWndPos( pCancel->GetXPos(), GetHeight() - 43 );
	}

	//
}

void ItemCustomMakeUploadWnd::SetFilePath( ioWnd *pReturnBtn, const ioHashString &rkFilePath )
{
	if( !pReturnBtn ) return;

	int iArray = pReturnBtn->GetID() - ID_FILE_FIND1;
	if( !COMPARE( iArray, 0, (int)m_TextureList.size() ) )
		return;

	// 파일 사이즈 확인
	ioTexture *pDDSTex = g_TextureMgr.CreateTexture( rkFilePath, D3DUSAGE_DYNAMIC, D3DX_DEFAULT, TQ_HIGH, false, false, true );
	if( !pDDSTex )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), rkFilePath.c_str() );
		return;
	}

	if( pDDSTex->GetWidth() != m_TextureList[iArray].m_iWidth ||
		pDDSTex->GetHeight() != m_TextureList[iArray].m_iHeight )
	{
		g_TextureMgr.DestroyByName( rkFilePath, true );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), m_TextureList[iArray].m_iWidth, m_TextureList[iArray].m_iHeight  );
		return;
	}

	g_TextureMgr.DestroyByName( rkFilePath, true );
	m_TextureList[iArray].m_szSelectFile = rkFilePath;

	// 디폴트 경로 변경
	char szDrive[MAX_PATH] = "";
	char szPath[MAX_PATH]  = "";
	char szFullPath[1024]  = "";
	_splitpath( rkFilePath.c_str(), szDrive, szPath, NULL, NULL );
	sprintf( szFullPath, "%s%s", szDrive, szPath );

	// 마지막 \이건 뺀다
	int iLen = strlen( szFullPath );
	if( szFullPath[iLen-1] == '\\' )
		szFullPath[iLen-1] = NULL;
	m_szFileFindPath = szFullPath;
}

void ItemCustomMakeUploadWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	m_ItemNameMent.PrintFullText( iXPos + 178, iYPos + 53, TAT_CENTER );
	m_TextureCountMent.PrintFullText( iXPos + 178, iYPos + 74, TAT_CENTER );

	if( m_pBottomFrm )
	{
		m_pBottomFrm->Render( iXPos, iYPos + (GetHeight() - 55), UI_RENDER_MULTIPLY );
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	for(int i = 0;i < (int)m_TextureList.size();i++)
	{	
		TextureData &rkTexture = m_TextureList[i];

		if( m_pTextLineFrm )
			m_pTextLineFrm->Render( iXPos + 17, iYPos + 102 + (i*LINE_ADD_HEIGHT) );
		
		if( rkTexture.m_szSelectFile.IsEmpty() )
		{
			g_FontMgr.PrintText( iXPos + 24, iYPos + 107 + (i*LINE_ADD_HEIGHT), FONT_SIZE_12, STR(1), i + 1, rkTexture.m_iWidth, rkTexture.m_iHeight );
		}
		else
		{
			g_FontMgr.PrintTextLeftCut( iXPos + 24, iYPos + 107 + (i*LINE_ADD_HEIGHT), FONT_SIZE_12, 208.0f, STR(2), rkTexture.m_szSelectFile.c_str() );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMakeDownloadWnd::ItemCustomMakeDownloadWnd()
{
	m_iFileCount = 0;
}

ItemCustomMakeDownloadWnd::~ItemCustomMakeDownloadWnd()
{

}

void ItemCustomMakeDownloadWnd::SetDownload( const ioHashString &rkDownPath, const ioHashString &rkDownFile, int iFileCount )
{
	m_szDownPath = rkDownPath;
	m_szDownFile = rkDownFile;
	m_iFileCount = iFileCount;
}

void ItemCustomMakeDownloadWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ItemCustomMakeDownloadWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	g_FontMgr.PrintText( iXPos + 140, iYPos + 53, FONT_SIZE_17, STR(1) );

	float fTextWidth = 247.0f;	
	char szDst[MAX_PATH] = "";
	memset( szDst, 0, sizeof( szDst ) );
	g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );

	char szFileName[1024] = "";
	SafeSprintf( szFileName, sizeof( szFileName ), STR(2), m_szDownFile.c_str(), m_iFileCount );	
	int iLen = Help::StringCutFun( FONT_SIZE_13, fTextWidth, TS_NORMAL, szDst, sizeof(szDst), szFileName );
	g_FontMgr.PrintText( iXPos + 140, iYPos + 80, FONT_SIZE_13, szDst );
	if( iLen < (int)strlen( szFileName ) )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 140, iYPos + 96, FONT_SIZE_13, fTextWidth, &szFileName[iLen] );	
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	memset( szDst, 0, sizeof( szDst ) );

	char szPath[1024] = "";
	sprintf( szPath, "%s", m_szDownPath.c_str() );	
	iLen = Help::StringCutFun( FONT_SIZE_12, fTextWidth, TS_NORMAL, szDst, sizeof(szDst), szPath );
	g_FontMgr.PrintText( iXPos + 140, iYPos + 119, FONT_SIZE_12, szDst );
	if( iLen < (int)strlen( szPath ) )
	{
		g_FontMgr.PrintTextWidthCut( iXPos + 140, iYPos + 134, FONT_SIZE_12, fTextWidth, &szPath[iLen] );	
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMakeWnd::ItemCustomMakeWnd()
{
	m_bMale = true;
	m_TexSelectCount = 0;
	m_eFileUplaodState = FUS_NONE;
	m_dwUploadDelayTime = 0;
	m_dwUploadUniqueIndex = 0;

	m_dwHelpID = 0;
}

ItemCustomMakeWnd::~ItemCustomMakeWnd()
{
}

void ItemCustomMakeWnd::iwm_create()
{
}

void ItemCustomMakeWnd::iwm_show()
{
	g_GUIMgr.HideWnd( ITEM_CUSTOM_SELECT_WND );
}

void ItemCustomMakeWnd::iwm_hide()
{
}

void ItemCustomMakeWnd::AfterViewLSCTexture( int iSelectCount )
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

void ItemCustomMakeWnd::SetExtraItemInfo( InventoryItemInfo &rkItem, bool bMale )
{
	ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pUserExtraItem )
		return;

	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;
	m_OriginalTexList.clear();
	m_AfterTexList.clear();
	m_TexSelectCount = 0;

	EXTRAITEMSLOT kSlot;
	if( !pUserExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
		return;

	if( kSlot.m_bCharEquip )
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			if( g_MyInfo.IsCharRentalToExtraItem( m_ItemInfo.m_iMagicCode ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
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
	m_ItemNameMent.AddTextPiece( FONT_SIZE_17, STR(1), kSlot.m_Name.c_str() );
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
		kItemData.m_item_code = kSlot.m_iItemCode;

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
		kItemData.m_item_code = kSlot.m_iItemCode;
		kItemData.m_item_male_custom = kSlot.m_dwMaleCustom;
		kItemData.m_item_female_custom = kSlot.m_dwFemaleCustom;

		pAfter3D->CreateCharacter( kCharInfo, kItemData, m_AfterTexList );
	}

	// 
	m_TextureCountMent.ClearList();
	m_TextureCountMent.SetTextStyle( TS_NORMAL );
	m_TextureCountMent.SetBkColor( 0, 0, 0 );	
	m_TextureCountMent.SetTextColor( TCT_DEFAULT_ORANGE );
	m_TextureCountMent.AddTextPiece( FONT_SIZE_13, STR(6), (int)m_OriginalTexList.size() );

	char szBtnTitle[MAX_PATH] = "";
	SafeSprintf( szBtnTitle, sizeof(szBtnTitle), STR(7), (int)m_OriginalTexList.size() );
	ioWnd *pUploadBtn = FindChildWnd( ID_UPLOAD_FILE );
	if( pUploadBtn )
	{
		pUploadBtn->SetTitleText( szBtnTitle );
		pUploadBtn->SetActive();
	}

	ShowWnd();
}

void ItemCustomMakeWnd::UploadTexture()
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

void ItemCustomMakeWnd::DownloadTexture()
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

void ItemCustomMakeWnd::ShowDetailView()
{
	ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pUserExtraItem )
		return;

	EXTRAITEMSLOT kSlot;
	if( !pUserExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
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
		kItemData.m_item_code = kSlot.m_iItemCode;

		pDetailView->CreateCharacter( kCharInfo, kItemData, m_AfterTexList );
		pDetailView->ShowWnd();
	}
}

void ItemCustomMakeWnd::StartCustomFileMake( DWORD dwUniqueIndex )
{
	if( !IsShow() )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
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
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), rkData.m_szTextureName.c_str() );
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
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), rkData.m_szTextureName.c_str() );
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

bool ItemCustomMakeWnd::iwm_esc()
{
	if( m_eFileUplaodState != FUS_NONE || g_App.IsMouseBusy() )
	{
		return false;
	}
	return ioWnd::iwm_esc();
}

void ItemCustomMakeWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwHelpID = xElement.GetIntAttribute_e( "HelpID" );
}

void ItemCustomMakeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
				g_GUIMgr.ShowWnd( ITEM_CUSTOM_SELECT_WND );
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
				ioUserEtcItem::ETCITEMSLOT kEtcItem;
				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				if( !pUserEtcItem || !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN, kEtcItem ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
				else if( m_TexSelectCount == 0 )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				else
				{
					//
					ItemCustomChangeConfirmWnd *pChangeConfirmWnd = dynamic_cast< ItemCustomChangeConfirmWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_CHANGE_CONFIRM_WND ) );
					if( pChangeConfirmWnd )
					{
						pChangeConfirmWnd->SetItemCustomChangeInfo( m_ItemInfo, m_bMale );
					}
				}
			}
			else  // g_FileWriteClient 응답
			{
				EndFileUpload( param );
			}			
		}
		break;
	}
}

void ItemCustomMakeWnd::OnRender()
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

void ItemCustomMakeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessUploadDelay();
}

void ItemCustomMakeWnd::ProcessUploadDelay()
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

void ItemCustomMakeWnd::EndFileUpload( DWORD param )
{
	if( param == ioFileWriteClient::RETURN_WND_PARAM_FAIL )
	{
		m_eFileUplaodState = FUS_NONE;
		g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( param == ioFileWriteClient::RETURN_WND_PARAM_COMPLETE )
	{
		if( !IsShow() )
		{
			m_eFileUplaodState = FUS_NONE;
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
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
					EXTRAITEMSLOT kSlot;
					ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
					if( pExtraItem && pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
					{
						if( m_bMale )
						{
							kSlot.m_dwMaleCustom = m_dwUploadUniqueIndex;
						}
						else
						{
							kSlot.m_dwFemaleCustom = m_dwUploadUniqueIndex;
						}
						pExtraItem->SetExtraItem( kSlot );

						SP2Packet kPacket( CTPK_ETCITEM_USE );
						kPacket << ioEtcItem::EIT_ETC_CUSTOM_ITEM_SKIN;
						kPacket << kSlot.m_iIndex;
						kPacket << kSlot.m_dwMaleCustom;
						kPacket << kSlot.m_dwFemaleCustom;
						TCPNetwork::SendToServer( kPacket );
						TCPNetwork::MouseBusy( true );
					}	
					else
					{
						m_eFileUplaodState = FUS_NONE;
						g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
					}
				}
				else
				{
					m_eFileUplaodState = FUS_NONE;
					g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				}
			}
		}
	}
}

void ItemCustomMakeWnd::CompleteCustomFileMake()
{
	g_MyInfo.UpdateEquipExtraItem( m_ItemInfo.m_iMagicCode );

	HideWnd();
	g_GUIMgr.HideWnd( FILE_WRITE_SERVER_PROCESS );

	ItemCustomMakeCompleteWnd *pCompleteWnd = dynamic_cast< ItemCustomMakeCompleteWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_COMPLETE_WND ) );
	if( pCompleteWnd )
	{
		pCompleteWnd->SetItemCustomMakeInfo( m_ItemInfo, m_bMale );
	}
	
	m_eFileUplaodState = FUS_NONE;
	for(int i = 0;i < (int)m_FileUploadCompleteList.size();i++)
	{
		FileUploadData &rkData = m_FileUploadCompleteList[i];
		::DeleteFile( rkData.m_szDstFilePath );
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMake3DViewWnd::ItemCustomMake3DViewWnd()
{
	m_pStateFrm   = NULL;
	m_pRectSrc    = NULL;
	m_pMouseLeft  = NULL;	
	m_pMouseRight = NULL;
	m_pMouseMiddle= NULL;
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;
}

ItemCustomMake3DViewWnd::~ItemCustomMake3DViewWnd()
{
	SAFEDELETE( m_pStateFrm );
	SAFEDELETE( m_pRectSrc );
	SAFEDELETE( m_pMouseLeft );	
	SAFEDELETE( m_pMouseRight );
	SAFEDELETE( m_pMouseMiddle );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void ItemCustomMake3DViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void ItemCustomMake3DViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomMake3DViewWnd::CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA &rkItem, CustomTextureList &rkReturn )
{
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

	pChar->EquipItem( rkItem.m_item_code, rkItem.m_item_male_custom, rkItem.m_item_female_custom );
	pChar->RotateY( -15.0f );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	m_aMyChar->SetMannequins( true );

	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{	
		const ioItem *pItem = g_ItemMaker.GetItemConst( rkItem.m_item_code, __FUNCTION__ );
		if( pItem )
		{
			switch( pItem->GetType() )
			{
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
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

					m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
					m_pUI3DRender->SetCameraPos( -110.0f, 20.0f, 615.0f );
				}
				break;
			case ioItem::IT_ARMOR:
				{
					const ioArmorItem *pArmorItem = ToArmorItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pArmorItem->GetBodyMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pArmorItem->GetBodyMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );
					
					szNewMeshName = Help::ConvertName( pArmorItem->GetHandMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pArmorItem->GetHandMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

					m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
					m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 600.0f );
				}
				break;
			case ioItem::IT_HELMET:
				{
					const ioHelmetItem *pHelmetItem = ToHelmetItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pHelmetItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pHelmetItem->GetEquipMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

					m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
					pChar->SetPosition( 0.0f, -130.0f, 950.0f );
					m_pUI3DRender->SetCameraPos( -110.0f, -20.0f, 710.0f );
				}
				break;
			case ioItem::IT_CLOAK:
				{
					const ioCloakItem *pCloakItem = ToCloakItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pCloakItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pCloakItem->GetEquipMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, rkReturn );

					m_aMyChar->SetLoopAni( "custom_pose_01.ani" );
					m_pUI3DRender->SetCameraPos( -110.0f, -15.0f, 1300.0f );
				}
				break;
			}			
		}
	}
}

void ItemCustomMake3DViewWnd::ChangeLSCTexture( CustomTextureList &rkOriTexList, CustomTextureList &rkAftTexList )
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

void ItemCustomMake3DViewWnd::iwm_create()
{
	
}

void ItemCustomMake3DViewWnd::iwm_show()
{
}

void ItemCustomMake3DViewWnd::iwm_hide()
{

}

void ItemCustomMake3DViewWnd::OnProcess( float fTimePerSec )
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

void ItemCustomMake3DViewWnd::OnRectRender( int iXPos, int iYPos )
{
	if( !m_pRectSrc ) return;

	for(int i = 0;i < RECT_HEIGHT_CNT;i++)
	{
		for(int j = 0;j < RECT_WIDTH_CNT;j++)
		{
			m_pRectSrc->Render( iXPos + (j * RECT_GAP_SIZE), iYPos );
		}
		iYPos += RECT_GAP_SIZE;
	}
}

void ItemCustomMake3DViewWnd::OnRender()
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
		g_FontMgr.PrintText( iXPos + 146, iYPos + 39, FONT_SIZE_10, STR(1) );
	}
	if( m_pMouseRight )
	{
		m_pMouseRight->SetScale( 0.66f );
		m_pMouseRight->Render( iXPos + 165, iYPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 168, iYPos + 39, FONT_SIZE_10, STR(2) );
	}
	if( m_pMouseMiddle )
	{
		m_pMouseMiddle->SetScale( 0.66f );
		m_pMouseMiddle->Render( iXPos + 187, iYPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 190, iYPos + 39, FONT_SIZE_10, STR(3) );
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
		case ItemCustomMakeWnd::ID_BEFORE_VIEW:
			{
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, STR(4) );
			}
			break;
		case ItemCustomMakeWnd::ID_AFTER_VIEW:
			{
				g_FontMgr.SetTextStyle( TS_NORMAL );
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.SetBkColor( 0, 0, 0 );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, STR(5) );
			}
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMakeDetailViewWnd::ItemCustomMakeDetailViewWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;

	m_pTitleBar   = NULL;
	m_pBackground = NULL;
	m_pMouseLeft  = NULL;	
	m_pMouseRight = NULL;
	m_pMouseMiddle= NULL;
}

ItemCustomMakeDetailViewWnd::~ItemCustomMakeDetailViewWnd()
{
	SAFEDELETE( m_pTitleBar );
	SAFEDELETE( m_pBackground );
	SAFEDELETE( m_pMouseLeft );
	SAFEDELETE( m_pMouseRight );
	SAFEDELETE( m_pMouseMiddle );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void ItemCustomMakeDetailViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void ItemCustomMakeDetailViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomMakeDetailViewWnd::CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA &rkItem, CustomTextureList &rkTexList )
{
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

	pChar->EquipItem( rkItem.m_item_code, 0, 0 );
	pChar->RotateY( -15.0f );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	m_aMyChar->SetMannequins( false );
	m_aMyChar->AttachEffect( "ui_grid.txt", NULL );
	
	CustomTextureList kTempList;
	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{	
		const ioItem *pItem = g_ItemMaker.GetItemConst( rkItem.m_item_code, __FUNCTION__ );
		if( pItem )
		{
			if( m_aMyChar->IsMale() )
			{
				SafeSprintf( m_szSkinName, sizeof(m_szSkinName), STR(1), pItem->GetName().c_str() );
			}
			else
			{
				SafeSprintf( m_szSkinName, sizeof(m_szSkinName), STR(2), pItem->GetName().c_str() );
			}
			
			switch( pItem->GetType() )
			{
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
			case ioItem::IT_ARMOR:
				{
					const ioArmorItem *pArmorItem = ToArmorItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pArmorItem->GetBodyMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pArmorItem->GetBodyMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

					szNewMeshName = Help::ConvertName( pArmorItem->GetHandMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pArmorItem->GetHandMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

					m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
				}
				break;
			case ioItem::IT_HELMET:
				{
					const ioHelmetItem *pHelmetItem = ToHelmetItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pHelmetItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pHelmetItem->GetEquipMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

					m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
				}
				break;
			case ioItem::IT_CLOAK:
				{
					const ioCloakItem *pCloakItem = ToCloakItemConst( pItem );
					ioHashString szNewMeshName = Help::ConvertName( pCloakItem->GetEquipMesh(NULL), m_aMyChar->IsMale() );
					if( !m_aMyChar->IsMale() )
					{
						char szFullName[MAX_PATH]="";
						StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\mesh\\%s.msh" , szNewMeshName.c_str() );
						if( !g_ResourceLoader.IsFileExist( szFullName ) )
						{
							szNewMeshName = Help::ConvertName( pCloakItem->GetEquipMesh(NULL), true );
						}
					}
					Help::CustomEntityTextureNameCnt( pCharGrp->GetEntity( szNewMeshName ), 0, kTempList );

					m_aMyChar->SetLoopAni( "custom_pose_02.ani" );
				}
				break;
			}			
		}
	}

	if( kTempList.size() == rkTexList.size() )
	{
		for(int i = 0;i < (int)rkTexList.size();i++)
		{
			Help::CustomEntityGroupTexture( m_aMyChar->GetGroup(), kTempList[i].m_szTextureName, rkTexList[i].m_szTextureName, kTempList[i].m_dwCustomSeq );
		}
	}
}

void ItemCustomMakeDetailViewWnd::iwm_create()
{

}

void ItemCustomMakeDetailViewWnd::iwm_show()
{
}

void ItemCustomMakeDetailViewWnd::iwm_hide()
{

}

void ItemCustomMakeDetailViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void ItemCustomMakeDetailViewWnd::OnProcess( float fTimePerSec )
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

void ItemCustomMakeDetailViewWnd::OnRenderBackground( int iXPos, int iYPos )
{
	if( !m_pBackground ) return;

	int iWidthCnt = 750 / m_pBackground->GetWidth();
	for(int i = 0;i < iWidthCnt;i++)
	{
		m_pBackground->Render( iXPos + 2 + (m_pBackground->GetWidth() * i), iYPos + 6 );
	}
}

void ItemCustomMakeDetailViewWnd::OnRender()
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
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(1) );

	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( 255, 188, 26 );
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(2), m_szSkinName );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 36, iYPos + 106, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 68, iYPos + 106, FONT_SIZE_12, STR(4) );
	g_FontMgr.PrintText( iXPos + 100, iYPos + 106, FONT_SIZE_12, STR(5) );

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
ItemCustomMakeConfirmWnd::ItemCustomMakeConfirmWnd()
{
	m_pExclamationMark = NULL;
}

ItemCustomMakeConfirmWnd::~ItemCustomMakeConfirmWnd()
{
	SAFEDELETE( m_pExclamationMark );
}

void ItemCustomMakeConfirmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioWnd::AddRenderFrame( szType, pFrame );
}

void ItemCustomMakeConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

bool ItemCustomMakeConfirmWnd::SetItemCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem && pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
	{
		m_szName = kSlot.m_Name;
		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ItemCustomIconBtn *pIconBtn = dynamic_cast<ItemCustomIconBtn*>(FindChildWnd(ID_ICON_BTN));
		if( pIconBtn )
		{
			pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kSlot.m_iReinforce );
			pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );
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

void ItemCustomMakeConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_MAKE_BTN:
		if( cmd == IOBN_BTNUP )
		{			
			ItemCustomMakeWnd *pMakeWnd = dynamic_cast<ItemCustomMakeWnd*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_MAKE_WND ));
			if( pMakeWnd )
			{
				pMakeWnd->SetExtraItemInfo( m_ItemInfo, m_bMale );
			}			

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

void ItemCustomMakeConfirmWnd::OnRender()
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
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(1), m_szName.c_str() );
	kPrinter.PrintFullText( iXPos+NAME_X, iYPos+NAME_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(2) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(3) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+20, TAT_CENTER );

	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+40, TAT_CENTER );

	kPrinter.ClearList();
}

void ItemCustomMakeConfirmWnd::OnRenderAfterChild()
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
ItemCustomChangeConfirmWnd::ItemCustomChangeConfirmWnd()
{
}

ItemCustomChangeConfirmWnd::~ItemCustomChangeConfirmWnd()
{
}

bool ItemCustomChangeConfirmWnd::SetItemCustomChangeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem && pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
	{
		m_szName = kSlot.m_Name;
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

void ItemCustomChangeConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE:
		if( cmd == IOBN_BTNUP )
		{	
			HideWnd();
			TCPNetwork::MouseBusy( true );
			SP2Packet kPacket( CTPK_CUSTOM_ITEM_SKIN_UNIQUE_INDEX );
			TCPNetwork::SendToServer( kPacket );
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

void ItemCustomChangeConfirmWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 53, FONT_SIZE_17, STR(1), m_szName.c_str() );
	if( m_bMale )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, STR(2) );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, STR(3) );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 101, FONT_SIZE_13, STR(4) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 128, FONT_SIZE_13, STR(5) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 146, FONT_SIZE_13, STR(6) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 164, FONT_SIZE_13, STR(7) );
}
//////////////////////////////////////////////////////////////////////////
ItemCustomMakeCompleteWnd::ItemCustomMakeCompleteWnd()
{
	m_pIconBack	  = NULL;
	m_pIcon		  = NULL;
	m_pCustomIcon = NULL;
	m_pNumText    = NULL;
	m_pPlusText   = NULL;

	m_bMale = true;
	m_iReinforce = 0;
}

ItemCustomMakeCompleteWnd::~ItemCustomMakeCompleteWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pCustomIcon );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
}

void ItemCustomMakeCompleteWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );
	}
}

bool ItemCustomMakeCompleteWnd::SetItemCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale )
{
	m_ItemInfo.Init();
	m_ItemInfo = rkItem;
	m_bMale    = bMale;

	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem && pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
	{
		m_szName = kSlot.m_Name;
		m_iReinforce = kSlot.m_iReinforce;	

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

void ItemCustomMakeCompleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			{
				pInvenWnd->ShowExtraItemTabDirect( -1 );
			}
			HideWnd();
		}
		break;
	}
}

void ItemCustomMakeCompleteWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioMovingWnd::OnProcess( fTimePerSec );
}

void ItemCustomMakeCompleteWnd::OnRender()
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
		{
			m_pIcon->Render( iNewXPos, iNewYPos );
		}

		if( m_pCustomIcon )
		{
			m_pCustomIcon->Render( iNewXPos + 7, iNewYPos - 43 );
		}

		// Num
		enum
		{
			REINFORCE_Y	= 14,

			NUM_SIZE	= 20,
			PLUS_SIZE	= 14,

			NUM_GAP		= 4,
		};

		if( m_pNumText && m_pPlusText && m_iReinforce > 0 )
		{			
			int iCurPos = 0;
			int iDefaultPos = iNewXPos;
			int iCurYPos = iNewYPos + REINFORCE_Y;

			int iTotalSize = 0;
			if( m_iReinforce < 10 )
				iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
			else if( m_iReinforce < 100 )
				iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
			else
				iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

			iCurPos = iDefaultPos - (iTotalSize / 2);

			m_pPlusText->Render( iCurPos, iCurYPos );

			iCurPos += (PLUS_SIZE-NUM_GAP);

			m_pNumText->RenderNum( iCurPos, iCurYPos, m_iReinforce, -NUM_GAP );
		}
	}

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 149, FONT_SIZE_17, STR(1), m_szName.c_str() );
	if( m_bMale )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 170, FONT_SIZE_17, STR(2) );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 170, FONT_SIZE_17, STR(3) );
	}

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 199, FONT_SIZE_13, STR(4) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 217, FONT_SIZE_13, STR(5) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 235, FONT_SIZE_13, STR(6) );
}
//////////////////////////////////////////////////////////////////////////
ItemCustomDeleteWnd::ItemCustomDeleteWnd()
{
	m_iSlotIndex = -1;
	m_pPreEdit   = NULL;
}

ItemCustomDeleteWnd::~ItemCustomDeleteWnd()
{

}

void ItemCustomDeleteWnd::SetSlotIndex( int iSlotIndex )
{
	m_iSlotIndex = iSlotIndex;

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem ) return;

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( m_iSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
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
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}		
	}
}

void ItemCustomDeleteWnd::iwm_show()
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

void ItemCustomDeleteWnd::iwm_hide()
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

void ItemCustomDeleteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			if( strcmp( pEdit->GetText(), STR(1) ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			TCPNetwork::MouseBusy( true );

			SP2Packet kPacket( CTPK_CUSTOM_ITEM_SKIN_DELETE );
			kPacket << m_iSlotIndex;
			kPacket << SDT_MALE;
			TCPNetwork::SendToServer( kPacket );

			HideWnd();
		}
		break;
	case ID_FEMALE_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			ioEdit *pEdit = dynamic_cast< ioEdit * >( FindChildWnd( ID_DELETE_EDIT ) );
			if( !pEdit ) return;
			if( strcmp( pEdit->GetText(), STR(3) ) != 0 )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				return;
			}

			TCPNetwork::MouseBusy( true );

			SP2Packet kPacket( CTPK_CUSTOM_ITEM_SKIN_DELETE );
			kPacket << m_iSlotIndex;
			kPacket << SDT_FEMALE;
			TCPNetwork::SendToServer( kPacket );

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

void ItemCustomDeleteWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;
}

void ItemCustomDeleteWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 53, FONT_SIZE_17, STR(1) );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 74, FONT_SIZE_17, STR(2) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 103, iYPos + 180, FONT_SIZE_12, STR(3) );
	if( FindChildWnd( ID_DELETE_EDIT ) != ioEdit::m_pKeyFocusEdit )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		g_FontMgr.PrintText( iXPos + 103, iYPos + 201, FONT_SIZE_12, STR(4) );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 98, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 115, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "*" );
	kPrinter.SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(9) );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(10) );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(11) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 132, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(12) );
	kPrinter.PrintFullText( iXPos + 103, iYPos + 149, TAT_CENTER );
	kPrinter.ClearList();
}
//////////////////////////////////////////////////////////////////////////
ItemCustomThumbnailWnd::ItemCustomThumbnailWnd()
{
	m_pBlueStateFrm   = NULL;
	m_pRedStateFrm   = NULL;
	m_pRectSrc    = NULL;
	m_pUI3DRender = NULL;
	m_aMyChar     = NULL;
	m_dwRotateCheckTime = 0;
}

ItemCustomThumbnailWnd::~ItemCustomThumbnailWnd()
{
	SAFEDELETE( m_pBlueStateFrm );
	SAFEDELETE( m_pRedStateFrm );
	SAFEDELETE( m_pRectSrc );
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
}

void ItemCustomThumbnailWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void ItemCustomThumbnailWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomThumbnailWnd::CreateCharacter( EXTRAITEMSLOT &rkExtraItem, bool bMale )
{
	if( IsShow() )
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

	ITEM_DATA kItemData;
	kItemData.m_item_code = rkExtraItem.m_iItemCode;
	kItemData.m_item_male_custom = rkExtraItem.m_dwMaleCustom;
	kItemData.m_item_female_custom = rkExtraItem.m_dwFemaleCustom;

	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );

	ioCreateChar *pChar = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
	if( !pChar )
		return;

	pChar->SetPosition( 0.0f, -80.0f, 950.0f );
	pChar->SetYaw( FLOAT10 );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );

	float fAspect = (float)(GetWidth() - 2)/(float)(GetHeight() - 2);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 20.0f, fAspect );

	pChar->EquipItem( kItemData.m_item_code, kItemData.m_item_male_custom, kItemData.m_item_female_custom );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;
	
	m_aMyChar->SetMannequins( true );
	m_aMyChar->SetLoopAni( "custom_pose_01.ani" );

	ioEntityGroup *pCharGrp = m_aMyChar->GetGroup();
	if( pCharGrp )
	{	
		const ioItem *pItem = g_ItemMaker.GetItemConst( kItemData.m_item_code, __FUNCTION__ );
		if( pItem )
		{
			switch( pItem->GetType() )
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
	}

	m_dwRotateCheckTime = FRAMEGETTIME();

	ShowWnd();
}

void ItemCustomThumbnailWnd::OnProcess( float fTimePerSec )
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

void ItemCustomThumbnailWnd::OnRectRender( int iXPos, int iYPos )
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

void ItemCustomThumbnailWnd::OnRender()
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
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, STR(1) );
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
				g_FontMgr.PrintText( iXPos + 28, iYPos + 10, FONT_SIZE_11, STR(2) );
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ItemCustomGlobalDetailViewWnd::ItemCustomGlobalDetailViewWnd()
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

ItemCustomGlobalDetailViewWnd::~ItemCustomGlobalDetailViewWnd()
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

void ItemCustomGlobalDetailViewWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void ItemCustomGlobalDetailViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void ItemCustomGlobalDetailViewWnd::CreateCharacter( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom )
{
	HideWnd();
	SAFEDELETE( m_CustomChar1 );
	SAFEDELETE( m_CustomChar2 );
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "custom_cam" );

	if( dwMaleCustom > 0 && dwFemaleCustom > 0 )
	{
		// 남성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		//kCharInfo.m_underwear  = 14;	//속옷 제거

		ITEM_DATA kItemData;
		kItemData.m_item_code = dwItemCode;
		kItemData.m_item_male_custom = dwMaleCustom;
		kItemData.m_item_female_custom = dwFemaleCustom;

		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( -50.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( kItemData.m_item_code, kItemData.m_item_male_custom, kItemData.m_item_female_custom );
		m_CustomChar1->RotateY( -11.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid_2people.txt", NULL );

		// 여성
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		//kCharInfo.m_underwear  = 14;	//속옷 제거

		m_CustomChar2 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar2 )
			return;

		m_CustomChar2->SetPosition( 50.0f, -80.0f, 953.0f );
		m_CustomChar2->SetYaw( FLOAT10 );

		m_CustomChar2->EquipItem( kItemData.m_item_code, kItemData.m_item_male_custom, kItemData.m_item_female_custom );
		m_CustomChar2->RotateY( -11.0f );
		m_CustomChar2->SetMannequins( false );
	}
	else if( dwMaleCustom > 0 )
	{
		// 남성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_MAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		//kCharInfo.m_underwear  = 14;	//속옷 제거

		ITEM_DATA kItemData;
		kItemData.m_item_code = dwItemCode;
		kItemData.m_item_male_custom = dwMaleCustom;
		kItemData.m_item_female_custom = dwFemaleCustom;

		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( 0.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( kItemData.m_item_code, kItemData.m_item_male_custom, kItemData.m_item_female_custom );
		m_CustomChar1->RotateY( -15.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid.txt", NULL );
	}
	else if( dwFemaleCustom > 0 )
	{
		// 여성
		CHARACTER kCharInfo;
		kCharInfo.m_class_type = 1;
		kCharInfo.m_sex        = RDT_HUMAN_WOMAN + 1;
		kCharInfo.m_skin_color = 9;		//회색
		kCharInfo.m_underwear  = 1;     //기본속옷
		//kCharInfo.m_underwear  = 14;	//속옷 제거

		ITEM_DATA kItemData;
		kItemData.m_item_code = dwItemCode;
		kItemData.m_item_male_custom = dwMaleCustom;
		kItemData.m_item_female_custom = dwFemaleCustom;

		m_CustomChar1 = m_pUI3DRender->CreateChar( kCharInfo, "ResultChar" );
		if( !m_CustomChar1 )
			return;

		m_CustomChar1->SetPosition( 0.0f, -80.0f, 950.0f );
		m_CustomChar1->SetYaw( FLOAT10 );

		m_CustomChar1->EquipItem( kItemData.m_item_code, kItemData.m_item_male_custom, kItemData.m_item_female_custom );
		m_CustomChar1->RotateY( -15.0f );
		m_CustomChar1->SetMannequins( false );
		m_CustomChar1->AttachEffect( "ui_grid.txt", NULL );
	}


	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 950.0f );
	float fAspect = (float)(VIEW_WIDTH)/(float)(VIEW_HEIGHT);
	m_pUI3DRender->SetProjection( FLOAT1, 6000.0f, 45.0f, fAspect );	

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemCode, __FUNCTION__ );
	if( pItem )
	{
		if( dwMaleCustom > 0 && dwFemaleCustom > 0 )
		{
			SafeSprintf( m_szSkinName, sizeof(m_szSkinName), STR(1), pItem->GetName().c_str() );
		}
		else if( dwMaleCustom > 0 )
		{
			SafeSprintf( m_szSkinName, sizeof(m_szSkinName), STR(2), pItem->GetName().c_str() );
		}
		else if( dwFemaleCustom > 0 )
		{
			SafeSprintf( m_szSkinName, sizeof(m_szSkinName), STR(3), pItem->GetName().c_str() );
		}

		switch( pItem->GetType() )
		{
		case ioItem::IT_WEAPON:
			{
				if( m_CustomChar1 )
					m_CustomChar1->SetLoopAni( "custom_pose_01.ani" );
				if( m_CustomChar2 )
					m_CustomChar2->SetLoopAni( "custom_pose_01.ani" );				
			}
			break;
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
	}
	ShowWnd();
}

void ItemCustomGlobalDetailViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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

void ItemCustomGlobalDetailViewWnd::OnProcess( float fTimePerSec )
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

void ItemCustomGlobalDetailViewWnd::OnRenderBackground( int iXPos, int iYPos )
{
	if( !m_pBackground ) return;

	int iWidthCnt = 750 / m_pBackground->GetWidth();
	for(int i = 0;i < iWidthCnt;i++)
	{
		m_pBackground->Render( iXPos + 2 + (m_pBackground->GetWidth() * i), iYPos + 6 );
	}
}

void ItemCustomGlobalDetailViewWnd::OnRender()
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
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(1) );

	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );	
	kPrinter.SetTextColor( 255, 188, 26 );
	kPrinter.AddTextPiece( FONT_SIZE_18, STR(2), m_szSkinName );
	kPrinter.PrintFullText( iXPos + 13, iYPos + 11, TAT_LEFT );
	kPrinter.ClearList();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 36, iYPos + 106, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 68, iYPos + 106, FONT_SIZE_12, STR(4) );
	g_FontMgr.PrintText( iXPos + 100, iYPos + 106, FONT_SIZE_12, STR(5) );

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