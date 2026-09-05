#include "StdAfx.h"
#include "GradeAdjustmentWnd.h"

#include "../ioPowerUpManager.h"
#include "ReinforceSelectWnd.h"
#include "SmallTabButton.h"
#include "GlobalWnd.h"

#include <vector>
#include <algorithm>
using namespace std;

bool CompareReinforce(EXTRAITEMSLOT a, EXTRAITEMSLOT b)
{
	return (a.m_iReinforce > b.m_iReinforce);
}

GradeAdjustmentWnd::GradeAdjustmentWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;

	m_pEditFrm			= NULL;
	m_pDotLineRight		= NULL;
	m_pDotLineBottom	= NULL;

	m_pMagicCircle	= NULL;
	m_pIconBack		= NULL;
	m_pIconSlot   = NULL;
	m_pIcon			= NULL;
	m_pNumText		= NULL;
	m_pPlusText		= NULL;

	m_iTabLineSize = 0;
	m_iTabPosX     = 0;

	m_dwCurSmallTabID = ID_SMALL_TAB_1_BTN;
	m_dwSmallTabEndID = ID_SMALL_TAB_6_BTN;

	m_bOpen = false;
	m_bReinforce = false;
	m_bShowResultEffect = false;
	m_eGradeUp = PUTT_NONE;

	m_szEffectName.Clear();
	m_szSuccessSound.Clear();
	m_SelectCharInfoVec.clear();
	m_SelectItemInfoVec.clear();
	m_HasNotItemVec.clear();
	m_CurrSelectInfoVec.clear();

	HeroReinforceClearListAll();
	m_pDarkTextBack = NULL;
	m_pLightTextBack = NULL;
	m_pSoldierInfoBG = NULL;
	m_CharInfo.Init();

	// ItemReinforce
	m_pLightImage = NULL;
	m_pSuccessLightImage = NULL;
	m_pItemInfoBG = NULL;
	m_iSelectArray = 0;
	m_iSelectValue = 0;

	ItemReinforceClearListAll();

	m_fScaleRate = 0.0f;
	m_fAlphaRate = 0.0f;
	m_fSlopeRate = 0.0f;
	m_dwResultEndTime = 0;
}

GradeAdjustmentWnd::~GradeAdjustmentWnd()
{
	SAFEDELETE( m_pEditFrm );
	SAFEDELETE( m_pDotLineRight );
	SAFEDELETE( m_pDotLineBottom );
	m_SelectCharInfoVec.clear();
	m_SelectItemInfoVec.clear();
	m_CurrSelectInfoVec.clear();

	SAFEDELETE(m_pMagicCircle);
	SAFEDELETE(m_pIconBack);
	SAFEDELETE( m_pIconSlot );
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pLightImage);
	SAFEDELETE(m_pSuccessLightImage);
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
	SAFEDELETE( m_pDarkTextBack );
	SAFEDELETE( m_pLightTextBack );
	SAFEDELETE(m_pItemInfoBG);

	for (int i = 0; i < ID_TAB_COUNT_MAX ; i++)
	{
		m_vSmallTabNameVecArray[i].clear();
		m_vSmallTabTypeVecArray[i].clear();
	}

	ItemReinforceClearListAll();
}

void GradeAdjustmentWnd::ItemReinforceClearListAll()
{
	m_ItemInfo.Clear();
	m_ItemReinforceInfo.Release();
	m_SoulStoneCountText.Release();
	m_ItemSelectPrinter.ClearList();

	m_dwMagicEffectCreateTime = 0;
	m_vMagicEffectInfoList.clear();

	m_bShowResultEffect = false;
	m_dwResultStartTime = 0;
	m_dwResultEndTime = 0;
	m_dwResultEffectEndTime = 0;
	m_dwRollingEffectCheckTime = 0;

	m_iAlpha1 = 0;
	m_iAlpha2 = 0;
	m_fCurEffectScale = 1.3f;
	m_iItemIndex = -1;
	m_iItemReinforce = 0;
	m_iCurSuccessEffectArray = -1;

	m_iIconRenderType = UI_RENDER_NORMAL;
	if ( m_pIcon )
		m_pIcon->SetColor( 0, 0, 0 );

	m_nGradeType = 0;

	SAFEDELETE(m_pSuccessLightImage);
}

void GradeAdjustmentWnd::ShowSubWnd( int iGradeAdjust_InfoType, int iItemValue )
{
	if( !COMPARE( iGradeAdjust_InfoType, ioPowerUpManager::PIT_HERO, ioPowerUpManager::PIT_MAX ) )
		return;

	m_iSelectValue = iItemValue;
	m_SoldierReinforcePrinter.ClearList();

	if(iGradeAdjust_InfoType == ioPowerUpManager::PIT_ITEM)
		m_iSelectArray = iItemValue;
	ChangeSmallTab(ID_SMALL_TAB_1_BTN);
	m_InfoType = (ioPowerUpManager::PowerUpInfoType)iGradeAdjust_InfoType;
	if(!m_bOpen)
		SetAllItemInfoVec();

	const TabUISettingVec& TabVec = g_PowerUpManager.GetTabUISettinVec( iGradeAdjust_InfoType );
	int iTabCount = (int)TabVec.size();
	int i = 0;
	if ( m_InfoType == ioPowerUpManager::PIT_HERO )
	{
		ItemReinforceClearListAll();
		SAFEDELETE( m_pIcon );
		GradeAdjustmentCategoryTab* pButton = dynamic_cast<GradeAdjustmentCategoryTab*>( FindChildWnd( ID_TAB_1_BTN ) );
		if( pButton )
		{
			if( i < (int)TabVec.size() )
				pButton->SetTab( TabVec );
			else
				pButton->ClearTab();
		}

		m_iTabLineSize = max(0, MAX_HERO_TAB_BTN - iTabCount) * 73;
		m_iTabPosX     = iTabCount * 73;
		ChangeTab( ID_TAB_1_BTN, "" );
	}
	else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
	{
		HeroReinforceClearListAll();

		GradeAdjustmentCategoryTab* pButton = dynamic_cast<GradeAdjustmentCategoryTab*>( FindChildWnd( ID_TAB_2_BTN ) );
		if( pButton )
		{
			if( i < (int)TabVec.size() )
				pButton->SetTab( TabVec );
			else
				pButton->ClearTab();
		}

		m_iTabLineSize = max(0, MAX_ITEM_TAB_BTN - iTabCount) * 87;
		m_iTabPosX     = iTabCount * 87;
		ChangeTab( ID_TAB_2_BTN, "" );
	}

	m_szCurrFindKeyword.Clear();

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	m_TitlePrinter.ClearList();
	m_TitlePrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	m_TitlePrinter.SetBkColor( 12, 66, 111 );
	m_TitlePrinter.SetTextColor( TCT_DEFAULT_WHITE );
	//if ( m_InfoType == ioPowerUpManager::PIT_HERO )
	//	m_TitlePrinter.AddTextPiece( FONT_SIZE_18, STR(1) );
	//else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
	//	m_TitlePrinter.AddTextPiece( FONT_SIZE_18, STR(2) );
	m_TitlePrinter.AddTextPiece( FONT_SIZE_18, STR(1) );

	if(!m_bOpen)
		ShowWnd();

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( GradeAdjustmentWnd::ID_SOLDIER_WND ) );
	if( pCharWnd )
	{
		if ( m_InfoType == ioPowerUpManager::PIT_HERO )
			pCharWnd->ShowWnd();
		else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
			pCharWnd->HideWnd();
	}
	Update();
	if(!m_bOpen)
		m_bOpen = true;
}

ioHashString GradeAdjustmentWnd::CheckGrade( int iCode )
{
	char szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	int iGradType = g_MyInfo.GetCharPowerUpGradeByClassType( iCode );
	ioHashString szGradeTitle;
	ioHashString szGradeFullTitle;
	if(m_InfoType == ioPowerUpManager::PIT_HERO)
	{
		if( iGradType == PUGT_NONE )
		{
			SafeSprintf( szText, sizeof( szText ), STR(1) );
			szGradeFullTitle = szText;
		}
		else
		{
			szGradeTitle = g_PowerUpManager.GetCharGradeTitle( iCode, iGradType );
			SafeSprintf( szText, sizeof( szText ), STR(2), szGradeTitle.c_str() );
			szGradeFullTitle = szText;
		}
	}
	else if(m_InfoType == ioPowerUpManager::PIT_ITEM)
	{
		int iEquipSlotIndex = iCode / DEFAULT_BASIC_ITEM_CODE;
		int iGradType;
		if ( iEquipSlotIndex == ES_WEAPON )
			iGradType = ConvertPowerUpItemToItemGrade( iCode );
		else
			iGradType = ConvertPowerUpRareItemToRareItemGrade( iCode );

		const ioItem *pItem = g_ItemMaker.GetItemConst( iCode, __FUNCTION__ );
		if( !pItem )
			return szGradeFullTitle;

		int iEnableClass = pItem->GetEnableClass();
		memset( szText, 0, sizeof(szText) );

		if( iGradType == PUGT_ITEM_GRADE_BASE || iGradType == PURIGT_RARE_ITEM_GRADE_BASE )
		{
			SafeSprintf( szText, sizeof( szText ), STR(3) );
		}
		else 
		{
			if ( iEquipSlotIndex == ES_WEAPON )
			{
				ioHashString szGradeTitleName = g_PowerUpManager.GetItemGradeTitle( iCode, iGradType );
				if( !szGradeTitleName.IsEmpty() )
					SafeSprintf( szText, sizeof( szText ), STR(4), szGradeTitleName.c_str() );
			}
			else
			{
				ioHashString szGradeTitleName = g_PowerUpManager.GetExtraItemGradeTitle( iCode, iGradType );
				if( !szGradeTitleName.IsEmpty() )
					SafeSprintf( szText, sizeof( szText ), STR(4), szGradeTitleName.c_str() );
			}
		}
	}
	szGradeFullTitle = szText;
	return szGradeFullTitle;
}

void GradeAdjustmentWnd::SetAllItemInfoVec()
{
	char szBuf[MAX_PATH], szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	memset( szBuf, 0, sizeof(szBuf) );
	m_SelectCharInfoVec.clear();
	m_SelectItemInfoVec.clear();
	vGradeInfo HasNotCharVec;
	HasNotCharVec.clear();
	std::vector< int > vHasCharCode;
	vHasCharCode.clear();

	bool bCheckChar = true;

	//if ( m_InfoType == ioPowerUpManager::PIT_HERO )
	{
		for( int i = 0; i < g_PowerUpManager.GetPowerUpCharCount(); ++i )
		{
			int iClassType = g_PowerUpManager.GetPowerUpCharClassType( i );
			ioHashString szError;

			if ( !g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError) )
			{
				if(!g_PowerUpManager.CheckHasChar(iClassType))
					bCheckChar = false;
				else
					continue;
			}
			else
				bCheckChar = true;
			int iSetIdx = iClassType - 1;
			const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
			if( !pInfo )
				continue;

			if( !g_MyInfo.IsCharMortmain( iClassType, true ) )
				continue;

			int iGradType = g_MyInfo.GetCharPowerUpGradeByClassType( iClassType );
			if( iGradType == PUGT_NONE )
			{
				if(bCheckChar)
					SafeSprintf( szText, sizeof( szText ), STR(1) );
				else
					SafeSprintf( szText, sizeof( szText ), STR(2) );
			}
			else
			{
				ioHashString szGradeTitle = g_PowerUpManager.GetCharGradeTitle( iClassType, iGradType );
				if ( szGradeTitle.IsEmpty() )
				{
					for(int j = 0; j < (int)HasNotCharVec.size(); j++)
					{
						m_SelectCharInfoVec.push_back(HasNotCharVec[j]);
					}
					HasNotCharVec.clear();
					return;
				}

				SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitle.c_str() );
			}

			int iLevel = g_MyInfo.GetClassLevel( iClassType, false );
			//SafeSprintf( szBuf, sizeof( szBuf ), "Lv%d %s", iLevel, g_MyInfo.GetClassName( iClassType ) );
			StringCbPrintf_e( szBuf, sizeof( szBuf ), "Lv%d %s", iLevel, g_MyInfo.GetClassName( iClassType ) );

			GradeInfo stGradeInfo;
			stGradeInfo.m_iType = ioPowerUpManager::PIT_HERO;
			stGradeInfo.m_iTabType = pInfo->GetCharAttackType();
			stGradeInfo.m_iValue = iClassType;
			stGradeInfo.m_iValue3 = iClassType;
			stGradeInfo.m_szName = szBuf;
			stGradeInfo.m_szSubDesc = szText;
			stGradeInfo.m_szIconName = g_MyInfo.GetMySoldierIconName( iClassType );
			stGradeInfo.m_iPriorityOrder = iClassType;

			int iCharGradeType = g_UIImageRenderMgr.GetPowerUpGradeType(iClassType);

			if(!bCheckChar)
			{
				stGradeInfo.m_bHasPowerUp = false;
				stGradeInfo.m_iPriorityOrder += 10000000;
				if (iCharGradeType == 1 || iCharGradeType == 0)		// 찾지 못했고 레어등급이 아닌 장비라면...
					stGradeInfo.m_iPriorityOrder += 5000000;
			}
			else if (iCharGradeType == 1 || iCharGradeType == 0)		// 레어등급이 아닌 장비라면...
				stGradeInfo.m_iPriorityOrder += 5000000;
			if(bCheckChar)
				m_SelectCharInfoVec.push_back( stGradeInfo );
			else
				HasNotCharVec.push_back(stGradeInfo);
			memset( szText, 0, sizeof(szText) );
			memset( szBuf, 0, sizeof(szBuf) );
		}

		for(int j = 0; j < (int)HasNotCharVec.size(); j++)
		{
			HasNotCharVec[j].m_bHasPowerUp = false;
			m_SelectCharInfoVec.push_back(HasNotCharVec[j]);
		}
		HasNotCharVec.clear();
	}
	//else if ( m_InfoType == ioPowerUpManager::PIT_ITEM )
	{
		// solt check
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
			return;
		bool bHasCheckItem = true;

		int cnt = pExtraItem->GetExtraItemCnt();
		// 가지고 있지 않은 진화 장비 아이템 담기
		const IntVec EnableClassNumVec = g_PowerUpManager.GetEnablePowerUpClassNumberInfo();
		if ( EnableClassNumVec.empty() )
			return;

		for ( int a = 0 ; a < (int)EnableClassNumVec.size(); ++a )
		{
			IntVec iItemCodeVec;
			iItemCodeVec.push_back( EnableClassNumVec[a] + PUGT_ITEM_GRADE1 + PUGT_ITEM_EXTREA );
			iItemCodeVec.push_back( EnableClassNumVec[a] + PUGT_ITEM_GRADE2 + PUGT_ITEM_EXTREA );
			iItemCodeVec.push_back( EnableClassNumVec[a] + PUGT_ITEM_GRADE3 + PUGT_ITEM_EXTREA );
			iItemCodeVec.push_back( EnableClassNumVec[a] + PUGT_ITEM_GRADE4 + PUGT_ITEM_EXTREA );
			iItemCodeVec.push_back( EnableClassNumVec[a] + PUGT_ITEM_GRADE5 + PUGT_ITEM_EXTREA );
			bool bTotalAdd = false;
			for( int i = 0; i < (int)iItemCodeVec.size(); i++ )
			{
				bool bCurAdd = SetItemInfoVec(iItemCodeVec[i], false);
				if( bCurAdd )
					bTotalAdd = true;
			}
			int iItemCode = EnableClassNumVec[a] + DEFAULT_EXTRA_ITEM_CODE;
			SetItemInfoVec(iItemCode, !bTotalAdd);
		}

		const IntVec EnableExtraItemNumVec = g_PowerUpManager.GetEnablePowerUpExtraItemNumberInfo( 0, true );
		if ( EnableExtraItemNumVec.empty() )
			return;
		for ( int k = 0 ; k < (int)EnableExtraItemNumVec.size(); ++k )
		{
			int iItemCode = EnableExtraItemNumVec[k];
			SetItemInfoVec(iItemCode);
		}

		// 가지고 있지 않은 진화 장비 아이템 담기
		for(int j = 0; j < (int)m_HasNotItemVec.size(); j++)
		{
			m_HasNotItemVec[j].m_bHasPowerUp = false;

			m_SelectItemInfoVec.push_back(m_HasNotItemVec[j]);
		}
		m_HasNotItemVec.clear();
	}
}

// 진화 장비 아이템 보유/미보유 구분하여 담는 함수
bool GradeAdjustmentWnd::SetItemInfoVec(int iItemCode, bool bNotItemAdd)
{
	char szBuf[MAX_PATH], szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	memset( szBuf, 0, sizeof(szBuf) );

	bool bHasCheckItem = true;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	int cnt = pExtraItem->GetExtraItemCnt();

	EXTRAITEMSLOT ExtraItem;
	if( !pExtraItem )
		return false;

	const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
	if ( !pItem )
		return false;

	ExtraItemList vExtraItem;
	vExtraItem.clear();
	bool bFindItem = false;

	pExtraItem->FindAllItem( iItemCode, vExtraItem );
	if ( !vExtraItem.empty() )
		bFindItem = true;

	if(!bFindItem)
	{
		ExtraItem.m_iItemCode = iItemCode;
		ExtraItem.m_Name = pItem->GetName();
		ExtraItem.m_IconName = g_ItemMaker.GetItemLargeIcon( iItemCode, 0 );
		vExtraItem.push_back( ExtraItem );
	}

	for ( int j= 0 ; j < (int)vExtraItem.size() ; ++j )
	{
		if ( pExtraItem )
		{
			int iUseMtrlCount,iCurMtrlCount;
			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM ,vExtraItem[j].m_iItemCode, iUseMtrlCount, iCurMtrlCount ) )
				bHasCheckItem = false;
			else
				bHasCheckItem = true;
			ioHashString szError;
			if ( !g_PowerUpManager.CheckEnableItemPowerUp( vExtraItem[j].m_iIndex, szError) )
				bHasCheckItem = false;
			else
				bHasCheckItem = true;

			int iEquipSlotIndex = vExtraItem[j].m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
			int iGradType;
			if ( iEquipSlotIndex == ES_WEAPON )
				iGradType = ConvertPowerUpItemToItemGrade( vExtraItem[j].m_iItemCode );
			else
				iGradType = ConvertPowerUpRareItemToRareItemGrade( vExtraItem[j].m_iItemCode );

			const ioItem *pItem = g_ItemMaker.GetItemConst( vExtraItem[j].m_iItemCode, __FUNCTION__ );
			if( !pItem )
				return false;

			int iEnableClass = pItem->GetEnableClass();
			memset( szText, 0, sizeof(szText) );
			if( iGradType == PUGT_ITEM_GRADE_BASE || iGradType == PURIGT_RARE_ITEM_GRADE_BASE )
			{
				if(bFindItem)
					SafeSprintf( szText, sizeof( szText ), STR(1) );
				else
					SafeSprintf( szText, sizeof( szText ), STR(2) );
			}
			else 
			{
				if ( iEquipSlotIndex == ES_WEAPON )
				{
					ioHashString szGradeTitleName = g_PowerUpManager.GetItemGradeTitle( vExtraItem[j].m_iItemCode, iGradType );
					if( !szGradeTitleName.IsEmpty() )
						SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitleName.c_str() );
				}
				else
				{
					ioHashString szGradeTitleName = g_PowerUpManager.GetExtraItemGradeTitle( vExtraItem[j].m_iItemCode, iGradType );
					if( !szGradeTitleName.IsEmpty() )
						SafeSprintf( szText, sizeof( szText ), STR(3), szGradeTitleName.c_str() );
				}
			}
			EquipItemExtendType eExtendType = GetEquipItemExtendType( vExtraItem[j].m_iItemCode );

			GradeInfo stGradeInfo;
			stGradeInfo.m_iType = ioPowerUpManager::PIT_ITEM;
			stGradeInfo.m_iTabType = iEquipSlotIndex + 1;
			stGradeInfo.m_iValue = vExtraItem[j].m_iIndex;
			stGradeInfo.m_iValue2 = vExtraItem[j].m_iReinforce;
			stGradeInfo.m_iValue3 = vExtraItem[j].m_iItemCode;
			stGradeInfo.m_szName = vExtraItem[j].m_Name;
			stGradeInfo.m_szSubDesc = szText;
			stGradeInfo.m_szIconName = vExtraItem[j].m_IconName;
			stGradeInfo.m_iPriorityOrder = vExtraItem[j].m_iItemCode;
			if(!bFindItem)
			{
				stGradeInfo.m_iPriorityOrder += 10000000;
				if (pItem->GetGradeType() == 1 || pItem->GetGradeType() == 0)		// 찾지 못했고 레어등급이 아닌 장비라면...
					stGradeInfo.m_iPriorityOrder += 5000000;
			}
			else if (pItem->GetGradeType() == 1 || pItem->GetGradeType() == 0)		// 레어등급이 아닌 장비라면...
				stGradeInfo.m_iPriorityOrder += 5000000;

			memset( szText, 0, sizeof(szText) );

			if ( iEquipSlotIndex == ES_WEAPON )
			{
				SafeSprintf( szText, sizeof( szText ), STR(4), g_MyInfo.GetClassName( iEnableClass, ConvertPowerUpItemToChar( iGradType ) ) );
				stGradeInfo.m_szWeaponDesc = szText;
			}
			if(bFindItem)
			{
				m_SelectItemInfoVec.push_back( stGradeInfo );		// 미보유 진화 장비 아이템 담기
			}
			else if( bNotItemAdd )
			{
				stGradeInfo.m_bHasPowerUp = false;
				stGradeInfo.m_iValue = 1000 + (int)m_HasNotItemVec.size();

				int iCheckCode = stGradeInfo.m_iValue3 % 10000;
				iCheckCode = iCheckCode / 1000;

				if(iCheckCode == 1 || pItem->GetType() == ioItem::IT_WEAPON)
					m_HasNotItemVec.push_back( stGradeInfo );		// 미보유 진화 장비 아이템 담기
			}

			memset( szText, 0, sizeof(szText) );
			memset( szBuf, 0, sizeof(szBuf) );
		}
	}
	return bFindItem;
}

void GradeAdjustmentWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	HeroReinforceClearListAll();
	ItemReinforceClearListAll();
	SAFEDELETE( m_pIcon );
	m_bOpen = false;
}

void GradeAdjustmentWnd::iwm_show()
{
	ShowChildWnd( ID_EXIT_BTN );
	ShowChildWnd( ID_FIND_BTN );

	ShowChildWnd( GradeAdjustmentWnd::ID_REINFORCE_BTN );
	ShowChildWnd( GradeAdjustmentWnd::ID_DEGENERATION_BTN );
	ShowChildWnd( GradeAdjustmentWnd::ID_SOLDIER_WND );
}

void GradeAdjustmentWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos, false );
}

void GradeAdjustmentWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioWnd::iwm_wheel( zDelta );
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

void GradeAdjustmentWnd::SetSoldierInfo( int iClassType )
{
	if ( g_MyInfo.GetClassArray( iClassType ) == -1 )
	{
		LOG.PrintTimeAndLog( 0, "HeroReinforceDownWnd::SetSoldierInfo - iClassType is not exist" );

		ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_REINFORCE_BTN ) );
		pButton->SetInActive();
		pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_DEGENERATION_BTN ) );
		pButton->SetInActive();
		//return;
	}
	else
	{
		ioPowerUpManager::PowerUpImpossibleFunc eCheckFunc = g_PowerUpManager.CheckEnableCharGrade(iClassType);
		RightButtonControl( eCheckFunc );
		//ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_REINFORCE_BTN ) );
		//pButton->SetActive();
		//pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_DEGENERATION_BTN ) );
		//pButton->SetActive();
	}

	m_CharInfo.Init();
	m_CharInfo = g_MyInfo.GetCharacterToClassType( iClassType );
	m_CharInfo.m_class_type = iClassType;
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( GradeAdjustmentWnd::ID_SOLDIER_WND ) );
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( FLOAT500 );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( false );
		pCharWnd->SetCharScale( 0.85f );
		if ( !m_szEffectName.IsEmpty() && m_bReinforce )
		{
			pCharWnd->CharAttachEffect( m_szEffectName );
			if( !m_szSuccessSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
		}
	}

	int iSoulStoneCount = 0;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			iSoulStoneCount = kSlot.m_iValue1;
	}

	HeroReinforceClearListAll();	
	char szText[MAX_PATH];
	ioHashString szGradeTitle;
	if( m_CharInfo.m_eCharPowerUpType != PUGT_NONE )
	{
		szGradeTitle = g_PowerUpManager.GetCharGradeTitle( m_CharInfo.m_class_type, m_CharInfo.m_eCharPowerUpType );
		if ( szGradeTitle.IsEmpty() )
			return;

		SafeSprintf( szText, sizeof( szText ), STR(1), szGradeTitle.c_str() );
		m_SoldierInfo = szText;
	}

	m_ReinforcedSoldierInfo.Set( 0, 0, g_PowerUpManager.GetCharGardeString( m_CharInfo.m_class_type ).c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);

	wsprintf( szText, m_szSoulStoneText.c_str(), iSoulStoneCount );
	m_SoulStoneCountText.Set( 0, 0, szText, FONT_SIZE_11, 200, TS_NORMAL, 0xFF6A6869, 0, TAT_LEFT, TVA_TOP, 3);

	const ioHashString& szClassName = g_MyInfo.GetClassName( m_CharInfo.m_class_type ) ;
	SafeSprintf( szText, sizeof( szText ), STR(2), g_MyInfo.GetClassLevel( m_CharInfo.m_class_type, false ), szClassName.c_str() );

	SafeSprintf( szText, sizeof( szText ), STR(3),  szGradeTitle.c_str() );
	m_SoldierSelectPrinter.SetTextStyle( TS_NORMAL );
	m_SoldierSelectPrinter.SetBkColor( 0, 0, 0 );
	m_SoldierSelectPrinter.SetTextColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ) );
	m_SoldierSelectPrinter.AddTextPiece( FONT_SIZE_12, szText );

	if(g_PowerUpManager.GetPowerUp() == PUTT_CHAR)
		SafeSprintf( szText, sizeof( szText ), STR(3),  szGradeTitle.c_str() );
	else if(g_PowerUpManager.GetPowerUp() == PDTT_CHAR)
	{
		if(!szGradeTitle.IsEmpty())
			SafeSprintf( szText, sizeof( szText ), STR(4),  szGradeTitle.c_str() );
		else
			SafeSprintf( szText, sizeof( szText ), STR(5) );
	}
	else if (g_PowerUpManager.GetPowerUp() == PUTT_NONE)
		return;

	m_SoldierReinforcePrinter.SetTextStyle( TS_NORMAL );
	m_SoldierReinforcePrinter.SetBkColor( 0, 0, 0 );	
	m_SoldierReinforcePrinter.SetTextColor( TCT_DEFAULT_GREEN );
	m_SoldierReinforcePrinter.AddTextPiece( FONT_SIZE_17, szText );
}

void GradeAdjustmentWnd::SetItemInfo( int iIndex, bool bClicked, GradeAdjustmentInfoWnd *pSelectItem )
{
	if(!bClicked)
		m_dwFinalResultStartTime = FRAMEGETTIME();

	ItemReinforceClearListAll();
	//soulstone check
	int iSoulStoneCount = 0;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			iSoulStoneCount = kSlot.m_iValue1;
	}

	// solt check
	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		LOG.PrintConsoleLog( 0, "ItemReinforceDownWnd::SetItemInfo - ioUserExtraItem is not exist");
		return;
	}

	if ( m_pIcon != NULL )
		SAFEDELETE( m_pIcon );

	EXTRAITEMSLOT ExtraItem;
	ExtraItem.Init();
	if ( !pExtraItem->GetExtraItem( iIndex, ExtraItem ) )
	{
		//m_ItemReinforceInfo.Set( 0, 0, m_szNoItemDesc.c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);

		//m_ItemSelectPrinter.SetTextStyle( TS_NORMAL );
		//m_ItemSelectPrinter.SetBkColor( 0, 0, 0 );	
		//m_ItemSelectPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		//m_ItemSelectPrinter.AddTextPiece( FONT_SIZE_12, STR(9) );
		if(pSelectItem)
		{
			ExtraItem.m_iIndex			= pSelectItem->GetPowerUpValue1();
			ExtraItem.m_iItemCode		= pSelectItem->GetPowerUpValue3();
			ExtraItem.m_Name			= pSelectItem->GetSlotItemName();
			ExtraItem.m_IconName		= pSelectItem->GetSlotItemIconName();
		}
		else
			return;
	}

	//enable power up check
	ioHashString szError;
	if ( !g_PowerUpManager.CheckEnableItemPowerUp( iIndex, szError ) )
		LOG.PrintConsoleLog( 0, "ItemReinforceDownWnd::SetItemInfo - %s",szError.c_str() );

	m_iItemIndex = iIndex;

	m_iItemReinforce = ExtraItem.m_iReinforce;

	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( ExtraItem.m_IconName );

	const ioItem *pItem = g_ItemMaker.GetItemConst( ExtraItem.m_iItemCode, __FUNCTION__ );
	if( pItem )
		m_nGradeType = pItem->GetGradeType();

	ioPowerUpManager::PowerUpImpossibleFunc eCheckFunc = g_PowerUpManager.CheckEnableItemGrade(ExtraItem.m_iIndex, ExtraItem.m_iItemCode);
	RightButtonControl( eCheckFunc );

	char szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	//아이템
	int iEquipSlotIndex = ExtraItem.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
	if( iEquipSlotIndex == ES_WEAPON )
	{
		int iGrade = ConvertPowerUpItemToItemGrade( ExtraItem.m_iItemCode );
		if ( IsPowerUpItemGrade( iGrade, true ) )
		{
			ioHashString szGradeTitleName = g_PowerUpManager.GetItemGradeTitle( ExtraItem.m_iItemCode, iGrade );
			if( !szGradeTitleName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(1), szGradeTitleName.c_str() );
		}
	}
	else
	{
		int iGrade = ConvertPowerUpRareItemToRareItemGrade( ExtraItem.m_iItemCode );
		if ( IsPowerUpRareItemGrade( iGrade, true ) )
		{
			ioHashString szGradeTitleName = g_PowerUpManager.GetExtraItemGradeTitle( ExtraItem.m_iItemCode, iGrade );
			if( !szGradeTitleName.IsEmpty() )
				SafeSprintf( szText, sizeof( szText ), STR(1), szGradeTitleName.c_str() );
		}
	}
	m_ItemInfo = szText;

	if ( iEquipSlotIndex == ES_WEAPON )
		m_ItemReinforceInfo.Set( 0, 0, g_PowerUpManager.GetItemGardeString( ExtraItem.m_iItemCode, iEquipSlotIndex ).c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);
	else
		m_ItemReinforceInfo.Set( 0, 0, g_PowerUpManager.GetExtraItemGardeString( ExtraItem.m_iItemCode, iEquipSlotIndex ).c_str(), FONT_SIZE_12, 194, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);

	m_ItemSelectPrinter.SetTextStyle( TS_NORMAL );
	m_ItemSelectPrinter.SetBkColor( 0, 0, 0 );	
	m_ItemSelectPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_ItemSelectPrinter.AddTextPiece( FONT_SIZE_12, ExtraItem.m_Name.c_str() );

	wsprintf( szText, m_szSoulStoneText.c_str(), iSoulStoneCount );
	m_SoulStoneCountText.Set( 0, 0, szText, FONT_SIZE_11, 200, TS_NORMAL, 0xFF6A6869, 0, TAT_LEFT, TVA_TOP, 3);
}

void GradeAdjustmentWnd::ResetSoulStone()
{
	char szText[MAX_PATH];
	memset( szText, 0, sizeof(szText) );
	int iSoulStoneCount = 0;
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			iSoulStoneCount = kSlot.m_iValue1;
	}

	wsprintf( szText, m_szSoulStoneText.c_str(), iSoulStoneCount );
	m_SoulStoneCountText.Set( 0, 0, szText, FONT_SIZE_11, 200, TS_NORMAL, 0xFF6A6869, 0, TAT_LEFT, TVA_TOP, 3);
}

void GradeAdjustmentWnd::HeroReinforceClearListAll()
{
	m_SoldierInfo.Clear();
	m_ReinforcedSoldierInfo.Release();
	m_SoulStoneCountText.Release();
	m_SoldierSelectPrinter.ClearList();
	m_SoldierReinforcePrinter.ClearList();
}

void GradeAdjustmentWnd::iwm_create()
{
	for( int i = 0; i < MAX_SMALL_TAB_BTN; ++i )
	{
		ioWnd *pWnd = g_GUIMgr.AddWndByTemplate( ioHashString("SmallTabButton"), this, ID_SMALL_TAB_1_BTN + i );
		if( pWnd )
			pWnd->SetWndPos( 27 + i * 53, 97 );
	}
}

void GradeAdjustmentWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
		//case ID_TAB_3_BTN:
		//case ID_TAB_4_BTN:
		//case ID_TAB_5_BTN:
		//case ID_TAB_6_BTN:
		//case ID_TAB_7_BTN:
		//case ID_TAB_8_BTN:
		//case ID_TAB_9_BTN:
		//case ID_TAB_10_BTN:
		//case ID_TAB_11_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

			m_iSelectArray = iStartArray;
			g_PowerUpManager.SetPowerUp(PUTT_NONE);
			//ChangeTab( dwID, "" );
			if(dwID == ID_TAB_1_BTN)
				ShowSubWnd( ioPowerUpManager::PIT_HERO );
			else if(dwID == ID_TAB_2_BTN)
				ShowSubWnd( ioPowerUpManager::PIT_ITEM );
		}
		break;
	case ID_SMALL_TAB_1_BTN:
	case ID_SMALL_TAB_2_BTN:
	case ID_SMALL_TAB_3_BTN:
	case ID_SMALL_TAB_4_BTN:
	case ID_SMALL_TAB_5_BTN:
	case ID_SMALL_TAB_6_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeSmallTab(dwID);
			g_PowerUpManager.SetPowerUp(PUTT_NONE);
			SetCurItemInfoVec( m_dwCurTabID, "" );
			Update();

			int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;

			m_iSelectArray = iStartArray;

			for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
			{
				GradeAdjustmentInfoWnd *pItem = dynamic_cast<GradeAdjustmentInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
				if( !pItem )
					continue;
				int iCurArray = iStartArray + i;
				if( m_iSelectArray == iCurArray )
				{
					m_iSelectValue = pItem->GetPowerUpValue1();
					break;
				}
			}
		}
		break;
	case ID_REINFORCE_BTN:
	case ID_DEGENERATION_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if(dwID == ID_REINFORCE_BTN)
			{
				if(m_dwCurTabID == ID_TAB_1_BTN)
					m_eGradeUp =  PUTT_CHAR;
				else if(m_dwCurTabID == ID_TAB_2_BTN)
					m_eGradeUp =  PUTT_ITEM;
			}
			else if(dwID == ID_DEGENERATION_BTN)
			{
				if(m_dwCurTabID == ID_TAB_1_BTN)
					m_eGradeUp =  PDTT_CHAR;
				else if(m_dwCurTabID == ID_TAB_2_BTN)
					m_eGradeUp =  PDTT_ITEM;
			}
			CheckReinforceButton();
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
			g_PowerUpManager.SetPowerUp(PUTT_NONE);
			GradeAdjustmentInfoWnd *pItem = dynamic_cast<GradeAdjustmentInfoWnd*>( FindChildWnd(dwID) );
			GradeAdjustmentInfoWnd *pOtherItem = NULL;
			for(DWORD dwItemInfoID = ID_ITEM_INFO_WND_1_BTN; dwItemInfoID < ID_ITEM_INFO_WND_8_BTN+1; dwItemInfoID++)
			{
				pOtherItem = dynamic_cast<GradeAdjustmentInfoWnd*>( FindChildWnd(dwItemInfoID) );
				if(dwItemInfoID == dwID)
				{
					pOtherItem->SetSelect(true);
					m_iSelectValue = pOtherItem->GetPowerUpValue1();
					m_iSelectArray = (m_iCurPage * PAGE_PER_ITEM_NUM) + (dwID - ID_ITEM_INFO_WND_1_BTN);
				}
				else
					pOtherItem->SetSelect(false);
			}
			if (pItem)
				SetSelectedReinforceItem( pItem, true );
		}
		break;
	case ID_FIND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetFind();
		}
		break;
	case MESSAGE_BOX3:
		{
			if( cmd == IOBN_BTNUP )
			{
				if( param == IDYES )
				{
					Reinforce();
				}
			}
		}
		break;
	}
}

bool GradeAdjustmentWnd::SetSelectedReinforceItem( GradeAdjustmentInfoWnd *pSelectItem, bool bSlotClick )
{
	if ( pSelectItem->GetPowerUpType() == ioPowerUpManager::PIT_HERO )
	{
		ioHashString szError;
		szError.Clear();
		int iClassType = pSelectItem->GetPowerUpValue1();

		// PowerUp이 가능한지 체크
		if( !g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError, false ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
			return false;
		}

		SetSoldierInfo( iClassType );
	}
	else if ( pSelectItem->GetPowerUpType() == ioPowerUpManager::PIT_ITEM )
	{
		ioHashString szError;
		szError.Clear();

		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
			return false;

		EXTRAITEMSLOT Slot;
		int iItemCode = 0;
		if( !pExtraItem->GetExtraItem( pSelectItem->GetPowerUpValue1(), Slot ) )
			iItemCode = pSelectItem->GetPowerUpValue3();
		else
			iItemCode = Slot.m_iItemCode;

		if ( !g_PowerUpManager.CheckEnableItemPowerUp( pSelectItem->GetPowerUpValue1(), szError, false ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
			return false;
		}
		SetItemInfo( Slot.m_iIndex, true, pSelectItem );
	}
	else
		return false;
	return true;
}

void GradeAdjustmentWnd::CheckReinforceButton()
{
	bool bGradeUp = false;
	if(m_eGradeUp == PUTT_CHAR || m_eGradeUp == PUTT_ITEM)
		bGradeUp = true;	
	if(m_InfoType == ioPowerUpManager::PIT_HERO)
	{
		int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
		if( iCharArray == g_MyInfo.GetSelectCharArray() )
		{
			bool bCheck = true;
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->GetPlayStage() )
			{
				ioMyRoomMode *pMyRoom = ToMyRoomMode( pInvenWnd->GetPlayStage()->GetPlayMode() );
				if( pMyRoom && pMyRoom->GetMyRoomType() == MRT_LOBBY )
					bCheck = false;
			}

			if( bCheck )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				return;
			}
		}

		int iUseMtrlCount,iCurMtrlCount;
		if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,m_CharInfo.m_class_type, iUseMtrlCount, iCurMtrlCount, bGradeUp ) )
		{
			SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
			if ( pSoulstoneBuyWnd )
				pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
			return;
		}

		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		char szBuf[MAX_PATH];
		memset( szBuf, 0, sizeof(szBuf) );
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2), iUseMtrlCount );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, szBuf );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		if(m_eGradeUp ==  PUTT_CHAR)
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(3) );
		else if(m_eGradeUp ==  PDTT_CHAR)
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
	}
	else if (m_InfoType == ioPowerUpManager::PIT_ITEM)
	{
		// solt check
		EXTRAITEMSLOT kSlot;
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			return;
		}

		EXTRAITEMSLOT ExtraItem;
		ExtraItem.Init();
		if ( !pExtraItem->GetExtraItem( m_iItemIndex, ExtraItem ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			return;
		}

		int iUseMtrlCount,iCurMtrlCount;
		if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM ,ExtraItem.m_iItemCode, iUseMtrlCount, iCurMtrlCount, bGradeUp ) )
		{
			SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
			if ( pSoulstoneBuyWnd )
				pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
			return;
		}

		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		char szBuf[MAX_PATH];
		memset( szBuf, 0, sizeof(szBuf) );

		if ( ExtraItem.m_dwMaleCustom || ExtraItem.m_dwFemaleCustom )
		{
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(7) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(8) );

			kPrinter[2].SetTextStyle( TS_NORMAL );
			kPrinter[2].SetBkColor( 0, 0, 0 );
			kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(9) );
		}
		else if( ExtraItem.m_iTradeState != EET_DISABLE )
		{
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			SafeSprintf( szBuf, sizeof( szBuf ), STR(10), iUseMtrlCount );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, szBuf );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(11) );
		}
		else
		{
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			SafeSprintf( szBuf, sizeof( szBuf ), STR(10), iUseMtrlCount );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, szBuf );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			if(m_eGradeUp ==  PUTT_ITEM)
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(12) );
			else if(m_eGradeUp ==  PDTT_ITEM)
				kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(13) );

		}
		g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
	}
}

void GradeAdjustmentWnd::Reinforce()
{
	bool bGradeUp = true;
	if(m_eGradeUp == PDTT_CHAR || m_eGradeUp == PDTT_ITEM)
		bGradeUp = false;
	if(m_InfoType == ioPowerUpManager::PIT_HERO)
	{
		int iClassType	= m_CharInfo.m_class_type;
		int iUseMtrlCount,iCurMtrlCount;
		if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,iClassType, iUseMtrlCount, iCurMtrlCount, bGradeUp ) )
		{
			SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
			if ( pSoulstoneBuyWnd )
				pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
			return;
		}

		ioHashString szError;
		if( g_PowerUpManager.CheckEnableCharPowerUp( iClassType, szError ) )
		{
			//CheckEnableCharPowerUp함수에서 유효한 값인지 체크됨
			int iCharIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
			ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
			if( pEtcItem )
			{
				pEtcItem->OnUseGradeAdjustment( iCharIndex, m_eGradeUp );
				m_bShowResultEffect = true;
				m_bReinforce = true;
			}
		}
	}
	else if (m_InfoType == ioPowerUpManager::PIT_ITEM)
	{
		if ( m_iItemIndex == -1 )
			return;

		// solt check
		EXTRAITEMSLOT kSlot;
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 슬롯 에러" );
			return;
		}

		EXTRAITEMSLOT ExtraItem;
		ExtraItem.Init();
		if ( !pExtraItem->GetExtraItem( m_iItemIndex, ExtraItem ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 슬롯을 찾을 수 없습니다." );
			return;

		}

		ioHashString szError;
		if( !g_PowerUpManager.CheckEnableItemPowerUp( m_iItemIndex, szError ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
			return;
		}

		int iUseMtrlCount,iCurMtrlCount;
		if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM, ExtraItem.m_iItemCode, iUseMtrlCount, iCurMtrlCount, bGradeUp ) )
		{
			SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
			if ( pSoulstoneBuyWnd )
				pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
			return;
		}

		ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
		if( pEtcItem )
		{
			pEtcItem->OnUseGradeAdjustment( m_iItemIndex, m_eGradeUp );
			m_bReinforce = true;
			m_dwResultStartTime = FRAMEGETTIME();
			m_dwResultEndTime = m_dwResultStartTime + m_dwResultEffectTime;
			m_dwResultEffectEndTime = m_dwResultEndTime;
			m_bShowResultEffect = true;
			m_iIconRenderType = UI_RENDER_NORMAL_ADD_COLOR;
			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
		}
	}
}

void GradeAdjustmentWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	ioWnd::ParseExtraInfo( xElement );

	////////////////////서브 탭
	ioHashString sSmallTabName;
	int          iType = 0;
	for (int i = 0; i < ID_TAB_COUNT_MAX ; i++)
	{
		char szName[MAX_PATH]="";

		StringCbPrintf_e( szName, sizeof( szName ), "Max_%d", ID_TAB_1_BTN+i );
		int iMax = xElement.GetIntAttribute( szName );
		for (int j = 0; j < iMax ; j++)
		{
			StringCbPrintf_e( szName, sizeof( szName ), "Name_%d_%d", ID_TAB_1_BTN+i, j+1 );
			sSmallTabName = xElement.GetStringAttribute( szName );
			m_vSmallTabNameVecArray[i].push_back( sSmallTabName );

			StringCbPrintf_e( szName, sizeof( szName ), "Type_%d_%d", ID_TAB_1_BTN+i, j+1 );
			iType = xElement.GetIntAttribute( szName );
			m_vSmallTabTypeVecArray[i].push_back( iType );
		}
	}
	/////////////////////////////////////////////

	m_szEffectName = xElement.GetStringAttribute_e( "powerup_effect" );

	m_szSoulStoneText = xElement.GetStringAttribute_e( "soulstonetext" );

	m_fScaleRate = xElement.GetFloatAttribute_e( "scale_rate" );
	m_fAlphaRate = xElement.GetFloatAttribute_e( "alpha_rate" );
	m_fSlopeRate = xElement.GetFloatAttribute_e( "slope_rate" );
	m_szNoItemDesc = xElement.GetStringAttribute_e( "NoItemDesc" );

	m_dwResultEffectTime = xElement.GetIntAttribute_e( "result_effect_Time" );
	m_szRollingSound = xElement.GetStringAttribute_e( "rolling_sound" );

	// Success Effect
	m_dwSuccessEffectTime = xElement.GetIntAttribute_e( "success_effect_Time" );
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vSuccessLightImage.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vSuccessLightImage.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;

		m_vSuccessLightImage.push_back( szEffect );
	}
	m_szSuccessSound = xElement.GetStringAttribute_e( "success_sound" );
}

void GradeAdjustmentWnd::UpdatePage( int iNewPage, bool bUpdateRightItem )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;
	if(m_iSelectValue != -1 && !m_bOpen)
	{
		int j = 0;
		for(j = 0; j < (int)m_CurrSelectInfoVec.size(); j++)
		{
			if( m_iSelectValue == m_CurrSelectInfoVec[ j ].m_iValue )
				break;
		}
		m_iCurPage = j / 4;
	}
	int iItemSize = m_CurrSelectInfoVec.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		GradeAdjustmentInfoWnd *pItem = dynamic_cast<GradeAdjustmentInfoWnd*>( FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i) );
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray + i;

		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			pItem->SetBlank();
			continue;
		}
		// 등급 갱신
		if(m_bReinforce)
		{
			int iCodeOrIndex = 0;
			if(m_InfoType == ioPowerUpManager::PIT_HERO)
			{
				iCodeOrIndex = g_PowerUpManager.GetReinforcedCode();
				if (iCodeOrIndex != 0 && iCodeOrIndex == m_CurrSelectInfoVec[ iCurArray ].m_iValue3)
				{
					m_CurrSelectInfoVec[ iCurArray ].m_szSubDesc = CheckGrade(iCodeOrIndex);
					m_CurrSelectInfoVec[ iCurArray ].m_szIconName = g_MyInfo.GetMySoldierIconName( m_CurrSelectInfoVec[ iCurArray ].m_iValue3 );
				}
			}
			else if(m_InfoType == ioPowerUpManager::PIT_ITEM)
			{
				iCodeOrIndex = g_PowerUpManager.GetReinforcedItemIndex();
				if(iCodeOrIndex != 0 && iCodeOrIndex == m_CurrSelectInfoVec[ iCurArray ].m_iValue)
				{
					m_CurrSelectInfoVec[ iCurArray ].m_szSubDesc = CheckGrade(g_PowerUpManager.GetReinforcedCode());
					int iCode = g_PowerUpManager.GetReinforcedCode();
					if((iCode/DEFAULT_EXTRA_ITEM_CODE) == 1 && (iCode % DEFAULT_EXTRA_ITEM_CODE) >= DEFAULT_RARE_ITEM_CODE)
						iCode = iCode - DEFAULT_RARE_ITEM_CODE;
					const ioItem *pItem = g_ItemMaker.GetItemConst( iCode, __FUNCTION__ );
					if ( pItem )
					{
						m_CurrSelectInfoVec[ iCurArray ].m_szName = pItem->GetName();
						m_CurrSelectInfoVec[ iCurArray ].m_szIconName = pItem->GetItemLargeIconName();
						int iEquipSlotIndex = iCode / DEFAULT_BASIC_ITEM_CODE;
						int iGradType;
						if ( iEquipSlotIndex == ES_WEAPON )		// 무기 장비 슬롯에서 제일 아래 장착 가능한 용병 텍스트 표시
						{
							iGradType = ConvertPowerUpItemToItemGrade( iCode );
							char szText[MAX_PATH];
							memset( szText, 0, sizeof(szText) );
							SafeSprintf( szText, sizeof( szText ), STR(1), g_MyInfo.GetClassName( pItem->GetEnableClass(), ConvertPowerUpItemToChar( iGradType ) ) );
							m_CurrSelectInfoVec[ iCurArray ].m_szWeaponDesc = szText;
						}
					}
				}
			}
		}

		pItem->SetPowerUpInfo( m_CurrSelectInfoVec[ iCurArray ]);

		// SetInfo 이후 적용
		if( (m_iSelectValue == m_CurrSelectInfoVec[ iCurArray ].m_iValue) || (m_iSelectValue == -1 && iCurArray == 0) )
		{
			pItem->SetSelect( true );
			if(bUpdateRightItem)
				SetSelectedReinforceItem( pItem, false );
		}
		else
		{
			pItem->SetSelect( false );
		}
	}
}

void GradeAdjustmentWnd::ChangeTab( DWORD dwID, const char *szFind  )
{
	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_TAB_1_BTN, ID_TAB_2_BTN, dwID );
		m_dwCurTabID = dwID;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_TAB_1_BTN; i < ID_TAB_2_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}

	SetCurItemInfoVec( m_dwCurTabID, szFind );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = 0;
		ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_REINFORCE_BTN ) );
		if(dwID == ID_TAB_1_BTN)
		{
			iItemSize = m_SelectCharInfoVec.size();
			if(pButton)
				pButton->SetTitleText( STR(1) );
		}
		else if(dwID == ID_TAB_2_BTN)
		{
			iItemSize = m_SelectItemInfoVec.size();
			if(pButton)
				pButton->SetTitleText( STR(2) );
		}

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}

	int iArray = dwID-ID_TAB_1_BTN;
	if( !COMPARE( iArray, 0, ID_TAB_COUNT_MAX ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Array is wrong.", __FUNCTION__ );
		return;
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


	if( strcmp( szFind, "" ) == 0 )
	{
		CheckRadioButton( ID_SMALL_TAB_1_BTN, m_dwSmallTabEndID, ID_SMALL_TAB_1_BTN );
		m_dwCurTabID = dwID;
		m_bReinforce = false;
	}
	else
	{
		// 검색이 있으면 모든 탭 선택되지 않은 상태로 변경
		for (int i = ID_SMALL_TAB_1_BTN; i < ID_SMALL_TAB_6_BTN+1 ; i++)
		{
			ioRadioButton *pButton = dynamic_cast<ioRadioButton*> ( FindChildWnd( i ) );
			if( !pButton )
				continue;

			pButton->SetClick( false );
		}
	}
}

int GradeAdjustmentWnd::UpdateSmallTabName( ioWnd* pWnd, int iXOffset, const ioHashString& szTitle )
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

DWORD GradeAdjustmentWnd::GetDefaultSmallTab( DWORD dwMainTab )
{
	int iSmallTab = 0;

	//if( dwMainTab == ID_ALCHEMIC_TAB_BTN )
	//{
	//	return ID_SMALL_TAB_1_BTN;
	//}

	int iArray = ( dwMainTab - ID_TAB_1_BTN );
	if( COMPARE( iArray, 0, ID_TAB_COUNT_MAX ) )
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

void GradeAdjustmentWnd::ChangeSmallTab( DWORD dwID )
{
	CheckRadioButton( ID_SMALL_TAB_1_BTN, m_dwSmallTabEndID, dwID );
	m_dwCurSmallTabID = dwID;
}

void GradeAdjustmentWnd::SetCurItemInfoVec( DWORD dwTabID, const char *szFind )
{
	vGradeInfo vTotalList;
	if(dwTabID == ID_TAB_1_BTN)
	{
		vTotalList = m_SelectCharInfoVec;
		if( m_SelectCharInfoVec.empty() )
		{
			return;
		}
	}
	else if(dwTabID == ID_TAB_2_BTN)
	{
		vTotalList = m_SelectItemInfoVec;
		if(m_SelectItemInfoVec.empty())
			return;
	}

	//vPowerUpInfo vTotalList = m_SelectPowerUpInfoVec;
	m_CurrSelectInfoVec.clear();

	bool bFind = false;
	if( strcmp( szFind, "" ) != 0 )
		bFind = true;

	int iSize = vTotalList.size();
	for( int i = 0; i < iSize ; i++ )
	{
		const GradeInfo& rkInfo = vTotalList[i];
		if( bFind )
		{
			ioHashString szItemName = rkInfo.m_szName;
			//소문자 변환
			std::string szName = szItemName.c_str();
			std::transform( szName.begin(), szName.end(), szName.begin(), tolower );

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->GetType() == ioLocalManager::LCT_KOREA )
			{
				//조합형 글자 비교(초성 비교 초함)
				if( !Help::Strstr( szName.c_str(), szFind ) )
					continue;
			}
			else
			{
				if( strstr( szName.c_str(), szFind ) == NULL )
					continue;
			}
		}
		else
		{
			if( !IsRightTab( dwTabID, rkInfo.m_iTabType ) )
				continue;
		}

		m_CurrSelectInfoVec.push_back( vTotalList[i] );
	}

	if( m_CurrSelectInfoVec.empty() )
		return;

	if( m_CurrSelectInfoVec[0].m_iType == ioPowerUpManager::PIT_HERO )
		return;


	GradeAdjustmentCategoryTab* pButton = dynamic_cast<GradeAdjustmentCategoryTab*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		const TabUISetting& rkSetting = pButton->GetTab();
		switch( rkSetting.m_iOrder )
		{
		case TabUISetting::ORDER_BY_SETTING:
			{
				std::sort( m_CurrSelectInfoVec.begin(), m_CurrSelectInfoVec.end(), GradeAdjustmentInfoSortByPriority() );
			}
			break;
		default:
			{
				std::sort( m_CurrSelectInfoVec.begin(), m_CurrSelectInfoVec.end(), GradeAdjustmentInfoSortByPriority() );
			}
			break;
		}
	}
}

void GradeAdjustmentWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		X_TITLE_OFFSET = 13,
		Y_TITLE_OFFSET = 11,

		X_OFFSET = 3,
		Y_OFFSET = 442,
	};
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit )
	{
		int iEditX = pEdit->GetXPos();
		int iEditY = pEdit->GetYPos();
		if( m_pEditFrm )
			m_pEditFrm->Render( iXPos + pEdit->GetXPos() - 5, iYPos + pEdit->GetYPos() - 5 );
	}

	if( m_pDotLineRight )
	{
		m_pDotLineRight->SetSize( m_iTabLineSize, 2 );
		m_pDotLineRight->Render( iXPos + m_iTabPosX, iYPos );
	}

	m_TitlePrinter.PrintFullText( iXPos + X_TITLE_OFFSET, iYPos + Y_TITLE_OFFSET, TAT_LEFT );

	/////////////////////////

	if(m_InfoType == ioPowerUpManager::PIT_ITEM)
	{
		OnRenderMagicCircle();
		if( m_bShowResultEffect )
			OnRenderResultEffect();
	}
}

void GradeAdjustmentWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{	
		REINFORCED_OFFSET_X = 128,
		REINFORCED_OFFSET_Y = 424,
	};

	ResetSoulStone();

	if( m_pDarkTextBack )
	{
		m_pDarkTextBack->Render( iXPos + 523, iYPos + 274, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( iXPos + 523, iYPos + 316, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( iXPos + 523, iYPos + 358, UI_RENDER_MULTIPLY );
		m_pDarkTextBack->Render( iXPos + 523, iYPos + 400, UI_RENDER_MULTIPLY );
	}
	if( m_pLightTextBack )
	{
		m_pLightTextBack->Render( iXPos + 523, iYPos + 296, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( iXPos + 523, iYPos + 338, UI_RENDER_MULTIPLY );
		m_pLightTextBack->Render( iXPos + 523, iYPos + 380, UI_RENDER_MULTIPLY );
	}

	if( !m_SoldierInfo.IsEmpty() )
	{
		if( m_pSoldierInfoBG )
			m_pSoldierInfoBG->Render( iXPos + 523, iYPos + 256, UI_RENDER_MULTIPLY );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 102 + 523, iYPos + 256, FONT_SIZE_13, m_SoldierInfo.c_str() );
	}


	m_ReinforcedSoldierInfo.OnRender( iXPos + 525, iYPos + 277 );
	m_ItemReinforceInfo.OnRender( iXPos + 525, iYPos + 277 );
	m_SoulStoneCountText.OnRender( iXPos + 532, iYPos + 64 );

	// ex)'1차 진화 되었습니다' 문구 출력
	if ( !m_SoldierReinforcePrinter.IsEmpty()/* && m_bReinforce*/ )
		m_SoldierReinforcePrinter.PrintFullText( iXPos + 102 + 523, iYPos + 430, TAT_CENTER );

	if(m_InfoType == ioPowerUpManager::PIT_ITEM)
	{
		if( !m_ItemInfo.IsEmpty() )
		{
			if( m_pSoldierInfoBG )
				m_pSoldierInfoBG->Render( iXPos + 523, iYPos + 256, UI_RENDER_MULTIPLY );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 102 + 523, iYPos + 256, FONT_SIZE_13, m_ItemInfo.c_str() );
		}
		//m_ItemSelectPrinter.PrintFullText( iXPos+525, iYPos+85, TAT_CENTER );
	}
}

void GradeAdjustmentWnd::OnRenderMagicCircle()
{	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		MAGIC_CIRCLE_OFFSET_X = 620,
		MAGIC_CIRCLE_OFFSET_Y = 160,

		ICON_OFFSET_X = 620,
		ICON_OFFSET_Y = 160,

		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	if( m_pMagicCircle )
	{
		m_pMagicCircle->SetAlpha( MAX_ALPHA_RATE );
		m_pMagicCircle->SetScale( FLOAT1 );
		m_pMagicCircle->Render( iXPos + MAGIC_CIRCLE_OFFSET_X, iYPos + MAGIC_CIRCLE_OFFSET_Y );

		// 퍼지는 이펙트
		vReinforceMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
		while( iter != m_vMagicEffectInfoList.end() )
		{
			m_pMagicCircle->SetAlpha( (*iter).m_iCurAlphaRate );
			m_pMagicCircle->SetScale( (*iter).m_fCurScaleRate );
			m_pMagicCircle->Render( iXPos + MAGIC_CIRCLE_OFFSET_X, iYPos + MAGIC_CIRCLE_OFFSET_Y );

			++iter;
		}
	}

	if ( m_pIconBack )
		m_pIconBack->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_MULTIPLY );
	if ( m_pIconSlot )
		m_pIconSlot->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_MULTIPLY );

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, FLOAT1, m_iIconRenderType, TFO_NEAREST, m_iAlpha1, m_iAlpha1, m_iAlpha1 );

	if ( m_pIcon )
	{
		if ( m_iIconRenderType == UI_RENDER_NORMAL_ADD_COLOR )
			m_pIcon->SetColor( m_iAlpha1 ,m_iAlpha1,m_iAlpha1 );
		m_pIcon->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, m_iIconRenderType );
		if( m_pNumText && m_pPlusText && m_iItemReinforce > 0 )
		{
			int iCurPos = 0;
			int iDefaultPos = iXPos + ICON_OFFSET_X;

			int iCurYPos = iYPos + ICON_OFFSET_Y + REINFORCE_Y ;

			int iTotalSize = 0;
			if( m_iItemReinforce < 10 )
				iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
			else if( m_iItemReinforce < 100 )
				iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
			else
				iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

			iCurPos = iDefaultPos - (iTotalSize / 2);

			m_pPlusText->Render( iCurPos, iCurYPos );

			iCurPos += (PLUS_SIZE-NUM_GAP);

			m_pNumText->RenderNum( iCurPos, iCurYPos, m_iItemReinforce, -NUM_GAP );
		}
	}

	if ( m_pSuccessLightImage )
	{
		m_pSuccessLightImage->SetScale( 2.0f );
		m_pSuccessLightImage->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_ADD );
	}
}

void GradeAdjustmentWnd::OnRenderResultEffect()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	enum
	{
		ICON_OFFSET_X = 620,
		ICON_OFFSET_Y = 160,
	};

	if( m_pLightImage )
	{
		m_pLightImage->SetScale( FLOAT1 );
		m_pLightImage->SetAlpha( m_iAlpha1 );
		m_pLightImage->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y );

		m_pLightImage->SetScale( m_fCurEffectScale );
		m_pLightImage->SetAlpha( m_iAlpha2 );
		m_pLightImage->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y);
	}
}

bool GradeAdjustmentWnd::IsRightTab( DWORD dwTabID, int iType )
{
	GradeAdjustmentCategoryTab* pButton = dynamic_cast<GradeAdjustmentCategoryTab*>( FindChildWnd( dwTabID ) );
	if( pButton )
	{
		const TabUISetting& rkSetting = pButton->GetTab((m_dwCurSmallTabID - ID_SMALL_TAB_1_BTN));
		for( TabFiterVec::const_iterator iter = rkSetting.m_FilterVec.begin(); iter != rkSetting.m_FilterVec.end(); ++iter )
		{
			const TabFiter& Filter = *iter;
			if( Filter.m_iTypeFilter == 0 )
				return true;
			else if( Filter.m_iTypeFilter == iType )
				return true;
		}
	}
	return false;
}

void GradeAdjustmentWnd::RightButtonControl( ioPowerUpManager::PowerUpImpossibleFunc eCheckFunc )
{
	//if(eCheckFunc == ioPowerUpManager::POSSIBLE_ALL)
	ioButton* pReinforceButton = dynamic_cast<ioButton*>( FindChildWnd( ID_REINFORCE_BTN ) );
	pReinforceButton->SetActive();
	ioButton* pDegenerationButton = dynamic_cast<ioButton*>( FindChildWnd( ID_DEGENERATION_BTN ) );
	pDegenerationButton->SetActive();

	if(eCheckFunc == ioPowerUpManager::IMPOSSIBLE_ALL)
	{
		pReinforceButton->SetInActive();
		pDegenerationButton->SetInActive();
	}
	else if(eCheckFunc == ioPowerUpManager::REINFORCE)
		pReinforceButton->SetInActive();
	else if(eCheckFunc == ioPowerUpManager::DEGENERATION)
		pDegenerationButton->SetInActive();
}

void GradeAdjustmentWnd::Update()
{
	UpdatePage( m_iCurPage );
}

void GradeAdjustmentWnd::SetFind()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_FIND_EDIT ) );
	if( !pEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s pEdit == NULL.", __FUNCTION__ );
		return;
	}

	ChangeTab( m_dwCurTabID, m_szCurrFindKeyword.c_str() );
}

void GradeAdjustmentWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "EditFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void GradeAdjustmentWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TabDot" )
	{
		SAFEDELETE( m_pDotLineRight );
		m_pDotLineRight = pImage;		
	}
	else if( szType == "magic_circle" )
	{
		SAFEDELETE( m_pMagicCircle );
		m_pMagicCircle = pImage;
	}
	else if( szType == "Icon_back" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon_Slot" )
	{
		SAFEDELETE( m_pIconSlot );
		m_pIconSlot = pImage;
	}
	else if( szType == "light_image" )
	{
		SAFEDELETE( m_pLightImage );
		m_pLightImage = pImage;
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
	else if( szType == "TabDotBottom" )
	{
		SAFEDELETE( m_pDotLineBottom );
		m_pDotLineBottom = pImage;		
	}
	else if( szType == "DarkTextBack" )
	{
		SAFEDELETE( m_pDarkTextBack );
		if( pImage )
		{
			m_pDarkTextBack = pImage;
			m_pDarkTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "LightTextBack" )
	{
		SAFEDELETE( m_pLightTextBack );
		if( pImage )
		{
			m_pLightTextBack = pImage;
			m_pLightTextBack->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightTextBack->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightTextBack->SetSize( 204, 21 );
		}
	}
	else if( szType == "SoldierInfoBG" )
	{
		SAFEDELETE( m_pSoldierInfoBG );
		m_pSoldierInfoBG = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void GradeAdjustmentWnd::CreateMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	ReinforceMagicEffectInfo kInfo;
	kInfo.m_dwCreateTime = dwCurTime;

	m_vMagicEffectInfoList.push_back( kInfo );
}

void GradeAdjustmentWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_FIND_EDIT ) );
	if( pEdit && pEdit->IsKeyFocus() )
	{
		if( g_InputBox.IsEnter() )
		{
			iwm_command( FindChildWnd( ID_FIND_BTN ), IOBN_BTNUP, 0 );
		}

		else if( g_InputBox.IsKeyDown() || g_InputBox.IsBackSpace() )
		{
			m_szCurrFindKeyword = g_InputBox.GetString();
			//iwm_command( FindChildWnd( ID_FIND_BTN ), IOBN_BTNUP, 0 );
		}
	}
	// Item 진화&퇴화
	ProcessMagicCircle();

	ProcessSuccessLightEffect();

	if ( m_bShowResultEffect )
	{
		if(m_dwCurTabID == ID_TAB_1_BTN)
			ProcessResultHeroEffect();
		else if(m_dwCurTabID == ID_TAB_2_BTN)
			ProcessResultItemEffect();
	}
}

void GradeAdjustmentWnd::ProcessMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMagicEffectCreateTime == 0 ||
		m_dwMagicEffectCreateTime+1000 < dwCurTime )
	{
		CreateMagicCircle();
		m_dwMagicEffectCreateTime = dwCurTime;
	}

	// process
	vReinforceMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
	while( iter != m_vMagicEffectInfoList.end() )
	{
		DWORD dwGap = dwCurTime - (*iter).m_dwCreateTime;
		if( dwGap >= 1500 )
		{
			iter = m_vMagicEffectInfoList.erase( iter );
			continue;
		}

		float fRate = FLOAT1 - ((float)dwGap / 1500.0f);

		float fCurEffectRate = (float)sin(D3DX_PI * FLOAT05 * fRate);

		// FLOAT05 -> 0.0f
		(*iter).m_iCurAlphaRate = (float)MAX_ALPHA_RATE * (0.5f*fCurEffectRate);

		// FLOAT1 -> 1.25f
		(*iter).m_fCurScaleRate = 1.25f - (0.25f*fCurEffectRate);

		++iter;
	}
}

void GradeAdjustmentWnd::ProcessSuccessLightEffect()
{
	bool bSuccess = true;
	bool bChange = false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( bSuccess )
	{
		if( m_dwFinalResultStartTime > 0 &&  (dwCurTime - m_dwFinalResultStartTime) < m_dwSuccessEffectTime )
			bSuccess = true;
		else
		{
			SAFEDELETE( m_pSuccessLightImage );
			bSuccess = false;
		}
	}
	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		bChange = true;
		m_dwRollingEffectCheckTime = dwCurTime;
	}

	// success effect
	if( bSuccess )
	{
		int iEffectListCnt = m_vSuccessLightImage.size();
		if( iEffectListCnt == 0 )
		{
			SAFEDELETE( m_pSuccessLightImage );
			return;
		}
		else if( bChange )
		{
			SAFEDELETE( m_pSuccessLightImage );

			m_iCurSuccessEffectArray++;

			if( COMPARE( m_iCurSuccessEffectArray, 0, iEffectListCnt ) )
			{
				ioHashString szEffect = m_vSuccessLightImage[m_iCurSuccessEffectArray];

				if( !szEffect.IsEmpty() )
					m_pSuccessLightImage = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			}
		}
	}
}

void GradeAdjustmentWnd::OnRenderSuccessLightEffect()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	enum
	{
		ICON_OFFSET_X = 123,
		ICON_OFFSET_Y = 167,
	};
	if ( m_pSuccessLightImage )
	{
		m_pSuccessLightImage->SetScale( 2.0f );
		m_pSuccessLightImage->Render( iXPos + ICON_OFFSET_X, iYPos + ICON_OFFSET_Y, UI_RENDER_ADD );
	}
}

void GradeAdjustmentWnd::ProcessResultHeroEffect()
{
	// 용병 정보 갱신 및 이펙트 
	int iCode = g_PowerUpManager.GetReinforcedCode();
	int iIndex = g_PowerUpManager.GetReinforcedItemIndex();
	if ( iCode != 0 && iIndex == 0 )
	{
		HeroReinforceClearListAll();
		//SetSoldierInfo(g_PowerUpManager.GetReinforcedCode());
		UpdatePage(m_iCurPage);

		g_PowerUpManager.ClearReinforcedCode();
		m_bShowResultEffect = false;
		m_bReinforce = false; 
	}
}

void GradeAdjustmentWnd::ProcessResultItemEffect()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwResultEndTime < dwCurTime )
	{
		if ( g_PowerUpManager.GetReinforcedItemIndex() != 0  )
		{
			ItemReinforceClearListAll();
			//MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			//if( pInvenWnd && pInvenWnd->IsShow() )
			//	pInvenWnd->ShowItemReinforceDoneWnd( g_PowerUpManager.GetReinforcedItemIndex() );
			SetItemInfo(g_PowerUpManager.GetReinforcedItemIndex());
			UpdatePage(m_iCurPage);

			g_PowerUpManager.ClearReinforcedCode();
			g_PowerUpManager.ClearReinforcedItemIndex();

			// 사운드
			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );
			if( !m_szSuccessSound.IsEmpty() )
				g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );

			m_bShowResultEffect = false;
			m_bReinforce = false; 
			//HideWnd();
		}
	}

	// 원 이펙트
	if( m_dwResultEffectEndTime > 0 && dwCurTime < m_dwResultEffectEndTime )
	{
		float fRate = FLOAT1;
		if( m_dwResultEffectTime > 0 )
		{
			DWORD dwGap = m_dwResultEffectEndTime - dwCurTime;
			fRate = FLOAT1 - ((float)dwGap / (float)m_dwResultEffectTime);
		}

		//float fCurEffectRate = (float)sin(D3DX_PI * FLOAT05 * fRate);
		float fXValue = fRate - FLOAT1;
		float fPowValue = fabs( pow(fXValue, m_fSlopeRate) );
		float fCurEffectRate = -fPowValue + FLOAT1;

		// 0.0f -> FLOAT1
		m_iAlpha1 = (float)MAX_ALPHA_RATE * fCurEffectRate;

		// 0.0f -> alpharate
		m_iAlpha2 = (float)MAX_ALPHA_RATE * (m_fAlphaRate*fCurEffectRate);

		// scalerate -> FLOAT1
		float fScaleGap = m_fScaleRate - FLOAT1;
		m_fCurEffectScale = m_fScaleRate - (fScaleGap*fCurEffectRate);
	}
}

void GradeAdjustmentWnd::CheckReinforceEnd( ioPowerUpManager::PowerUpInfoType eType )
{
	m_bReinforce = false;

	if(eType == ioPowerUpManager::PIT_HERO)
	{

	}
	else if (eType == ioPowerUpManager::PIT_ITEM)
	{

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GradeAdjustmentInfoWnd::GradeAdjustmentInfoWnd()
{
	m_pEmptyImg = NULL;
	m_pSelectFrm = NULL;
	m_nGradeType = 0;
	m_bSelect = false;
}

GradeAdjustmentInfoWnd::~GradeAdjustmentInfoWnd()
{
	//ClearDesc();

	SAFEDELETE(m_pEmptyImg);
	SAFEDELETE(m_pSelectFrm);
}

void GradeAdjustmentInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT_BTN:
	case ID_ICON_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_HERO )
			{
				ioHashString szError;
				szError.Clear();
				int iClassType = m_PowerUpInfo.m_iValue;
				//int iUseMtrlCount,iCurMtrlCount;
				//if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO ,iClassType, iUseMtrlCount, iCurMtrlCount ) )
				//{
				//	SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
				//	if ( pSoulstoneBuyWnd )
				//		pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
				//	break;
				//}
				// PowerUp이 가능한지 체크
				if( !g_PowerUpManager.CheckEnableCharPowerUp( m_PowerUpInfo.m_iValue, szError, false ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
					break;
				}

				GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
				if( pWnd )
				{
					pWnd->SetSoldierInfo( iClassType );
					//pWnd->ShowWnd();
				}
				//HideWnd();
				//MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				//if( pInvenWnd && pInvenWnd->IsShow() )
				//	pInvenWnd->ShowHeroReinforceDownWnd( iClassType );

			}
			else if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_ITEM )
			{
				ioHashString szError;
				szError.Clear();

				ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
				if( !pExtraItem )
					return;

				EXTRAITEMSLOT Slot;
				if( !pExtraItem->GetExtraItem( m_PowerUpInfo.m_iValue, Slot ) )
					return;

				//if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM , Slot.m_iItemCode, iUseMtrlCount, iCurMtrlCount ) )
				//{
				//	SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
				//	if ( pSoulstoneBuyWnd )
				//		pSoulstoneBuyWnd->ShowBuyWnd( iUseMtrlCount, iCurMtrlCount );
				//	return;
				//}

				if ( !g_PowerUpManager.CheckEnableItemPowerUp( m_PowerUpInfo.m_iValue, szError ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, this, szError.c_str() );
					return;
				}
				GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
				if( pWnd )
				{
					pWnd->SetItemInfo( Slot.m_iIndex, true );
				}

				//HideWnd();
				//MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				//if( pInvenWnd && pInvenWnd->IsShow() )
				//	pInvenWnd->ShowItemReinforceDownWnd( m_PowerUpInfo.m_iValue );
			}
			//else
			//{
			//	g_GUIMgr.SetMsgBox( MB_OK, this, STR(1) );
			//	break;
			//}
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, ID_SELECT_BTN );
		}
		break;
	}
}

void GradeAdjustmentInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void GradeAdjustmentInfoWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "SelectFrm" )
		{
			SAFEDELETE( m_pSelectFrm );
			m_pSelectFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void GradeAdjustmentInfoWnd::SetBlank()
{
	SetInActive();

	m_PowerUpInfo.Clear();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_SELECT_BTN );

	ClearDesc();

	m_nGradeType = 0;
}

void GradeAdjustmentInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void GradeAdjustmentInfoWnd::OnRender()
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
			m_pEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
	}
	else
	{
		ioButton::OnRender();
	}

	RenderDesc( iXPos, iYPos );
}

void GradeAdjustmentInfoWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if(m_bSelect && m_pSelectFrm)
		m_pSelectFrm->Render( iXPos, iYPos );
}

void GradeAdjustmentInfoWnd::RenderDesc( int iXPos, int iYPos )
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
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, STR(2) );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[2].SetTextStyle( TS_NORMAL );
		m_szDescArray[2].SetBkColor( 0, 0, 0 );	
		m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "" );
		m_szDescArray[2].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
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

void GradeAdjustmentInfoWnd::SetPowerUpInfo( const GradeInfo& rkItem )
{
	SetBlank();
	SetActive();

	m_PowerUpInfo = rkItem;

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

bool GradeAdjustmentInfoWnd::SetInfo()
{
	if( m_PowerUpInfo.IsEmpty() )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );

	GradeAdjustmentSelectIconBtn *pBtn = dynamic_cast< GradeAdjustmentSelectIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );

	ioUIRenderImage *pImg = NULL;
	ioHashString szIcon = m_PowerUpInfo.m_szIconName;
	if( !szIcon.IsEmpty() )
		pImg = g_UIImageSetMgr.CreateImageByFullName( szIcon );

	pBtn->SetIcon( pImg, FLOAT1, FLOAT1, m_PowerUpInfo.m_iValue2 );
	ShowChildWnd( ID_SELECT_BTN );

	// desc
	ClearDesc();

	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회

	if(m_PowerUpInfo.m_szName.Length() != 0)
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, "%s", m_PowerUpInfo.m_szName.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );

	if(m_PowerUpInfo.m_szSubDesc.Length() != 0)
		m_szDescArray[1].AddTextPiece( FONT_SIZE_13, "%s", m_PowerUpInfo.m_szSubDesc.c_str() );

	m_szDescArray[2].ClearList();
	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if(m_PowerUpInfo.m_szWeaponDesc.Length() != 0)
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, "%s", m_PowerUpInfo.m_szWeaponDesc.c_str() );

	if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_HERO )
		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( m_PowerUpInfo.m_iValue );
	else if ( m_PowerUpInfo.m_iType == ioPowerUpManager::PIT_ITEM )
	{
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( pExtraItem )
		{
			EXTRAITEMSLOT ExtraItem;
			if ( pExtraItem->GetExtraItem( m_PowerUpInfo.m_iValue, ExtraItem ) )
			{
				const ioItem *pItem = g_ItemMaker.GetItemConst( ExtraItem.m_iItemCode, __FUNCTION__ );
				if( pItem )
					m_nGradeType = pItem->GetGradeType();
			}
			else
			{
				const ioItem *pItem = g_ItemMaker.GetItemConst( m_PowerUpInfo.m_iValue3, __FUNCTION__ );
				if( pItem )
					m_nGradeType = pItem->GetGradeType();
			}
		}
	}
	if(!m_PowerUpInfo.m_bHasPowerUp)
		pBtn->SetInActive();
	else
		pBtn->SetActive();
	pBtn->SetGradeType( m_nGradeType );

	return true;
}

void GradeAdjustmentInfoWnd::SetItem()
{
	SetBlank();
	SetActive();

	if( !SetInfo() )
	{
		SetBlank();
		return;
	}
}

void GradeAdjustmentInfoWnd::SetSelect(bool bSelect)
{
	m_bSelect = bSelect;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GradeAdjustmentSelectIconBtn::GradeAdjustmentSelectIconBtn()
{
	m_pIcon       = NULL;
	m_pIconBack   = NULL;
	m_pNumText = NULL;
	m_pPlusText = NULL;
	m_iItemReinforce = 0;

	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_nGradeType = 0;
}

GradeAdjustmentSelectIconBtn::~GradeAdjustmentSelectIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );
}

void GradeAdjustmentSelectIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void GradeAdjustmentSelectIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void GradeAdjustmentSelectIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void GradeAdjustmentSelectIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
}

void GradeAdjustmentSelectIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void GradeAdjustmentSelectIconBtn::RenderBack( int iXPos, int iYPos )
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

void GradeAdjustmentSelectIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,

		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType );

		if( m_pNumText && m_pPlusText && m_iItemReinforce > 0 )
		{
			int iCurPos = 0;
			int iDefaultPos = iNewXPos;

			int iCurYPos = iNewYPos+(REINFORCE_Y * m_fScale);

			int iTotalSize = 0;
			if( m_iItemReinforce < 10 )
				iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
			else if( m_iItemReinforce < 100 )
				iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
			else
				iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

			iCurPos = iDefaultPos - (iTotalSize / 2);

			m_pPlusText->Render( iCurPos, iCurYPos );

			iCurPos += (PLUS_SIZE-NUM_GAP);

			m_pNumText->RenderNum( iCurPos, iCurYPos, m_iItemReinforce, -NUM_GAP );
		}
	}
}

bool GradeAdjustmentSelectIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int ireinforce )
{
	m_fScale = fScale;
	m_fBackScale = fBackScale;
	m_iItemReinforce = ireinforce;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		return false;
	}

	m_pIcon = pIcon;

	return true;
}

//////////////////////////////////////////////////////////////////////////
GradeAdjustmentCategoryTab::GradeAdjustmentCategoryTab()
{

}

GradeAdjustmentCategoryTab::~GradeAdjustmentCategoryTab()
{

}

void GradeAdjustmentCategoryTab::ClearTab(int iIndex)
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

void GradeAdjustmentCategoryTab::SetTab( const TabUISettingVec& rkTabVec ) 
{
	m_Setting = rkTabVec;
	//SetTitleText( rkTabVec[0].m_szTtitle.c_str() );
	ShowWnd();
}

const TabUISetting& GradeAdjustmentCategoryTab::GetTab(int iIndex)
{
	return m_Setting[iIndex]; 
}