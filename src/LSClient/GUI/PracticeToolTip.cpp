#include "StdAfx.h"

#include "../ioDateHelp.h"

#include "PracticeToolTip.h"

#include "LobbyMainWnd.h"
#include "PracticeMainWnd.h"
#include "NewShopIconBtn.h"
#include "MyInventoryWnd.h"
#include "MyInventoryInfoWnd.h"

PracticeToolTip::PracticeToolTip()
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

PracticeToolTip::~PracticeToolTip()
{
	ClearDesc();

	SAFEDELETE( m_pGrowthLineLeft );
	SAFEDELETE( m_pGrowthLineCenter );
	SAFEDELETE( m_pGrowthLineRight );
	SAFEDELETE( m_pGrowthBackGray );
	SAFEDELETE( m_pGrowthBackLight );
}

void PracticeToolTip::OnProcess( float fTimePerSec )
{
	if( !g_GUIMgr.IsShow( LOBBY_MAIN_WND ) )
	{
		HideWnd();
		return;
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	InventoryItemInfo kInfo;
	ioWnd* pWnd = g_GUIMgr.FindWnd(LOBBY_MAIN_WND);
	PracticeLobbyInfoWnd *pLobby = dynamic_cast<PracticeLobbyInfoWnd*> ( pWnd->FindChildWnd(8) );
	if(!pLobby)
		return;
	if(!pLobby->IsShow())
		return;

	DWORD dwid = pLobby->GetOveredIcon();
	pLobby->GetCurItemInfo(kInfo, dwid);

	if(!dwid)
	{
		HideWnd();
		return;
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

void PracticeToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { PRINT_XOFFSET = 14, FIRST_YOFFSET = 99, SECOND_YOFFSET = 207,   };
	RenderDesc( iXPos, iYPos );
	RenderGrowth( iXPos, iYPos + FIRST_YOFFSET );

	int iPrintYPos = iYPos + FIRST_YOFFSET;
	if( m_iMainType == INVENTORY_ITEM_EXTRAITEM ||
		m_iMainType == INVENTORY_ITEM_MEDALITEM )
		iPrintYPos = iYPos + SECOND_YOFFSET;

	int iPrintManualYPos = iPrintYPos;
	PrintManual( iXPos+PRINT_XOFFSET, iPrintManualYPos, FONT_SIZE_12 );
}

void PracticeToolTip::ClearDesc()
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

void PracticeToolTip::RenderDesc( int iXPos, int iYPos )
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



void PracticeToolTip::PrintManual( int a_iXPos, int a_iYPos , float fScale )
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

#ifdef POPUPSTORE
	else if( m_iMainType == POPUPSTORE_ITEM_PRESENT )
		pManual = ioPopupStorePresentItemManualMgr::GetSingletonPtr();
#endif

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
			//			enum { MAX_LOOP = 20, Y_GAP = 18, };
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
						//						a_iYPos += Y_GAP;
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
					//					a_iYPos += Y_GAP;
					a_iYPos += LINE_GAP;
				kPrinterA.PrintFullText( a_iXPos, a_iYPos, TAT_LEFT );
				kPrinterB.CopyFullText( kPrinter );
				kPrinterA.ClearList();
				kPrinterB.ClearList();
			}
		}
	}	
}

bool PracticeToolTip::SetInfo( InventoryItemInfo &rkInfo )
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

	m_iMainType    = rkInfo.m_iMainType;
	m_iMagicCode   = rkInfo.m_iMagicCode;
	m_iMagicCodeEx = rkInfo.m_iMagicCodeEx;
	m_iMagicCodeEx2= rkInfo.m_iMagicCodeEx2;
	m_iManualIndex = kToolTiopItemInfo.m_iManualIndex;
	
	return true;
}


void PracticeToolTip::GetItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetExtraItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetExtraItemInfoByItemCode( IN int iItemCode , IN int iReinforce, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetDecoItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
{
	const DecoData *pData =  g_DecorationPrice.GetDecoDataPtr( rkInfo.m_iSexType, rkInfo.m_iDecoType, rkInfo.m_iDecoCode );
	if( !pData ) 
		return;

	rkReturnInfo.m_sName = pData->m_szName;
	rkReturnInfo.m_sIcon = pData->m_szIcon;
	rkReturnInfo.m_iLimitLevel  = pData->m_iLimitLevel;
	rkReturnInfo.m_iManualIndex = pData->m_iManual - 1; // index -1
}

void PracticeToolTip::GetEtcItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetMedalItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetPresentItemInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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
		rkReturnInfo.m_sIcon =  g_MyInfo.GetMySoldierIconName( iClassType );
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
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
}

void PracticeToolTip::GetCostumeInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

void PracticeToolTip::GetAccessoryInfo( IN const InventoryItemInfo &rkInfo, OUT MToolTipItemInfo &rkReturnInfo )
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

	rkReturnInfo.m_sName = kSlot.m_Name;
	rkReturnInfo.m_sNameAdd = kSlot.GetNameAdd();
	rkReturnInfo.m_sIcon = kSlot.m_IconName;
	rkReturnInfo.m_nGradeType = pItem->GetGradeType();
}

DWORD PracticeToolTip::GetUseInfo( OUT ioHashString &rsInfo )
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



void PracticeToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void PracticeToolTip::RenderGrowth( int iXPos, int iYPos )
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

void PracticeToolTip::Clear()
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

void PracticeToolTip::UpdateWndSize()
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

		//		enum { DEFAULT_HEIGHT = 123, LINE_GAP = 18, GROWTH_HEIGHT = 108, };
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
				//				enum { MAX_LOOP = 20, Y_GAP = 18, };
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
							//							iTotalLineGap += Y_GAP;
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
						//						iTotalLineGap += Y_GAP;
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

void PracticeToolTip::GetPopupItemInfo( int iPresentType, int iMent, int iPresentValue1, int iPresentValue2, OUT MToolTipItemInfo &rkReturnInfo )
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
		rkReturnInfo.m_iManualIndex = sPresentMent;
	}
}