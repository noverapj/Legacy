

#include "stdafx.h"

#include "ioItemMaker.h"
#include "ItemDefine.h"

template<> ioItemMaker* Singleton< ioItemMaker >::ms_Singleton = 0;

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

ioItem::ItemType ConvertStringToItemType( const char *szType )
{
	if( !strcmp( "WEAPON", szType ) )
		return ioItem::IT_WEAPON;
	else if( !strcmp( "ARMOR", szType ) )
		return ioItem::IT_ARMOR;
	else if( !strcmp( "HELMET", szType ) )
		return ioItem::IT_HELMET;
	else if( !strcmp( "CLOAK", szType ) )
		return ioItem::IT_CLOAK;
	else if( !strcmp( "OBJECT", szType ) )
		return ioItem::IT_OBJECT;
	else if( !strcmp( "WEAR", szType ) )
		return ioItem::IT_WEAR;
	else if( !strcmp( "RING", szType ) )
		return ioItem::IT_RING;
	else if( !strcmp( "NECKLACE", szType ) )
		return ioItem::IT_NECKLACE;
	else if( !strcmp( "BRACELET", szType ) )
		return ioItem::IT_BRACELET;
	
#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", 16 );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", 16 );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
	}	
#endif

	LOG.PrintTimeAndLog( 0, "ExitProgram - 16" );
	LOG.PrintTimeAndLog( 0, "ConvertStringToItemType - %s UnknownType", szType );
	return ioItem::IT_NONE;
}

ioItemMaker::ioItemMaker()
{
	m_dwNextCreateIndex = 1;
	m_pPlayStage = NULL;
}

ioItemMaker::~ioItemMaker()
{
	m_pPlayStage = NULL;

	ClearAllItemInfo();
}

void ioItemMaker::ClearAllItemInfo()
{
	ItemVector::iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		delete *iter;
	}

	m_ItemList.clear();
	m_ItemNameMap.clear();
	m_ItemCodeMap.clear();

	ItemCreateMap::iterator iCreate;
	for( iCreate = m_ItemCreateMap.begin() ; iCreate != m_ItemCreateMap.end() ; ++iCreate )
	{
		if( iCreate->second > 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioItemMaker::ClearAllItemInfo - %d Item %d Not destroyed",
									iCreate->first, iCreate->second );
		}
	}
}

void ioItemMaker::SetPlayStage( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

ioPlayStage* ioItemMaker::GetPlayStage()
{
	return m_pPlayStage;
}

void ioItemMaker::LoadItemInfo()
{
	ClearAllItemInfo();

	_ENCSTR("config/sp2_item.ini", szSP2Item );
	LoadBaseItemList( szSP2Item );

	_ENCSTR("config/sp2_extraitem.ini", szSP2ExtraItem );
	LoadItemList( szSP2ExtraItem );

	_ENCSTR("config/sp2_item_npc.ini", szSP2ItemNpc );
	LoadItemList( szSP2ItemNpc );

	_ENCSTR("config/sp2_powerup_item.ini", szSP2PowerUpItem );
	LoadItemList( szSP2PowerUpItem );

	_ENCSTR("config/sp2_powerup_extraitem.ini", szSP2ExtraPowerUpItem );
	LoadItemList( szSP2ExtraPowerUpItem );

	_ENCSTR("config/sp2_accessory.ini", szSP2AccessoryItem );
	LoadItemList( szSP2AccessoryItem );
}

void ioItemMaker::LoadBaseItemList( const char *szFileName )
{
	ioINILoader kLoader( szFileName );

	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int i = 0;

	kLoader.SetTitle_e( "common_info" );

	m_fItemDropRange = kLoader.LoadFloat_e( "item_drop_range", 0.0f );
	m_bUseBaseEquipMesh = kLoader.LoadBool_e( "use_base_equip_mesh", false );

	int iGroupCnt = kLoader.LoadInt_e( "item_group_cnt", 0 );

	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "item_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kSetItemLoader( szGroupName );
		kSetItemLoader.SetTitle_e( "common" );

		int iItemCnt = kSetItemLoader.LoadInt_e( "item_cnt", 0 );
		for( int j=0; j < iItemCnt; ++j )
		{
			// 그룹에 속한 아이템 추가
			wsprintf_e( szTitle, "item%d", j+1 );
			kSetItemLoader.SetTitle( szTitle );

			DWORD dwCode = (DWORD)kSetItemLoader.LoadInt_e( "code", 0 );
			if( dwCode == 0 )
			{
				LOG.PrintTimeAndLog( 0, "ioItemMaker::LoadBaseItemList - %s %d item code 0", szGroupName, j );
				continue;
			}

			kSetItemLoader.LoadString_e( "item_type", "unknown", szBuf, MAX_PATH );
			ioItem::ItemType eType = ConvertStringToItemType( szBuf );

			ItemCodeMap::const_iterator iter = m_ItemCodeMap.find( dwCode );
			if( iter != m_ItemCodeMap.end() )
			{
				LOG.PrintTimeAndLog( 0, "ioItemMaker::LoadBaseItemList - %d Already Exist item code", dwCode );
				continue;
			}

			ioItem *pItem = CreateItemTemplete( eType, kSetItemLoader );
			if( pItem )
			{
				pItem->SetGroupName( szGroupName );
				pItem->SetTitleName( szTitle );
				pItem->LoadProperty( kSetItemLoader );
				AddNewItem( pItem );

				pItem->SetCreator( this );
				IncreaseItemCreateCnt( pItem->GetItemCode() );
			}
		}
	}
}

void ioItemMaker::LoadItemList( const char *szFileName )
{
	ioINILoader kLoader( szFileName );

	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int i = 0;

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "item_group_cnt", 0 );

	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "item_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kSetItemLoader( szGroupName );
		kSetItemLoader.SetTitle_e( "common" );

		int iItemCnt = kSetItemLoader.LoadInt_e( "item_cnt", 0 );
		for( int j=0; j < iItemCnt; ++j )
		{
			// 그룹에 속한 아이템 추가
			wsprintf_e( szTitle, "item%d", j+1 );
			kSetItemLoader.SetTitle( szTitle );

			DWORD dwCode = (DWORD)kSetItemLoader.LoadInt_e( "code", 0 );
			if( dwCode == 0 )
			{
				LOG.PrintTimeAndLog( 0, "ioItemMaker::LoadItemList - %s %d item code 0", szGroupName, j );
				continue;
			}

			kSetItemLoader.LoadString_e( "item_type", "unknown", szBuf, MAX_PATH );
			ioItem::ItemType eType = ConvertStringToItemType( szBuf );

			ItemCodeMap::const_iterator iter = m_ItemCodeMap.find( dwCode );
			if( iter != m_ItemCodeMap.end() )
			{
				LOG.PrintTimeAndLog( 0, "ioItemMaker::LoadItemList - %d Already Exist item code", dwCode );
				continue;
			}

			ioItem *pItem = CreateItemTemplete( eType, kSetItemLoader );
			if( pItem )
			{
				pItem->SetGroupName( szGroupName );
				pItem->SetTitleName( szTitle );
				pItem->LoadProperty( kSetItemLoader );
				AddNewItem( pItem );

				pItem->SetCreator( this );
				IncreaseItemCreateCnt( pItem->GetItemCode() );
			}
		}
	}
}

ioItem* ioItemMaker::CreateItem( DWORD dwCode )
{
	ItemCodeMap::iterator iter = m_ItemCodeMap.find( dwCode );
	if( iter != m_ItemCodeMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;

		if( pItem )
		{
			pItem->SetItemType();
			return CreateCloneItem( pItem );
		}
	}

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", 17 );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", 17 );

		g_HttpMng.SetSubErrCode( dwCode );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
	}	
#endif

	LOG.PrintTimeAndLog( 0, "ExitProgram - 17" );
	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreatItem - %d Not Exist Item", dwCode );
	return NULL;
}

ioItem* ioItemMaker::CreateItem( const ioHashString &rkName )
{
	ItemNameMap::iterator iter = m_ItemNameMap.find( rkName );
	if( iter!=m_ItemNameMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;
		pItem->SetItemType();

		return CreateCloneItem( pItem );
	}

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", 18 );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", 18 );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
	}	
#endif

	LOG.PrintTimeAndLog( 0, "ExitProgram - 18" );
	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreateItem - %s Not Exist Item", rkName.c_str() );
	return NULL;
}

ioItem* ioItemMaker::CreateCloneItem( ioItem *pSrc )
{
	ioItem *pItem = pSrc->Clone();
	if( pSrc->UseCreateIndexItem() )
	{
		pItem->SetItemCreateIndex( GetNextCreateIndex() );
	}
	else
	{
		pItem->SetItemCreateIndex( -1 );
	}

	pItem->SetCreator( this );
	IncreaseItemCreateCnt( pItem->GetItemCode() );

	return pItem;
}

ioExtendDash* ioItemMaker::CreateExtendDash( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "dash_type", "DASH_ATTACK", szBuf, MAX_PATH );

	ioExtendDash *pDash = NULL;

	if( !strcmp( "SUPER_DASH", szBuf ) )
		pDash = new ioSuperDash;
	else if( !strcmp( "DASH_SPEAR", szBuf ) )
		pDash = new ioDashSpear;
	else if( !strcmp( "ROLLING_AIM", szBuf ) )
		pDash = new ioRollingDash;
	else if( !strcmp( "ASSAULT_DASH", szBuf ) )
		pDash = new ioAssaultDash;
	else if( !strcmp( "DASH_AIM", szBuf ) )
		pDash = NULL;
	else if( !strcmp( "SIMPLE_DASH", szBuf ) )
		pDash = new ioSimpleDash;
	else if( !strcmp( "POWER_CHARGE_DASH", szBuf ) )
		pDash = new ioPowerChargeDash;
	else if( !strcmp( "SHAOLIN_DASH", szBuf ) )
		pDash = new ioShaolinDash;
	else if( !strcmp( "EXTEND_ASSAULT_DASH", szBuf ) )
		pDash = new ioExtendAssaultDash;
	else if( !strcmp( "POWER_CHARGE_DASH2", szBuf ) )
		pDash = new ioPowerChargeDash2;
	else if( !strcmp( "POWER_CHARGE_DASH3", szBuf ) )
		pDash = new ioPowerChargeDash3;
	else if( !strcmp( "ASSAULT_CHARGE_DASH", szBuf ) )
		pDash = new ioAssaultChargeDash;
	else if( !strcmp( "CHARGE_COMBO_DASH", szBuf ) )
		pDash = new ioChargeComboDash;
	else if( !strcmp( "RUN_ASSAULT_DASH", szBuf ) )
		pDash = new ioRunAssaultDash;
	else if( !strcmp( "NORMAL_COMBO_DASH", szBuf ) )
		pDash = new ioNormalComboDash;
	else if( !strcmp( "DASH_ATTACK2", szBuf ) )
		pDash = new ioDashAttack2;
	else if( !strcmp( "SUDDEN_ATTACK_DASH", szBuf ) )
		pDash = new ioSuddenAttackDash;
	else if( !strcmp( "TELEPORT_DASH", szBuf ) )
		pDash = new ioTeleportDash;
	else if( !strcmp( "FROZEN_COMBO_DASH", szBuf ) )	// 얼음신 2020-03-03 
		pDash = new ioFrozenComboDash;
	else if( !strcmp( "GENISYSARMY_DASH", szBuf ) )		// 제네시스 2020-06-25
		pDash = new ioGenisysarmyDash;
	else if( !strcmp( "SIZE_COMBO_DASH", szBuf ) )		// 사이즈 2020-09-15 
		pDash = new ioSizeComboDash;
	else if( !strcmp( "EVA_DASH", szBuf ) )				// 이브 2020-10-16 
		pDash = new ioEvaDash;
	else if( !strcmp( "MAGNETIC_DASH", szBuf ) )		// 마그네틱 2020-11-25 
		pDash = new ioMagneticDash;
	else if( !strcmp( "TAOIST_DASH", szBuf ) )
		pDash = new ioTaoistDash;
	if( pDash )
	{
		pDash->LoadProperty( rkLoader );
	}
	
	return pDash;
}

ioExtendJump* ioItemMaker::CreateExtendJump( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "jump_type", "JUMP_DEFAULT", szBuf, MAX_PATH );

	ioExtendJump *pJump = NULL;
	if( !strcmp( "DEFENSE_JUMP", szBuf ) )
		pJump = new ioDefenseJump;
	else if( !strcmp( "LONG_JUMP", szBuf ) )
		pJump = new ioLongJump;
	else if( !strcmp( "HIGH_JUMP", szBuf ) )
		pJump = new ioHighJump;
	else if( !strcmp( "DOUBLE_JUMP", szBuf ) )
		pJump = new ioDoubleJump;
	else if( !strcmp( "AIM_JUMP", szBuf ) )
		pJump = new ioAimJump;
	else if( !strcmp( "AIM_JUMP2", szBuf ) )
		pJump = new ioAimJump2;
	else if( !strcmp( "COMBO_JUMP", szBuf ) )
		pJump = new ioComboJump;
	else if( !strcmp( "ROTATE_DOUBLE_JUMP", szBuf ) )
		pJump = new ioRotateDoubleJump;
	else if( !strcmp( "LAND_ATTACK_JUMP", szBuf ) )
		pJump = new ioLandAttackJump;
	else if( !strcmp( "AUTO_SHOT_JUMP", szBuf ) )
		pJump = new ioAutoShotJump;
	else if( !strcmp( "FLASH_JUMP", szBuf ) )
		pJump = new ioFlashJump;
	else if( !strcmp( "COMBO_FLASH_JUMP", szBuf ) )
		pJump = new ioComboFlashJump;
	else if( !strcmp( "CHARGE_FLASH_JUMP", szBuf ) )
		pJump = new ioChargeFlashJump;
	else if( !strcmp( "BADGUY_JUMP", szBuf ) )
		pJump = new ioBadGuyJump;
	else if( !strcmp( "KISARAGI_JUMP", szBuf ) )
		pJump = new ioKisaragiJump;
	else if( !strcmp( "COLLECT_JUMP", szBuf ) )
		pJump = new ioCollectJump;
	else if( !strcmp( "POWER_CHARGE_JUMP", szBuf ) )
		pJump = new ioPowerChargeJump;
	else if( !strcmp( "RAGNA_JUMP", szBuf ) )
		pJump = new ioRagnaJump;
	else if( !strcmp( "CHARGE_COMBO", szBuf ) )
		pJump = new ioChargeComboJump;
	else if( !strcmp( "CHARGE_COMBO2", szBuf ) )
		pJump = new ioChargeComboJump2;
	else if( !strcmp( "CHARGE_COMBO3", szBuf ) )
		pJump = new ioChargeComboJump3;
	else if( !strcmp( "CHARGE_COMBO4", szBuf ) )
		pJump = new ioChargeComboJump4;
	else if( !strcmp( "GATOTKACA_JUMP", szBuf ) )
		pJump = new ioGatotkacaJump;
	else if( !strcmp( "RACHEL_JUMP", szBuf ) )
		pJump = new ioRachelJump;
	else if( !strcmp( "ILJIMAE_JUMP", szBuf ) )
		pJump = new ioIljimaeJump;
	else if( !strcmp( "DESPERADO_JUMP", szBuf ) )
		pJump = new ioDesperadoJump;
	else if( !strcmp( "TRINITY_JUMP", szBuf ) )
		pJump = new ioTrinityJump;
	else if( !strcmp( "DOUBLE_SHOT_JUMP", szBuf ) )
		pJump = new ioDoubleShotJump;
	else if( !strcmp( "DOUBLE_JUMP2", szBuf ) )
		pJump = new ioDoubleJump2;
	else if( !strcmp( "CHARGE_COMBO_DEFENSE_JUMP", szBuf ) )
		pJump = new ioChargeComboDefenseJump;
	else if( !strcmp( "HAKUMEN_JUMP", szBuf ) )
		pJump = new ioHakumenJump;
	else if( !strcmp( "SUDDEN_JUMP", szBuf ) )
		pJump = new ioSuddenAttackJump;
	else if( !strcmp( "TRACE_JUMP", szBuf ) )
		pJump = new ioTraceJump;
	else if( !strcmp( "LEE_JUMP", szBuf ) )
		pJump = new ioLeeJump;
	else if( !strcmp( "PARKILPYO_JUMP", szBuf ) )
		pJump = new ioParkilpyoJump;
	else if( !strcmp( "JORO_EXPANSION_JUMP", szBuf ) )
		pJump = new ioJoroExpansionJump;
	else if( !strcmp( "DOUBLE_JUMP3", szBuf ) )
		pJump = new ioDoubleJump3;
	else if( !strcmp( "ROBIN_EXPANSION_JUMP", szBuf ) )
		pJump = new ioRobinExpansionJump;
	else if( !strcmp( "SABER_JUMP", szBuf ) )
		pJump = new ioSaberJump;
	else if( !strcmp( "NATA_JUMP", szBuf ) )
		pJump = new ioNataJump;
	else if( !strcmp( "GRAPPLER_EXPANSION_JUMP", szBuf ) )
		pJump = new ioGrapplingExpansionJump;
	else if ( !strcmp( "STRIDER_JUMP", szBuf ) )
		pJump = new ioStriderJump;
	else if ( !strcmp( "URIEL_JUMP", szBuf ) )
		pJump = new ioUrielJump;
	else if( !strcmp( "PHOENIX_JUMP", szBuf ) )
		pJump = new ioPhoenixJump;
	else if( !strcmp( "TIMEGATE_JUMP", szBuf ) )
		pJump = new ioTimeGateJump;
	else if( !strcmp( "OMEN_JUMP", szBuf ) )
		pJump = new ioOmenJump;
	else if( !strcmp( "POSEIDON_JUMP", szBuf ) )
		pJump = new ioPoseidonJump;
	else if( !strcmp( "SKY_WALKER_JUMP", szBuf ) )
		pJump = new ioSkyWalkerJump;
	else if( !strcmp( "GWONWANG_JUMP", szBuf ) )		// 2020-06-02 권왕
		pJump = new ioGwonwangJump;
	else if( !strcmp( "GENISYSARMY_JUMP", szBuf ) )		// 2020-06-29 제네시스
		pJump = new ioGenisysarmyJump;
	else if( !strcmp( "HERACLES_JUMP", szBuf ) )
		pJump = new ioHeraclesJump;
	else if( !strcmp( "SIZECOMBO_JUMP", szBuf ) )		// 2020-09-23 사이즈
		pJump = new ioSizeComboJump;
	else if( !strcmp( "EVACOMBO_JUMP", szBuf ) )		// 2020-10-19 이브
		pJump = new ioEvaComboJump;
	else if( !strcmp( "MAGNETIC_JUMP", szBuf ) )		// 마그네틱 2020-11-25
		pJump = new ioMagneticJump;
	else if( !strcmp( "TAOIST_JUMP", szBuf ) )
		pJump = new ioTaoistJump;
	
	if( pJump )
	{
		pJump->LoadProperty( rkLoader );
	}

	return pJump;
}

ioCounterAttack* ioItemMaker::CreateCounterAttack( ioINILoader &rkLoader )
{
	int iType = rkLoader.LoadInt_e( "counter_type", CAT_DEFAULT );

	ioCounterAttack *pCounter = NULL;
	switch( iType )
	{
	case CAT_DEFAULT:
		pCounter = new ioDefaultCounter;
		break;
	case CAT_CHARGE:
		pCounter = new ioChargeCounter;
		break;
	case CAT_EXTRA:
		pCounter = new ioExtraCounter;
		break;
	case CAT_CHARGE_COMBO:
		pCounter = new ioChargeComboCounter;
		break;
	}

	if( pCounter )
	{
		pCounter->LoadProperty( rkLoader );
	}

	return pCounter;
}

const ioItem* ioItemMaker::GetItemConst( DWORD dwCode ) const
{
	if( dwCode == 0 ) return NULL;

	ItemCodeMap::const_iterator iter = m_ItemCodeMap.find( dwCode );
	if( iter != m_ItemCodeMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;
		return pItem;
	}
	return NULL;
}
const ioItem* ioItemMaker::GetItemConst( DWORD dwCode, const ioHashString& szFuncName ) const
{
	if( dwCode == 0 ) return NULL;

	ItemCodeMap::const_iterator iter = m_ItemCodeMap.find( dwCode );
	if( iter != m_ItemCodeMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;
		return pItem;
	}

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chCD5[32] = {0,};
		char chLabel[128] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chCD5, "%d", 19 );
			sprintf_e( chLabel, "%d_%s", 19, szFuncName.c_str() );
		}
		else
		{
			SafeSprintf( chCD5, sizeof(chCD5), "%1", 19 );
			SafeSprintf( chLabel, sizeof(chLabel), "%1_%2", 19, szFuncName.c_str() );
		}

		g_HttpMng.SetSubErrCode( dwCode );

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chCD5 );
	}	
#endif

	__try
	{
		srand(GetTickCount());

		int s = rand() % 100;

		if (s < 10)
		{
			int k = 0;
			int result = 56 / k;

			LOG.PrintTimeAndLog( 0, "ExitProgram - 19" );
			LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %d Not Exist Item", dwCode );
		}
	}
	__except (ExceptCallBack(GetExceptionInformation()))
	{
	}
	
	LOG.PrintTimeAndLog( 0, "ExitProgram - 19" );
	LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %d Not Exist Item", dwCode );
	
#ifdef __DEV_QA__
	LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %d Not Exist Item, Func : %s", dwCode, szFuncName.c_str() );
#else
	LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %d Not Exist Item", dwCode );
#endif
	return NULL;
}

const ioItem* ioItemMaker::GetItemConst( const ioHashString &rkName, const ioHashString& szFuncName ) const
{
	if( rkName.IsEmpty() ) return NULL;
	
	ItemNameMap::const_iterator iter = m_ItemNameMap.find( rkName );
	if( iter != m_ItemNameMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;
		return pItem;
	}

#if defined( USE_GA )
	if( g_App.GetGAStart() == true )
	{
		char chCD5[32] = {0,};
		char chLabel[128] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chCD5, "%d", 20 );
			sprintf_e( chLabel, "%d_%s_%s", 20, szFuncName.c_str(), rkName.c_str() );
		}
		else
		{
			SafeSprintf( chCD5, sizeof(chCD5), "%1", 20 );
			SafeSprintf( chLabel, sizeof(chLabel), "%1_%2_%3", 20, szFuncName.c_str(), rkName.c_str() );
		}

		// GAME_END_ERR_POS
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Game"
			, "Error"
			, chLabel
			, 1
			, "%2FGAME%2FOVER%2FERR"
			, 1 );

		g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chCD5 );
	}	
#endif

	LOG.PrintTimeAndLog( 0, "ExitProgram - 20" );
#ifdef __DEV_QA__
	LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %s Not Exist Item, Func : %s", rkName.c_str(), szFuncName.c_str() );
#else
	LOG.PrintTimeAndLog( 0, "ioItemMaker::GetItemConst - %s Not Exist Item", rkName.c_str() );
#endif

	return NULL;
}

int ioItemMaker::GetNumItemTemplete() const
{
	return m_ItemList.size();
}

ioItem* ioItemMaker::CreateItemTemplete( ioItem::ItemType eType, ioINILoader &rkLoader )
{
	switch( eType )
	{
	case ioItem::IT_WEAPON:
		return CreateWeaponItemTemplete( rkLoader );
	case ioItem::IT_ARMOR:
		return new ioArmorItem;
	case ioItem::IT_HELMET:
		return new ioHelmetItem;
	case ioItem::IT_CLOAK:
		return new ioCloakItem;
	case ioItem::IT_OBJECT:
		return CreateObjectItemTemplete( rkLoader );
	case ioItem::IT_WEAR:
		return CreateWearItemTemplete( rkLoader );
	case ioItem::IT_RING:
		return new ioRingItem;
	case ioItem::IT_NECKLACE:
		return new ioNecklaceItem;
	case ioItem::IT_BRACELET:
		return new ioBraceletItem;
	}

	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreateItemTemplete - %d UnknownType", eType );

	return NULL;
}

ioItem* ioItemMaker::CreateWeaponItemTemplete( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "sub_type", "", szBuf, MAX_PATH );

	if( !strcmp( "FIRE", szBuf ) )
		return new ioFireWeaponItem;
	if( !strcmp( "CHARGE_CONTACT", szBuf ) )
		return new ioChargeContactWeaponItem;
	if( !strcmp( "HOOK", szBuf ) )
		return new ioHookItem;
	if( !strcmp( "MAGIC_STAFF", szBuf ) )
		return new ioMagicStaffItem;
	if( !strcmp( "HEAL_STAFF", szBuf ) )
		return new ioHealStaffItem;
	if( !strcmp( "BOW", szBuf) )
		return new ioBowItem;
	if( !strcmp( "MINE", szBuf ) )
		return new ioMineItem;
	if( !strcmp( "LASER", szBuf ) )
		return new ioLaserItem;
	if( !strcmp( "SHIELD", szBuf ) )
		return new ioShieldWeaponItem;
	if( !strcmp( "CREATE_STAFF", szBuf ) )
		return new ioCreateStaffItem;
	if( !strcmp( "POWER_CHARGE", szBuf ) )
		return new ioPowerChargeItem;
	if( !strcmp( "AUTO_SHOT", szBuf ) )
		return new ioAutoShotItem;
	if( !strcmp( "CHARGE_AREAWEAPON", szBuf ) )
		return new ioChargeAreaweaponItem;
	if( !strcmp( "CHARGE_SHOT", szBuf ) )
		return new ioChargeShotItem;
	if( !strcmp( "CHARGE_SHOT2", szBuf ) )
		return new ioChargeShotItem2;
	if( !strcmp( "COUNT_CHARGE", szBuf ) )
		return new ioCountChargeItem;
	if( !strcmp( "RUN_CHARGE", szBuf ) )
		return new ioRunChargeItem;
	if( !strcmp( "RANGE_CHARGE", szBuf ) )
		return new ioRangeChargeItem;
	if( !strcmp( "ROTATE_CHARGE", szBuf ) )
		return new ioRotateChargeItem;
	if( !strcmp( "SUMMON_CHARGE", szBuf ) )
		return new ioSummonChargeItem;
	if( !strcmp( "SUMMON_CHARGE3", szBuf ) )
		return new ioSummonChargeItem3;
	if( !strcmp( "LINEAR_CHARGE", szBuf ) )
		return new ioLinearChargeItem;
	if( !strcmp( "COUNT_CHARGE2", szBuf ) )
		return new ioCountChargeItem2;
	if( !strcmp( "SHIELD2", szBuf ) )
		return new ioShieldWeaponItem2;
	if( !strcmp( "SHIELD3", szBuf ) )
		return new ioShieldWeaponItem3;
	if( !strcmp( "SUMMON_CHARGE2", szBuf ) )
		return new ioSummonChargeItem2;
	if( !strcmp( "POWER_CHARGE2", szBuf ) )
		return new ioPowerChargeItem2;
	if( !strcmp( "GRAPPLING", szBuf ) )
		return new ioGrapplingItem;
	if( !strcmp( "DUMMY_CHAR", szBuf ) )
		return new ioDummyCharItem;
	if( !strcmp( "CROSS_BOW", szBuf ) )
		return new ioCrossBowItem;
	if( !strcmp( "CHARGE_MOVE_ITEM", szBuf ) )
		return new ioChargeMoveItem;
	if( !strcmp( "LINEAR_CHARGE2", szBuf ) )
		return new ioLinearChargeItem2;
	if( !strcmp( "POWER_CHARGE3", szBuf ) )
		return new ioPowerChargeItem3;
	if( !strcmp( "CHARGE_MOVE_ITEM2", szBuf ) )
		return new ioChargeMoveItem2;
	if( !strcmp( "POWER_CHARGE4", szBuf ) )
		return new ioPowerChargeItem4;
	if( !strcmp( "MAGIC_STAFF2", szBuf ) )
		return new ioMagicStaffItem2;
	if( !strcmp( "UROBORUS_ITEM", szBuf ) )
		return new ioUroborusItem;
	if( !strcmp( "RUN_CHARGE2", szBuf ) )
		return new ioRunChargeItem2;
	if( !strcmp( "RUN_CHARGE3", szBuf ) )
		return new ioRunChargeItem3;
	if( !strcmp( "WEREWOLF", szBuf ) )
		return new ioWereWolfItem;
	if( !strcmp( "POWER_CHARGE5", szBuf ) )
		return new ioPowerChargeItem5;
	if( !strcmp( "CONSECUTION_ITEM", szBuf ) )
		return new ioConsecutionItem;
	if( !strcmp( "BUFF_ADD_ITEM", szBuf ) )
		return new ioBuffAddItem;
	if( !strcmp( "GATOTKACA_ITEM", szBuf ) )
		return new ioGatotkacaItem;
	if( !strcmp( "RACHEL_ITEM", szBuf ) )
		return new ioRachelItem;
	if( !strcmp( "ILJIMAE_ITEM", szBuf) )
		return new ioIljimaeItem;
	if( !strcmp( "ALTER_EGO", szBuf ) )
		return new ioAlterEgoItem;
	if( !strcmp( "WITCH_ITEM", szBuf ) )
		return new ioWitchItem;
	if( !strcmp( "EXCALIBUR", szBuf ) )
		return new ioExcaliburItem;
	if( !strcmp( "MECHANICS", szBuf ) )
		return new ioMechanicsItem;
	if( !strcmp( "CHARGE_COMBO_ITEM", szBuf ) )
		return new ioChargeComboItem;
	if( !strcmp( "POWER_CHARGE6", szBuf ) )
		return new ioPowerChargeItem6;
	if( !strcmp( "CHARGE_COMMAND", szBuf ) )
		return new ioChargeCommandItem;
	if( !strcmp( "CHARGE_CONTACT2", szBuf ) )
		return new ioChargeContactWeaponItem2;
	if( !strcmp( "PHANTOM_ITEM", szBuf ) )
		return new ioPhantomItem;
	if( !strcmp( "MAGIC_STAFF3", szBuf ) )
		return new ioMagicStaffItem3;
	if( !strcmp( "LUCIFER_ITEM", szBuf) )
		return new ioLuciferItem;
	if( !strcmp( "YUMIRA_ITEM", szBuf ) )
		return new ioYuMiRaItem;
	if( !strcmp( "CHARGECOMBOBOW_ITEM", szBuf ) )
		return new ioChargeComboBowItem;
	if( !strcmp( "NAKORURU", szBuf ) )
		return new ioNakoruruItem;
	if( !strcmp( "DESTROYER", szBuf ) )
		return new ioDestroyerItem;
	if( !strcmp( "MICHAEL", szBuf ) )
		return new ioMichaelItem;
	if( !strcmp( "GOOMIHO", szBuf ) )
		return new ioGoomihoItem;
	if( !strcmp( "PETERPAN_ITEM", szBuf ) )
		return new ioPeterpanItem;
	if( !strcmp( "SUCCUBUS", szBuf ) )
		return new ioSuccubusItem;
	if( !strcmp( "CYVOGUE", szBuf ) )
		return new ioCyvogueItem;
	if( !strcmp( "TRINITY", szBuf ) )
		return new ioTrinityItem;
	if( !strcmp( "WIND_LORD", szBuf ) )
		return new ioWindLordItem;
	if( !strcmp( "SHAOLIN_ITEM", szBuf ) )
		return new ioShaolinItem;
	if( !strcmp( "ANUBIS_ITEM", szBuf ) )
		return new ioAnubisItem;
	if( !strcmp( "SCATHI_ITEM", szBuf ) )
		return new ioScathiItem;
	if( !strcmp( "GHOST_STEALER", szBuf ) )
		return new ioGhostStealer;
	if( !strcmp( "JINMORI_ITEM", szBuf ) )
		return new ioJinMoRiItem;
	if( !strcmp( "DRACULA_ITEM", szBuf ) )
		return new ioDraculaItem;
	if( !strcmp( "PROJECT_K", szBuf ) )
		return new ioProject_K_Item;
	if( !strcmp( "POWER_CHARGE_SHIELD", szBuf ) )
		return new ioPowerChargeShieldItem;
	if( !strcmp( "HAKUMEN_ITEM", szBuf ) )
		return new ioHakumenItem;
	if( !strcmp( "DEVIL_ITEM", szBuf ) )
		return new ioDevilItem;
	if( !strcmp( "MAID_ITEM", szBuf ) )
		return new ioMaidItem;
	if( !strcmp( "FLASH_ITEM", szBuf ) )
		return new ioFlash_Item;
	if( !strcmp( "EVY_ITEM", szBuf ) )
		return new ioEvyItem;
	if( !strcmp( "PSYCHO_ITEM", szBuf ) )
		return new ioPsychoMetry_Item;
	if( !strcmp( "SUDDEN_ATTACK_ITEM", szBuf ) )
		return new ioSuddenAttackItem;
	if( !strcmp( "SLASHER_ITEM", szBuf ) )
		return new ioSlasher_Item;
	if( !strcmp( "DUMMY_CHAR2", szBuf ) )
		return new ioDummyCharItem2;
	if( !strcmp( "MAGIC_STAFF4", szBuf ) )
		return new ioMagicStaffItem4;
	if( !strcmp( "TITAN_ITEM", szBuf ) )
		return new ioTitanItem;
	if( !strcmp( "SUPER_DIMENTION_ITEM", szBuf ) )
		return new ioSuperDimensionItem;
	if( !strcmp( "TRACE_ITEM", szBuf ) )
		return new ioTraceItem;
	if( !strcmp( "KAELTHAS_ITEM", szBuf ) )
		return new ioKaelthasItem;
	if( !strcmp( "GALAXY_ITEM", szBuf ) )
		return new ioGalaxyItem;
	if( !strcmp( "DRAGON_SLAYER_ITEM", szBuf ) )
		return new ioDragonSlayerItem;
	if( !strcmp( "NANGIN_ITEM", szBuf ) )
		return new ioNanginItem;
	if( !strcmp( "STRIKER_ITEM", szBuf ) )
		return new ioStrikerItem;
	if( !strcmp( "DON_QUIXOTE_ITEM", szBuf ) )
		return new ioDonQuixoteItem;
	if( !strcmp( "DRAGONNEST_ASS_ITEM", szBuf ) )
		return new ioDragonNestAssassinItem;
	if( !strcmp( "THUNDER_BIRD_ITEM", szBuf ) )
		return new ioThunderBirdItem;
	if( !strcmp( "ALICE_ITEM", szBuf ) )
		return new ioAliceItem;
	if( !strcmp( "LEE_ITEM", szBuf ) )
		return new ioLeeItem;
	if( !strcmp( "VALKYRIE", szBuf ) )
		return new ioValkyrie;
	if( !strcmp( "PARKILPYO_ITEM", szBuf ) )
		return new ioParkilpyoItem;
	if( !strcmp( "JORO_EXPANSION", szBuf ) )
		return new ioJoroExpansionItem;
	if( !strcmp( "EARTH_QUAKE", szBuf ) )
		return new ioEarthQuakeItem;
	if( !strcmp( "PENTJAK_SILAT_ITEM", szBuf) )
		return new ioPentjakSilatItem;
	if( !strcmp( "JACK_ITEM", szBuf ) )
		return new ioJackItem;
	if( !strcmp( "SHADOW_RANGER", szBuf ) )
		return new ioShadowRangerItem;
	if( !strcmp( "NINJA_EXPANSION", szBuf ) )
		return new ioNinjaExpansionItem;
	if( !strcmp( "DARK_KNIGHT", szBuf ) )
		return new ioDarkKnightItem;
	if( !strcmp( "JUMPER", szBuf ) )
		return new ioJumperItem;
	if( !strcmp( "ROBIN_EXPANSION", szBuf) )
		return new ioRobinExpansionItem;
	if( !strcmp("ENGINEER", szBuf ) )
		return new ioEngineerItem;
	if( !strcmp( "GEOMJE", szBuf) )
		return new ioGeomjeItem;
	if( !strcmp("FIRE_MAGE_EXPANSION", szBuf) )
		return new ioFireMageExpansionItem;
	if( !strcmp( "ZEUS", szBuf ) )
		return new ioZeusItem;
	if( !strcmp( "SABER", szBuf ) )
		return new ioSaberItem;
	if( !strcmp( "OUTBOXER", szBuf ) )
		return new ioOutBoxerItem;
	if( !strcmp( "ANGEL_OF_DEATH", szBuf ) )
		return new ioAngelOfDeathItem;
	if( !strcmp( "HADES", szBuf ) )
		return new ioHadesItem;
	if( !strcmp( "MAGIC_SWORD", szBuf ) )
		return new ioMagicSwordItem;
	if( !strcmp( "NATA", szBuf ) )
		return new ioNataItem;
	if( !strcmp( "TIGER_EXPANSION", szBuf ) )
		return new ioTigerExpansionItem;
	if( !strcmp( "LEESUNSIN", szBuf ) )
		return new ioLeeSunSin;
	if( !strcmp( "RESEARCHER", szBuf ) )
		return new ioResearcher;
	if( !strcmp( "BLADE_ITEM", szBuf ) )
		return new ioBladeItem;
	if( !strcmp( "GRAPPLING_EXPANSION", szBuf ) )
		return new ioGrapplingExpansionItem;
	if( !strcmp( "BULLFIGHT", szBuf ) )
		return new ioBullFightItem;
	if( !strcmp( "JEONWOOCHI_ITEM", szBuf ) )
		return new ioJeonWooChiItem;
	if( !strcmp( "SAWANG_ITEM", szBuf ) )
		return new ioSawangItem;
	if( !strcmp( "CHEMISTRY_ITEM", szBuf ) )
		return new ioChemistryItem;
	if( !strcmp( "CHAIN_MAGICIAN", szBuf ) )
		return new ioChainMagicianItem;
	if( !strcmp( "RED_HOOD_ITEM", szBuf ) )
		return new ioRedHoodItem;
	if( !strcmp( "STRIDER_ITEM", szBuf ) )
		return new ioStriderItem;
	if( !strcmp( "BOW2", szBuf) )
		return new ioBowItem2;
	if( !strcmp( "NEPHILIM_ITEM", szBuf ) )
		return new ioNephilimItem;
	if( !strcmp( "APOSTATE_MONK", szBuf) )
		return new ioApostatemonkItem;
	if( !strcmp( "MUTANT_ITEM", szBuf ) )
		return new ioMutantItem;
	if( !strcmp( "GANESA", szBuf ) )
		return new ioGanesa;
	if( !strcmp( "SAMURAI_EXPASION", szBuf ) )
		return new ioSamuraiExpansionItem;
	if( !strcmp( "MIR_WARRIOR_ITEM", szBuf ) )
		return new ioMirWarrior;
	if( !strcmp( "CUCHULAIN_ITEM", szBuf ) )
		return new ioCuchulainItem;
	if( !strcmp( "GALFORD_ITEM", szBuf  ) )
		return new ioGalfordItem;
	if( !strcmp( "ONIMUSHA_ITEM", szBuf ) )
		return new ioOniMushaItem;	
	if( !strcmp( "WEST_GUN_EXTEND_ITEM", szBuf ) )
		return new ioWestGunExtendItem;
	if( !strcmp( "DRUID", szBuf  ) )
		return new ioDruidItem;
	if( !strcmp( "SASINMU_ITEM", szBuf ) )
		return new ioSasinmuItem;
	if( !strcmp( "DARK_SEAS", szBuf ) )
		return new ioDarkSeasItem;
	if( !strcmp( "PREDATOR_ITEM", szBuf ) )
		return new ioPredatorItem;
	if( !strcmp( "CROW_ITEM", szBuf ) )
		return new ioCrowItem;
	if( !strcmp( "MEPHISTO_ITEM", szBuf) )
		return new ioMephistoItem;
	if( !strcmp( "BLACK_SPHEAR", szBuf ) )
		return new ioBlackSpearItem;
	if( !strcmp( "GUNNER_ITEM", szBuf ) )
		return new ioGunnerItem;
	if( !strcmp( "TRAPPER", szBuf ) )
		return new ioTrapItem;
	if( !strcmp( "BANGTAN", szBuf ) )
		return new ioBangtanItem;
	if( !strcmp( "WITCH_DOCTOR", szBuf ) )
		return new ioWitchDoctoritem;
	if( !strcmp( "SURFER_ITEM", szBuf ) )
		return new ioSurferItem;
	if( !strcmp( "BUBBLE_FIGTHER", szBuf ) )
		return new ioBubbleFighterItem;
	if( !strcmp( "URIEL_ITEM", szBuf ) )
		return new ioUrielItem;
	if( !strcmp( "PATRIOT_ITEM", szBuf ) )
		return new ioPatriotItem;
	if( !strcmp( "PUNGSIN_ITEM", szBuf ) )
		return new ioWindGodItem;
	if( !strcmp( "WARP_ITEM", szBuf ) )
		return new ioWarpItem;
	if( !strcmp("DRILL_ITEM", szBuf ) )
		return new ioDrillItem;
	if( !strcmp( "STRANGER_ITEM", szBuf ) )
		return new ioStrangerItem;
	if( !strcmp( "GARGOYLE_ITEM", szBuf ) )
		return new ioGargoyleItem;
	if( !strcmp( "HACKER_ITEM", szBuf ) )
		return new ioHackerItem;
	if ( !strcmp( "CANNON_ITEM", szBuf ) )
		return new ioCannonWeaponItem;
	if ( !strcmp( "MAGIC_ENGINEER_ITEM", szBuf ) )
		return new ioMagicEngineerItem;
	if ( !strcmp( "JINRAH_ITEM", szBuf ) )
		return new ioJinrahItem;
	if ( !strcmp( "KUNGFU_MASTER_ITEM", szBuf ) )
		return new ioKungfuMasterItem;
	if ( !strcmp( "NIGHTMARE_ITEM", szBuf ) )
		return new ioNightmareItem;
	if ( !strcmp( "AZAZEL_ITEM", szBuf ) )
		return new ioAzazelItem;
	if( !strcmp( "MEGA_AGENT_ITEM", szBuf) )
		return new ioMegaAgentItem;
	if( !strcmp( "NEO_ITEM", szBuf ) )
		return new ioNeoItem;
	if( !strcmp( "SANTA_ITEM", szBuf ) )
		return new ioSantaItem;
	if( !strcmp( "JACK_FROST_ITEM", szBuf ) )
		return new ioJackFrostItem;
	if( !strcmp( "AUTOMATON", szBuf ) )
		return new ioAutomatonItem;
	if( !strcmp( "GARDENER_ITEM", szBuf ) )
		return new ioGardenerItem;
	if( !strcmp( "BASEBALL_ITEM", szBuf ) )
		return new ioBaseballItem;
	if( !strcmp( "UFC_ITEM", szBuf ) )
		return new ioUFCItem;
	if( !strcmp( "SOCCER_ITEM", szBuf ) )
		return new ioSoccerItem;
	if( !strcmp( "VULCAN_ITEM", szBuf ) )
		return new ioVulcanItem;
	if( !strcmp( "BUBBLE_ITEM", szBuf ) )
		return new ioBubbleItem;
	if( !strcmp( "CHANGE_ON_ITEM", szBuf ) )
		return new ioChangeOnItem;
	if( !strcmp( "MURIM_GOSU_ITEM", szBuf ) )
		return new ioMurimGosuItem;
	if( !strcmp( "GHOST_ITEM", szBuf ) )
		return new ioGhostItem;
	if( !strcmp( "SLOPE_ITEM", szBuf ) )
		return new ioSlopeItem;
	if ( !strcmp( "ODIN_ITEM", szBuf ) )
		return new ioOdinItem;
	if ( !strcmp( "HANZO_ITEM", szBuf ) )
		return new ioHanzoItem;
	if( !strcmp( "PHOENIXBOW_ITEM", szBuf ) )
		return new ioPhoenixBowItem;
	if( !strcmp( "TIMEGATE_ITEM", szBuf ) )
		return new ioTimeGateItem;
	if( !strcmp( "OMEN_ITEM", szBuf ) )
		return new ioOmenItem;
	if( !strcmp( "MADBLASTER_ITEM", szBuf ) )
		return new ioMadBlasterItem;
	if( !strcmp( "DR_SPIDER_ITEM", szBuf ) )
		return new ioDrSpiderItem;
	if( !strcmp( "LUPIN_ITEM", szBuf ) )
		return new iolupinItem;
	if( !strcmp( "MIDNIGHT_ITEM", szBuf ) )
		return new ioMidnightItem;
	if( !strcmp( "ROCKET_LAUNCHER_ITEM", szBuf ) )
		return new ioRocketLauncherItem;
	if( !strcmp( "DIMENSIONIST_ITEM", szBuf ) )
		return new ioDimensionistItem;
	if( !strcmp( "PAPARAZZI_ITEM", szBuf ) )
		return new ioPaparazziItem;
	if( !strcmp( "TYR_ITEM", szBuf ) )
		return new ioTyrItem;
	if( !strcmp( "NUCLEARSOLDIER_ITEM", szBuf ) )	// 2020-02-14 신규용병 레어(뉴클리어 솔져) 추가
		return new ioNuClearSoldierItem;
	if( !strcmp( "FROZEN_ITEM", szBuf ) )			// 2020-30-02 신규용병 레어(얼음 신) 추가
		return new ioFrozenItem;
	if( !strcmp( "POSEIDON_ITEM", szBuf ) )
		return new ioPoseidonItem;
	if( !strcmp( "SKY_WALKER_ITEM", szBuf ) )
		return new ioSkyWalkerItem;
	if( !strcmp( "GWONWANG_ITEM", szBuf ) )			// 2020-05-21 신규용병 레어(권왕) 추가 
		return new ioGwonwangItem;
	if( !strcmp( "GENISYSARMY_ITEM", szBuf ) )		// 2020-06-25 신규용병 레어(제네시스) 추가 
		return new ioGenisysarmyItem;
	if( !strcmp( "LILLY_ITEM", szBuf ) )
		return new ioLillyItem;
	if( !strcmp( "HERACLES_ITEM", szBuf ) )
		return new ioHeraclesItem;
	if( !strcmp( "SIZE_ITEM", szBuf ) )			// 2020-09-14 신규용병 레어(사이즈) 추가 
		return new ioSizeItem;
	if( !strcmp( "EVA_ITEM", szBuf ) )			// 2020-10-16 신규용병 레어(이브) 추가 
		return new ioEvaItem;
	if( !strcmp( "MAGNETIC_ITEM", szBuf ) )			// 2020-11-19 신규용병 레어(마그네틱) 추가 
		return new ioMagneticItem;
	if( !strcmp( "TAOIST_ITEM", szBuf ) )
		return new ioTaoistItem;

	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreateWeaponItemTesmplete - %s Unknown SubType", szBuf );
	return NULL;
}

ioItem* ioItemMaker::CreateWearItemTemplete( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "sub_type", "", szBuf, MAX_PATH );

	if( !strcmp( "HELMET", szBuf ) )
		return new ioHelmetWearItem;
	else if( !strcmp( "CLOAK", szBuf ) )
		return new ioCloakWearItem;

	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreateWearItemTemplete - %s Unknown SubType", szBuf );
	return NULL;
}

ioItem* ioItemMaker::CreateObjectItemTemplete( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "sub_type", "", szBuf, MAX_PATH );

	if( !strcmp( "THROW", szBuf ) )
		return new ioThrowItem;
	else if( !strcmp( "THROWBOMB", szBuf ) )
		return new ioThrowBombItem;
	else if( !strcmp( "DRINK", szBuf ) )
		return new ioDrinkItem;
	else if( !strcmp( "EXCAVATION", szBuf ) )
		return new ioExcavationItem;
	else if( !strcmp( "CONTACT_WEAPON", szBuf ) )
		return new ioObjectWeaponItem;
	else if( !strcmp( "WERE_WOLF", szBuf ) )
		return new ioObjectWereWolfItem;
	else if( !strcmp( "SMART_BOOM", szBuf ) )
		return new ioSmartBoomObjectItem;
	else if( !strcmp( "POWERED", szBuf ) )
		return new ioPoweredObjectItem;
	else if( !strcmp( "POWER_CHARGE", szBuf ) )
		return new ioObjectPowerChargeItem;
	else if( !strcmp( "CHARGE_SHOT", szBuf ) )
		return new ioObjectChargeShotItem;
	else if( !strcmp( "DOUBLE_SHOT", szBuf ) )
		return new ioObjectDoubleShotItem;
	else if( !strcmp( "JIN_MO_RI_GP", szBuf ) )
		return new ioObjectJinMoRiItem;
	else if( !strcmp( "TARGET_SHOT", szBuf ) )
		return new ioObjectTargetShotItem;
	else if( !strcmp( "RUNNING_ATTACK", szBuf ) )
		return new ioObjectRunningAttackItem;
	else if( !strcmp( "COUNT", szBuf ) )
		return new ioObjectCountItem;
	else if( !strcmp( "GALAXY", szBuf ) )
		return new ioGalaxyObjectItem;
	else if( !strcmp( "ATTACH", szBuf ) )
		return new ioObjectAttachItem;
	else if( !strcmp( "THUNDER_BIRD", szBuf ) )
		return new ioThunderBirdObjectItem;	
	else if( !strcmp( "ALICE_GUN", szBuf ) )
		return new ioAliceObjectItem;
	else if( !strcmp( "PARKILPYO", szBuf ) )
		return new ioParkilpyoObjectItem;
	else if( !strcmp( "BLADE_CHARGE", szBuf ) )
		return new ioBladeChargeObjItem;
	else if( !strcmp( "SPIRAL_CHARGE", szBuf ) )
		return new ioSpiralChargeObjectItem;
	else if( !strcmp( "VEHICLE_DRIVER", szBuf ) )
		return new ioVehicleDriverObjectItem;
	else if( !strcmp( "DARKKNIGHT_OBJECT", szBuf ) )
		return new ioDarkKnightObjectItem;
	else if( !strcmp( "RUNNING_COMBO_ATTACK", szBuf ) )
		return new ioObjectRunningComboAttackItem;
	else if( !strcmp( "ONIMUSHA", szBuf ) )
		return new ioOniMushaObjectItem;
	else if( !strcmp( "DRUID_BEAR", szBuf ) )
		return new ioDruidObjectItem;
	else if( !strcmp( "JACK_FORST", szBuf ) )
		return new ioJackFrostObjectItem;
	else if( !strcmp( "MIDNIGHT_SCYTHE", szBuf ) )
		return new ioMidnightScytheObjectItem;
	else if( !strcmp( "MIDNIGHT_GUN", szBuf ) )
		return new ioMidnightGunObjectItem;
	else if( !strcmp( "BANKRUPTCY", szBuf ) )
		return new ioObjectBankruptcyItem;	// 2020-06-08

	LOG.PrintTimeAndLog( 0, "ioItemMaker::CreateObjectItemTemplete - %s Unknown SubType", szBuf );
	return NULL;
}

void ioItemMaker::SetNextCreateIndex( DWORD dwIndex )
{
	m_dwNextCreateIndex = dwIndex;
}

DWORD ioItemMaker::GetNextCreateIndex()
{
	DWORD dwNext = m_dwNextCreateIndex;
	m_dwNextCreateIndex++;

	return dwNext;
}

void ioItemMaker::AddNewItem( ioItem *pItem )
{
	if( pItem )
	{
		m_ItemList.push_back( pItem );

		m_ItemCodeMap.insert( ItemCodeMap::value_type( pItem->GetItemCode(), pItem ) );
		m_ItemNameMap.insert( ItemNameMap::value_type( pItem->GetName(), pItem ) );
	}
}

void ioItemMaker::IncreaseItemCreateCnt( DWORD dwItemCode )
{
	ItemCreateMap::iterator iter = m_ItemCreateMap.find( dwItemCode );
	if( iter != m_ItemCreateMap.end() )
	{
		iter->second++;
	}
	else
	{
		m_ItemCreateMap.insert( ItemCreateMap::value_type( dwItemCode, 1 ) );
	}
}

void ioItemMaker::NotifyItemDestroyed( DWORD dwItemCode )
{
	ItemCreateMap::iterator iter = m_ItemCreateMap.find( dwItemCode );
	if( iter != m_ItemCreateMap.end() )
	{
		int iRefCount = iter->second;
		iRefCount--;

		if( iRefCount >= 0 )
		{
			iter->second = iRefCount;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioItemMaker::NotifyItemDestroyed - %d Item Underflow(%d)",
									dwItemCode, iRefCount );
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioItemMaker::NotifyItemDestoryed - %d Not Created", dwItemCode );
	}
}

int ioItemMaker::GetCreatedCnt( DWORD dwItemCode ) const
{
	ItemCreateMap::const_iterator iter = m_ItemCreateMap.find( dwItemCode );
	if( iter != m_ItemCreateMap.end() )
		return iter->second;

	return 0;
}

DWORDVec ioItemMaker::GetSetItemList( DWORD dwSetCode )
{
	DWORDVec vSetList;

	ItemVector::iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		ioItem *pItem = *iter;
		
		if( pItem->GetSetItemCode() == dwSetCode )
		{
			vSetList.push_back( pItem->GetItemCode() );
		}
	}

	return vSetList;
}

const ioHashString& ioItemMaker::GetItemLargeIcon( DWORD dwItemCode, DWORD dwType ) const
{
	const ioItem *pItem = GetItemConst( dwItemCode, __FUNCTION__ );
	if( pItem )
		return pItem->GetItemLargeIconName();

	static ioHashString sBlank;
	return sBlank;
}

const ioItem* ioItemMaker::GetWeaponRandom( bool bIncludeNoItem ) const
{
	ioWeaponItem *pWeapon;
	ItemVector vWeaponList;

	ItemVector::const_iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		pWeapon = ToWeaponItem( *iter );
		if( !pWeapon )	continue;

		if( CheckCanRandomSupply( pWeapon, false ) )
		{
			vWeaponList.push_back( pWeapon );
		}
	}

	if( vWeaponList.empty() )
		return NULL;

	int iItemIdx = PickRandomItemNum( vWeaponList.size(), bIncludeNoItem );
	if( iItemIdx == -1 )
		return NULL;

	return vWeaponList[iItemIdx];
}

const ioItem* ioItemMaker::GetArmorRandom( bool bIncludeNoItem ) const
{
	ioArmorItem *pArmor;
	ItemVector vArmorList;

	ItemVector::const_iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end(); ++iter )
	{
		pArmor = ToArmorItem( *iter );
		if( pArmor && CheckCanRandomSupply( pArmor, false ) )
		{
			vArmorList.push_back( pArmor );
		}
	}

	if( vArmorList.empty() )
		return NULL;

	int iItemIdx = PickRandomItemNum( vArmorList.size(), bIncludeNoItem );
	if( iItemIdx == -1 )
		return NULL;

	return vArmorList[iItemIdx];
}

const ioItem* ioItemMaker::GetCloakRandom( bool bIncludeNoItem ) const
{
	ioCloakItem *pCloak;
	ItemVector vCloakList;

	ItemVector::const_iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		pCloak = ToCloakItem( *iter );
		if( pCloak && CheckCanRandomSupply( pCloak, false ) )
		{
			vCloakList.push_back( pCloak );
		}
	}

	if( vCloakList.empty() )
		return NULL;

	int iItemIdx = PickRandomItemNum( vCloakList.size(), bIncludeNoItem );
	if( iItemIdx == -1 )
		return NULL;

	return vCloakList[iItemIdx];
}

const ioItem* ioItemMaker::GetHelmetRandom( bool bIncludeNoItem ) const
{
	ioHelmetItem *pHelmet;
	ItemVector vHelmetList;

	ItemVector::const_iterator iter;
	for( iter=m_ItemList.begin() ; iter!=m_ItemList.end() ; ++iter )
	{
		pHelmet = ToHelmetItem( *iter );
		if( pHelmet && CheckCanRandomSupply( pHelmet, false ) )
		{
			vHelmetList.push_back( pHelmet );
		}
	}

	if( vHelmetList.empty() )
		return NULL;

	int iItemIdx = PickRandomItemNum( vHelmetList.size(), bIncludeNoItem );
	if( iItemIdx == -1 )
		return NULL;

	return vHelmetList[iItemIdx];
}

int ioItemMaker::PickRandomItemNum( int iItemCnt, bool bIncludeNoItem ) const
{
	int iMaxRandom = iItemCnt;
	if( bIncludeNoItem )
		iMaxRandom++;

	int iRandWeaponIdx = rand() % iMaxRandom;
	if( iRandWeaponIdx == iItemCnt )
		return -1;

	return iRandWeaponIdx;
}

bool ioItemMaker::CheckCanRandomSupply( ioItem *pItem, bool bIncludeLimitItem ) const
{
	int iCreateLimit = pItem->GetItemCreateLimit();
	if( iCreateLimit == ITEM_CREATE_NO_LIMIT )
		return true;

	if( bIncludeLimitItem )
	{
		if( iCreateLimit + 1 > GetCreatedCnt( pItem->GetItemCode() ) )	// Tempelete생성때 한개를 이미 만들었다.
			return true;
	}

	return false;
}

bool ioItemMaker::IsItem( DWORD dwCode )
{
	if( dwCode == 0 )
		return false;

	ItemCodeMap::const_iterator iter = m_ItemCodeMap.find( dwCode );
	if( iter != m_ItemCodeMap.end() )
	{
		ioItem *pItem = (ioItem*)iter->second;
		if( pItem )
			return true;
	}

	return false;
}

ioItemMaker& ioItemMaker::GetSingleton()
{
	return Singleton<ioItemMaker>::GetSingleton();
}