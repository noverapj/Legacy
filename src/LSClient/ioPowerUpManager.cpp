#include "stdafx.h"

#include "ioPowerUpManager.h"
#include "ioBasicTooltipManager.h"

#include "GUI/ioPullDownList.h"
#include "GUI/ReinforceSelectWnd.h"
#include "GUI/GradeAdjustmentWnd.h"

void PowerUpPullDownItem::OnRender( int iOverIdx, int iOverImageIdx )
{
	ioPullDownItem::OnRender( iOverIdx, iOverImageIdx );

	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( m_iIndex == iOverIdx && PullDownEvent::NONE_SELECT == iOverImageIdx )
		g_FontMgr.SetTextColor( m_dwTitleOverColor );
	else
		g_FontMgr.SetTextColor( m_dwTitleColor );

	int iHeight = m_SelectRect.bottom - m_SelectRect.top;
	int iYPos = m_SelectRect.top + ( iHeight / 2 ) - static_cast<int>( g_PullDownFactory.GetFontTextPixel( FONT_SIZE_12 ) * FLOAT05 );
	g_FontMgr.PrintTextWidthCut( m_SelectRect.left + 41, iYPos -1, FONT_SIZE_12, m_SelectRect.right - m_SelectRect.left, m_szTitle.c_str() );
}
//////////////////////////////////////////////////////////////////////////////////////////

template<> ioPowerUpManager* Singleton< ioPowerUpManager >::ms_Singleton = 0;
ioPowerUpManager& ioPowerUpManager::GetSingleton()
{
	return Singleton< ioPowerUpManager >::GetSingleton();
}

ioPowerUpManager::ioPowerUpManager()
{
}

ioPowerUpManager::~ioPowerUpManager()
{
}

void ioPowerUpManager::Initialize()
{	
	ioINILoader_e kLoader( "config/sp2_powerup_manager.ini" );

	char szBuf[MAX_PATH];	

	kLoader.SetTitle_e( "common" );
	int iMax = kLoader.LoadInt_e( "max_char", 0 );

	m_CharPowerUpInfoMap.clear();
	for( int i = 1; i <= iMax; ++i )
	{
		sprintf_s_e( szBuf, "char%d", i );
		kLoader.SetTitle( szBuf );
		LoadPowerUpCharInfo( kLoader );
	}

	kLoader.SetTitle_e( "need_material" );
	m_iMtrlEtcCode = kLoader.LoadInt_e( "material_etc_code", 0 );
	LoadMtrlInfo( kLoader, "char", m_CharGradeNeedCountList );
	LoadMtrlInfo( kLoader, "item", m_ItemGradeNeedCountList );

	LoadMtrlInfo( kLoader, "char_down", m_CharDownGradeNeedCountList, false );
	LoadMtrlInfo( kLoader, "item_down", m_ItemDownGradeNeedCountList, false );

	m_iReinforcedCode = 0;
	m_ePowerUp = PUTT_NONE;
	m_iReinforcedItemIndex = 0;

	LoadRarePowerUpItemInfo();
}

void ioPowerUpManager::LoadGUI()
{
	ioINILoader_e kLoader( "config/sp2_powerup_manager.ini" );

	char szBuf[MAX_PATH];

	kLoader.SetTitle_e( "powerup_select_btn_title" );
	LoadUITabBtnInfo( kLoader );

	kLoader.SetTitle_e( "load_tooltip" );
	kLoader.LoadString( "load_tooltip_name", "", szBuf, MAX_PATH );
	g_BasicTooltipMgr.NewReserveToolTip( szBuf );
}

void ioPowerUpManager::LoadPowerUpCharInfo( ioINILoader& rkLoader )
{
	int iCharCode = rkLoader.LoadInt_e( "char_code", 0 );

	CharPowerUpInfo Info;	
	Info.iPowerUpMax = rkLoader.LoadInt_e( "max_power_grade", 0 );
	
	char szBuf[MAX_PATH];
	for( int i = 0; i < Info.iPowerUpMax; ++i )
	{
		PowerUpItemInfo ItemInfo;
		sprintf_s_e( szBuf, "%s%d", "grade", i+1 );
		LoadPowerUpItemInfo( rkLoader, szBuf, "_default", ItemInfo );
		Info.vPowerUpDefaultItemInfo.push_back( ItemInfo );

		LoadPowerUpItemInfo( rkLoader, szBuf, "", ItemInfo );
		Info.vPowerUpExtraItemItemInfo.push_back( ItemInfo );
	}

	LoadGradeInfo( rkLoader, Info );
	LoadGradedInfo( rkLoader, Info );
	LoadCharToolTip( rkLoader, Info );

	LoadGradeTitlelInfo( rkLoader, "char_grade", Info.m_CharTitleList );
	LoadGradeTitlelInfo( rkLoader, "item_grade", Info.m_ItemTitleList );

	m_CharPowerUpInfoMap.insert( CharPowerUpInfoMap::value_type( iCharCode, Info ) );
}

void ioPowerUpManager::LoadPowerUpItemInfo( ioINILoader& rkLoader, const char* szTitle, const char* szExtend, PowerUpItemInfo& ItemInfo )
{
	char szBuf[MAX_PATH];

	sprintf_s_e( szBuf, "%s%s_weapon", szTitle, szExtend );
	ItemInfo.iItemCode[ES_WEAPON] = rkLoader.LoadInt( szBuf, 0 );

	sprintf_s_e( szBuf, "%s%s_armor", szTitle, szExtend );
	ItemInfo.iItemCode[ES_ARMOR] = rkLoader.LoadInt( szBuf, 0 );

	sprintf_s_e( szBuf, "%s%s_helmet", szTitle, szExtend );
	ItemInfo.iItemCode[ES_HELMET] = rkLoader.LoadInt( szBuf, 0 );

	sprintf_s_e( szBuf, "%s%s_cloak", szTitle, szExtend );
	ItemInfo.iItemCode[ES_CLOAK] = rkLoader.LoadInt( szBuf, 0 );
}

void ioPowerUpManager::LoadMtrlInfo( ioINILoader& rkLoader, const char* szTitle, IntVec& vNeedList, bool bGradeUp )
{
	vNeedList.clear();

	char szBuf[MAX_PATH];

	wsprintf_e( szBuf, "%s_max", szTitle );
	int nMax = rkLoader.LoadInt( szBuf, 0 );

	for( int i=0; i<nMax; i++ )
	{
		if(bGradeUp)
			wsprintf_e( szBuf, "%s_grade%d_need_cnt", szTitle, i+1 );
		else
			wsprintf_e( szBuf, "%s_grade%d_cnt", szTitle, i+1 );
		vNeedList.push_back( rkLoader.LoadInt( szBuf, 0 ) );
	}
}

void ioPowerUpManager::LoadGradeTitlelInfo( ioINILoader &kLoader, const char* szTitle, ioHashStringVec& vTitleList )
{
	vTitleList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	for( int i=0; i<PUGT_CHAR_GRADE_MAX; i++ )
	{
		wsprintf_e( szKey, "%s%d_title", szTitle, i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szGradeTitle = szBuf;
		if ( szGradeTitle.IsEmpty() )
			break;

		vTitleList.push_back( szGradeTitle );
	}
}

void ioPowerUpManager::LoadTextDescInfo( ioINILoader& kLoader, const char* szTitle, ioHashString& vTextList )
{
	char szKey[MAX_PATH], szBuf[MAX_BUFFER_SIZE];

	wsprintf_e( szKey, "%s_text", szTitle );
	kLoader.LoadString( szKey, "", szBuf, MAX_BUFFER_SIZE );
	vTextList = szBuf;
}

void ioPowerUpManager::LoadGradeInfo( ioINILoader& kLoader, CharPowerUpInfo& info )
{
	char szTitle[MAX_PATH];

	int nMaxCount = kLoader.LoadInt_e( "item_desc_cnt", 0 );	
	for ( int i=0; i<nMaxCount; i++ )
	{
		if ( i == 0 )
			wsprintf( szTitle, "char_grade" );
		else if ( i == 1 )
			wsprintf( szTitle, "weapon_grade" );
		else if ( i == 2 )
			wsprintf( szTitle, "armor_grade" );
		else if ( i == 3 )
			wsprintf( szTitle, "helmet_grade" );
		else if ( i == 4 )
			wsprintf( szTitle, "cloak_grade" );
		
		if ( i == 0)
			LoadTextDescInfo( kLoader, szTitle, info.vCharGradeText );
		else
		{
			if ( COMPARE( i-1, 0, MAX_EQUIP_ITEM ) )
				LoadTextDescInfo( kLoader, szTitle, info.vItemGradeList[i-1] );
		}
	}
}

void ioPowerUpManager::LoadGradedInfo( ioINILoader& kLoader, CharPowerUpInfo& info )
{
	char szTitle[MAX_PATH], szKey[MAX_PATH];

	int nMaxCount = kLoader.LoadInt_e( "graded_item_desc_cnt", 0 );	
	for ( int i=0; i<nMaxCount; i++ )
	{
		if ( i == 0 )
			wsprintf( szTitle, "char" );
		else if ( i == 1 )
			wsprintf( szTitle, "weapon" );
		else if ( i == 2 )
			wsprintf( szTitle, "armor" );
		else if ( i == 3 )
			wsprintf( szTitle, "helmet" );
		else if ( i == 4 )
			wsprintf( szTitle, "cloak" );

		wsprintf_e( szKey, "%s_graded_max_count", szTitle );
		int nGradeCnt = kLoader.LoadInt( szKey, 0 );
		for ( int j=0; j<nGradeCnt; j++ )
		{
			ioHashString vGradedText;
			wsprintf( szKey, "%s%d_graded", szTitle, j+1 );
			LoadTextDescInfo( kLoader, szKey, vGradedText );

			if ( i == 0)
				info.vCharGradedList.push_back( vGradedText );
			else
			{
				if ( COMPARE( i-1, 0, MAX_EQUIP_ITEM ) )
					info.vItemGradedList[i-1].push_back( vGradedText );
			}
		}
	}
}

void ioPowerUpManager::LoadCharToolTip( ioINILoader &kLoader, CharPowerUpInfo& info )
{
	info.m_iCharGradeToolTipSizeX = kLoader.LoadInt_e( "tooltip_size_x", 0);

	LoadTextDescInfo( kLoader, "tooltip", info.vCharGradeToolTip );
}

void ioPowerUpManager::LoadUITabBtnInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	for( int i=0; i<PIT_MAX; ++i )
		m_TabUISettingVec[i].clear();

	//soldier Info
	int iCount = rkLoader.LoadInt( "soldier_tab_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		TabUISetting TabInfo;

		StringCbPrintf( szKey, sizeof(szKey), "soldier_tab%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		TabInfo.m_szTtitle = szBuf;

		StringCbPrintf( szKey, sizeof(szKey), "soldier_tab%d_attack_type_count", i+1 );
		int iTypeCount = rkLoader.LoadInt( szKey, 0 );

		for( int j = 0; j < iTypeCount; ++j )
		{
			StringCbPrintf( szKey, sizeof(szKey), "soldier_tab%d_attack_type%d", i+1, j+1 );
			int iType = rkLoader.LoadInt( szKey, 0 );
			if( COMPARE( iType, ioSetItemInfo::CAT_NONE, ioSetItemInfo::CAT_PREMIUM + 1 ) )
			{
				TabFiter Filter;
				Filter.m_iTypeFilter = iType;
				TabInfo.m_FilterVec.push_back( Filter );
			}
		}
		m_TabUISettingVec[PIT_HERO].push_back( TabInfo );
	}


	iCount = rkLoader.LoadInt( "item_tab_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		TabUISetting TabInfo;

		StringCbPrintf( szKey, sizeof(szKey), "item_tab%d_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		TabInfo.m_szTtitle = szBuf;

		StringCbPrintf( szKey, sizeof(szKey), "item_tab%d_type_count", i+1 );
		int iTypeCount = rkLoader.LoadInt( szKey, 0 );

		for( int j = 0; j < iTypeCount; ++j )
		{
			StringCbPrintf( szKey, sizeof(szKey), "item_tab%d_type%d", i+1, j+1 );
			int iType = rkLoader.LoadInt( szKey, 0 );
			// iType = 0 전체, 1 무기
			if( COMPARE( iType, ES_WEAPON, MAX_EQUIP_ITEM + 1) )
			{
				TabFiter Filter;
				Filter.m_iTypeFilter = iType;
				TabInfo.m_FilterVec.push_back( Filter );
			}
		}
		m_TabUISettingVec[PIT_ITEM].push_back( TabInfo );
	}
}

void ioPowerUpManager::LoadRarePowerUpItemInfo()
{
	ioINILoader_e kLoader( "config/sp2_powerup_rare_manager.ini" );

	char szBuf[MAX_PATH], szKey[MAX_PATH];	

	kLoader.SetTitle_e( "common" );
	int iMax = kLoader.LoadInt_e( "max_item", 0 );

	LoadRareItemMtrlInfo( kLoader );

	for( int i = 0; i < iMax; ++i )
	{	
		sprintf_s_e( szBuf, "item%d", i + 1 );
		kLoader.SetTitle( szBuf );

		int iItemCode = kLoader.LoadInt( "item_code", 0 );

		PowerUpRareItemInfo Info;		
		int iGradeMax = kLoader.LoadInt( "grade_max", 0 );
		for( int j = 0; j < iGradeMax; ++j )
		{
			sprintf_s_e( szBuf, "grade%d", j + 1 );
			Info.GradeItemCode.push_back( kLoader.LoadInt( szBuf, 0 ) );
		}
				
		int nSlot = iItemCode / DEFAULT_BASIC_ITEM_CODE;
		if ( COMPARE( nSlot, 0, MAX_EQUIP_ITEM ) )
		{
			LoadTextDescInfo( kLoader, "grade", Info.vItemGradeList[nSlot] );

			int nGradedCnt = kLoader.LoadInt_e( "graded_max_count", 0 );
			for ( int j=0; j<nGradedCnt; j++ )
			{
				ioHashString vGradedText;
				wsprintf( szKey, "item%d_graded", j+1 );
				LoadTextDescInfo( kLoader, szKey, vGradedText );

				Info.vItemGradedList[nSlot].push_back( vGradedText );
			}
		}

		Info.m_nItemGradeToolTipSizeX = kLoader.LoadInt_e( "tooltip_size_x", 0);

		LoadTextDescInfo( kLoader, "tooltip", Info.vItemGradeToolTip );
		LoadGradeTitlelInfo( kLoader, "grade", Info.m_ItemTitleList );

		m_PowerUpRareItemInfoMap.insert( PowerUpRareItemInfoMap::value_type( iItemCode, Info ) );
	}
}

void ioPowerUpManager::LoadRareItemMtrlInfo( ioINILoader& rkLoader )
{
	m_RareItemGradeNeedCountList.clear();

	char szBuf[MAX_PATH];
	int iMax = rkLoader.LoadInt_e( "grade_mtrl_max", 0 );
	for( int i = 1; i <= iMax; ++i )
	{
		sprintf_s_e( szBuf, "grade%d_mtrl_cnt", i );
		m_RareItemGradeNeedCountList.push_back( rkLoader.LoadInt( szBuf, 0 ) );
		sprintf_s_e( szBuf, "down_grade%d_mtrl_cnt", i );
		m_RareItemDownGradeNeedCountList.push_back( rkLoader.LoadInt( szBuf, 0 ) );
	}
}

int ioPowerUpManager::GetCharGradeUpMtrlCount( int eCharGrade )
{
	//유효한 강화캐릭터 등급인지 판단
	if( !COMPARE( eCharGrade, PUGT_CHAR_GRADE1, PUGT_CHAR_GRADE_MAX ) )
		return 0;
	
	if( !COMPARE( eCharGrade - 1, 0, (int)m_CharGradeNeedCountList.size() ) )
		return 0;

	return m_CharGradeNeedCountList[ eCharGrade - 1];
}

int ioPowerUpManager::GetCharGradeDownMtrlCount( int eCharGrade )
{
	//유효한 강화캐릭터 등급인지 판단
	if( !COMPARE( eCharGrade, PUGT_CHAR_GRADE1-1, PUGT_CHAR_GRADE_MAX ) )
		return 0;

	if( !COMPARE( eCharGrade, 0, (int)m_CharDownGradeNeedCountList.size() ) )
		return 0;

	return m_CharDownGradeNeedCountList[ eCharGrade ];
}

int ioPowerUpManager::GetItemGradeUpMtrlCount( int eItemGrade )
{
	if( !IsPowerUpItemGrade( eItemGrade, true ) )
		return 0;

	 int Index = (eItemGrade - PUGT_ITEM_GRADE1) / PUGT_ITEM_GRADE_BASE;
	 if( !COMPARE( Index, 0, (int)m_ItemGradeNeedCountList.size() ) )
		 return 0;

	return m_ItemGradeNeedCountList[ Index ];
}

int ioPowerUpManager::GetItemGradeDownMtrlCount( int eItemGrade )
{
	if( !IsPowerUpItemGrade( eItemGrade, true ) )
		return 0;

	int Index = 0;
	if(eItemGrade == PUGT_ITEM_GRADE_BASE)
		Index = (eItemGrade - PUGT_ITEM_GRADE_BASE) / PURIGT_RARE_ITEM_GRADE_BASE;
	else
		Index = ((eItemGrade - PUGT_ITEM_GRADE1) / PUGT_ITEM_GRADE_BASE) + 1;
	if( !COMPARE( Index, 0, (int)m_ItemDownGradeNeedCountList.size() ) )
		return 0;

	return m_ItemDownGradeNeedCountList[ Index ];
}

int ioPowerUpManager::GetRareItemGradeUpMtrlCount( int eItemGrade )
{
	if( !IsPowerUpRareItemGrade( eItemGrade, true ) )
		return 0;

	int Index = (eItemGrade - PURIGT_RARE_ITEM_GRADE1) / PURIGT_RARE_ITEM_GRADE_BASE;
	if( !COMPARE( Index, 0, (int)m_RareItemGradeNeedCountList.size() ) )
		return 0;

	return m_RareItemGradeNeedCountList[ Index ];
}

int ioPowerUpManager::GetRareItemGradeDownMtrlCount( int eItemGrade )
{
	if( !IsPowerUpRareItemGrade( eItemGrade, true ) )
		return 0;
	int Index = 0;
	if(eItemGrade == PURIGT_RARE_ITEM_GRADE_BASE)
		Index = (eItemGrade - PURIGT_RARE_ITEM_GRADE_BASE) / PURIGT_RARE_ITEM_GRADE_BASE;
	else
		Index = ((eItemGrade - PURIGT_RARE_ITEM_GRADE1) / PURIGT_RARE_ITEM_GRADE_BASE) + 1;

	if( !COMPARE( Index, 0, (int)m_RareItemDownGradeNeedCountList.size() ) )
		return 0;

	return m_RareItemDownGradeNeedCountList[ Index ];
}

int ioPowerUpManager::GetPowerUpCharCount()
{
	return (int)m_CharPowerUpInfoMap.size();
}

int ioPowerUpManager::GetPowerUpItemCount()
{
	return (int)m_PowerUpRareItemInfoMap.size();
}

int ioPowerUpManager::GetPowerUpCharClassType( int iIndex )
{
	CharPowerUpInfoMap::iterator iter = m_CharPowerUpInfoMap.begin();
	for(int i = 0; i < iIndex; i++)
		iter++;
	return (int)iter->first;
}

const ioPowerUpManager::CharPowerUpInfo* ioPowerUpManager::GetCharGradeInfo( int iClassType ) const
{
	CharPowerUpInfoMap::const_iterator iter = m_CharPowerUpInfoMap.find( iClassType );
	if( iter != m_CharPowerUpInfoMap.end() )
		return &iter->second;

	return NULL;
}

const ioPowerUpManager::PowerUpRareItemInfo* ioPowerUpManager::GetRarePowerUpItemInfo( int iCode ) const
{
	int iCalcCode = 0;
	EquipItemExtendType eExtendType = GetEquipItemExtendType( iCode );
	if( eExtendType == EIET_RARE )
	{
		iCalcCode = iCode;
	}
	else if( eExtendType == EIET_RARE_POWERUP )
	{
		//진화된 아이템이면 본래의 아이템 코드를 계산하여 정보를 찾는다.
		int eCurrGrade	= ConvertPowerUpRareItemToRareItemGrade( iCode );
		iCalcCode = iCode - eCurrGrade + PURIGT_RARE_ITEM_GRADE_BASE;
	}

	PowerUpRareItemInfoMap::const_iterator iter = m_PowerUpRareItemInfoMap.find( iCalcCode );
	if( iter != m_PowerUpRareItemInfoMap.end() )
		return &iter->second;

	return NULL;
}

bool ioPowerUpManager::IsPowerUpGradeItem( int iClassType, int iItemCode )
{
	if( iItemCode == 0 )
		return false;

	const CharPowerUpInfo* pInfo = GetCharGradeInfo( iClassType );
	if( pInfo )
	{
		//기본
		{
			PowerUpItemInfoVec::const_iterator iter = pInfo->vPowerUpDefaultItemInfo.begin();
			for( ; iter != pInfo->vPowerUpDefaultItemInfo.end(); ++iter )
			{
				const PowerUpItemInfo& ItemInfo = *iter;
				for( int i = 0; i < MAX_EQUIP_ITEM; ++i )
				{
					if( ItemInfo.iItemCode[i] != 0 && ItemInfo.iItemCode[i] == iItemCode )
						return true;
				}
			}
		}

		//고급
		{
			PowerUpItemInfoVec::const_iterator iter = pInfo->vPowerUpExtraItemItemInfo.begin();
			for( ; iter != pInfo->vPowerUpExtraItemItemInfo.end(); ++iter )
			{
				const PowerUpItemInfo& ItemInfo = *iter;
				for( int i = 0; i < MAX_EQUIP_ITEM; ++i )
				{
					if( ItemInfo.iItemCode[i] != 0 && ItemInfo.iItemCode[i] == iItemCode )
						return true;
				}
			}
		}
	}

	return false;
}

const IntVec ioPowerUpManager::GetEnablePowerUpClassNumberInfo() const
{
	IntVec EnableClassNumVec;
	for ( CharPowerUpInfoMap::const_iterator iter = m_CharPowerUpInfoMap.begin() ; iter != m_CharPowerUpInfoMap.end() ; ++iter )
		EnableClassNumVec.push_back( iter->first );

	const IntVec ConstEnableClassNumVec = static_cast<const IntVec>(EnableClassNumVec);
	return ConstEnableClassNumVec;
}

const IntVec ioPowerUpManager::GetEnablePowerUpExtraItemNumberInfo( int nSlot, bool bAllItem ) const
{
	IntVec EnableExtraItemNumVec;
	for ( PowerUpRareItemInfoMap::const_iterator iter = m_PowerUpRareItemInfoMap.begin() ; iter != m_PowerUpRareItemInfoMap.end() ; ++iter )
	{
		int nEquipSlot = iter->first / DEFAULT_BASIC_ITEM_CODE;
		if ( nEquipSlot != nSlot && !bAllItem )
			continue;

		EnableExtraItemNumVec.push_back( iter->first );

		if(bAllItem)
		{
			for(int i = 0; i < (int)(iter->second).GradeItemCode.size(); i++)
			{
				PowerUpRareItemInfo GradeItem = (iter->second);
				EnableExtraItemNumVec.push_back(GradeItem.GradeItemCode[i]);
			}
		}
	}

	const IntVec ConstEnableExtraItemNumVec = static_cast<const IntVec>(EnableExtraItemNumVec);
	return ConstEnableExtraItemNumVec;
}

const ioHashString& ioPowerUpManager::GetCharGradeTitle( int iClassType, int eCharGrade ) const
{
	static ioHashString szNull;

	const CharPowerUpInfo* pInfo = GetCharGradeInfo( iClassType );
	if ( pInfo == NULL )
		return szNull;

	//유효한 강화캐릭터 등급인지 판단
	if( !COMPARE( eCharGrade, PUGT_CHAR_GRADE1, PUGT_CHAR_GRADE_MAX ) )
		return szNull;

	if( !COMPARE( eCharGrade - 1, 0, (int)pInfo->m_CharTitleList.size() ) )
		return szNull;

	return pInfo->m_CharTitleList[ eCharGrade - 1 ];
}

const ioHashString& ioPowerUpManager::GetItemGradeTitle( int iItemCode, int eItemGrade ) const
{
	static ioHashString szNull;

	const CharPowerUpInfo* pInfo = GetCharGradeInfo( ConvertItemToClassType( iItemCode ) );
	if ( pInfo == NULL )
		return szNull;

	//유효한 강화캐릭터 등급인지 판단
	if( !COMPARE( eItemGrade, PUGT_ITEM_GRADE1, PUGT_ITEM_GRADE_MAX ) )
		return szNull;

	int icharGrade = ConvertPowerUpItemToChar( eItemGrade );

	if( !COMPARE( icharGrade - 1, 0, (int)pInfo->m_ItemTitleList.size() ) )
		return szNull;

	return pInfo->m_ItemTitleList[ icharGrade - 1 ];
}

const ioHashString& ioPowerUpManager::GetExtraItemGradeTitle( int iItemCode, int eItemGrade ) const
{
	static ioHashString szNull;

	const PowerUpRareItemInfo* pInfo = GetRarePowerUpItemInfo( iItemCode );
	if ( pInfo == NULL )
		return szNull;

	//유효한 강화캐릭터 등급인지 판단
	if( !COMPARE( eItemGrade, PURIGT_RARE_ITEM_GRADE1, PURIGT_RARE_ITEM_GRADE_MAX ) )
		return szNull;

	int nItemGrade = (eItemGrade - PURIGT_RARE_ITEM_GRADE1) / PURIGT_RARE_ITEM_GRADE_BASE;
	if( !COMPARE( nItemGrade, 0, (int)pInfo->m_ItemTitleList.size() ) )
		return szNull;

	return pInfo->m_ItemTitleList[nItemGrade];
}

bool ioPowerUpManager::CheckHasChar( int iClassType )
{
	//캐릭터를 보유하고 있는지 체크
	const CHARACTER& rkChar = g_MyInfo.GetCharacterToClassType( iClassType );
	if( rkChar.m_class_type != iClassType )
	{
		char szBuf[MAX_PATH];
		ioHashString szCharName = g_PresentMgr.GetPresentValue1Text( PRESENT_SOLDIER, iClassType, 0 );
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2), szCharName.c_str() );
		return false;
	}
	return true;
}

bool ioPowerUpManager::CheckEnableCharPowerUp( int iClassType, ioHashString& szError, bool bCheckHold )
{
	//진화가 가능한 용병인지 체크
	const CharPowerUpInfo* pInfo = GetCharGradeInfo( iClassType );
	if( !pInfo )
	{
		szError = STR(1);
		return false;
	}

	if( !bCheckHold )
		return true;

	//캐릭터를 보유하고 있는지 체크
	const CHARACTER& rkChar = g_MyInfo.GetCharacterToClassType( iClassType );
	if( rkChar.m_class_type != iClassType )
	{
		char szBuf[MAX_PATH];
		ioHashString szCharName = g_PresentMgr.GetPresentValue1Text( PRESENT_SOLDIER, iClassType, 0 );
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2), szCharName.c_str() );
		szError = szBuf;
		return false;
	}

	//영구 용병인지 체크
	if( rkChar.m_ePeriodType != CPT_MORTMAIN )
	{
		szError = STR(3);
		return false;
	}

	// 구매 용병인지 체크
	if( rkChar.m_chExerciseStyle != EXERCISE_NONE )
	{
		szError = STR(4);
		return false;
	}

	// 용병이 전투체험 상태인지 체크
	if( rkChar.m_sRentalType != CRT_GENERAL )
	{
		szError = STR(5);
		return false;
	}

	// 진화 등급 체크
	int eNextGrade = rkChar.m_eCharPowerUpType + PUGT_CHAR_GRADE_BASE;
	//if( !COMPARE( eNextGrade, PUGT_CHAR_GRADE1, min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) ) )
	//{
	//	//최고 등급을 초과했다면
	//	if( eNextGrade == min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) )
	//		szError = STR(7);
	//	else
	//		szError = STR(6);
	//	return false;
	//}

	return true;
}

bool ioPowerUpManager::CheckEnableItemPowerUp( int iSlotIndex, ioHashString& szError, bool bCheckHold )
{
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		szError = STR(1);
		return false;
	}

	if( !bCheckHold )
		return true;

	EXTRAITEMSLOT Slot;
	if( !pExtraItem->GetExtraItem( iSlotIndex, Slot ) )
	{
		szError = STR(2);
		return false;
	}

	if( Slot.m_bCharEquip == true )
	{
		szError = STR(3);
		return false;
	}
	
	if( Slot.m_PeriodType != EPT_MORTMAIN )
	{
		szError = STR(4);
		return false;
	}
	
	EquipItemExtendType eExtendType = GetEquipItemExtendType( Slot.m_iItemCode );
	if( eExtendType != EIET_EXTRA_POWERUP	&&
		eExtendType != EIET_BASIC_EXTREA	&&
		eExtendType != EIET_SPECIAL_EXTREA	&&
		eExtendType != EIET_RARE			&&
		eExtendType != EIET_RARE_POWERUP )
	{
		szError = STR(6);
		return false;
	}

	switch( eExtendType )
	{
	case EIET_EXTRA_POWERUP:
	case EIET_BASIC_EXTREA:
	case EIET_SPECIAL_EXTREA:
		{
			const CharPowerUpInfo* pInfo = GetCharGradeInfo( ConvertItemToClassType( Slot.m_iItemCode ) );
			if( !pInfo )
			{
				szError = STR(6);
				return false;
			}

			int eCurrGrade = PUGT_NONE;
			int eNextGrade = PUGT_NONE;
			if( eExtendType == EIET_BASIC_EXTREA || eExtendType == EIET_SPECIAL_EXTREA )
			{
				eNextGrade = PUGT_ITEM_GRADE1;
			}
			else if( eExtendType == EIET_EXTRA_POWERUP )
			{
				eCurrGrade = ConvertPowerUpItemToItemGrade( Slot.m_iItemCode );
				eNextGrade = eCurrGrade + PUGT_ITEM_GRADE_BASE;
			}

			int iHeroGrade = ConvertPowerUpItemToChar( eNextGrade );
			if(iHeroGrade == min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) )
				return true;
			//if ( !COMPARE(iHeroGrade, PUGT_NONE, min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) ) )
			//{
			//	//최고 등급을 초과하였다면
			//	if(iHeroGrade == min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) )
			//		szError = STR(8);
			//	else
			//		szError = STR(7);
			//	return false;
			//}

			int iEquipSlotIndex = Slot.m_iItemCode / DEFAULT_BASIC_ITEM_CODE;
			int iUpGradeCode = pInfo->GetCharPowerUpValue( eNextGrade, iEquipSlotIndex );
			if( iUpGradeCode == 0 )
			{
				szError = STR(6);
				return false;
			}
		}
		break;
	case EIET_RARE:
	case EIET_RARE_POWERUP:
		{
			const PowerUpRareItemInfo* pInfo = GetRarePowerUpItemInfo( Slot.m_iItemCode );
			if( !pInfo )
			{
				szError = STR(6);
				return false;
			}

			int eCurrGrade = PURIGT_NONE;
			int eNextGrade = PURIGT_NONE;
			if( eExtendType == EIET_RARE )
			{
				eNextGrade = PURIGT_RARE_ITEM_GRADE1;
			}
			else if( eExtendType == EIET_RARE_POWERUP )
			{
				eCurrGrade = ConvertPowerUpRareItemToRareItemGrade( Slot.m_iItemCode );
				eNextGrade = eCurrGrade + PURIGT_RARE_ITEM_GRADE_BASE;
			}

			int iItemGrade = ConvertPowerUpRareItemToChar( eNextGrade );
			//if ( !COMPARE(iItemGrade, PURIGT_NONE, min( (int)pInfo->GradeItemCode.size() + 1, PUGT_CHAR_GRADE_MAX ) ) )
			//{
			//	//최고 등급을 초과하였다면
			//	if(iItemGrade == min( (int)pInfo->GradeItemCode.size() + 1, PUGT_CHAR_GRADE_MAX ) )
			//		szError = STR(8);
			//	else
			//		szError = STR(7);
			//	return false;
			//}

			int iUpGradeCode = pInfo->GetPowerUpValue( eNextGrade );
			if( iUpGradeCode == 0 )	// 
			{
				// 궁극상태이면 
				if(eNextGrade == PURIGT_RARE_ITEM_GRADE2 || eNextGrade == PURIGT_RARE_ITEM_GRADE3)
					return true;
				szError = STR(6);
				return false;
			}
		}
		break;
	}

	return true;
}

ioPowerUpManager::PowerUpImpossibleFunc ioPowerUpManager::CheckEnableCharGrade( int iCode )
{
	const CharPowerUpInfo* pInfo = GetCharGradeInfo( iCode );
	if( !pInfo )
		return IMPOSSIBLE_ALL;

	//캐릭터를 보유하고 있는지 체크
	const CHARACTER& rkChar = g_MyInfo.GetCharacterToClassType( iCode );
	if( rkChar.m_class_type != iCode )
	{
		//char szBuf[MAX_PATH];
		//ioHashString szCharName = g_PresentMgr.GetPresentValue1Text( PRESENT_SOLDIER, iCode, 0 );
		//SafeSprintf( szBuf, sizeof( szBuf ), STR(2), szCharName.c_str() );
		return IMPOSSIBLE_ALL;
	}

	//영구 용병인지 체크
	if( rkChar.m_ePeriodType != CPT_MORTMAIN )
	{
		return IMPOSSIBLE_ALL;
	}

	// 구매 용병인지 체크
	if( rkChar.m_chExerciseStyle != EXERCISE_NONE )
	{
		return IMPOSSIBLE_ALL;
	}

	// 용병이 전투체험 상태인지 체크
	if( rkChar.m_sRentalType != CRT_GENERAL )
	{
		return IMPOSSIBLE_ALL;
	}

	// 진화 등급 체크
	int eNextGrade = rkChar.m_eCharPowerUpType + PUGT_CHAR_GRADE_BASE;
	if( !COMPARE( eNextGrade, PUGT_CHAR_GRADE1, min( pInfo->iPowerUpMax, PUGT_CHAR_GRADE_MAX ) ) )
	{
		//최고 등급을 초과했다면
		if( eNextGrade == min( pInfo->iPowerUpMax + 1, PUGT_CHAR_GRADE_MAX ) )
			return REINFORCE;
	}
	// 진화를 한번도 하지 않았다면 되돌리기 불가
	if(rkChar.m_eCharPowerUpType == PUGT_NONE)
		return DEGENERATION;
	return POSSIBLE_ALL;
}

ioPowerUpManager::PowerUpImpossibleFunc ioPowerUpManager::CheckEnableItemGrade( int iIndex, int iCode )
{
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return IMPOSSIBLE_ALL;;

	EXTRAITEMSLOT Slot;
	if( !pExtraItem->GetExtraItem( iIndex, Slot ) )
		return IMPOSSIBLE_ALL;;

	EquipItemExtendType eExtendType = GetEquipItemExtendType( iCode );
	if( eExtendType != EIET_EXTRA_POWERUP	&&
		eExtendType != EIET_BASIC_EXTREA	&&
		eExtendType != EIET_SPECIAL_EXTREA	&&
		eExtendType != EIET_RARE			&&
		eExtendType != EIET_RARE_POWERUP )
		return IMPOSSIBLE_ALL;

	switch( eExtendType )
	{
	case EIET_EXTRA_POWERUP:
	case EIET_BASIC_EXTREA:
	case EIET_SPECIAL_EXTREA:
		{
			const CharPowerUpInfo* pInfo = GetCharGradeInfo( ConvertItemToClassType( iCode ) );
			if( !pInfo )
				return IMPOSSIBLE_ALL;

			int eCurrGrade = PUGT_NONE;
			int eNextGrade = PUGT_NONE;
			if( eExtendType == EIET_BASIC_EXTREA || eExtendType == EIET_SPECIAL_EXTREA )
			{
				eNextGrade = PUGT_ITEM_GRADE1;
			}
			else if( eExtendType == EIET_EXTRA_POWERUP )
			{
				eCurrGrade = ConvertPowerUpItemToItemGrade( iCode );
				eNextGrade = eCurrGrade + PUGT_ITEM_GRADE_BASE;
			}

			int iHeroGrade = ConvertPowerUpItemToChar( eCurrGrade );
			if ( !COMPARE(iHeroGrade, PUGT_NONE, min( pInfo->iPowerUpMax, PUGT_CHAR_GRADE_MAX ) ) )
			{
				//최고 등급을 초과하였다면
				if(iHeroGrade == min( pInfo->iPowerUpMax, PUGT_CHAR_GRADE_MAX ) )
					return REINFORCE;
				//else
				//	return DEGENERATION;
			}
			if(iHeroGrade == PUGT_NONE)
				return DEGENERATION;

			int iEquipSlotIndex = iCode / DEFAULT_BASIC_ITEM_CODE;
			int iUpGradeCode = pInfo->GetCharPowerUpValue( eNextGrade, iEquipSlotIndex );
			if( iUpGradeCode == 0 )
				return IMPOSSIBLE_ALL;
		}
		break;
	case EIET_RARE:
	case EIET_RARE_POWERUP:
		{
			const PowerUpRareItemInfo* pInfo = GetRarePowerUpItemInfo( iCode );
			if( !pInfo )
				return IMPOSSIBLE_ALL;

			int eCurrGrade = PURIGT_NONE;
			int eNextGrade = PURIGT_NONE;
			if( eExtendType == EIET_RARE )
			{
				eNextGrade = PURIGT_RARE_ITEM_GRADE1;
			}
			else if( eExtendType == EIET_RARE_POWERUP )
			{
				eCurrGrade = ConvertPowerUpRareItemToRareItemGrade( iCode );
				eNextGrade = eCurrGrade + PURIGT_RARE_ITEM_GRADE_BASE;
			}

			int iItemGrade = ConvertPowerUpRareItemToChar( eCurrGrade );
			if ( !COMPARE(iItemGrade, PURIGT_NONE, min( (int)pInfo->GradeItemCode.size() + 1, PUGT_CHAR_GRADE_MAX ) ) )
			{
				//최고 등급을 초과하였다면
				if(iItemGrade == min( (int)pInfo->GradeItemCode.size() + 1, PUGT_CHAR_GRADE_MAX ) )
					return REINFORCE;
				//else
				//	return DEGENERATION;
			}
			if(iItemGrade == PURIGT_NONE)
				return DEGENERATION;

			int iUpGradeCode = pInfo->GetPowerUpValue( eNextGrade );
			if( iUpGradeCode == 0 )	// 
			{
				// 궁극상태이면 
				if(eNextGrade == PURIGT_RARE_ITEM_GRADE2 || eNextGrade == PURIGT_RARE_ITEM_GRADE3)
					return REINFORCE;
				return IMPOSSIBLE_ALL;
			}
		}
		break;
	}
	return POSSIBLE_ALL;
}

bool ioPowerUpManager::CheckEnableEquipPowerUpItem( int eCharPowerUpType, int iItemCode )
{
	PowerUpGradeType eCurrGrade = static_cast<PowerUpGradeType>( ConvertPowerUpItemToItemGrade( iItemCode ) );
	if( ConvertPowerUpCharToItem( eCharPowerUpType ) != eCurrGrade )
	{
		return false;
	}

	return true;
}

void ioPowerUpManager::ApplyCharPowerUp( int iIndex, int iCode, byte eGrade )
{
	g_MyInfo.ApplyCharGrade( iIndex, eGrade );

	int iClassType = g_MyInfo.GetClassType( (DWORD)iIndex );
	m_iReinforcedCode = iClassType;

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", iIndex );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", iIndex );

	// CHAR_EVOLUTION_CHAR
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Character"
		, "Gold"
		, chLabel
		, 1
		, "%2FCHAR%2FEVOLUTION%2FGOLD" );
#endif
}

void ioPowerUpManager::ApplyItemPowerUp( int iIndex, int iCode )
{
	ioUserExtraItem* pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 슬롯 에러 - 1" );
		return;
	}

	EXTRAITEMSLOT Slot;
	if( !pExtraItem->GetExtraItem( iIndex, Slot ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "아이템 슬롯 에러 - 2" );
		return;
	}
	if((iCode/DEFAULT_EXTRA_ITEM_CODE) == 1 && (iCode % DEFAULT_EXTRA_ITEM_CODE) >= DEFAULT_RARE_ITEM_CODE)
		iCode = iCode - DEFAULT_RARE_ITEM_CODE;

	Slot.m_iItemCode = iCode;
	Slot.m_iTradeState = EET_DISABLE;
	const ioItem *pItem = g_ItemMaker.GetItemConst( iCode, __FUNCTION__ );
	if( pItem )
		Slot.m_Name = pItem->GetName();

	Slot.m_IconName = g_ItemMaker.GetItemLargeIcon( iCode, 0 );

	pExtraItem->SetExtraItem( Slot );

	/*
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
	*/

	m_iReinforcedItemIndex = iIndex;
	m_iReinforcedCode = iCode;

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", iIndex );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", iIndex );

	// CHAR_EVOLUTION_ITEM
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Character"
		, "Item"
		, chLabel
		, 1
		, "%2FCHAR%2FEVOLUTION%2FITEM" );
#endif
}

ioHashString ioPowerUpManager::GetCharGardeString( const int iClassType )
{
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );
	if ( pCharInfo == NULL )
		return ioHashString();

	return pCharInfo->vCharGradeText;
}

ioHashString ioPowerUpManager::GetCharGardedString( const int iClassType, const int iPowerUpGrade )
{
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );

	if ( pCharInfo == NULL )
		return ioHashString();

	int iArray = iPowerUpGrade - 1;
	if ( !COMPARE( iArray, 0, (int)pCharInfo->vCharGradedList.size() ) )
		return ioHashString();

	return pCharInfo->vCharGradedList[iArray];
}

ioHashString ioPowerUpManager::GetItemGardeString( const int iItemCode, int iEquipSlotIndex )
{
	int iClassType = ConvertItemToClassType( iItemCode );
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );
	if ( pCharInfo == NULL )
		return ioHashString();

	if(! COMPARE( iEquipSlotIndex, 0, MAX_EQUIP_ITEM ) )
		return ioHashString();

	return pCharInfo->vItemGradeList[iEquipSlotIndex];
}

ioHashString ioPowerUpManager::GetItemGardedString( const int iItemCode, const int iPowerUpGrade, int iEquipSlotIndex )
{
	int iClassType = ConvertItemToClassType( iItemCode );
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );
	if ( pCharInfo == NULL )
		return ioHashString();

	if( !COMPARE( iEquipSlotIndex, 0, MAX_EQUIP_ITEM ) )
		return ioHashString();

	int iArray = ConvertPowerUpItemToChar( iPowerUpGrade ) - 1;

	if ( !COMPARE( iArray, 0, (int)pCharInfo->vItemGradedList[iEquipSlotIndex].size() ) )
		return ioHashString();

	return pCharInfo->vItemGradedList[iEquipSlotIndex][iArray];
}

ioHashString ioPowerUpManager::GetExtraItemGardeString( const int iItemCode, int iEquipSlotIndex )
{
	const PowerUpRareItemInfo* pItemInfo = GetRarePowerUpItemInfo( iItemCode );
	if ( !pItemInfo )
		return ioHashString();

	if( !COMPARE( iEquipSlotIndex, 0, MAX_EQUIP_ITEM ) )
		return ioHashString();

	return pItemInfo->vItemGradeList[iEquipSlotIndex];
}

ioHashString ioPowerUpManager::GetExtraItemGardedString( const int iItemCode, const int iPowerUpGrade, int iEquipSlotIndex )
{
	const PowerUpRareItemInfo* pItemInfo = GetRarePowerUpItemInfo( iItemCode );
	if ( !pItemInfo )
		return ioHashString();

	if( !COMPARE( iEquipSlotIndex, 0, MAX_EQUIP_ITEM ) )
		return ioHashString();

	int nItemGrade = (iPowerUpGrade - PURIGT_RARE_ITEM_GRADE1) / PURIGT_RARE_ITEM_GRADE_BASE;
	int iArray = nItemGrade;
	if ( !COMPARE( iArray, 0, (int)pItemInfo->vItemGradedList[iEquipSlotIndex].size() ) )
		return ioHashString();

	return pItemInfo->vItemGradedList[iEquipSlotIndex][iArray];
}

ioHashString ioPowerUpManager::GetCharToolTipString( const int iClassType )
{
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );

	if ( pCharInfo == NULL )
		return ioHashString();

	return pCharInfo->vCharGradeToolTip;
}

ioHashString ioPowerUpManager::GetExtraItemToolTipString( const int nItemCode )
{
	const PowerUpRareItemInfo* pExtraItemInfo = GetRarePowerUpItemInfo( nItemCode );
	if ( !pExtraItemInfo )
		return ioHashString();

	return pExtraItemInfo->vItemGradeToolTip;
}

const TabUISettingVec& ioPowerUpManager::GetTabUISettinVec( int iType )
{
	static TabUISettingVec SettingVec;
	if( !COMPARE( iType, PIT_HERO, PIT_MAX ) )
		return SettingVec;
	
	return m_TabUISettingVec[ iType ];
}

void ioPowerUpManager::GetMenuList( IN ioPullDownList* pList )
{
	for( int i = 0; i < (int)m_PowerUpPullDownItemVec.size() ; ++i )
	{
		PowerUpPullDownItem* pPullDown = new PowerUpPullDownItem;
		pPullDown->m_szTitle		= m_PowerUpPullDownItemVec[i].m_szTitle;
		pPullDown->m_iTooltipIdx	= m_PowerUpPullDownItemVec[i].m_iTooltipIdx;

		if( !m_PowerUpPullDownItemVec[i].IsEnableIamgeRender() )
			pPullDown->SetEnableImageRender( false );

		pPullDown->m_dwTitleColor		= m_PowerUpPullDownItemVec[i].m_dwTitleColor;
		pPullDown->m_iManualIndex = m_PowerUpPullDownItemVec[i].m_iManualIndex;
		pList->AddList( pPullDown );
	}
}

void ioPowerUpManager::SetMenuList( ioXMLElement &xElement )
{
	int iMaxTab = 0;
	int iCurTab = 1;
	char szBuf[MAX_PATH];
	ioHashString szName;
	ioXMLElement elem = xElement.FirstChild();
	m_PowerUpPullDownItemVec.clear();
	while ( !elem.IsEmpty() )
	{
		memset( szBuf, 0, sizeof(szBuf) );
		StringCbPrintf_e( szBuf, sizeof( szBuf ), "TabText%d", iCurTab );
		szName.Clear();
		if( strcmp( elem.GetTagName(), "MaxTab" ) == 0 )
		{
			iMaxTab = elem.GetIntAttribute( "Max" );
			m_PowerUpPullDownItemVec.reserve( iMaxTab );
		}
		else if( strcmp( elem.GetTagName(), szBuf ) == 0 )
		{
			if ( iCurTab > iMaxTab )
				break;
			PowerUpPullDownItem PullDown;
#ifdef SRC_OVERSEAS 
			elem.SetUseStringMgr(true);
			elem.SetStringMgrKeyName("myinventorywnd");
#endif
			PullDown.m_szTitle = elem.GetStringAttribute( "Text" );
			PullDown.m_iTooltipIdx = g_BasicTooltipMgr.GetToolTipID( elem.GetStringAttribute( "ToolTip" ) );
			PullDown.SetEnableImageRender( elem.GetBoolAttribute("ShowToolTip") );
			PullDown.m_iManualIndex = elem.GetIntAttribute( "ManualIndex" );
			m_PowerUpPullDownItemVec.push_back( PullDown );
			iCurTab++;
		}

		elem = elem.NextSibling();
	}
}

void ioPowerUpManager::OnPowerUpMenuToolTipShow( const OnOverPullDownEvent& Event ) const
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const PowerUpPullDownItem* pItem = (const PowerUpPullDownItem*)( pList->GetPullDown( Event.iOverIdx ) );
	if( !pItem )
		return;

	g_BasicTooltipMgr.ShowToolTip( pItem->m_iTooltipIdx );
}

void ioPowerUpManager::OnPowerUpMenuToolTipHide( const OnLeavePullDownEvent& Event ) const
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const PowerUpPullDownItem* pItem = (const PowerUpPullDownItem*)( pList->GetPullDown( Event.iLeaveIdx ) );
	if( !pItem )
		return;

	g_BasicTooltipMgr.HideToolTip( pItem->m_iTooltipIdx );
}

void ioPowerUpManager::OnPowerUpMenuClick( const ClickPullDownEvent& Event, int itemindex, ioWnd* pCalledWnd )
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const PowerUpPullDownItem* pItem = (const PowerUpPullDownItem*)( pList->GetPullDown( Event.iClickIdx ) );
	if( !pItem )
		return;

	//진화
	if ( pItem->m_iIndex == 0 )
	{
		EXTRAITEMSLOT kSlot;
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pExtraItem )
			return;

		if( !pExtraItem->GetExtraItem( itemindex, kSlot ) )
			return;

		ioHashString szError;
		if ( !CheckEnableItemPowerUp( kSlot.m_iIndex ,szError) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, szError.c_str() );
			return;
		}

		GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
		if( pWnd )
		{
			if( pWnd->IsShow() )
			{
				pWnd->GoToTop();
			}
			else
			{
				pWnd->ShowSubWnd( ioPowerUpManager::PIT_ITEM, kSlot.m_iIndex );
			}
		}
	}
	//분해
	else if ( pItem->m_iIndex == 1 )
	{
		if( pCalledWnd->GetParent() )
			pCalledWnd->GetParent()->iwm_command( pCalledWnd, IOBN_BTNUP, INVENTORY_ACTION_DISASSEMBLE );
	}
	//팔기
	else if ( pItem->m_iIndex == 2 )
	{
		if( pCalledWnd->GetParent() )
			pCalledWnd->GetParent()->iwm_command( pCalledWnd, IOBN_BTNUP, INVENTORY_ACTION_RESELL );
	}
}

void ioPowerUpManager::OnPowerUpImageClick( const ClickPullDownEvent& Event )
{
	const ioPullDownList* pList = Event.GetEventCaller();
	if( !pList )
		return;

	const PowerUpPullDownItem* pItem = dynamic_cast<const PowerUpPullDownItem*>( pList->GetPullDown( Event.iClickIdx ) );
	if( !pItem )
		return;

	if ( pItem->m_iManualIndex <= 0 )
		return;

	ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> ( g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
	if( pManualWnd )
		pManualWnd->ShowWndByCurManual( pItem->m_iManualIndex );
}

//용병의 경우는 class type, 아이템의 경우는 code
bool ioPowerUpManager::CheckEnableSoulStone( int iType, int iData, int& iUseMtrlCount, int& iCurMtrlCount, bool bGradeUp )
{
	iUseMtrlCount = iCurMtrlCount = 0;
	ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( m_iMtrlEtcCode ) );
	if( !pEtcItem )
		return false;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )	
		return false;

	if( iType == PIT_HERO )
	{
		int eNextGrade = 0;
		const CHARACTER& rkChar = g_MyInfo.GetCharacterToClassType( iData );
		if(bGradeUp)
		{
			eNextGrade = rkChar.m_eCharPowerUpType + PUGT_CHAR_GRADE_BASE;
			iUseMtrlCount = GetCharGradeUpMtrlCount( eNextGrade );
		}
		else 
		{
			if(rkChar.m_eCharPowerUpType > 0)
			{
				eNextGrade = rkChar.m_eCharPowerUpType - PUGT_CHAR_GRADE_BASE;
				iUseMtrlCount = GetCharGradeDownMtrlCount( eNextGrade );
			}
		}
		if( rkChar.m_class_type != iData )
		{
			eNextGrade = PUGT_CHAR_GRADE_BASE;
			//return false;
		
			iUseMtrlCount = GetCharGradeDownMtrlCount( eNextGrade );
		}
	}
	else if ( iType == PIT_ITEM )
	{
		EquipItemExtendType eExtendType = GetEquipItemExtendType( iData );
		int eCurrGrade = PUGT_NONE;
		int eNextGrade = PUGT_NONE;

		switch( eExtendType )
		{
		case EIET_BASIC_EXTREA:
		case EIET_EXTRA_POWERUP:
		case EIET_SPECIAL_EXTREA:
			{
				if( eExtendType == EIET_BASIC_EXTREA || eExtendType == EIET_SPECIAL_EXTREA )
				{
					eNextGrade = PUGT_ITEM_GRADE1;
				}
				else if( eExtendType == EIET_EXTRA_POWERUP )
				{
					eCurrGrade = ConvertPowerUpItemToItemGrade( iData );
					if(bGradeUp)
						eNextGrade = eCurrGrade + PUGT_ITEM_GRADE_BASE;
					else
					{
						eNextGrade = eCurrGrade - PUGT_ITEM_GRADE_BASE;
						if(eNextGrade < PUGT_ITEM_GRADE1)
							eNextGrade = PUGT_ITEM_GRADE_BASE;
					}
				}

				if( !IsPowerUpItemGrade( eNextGrade, true ) )
					return false;
				
				if(bGradeUp)
					iUseMtrlCount = GetItemGradeUpMtrlCount( eNextGrade );
				else
					iUseMtrlCount = GetItemGradeDownMtrlCount( eNextGrade );
			}
			break;
		case EIET_RARE:
		case EIET_RARE_POWERUP:
			{
				if( eExtendType == EIET_RARE )
				{
					eNextGrade = PURIGT_RARE_ITEM_GRADE1;
				}
				else if( eExtendType == EIET_RARE_POWERUP )
				{
					eCurrGrade = ConvertPowerUpRareItemToRareItemGrade( iData );
					if(bGradeUp)
						eNextGrade = eCurrGrade + PURIGT_RARE_ITEM_GRADE_BASE;
					else
					{
						eNextGrade = eCurrGrade - PURIGT_RARE_ITEM_GRADE_BASE;
						if(eNextGrade < PURIGT_RARE_ITEM_GRADE1)
							eNextGrade = PURIGT_RARE_ITEM_GRADE_BASE;
					}
				}

				if( !IsPowerUpRareItemGrade( eNextGrade, true ) )
					return false;

				if(bGradeUp)
					iUseMtrlCount = GetRareItemGradeUpMtrlCount( eNextGrade );
				else
					iUseMtrlCount = GetRareItemGradeDownMtrlCount( eNextGrade );
			}
			break;
		}
	}
	else
		return false;

	ioUserEtcItem::ETCITEMSLOT kEtcSlot;
	if( !pUserEtcItem->GetEtcItem( m_iMtrlEtcCode, kEtcSlot ) )
		return false;

	iCurMtrlCount = kEtcSlot.m_iValue1;

	if( kEtcSlot.m_iValue1 < iUseMtrlCount )
		return false;

	return true;
}

void ioPowerUpManager::GetCharGradeToolTipSize( int iClassType, int& iSizeX )
{
	iSizeX = 0;
	const CharPowerUpInfo* pCharInfo = GetCharGradeInfo( iClassType );
	if ( pCharInfo == NULL )
		return;

	iSizeX = pCharInfo->m_iCharGradeToolTipSizeX;
}

void ioPowerUpManager::GetExtraItemGradeToolTipSize( int nItemCode, int& iSizeX )
{
	iSizeX = 0;
	const PowerUpRareItemInfo* pExtraItemInfo = GetRarePowerUpItemInfo( nItemCode );
	if ( !pExtraItemInfo )
		return;

	iSizeX = pExtraItemInfo->m_nItemGradeToolTipSizeX;
}