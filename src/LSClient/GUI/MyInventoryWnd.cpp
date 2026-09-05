#include "StdAfx.h"

#include "../ioAccessoryInfoManager.h"
#include "../ioDateHelp.h"
#include "../ioSubscriptionMgr.h"
#include "../ioSoldierAwakeManager.h"
#include "../ioPowerUpManager.h"
#include "../ioRSoldierInfoManager.h"
#include "../ioBonusCashManager.h"
#include "../Housing/BlockDefine.h"
#include "../ioUserSpiritManager.h"
#include "../ioUserMedalItem.h"

#include "MyInventoryWnd.h"
#include "MyInventoryInfoWnd.h"

#include "GlobalWnd.h"
#include "ioPullDownList.h"
#include "ioGrowthSelectWnd.h"
#include "RandomDecoSelectWnd.h"
#include "ItemCompoundWnd.h"
#include "ItemCompoundResultWnd.h"
#include "ItemMultipleCompoundWnd.h"
#include "ItemMultipleCompoundResultWnd.h"
#include "GrowthDownSelectWnd.h"
#include "GashaponWnd.h"
#include "RandomDecoWnd.h"
#include "ioExtraItemRandomBox.h"
#include "GrowthDownDoneWnd.h"
#include "ItemResellWnd.h"
#include "ChangeNameWnd.h"
#include "RecordInitWnd.h"
#include "UISoldierSelectWnd.h"
#include "ioSpecificDragItem.h"
#include "ChangeTradeStateWnd.h"
#include "ItemCustomWnd.h"
#include "ExpandMedalSlotWnd.h"
#include "ExpandMedalSlotResultWnd.h"
#include "AlchemicFuncWnd.h"
#include "EtcItemSlotWnd.h"
#include "invensoldierselectwnd.h"
#include "InvenFourExtraCompoundWnd.h"
#include "SuperGashaponWnd.h"
#include "MultiSuperGashaponWnd.h"
#include "ItemMaterialCompoundWnd.h"
#include "ItemMaterialCompoundResultWnd.h"
#include "PetEggResultWnd.h"
#include "PetWnd.h"
#include "CostumeCustomWnd.h"
#include "ioCostumeRandomBox.h"
#include "HeroReinforceDownWnd.h"
#include "ItemReinforceDownWnd.h"
#include "PopUpStoreWnd.h"
#include "ItemNewMultipleCompoundWnd.h"
#include "HomeEtcItemNeedPopup.h"
#include "ioHousingBlockInvenWnd.h"
#include "NamedTitleResultWnd.h"
#include "NamedTitlePremiumSelectWnd.h"
#include "SmallTabButton.h"
#include "AccessoryFuncWnd.h"
#include "ExtraItemFuncWnd.h"
#include "SelectGashaponWnd.h"	// 2020-04-08 툴팁

InventoryIconBtn::InventoryIconBtn()
{
	m_pIcon       = NULL;
	m_pSubIcon    = NULL;
	m_pIconBack   = NULL;
	m_pIconSelect = NULL;
	m_pCustomIcon = NULL;
	m_pCostumeMark = NULL;
	m_pEmptyOver = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_pSmallNumText = NULL;
	m_pSmallPlusText = NULL;

	m_pGaugeBack = NULL;
	m_pGaugeImg = NULL;

	m_pStateBlueFrm = NULL;
	m_pStateGreenFrm = NULL;
	m_pStateGreenFrm2 = NULL;
	m_pStateOrangeFrm = NULL;
	m_pStatePinkFrm = NULL;

	m_iMarkType = 0; // INVENTORY_MARK_TYPE_NONE

	m_iNum = 0;
	m_eNumberType = NT_NONE;
	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;

	m_iSexType  = 0;
	m_iDecoType = 0;
	m_iDecoCode = 0;

	m_iGapTime = 0;
	m_iNeedTime = 0;

	m_iClassType= 0;

	m_iEtcItemType = 0;

	m_bEffect = false;
	m_dwCheckTime = 0;

	m_bSelect   = false;

	m_bExtraItemCustom = false;
	m_bExtraItemGrowthCatalyst = false;

	m_pNewShopItemInfo = NULL;

	m_bIsCostume = false;
	m_bRenderGray = false;

	m_nGradeType = 0;
}

InventoryIconBtn::~InventoryIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconSelect );
	SAFEDELETE( m_pCustomIcon );
	SAFEDELETE( m_pCostumeMark );
	SAFEDELETE( m_pEmptyOver );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pSmallNumText );
	SAFEDELETE( m_pSmallPlusText );

	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGaugeImg );

	SAFEDELETE( m_pStateBlueFrm );
	SAFEDELETE( m_pStateGreenFrm );
	SAFEDELETE( m_pStateGreenFrm2 );
	SAFEDELETE( m_pStateOrangeFrm );
	SAFEDELETE( m_pStatePinkFrm );

	SAFEDELETE( m_pNewShopItemInfo );
}

void InventoryIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconSelect" )
	{
		SAFEDELETE( m_pIconSelect );
		m_pIconSelect = pImage;
	}
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pCustomIcon );
		m_pCustomIcon = pImage;
	}
	else if( szType == "costume_image" )
	{
		SAFEDELETE( m_pCostumeMark );
		m_pCostumeMark = pImage;
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
	else if( szType == "SmallNumText" )
	{
		SAFEDELETE( m_pSmallNumText );
		m_pSmallNumText = pImage;
	}
	else if( szType == "SmallPlusText" )
	{
		SAFEDELETE( m_pSmallPlusText );
		m_pSmallPlusText = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "GaugeImg" )
	{
		SAFEDELETE( m_pGaugeImg );
		m_pGaugeImg = pImage;
	}
	else if( szType == "EmptyOver" )
	{
		SAFEDELETE( m_pEmptyOver );
		m_pEmptyOver = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void InventoryIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateBlueFrm" )
		{
			SAFEDELETE( m_pStateBlueFrm );
			m_pStateBlueFrm = pFrame;
		}
		else if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else if( szType == "StateGreenFrm2" )
		{
			SAFEDELETE( m_pStateGreenFrm2 );
			m_pStateGreenFrm2 = pFrame;
		}
		else if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else if( szType == "StatePinkFrm" )
		{
			SAFEDELETE( m_pStatePinkFrm );
			m_pStatePinkFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void InventoryIconBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_dwEffectTime = xElement.GetIntAttribute_e( "EffectShow" );
	m_dwEffectHide = xElement.GetIntAttribute_e( "EffectHide" );
}

int InventoryIconBtn::GetRenderImageStartHeight( ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax )
{
	if( iMax == iCur ) return 0;
	if( !pImage )	return 0;

	float fGap = (float)iCur / (float)iMax;
	float fGapHeight = (float)iSrcHeight * fGap;
	return pImage->GetOffsetY() + ( (iSrcHeight / 2) - fGapHeight );							
}

void InventoryIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderSelect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void InventoryIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderSelect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderEmptyOver( iXPos, iYPos );
	RenderOtherInfo( iXPos, iYPos );
}

void InventoryIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderSelect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void InventoryIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderSelect( iXPos, iYPos );
	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderSubIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void InventoryIconBtn::RenderBack( int iXPos, int iYPos )
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

void InventoryIconBtn::RenderGaugeIcon( int iXPos, int iYPos, ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax, UIRenderType eType )
{
	int iRealHeight = GetRenderImageStartHeight( pImage, iSrcHeight, iCur, iMax );

	pImage->Render( iXPos, iYPos, UI_RENDER_GRAY );
	if( eType == UI_RENDER_SCREEN )
	{
		pImage->Render( iXPos, iYPos, UI_RENDER_GRAY, TFO_NEAREST, false, UI_RENDER_SCREEN );
	}

	pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, pImage->GetHeight(), UI_RENDER_NORMAL );
	if( eType == UI_RENDER_SCREEN )
	{
		pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, pImage->GetHeight(), eType );
	}

	if( iRealHeight + 2 <= pImage->GetHeight() )
	{
		pImage->SetColor( 0, 0, 0 );
		pImage->RenderHeightCut( iXPos, iYPos, iRealHeight, iRealHeight + 2, UI_RENDER_NORMAL );
		pImage->SetColor( 255, 255, 255 );
	}	
}

void InventoryIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
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
		if( m_pSubIcon )
			m_pSubIcon->SetScale( m_fScale );

		if( COMPARE( m_iEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON1,	ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON100 + 1 ) ||
			COMPARE( m_iEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON101,	ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON300 + 1 ) ||
			COMPARE( m_iEtcItemType, ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON301,	ioEtcItem::EIT_ETC_ITEM_TIME_GASHAPON600 + 1 ) )
		{
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			ioEtcItemTimeGashapon *pTimeGashapon = dynamic_cast< ioEtcItemTimeGashapon * >( g_EtcItemMgr.FindEtcItem( m_iEtcItemType ) );
			if( pUserEtcItem && pTimeGashapon )
			{
				ioUserEtcItem::ETCITEMSLOT kSlot;
				if( pUserEtcItem->GetEtcItem( m_iEtcItemType, kSlot ) )
				{
					if( pTimeGashapon->IsExtendData() && !pTimeGashapon->IsSequenceOrder() )
					{
						RenderGaugeIcon( iNewXPos, iNewYPos, m_pIcon, m_pIcon->GetHeight(), pTimeGashapon->GetRepeatDate(), pTimeGashapon->GetRepeatDate(), eType );
					}
					else
					{
						if( pTimeGashapon->IsSequenceOrder() )
						{
							int iState    = kSlot.GetDateExcludeValue3State();
							int iRealTime = kSlot.GetDateExcludeValue3Time();

							RenderGaugeIcon( iNewXPos, iNewYPos, m_pIcon, m_pIcon->GetHeight(), 
								pTimeGashapon->GetSequenceDate( iState ) - iRealTime, pTimeGashapon->GetSequenceDate( iState ), eType );
						}
						else
						{
							m_pIcon->Render( iNewXPos, iNewYPos, UI_RENDER_GRAY );
							RenderGaugeIcon( iNewXPos, iNewYPos, m_pIcon, m_pIcon->GetHeight(), 
								pTimeGashapon->GetRepeatDate() - kSlot.GetDateExcludeValue2(), pTimeGashapon->GetRepeatDate(), eType );
						}
					}
				}
			}
		}
		else
		{
			UIRenderType eNewType = eType;
			if( HasWndStyle( IWS_INACTIVE ) || m_bRenderGray )
				eNewType = UI_RENDER_GRAY;

			if ( eNewType != UI_RENDER_SCREEN )
				g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

			if( m_fScale != FLOAT1 )
				m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
			else
				m_pIcon->Render( iNewXPos, iNewYPos, eNewType );
		}
	}
}

void InventoryIconBtn::RenderSubIcon( int iXPos, int iYPos, UIRenderType eType )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;
	UIRenderType eNewType = UI_RENDER_NORMAL;
	if( HasWndStyle( IWS_INACTIVE ) || m_bRenderGray )
		eNewType = UI_RENDER_GRAY;

	if( m_pSubIcon )
		m_pSubIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );

	if( m_bExtraItemCustom && m_pCustomIcon )
	{
		iNewXPos = iNewXPos + ( 7.0f * m_fScale );
		iNewYPos = iNewYPos - ( 43.0f * m_fScale );
		m_pCustomIcon->SetScale( m_fScale );
		if( m_fScale != FLOAT1 )
			m_pCustomIcon->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		else
			m_pCustomIcon->Render( iNewXPos, iNewYPos );
	}

	// UI코스튬 관련 (코스튬 마크)
	if( m_bIsCostume && m_pCostumeMark )
	{
		m_pCostumeMark->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) || m_bRenderGray )
			eNewType = UI_RENDER_GRAY;

		if( m_fScale != FLOAT1 )
			m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eNewType, TFO_BILINEAR );
		else
			m_pCostumeMark->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, eNewType );
	}
}

void InventoryIconBtn::RenderEmptyOver( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pEmptyOver && !m_pIcon )
		m_pEmptyOver->Render( iNewXPos, iNewYPos );
}

void InventoryIconBtn::RenderOtherInfo( int iXPos, int iYPos )
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
	if( m_iMarkType == INVENTORY_MARK_TYPE_GROWTH_ENABLE )
	{
		iMarkX = iNewXPos-62;
		iMarkY = iNewYPos-37;

		if( m_pStateGreenFrm )
			m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(1) );
	}
	else if( m_iMarkType == INVENTORY_MARK_TYPE_GROWTH_DISABLE )
	{
		iMarkX = iNewXPos-62;
		iMarkY = iNewYPos-37;

		if( m_pStateGreenFrm )
			m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_GRAY_LIGHT );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(2) );
	}
	else if( m_iMarkType == INVENTORY_MARK_TYPE_EQUIP )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateBlueFrm )
			m_pStateBlueFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+X_TEXT_OFFSET;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(3) );
	}
	else if( m_iMarkType == INVENTORY_MARK_TYPE_TIMEGROWTH )
	{
		iMarkX = iNewXPos-62;
		iMarkY = iNewYPos-37;

		iTextX = iMarkX + 62;
		iTextY = iMarkY + 2;

		enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };

		// Gap Time
		int iHour   = m_iGapTime / HOUR_SEC;
		int iMinute = ( m_iGapTime-(iHour*HOUR_SEC) ) / MINUTE_SEC;
		int iSec = m_iGapTime - (iHour*HOUR_SEC) - (iMinute*MINUTE_SEC);

		iHour = max( 0, iHour );
		iMinute = max( 0, iMinute );
		iSec = max( 0, iSec );

		if( m_iGapTime > MINUTE_SEC )
			SafeSprintf( szName, sizeof(szName), STR(4), iHour, iMinute, iSec );
		else
			SafeSprintf( szName, sizeof(szName), STR(5) );

		float fRate = 0.0f;
		if( m_iNeedTime > 0 )
			fRate = FLOAT1 - (float)m_iGapTime / m_iNeedTime;

		fRate = min( fRate, FLOAT1 );

		int iCurPos = 121 * fRate;

		if( m_pGaugeBack )
			m_pGaugeBack->Render( iMarkX, iMarkY );

		if( m_pGaugeImg )
		{
			m_pGaugeImg->RenderWidthCut( iMarkX+2, iMarkY+2, 0, iCurPos );

			if( m_bEffect )
				m_pGaugeImg->RenderWidthCut( iMarkX+2, iMarkY+2, 0, iCurPos, UI_RENDER_SCREEN );
		}
	}
	else if( m_iMarkType == INVENTORY_MARK_TYPE_TRADE_WAIT )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateOrangeFrm )
			m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(6) );
	}
	else if( m_iMarkType == INVENTORY_MARK_TYPE_TRADE_ENABLE )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateGreenFrm2 )
			m_pStateGreenFrm2->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(7) );
	}
	else if( m_bExtraItemGrowthCatalyst )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStatePinkFrm )
			m_pStatePinkFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(8), m_iNum );
	}

	g_FontMgr.SetTextStyle( TS_BOLD );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	switch( m_eNumberType )
	{
	case NT_SMALL:
		if( m_pSmallNumText && m_pSmallPlusText && m_iNum > 0 )
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

			m_pSmallPlusText->Render( iCurPos, iCurYPos - 3 );

			iCurPos += (PLUS_SIZE-NUM_GAP);

			m_pSmallNumText->RenderNum( iCurPos, iCurYPos - 3, m_iNum, -NUM_GAP );
		}
		break;
	default:
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
}

void InventoryIconBtn::RenderSelect( int iXPos, int iYPos )
{
	if( !m_bSelect )
		return;

	int iNewXPos = iXPos;
	int iNewYPos = iYPos;

	if( m_pIconSelect )
		m_pIconSelect->Render( iNewXPos, iNewYPos );
}

bool InventoryIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int iNum /* = 0 */, InventoryIconBtn::NumberType eType /*= NT_NONE*/ )
{
	m_bExtraItemCustom = false;
	m_bExtraItemGrowthCatalyst = false;
	m_bIsCostume = false;
	m_bRenderGray = false;
	m_nGradeType = 0;
	
	m_iNum = iNum;
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_eNumberType = eType;

	m_iMarkType = 0;
	m_iClassType= 0;

	m_iGapTime = 0;
	m_iNeedTime = 0;

	m_iEtcItemType = 0;

	m_bEffect = false;
	m_dwCheckTime = FRAMEGETTIME();

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	if( !pIcon )
	{
		m_iNum = 0;
		return false;
	}

	m_pIcon = pIcon;

	return true;
}

void InventoryIconBtn::SetSubIcon( ioUIRenderImage *pIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pIcon;
}

void InventoryIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_pParent )
		return;

	if( m_iMarkType == INVENTORY_MARK_TYPE_TIMEGROWTH )
		ProcessTimeGrowth( fTimePerSec );

	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
		{
			DWORD dwID = 0;
			if( m_iClassType != 0 )
			{
				pMouse->SetToolTipID( g_MyInfo.GetPublicID(), m_iClassType );
			}
			else if( m_pParent->GetID() == MyInventoryWnd::ID_INVENTORY_INFO_WND )
			{
				dwID = GetID();
				pMouse->SetToolTipWndID( dwID );
			}
			else if( m_pParent->GetID() == NewShopWnd::ID_BUY_WND )
			{
				dwID = GetID();
				pMouse->SetToolTipWndID( dwID );
			}
			else
			{
				dwID = m_pParent->GetID();
				pMouse->SetToolTipWndID( dwID );
			}
			return;
		}
	}	
}

void InventoryIconBtn::ProcessTimeGrowth( float fTimePerSec )
{
	m_iGapTime = 0;
	m_iNeedTime = 0;
	m_bExtraItemCustom = false;
	m_bIsCostume = false;
	m_bRenderGray = false;

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		// TimeGrowth
		TimeGrowthInfo *pInfo = pLevel->FindTimeGrowthInfo( m_iClassType );
		if( pInfo )
		{
			m_iGapTime = pLevel->GetTimeGrowthRemainTime( m_iClassType );

			int iTimeSlot = pInfo->m_iTimeSlot;
			int iLevel = pLevel->GetGrowthLevelByTimeSlot( m_iClassType, iTimeSlot );

			bool bChar = false;
			if( COMPARE( iTimeSlot, TIG_ATTACK, TIG_DROP+1 ) )
				bChar = true;

			m_iNeedTime = g_GrowthInfoMgr.GetTimeGrowthNeedTime( bChar, iLevel+1 ) * 60;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bEffect )
	{
		if( m_dwCheckTime+m_dwEffectTime < dwCurTime )
		{
			m_bEffect = false;
			m_dwCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwCheckTime+m_dwEffectHide < dwCurTime )
		{
			m_bEffect = true;
			m_dwCheckTime = dwCurTime;
		}
	}
}

void InventoryIconBtn::SetDecoInfo( int iSexType, int iDecoType, int iDecoCode )
{
	m_iSexType  = iSexType;
	m_iDecoType = iDecoType;
	m_iDecoCode = iDecoCode;
	m_bExtraItemCustom = false;
	m_bExtraItemGrowthCatalyst = false;
	m_bIsCostume = false;
	m_bRenderGray = false;
}

void InventoryIconBtn::GetDecoInfo( OUT int &riSexType, OUT int &riDecoType, OUT int &riDecoCode )
{
	riSexType = m_iSexType;
	riDecoType= m_iDecoType;
	riDecoCode= m_iDecoCode;
	m_bExtraItemCustom = false;
	m_bExtraItemGrowthCatalyst = false;
	m_bIsCostume = false;
	m_bRenderGray = false;
}

void InventoryIconBtn::SetEtcItemType( int iEtcItem )
{
	m_iEtcItemType = iEtcItem;
}

void InventoryIconBtn::SetNewShopItemInfo( const NewShopItemInfo &rkNewShopItemInfo )
{
	if( !m_pNewShopItemInfo )
		m_pNewShopItemInfo = new NewShopItemInfo;

	if( m_pNewShopItemInfo )
		*m_pNewShopItemInfo = rkNewShopItemInfo;
	m_bExtraItemCustom = false;
	m_bExtraItemGrowthCatalyst = false;
	m_bIsCostume = false;
	m_bRenderGray = false;
}

void InventoryIconBtn::GetNewShopItemInfo( OUT NewShopItemInfo &rkNewShopItemInfo )
{
	if( !m_pNewShopItemInfo )
		return;
	rkNewShopItemInfo = *m_pNewShopItemInfo;
}
//////////////////////////////////////////////////////////////////////////
InventoryEtcMotionBtn::InventoryEtcMotionBtn()
{
	m_pOrangeNor	= NULL;
	m_pOrangePush	= NULL;
	m_pGrayNor		= NULL;
	m_pGrayPush		= NULL;
	m_pCurrentOver  = NULL;
	m_pCurrentNor	= NULL;
	m_pCurrentPush	= NULL;

	m_dwEtcItem	= 0;
	m_bSelected	= 0;
}

InventoryEtcMotionBtn::~InventoryEtcMotionBtn()
{
	SAFEDELETE( m_pOrangeNor );
	SAFEDELETE( m_pOrangePush );
	SAFEDELETE( m_pGrayNor );
	SAFEDELETE( m_pGrayPush );
	SAFEDELETE( m_pCurrentOver );
}

void InventoryEtcMotionBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "OrangeNor" )
	{
		SAFEDELETE( m_pOrangeNor );
		m_pOrangeNor = pFrame;
	}
	else if( szType == "OrangePush" )
	{
		SAFEDELETE( m_pOrangePush );
		m_pOrangePush = pFrame;
	}
	else if( szType == "GrayNor" )
	{
		SAFEDELETE( m_pGrayNor );
		m_pGrayNor = pFrame;
	}
	else if( szType == "GrayPush" )
	{	
		SAFEDELETE( m_pGrayPush );
		m_pGrayPush = pFrame;
	}
	else if( szType == "CurrentOver" )
	{
		SAFEDELETE( m_pCurrentOver );
		m_pCurrentOver = pFrame;
	}
	else 
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void InventoryEtcMotionBtn::SetEtcCode( DWORD dwCode )
{
	if( !COMPARE( dwCode, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) &&
		!COMPARE( dwCode, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
		return;

	m_dwEtcItem = dwCode;
	m_szTitle.Clear();
	
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( m_dwEtcItem, kSlot ) )
		{
			int iUseType = kSlot.GetUse();
			if( COMPARE( iUseType, ioEtcItemMotion::MOTION_OPTION_1, ioEtcItemMotion::MAX_OPTION ) )
			{
				ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( m_dwEtcItem ) );
				if( pEtcMotion )
				{					
					m_szTitle = pEtcMotion->GetMotionOptionTitle( iUseType );
				}
			}
		}
	}

	// 설정된 옵션이 없다.
	if( m_szTitle.IsEmpty() )
	{
		m_szTitle = STR(1);

		m_pCurrentNor = m_pGrayNor;
		m_pCurrentPush= m_pGrayPush;
		m_bSelected   = false;
	}
	else
	{
		// 설정된 옵션이 있음
		m_pCurrentNor = m_pOrangeNor;
		m_pCurrentPush= m_pOrangePush;
		m_bSelected   = true;
	}
}

void InventoryEtcMotionBtn::OnDrawNormal( int iXPos, int iYPos )
{
	if( m_pCurrentNor )
	{
		m_pCurrentNor->Render( iXPos, iYPos );
	}

	OnDrawTitle( iXPos, iYPos );
}

void InventoryEtcMotionBtn::OnDrawOvered( int iXPos, int iYPos )
{
	if( m_pCurrentNor )
	{
		m_pCurrentNor->Render( iXPos, iYPos );
	}	
	
	if( m_pCurrentOver )
	{
		m_pCurrentOver->Render( iXPos, iYPos, UI_RENDER_ADD );
	}

	OnDrawTitle( iXPos, iYPos );
}

void InventoryEtcMotionBtn::OnDrawPushed( int iXPos, int iYPos )
{
	if( m_pCurrentPush )
	{
		m_pCurrentPush->Render( iXPos, iYPos );
	}	

	OnDrawTitle( iXPos, iYPos );
}

void InventoryEtcMotionBtn::OnDrawTitle( int iXPos, int iYPos )
{
	if( !m_bSelected )
	{
		iXPos += (GetWidth() / 2);
		iYPos += 3;

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetBkColor( 0, 0, 0 );
	}
	else
	{
		iXPos += (GetWidth() / 2);
		iYPos += 1;

		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.SetBkColor( 190, 101, 1 );
	}
	g_FontMgr.PrintText( iXPos, iYPos, FONT_SIZE_11, m_szTitle.c_str() );
}
/////////////////////////////////////////////////////////////////////////
InventoryPresentBtn::InventoryPresentBtn()
{
	m_iPresentBtnType = IPBT_RECV;
}

InventoryPresentBtn::~InventoryPresentBtn()
{
}

void InventoryPresentBtn::SetPresentBtnType( int iType )
{
	m_iPresentBtnType = iType;
}
/////////////////////////////////////////////////////////////////////////
InventoryItemWnd::InventoryItemWnd()
{
	m_bEquip = false;
	m_iMainType = 0;
	m_iMagicCode = 0;
	m_iMagicCodeEx = 0;

	m_iDecoType = 0;
	m_iDecoCode = 0;

	m_pEmptyImg = NULL;
	m_pSelectFrm = NULL;
	m_bSelect    = false;

	m_dwCurIconActionID = INVENTORY_ACTION_NONE;
	m_szCurActionText.Clear();
	m_iCurXGap = 0;
	m_iCurYGap = 0;
	m_pCurActionImg = NULL;

	m_pPlusImg = NULL;
	m_pMinusImg = NULL;
	m_pLeftImg = NULL;
	m_pPlayStage = NULL;

	// 2019-06-04
	m_nSelectItemIndex = -1;
	m_bSelectPos = false;
	m_pSelectPosFrm = NULL;
	m_pCompleteSelectPosFrm = NULL;
	m_bCompleteSelectPos = false;
	m_dwCompleteSelectTimer = 0;
	m_dwOverTicTime = 0;
	m_byOverFrmPrintCount = 0;
	m_bCompleteActive = false;
	m_bKeyActive = false;
}

InventoryItemWnd::~InventoryItemWnd()
{
	ClearDesc();

	m_pCurActionImg = NULL;

	SAFEDELETE(m_pEmptyImg);
	SAFEDELETE(m_pSelectFrm);

	SAFEDELETE(m_pPlusImg);
	SAFEDELETE(m_pMinusImg);
	SAFEDELETE(m_pLeftImg);

	// 2019-06-04
	SAFEDELETE(m_pSelectPosFrm);
	SAFEDELETE(m_pCompleteSelectPosFrm);
	m_nSelectItemIndex = -1;
	m_bSelectPos = false;
	m_bCompleteSelectPos = false;
	m_dwCompleteSelectTimer = 0;
	m_dwOverTicTime = 0;
	m_byOverFrmPrintCount = 0;
	m_bCompleteActive = false;
	m_bKeyActive = false;
}

void InventoryItemWnd::iwm_hide()
{
	// 2019-04-30
	ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
	if( NULL != pExtraItemWnd )
	{
		pExtraItemWnd->SetRollingStopSound();
	}

	for ( int i = ID_ICON_BTN; i < MAX_BTN ; i++ )
	{
		HideChildWnd( i );
	}

	ItemCustomThumbnailTooltip *pThumbnailTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( MY_INVENTORY_CUSTOM_TOOLTIP ));
	if( pThumbnailTooltip && pThumbnailTooltip->IsShow() )
	{
		pThumbnailTooltip->HideWnd();
	}

	CostumeCustomThumbnailTooltip *pCostumeThumbnailTooltip = dynamic_cast<CostumeCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( MY_INVENTORY_COSTUME_CUSTOM_TOOLTIP ));
	if( pCostumeThumbnailTooltip && pCostumeThumbnailTooltip->IsShow() )
		pCostumeThumbnailTooltip->HideWnd();
}

void InventoryItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ICON_BTN:
	case ID_EQUIP_BTN:
	case ID_RELEASE_BTN:
		if( cmd == IOBN_BTNUP ||
			cmd == IOWN_OVERED||
			cmd == IOWN_LEAVED )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, m_dwCurIconActionID );
		}
		break;
	case ID_RESELL_BTN:
	case ID_RESELL_MEDAL_BTN:
		if( cmd == IOBN_BTNUP ||
			cmd == IOWN_OVERED||
			cmd == IOWN_LEAVED )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, INVENTORY_ACTION_RESELL );
		}
		break;
	case ID_DISASSEMBLE_BTN:
		if( cmd == IOBN_BTNUP ||
			cmd == IOWN_OVERED||
			cmd == IOWN_LEAVED )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, INVENTORY_ACTION_DISASSEMBLE );
		}
		break;
	case ID_USE_ON_BTN:
	case ID_USE_OFF_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			SendEtcItemOnOff( dwID );
			if( GetParent() )
				GetParent()->iwm_command( this, IOBN_BTNUP, m_dwCurIconActionID );
		}
		break;
	case ID_CUSTOM_INFO_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, INVENTORY_ACTION_CUSTOM_INFO );
		}
		break;
	case ID_ETC_MOTION_BTN:
		if( cmd == IOBN_BTNUP )
		{
			EtcMotionSelectWnd *pSelectWnd = dynamic_cast< EtcMotionSelectWnd * >( g_GUIMgr.FindWnd( ETC_MOTION_SELECT_WND ) );
			if( pSelectWnd )
			{
				pSelectWnd->ShowSelectWnd( pWnd );
			}
		}
		break;

	case ID_ETC_SLOT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			EtcItemSlotWnd *pSlotWnd = dynamic_cast< EtcItemSlotWnd * >( g_GUIMgr.FindWnd( ETC_ITEM_SLOT_WND ) );
			if(pSlotWnd->IsShow())
				pSlotWnd->GoToTop();
			else
				pSlotWnd->ShowSlotWnd();
		}
		break;
	case ID_PRESENT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			InventoryPresentBtn *pPresentBtn = dynamic_cast< InventoryPresentBtn * >( FindChildWnd(ID_PRESENT_BTN) );
			if( pPresentBtn )
			{
				MyInventoryWnd *pInven = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );

				if( pInven )
				{
					pInven->iwm_command( this, cmd, m_dwCurIconActionID );

					if( pPresentBtn->GetButtonType() == InventoryPresentBtn::IPBT_RECV )
					{
						pInven->CheckDefaultActionBtn();
					}
				}
			}
		}
		break;
	case ID_ADDED_BTN:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioPullDownList *pList = dynamic_cast<ioPullDownList *>( FindChildWnd( ID_POWERUP_LIST ) );
				if( pList )
				{
					pList->OpenList( pWnd );
					g_PowerUpManager.GetMenuList( pList );
				}
			}
		}
		break;
	}
}

void InventoryItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pEmptyImg );
		m_pEmptyImg = pImage;
	}
	else if( szType == "plus_image" )
	{
		SAFEDELETE( m_pPlusImg );
		m_pPlusImg = pImage;
	}
	else if( szType == "minus_image" )
	{
		SAFEDELETE( m_pMinusImg );
		m_pMinusImg = pImage;
	}
	else if( szType == "left_image" )
	{
		SAFEDELETE( m_pLeftImg );
		m_pLeftImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void InventoryItemWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "SelectFrm" )
		{
			SAFEDELETE( m_pSelectFrm );
			m_pSelectFrm = pFrame;
		}
		else if( szType == "SelectPosFrm" )	// 2019-06-04
		{
			SAFEDELETE( m_pSelectPosFrm );
			m_pSelectPosFrm = pFrame;
		}
		else if( szType == "CompleteSelectPosFrm" )
		{
			SAFEDELETE( m_pCompleteSelectPosFrm );
			m_pCompleteSelectPosFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void InventoryItemWnd::SetBlank()
{
	m_ItemInfo.Init();

	m_bEquip = false;
	m_bActive = true;
	m_iMainType = 0;
	m_iMagicCode = 0;
	m_iMagicCodeEx = 0;

	m_iDecoType = 0;
	m_iDecoCode = 0;

	m_dwCurIconActionID = INVENTORY_ACTION_NONE;
	m_szCurActionText.Clear();
	m_iCurXGap = 0;
	m_iCurYGap = 0;
	m_pCurActionImg = NULL;

	SetInActive();

	for (int i = ID_ICON_BTN; i < MAX_BTN ; i++)
	{
		HideChildWnd( i );
	}

	ClearDesc();
}

void InventoryItemWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

bool InventoryItemWnd::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_INFO_BTN ) )
		return true;
	return false;
}

void InventoryItemWnd::OnRender()
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

		// 2019-06-25
		if( (true == m_bSelectPos) && (NULL != m_pSelectPosFrm) )
		{
			m_pSelectPosFrm->Render( iXPos, iYPos );
		}

		// 2019-06-25
		if( (true == m_bCompleteSelectPos) && (true == m_bCompleteActive) )
		{
			m_pCompleteSelectPosFrm->Render( iXPos, iYPos );
		}

		if( m_pMultiplyFrm )
			m_pMultiplyFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

		if( m_bSelect )
		{
			if( m_pSelectFrm )
				m_pSelectFrm->Render( iXPos, iYPos );
		}

		if( !m_szCurActionText.IsEmpty() && m_pCurActionImg )
		{
			int iImgSize = m_pCurActionImg->GetWidth();
			int iTextSize = g_FontMgr.GetTextWidth( m_szCurActionText.c_str(), TS_NORMAL, FONT_SIZE_12 );

			int iTotalSize = iTextSize + m_iCurXGap + iImgSize;
			int iHalfSize = iTotalSize / 2;

			int iNewXPos = iXPos + 52;
			int iNewYPos = iYPos + 157;

			int iCurXPos = iNewXPos;

			// 텍스트
			iCurXPos -= iHalfSize;
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iCurXPos, iNewYPos, FONT_SIZE_12, STR(1), m_szCurActionText.c_str() );

			// 이미지
			iCurXPos += (iTextSize + m_iCurXGap);
			m_pCurActionImg->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
			m_pCurActionImg->Render( iCurXPos, iNewYPos+m_iCurYGap );
		}
	}

	RenderDesc( iXPos, iYPos );
}

void InventoryItemWnd::RenderDesc( int iXPos, int iYPos )
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

void InventoryItemWnd::SetInfo( ioPlayStage *pPlayStage, InventoryItemInfo &rkInfo )
{
	SetBlank();
	
	m_pPlayStage = pPlayStage;

	ioHashString szName, szIcon;

	m_bEquip = rkInfo.m_bEquip;
	m_bActive = rkInfo.m_bActive;

	m_iMainType = rkInfo.m_iMainType;
	m_iMagicCode = rkInfo.m_iMagicCode;
	m_iMagicCodeEx = rkInfo.m_iMagicCodeEx;

	SetActive();

	switch( m_iMainType )
	{
	case INVENTORY_ITEM_EXTRAITEM:
		{
			if( !SetExtraItemInfo() )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_DECO:
		{
			if( !SetDecoItemInfo( rkInfo ) )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_ETC:
		{
			if( !SetEtcItemInfo() )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_PRESENT:
		{
			if( !SetPresentItemInfo() )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_MEDALITEM:
		{
			if( !SetMedalItemInfo() )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_SUBSCRIPTION:
		{
			if( !SetSubscriptionItemInfo( rkInfo ) )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_COSTUME:
		{
			// UI코스튬 관련 (정보 확인)
			if( !SetCostumeInfo() )
			{
				SetBlank();
				return;
			}
		}
		break;
	case INVENTORY_ITEM_ACCESSORY:
		if( !SetAccessoryInfo() )
		{
			SetBlank();
			return;
		}
		break;
	default:
		SetBlank();
		return;
	}

	m_ItemInfo = rkInfo;
}

void InventoryItemWnd::SetSelect( bool bSelect )
{
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM ||
		m_iMainType == INVENTORY_ITEM_MEDALITEM ||
		m_iMainType == INVENTORY_ITEM_DECO ||
		m_iMainType == INVENTORY_ITEM_COSTUME ||
		m_iMainType == INVENTORY_ITEM_ACCESSORY )
	{
		m_bSelect = false;
	}
	else
	{
		m_bSelect = bSelect;
	}
}

InventoryItemInfo& InventoryItemWnd::GetInfo()
{
	return m_ItemInfo;
}

bool InventoryItemWnd::SetExtraItemInfo()
{
	if( m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	if( !pExtraItem->GetExtraItem( m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	if( m_bEquip )
	{
		ShowChildWnd( ID_RELEASE_BTN );

		m_dwCurIconActionID = INVENTORY_ACTION_RELEASE;
	}
	else
	{
		ioHashString szError;
		if ( g_PowerUpManager.CheckEnableItemPowerUp( kSlot.m_iIndex, szError ) )
		{
			ShowChildWnd( ID_ADDED_BTN );
		}
		else
		{
			ShowChildWnd( ID_RESELL_BTN );
			ShowChildWnd( ID_DISASSEMBLE_BTN );
		}
		m_dwCurIconActionID = INVENTORY_ACTION_EQUIP;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		ShowChildWnd( ID_CUSTOM_INFO_BTN );
	}

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );

		if( m_bEquip )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_EQUIP );
			pBtn->SetInActive();
		}
		else if( kSlot.m_iTradeState == EET_NORMAL && kSlot.m_PeriodType == EPT_MORTMAIN )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_WAIT );
			pBtn->SetActive();
		}
		else if( kSlot.m_iTradeState == EET_ENABLE && kSlot.m_PeriodType == EPT_MORTMAIN )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_ENABLE );
			pBtn->SetActive();
		}
		else
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}

		if( kSlot.m_PeriodType == EPT_GROW_TIME )
			pBtn->SetExtraItemGrowthCatalyst( true );
	}
	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(1), kSlot.m_Name.c_str() );

	// Limit
	int iLimitLevel = kSlot.m_iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iEnableClass = 0;
	int iOnlySexType = ioSetItemInfo::CST_ALL;
	int iPowerUpItemGrade = ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode );

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if( pItem )
	{
		iEnableClass = pItem->GetEnableClass();
		iOnlySexType = pItem->GetCharOnlySexType();

		if ( pBtn )
			pBtn->SetGradeType( pItem->GetGradeType() );
	}

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( iEnableClass > 0 )
	{
		switch( iOnlySexType )
		{
		case ioSetItemInfo::CST_ALL:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(8), g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade ) ) );
			break;
		case ioSetItemInfo::CST_MALE:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(4), iLimitLevel, g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade )  ) );
			break;
		case ioSetItemInfo::CST_FEMALE:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(5), iLimitLevel, g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iPowerUpItemGrade )  ) );
			break;
		}
	}
	else
	{
		switch( iOnlySexType )
		{
		case ioSetItemInfo::CST_ALL:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(3), iLimitLevel );
			break;
		case ioSetItemInfo::CST_MALE:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(6), iLimitLevel );
			break;
		case ioSetItemInfo::CST_FEMALE:
			m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(7), iLimitLevel );
			break;
		}
	}
	return true;
}

bool InventoryItemWnd::SetDecoItemInfo( InventoryItemInfo &rkInfo )
{
	int iClassType = g_MyInfo.GetClassType();
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
	if( pInvenWnd )
	{
		iClassType = pInvenWnd->GetSelectClassType();
	}

	int iSexType = rkInfo.m_iSexType;
	m_iDecoType = rkInfo.m_iDecoType;
	m_iDecoCode = rkInfo.m_iDecoCode;

	const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( iSexType, m_iDecoType, m_iDecoCode );
	if( !pData ) return false;

	// icon btn
	ioUIRenderImage *pImg = NULL;
	ioUIRenderImage *pSubImg = NULL;
	// 종족 아이콘은 용병 아이콘을 사용한다.
	if( m_iDecoType == UID_KINDRED )
	{
		bool bMale = true;
		// 여성 캐릭터 타입
		switch( m_iDecoCode )
		{
		case RDT_HUMAN_WOMAN:
		case RDT_ELF_WOMAN:
		case RDT_DWARF_WOMAN:
			bMale = false;
			break;
		}

		pImg = g_MyInfo.GetSoldierIcon( iClassType, bMale );
		pSubImg = g_MyInfo.GetSoldierSubIcon( iClassType );
	}
	else
	{
		pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
	}

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		if( m_bEquip )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_EQUIP );
			pBtn->SetInActive();
		}
		else
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}

		ShowChildWnd( ID_ICON_BTN );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}

	if( m_bEquip )
	{
		m_szCurActionText = STR(1);
		m_pCurActionImg = m_pMinusImg;

		m_iCurXGap = 0;
		m_iCurYGap = -1;

		m_dwCurIconActionID = INVENTORY_ACTION_RELEASE;
	}
	else
	{
		if( m_iDecoType == UID_KINDRED )
		{
			m_szCurActionText = STR(2);
			m_pCurActionImg = m_pPlusImg;
		}
		else
			ShowChildWnd( ID_RESELL_MEDAL_BTN );

		m_iCurXGap = 0;
		m_iCurYGap = -1;

		m_dwCurIconActionID = INVENTORY_ACTION_EQUIP;
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, pData->m_szName.c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, STR(3) );

	return true;
}

bool InventoryItemWnd::SetMedalItemInfo()
{
	int iMedalItemType = m_iMagicCode;
	int iCustomIndex = m_iMagicCodeEx;
	if( iMedalItemType <= 0 )
		return false;

	ioUserMedalItem *pMedaltem = g_MyInfo.GetUserMedalItem();
	if( !pMedaltem )
		return false;

	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( !pMedaltem->GetMedalItem( iMedalItemType, iCustomIndex , kSlot ) )
		return false;

	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iMedalItemType );
	if( !pItemInfo )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	if( m_bEquip )
	{
		m_dwCurIconActionID = INVENTORY_ACTION_RELEASE;

		m_szCurActionText = STR(1);
		m_pCurActionImg = m_pMinusImg;

		m_iCurXGap = 0;
		m_iCurYGap = -1;
	}
	else
	{
		m_dwCurIconActionID = INVENTORY_ACTION_EQUIP;

		m_pCurActionImg = m_pPlusImg;

		m_iCurXGap = 0;
		m_iCurYGap = -1;

		ShowChildWnd( ID_RESELL_MEDAL_BTN );
	}

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		ioUIRenderImage *pSubImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
		pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, 0 );
		pBtn->SetSubIcon( pSubImg );

		if( m_bEquip )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_EQUIP );
			pBtn->SetInActive();
		}
		else
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(3), pItemInfo->m_sName.c_str() );

	// Limit
	int iLimitLevel = g_MedalItemMgr.GetLevelLimit( iMedalItemType );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(4), iLimitLevel );
	return true;
}

int InventoryItemWnd::GetDecoType()
{
	if( m_iMainType == INVENTORY_ITEM_DECO )
		return m_iDecoType;

	return 0;
}

int InventoryItemWnd::GetDecoCode()
{
	if( m_iMainType == INVENTORY_ITEM_DECO )
		return m_iDecoCode;

	return 0;
}

bool InventoryItemWnd::SetEtcItemInfo()
{
	if( m_iMagicCode <= 0 )
		return false;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iMagicCode );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s ioEtcItem == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
		return false;
	}

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kItemSlot;
	if( !pUserEtcItem->GetEtcItem( m_iMagicCode, kItemSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Type:%d", __FUNCTION__, m_iMagicCode );
		return false;
	}

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
	}
	else
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( pEtcItem->GetGradeType() );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
		pBtn->SetEtcItemType( m_iMagicCode );
	}

	ShowChildWnd( ID_ICON_BTN );
	m_dwCurIconActionID = INVENTORY_ACTION_DETAIL;

	if( pEtcItem->IsCanOnOff() )
	{
		ShowChildWnd( ID_USE_ON_BTN );
		ShowChildWnd( ID_USE_OFF_BTN );
		if( kItemSlot.IsUse() )
			CheckRadioButton( ID_USE_ON_BTN, ID_USE_OFF_BTN, ID_USE_ON_BTN );
		else
			CheckRadioButton( ID_USE_ON_BTN, ID_USE_OFF_BTN, ID_USE_OFF_BTN );
	}
	else if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) ||
		     COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) )
	{
		ioEtcItemMotion *pEtcMotion = dynamic_cast< ioEtcItemMotion * > ( pEtcItem );
		if( pEtcMotion->GetMaxMotionOption() > 0 )
		{
			InventoryEtcMotionBtn *pMotionBtn = dynamic_cast< InventoryEtcMotionBtn * >( FindChildWnd( ID_ETC_MOTION_BTN ) );
			if( pMotionBtn )
			{
				pMotionBtn->SetEtcCode( pEtcItem->GetType() );
				pMotionBtn->ShowWnd();
			}
		}
		else
		{
			m_szCurActionText = STR(1);
			m_pCurActionImg = m_pLeftImg;		
		}
	}
	else if( COMPARE( pEtcItem->GetType(), ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE + 1 ) )
	{
		ioButton *pSlotBtn = dynamic_cast< ioButton * >( FindChildWnd( ID_ETC_SLOT_BTN ) );
		if( pSlotBtn )
			pSlotBtn->ShowWnd();
	}
	else
	{
		m_szCurActionText = STR(1);
		m_pCurActionImg = m_pLeftImg;		
	}

	m_iCurXGap = 2;
	m_iCurYGap = 1;

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, pEtcItem->GetName().c_str() );
	return true;
}

void InventoryItemWnd::SendEtcItemOnOff( DWORD dwID )
{
	if( m_iMagicCode <= 0 )
		return;

	CheckRadioButton( ID_USE_ON_BTN, ID_USE_OFF_BTN, dwID );

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iMagicCode );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s ioEtcItem == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	if( pEtcItem->IsCanOnOff() )
	{
		if( dwID == ID_USE_ON_BTN )
		{
			if( !pEtcItem->OnUseSwitchBtnPush( g_MyInfo.GetUserEtcItem(), true, m_pPlayStage ) )
			{
				CheckRadioButton( ID_USE_ON_BTN, ID_USE_OFF_BTN, ID_USE_OFF_BTN );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
		}
		else
		{
			if( !pEtcItem->OnUseSwitchBtnPush( g_MyInfo.GetUserEtcItem(), false, m_pPlayStage ) )
			{
				CheckRadioButton( ID_USE_ON_BTN, ID_USE_OFF_BTN, ID_USE_ON_BTN );
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			}
		}
	}
}

bool InventoryItemWnd::SetPresentItemInfo()
{
	if( m_iMagicCode <= 0 && m_iMagicCodeEx <= 0 )
		return false;

	ioHashString szSendID;
	short sPresentType, sPresentMent;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
	DWORD dwLimitDate;

	ShowChildWnd( ID_ICON_BTN );
	m_dwCurIconActionID = INVENTORY_ACTION_DETAIL;

	// New
	ShowChildWnd( ID_PRESENT_BTN );

	m_szCurActionText.Clear();
	m_pCurActionImg = NULL;

	m_iCurXGap = 2;
	m_iCurYGap = 1;

	g_PresentMgr.GetPresentDataToIndex( m_iMagicCode, m_iMagicCodeEx, szSendID, sPresentType, sPresentMent,
										iPresentValue1,	iPresentValue2,	iPresentValue3,	iPresentValue4,	dwLimitDate );

	switch( sPresentType )
	{
	case PRESENT_SOLDIER:
		SetPresentSoldier( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_DECORATION:
		SetPresentDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ETC_ITEM:
		SetPresentEtcItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_PESO:
		SetPresentPeso( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_EXTRAITEM:
		{
			SetPresentExtraItem( iPresentValue1, iPresentValue2, sPresentMent );
			if( iPresentValue3 > 0 || iPresentValue4 > 0 )
			{
				ShowChildWnd( ID_CUSTOM_INFO_BTN );
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		SetPresentExtraItemBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_RANDOM_DECO:
		SetPresentRandomDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_GRADE_EXP:
		SetPresentGradeExp( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_MEDALITEM:
		SetPresentMedalItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		SetPresentPet( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_COSTUME:
		{
			SetPresentCostume( iPresentValue1, iPresentValue2 );
			if( iPresentValue3 > 0 || iPresentValue4 > 0 )
			{
				ShowChildWnd( ID_CUSTOM_INFO_BTN );
			}
		}
		break;
	case PRESENT_COSTUME_BOX:
		SetPresentCostumeBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_BONUS_CASH:
		SetPresentBonusCash( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ACCESSORY:
		SetPresentAccessory( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_SPIRIT:
		SetPresentSpiritItem( iPresentValue1, iPresentValue2 );
		break;
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 1, true );

	// 첫 라인 : 이름
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );

	// 두번째 라인 : 기간
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );

	char szUseInfo[MAX_PATH]="";
	if( sPresentType == PRESENT_MEDALITEM )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), STR(2),  g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	else if( sPresentType == PRESENT_PET )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), "Lv %d %s", iPresentValue2/10000, g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	else
		StringCbCopy( szUseInfo, sizeof( szUseInfo ), g_PresentMgr.GetPresentValue2Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str() );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_11,  szUseInfo );

	// 세번째 라인 : 선물 기간
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, g_PresentMgr.GetLmitDateText( dwLimitDate ).c_str() );

	return true;
}

bool InventoryItemWnd::SetSubscriptionItemInfo( InventoryItemInfo &rkInfo )
{
	if( m_iMagicCode <= 0 && m_iMagicCodeEx <= 0 )
		return false;

	ioHashString szSendID;
	short sPresentType, sPresentMent;
	int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;

	ShowChildWnd( ID_ICON_BTN );
	m_dwCurIconActionID = INVENTORY_ACTION_DETAIL;

	m_szCurActionText = "자세히보기";
	m_pCurActionImg = m_pLeftImg;

	m_iCurXGap = 2;
	m_iCurYGap = 1;

	ioSubscriptionMgr::SubscriptionData kData;
	g_SubscriptionMgr.GetSubscriptionDataToIndex( rkInfo.m_iMagicCode, rkInfo.m_szSubscriptionID, kData );

	sPresentType = kData.m_iPresentType;
	iPresentValue1 = kData.m_iPresentValue1;
	iPresentValue2 = kData.m_iPresentValue2;
	iPresentValue3 = iPresentValue4 = 0;
	sPresentMent = kData.m_iMentType;

	ioHashString szValueText1 = g_PresentMgr.GetPresentValue1Text( kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );
	ioHashString szValueText2 = g_PresentMgr.GetPresentValue2Text( kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2 );

	switch( sPresentType )
	{
	case PRESENT_SOLDIER:
		SetPresentSoldier( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_DECORATION:
		SetPresentDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ETC_ITEM:
		SetPresentEtcItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_PESO:
		SetPresentPeso( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_EXTRAITEM:
		{
			SetPresentExtraItem( iPresentValue1, iPresentValue2, sPresentMent );
			if( iPresentValue3 > 0 || iPresentValue4 > 0 )
			{
				ShowChildWnd( ID_CUSTOM_INFO_BTN );
			}
		}
		break;
	case PRESENT_EXTRAITEM_BOX:
		SetPresentExtraItemBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_RANDOM_DECO:
		SetPresentRandomDeco( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_GRADE_EXP:
		SetPresentGradeExp( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_MEDALITEM:
		SetPresentMedalItem( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_ALCHEMIC_ITEM:
		break;
	case PRESENT_PET:
		SetPresentPet( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_COSTUME:
		{
			SetPresentCostume( iPresentValue1, iPresentValue2 );
			if( iPresentValue3 > 0 || iPresentValue4 > 0 )
			{
				ShowChildWnd( ID_CUSTOM_INFO_BTN );
			}
		}
		break;
	case PRESENT_COSTUME_BOX:
		SetPresentCostumeBox( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_BONUS_CASH:
		SetPresentBonusCash( iPresentValue1, iPresentValue2 );
		break;
	case PRESENT_SPIRIT:
		SetPresentSpiritItem( iPresentValue1, iPresentValue2 );
		break;
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 1, true );

	// 첫 라인 : 이름
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, szValueText1.c_str() );

	// 두번째 라인 : 기간
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );

	char szUseInfo[MAX_PATH]="";
	if( sPresentType == PRESENT_MEDALITEM )
		SafeSprintf( szUseInfo, sizeof( szUseInfo ), "[메달] %s",  szValueText2.c_str() );
	else
		StringCbCopy( szUseInfo, sizeof( szUseInfo ), szValueText2.c_str() );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_11, szUseInfo );

	// 세번째 라인 : 선물 기간
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "구매한 아이템" );

	return true;
}

// UI코스튬 관련 (코스튬 인벤토리 해당 아이템 버튼 관련)
bool InventoryItemWnd::SetCostumeInfo()
{
	if( m_iMagicCode <= 0 )
		return false;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
		return false;

	if( !pCostume->GetCostumeSlot( m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	if( m_bEquip )
	{
		ShowChildWnd( ID_RELEASE_BTN );

		m_dwCurIconActionID = INVENTORY_ACTION_RELEASE;
	}
	else
	{
		ShowChildWnd( ID_RESELL_MEDAL_BTN );

		m_dwCurIconActionID = INVENTORY_ACTION_EQUIP;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		ShowChildWnd( ID_CUSTOM_INFO_BTN );
	}

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, 0 );
		pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( kSlot.m_nCostumeCode ) );
		pBtn->SetCostume( true );

		if( m_bEquip )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_EQUIP );
			pBtn->SetInActive();
		}
		else
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", kSlot.m_Name.c_str() );

	// Limit
	CostumeInfo sCostumeInfo;
	g_CostumeInfoMgr.GetCostumeInfo( kSlot.m_nCostumeCode, sCostumeInfo );

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( sCostumeInfo.m_nEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s 전용", g_MyInfo.GetClassName( sCostumeInfo.m_nEnableClass ) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "장착제한없음" );

	return true;
}

void InventoryItemWnd::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool InventoryItemWnd::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}

void InventoryItemWnd::SetPresentSoldier( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetMySoldierIconName( iValue1 ) );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( iValue1 ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( g_UIImageRenderMgr.GetPowerUpGradeType( iValue1 ) );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentDeco( int iValue1, int iValue2 )
{
	int iClassType = iValue1 / 100000;
	int iSexType   = (iValue1 % 100000) / 1000;
	int iDecoType  = iValue1 % 1000;
	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iValue2 );

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentRandomDeco( int iValue1, int iValue2 )
{
	int iClassType = iValue1 / 100000;
	int iSexType   = (iValue1 % 100000) / 1000;
	int iDecoType  = iValue1 % 1000;

	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, iValue2 );

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentEtcItem( int iValue1, int iValue2 )
{
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iValue1 );
	if( !pEtcItem ) return;

	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetSubIcon( pSubImg );
		pBtn->SetGradeType( pEtcItem->GetGradeType() );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentPeso( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack13#quest_002" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentExtraItem( int iValue1, int iValue2, short sPresentMent )
{
	// icon btn
	const ioItem *pItem = g_ItemMaker.GetItemConst( iValue1, __FUNCTION__ );
	if( pItem )
	{
		ioHashString szIcon = pItem->GetItemLargeIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
			if( pBtn )
			{
				int iReinforce = ( iValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1, iReinforce );
				pBtn->SetGradeType( pItem->GetGradeType() );

				if( iValue2 / PRESENT_EXTRAITEM_DIVISION_1 != 0 )
					pBtn->SetExtraItemGrowthCatalyst( true );

				if( sPresentMent == PRESENT_TRADE_CANCEL_MENT || sPresentMent == PRESENT_TRADE_TIMEOUT_MENT )
				{
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_TRADE_ENABLE );
					pBtn->SetActive();
				}
				else
				{
					pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
					pBtn->SetActive();
				}
			}
		}
	}
}

void InventoryItemWnd::SetPresentExtraItemBox( int iValue1, int iValue2 )
{
	// icon btn
	RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iValue1 );
	if( pInfo )
	{
		ioHashString szIcon = pInfo->GetIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pInfo->GetGradeType() );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void InventoryItemWnd::SetPresentGradeExp( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack13#quest_001" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

void InventoryItemWnd::SetPresentMedalItem( int iValue1, int iValue2 )
{
	// icon btn
	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iValue1 );
	if( pItemInfo )
	{
		ioHashString szIcon = pItemInfo->m_sIcon;
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetSubIcon( g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon ) );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void InventoryItemWnd::SetPresentPet( int iValue1, int iValue2 )
{
	// icon btn
	int nPetRank = iValue2%10000;
	ioHashString szIcon = g_PetInfoMgr.GetPetIcon( iValue1, (PetRankType)nPetRank );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > (FindChildWnd(ID_ICON_BTN));
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

// UI코스튬 관련 (선물 설정)
void InventoryItemWnd::SetPresentCostume( int iValue1, int iValue2 )
{
	ioHashString szIcon = g_CostumeInfoMgr.GetCostumeIcon( iValue1 );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > (FindChildWnd(ID_ICON_BTN));
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( iValue1 ) );
			pBtn->SetCostume( true );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

void InventoryItemWnd::SetPresentCostumeBox( int iValue1, int iValue2 )
{
	// icon btn
	CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iValue1 );
	if( pInfo )
	{
		ioHashString szIcon = pInfo->GetIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pInfo->GetGradeType() );
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void InventoryItemWnd::SetPresentAccessory( int iValue1, int iValue2 )
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( iValue1, __FUNCTION__ );
	if( pItem )
	{
		ioHashString szIcon = pItem->GetItemLargeIconName();
		if( !szIcon.IsEmpty() )
		{
			InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
			if( pBtn )
			{
				pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
				pBtn->SetGradeType( pItem->GetGradeType() );				
				pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pBtn->SetActive();
			}
		}
	}
}

void InventoryItemWnd::SetPresentSpiritItem( int iValue1, int iValue2 )
{
	// icon btn
	ioHashString szIcon = g_SpiritMgr.GetIconName( iValue1 );
	if( !szIcon.IsEmpty() )
	{
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > (FindChildWnd(ID_ICON_BTN));
		if( pBtn )
		{
			pBtn->SetIcon( g_UIImageSetMgr.CreateImageByFullName( szIcon ), FLOAT1, FLOAT1 );
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetActive();
		}
	}
}

void InventoryItemWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessDesc();

	// 2019-06-24
	if( (true == g_Input.IsKeyUp(KEY_SPACE)) && (true == m_bKeyActive) )
	{
		MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
		if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchComplete()) )
		{
			pMyInventoryWnd->SetSearchComplete(false);
			ioWnd *pWnd = GetParent()->FindChildWnd(GetID());
			if( NULL != pWnd )
			{
				m_bCompleteSelectPos = false;
				m_bKeyActive = false;
				GetParent()->iwm_command( pWnd, IOBN_BTNUP, 0 );
				return;
			}
		}
	}

	// 2019-06-24
	if( true == m_bCompleteSelectPos )
	{
		if( FRAMEGETTIME() - m_dwCompleteSelectTimer > m_dwOverTicTime )
		{
			m_dwCompleteSelectTimer = FRAMEGETTIME();
			m_bCompleteActive = !m_bCompleteActive;
			m_bKeyActive = true;
		}		
		return;
	}
	else
	{
		m_bKeyActive = false;
		m_dwCompleteSelectTimer = 0;
	}
}

void InventoryItemWnd::ProcessDesc()
{
	switch( m_iMainType )
	{
	case INVENTORY_ITEM_EXTRAITEM:
		ProcessExtraItemDesc();
		break;
	case INVENTORY_ITEM_ETC:
		ProcessEtcItemDesc();
		break;
	case INVENTORY_ITEM_MEDALITEM:
		ProcessMedalItemDesc();
		break;
		// UI코스튬 관련 (설명 관련)
	case INVENTORY_ITEM_COSTUME:
		ProcessCostumeDesc();
		break;
	case INVENTORY_ITEM_ACCESSORY:
		ProcessAccessoryDesc();
		break;
	}
}

void InventoryItemWnd::ProcessExtraItemDesc()
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserExtraItem == NULL", __FUNCTION__ );
		return;
	}

	ioHashString sInfo;
	pExtraItem->GetUseInfo( m_iMagicCode, sInfo );

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( m_iMagicCode, kSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	if( kSlot.m_PeriodType == EPT_MORTMAIN )
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	else
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, sInfo.c_str() );
}

void InventoryItemWnd::ProcessMedalItemDesc()
{
	ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pMedalItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserMedalItem == NULL", __FUNCTION__ );
		return;
	}

	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( !pMedalItem->GetMedalItem( m_iMagicCode, m_iMagicCodeEx, kSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "InventoryItemWnd::ProcessMedalItemDesc() Not Exist Medal Item : %d", m_iMagicCode );
		return;
	}

	ioHashString sInfo;
	pMedalItem->GetUseInfo( m_iMagicCode, m_iMagicCodeEx, sInfo );
	
	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	if( kSlot.m_iPeriodType == ioUserMedalItem::PT_MORTMAIN )
	{
		m_szDescArray[1].ClearList();
		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, STR(1), sInfo.c_str() );
	}
	else
	{
		m_szDescArray[1].ClearList();
		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "%s", sInfo.c_str() );
	}
}

void InventoryItemWnd::ProcessEtcItemDesc()
{
	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	DWORD sInfo1Color;
	DWORD sInfo2Color;
	ioHashString sInfo1;
	ioHashString sInfo2;
	pUserEtcItem->GetUseInfo( m_iMagicCode, sInfo1, sInfo2, sInfo1Color, sInfo2Color );

	if( !sInfo1.IsEmpty() )
	{
		m_szDescArray[1].ClearList();
		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( sInfo1Color );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, sInfo1.c_str() );
	}

	if( !sInfo2.IsEmpty() )
	{
		m_szDescArray[2].ClearList();
		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( sInfo2Color );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, sInfo2.c_str() );
	}
}

void InventoryItemWnd::ProcessCostumeDesc()
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
	{
		LOG.PrintTimeAndLog( 0, "%s UserCostume == NULL", __FUNCTION__ );
		return;
	}

	ioHashString sInfo;
	pCostume->GetUseInfo( m_iMagicCode, sInfo );

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( m_iMagicCode, kSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	

	if( kSlot.m_PeriodType == PCPT_MORTMAIN )
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	else
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, sInfo.c_str() );
}

void InventoryItemWnd::ProcessToolTip()
{
	if( m_bOver && !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
		{
			pMouse->SetToolTipWndID( m_dwID );
			return;
		}
	}	

	for(int i = ID_ICON_BTN; i < MAX_BTN ; i++)
	{
		ioButton *pButton = dynamic_cast<ioButton*> ( FindChildWnd( i ) );
		if( pButton && pButton->IsOvered() )
		{
			ioLSMouse *pMouse = g_App.GetMouse();
			if( pMouse )
			{
				pMouse->SetToolTipWndID( m_dwID );
				return;
			}
		}
	}
}

bool InventoryItemWnd::QuestGuide( const IntVec &rValue, float &rfXPos, float &rfYPos )
{
	if( rValue.size() < 8 ) return false;

	// MainTabID.SubTabID.Btn(Type.MainCode.SubCode1.SubCode2.SubCode3.SubCode4)
	int iMainCode = rValue[3];

	ioWnd *pGuideBtn = NULL;
	switch( m_iMainType )
	{
	case INVENTORY_ITEM_EXTRAITEM:
		{
			if( m_iMagicCode <= 0 )
				return false;

			EXTRAITEMSLOT kSlot;
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( !pExtraItem )
				return false;

			if( !pExtraItem->GetExtraItem( m_iMagicCode, kSlot ) )
				return false;
			
			if( kSlot.m_iItemCode != iMainCode )
				return false;
            
			pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	case INVENTORY_ITEM_DECO:
		{
			if( iMainCode != g_MyInfo.GetClassType() )
				return false;
			
			if( m_ItemInfo.m_iSexType != rValue[4] )
				return false;

			if( m_ItemInfo.m_iDecoType != rValue[5] )
				return false;

			if( m_ItemInfo.m_iDecoCode != rValue[6] )
				return false;

			pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	case INVENTORY_ITEM_ETC:
		{
			if( m_iMagicCode != iMainCode )
				return false;
            
			pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	case INVENTORY_ITEM_PRESENT:
		{
			if( m_iMagicCode <= 0 )
				return false;

			ioHashString szSendID;
			short sPresentType, sPresentMent;
			int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
			DWORD dwLimitDate;
			g_PresentMgr.GetPresentDataToIndex( m_iMagicCode, m_iMagicCodeEx, szSendID, sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );
			if( (int)sPresentType != iMainCode )
				return false;

			if( iPresentValue1 != rValue[4] )
				return false;

			if( iPresentValue2 != rValue[5] )
				return false;

			pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	case INVENTORY_ITEM_MEDALITEM:
		{
			if( m_iMagicCode == iMainCode )
				pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	case INVENTORY_ITEM_COSTUME:
		{
			// UI코스튬 관련 (퀘스트 가이드)
			if( m_iMagicCode <= 0 )
				return false;

			CostumeSlot kSlot;
			ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
			if( !pCostume )
				return false;

			if( !pCostume->GetCostumeSlot( m_iMagicCode, kSlot ) )
				return false;

			if( kSlot.m_nIndex != iMainCode )
				return false;

			pGuideBtn = FindChildWnd( ID_ICON_BTN );
		}
		break;
	}

	if( pGuideBtn && pGuideBtn->IsShow() )
	{
		rfXPos = pGuideBtn->GetDerivedPosX() + (pGuideBtn->GetWidth() / 2);
		rfYPos = pGuideBtn->GetDerivedPosY();
		return true;
	}

	return false;
}

void InventoryItemWnd::iwm_pulldown_event( const PullDownEvent& Event )
{
	//마우스 오버
	if( CLASSTYPEQUALS( OnOverPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnOverPullDownEvent& OverEvent = dynamic_cast<const OnOverPullDownEvent&>( Event );
		if( OverEvent.IsImageMouseOver() )
			g_PowerUpManager.OnPowerUpMenuToolTipShow( OverEvent );
	}
	//마우스 클릭
	else if( CLASSTYPEQUALS( ClickPullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const ClickPullDownEvent& ClickEvent = dynamic_cast<const ClickPullDownEvent&>( Event );
		if( ClickEvent.IsPulldownClick() )
		{
			g_PowerUpManager.OnPowerUpMenuClick( ClickEvent, m_iMagicCode, this );
		}
		else if( ClickEvent.IsImageClick() )
		{
			g_PowerUpManager.OnPowerUpImageClick( ClickEvent);
		}
	}
	//마우스 리브
	else if( CLASSTYPEQUALS( OnLeavePullDownEvent ) == CLASSTYPEQUALS( Event ) )
	{
		const OnLeavePullDownEvent& LeaveEvent = dynamic_cast<const OnLeavePullDownEvent&>( Event );
		if( LeaveEvent.IsImageMouseLeave() )
			g_PowerUpManager.OnPowerUpMenuToolTipHide( LeaveEvent );
	}
}

void InventoryItemWnd::SetPresentBonusCash( int iValue1, int iValue2 )
{
	// icon btn
	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "UIIconPack101#Cash" ) );
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( pBtn )
	{
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
		pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		pBtn->SetActive();
	}
}

bool InventoryItemWnd::SetAccessoryInfo()
{
	if( m_iMagicCode <= 0 )
		return false;

	AccessorySlot kSlot;
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
		return false;

	if( !pAccessory->GetAccessorySlot( m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	if( m_bEquip )
	{
		ShowChildWnd( ID_RELEASE_BTN );

		m_dwCurIconActionID = INVENTORY_ACTION_RELEASE;
	}
	else
	{
		ShowChildWnd( ID_RESELL_MEDAL_BTN );

		m_dwCurIconActionID = INVENTORY_ACTION_EQUIP;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		ShowChildWnd( ID_CUSTOM_INFO_BTN );
	}

	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, 0 );

		if( m_bEquip )
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_EQUIP );
			pBtn->SetInActive();
		}
		else
		{
			pBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pBtn->SetIconGray( !m_bActive );
			pBtn->SetActive();
		}
	}

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", kSlot.GetName().c_str() );

	return true;
}

void InventoryItemWnd::ProcessAccessoryDesc()
{
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
	{
		LOG.PrintTimeAndLog( 0, "%s UserAccessoy == NULL", __FUNCTION__ );
		return;
	}

	ioHashString sInfo;
	bool bActive = pAccessory->GetUseInfo( m_iMagicCode, sInfo );
	if( bActive != m_bActive )
	{
		m_bActive = bActive;
		InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
		if( pBtn )
			pBtn->SetIconGray( !m_bActive );
	}

	AccessorySlot kSlot;
	if( !pAccessory->GetAccessorySlot( m_iMagicCode, kSlot ) )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, m_iMagicCode );
		return;
	}

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	

	if( kSlot.m_PeriodType == ACCPT_MORTMAIN )
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	else
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_RED );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, sInfo.c_str() );
}

// 2019-06-04
void InventoryItemWnd::OnDrawOvered( int iXPos, int iYPos )
{	
	ioButton::OnDrawOvered( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectItemIndex) )
	{
		m_bSelectPos = true;
	}
	else if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) )
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}

}

void InventoryItemWnd::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );	
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectItemIndex) )
	{
		m_bSelectPos = true;
	}
}

// 2019-06-14
void InventoryItemWnd::SetCompleteSelectPos( bool bCompleteSelectPos )
{
	m_bCompleteSelectPos = bCompleteSelectPos;
	m_dwCompleteSelectTimer = 0;

	if( true == m_bCompleteSelectPos )
	{
		m_dwCompleteSelectTimer = FRAMEGETTIME();
	}
}

void InventoryItemWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );
	m_dwOverTicTime = xElement.GetIntAttribute_e( "OverTicTime" );
	m_byOverFrmPrintCount = (BYTE)xElement.GetIntAttribute_e( "OverFrmPrintCount" );
}

//////////////////////////////////////////////////////////////////////////
GrowthSlotBtn::GrowthSlotBtn()
{
	m_pSmallIcon = NULL;

	ClearData();
}

GrowthSlotBtn::~GrowthSlotBtn()
{
	SAFEDELETE( m_pSmallIcon );
}

void GrowthSlotBtn::ClearData()
{
	m_iClassType = 0;

	m_iItemSlot = 0;
	m_iGrowthLevel = 0;
	m_iItemReinforce = 0;

	m_iCurLevel = 0;

	m_bSkill = false;
	m_bTimeGrowth = false;

	m_Name.Clear();

	SAFEDELETE( m_pSmallIcon );
}

void GrowthSlotBtn::SetInfo( int iClassType, const ioHashString &szName, bool bSkill, int iSlot )
{
	ClearData();

	if( iClassType <= 0 )
		return;

	int iCharArray = g_MyInfo.GetClassArray( iClassType );

	CHARACTER kCharInfo;
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		kCharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( kCharInfo, iClassType );

	// Small Icon
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( bSkill && pExtraItem )
	{
		SAFEDELETE( m_pSmallIcon );

		EXTRAITEMSLOT kSlot;
		int iExtraItemIndex = kCharInfo.m_extra_item[iSlot];
		if( pExtraItem->GetExtraItem( iExtraItemIndex, kSlot ) )
		{
			m_pSmallIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		}
	}

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		m_Name = szName;
		m_bSkill = bSkill;
		m_iItemSlot = iSlot;
		m_iClassType = iClassType;

		if( m_bSkill )
			m_iGrowthLevel = pLevel->GetItemGrowthLevel( m_iClassType, m_iItemSlot, false );
		else
			m_iGrowthLevel = pLevel->GetCharGrowthLevel( m_iClassType, m_iItemSlot, false );

		// TimeGrowth
		TimeGrowthInfo *pInfo = pLevel->FindTimeGrowthInfo( m_iClassType );
		if( pInfo )
		{
			int iSlotIndex = m_iItemSlot+1;
			if( !m_bSkill )
				iSlotIndex += CUR_GROWTH_CNT;

			int iTimeGrowthSlot = pInfo->m_iTimeSlot;
			if( iTimeGrowthSlot == iSlotIndex )
			{
				m_bTimeGrowth = true;
			}
		}

		const ITEM_DATA &kItem = g_MyInfo.GetCharItem( iCharArray, m_iItemSlot );
		m_iItemReinforce = kItem.m_item_reinforce;

		if ( m_iItemReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
			m_iCurLevel = m_iGrowthLevel + m_iItemReinforce;
		else
			m_iCurLevel = m_iGrowthLevel + g_GrowthInfoMgr.GetReinforceAddStat( m_iItemReinforce );

		// medal
		MedalSyncInfoVec vMedalItemTypeVec;
		g_MyInfo.GetMedalItemTypeVec( vMedalItemTypeVec, m_iClassType );

		int iMedalSize = vMedalItemTypeVec.size();
		for (int i = 0; i < iMedalSize ; i++)
		{
			if( vMedalItemTypeVec[i].m_iCustomIndex == 0 )
				m_iCurLevel += g_MedalItemMgr.GetMedalItemGrowth( vMedalItemTypeVec[i].m_iItemType, !m_bSkill, m_iItemSlot );	
			else
			{
				int iIndex = m_iItemSlot;
				if( m_bSkill )
					iIndex += MAX_CHAR_GROWTH;

				m_iCurLevel += vMedalItemTypeVec[i].m_iStat[iIndex];
			}
		}
		//펫 성장
		PetSlot sPetSlot;
		ioUserPet *pUserPet = g_MyInfo.GetUserPet();
		if( pUserPet )
			pUserPet->GetPetSlotEquip( sPetSlot );

		if( sPetSlot.m_nPetCode > 0 )
		{
			if( m_bSkill )
				m_iCurLevel += g_PetInfoMgr.GetTotalStat( sPetSlot.m_nPetCode, m_iItemSlot + MAX_ITEM_GROWTH, sPetSlot.m_nCurLevel );
			else
				m_iCurLevel += g_PetInfoMgr.GetTotalStat( sPetSlot.m_nPetCode, m_iItemSlot, sPetSlot.m_nCurLevel );
		}

		//각성
		if( m_bSkill )
			m_iCurLevel += g_AwakeMgr.GetAwakeCharGrowth( static_cast<AwakeType>( g_MyInfo.GetAwakeType( m_iClassType ) ), m_iItemSlot );
		else
			m_iCurLevel += g_AwakeMgr.GetAwakeItemGrowth( static_cast<AwakeType>( g_MyInfo.GetAwakeType( m_iClassType ) ), m_iItemSlot );

		const CHARACTER& rkCharInfo = g_MyInfo.GetCharacterToClassType( m_iClassType );
		if( g_RSoldierInfoManager.IsRSoldierType( rkCharInfo ) )
		{
			if( m_bSkill )
				m_iCurLevel += g_RSoldierInfoManager.GetSoldierAddStat( m_iItemSlot + MAX_ITEM_GROWTH, g_MyInfo.GetRSoldierPossessionCount() );
			else
				m_iCurLevel += g_RSoldierInfoManager.GetSoldierAddStat( m_iItemSlot, g_MyInfo.GetRSoldierPossessionCount() );
		}

		if( g_RSoldierInfoManager.IsSoldierSet( rkCharInfo ) )
		{
			if( m_bSkill )
				m_iCurLevel += g_RSoldierInfoManager.GetSoldierSetAddStat( m_iItemSlot + MAX_ITEM_GROWTH, rkCharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkCharInfo.m_class_type ) ) );
			else
				m_iCurLevel += g_RSoldierInfoManager.GetSoldierSetAddStat( m_iItemSlot, rkCharInfo.m_class_type, g_MyInfo.GetSoldierSetCnt( g_RSoldierInfoManager.GetSoldierSetNum( rkCharInfo.m_class_type ) ) );
		}
	}
}

void GrowthSlotBtn::OnRender()
{
	if( HasWndStyle( IWS_INACTIVE ) )
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// Small Icon
	enum
	{
		ICON_X_OFFSET	= 17,
		ICON_Y_OFFSET	= 165,
	};

	if( m_pSmallIcon )
	{
		m_pSmallIcon->SetScale( 0.24f );
		m_pSmallIcon->Render( iXPos+10.0f, iYPos+10.0f, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	// Text
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );

	char szBuff[MAX_PATH]="";
	if( m_iCurLevel > 0 )
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );

	if( m_pSmallIcon )
		g_FontMgr.PrintText( iXPos+20, iYPos+3, FONT_SIZE_12,  m_Name.c_str() );
	else
		g_FontMgr.PrintText( iXPos+3, iYPos+3, FONT_SIZE_12,  m_Name.c_str() );

	if( m_iCurLevel > 0 )
	{
		wsprintf_e( szBuff, " +%d", m_iCurLevel );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+122, iYPos+3, FONT_SIZE_12,  szBuff );
	}
}
//////////////////////////////////////////////////////////////////////////
InventorySoldierItemWnd::InventorySoldierItemWnd()
{
	m_iClassType = 0;
	m_iCurPoint = 0;
	m_dwCurWndID = 0;

	m_pCurGrowthBack = NULL;
	m_pPointImg = NULL;

	m_pBottomFrm = NULL;
	m_pSelectFrm = NULL;
	m_bSelect    = false;

	m_pEmptyIcon = NULL;
	m_pBoxLeft = NULL;
	m_pBoxTop = NULL;
	m_pDarkStrip = NULL;
	m_pLightStrip = NULL;


	m_ClassInfo[0].ClearList();
	m_ClassInfo[1].ClearList();

	m_PointText.ClearList();

	m_CharGrowthText.clear();
	m_CharGrowthText.reserve(CUR_GROWTH_CNT);

	m_ItemGrowthText.clear();
	m_ItemGrowthText.reserve(CUR_GROWTH_CNT);

	ClearGrowthInfo();
	ClearSkillText();

	m_bHired = false;

	// 2019-06-04
	m_nSelectCharIndex = -1;
	m_bSelectPos = false;
	m_pSelectPosFrm = NULL;
}

InventorySoldierItemWnd::~InventorySoldierItemWnd()
{
	SAFEDELETE(m_pCurGrowthBack);
	SAFEDELETE(m_pPointImg);

	SAFEDELETE(m_pBottomFrm);
	SAFEDELETE( m_pSelectFrm );

	SAFEDELETE(m_pEmptyIcon);
	SAFEDELETE(m_pBoxLeft);
	SAFEDELETE(m_pBoxTop);
	SAFEDELETE(m_pDarkStrip);
	SAFEDELETE(m_pLightStrip);

	ClearGrowthInfo();
	ClearSkillText();

	// 2019-06-04
	SAFEDELETE(m_pSelectPosFrm);
	m_nSelectCharIndex = -1;
	m_bSelectPos = false;
}

void InventorySoldierItemWnd::ClearGrowthInfo()
{
	int i, j;
	for( i=0; i < CUR_GROWTH_CNT; ++i )
	{
		m_CharLevel[i] = 0;
		m_CharGrowth[i] = 0;
	}

	for( j=0; j < CUR_GROWTH_CNT; ++j )
	{
		m_ItemLevel[j] = 0;
		m_ItemGrowth[j] = 0;
	}
}

void InventorySoldierItemWnd::ClearSkillText()
{
	for( int i=0; i < CUR_GROWTH_CNT; ++i )
	{
		m_ItemSkillText[i].Clear();
	}
}

void InventorySoldierItemWnd::SetInfo( int iClassType )
{
	m_iClassType = iClassType;
	m_bHired = false;

	ClearGrowthInfo();
	ClearSkillText();

	if( m_iClassType <= 0 )
	{
		m_iClassType = 0;
		m_iCurPoint = 0;

		m_ClassInfo[0].ClearList();
		m_ClassInfo[1].ClearList();

		InventoryIconBtn *pWnd = dynamic_cast< InventoryIconBtn* > ( FindChildWnd(ID_ICON_BTN) );
		if( pWnd )
		{
			pWnd->SetIcon( NULL, FLOAT1, FLOAT1 );
			pWnd->SetMarkType( INVENTORY_MARK_TYPE_NONE );
		}

		SetInActive();

		HideChildWnd( ID_ICON_BTN );

		HideChildWnd( ID_SKILL_SLOT1 );
		HideChildWnd( ID_SKILL_SLOT2 );
		HideChildWnd( ID_SKILL_SLOT3 );
		HideChildWnd( ID_SKILL_SLOT4 );

		HideChildWnd( ID_CHAR_SLOT1 );
		HideChildWnd( ID_CHAR_SLOT2 );
		HideChildWnd( ID_CHAR_SLOT3 );
		HideChildWnd( ID_CHAR_SLOT4 );
		HideChildWnd( ID_PROMOTE_BTN );
	}
	else
	{
		// UpdateItemData에서 Active 체크한다. 그래서 먼저...
		SetActive();

		UpdateItemData();

		bool bHired = false;
		bool bTimeGrowth = false;
		InventoryIconBtn *pWnd = dynamic_cast< InventoryIconBtn* > ( FindChildWnd(ID_ICON_BTN) );
		if( pWnd )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName(g_MyInfo.GetMySoldierIconName(m_iClassType));
			ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( g_MyInfo.GetSoldierSubIconName( m_iClassType ) );
			pWnd->SetIcon( pImg, FLOAT1, FLOAT1 );
			pWnd->SetSubIcon( pSubImg );
			pWnd->SetGradeType( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) );

			int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
			if( g_MyInfo.IsCharMortmain( m_iClassType ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
				bHired = true;
			else
			{
				if( g_MyInfo.GetCharTotalSecond( iCharArray ) > 0 )
					bHired = true;
			}

			int iCurLevel = 0;
			ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
			if( pLevel )
			{
				TimeGrowthInfo *pInfo = pLevel->FindTimeGrowthInfo( m_iClassType );
				if( pInfo )
					bTimeGrowth = true;
			}

			pWnd->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			if( bTimeGrowth )
			{
				pWnd->SetMarkType( INVENTORY_MARK_TYPE_TIMEGROWTH );
			}
			else
			{
				if( bHired )
				{
					if( m_iCurPoint > 0 )
						pWnd->SetMarkType( INVENTORY_MARK_TYPE_GROWTH_ENABLE );
					else
						pWnd->SetMarkType( INVENTORY_MARK_TYPE_GROWTH_DISABLE );
				}
			}

			pWnd->SetClassType( m_iClassType );
		}

		ShowChildWnd( ID_ICON_BTN );

		if( bHired )
		{
			m_bHired = true;
			SetChildActive( ID_ICON_BTN );
		}
		else
			SetChildInActive( ID_ICON_BTN );

		ShowChildWnd( ID_SKILL_SLOT1 );
		ShowChildWnd( ID_SKILL_SLOT2 );
		ShowChildWnd( ID_SKILL_SLOT3 );
		ShowChildWnd( ID_SKILL_SLOT4 );

		ShowChildWnd( ID_CHAR_SLOT1 );
		ShowChildWnd( ID_CHAR_SLOT2 );
		ShowChildWnd( ID_CHAR_SLOT3 );
		ShowChildWnd( ID_CHAR_SLOT4 );
		ShowChildWnd( ID_PROMOTE_BTN );
	}
}

void InventorySoldierItemWnd::SetSelect( bool bSelect )
{
	m_bSelect = bSelect;
}

// 2019-05-29 
void InventorySoldierItemWnd::SetSelectCharIndex( int nIndex )
{
	m_nSelectCharIndex = nIndex;
}	

void InventorySoldierItemWnd::UpdateItemData()
{
	if( HasWndStyle( IWS_INACTIVE ) )
		return;

	char szBuf[MAX_PATH] = "";

	m_ClassInfo[0].ClearList();
	m_ClassInfo[1].ClearList();

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( !pLevel )
	{
		return;
	}

	bool bHired = false;
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( g_MyInfo.IsCharMortmain( m_iClassType ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
		bHired = true;
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL) )
		bHired = true;
	else
	{
		if( g_MyInfo.GetCharTotalSecond( iCharArray ) > 0 )
			bHired = true;
	}
	
	wsprintf_e( szBuf, "Lv%d %s", g_MyInfo.GetClassLevel( m_iClassType, false ), g_MyInfo.GetClassName( m_iClassType ) );
	if( g_FontMgr.GetTextWidth( szBuf, TS_NORMAL, FONT_SIZE_13 ) >= 117 )
	{
		char szTextWidthCut[MAX_PATH];
		Help::StringCutFun( FONT_SIZE_13, 112, TS_NORMAL, szTextWidthCut, MAX_PATH, szBuf );
		sprintf_s( szBuf, "%s...", szTextWidthCut );
	}

	m_ClassInfo[0].SetTextStyle( TS_NORMAL );
	m_ClassInfo[0].SetBkColor( 0, 0, 0 );
	if( bHired )
		m_ClassInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	else
		m_ClassInfo[0].SetTextColor( TCT_DEFAULT_GRAY );
	m_ClassInfo[0].AddTextPiece( FONT_SIZE_13, STR(1), szBuf );

	ClearSkillText();

	ioGrowthInfo *pInfo = NULL;
	// Skill
	int i;
	int iRealItemCode = 0;
	for( i=0; i < CUR_GROWTH_CNT; ++i )
	{
		// GrowthInfoNum
		int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
		if( iCharArray != -1 )
		{
			const ITEM_DATA &kItem = g_MyInfo.GetCharItem( iCharArray, i );
			iRealItemCode = kItem.m_item_code;
		}
		else
		{
			iRealItemCode = i*DEFAULT_BASIC_ITEM_CODE + m_iClassType;
		}

		int iGrowthInfo = 0;
		const ioItem *pItem = g_ItemMaker.GetItemConst( iRealItemCode, __FUNCTION__ );
		if( pItem )
		{
			ioHashString szSkillName = pItem->GetSkillName();
			const ioSkill *pSkill = g_SkillMaker.GetSkillConst( szSkillName );
			if( pSkill )
			{
				ioHashString szSkillIcon = pSkill->GetIconName();
				m_SkillIconName[i] = szSkillIcon;

				m_ItemSkillText[i] = pSkill->GetDescName();
			}

			iGrowthInfo = pItem->GetGrowthInfoNum( 1 );
			m_ItemGrowth[i] = iGrowthInfo;
		}

		// GrowthLevel
		ioHashString szName;
		int iGrowthType = 0;

		m_ItemLevel[i] = 0;
		pInfo = g_GrowthInfoMgr.GetGrowthInfo( m_ItemGrowth[i] );
		if( pInfo )
		{
			szName = pInfo->GetGrowthInfoName();
			iGrowthType = pInfo->GetGrowthType();
			m_ItemLevel[i] = pLevel->GetItemGrowthLevel( m_iClassType, i, false );
		}

		bool bBaseGrowthType = false;
		switch( iGrowthType )
		{
		case GT_CHAR_ATTACK:
		case GT_CHAR_DEFENSE:
		case GT_CHAR_MOVE_SPEED:
		case GT_CHAR_DROP_DAMAGE:
		case GT_SKILL_ATTACK:
		case GT_SKILL_MOVE_SPEED:
		case GT_SKILL_DROP_DAMAGE:
		case GT_SKILL_DEFENSE:
			bBaseGrowthType = true;
			break;
		}

		// Btn
		GrowthSlotBtn *pSlot = dynamic_cast<GrowthSlotBtn*>(FindChildWnd(ID_SKILL_SLOT1+i));
		if( pSlot )
		{
			if( bBaseGrowthType )
				pSlot->SetInfo( m_iClassType, szName, true, i );
			else
				pSlot->SetInfo( m_iClassType, m_ItemGrowthText[i], true, i );
		}
	}

	// Char
	for( i=0; i < CUR_GROWTH_CNT; ++i )
	{
		// GrowthInfoNum
		m_CharGrowth[i] = ioBaseChar::m_vGrowthInfoNum[i];

		// GrowthLevel
		m_CharLevel[i] = 0;
		pInfo = g_GrowthInfoMgr.GetGrowthInfo( m_CharGrowth[i] );
		if( pInfo )
		{
			m_CharLevel[i] = pLevel->GetCharGrowthLevel( m_iClassType, i, false );
		}

		// Btn
		GrowthSlotBtn *pSlot = dynamic_cast<GrowthSlotBtn*>(FindChildWnd(ID_CHAR_SLOT1+i));
		if( pSlot )
		{
			pSlot->SetInfo( m_iClassType, m_CharGrowthText[i], false, i );
		}
	}
	// CurPoint
	m_iCurPoint = pLevel->GetCharGrowthPoint( m_iClassType );
	if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
		m_iCurPoint = 0;

	m_PointText.ClearList();
	m_PointText.SetTextStyle( TS_NORMAL );
	m_PointText.SetBkColor( 0, 0, 0 );
	m_PointText.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PointText.AddTextPiece( FONT_SIZE_12, STR(2) );

	m_PointText.SetTextColor( TCT_DEFAULT_RED );
	m_PointText.AddTextPiece( FONT_SIZE_12, STR(3), m_iCurPoint );
}

void InventorySoldierItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ICON_BTN:
		if( cmd == IOBN_BTNUP ||
			cmd == IOWN_OVERED||
			cmd == IOWN_LEAVED )
		{
			if( GetParent() )
			{
				if( !HasWndStyle( IWS_INACTIVE ) )
					GetParent()->iwm_command( this, cmd, dwID );
			}
		}
		break;
	case ID_PROMOTE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
			{
				if( !HasWndStyle( IWS_INACTIVE ) )
				{
					GetParent()->iwm_command( this, cmd, dwID );
				}
			}
			
			bool bHired = false;
			int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
			if( g_MyInfo.IsCharMortmain( m_iClassType ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
				bHired = true;
			else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
				bHired = true;
			else
			{
				if( g_MyInfo.GetCharTotalSecond( iCharArray ) > 0 )
					bHired = true;
			}

			if( g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetClassArray( m_iClassType ), EXERCISE_GENERAL ) ||
				g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetClassArray( m_iClassType ), EXERCISE_EVENT )   ||
				g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetClassArray( m_iClassType ), EXERCISE_RENTAL ))
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}

			if( !bHired )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			if( Help::IsMonsterDungeonMode( ioPlayMode::GetModeType() ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "육성할 수 없는 모드입니다." );
				return;
			}

			if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
				return;
			}

			if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );
				return;
			}
			CheckSelectInfo();
		}
		break;
	}
}

void InventorySoldierItemWnd::CheckSelectInfo()
{
	ioGrowthSelectWnd *pWnd = dynamic_cast<ioGrowthSelectWnd*>(g_GUIMgr.FindWnd(GROWTH_SELECT_WND));
	if( pWnd )
	{
		pWnd->SetGrowthSelectInfo( m_iClassType );
	}
}

void InventorySoldierItemWnd::BeforeRenderDesc()
{
	char szText[MAX_PATH]="";

	DWORD dwColor = TCT_DEFAULT_RED;
	bool bHired = false;
	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );

	//용병진화관련
	ioHashString szPowerupName;
	const ioClassExpert::CLASSSLOT* pClassSlot = g_MyInfo.GetClassExpertSlot( m_iClassType );
	if ( pClassSlot && pClassSlot->m_ePowerUpCharGrade != PUGT_NONE )
		szPowerupName = g_PowerUpManager.GetCharGradeTitle( m_iClassType, pClassSlot->m_ePowerUpCharGrade );

	if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_RENTAL ) )
	{
		SafeSprintf( szText, sizeof( szText ), STR(8) );
	}
	else if( g_MyInfo.IsCharMortmain( m_iClassType ) )
	{		
		if( szPowerupName.IsEmpty() )
		{			
			SafeSprintf( szText, sizeof( szText ), STR(1) );
		}
		else
			SafeSprintf( szText, sizeof( szText ), STR(9), szPowerupName.c_str() );

		dwColor = TCT_DEFAULT_BLUE;
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_GENERAL ) )
	{
		if( szPowerupName.IsEmpty() )		
			SafeSprintf( szText, sizeof( szText ), STR(2) );		
		else
			SafeSprintf( szText, sizeof( szText ), "%s(체험)", szPowerupName.c_str() );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) && !g_MyInfo.IsUserEvent() )
	{
			SafeSprintf( szText, sizeof( szText ), STR(3) );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_PCROOM ) && g_MyInfo.IsUserEvent() )
	{
		if( szPowerupName.IsEmpty() )
			SafeSprintf( szText, sizeof( szText ), STR(10) );
		else
			SafeSprintf( szText, sizeof( szText ), "%s(프리데이)", szPowerupName.c_str() );
	}
	else if( g_MyInfo.IsCharExerciseStyle( iCharArray, EXERCISE_EVENT ) )
	{
		if( szPowerupName.IsEmpty() )
			SafeSprintf( szText, sizeof( szText ), STR(4) );
		else
			SafeSprintf( szText, sizeof( szText ), "%s(이벤트)", szPowerupName.c_str() );
	}
	else
	{
		if( g_MyInfo.GetCharTotalSecond( iCharArray ) <= 0 )
			dwColor = TCT_DEFAULT_GRAY;

		int iTotalLimitMinute = g_MyInfo.GetCharTotalSecond( iCharArray ) / 60;
		int iHour     = iTotalLimitMinute / 60;      
		int iMinute   = iTotalLimitMinute % 60;

		if( iHour == 0 )
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(5), iMinute );
			else			
				SafeSprintf( szText, sizeof( szText ), "%s(%d분)", szPowerupName.c_str(), iMinute );	
		}
		else if( iMinute == 0 )
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(6), iHour );
			else
				SafeSprintf( szText, sizeof( szText ), "%s(%d시간)", szPowerupName.c_str(), iHour );
		}
		else
		{
			if( szPowerupName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(7), iHour, iMinute );
			else
				SafeSprintf( szText, sizeof( szText ), "%s(%d시간 %d분)", szPowerupName.c_str(), iHour, iMinute );
		}
	}

	m_ClassInfo[1].ClearList();
	m_ClassInfo[1].SetTextStyle( TS_NORMAL );
	m_ClassInfo[1].SetBkColor( 0, 0, 0 );
	m_ClassInfo[1].SetTextColor( dwColor );
	m_ClassInfo[1].AddTextPiece( FONT_SIZE_13, szText );
}

void InventorySoldierItemWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos );

		RenderStrip();

		if( m_pEmptyIcon )
			m_pEmptyIcon->Render( iXPos+69, iYPos+48, UI_RENDER_MULTIPLY );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+69, iYPos+96, FONT_SIZE_13,  STR(1) );
	}
	else
	{
		ioButton::OnRender();

		// 2019-06-04
		if( (true == m_bSelectPos) && (NULL != m_pSelectPosFrm) )
		{
			m_pSelectPosFrm->Render( iXPos, iYPos );
		}

		if( m_pBottomFrm )
		{
			m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
		}

		if( m_bSelect )
		{
			if( m_pSelectFrm )
				m_pSelectFrm->Render( iXPos, iYPos );
		}

		RenderStrip();
		BeforeRenderDesc();
		RenderDesc();
	}
}

void InventorySoldierItemWnd::RenderStrip()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		STRIP_X		= 7,
		STRIP_Y		= 155,
		STRIP_GAP	= 21,
	};

	if( m_pDarkStrip && m_pLightStrip )
	{
		m_pDarkStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pDarkStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pDarkStrip->SetSize( 125, 21 );

		m_pLightStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pLightStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pLightStrip->SetSize( 125, 21 );

		for( int i=0; i < 8; ++i )
		{
			if( (i%2) == 0 )
				m_pDarkStrip->Render( iXPos+STRIP_X, iYPos+STRIP_Y+(STRIP_GAP*i), UI_RENDER_MULTIPLY );
			else
				m_pLightStrip->Render( iXPos+STRIP_X, iYPos+STRIP_Y+(STRIP_GAP*i), UI_RENDER_MULTIPLY );
		}
	}
}

void InventorySoldierItemWnd::RenderDesc()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	m_ClassInfo[0].PrintFullText( iXPos+69, iYPos+96, TAT_CENTER );
	m_ClassInfo[1].PrintFullText( iXPos+69, iYPos+96+16, TAT_CENTER );

	m_PointText.PrintFullText( iXPos+69, iYPos+130, TAT_CENTER );

	if( m_pPointImg )
	{
		float fWidth = m_PointText.GetFullWidth() * FLOAT05;
		m_pPointImg->Render( iXPos+69+fWidth+1, iYPos+130 );
	}
}

void InventorySoldierItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Growth_Back" )
	{
		SAFEDELETE( m_pCurGrowthBack );
		m_pCurGrowthBack = pImage;
	}
	else if( szType == "P_Image" )
	{
		SAFEDELETE( m_pPointImg );
		m_pPointImg = pImage;
	}
	else if( szType == "empty_icon" )
	{
		SAFEDELETE( m_pEmptyIcon );
		m_pEmptyIcon = pImage;
	}
	else if( szType == "box_left" )
	{
		SAFEDELETE( m_pBoxLeft );
		m_pBoxLeft = pImage;
	}
	else if( szType == "box_top" )
	{
		SAFEDELETE( m_pBoxTop );
		m_pBoxTop = pImage;
	}
	else if( szType == "dark_strip" )
	{
		SAFEDELETE( m_pDarkStrip );
		m_pDarkStrip = pImage;
	}
	else if( szType == "light_strip" )
	{
		SAFEDELETE( m_pLightStrip );
		m_pLightStrip = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void InventorySoldierItemWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "BottomFrm" )
		{
			SAFEDELETE( m_pBottomFrm );
			m_pBottomFrm = pFrame;
		}
		else if( szType == "SelectFrm" )
		{
			SAFEDELETE( m_pSelectFrm );
			m_pSelectFrm = pFrame;
		}
		else if( szType == "SelectPosFrm" )	// 2019-06-04
		{
			SAFEDELETE( m_pSelectPosFrm );
			m_pSelectPosFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void InventorySoldierItemWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	for (int i = 0; i < CUR_GROWTH_CNT ; i++)
	{
		// CharGrowth IconName
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "CharIcon%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_CharIconName[i] = szTemp;

		// Char, Item Growth Text
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "CharGrowth%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_CharGrowthText.push_back( szTemp );

		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "ItemGrowth%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_ItemGrowthText.push_back( szTemp );
	}
}

bool InventorySoldierItemWnd::QuestGuide( int iClassType, float &rfXPos, float &rfYPos )
{
	if( m_iClassType != iClassType )
		return false;

	ioWnd *pIconBtn = FindChildWnd( ID_ICON_BTN );
	if( !pIconBtn ) return false;
	if( !pIconBtn->IsShow() ) return false;

	rfXPos = pIconBtn->GetDerivedPosX() + (pIconBtn->GetWidth() / 2);
	rfYPos = pIconBtn->GetDerivedPosY();
	return true;
}

// 2019-06-04
void InventorySoldierItemWnd::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectCharIndex) )
	{
		m_bSelectPos = true;
	}
}

void InventorySoldierItemWnd::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );	
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectCharIndex) )
	{
		m_bSelectPos = true;
	}
}

//////////////////////////////////////////////////////////////////////////
bool MyInventoryWnd::m_bGashaponListRequest = false;

MyInventoryWnd::MyInventoryWnd()
{
	m_pPlayStage = NULL;
	m_pTabDot = NULL;

	m_dwExtraItemChangeTime = 0;
	m_dwMedalItemChangeTime = 0;

	m_iDefaultTabType = ID_SOLDIER_TAB_BTN;

	m_dwCurTabID = ID_SOLDIER_TAB_BTN;
	m_dwSmallTabEndID = ID_SMALL_TAB_1_BTN;

	m_bRoomOutTimeCheck = false;
	m_bCharItem = false;

	m_iSelectClassType = -1;
	m_iSellSlotIndex = 0;
	m_iSellSubIndex = 0;

	m_bMileage = false;
	m_dwBonusCashTooltipCheckStartTime = 0;

	// 2019-05-27
	m_pNewSearchWnd = NULL;
	m_nSelectCharIndex = 0;
	m_nCurSelectCharIndex = 0;
	m_szSearchName = "";
	m_dwSelectCharID = 0;
	m_bSearchInputEnter = false;
	m_nCurOveredCharIndex = -1;
	m_szCurOveredSearchName = "";
	m_nCostumeHelpPosX = 0;
	m_nSearchShowBtnPosX = 0;
	m_bSearchComplete = false;

	// 2019-06-17
	m_nSearchIndex = -1;
}

MyInventoryWnd::~MyInventoryWnd()
{
	SAFEDELETE( m_pTabDot );

	for (int i = 0; i < MAX_TAB_BTN ; i++)
	{
		m_vSmallTabNameVecArray[i].clear();
		m_vSmallTabTypeVecArray[i].clear();
	}

	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	// 2019-05-27
	m_pNewSearchWnd = NULL;
	m_nSelectCharIndex = 0;
	m_nCurSelectCharIndex = 0;
	m_szSearchName = "";
	m_dwSelectCharID = 0;
	m_bSearchInputEnter = false;
	m_nCurOveredCharIndex = -1;
	m_szCurOveredSearchName = "";
	m_nCostumeHelpPosX = 0;
	m_nSearchShowBtnPosX = 0;
	m_bSearchComplete = false;

	// 2019-06-17
	m_nSearchIndex = -1;
}

void MyInventoryWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/MyInventoryInfoWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/growthdownselectwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemcompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ItemMultipleCompoundWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ItemMultipleCompoundResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/uniqueitemmultiplecompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/uniqueitemmultiplecompoundresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/PackageItemBuyWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/GashaponWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/RandomDecoWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/RandomDecoSelectWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ItemCompoundResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ExtraItemRandomBox.xml", this );
	g_GUIMgr.AddWnd( "XML/growthdowndonewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/PackageItemBuyResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ChangeNameWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ChangeNameResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ChangeTradeStateResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ItemRecvSellInfoWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/RecordInitWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/RecordInitResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemgrowthcatalystwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemgrowthcatalystresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemcompoundexwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemcompoundexresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemluckycoinwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemluckycoinresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemrainbowmixerwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemrainbowmixerresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemlostsagamixerwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemlostsagamixerresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemgoldboxwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/invensoldierselectbuywnd.xml", this );
	g_GUIMgr.AddWnd( "XML/invensoldierselectbuyresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/InvenFourExtraCompoundBuyWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/InvenFourExtraCompoundBuyResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/invensoldierexpbonuswnd.xml", this );
	g_GUIMgr.AddWnd( "XML/invensoldierexpbonusresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ExpandMedalSlotWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ExpandMedalSlotResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/PreSetSoldierPackageWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/PreSetSoldierPackageResultWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/growthalldownselectwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/growthalldowndonewnd.xml", this );
	g_GUIMgr.AddWnd( "XML/supergashponinvenwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/multisupergashponinvenwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/MultiSuperGashaponToastPopupInvenWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/SpiritFuncWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/AccessoryFuncWnd.xml", this );
	g_GUIMgr.AddWnd( "XML/ExtraitemFuncWnd.xml", this );

	g_GUIMgr.AddWnd( "XML/itemmaterialcompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/itemmaterialcompoundresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/peteggresultwnd.xml", this );

	g_GUIMgr.AddWnd( "XML/accessorycompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/AccessoryCompoundResultWnd.xml", this );
	//소울스톤 강화용병
	g_GUIMgr.AddWnd( "XML/heroreinforcedownwnd.xml",this );
	g_GUIMgr.AddWnd( "XML/heroreinforcedonewnd.xml",this );
	g_GUIMgr.AddWnd( "XML/itemreinforcedownwnd.xml",this );
	g_GUIMgr.AddWnd( "XML/itemreinforcedonewnd.xml",this );

	// UI코스튬 관련 (코스튬 박스)
	g_GUIMgr.AddWnd( "XML/CostumeRandomBox.xml", this );

	g_GUIMgr.AddWnd( "XML/Itemnewmultiplecompoundwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/namedtitleresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/namedtitlepremiumresultwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/namedtitlepremiumselectwnd.xml", this );

	// 2019-05-20 찾기 기능 
	g_GUIMgr.AddWnd( "XML/newsearchwnd.xml", this );	
	m_pNewSearchWnd = dynamic_cast<NewSearchWnd*>( FindChildWnd( ID_NEW_SEARCH_WND ) );
	if( NULL != m_pNewSearchWnd )
	{
		m_pNewSearchWnd->SetWndType( NewSearchWnd::WT_MY_INVENTORY );
		m_pNewSearchWnd->SetPosX(27);		
	}	

	// 2019-06-13
	ioWnd *pWnd = FindChildWnd( ID_COSTUME_HELP );
	if( NULL != pWnd )	{	m_nCostumeHelpPosX = pWnd->GetPosX();	}

	pWnd = FindChildWnd( ID_SEARCH_SHOW_BTN );
	if( NULL != pWnd )	{	m_nSearchShowBtnPosX = pWnd->GetPosX();	}

	UppderAlphaWnd *pFlashWnd = dynamic_cast<UppderAlphaWnd*> ( FindChildWnd( ID_FLASH_WND ) );
	if( pFlashWnd )
		pFlashWnd->SetSizeFromParent( false );

	for( int i = 0; i < MAX_TAB_BTN; ++i )
	{       
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("ioRadioButton"), this, ID_SOLDIER_TAB_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( m_iTabPos[i], 59 );
	}
	for( int i = 0; i < MAX_SMALL_TAB_BTN; ++i )
	{
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("SmallTabButton"), this, ID_SMALL_TAB_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + i * 53, 97 );
	}
	for( int i = 0; i < MAX_ITEM_INFO_WND_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_ITEM_NUM;
		int iYIndex = i / PAGE_PER_ITEM_NUM;
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("InventoryItemWnd"), this, ID_ITEM_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + iXIndex * 106, 119 + iYIndex * 181 );
	}
	for( int i = 0; i < MAX_CHAR_INFO_WND_BTN; ++i )
	{
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("InventorySoldierItemWnd"), this, ID_CHAR_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + i * 141, 119 );
	}
	for( int i = 0; i < MAX_SPIRIT_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_SPIRIT_NUM;
		int iYIndex = i / PAGE_PER_SPIRIT_NUM;
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("SpiritItemWnd"), this, ID_SPIRIT_INFO_1 + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + iXIndex * 86, 121 + iYIndex * 90 );
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
		return;

	m_bMileage = pLocal->IsMileage();
}

void MyInventoryWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TabDot" )
	{
		SAFEDELETE( m_pTabDot );
		m_pTabDot = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );	
	}
}

void MyInventoryWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioHashString sSmallTabName;
	int          iType = 0;
	for (int i = 0; i < MAX_TAB_BTN ; i++)
	{
		char szName[MAX_PATH]="";
		wsprintf_e( szName, "Pos_%d", ID_SOLDIER_TAB_BTN+i );
		m_iTabPos[i] = xElement.GetIntAttribute( szName );
		wsprintf_e( szName, "Name_%d", ID_SOLDIER_TAB_BTN+i );
		m_MainTabName[i] = xElement.GetStringAttribute( szName );

		StringCbPrintf_e( szName, sizeof( szName ), "Max_%d", ID_SOLDIER_TAB_BTN+i );
		int iMax = xElement.GetIntAttribute( szName );
		for (int j = 0; j < iMax ; j++)
		{
			StringCbPrintf_e( szName, sizeof( szName ), "Name_%d_%d", ID_SOLDIER_TAB_BTN+i, j+1 );
			sSmallTabName = xElement.GetStringAttribute( szName );
			m_vSmallTabNameVecArray[i].push_back( sSmallTabName );

			StringCbPrintf_e( szName, sizeof( szName ), "Type_%d_%d", ID_SOLDIER_TAB_BTN+i, j+1 );
			iType = xElement.GetIntAttribute( szName );
			m_vSmallTabTypeVecArray[i].push_back( iType );
		}

		StringCbPrintf_e( szName, sizeof( szName ), "Default_%d", ID_SOLDIER_TAB_BTN+i );
		bool bUse = xElement.GetBoolAttribute( szName );
		if( bUse )
		{
			StringCbPrintf_e( szName, sizeof( szName ), "Default_Type_%d", ID_SOLDIER_TAB_BTN+i );
			m_iDefaultTypeArray[i] = xElement.GetIntAttribute( szName );
		}
		else
			m_iDefaultTypeArray[i] = ETC_ITEM_OTHER_VALUE;
	}

	m_iDefaultTabType = xElement.GetIntAttribute_e( "Default_Tab_Type" );	
	m_nCostumeHelpTipIdx = xElement.GetIntAttribute_e( "costume_help_tip_idx" );
	m_dwBonusCashTooltipCheckDuration = (DWORD)xElement.GetIntAttribute_e( "BonusCashTooltipCheckDuration" );

	m_szEquipAccessoryErrMsg = xElement.GetStringAttribute_e( "EquipAccessoryErrMsg" );
	m_szTimeEndAccessoryDelMsg = xElement.GetStringAttribute_e( "TimeEndAccessoryDelMsg" );
}

void MyInventoryWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

bool MyInventoryWnd::IsNetworkMode()
{
	if( m_pPlayStage == NULL )
		return false;

	return m_pPlayStage->IsNetworkMode();
}

void MyInventoryWnd::iwm_show()
{
	// 현재 select classtype
	m_iSelectClassType = g_MyInfo.GetClassType();
	m_iSellSlotIndex = 0;
	m_iSellSubIndex = 0;

	SendRequestGashaponList();

	m_bRoomOutTimeCheck = false;

	enum { Y_OFFSET = -20 };
	int iXPos = ( Setting::Width() - GetWidth() ) / 2;
	int iYPos = ( Setting::Height() - GetHeight() ) / 2 + Y_OFFSET;

	PetWnd *pPetWnd = dynamic_cast<PetWnd*>(g_GUIMgr.FindWnd(PET_WND));
	if ( pPetWnd && pPetWnd->IsShow() )
		pPetWnd->HideWnd();

	NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
	if( pShopWnd && pShopWnd->IsShow() )
	{
		iXPos = pShopWnd->GetDerivedPosX();
		iYPos = pShopWnd->GetDerivedPosY();
		pShopWnd->HideWnd();
	}
	ioMovingWnd::SetWndPos( iXPos, iYPos );

	if( !g_App.IsWemadeChannelingGetCash() )
	{
		if( pShopWnd )
			pShopWnd->SendGetCash();
	}

	DWORD dwSmallTab = GetDefaultSmallTab( m_iDefaultTabType );
	ChangeMainTab( m_iDefaultTabType );
	ChangeSmallTab( dwSmallTab );

	if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
	{
		ShowMainInfoWnd();
		ShowAlchemicFuncWnd();
	}
	else if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN )
	{
		ShowMainInfoWnd();
		ShowAccessoryFuncWnd();
	}
	else if( m_dwCurTabID == ID_EXTRA_TAB_BTN )
	{
		ShowMainInfoWnd();
		ShowExtraitemFuncWnd();
	}
	else
	{
		SetFirstBtnInfo();

		if( m_vInventoryItemInfoList.empty() )
		{
			ChangeMainTab( m_iDefaultTabType );
			SetFirstBtnInfo();
		}
	}
	for( int i = 0; i < MAX_TAB_BTN; ++i )
	{
		ioWnd *pWnd = FindChildWnd( ID_SOLDIER_TAB_BTN + i );
		if( pWnd )
		{
			pWnd->SetWndPos( m_iTabPos[i], 59 );
			pWnd->SetTitleText( m_MainTabName[i].c_str() );
		}
	}
	for( int i = 0; i < MAX_ITEM_INFO_WND_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_ITEM_NUM;
		int iYIndex = i / PAGE_PER_ITEM_NUM;
		ioWnd *pWnd = FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + iXIndex * 106, 119 + iYIndex * 181 );
	}
	for( int i = 0; i < MAX_CHAR_INFO_WND_BTN; ++i )
	{
		ioWnd *pWnd = FindChildWnd( ID_CHAR_INFO_WND_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + i * 141, 119 );
	}
	for( int i = 0; i < MAX_SPIRIT_BTN; ++i )
	{
		int iXIndex = i % PAGE_PER_SPIRIT_NUM;
		int iYIndex = i / PAGE_PER_SPIRIT_NUM;
		ioWnd *pWnd = FindChildWnd( ID_SPIRIT_INFO_1 + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + iXIndex * 86, 121 + iYIndex * 90 );
	}

	// 2019-05-23 Key 
	m_kKeys.SetDefaultFirstKey();
	m_kSecondKeys.SetDefaultSecondKey();
	m_kJoyKeys.SetDefaultJoyKey();

	g_KeyManager.GetGameKey(m_kKeys);
	g_KeyManager.GetSecondGameKey(m_kSecondKeys);
	g_KeyManager.GetJoyGameKey(m_kJoyKeys);

	if( g_MyInfo.GetChannelingType() == CNT_NEXON )
	{
		ioWnd* pWnd = FindChildWnd( ID_CHARGE_GOLD_BTN );
		if( pWnd )
			pWnd->SetInActive();
	}
}

// 2019-07-08
void MyInventoryWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( NULL != m_pNewSearchWnd )
	{
		m_pNewSearchWnd->SetPosX(27);		
	}

	HideWnd();
}

void MyInventoryWnd::iwm_hide()
{	
	// 2019-04-30
	ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
	if( NULL != pExtraItemWnd )
	{
		pExtraItemWnd->SetRollingStopSound();
	}

	// 2019-07-24
	UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
	if( pUISoliderWnd && pUISoliderWnd->IsShow() )
	{
		pUISoliderWnd->ClearSearchHide();
		pUISoliderWnd->HideWnd();
	}

	// 2019-7-17
	if( NULL != m_pNewSearchWnd )
	{
		m_pNewSearchWnd->HideWnd();
	}

	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	for (int i= 0; i < MAX_CHAR_INFO_WND_BTN ; i++)
		HideChildWnd( ID_CHAR_INFO_WND_1_BTN + i );

	for ( int i=0; i < MAX_SPIRIT_BTN; ++i )
		HideChildWnd( ID_SPIRIT_INFO_1 + i );

	HideChildWnd(ID_INVENTORY_INFO_WND);
	HideChildWnd(ID_GROWTH_DOWN_WND);
	HideChildWnd(ID_ITEM_COMPOUND_WND);
	HideChildWnd(ID_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_PACKAGE_ITEM_WND);
	HideChildWnd(ID_GASHPON_WND);
	HideChildWnd(ID_SUPER_GASHAPON_WND);
	HideChildWnd(ID_MULTI_SUPER_GASHAPON_WND);
	HideChildWnd(ID_MULTI_SUPER_GASHAPON_TOAST_POPUP_WND);
	HideChildWnd(ID_RANDOM_DECO_WND);
	HideChildWnd(ID_DECO_SELECT_WND);
	HideChildWnd(ID_MULTIPLE_COMPOUND_RESULT);
	HideChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_RESULT);
	HideChildWnd(ID_ITEM_COMPOUND_RESULT);
	HideChildWnd(ID_EXTRAITEM_BOX_WND);
	HideChildWnd(ID_GROWTH_DOWN_RESULT);
	HideChildWnd(ID_PACKAGE_ITEM_RESULT);
	HideChildWnd(ID_CHANGE_NAME_WND);
	HideChildWnd(ID_TRADE_STATE_CHANGE_WND);
	HideChildWnd(ID_RECORD_INIT_WND);
	HideChildWnd(ID_RECORD_INIT_RESULT_WND);
	HideChildWnd(ID_ITEM_GROWTH_CATALYST_WND);
	HideChildWnd(ID_ITEM_GROWTH_CATALYST_RESULT_WND);
	HideChildWnd(ID_ITEM_LUCKY_COIN_USE_WND);
	HideChildWnd(ID_ITEM_LUCKY_COIN_USE_RESULT_WND);
	HideChildWnd(ID_ITEM_COMPOUNDEX_WND);
	HideChildWnd(ID_ITEM_COMPOUNDEX_RESULT_WND);
	HideChildWnd(ID_ITEM_RAINBOW_MIXER_WND );
	HideChildWnd(ID_ITEM_RAINBOW_MIXER_RESULT_WND);
	HideChildWnd(ID_ITEM_LOSTSAGA_MIXER_WND );
	HideChildWnd(ID_ITEM_LOSTSAGA_MIXER_RESULT_WND);
	HideChildWnd(ID_ITEM_GOLD_BOX_WND);
	HideChildWnd(ID_SOLDIER_SELECT_BUY_WND);
	HideChildWnd(ID_SOLDIER_SELECT_RESULT_WND);
	HideChildWnd(ID_FOUR_EXTRA_COMPOUND_BUY_WND);
	HideChildWnd(ID_FOUR_EXTRA_COMPOUND_RESULT_WND);
	HideChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_WND);
	HideChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_RESULT_WND);
	HideChildWnd(ID_EXPAND_MEDAL_SLOT_OPEN_WND);
	HideChildWnd(ID_EXPAND_MEDAL_SLOT_RESULT_WND);
	HideChildWnd(ID_PRESET_PACKAGE_WND);
	HideChildWnd(ID_PRESET_PACKAGE_RESULT_WND);
	HideChildWnd(ID_GROWTH_ALL_DOWN_WND);
	HideChildWnd(ID_GROWTH_ALL_DOWN_RESULT);
	HideChildWnd(ID_SPIRITFUNC_WND);
	HideChildWnd(ID_ACCESSORYFUNC_WND);
	HideChildWnd(ID_EXTRAITEMFUNC_WND);

	HideChildWnd(ID_ITEM_MATERIAL_COMPOUND_WND);
	HideChildWnd(ID_ITEM_MATERIAL_COMPOUND_RESULT);

	HideChildWnd(ID_PET_EGG_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_PREMIUM_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_PREMIUM_SELECT_WND);

	HideChildWnd(ID_HERO_REINFORCE_DWON_WND);
	HideChildWnd(ID_HERO_REINFORCE_DONE_WND);
	HideChildWnd(ID_ITEM_REINFORCE_DWON_WND);
	HideChildWnd(ID_ITEM_REINFORCE_DONE_WND);
	HideChildWnd(ID_COSTUME_BOX_WND);
	HideChildWnd(ID_NEW_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_ACCESSORY_COMPOUND_WND);
	HideChildWnd(ID_ACCESSORY_COMPOUND_RESULT);

	m_bRoomOutTimeCheck = false;
}

void MyInventoryWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void MyInventoryWnd::iwm_wheel( int zDelta )
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

void MyInventoryWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHARGE_GOLD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_MYROOM )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
			}
			else
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드충전은 광장/훈련소/본부에서만 가능합니다." );
					return;
				}
			}

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
				pLocal->OpenFillCashPage();
		}
		break;
	case ID_SHOP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
			{
				switch( m_dwCurTabID )
				{
				case ID_SOLDIER_TAB_BTN:
					pShopWnd->ShowSoldierTab();
					break;
				case ID_EXTRA_TAB_BTN:
				case ID_ACCESSORY_TAB_BTN:
					pShopWnd->ShowExtraItemTab();
					break;
				case ID_DECO_TAB_BTN:
					pShopWnd->ShowDecoItemTab();
					break;
				case ID_ETC_TAB_BTN:
				case ID_ALCHEMIC_TAB_BTN:
					pShopWnd->ShowEtcItemTab();
					break;
					// UI코스튬 관련 (해당 탭에서 상점가기 눌렀을때)
				case ID_COSTUME_TAB_BTN:
					pShopWnd->ShowCostumeItemTab();
					break;
				default:
					pShopWnd->ShowWnd();
					break;
				}
			}
		}
		break;
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 2019-04-30
			ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
			if( NULL != pExtraItemWnd )
			{
				pExtraItemWnd->SetRollingStopSound();
			}
			HideWnd();
		}
		break;
	case ID_SOLDIER_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			DWORD dwSmallTab = GetDefaultSmallTab( ID_SOLDIER_TAB_BTN );
			ChangeMainTab( ID_SOLDIER_TAB_BTN );
			ChangeSmallTab( dwSmallTab );
			SetFirstBtnInfo();

			if( m_vInventoryItemInfoList.empty() )
			{
				ChangeMainTab( dwID );

				m_iSelectArray = FindSelectCharArray( m_iSelectClassType );

				int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;
				for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
				{
					InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
					if( !pItem )
					{
						continue;
					}

					// 2019-05-27
					pItem->SetSelectCharIndex(-1);
					if( true == IsSearchInputEnter() )
					{
						pItem->SetSelectCharIndex(i);
					}

					int iCurArray = iStartArray + i;
					if( m_iSelectArray == iCurArray )
					{
						pItem->SetSelect( true );
					}
					else
					{
						pItem->SetSelect( false );
					}
				}

				MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
				if( pInfoWnd )
				{
					ShowMainInfoWnd();
					pInfoWnd->SetSlodierInfo( m_iSelectClassType );
				}
			}
		}
		break;
	case ID_EXTRA_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			// UI코스튬 관련 (메인 탭 클릭시)
			UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
			if( pUISoliderWnd && pUISoliderWnd->IsShow() )
				pUISoliderWnd->HideWnd();

			ChangeMainTab( dwID );
			ShowMainInfoWnd();
			ShowExtraitemFuncWnd();

			ExtraItemFuncWnd *pExtraInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
			if( pExtraInfoWnd )
				pExtraInfoWnd->SetExtraItemInfo( m_iSelectClassType );
		}
		break;
	case ID_DECO_TAB_BTN:
	case ID_ETC_TAB_BTN:
	case ID_COSTUME_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			// UI코스튬 관련 (메인 탭 클릭시)
			UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
			if( pUISoliderWnd && pUISoliderWnd->IsShow() )
				pUISoliderWnd->HideWnd();

			ChangeMainTab( dwID );
			SetFirstBtnInfo();
		}
		break;
	case ID_PRESENT_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
			if( pUISoliderWnd && pUISoliderWnd->IsShow() )
				pUISoliderWnd->HideWnd();

			ShowSubscriptionTabDirect();
			SetFirstBtnInfo();
		}
		break;
	case ID_ALCHEMIC_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
			if( pUISoliderWnd && pUISoliderWnd->IsShow() )
				pUISoliderWnd->HideWnd();

			ChangeMainTab( dwID );

			ShowMainInfoWnd();
			ShowAlchemicFuncWnd();
		}
		break;
	case ID_ACCESSORY_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			// UI코스튬 관련 (메인 탭 클릭시)
			UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
			if( pUISoliderWnd && pUISoliderWnd->IsShow() )
				pUISoliderWnd->HideWnd();

			ChangeMainTab( dwID );
			ShowMainInfoWnd();
			ShowAccessoryFuncWnd();

			AccessoryFuncWnd *pAccessoryInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
			if( pAccessoryInfoWnd )
				pAccessoryInfoWnd->SetAccessoryInfo( m_iSelectClassType );
		}
		break;
	case ID_SMALL_TAB_1_BTN:
	case ID_SMALL_TAB_2_BTN:
	case ID_SMALL_TAB_3_BTN:
	case ID_SMALL_TAB_4_BTN:
	case ID_SMALL_TAB_5_BTN:
	case ID_SMALL_TAB_6_BTN:
	case ID_SMALL_TAB_7_BTN:
	case ID_SMALL_TAB_8_BTN:
	case ID_SMALL_TAB_9_BTN:
	case ID_SMALL_TAB_10_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}

			ChangeSmallTab( dwID );

			//2019-05-23
			if( true == IsHideSmallTabSearch( dwID ) )
			{		
				HideChildWnd( ID_SEARCH_SHOW_BTN );
				HideChildWnd( ID_SEARCH_HIDE_BTN );
			}
			else 
			{
				ShowChildWnd( ID_SEARCH_SHOW_BTN );
				HideChildWnd( ID_SEARCH_HIDE_BTN );
			}

			if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
			{
				ShowMainInfoWnd();
				ShowAlchemicFuncWnd();
			}
			else if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN )
			{
				ShowMainInfoWnd();
				ShowAccessoryFuncWnd();
			}
			else if( m_dwCurTabID == ID_EXTRA_TAB_BTN )
			{
				ShowMainInfoWnd();
				ShowExtraitemFuncWnd();
			}
			else
				SetFirstBtnInfo();
		}
		break;
	case ID_HOUSE_ITEM_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ioHousingBlockInvenWnd* pWnd = dynamic_cast<ioHousingBlockInvenWnd*>( g_GUIMgr.FindWnd( HOUSING_BLOCK_INVEN_WND ) );
			if( pWnd )
			{
				pWnd->SetBlockModeType( BMT_PERSONAL );
				pWnd->ShowWnd();
			}
		}
		else if( cmd == IOEX_BTNUP )
		{
			HomeEtcItemNeedPopup* pWnd = dynamic_cast<HomeEtcItemNeedPopup*>( g_GUIMgr.FindWnd( HOUSE_NEED_POPUP ) );
			if( pWnd )
				pWnd->ShowWnd();
		}
		break;
	case ID_REFRESH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			g_PresentMgr.SendPresentDataToTime( ioPresentMgr::REQUEST_BTN );
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
			MultiSuperGashaponWnd* pMultiSuperWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ));
			if( pMultiSuperWnd && pMultiSuperWnd->IsShow() && pMultiSuperWnd->IsAllItemShow() )
			{
				pMultiSuperWnd->UpdateAllItemInfo();
				break;
			}
			// 2019-04-30
			ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
			if( NULL != pExtraItemWnd )
			{
				pExtraItemWnd->SetRollingStopSound();
			}

			// 2019-07-01
			for( int i=0; i < MAX_ITEM_INFO_WND_BTN; i++ )
			{
				InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
				if( NULL == pItem )
				{
					continue;
				}

				pItem->SetCompleteSelectPos( false );
				pItem->SetSelect( false );
			}

			// 2019-07-01 검색중 
			if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsShow()) && (true == IsSearchInputEnter()) && (INVENTORY_ACTION_RESELL != param) && (INVENTORY_ACTION_DISASSEMBLE != param) )
			{
				// 최종선택 캐릭터 
				InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
				if( NULL != pItem ) 										
				{
					m_szSearchName = pItem->GetItemInfoSlotName();
					m_nSearchIndex = pItem->GetItemInfoSlotIndex();

					// 2019-07-08
					if( (true == m_szSearchName.IsEmpty()) && (-1 >= m_nSearchIndex) ) 
					{
						return;
					}
				}

				// 2019-06-12
				ioWnd *pWnd = FindChildWnd(ID_SEARCH_HIDE_BTN);
				if( NULL != pWnd )
				{
					iwm_command( pWnd, IOBN_BTNUP, param ); 
				}

				if( m_dwSelectCharID > 0 )
				{
					dwID = m_dwSelectCharID;
					m_dwSelectCharID = 0;
				}

				// 최종선택 2019-06-24
				int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
				m_iSelectArray = iStartArray + (dwID-ID_ITEM_INFO_WND_1_BTN);

				for( int i=0; i < MAX_ITEM_INFO_WND_BTN; i++ )
				{
					InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
					if( NULL == pItem )
					{
						continue;
					}

					int iCurArray = iStartArray + i;

					pItem->SetCompleteSelectPos( false );
					pItem->SetSelect( false );	// 2019-06-28
					if( m_iSelectArray == iCurArray )
					{
						pItem->SetCompleteSelectPos( true );
						pItem->SetSelect( true );	// 2019-06-28
						m_bSearchComplete = true;
					}
				}

				return;
			}

			SelectItemBtn( dwID, param );

			InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
			if( pItem && param == 0 )
			{
				param = pItem->GetCurActionID();
			}

			if( param == INVENTORY_ACTION_NONE )
				OnItemIconPush( dwID );
			else if( param == INVENTORY_ACTION_EQUIP )
				OnItemEquipPush( dwID );
			else if( param == INVENTORY_ACTION_RELEASE )
				OnItemReleasePush( dwID );
			else if( param == INVENTORY_ACTION_RESELL )
				OnItemReSellPush( dwID );
			else if( param == INVENTORY_ACTION_DETAIL )
				OnItemDetailPush( dwID );
			else if( param == INVENTORY_ACTION_CUSTOM_INFO )
				OnItemCustomInfoPush( dwID );
			else if( param == INVENTORY_ACTION_DISASSEMBLE )
				OnItemDisassemblePush( dwID );
		}
		else if( (cmd == IOWN_OVERED) && (true == IsSearchInputEnter()) ) // 2019-05-31
		{
			if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)	||	// 2019-05-31 장비,치장,특별,보관함,코스튬, 액세서리
				(m_dwCurTabID == ID_DECO_TAB_BTN)		||
				(m_dwCurTabID == ID_ETC_TAB_BTN)		||
				(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||
				(m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
				(m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )	
			{
				// 2019-06-25
				InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(pWnd);	
				//if( (NULL != pItem) && ( (INVENTORY_ITEM_EXTRAITEM == pItem->GetMainType()) ||
				//						 (INVENTORY_ITEM_MEDALITEM == pItem->GetMainType()) ||
				//						 (INVENTORY_ITEM_ETC == pItem->GetMainType())		||
				//						 (INVENTORY_ITEM_COSTUME == pItem->GetMainType())	||		// 2019-07-03
				//						 (INVENTORY_ITEM_ACCESSORY == pItem->GetMainType()) )  )
				if( NULL != pItem )
				{
					//if( INVENTORY_ITEM_ETC == pItem->GetMainType() )
					//{   
					//	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( pItem->GetMagicCode() );
					//	if( NULL != pEtcItem )
					//	{
					//		ioHashString szName = pEtcItem->GetName();
							if( m_szCurOveredSearchName != pItem->GetItemInfoSlotName() )
							{
								m_szCurOveredSearchName = pItem->GetItemInfoSlotName();
								m_nCurSelectCharIndex = pItem->GetSelectItemIndex();
								int nIndex = 0;
								for( vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin(); iter != m_vInventoryItemInfoList.end(); ++iter, ++nIndex )
								{
									InventoryItemInfo kInfo = *iter;
									if( pItem->GetMagicCode() == kInfo.m_iMagicCode )
									{
										m_nSelectCharIndex = nIndex;
										break;
									}
								}
							}
					//	}
					//	break;
					//}
					//else
					//{
					//	EXTRAITEMSLOT kSlot;
					//	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
					//	if( NULL == pExtraItem )	
					//	{
					//		break;
					//	}

					//	if( false !=  pExtraItem->GetExtraItem(pItem->GetMagicCode(), kSlot) )
					//	{
					//		ioHashString szName = kSlot.m_Name;
					//		if( m_szCurOveredSearchName != szName )
					//		{
					//			m_szCurOveredSearchName = szName;
					//			m_nCurSelectCharIndex = pItem->GetSelectItemIndex();
					//			int nIndex = 0;
					//			for( vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin(); iter != m_vInventoryItemInfoList.end(); ++iter, ++nIndex )
					//			{
					//				InventoryItemInfo kInfo = *iter;
					//				if( pItem->GetMagicCode() == kInfo.m_iMagicCode )
					//				{
					//					m_nSelectCharIndex = nIndex;
					//					break;
					//				}
					//			}
					//		}
					//	}
					//}
				}
			}
		}
		break;
	case ID_CHAR_INFO_WND_1_BTN:
	case ID_CHAR_INFO_WND_2_BTN:
	case ID_CHAR_INFO_WND_3_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 2019-07-08 검색중이면
			if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsShow()) && (true == IsSearchInputEnter()) && (InventorySoldierItemWnd::ID_PROMOTE_BTN != param) )
			{
				// 최종선택 캐릭터 
				InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(dwID));
				if( NULL != pItem )
				{
					m_szSearchName = g_MyInfo.GetClassName( pItem->GetClassType() );
				}

				// 2019-06-12
				ioWnd *pWnd = FindChildWnd(ID_SEARCH_HIDE_BTN);
				if( NULL != pWnd )
				{
					iwm_command( pWnd, IOBN_BTNUP, param ); 
				}

				if( m_dwSelectCharID > 0 )
				{
					dwID = m_dwSelectCharID;
					m_dwSelectCharID = 0;
				}       
			}

			SelectCharItemBtn( dwID, param );
		}
		else if( (cmd == IOWN_OVERED) && (true == IsSearchInputEnter()) ) // 2019-05-30
		{
			if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )		// 용병
			{	
				InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(pWnd);	
				if( NULL != pItem )
				{
					ioHashString szName = g_MyInfo.GetClassName( pItem->GetClassType() );
					if( m_szCurOveredSearchName != szName )
					{
						m_szCurOveredSearchName = szName;
						m_nCurSelectCharIndex = pItem->GetSelectCharIndex();
						int nIndex = 0;
						for( vSoldierItemInfoList::iterator iter = m_vCharInfoList.begin(); iter != m_vCharInfoList.end(); ++iter, ++nIndex )
						{
							SoldierItemInfo kInfo = *iter;
							if( pItem->GetClassType() == kInfo.m_iClassType )
							{
								m_nSelectCharIndex = nIndex;
								break;
							}
						}
					}
				}
			}
		}
		break;
	case ITEM_RESELL_WND:
		if( cmd == IOBN_BTNUP )
		{
			if( param != ACST_SELL && 
				param != ACST_DISASSEMBLE && 
				param != ACST_SELL_MEDAL &&
				param != ACST_SELL_COSTUME &&
				param != ACST_SELL_ACCESSORY &&
				param != ACST_SELL_DECO )
				break;

			switch( m_dwCurTabID )
			{
			case ID_EXTRA_TAB_BTN:
				if( m_iSellSlotIndex > 0 )
				{
					if( param == ACST_SELL )
					{
						if( !CheckTimeEndExtraItem( m_iSellSlotIndex ) )
							return;

						TCPNetwork::MouseBusy( true );

						SP2Packet kPacket( CTPK_EXTRAITEM_SELL );
						kPacket << m_iSellSlotIndex;
						TCPNetwork::SendToServer( kPacket );
					}
					else if( param == ACST_DISASSEMBLE )
					{
						if( !CheckTimeEndExtraItem( m_iSellSlotIndex ) )
							return;

						TCPNetwork::MouseBusy( true );

						SP2Packet kPacket( CTPK_EXTRAITEM_DISASSEMBLE );
						kPacket << m_iSellSlotIndex;
						TCPNetwork::SendToServer( kPacket );
					}
					else if( param == ACST_SELL_MEDAL )
					{
						TCPNetwork::MouseBusy( true );

						if( m_iSellSubIndex == 0 )
						{
							SP2Packet kPacket( CTPK_MEDALITEM_SELL );
							kPacket << m_iSellSlotIndex;
							TCPNetwork::SendToServer( kPacket );
						}
						else
						{
							SP2Packet kPacket( CTPK_CUSTOM_MEDALITEM_SELL );
							kPacket << m_iSellSlotIndex;
							kPacket << m_iSellSubIndex;
							TCPNetwork::SendToServer( kPacket );
						}
					}
				}
				break;
			case ID_COSTUME_TAB_BTN:
				if( m_iSellSlotIndex > 0 )
				{
					// UI코스튬 (아이템 판매)
					if( param == ACST_SELL_COSTUME )
					{
						if( !CheckTimeEndCostume( m_iSellSlotIndex ) )
							return;
												
						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_COSTUME_SELL );
						kPacket << m_iSellSlotIndex;
						TCPNetwork::SendToServer( kPacket );
					}
				}
				break;
			case ID_ACCESSORY_TAB_BTN:
				if( m_iSellSlotIndex > 0 )
				{
					if( param == ACST_SELL_ACCESSORY )
					{
						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_ACCESSORY_SELL );
						kPacket << m_iSellSlotIndex;
						TCPNetwork::SendToServer( kPacket );
					}
				}
				break;
			case ID_DECO_TAB_BTN:
				if( m_iSellSlotIndex > 0 )
				{
					if( param == ACST_SELL_DECO )
					{
						MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
						if( !pInfoWnd )
							return;

						CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();
						int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );

						TCPNetwork::MouseBusy( true );
						SP2Packet kPacket( CTPK_DECO_SELL );
						kPacket << iCharArray;
						kPacket << m_iSellSlotIndex;
						kPacket << m_iSellSubIndex;
						TCPNetwork::SendToServer( kPacket );
					}
				}
				break;
			}
			m_iSellSlotIndex = 0;
			m_iSellSubIndex = 0;
		}
		break;
	case ID_COSTUME_HELP:
		{
			if( cmd == IOBN_BTNUP )
			{
				HideChildWnd( ID_COSTUME_HELP_TIP );

				ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
				if( pManualWnd )
					pManualWnd->ShowWndByCurManual( m_nCostumeHelpTipIdx );
			}
			else if( cmd == IOWN_OVERED )
			{
				ShowChildWnd( ID_COSTUME_HELP_TIP );
			}
			else if( cmd == IOWN_LEAVED )
			{
				HideChildWnd( ID_COSTUME_HELP_TIP );
			}
		}
		break;
	case ID_BONUS_CASH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( !g_GUIMgr.IsShow( BONUS_CASH_WND ))
				g_GUIMgr.ShowWnd( BONUS_CASH_WND );
			else
				g_GUIMgr.HideWnd( BONUS_CASH_WND );
		}
		break;
	case MESSAGE_BOX:
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN && m_iSellSlotIndex > 0 )
				{
					TCPNetwork::MouseBusy( true );
					SP2Packet kPacket( CTPK_ACCESSORY_SELL );
					kPacket << m_iSellSlotIndex;
					TCPNetwork::SendToServer( kPacket );
				}
			}				
		}
		break;

	case ID_SEARCH_SHOW_BTN: // 2019-05-20
		{
			if( cmd == IOBN_BTNUP )
			{
				ioWnd* pWnd = FindChildWnd( ID_NEW_SEARCH_WND );
				if( pWnd )
				{					
					ShowChildWnd( ID_NEW_SEARCH_WND );
					HideChildWnd( ID_SEARCH_SHOW_BTN );
					ShowChildWnd( ID_SEARCH_HIDE_BTN );	

					if( ID_ETC_TAB_BTN == m_dwCurTabID )
					{
						HideChildWnd( ID_HOUSE_ITEM_BTN );	// 개인본부 꾸미기 아이템 
					}
					else if( ID_COSTUME_TAB_BTN == m_dwCurTabID )
					{
						HideChildWnd( ID_COSTUME_HELP );	// 코스튬 도움말
					}

					ChangeSmallTab( m_dwCurSmallTabID );	
				}
			}
		}
		break;
	case ID_SEARCH_HIDE_BTN: // 2019-05-27
		{
			if( cmd == IOBN_BTNUP )
			{
				ioWnd* pWnd = FindChildWnd( ID_NEW_SEARCH_WND );
				if( pWnd )
				{
					HideChildWnd( ID_NEW_SEARCH_WND );					
					HideChildWnd( ID_SEARCH_HIDE_BTN );
					ShowChildWnd( ID_SEARCH_SHOW_BTN );

					if( ID_ETC_TAB_BTN == m_dwCurTabID )
					{
						ShowChildWnd( ID_HOUSE_ITEM_BTN );	// 개인본부 꾸미기 아이템 
					}
					else if( ID_COSTUME_TAB_BTN == m_dwCurTabID )
					{
						ShowChildWnd( ID_COSTUME_HELP );	// 코스튬 도움말
					}

					SetSearchInputEnter(false);
	
					ChangeSmallTab( m_dwCurSmallTabID );
				}
			}
		}
		break;

	default:
		{
			//if( COMPARE( dwID, ID_SPIRIT_INFO_1, ID_SPIRIT_INFO_20+1) &&
			//	(cmd == IOBN_BTNUP) )
			if( COMPARE( dwID, ID_SPIRIT_INFO_1, ID_SPIRIT_INFO_20+1) )
			{
				if( cmd == IOBN_BTNUP )
				{
					// 2019-07-01
					for( int i=0; i < MAX_SPIRIT_BTN; ++i )           
					{
						SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(ID_SPIRIT_INFO_1 + i));
						if( !pItem )
							continue;
						
						pItem->SetCompleteSelectPos(false);
						m_bSearchComplete = false;
					}

					// 2019-06-13 검색중 
					if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsShow()) && (true == IsSearchInputEnter()) )
					{
						// 최종 선택
						SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(dwID));
						if( NULL != pItem )
						{
							m_szSearchName = g_SpiritMgr.GetName(pItem->GetItemCode());
						}

						ioWnd *pWnd = FindChildWnd(ID_SEARCH_HIDE_BTN);
						if( NULL != pWnd )
						{
							iwm_command( pWnd, IOBN_BTNUP, param ); 
						}

						// 2019-06-18
						if( m_dwSelectCharID > 0 )
						{
							dwID = m_dwSelectCharID;
							m_dwSelectCharID = 0;
						}
				
						int iStartArray = m_iCurPage * PAGE_PER_SPIRIT_NUM;
						m_iSelectArray = iStartArray + (dwID-ID_SPIRIT_INFO_1);

						for( int i=0; i < MAX_SPIRIT_BTN; ++i )           
						{
							SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(ID_SPIRIT_INFO_1 + i));
							if( !pItem )
								continue;

							int iCurArray = iStartArray + i;
							pItem->SetCompleteSelectPos(false);
							if( m_iSelectArray == iCurArray )
							{
								pItem->SetCompleteSelectPos(true);         
								m_bSearchComplete = true;
							}
						}

						return;
					}
					SetSelectSpiritClickAction( dwID );
				}
				else if( (cmd == IOWN_OVERED) && (true == IsSearchInputEnter()) ) // 2019-06-05
				{	
					SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(pWnd);
					if( NULL != pItem )
					{
						ioHashString szItemName = g_SpiritMgr.GetName(pItem->GetItemCode());

						if( m_szCurOveredSearchName != szItemName )
						{
							m_szCurOveredSearchName = szItemName;
							m_nCurSelectCharIndex = pItem->GetSelectItemIndex();
							int nIndex = 0;
							for( vAlchemicItemInfoList::iterator iter = m_vAlchemicItemInfoList.begin(); iter != m_vAlchemicItemInfoList.end(); ++iter, ++nIndex )
							{
								AlchemicItemInfo kInfo = *iter;
								if( kInfo.m_szName == szItemName )
								{ 
									m_nSelectCharIndex = nIndex;
									break;
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}

void MyInventoryWnd::SetSelectSpiritClickAction( DWORD dwID )
{
	SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(dwID));
	if( !pItem )
		return;

	HideChildWnd(ID_ITEM_RECVSELL_INFO_WND);

	SpiritFuncWnd *pSpiritWnd = dynamic_cast<SpiritFuncWnd*>(FindChildWnd(ID_SPIRITFUNC_WND));
	if( pSpiritWnd && !pItem->IsLockState() )
	{
		pSpiritWnd->SelectClickAction( pItem->GetItemCode(), pItem->GetItemCount(), pItem->IsSoldier(), pItem->IsCompose() );
		if( !pSpiritWnd->IsShow() )
			pSpiritWnd->ShowWnd();
	}
}

void MyInventoryWnd::UpdatePage( int iNewPage )
{
	for ( int i = 0; i < MAX_ITEM_INFO_WND_BTN; ++i )
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	for ( int i= 0; i < MAX_CHAR_INFO_WND_BTN; ++i )
		HideChildWnd( ID_CHAR_INFO_WND_1_BTN + i );

	for ( int i=0; i < MAX_SPIRIT_BTN; ++i )
		HideChildWnd( ID_SPIRIT_INFO_1 + i );

	m_iCurPage = iNewPage;

	int iCharItemSize = m_vCharInfoList.size();
	int iItemSize = m_vInventoryItemInfoList.size();
	int iAlchemicSize = m_vAlchemicItemInfoList.size();
	if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
	{
		for( int i=0; i < MAX_SPIRIT_BTN; ++i )
		{
			SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(ID_SPIRIT_INFO_1 + i));
			if( !pItem )
				continue;

			pItem->ShowWnd();
			int iIndex = m_iCurPage * PAGE_PER_SPIRIT_NUM + i;
			if( COMPARE( iIndex, 0, iAlchemicSize ) )
				pItem->SetInfo( m_vAlchemicItemInfoList[iIndex].m_iCode,
								m_vAlchemicItemInfoList[iIndex].m_iCount,
								m_vAlchemicItemInfoList[iIndex].m_szIcon );
			else
				pItem->SetInfo( 0, 0, ioHashString() );

			// 2019-06-10
			pItem->SetSelectItemIndex(-1);
			pItem->SetCompleteSelectPos(false);
			if( true == IsSearchInputEnter() )
			{
				pItem->SetSelectItemIndex(i);
			}
		}
	}
	else if( m_bCharItem )
	{
		int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;
		for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
		{
			InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
			if( !pItem )
				continue;

			pItem->ShowWnd();

			int iCurArray = iStartArray +i;
			if( !COMPARE( iCurArray, 0, iCharItemSize ) )
			{
				// 초기화
				pItem->SetInfo( -1 );
				continue;
			}

			pItem->SetInfo( m_vCharInfoList[iCurArray].m_iClassType );

			// 2019-05-27
			pItem->SetSelectCharIndex(-1);
			if( true == IsSearchInputEnter() )
			{
				pItem->SetSelectCharIndex(i);
			}

			// SetInfo 이후 적용
			if( m_iSelectArray == iCurArray )
				pItem->SetSelect( true );
			else
				pItem->SetSelect( false );
		}
	}
	else
	{
		int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
		for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
		{
			InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
			if( !pItem )
				continue;

			pItem->ShowWnd();

			int iCurArray = iStartArray +i;
			if( !COMPARE( iCurArray, 0, iItemSize ) )
			{
				// 초기화
				InventoryItemInfo kInfo;
				pItem->SetInfo( m_pPlayStage, kInfo );
				continue;
			}

			pItem->SetInfo( m_pPlayStage, m_vInventoryItemInfoList[iCurArray] );

			// 2019-06-04
			pItem->SetSelectItemIndex(-1);
			pItem->SetCompleteSelectPos(false);
			if( true == IsSearchInputEnter() )
			{
				pItem->SetSelectItemIndex(i);
			}

			// SetInfo 이후 적용
			if( m_iSelectArray == iCurArray )
			{
				pItem->SetSelect( true );
			}
			else
			{
				pItem->SetSelect( false );
			}
		}
	}
}

void MyInventoryWnd::SetFirstBtnInfo()
{
	// 각 텝의 첫번째걸로 오른쪽 창에 정보 전달하는 함수.
	// 일반적인 형태로 텝 변경할 경우 호출.
	if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
		return;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return;

	pInfoWnd->HideWnd();

	switch( m_dwCurTabID )
	{
	case ID_SOLDIER_TAB_BTN:
		{
			if( m_bCharItem )
			{
				int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;

				if( !m_vCharInfoList.empty() )
				{
					if( (g_MyInfo.GetCharIndexByClassType( m_iSelectClassType ) <= 0) ||
						(FindSelectCharArray( m_iSelectClassType ) == -1) )
					{
						m_iSelectClassType = m_vCharInfoList[0].m_iClassType;
						m_iSelectArray = iStartArray;
					}
					else
					{
						m_iSelectArray = FindSelectCharArray( m_iSelectClassType );
					}

					ShowMainInfoWnd();
					pInfoWnd->SetSlodierInfo( m_iSelectClassType );
				}

				// 해당 용병이 있는 페이지로 갱신
				int iNewPage = m_iSelectArray / PAGE_PER_CHAR_NUM;
				ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
				if( pScroll )
				{
					pScroll->SetScrollPos( iNewPage );
				}

				for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
				{
					InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
					if( !pItem ) continue;

					// 2019-05-27
					pItem->SetSelectCharIndex(-1);
					if( true == IsSearchInputEnter() )
					{
						pItem->SetSelectCharIndex(i);
					}

					int iCurArray = iStartArray + i;
					if( m_iSelectArray == iCurArray )
						pItem->SetSelect( true );
					else
						pItem->SetSelect( false );
				}
			}
			else
			{
				if( !m_vCharInfoList.empty() )
				{
					if( g_MyInfo.GetCharIndexByClassType( m_iSelectClassType ) <= 0 )
						m_iSelectClassType = m_vCharInfoList[0].m_iClassType;

					ShowMainInfoWnd();
					pInfoWnd->SetSlodierInfo( m_iSelectClassType );
				}

				int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
				m_iSelectArray = -1;

				for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
				{
					InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
					if( !pItem )
						continue;

					pItem->SetSelect( false );
				}
			}
		}
		break;
	case ID_EXTRA_TAB_BTN:
	case ID_DECO_TAB_BTN:
		// UI코스튬 관련 (첫 아이템 선택)
	case ID_COSTUME_TAB_BTN:
	case ID_ACCESSORY_TAB_BTN:
		{
			ShowMainInfoWnd();

			if( g_MyInfo.GetCharIndexByClassType( m_iSelectClassType ) > 0 )
				pInfoWnd->SetItemBlank( m_dwCurTabID, m_dwCurSmallTabID, m_iSelectClassType );
			else
			{
				if( !m_vCharInfoList.empty() )
				{
					m_iSelectClassType = m_vCharInfoList[0].m_iClassType;
					if( g_MyInfo.GetCharIndexByClassType( m_iSelectClassType ) <= 0 )
						m_iSelectClassType = g_MyInfo.GetClassType();
				}
				
				if( m_iSelectClassType == -1 )
					m_iSelectClassType = g_MyInfo.GetClassType();

				pInfoWnd->SetItemBlank( m_dwCurTabID, m_dwCurSmallTabID, m_iSelectClassType );
			}

			int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
			m_iSelectArray = -1;

			for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
			{
				InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
				if( !pItem )
					continue;

				pItem->SetSelect( false );
			}
		}
		break;
	case ID_ETC_TAB_BTN:
	case ID_PRESENT_TAB_BTN:
		{
			ShowMainInfoWnd();
			if( !m_vInventoryItemInfoList.empty() )
			{
				pInfoWnd->SetInvenItemInfo( m_iSelectClassType, m_vInventoryItemInfoList[0] );
			}
			else
			{
				pInfoWnd->SetItemBlank( m_dwCurTabID, m_dwCurSmallTabID, m_iSelectClassType );
			}

			int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
			m_iSelectArray = iStartArray;

			for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
			{
				InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
				if( !pItem )
					continue;

				int iCurArray = iStartArray + i;
				if( m_iSelectArray == iCurArray )
				{
					pItem->SetSelect( true );
				}
				else
				{
					pItem->SetSelect( false );
				}
			}
		}
		break;
	}
}

DWORD MyInventoryWnd::GetDefaultSmallTab( DWORD dwMainTab )
{
	int iSmallTab = 0;

	if( dwMainTab == ID_ALCHEMIC_TAB_BTN )
	{
		return ID_SMALL_TAB_1_BTN;
	}

	int iArray = ( dwMainTab - ID_SOLDIER_TAB_BTN );
	if( COMPARE( iArray, 0, MAX_TAB_BTN ) )
	{
		int iSmallSize = m_vSmallTabTypeVecArray[iArray].size();
		for( int i=0; i < iSmallSize; ++i )
		{
			if( m_vSmallTabTypeVecArray[iArray][i] == m_iDefaultTypeArray[iArray] )
				iSmallTab = ID_SMALL_TAB_1_BTN + i;
		}
	}

	return iSmallTab;
}

void MyInventoryWnd::ChangeMainTab( DWORD dwID )
{
	// 2019-04-30
	ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
	if( NULL != pExtraItemWnd )
	{
		pExtraItemWnd->SetRollingStopSound();
	}

	// 2019-06-05
	ShowSearchWnd( dwID );

	m_dwCurTabID = dwID;
	CheckRadioButton( ID_SOLDIER_TAB_BTN, ID_ACCESSORY_TAB_BTN, dwID );

	int iArray = dwID-ID_SOLDIER_TAB_BTN;
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Array is wrong.", __FUNCTION__ );
		return;
	}

	enum
	{
		NOR_WIDTH	= 55,
		NEW_WIDTH	= 94,

		NOR_GAP		= 56,
		NEW_GAP		= 95,

		NOR_TITLE_X	= 27,
		NOR_TITLE_Y	= 4,
		NEW_TITLE_X	= 47,
		NEW_TITLE_Y	= 4,
	};

	int iXPos = 27;
	int iYPos = 91;

	int iCurWidth = NOR_WIDTH;
	int iCurGap = NOR_GAP;
	int iCurTitleX = NOR_TITLE_X;
	int iCurTitleY = NOR_TITLE_Y;

	if( m_dwCurTabID == ID_PRESENT_TAB_BTN )
	{
		iCurWidth = NEW_WIDTH;
		iCurGap = NEW_GAP;

		iCurTitleX = NEW_TITLE_X;
		iCurTitleY = NEW_TITLE_Y;
	}
	int iXOffset = 0;
	int iSize = m_vSmallTabNameVecArray[iArray].size();
	for (int i = 0; i < MAX_SMALL_TAB_BTN; i++)
	{
		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( ID_SMALL_TAB_1_BTN + i ) );
		if( !pButton )
			continue;
		if( COMPARE( i, 0, iSize ) )
		{
			if( i == 0 )
				iXOffset = pButton->GetXPos();

			if( i == iSize-1 )			
				pButton->SetDisable( true );
			else
				pButton->SetDisable( false );

			ShowChildWnd( ID_SMALL_TAB_1_BTN + i );
			SetChildActive( ID_SMALL_TAB_1_BTN  + i );

			iXOffset += UpdateSmallTabName( pButton, iXOffset, m_vSmallTabNameVecArray[iArray][i] );
		}
		else
		{
			HideChildWnd( ID_SMALL_TAB_1_BTN + i );
		}
	}

	int iAddID = (iSize-1);
	if( !COMPARE( iAddID, 0, MAX_SMALL_TAB_BTN ) )
		iAddID = 0;
	m_dwSmallTabEndID = ID_SMALL_TAB_1_BTN+iAddID;

	ChangeSmallTab( ID_SMALL_TAB_1_BTN );

	// UI코스튬 관련 (공략 물음표 버튼 설정)
	if ( dwID == ID_COSTUME_TAB_BTN )
	{
		ShowChildWnd( ID_COSTUME_HELP );
		HideChildWnd( ID_COSTUME_HELP_TIP );
	}
	else
	{
		HideChildWnd( ID_COSTUME_HELP );
		HideChildWnd( ID_COSTUME_HELP_TIP );
	}
}

void MyInventoryWnd::ChangeSmallTab( DWORD dwID )
{
	// 2019-04-30
 	ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
	if( NULL != pExtraItemWnd )
	{
		pExtraItemWnd->SetRollingStopSound();
	}

	HideChildWnd( ID_HOUSE_ITEM_BTN ); 
	HideChildWnd( ID_REFRESH_BTN ); 
	CheckRadioButton( ID_SMALL_TAB_1_BTN, m_dwSmallTabEndID, dwID );
	m_dwCurSmallTabID = dwID;

	// 2019-05-29
	m_nSelectCharIndex = 0;	
	m_nCurSelectCharIndex = 0;
	m_nCurOveredCharIndex = -1;

	m_bCharItem = false;
	if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )
	{
		SetSoldierItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}
	else if( m_dwCurTabID == ID_EXTRA_TAB_BTN )
	{
		SetExtraItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}
	else if( m_dwCurTabID == ID_DECO_TAB_BTN )
	{
		SetDecoItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}
	else if( m_dwCurTabID == ID_ETC_TAB_BTN )
	{
		SetEtcItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
		ShowChildWnd( ID_HOUSE_ITEM_BTN );

		// 검색중 비활성화 2019-06-18
		if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsShow()) )
		{
			HideChildWnd( ID_HOUSE_ITEM_BTN );
		}
	}
	else if( m_dwCurTabID == ID_PRESENT_TAB_BTN )
	{
		SetPresentItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
		ShowChildWnd( ID_REFRESH_BTN ); 
	}
	else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
	{
		SetAlchemicItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}
	else if( m_dwCurTabID == ID_COSTUME_TAB_BTN )
	{
		// UI코스튬 관련 (아이템 데이터 추가)
		IsHideSmallTabSearch( m_dwCurSmallTabID );	// 2019-06-18
		SetCostumeInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}
	else if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN )
	{
		SetAccessoryInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	}

	m_iCurPage = 0;

	// 2019-06-18
	if( true != m_szSearchName.IsEmpty() )
	{
		int nIndex = 0;
		if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )		// 용병
		{
			for( vSoldierItemInfoList::iterator iter = m_vCharInfoList.begin(); iter != m_vCharInfoList.end(); ++iter, ++nIndex )
			{
				SoldierItemInfo kInfo = *iter;

				ioHashString szClassName= g_MyInfo.GetClassName( kInfo.m_iClassType );
				if( szClassName == m_szSearchName )
				{
					m_iCurPage = nIndex / PAGE_PER_CHAR_NUM;
					m_szSearchName = "";
					m_dwSelectCharID = nIndex + ID_CHAR_INFO_WND_1_BTN;
					if( nIndex >= MAX_CHAR_INFO_WND_BTN )
					{
						m_dwSelectCharID = ((nIndex - MAX_CHAR_INFO_WND_BTN)%PAGE_PER_CHAR_NUM) + ID_CHAR_INFO_WND_1_BTN;
					}
				}
			}
		}
		else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)			||	// 2019-06-18 장비,치장,특별,보관함,코스튬, 액세서리
				 (m_dwCurTabID == ID_ETC_TAB_BTN)			||
				 /*(m_dwCurTabID == ID_PRESENT_TAB_BTN)		||*/
				 /*(m_dwCurTabID == ID_DECO_TAB_BTN)		||*/
				 (m_dwCurTabID == ID_COSTUME_TAB_BTN)		||
				 (m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )	
		{
			for( vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin(); iter != m_vInventoryItemInfoList.end(); ++iter, ++nIndex )
			{
				InventoryItemInfo kInfo = *iter;
				if( (kInfo.m_szItemName == m_szSearchName) && (m_nSearchIndex == kInfo.m_nSlotIndex) )
				{
					m_szSearchName = "";

					m_iCurPage = nIndex / PAGE_ITEM_INFO_NUM;					
					if( m_iCurPage > 0 ) m_iCurPage = m_iCurPage - 1;

					m_dwSelectCharID = nIndex + ID_ITEM_INFO_WND_1_BTN;					
					if( nIndex > PAGE_ITEM_INFO_NUM )
					{
						m_dwSelectCharID = ((nIndex - PAGE_ITEM_INFO_NUM)%PAGE_ITEM_INFO_NUM) + ID_ITEM_INFO_WND_5_BTN;
					}
					break;
				}
			}
		}
		else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )	// 2019-06-13 정기
		{
			for( vAlchemicItemInfoList::iterator iter = m_vAlchemicItemInfoList.begin(); iter != m_vAlchemicItemInfoList.end(); ++iter, ++nIndex )
			{
				AlchemicItemInfo kInfo = *iter;
				if( kInfo.m_szName == m_szSearchName )
				{
					m_szSearchName = "";

					m_iCurPage = 0;
					if( (nIndex / PAGE_PER_SPIRIT_NUM) > 3 )
					{
						m_iCurPage = (nIndex / PAGE_PER_SPIRIT_NUM) - 3;
					}

					m_dwSelectCharID = nIndex + ID_SPIRIT_INFO_1;
					if( nIndex > MAX_SPIRIT_BTN )
					{
						//m_dwSelectCharID = ((m_nSelectCharIndex - PAGE_PER_SPIRIT_NUM)%PAGE_PER_SPIRIT_NUM) + ID_SPIRIT_INFO_1;
						m_dwSelectCharID = (nIndex%PAGE_PER_SPIRIT_NUM) + (ID_SPIRIT_INFO_1 + (PAGE_PER_SPIRIT_NUM * 3));
					}
					break;
				}
			}
		}
	}

	// 2019-05-23
	UpdateSmallTabSearchCategoryTitle();
	
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	bool bSpirit = false;
	if( pScroll )
	{
		if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
			bSpirit = true;

		int iCharItemSize = m_vCharInfoList.size();
		int iItemSize = m_vInventoryItemInfoList.size();
		int iSpiritSize = m_vAlchemicItemInfoList.size();
		if( m_bCharItem )
		{
			int iMaxPos =  iCharItemSize / PAGE_PER_CHAR_NUM;
			if( iCharItemSize % PAGE_PER_CHAR_NUM )
				iMaxPos++;

			pScroll->SetPageRange(1);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}
		else if( bSpirit )
		{
			int iMaxPos =  iSpiritSize / PAGE_PER_SPIRIT_NUM;
			if( iSpiritSize % PAGE_PER_SPIRIT_NUM )
				iMaxPos++;

			pScroll->SetPageRange(4);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}
		else
		{
			int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
			if( iItemSize % PAGE_PER_ITEM_NUM )
				iMaxPos++;

			pScroll->SetPageRange(2);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}

		pScroll->ShowWnd();
	}

#if defined( USE_GA )
	switch( m_dwCurTabID )
	{
	case ID_SOLDIER_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_HERO_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_HERO_PREMIUM
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FPREMIUM" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_INVEN_HERO_MELEE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FMELEE" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_INVEN_HERO_RANGED
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FRANGED" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_INVEN_HERO_MAGIC
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FMAGIC" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_INVEN_HERO_SPECIAL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FSPECIAL" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_INVEN_HERO_PACKAGE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FHERO%2FPACKAGE" );
				}
				break;
			}
		}
		break;

	case ID_EXTRA_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_GEAR_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_GEAR_WEAPON
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FWEAPON" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_INVEN_GEAR_ARMOR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FARMOR" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_INVEN_GEAR_HELMET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FHELMET" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_INVEN_GEAR_TRINKET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FTRINKET" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_INVEN_GEAR_MEDAL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FMEDAL" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_INVEN_GEAR_FEATURES
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FGEAR%2FFEATURES" );
				}
				break;
			}
		}
		break;

	case ID_DECO_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_DECO_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_DECO_GENDER
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FGENDER" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_INVEN_DECO_HAIR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FHAIR" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_INVEN_DECO_DYE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FDYE" );
				}
				break;
			case ID_SMALL_TAB_5_BTN:
				{
					// WND_INVEN_DECO_FACE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FFACE" );
				}
				break;
			case ID_SMALL_TAB_6_BTN:
				{
					// WND_INVEN_DECO_SKIN
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FSKIN" );
				}
				break;
			case ID_SMALL_TAB_7_BTN:
				{
					// WND_INVEN_DECO_UNDIES
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FUNDIES" );
				}
				break;
			case ID_SMALL_TAB_8_BTN:
				{
					// WND_INVEN_DECO_PACKAGE
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FDECO%2FPACKAGE" );
				}
				break;
			}
		}
		break;	

	case ID_ETC_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_SPECIAL_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSPECIAL%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_SPECIAL_BOOST
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSPECIAL%2FBOOST" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_INVEN_SPECIAL_PREMIUM
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSPECIAL%2FPREMIUM" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_INVEN_SPECIAL_ETC
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSPECIAL%2FETC" );
				}
				break;
			}
		}
		break;

	case ID_PRESENT_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_STORAGE_GIFT
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSTORAGE%2FGIFT" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_STORAGE_TEMP
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FSTORAGE%2FTEMP" );
				}
				break;
			}
		}
		break;

	case ID_COSTUME_TAB_BTN:
		{
			switch( m_dwCurSmallTabID )
			{
			case ID_SMALL_TAB_1_BTN:
				{
					// WND_INVEN_COSTUME_ALL
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FCOSTUME%2FALL" );
				}
				break;
			case ID_SMALL_TAB_2_BTN:
				{
					// WND_INVEN_COSTUME_ARMOR
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FCOSTUME%2FARMOR" );
				}
				break;
			case ID_SMALL_TAB_3_BTN:
				{
					// WND_INVEN_COSTUME_HELMET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FCOSTUME%2FHELMET" );
				}
				break;
			case ID_SMALL_TAB_4_BTN:
				{
					// WND_INVEN_COSTUME_TRINKET
					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FCOSTUME%2FTRINKET" );
				}
				break;
			}
		}
		break;
	case ID_ALCHEMIC_TAB_BTN:
		{
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FINVEN%2FCRAFT" );
		}
		break;
	}
#endif

}

void MyInventoryWnd::SetSoldierItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	if( dwTabID != ID_SOLDIER_TAB_BTN )
	{
		return;
	}

	DWORD dwCheckSmallTabID = dwSmallTabID;

	m_bCharItem = true;
	if( IsRightSmallTab(dwTabID, dwSmallTabID, ETC_ITEM_OTHER_VALUE) )
	{
		m_bCharItem = false;

		// Etc Soldier 추가
		AddItemInfoVecFromEtcItem( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_SOLDIER );

		if( !m_vInventoryItemInfoList.empty() )
		{
			std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort() );
		}

		dwCheckSmallTabID = ID_SMALL_TAB_1_BTN;
	}

	// Soldier
	int iMax = g_MyInfo.GetCharCount();
	for ( int i=0; i < iMax; i++ )
	{
		int iClassType = g_MyInfo.GetClassType( i );
		int iSetIdx = iClassType - 1;
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo )
			continue;

		if( !IsRightSmallTab( dwTabID, dwCheckSmallTabID, pInfo->GetCharAttackType() ) )
			continue;

		SoldierItemInfo kInfo;
		kInfo.m_iClassType = iClassType;
		kInfo.m_iPriorityOrder = g_MyInfo.GetCharArrayToSlotIndex( i );

		// 2019-05-23 
		if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
		{
			//// 2019-06-24 기간제
			//int iCharArray = g_MyInfo.GetClassArray( iClassType );
			//if( -1 >= iCharArray )
			//{
			//	continue;
			//}

			if( false == m_pNewSearchWnd->IsSearch( pInfo->GetName(), m_pNewSearchWnd->GetFindKeyWord()) )
			{
				continue;
			}
		}

		m_vCharInfoList.push_back( kInfo );
	}

	// 보유중이지 않은 용병...
	int iMaxClass = g_ClassPrice.MaxClassPrice();
	for ( int i=0; i < iMaxClass; i++ )
	{
		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		if( !IsRightSmallTab( dwTabID, dwCheckSmallTabID, pInfo->GetCharAttackType() ) )
			continue;

		int iClassType = iSetIdx + 1;
		if( g_MyInfo.GetCharIndexByClassType( iClassType ) > 0 )		// 보유중인 용병은 패스
			continue;

		if( !g_MyInfo.IsClassTypeInventory( iClassType ) )		// 한 번도 고용된적 없으면 패스
			continue;

		SoldierItemInfo kInfo;
		kInfo.m_iClassType = iClassType;
		kInfo.m_iPriorityOrder = 1000 + iClassType;

		// 2019-05-23 
		if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
		{
			//// 2019-06-24 기간제
			//int iCharArray = g_MyInfo.GetClassArray( iClassType );
			//if( -1 >= iCharArray )
			//{
			//	continue;
			//}

			if( false == m_pNewSearchWnd->IsSearch( pInfo->GetName(), m_pNewSearchWnd->GetFindKeyWord()) )
			{
				continue;
			}
		}

		m_vCharInfoList.push_back( kInfo );
	}

	if( m_vCharInfoList.empty() )
		return;

	std::sort( m_vCharInfoList.begin(), m_vCharInfoList.end(), SoldierItemInfoSort() );
}

void MyInventoryWnd::SetExtraItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return;

	CHARACTER rkCharInfo;
	int iCharArray = g_MyInfo.GetClassArray( m_iSelectClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		rkCharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( rkCharInfo, m_iSelectClassType );

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		int iGroup = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightSmallTab( dwTabID, dwSmallTabID, iGroup ) )
			continue;

		if( kSlot.m_bCharEquip )
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

		// 2019-06-18	
		kInfo.m_szItemName = kSlot.m_Name;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	// Etc ExtraItem 추가
	AddItemInfoVecFromEtcItem( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_EXTRA );

	// medalitem extraitem 추가
	AddItemInfoVecFromMedalItem( dwTabID, dwSmallTabID );

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwSmallTabID == ID_SMALL_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
	{
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );
	}

	int iArrayCnt = 0;
	for( iArrayCnt; iArrayCnt < (int)m_vInventoryItemInfoList.size(); ++iArrayCnt )
	{
		m_vInventoryItemInfoList[iArrayCnt].m_nSlotIndex = iArrayCnt;
	}

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

void MyInventoryWnd::SetDecoItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	CHARACTER kCurCharInfo;

	if( g_MyInfo.GetCharIndexByClassType( m_iSelectClassType ) <= 0 )
		m_iSelectClassType = g_MyInfo.GetClassType();

	int iCharArray = g_MyInfo.GetClassArray( m_iSelectClassType );
	if( iCharArray == - 1 )
		kCurCharInfo = g_MyInfo.GetCharacter();
	else
		kCurCharInfo = g_MyInfo.GetCharacter( iCharArray );

	int iCurSexType = kCurCharInfo.m_sex - 1;

	int i = 0;
	int iSexSize = g_DecorationPrice.GetSexSize();
	for (i = 0; i < iSexSize ; i++)
	{
		InventoryItemInfo kInfo;

		kInfo.m_iMainType = INVENTORY_ITEM_DECO;
		kInfo.m_iSexType = g_DecorationPrice.GetSexType( i );
		if( kInfo.m_iSexType != iCurSexType )
			continue;

		int iDecoTypeSize = g_DecorationPrice.GetDecoTypeSize( i );
		for (int j = 0; j < iDecoTypeSize; j++)
		{
			kInfo.m_iDecoType = g_DecorationPrice.GetDecoType( i , j );

			if( !IsRightSmallTab( dwTabID, dwSmallTabID, kInfo.m_iDecoType ) )
				continue;

			IntVec vBoughtList;
			int iDecoSize = g_DecorationPrice.GetDecoSize( i, j );
			for (int z = 0; z < iDecoSize; z++)
			{
				int iDecoCode = g_DecorationPrice.GetDecoCode( i, j, z );

				if( IsBoughtDeco( kCurCharInfo.m_class_type, kInfo.m_iSexType, kInfo.m_iDecoType, iDecoCode ) )
				{
					vBoughtList.push_back( iDecoCode );
				}
			}

			int iBoughtCnt = vBoughtList.size();
			for( int k=0; k < iBoughtCnt; ++k )
			{
				kInfo.m_iDecoCode = vBoughtList[k];
				kInfo.m_bEquip = IsEquipDeco( kCurCharInfo, kInfo.m_iDecoType, kInfo.m_iDecoCode );

				if( kInfo.m_bEquip )
					continue;

				kInfo.m_iPriorityOrder = j * 10000;

				kInfo.m_bActive = false;
				if( iBoughtCnt > 1 )
					kInfo.m_bActive = true;

				kInfo.m_iPriorityOrder += 1000;
				kInfo.m_iPriorityOrder += k;

				// 2019-06-18	
				const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( kInfo.m_iSexType, kInfo.m_iDecoType, kInfo.m_iDecoCode );
				if( NULL != pData )
				{
					kInfo.m_szItemName = pData->m_szName;
				}

				m_vInventoryItemInfoList.push_back( kInfo );
			}
		}
	}

	// Etc Deco 추가
	AddItemInfoVecFromEtcItem( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_DECO );

	if( m_vInventoryItemInfoList.empty() )
		return;

	std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), EtcItemPrioritySort() );

	int iArrayCnt = 0;
	for( iArrayCnt; iArrayCnt < (int)m_vInventoryItemInfoList.size(); ++iArrayCnt )
	{
		m_vInventoryItemInfoList[iArrayCnt].m_nSlotIndex = iArrayCnt;
	}

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

void MyInventoryWnd::SetEtcItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	// etc
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return;
	}

	int iMax = g_EtcItemMgr.GetEtcItemCount();
	for (int i = 0; i < iMax ; i++)
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.GetEtcItemByIdx( i );
		if( !pEtcItem ) continue;

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ))
		{
			bool bAdd = false;
			int iSize = pEtcItem->GetValueSize();
			for (int j = 0; j < iSize ; j++)
			{
				int iShopTabType = pEtcItem->GetShopTabType(j);
				if( iShopTabType != SHOP_TAB_TYPE_NONE && iShopTabType != SHOP_TAB_TYPE_ETC && iShopTabType != SHOP_TAB_TYPE_LIMIT )
					continue;

				if( !IsRightSmallTab( dwTabID, dwSmallTabID, pEtcItem->GetGroup() ) )
					continue;

				bAdd = true;
			}

			if( bAdd )
			{
				InventoryItemInfo kInfo;
				kInfo.m_iMainType = INVENTORY_ITEM_ETC;
				kInfo.m_iMagicCode = pEtcItem->GetType();
				kInfo.m_iManualIndex = pEtcItem->GetInventoryManual()-1;
				kInfo.m_iPriorityOrder = pEtcItem->GetInventoryOrder();

				// 2019-06-18	
				kInfo.m_szItemName = pEtcItem->GetName();

				m_vInventoryItemInfoList.push_back( kInfo );
			}
		}
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort() );

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

void MyInventoryWnd::SetPresentItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	// HARDCODE:: 타입값을 xml 파일에서 바꿀경우 함께 바꿔줘야함
	int iSmallTabType = GetSmallTabType( dwTabID, dwSmallTabID );
	if( iSmallTabType == 0 )			// 선물
	{
		int iSize = g_PresentMgr.GetMaxPresent();
		for( int i=0; i < iSize; ++i )
		{
			DWORD dwSlotIndex = 0;
			DWORD dwIndex = g_PresentMgr.GetPresentIndexToArray( i, dwSlotIndex );

			ioHashString szSendID;
			short sPresentType, sPresentMent;
			int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
			DWORD dwLimitDate;

			g_PresentMgr.GetPresentDataToIndex( dwIndex, dwSlotIndex, szSendID, sPresentType, sPresentMent,	iPresentValue1,
												iPresentValue2,	iPresentValue3,	iPresentValue4,	dwLimitDate );

			InventoryItemInfo kInfo;
			kInfo.m_iMainType = INVENTORY_ITEM_PRESENT;
			kInfo.m_iMagicCode = dwIndex;
			kInfo.m_iMagicCodeEx = dwSlotIndex;
			kInfo.m_iManualIndex = sPresentMent;
			kInfo.m_iPriorityOrder = i;

			// 2019-06-18	
			kInfo.m_szItemName = g_PresentMgr.GetPresentValue1Text(sPresentType, iPresentValue1,iPresentValue2).c_str(); ;

			m_vInventoryItemInfoList.push_back( kInfo );
		}
	}
	else if( iSmallTabType == ETC_ITEM_OTHER_VALUE )		// 임시보관함
	{
		int iSize = g_SubscriptionMgr.GetMaxSubscription();
		for( int i=0; i < iSize; ++i )
		{
			ioSubscriptionMgr::SubscriptionData kData;

			if( g_SubscriptionMgr.GetSubscriptionDataToArray( i, kData ) )
			{
				InventoryItemInfo kInfo;
				kInfo.m_iMainType = INVENTORY_ITEM_SUBSCRIPTION;
				kInfo.m_iMagicCode = kData.m_dwIndex;
				kInfo.m_szSubscriptionID = kData.m_szSubscriptionID;
				kInfo.m_iSubscriptionGold = kData.m_iSubscriptionGold;
				kInfo.m_iSubscriptionBonusGold = kData.m_iSubscriptionBonusGold;
				kInfo.m_iManualIndex = kData.m_iMentType;
				kInfo.m_iPriorityOrder = i;		

				m_vInventoryItemInfoList.push_back( kInfo );
			}
		}
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort4() );

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

// UI코스튬 (아이템 데이터 얻기)
void MyInventoryWnd::SetCostumeInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume ) 
		return;
	
	int nSize = pCostume->GetCostumeSlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		CostumeSlot kSlot;
		if( !pCostume->GetCostumeSlotArray( i, kSlot ) )
			continue;

		// 스몰탭 인덱스랑 같아야함
		int nGroup = (kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE) + 1;
		if( !IsRightSmallTab( dwTabID, dwSmallTabID, nGroup ) )
			continue;

		if( kSlot.m_bEquip )
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

		// 2019-06-18	
		kInfo.m_szItemName = kSlot.m_Name;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	// Etc Costume 추가
	AddItemInfoVecFromEtcItem( dwTabID, dwSmallTabID, SHOP_TAB_TYPE_COSTUME );
	
	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwSmallTabID == ID_SMALL_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

void MyInventoryWnd::SetAlchemicItemInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vCharInfoList.clear();
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	g_SpiritMgr.GetSpiritInfo( m_vAlchemicItemInfoList, GetSmallTabType( dwTabID, dwSmallTabID ) );

	// 2019-05-30
	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{
		vAlchemicItemInfoList::iterator iter = m_vAlchemicItemInfoList.begin();
		while( iter != m_vAlchemicItemInfoList.end() )
		{		
			if( (NULL != m_pNewSearchWnd) && (false == m_pNewSearchWnd->IsSearch((*iter).m_szName, m_pNewSearchWnd->GetFindKeyWord())) )
			{
				iter = m_vAlchemicItemInfoList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	std::sort( m_vAlchemicItemInfoList.begin(), m_vAlchemicItemInfoList.end(), SpiritSort() );
}

void MyInventoryWnd::AddItemInfoVecFromPresent( DWORD dwTabID, int iValue1, int iValue2 )
{
	int iSize = g_PresentMgr.GetMaxPresent();
	for( int i=0; i < iSize; ++i )
	{
		DWORD dwSlotIndex = 0;
		DWORD dwIndex = g_PresentMgr.GetPresentIndexToArray( i, dwSlotIndex );

		ioHashString szSendID;
		short sPresentType, sPresentMent;
		int iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4;
		DWORD dwLimitDate;

		g_PresentMgr.GetPresentDataToIndex( dwIndex, dwSlotIndex, szSendID, sPresentType, sPresentMent,	iPresentValue1, iPresentValue2,	iPresentValue3,	iPresentValue4,	dwLimitDate );
		if( iValue1 == iPresentValue1 && iValue2 == iPresentValue2 )
		{
			InventoryItemInfo kInfo;
			kInfo.m_iMainType = INVENTORY_ITEM_PRESENT;
			kInfo.m_iMagicCode = dwIndex;
			kInfo.m_iMagicCodeEx = dwSlotIndex;
			kInfo.m_iManualIndex = sPresentMent;
			kInfo.m_iPriorityOrder = i;
			m_vInventoryItemInfoList.push_back( kInfo );
		}
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	if( !IsRightSmallTab( dwTabID, 0, 0 ) ) 
		return;

	std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort4() );
	UpdatePage( m_iCurPage );
}

void MyInventoryWnd::AddItemInfoVecFromEtcItem( DWORD dwTabID, DWORD dwSmallTabID, int iShopTabType )
{
	if( !IsRightSmallTab( dwTabID, dwSmallTabID, ETC_ITEM_OTHER_VALUE ) ) 
		return;

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return;
	}

	int iMax = g_EtcItemMgr.GetEtcItemCount();
	for (int i = 0; i < iMax ; i++)
	{
		ioEtcItem *pEtcItem = g_EtcItemMgr.GetEtcItemByIdx( i );
		if( !pEtcItem ) continue;

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ))
		{
			bool bAdd = false;
			int iSize = pEtcItem->GetValueSize();
			for (int j = 0; j < iSize ; j++)
			{
				if( pEtcItem->GetShopTabType( j ) != iShopTabType )
					continue;

				bAdd = true;
			}

			if( bAdd )
			{
				InventoryItemInfo kInfo;
				kInfo.m_iMainType = INVENTORY_ITEM_ETC;
				kInfo.m_iMagicCode = pEtcItem->GetType();
				kInfo.m_iManualIndex = pEtcItem->GetInventoryManual()-1;
				kInfo.m_iPriorityOrder = pEtcItem->GetInventoryOrder();
				kInfo.m_szItemName = pEtcItem->GetName(); // 2019-06-18
				

				//// 2019-05-30
				//if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
				//{
				//	if( false == m_pNewSearchWnd->IsSearch( pEtcItem->GetName(), m_pNewSearchWnd->GetFindKeyWord()) )
				//	{
				//		continue;
				//	}
				//}

				m_vInventoryItemInfoList.push_back( kInfo );
			}
		}
	}
}

void MyInventoryWnd::AddItemInfoVecFromMedalItem( DWORD dwTabID, DWORD dwSmallTabID )
{
	if( !IsRightSmallTab( dwTabID, dwSmallTabID, MEDALITEM_SMALL_TAB_TYPE ) ) 
		return;

	ioUserMedalItem *pUserMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pUserMedalItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserMedalItem == NULL", __FUNCTION__ );
		return;
	}

	int iMax = pUserMedalItem->GetMedalItemCnt();
	for (int i = 0; i < iMax ; i++)
	{
		ioUserMedalItem::MEDALITEMSLOT kSlot;
		pUserMedalItem->GetMedalItemArray( i , kSlot );
		if( kSlot.m_iItemType == 0 )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType      = INVENTORY_ITEM_MEDALITEM;
		kInfo.m_iMagicCode     = kSlot.m_iItemType;
		kInfo.m_iMagicCodeEx = kSlot.m_iCustomIndex;
		kInfo.m_iPriorityOrder = kSlot.m_iItemType;
		if( kSlot.m_iEquipClass != 0 )
		{
			continue;
		}
		else
			kInfo.m_bEquip     = false;

		// 2019-06-18
		const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo(kSlot.m_iItemType);
		if( NULL != pItem )
		{
			kInfo.m_szItemName = pItem->m_sName;
		}

		m_vInventoryItemInfoList.push_back( kInfo );
	}
}

bool MyInventoryWnd::IsBoughtDeco( int iClassType, int iSexType, int iDecoType, int iDecoCode )
{
	int iCharArray = g_MyInfo.GetClassArray( iClassType );

	// 체험,미고용용병이면 남자종족은 소유하지 않은 것
	if( g_MyInfo.IsCharExercise( iCharArray ) || !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) ) 
	{
		if( iDecoType == UID_KINDRED && iDecoCode == RDT_HUMAN_MAN )
			return false;
	}

	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )

	int iType = ( iClassType * 100000) + ( iSexType * 1000);
	int iCode = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			iType += iDecoType;
			iCode =  iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			iType += iEquipType;
			iCode  = iEquipCode;
		}
		break;			
	}
	return g_MyInfo.IsSlotItem( iType, iCode );
}

bool MyInventoryWnd::IsEquipDeco( const CHARACTER &rkCharInfo, int iDecoType, int iDecoCode )
{
	switch( iDecoType )
	{
	case UID_FACE:
		{
			if( rkCharInfo.m_face == iDecoCode )
				return true;
		}
		break;
	case UID_HAIR:
		{
			if( rkCharInfo.m_hair == iDecoCode )
				return true;
		}
		break;
	case UID_SKIN_COLOR:
		{
			if( rkCharInfo.m_skin_color == iDecoCode )
				return true;
		}
		break;
	case UID_HAIR_COLOR:
		{
			if( rkCharInfo.m_hair_color == iDecoCode )
				return true;
		}
		break;
	case UID_KINDRED:
		{
			if( (int)GetRaceDetailType( rkCharInfo ) == iDecoCode )
				return true;
		}
		break;
	case UID_UNDERWEAR:
		{
			if( rkCharInfo.m_underwear == iDecoCode )
				return true;
		}
		break;
	}

	return false;
}

void MyInventoryWnd::SelectItemBtn( DWORD dwID, DWORD dwParam )
{
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	m_iSelectArray = iStartArray + (dwID-ID_ITEM_INFO_WND_1_BTN);

	for( int i=0; i < MAX_ITEM_INFO_WND_BTN; i++ )
	{
		InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem ) continue;

		int iCurArray = iStartArray + i;

		if( m_iSelectArray == iCurArray )
		{
			pItem->SetSelect( true );
		}
		else
		{
			pItem->SetSelect( false );
		}
	}
}

void MyInventoryWnd::SelectCharItemBtn( DWORD dwID, DWORD dwParam )
{
	int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;
	m_iSelectArray = iStartArray + (dwID-ID_CHAR_INFO_WND_1_BTN);

	for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
	{
		InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
		if( !pItem ) continue;

		// 2019-05-27
		pItem->SetSelectCharIndex(-1);
		if( true == IsSearchInputEnter() )
		{
			pItem->SetSelectCharIndex(i);
		}

		int iCurArray = iStartArray + i;
		if( m_iSelectArray == iCurArray )
			pItem->SetSelect( true );
		else
			pItem->SetSelect( false );
	}

	InventorySoldierItemWnd *pItemBtn = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(dwID));
	if( pItemBtn )
	{
		m_iSelectClassType = pItemBtn->GetClassType();

		MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
		if( pInfoWnd )
		{
			ShowMainInfoWnd();
			pInfoWnd->SetSlodierInfo( m_iSelectClassType );
		}
	}
}

void MyInventoryWnd::UpdateTab( bool bPage, bool bInfo, bool bSelectChange )
{
	CheckRadioButton( ID_SMALL_TAB_1_BTN, m_dwSmallTabEndID, m_dwCurSmallTabID );
	
	// UI코스튬 관련 (탭 업데이트)
	m_bCharItem = false;
	if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )
		SetSoldierItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_EXTRA_TAB_BTN )
		SetExtraItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_DECO_TAB_BTN )
		SetDecoItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_ETC_TAB_BTN )
		SetEtcItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_PRESENT_TAB_BTN )
		SetPresentItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
		SetAlchemicItemInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_COSTUME_TAB_BTN )
		SetCostumeInfoVec( m_dwCurTabID, m_dwCurSmallTabID );
	else if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN )
		SetAccessoryInfoVec( m_dwCurTabID, m_dwCurSmallTabID );

	// 페이지 갱신 여부
	if( bPage )
	{
		m_iCurPage = 0;
	}

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	bool bSpirit = false;
	if( pScroll )
	{
		if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
			bSpirit = true;

		int iMinPos = pScroll->GetMinPos();
		int iMaxPos = pScroll->GetMaxPos();
		m_iCurPage = max( iMinPos, min( m_iCurPage, iMaxPos ) );

		int iCharItemSize = m_vCharInfoList.size();
		int iItemSize = m_vInventoryItemInfoList.size();
		int iSpiritSize = m_vAlchemicItemInfoList.size();
		if( m_bCharItem )
		{
			int iMaxPos =  iCharItemSize / PAGE_PER_CHAR_NUM;
			if( iCharItemSize % PAGE_PER_CHAR_NUM )
				iMaxPos++;

			pScroll->SetPageRange(1);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}
		else if( bSpirit )
		{
			int iMaxPos =  iSpiritSize / PAGE_PER_SPIRIT_NUM;
			if( iSpiritSize % PAGE_PER_SPIRIT_NUM )
				iMaxPos++;

			pScroll->SetPageRange(4);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}
		else
		{
			int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
			if( iItemSize % PAGE_PER_ITEM_NUM )
				iMaxPos++;

			pScroll->SetPageRange(2);
			pScroll->SetScrollRange( 0, iMaxPos );
			pScroll->SetScrollPos( m_iCurPage );
		}
	}

	UpdatePage( m_iCurPage );

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return;

	if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )
	{
		if( bInfo )
		{
			ShowMainInfoWnd();
			ShowAlchemicFuncWnd();
		}
	}
	else if( m_dwCurTabID == ID_ACCESSORY_TAB_BTN )
	{
		if( bInfo )
		{
			ShowMainInfoWnd();
			ShowAccessoryFuncWnd();

			AccessoryFuncWnd *pAccessoryInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
			if( pAccessoryInfoWnd )
				pAccessoryInfoWnd->SetAccessoryInfo( m_iSelectClassType );
		}	
	}
	else if( m_dwCurTabID == ID_EXTRA_TAB_BTN )
	{
		if( bInfo )
		{
			ShowMainInfoWnd();
			ShowExtraitemFuncWnd();

			ExtraItemFuncWnd *pExtraInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
			if( pExtraInfoWnd )
				pExtraInfoWnd->SetExtraItemInfo( m_iSelectClassType );
		}
	}
	else if( m_bCharItem )
	{
		int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;
		if( bPage )
		{
			m_iSelectArray = iStartArray;
		}

		if( !COMPARE( m_iSelectArray, 0, (int)m_vCharInfoList.size() ) )
		{
			m_iSelectArray = iStartArray;
		}

		for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
		{
			InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
			if( !pItem ) continue;

			pItem->SetSelect( false );
		}

		if( bInfo )
		{
			ShowMainInfoWnd();
			pInfoWnd->SetSlodierInfo( m_iSelectClassType );
		}
	}
	else
	{
		// UI코스튬 관련 (아이템 선택)
		switch( m_dwCurTabID )
		{
		case ID_DECO_TAB_BTN:
		case ID_EXTRA_TAB_BTN:
		case ID_ETC_TAB_BTN:
		case ID_PRESENT_TAB_BTN:
		case ID_COSTUME_TAB_BTN:
		case ID_ACCESSORY_TAB_BTN:
			{
				int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
				DWORD dwSelectID = ID_ITEM_INFO_WND_1_BTN;

				if( !bSelectChange )
				{
					if( bPage )
					{
						m_iSelectArray = iStartArray;
					}

					if( !COMPARE( m_iSelectArray, 0, (int)m_vInventoryItemInfoList.size() ) )
					{
						m_iSelectArray = iStartArray;
					}

					for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
					{
						InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
						if( !pItem )
							continue;

						int iCurArray = iStartArray + i;

						if( m_iSelectArray == iCurArray )
						{
							pItem->SetSelect( true );
							dwSelectID = ID_ITEM_INFO_WND_1_BTN + i;
						}
						else
						{
							pItem->SetSelect( false );
						}
					}
				}
				else
				{
					m_iSelectArray = -1;

					for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
					{
						InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
						if( !pItem )
							continue;

						int iCurArray = iStartArray + i;
						pItem->SetSelect( false );
					}
				}

				if( bInfo )
				{
					ShowMainInfoWnd();
					if( m_iSelectArray != -1 && !m_vInventoryItemInfoList.empty() )
					{
						InventoryItemWnd *pSelectItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwSelectID));
						if( pSelectItem )
						{
							pInfoWnd->SetInvenItemInfo( m_iSelectClassType, pSelectItem->GetInfo() );							
						}
					}
					else
					{
						pInfoWnd->SetItemBlank( m_dwCurTabID, m_dwCurSmallTabID, m_iSelectClassType );
					}
				}
			}
			break;
		}
	}
}

void MyInventoryWnd::UpdateData()
{
	ChangeMainTab( m_dwCurTabID );
	SetFirstBtnInfo();
}

void MyInventoryWnd::ChangeClassType( int iClassType )
{
	if( !IsShow() )
		return;

	m_iSelectClassType = iClassType;

	switch( m_dwCurTabID )
	{
	case ID_SOLDIER_TAB_BTN:
		ShowSoldierTabDirect( iClassType );
		break;
	case ID_DECO_TAB_BTN:
	case ID_EXTRA_TAB_BTN:
		// UI코스튬 관련 (캐릭터 변경시)
	case ID_COSTUME_TAB_BTN:
	case ID_ACCESSORY_TAB_BTN:
		UpdateTab( true, true );
		break;
	}
}

int MyInventoryWnd::FindSelectCharArray( int iSelectClassType )
{
	// 해당 용병이 있는 페이지로 갱신
	int iSize = m_vCharInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vCharInfoList[i].m_iClassType == iSelectClassType )
		{
			return i;
		}
	}

	return -1;
}

void MyInventoryWnd::ShowSoldierTabDirect()
{
	if( !IsShow() )
		ShowWnd();

	DWORD dwSmallTab = GetDefaultSmallTab( ID_SOLDIER_TAB_BTN );
	ChangeMainTab( ID_SOLDIER_TAB_BTN );
	ChangeSmallTab( dwSmallTab );
	SetFirstBtnInfo();

	if( m_vInventoryItemInfoList.empty() )
	{
		ChangeMainTab( ID_SOLDIER_TAB_BTN );

		m_iSelectArray = FindSelectCharArray( m_iSelectClassType );

		int iStartArray = m_iCurPage * PAGE_PER_CHAR_NUM;
		for( int i = 0 ; i < MAX_CHAR_INFO_WND_BTN ; i++ )
		{
			InventorySoldierItemWnd *pItem = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd(ID_CHAR_INFO_WND_1_BTN + i));
			if( !pItem ) continue;

			// 2019-05-27
			pItem->SetSelectCharIndex(-1);
			if( true == IsSearchInputEnter() )
			{
				pItem->SetSelectCharIndex(i);
			}

			int iCurArray = iStartArray + i;
			if( m_iSelectArray == iCurArray )
				pItem->SetSelect( true );
			else
				pItem->SetSelect( false );
		}

		MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
		if( pInfoWnd )
		{
			ShowMainInfoWnd();
			pInfoWnd->SetSlodierInfo( m_iSelectClassType );
		}
	}
}

void MyInventoryWnd::ShowSoldierTabDirect( int iClassType )
{
	if( !IsShow() )
		ShowWnd();

	m_iSelectClassType = iClassType;

	ChangeMainTab( ID_SOLDIER_TAB_BTN );

	// 오른쪽 창쪽에 클레스 타입 전달 필요.
	if( m_iSelectClassType == -1 )
	{
		SetFirstBtnInfo();
		return;
	}

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd )
	{
		ShowMainInfoWnd();
		pInfoWnd->SetSlodierInfo( m_iSelectClassType );
	}

	// 해당 용병이 있는 페이지로 갱신
	int iSize = m_vCharInfoList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vCharInfoList[i].m_iClassType == m_iSelectClassType )
		{
			m_iSelectArray = i;

			int iNewPage = i / PAGE_PER_CHAR_NUM;

			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollPos( iNewPage );
			}
		}
	}
}

void MyInventoryWnd::ShowExtraItemTabDirect( int iClassType, int iIndex )
{
	if( !IsShow() )
		ShowWnd();

	m_iSelectClassType = iClassType;

	ChangeMainTab( ID_EXTRA_TAB_BTN );
	ShowMainInfoWnd();
	ShowExtraitemFuncWnd();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	EXTRAITEMSLOT kSlot;
	if( !pExtraItem || !pExtraItem->GetExtraItem( iIndex, kSlot ) )
		return;

	int iArrayCnt = 0;
	for( iArrayCnt; iArrayCnt < (int)m_vInventoryItemInfoList.size(); ++iArrayCnt )
	{
		if( m_vInventoryItemInfoList[iArrayCnt].m_iMainType == INVENTORY_ITEM_EXTRAITEM &&
			m_vInventoryItemInfoList[iArrayCnt].m_iMagicCode == iIndex )
		{
			int iNewPage = iArrayCnt / PAGE_PER_ITEM_NUM;

			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollPos( iNewPage );
			}

			break;
		}
	}
}

void MyInventoryWnd::ShowDecoTabDirect( int iClassType )
{
	if( !IsShow() )
		ShowWnd();

	m_iSelectClassType = iClassType;

	ChangeMainTab( ID_DECO_TAB_BTN );
	SetFirstBtnInfo();
}

void MyInventoryWnd::ShowEtcTabDirect( DWORD dwItemCode )
{
	if( !IsShow() )
		ShowWnd();

	if( dwItemCode == 0 )
	{
		ChangeMainTab( ID_ETC_TAB_BTN );
		SetFirstBtnInfo();
		return;
	}

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwItemCode );
	if( pEtcItem )
	{
		int iShopTabType = SHOP_TAB_TYPE_NONE;
		int iSize = pEtcItem->GetValueSize();
		for (int j = 0; j < iSize ; j++)
		{
			iShopTabType = pEtcItem->GetShopTabType( j );
			if( iShopTabType != SHOP_TAB_TYPE_NONE )
				break;
		}

		DWORD dwSmallTab = 0;
		switch( iShopTabType )
		{
		case SHOP_TAB_TYPE_SOLDIER:
			{
				dwSmallTab = GetDefaultSmallTab( ID_SOLDIER_TAB_BTN );
				ChangeMainTab( ID_SOLDIER_TAB_BTN );
				ChangeSmallTab( dwSmallTab );
				SetFirstBtnInfo();
			}
			break;
		case SHOP_TAB_TYPE_EXTRA:
			{
				dwSmallTab = GetDefaultSmallTab( ID_EXTRA_TAB_BTN );
				ChangeMainTab( ID_EXTRA_TAB_BTN );
				ChangeSmallTab( dwSmallTab );
				SetFirstBtnInfo();
			}
			break;
		case SHOP_TAB_TYPE_DECO:
			{
				dwSmallTab = GetDefaultSmallTab( ID_DECO_TAB_BTN );
				ChangeMainTab( ID_DECO_TAB_BTN );
				ChangeSmallTab( dwSmallTab );
				SetFirstBtnInfo();
			}
			break;
		case SHOP_TAB_TYPE_COSTUME:
			{
				// UI코스튬 관련 (특별아이템 재구매시)
				dwSmallTab = GetDefaultSmallTab( ID_COSTUME_TAB_BTN );
				ChangeMainTab( ID_COSTUME_TAB_BTN );
				ChangeSmallTab( dwSmallTab );
				SetFirstBtnInfo();
			}
			break;
		default:
			{
				ChangeMainTab( ID_ETC_TAB_BTN );
				SetFirstBtnInfo();
			}
			break;
		}
	}
	else
	{
		ChangeMainTab( ID_ETC_TAB_BTN );
		SetFirstBtnInfo();
	}
}

bool MyInventoryWnd::IsShowPresentTab()
{
	if( !IsShow() )
		return false;

	return (m_dwCurTabID == ID_PRESENT_TAB_BTN);
}

void MyInventoryWnd::ShowPresentTabDirect()
{
	if( !IsShow() )
		ShowWnd();

	ChangeMainTab( ID_PRESENT_TAB_BTN );
	SetFirstBtnInfo();
}

void MyInventoryWnd::ShowSubscriptionTabDirect()
{
	if( !IsShow() )
		ShowWnd();

	ChangeMainTab( ID_PRESENT_TAB_BTN );

	int iSize = g_SubscriptionMgr.GetMaxSubscription();
	if( iSize > 0 )
	{
		ChangeSmallTab( ID_SMALL_TAB_2_BTN );
	}

	SetFirstBtnInfo();
}

void MyInventoryWnd::ShowAlchemicTabDirect()
{
	if( !IsShow() )
		ShowWnd();

	ChangeMainTab( ID_ALCHEMIC_TAB_BTN );
	
	ShowMainInfoWnd();
	ShowAlchemicFuncWnd();
}

// UI코스튬 관련 (코스튬 탭으로 바로 이동)
void MyInventoryWnd::ShowCostumeTabDirect( int iClassType, int iIndex )
{
	if( !IsShow() )
		ShowWnd();

	m_iSelectClassType = iClassType;

	ChangeMainTab( ID_COSTUME_TAB_BTN );
	SetFirstBtnInfo();
	
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	CostumeSlot kSlot;
	if( !pCostume || !pCostume->GetCostumeSlot( iIndex, kSlot ) )
		return;

	int iArrayCnt = 0;
	for( iArrayCnt; iArrayCnt < (int)m_vInventoryItemInfoList.size(); ++iArrayCnt )
	{
		if( m_vInventoryItemInfoList[iArrayCnt].m_iMainType == INVENTORY_ITEM_COSTUME &&
			m_vInventoryItemInfoList[iArrayCnt].m_iMagicCode == iIndex )
		{
			int iNewPage = iArrayCnt / PAGE_PER_ITEM_NUM;

			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollPos( iNewPage );
			}

			break;
		}
	}
}

void MyInventoryWnd::ShowAccessoryTabDirect( int iClassType, int iIndex /*= -1 */ )
{
	if( !IsShow() )
		ShowWnd();

	m_iSelectClassType = iClassType;

	ChangeMainTab( ID_ACCESSORY_TAB_BTN );
	ShowMainInfoWnd();
	ShowAccessoryFuncWnd();

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	AccessorySlot kSlot;
	if( !pAccessory || !pAccessory->GetAccessorySlot( iIndex, kSlot ) )
		return;

	int iArrayCnt = 0;
	for( iArrayCnt; iArrayCnt < (int)m_vInventoryItemInfoList.size(); ++iArrayCnt )
	{
		if( m_vInventoryItemInfoList[iArrayCnt].m_iMainType == INVENTORY_ITEM_ACCESSORY &&
			m_vInventoryItemInfoList[iArrayCnt].m_iMagicCode == iIndex )
		{
			int iNewPage = iArrayCnt / PAGE_PER_ITEM_NUM;

			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollPos( iNewPage );
			}

			break;
		}
	}
}

void MyInventoryWnd::RenderWnd()
{
	if( OnRenderShowAndHide() ) return;
	if( !IsShow() )	return;

	OnRender();

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWndList::iterator iter = m_ChildList.begin();
		for( ; iter!=m_ChildList.end() ; ++iter )
		{
			ioWnd *pWnd = *iter;
			if( pWnd )
			{
				// HARDCODE : ID_CHARGE_GOLD_BTN, ID_SHOP_BTN, ID_EXIT_BTN들은 RenderAfterChild 시점에 한다
				DWORD dwID = pWnd->GetID();
				switch( dwID )
				{
				case ID_CHARGE_GOLD_BTN:
				case ID_SHOP_BTN:
				case ID_EXIT_BTN:
					continue;
				}

				pWnd->RenderWnd();
			}
		}

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}

	OnRenderAfterChild();
}

void MyInventoryWnd::OnRender()
{
	// Title은 나중에 출력.
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pSelectedFrm )
		m_pSelectedFrm->Render( iXPos, iYPos );
	if( m_pMultiplyFrm )
		m_pMultiplyFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	RenderImageList( iXPos, iYPos, UI_RENDER_NORMAL );

	// tab space dot
	if( !m_pTabDot )
		return;

	enum { WIDTH = 1, HEIGHT = 2, TOTAL_WIDTH = 444, };

	m_pTabDot->SetSize( WIDTH, HEIGHT );
	m_pTabDot->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );

	int iSpaceSize = m_dwSmallTabEndID-ID_SMALL_TAB_1_BTN;

	if( m_dwCurTabID == ID_PRESENT_TAB_BTN )
	{
		enum { X_TAB_DOT = 121, Y_TAB_DOT = 112, TAB_DOT_X_GET = 95, };

		for (int i = 0; i < iSpaceSize ; i++)
		{
			m_pTabDot->Render( iXPos + X_TAB_DOT + (TAB_DOT_X_GET * i), iYPos + Y_TAB_DOT );
		}

		// 하단탭 줄
		int iCurWidth = TOTAL_WIDTH - ( TAB_DOT_X_GET*(iSpaceSize+1) ); // 탭표시 공간 만큼 축소 , +1는 첫번째탭 크기를 더한다.
		if( iCurWidth > 0 )
		{
			m_pTabDot->SetSize( iCurWidth, HEIGHT );
			m_pTabDot->Render( iXPos + X_TAB_DOT + (TAB_DOT_X_GET * iSpaceSize), iYPos + Y_TAB_DOT );
		}
	}
	else
	{
		enum { X_OFFSET = 517, Y_OFFSET = 14, X_TAB_DOT = 82, Y_TAB_DOT = 112, TAB_DOT_X_GET = 56, };

		for (int i = 0; i < iSpaceSize ; i++)
		{
			m_pTabDot->Render( iXPos + X_TAB_DOT + (TAB_DOT_X_GET * i), iYPos + Y_TAB_DOT );
		}

		// 하단탭 줄
		int iCurWidth = TOTAL_WIDTH - ( TAB_DOT_X_GET*(iSpaceSize+1) ); // 탭표시 공간 만큼 축소 , +1는 첫번째탭 크기를 더한다.
		if( iCurWidth > 0 )
		{
			m_pTabDot->SetSize( iCurWidth, HEIGHT );
			m_pTabDot->Render( iXPos + X_TAB_DOT + (TAB_DOT_X_GET * iSpaceSize), iYPos + Y_TAB_DOT );
		}
	}
}

void MyInventoryWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// Title 출력
	if( m_pSelectedTitleFrm)
		m_pSelectedTitleFrm->Render( iXPos, iYPos );

	if( m_pUITitle )
		m_pUITitle->PrintNormal( iXPos, iYPos );

	// Title 상에 존재하는 Child 출력
	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWnd *pWnd = NULL;
		pWnd = FindChildWnd( ID_CHARGE_GOLD_BTN );
		if( pWnd )
			pWnd->RenderWnd();


		pWnd = FindChildWnd( ID_SHOP_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		pWnd = FindChildWnd( ID_EXIT_BTN );
		if( pWnd )
			pWnd->RenderWnd();

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}

	// Title 상에 존재하는 텍스트 출력
	enum { X_OFFSET = 594, X_OFFSET2	= 578, Y_OFFSET = 15, };

	char szPeso[MAX_PATH]="";
	char szCash[MAX_PATH]="";
	char szStamp[MAX_PATH]="";
	char szClover[MAX_PATH]="";
	char szMileage[MAX_PATH]="";
	int  iMaxStamp = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_SILVER_COIN );
	int  iMaxClover = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_CLOVER );
	int  iMaxMileage = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_MILEAGE_COIN );
	Help::ConvertNumToStrComma( g_MyInfo.GetMoney(), szPeso, sizeof( szPeso ) );
	Help::ConvertNumToStrComma( g_MyInfo.GetTotalCash(), szCash, sizeof( szCash ) );
	Help::ConvertNumToStrComma( iMaxStamp, szStamp, sizeof( szStamp ) );
	Help::ConvertNumToStrComma( iMaxClover, szClover, sizeof( szClover ) );
	
	int iAdd_OffsetY = 0;
	if( iMaxMileage > 0 )
	{
		Help::ConvertNumToStrComma( iMaxMileage, szMileage, sizeof( szMileage ) );
		iAdd_OffsetY = 7;
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 12, 66, 111 );
	if( iMaxStamp > 0 )
	{
		kPrinter.SetTextColor( 130, 198, 255 );	
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_SILVER_COIN );
		if( pEtcItem )
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(4), pEtcItem->GetName().c_str(), szStamp );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(3), szStamp );
		}
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	}
	if( iMaxClover > 0 )
	{
		kPrinter.SetTextColor( 130, 198, 255 );	
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CLOVER );
		if( pEtcItem )
			kPrinter.AddTextPiece( FONT_SIZE_11, "%s : %s개", pEtcItem->GetName().c_str(), szClover );
		else
			kPrinter.AddTextPiece( FONT_SIZE_11, "클로버 : %s개", szClover );
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );		
		kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	}


	kPrinter.SetTextColor( 130, 198, 255 );
	kPrinter.AddTextPiece( FONT_SIZE_11, STR(1) );
	kPrinter.AddTextPiece( FONT_SIZE_11, szPeso );

	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_11, " / " );
	kPrinter.SetTextColor( 255, 188, 26 );
	kPrinter.AddTextPiece( FONT_SIZE_11, STR(2) );
	kPrinter.AddTextPiece( FONT_SIZE_11, szCash );
	
	ioButton* pBonusCashBtn = dynamic_cast<ioButton*>(FindChildWnd( ID_BONUS_CASH_BTN ));
	if ( pBonusCashBtn && pBonusCashBtn->IsShow() )
	{
		//마일리지가 존재한다면
		if ( iMaxMileage <= 0 )
		{
			pBonusCashBtn->SetWndPos( pBonusCashBtn->GetXPos() - 2 , pBonusCashBtn->GetYPos() + 6 );
			pBonusCashBtn->RenderWnd();
			pBonusCashBtn->SetWndPos( pBonusCashBtn->GetXPos() + 2 , pBonusCashBtn->GetYPos() - 6 );
		}
		else
			pBonusCashBtn->RenderWnd();
		kPrinter.PrintFullText( iXPos + X_OFFSET2, iYPos + Y_OFFSET-iAdd_OffsetY, TAT_RIGHT );
	}
	else
		kPrinter.PrintFullText( iXPos + X_OFFSET, iYPos + Y_OFFSET-iAdd_OffsetY, TAT_RIGHT );
	kPrinter.ClearList();

	if( iMaxMileage > 0 )
	{
		iAdd_OffsetY = 6;
		kPrinter.SetTextColor( 115, 204, 0 );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_MILEAGE_COIN );
		if( pEtcItem )
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(4), pEtcItem->GetName().c_str(), szMileage );
		}
		else
		{
			kPrinter.AddTextPiece( FONT_SIZE_11, STR(4), szMileage );
		}
		kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );	
		kPrinter.PrintFullText( iXPos + X_OFFSET, iYPos + Y_OFFSET+iAdd_OffsetY, TAT_RIGHT );
		kPrinter.ClearList();	
	}

	enum
	{
		X_BONUS_CASH_TOOLTIP_OFFSET	= 63,
		Y_BONUS_CASH_TOOLTIP_OFFSET	= 22,
	};
	ioWnd* pBonusCashToolTip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
	if ( pBonusCashToolTip && pBonusCashToolTip->IsShow() )
	{
		pBonusCashToolTip->SetWndPos( X_OFFSET2 - X_BONUS_CASH_TOOLTIP_OFFSET, Y_OFFSET-iAdd_OffsetY - Y_BONUS_CASH_TOOLTIP_OFFSET );
		pBonusCashToolTip->RenderWnd();
	}

	ioWnd *pWnd = NULL;
	pWnd = FindChildWnd( ID_MULTI_SUPER_GASHAPON_TOAST_POPUP_WND );
	if( pWnd && pWnd->IsShow() )
		pWnd->RenderWnd();
}
 
void MyInventoryWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );

	// 2019-06-10
	OnKeyProcess();
	
	ProcessCustomBtnTooltip();

	//본부에서는 g_MyInfo.GetRoomOutSec() 내부에서 시간 확인 한후 다른 룸으로 내보내지않음
	if( g_MyInfo.GetRoomOutSec() != -1 && ioPlayMode::GetModeType() != MT_HEADQUARTERS && ioPlayMode::GetModeType() != MT_HOUSE ) 
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_MyInfo.GetRoomOutSec() );
		SetTitleText( szTitle );
		m_bRoomOutTimeCheck = true;
	}
	else if( m_bRoomOutTimeCheck )
	{
		HideWnd();
	}

	UpdateHouseButton();
	CheckBonusCash();

	if( m_dwCurTabID == ID_PRESENT_TAB_BTN )
	{
		if( g_PresentMgr.IsEnableSendPresentDataToTime() )
			SetChildActive( ID_REFRESH_BTN );
		else
			SetChildInActive( ID_REFRESH_BTN );
	}
}

void MyInventoryWnd::UpdateHouseButton()
{
	SetChildWndStyleAdd( ID_HOUSE_ITEM_BTN, IWS_EXACTIVE );
	if( g_MyInfo.IsHouseOpen() )
	{
		SetChildWndStyleRemove( ID_HOUSE_ITEM_BTN, IWS_EXACTIVE );
	}
}

bool MyInventoryWnd::ProcessCustomBtnTooltipExtraItem()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(int i = 0;i < MAX_ITEM_INFO_WND_BTN;i++)
	{
		InventoryItemWnd *pItemWnd = dynamic_cast<InventoryItemWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( pItemWnd->GetMainType() != INVENTORY_ITEM_EXTRAITEM ) continue;
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
		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1) );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(2) );
		}		
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(3) );
		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(4) );
		}
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(5) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(6) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(7) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(8) );
	}

	ItemCustomThumbnailTooltip *pThumbnailTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( MY_INVENTORY_CUSTOM_TOOLTIP ));
	if( pThumbnailTooltip )
	{
		pThumbnailTooltip->SetTooltipText( kPrinter, kSlot );
		return pThumbnailTooltip->IsShow();
	}
	return false;
}

bool MyInventoryWnd::ProcessCustomBtnTooltipPresent()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(int i = 0;i < MAX_ITEM_INFO_WND_BTN;i++)
	{
		InventoryItemWnd *pItemWnd = dynamic_cast<InventoryItemWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( pItemWnd->GetMainType() != INVENTORY_ITEM_PRESENT ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		ioHashString szSendID;
		short sPresentType = 0;
		short sPresentMent = 0;
		int iPresentValue1 = 0;
		int iPresentValue2 = 0;
		int iPresentValue3 = 0;
		int iPresentValue4 = 0;
		DWORD dwLimitDate  = 0;
		g_PresentMgr.GetPresentDataToIndex( pItemWnd->GetMagicCode(), pItemWnd->GetMagicCodeEx(), szSendID,	sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );
        
		if( sPresentType == PRESENT_EXTRAITEM )
		{
			kSlot.m_iItemCode = iPresentValue1;
			kSlot.m_iReinforce= ( iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2;
			kSlot.m_dwMaleCustom = iPresentValue3;
			kSlot.m_dwFemaleCustom = iPresentValue4;
			break;
		}
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

	ItemCustomThumbnailTooltip *pTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( PRESENT_ITEM_THUMBNAIL_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, kSlot );
		return pTooltip->IsShow();
	}
	return false;
}

// UI코스튬 관련 (코스튬 스킨 적용 미리보기 설정 - 코스튬)
bool MyInventoryWnd::ProcessCustomBtnTooltipCostume()
{
	CostumeSlot kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for( int i=0; i<MAX_ITEM_INFO_WND_BTN; i++ )
	{
		InventoryItemWnd *pItemWnd = dynamic_cast<InventoryItemWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( pItemWnd->GetMainType() != INVENTORY_ITEM_COSTUME ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
		if( !pCostume ) continue;

		int iMagicCode = pItemWnd->GetMagicCode();
		if( !pCostume->GetCostumeSlot( iMagicCode, kSlot ) ) continue;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
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
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "장비 스킨이 적용되어있습니다" );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "＊아이콘을 클릭해서 " );
		kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "삭제" );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, "할 수 있습니다" );
	}

	CostumeCustomThumbnailTooltip *pThumbnailTooltip = dynamic_cast<CostumeCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( MY_INVENTORY_COSTUME_CUSTOM_TOOLTIP ));
	if( pThumbnailTooltip )
	{
		CostumeEquipInfo sCostumeEquipInfo;
		sCostumeEquipInfo.m_CostumeCode = kSlot.m_nCostumeCode;
		sCostumeEquipInfo.m_Costume_Male_Custom = kSlot.m_dwMaleCustom;
		sCostumeEquipInfo.m_Costume_Female_Custom = kSlot.m_dwFemaleCustom;

		pThumbnailTooltip->SetTooltipText( kPrinter, sCostumeEquipInfo );
		return pThumbnailTooltip->IsShow();
	}
	return false;
}

// UI코스튬 관련 (코스튬 스킨 적용 미리보기 설정 - 선물)
bool MyInventoryWnd::ProcessCustomBtnTooltipCostumePresent()
{
	CostumeEquipInfo sCostumeEquipInfo;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for( int i=0; i<MAX_ITEM_INFO_WND_BTN; i++ )
	{
		InventoryItemWnd *pItemWnd = dynamic_cast<InventoryItemWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( pItemWnd->GetMainType() != INVENTORY_ITEM_PRESENT ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		ioHashString szSendID;
		short sPresentType = 0;
		short sPresentMent = 0;
		int iPresentValue1 = 0;
		int iPresentValue2 = 0;
		int iPresentValue3 = 0;
		int iPresentValue4 = 0;
		DWORD dwLimitDate  = 0;
		g_PresentMgr.GetPresentDataToIndex( pItemWnd->GetMagicCode(), pItemWnd->GetMagicCodeEx(), szSendID,	sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );

		if( sPresentType == PRESENT_COSTUME )
		{
			sCostumeEquipInfo.m_CostumeCode = iPresentValue1;
			sCostumeEquipInfo.m_Costume_Male_Custom = iPresentValue3;
			sCostumeEquipInfo.m_Costume_Female_Custom = iPresentValue4;
			break;
		}
	}

	if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 || sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, "장비 스킨 - " );

		if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 && sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "남성" );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "여성 " );
		}		
		else if( sCostumeEquipInfo.m_Costume_Male_Custom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, "남성 " );
		}
		else if( sCostumeEquipInfo.m_Costume_Female_Custom > 0 )
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

	CostumeCustomThumbnailTooltip *pTooltip = dynamic_cast<CostumeCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( PRESENT_COSTUME_THUMBNAIL_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, sCostumeEquipInfo );
		return pTooltip->IsShow();
	}
	return false;
}

void MyInventoryWnd::ProcessCustomBtnTooltip()
{		
	if( !ProcessCustomBtnTooltipExtraItem() )
	{
		ProcessCustomBtnTooltipPresent();
	}
	
	// UI코스튬 관련 (코스튬 스킨 적용 미리보기 설정)
	if( !ProcessCustomBtnTooltipCostume() )
	{
		ProcessCustomBtnTooltipCostumePresent();
	}
}

void MyInventoryWnd::SendRequestGashaponList()
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsSendGashaponList() )
		return;

	if( m_bGashaponListRequest )
		return;

	m_bGashaponListRequest = true;

	NewShopWnd *pShop = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
	if( pShop )
	{
		if( pShop->IsGashaponListRequest() )
			return;
	}

	SP2Packet kPacket( CTPK_GASHAPON_LIST );
	kPacket << ioEtcItem::EIT_ETC_GASHAPON14;
	TCPNetwork::SendToServer( kPacket );
}

int MyInventoryWnd::GetSmallTabType( DWORD dwTabID, DWORD dwSmallTabID )
{
	int iArray = ( dwTabID - ID_SOLDIER_TAB_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	int iSmallArray = ( dwSmallTabID - ID_SMALL_TAB_1_BTN );
	if( !COMPARE( iSmallArray, 0, (int) m_vSmallTabTypeVecArray[iArray].size() ) )
		return -1;

	return m_vSmallTabTypeVecArray[iArray][iSmallArray];
}

bool MyInventoryWnd::IsRightSmallTab( DWORD dwTabID, DWORD dwSmallTabID, int iValue1 )
{
	if( dwTabID == ID_SOLDIER_TAB_BTN )
	{
		if( iValue1 != ETC_ITEM_OTHER_VALUE && dwSmallTabID == ID_SMALL_TAB_1_BTN ) 
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_EXTRA_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_DECO_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_ETC_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_PRESENT_TAB_BTN )
	{
		if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_COSTUME_TAB_BTN )
	{
		// UI코스튬 관련 (스몰 탭 확인)
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}
	else if( dwTabID == ID_ACCESSORY_TAB_BTN )
	{
		if( dwSmallTabID == ID_SMALL_TAB_1_BTN )
			return true;
		else if( iValue1 == GetSmallTabType( dwTabID, dwSmallTabID ) )
			return true;
	}

	return false;
}

void MyInventoryWnd::OnItemIconOver( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();

		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			break;
		case INVENTORY_ITEM_DECO:
			break;
		case INVENTORY_ITEM_COSTUME:
			break;
		}
	}
}

void MyInventoryWnd::OnItemIconPush( DWORD dwID )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return;

	ShowMainInfoWnd();

	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();

		switch( iMainType )
		{
		case INVENTORY_ITEM_ETC:
		case INVENTORY_ITEM_PRESENT:
			pInfoWnd->SetInvenItemInfo( m_iSelectClassType, pItem->GetInfo() );
			break;
		}
	}
}

void MyInventoryWnd::OnItemEquipPush( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();

		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			{
				SetSelectExtraItem( dwID );
			}
			break;
		case INVENTORY_ITEM_MEDALITEM:
			{
				SetEquipMedalItem(dwID);
			}
			break;
		case INVENTORY_ITEM_DECO:
			{
				SetEquipDecoItem( dwID );
			}
			break;
		case INVENTORY_ITEM_COSTUME:
			{
				// UI코스튬 관련 (코스튬 장착)
				SetEquipCostume( dwID );
			}
			break;
		case INVENTORY_ITEM_ACCESSORY:
			{
				SetSelectAccessory( dwID );
			}
			break;
		}
	}
}

void MyInventoryWnd::OnItemReleasePush( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();
		int iMagicCode = pItem->GetMagicCode();
		int iMagicCodeEx = pItem->GetMagicCodeEx();
		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			{
				SetReleaseExtraItem(iMagicCode);
			}
			break;
		case INVENTORY_ITEM_DECO:
			{
				int iDecoType = pItem->GetDecoType();
				int iDecoCode = pItem->GetDecoCode();

				SetReleaseDecoItem( iDecoType, iDecoCode );
			}
			break;
		case INVENTORY_ITEM_MEDALITEM:
			{
				SetReleaseMedalItem( iMagicCode, iMagicCodeEx );
			}
			break;
		case INVENTORY_ITEM_COSTUME:
			{
				// UI코스튬 관련 (코스튬 해제)
				SetReleaseCostume(iMagicCode);
			}
			break;
		}
	}
}

void MyInventoryWnd::OnItemDetailPush( DWORD dwID )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return;

	ShowMainInfoWnd();

	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();

		switch( iMainType )
		{
		case INVENTORY_ITEM_ETC:
		case INVENTORY_ITEM_PRESENT:
		case INVENTORY_ITEM_SUBSCRIPTION:
			{
				pInfoWnd->SetInvenItemInfo( m_iSelectClassType, pItem->GetInfo() );
			}
			break;
		}
	}
}

void MyInventoryWnd::OnItemReSellPush( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();

		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			SetReSellExtraItem( dwID );
			break;
		case INVENTORY_ITEM_MEDALITEM:
			SetReSellMedalItem( dwID );
			break;
		case INVENTORY_ITEM_COSTUME:
			// UI코스튬 관련 (코스튬 판매)
			SetReSellCostume( dwID );
			break;
		case INVENTORY_ITEM_ACCESSORY:
			SetReSellAccessory( dwID );
			break;
		case INVENTORY_ITEM_DECO:
			SetReSellDeco( dwID );
			break;
		}
	}
}

void MyInventoryWnd::OnItemCustomInfoPush( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType  = pItem->GetMainType();
		int iMagicCode = pItem->GetMagicCode();
		int iMagicCodeEx = pItem->GetMagicCodeEx();

		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			{
				// 장비 스킨 삭제 팝업
				ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
				if( !pExtraItem ) return;

				EXTRAITEMSLOT kSlot;
				if( !pExtraItem->GetExtraItem( iMagicCode, kSlot ) )
				{
					if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					}
					else
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 삭제는 광장이나 본부에서만 가능합니다" );
					}
				}
				else
				{
					bool bError = false;
					if( !m_pPlayStage )
						bError = true;

					if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
					{
						if( !bError && m_pPlayStage->GetModeType() != MT_MYROOM )
							bError = true;

						if( !bError && ioMyRoomMode::GetMyRoomType() != MRT_LOBBY )
							bError = true;
					}
					else
					{
						if( !bError && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS&& m_pPlayStage->GetModeType() != MT_HOUSE )
							bError = true;
					}
					
					if( bError )
					{
						if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
						}
						else
						{
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 삭제는 광장이나 본부에서만 가능합니다" );
						}
					}
					else
					{
						ItemCustomDeleteWnd *pCustomDelWnd = dynamic_cast<ItemCustomDeleteWnd*>(g_GUIMgr.FindWnd( ITEM_CUSTOM_DELETE_WND ));
						if( pCustomDelWnd )
							pCustomDelWnd->SetSlotIndex( iMagicCode );
					}
				}
			}
			break;
		case INVENTORY_ITEM_PRESENT:
			{
				ioHashString szSendID;
				short sPresentType = 0;
				short sPresentMent = 0;
				int iPresentValue1 = 0;
				int iPresentValue2 = 0;
				int iPresentValue3 = 0;
				int iPresentValue4 = 0;
				DWORD dwLimitDate  = 0;
				g_PresentMgr.GetPresentDataToIndex( iMagicCode, iMagicCodeEx, szSendID,	sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );

				if( sPresentType == PRESENT_EXTRAITEM )
				{
					ItemCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast< ItemCustomGlobalDetailViewWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
					if( pDetailViewWnd )
					{
						pDetailViewWnd->CreateCharacter( iPresentValue1, iPresentValue3, iPresentValue4 );
					}
				}
				else if( sPresentType == PRESENT_COSTUME )
				{
					CostumeEquipInfo sCostumeEquipInfo;
					sCostumeEquipInfo.m_CostumeCode = iPresentValue1;
					sCostumeEquipInfo.m_Costume_Male_Custom = iPresentValue3;
					sCostumeEquipInfo.m_Costume_Female_Custom = iPresentValue4;

					CostumeCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast<CostumeCustomGlobalDetailViewWnd*>( g_GUIMgr.FindWnd( COSTUME_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
					if( pDetailViewWnd )
						pDetailViewWnd->CreateCharacter( sCostumeEquipInfo );
				}
			}
			break;
		case INVENTORY_ITEM_COSTUME:
			{
				// 장비 스킨 삭제 팝업
				ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
				if( !pCostume ) 
					return;

				CostumeSlot kSlot;
				if( !pCostume->GetCostumeSlot( iMagicCode, kSlot ) )
				{
					if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					else
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 삭제는 광장이나 본부에서만 가능합니다" );
				}
				else
				{
					bool bError = false;
					if( !m_pPlayStage )
						bError = true;

					if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
					{
						if( !bError && m_pPlayStage->GetModeType() != MT_MYROOM )
							bError = true;

						if( !bError && ioMyRoomMode::GetMyRoomType() != MRT_LOBBY )
							bError = true;
					}
					else
					{
						if( !bError && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
							bError = true;
					}

					if( bError )
					{
						if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
							g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
						else
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비스킨 삭제는 광장이나 본부에서만 가능합니다" );
					}
					else
					{
						CostumeCustomDeleteWnd *pCustomDelWnd = dynamic_cast<CostumeCustomDeleteWnd*>(g_GUIMgr.FindWnd( COSTUME_CUSTOM_DELETE_WND ));
						if( pCustomDelWnd )
							pCustomDelWnd->SetSlotIndex( iMagicCode );
					}
				}
			}
			break;
		}
	}
}

void MyInventoryWnd::OnItemDisassemblePush( DWORD dwID )
{
	InventoryItemWnd *pItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( pItem )
	{
		int iMainType = pItem->GetMainType();
		switch( iMainType )
		{
		case INVENTORY_ITEM_EXTRAITEM:
			SetDisassembleExtraItem( dwID );
			break;
		}
	}
}

bool MyInventoryWnd::SetSelectExtraItem( DWORD dwID )
{
	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( !pInfoWnd )
		return false;

	if( pInfoWnd->GetTabState() == ExtraItemFuncWnd::ID_INFO_TAB_BTN )
		return SetEquipExtraItem( dwID );
	else
		return SetSelectExtraItemClickAction( dwID );
}

bool MyInventoryWnd::SetSelectExtraItemClickAction( DWORD dwID )
{
	ShowMainInfoWnd();
	ShowExtraitemFuncWnd();

	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem )
		return false;

	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( pInfoWnd )
	{
		pInfoWnd->SelectClickAction( pInvenItem->GetMagicCode() );
		if( !pInfoWnd->IsShow() )
			pInfoWnd->ShowWnd();
	}
	return true;
}

bool MyInventoryWnd::SetEquipExtraItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	ShowMainInfoWnd();
	ShowExtraitemFuncWnd();

	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( !pInfoWnd )
		return false;

	if( m_iSelectClassType == -1 )
		m_iSelectClassType = g_MyInfo.GetClassType();

	if(m_pPlayStage->GetModeType() == MT_PRACTICE)
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "수련장에서는 장비를 교체할 수 없습니다." );
		return false;
	}

	pInfoWnd->SetExtraItemInfo( m_iSelectClassType );

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();
	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Wrong Btn : %d", dwID );
		return false;
	}

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
		return false;
	}	

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Don't Have UserExtraItem" );
		return false;
	}

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Not Exist Item : %d", iNewSlotIndex );
		return false;
	}

	if( kSlot.m_bCharEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Equiped Item : %d", iNewSlotIndex );
		return false;
	}

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipExtraItem Not Exist Item : %d", kSlot.m_iItemCode );
		return false;
	}

	// 클레스 체크
	bool bDisableClass = false;
	int iEnableClass = pItem->GetEnableClass();
	if( iEnableClass > 0 && rkCharInfo.m_class_type != iEnableClass )
	{
		bDisableClass = true;
	}

	// 성별 체크
	bool bDisableSex = false;
	int iOnlySetType = pItem->GetCharOnlySexType();
	if( iOnlySetType != ioSetItemInfo::CST_ALL && rkCharInfo.m_sex != iOnlySetType )
	{
		bDisableSex = true;
	}

	if( bDisableClass || bDisableSex )
	{
		if( iEnableClass > 0 )
		{
			ioHashString szClassName = g_MyInfo.GetClassName( iEnableClass );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(13) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );

			switch( iOnlySetType )
			{
			case ioSetItemInfo::CST_ALL:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(14), szClassName.c_str() );
				break;
			case ioSetItemInfo::CST_MALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(15), szClassName.c_str() );
				break;
			case ioSetItemInfo::CST_FEMALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(16), szClassName.c_str() );
				break;
			}

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(17) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );

			switch( iOnlySetType )
			{
			case ioSetItemInfo::CST_ALL:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(18) );
				break;
			case ioSetItemInfo::CST_MALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(19) );
				break;
			case ioSetItemInfo::CST_FEMALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(20) );
				break;
			}

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}

		return false;
	}
	
	// 가능 레벨 체크
	int iEnableLevel = kSlot.m_iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iCurLevel = g_MyInfo.GetClassLevel(rkCharInfo.m_class_type,true);
	if( iCurLevel < iEnableLevel )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(9) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(10), iEnableLevel );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return false;
	}
	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	//강화 등급별 장착여부 체크
	if( iEnableClass > 0 )
	{
		ioHashString szClassName;
		bool bChangeFail = false;
		int eTargetCharGrade = ConvertPowerUpItemToChar( ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode ) );		
		EquipItemExtendType eExtendType = GetEquipItemExtendType( kSlot.m_iItemCode );

		if( eExtendType == EIET_EXTRA_POWERUP )
		{
			if( !g_PowerUpManager.CheckEnableEquipPowerUpItem( rkCharInfo.m_eCharPowerUpType, kSlot.m_iItemCode ) )
			{
				bChangeFail	= true;
				szClassName = g_MyInfo.GetClassName( iEnableClass, eTargetCharGrade );
			}
		}
		else if(  eExtendType == EIET_BASIC_EXTREA || eExtendType == EIET_SPECIAL_EXTREA )
		{		
			if( eTargetCharGrade != rkCharInfo.m_eCharPowerUpType )
			{
				bChangeFail = true;
				szClassName = g_MyInfo.GetClassName( iEnableClass );
			}
		}

		if( bChangeFail )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(13) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s만 착용가능합니다.", szClassName.c_str() );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
			return false;
		}
	}

	// 봉인, 봉인가능 체크
	int iSlot = kSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	if( kSlot.m_iTradeState != EET_DISABLE )
	{
		ExtraItemEquipConfirmWnd *pConfirmWnd = dynamic_cast<ExtraItemEquipConfirmWnd*>(g_GUIMgr.FindWnd(EXTRAITEM_EQUIPL_CONFIRM_WND));
		if( pConfirmWnd && pConfirmWnd->SetExtraItemEquipInfo( iCharArray, iSlot, iNewSlotIndex ) )
		{
			m_dwExtraItemChangeTime = dwCurTime;
			return true;
		}
		return false;
	}

	// 출전중인 용병만
	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( !pOwner->SetExtraItemChangeState( false ) )
					return false;
			}
		}
	}

	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_EXTRAITEM_CHANGE );
	kPacket << iCharArray;
	kPacket << iSlot;
	kPacket << iNewSlotIndex;
	TCPNetwork::SendToServer( kPacket );

	g_MyInfo.SetExtraItemChange( 1, iCharArray, iSlot, iNewSlotIndex );
	g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP, kSlot.m_iItemCode, kSlot.m_PeriodType );
	return true;
}

bool MyInventoryWnd::SetEquipDecoItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	// 오른쪽 창으로부터 캐릭터 정보 가져와야함.
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return false;
	if( !pInfoWnd->IsShow() ) return false;

	ShowMainInfoWnd();
	pInfoWnd->SetInvenItemInfo( m_iSelectClassType, pInvenItem->GetInfo() );

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iDecoType = pInvenItem->GetDecoType();
	int iDecoCode = pInvenItem->GetDecoCode();

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipDecoItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return false;
	}

	if( iDecoType == UID_KINDRED && !EnableDecoChangeByEquipItem( rkCharInfo ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return false;
	}

	// 출전중인 용병만
	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( pOwner->IsObejctEquipState() )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->IsInvisibleHead() )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->HasBuff( BT_CHANGE_SKIN_COLOR ) )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->HasBuff( BT_CHANGE_MESH ) )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				int iCharState = (int)pOwner->GetState();
				if ( iCharState != CS_DELAY )
					return false;

				if( !pOwner->EnableDecoChangeByEquipItem( iDecoType ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
					return false;
				}
			}
		}
	}

	if( iDecoType == 0 )
		return false;

	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	int iType = (rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000);
	int iCode = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			iType += iDecoType;
			iCode =  iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			iType += iEquipType;
			iCode  = iEquipCode;
		}
		break;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_EQUIP_SLOT_ITEM );
	kPacket << EQUIP_SLOT_ITEM_STATE;
	kPacket << iCharArray;
	kPacket << iType << iCode;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool MyInventoryWnd::SetEquipMedalItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	// 오른쪽 창으로부터 캐릭터 정보 가져와야함.
	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( !pInfoWnd ) return false;

	ShowMainInfoWnd();
	ShowExtraitemFuncWnd();
	pInfoWnd->SetExtraItemInfo( m_iSelectClassType );

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iMedalItemType = pInvenItem->GetMagicCode();
	if( iMedalItemType <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Wrong Btn : %d", dwID );
		return false;
	}

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pMedalItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Don't Have UserMedalItem" );
		return false;
	}
	
	int iCustomIndex = pInvenItem->GetMagicCodeEx();
	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( !pMedalItem->GetMedalItem( iMedalItemType, iCustomIndex, kSlot, true ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Not Exist Item : %d", iMedalItemType );
		return false;
	}

	if( kSlot.m_iEquipClass != 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Equiped Item : %d", iMedalItemType );
		return false;
	}

	const ioMedalItemInfoManager::ItemInfo *pItem = g_MedalItemMgr.GetItemInfo( iMedalItemType );
	if( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetEquipMedalItem Not Exist Item : %d", iMedalItemType );
		return false;
	}

	// 클레스 체크
	if( !g_MedalItemMgr.IsRight( iMedalItemType, rkCharInfo.m_class_type) )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(7) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
		return false;
	}	

	// 가능 레벨 체크
	int iEnableLevel = g_MedalItemMgr.GetLevelLimit( iMedalItemType );
	int iCurLevel = g_MyInfo.GetClassLevel( rkCharInfo.m_class_type, true );
	if( iCurLevel < iEnableLevel )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(8) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY ); // 노란
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(9), iEnableLevel );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return false;
	}

	MedalSyncInfoVec vItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vItemTypeVec, rkCharInfo.m_class_type );
	int iEquipMedalItemCnt = vItemTypeVec.size();
	if( iEquipMedalItemCnt >= ioMedalItemInfoManager::MAX_SLOT_NUM || 
		iEquipMedalItemCnt >= g_MedalItemMgr.GetSlotNum( iCurLevel ) + g_ExpandMedalSlotMgr.GetExpandMedalSlotNum( rkCharInfo.m_class_type ) )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(10) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwMedalItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}
	m_dwMedalItemChangeTime = dwCurTime;
	
	if( pItem->m_iSubMedalType == ioMedalItemInfoManager::MEDALTYPE_NORMAL )
	{
		SP2Packet kPacket( CTPK_MEDALITEM_CHANGE );
		kPacket << iCharArray;
		kPacket << iMedalItemType;
		kPacket << true;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		SP2Packet kPacket( CTPK_CUSTOM_MEDALITEM_CHANGE );
		kPacket << rkCharInfo.m_class_type;
		kPacket << true;
		kPacket << iMedalItemType;
		kPacket << iCustomIndex;
		TCPNetwork::SendToServer( kPacket );
	}
	TCPNetwork::MouseBusy( true );
	return true;
}

// UI코스튬 관련 (코스튬 장착)
bool MyInventoryWnd::SetEquipCostume( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) 
		return false;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) 
		return false;

	ShowMainInfoWnd();
	pInfoWnd->SetInvenItemInfo( m_iSelectClassType, pInvenItem->GetInfo() );

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int nNewSlotIndex = pInvenItem->GetMagicCode();
	if( nNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류1" );
		return false;
	}

	int nCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( nCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류2" );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#장비 장착이 불가능합니다." );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return false;
	}	

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류3" );
		return false;
	}

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( nNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류4" );
		return false;
	}

	if( kSlot.m_bEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류5" );
		return false;
	}

	CostumeInfo sCostumeInfo;
	if( !g_CostumeInfoMgr.GetCostumeInfo( kSlot.m_nCostumeCode, sCostumeInfo ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 장착중 오류6" );
		return false;
	}

	// 클레스 체크
	if( sCostumeInfo.m_nEnableClass > 0 && 
		rkCharInfo.m_class_type != sCostumeInfo.m_nEnableClass )
	{
		if( sCostumeInfo.m_nEnableClass > 0 )
		{
			ioHashString szClassName = g_MyInfo.GetClassName( sCostumeInfo.m_nEnableClass, rkCharInfo.m_eCharPowerUpType );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택한 장비는" );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s만 착용가능합니다.", szClassName.c_str() );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택한 장비는" );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, "착용할 수 없습니다." );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}

		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}
	
	// 출전중인 용병만
	if( nCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				// 장비 교체 가능상태 체크
				if( !pOwner->SetExtraItemChangeState( false ) )
					return false;
			}
		}
	}
	
	m_dwExtraItemChangeTime = dwCurTime;

	// UI코스튬 관련 (장착 패킷 전송)
	TCPNetwork::MouseBusy( true );	
	SP2Packet kPacket( CTPK_COSTUME_CHANGE );
	kPacket << g_MyInfo.GetCharIndex(nCharArray);
	kPacket << (byte)(kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE);
	kPacket << nNewSlotIndex;
	kPacket << true;
	TCPNetwork::SendToServer( kPacket );
	return true;
}

bool MyInventoryWnd::SetSelectAccessory( DWORD dwID )
{
	AccessoryFuncWnd *pInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( !pInfoWnd )
		return false;

	if( pInfoWnd->GetTabState() == AccessoryFuncWnd::ID_INFO_TAB_BTN )
		return SetEquipAccessory( dwID );
	else
		return SetSelectAccessoryClickAction( dwID );
}

bool MyInventoryWnd::SetSelectAccessoryClickAction( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem )
		return false;

	AccessoryFuncWnd *pInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( pInfoWnd )
	{
		pInfoWnd->SelectClickAction( pInvenItem->GetMagicCode() );
		if( !pInfoWnd->IsShow() )
			pInfoWnd->ShowWnd();
	}
	return true;
}

bool MyInventoryWnd::SetEquipAccessory( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem )
		return false;

	ShowAccessoryFuncWnd();

	AccessoryFuncWnd *pInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( !pInfoWnd )
		return false;

	if( m_iSelectClassType == -1 )
		m_iSelectClassType = g_MyInfo.GetClassType();

	pInfoWnd->SetAccessoryInfo( m_iSelectClassType );

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류1" );
		return false;
	}

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류2" );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#장비 장착이 불가능합니다." );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return false;
	}	

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류3" );
		return false;
	}

	AccessorySlot kSlot;
	if( !pAccessory->GetAccessorySlot( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류4" );
		return false;
	}

	if( kSlot.m_bEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류5" );
		return false;
	}

	time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
	kLimitTime += 30;
	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );
	int iGapSec = kLimitTime - kCurServerTime;
	if( iGapSec <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szEquipAccessoryErrMsg.c_str() );
		return false;
	}

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iAccessoryCode, __FUNCTION__ );
	if( !pItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 장착중 오류6" );
		return false;
	}

	// 클레스 체크
	bool bDisableClass = false;
	int iEnableClass = pItem->GetEnableClass();
	if( iEnableClass > 0 && rkCharInfo.m_class_type != iEnableClass )
	{
		bDisableClass = true;
	}

	// 성별 체크
	bool bDisableSex = false;
	int iOnlySetType = pItem->GetCharOnlySexType();
	if( iOnlySetType != ioSetItemInfo::CST_ALL && rkCharInfo.m_sex != iOnlySetType )
	{
		bDisableSex = true;
	}

	if( bDisableClass || bDisableSex )
	{
		if( iEnableClass > 0 )
		{
			ioHashString szClassName = g_MyInfo.GetClassName( iEnableClass );

			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택한 장비는" );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );

			switch( iOnlySetType )
			{
			case ioSetItemInfo::CST_ALL:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s만 착용가능합니다.", szClassName.c_str() );
				break;
			case ioSetItemInfo::CST_MALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s 남성만 착용가능합니다.", szClassName.c_str() );
				break;
			case ioSetItemInfo::CST_FEMALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "%s 여성만 착용가능합니다.", szClassName.c_str() );
				break;
			}

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		else
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택한 장비는" );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );

			switch( iOnlySetType )
			{
			case ioSetItemInfo::CST_ALL:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "착용할 수 없습니다." );
				break;
			case ioSetItemInfo::CST_MALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "남성만 착용가능합니다." );
				break;
			case ioSetItemInfo::CST_FEMALE:
				kPrinter[1].AddTextPiece( FONT_SIZE_13, "여성만 착용가능합니다." );
				break;
			}

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}

		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	// 출전중인 용병만
	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				// 장비 교체 가능상태 체크
				if( !pOwner->SetExtraItemChangeState( false ) )
					return false;
			}
		}
	}

	int iSlot = kSlot.m_iAccessoryCode / DEFAULT_BASIC_ACCESSORY_CODE - 1;
	if ( !COMPARE( iSlot, 0, MAX_ACC_INVENTORY ) )
		return false;
	
	m_dwExtraItemChangeTime = dwCurTime;

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_ACCESSORY_CHANGE );
	kPacket << g_MyInfo.GetCharIndex(iCharArray);
	kPacket << (BYTE)iSlot;
	kPacket << iNewSlotIndex;
	kPacket << true;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool MyInventoryWnd::SetReleaseDecoItem( int iDecoType, int iDecoCode )
{
	// 오른쪽 창으로부터 캐릭터 정보 가져와야함.
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseDecoItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	if( iDecoType == UID_KINDRED && !EnableDecoChangeByEquipItem( rkCharInfo ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}

	// 출전중인 용병만
	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( pOwner->IsObejctEquipState() )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->IsInvisibleHead() )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->HasBuff( BT_CHANGE_SKIN_COLOR ) )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( pOwner->HasBuff( BT_CHANGE_MESH ) )
				{
					g_ChatMgr.SetSystemMsg( STR(2) );
					return false;
				}

				if( !pOwner->EnableDecoChangeByEquipItem( iDecoType ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
					return false;
				}
			}
		}
	}

	if( iDecoType == 0 )
		return false;

	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	int iType = (rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000);
	int iCode = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			iType += iDecoType;
			iCode =  iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			iType += iEquipType;
			iCode  = iEquipCode;
		}
		break;
	}

	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_EQUIP_SLOT_ITEM );
	kPacket << RELEASE_SLOT_ITEM_STATE;
	kPacket << iCharArray;
	kPacket << iType << iCode;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool MyInventoryWnd::SetReleaseExtraItem( int iSlotIndex )
{
	// 오른쪽 창으로부터 캐릭터 정보 가져와야함.
	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( !pInfoWnd ) return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseExtraItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}	

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseExtraItem Don't Have UserExtraItem" );
		return false;
	}

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseExtraItem Not Exist Item : %d", iSlotIndex );
		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	// 출전중인 용병만
	if( iCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( !pOwner->SetExtraItemChangeState( false ) )
				{
					return false;
				}
			}
		}
	}

	int iSlot = kSlot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	m_dwExtraItemChangeTime = dwCurTime;

	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_EXTRAITEM_CHANGE );
	kPacket << iCharArray;
	kPacket << iSlot;
	kPacket << 0;
	TCPNetwork::SendToServer( kPacket );

	g_MyInfo.SetExtraItemChange( 2, iCharArray, iSlot, 0 );
	return true;
}

bool MyInventoryWnd::SetReleaseMedalItem( int iMedalItemType, int iCustomIndex )
{
	// 오른쪽 창으로부터 캐릭터 정보 가져와야함.
	ExtraItemFuncWnd *pInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( !pInfoWnd ) return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseMedalItem Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return false;
	}	

	ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pMedalItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseMedalItem Don't Have UserMedalItem" );
		return false;
	}

	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( !pMedalItem->GetMedalItem( iMedalItemType, iCustomIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReleaseMedalItem Not Exist Item : %d", iMedalItemType );
		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwMedalItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	m_dwMedalItemChangeTime = dwCurTime;

	if( iCustomIndex == 0 )
	{
		SP2Packet kPacket( CTPK_MEDALITEM_CHANGE );
		kPacket << iCharArray;
		kPacket << iMedalItemType;
		kPacket << false;
		TCPNetwork::SendToServer( kPacket );
	}
	else
	{
		SP2Packet kPacket( CTPK_CUSTOM_MEDALITEM_CHANGE );
		kPacket << rkCharInfo.m_class_type;
		kPacket << false;
		kPacket << iMedalItemType;
		kPacket << iCustomIndex;
		TCPNetwork::SendToServer( kPacket );
	}	
	TCPNetwork::MouseBusy( true );
	return true;
}

// UI코스튬 관련 (코스튬 해제)
bool MyInventoryWnd::SetReleaseCostume( int iSlotIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd ) 
		return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int nCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( nCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 해제 오류1" );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#장비 해제가 불가능합니다." );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return false;
	}	

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 해제 오류2" );
		return false;
	}

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( iSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 해제 오류3" );
		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	// 출전중인 용병만
	if( nCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( !pOwner->SetExtraItemChangeState( false ) )
					return false;
			}
		}
	}

	m_dwExtraItemChangeTime = dwCurTime;

	// UI코스튬 관련 (장착 패킷 전송)
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_COSTUME_CHANGE );
	kPacket << g_MyInfo.GetCharIndex(nCharArray);
	kPacket << (byte)(kSlot.m_nCostumeCode/DEFAULT_BASIC_COSTUME_CODE);
	kPacket << iSlotIndex;
	kPacket << false;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool MyInventoryWnd::SetReleaseAccessory( int iSlotIndex )
{
	AccessoryFuncWnd *pInfoWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( !pInfoWnd )
		return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();
	
	int nCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( nCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 해제 오류1" );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#장비 해제가 불가능합니다." );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return false;
	}	

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 해제 오류2" );
		return false;
	}

	AccessorySlot kSlot;
	if( !pAccessory->GetAccessorySlot( iSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 해제 오류3" );
		return false;
	}

	// 제한 시간 체크
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGap = dwCurTime - m_dwExtraItemChangeTime;
	if( dwGap < 500 )
	{
		return false;
	}

	// 출전중인 용병만
	if( nCharArray == g_MyInfo.GetSelectCharArray() )
	{
		if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
		{
			ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
			if( pOwner )
			{
				if( !pOwner->SetExtraItemChangeState( false ) )
					return false;
			}
		}
	}

	int iSlot = kSlot.m_iAccessoryCode / DEFAULT_BASIC_ACCESSORY_CODE - 1;
	if ( !COMPARE( iSlot, 0, MAX_ACC_INVENTORY ) )
		return false;

	m_dwExtraItemChangeTime = dwCurTime;

	// UI코스튬 관련 (장착 패킷 전송)
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_ACCESSORY_CHANGE );
	kPacket << g_MyInfo.GetCharIndex(nCharArray);
	kPacket << (BYTE)iSlot;
	kPacket << iSlotIndex;
	kPacket << false;
	TCPNetwork::SendToServer( kPacket );

	return true;
}

bool MyInventoryWnd::SetReSellExtraItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellExtraItem Wrong Btn : %d", dwID );
		return false;
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellExtraItem Don't Have UserExtraItem" );
		return false;
	}

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellExtraItem Not Exist Item : %d", iNewSlotIndex );
		return false;
	}

	if( kSlot.m_bCharEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellExtraItem Item : %d", iNewSlotIndex );
		return false;
	}

	if( !CheckTimeEndExtraItem( iNewSlotIndex ) )
		return false;

	m_iSellSlotIndex = iNewSlotIndex;

	float fReturnPeso = 0.0f;
	DWORD dwEditType = SELL_EDIT_NONE;
	if( kSlot.m_PeriodType == EPT_MORTMAIN )
	{
		fReturnPeso = g_ExtraItemInfoMgr.GetMortmainItemSellPesoReinforce( kSlot.m_iReinforce );
		dwEditType = SELL_EDIT_EXTRAITEM;
	}
	else
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		int iTotal = iGapSec / 60;

		fReturnPeso = g_ExtraItemInfoMgr.GetTimeItemSellPesoReinforce( iTotal, kSlot.m_iReinforce );
		fReturnPeso = max( 0, fReturnPeso );
	}

	__int64 iPeso = (__int64)fReturnPeso;

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof( szConvertNum ) );

	ioHashString sInfo;
	pExtraItem->GetUseInfo( iNewSlotIndex, sInfo );

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, iNewSlotIndex );
	}

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, STR(5), kSlot.m_Name.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, STR(6), sInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(7) );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(8) );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, STR(9), szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, STR(10) );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		int nGradeType = 0;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ioUIRenderImage *pSubImg = NULL;
		const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
		if( pItem )
			nGradeType = pItem->GetGradeType();

		pResellWnd->SetInfoAndShow( STR(11), pImg, pSubImg, kSlot.m_iReinforce, kPrinter, this, dwEditType, ACST_SELL, false, nGradeType );
	}

	return true;
}

bool MyInventoryWnd::SetReSellMedalItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	int iMedalIndex = pInvenItem->GetMagicCode();
	if( iMedalIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생1" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellMedalItem Wrong Btn : %d", dwID );
		return false;
	}

	ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
	if( !pMedalItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생2" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellMedalItem Don't Have UserMedalItem" );
		return false;
	}
	
	int iCustomIndex = pInvenItem->GetMagicCodeEx();
	ioUserMedalItem::MEDALITEMSLOT kSlot;
	if( !pMedalItem->GetMedalItem( iMedalIndex, iCustomIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생3" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellMedalItem Not Exist Item : %d", iMedalIndex );
		return false;
	}
	
	if( !CheckTimeEndMedalItem( kSlot ) )
		return false;

	m_iSellSlotIndex = iMedalIndex;
	m_iSellSubIndex = iCustomIndex;

	float fReturnPeso = 0.0f;
	DWORD dwEditType = SELL_EDIT_NONE;
	if( kSlot.m_iPeriodType == ioUserMedalItem::PT_MORTMAIN )
	{
		fReturnPeso = g_MedalItemMgr.GetSellPeso( kSlot.m_iItemType );
		dwEditType = SELL_EDIT_EXTRAITEM;
	}
	else
	{
		fReturnPeso = g_MedalItemMgr.GetSellPeso( kSlot.m_iItemType );
		fReturnPeso = fReturnPeso / g_MedalItemMgr.GetSellPesoByMinute();

		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		int iTotal = iGapSec / 60;

		fReturnPeso = iTotal * fReturnPeso;
		fReturnPeso = min( fReturnPeso, g_MedalItemMgr.GetSellPeso( kSlot.m_iItemType ) );
	}

	int iPeso = static_cast<int>(fReturnPeso);

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof( szConvertNum ) );

	ioHashString sInfo;
	pMedalItem->GetUseInfo( iMedalIndex, iCustomIndex, sInfo );
	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, iMedalIndex );
	}

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	const ioMedalItemInfoManager::ItemInfo *kMedalInfo = g_MedalItemMgr.GetItemInfo( kSlot.m_iItemType );
	if( !kMedalInfo )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생5" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellMedalItem Not Exist Item : %d", kSlot.m_iItemType );
		return false;
	}

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, "%s", kMedalInfo->m_sName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, "%s", sInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "위 메달을 판매합니다." );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "예상 환불금액 :" );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "%s 페소", szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "(다소 차이가 있을 수 있습니다)" );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kMedalInfo->m_sIcon );
		ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( kMedalInfo->m_sSubIcon );
		pResellWnd->SetInfoAndShow( "메달팔기", pImg, pSubImg, 0, kPrinter, this, dwEditType, ACST_SELL_MEDAL, false );
	}

	return true;
}

// UI코스튬 관련(되팔기)
bool MyInventoryWnd::SetReSellCostume( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) 
		return false;

	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 판매 오류 발생1" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellCostume Wrong Btn : %d", dwID );
		return false;
	}

	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 판매 오류 발생2" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellCostume Don't Have UserExtraItem" );
		return false;
	}

	CostumeSlot kSlot;
	if( !pCostume->GetCostumeSlot( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 판매 오류 발생3" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellCostume Not Exist Item : %d", iNewSlotIndex );
		return false;
	}

	if( kSlot.m_bEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "코스튬 판매 오류 발생4" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellCostume Item : %d", iNewSlotIndex );
		return false;
	}

	if( !CheckTimeEndCostume( iNewSlotIndex ) )
		return false;

	m_iSellSlotIndex = iNewSlotIndex;

	float fReturnPeso = 0.0f;
	DWORD dwEditType = SELL_EDIT_NONE;
	if( kSlot.m_PeriodType == PCPT_MORTMAIN )
	{
		fReturnPeso = g_CostumeShopInfoMgr.GetResellMortmainCostumePeso();
		dwEditType = SELL_EDIT_EXTRAITEM;
	}
	else
		fReturnPeso = g_CostumeShopInfoMgr.GetResellTimeCostumeTimePeso();

	__int64 iPeso = (__int64)fReturnPeso;

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof( szConvertNum ) );

	ioHashString sInfo;
	pCostume->GetUseInfo( iNewSlotIndex, sInfo );

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, iNewSlotIndex );
	}

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, "%s", kSlot.m_Name.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, "%s", sInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "위 코스튬을 판매합니다." );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "예상 환불금액 : " );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "%s 페소", szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "(다소 차이가 있을 수 있습니다)" );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ioUIRenderImage *pSubImg = NULL;
		pResellWnd->SetInfoAndShow( "코스튬팔기", pImg, pSubImg, 0, kPrinter, this, dwEditType, ACST_SELL_COSTUME, true, g_CostumeInfoMgr.GetGradeType( kSlot.m_nCostumeCode ) );
	}

	return true;
}

bool MyInventoryWnd::SetEtcItemUse( int iType )
{
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, iType );
		return false;
	}

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL : %d", __FUNCTION__, iType );
		return false;
	}

	pEtcItem->OnUseBtnPush( iType, pUserEtcItem );

	return true;
}

bool MyInventoryWnd::SetDisassembleExtraItem( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) return false;

	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생1" );
		return false;
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생2" );
		return false;
	}

	EXTRAITEMSLOT kSlot;
	if( !pExtraItem->GetExtraItem( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생3" );
		return false;
	}

	if( kSlot.m_bCharEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류 발생4" );
		return false;
	}

	if( !CheckTimeEndExtraItem( iNewSlotIndex ) )
		return false;

	m_iSellSlotIndex = iNewSlotIndex;

	int iMinCnt = 0;
	int iMaxCnt = 0;
	DWORD dwEditType = SELL_EDIT_NONE;
	if( kSlot.m_PeriodType == EPT_MORTMAIN )
	{
		dwEditType = SELL_EDIT_NONE;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간제 장비는 분해할 수 없습니다." );
		return false;
	}

	// ui 처리
	ioHashString sInfo;
	pExtraItem->GetUseInfo( iNewSlotIndex, sInfo );

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, "%s", kSlot.m_Name.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, "%s", sInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "위 장비를 분해합니다." );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "분해 : " );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE );
	if( pEtcItem )
		kPrinter[3].AddTextPiece( FONT_SIZE_13, "%s 획득", pEtcItem->GetName().c_str() );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		int nGradeType = 0;
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ioUIRenderImage *pSubImg = NULL;
		const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
		if( pItem )
			nGradeType = pItem->GetGradeType();

		pResellWnd->SetInfoAndShow( "장비분해", pImg, pSubImg, kSlot.m_iReinforce, kPrinter, this, dwEditType, ACST_DISASSEMBLE, false, nGradeType );
	}

	return true;
}

bool MyInventoryWnd::CheckTimeEndExtraItem( int iSlotIndex )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
		{
			if( kSlot.m_PeriodType != EPT_MORTMAIN )
			{
				time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
				time_t kCurServerTime = g_MyInfo.GetServerDate();
				kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

				int iGapSec = kLimitTime - kCurServerTime;
				enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
				int iDay    = iGapSec/DAY_SEC;
				int iHourSec= iGapSec-(iDay*DAY_SEC);
				int iHour   = iHourSec / HOUR_SEC;
				int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

				if( iDay <= 0 && iHour <= 0 && iMinute <= 1 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return false;
				}
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	return true;
}

bool MyInventoryWnd::CheckTimeEndMedalItem( ioUserMedalItem::MEDALITEMSLOT kSlot )
{
	if( kSlot.m_iPeriodType != EPT_MORTMAIN )
	{
		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

		int iGapSec = kLimitTime - kCurServerTime;
		enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
		int iDay    = iGapSec/DAY_SEC;
		int iHourSec= iGapSec-(iDay*DAY_SEC);
		int iHour   = iHourSec / HOUR_SEC;
		int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

		if( iDay <= 0 && iHour <= 0 && iMinute <= 1 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간만료로 판매할 수 없습니다." );
			return false;
		}
	}

	return true;
}

// UI코스튬 관련 (기간제 시간 체크)
bool MyInventoryWnd::CheckTimeEndCostume( int iSlotIndex )
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( pCostume )
	{
		CostumeSlot kSlot;
		if( pCostume->GetCostumeSlot( iSlotIndex, kSlot ) )
		{
			if( kSlot.m_PeriodType != PCPT_MORTMAIN )
			{
				time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
				time_t kCurServerTime = g_MyInfo.GetServerDate();
				kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

				int iGapSec = kLimitTime - kCurServerTime;
				enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
				int iDay    = iGapSec/DAY_SEC;
				int iHourSec= iGapSec-(iDay*DAY_SEC);
				int iHour   = iHourSec / HOUR_SEC;
				int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

				if( iDay <= 0 && iHour <= 0 && iMinute <= 1 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간만료로 판매할 수 없습니다." );
					return false;
				}
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간만료로 판매할 수 없습니다." );
		return false;
	}

	return true;
}

void MyInventoryWnd::CheckDefaultActionBtn()
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
	{
		pInfoWnd->CheckDefaultActionBtn();
	}
}

void MyInventoryWnd::SetEtcItemUseBtn( const InventoryItemInfo &rkInfo )
{
	int iType = rkInfo.m_iMagicCode;

	SetEtcItemUse( iType );
}

void MyInventoryWnd::ShowMainInfoWnd()
{
	// 예외 체크
	GashaponWnd *pGashaponWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pGashaponWnd && pGashaponWnd->IsShow() && pGashaponWnd->IsRolling() )
		return;

	SuperGashaponWnd *pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*>(FindChildWnd(ID_SUPER_GASHAPON_WND));
	if( pSuperGashaponWnd && pSuperGashaponWnd->IsShow() && pSuperGashaponWnd->IsRolling() )
		return;

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd(ID_MULTI_SUPER_GASHAPON_WND));
	if( pMultiSuperGashaponWnd && pMultiSuperGashaponWnd->IsShow() && pMultiSuperGashaponWnd->IsRolling() )
		return;

	RandomDecoWnd *pRandomDecoWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pRandomDecoWnd && pRandomDecoWnd->IsShow() && pRandomDecoWnd->IsRolling() )
		return;

	PetEggResultWnd *pPetEggResultWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pPetEggResultWnd && pPetEggResultWnd->IsShow() && pPetEggResultWnd->IsRolling() )
		return;      

	// 2019-04-30
	ExtraItemFuncWnd *pExtraItemWnd = dynamic_cast<ExtraItemFuncWnd*> (FindChildWnd(MyInventoryWnd::ID_EXTRAITEMFUNC_WND) );
	if( NULL != pExtraItemWnd )
	{
		pExtraItemWnd->SetRollingStopSound();
	}

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd )
	{
		pInfoWnd->HideWnd();
		ShowChildWnd( ID_FLASH_WND );
		pInfoWnd->ShowWnd();
	}

	HideChildWnd(ID_GROWTH_DOWN_WND);
	HideChildWnd(ID_ITEM_COMPOUND_WND);
	HideChildWnd(ID_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_PACKAGE_ITEM_WND);
	HideChildWnd(ID_GASHPON_WND);
	HideChildWnd(ID_SUPER_GASHAPON_WND);
	HideChildWnd(ID_MULTI_SUPER_GASHAPON_WND);
	HideChildWnd(ID_MULTI_SUPER_GASHAPON_TOAST_POPUP_WND);
	HideChildWnd(ID_RANDOM_DECO_WND);
	HideChildWnd(ID_DECO_SELECT_WND);
	HideChildWnd(ID_MULTIPLE_COMPOUND_RESULT);
	HideChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_RESULT);
	HideChildWnd(ID_ITEM_COMPOUND_RESULT);
	HideChildWnd(ID_EXTRAITEM_BOX_WND);
	HideChildWnd(ID_GROWTH_DOWN_RESULT);
	HideChildWnd(ID_PACKAGE_ITEM_RESULT);
	HideChildWnd(ID_CHANGE_NAME_WND);
	HideChildWnd(ID_CHANGE_NAME_RESULT_WND);
	HideChildWnd(ID_TRADE_STATE_CHANGE_WND);
	HideChildWnd(ID_ITEM_RECVSELL_INFO_WND);
	HideChildWnd(ID_RECORD_INIT_WND);
	HideChildWnd(ID_RECORD_INIT_RESULT_WND);
	HideChildWnd(ID_ITEM_GROWTH_CATALYST_WND);
	HideChildWnd(ID_ITEM_GROWTH_CATALYST_RESULT_WND);
	HideChildWnd(ID_ITEM_LUCKY_COIN_USE_WND);
	HideChildWnd(ID_ITEM_LUCKY_COIN_USE_RESULT_WND);
	HideChildWnd(ID_ITEM_COMPOUNDEX_WND);
	HideChildWnd(ID_ITEM_COMPOUNDEX_RESULT_WND);
	HideChildWnd(ID_ITEM_RAINBOW_MIXER_WND);
	HideChildWnd(ID_ITEM_RAINBOW_MIXER_RESULT_WND);
	HideChildWnd(ID_ITEM_LOSTSAGA_MIXER_WND);
	HideChildWnd(ID_ITEM_LOSTSAGA_MIXER_RESULT_WND);
	HideChildWnd(ID_ITEM_GOLD_BOX_WND);
	HideChildWnd(ID_SOLDIER_SELECT_BUY_WND);
	HideChildWnd(ID_SOLDIER_SELECT_RESULT_WND);
	HideChildWnd(ID_FOUR_EXTRA_COMPOUND_BUY_WND);
	HideChildWnd(ID_FOUR_EXTRA_COMPOUND_RESULT_WND);
	HideChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_WND);
	HideChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_RESULT_WND);
	HideChildWnd(ID_EXPAND_MEDAL_SLOT_OPEN_WND);
	HideChildWnd(ID_EXPAND_MEDAL_SLOT_RESULT_WND);
	HideChildWnd(ID_PRESET_PACKAGE_WND);
	HideChildWnd(ID_PRESET_PACKAGE_RESULT_WND);
	HideChildWnd(ID_GROWTH_ALL_DOWN_WND);
	HideChildWnd(ID_GROWTH_ALL_DOWN_RESULT);
	HideChildWnd(ID_SPIRITFUNC_WND);
	HideChildWnd(ID_ACCESSORYFUNC_WND);
	HideChildWnd(ID_EXTRAITEMFUNC_WND);

	HideChildWnd(ID_ITEM_MATERIAL_COMPOUND_WND);
	HideChildWnd(ID_ITEM_MATERIAL_COMPOUND_RESULT);
	HideChildWnd(ID_PET_EGG_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_PREMIUM_RESULT_WND);
	HideChildWnd(ID_NAMED_TITLE_PREMIUM_SELECT_WND);

	HideChildWnd(ID_HERO_REINFORCE_DWON_WND);
	HideChildWnd(ID_HERO_REINFORCE_DONE_WND);
	HideChildWnd(ID_ITEM_REINFORCE_DWON_WND);
	HideChildWnd(ID_ITEM_REINFORCE_DONE_WND);
	HideChildWnd(ID_COSTUME_BOX_WND);
	HideChildWnd(ID_NEW_MULTIPLE_COMPOUND_WND);
	HideChildWnd(ID_ACCESSORY_COMPOUND_WND);
	HideChildWnd(ID_ACCESSORY_COMPOUND_RESULT);
}

void MyInventoryWnd::ShowGrowthDownWnd()
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GrowthDownSelectWnd *pWnd = dynamic_cast<GrowthDownSelectWnd*>(FindChildWnd(ID_GROWTH_DOWN_WND));
	if( pWnd )
		pWnd->ShowWnd();
}

void MyInventoryWnd::OnGrowthDownResult( int iClassType, int iReturnPoint )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GrowthDownDoneWnd *pWnd = dynamic_cast<GrowthDownDoneWnd*>(FindChildWnd(ID_GROWTH_DOWN_RESULT));
	if( pWnd )
		pWnd->ShowGrowthDownDone( iClassType, iReturnPoint );
}

void MyInventoryWnd::ShowGrowthAllDownWnd()
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GrowthAllDownSelectWnd *pWnd = dynamic_cast<GrowthAllDownSelectWnd*>(FindChildWnd(ID_GROWTH_ALL_DOWN_WND));
	if( pWnd )
		pWnd->ShowWnd();
}

void MyInventoryWnd::OnGrowthAllDownResult( int iClassType, int iReturnPoint, __int64 iReturnPeso )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GrowthAllDownDoneWnd *pWnd = dynamic_cast<GrowthAllDownDoneWnd*>(FindChildWnd(ID_GROWTH_ALL_DOWN_RESULT));
	if( pWnd )
		pWnd->ShowGrowthAllDownDone( iClassType, iReturnPoint, iReturnPeso );
}

void MyInventoryWnd::ShowItemCompoundWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemCompoundWnd *pWnd = dynamic_cast<ItemCompoundWnd*>(FindChildWnd(ID_ITEM_COMPOUND_WND));
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwCode );
}

void MyInventoryWnd::OnItemCompoundResult( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_COMPOUND_WND);

	ShowChildWnd( ID_FLASH_WND );
	ItemCompoundResultWnd *pWnd = dynamic_cast<ItemCompoundResultWnd*>(FindChildWnd(ID_ITEM_COMPOUND_RESULT));
	if( pWnd )
		pWnd->SetCompoundResultInfo( bSuccess, iTargetSlot, iReinforce, dwCode );
}

void MyInventoryWnd::ShowItemGrowthCatalystWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemGrowthCatalystWnd *pWnd = dynamic_cast<ItemGrowthCatalystWnd*>(FindChildWnd(ID_ITEM_GROWTH_CATALYST_WND));
	if( pWnd )
	{
		pWnd->ShowWndWithEtcItemCode( dwCode );
	}
}

void MyInventoryWnd::OnItemGrowthCatalystDelay( int iTargetSlot )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_GROWTH_CATALYST_WND);

	ShowChildWnd( ID_FLASH_WND );
	
	ItemGrowthCatalystResultWnd *pWnd = dynamic_cast<ItemGrowthCatalystResultWnd*>(FindChildWnd(ID_ITEM_GROWTH_CATALYST_RESULT_WND));
	if( pWnd )
		pWnd->SetGrowthCatalystDelayInfo( iTargetSlot );
}

void MyInventoryWnd::OnItemGrowthCatalystResult( bool bSuccess, int iTargetSlot, int iReinforce, int iTargetPeriodType, DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_GROWTH_CATALYST_WND);

	ItemGrowthCatalystResultWnd *pWnd = dynamic_cast<ItemGrowthCatalystResultWnd*>(FindChildWnd(ID_ITEM_GROWTH_CATALYST_RESULT_WND));
	if( pWnd )
		pWnd->SetGrowthCatalystResultInfo( bSuccess, iTargetSlot, iReinforce, iTargetPeriodType, dwCode );
}

void MyInventoryWnd::ShowItemCompoundExWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemCompoundExWnd *pWnd = dynamic_cast<ItemCompoundExWnd*>(FindChildWnd(ID_ITEM_COMPOUNDEX_WND));
	if( pWnd )
	{
		pWnd->ShowWndWithEtcItemCode( dwCode );
	}
}

void MyInventoryWnd::OnItemCompoundExResult( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_COMPOUNDEX_WND);

	ItemCompoundExResultWnd *pWnd = dynamic_cast<ItemCompoundExResultWnd*>(FindChildWnd(ID_ITEM_COMPOUNDEX_RESULT_WND));
	if( pWnd )
		pWnd->SetItemCompoundExInfo( bSuccess, iTargetSlot, iReinforce, dwCode );
}

void MyInventoryWnd::ShowItemLuckyCoinWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemLuckyCoinWnd *pWnd = dynamic_cast<ItemLuckyCoinWnd*>(FindChildWnd(ID_ITEM_LUCKY_COIN_USE_WND));
	if( pWnd )
	{
		pWnd->ShowWndWithEtcItemCode( dwCode );
	}
}

void MyInventoryWnd::OnItemLuckyCoinResult( DWORD dwCode, int iSelectIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_LUCKY_COIN_USE_WND);
	ItemLuckyCoinResultWnd *pWnd = dynamic_cast<ItemLuckyCoinResultWnd*>(FindChildWnd(ID_ITEM_LUCKY_COIN_USE_RESULT_WND));
	if( pWnd )
		pWnd->SetItemLuckyCoinResultInfo( dwCode, iSelectIndex );
}

void MyInventoryWnd::ShowMultipleCompoundWnd( DWORD dwType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	if ( dwType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 )
	{
		UniqueItemMultipleCompoundWnd *pWnd = dynamic_cast<UniqueItemMultipleCompoundWnd*>(FindChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_WND));
		if( pWnd )
			pWnd->SetInfoAndShow( dwType );
	}
	else
	{
		ItemMultipleCompoundWnd *pWnd = dynamic_cast<ItemMultipleCompoundWnd*>(FindChildWnd(ID_MULTIPLE_COMPOUND_WND));
		if( pWnd )
			pWnd->SetInfoAndShow( dwType );
	}
}

void MyInventoryWnd::OnMultipleCompoundResult(int iEtcType, int iItemCode, int iReinforce )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	

	ShowChildWnd( ID_FLASH_WND );
	if ( iEtcType == ioEtcItem::EIT_ETC_MULTIPLE_COMPOUND4 )
	{
		HideChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_WND);
		UniqueItemMultipleCompoundResultWnd *pWnd = dynamic_cast<UniqueItemMultipleCompoundResultWnd*>(FindChildWnd(ID_UNIQUE_MULTIPLE_COMPOUND_RESULT));
		if( pWnd )
			pWnd->SetInfoAndShow( iEtcType, iItemCode, iReinforce );
	}
	else
	{
		HideChildWnd(ID_MULTIPLE_COMPOUND_WND);
		ItemMultipleCompoundResultWnd *pWnd = dynamic_cast<ItemMultipleCompoundResultWnd*>(FindChildWnd(ID_MULTIPLE_COMPOUND_RESULT));
		if( pWnd )
			pWnd->SetInfoAndShow( iEtcType, iItemCode, iReinforce );
	}
}

void MyInventoryWnd::ShowRainbowMixerWnd( DWORD dwType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemRainbowMixerWnd *pWnd = dynamic_cast<ItemRainbowMixerWnd*>(FindChildWnd(ID_ITEM_RAINBOW_MIXER_WND));
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwType );
}

void MyInventoryWnd::OnRainbowMixerResult( int iEtcType, int iSelectIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_RAINBOW_MIXER_WND);
	HideChildWnd(ID_ITEM_GOLD_BOX_WND);

	ShowChildWnd( ID_FLASH_WND );
	ItemRainbowMixerResultWnd *pWnd = dynamic_cast<ItemRainbowMixerResultWnd*>(FindChildWnd(ID_ITEM_RAINBOW_MIXER_RESULT_WND));
	if( pWnd )
		pWnd->SetInfoAndShow( iEtcType, iSelectIndex );
}

void MyInventoryWnd::ShowLostSagaMixerWnd( DWORD dwType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemLostSagaMixerWnd *pWnd = dynamic_cast<ItemLostSagaMixerWnd*>(FindChildWnd(ID_ITEM_LOSTSAGA_MIXER_WND));
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwType );
}

void MyInventoryWnd::OnLostSagaMixerResult( int iEtcType, int iSelectIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_LOSTSAGA_MIXER_WND);

	ShowChildWnd( ID_FLASH_WND );
	ItemLostSagaMixerResultWnd *pWnd = dynamic_cast<ItemLostSagaMixerResultWnd*>(FindChildWnd(ID_ITEM_LOSTSAGA_MIXER_RESULT_WND));
	if( pWnd )
		pWnd->SetInfoAndShow( iEtcType, iSelectIndex );
}

void MyInventoryWnd::ShowGoldBoxWnd( DWORD dwType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemGoldBoxWnd *pWnd = dynamic_cast<ItemGoldBoxWnd*>( FindChildWnd( ID_ITEM_GOLD_BOX_WND ) );
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwType );
}

void MyInventoryWnd::ShowAccessoryCompoundWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	AccessoryCompoundWnd *pWnd = dynamic_cast<AccessoryCompoundWnd*>(FindChildWnd(ID_ACCESSORY_COMPOUND_WND));
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwCode );
}

void MyInventoryWnd::OnAccessoryCompoundResult( int iTargetSlot, int iCompoundCode, int iCompoundValue, DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ACCESSORY_COMPOUND_WND);
	ShowChildWnd( ID_FLASH_WND );
	AccessoryCompoundResultWnd *pWnd = dynamic_cast<AccessoryCompoundResultWnd*>(FindChildWnd(ID_ACCESSORY_COMPOUND_RESULT));
	if( pWnd )
		pWnd->SetCompoundResultInfo( iTargetSlot, iCompoundCode, iCompoundValue, dwCode );
}

void MyInventoryWnd::OnAccessoryReinforceResult( int iTargetSlot )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ACCESSORY_COMPOUND_WND);
	ShowChildWnd( ID_FLASH_WND );
	AccessoryFuncWnd *pWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( pWnd )
		pWnd->OnAccessoryReinforceResult( iTargetSlot );
}

void MyInventoryWnd::ResetAccessoryReinforceTab()
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ACCESSORY_COMPOUND_WND);
	ShowChildWnd( ID_FLASH_WND );
	AccessoryFuncWnd *pWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( pWnd )
		pWnd->ResetAccessoryFuncTab();
}

void MyInventoryWnd::ShowPackageItemWnd( PackageItemBuyWnd::PackageStyle ePackageStyle, DWORD dwType, int eActiveFilter /* = 0 : none */ )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>( FindChildWnd( ID_INVENTORY_INFO_WND ) );
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PackageItemBuyWnd *pWnd = dynamic_cast<PackageItemBuyWnd*>( FindChildWnd( ID_PACKAGE_ITEM_WND ) );
	if( pWnd )
		pWnd->ShowPackage( ePackageStyle, dwType, eActiveFilter );
}

void MyInventoryWnd::ShowSoldierSelectBuyWnd( DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	InvenSoldierSelectBuyWnd *pWnd = dynamic_cast<InvenSoldierSelectBuyWnd*>(FindChildWnd(ID_SOLDIER_SELECT_BUY_WND));
	if( pWnd )
	{
		pWnd->SetEtcItemType( dwEtcItemType );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::ShowFourExtraCompoundBuyWnd( DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	InvenFourExtraCompoundBuyWnd *pWnd = dynamic_cast<InvenFourExtraCompoundBuyWnd*>(FindChildWnd(ID_FOUR_EXTRA_COMPOUND_BUY_WND));
	if( pWnd )
	{
		pWnd->SetEtcItemType( dwEtcItemType );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::OnPackageItemSoldierResult( CHARACTER &rkCharInfo, int iKeepBonusPeso )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	PackageItemBuyWnd *pBuyWnd = dynamic_cast<PackageItemBuyWnd*>(FindChildWnd(ID_PACKAGE_ITEM_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PackageItemBuyResultWnd *pResultWnd = dynamic_cast<PackageItemBuyResultWnd*>(FindChildWnd(ID_PACKAGE_ITEM_RESULT));
	if( pResultWnd )
		pResultWnd->SetResultSoldierPackage( rkCharInfo, iKeepBonusPeso );
}

void MyInventoryWnd::OnPackageItemDecoResult( CHARACTER &rkCharInfo, int iKeepBonusPeso )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	PackageItemBuyWnd *pBuyWnd = dynamic_cast<PackageItemBuyWnd*>(FindChildWnd(ID_PACKAGE_ITEM_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PackageItemBuyResultWnd *pResultWnd = dynamic_cast<PackageItemBuyResultWnd*>(FindChildWnd(ID_PACKAGE_ITEM_RESULT));
	if( pResultWnd )
		pResultWnd->SetResultDecorationPackage( rkCharInfo, iKeepBonusPeso );
}

void MyInventoryWnd::OnSoldierSelectResult( int iClassType, int iClassTime, DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	InvenSoldierSelectBuyWnd *pBuyWnd = dynamic_cast<InvenSoldierSelectBuyWnd*>(FindChildWnd(ID_SOLDIER_SELECT_BUY_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	InvenSoldierSelectBuyResultWnd *pResultWnd = dynamic_cast<InvenSoldierSelectBuyResultWnd*>(FindChildWnd(ID_SOLDIER_SELECT_RESULT_WND));
	if( pResultWnd )
		pResultWnd->ShowResult( iClassType, iClassTime, dwEtcItemType );
}

void MyInventoryWnd::OnForExtraCompoundResult( int iClassType, int iClassTime, DWORD dwEtcItemType, bool bSuccess )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	InvenFourExtraCompoundBuyWnd *pBuyWnd = dynamic_cast<InvenFourExtraCompoundBuyWnd*>(FindChildWnd(ID_FOUR_EXTRA_COMPOUND_BUY_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	InvenFourExtraCompoundBuyResultWnd *pResultWnd = dynamic_cast<InvenFourExtraCompoundBuyResultWnd*>(FindChildWnd(ID_FOUR_EXTRA_COMPOUND_RESULT_WND));
	if( pResultWnd )
		pResultWnd->ShowResult( iClassType, iClassTime, dwEtcItemType, bSuccess );
}

void MyInventoryWnd::ShowGashponWnd( DWORD dwPossessEtcItemType, DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GashaponWnd *pWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pWnd )
		pWnd->ShowRandomBox( dwPossessEtcItemType, dwEtcItemType );
}

void MyInventoryWnd::OnGashponResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso )
{
	GashaponWnd *pWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->OnResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bAlram, iPresentPeso );
	}
}

void MyInventoryWnd::ShowSuperGashponWnd( DWORD dwEtcItemType )
{
	ShowPresentTabDirect();
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();
	
	SuperGashaponWnd *pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*> ( FindChildWnd( ID_SUPER_GASHAPON_WND ) );
	if( pSuperGashaponWnd )
		pSuperGashaponWnd->ShowSuperGashaponWnd( PRESENT_ETC_ITEM, dwEtcItemType, 0, SUPERGASHPON_USE );
}

void MyInventoryWnd::ShowMultiSuperGashaponWnd( DWORD dwEtcItemType )
{
	ShowPresentTabDirect();
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*> ( FindChildWnd( ID_MULTI_SUPER_GASHAPON_WND ) );
	if( pMultiSuperGashaponWnd )
		pMultiSuperGashaponWnd->ShowMultiSuperGashaponWnd( PRESENT_ETC_ITEM, dwEtcItemType, 0, MULTISUPERGASHPON_USE );
}

void MyInventoryWnd::ShowRandomDecoWnd( bool bMan )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	RandomDecoWnd *pWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pWnd )
		pWnd->ShowRandomBox( bMan );
}

void MyInventoryWnd::OnRandomDecoResultInfo( short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, bool bAlram, int iPresentPeso )
{
	RandomDecoWnd *pWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->OnResultInfo( iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bAlram, iPresentPeso );
	}
}

void MyInventoryWnd::ShowDecoSelectWnd( int iIndex, const ioHashString& szSubscriptionID )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	RandomDecoSelectWnd *pWnd = dynamic_cast<RandomDecoSelectWnd*>(FindChildWnd(ID_DECO_SELECT_WND));
	if( pWnd )
		pWnd->SetSubscription( iIndex, szSubscriptionID );
}

void MyInventoryWnd::ShowDecoSelectWnd( int iPresentIndex, int iPresentSlot )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	RandomDecoSelectWnd *pWnd = dynamic_cast<RandomDecoSelectWnd*>(FindChildWnd(ID_DECO_SELECT_WND));
	if( pWnd )
		pWnd->SetPresent( iPresentIndex, iPresentSlot );
}

void MyInventoryWnd::OnDecoSelectResult()
{
	RandomDecoSelectWnd *pWnd = dynamic_cast<RandomDecoSelectWnd*>(FindChildWnd(ID_DECO_SELECT_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->OnResultState();
}

void MyInventoryWnd::ShowExtraItemBoxWnd( int iMachineCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ioExtraItemRandomBox *pWnd = dynamic_cast<ioExtraItemRandomBox*>(FindChildWnd(ID_EXTRAITEM_BOX_WND));
	if( pWnd )
		pWnd->ShowRandomBox( iMachineCode );
}

void MyInventoryWnd::OnExtraItemBoxResultInfo( int iItemCode, int iPeriodTime, int iReinforce, int iTradeType )
{
	ioExtraItemRandomBox *pWnd = dynamic_cast<ioExtraItemRandomBox*>(FindChildWnd(ID_EXTRAITEM_BOX_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->OnResultInfo( iItemCode, iPeriodTime, iReinforce, iTradeType );
	}
}

void MyInventoryWnd::ShowChangeNameWnd( DWORD dwEtcType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ChangeNameWnd *pWnd = dynamic_cast<ChangeNameWnd*>(FindChildWnd(ID_CHANGE_NAME_WND));
	if( pWnd )
		pWnd->ShowChangeIDWnd( dwEtcType );
}

void MyInventoryWnd::OnChangeNameResultWnd( const ioHashString &szNewName )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ChangeNameWnd *pWnd = dynamic_cast<ChangeNameWnd*>(FindChildWnd(ID_CHANGE_NAME_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ChangeNameResultWnd *pResultWnd = dynamic_cast<ChangeNameResultWnd*>(FindChildWnd(ID_CHANGE_NAME_RESULT_WND));
	if( pResultWnd )
		pResultWnd->SetInfoAndShow( szNewName );
}

void MyInventoryWnd::OnChangeGuildNameResultWnd( const ioHashString &szPreName, const ioHashString &szNewName )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ChangeNameWnd *pWnd = dynamic_cast<ChangeNameWnd*>(FindChildWnd(ID_CHANGE_NAME_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ChangeNameResultWnd *pResultWnd = dynamic_cast<ChangeNameResultWnd*>(FindChildWnd(ID_CHANGE_NAME_RESULT_WND));
	if( pResultWnd )
		pResultWnd->SetInfoAndShow( szPreName, szNewName );
}

void MyInventoryWnd::ShowSoldierExpSelectWnd( DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	InvenSoldierExpBonusWnd *pWnd = dynamic_cast<InvenSoldierExpBonusWnd*>(FindChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_WND));
	if( pWnd )
	{
		pWnd->SetEtcItemType( dwEtcItemType );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::OnSoldierExpSelectResult( int iClassType, DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	InvenSoldierExpBonusWnd *pBuyWnd = dynamic_cast<InvenSoldierExpBonusWnd*>(FindChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();
	
	InvenSoldierExpBonusResultWnd *pWnd = dynamic_cast<InvenSoldierExpBonusResultWnd*>(FindChildWnd(ID_SOLDIER_EXP_BONUS_SELECT_RESULT_WND));
	if( pWnd )
	{
		pWnd->ShowResult( iClassType, dwEtcItemType );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::OnErrorEdit( const char *szError )
{
	ChangeNameWnd *pWnd = dynamic_cast<ChangeNameWnd*>(FindChildWnd(ID_CHANGE_NAME_WND));
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->ErrorEdit( szError );
	}
}

void MyInventoryWnd::OnChangeTradeStateWnd( int iIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ChangeTradeStateResultWnd *pResultWnd = dynamic_cast<ChangeTradeStateResultWnd*>(FindChildWnd(ID_TRADE_STATE_CHANGE_WND));
	if( pResultWnd )
		pResultWnd->SetChangeResultInfo( iIndex );
}

void MyInventoryWnd::ShowRecordInitWnd( DWORD dwEtcType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	RecordInitWnd *pRecordInit = dynamic_cast<RecordInitWnd*>(FindChildWnd(ID_RECORD_INIT_WND));
	if( pRecordInit )
	{
		pRecordInit->ShowRecordInitWnd( dwEtcType );
	}
}

void MyInventoryWnd::OnRecordInitResultWnd( DWORD dwEtcType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	RecordInitWnd *pWnd = dynamic_cast<RecordInitWnd*>(FindChildWnd(ID_RECORD_INIT_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	RecordInitResultWnd *pResultWnd = dynamic_cast<RecordInitResultWnd*>(FindChildWnd(ID_RECORD_INIT_RESULT_WND));
	if( pResultWnd )
		pResultWnd->SetInfoAndShow( dwEtcType );
}

void MyInventoryWnd::ShowItemRecvSellInfoWnd( int iCommandType,
											  int iItemInfoType,
											  const ioHashString szIconName,
											  const ioHashString szSubIconName,
											  const ComplexStringPrinterVec &pTitleVec,
											  const ComplexStringPrinterVec &pDescVec,
											  int iClassType,
											  int iReinforce,
											  bool bExtraItemCustom,
											  bool bIsCostume,
											  int nGradeType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	SpiritFuncWnd *pSpiritWnd = dynamic_cast<SpiritFuncWnd*>(FindChildWnd(ID_SPIRITFUNC_WND));
	if( pSpiritWnd && pSpiritWnd->IsShow() )
		pSpiritWnd->HideWnd();

	HideChildWnd(ID_ACCESSORYFUNC_WND);
	HideChildWnd(ID_EXTRAITEMFUNC_WND);
	ShowChildWnd( ID_FLASH_WND );
	ItemRecvSellInfoWnd *pWnd = dynamic_cast<ItemRecvSellInfoWnd*>(FindChildWnd(ID_ITEM_RECVSELL_INFO_WND));
	if( pWnd )
		pWnd->SetItemRecvSellInfo( iCommandType, iItemInfoType, szIconName, szSubIconName, pTitleVec, pDescVec, iClassType, iReinforce, bExtraItemCustom, bIsCostume, nGradeType );
}

void MyInventoryWnd::ShowSubscriptionRetrCheckLimit()
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
	{
		pInfoWnd->ShowSubscriptionRetrCheckLimit();
	}
}

void MyInventoryWnd::ShowSubscriptionRetrCheck( int iIndex, const ioHashString& szSubscriptionID, int iSubscriptionGold, int iRetraGold )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
	{
		pInfoWnd->ShowSubscriptionRetrCheck( iIndex, szSubscriptionID, iSubscriptionGold, iRetraGold );
	}
}

void MyInventoryWnd::ShowExpandMedalSlotOpenWnd( DWORD dwClassType )
{
	ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> (g_EtcItemMgr.FindEtcItem(dwClassType) );
	if( !pItem )
	{
		LOG.PrintTimeAndLog( 0, "%s ioEtcItem == NULL , Type:%d", __FUNCTION__, dwClassType );
		return;
	}

	int iUseLevel = pItem->GetLimitLevel();
	BYTE iSlotNumber = pItem->GetUseSlotNumber();

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ExtraItemFuncWnd *pExtraInfoWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( pExtraInfoWnd && pExtraInfoWnd->IsShow() )
		pExtraInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ExpandMedalSlotWnd *pWnd = dynamic_cast<ExpandMedalSlotWnd*>(FindChildWnd(ID_EXPAND_MEDAL_SLOT_OPEN_WND));
	if( pWnd )
	{
		pWnd->SetItemInfo( dwClassType );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::OnExpandMedalSlotResultWnd( int iClassType, int iSlotNumber )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ExpandMedalSlotResultWnd *pWnd = dynamic_cast<ExpandMedalSlotResultWnd*> ( FindChildWnd(ID_EXPAND_MEDAL_SLOT_RESULT_WND) );
	if( pWnd )
		pWnd->SetInfo( iClassType, iSlotNumber );
}

void MyInventoryWnd::ShowAlchemicFuncWnd()
{
	// 예외 체크
	GashaponWnd *pGashaponWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pGashaponWnd && pGashaponWnd->IsShow() && pGashaponWnd->IsRolling() )
		return;

	SuperGashaponWnd *pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*>(FindChildWnd(ID_SUPER_GASHAPON_WND));
	if( pSuperGashaponWnd && pSuperGashaponWnd->IsShow() && pSuperGashaponWnd->IsRolling() )
		return;

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd(ID_MULTI_SUPER_GASHAPON_WND));
	if( pMultiSuperGashaponWnd && pMultiSuperGashaponWnd->IsShow() && pMultiSuperGashaponWnd->IsRolling() )
		return;

	RandomDecoWnd *pRandomDecoWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pRandomDecoWnd && pRandomDecoWnd->IsShow() && pRandomDecoWnd->IsRolling() )
		return;

	PetEggResultWnd *pPetEggResultWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pPetEggResultWnd && pPetEggResultWnd->IsShow() && pPetEggResultWnd->IsRolling() )
		return;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd )
		pInfoWnd->HideWnd();

	ItemRecvSellInfoWnd *pSellWnd = dynamic_cast<ItemRecvSellInfoWnd*>(FindChildWnd(ID_ITEM_RECVSELL_INFO_WND));
	if( pSellWnd )
		pSellWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	SpiritFuncWnd *pWnd = dynamic_cast<SpiritFuncWnd*>(FindChildWnd(ID_SPIRITFUNC_WND));
	if( pWnd && !pWnd->IsShow() )
		pWnd->ShowWnd();
}

void MyInventoryWnd::ShowAccessoryFuncWnd()
{
	// 예외 체크
	GashaponWnd *pGashaponWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pGashaponWnd && pGashaponWnd->IsShow() && pGashaponWnd->IsRolling() )
		return;

	SuperGashaponWnd *pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*>(FindChildWnd(ID_SUPER_GASHAPON_WND));
	if( pSuperGashaponWnd && pSuperGashaponWnd->IsShow() && pSuperGashaponWnd->IsRolling() )
		return;

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd(ID_MULTI_SUPER_GASHAPON_WND));
	if( pMultiSuperGashaponWnd && pMultiSuperGashaponWnd->IsShow() && pMultiSuperGashaponWnd->IsRolling() )
		return;

	RandomDecoWnd *pRandomDecoWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pRandomDecoWnd && pRandomDecoWnd->IsShow() && pRandomDecoWnd->IsRolling() )
		return;

	PetEggResultWnd *pPetEggResultWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pPetEggResultWnd && pPetEggResultWnd->IsShow() && pPetEggResultWnd->IsRolling() )
		return;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd )
		pInfoWnd->HideWnd();

	ItemRecvSellInfoWnd *pSellWnd = dynamic_cast<ItemRecvSellInfoWnd*>(FindChildWnd(ID_ITEM_RECVSELL_INFO_WND));
	if( pSellWnd )
		pSellWnd->HideWnd();

	AccessoryFuncWnd *pWnd = dynamic_cast<AccessoryFuncWnd*>(FindChildWnd(ID_ACCESSORYFUNC_WND));
	if( pWnd && !pWnd->IsShow() )
	{
		ShowChildWnd( ID_FLASH_WND );
		pWnd->ShowWnd();
	}
}

void MyInventoryWnd::ShowExtraitemFuncWnd()
{
	// 예외 체크
	GashaponWnd *pGashaponWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pGashaponWnd && pGashaponWnd->IsShow() && pGashaponWnd->IsRolling() )
		return;

	SuperGashaponWnd *pSuperGashaponWnd = dynamic_cast<SuperGashaponWnd*>(FindChildWnd(ID_SUPER_GASHAPON_WND));
	if( pSuperGashaponWnd && pSuperGashaponWnd->IsShow() && pSuperGashaponWnd->IsRolling() )
		return;

	MultiSuperGashaponWnd *pMultiSuperGashaponWnd = dynamic_cast<MultiSuperGashaponWnd*>(FindChildWnd(ID_MULTI_SUPER_GASHAPON_WND));
	if( pMultiSuperGashaponWnd && pMultiSuperGashaponWnd->IsShow() && pMultiSuperGashaponWnd->IsRolling() )
		return;

	RandomDecoWnd *pRandomDecoWnd = dynamic_cast<RandomDecoWnd*>(FindChildWnd(ID_RANDOM_DECO_WND));
	if( pRandomDecoWnd && pRandomDecoWnd->IsShow() && pRandomDecoWnd->IsRolling() )
		return;

	PetEggResultWnd *pPetEggResultWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pPetEggResultWnd && pPetEggResultWnd->IsShow() && pPetEggResultWnd->IsRolling() )
		return;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd )
		pInfoWnd->HideWnd();

	ItemRecvSellInfoWnd *pSellWnd = dynamic_cast<ItemRecvSellInfoWnd*>(FindChildWnd(ID_ITEM_RECVSELL_INFO_WND));
	if( pSellWnd )
		pSellWnd->HideWnd();

	ExtraItemFuncWnd *pWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( pWnd && !pWnd->IsShow() )
	{
		ShowChildWnd( ID_FLASH_WND );
		pWnd->ShowWnd();
	}
}

bool MyInventoryWnd::GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( !COMPARE( dwItemInfoID, ID_ITEM_INFO_WND_1_BTN, ID_ITEM_INFO_WND_8_BTN+1) )
		return false;

	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	int iCurArray   = iStartArray + ( dwItemInfoID - ID_ITEM_INFO_WND_1_BTN );
	int iSize       = m_vInventoryItemInfoList.size();
	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vInventoryItemInfoList[iCurArray];
	return true;
}

bool MyInventoryWnd::GetCurAlchemicItemInfo( OUT AlchemicToolTipInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( COMPARE(dwItemInfoID, ID_SPIRIT_INFO_1, ID_SPIRIT_INFO_20+1) )
	{
		SpiritItemWnd *pItem = dynamic_cast<SpiritItemWnd*>(FindChildWnd(dwItemInfoID));
		if( pItem )
		{
			rkInfo.m_iCode = pItem->GetItemCode();
			rkInfo.m_iType = AlchemicItemToolTip::ATT_ALCHEMIC_ITEM;
			return true;
		}
	}
	else
	{
		ItemMaterialCompoundWnd *pItemMaterialCompoundWnd = dynamic_cast<ItemMaterialCompoundWnd*>(FindChildWnd( ID_ITEM_MATERIAL_COMPOUND_WND ));
		if ( pItemMaterialCompoundWnd && pItemMaterialCompoundWnd->IsShow() )
		{
			if( pItemMaterialCompoundWnd->GetCurAlchemicItemInfo( rkInfo, dwItemInfoID ) )
				return true;
		}
		ExtraItemFuncWnd *pExtraItemFuncWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd( ID_EXTRAITEMFUNC_WND ));
		if ( pExtraItemFuncWnd && pExtraItemFuncWnd->IsShow() )
		{
			if( pExtraItemFuncWnd->GetCurAlchemicItemInfo( rkInfo, dwItemInfoID ) )
				return true;
		}
	}

	return false;
}

void MyInventoryWnd::ShowPreSetPackageItemWnd( DWORD dwType )
{
	ShowMainInfoWnd();

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PreSetSoldierPackageWnd *pWnd = dynamic_cast<PreSetSoldierPackageWnd*>(FindChildWnd(ID_PRESET_PACKAGE_WND));
	if( pWnd )
		pWnd->ShowPackage( dwType );
}

void MyInventoryWnd::OnPreSetPackageItemResult( CHARACTER &rkCharInfo, int iKeepBonusPeso, int iLimitTime )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	PreSetSoldierPackageWnd *pBuyWnd = dynamic_cast<PreSetSoldierPackageWnd*>(FindChildWnd(ID_PRESET_PACKAGE_WND));
	if( pBuyWnd && pBuyWnd->IsShow() )
		pBuyWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PreSetSoldierPackageResultWnd *pResultWnd = dynamic_cast<PreSetSoldierPackageResultWnd*>(FindChildWnd(ID_PRESET_PACKAGE_RESULT_WND));
	if( pResultWnd )
		pResultWnd->SetResultSoldierPackage( rkCharInfo, iKeepBonusPeso, iLimitTime );
}

bool MyInventoryWnd::QuestGuide( const IntVec &rValue, float &rfXPos, float &rfYPos )
{
	if( rValue.size() < 8 ) return false;

	int iMTabID = rValue[0];
	int iSTabID = rValue[1];

	switch( rValue[2] )
	{
	case INVENTORY_ITEM_EXTRAITEM:
	case INVENTORY_ITEM_DECO:
	case INVENTORY_ITEM_ETC:
	case INVENTORY_ITEM_PRESENT:
	case INVENTORY_ITEM_MEDALITEM:
	case INVENTORY_ITEM_COSTUME:
		{
			for(int i = 0;i < MAX_ITEM_INFO_WND_BTN;i++)
			{
				InventoryItemWnd *pItemWnd = dynamic_cast<InventoryItemWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN + i ));
				if( !pItemWnd ) continue;
				if( !pItemWnd->IsShow() ) continue;
				if( pItemWnd->GetMainType() != rValue[2] ) continue;

                if( pItemWnd->QuestGuide( rValue, rfXPos, rfYPos ) )
					return true;
			}
		}
		break;
	default:
		{
			for(int i = 0;i < MAX_CHAR_INFO_WND_BTN;i++)
			{
				InventorySoldierItemWnd *pItemWnd = dynamic_cast<InventorySoldierItemWnd*>(FindChildWnd( ID_CHAR_INFO_WND_1_BTN + i ));
				if( !pItemWnd ) continue;
				if( !pItemWnd->IsShow() ) continue;

				if( pItemWnd->QuestGuide( rValue[3], rfXPos, rfYPos ) )
					return true;
			}
		}
		break;
	}

	if( m_dwCurTabID == (DWORD)iMTabID && m_dwCurSmallTabID == (DWORD)iSTabID ) 
	{
		// 탭 버튼이 정상적으로 선태되어있으면 스크롤 UI에 위치
		ioScroll *pScroll = dynamic_cast<ioScroll*>(FindChildWnd( ID_VERT_SCROLL ));
		if( pScroll )
		{
			rfXPos = pScroll->GetDerivedPosX() + (pScroll->GetWidth() / 2);
			rfYPos = pScroll->GetDerivedPosY();
			return true;
		}
	}
	else if( m_dwCurTabID == (DWORD)iMTabID )	
	{
		ioWnd *pSubTab = FindChildWnd( iSTabID );
		if( pSubTab )
		{
			rfXPos = pSubTab->GetDerivedPosX() + (pSubTab->GetWidth() / 2);
			rfYPos = pSubTab->GetDerivedPosY();
			return true;
		}
	}
	else
	{
		ioWnd *pMainTab = FindChildWnd( iMTabID );
		if( pMainTab )
		{
			rfXPos = pMainTab->GetDerivedPosX() + (pMainTab->GetWidth() / 2);
			rfYPos = pMainTab->GetDerivedPosY();
			return true;
		}
	}
	return false;
}

void MyInventoryWnd::iwm_gototop()
{
	// 용병 선택창이 항상 상위에 위치한다.
	UISoldierSelectWnd *pUISoliderWnd = dynamic_cast<UISoldierSelectWnd *>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ));
	if( pUISoliderWnd && pUISoliderWnd->IsShow() )
		pUISoliderWnd->GoToTop();
}

bool MyInventoryWnd::EnableDecoChangeByEquipItem( CHARACTER &rkCharInfo )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return true;

	for( int i=0; i < 4; ++i )
	{
		EXTRAITEMSLOT kSlot;
		int iExtraItemIndex = rkCharInfo.m_extra_item[i];

		if( pExtraItem->GetExtraItem( iExtraItemIndex, kSlot ) )
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
			if( pItem && pItem->GetCharOnlySexType() != ioSetItemInfo::CST_ALL )
				return false;
		}
	}

	return true;
}
//-------------------------------------------------------------------------
MyInventoryToolTip::MyInventoryToolTip()
{
	m_iMainType    = 0;
	m_iMagicCode   = 0;
	m_iMagicCodeEx = 0;
	m_iMagicCodeEx2= 0;
	m_iManualIndex = 0;

	m_pGrowthLineLeft   = NULL;
	m_pGrowthLineCenter = NULL;
	m_pGrowthLineRight  = NULL;
	m_pGrowthBackGray   = NULL;
	m_pGrowthBackLight  = NULL;

	for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
		m_iCharGrowth[i] = 0;
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
		m_iItemGrowth[i] = 0;
}

MyInventoryToolTip::~MyInventoryToolTip()
{
	ClearDesc();

	SAFEDELETE( m_pGrowthLineLeft );
	SAFEDELETE( m_pGrowthLineCenter );
	SAFEDELETE( m_pGrowthLineRight );
	SAFEDELETE( m_pGrowthBackGray );
	SAFEDELETE( m_pGrowthBackLight );
}

void MyInventoryToolTip::OnProcess( float fTimePerSec )
{
	//팝업이 보이면 이쪽 먼저
	if( g_GUIMgr.IsShow( POPUP_STORE_WND ) )
	{
		OnProcessPopup();
		return;
	}

	// 2020-04-08 툴팁 
	if( !g_GUIMgr.IsShow( MY_INVENTORY_WND ) && !g_GUIMgr.IsShow( SELECT_GASHAPON_SUB_WND ) )
	{
		HideWnd();
		return;
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	InventoryItemInfo kInfo;
	MyInventoryWnd *pInven = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	ioWnd *pInvenInfo = NULL;
	if( pInven )
		pInvenInfo = pInven->FindChildWnd( MyInventoryWnd::ID_INVENTORY_INFO_WND );

	if( COMPARE( pMouse->GetToolTipWndID(), MyInventoryWnd::ID_ITEM_INFO_WND_1_BTN, MyInventoryWnd::ID_ITEM_INFO_WND_8_BTN + 1 ) )
	{
		if( pInven )
			pInven->GetCurItemInfo( kInfo, pMouse->GetToolTipWndID() );
	}
	else if( COMPARE( pMouse->GetToolTipWndID(), MyInventoryInfoWnd::ID_KINDRED_BTN, MyInventoryInfoWnd::ID_UNDERWEAR_BTN + 1 ) )
	{
		if( pInvenInfo )
		{
			InventoryIconBtn *pBtn = dynamic_cast<InventoryIconBtn*> ( pInvenInfo->FindChildWnd( pMouse->GetToolTipWndID() ) );
			if( pBtn )
				pBtn->GetDecoInfo( kInfo.m_iSexType, kInfo.m_iDecoType, kInfo.m_iDecoCode );
			kInfo.m_iMainType = INVENTORY_ITEM_DECO;
		}
	}
	else if( COMPARE( pMouse->GetToolTipWndID(), ExtraItemFuncWnd::ID_WEAPON_ITEM_BTN, ExtraItemFuncWnd::ID_MEDAL_ITEM_6_BTN + 1 ) )
	{
		ioWnd *pExtraItemInfo = pInven->FindChildWnd( MyInventoryWnd::ID_EXTRAITEMFUNC_WND );
		if( pExtraItemInfo )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( pExtraItemInfo->FindChildWnd( pMouse->GetToolTipWndID() ) );
			if( pBtn )
			{
				kInfo.m_iMagicCode   = pBtn->GetMagicCode();
				kInfo.m_iMagicCodeEx = pBtn->GetMagicCodeEx();
				kInfo.m_iMagicCodeEx2= pBtn->GetMagicCodeEx2();
			}

			if( COMPARE( pMouse->GetToolTipWndID(), ExtraItemFuncWnd::ID_WEAPON_ITEM_BTN, ExtraItemFuncWnd::ID_CLOAK_ITEM_BTN + 1 ) )
				kInfo.m_iMainType = INVENTORY_ITEM_EXTRAITEM;
			else
				kInfo.m_iMainType = INVENTORY_ITEM_MEDALITEM;
		}
	}
	// UI코스튬 관련 (툴팁 설정)
	else if( COMPARE( pMouse->GetToolTipWndID(), MyInventoryInfoWnd::ID_WEAPON_COSTUME_BTN, MyInventoryInfoWnd::ID_CLOAK_COSTUME_BTN + 1 ) )
	{
		if( pInvenInfo )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( pInvenInfo->FindChildWnd( pMouse->GetToolTipWndID() ) );
			if( pBtn )
			{
				kInfo.m_iMagicCode   = pBtn->GetMagicCode();
				kInfo.m_iMagicCodeEx = pBtn->GetMagicCodeEx();
				kInfo.m_iMagicCodeEx2= pBtn->GetMagicCodeEx2();
			}

			kInfo.m_iMainType = INVENTORY_ITEM_COSTUME;
		}
	}
	else if( COMPARE( pMouse->GetToolTipWndID(), AccessoryFuncWnd::ID_ACCESSORY_RING_BTN, AccessoryFuncWnd::ID_ACCESSORY_BRACELET_BTN + 1 ) )
	{
		ioWnd *pAccessoryInfo = pInven->FindChildWnd( MyInventoryWnd::ID_ACCESSORYFUNC_WND );
		if( pAccessoryInfo )
		{
			SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( pAccessoryInfo->FindChildWnd( pMouse->GetToolTipWndID() ) );
			if( pBtn )
			{
				kInfo.m_iMagicCode   = pBtn->GetMagicCode();
				kInfo.m_iMagicCodeEx = pBtn->GetMagicCodeEx();
				kInfo.m_iMagicCodeEx2= pBtn->GetMagicCodeEx2();
			}

			kInfo.m_iMainType = INVENTORY_ITEM_ACCESSORY;
		}
	}

	// 2020-04-08 툴팁 ( 소원상자, 메달상자, 장비보급, 축구메달상자, 겨울맞이 메달상자, 설날 메달상자, 신년 메달상자 )
	if( kInfo.m_iMagicCode <= 0 && g_GUIMgr.IsShow( SELECT_GASHAPON_SUB_WND ) )
	{
		if( COMPARE( pMouse->GetToolTipWndID(), SelectGashaponSubWnd::ID_ITEM_INFO_WND_1_BTN, SelectGashaponSubWnd::ID_ITEM_INFO_WND_8_BTN + 1 ) )
		{
			DWORD dwID = pMouse->GetToolTipWndID();
			SelectGashaponSubWnd* pSelectGashaponSubWnd = dynamic_cast<SelectGashaponSubWnd*>( g_GUIMgr.FindWnd( SELECT_GASHAPON_SUB_WND ) );
			if( pSelectGashaponSubWnd )
			{
				SelectGashaponItemInfoWnd *pItem = dynamic_cast<SelectGashaponItemInfoWnd*>( pSelectGashaponSubWnd->FindChildWnd(pMouse->GetToolTipWndID()) );
				if( pItem )
				{
					const GashaponPresent& rkPresent = pItem->GetGashaponItem();

					// 2020-04-13
					if( rkPresent.m_iType == 9 )
					{
						kInfo.m_iMainType	= INVENTORY_ITEM_MEDALITEM;
					}
					//else
					//{
					//	kInfo.m_iMainType	= INVENTORY_ITEM_ETC;
					//}

					kInfo.m_iMagicCode   = rkPresent.m_iValue1;
					kInfo.m_iMagicCodeEx = 0;
					kInfo.m_iMagicCodeEx2= 0;
				}
			}
		}
	}

	if( IsShow() )
	{	
		if( !SetInfo( kInfo ) )                                          
			HideWnd();
		else
		{
			pMouse->SetToolTipWndID( 0 );
			pMouse->SetToolTipCursor();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );

		}

		ProcessDesc();
	}
	else
	{
		if( SetInfo( kInfo ) ) // setinfo 후에 showwnd
		{
			ShowWnd();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
		}
	}
}

void MyInventoryToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { PRINT_XOFFSET = 14, FIRST_YOFFSET = 99, SECOND_YOFFSET = 207,   };
	RenderDesc( iXPos, iYPos );
	RenderGrowth( iXPos, iYPos + FIRST_YOFFSET );
	RenderAccDesc( iXPos, iYPos );

	int iPrintYPos = iYPos + FIRST_YOFFSET;
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM ||
		m_iMainType == INVENTORY_ITEM_MEDALITEM )
		iPrintYPos = iYPos + SECOND_YOFFSET;
	
	int iAccOffset = m_AccDesc.GetLineSize() * LINE_GAP;
	if( m_iMaxAccComDesc != 0 )
		iAccOffset += m_iMaxAccComDesc * LINE_GAP + ACC_COM_GAP;

	int iPrintManualYPos = iPrintYPos + iAccOffset;
	PrintManual( iXPos+PRINT_XOFFSET, iPrintManualYPos, FONT_SIZE_12 );
}

void MyInventoryToolTip::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}

	m_AccDesc.Release();

	m_iMaxAccComDesc = 0;
	for (int i = 0; i < MAX_ACC_COM_DESC ; i++)
	{
		m_szAccComDescArray[i].ClearList();
	}
}

void MyInventoryToolTip::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET       = 90,
		MAX_TYPE       = 2,
	};

	iXPos += X_OFFSET;

	int iYPosArray[MAX_TYPE][MAX_DESC]={ 21, 41, 59, 
		32, 53, 0, };

	int iDescCnt = 0;
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;
		iDescCnt++;
	}

	int iType = 0;
	if( iDescCnt != MAX_DESC )
		iType = 1;

	int iCnt  = 0;
	for( int i = 0 ; i < MAX_DESC ; i++ )
	{
		if( m_szDescArray[i].IsEmpty() )
			continue;
		if( COMPARE( iType, 0, MAX_TYPE ) && COMPARE( iCnt, 0, MAX_DESC ) )
			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + iYPosArray[iType][iCnt] ,TAT_LEFT, INVEN_TOOLTIP_WIDTHCUT_SIZE );
		iCnt++;
	}
}

void MyInventoryToolTip::RenderAccDesc( int iXPos, int iYPos )
{
	if( m_AccDesc.GetLineSize() == 0 && m_iMaxAccComDesc == 0 )
		return;

	enum { XOFFSET = 14, YOFFSET = 99, INVEN_TOOLTIP_ACC_WIDTHCUT_SIZE = 210, };

	m_AccDesc.OnRender( iXPos + XOFFSET, iYPos + YOFFSET );

	for( int i = 0; i < MAX_ACC_COM_DESC; i++ )
	{
		if( m_szAccComDescArray[i].IsEmpty() )
			continue;

		m_szAccComDescArray[i].PrintFullTextWidthCut( iXPos + XOFFSET, iYPos + YOFFSET + ACC_COM_GAP + LINE_GAP * (m_AccDesc.GetLineSize() + i), TAT_LEFT, INVEN_TOOLTIP_ACC_WIDTHCUT_SIZE );
	}
}

void MyInventoryToolTip::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	ManualMgr *pManual = NULL;
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM )	
		pManual = ioExtraInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_DECO )
		pManual = ioDecoInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_ETC )
		pManual = ioEtcInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_PRESENT )
		pManual = ioPresentInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_MEDALITEM )
		pManual = ioMedalInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_COSTUME )
		pManual = ioCostumeInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == POPUPSTORE_ITEM_PRESENT )
		pManual = ioPopupStorePresentItemManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_ACCESSORY )
		pManual = ioAccessoryInvenManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;


	ioComplexStringPrinter kPrinter;
	int iMaxLine = pManual->GetMaxLine( m_iManualIndex );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		if( !bAutoLine )
		{
			pManual->SetPrinter( m_iManualIndex, iLine, fScale, kPrinter);
			a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
			kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
			kPrinter.ClearList();
		}
		else
		{
			pManual->SetPrinter( m_iManualIndex, iLine, fScale, kPrinter);
			enum { MAX_LOOP = 20, };
			bool bFirstGap = true;
			for (int i = 0; i < MAX_LOOP ; i++)
			{
				if( kPrinter.GetFullWidth() < DESC_WIDTH )
				{
					if( bFirstGap )
					{
						a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
						bFirstGap = false;
					}
					else
						a_iYPos += LINE_GAP;
					kPrinter.PrintFullText( a_iXPos, a_iYPos, TAT_LEFT );
					kPrinter.ClearList();
					break;
				}

				ioComplexStringPrinter kPrinterA, kPrinterB;
				kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
				if( bFirstGap )
				{
					a_iYPos += pManual->GetSpaceLine( m_iManualIndex, iLine);
					bFirstGap = false;
				}
				else
					a_iYPos += LINE_GAP;
				kPrinterA.PrintFullText( a_iXPos, a_iYPos, TAT_LEFT );
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}
	}	
}

bool MyInventoryToolTip::SetInfo( InventoryItemInfo &rkInfo )
{
	Clear();

	MToolTipItemInfo kToolTiopItemInfo;
	GetItemInfo( rkInfo, kToolTiopItemInfo );

	if( kToolTiopItemInfo.m_sName.IsEmpty() || kToolTiopItemInfo.m_sIcon.IsEmpty() )
	{
		Clear();
		return false;
	}

	// desc
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	if( !kToolTiopItemInfo.m_sNameAdd.IsEmpty() )
	{
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_BLUE ); // 진회색
		m_szDescArray[0].AddTextPiece( FONT_SIZE_17, kToolTiopItemInfo.m_sNameAdd.c_str() );
	}
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, kToolTiopItemInfo.m_sName.c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0);	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY ); // 연회색
	if( kToolTiopItemInfo.m_iLimitLevel > 0 )
	{
		char szText[MAX_PATH]="";
		SafeSprintf( szText, sizeof( szText ),  STR(1), kToolTiopItemInfo.m_iLimitLevel );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText );
	}

	// icon btn
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		Clear();
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL", __FUNCTION__ );
		return false;
	}

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kToolTiopItemInfo.m_sIcon );
	ioUIRenderImage *pSubIcon = g_UIImageSetMgr.CreateImageByFullName( kToolTiopItemInfo.m_sSubIcon );
	if( !pBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kToolTiopItemInfo.m_iReinForce ) )
	{
		Clear();
		return false;
	}
	pBtn->SetSubIcon( pSubIcon );
	pBtn->SetGradeType( kToolTiopItemInfo.m_nGradeType );

	if ( rkInfo.m_iMainType == INVENTORY_ITEM_COSTUME )
		pBtn->SetCostume( true );

	m_iMainType    = rkInfo.m_iMainType;
	m_iMagicCode   = rkInfo.m_iMagicCode;
	m_iMagicCodeEx = rkInfo.m_iMagicCodeEx;
	m_iMagicCodeEx2= rkInfo.m_iMagicCodeEx2;
	m_iManualIndex = kToolTiopItemInfo.m_iManualIndex;
	for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
		m_iCharGrowth[i] = kToolTiopItemInfo.m_iCharGrowth[i];
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
		m_iItemGrowth[i] = kToolTiopItemInfo.m_iItemGrowth[i];

	return true;
}
bool MyInventoryToolTip::SetInfoPopup( MToolTipItemInfo &kToolTiopItemInfo )
{
	Clear();

	if( kToolTiopItemInfo.m_sName.IsEmpty() || kToolTiopItemInfo.m_sIcon.IsEmpty() )
	{
		Clear();
		return false;
	}

	m_iMainType = POPUPSTORE_ITEM_PRESENT;

	// desc
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색
	m_szDescArray[0].AddTextPiece( FONT_SIZE_17, kToolTiopItemInfo.m_sName.c_str() );

	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0);	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY ); // 연회색
	if( kToolTiopItemInfo.m_iLimitLevel > 0 )
	{
		char szText[MAX_PATH]="";
		SafeSprintf( szText, sizeof( szText ),  STR(1), kToolTiopItemInfo.m_iLimitLevel );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, szText );
	}

	// icon btn
	InventoryIconBtn *pBtn = dynamic_cast< InventoryIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		Clear();
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL", __FUNCTION__ );
		return false;
	}

	ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kToolTiopItemInfo.m_sIcon );
	ioUIRenderImage *pSubIcon = g_UIImageSetMgr.CreateImageByFullName( kToolTiopItemInfo.m_sSubIcon );
	if( !pBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kToolTiopItemInfo.m_iReinForce ) )
	{
		Clear();
		return false;
	}
	pBtn->SetSubIcon( pSubIcon );

	m_iManualIndex = kToolTiopItemInfo.m_iManualIndex;
	for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
		m_iCharGrowth[i] = kToolTiopItemInfo.m_iCharGrowth[i];
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
		m_iItemGrowth[i] = kToolTiopItemInfo.m_iItemGrowth[i];

	return true;
}

void MyInventoryToolTip::GetItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	rkReturnInfo.Clear();

	if( rkInfo.m_iMainType == INVENTORY_ITEM_EXTRAITEM )
		GetExtraItemInfo( rkInfo, rkReturnInfo );
	else if( rkInfo.m_iMainType == INVENTORY_ITEM_DECO )
		GetDecoItemInfo( rkInfo, rkReturnInfo );
	else if( rkInfo.m_iMainType == INVENTORY_ITEM_ETC )
		GetEtcItemInfo( rkInfo, rkReturnInfo );
	else if( rkInfo.m_iMainType == INVENTORY_ITEM_PRESENT )
		GetPresentItemInfo( rkInfo, rkReturnInfo );
	else if( rkInfo.m_iMainType == INVENTORY_ITEM_MEDALITEM )
		GetMedalItemInfo( rkInfo, rkReturnInfo );
	// UI코스튬 관련 (코스튬 정보 얻기)
	if( rkInfo.m_iMainType == INVENTORY_ITEM_COSTUME )
		GetCostumeInfo( rkInfo, rkReturnInfo );
	if( rkInfo.m_iMainType == INVENTORY_ITEM_ACCESSORY )
		GetAccessoryInfo( rkInfo, rkReturnInfo );
}

void MyInventoryToolTip::GetExtraItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{	
	int iSlotIndex = rkInfo.m_iMagicCode;
	if( iSlotIndex <= 0 )
	{
		// 대여 장비
		GetExtraItemInfoByItemCode( rkInfo.m_iMagicCodeEx, rkInfo.m_iMagicCodeEx2, rkReturnInfo );
	}
	else
	{
		EXTRAITEMSLOT kSlot;
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
			return;

		if( !pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
			return;

		GetExtraItemInfoByItemCode( kSlot.m_iItemCode, kSlot.m_iReinforce, rkReturnInfo );
	}
}

void MyInventoryToolTip::GetExtraItemInfoByItemCode( IN int iItemCode , IN int iReinforce, OUT MToolTipItemInfo &rkReturnInfo )
{
	const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
	if( !pItem )
		return;

	rkReturnInfo.m_sName = pItem->GetName();
	rkReturnInfo.m_sIcon = pItem->GetItemLargeIconName();
	rkReturnInfo.m_iLimitLevel = iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	rkReturnInfo.m_iReinForce  = iReinforce;
	rkReturnInfo.m_iManualIndex = pItem->GetManual() - 1; // index -1
	rkReturnInfo.m_nGradeType = pItem->GetGradeType();

	int iArray = pItem->GetType()-1;
	if( COMPARE( iArray, 0, MAX_CHAR_GROWTH ) )
	{
		if ( iReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
			rkReturnInfo.m_iCharGrowth[iArray] = iReinforce;
		else
			rkReturnInfo.m_iCharGrowth[iArray] = g_GrowthInfoMgr.GetReinforceAddStat( iReinforce );
	}
	if( COMPARE( iArray, 0, MAX_ITEM_GROWTH ) )
	{
		if ( iReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
			rkReturnInfo.m_iItemGrowth[iArray] = iReinforce;
		else
			rkReturnInfo.m_iItemGrowth[iArray] = g_GrowthInfoMgr.GetReinforceAddStat( iReinforce );
	}
}

void MyInventoryToolTip::GetDecoItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( rkInfo.m_iSexType, rkInfo.m_iDecoType, rkInfo.m_iDecoCode );
	if( !pData ) 
		return;

	rkReturnInfo.m_sName = pData->m_szName;
	rkReturnInfo.m_sIcon = pData->m_szIcon;
	rkReturnInfo.m_iLimitLevel  = pData->m_iLimitLevel;
	rkReturnInfo.m_iManualIndex = pData->m_iManual - 1; // index -1
}

void MyInventoryToolTip::GetEtcItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	int iItemType = rkInfo.m_iMagicCode;
	if( iItemType <= 0 )
		return;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iItemType );
	if( !pEtcItem ) 
		return;

	rkReturnInfo.m_sName = pEtcItem->GetName();
	rkReturnInfo.m_sIcon = pEtcItem->GetIconName();
	rkReturnInfo.m_sSubIcon = pEtcItem->GetSubIconName();
	rkReturnInfo.m_iManualIndex = pEtcItem->GetInventoryManual() - 1; // index -1
	rkReturnInfo.m_nGradeType = pEtcItem->GetGradeType();
}

void MyInventoryToolTip::GetMedalItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	int iMedalItemType = rkInfo.m_iMagicCode;
	int iCustomIndex = rkInfo.m_iMagicCodeEx;
	if( iMedalItemType <= 0 )
		return;

	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iMedalItemType );
	if( !pItemInfo )
		return;

	rkReturnInfo.m_sName = pItemInfo->m_sName;
	rkReturnInfo.m_sIcon = pItemInfo->m_sIcon;
	rkReturnInfo.m_sSubIcon = pItemInfo->m_sSubIcon;
	rkReturnInfo.m_iLimitLevel  = g_MedalItemMgr.GetLevelLimit( iMedalItemType );
	rkReturnInfo.m_iManualIndex = pItemInfo->m_iManual - 1; // index -1

	if( pItemInfo->m_iSubMedalType == ioMedalItemInfoManager::MEDALTYPE_NORMAL )
	{
		for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
			rkReturnInfo.m_iCharGrowth[i] = pItemInfo->m_iCharGrowth[i];
		for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
			rkReturnInfo.m_iItemGrowth[i] = pItemInfo->m_iItemGrowth[i];
	}
	else
	{
		ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
		ioUserMedalItem::MEDALITEMSLOT kSlot;
		if( pMedalItem && pMedalItem->GetMedalItem( iMedalItemType, iCustomIndex, kSlot ) )
		{
			for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
				rkReturnInfo.m_iCharGrowth[i] = kSlot.m_iStat[i];
			for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
				rkReturnInfo.m_iItemGrowth[i] = kSlot.m_iStat[MAX_CHAR_GROWTH + i];
		}
	}
}

void MyInventoryToolTip::GetPresentItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	ioHashString szSendID;
	short sPresentType = 0;
	short sPresentMent = 0;
	int iPresentValue1 = 0;
	int iPresentValue2 = 0;
	int iPresentValue3 = 0;
	int iPresentValue4 = 0;
	DWORD dwLimitDate  = 0;

	g_PresentMgr.GetPresentDataToIndex( rkInfo.m_iMagicCode, rkInfo.m_iMagicCodeEx, szSendID, sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );

	if( sPresentType ==  PRESENT_SOLDIER )
	{
		int iClassType      = iPresentValue1;
		DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
		const ioSetItemInfo *pSetItemInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
		if( !pSetItemInfo )
			return;

		rkReturnInfo.m_sName = pSetItemInfo->GetName();
		rkReturnInfo.m_sIcon = g_MyInfo.GetMySoldierIconName( iClassType );
		rkReturnInfo.m_sSubIcon = g_MyInfo.GetSoldierSubIconName( iClassType );
		rkReturnInfo.m_iLimitLevel  = pSetItemInfo->GetNeedLevel( 0 );        // 0번째 array 정보 ;
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );
	}
	else if( sPresentType == PRESENT_DECORATION ||
		     sPresentType == PRESENT_RANDOM_DECO )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_DECO;
		kInfo.m_iSexType  = (iPresentValue1 % 100000) / 1000;
		kInfo.m_iDecoType = iPresentValue1 % 1000;
		kInfo.m_iDecoCode = iPresentValue2;
		GetDecoItemInfo( kInfo, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_ETC_ITEM )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_ETC;
		kInfo.m_iMagicCode= iPresentValue1;
		GetEtcItemInfo( kInfo, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}	
	else if( sPresentType == PRESENT_PESO )
	{
		rkReturnInfo.m_sName        = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon        = "UIIconPack13#quest_002";
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_EXTRAITEM )
	{	
		GetExtraItemInfoByItemCode( iPresentValue1, (iPresentValue2%PRESENT_EXTRAITEM_DIVISION_1)/PRESENT_EXTRAITEM_DIVISION_2, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
		if( !pInfo )
			return;

		rkReturnInfo.m_sName = pInfo->GetName();
		rkReturnInfo.m_sIcon = pInfo->GetIconName();
		rkReturnInfo.m_iLimitLevel  = pInfo->GetNeedLevel();
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = pInfo->GetGradeType();
	}
	else if( sPresentType == PRESENT_GRADE_EXP )
	{
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = "UIIconPack13#quest_001";
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_MEDALITEM )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_MEDALITEM;
		kInfo.m_iMagicCode= iPresentValue1;
		GetMedalItemInfo( kInfo, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_PET )
	{
		int nPetRank = iPresentValue2%10000;
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = g_PetInfoMgr.GetPetIcon( iPresentValue1, (PetRankType)nPetRank );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_COSTUME )
	{
		// UI코스튬 관련 (선물 툴팁 설정)
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = g_CostumeInfoMgr.GetCostumeIcon( iPresentValue1 );
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = g_CostumeInfoMgr.GetGradeType( iPresentValue1 );
	}
	else if( sPresentType == PRESENT_COSTUME_BOX )
	{
		CostumeShopInfo *pInfo = g_CostumeShopInfoMgr.GetCostumeMachineInfo( iPresentValue1 );
		if( !pInfo )
			return;

		rkReturnInfo.m_sName = pInfo->GetName();
		rkReturnInfo.m_sIcon = pInfo->GetIconName();
		rkReturnInfo.m_iLimitLevel  = pInfo->GetNeedLevel();
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = pInfo->GetGradeType();
	}
	else if ( sPresentType == PRESENT_ACCESSORY )
	{
		const ioItem *pItem = g_ItemMaker.GetItemConst( iPresentValue1, __FUNCTION__ );
		if( !pItem )
			return;

		rkReturnInfo.m_sName = pItem->GetName();
		rkReturnInfo.m_sIcon = pItem->GetItemLargeIconName();
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = pItem->GetGradeType();
	}
	else if( sPresentType == PRESENT_SPIRIT )
	{
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = g_SpiritMgr.GetIconName( iPresentValue1 );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
}

void MyInventoryToolTip::GetCostumeInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{	
	int iSlotIndex = rkInfo.m_iMagicCode;

	CostumeSlot kSlot;
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if( !pCostume )
		return;

	if( !pCostume->GetCostumeSlot( iSlotIndex, kSlot ) )
		return;

	CostumeInfo sInfo;
	if( !g_CostumeInfoMgr.GetCostumeInfo( kSlot.m_nCostumeCode, sInfo ) )
		return;

	rkReturnInfo.m_sName = sInfo.m_szName;
	rkReturnInfo.m_sIcon = sInfo.m_szIconName;
	rkReturnInfo.m_iManualIndex = sInfo.m_nManual-1;
	rkReturnInfo.m_nGradeType = sInfo.m_nGradeType;
}

void MyInventoryToolTip::GetAccessoryInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	int iSlotIndex = rkInfo.m_iMagicCode;

	AccessorySlot kSlot;
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
		return;

	if( !pAccessory->GetAccessorySlot( iSlotIndex, kSlot ) )
		return;
	const ioItem* pItem = g_ItemMaker.GetItemConst( kSlot.m_iAccessoryCode, __FUNCTION__ );
	if ( !pItem )
		return;
	int iSlot = ES_RING + kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE - 1;

	char szTooltip[MAX_PATH] = "";
	float fRanNum = kSlot.m_iRandomNumber/FLOAT1000;
	if ( iSlot == ES_RING )
	{
		const ioRingItem* pRing = ToRingItemConst( pItem );
		if ( !pRing )
			return;
		rkReturnInfo.m_iManualIndex = pRing->GetManualIndex();
		sprintf( szTooltip, pRing->GetToolTipText().c_str(), fRanNum );
	}
	else if ( iSlot == ES_NECKLACE )
	{
		const ioNecklaceItem* pNeck = ToNecklaceItemConst( pItem );
		if ( !pNeck )
			return;
		rkReturnInfo.m_iManualIndex = pNeck->GetManualIndex();
		sprintf( szTooltip, pNeck->GetToolTipText().c_str(), fRanNum );
	}
	else if ( iSlot == ES_BRACELET )
	{
		const ioBraceletItem* pBracelet = ToBraceletItemConst( pItem );
		if ( !pBracelet )
			return;
		rkReturnInfo.m_iManualIndex = pBracelet->GetManualIndex();
		sprintf( szTooltip, pBracelet->GetToolTipText().c_str(), fRanNum );
	}
	m_AccDesc.Set(0, 0, szTooltip, FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);

	AccessoryCompoundInfo cInfo = g_AccessoryInfoMgr.FindCompoundInfo( kSlot.m_iCompoundCode );
	if( cInfo.m_iCompoundCode != 0 )
	{
		m_iMaxAccComDesc = cInfo.m_vToolTipTextList.size();
		if ( !COMPARE( m_iMaxAccComDesc, 0, MAX_ACC_COM_DESC + 1 ) )
			return;

		float fRanNum = kSlot.m_iCompoundValue / FLOAT1000;
		for ( int i=0 ; i < m_iMaxAccComDesc ; ++i )
		{
			m_szAccComDescArray[i].ClearList();
			m_szAccComDescArray[i].SetTextStyle( TS_NORMAL );

			float fFontSize = FONT_SIZE_12;
			DWORD dwTextColor = TCT_DEFAULT_DARKGRAY;
			if( i == 0 )
			{
				fFontSize = FONT_SIZE_13;
				dwTextColor = TCT_DEFAULT_BLUE;
			}			
			m_szAccComDescArray[i].SetTextColor( dwTextColor );

			if ( i == 1 )
				m_szAccComDescArray[i].AddTextPiece( fFontSize, cInfo.m_vToolTipTextList[i].c_str(), fRanNum );
			else
				m_szAccComDescArray[i].AddTextPiece( fFontSize, cInfo.m_vToolTipTextList[i].c_str() );
		}
	}

	rkReturnInfo.m_sName = kSlot.m_Name;
	rkReturnInfo.m_sNameAdd = kSlot.GetNameAdd();
	rkReturnInfo.m_sIcon = kSlot.m_IconName;
	rkReturnInfo.m_nGradeType = pItem->GetGradeType();
}

DWORD MyInventoryToolTip::GetUseInfo( OUT ioHashString &rsInfo )
{
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM )
	{
		if( m_iMagicCode == 0 )
			return TCT_DEFAULT_RED;
		else if( m_iMagicCode < 0 )
		{
			rsInfo = STR(3);
			return TCT_DEFAULT_BLUE;
		}
		else
		{
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( !pExtraItem )
			{
				LOG.PrintTimeAndLog( 0, "%s UserExtraItem == NULL", __FUNCTION__ );
				return TCT_DEFAULT_RED;
			}

			EXTRAITEMSLOT kSlot;
			if( !pExtraItem->GetExtraItem( m_iMagicCode, kSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "%s No ExtraItem : %d", __FUNCTION__, m_iMagicCode );
				return TCT_DEFAULT_RED;
			}

			pExtraItem->GetUseInfo( m_iMagicCode, rsInfo );

			if( kSlot.m_PeriodType == EPT_MORTMAIN )
				return TCT_DEFAULT_BLUE;
		}
		return TCT_DEFAULT_RED;
	}
	else if( m_iMainType == INVENTORY_ITEM_DECO )
	{
		rsInfo = STR(1);
		return TCT_DEFAULT_BLUE;
	}
	else if( m_iMainType == INVENTORY_ITEM_ETC )
	{
		ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			LOG.PrintTimeAndLog( 0, "%s UserEtcItem == NULL , Type:%d", __FUNCTION__, m_iMagicCode );
			return TCT_DEFAULT_RED;
		}

		// 1번만 사용
		DWORD sInfo1Color;
		DWORD sInfo2Color;
		ioHashString sInfo2;
		pUserEtcItem->GetUseInfo( m_iMagicCode, rsInfo, sInfo2, sInfo1Color, sInfo2Color );
	}
	else if( m_iMainType == INVENTORY_ITEM_PRESENT )
	{
		ioHashString szSendID;
		short sPresentType = 0;
		short sPresentMent = 0;
		int iPresentValue1 = 0;
		int iPresentValue2 = 0;
		int iPresentValue3 = 0;
		int iPresentValue4 = 0;
		DWORD dwLimitDate  = 0;

		g_PresentMgr.GetPresentDataToIndex( m_iMagicCode, m_iMagicCodeEx, szSendID,	sPresentType, sPresentMent, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, dwLimitDate );
		rsInfo = g_PresentMgr.GetLmitDateText( dwLimitDate );
	}
	else if( m_iMainType == INVENTORY_ITEM_MEDALITEM )
	{
		ioUserMedalItem::MEDALITEMSLOT kSlot;
		ioUserMedalItem *pMedalItem = g_MyInfo.GetUserMedalItem();
		if( !pMedalItem )
		{
			LOG.PrintTimeAndLog( 0, "%s UserMedalItem == NULL", __FUNCTION__ );
			return TCT_DEFAULT_RED;
		}

		pMedalItem->GetUseInfo( m_iMagicCode, m_iMagicCodeEx, rsInfo );
		char szTemp[MAX_PATH]="";
		SafeSprintf( szTemp, sizeof( szTemp ), STR(2), rsInfo.c_str() );
		rsInfo = szTemp;
		return TCT_DEFAULT_BLUE;
	}
	else if( m_iMainType == INVENTORY_ITEM_COSTUME )
	{
		// UI코스튬 (툴팁 정보 얻기)
		if( m_iMagicCode == 0 )
			return TCT_DEFAULT_RED;
		else if( m_iMagicCode < 0 )
		{
			rsInfo = STR(3);
			return TCT_DEFAULT_BLUE;
		}
		else
		{
			ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
			if( !pCostume )
			{
				LOG.PrintTimeAndLog( 0, "%s UserCostume == NULL", __FUNCTION__ );
				return TCT_DEFAULT_RED;
			}

			CostumeSlot kSlot;
			if( !pCostume->GetCostumeSlot( m_iMagicCode, kSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "%s No Costume : %d", __FUNCTION__, m_iMagicCode );
				return TCT_DEFAULT_RED;
			}

			pCostume->GetUseInfo( m_iMagicCode, rsInfo );

			if( kSlot.m_PeriodType == PCPT_MORTMAIN )
				return TCT_DEFAULT_BLUE;
		}
		return TCT_DEFAULT_RED;
	}
	else if( m_iMainType == INVENTORY_ITEM_ACCESSORY )
	{
		if( m_iMagicCode == 0 )
			return TCT_DEFAULT_RED;
		else if( m_iMagicCode < 0 )
		{
			rsInfo = STR(3);
			return TCT_DEFAULT_BLUE;
		}
		else
		{
			ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
			if( !pAccessory )
			{
				LOG.PrintTimeAndLog( 0, "%s UserAccessory == NULL", __FUNCTION__ );
				return TCT_DEFAULT_RED;
			}

			AccessorySlot kSlot;
			if( !pAccessory->GetAccessorySlot( m_iMagicCode, kSlot ) )
			{
				LOG.PrintTimeAndLog( 0, "%s No Accessory : %d", __FUNCTION__, m_iMagicCode );
				return TCT_DEFAULT_RED;
			}

			pAccessory->GetUseInfo( m_iMagicCode, rsInfo );

			if( kSlot.m_PeriodType == ACCPT_MORTMAIN )
				return TCT_DEFAULT_BLUE;
		}
		return TCT_DEFAULT_RED;
	}

	return TCT_DEFAULT_RED;
}

void MyInventoryToolTip::ProcessDesc()
{
	m_szDescArray[2].ClearList();

	ioHashString sInfo;
	DWORD dwColor = GetUseInfo( sInfo );
	if( sInfo.IsEmpty() )
		return;

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( dwColor );
	m_szDescArray[2].AddTextPiece( FONT_SIZE_13, sInfo.c_str() );
}

void MyInventoryToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "GrowthLineLeft" )
	{
		SAFEDELETE( m_pGrowthLineLeft );
		m_pGrowthLineLeft = pImage;
	}
	else if( szType == "GrowthLineCenter" )
	{
		SAFEDELETE( m_pGrowthLineCenter );
		m_pGrowthLineCenter = pImage;
	}
	else if( szType == "GrowthLineRight" )
	{
		SAFEDELETE( m_pGrowthLineRight );
		m_pGrowthLineRight = pImage;
		if( m_pGrowthLineRight )
			m_pGrowthLineRight->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
	}
	else if( szType == "GrowthBackGray" )
	{
		SAFEDELETE( m_pGrowthBackGray );
		m_pGrowthBackGray = pImage;
	}
	else if( szType == "GrowthBackLight" )
	{
		SAFEDELETE( m_pGrowthBackLight );
		m_pGrowthBackLight = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void MyInventoryToolTip::RenderGrowth( int iXPos, int iYPos )
{
	if( m_iMainType != INVENTORY_ITEM_EXTRAITEM && 
		m_iMainType != INVENTORY_ITEM_MEDALITEM )
		return;

	if( !m_pGrowthLineLeft  ||
		!m_pGrowthLineCenter||
		!m_pGrowthLineRight ||
		!m_pGrowthBackGray  ||
		!m_pGrowthBackLight )
		return;

	enum 
	{
		GROWTH_XOFFSET       = 116,
		GROWTH_YOFFSET       = -4,

		LEFT_XOFFSET         = 14, 
		CENTER_LEFT_XOFFSET  = 21,
		CENTER_RIGHT_XOFFSET = 136,
		RIGHT_XOFFSET        = 212,

		CENTER_WIDTH         = 76,
		CENTER_HEIGHT        = 10,

		LEFT_BACK_XOFFSET    = 14,
		RIGHT_BACK_XOFFSET   = 117,
		BACK_YOFFSET         = 12,
		YGAP                 = 21,

		BACK_WIDTH           = 102,
		BACK_HEIGHT          = 21,

		MAX_BACK             = 4,

		LEFT_TEXT_XOFFSET    = 19,
		RIGHT_TEXT_XOFFSET   = 122,
		TEXT_YOFFSET         = 16,
	};

	// |------- 능력치 --------|
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + GROWTH_XOFFSET, iYPos + GROWTH_YOFFSET, FONT_SIZE_11,  STR(1) );

	m_pGrowthLineLeft->Render( iXPos + LEFT_XOFFSET, iYPos );
	// left center
	m_pGrowthLineCenter->SetSize( CENTER_WIDTH, CENTER_HEIGHT );
	m_pGrowthLineCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
	m_pGrowthLineCenter->Render( iXPos + CENTER_LEFT_XOFFSET, iYPos );

	// right center
	m_pGrowthLineCenter->Render( iXPos + CENTER_RIGHT_XOFFSET, iYPos );
	m_pGrowthLineRight->Render( iXPos + RIGHT_XOFFSET, iYPos );

	// back
	POINT ptGrayOffset[MAX_BACK]={ LEFT_BACK_XOFFSET, BACK_YOFFSET, RIGHT_BACK_XOFFSET, BACK_YOFFSET, 
		LEFT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*2), RIGHT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*2) };
	POINT ptLightOffset[MAX_BACK]={ LEFT_BACK_XOFFSET, BACK_YOFFSET+YGAP, RIGHT_BACK_XOFFSET, BACK_YOFFSET+YGAP, 
		LEFT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*3), RIGHT_BACK_XOFFSET, BACK_YOFFSET+(YGAP*3) };

	for (int i = 0; i < MAX_BACK ; i++)
	{
		m_pGrowthBackGray->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGrowthBackGray->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGrowthBackGray->SetSize( BACK_WIDTH, BACK_HEIGHT );
		m_pGrowthBackGray->Render( iXPos + ptGrayOffset[i].x, iYPos + ptGrayOffset[i].y, UI_RENDER_MULTIPLY );

		m_pGrowthBackLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pGrowthBackLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pGrowthBackLight->SetSize( BACK_WIDTH, BACK_HEIGHT );
		m_pGrowthBackLight->Render( iXPos + ptLightOffset[i].x, iYPos + ptLightOffset[i].y, UI_RENDER_MULTIPLY );
	}

	// text
	char szCharGrowthArray[MAX_CHAR_GROWTH][MAX_PATH]={ "", "", "", "" };
	for (int i = 0; i < MAX_CHAR_GROWTH ; i++) // 스트링 추출을 위해서 loop을 돌지 않고 처리
	{
		if( i == 0 )
			SafeSprintf( szCharGrowthArray[i] , sizeof( szCharGrowthArray[i]), STR(2) );
		else if( i == 1 )
			SafeSprintf( szCharGrowthArray[i] , sizeof( szCharGrowthArray[i]), STR(3) );
		else if( i == 2 )
			SafeSprintf( szCharGrowthArray[i] , sizeof( szCharGrowthArray[i]), STR(4) );
		else if( i == 3 )
			SafeSprintf( szCharGrowthArray[i] , sizeof( szCharGrowthArray[i]), STR(5) );
	}

	for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL);
		kPrinter.SetBkColor( 0, 0, 0 );	
		if( m_iCharGrowth[i] == 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, szCharGrowthArray[i] );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, szCharGrowthArray[i] );
			kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
			if( m_iCharGrowth[i] > 0 )
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(6), m_iCharGrowth[i] );
			else
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(7), m_iCharGrowth[i] );
		}
		kPrinter.PrintFullText( iXPos+RIGHT_TEXT_XOFFSET, iYPos+TEXT_YOFFSET+(YGAP*i), TAT_LEFT );	
	}

	char szItemGrowthArray[MAX_ITEM_GROWTH][MAX_PATH]={ "", "", "", "" };
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++) // 스트링 추출을 위해서 loop을 돌지 않고 처리
	{
		if( i == 0 )
			SafeSprintf( szItemGrowthArray[i] , sizeof( szItemGrowthArray[i]), STR(8) );
		else if( i == 1 )
			SafeSprintf( szItemGrowthArray[i] , sizeof( szItemGrowthArray[i]), STR(9) );
		else if( i == 2 )
			SafeSprintf( szItemGrowthArray[i] , sizeof( szItemGrowthArray[i]), STR(10) );
		else if( i == 3 )
			SafeSprintf( szItemGrowthArray[i] , sizeof( szItemGrowthArray[i]), STR(11) );
	}
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_NORMAL);
		kPrinter.SetBkColor( 0, 0, 0 );	
		if( m_iItemGrowth[i] == 0 )
		{
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, szItemGrowthArray[i] );
		}
		else
		{
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, szItemGrowthArray[i] );
			kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
			if( m_iItemGrowth[i] > 0 )
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(12), m_iItemGrowth[i] );
			else
				kPrinter.AddTextPiece( FONT_SIZE_12, STR(13), m_iItemGrowth[i] );
		}
		kPrinter.PrintFullText( iXPos+LEFT_TEXT_XOFFSET, iYPos+TEXT_YOFFSET+(YGAP*i), TAT_LEFT );	
	}
}

void MyInventoryToolTip::Clear()
{
	for (int i = 0; i < MAX_CHAR_GROWTH ; i++)
		m_iCharGrowth[i] = 0;
	for (int i = 0; i < MAX_ITEM_GROWTH ; i++)
		m_iItemGrowth[i] = 0;

	ClearDesc();

	m_iMainType    = 0;
	m_iMagicCode   = 0;
	m_iMagicCodeEx = 0;
	m_iManualIndex = 0;
}

void MyInventoryToolTip::UpdateWndSize()
{
	ManualMgr *pManual = NULL;
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM )	
		pManual = ioExtraInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_DECO )
		pManual = ioDecoInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_ETC )
		pManual = ioEtcInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_PRESENT )
		pManual = ioPresentInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_MEDALITEM )
		pManual = ioMedalInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_COSTUME )
		pManual = ioCostumeInvenManualMgr::GetSingletonPtr();
	else if( m_iMainType == POPUPSTORE_ITEM_PRESENT )
		pManual = ioPopupStorePresentItemManualMgr::GetSingletonPtr();
	else if( m_iMainType == INVENTORY_ITEM_ACCESSORY )
		pManual = ioAccessoryInvenManualMgr::GetSingletonPtr();

	if( !pManual )
		return;

	bool bAutoLine = false;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsTooltipAutoLine() )
		bAutoLine = true;

	if( !bAutoLine )
	{
		int iMaxLine = pManual->GetMaxLine( m_iManualIndex );
		iMaxLine--; // 1번째는 타이틀이므로 삭제
		if( iMaxLine <= 0 )
			iMaxLine = 0;

		enum { DEFAULT_HEIGHT = 123, GROWTH_HEIGHT = 108, };
		int iGrwothHeight = 0;
		if( m_iMainType == INVENTORY_ITEM_EXTRAITEM ||
			m_iMainType == INVENTORY_ITEM_MEDALITEM )
			iGrwothHeight = GROWTH_HEIGHT;

		int iAccSize = m_AccDesc.GetLineSize() * LINE_GAP;
		if( m_iMaxAccComDesc != 0 )
			iAccSize += m_iMaxAccComDesc * LINE_GAP + ACC_COM_GAP;

		SetSize( GetWidth(), DEFAULT_HEIGHT+( LINE_GAP * iMaxLine) + iGrwothHeight + iAccSize );
	}
	else
	{
		int iMaxLine = pManual->GetMaxLine( m_iManualIndex );
		int iNewLine = iMaxLine - 1; // 1번째는 타이틀이므로 삭제
		if( iNewLine <= 0 )
			iNewLine = 0;

		ioComplexStringPrinter kPrinter;
		int iTotalLineGap = 18;
		if( iNewLine > 0 )
		{
			for(int iLine = 1; iLine < iMaxLine; iLine++)
			{
				int iGap = pManual->GetSpaceLine( m_iManualIndex, iLine);
				enum { MAX_LOOP = 20, };
				bool bFirstGap = true;
				for (int i = 0; i < MAX_LOOP ; i++)
				{
					if( kPrinter.GetFullWidth() < DESC_WIDTH )
					{
						if( bFirstGap )
						{
							iTotalLineGap += pManual->GetSpaceLine( m_iManualIndex, iLine);
							bFirstGap = false;
						}
						else
							iTotalLineGap += LINE_GAP;
						kPrinter.ClearList();
						break;
					}

					ioComplexStringPrinter kPrinterA, kPrinterB;
					kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
					if( bFirstGap )
					{
						iTotalLineGap += pManual->GetSpaceLine( m_iManualIndex, iLine);
						bFirstGap = false;
					}
					else
						iTotalLineGap += LINE_GAP;
					kPrinterB.CopyFullText( kPrinter );
					kPrinterA.ClearList();
					kPrinterB.ClearList();
				}
			}
		}

		enum { DEFAULT_HEIGHT = 123, GROWTH_HEIGHT = 108, };

		int iGrwothHeight = 0;
		if( m_iMainType == INVENTORY_ITEM_EXTRAITEM ||
			m_iMainType == INVENTORY_ITEM_MEDALITEM )
			iGrwothHeight = GROWTH_HEIGHT;

		int iAccSize = m_AccDesc.GetLineSize() * LINE_GAP;
		if( m_iMaxAccComDesc != 0 )
			iAccSize += m_iMaxAccComDesc * LINE_GAP + ACC_COM_GAP;

		SetSize( GetWidth(), DEFAULT_HEIGHT+ iTotalLineGap + iGrwothHeight + iAccSize );
	}
}

void MyInventoryToolTip::OnProcessPopup()
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	PopupStoreWnd* pWnd = dynamic_cast<PopupStoreWnd*> ( g_GUIMgr.FindWnd( POPUP_STORE_WND ) );

	MToolTipItemInfo kInfo;
	if( pWnd && pMouse->GetToolTipWndID() == PopupStoreWnd::eID_ITEM_IMG )
	{
		PopupStoreWnd::sPopupItemInfo pInfo = pWnd->GetPopupItemInfo();

		GetPopupItemInfo( pInfo.iPresentType, pInfo.iManualIdx, pInfo.iPresentValue1, pInfo.iPresentValue2, kInfo );
	}

	if( IsShow() )
	{	
		if( !SetInfoPopup( kInfo ) )                                          
			HideWnd();
		else
		{
			pMouse->SetToolTipCursor();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );

		}

		ProcessDesc();
	}
	else
	{
		if( SetInfoPopup( kInfo ) ) // setinfo 후에 showwnd
		{
			ShowWnd();
			UpdateWndSize();
			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
		}
	}
}
void MyInventoryToolTip::GetPopupItemInfo( int iPresentType, int iMent, int iPresentValue1, int iPresentValue2, OUT MToolTipItemInfo &rkReturnInfo )
{
	short sPresentType = iPresentType;
	short sPresentMent = iMent;

	InventoryItemInfo rkInfo;
	if( sPresentType == PRESENT_SOLDIER )
	{
		int iClassType      = iPresentValue1;
		DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
		const ioSetItemInfo *pSetItemInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
		if( !pSetItemInfo )
			return;

		rkReturnInfo.m_sName = pSetItemInfo->GetName();
		rkReturnInfo.m_sIcon =  g_MyInfo.GetMySoldierIconName( iClassType );
		rkReturnInfo.m_sSubIcon = g_MyInfo.GetSoldierSubIconName( iClassType );
		rkReturnInfo.m_iLimitLevel  = pSetItemInfo->GetNeedLevel( 0 );        // 0번째 array 정보 ;
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );
	}
	else if( sPresentType == PRESENT_DECORATION ||
		sPresentType == PRESENT_RANDOM_DECO )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_DECO;
		kInfo.m_iSexType  = (iPresentValue1 % 100000) / 1000;
		kInfo.m_iDecoType = iPresentValue1 % 1000;
		kInfo.m_iDecoCode = iPresentValue2;
		GetDecoItemInfo( kInfo, rkReturnInfo );

		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_ETC_ITEM )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_ETC;
		kInfo.m_iMagicCode= iPresentValue1;
		GetEtcItemInfo( kInfo, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}	
	else if( sPresentType == PRESENT_PESO )
	{
		rkReturnInfo.m_sName        = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon        = "UIIconPack13#quest_002";
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_EXTRAITEM )
	{	
		GetExtraItemInfoByItemCode( iPresentValue1, (iPresentValue2%PRESENT_EXTRAITEM_DIVISION_1)/PRESENT_EXTRAITEM_DIVISION_2, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType == PRESENT_EXTRAITEM_BOX )
	{
		RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iPresentValue1 );
		if( !pInfo )
			return;

		rkReturnInfo.m_sName = pInfo->GetName();
		rkReturnInfo.m_sIcon = pInfo->GetIconName();
		rkReturnInfo.m_iLimitLevel  = pInfo->GetNeedLevel();
		rkReturnInfo.m_iManualIndex = sPresentMent;
		rkReturnInfo.m_nGradeType = pInfo->GetGradeType();
	}
	else if( sPresentType == PRESENT_GRADE_EXP )
	{
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = "UIIconPack13#quest_001";
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType ==PRESENT_MEDALITEM )
	{
		InventoryItemInfo kInfo = rkInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_MEDALITEM;
		kInfo.m_iMagicCode= iPresentValue1;
		GetMedalItemInfo( kInfo, rkReturnInfo );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType ==PRESENT_PET )
	{
		int nPetRank = iPresentValue2%10000;
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = g_PetInfoMgr.GetPetIcon( iPresentValue1, (PetRankType)nPetRank );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
	else if( sPresentType ==PRESENT_SPIRIT )
	{
		rkReturnInfo.m_sName = g_PresentMgr.GetPresentValue1Text( sPresentType, iPresentValue1, iPresentValue2 ).c_str();
		rkReturnInfo.m_sIcon = g_SpiritMgr.GetIconName( iPresentValue1 );
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
}
//////////////////////////////////////////////////////////////////////////
ExtraItemEquipConfirmWnd::ExtraItemEquipConfirmWnd()
{
	m_pExclamationMark = NULL;
	m_pPlayStage = NULL;
}

ExtraItemEquipConfirmWnd::~ExtraItemEquipConfirmWnd()
{
	SAFEDELETE( m_pExclamationMark );
}

void ExtraItemEquipConfirmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	ioMovingWnd::AddRenderFrame( szType, pFrame );
}

void ExtraItemEquipConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ExclamationMark" )
	{
		SAFEDELETE( m_pExclamationMark );
		m_pExclamationMark = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );	
	}
}

bool ExtraItemEquipConfirmWnd::SetExtraItemEquipInfo( int iCharArray, int iSlot, int iSlotIndex )
{
	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem && pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
	{
		m_szName = kSlot.m_Name;

		m_iCharArray = iCharArray;
		m_iEquipSlot = iSlot;
		m_iExtraItemIndex = iSlotIndex;

		m_iItemCode = kSlot.m_iItemCode;
		m_iPeriodType = kSlot.m_PeriodType;

		ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
		if( pIconBtn )
		{
			pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kSlot.m_iReinforce );

			const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
			if ( pItem )
				pIconBtn->SetGradeType( pItem->GetGradeType() );

			pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

			if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
				pIconBtn->SetCustomItem( true );
			else
				pIconBtn->SetCustomItem( false );
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

void ExtraItemEquipConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EQUIP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 출전중인 용병만
			if( m_iCharArray == g_MyInfo.GetSelectCharArray() )
			{
				if( m_pPlayStage && !m_pPlayStage->IsModeStateResult() )
				{
					ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
					// 장비 교체 가능상태 체크
					if( pOwner && !pOwner->SetExtraItemChangeState( false ) )
					{
						HideWnd();
						return;
					}
				}
			}

			TCPNetwork::MouseBusy( true );

			SP2Packet kPacket( CTPK_EXTRAITEM_CHANGE );
			kPacket << m_iCharArray;
			kPacket << m_iEquipSlot;
			kPacket << m_iExtraItemIndex;
			TCPNetwork::SendToServer( kPacket );

			g_MyInfo.SetExtraItemChange( 3, m_iCharArray, m_iEquipSlot, m_iExtraItemIndex );
			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_EQUIP, m_iItemCode, m_iPeriodType  );

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

void ExtraItemEquipConfirmWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

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

void ExtraItemEquipConfirmWnd::OnRenderAfterChild()
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

void ExtraItemEquipConfirmWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}
//////////////////////////////////////////////////////////////////////////
SpiritItemWnd::SpiritItemWnd()
{
	for( int i = 0; i < 6; i++ )
		m_pSlotBG[i] = NULL;

	m_pEmptySlot = NULL;
	m_pNullSlot = NULL;
	m_pIconBackImg = NULL;
	m_pIconImg = NULL;

	m_pSpiritMark = NULL;

	m_iCode = 0;
	m_iCount = 0;
	m_bLockState = false;
	m_bSoldier = false;
	m_bCompose = false;
	m_bSlotEffect = false;

	// 2019-06-10
	m_bSelectPos = false;
	m_pSelectPosFrm = NULL;
	m_pCompleteSelectPosFrm = NULL;
	m_bCompleteSelectPos = false;
	m_nSelectItemIndex = -1;

	// 2019-06-14
	m_dwOverTicTime = 0;
	m_byOverFrmPrintCount = 0;
	m_bCompleteSelectPos =false;
	m_bCompleteActive = false;
	m_bKeyActive = false;

}

SpiritItemWnd::~SpiritItemWnd()
{
	for( int i = 0; i < 6; i++ )
		SAFEDELETE(m_pSlotBG[i]);

	SAFEDELETE(m_pEmptySlot);
	SAFEDELETE(m_pNullSlot);
	SAFEDELETE(m_pIconBackImg);
	SAFEDELETE(m_pIconImg);

	SAFEDELETE(m_pSpiritMark);

	// 2019-06-10
	m_bSelectPos = false;
	SAFEDELETE(m_pSelectPosFrm);
	SAFEDELETE(m_pCompleteSelectPosFrm);
	m_bCompleteSelectPos = false;
	m_nSelectItemIndex = -1;

	// 2019-06-14
	m_dwOverTicTime = 0;
	m_byOverFrmPrintCount = 0;
	m_bCompleteSelectPos =false;
	m_bCompleteActive = false;
	m_bKeyActive = false;
}

void SpiritItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "icon_back_image" )
	{
		SAFEDELETE( m_pIconBackImg );
		m_pIconBackImg = pImage;
		m_pIconBackImg->SetScale( 0.8f );
	}
	else if( szType == "SpiritMark" )
	{
		SAFEDELETE( m_pSpiritMark );
		m_pSpiritMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void SpiritItemWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "UniqueSlot" )
	{
		SAFEDELETE( m_pSlotBG[1] );
		m_pSlotBG[1] = pFrame;
	}
	else if( szType == "PrimiumSlot" )
	{
		SAFEDELETE( m_pSlotBG[2] );
		m_pSlotBG[2] = pFrame;
	}
	else if( szType == "IdolSlot" )
	{
		SAFEDELETE( m_pSlotBG[3] );
		m_pSlotBG[3] = pFrame;
	}
	else if( szType == "RareSlot" )
	{
		SAFEDELETE( m_pSlotBG[4] );
		m_pSlotBG[4] = pFrame;
	}
	else if( szType == "NormalSlot" )
	{
		SAFEDELETE( m_pSlotBG[5] );
		m_pSlotBG[5] = pFrame;
	}
	else if( szType == "SpecialSlot" )
	{
		SAFEDELETE( m_pSlotBG[0] );
		m_pSlotBG[0] = pFrame;
	}
	else if( szType == "EmptySlot" )
	{
		SAFEDELETE( m_pEmptySlot );
		m_pEmptySlot = pFrame;
	}
	else if( szType == "NullSlot" )
	{
		SAFEDELETE( m_pNullSlot );
		m_pNullSlot = pFrame;
	}
	else if( szType == "SelectPosFrm" )	// 2019-06-10
	{
		SAFEDELETE( m_pSelectPosFrm );
		m_pSelectPosFrm = pFrame;
	}
	else if( szType == "CompleteSelectPosFrm" )	// 2019-06-10
	{
		SAFEDELETE( m_pCompleteSelectPosFrm );
		m_pCompleteSelectPosFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void SpiritItemWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szBuf[MAX_PATH];
	for( int i = 0; i < 5; ++i )
	{
		wsprintf_e( szBuf, "DeleteMsg%d", i + 1 );
		m_szDeleteMsg[i] = xElement.GetStringAttribute( szBuf );
	}
	m_dwSlotEffectGapTime = xElement.GetIntAttribute_e( "SlotEffectGapTime" );
	if( m_dwSlotEffectGapTime == 0 )
		m_dwSlotEffectGapTime = 500;

	ioButton::ParseExtraInfo( xElement );
	m_dwOverTicTime = xElement.GetIntAttribute_e( "OverTicTime" );
	m_byOverFrmPrintCount = (BYTE)xElement.GetIntAttribute_e( "OverFrmPrintCount" );
}

void SpiritItemWnd::SetBlank()
{
	m_iCode = 0;
	m_iCount = 0;
	m_iMaxCount = 0;
	m_bLockState = false;
	m_bSoldier = false;
	m_bCompose = false;
	m_bSlotEffect = false;
	m_iSpiritType = 0;

	SAFEDELETE( m_pIconImg );
	SetInActive();
}

void SpiritItemWnd::OnRender()
{
	enum 
	{
		CENTER_X	= 43,
		CENTER_Y	= 34,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( m_iCode <= 0 )
	{
		if( m_pNullSlot )
			m_pNullSlot->Render( iXPos, iYPos );
	}
	else
	{
		// 2019-06-10
		ioButton::OnRender();

		ioUIRenderFrame *pCurSlot = NULL;
		if( m_iCount == 0 && m_pEmptySlot )
			pCurSlot = m_pEmptySlot;
		else if( COMPARE( m_iSpiritType, ioUserSpiritManager::SPT_SPECIAL, ioUserSpiritManager::SPT_NORMAL + 1 )
			&& m_pSlotBG[m_iSpiritType] )
			pCurSlot = m_pSlotBG[m_iSpiritType];

		if( pCurSlot )
		{
			pCurSlot->Render( iXPos, iYPos );
			if( m_bSlotEffect )
			{
				DWORD dwCurTime = FRAMEGETTIME();
				if( dwCurTime > m_dwSlotEffectTime + m_dwSlotEffectGapTime * 2 )
					m_dwSlotEffectTime = dwCurTime;
				if( dwCurTime > m_dwSlotEffectTime + m_dwSlotEffectGapTime )
					pCurSlot->Render( iXPos, iYPos, UI_RENDER_SCREEN );
			}
		}

		// 2019-06-25
		if( (true == m_bSelectPos) && (NULL != m_pSelectPosFrm) )
		{
			m_pSelectPosFrm->Render( iXPos, iYPos );
		}

		// 2019-06-25
		if( (true == m_bCompleteSelectPos) && (true == m_bCompleteActive) )
		{
			m_pCompleteSelectPosFrm->Render( iXPos, iYPos );
		}

		if( m_pIconBackImg )
			m_pIconBackImg->Render( iXPos+CENTER_X, iYPos+CENTER_Y, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		if( m_pIconImg )
		{
			if( m_bLockState )
				m_pIconImg->Render( iXPos+CENTER_X, iYPos+CENTER_Y, UI_RENDER_GRAY, TFO_BILINEAR );
			else
				m_pIconImg->Render( iXPos+CENTER_X, iYPos+CENTER_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		
		}

		char szBuf[MAX_PATH] = "";
		if( m_iMaxCount != 0 )
			wsprintf_e( szBuf, "%d/%d", m_iCount, m_iMaxCount );
		else
			wsprintf_e( szBuf, "%d", m_iCount );

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		if( m_iCount != 0 )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		g_FontMgr.PrintText( iXPos + 42, iYPos + 70, FONT_SIZE_12, szBuf );
		if( m_bSoldier )
		{
			if( m_pSpiritMark )
				m_pSpiritMark->Render( iXPos + 7, iYPos + 7 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 24, iYPos + 9, FONT_SIZE_11, "보유" );
		}
		if( IsOver() )
			OnDrawOvered( iXPos, iYPos );
	}	
}

void SpiritItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_DELETE:
		if( cmd == IOBN_BTNUP )
		{
			SetSpiritItemResell();
		}
		break;
	case ITEM_RESELL_WND:
		if( cmd == IOBN_BTNUP )
		{
			if( param == ACST_SELL )
			{
				SendDelete();
			}				
		}
		break;
	}
}

void SpiritItemWnd::SendDelete()
{
	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_SPIRIT_SELL );
	kPacket << m_iCode;
	TCPNetwork::SendToServer( kPacket );
}

void SpiritItemWnd::SetLockState( bool bLock )
{
	m_bLockState = bLock;
}

void SpiritItemWnd::SetInfo( int iCode, int iCount, ioHashString szIconName )
{
	SetBlank();
	
	m_iCode = iCode;
	m_iCount = iCount;
	m_IconName = szIconName;
	m_bLockState = false;
	m_iSpiritType = g_SpiritMgr.GetSpiritType( iCode );
	m_iMaxCount = g_SpiritMgr.GetComposeMax( iCode );
	m_bSoldier = g_MyInfo.IsCharMortmain( iCode );
	m_bCompose = (m_iCount >= g_SpiritMgr.GetComposeMin( iCode ));
	if( m_iMaxCount != 0 && m_iCount >= m_iMaxCount )
	{
		m_dwSlotEffectTime = FRAMEGETTIME();
		m_bSlotEffect = true;
	}
	if( m_iCode > 0 )
	{
		m_pIconImg = g_UIImageSetMgr.CreateImageByFullName( m_IconName );
		if( m_pIconImg )
			m_pIconImg->SetScale( 0.8f );

		SetActive();
	}
}

void SpiritItemWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_iCode == 0 )
		return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rect;
	rect.left	= GetDerivedPosX();
	rect.top	= GetDerivedPosY();
	rect.right	= GetDerivedPosX() + GetWidth();
	rect.bottom	= GetDerivedPosY() + GetHeight();

	if( ::PtInRect( &rect, pMouse->GetMousePos() ) && m_iCount != 0 )
		ShowChildWnd( ID_DELETE );
	else
		HideChildWnd( ID_DELETE );

	if( m_bOver && !m_bClicked )
	{
		DWORD dwID = GetID();
		pMouse->SetToolTipWndID( dwID );
	}

	// 2019-06-24
	if( (true == g_Input.IsKeyUp(KEY_SPACE)) && (true == m_bKeyActive) )
	{
		MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
		if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchComplete()) )
		{
			pMyInventoryWnd->SetSearchComplete(false);
			ioWnd *pWnd = GetParent()->FindChildWnd(GetID());
			if( NULL != pWnd )
			{
				m_bCompleteSelectPos = false;
				m_bKeyActive = false;
				GetParent()->iwm_command( pWnd, IOBN_BTNUP, 0 ); 
				return;
			}
		}

	}

	// 2019-06-24
	if( true == m_bCompleteSelectPos )
	{
		if( FRAMEGETTIME() - m_dwCompleteSelectTimer > m_dwOverTicTime )
		{
			m_dwCompleteSelectTimer = FRAMEGETTIME();
			m_bCompleteActive = !m_bCompleteActive;
			m_bKeyActive = true;
		}
		return;
	}
	else
	{
		m_bKeyActive = false;
		m_dwCompleteSelectTimer = 0;
	}
}

void SpiritItemWnd::SetSpiritItemResell()
{
	ioHashString szItemName = g_SpiritMgr.GetName(m_iCode);

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( m_iCount, szConvertNum, sizeof( szConvertNum ) );
			
	char szTemp[MAX_PATH] = "";
	SafeSprintf( szTemp, sizeof( szTemp ), "%s개", szConvertNum );
	ioHashString szItemInfo = szTemp;

	int iPeso = m_iCount * g_SpiritMgr.GetSellConst( m_iCode );
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof(szConvertNum) );

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, szItemName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, szItemInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, m_szDeleteMsg[0].c_str());

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, m_szDeleteMsg[1].c_str() );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, m_szDeleteMsg[2].c_str(), szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, m_szDeleteMsg[3].c_str() );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( m_IconName );
		ioUIRenderImage *pSubImg = NULL;
		pResellWnd->SetInfoAndShow( m_szDeleteMsg[4].c_str(), pImg, pSubImg, 0, kPrinter, this, SELL_EDIT_NONE, ACST_SELL, false );
	}
}

// 2019-06-10
void SpiritItemWnd::OnDrawOvered( int iXPos, int iYPos )
{	
	ioButton::OnDrawOvered( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectItemIndex) )
	{
		m_bSelectPos = true;
	}
	else if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) )
	{
		ioButton::OnDrawNormal( iXPos, iYPos );
	}

}

// 2019-06-10
void SpiritItemWnd::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	m_bSelectPos = false;

	MyInventoryWnd* pMyInventoryWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );	
	if( (NULL != pMyInventoryWnd) && (true == pMyInventoryWnd->IsSearchInputEnter()) && (pMyInventoryWnd->GetCurSelectCharIndex() == m_nSelectItemIndex) )
	{
		m_bSelectPos = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
AlchemicItemToolTip::AlchemicItemToolTip()
{
	Clear();
}

AlchemicItemToolTip::~AlchemicItemToolTip()
{
	Clear();
}

void AlchemicItemToolTip::OnProcess( float fTimePerSec )
{
	if( !g_GUIMgr.IsShow( MY_INVENTORY_WND ) )
	{
		HideWnd();
		return;
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	AlchemicToolTipInfo kInfo;
	MyInventoryWnd *pInven = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );

	if( pInven )
	{
		pInven->GetCurAlchemicItemInfo( kInfo, pMouse->GetToolTipWndID() );
	}

	if( IsShow() )
	{
		if( !SetInfo( kInfo.m_iCode, kInfo.m_iType ) )
		{
			HideWnd();
		}
		else
		{
			pMouse->SetToolTipWndID( 0 );
			pMouse->SetToolTipCursor();

			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
			ProcessPosition();
		}
	}
	else
	{
		if( SetInfo( kInfo.m_iCode, kInfo.m_iType ) ) // setinfo 후에 showwnd
		{
			ShowWnd();

			int iXPos = 0;
			int iYPos = 0;
			Help::GetNotCutWndPos( pMouse->GetMousePos().x, pMouse->GetMousePos().y, GetWidth(), GetHeight(), iXPos, iYPos );
			SetWndPos( iXPos, iYPos );
			ProcessPosition();
		}
	}
}

bool AlchemicItemToolTip::SetInfo( int iCode, int iType )
{
	Clear();

	m_iCode = iCode;
	m_iType = iType;

	if( m_iCode == 0 )
	{
		Clear();
		return false;
	}

	// desc
	m_szDescArray.SetTextStyle( TS_NORMAL );
	m_szDescArray.SetBkColor( 0, 0, 0 );	
	m_szDescArray.SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회색

	switch( m_iType )
	{
	case ATT_ALCHEMIC_ITEM:
		{
			m_szDescArray.AddTextPiece( FONT_SIZE_12, g_SpiritMgr.GetName(m_iCode).c_str() );
		}
		break;
	case ATT_SOLDIER:
		{
			m_szDescArray.AddTextPiece( FONT_SIZE_12, "%s", g_MyInfo.GetClassName(m_iCode) );
		}
		break;
	case ATT_ITEM:
		{
			const ioItem *pItem = g_ItemMaker.GetItemConst( m_iCode, __FUNCTION__ );
			if( pItem )
			{
				m_szDescArray.AddTextPiece( FONT_SIZE_12, "%s", pItem->GetName().c_str() );
			}
		}
		break;
	case ATT_ETCITEM:
		{
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_iCode );
			if( pEtcItem )
				m_szDescArray.AddTextPiece( FONT_SIZE_12, pEtcItem->GetName().c_str() );
		}
		break;
	}

	int iMaxWidth  = m_szDescArray.GetFullWidth() + SIDE_TEXT_GAP;
	int iMaxHeight = MIN_HEIGHT;

	SetSize( iMaxWidth, iMaxHeight );

	return true;
}

void AlchemicItemToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX() + 7;
	int iYPos = GetDerivedPosY() + 6;

	if( !m_szDescArray.IsEmpty() )
	{
		m_szDescArray.PrintFullTextWidthCut( iXPos, iYPos, TAT_LEFT, 126.0f );
	}
}

void AlchemicItemToolTip::Clear()
{
	m_iCode = 0;
	m_iType = 0;

	m_szDescArray.ClearList();
}

void AlchemicItemToolTip::ProcessPosition()
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MyInventoryWnd::ShowItemMaterialCompoundWnd( DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	
	ItemMaterialCompoundWnd *pWnd = dynamic_cast<ItemMaterialCompoundWnd*>(FindChildWnd(ID_ITEM_MATERIAL_COMPOUND_WND));
	if( pWnd )
		pWnd->ShowWndWithEtcItemCode( dwCode );
}

void MyInventoryWnd::OnItemMaterialCompoundResult( bool bSuccess, int iTargetSlot, int iReinforce, DWORD dwCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	HideChildWnd(ID_ITEM_MATERIAL_COMPOUND_WND);

	ShowChildWnd( ID_FLASH_WND );
	ItemMaterialCompoundResultWnd *pWnd = dynamic_cast<ItemMaterialCompoundResultWnd*>(FindChildWnd(ID_ITEM_MATERIAL_COMPOUND_RESULT));
	if( pWnd )
		pWnd->SetCompoundResultInfo( bSuccess, iTargetSlot, iReinforce, dwCode );
}

void MyInventoryWnd::OnItemMaterialCompoundResultFailExp( int nPreFailExp, int nCurFailExp, int nItemCode )
{
	ItemMaterialCompoundResultWnd *pWnd = dynamic_cast<ItemMaterialCompoundResultWnd*>(FindChildWnd(ID_ITEM_MATERIAL_COMPOUND_RESULT));
	if( pWnd && pWnd->IsShow() )
		pWnd->SetFailExp( nPreFailExp, nCurFailExp, nItemCode );
}

void MyInventoryWnd::SetReinforceResultInfo( bool bSuccess, int iTargetSlot, int iReinforce )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ExtraItemFuncWnd *pWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( pWnd )
		pWnd->SetReinforceResultInfo( bSuccess, iTargetSlot, iReinforce );
}

void MyInventoryWnd::SetReinforceResultFailExp( int nPreFailExp, int nCurFailExp )
{
	ExtraItemFuncWnd *pWnd = dynamic_cast<ExtraItemFuncWnd*>(FindChildWnd(ID_EXTRAITEMFUNC_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->SetFailExp( nPreFailExp, nCurFailExp );
}

void MyInventoryWnd::OnSpiritComposeResult( int iClassType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	SpiritFuncWnd *pSpiritWnd = dynamic_cast<SpiritFuncWnd*>(FindChildWnd(ID_SPIRITFUNC_WND));
	if( pSpiritWnd )
		pSpiritWnd->OnSpiritComposeResult( iClassType );
}

void MyInventoryWnd::OnSpiritConversionResult( int iSpiritCode, int iCount, bool bCritical )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	SpiritFuncWnd *pSpiritWnd = dynamic_cast<SpiritFuncWnd*>(FindChildWnd(ID_SPIRITFUNC_WND));
	if( pSpiritWnd )
		pSpiritWnd->OnSpiritConversionResult( iSpiritCode, iCount, bCritical );
}

void MyInventoryWnd::ShowPetEggResultWnd( DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PetEggResultWnd *pWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pWnd )
		pWnd->ShowRandomBox( dwEtcItemType );
}

void MyInventoryWnd::OnPetEggResultInfo( int nPetCode, int nPetRank, int nPetSlotIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	PetEggResultWnd *pWnd = dynamic_cast<PetEggResultWnd*>(FindChildWnd(ID_PET_EGG_RESULT_WND));
	if( pWnd )
		pWnd->OnResultInfo( nPetCode, nPetRank, nPetSlotIndex );
}

void MyInventoryWnd::ShowHeroReinforceDownWnd( int iClassType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	HeroReinforceDownWnd *pWnd = dynamic_cast<HeroReinforceDownWnd*>(FindChildWnd(ID_HERO_REINFORCE_DWON_WND));
	if( pWnd )
	{
		pWnd->SetSoldierInfo( iClassType );
		pWnd->ShowWnd();
	}	
}

void MyInventoryWnd::ShowHeroReinforceDoneWnd( int iClassType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	HeroReinforceDoneWnd *pWnd = dynamic_cast<HeroReinforceDoneWnd*>(FindChildWnd(ID_HERO_REINFORCE_DONE_WND));
	if( pWnd )
	{
		pWnd->SetSoldierInfo( iClassType );
		pWnd->ShowWnd();
	}
}

ioWnd* MyInventoryWnd::ShowItemReinforceDownWnd( int iIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	ItemReinforceDownWnd *pWnd = dynamic_cast<ItemReinforceDownWnd*>(FindChildWnd(ID_ITEM_REINFORCE_DWON_WND));
	if( pWnd )
	{
		pWnd->SetItemInfo( iIndex );
		pWnd->ShowWnd();
		return pWnd;
	}

	return NULL;
}

void MyInventoryWnd::ShowItemReinforceDoneWnd( int iIndex )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	ItemReinforceDoneWnd *pWnd = dynamic_cast<ItemReinforceDoneWnd*>(FindChildWnd(ID_ITEM_REINFORCE_DONE_WND));
	if( pWnd )
	{
		pWnd->SetItemInfo( iIndex );
		pWnd->ShowWnd();
	}
}

// UI코스튬 관련 (코스튬 박스 윈도우)
void MyInventoryWnd::ShowCostumeBoxWnd( int nMachineCode )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );

	ioCostumeRandomBox *pWnd = dynamic_cast<ioCostumeRandomBox*>(FindChildWnd(ID_COSTUME_BOX_WND));
	if( pWnd )
		pWnd->ShowRandomBox( nMachineCode );
}

// UI코스튬 관련 (코스튬 박스 윈도우)
void MyInventoryWnd::OnCostumeBoxResultInfo( int nCostumeCode, int nPeriodTime )
{
	ioCostumeRandomBox *pWnd = dynamic_cast<ioCostumeRandomBox*>(FindChildWnd(ID_COSTUME_BOX_WND));
	if( pWnd && pWnd->IsShow() )
		pWnd->OnResultInfo( nCostumeCode, nPeriodTime );
}

void MyInventoryWnd::ShowNewMultipleCompoundWnd( DWORD dwType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	ItemNewMultipleCompoundWnd *pWnd = dynamic_cast<ItemNewMultipleCompoundWnd*>(FindChildWnd(ID_NEW_MULTIPLE_COMPOUND_WND));
	if( pWnd )
		pWnd->SetInfoAndShow( dwType );
}

void MyInventoryWnd::ShowGashponWndByNewMultipleCompound( DWORD dwPossessEtcItemType, DWORD dwEtcItemType )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_NEW_MULTIPLE_COMPOUND_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	GashaponWnd *pWnd = dynamic_cast<GashaponWnd*>(FindChildWnd(ID_GASHPON_WND));
	if( pWnd )
		pWnd->ShowRandomBox( dwPossessEtcItemType, dwEtcItemType );
}

void MyInventoryWnd::ShowNamedTitleResultWnd( DWORD dwCode, int nLevel, BYTE byPremium )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	NamedTitleResultWnd *pWnd = dynamic_cast<NamedTitleResultWnd*>(FindChildWnd(ID_NAMED_TITLE_RESULT_WND));
	if( pWnd )
		pWnd->ShowResult( dwCode, nLevel, byPremium );
}

void MyInventoryWnd::ShowNamedTitlePremiumResultWnd( DWORD dwCode, int nLevel, BYTE byPremium )
{
	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	NamedTitlePremiumResultWnd *pWnd = dynamic_cast<NamedTitlePremiumResultWnd*>(FindChildWnd(ID_NAMED_TITLE_PREMIUM_RESULT_WND));
	if( pWnd )
		pWnd->ShowResult( dwCode, nLevel, byPremium );
}

void MyInventoryWnd::ShowNamedTitlePremiumSelectWnd( DWORD dwEtcItemCode )
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	if ( !pUserNamedTitle->IsEnablePremiumNamedTitle() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "변경할 칭호가 없습니다." );
		return;
	}

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( pInfoWnd && pInfoWnd->IsShow() )
		pInfoWnd->HideWnd();

	ShowChildWnd( ID_FLASH_WND );
	NamedTitlePremiumSelectWnd *pWnd = dynamic_cast<NamedTitlePremiumSelectWnd*>(FindChildWnd(ID_NAMED_TITLE_PREMIUM_SELECT_WND));
	if( pWnd )
		pWnd->ShowSelectWnd( dwEtcItemCode );
}

void MyInventoryWnd::CheckBonusCash()
{
	ioButton* pBonusCashBtn = dynamic_cast<ioButton*>(FindChildWnd( ID_BONUS_CASH_BTN ));
	if ( !pBonusCashBtn )
		return;

	int iBonusCash = g_MyInfo.GetBonusCash();
	if ( iBonusCash <=0 )
	{
		pBonusCashBtn->HideWnd();
		return;
	}
	else if( !pBonusCashBtn->IsShow() )
	{
		pBonusCashBtn->ShowWnd();
	}

	if ( pBonusCashBtn->IsShow() && pBonusCashBtn->IsOver() )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if ( pMouse )
			pMouse->SetToolTipCursor();
	}

	if ( m_dwBonusCashTooltipCheckDuration == 0 )
		return;
	if ( m_dwBonusCashTooltipCheckStartTime + m_dwBonusCashTooltipCheckDuration > FRAMEGETTIME() )
		return;

	m_dwBonusCashTooltipCheckStartTime = FRAMEGETTIME();

	if ( g_BonusCashMgr.CheckAlarmExpireBonusCash() )
	{
		ioWnd* pBonusTooltip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
		if ( pBonusTooltip && !pBonusTooltip->IsShow() )
			pBonusTooltip->ShowWnd();
	}
	else
	{
		ioWnd* pBonusTooltip = FindChildWnd( ID_BONUS_CASH_TOOLTIP );
		if ( pBonusTooltip && pBonusTooltip->IsShow() )
			pBonusTooltip->HideWnd();
	}
}

int MyInventoryWnd::UpdateSmallTabName( ioWnd* pWnd, int iXOffset, const ioHashString& szTitle )
{
	if( !pWnd )
		return 0;

	ioUITitle* pTitle = pWnd->GetTitle();
	if( !pTitle )
		return 0;

	if( szTitle.IsEmpty() )
		return 0;

	float fScale	= (float)pTitle->GetSize() / (float)g_FontMgr.GetFontSize();
	int iTextWidth	= g_FontMgr.GetTextWidth( szTitle.c_str(), TS_NORMAL, fScale ) + 11;
	pTitle->SetOffset( iTextWidth/2, 0 );

	pWnd->SetSize( iTextWidth, g_FontMgr.GetFontSize() );
	pWnd->SetWndPos( iXOffset, pWnd->GetYPos() );
	pWnd->SetTitleText( szTitle.c_str() );
	pWnd->ShowWnd();

	return pWnd->GetWidth();
}

void MyInventoryWnd::SetAccessoryInfoVec( DWORD dwTabID, DWORD dwSmallTabID )
{
	m_vInventoryItemInfoList.clear();
	m_vAlchemicItemInfoList.clear();

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory ) 
		return;

	int nSize = pAccessory->GetAccessorySlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		AccessorySlot kSlot;
		if( !pAccessory->GetAccessorySlotArray( i, kSlot ) )
			continue;

		// 스몰탭 인덱스랑 같아야함
		int nGroup = (kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE);
		if( !IsRightSmallTab( dwTabID, dwSmallTabID, nGroup ) )
			continue;

		if( kSlot.m_bEquip )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_ACCESSORY;
		kInfo.m_iMagicCode = kSlot.m_iIndex;
		kInfo.m_bEquip = kSlot.m_bEquip;

		int nSlot = kSlot.m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE;
		int nValue = kSlot.m_iValue1;

		kInfo.m_bActive = true;

		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		kLimitTime += 30;
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );
		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec <= 0 )
			kInfo.m_bActive = false;

		kInfo.m_iPriorityOrder = nSlot * 10000 + nValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_iAccessoryCode;

		// 2019-06-18	
		kInfo.m_szItemName = kSlot.m_Name;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwSmallTabID == ID_SMALL_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );

	// 최종 슬롯 인덱스 저장 2019-06-18
 	if( (NULL != m_pNewSearchWnd) && ( true != m_pNewSearchWnd->IsFindKeyWordEmpty()) )
	{		
		vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin();
		while( iter != m_vInventoryItemInfoList.end() )
		{	
			InventoryItemInfo kInfo = (*iter);

			if( false == m_pNewSearchWnd->IsSearch( kInfo.m_szItemName, m_pNewSearchWnd->GetFindKeyWord()) )
			{
				iter = m_vInventoryItemInfoList.erase(iter);
			}
			else
			{
				++iter;				
			}
		}
	}
}

bool MyInventoryWnd::SetReSellAccessory( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) 
		return false;

	int iNewSlotIndex = pInvenItem->GetMagicCode();
	if( iNewSlotIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 판매 오류 발생1" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellAccessory Wrong Btn : %d", dwID );
		return false;
	}

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 판매 오류 발생2" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellAccessory Don't Have UserExtraItem" );
		return false;
	}

	AccessorySlot kSlot;
	if( !pAccessory->GetAccessorySlot( iNewSlotIndex, kSlot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 판매 오류 발생3" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellAccessory Not Exist Item : %d", iNewSlotIndex );
		return false;
	}

	if( kSlot.m_bEquip )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "액세서리 판매 오류 발생4" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellAccessory Item : %d", iNewSlotIndex );
		return false;
	}

	m_iSellSlotIndex = iNewSlotIndex;
	if( !CheckTimeEndAccessory( iNewSlotIndex ) )
		return false;	

	float fReturnPeso = 0.0f;
	DWORD dwEditType = SELL_EDIT_NONE;
	if( kSlot.m_PeriodType == ACCPT_MORTMAIN )
	{
		fReturnPeso = g_AccessoryInfoMgr.GetResellMortmainAccessoryPeso();
		dwEditType = SELL_EDIT_EXTRAITEM;
	}
	else
		fReturnPeso = g_AccessoryInfoMgr.GetResellTimeAccessoryTimePeso();

	__int64 iPeso = (__int64)fReturnPeso;

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof( szConvertNum ) );

	ioHashString sInfo;
	pAccessory->GetUseInfo( iNewSlotIndex, sInfo );

	if( sInfo.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s don't have , Use Info:%d", __FUNCTION__, iNewSlotIndex );
	}

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, "%s", kSlot.GetName().c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, "%s", sInfo.c_str() );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "위 액세서리를 판매합니다." );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "예상 환불금액 : " );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "%s 페소", szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "(다소 차이가 있을 수 있습니다)" );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		ioUIRenderImage *pSubImg = NULL;
		pResellWnd->SetInfoAndShow( "액세서리 팔기", pImg, pSubImg, 0, kPrinter, this, dwEditType, ACST_SELL_ACCESSORY, false );
	}

	return true;
}

bool MyInventoryWnd::SetReSellDeco( DWORD dwID )
{
	InventoryItemWnd *pInvenItem = dynamic_cast<InventoryItemWnd*>(FindChildWnd(dwID));
	if( !pInvenItem ) 
		return false;

	MyInventoryInfoWnd *pInfoWnd = dynamic_cast<MyInventoryInfoWnd*>(FindChildWnd(ID_INVENTORY_INFO_WND));
	if( !pInfoWnd )
		return false;

	CHARACTER rkCharInfo = pInfoWnd->GetCharInfo();

	int iDecoType = pInvenItem->GetDecoType();
	int iDecoCode = pInvenItem->GetDecoCode();

	m_iSellSlotIndex = (rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000);
	m_iSellSubIndex = 0;
	switch( iDecoType )
	{
	case UID_FACE:
	case UID_HAIR:
	case UID_SKIN_COLOR:
	case UID_HAIR_COLOR:
	case UID_UNDERWEAR:
	case UID_KINDRED:
		{
			m_iSellSlotIndex += iDecoType;
			m_iSellSubIndex = iDecoCode;
		}
		break;
	case UID_CLASS:
		{
			int iEquipType = iDecoCode / 1000;
			int iEquipCode = iDecoCode % 1000;
			m_iSellSlotIndex += iEquipType;
			m_iSellSubIndex = iEquipCode;
		}
		break;
	}

	int iCharArray = g_MyInfo.GetClassArray( rkCharInfo.m_class_type );
	if( !COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "치장 판매 오류 발생1" );
		LOG.PrintTimeAndLog( 0, "MyInventoryWnd::SetReSellDeco Wrong Char Array : %d", iCharArray );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( rkCharInfo.m_class_type ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "치장 판매 오류 발생2" );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( rkCharInfo.m_class_type, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "치장 판매 오류 발생3" );
		return false;
	}

	if( iDecoType == UID_KINDRED )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "치장 판매 오류 발생4" );
		return false;
	}
	const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( rkCharInfo.m_sex - 1, iDecoType, iDecoCode );
	if( !pData ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "치장 판매 오류 발생5" );
		return false;
	}

	DWORD dwEditType = SELL_EDIT_NONE;
	__int64 iPeso = (__int64)pData->m_iSellPeso;

	char szConvertNum[MAX_PATH] = "";
	Help::ConvertNumToStrComma( iPeso, szConvertNum, sizeof( szConvertNum ) );

	ioComplexStringPrinter kPrinter[ItemResellWnd::MAX_INFO_LINE];

	// ItemInfo
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_17, "%s", pData->m_szName.c_str() );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[1].AddTextPiece( FONT_SIZE_17, "%s", "영구사용" );

	// Desc
	kPrinter[2].SetTextStyle( TS_NORMAL );
	kPrinter[2].SetBkColor( 0, 0, 0 );
	kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[2].AddTextPiece( FONT_SIZE_13, "위 치장을 판매합니다." );

	kPrinter[3].SetTextStyle( TS_NORMAL );
	kPrinter[3].SetBkColor( 0, 0, 0 );
	kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "예상 환불금액 : " );
	kPrinter[3].SetTextColor( TCT_DEFAULT_BLUE );
	kPrinter[3].AddTextPiece( FONT_SIZE_13, "%s 페소", szConvertNum );

	kPrinter[4].SetTextStyle( TS_NORMAL );
	kPrinter[4].SetBkColor( 0, 0, 0 );
	kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[4].AddTextPiece( FONT_SIZE_13, "(다소 차이가 있을 수 있습니다)" );

	ItemResellWnd *pResellWnd = dynamic_cast<ItemResellWnd*>(g_GUIMgr.FindWnd(ITEM_RESELL_WND));
	if( pResellWnd )
	{
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pData->m_szIcon );
		ioUIRenderImage *pSubImg = NULL;
		pResellWnd->SetInfoAndShow( "치장 팔기", pImg, pSubImg, 0, kPrinter, this, dwEditType, ACST_SELL_DECO, false );
	}

	return true;
}

bool MyInventoryWnd::CheckTimeEndAccessory( int iSlotIndex )
{
	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( pAccessory )
	{
		AccessorySlot kSlot;
		if( pAccessory->GetAccessorySlot( iSlotIndex, kSlot ) )
		{
			if( kSlot.m_PeriodType != ACCPT_MORTMAIN )
			{
				time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
				time_t kCurServerTime = g_MyInfo.GetServerDate();
				kCurServerTime += ( REALGETTIME()/1000 );// 현재까지의 클라이언트초를 넣는다.

				int iGapSec = kLimitTime - kCurServerTime;
				enum { DAY_SEC = 86400, HOUR_SEC = 3600, MINUTE_SEC = 60, };
				int iDay    = iGapSec/DAY_SEC;
				int iHourSec= iGapSec-(iDay*DAY_SEC);
				int iHour   = iHourSec / HOUR_SEC;
				int iMinute = ( iHourSec-(iHour*HOUR_SEC) ) / MINUTE_SEC;

				if( iDay <= 0 && iHour <= 0 && iMinute <= 1 )
				{
					g_GUIMgr.SetMsgBox( MB_YESNO, this, m_szTimeEndAccessoryDelMsg.c_str() );
					return false;
				}
			}
		}
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간만료로 판매할 수 없습니다." );
		return false;
	}

	return true;
}

// 2019-05-20 
void MyInventoryWnd::GetSmallTabNameList( OUT ioHashStringVec& NameList )
{
	NameList.clear();
	for (int i = 0; i < MAX_SMALL_TAB_BTN; i++ )
	{
		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( ID_SMALL_TAB_1_BTN + i ) );
		if( !pButton )
			continue;

		// 2019-05-20 추가 
		if( !pButton->IsShow() || ( true == IsHideSmallTabSearch(ID_SMALL_TAB_1_BTN + i) ) )
		{
			continue;
		}

		if( pButton->HasWndStyle( IWS_INACTIVE ) )
		{
			NameList.push_back( "" );
		}
		else
		{
			NameList.push_back( pButton->GetTitleText() );
		}
	}
}

// 2019-05-20 
void MyInventoryWnd::UpdateSmallTabSearchCategoryTitle()
{					
	if( NULL != m_pNewSearchWnd )
	{
		SmallTabButton* pButton = dynamic_cast<SmallTabButton*>( FindChildWnd( m_dwCurSmallTabID ) );
		if( pButton )
		{
			m_pNewSearchWnd->CreateCategoryPullDownTitle( pButton->GetTitleText() );
		}
	}	
}

// 2019-05-20
bool MyInventoryWnd::IsHideSmallTabSearch( DWORD dwSmallTabID )
{
	switch( m_dwCurTabID )
	{
		case ID_SOLDIER_TAB_BTN:	// 용병 
		{
			if( ID_SMALL_TAB_7_BTN == dwSmallTabID )	// *패키지
			{
				return true;
			}			
		}
		break;

		case ID_EXTRA_TAB_BTN:		//	장비 
		{
			if( ID_SMALL_TAB_7_BTN == dwSmallTabID )	// *기능
			{
				return true;
			}			
		}
		break;

		case ID_DECO_TAB_BTN:		//	치장
		{
			//if( ID_SMALL_TAB_8_BTN == dwSmallTabID )	// *패키지
			//{
			//	return true;
			//}	

			return true; // 2019-06-18
		}
		break;

		case ID_ALCHEMIC_TAB_BTN:	//	정기
		{
		}
		break;

		case ID_ETC_TAB_BTN:		//	특별		
		{
		}
		break;

		case ID_COSTUME_TAB_BTN:	//	코스튬
		{			
			ioWnd *pCostumeHelpWnd = FindChildWnd( ID_COSTUME_HELP );
			if( NULL != pCostumeHelpWnd )
			{	
				pCostumeHelpWnd->SetPosX(m_nCostumeHelpPosX);				
			}

			if( ID_SMALL_TAB_5_BTN == dwSmallTabID )	// *기능
			{
				if( NULL != pCostumeHelpWnd )
				{
					pCostumeHelpWnd->SetPosX(m_nSearchShowBtnPosX);
				}

				return true;
			}		
		}
		break;

		case ID_ACCESSORY_TAB_BTN:	//	액세서리
		{
		}
		break;

		case ID_PRESENT_TAB_BTN:	// 보관함
		{
			return true;
		}

		default:
		{

		}
	}

	return false;
}

// 2019-06-05
void MyInventoryWnd::ShowSearchWnd( DWORD dwID )
{
	HideChildWnd( ID_NEW_SEARCH_WND );
	HideChildWnd( ID_SEARCH_SHOW_BTN );
	HideChildWnd( ID_SEARCH_HIDE_BTN );
	
	if( true == COMPARE( dwID, ID_SOLDIER_TAB_BTN, (ID_SOLDIER_TAB_BTN+MAX_TAB_BTN) ) )
	{
		if( (ID_PRESENT_TAB_BTN == dwID) || (ID_DECO_TAB_BTN == dwID) )	// 보관함, 치장 
		{
			return; 
		}

		ShowChildWnd( ID_SEARCH_SHOW_BTN );
		HideChildWnd( ID_SEARCH_HIDE_BTN );	
	}

}

void MyInventoryWnd::UpdateSmallTabSearchList()
{
	ChangeSmallTab( m_dwCurSmallTabID );
}

void MyInventoryWnd::UpdateSmallTabSearch( ioHashString strName, OUT vSearchInfoVec& rkVec )
{
	// 검색관련 인자 가져오기
	std::string szFind = strName.c_str();
	if( !szFind.empty() )
	{
		std::transform( szFind.begin(), szFind.end(), szFind.begin(), tolower );

		int nIndex = 0;
		if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )		// 용병
		{			
			for( vSoldierItemInfoList::iterator iter = m_vCharInfoList.begin(); iter != m_vCharInfoList.end(); ++iter, ++nIndex )
			{
				SoldierItemInfo kInfo = *iter;

				ioHashString szClassName= g_MyInfo.GetClassName( kInfo.m_iClassType );
				if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsSearch(szClassName, strName)) )
				{
					SearchInfo sInfo;
					sInfo.m_nIndex = nIndex;
					sInfo.m_sName = szClassName;
					rkVec.push_back( sInfo );
				}
			}
		}
		else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN) || (m_dwCurTabID == ID_DECO_TAB_BTN) || (m_dwCurTabID == ID_ETC_TAB_BTN) ||
			     (m_dwCurTabID == ID_PRESENT_TAB_BTN) || (m_dwCurTabID == ID_COSTUME_TAB_BTN) || (m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )	// 2019-05-24 장비,치장,특별,보관함,코스튬, 액세서리
		{
			for( vInventoryItemInfoList::iterator iter = m_vInventoryItemInfoList.begin(); iter != m_vInventoryItemInfoList.end(); ++iter, ++nIndex )
			{
				InventoryItemInfo kInfo = *iter;
				const DecoData* pData = g_DecorationPrice.GetDecoDataPtr( kInfo.m_iSexType, kInfo.m_iDecoType, kInfo.m_iDecoCode );
				if( NULL != pData )
				{
					if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsSearch(pData->m_szName, strName)) )
					{
						SearchInfo sInfo;
						sInfo.m_nIndex = nIndex;
						sInfo.m_sName = pData->m_szName;
						rkVec.push_back( sInfo );
					}
				}
			}
		}
		else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )	// 정기
		{
			for( vAlchemicItemInfoList::iterator iter = m_vAlchemicItemInfoList.begin(); iter != m_vAlchemicItemInfoList.end(); ++iter, ++nIndex )
			{
				AlchemicItemInfo kInfo = *iter;
				if( (NULL != m_pNewSearchWnd) && (true == m_pNewSearchWnd->IsSearch(kInfo.m_szName, strName)) )
				{
					SearchInfo sInfo;
					sInfo.m_nIndex = nIndex;
					sInfo.m_sName = kInfo.m_szName;
					rkVec.push_back( sInfo );
				}
			}			
		}
	}
}

void MyInventoryWnd::OnMyInfoSelectEnter( DWORD dwItemInfoID )
{
	int nCurPage = dwItemInfoID / PAGE_PER_CHAR_NUM;	
	UpdatePage( nCurPage );

	ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
	if( NULL != pScroll )
	{
		pScroll->SetScrollPos( nCurPage );
	}

	DWORD dwSelectID = dwItemInfoID + ID_CHAR_INFO_WND_1_BTN;
	if( dwItemInfoID >= MAX_CHAR_INFO_WND_BTN )
	{
		dwSelectID = ((dwItemInfoID - MAX_CHAR_INFO_WND_BTN)%PAGE_PER_CHAR_NUM) + ID_CHAR_INFO_WND_1_BTN;
	}
	SelectCharItemBtn( dwSelectID, 0 );
	
}

void MyInventoryWnd::OnKeyProcess()
{
	if( (NULL == m_pNewSearchWnd) || (false == m_pNewSearchWnd->IsShow()) )
	{	
		return;
	}

	int nCurPageIndex  = -1;
	bool bKey = false;
	bool bFindKeyWordEmpty = m_pNewSearchWnd->IsFindKeyWordEmpty();
	if( (false == bFindKeyWordEmpty) && (false == m_pNewSearchWnd->IsKeyFocus()) )	// 2019-06-28
	{
		if( (true == g_Input.IsKeyUp(m_kKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wLeftKey)) )
		{
			m_nSelectCharIndex -= 1;
			if( m_nSelectCharIndex < 0 )
			{
				m_nSelectCharIndex = 0;
			}

			bKey = true;
		}
		else if( (true == g_Input.IsKeyUp(m_kKeys.m_wRightKey))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wRightKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wRightKey)) )
		{
			m_nSelectCharIndex += 1;
			 
			if( (ID_SOLDIER_TAB_BTN == m_dwCurTabID) && (m_nSelectCharIndex > (int)m_vCharInfoList.size()-1) )	// 용병
			{
				m_nSelectCharIndex = (int)m_vCharInfoList.size() - 1;
			}
			else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)		||	// 2019-06-13 장비(102),액세서리(108),코스튬(107),특별(105)
					 (m_dwCurTabID == ID_ETC_TAB_BTN)		||
					 (m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
				     /*(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||*/
				     /*(m_dwCurTabID == ID_DECO_TAB_BTN)	||*/
					 (m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )

			{
				if( m_nSelectCharIndex > (int)m_vInventoryItemInfoList.size()-1 )
				{
					m_nSelectCharIndex = (int)m_vInventoryItemInfoList.size() - 1;
				}
			}
			else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )	// 2019-06-10
			{
				if( m_nSelectCharIndex > (int)m_vAlchemicItemInfoList.size()-1 )
				{
					m_nSelectCharIndex = (int)m_vAlchemicItemInfoList.size() - 1;
				}
			}

			bKey = true;
		}
		else if( (true == g_Input.IsKeyUp( m_kKeys.m_wUpKey ))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wUpKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wUpKey)) )
		{
			if( ID_SOLDIER_TAB_BTN == m_dwCurTabID )
			{
				nCurPageIndex = m_iCurPage - 1;
				if( nCurPageIndex < 0 )
				{
					nCurPageIndex = 0;
				}

				m_nSelectCharIndex = m_nSelectCharIndex - PAGE_PER_CHAR_NUM;
				if( m_nSelectCharIndex < 0 )
				{		
					m_nSelectCharIndex += PAGE_PER_CHAR_NUM;
				}
			}
			else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)		||	// 2019-06-13 장비(102),액세서리(108),코스튬(107),특별(105)
					 (m_dwCurTabID == ID_ETC_TAB_BTN)		||
					 (m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
				     /*(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||*/
				     /*(m_dwCurTabID == ID_DECO_TAB_BTN)	||*/
					 (m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )
			{
				nCurPageIndex = (m_nSelectCharIndex/PAGE_ITEM_INFO_NUM) - 1;				
				if( nCurPageIndex < 0 )
				{
					nCurPageIndex = 0;
				}

				m_nSelectCharIndex = m_nSelectCharIndex - PAGE_ITEM_INFO_NUM;
				if( m_nSelectCharIndex < 0 )
				{		
					m_nSelectCharIndex +=PAGE_ITEM_INFO_NUM;
				}
			}
			else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )	// 2019-06-10
			{
				nCurPageIndex = (m_nSelectCharIndex/PAGE_PER_SPIRIT_NUM) - 1;				
				if( nCurPageIndex < 0 )
				{
					nCurPageIndex = 0;
				}

				m_nSelectCharIndex = m_nSelectCharIndex - PAGE_PER_SPIRIT_NUM;
				if( m_nSelectCharIndex < 0 )
				{		
					m_nSelectCharIndex +=PAGE_PER_SPIRIT_NUM;
				}
			}

			bKey = true;
		}
		else if( (true == g_Input.IsKeyUp(m_kKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wDownKey)) )
		{
			if( ID_SOLDIER_TAB_BTN == m_dwCurTabID )
			{
				int nSelectCharIndex = m_nSelectCharIndex + PAGE_PER_CHAR_NUM;
				if( nSelectCharIndex <= ((int)m_vCharInfoList.size() - 1) )
				{
					m_nSelectCharIndex = nSelectCharIndex;			
				}

				nCurPageIndex = m_nSelectCharIndex/PAGE_PER_CHAR_NUM;
			}
			else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)		||	// 2019-06-13 장비(102),액세서리(108),코스튬(107),특별(105)
					 (m_dwCurTabID == ID_ETC_TAB_BTN)		||
					 (m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
				     /*(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||*/
				     /*(m_dwCurTabID == ID_DECO_TAB_BTN)	||*/
					 (m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )
			{
				m_nSelectCharIndex = m_nSelectCharIndex + PAGE_ITEM_INFO_NUM;
				if( m_nSelectCharIndex == ((int)m_vInventoryItemInfoList.size()-1) )
				{		
					m_nSelectCharIndex = ((int)m_vInventoryItemInfoList.size()-1);
				}
				else if( m_nSelectCharIndex > ((int)m_vInventoryItemInfoList.size()-1) )
				{
					m_nSelectCharIndex = m_nSelectCharIndex - PAGE_ITEM_INFO_NUM;
				}

				nCurPageIndex = (m_nSelectCharIndex/PAGE_ITEM_INFO_NUM) - 1;
				if( nCurPageIndex < 0 )
				{
					nCurPageIndex = 0;
				}
			}
			else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN )	// 2019-06-10
			{
				m_nSelectCharIndex = m_nSelectCharIndex + PAGE_PER_SPIRIT_NUM;
				if( m_nSelectCharIndex == ((int)m_vAlchemicItemInfoList.size()-1) )
				{		
					m_nSelectCharIndex = ((int)m_vAlchemicItemInfoList.size()-1);
				}
				else if( m_nSelectCharIndex > ((int)m_vAlchemicItemInfoList.size()-1) )
				{
					m_nSelectCharIndex = m_nSelectCharIndex - PAGE_PER_SPIRIT_NUM;
				}

				nCurPageIndex = (m_nSelectCharIndex/PAGE_PER_SPIRIT_NUM) - 3;
				if( nCurPageIndex < 0 )
				{
					nCurPageIndex = 0;
				}
			}


			bKey = true;
		}
	}	

	if( (true == g_Input.IsKeyUp(KEY_ENTER)) || (true == g_Input.IsKeyUp(KEY_NUMENTER)) )	// 2019-06-28
	{
		if( (NULL != m_pNewSearchWnd) && (true == IsSearchInputEnter()) )
		{
			m_pNewSearchWnd->SetKeyFocus();	// 2019-07-03
			SetSearchInputEnter(false);
		}
		else if( (NULL != m_pNewSearchWnd) && (false == IsSearchInputEnter()) )
		{
			if( true != m_pNewSearchWnd->IsFindKeyWordEmpty() )
			{
 				SetSearchInputEnter(true);
				UpdateSmallTabSearchList();
			}
			else	// 2019-06-28
			{
				m_pNewSearchWnd->SetKeyFocus();
			}
		}

		return;
	}
	else if( (true == g_Input.IsKeyUp(KEY_SPACE)) && ((NULL != m_pNewSearchWnd) && (true != m_pNewSearchWnd->IsKeyFocus())) ) // 2019-06-28
	{
  		if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )		// 용병
		{
			m_iCurPage = m_nSelectCharIndex / PAGE_PER_CHAR_NUM;

			m_dwSelectCharID = m_nCurSelectCharIndex + ID_CHAR_INFO_WND_1_BTN;
		}
		else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)		||	// 2019-06-13 장비(102),액세서리(108),코스튬(107),특별(105)
			(m_dwCurTabID == ID_ETC_TAB_BTN)		||
			(m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
			/*(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||*/
			/*(m_dwCurTabID == ID_DECO_TAB_BTN)	||*/
			(m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )
		{
			m_iCurPage = m_nSelectCharIndex / PAGE_ITEM_INFO_NUM;
			if( m_iCurPage > 0 ) m_iCurPage = m_iCurPage - 1;

			m_dwSelectCharID = m_nCurSelectCharIndex + ID_ITEM_INFO_WND_1_BTN;
		}
		else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN ) // 2019-06-13 정기 
		{
			m_iCurPage = 0;
			if( (m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM) > 3 )
			{
				m_iCurPage = (m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM) - 3;
			}

			m_dwSelectCharID = m_nCurSelectCharIndex + ID_SPIRIT_INFO_1;
		}

		ioWnd *pWnd = FindChildWnd( m_dwSelectCharID );
		if( NULL != pWnd )
		{
			iwm_command(pWnd, IOBN_BTNUP, 0);
			return;
		}

		bKey = false;
	}

	if( true == bKey )
	{
		int nCurPage = 0;
		if( m_dwCurTabID == ID_SOLDIER_TAB_BTN )	// 용병
		{
			nCurPage = m_nSelectCharIndex / PAGE_PER_CHAR_NUM;	
			if( nCurPageIndex > -1 )
			{
				nCurPage = nCurPageIndex;
			}

			DWORD dwSelectID = m_nSelectCharIndex + ID_CHAR_INFO_WND_1_BTN;
			if( m_nSelectCharIndex >= MAX_CHAR_INFO_WND_BTN )
			{
				dwSelectID = ((m_nSelectCharIndex - MAX_CHAR_INFO_WND_BTN)%PAGE_PER_CHAR_NUM) + ID_CHAR_INFO_WND_1_BTN;
			}

			// 최종
			m_nCurSelectCharIndex = dwSelectID - ID_CHAR_INFO_WND_1_BTN;

		}
		else if( (m_dwCurTabID == ID_EXTRA_TAB_BTN)		||	// 2019-06-13 장비(102),액세서리(108),코스튬(107),특별(105)
			(m_dwCurTabID == ID_ETC_TAB_BTN)		||
			(m_dwCurTabID == ID_COSTUME_TAB_BTN)	||
			/*(m_dwCurTabID == ID_PRESENT_TAB_BTN)	||*/
			/*(m_dwCurTabID == ID_DECO_TAB_BTN)	||*/
			(m_dwCurTabID == ID_ACCESSORY_TAB_BTN) )	
		{
			
			if( (true == g_Input.IsKeyUp(m_kKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wLeftKey)) )
			{
				nCurPage = m_nSelectCharIndex / PAGE_ITEM_INFO_NUM;
				if(  nCurPage > m_iCurPage ) 
				{
					nCurPage = nCurPage - 1;
				}
			}
			else
			{
				nCurPage = (m_nSelectCharIndex / PAGE_ITEM_INFO_NUM) - 1;				
				if( nCurPage < m_iCurPage ) nCurPage = m_iCurPage;
			}

			if( (true == g_Input.IsKeyUp(m_kKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wDownKey)) )
			{
				if( (nCurPageIndex > -1) && (m_iCurPage < nCurPageIndex) )	nCurPage = nCurPageIndex;
			}
			else
			{
				if( nCurPageIndex > -1 )	nCurPage = nCurPageIndex;
			}
			

			m_nCurSelectCharIndex = m_nSelectCharIndex;
			if( m_nSelectCharIndex >= PAGE_ITEM_INFO_NUM )
			{
				if( (true == g_Input.IsKeyUp( m_kKeys.m_wUpKey )) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wUpKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wUpKey)) )
				{
					m_nCurSelectCharIndex = (m_nSelectCharIndex - PAGE_ITEM_INFO_NUM) % PAGE_ITEM_INFO_NUM;	// 위
				}
				else if( (true == g_Input.IsKeyUp(m_kKeys.m_wRightKey))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wRightKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wRightKey)) )					      
				{
					// 오른쪽
					m_nCurSelectCharIndex = (m_nSelectCharIndex % PAGE_ITEM_INFO_NUM) + PAGE_ITEM_INFO_NUM;
					int nPage  = (m_nSelectCharIndex / PAGE_ITEM_INFO_NUM) - 1;
					if( nPage < m_iCurPage )
					{
						m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_ITEM_INFO_NUM;
					}					
				}
				else if( (true == g_Input.IsKeyUp(m_kKeys.m_wDownKey))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wDownKey)) )
				{
					m_nCurSelectCharIndex = (m_nSelectCharIndex % PAGE_ITEM_INFO_NUM) + PAGE_ITEM_INFO_NUM;	// 아래
					if( m_iCurPage > nCurPageIndex )
					{
						m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_ITEM_INFO_NUM;
					}
				}
				else if( (true == g_Input.IsKeyUp(m_kKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wLeftKey)) )
				{
					// 왼쪽										
					m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_ITEM_INFO_NUM;
					if( m_nSelectCharIndex / PAGE_ITEM_INFO_NUM > 0 ) m_nCurSelectCharIndex += PAGE_ITEM_INFO_NUM;

					if( (m_nSelectCharIndex / PAGE_ITEM_INFO_NUM) <= m_iCurPage )
					{
						m_nCurSelectCharIndex -= PAGE_ITEM_INFO_NUM;
					}					
				}
			}
		}
		else if( m_dwCurTabID == ID_ALCHEMIC_TAB_BTN ) // 2019-06-10 정기 
		{			
			if( (true == g_Input.IsKeyUp(m_kKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wLeftKey)) )
			{
				// 왼쪽				
				nCurPage = m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM;
				if(  nCurPage > m_iCurPage ) 
				{
					nCurPage = m_iCurPage;
				}
			}
			else
			{
				// 오른쪽
				if( m_nSelectCharIndex >= MAX_SPIRIT_BTN )
				{
					nCurPage = (m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM) - 3;
				}

				if( m_iCurPage > nCurPage )
				{
					nCurPage = m_iCurPage;
				}				

			}

			if( (true == g_Input.IsKeyUp(m_kKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wDownKey)) ) 
			{
				if( (nCurPageIndex > -1) && (m_iCurPage < nCurPageIndex) )
				{
					nCurPage = nCurPageIndex;
				}
			}
			else
			{
				if( (nCurPageIndex > -1) && (m_iCurPage > nCurPageIndex) )
				{
					nCurPage = nCurPageIndex;
				}
			}	

			m_nCurSelectCharIndex = m_nSelectCharIndex;
			if( m_nSelectCharIndex >= MAX_SPIRIT_BTN )
			{
				if( (true == g_Input.IsKeyUp(m_kKeys.m_wRightKey))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wRightKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wRightKey)) ||
					(true == g_Input.IsKeyUp(m_kKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wDownKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wDownKey)) ) 
				{					
					m_nCurSelectCharIndex = (m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM) + (MAX_SPIRIT_BTN - PAGE_PER_SPIRIT_NUM);	// 오른쪽
					int nPage = m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM;
					nPage = nPage - 3;					
					if( m_iCurPage > nPage )
					{
						nPage = m_iCurPage - nPage;
						if( nPage <= 3 )
						{
							m_nCurSelectCharIndex = (m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM) + (PAGE_PER_SPIRIT_NUM * (3-nPage));
						}
					}
				}
				else if( (true == g_Input.IsKeyUp(m_kKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wLeftKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wLeftKey)) ||
					     (true == g_Input.IsKeyUp( m_kKeys.m_wUpKey )) || (true == g_Input.IsKeyUp(m_kSecondKeys.m_wUpKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wUpKey)) )
				{
					// 왼쪽		
					m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM;
					if( m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM > 0 )
					{						
						int nPage = m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM;
						if( (0 == (nPage - m_iCurPage)) || (m_iCurPage > nPage) )
						{
							// 0 ~ 4
						}
						else if( 1 == ( nPage - m_iCurPage ) )
						{
							// 5 ~ 9
							m_nCurSelectCharIndex = m_nCurSelectCharIndex + (MAX_SPIRIT_BTN - (PAGE_PER_SPIRIT_NUM*3));
						}
						else if( 2 == (nPage - m_iCurPage ) )
						{
							// 10 ~ 14
							m_nCurSelectCharIndex = m_nCurSelectCharIndex + (MAX_SPIRIT_BTN - (PAGE_PER_SPIRIT_NUM*2));
						}						
						else
						{  
							// 15 ~ 19
							m_nCurSelectCharIndex = m_nCurSelectCharIndex + (MAX_SPIRIT_BTN - PAGE_PER_SPIRIT_NUM);
						}
					}
				}
			}
			else if( m_iCurPage > 0 )
			{
				if( (true == g_Input.IsKeyUp(m_kKeys.m_wRightKey))	|| (true == g_Input.IsKeyUp(m_kSecondKeys.m_wRightKey)) || (true == g_Input.IsKeyUp(m_kJoyKeys.m_wRightKey)) ) 
				{
					int nPage = m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM;
					m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM;
					if( 1 == nPage )
					{
						// 0 ~ 4
					}
					else if( 2 == nPage )
					{
						// 5 ~ 9
						m_nCurSelectCharIndex = m_nCurSelectCharIndex + PAGE_PER_SPIRIT_NUM;
					}
					else if( 3 == nPage )
					{
						// 10 ~ 14
						m_nCurSelectCharIndex = m_nCurSelectCharIndex + (PAGE_PER_SPIRIT_NUM * 2);
					}
				}
				else
				{
					int nPage = m_nSelectCharIndex / PAGE_PER_SPIRIT_NUM;
					if( nPage >= m_iCurPage )
					{
						m_nCurSelectCharIndex = (m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM) + (PAGE_PER_SPIRIT_NUM * (nPage-m_iCurPage));
					}
					else
					{
						m_nCurSelectCharIndex = m_nSelectCharIndex % PAGE_PER_SPIRIT_NUM;
					}
				}
			}
		}

		UpdatePage( nCurPage );
		ioScroll *pScroll = dynamic_cast<ioScroll*>( FindChildWnd( ID_VERT_SCROLL ) );
		if( NULL != pScroll )
		{
			pScroll->SetScrollPos( nCurPage );
		}
	}
}

// 2019-07-01
void MyInventoryWnd::ClearSearchHide()
{
	ioWnd* pWnd = FindChildWnd( ID_NEW_SEARCH_WND );
	if( (NULL != pWnd) && (true == pWnd->IsShow()) )
	{
		HideChildWnd( ID_NEW_SEARCH_WND );					
		HideChildWnd( ID_SEARCH_HIDE_BTN );
		ShowChildWnd( ID_SEARCH_SHOW_BTN );

		if( ID_ETC_TAB_BTN == m_dwCurTabID )
		{
			ShowChildWnd( ID_HOUSE_ITEM_BTN );	// 개인본부 꾸미기 아이템 
		}
		else if( ID_COSTUME_TAB_BTN == m_dwCurTabID )
		{
			ShowChildWnd( ID_COSTUME_HELP );	// 코스튬 도움말
		}

		SetSearchInputEnter(false);

		ChangeSmallTab( m_dwCurSmallTabID );
	}
}

void MyInventoryWnd::ShowToastPopup()
{
	ShowChildWnd( ID_MULTI_SUPER_GASHAPON_TOAST_POPUP_WND );
}