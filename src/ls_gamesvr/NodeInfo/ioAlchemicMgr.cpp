

#include "stdafx.h"

#include "../Util/IORandom.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../DataBase/LogDBClient.h"

#include "User.h"

#include "ioSetItemInfo.h"
#include "ioSetItemInfoManager.h"
#include "ioItemInfoManager.h"

#include "ioAlchemicInventory.h"
#include "ioAlchemicMgr.h"
#include "ioPresentHelper.h"

#include <strsafe.h>
#include "ioSaleManager.h"

extern CLog EventLOG;
extern CLog RateCheckLOG;

//////////////////////////////////////////////////////////////////////////////////////////
template<> ioAlchemicMgr* Singleton< ioAlchemicMgr >::ms_Singleton = 0;

ioAlchemicMgr::ioAlchemicMgr()
{
	m_MainFailRandom.SetRandomSeed( timeGetTime() );

	m_RecipeRandom.SetRandomSeed( timeGetTime()+1 );
	m_RandomRandom.SetRandomSeed( timeGetTime()+2 );
	m_ReinforceRandom.SetRandomSeed( timeGetTime()+3 );
	m_TradeTypeRandom.SetRandomSeed( timeGetTime()+4 );

	m_CountRandom.SetRandomSeed( timeGetTime()+5 );
	m_DisassembleRandom.SetRandomSeed( timeGetTime()+6 );
	m_NormalRandom.SetRandomSeed( timeGetTime() + 7 );

	m_PeriodRandom.SetRandomSeed( timeGetTime() + 8 );

	m_iTotalSoulStoneRandom = 0;
}

ioAlchemicMgr::~ioAlchemicMgr()
{
	ClearAllInfo();
}

void ioAlchemicMgr::ClearAllInfo()
{
	m_AlchemicFuncInfoList.clear();

	m_RecipeMap.clear();
	m_RandomMap.clear();
	m_ReinforceMap.clear();
	m_TradeTypeMap.clear();

	m_SoldierDisassembleMap.clear();
	m_ExtraItemDisassembleMap.clear();

	//
	m_NewAlchemicSoldierInfoMap.clear();
	m_NewAlchemicItemInfoMap.clear();

	m_SoldierPeriodTabelMap.clear();
	m_ItemPeriodTabelMap.clear();
	
	m_SoldierPeriodInfoList.clear();
	m_ItemPeriodInfoList.clear();

	m_vSoultoneRandomInfo.clear();
}

void ioAlchemicMgr::LoadMgrInfo()
{
	char szKey[MAX_PATH] = "";
	
	ClearAllInfo();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_alchemic_info.ini" );

	ioINILoader kLoader2;
	kLoader2.ReloadFile( "config/sp2_new_alchemic_info.ini" );

	kLoader.SetTitle( "common_info" );

	// 실패시 재료 소모 비율
	m_fMinFailRate = kLoader.LoadFloat( "alchemic_fail_min_rate", 25.0f );
	m_fMaxFailRate = kLoader.LoadFloat( "alchemic_fail_max_rate", 75.0f );

	// 첨가물 갯수에 대한 상황별 곱하는 상수값
	m_fAdditiveConstSoldier = kLoader.LoadFloat( "additive_const_soldier", 1.0f );
	m_fAdditiveConstItem = kLoader.LoadFloat( "additive_const_item", 1.0f );
	m_fAdditiveConstExchange = kLoader.LoadFloat( "additive_const_exchange", 1.0f );
	
	// 아이템 change 상황에서 변화 비율
	m_fPieceChangeConstMin = kLoader.LoadFloat( "piece_change_const_min", 1.0f );
	m_fPieceChangeConstMax = kLoader.LoadFloat( "piece_change_const_max", 1.0f );

	// alchemicitem 판매시 곱하는 상수값
	m_fItemSellConst = kLoader.LoadFloat( "item_sell_const", 1.0f );

	// disassemble
	m_dwSoldierDisassembleConst = kLoader.LoadInt( "soldier_disassemble_const", 100 );
	m_dwExtraItemDisassembleConst = kLoader.LoadInt( "extraitem_disassemble_const", 50 );

	// 변환 최소갯수
	m_iMinTotalCnt = kLoader.LoadInt( "change_min_cnt", 10 );

	//
	m_fMinExchangeRate = kLoader.LoadFloat( "exchange_min_rate", 25.0f );
	m_fMaxExchangeRate = kLoader.LoadFloat( "exchange_max_rate", 100.0f );

	//
	int iInfoCnt = kLoader.LoadInt( "alchemic_info_cnt", 0 );
	for( int i=0; i < iInfoCnt; ++i )
	{
		wsprintf( szKey, "alchemic_info%d", i+1 );
		kLoader.SetTitle( szKey );
		LoadFuncInfo( kLoader );
	}

	kLoader.SetTitle( "soulstone_gain_random" );
	LoadSoulStoneInfo( kLoader );

	LoadRecipeInfo( kLoader );
	LoadRandomInfo( kLoader );
	LoadReinforceInfo( kLoader );
	LoadTradeTypeInfo( kLoader );

	// disassemble
	LoadSoldierDisassembleInfo( kLoader );
	LoadExtraItemDisassembleInfo( kLoader );

	// new alchemic info
	LoadNewAlchemicListTable( kLoader2 );
	LoadNewAlchemicInfo( kLoader2 );
	LoadNewAlchemicPeriodInfo( kLoader2 );
}

void ioAlchemicMgr::LoadFuncInfo( ioINILoader &rkLoader )
{
	AlchemicFuncInfo kFuncInfo;

	kFuncInfo.m_iCode = rkLoader.LoadInt( "alchemic_code", 0 );
	kFuncInfo.m_bActive = rkLoader.LoadBool( "alchemic_active", false );
	kFuncInfo.m_AlchemicType = (AlchemicType)rkLoader.LoadInt( "alchemic_type", ALT_NONE );

	kFuncInfo.m_iMaxCnt1 = rkLoader.LoadInt( "alchemic_max_cnt1", 0 );
	kFuncInfo.m_iMaxCnt2 = rkLoader.LoadInt( "alchemic_max_cnt2", 0 );
	kFuncInfo.m_iMaxCnt3 = rkLoader.LoadInt( "alchemic_max_cnt3", 0 );
	kFuncInfo.m_iMaxCnt4 = rkLoader.LoadInt( "alchemic_max_cnt4", 0 );
	kFuncInfo.m_iMaxAdditive = rkLoader.LoadInt( "alchemic_max_additive", 0 );

	kFuncInfo.m_fSuccessRate = rkLoader.LoadFloat( "alchemic_rate", 0.0f );
	kFuncInfo.m_iPeriodValue = rkLoader.LoadInt( "alchemic_period_value", 0 );

	kFuncInfo.m_iRecipeListNum = rkLoader.LoadInt( "alchemic_recipe_num", 0 );
	kFuncInfo.m_iRandomListNum = rkLoader.LoadInt( "alchemic_random_num", 0 );
	kFuncInfo.m_iReinforceNum = rkLoader.LoadInt( "alchemic_reinforce_num", 0 );
	kFuncInfo.m_iTradeTypeNum = rkLoader.LoadInt( "alchemic_tradetype_num", 0 );

	kFuncInfo.m_iNewAlchemicListTable = rkLoader.LoadInt( "new_alchemic_table_num", 0 );

	m_AlchemicFuncInfoList.push_back( kFuncInfo );
}

void ioAlchemicMgr::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_alchemic_info.ini" );
	if( kLoader.ReadBool( "common_info", "Change", false ) )
	{
		LoadMgrInfo();
	}
}

void ioAlchemicMgr::LoadSoulStoneInfo( ioINILoader &rkLoader )
{
	int iMaxRandom = rkLoader.LoadInt( "max_cnt", 0 );
	m_iTotalSoulStoneRandom = 0;
	char szKey[MAX_PATH]="";

	for( int i = 0; i < iMaxRandom; i++ )
	{
		wsprintf( szKey, "gain%d_random", i+1 );
		int iRandom = rkLoader.LoadInt( szKey, 0 );
		m_iTotalSoulStoneRandom += iRandom;
		m_vSoultoneRandomInfo.push_back(iRandom);
	}
}

void ioAlchemicMgr::LoadRecipeInfo( ioINILoader &rkLoader )
{
	enum 
	{
		MAX_LOOP = 100,
	};

	char szKey[MAX_PATH]="";

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		wsprintf( szKey, "recipe_table%d", i+1 );
		rkLoader.SetTitle( szKey );

		int iRecipeCnt = rkLoader.LoadInt( "recipe_cnt", 0 );
		if( iRecipeCnt <= 0 )
			break;

		vRecipeResultInfoList vRecipeList;
		vRecipeList.clear();
		for( int j=0; j < iRecipeCnt; ++j )
		{
			RecipeResultInfo kRecipeInfo;

			wsprintf( szKey, "recipe%d_piece1", j+1 );
			kRecipeInfo.m_iPieceCode1 = rkLoader.LoadInt( szKey, 0 );
			wsprintf( szKey, "recipe%d_piece2", j+1 );
			kRecipeInfo.m_iPieceCode2 = rkLoader.LoadInt( szKey, 0 );
			wsprintf( szKey, "recipe%d_piece3", j+1 );
			kRecipeInfo.m_iPieceCode3 = rkLoader.LoadInt( szKey, 0 );
			wsprintf( szKey, "recipe%d_piece4", j+1 );
			kRecipeInfo.m_iPieceCode4 = rkLoader.LoadInt( szKey, 0 );
			wsprintf( szKey, "recipe%d_additive", j+1 );
			kRecipeInfo.m_iAdditiveCode = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "recipe%d_success_rate", j+1 );
			kRecipeInfo.m_fSuccessRate = rkLoader.LoadFloat( szKey, 0.0f );

			wsprintf( szKey, "recipe%d_result", j+1 );
			kRecipeInfo.m_iResultCode = rkLoader.LoadInt( szKey, 0 );

			vRecipeList.push_back( kRecipeInfo );
		}

		m_RecipeMap.insert( RecipeResultInfoMap::value_type( i+1, vRecipeList ) );
	}
}

void ioAlchemicMgr::LoadRandomInfo( ioINILoader &rkLoader )
{
	enum 
	{
		MAX_LOOP = 100,
	};

	char szKey[MAX_PATH]="";

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		wsprintf( szKey, "random_table%d", i+1 );
		rkLoader.SetTitle( szKey );

		int iRandomCnt = rkLoader.LoadInt( "random_cnt", 0 );
		if( iRandomCnt <= 0 )
			break;

		RandomResultInfo kRandomInfo;

		for( int j=0; j < iRandomCnt; ++j )
		{
			RandomResult kRandomResult;

			wsprintf( szKey, "random%d_rate", j+1 );
			kRandomResult.m_iRandomRate = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "random%d_result", j+1 );
			kRandomResult.m_iResultCode = rkLoader.LoadInt( szKey, 0 );

			kRandomInfo.m_vList.push_back( kRandomResult );
			kRandomInfo.m_dwTotalRate += kRandomResult.m_iRandomRate;
		}

		m_RandomMap.insert( RandomResultInfoMap::value_type( i+1, kRandomInfo ) );
	}
}

void ioAlchemicMgr::LoadReinforceInfo( ioINILoader &rkLoader )
{
	enum 
	{
		MAX_LOOP = 100,
	};

	char szKey[MAX_PATH]="";

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		wsprintf( szKey, "reinforce_table%d", i+1 );
		rkLoader.SetTitle( szKey );

		int iReinforceCnt = rkLoader.LoadInt( "reinforce_cnt", 0 );
		if( iReinforceCnt <= 0 )
			break;

		RandomReinforceInfo kReinforceInfo;

		for( int j=0; j < iReinforceCnt; ++j )
		{
			RandomReinforce kReinforce;

			wsprintf( szKey, "reinforce%d_rate", j+1 );
			kReinforce.m_iRandomRate = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "reinforce%d_value", j+1 );
			kReinforce.m_iReinforce = rkLoader.LoadInt( szKey, 0 );

			kReinforceInfo.m_vList.push_back( kReinforce );
			kReinforceInfo.m_dwTotalRate += kReinforce.m_iRandomRate;
		}

		m_ReinforceMap.insert( RandomReinforceInfoMap::value_type( i+1, kReinforceInfo ) );
	}
}

void ioAlchemicMgr::LoadNewAlchemicListTable( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_NewAlchemicListMap.clear();

	rkLoader.SetTitle( "new_alchemic_table" );

	int iTableCnt = rkLoader.LoadInt( "new_alchemic_table_cnt", 0 );
	for( int i=0; i < iTableCnt; ++i )
	{
		wsprintf( szKey, "alchemic_table%d_list_cnt", i+1 );
		int iListCnt = rkLoader.LoadInt( szKey, 0 );

		IntVec vValueList;
		for( int j=0; j < iListCnt; ++j )
		{
			wsprintf( szKey, "alchemic_table%d_value%d", i+1, j+1 );
			int iAlchemicValue = rkLoader.LoadInt( szKey, 0 );

			vValueList.push_back( iAlchemicValue );
		}

		m_NewAlchemicListMap.insert( AlcemicListMap::value_type( i+1, vValueList ) );
	}
}

void ioAlchemicMgr::LoadNewAlchemicInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_NewAlchemicSoldierInfoMap.clear();
	m_NewAlchemicItemInfoMap.clear();

	rkLoader.SetTitle( "new_alchemic_info" );

	m_iSoldierAdditive = rkLoader.LoadInt( "soldier_additive", 0 );
	m_iWeaponAdditive = rkLoader.LoadInt( "weapon_additive", 0 );
	m_iArmorAdditive = rkLoader.LoadInt( "armor_additive", 0 );
	m_iHelmetAdditive = rkLoader.LoadInt( "helmet_additive", 0 );
	m_iCloakAdditive = rkLoader.LoadInt( "cloak_additive", 0 );

	m_iNewAlchemicMinTotalCnt = rkLoader.LoadInt( "new_alchemic_min_cnt", 1 );

	// soldier
	int iSoldierInfoCnt = rkLoader.LoadInt( "rare_soldier_info_cnt", 0 );
	for( int i=0; i < iSoldierInfoCnt; ++i )
	{
		wsprintf( szKey, "rare_soldier%d_result", i+1 );
		int iResultValue = rkLoader.LoadInt( szKey, 0 );

		NewAlchemicInfo kNewInfo;

		wsprintf( szKey, "rare_soldier%d_piece", i+1 );
		kNewInfo.m_iPieceNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "rare_soldier%d_additive", i+1 );
		kNewInfo.m_iAdditiveNum = rkLoader.LoadInt( szKey, 0 );

		m_NewAlchemicSoldierInfoMap.insert( NewAlchemicInfoMap::value_type( iResultValue, kNewInfo ) );
	}

	// item
	int iItemInfoCnt = rkLoader.LoadInt( "rare_item_info_cnt", 0 );	
	for( int j=0; j < iItemInfoCnt; ++j )
	{
		wsprintf( szKey, "rare_item%d_result", j+1 );
		int iResultValue = rkLoader.LoadInt( szKey, 0 );

		NewAlchemicInfo kNewInfo;

		wsprintf( szKey, "rare_item%d_piece", j+1 );
		kNewInfo.m_iPieceNum = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "rare_item%d_additive", j+1 );
		kNewInfo.m_iAdditiveNum = rkLoader.LoadInt( szKey, 0 );

		m_NewAlchemicItemInfoMap.insert( NewAlchemicInfoMap::value_type( iResultValue, kNewInfo ) );
	}
}

void ioAlchemicMgr::LoadNewAlchemicPeriodInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_SoldierPeriodInfoList.clear();
	m_ItemPeriodInfoList.clear();

	// soldier
	rkLoader.SetTitle( "new_alchemic_soldier_period_info" );
	
	int iPieceCnt = rkLoader.LoadInt( "piece_group_cnt", 0 );
	for( int i=0; i < iPieceCnt; ++i )
	{
		NewAlchemicPeriodInfo kPeriodInfo;

		wsprintf( szKey, "piece_group%d_limit_value", i+1 );
		kPeriodInfo.m_dwLimitCnt = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "piece_group%d_table_num", i+1 );
		kPeriodInfo.m_dwTableNum = rkLoader.LoadInt( szKey, 0 );

		m_SoldierPeriodInfoList.push_back( kPeriodInfo );
	}

	if( !m_SoldierPeriodInfoList.empty() )
	{
		std::sort( m_SoldierPeriodInfoList.begin(), m_SoldierPeriodInfoList.end(), PeriodInfoSort() );
	}

	int iTableCnt = rkLoader.LoadInt( "period_table_cnt", 0 );
	for( int j=0; j < iTableCnt; ++j )
	{
		NewAlchemicPeriodTable kTable;

		wsprintf( szKey, "period_table%d_period_cnt", j+1 );
		int iPeriodCnt = rkLoader.LoadInt( szKey, 0 );

		for( int k=0; k < iPeriodCnt; ++k )
		{
			RandomValue kValue;

			wsprintf( szKey, "period_table%d_period%d_value", j+1, k+1 );
			kValue.m_iValue = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "period_table%d_period%d_rate", j+1, k+1 );
			kValue.m_iRandomRate = rkLoader.LoadInt( szKey, 0 );

			kTable.m_vList.push_back( kValue );
			kTable.m_dwTotalRate += kValue.m_iRandomRate;
		}

		m_SoldierPeriodTabelMap.insert( NewAlchemicPeriodTableMap::value_type( j+1, kTable ) );
	}

	// item
	rkLoader.SetTitle( "new_alchemic_item_period_info" );
	
	iPieceCnt = rkLoader.LoadInt( "piece_group_cnt", 0 );
	for( int i=0; i < iPieceCnt; ++i )
	{
		NewAlchemicPeriodInfo kPeriodInfo;

		wsprintf( szKey, "piece_group%d_limit_value", i+1 );
		kPeriodInfo.m_dwLimitCnt = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "piece_group%d_table_num", i+1 );
		kPeriodInfo.m_dwTableNum = rkLoader.LoadInt( szKey, 0 );

		m_ItemPeriodInfoList.push_back( kPeriodInfo );
	}

	if( !m_ItemPeriodInfoList.empty() )
	{
		std::sort( m_ItemPeriodInfoList.begin(), m_ItemPeriodInfoList.end(), PeriodInfoSort() );
	}

	iTableCnt = rkLoader.LoadInt( "period_table_cnt", 0 );
	for( int j=0; j < iTableCnt; ++j )
	{
		NewAlchemicPeriodTable kTable;

		wsprintf( szKey, "period_table%d_period_cnt", j+1 );
		int iPeriodCnt = rkLoader.LoadInt( szKey, 0 );

		for( int k=0; k < iPeriodCnt; ++k )
		{
			RandomValue kValue;

			wsprintf( szKey, "period_table%d_period%d_value", j+1, k+1 );
			kValue.m_iValue = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "period_table%d_period%d_rate", j+1, k+1 );
			kValue.m_iRandomRate = rkLoader.LoadInt( szKey, 0 );

			kTable.m_vList.push_back( kValue );
			kTable.m_dwTotalRate += kValue.m_iRandomRate;
		}

		m_ItemPeriodTabelMap.insert( NewAlchemicPeriodTableMap::value_type( j+1, kTable ) );
	}
}

void ioAlchemicMgr::LoadTradeTypeInfo( ioINILoader &rkLoader )
{
	enum 
	{
		MAX_LOOP = 100,
	};

	char szKey[MAX_PATH]="";

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		wsprintf( szKey, "tradetype_table%d", i+1 );
		rkLoader.SetTitle( szKey );

		int iTradeTypeCnt = rkLoader.LoadInt( "tradetype_cnt", 0 );
		if( iTradeTypeCnt <= 0 )
			break;

		RandomTradeType kTradeType;

		for( int n=0; n < TRADE_TYPE_MAX; ++n )
		{
			wsprintf( szKey, "tradetype_rate%d", n+1 );
			kTradeType.m_iRandomRate[n] = rkLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "tradetype_value%d", n+1 );
			kTradeType.m_iTradeType[n] = rkLoader.LoadInt( szKey, 0 );

			kTradeType.m_dwTotalRate += kTradeType.m_iRandomRate[n];
		}

		m_TradeTypeMap.insert( RandomTradeTypeInfoMap::value_type( i+1, kTradeType ) );
	}
}

void ioAlchemicMgr::LoadSoldierDisassembleInfo( ioINILoader &rkLoader )
{
	m_SoldierDisassembleMap.clear();

	rkLoader.SetTitle( "soldier_disassemble_info" );

	char szKey[MAX_PATH]="";

	int iCnt = rkLoader.LoadInt( "disassemble_info_cnt", 0 );
	int iCode = 0;
	int iResultCode = 0;

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf( szKey, "disassemble%d_code", i+1 );
		iCode = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "disassemble%d_result", i+1 );
		iResultCode = rkLoader.LoadInt( szKey, 0 );

		m_SoldierDisassembleMap.insert( DisassembleInfoMap::value_type( iCode, iResultCode ) );
	}
}

void ioAlchemicMgr::LoadExtraItemDisassembleInfo( ioINILoader &rkLoader )
{
	m_ExtraItemDisassembleMap.clear();

	rkLoader.SetTitle( "extraitem_disassemble_info" );

	char szKey[MAX_PATH]="";

	int iCnt = rkLoader.LoadInt( "disassemble_info_cnt", 0 );
	int iCode = 0;
	int iResultCode = 0;

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf( szKey, "disassemble%d_code", i+1 );
		iCode = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "disassemble%d_result", i+1 );
		iResultCode = rkLoader.LoadInt( szKey, 0 );

		m_ExtraItemDisassembleMap.insert( DisassembleInfoMap::value_type( iCode, iResultCode ) );
	}
}

bool ioAlchemicMgr::FindAlchemicFunc( int iCode, AlchemicFuncInfo &rkFuncInfo )
{
	rkFuncInfo.Init();

	if( m_AlchemicFuncInfoList.empty() )
		return false;

	vAlchemicFuncInfoList::iterator iter = m_AlchemicFuncInfoList.begin();
	while( iter != m_AlchemicFuncInfoList.end() )
	{
		if( (*iter).m_iCode == iCode )
		{
			rkFuncInfo = (*iter);
			return true;
		}

		++iter;
	}

	return false;
}

int ioAlchemicMgr::AlchemicSoldierFunc( User *pUser,
										int iCode,
										int iPiece1, int iValue1,
										int iPiece2, int iValue2,
										int iPiece3, int iValue3,
										int iPiece4, int iValue4,
										int iAdditive, int iAddValue )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_SOLDIER )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	if( iPiece1 > 0 && (iPiece1 == iPiece2 || iPiece1 == iPiece3 || iPiece1 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece2 > 0 && (iPiece2 == iPiece1 || iPiece2 == iPiece3 || iPiece2 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece3 > 0 && (iPiece3 == iPiece1 || iPiece3 == iPiece2 || iPiece3 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece4 > 0 && (iPiece4 == iPiece1 || iPiece4 == iPiece2 || iPiece4 == iPiece3) )
		return ART_EXCEPTION;

	// 아이템, 첨가물 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 < 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iValue2 < 0 || iCurCount < iValue2 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece3 );
	if( iValue3 < 0 || iCurCount < iValue3 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece4 );
	if( iValue4 < 0 || iCurCount < iValue4 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iAdditive );
	if( iAddValue < 0 || iCurCount < iAddValue )
		return ART_NOT_MACH_VALUE;

	// 1차로 아이템 갯수에 의한 성공률 체크
	float fMaxCnt = kAlchemicFunc.m_iMaxCnt1 + kAlchemicFunc.m_iMaxCnt2 + kAlchemicFunc.m_iMaxCnt3 + kAlchemicFunc.m_iMaxCnt4;
	float fCurCnt = (iValue1+iValue2+iValue3+iValue4) + (m_fAdditiveConstSoldier*iAddValue);

	if( fMaxCnt <= 0.0f )
		return ART_EXCEPTION;

	if( fCurCnt <= 0.0f )
		return ART_EXCEPTION;

	float fSuccessRate = fCurCnt / fMaxCnt;
	int iRand = m_CountRandom.Random( 10001 );
	float fRandRate = (float)iRand / 10000.0f;

	int iUseCnt1 = iValue1+iValue2+iValue3+iValue4;
	int iUseCnt2 = iAddValue;

	if( fSuccessRate < fRandRate )
	{
		// 소모처리
		float fRate = GetCurFailRandomRate();
		int iNewValue1 = iValue1 * fRate;
		if( iValue1 < 10 )
			iNewValue1 = iValue1;
		pAlchemicInven->UseAlchemicItem( iPiece1, iNewValue1 );

		fRate = GetCurFailRandomRate();
		int iNewValue2 = iValue2 * fRate;
		if( iValue2 < 10 )
			iNewValue2 = iValue2;
		pAlchemicInven->UseAlchemicItem( iPiece2, iNewValue2 );

		fRate = GetCurFailRandomRate();
		int iNewValue3 = iValue3 * fRate;
		if( iValue3 < 10 )
			iNewValue3 = iValue3;
		pAlchemicInven->UseAlchemicItem( iPiece3, iNewValue3 );

		fRate = GetCurFailRandomRate();
		int iNewValue4 = iValue4 * fRate;
		if( iValue4 < 10 )
			iNewValue4 = iValue4;
		pAlchemicInven->UseAlchemicItem( iPiece4, iNewValue4 );

		fRate = GetCurFailRandomRate();
		int iNewAdd = iAddValue * fRate;
		if( iAddValue < 10 )
			iNewAdd = iAddValue;
		pAlchemicInven->UseAlchemicItem( iAdditive, iNewAdd );

		// 실패 정보전송
		SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
		PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_SOLDIER_FAIL) );
		PACKET_GUARD_INT( kReturn.Write(iPiece1) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue1) );
		PACKET_GUARD_INT( kReturn.Write(iPiece2) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue2) );
		PACKET_GUARD_INT( kReturn.Write(iPiece3) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue3) );
		PACKET_GUARD_INT( kReturn.Write(iPiece4) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue4) );
		PACKET_GUARD_INT( kReturn.Write(iAdditive) );
		PACKET_GUARD_INT( kReturn.Write(iNewAdd) );

		pUser->SendMessage( kReturn );

		// 실패 로그
		iUseCnt1 = iNewValue1 + iNewValue2 + iNewValue3 + iNewValue4;
		iUseCnt2 = iNewAdd;

		g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_SOLDIER, kAlchemicFunc.m_iCode, LogDBClient::AFRT_FAIL,
											iUseCnt1, iUseCnt2, 0, 0 );

		return ART_FAIL;
	}

	bool bSuccess = false;
	// Recipe 체크, recipe에 있는지, recipe로 성공하는지까지 체크
	int iRecipeResult = CheckRecipeFunc( kAlchemicFunc.m_iRecipeListNum, iPiece1, iPiece2, iPiece3, iPiece4, iAdditive );
	if( iRecipeResult > 0 )
		bSuccess = true;

	// random 체크
	int iRandomResult = 0;
	if( !bSuccess )
	{
		iRandomResult = CheckRandomFunc( kAlchemicFunc.m_iRandomListNum );
		if( iRandomResult > 0 )
			bSuccess = true;
	}

	if( bSuccess )
	{
		// 소모처리
		pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
		pAlchemicInven->UseAlchemicItem( iPiece2, iValue2 );
		pAlchemicInven->UseAlchemicItem( iPiece3, iValue3 );
		pAlchemicInven->UseAlchemicItem( iPiece4, iValue4 );
		pAlchemicInven->UseAlchemicItem( iAdditive, iAddValue );

		// 선물 발송
		int iClassType = iRecipeResult;
		if( iClassType <= 0 )
			iClassType = iRandomResult;

		g_PresentHelper.SendPresentByAlchemicSoldier( pUser, iClassType, kAlchemicFunc.m_iPeriodValue );

		// 결과 전송
		SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
		PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_SOLDIER_SUCC) );
		PACKET_GUARD_INT( kReturn.Write(iPiece1) );
		PACKET_GUARD_INT( kReturn.Write(iValue1) );
		PACKET_GUARD_INT( kReturn.Write(iPiece2) );
		PACKET_GUARD_INT( kReturn.Write(iValue2) );
		PACKET_GUARD_INT( kReturn.Write(iPiece3) );
		PACKET_GUARD_INT( kReturn.Write(iValue3) );
		PACKET_GUARD_INT( kReturn.Write(iPiece4) );
		PACKET_GUARD_INT( kReturn.Write(iValue4) );
		PACKET_GUARD_INT( kReturn.Write(iAdditive) );
		PACKET_GUARD_INT( kReturn.Write(iAddValue) );
		PACKET_GUARD_INT( kReturn.Write(iClassType) );

		pUser->SendMessage( kReturn );

		// 성공로그
		if( iRecipeResult > 0 )
			g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_SOLDIER, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RECIPE_SUCC,
												iUseCnt1, iUseCnt2, iClassType, 0 );
		else
			g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_SOLDIER, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RANDOM_SUCC,
												iUseCnt1, iUseCnt2, iClassType, 0 );

		g_LogDBClient.OnInsertChar( pUser, iClassType, kAlchemicFunc.m_iPeriodValue, 0, "", LogDBClient::CT_ALCHEMIC_SOLDIER );

		return ART_SUCCESS;
	}

	// 소모처리
	float fRate = GetCurFailRandomRate();
	int iNewValue1 = iValue1 * fRate;
	if( iValue1 < 10 )
		iNewValue1 = iValue1;
	pAlchemicInven->UseAlchemicItem( iPiece1, iNewValue1 );

	fRate = GetCurFailRandomRate();
	int iNewValue2 = iValue2 * fRate;
	if( iValue2 < 10 )
		iNewValue2 = iValue2;
	pAlchemicInven->UseAlchemicItem( iPiece2, iNewValue2 );

	fRate = GetCurFailRandomRate();
	int iNewValue3 = iValue3 * fRate;
	if( iValue3 < 10 )
		iNewValue3 = iValue3;
	pAlchemicInven->UseAlchemicItem( iPiece3, iNewValue3 );

	fRate = GetCurFailRandomRate();
	int iNewValue4 = iValue4 * fRate;
	if( iValue4 < 10 )
		iNewValue4 = iValue4;
	pAlchemicInven->UseAlchemicItem( iPiece4, iNewValue4 );

	fRate = GetCurFailRandomRate();
	int iNewAdd = iAddValue * fRate;
	if( iAddValue < 10 )
		iNewAdd = iAddValue;
	pAlchemicInven->UseAlchemicItem( iAdditive, iNewAdd );

	// 실패 정보전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_SOLDIER_FAIL) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue1) );
	PACKET_GUARD_INT( kReturn.Write(iPiece2) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue2) );
	PACKET_GUARD_INT( kReturn.Write(iPiece3) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue3) );
	PACKET_GUARD_INT( kReturn.Write(iPiece4) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue4) );
	PACKET_GUARD_INT( kReturn.Write(iAdditive) );
	PACKET_GUARD_INT( kReturn.Write(iNewAdd) );

	pUser->SendMessage( kReturn );

	// 실패 로그
	iUseCnt1 = iNewValue1 + iNewValue2 + iNewValue3 + iNewValue4;
	iUseCnt2 = iNewAdd;

	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_SOLDIER, kAlchemicFunc.m_iCode, LogDBClient::AFRT_FAIL,
										iUseCnt1, iUseCnt2, 0, 0 );

	return ART_FAIL;
}

int ioAlchemicMgr::AlchemicItemFunc( User *pUser,
									 int iCode,
									 int iPiece1, int iValue1,
									 int iPiece2, int iValue2,
									 int iPiece3, int iValue3,
									 int iPiece4, int iValue4,
									 int iAdditive, int iAddValue )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_ITEM )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	if( iPiece1 > 0 && (iPiece1 == iPiece2 || iPiece1 == iPiece3 || iPiece1 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece2 > 0 && (iPiece2 == iPiece1 || iPiece2 == iPiece3 || iPiece2 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece3 > 0 && (iPiece3 == iPiece1 || iPiece3 == iPiece2 || iPiece3 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece4 > 0 && (iPiece4 == iPiece1 || iPiece4 == iPiece2 || iPiece4 == iPiece3) )
		return ART_EXCEPTION;

	// 아이템, 첨가물 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 < 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iValue2 < 0 || iCurCount < iValue2 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece3 );
	if( iValue3 < 0 || iCurCount < iValue3 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece4 );
	if( iValue4 < 0 || iCurCount < iValue4 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iAdditive );
	if( iAddValue < 0 || iCurCount < iAddValue )
		return ART_NOT_MACH_VALUE;

	// 1차로 아이템 갯수에 의한 성공률 체크
	float fMaxCnt = kAlchemicFunc.m_iMaxCnt1 + kAlchemicFunc.m_iMaxCnt2 + kAlchemicFunc.m_iMaxCnt3 + kAlchemicFunc.m_iMaxCnt4;
	float fCurCnt = (float)(iValue1+iValue2+iValue3+iValue4) + (m_fAdditiveConstItem*iAddValue);

	if( fMaxCnt <= 0.0f )
		return ART_EXCEPTION;

	if( fCurCnt <= 0.0f )
		return ART_EXCEPTION;

	float fSuccessRate = fCurCnt / fMaxCnt;
	int iRand = m_CountRandom.Random( 10001 );
	float fRandRate = (float)iRand / 10000.0f;

	int iUseCnt1 = iValue1+iValue2+iValue3+iValue4;
	int iUseCnt2 = iAddValue;

	if( fSuccessRate < fRandRate )
	{
		// 소모처리
		float fRate = GetCurFailRandomRate();
		int iNewValue1 = iValue1 * fRate;
		if( iValue1 < 10 )
			iNewValue1 = iValue1;
		pAlchemicInven->UseAlchemicItem( iPiece1, iNewValue1 );

		fRate = GetCurFailRandomRate();
		int iNewValue2 = iValue2 * fRate;
		if( iValue2 < 10 )
			iNewValue2 = iValue2;
		pAlchemicInven->UseAlchemicItem( iPiece2, iNewValue2 );

		fRate = GetCurFailRandomRate();
		int iNewValue3 = iValue3 * fRate;
		if( iValue3 < 10 )
			iNewValue3 = iValue3;
		pAlchemicInven->UseAlchemicItem( iPiece3, iNewValue3 );

		fRate = GetCurFailRandomRate();
		int iNewValue4 = iValue4 * fRate;
		if( iValue4 < 10 )
			iNewValue4 = iValue4;
		pAlchemicInven->UseAlchemicItem( iPiece4, iNewValue4 );

		fRate = GetCurFailRandomRate();
		int iNewAdd = iAddValue * fRate;
		if( iAddValue < 10 )
			iNewAdd = iAddValue;
		pAlchemicInven->UseAlchemicItem( iAdditive, iNewAdd );

		// 실패 정보전송
		SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
		PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_ITEM_FAIL) );
		PACKET_GUARD_INT( kReturn.Write(iPiece1) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue1) );
		PACKET_GUARD_INT( kReturn.Write(iPiece2) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue2) );
		PACKET_GUARD_INT( kReturn.Write(iPiece3) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue3) );
		PACKET_GUARD_INT( kReturn.Write(iPiece4) );
		PACKET_GUARD_INT( kReturn.Write(iNewValue4) );
		PACKET_GUARD_INT( kReturn.Write(iAdditive) );
		PACKET_GUARD_INT( kReturn.Write(iNewAdd) );

		pUser->SendMessage( kReturn );

		// 실패 로그
		iUseCnt1 = iNewValue1 + iNewValue2 + iNewValue3 + iNewValue4;
		iUseCnt2 = iNewAdd;

		g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXTRAITEM, kAlchemicFunc.m_iCode, LogDBClient::AFRT_FAIL,
											iUseCnt1, iUseCnt2, 0, 0 );

		return ART_FAIL;
	}

	bool bSuccess = false;
	// Recipe 체크, recipe에 있는지, recipe로 성공하는지까지 체크
	int iRecipeResult = CheckRecipeFunc( kAlchemicFunc.m_iRecipeListNum, iPiece1, iPiece2, iPiece3, iPiece4, iAdditive );
	if( iRecipeResult > 0 )
		bSuccess = true;

	// random 체크
	int iRandomResult = 0;
	if( !bSuccess )
	{
		iRandomResult = CheckRandomFunc( kAlchemicFunc.m_iRandomListNum );
		if( iRandomResult > 0 )
			bSuccess = true;
	}

	if( bSuccess )
	{
		// 소모처리
		pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
		pAlchemicInven->UseAlchemicItem( iPiece2, iValue2 );
		pAlchemicInven->UseAlchemicItem( iPiece3, iValue3 );
		pAlchemicInven->UseAlchemicItem( iPiece4, iValue4 );
		pAlchemicInven->UseAlchemicItem( iAdditive, iAddValue );

		// 강화값
		int iReinforce = CheckReinforceFunc( kAlchemicFunc.m_iReinforceNum );

		// 거래타입
		int iTradeType = CheckTradeTypeFunc( kAlchemicFunc.m_iTradeTypeNum );

		// 선물 발송
		int iItemCode = iRecipeResult;
		if( iItemCode <= 0 )
			iItemCode = iRandomResult;

		g_PresentHelper.SendPresentByAlchemicItem( pUser, iItemCode, kAlchemicFunc.m_iPeriodValue, iReinforce );

		// 결과 전송
		SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
		PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_ITEM_SUCC) );
		PACKET_GUARD_INT( kReturn.Write(iPiece1) );
		PACKET_GUARD_INT( kReturn.Write(iValue1) );
		PACKET_GUARD_INT( kReturn.Write(iPiece2) );
		PACKET_GUARD_INT( kReturn.Write(iValue2) );
		PACKET_GUARD_INT( kReturn.Write(iPiece3) );
		PACKET_GUARD_INT( kReturn.Write(iValue3) );
		PACKET_GUARD_INT( kReturn.Write(iPiece4) );
		PACKET_GUARD_INT( kReturn.Write(iValue4) );
		PACKET_GUARD_INT( kReturn.Write(iAdditive) );
		PACKET_GUARD_INT( kReturn.Write(iAddValue) );
		PACKET_GUARD_INT( kReturn.Write(iItemCode) );

		pUser->SendMessage( kReturn );

		// 성공로그
		if( iRecipeResult > 0 )
			g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXTRAITEM, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RECIPE_SUCC,
												iUseCnt1, iUseCnt2, iItemCode, 0 );
		else
			g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXTRAITEM, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RANDOM_SUCC,
												iUseCnt1, iUseCnt2, iItemCode, 0 );

		int iPeriodType = ioUserExtraItem::EPT_TIME;
		if( kAlchemicFunc.m_iPeriodValue == 0 ) // 무제한
		{
			iPeriodType = ioUserExtraItem::EPT_MORTMAIN;
		}

		g_LogDBClient.OnInsertExtraItem( pUser, iItemCode, iReinforce, 0, kAlchemicFunc.m_iPeriodValue, 0, iPeriodType, 0, 0, "NONE", LogDBClient::ERT_ALCHEMIC_ITEM );

		return ART_SUCCESS;
	}

	// 소모처리
	float fRate = GetCurFailRandomRate();
	int iNewValue1 = iValue1 * fRate;
	if( iValue1 < 10 )
		iNewValue1 = iValue1;
	pAlchemicInven->UseAlchemicItem( iPiece1, iNewValue1 );

	fRate = GetCurFailRandomRate();
	int iNewValue2 = iValue2 * fRate;
	if( iValue2 < 10 )
		iNewValue2 = iValue2;
	pAlchemicInven->UseAlchemicItem( iPiece2, iNewValue2 );

	fRate = GetCurFailRandomRate();
	int iNewValue3 = iValue3 * fRate;
	if( iValue3 < 10 )
		iNewValue3 = iValue3;
	pAlchemicInven->UseAlchemicItem( iPiece3, iNewValue3 );

	fRate = GetCurFailRandomRate();
	int iNewValue4 = iValue4 * fRate;
	if( iValue4 < 10 )
		iNewValue4 = iValue4;
	pAlchemicInven->UseAlchemicItem( iPiece4, iNewValue4 );

	fRate = GetCurFailRandomRate();
	int iNewAdd = iAddValue * fRate;
	if( iAddValue < 10 )
		iNewAdd = iAddValue;
	pAlchemicInven->UseAlchemicItem( iAdditive, iNewAdd );

	// 실패 정보전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_ITEM_FAIL) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue1) );
	PACKET_GUARD_INT( kReturn.Write(iPiece2) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue2) );
	PACKET_GUARD_INT( kReturn.Write(iPiece3) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue3) );
	PACKET_GUARD_INT( kReturn.Write(iPiece4) );
	PACKET_GUARD_INT( kReturn.Write(iNewValue4) );
	PACKET_GUARD_INT( kReturn.Write(iAdditive) );
	PACKET_GUARD_INT( kReturn.Write(iNewAdd) );

	pUser->SendMessage( kReturn );

	// 실패 로그
	iUseCnt1 = iNewValue1 + iNewValue2 + iNewValue3 + iNewValue4;
	iUseCnt2 = iNewAdd;

	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXTRAITEM, kAlchemicFunc.m_iCode, LogDBClient::AFRT_FAIL,
										iUseCnt1, iUseCnt2, 0, 0 );

	return ART_FAIL;
}

int ioAlchemicMgr::AlchemicChangeFunc( User *pUser,
									   int iCode,
									   int iPiece1, int iValue1,
									   int iPiece2, int iValue2,
									   int iPiece3, int iValue3,
									   int iPiece4, int iValue4 )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_CHANGE )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	if( iPiece1 > 0 && (iPiece1 == iPiece2 || iPiece1 == iPiece3 || iPiece1 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece2 > 0 && (iPiece2 == iPiece1 || iPiece2 == iPiece3 || iPiece2 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece3 > 0 && (iPiece3 == iPiece1 || iPiece3 == iPiece2 || iPiece3 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece4 > 0 && (iPiece4 == iPiece1 || iPiece4 == iPiece2 || iPiece4 == iPiece3) )
		return ART_EXCEPTION;

	// 아이템 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 < 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iValue2 < 0 || iCurCount < iValue2 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece3 );
	if( iValue3 < 0 || iCurCount < iValue3 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece4 );
	if( iValue4 < 0 || iCurCount < iValue4 )
		return ART_NOT_MACH_VALUE;

	// 변화될 code
	int iRandomResult = CheckRandomFunc( kAlchemicFunc.m_iRandomListNum );
	if( iRandomResult <= 0 )
		return ART_EXCEPTION;

	int iTotalCnt = iValue1+iValue2+iValue3+iValue4;
	if( iTotalCnt < m_iMinTotalCnt )
		return ART_NOT_ENOUGH_CNT;

	// 변화될 수량
	float fMinConstRate = m_fPieceChangeConstMin;
	float fMaxConstRate = m_fPieceChangeConstMax;
	if( fMinConstRate > fMaxConstRate )
	{
		fMinConstRate = m_fPieceChangeConstMax;
		fMaxConstRate = m_fPieceChangeConstMin;
	}

	float fGapConstRate = fMaxConstRate - fMinConstRate;
	int iRand = m_NormalRandom.Random( 10001 );
	float fCurRate = (float)iRand / 10000.0f;

	float fNewChangeRate = fMinConstRate + (fGapConstRate * fCurRate);

	int iChangeCnt = iTotalCnt * fNewChangeRate;
	if( iChangeCnt <= 0 )
		return ART_EXCEPTION;

	// 빈칸, 수량 체크
	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iRandomResult );
	if( iCurCount > 0 )		// 기존에 존재하던 것, 최대갯수 체크 필요
	{
		int iTotal = iCurCount + iChangeCnt;
		if( iTotal > ioAlchemicInventory::MAX_SLOT_CNT )	// 받으면 최대갯수 초과하므로 받기 실패
			return ART_OVER_MAX_CNT;
	}
	else					// 기존에 없던것, 빈칸 체크 필요
	{
		if( !pAlchemicInven->CheckEmptySlot() )			// 빈칸 없으므로 받기 실패
			return ART_NOT_EMPTY_SLOT;
	}

	int iUseCnt1 = iValue1+iValue2+iValue3+iValue4;
	int iUseCnt2 = 0;

	// 소모처리
	pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
	pAlchemicInven->UseAlchemicItem( iPiece2, iValue2 );
	pAlchemicInven->UseAlchemicItem( iPiece3, iValue3 );
	pAlchemicInven->UseAlchemicItem( iPiece4, iValue4 );

	// 획득처리
	pAlchemicInven->GainAlchemicItem( iRandomResult, iChangeCnt );

	// 결과전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_CHANGE_SUCC) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iValue1) );
	PACKET_GUARD_INT( kReturn.Write(iPiece2) );
	PACKET_GUARD_INT( kReturn.Write(iValue2) );
	PACKET_GUARD_INT( kReturn.Write(iPiece3) );
	PACKET_GUARD_INT( kReturn.Write(iValue3) );
	PACKET_GUARD_INT( kReturn.Write(iPiece4) );
	PACKET_GUARD_INT( kReturn.Write(iValue4) );
	PACKET_GUARD_INT( kReturn.Write(iRandomResult) );
	PACKET_GUARD_INT( kReturn.Write(iChangeCnt) );

	pUser->SendMessage( kReturn );

	// 변환로그
	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_CHANGE, kAlchemicFunc.m_iCode, LogDBClient::AFRT_CHANGE_SUCC,
										iUseCnt1, iUseCnt2, iRandomResult, iChangeCnt );

	return ART_SUCCESS;
}

int ioAlchemicMgr::AlchemicExchangeFunc( User *pUser,
										 int iCode,
										 int iPiece1, int iValue1,
										 int iPiece2, int iValue2,
										 int iAdditive, int iAddValue )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_EXCHANGE )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	if( iPiece1 > 0 && iPiece1 == iPiece2 )
		return ART_EXCEPTION;

	// 아이템 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 < 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iCurCount <= 0 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iAdditive );
	if( iAddValue < 0 || iCurCount < iAddValue )
		return ART_NOT_MACH_VALUE;

	int iTotalCnt = iValue1;
	if( iTotalCnt < m_iMinTotalCnt )
		return ART_NOT_ENOUGH_CNT;

	if( kAlchemicFunc.m_iMaxAdditive <= 0 )
		return ART_EXCEPTION;

	// 첨가제 갯수에 곱할 상수값 계산
	float fMaxConstRate = max( m_fAdditiveConstExchange, 1.0f );
	float fMinConstRate = min( m_fAdditiveConstExchange, 1.0f );
	float fGapConstRate = fMaxConstRate - fMinConstRate;

	int iRand = m_NormalRandom.Random( 10001 );
	float fCurRate = (float)iRand / 10000.0f;
	float fNewExchange = fMinConstRate + (fGapConstRate * fCurRate);

	// 조각 갯수에 곱할 비율 계산
	float fGapRate = m_fMaxExchangeRate - m_fMinExchangeRate;
	
	float fNewAddValue = (float)iAddValue * fNewExchange;
	float fChangeRate = fNewAddValue / kAlchemicFunc.m_iMaxAdditive;
	float fResultRate = m_fMinExchangeRate + (fChangeRate * fGapRate);

	// 변화될 수량
	int iChangeCnt = iTotalCnt * fResultRate;
	if( iChangeCnt <= 0 )
		return ART_EXCEPTION;

	// 빈칸, 수량 체크
	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iCurCount > 0 )		// 기존에 존재하던 것, 최대갯수 체크 필요
	{
		int iTotal = iCurCount + iChangeCnt;
		if( iTotal > ioAlchemicInventory::MAX_SLOT_CNT )	// 받으면 최대갯수 초과하므로 받기 실패
			return ART_OVER_MAX_CNT;
	}
	else					// 기존에 없던것, 빈칸 체크 필요
	{
		if( !pAlchemicInven->CheckEmptySlot() )			// 빈칸 없으므로 받기 실패
			return ART_NOT_EMPTY_SLOT;
	}

	// 소모처리
	pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
	pAlchemicInven->UseAlchemicItem( iAdditive, iAddValue );

	int iUseCnt1 = iValue1;
	int iUseCnt2 = iAddValue;

	// 획득처리
	pAlchemicInven->GainAlchemicItem( iPiece2, iChangeCnt );

	// 결과전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_EXCHANGE_SUCC) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iValue1) );
	PACKET_GUARD_INT( kReturn.Write(iAdditive) );
	PACKET_GUARD_INT( kReturn.Write(iAddValue) );
	PACKET_GUARD_INT( kReturn.Write(iPiece2) );
	PACKET_GUARD_INT( kReturn.Write(iChangeCnt) );

	pUser->SendMessage( kReturn );

	// 변환로그
	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXCHANGE, kAlchemicFunc.m_iCode, LogDBClient::AFRT_CHANGE_SUCC,
										iUseCnt1, iUseCnt2, iPiece2, iChangeCnt );

	pUser->DoAdditiveMission(iAddValue, AMT_COMPOUND);
	return ART_SUCCESS;
}

int ioAlchemicMgr::AlchemicSellFunc( User *pUser,
									 int iCode,
									 int iPiece1, int iValue1,
									 int iPiece2, int iValue2,
									 int iPiece3, int iValue3,
									 int iPiece4, int iValue4 )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_SELL )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
	{
		return ART_EXCEPTION;
	}

	if( iPiece1 > 0 && (iPiece1 == iPiece2 || iPiece1 == iPiece3 || iPiece1 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece2 > 0 && (iPiece2 == iPiece1 || iPiece2 == iPiece3 || iPiece2 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece3 > 0 && (iPiece3 == iPiece1 || iPiece3 == iPiece2 || iPiece3 == iPiece4) )
		return ART_EXCEPTION;

	if( iPiece4 > 0 && (iPiece4 == iPiece1 || iPiece4 == iPiece2 || iPiece4 == iPiece3) )
		return ART_EXCEPTION;

	// 아이템 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 < 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece2 );
	if( iValue2 < 0 || iCurCount < iValue2 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece3 );
	if( iValue3 < 0 || iCurCount < iValue3 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece4 );
	if( iValue4 < 0 || iCurCount < iValue4 )
		return ART_NOT_MACH_VALUE;

	// 변화될 peso
	int iTotalCnt = iValue1+iValue2+iValue3+iValue4;
	if( iTotalCnt <= 0 )
		return ART_EXCEPTION;

	__int64 iChangePeso = iTotalCnt * m_fItemSellConst;

	// 소모처리
	bool bFail = false;
	if( !pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 ) )
	{
		if( iPiece1 > 0 && iValue1 > 0 )
			bFail = true;
	}
	if( !pAlchemicInven->UseAlchemicItem( iPiece2, iValue2 ) )
	{
		if( iPiece2 > 0 && iValue2 > 0 )
			bFail = true;
	}
	if( !pAlchemicInven->UseAlchemicItem( iPiece3, iValue3 ) )
	{
		if( iPiece3 > 0 && iValue3 > 0 )
			bFail = true;
	}
	if( !pAlchemicInven->UseAlchemicItem( iPiece4, iValue4 ) )
	{
		if( iPiece4 > 0 && iValue4 > 0 )
			bFail = true;
	}

	if( !bFail )
	{
		// 획득처리
		pUser->AddMoney( iChangePeso );
		g_LogDBClient.OnInsertPeso( pUser, iChangePeso, LogDBClient::PT_ALCHEMIC_PESO );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ETC, LogDBClient::PT_ALCHEMIC_PESO, PRESENT_ALCHEMIC_ITEM, 0, iChangePeso, NULL);
	}
	else
		return ART_EXCEPTION;

	int iUseCnt1 = 0;
	int iUseCnt2 = 0;

	if( iPiece1 > ALCHEMIC_ADDITIVE_DIV )
		iUseCnt2 += iValue1;
	else
		iUseCnt1 += iValue1;

	if( iPiece2 > ALCHEMIC_ADDITIVE_DIV )
		iUseCnt2 += iValue2;
	else
		iUseCnt1 += iValue2;

	if( iPiece3 > ALCHEMIC_ADDITIVE_DIV )
		iUseCnt2 += iValue3;
	else
		iUseCnt1 += iValue3;

	if( iPiece4 > ALCHEMIC_ADDITIVE_DIV )
		iUseCnt2 += iValue4;
	else
		iUseCnt1 += iValue4;

	// 결과전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_PESO_SUCC) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iValue1) );
	PACKET_GUARD_INT( kReturn.Write(iPiece2) );
	PACKET_GUARD_INT( kReturn.Write(iValue2) );
	PACKET_GUARD_INT( kReturn.Write(iPiece3) );
	PACKET_GUARD_INT( kReturn.Write(iValue3) );
	PACKET_GUARD_INT( kReturn.Write(iPiece4) );
	PACKET_GUARD_INT( kReturn.Write(iValue4) );
	PACKET_GUARD_INT( kReturn.Write(iChangePeso) );
	PACKET_GUARD_INT( kReturn.Write(pUser->GetMoney()) );

	pUser->SendMessage( kReturn );

	// 변환로그
	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_PESO, kAlchemicFunc.m_iCode, LogDBClient::AFRT_CHANGE_SUCC,
										iUseCnt1, iUseCnt2, iChangePeso, 0 );

	int iAdditiveCount = 0;
	if( ADDITIVE_CODE_NUM == iPiece1 )
		iAdditiveCount += iValue1;
	if( ADDITIVE_CODE_NUM == iPiece2 )
		iAdditiveCount += iValue2;
	if( ADDITIVE_CODE_NUM == iPiece3 )
		iAdditiveCount += iValue3;
	if( ADDITIVE_CODE_NUM == iPiece4 )
		iAdditiveCount += iValue4;

	if( iAdditiveCount != 0 )
		pUser->DoAdditiveMission(iAdditiveCount, AMT_COMPOUND);

	return ART_SUCCESS;
}

int ioAlchemicMgr::CheckRecipeFunc( int iRecipeNum,
									int iPiece1,
									int iPiece2,
									int iPiece3,
									int iPiece4,
									int iAdditive )
{
	if( iRecipeNum == 0 )
		return 0;

	RecipeResultInfoMap::iterator iter = m_RecipeMap.find( iRecipeNum );
	if( iter != m_RecipeMap.end() )
	{
		vRecipeResultInfoList vInfoList = iter->second;

		bool bCheck1, bCheck2, bCheck3, bCheck4, bCheckAdd;
		bool bCheck = false;
		int iSize = vInfoList.size();
		for( int i=0; i < iSize; ++i )
		{
			// 아이템 코드 체크
			bCheck = false;
			bCheck1 = bCheck2 = bCheck3 = bCheck4 = bCheckAdd = false;

			if( !bCheck1 && vInfoList[i].m_iPieceCode1 == iPiece1 )
			{
				bCheck1 = true;
				bCheck = true;
			}
			else if( !bCheck2 && vInfoList[i].m_iPieceCode1 == iPiece2 )
			{
				bCheck2 = true;
				bCheck = true;
			}
			else if( !bCheck3 && vInfoList[i].m_iPieceCode1 == iPiece3 )
			{
				bCheck3 = true;
				bCheck = true;
			}
			else if( !bCheck4 && vInfoList[i].m_iPieceCode1 == iPiece4 )
			{
				bCheck4 = true;
				bCheck = true;
			}

			if( !bCheck ) continue;

			bCheck = false;
			if( !bCheck1 && vInfoList[i].m_iPieceCode2 == iPiece1 )
			{
				bCheck1 = true;
				bCheck = true;
			}
			else if( !bCheck2 && vInfoList[i].m_iPieceCode2 == iPiece2 )
			{
				bCheck2 = true;
				bCheck = true;
			}
			else if( !bCheck3 && vInfoList[i].m_iPieceCode2 == iPiece3 )
			{
				bCheck3 = true;
				bCheck = true;
			}
			else if( !bCheck4 && vInfoList[i].m_iPieceCode2 == iPiece4 )
			{
				bCheck4 = true;
				bCheck = true;
			}

			if( !bCheck ) continue;

			bCheck = false;
			if( !bCheck1 && vInfoList[i].m_iPieceCode3 == iPiece1 )
			{
				bCheck1 = true;
				bCheck = true;
			}
			else if( !bCheck2 && vInfoList[i].m_iPieceCode3 == iPiece2 )
			{
				bCheck2 = true;
				bCheck = true;
			}
			else if( !bCheck3 && vInfoList[i].m_iPieceCode3 == iPiece3 )
			{
				bCheck3 = true;
				bCheck = true;
			}
			else if( !bCheck4 && vInfoList[i].m_iPieceCode3 == iPiece4 )
			{
				bCheck4 = true;
				bCheck = true;
			}

			if( !bCheck ) continue;

			bCheck = false;
			if( !bCheck1 && vInfoList[i].m_iPieceCode4 == iPiece1 )
			{
				bCheck1 = true;
				bCheck = true;
			}
			else if( !bCheck2 && vInfoList[i].m_iPieceCode4 == iPiece2 )
			{
				bCheck2 = true;
				bCheck = true;
			}
			else if( !bCheck3 && vInfoList[i].m_iPieceCode4 == iPiece3 )
			{
				bCheck3 = true;
				bCheck = true;
			}
			else if( !bCheck4 && vInfoList[i].m_iPieceCode4 == iPiece4 )
			{
				bCheck4 = true;
				bCheck = true;
			}

			if( !bCheck ) continue;

			bCheck = false;
			if( !bCheckAdd && vInfoList[i].m_iAdditiveCode == iAdditive )
			{
				bCheckAdd = true;
				bCheck = true;
			}

			// 코드값 만족하면 rate 체크
			if( bCheck )
			{
				int iRand = m_RecipeRandom.Random( 100001 );	// 0 ~ 100000까지 나오게
				float fCurRate = (float)iRand / 100000;

				if( fCurRate <= vInfoList[i].m_fSuccessRate )
				{
					return vInfoList[i].m_iResultCode;
				}
			}
		}
	}

	return 0;
}

int ioAlchemicMgr::CheckRandomFunc( int iRandomNum )
{
	RandomResultInfoMap::iterator iter = m_RandomMap.find( iRandomNum );
	if( iter != m_RandomMap.end() )
	{
		RandomResultInfo kResultInfo = iter->second;

		int iRand = m_RandomRandom.Random( kResultInfo.m_dwTotalRate );
		int iCurPosition = 0;
		int iItemCnt = kResultInfo.m_vList.size();
		
		for( int i=0; i < iItemCnt; ++i )
		{
			int iCurRate = kResultInfo.m_vList[i].m_iRandomRate;
			if( COMPARE( iRand, iCurPosition, iCurPosition+iCurRate ) )
			{
				return kResultInfo.m_vList[i].m_iResultCode;
			}

			iCurPosition += iCurRate;
		}
	}

	return 0;
}

int ioAlchemicMgr::CheckReinforceFunc( int iReinforceNum )
{
	RandomReinforceInfoMap::iterator iter = m_ReinforceMap.find( iReinforceNum );
	if( iter != m_ReinforceMap.end() )
	{
		RandomReinforceInfo kResultInfo = iter->second;

		int iRand = m_ReinforceRandom.Random( kResultInfo.m_dwTotalRate );
		int iCurPosition = 0;
		int iItemCnt = kResultInfo.m_vList.size();

		for( int i=0; i < iItemCnt; ++i )
		{
			int iCurRate = kResultInfo.m_vList[i].m_iRandomRate;
			if( COMPARE( iRand, iCurPosition, iCurPosition+iCurRate ) )
			{
				return kResultInfo.m_vList[i].m_iReinforce;
			}

			iCurPosition += iCurRate;
		}
	}

	return 0;
}

int ioAlchemicMgr::CheckTradeTypeFunc( int iTradeNum )
{
	RandomTradeTypeInfoMap::iterator iter = m_TradeTypeMap.find( iTradeNum );
	if( iter != m_TradeTypeMap.end() )
	{
		RandomTradeType kResultInfo = iter->second;

		int iRand = m_TradeTypeRandom.Random( kResultInfo.m_dwTotalRate );
		int iCurPosition = 0;

		for( int i=0; i < TRADE_TYPE_MAX; ++i )
		{
			int iCurRate = kResultInfo.m_iRandomRate[i];
			if( iCurRate > 0 && COMPARE( iRand, iCurPosition, iCurPosition+iCurRate ) )
			{
				return kResultInfo.m_iTradeType[i];
			}

			iCurPosition += iCurRate;
		}
	}

	return 0;
}

float ioAlchemicMgr::GetCurFailRandomRate()
{
	float fGapRate = m_fMaxFailRate - m_fMinFailRate;

	int iRate = (100 * fGapRate) + 1;
	int iRand = m_MainFailRandom.Random( iRate );

	float fCurRate = (float)iRand / 100;
	fCurRate += m_fMinFailRate;

	return fCurRate;
}

ioAlchemicMgr& ioAlchemicMgr::GetSingleton()
{
	return Singleton< ioAlchemicMgr >::GetSingleton();
}

int ioAlchemicMgr::GetDisassembleCode( int iType, DWORD dwMagicCode )
{
	DisassembleInfoMap::iterator iter;
	int iCode = 0;

	switch( iType )
	{
	case ADT_SOLDIER:
		iter = m_SoldierDisassembleMap.find( dwMagicCode );

		if( iter != m_SoldierDisassembleMap.end() )
			iCode = iter->second;
		else
			iCode = dwMagicCode;
		break;
	case ADT_EXTRAITEM:
		iter = m_ExtraItemDisassembleMap.find( dwMagicCode );

		if( iter != m_ExtraItemDisassembleMap.end() )
			iCode = iter->second;
		else
			iCode = dwMagicCode%1000;

		break;
	}

	return iCode;
}

int ioAlchemicMgr::GetSoldierNeedPiece( int iClassType )
{
	NewAlchemicInfoMap::iterator iter = m_NewAlchemicSoldierInfoMap.find( iClassType );

	if( iter != m_NewAlchemicSoldierInfoMap.end() )
	{
		return iter->second.m_iPieceNum; 
	}
	else
	{
		return iClassType % 1000;
	}

}


int ioAlchemicMgr::GetDisassembleCnt( int iType, bool bMortmain, DWORD dwTime, DWORD dwMagicValue )
{
	int iCount = 0;
	DWORD dwCurTime = dwTime;

	DWORD dwMinCnt = 0;
	DWORD dwMaxCnt = 0;

	switch( iType )
	{
	case ADT_SOLDIER:
		{
			if( bMortmain )
				dwCurTime = m_dwSoldierDisassembleConst;

			dwMinCnt = dwCurTime * 1.0f;
			dwMaxCnt = dwCurTime * 2.0f;
		}
		break;
	case ADT_EXTRAITEM:
		{
			if( bMortmain )
				dwCurTime = m_dwExtraItemDisassembleConst;

			int iCurReinforce = dwMagicValue;
			if( iCurReinforce < 10 )
				iCurReinforce = 10;

			float fReinforceRate = (float)iCurReinforce/10.0;

			dwMinCnt = dwCurTime * 1.0f;
			dwMaxCnt = dwCurTime * fReinforceRate * 2.0f;
		}
		break;
	}

	DWORD dwGapCnt = dwMaxCnt - dwMinCnt;
	if( dwGapCnt > 0 )
	{
		int iRand = m_DisassembleRandom.Random( dwGapCnt+1 );
		iCount = dwMinCnt + iRand;
	}

	return iCount;
}

bool ioAlchemicMgr::CheckAlchemicTable( int iTableNum, int iValue )
{
	AlcemicListMap::iterator iter = m_NewAlchemicListMap.find( iTableNum );
	if( iter != m_NewAlchemicListMap.end() )
	{
		IntVec vList = iter->second;

		int iListSize = vList.size();
		for( int i=0; i < iListSize; ++i )
		{
			if( vList[i] == iValue )
				return true;
		}
	}

	return false;
}

int ioAlchemicMgr::NewAlchemicSoldierFunc( User *pUser,
										   int iCode,
										   int iClassType,
										   int iPiece1, int iValue1,
										   int iAdditive, int iAddValue )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_NEW_SOLDIER )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	// alchemic table check
	if( !CheckAlchemicTable( kAlchemicFunc.m_iNewAlchemicListTable, iClassType ) )
		return ART_TABLE_ERROR;

	DWORD dwSetItemCode = iClassType + SET_ITEM_CODE;
	const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByCode( dwSetItemCode );
	if( !pSetInfo )
		return ART_EXCEPTION;

	if( iPiece1 <= 0 || iValue1 <= 0 )
		return ART_EXCEPTION;

	if( iAdditive <= 0 || iAddValue <= 0 )
		return ART_EXCEPTION;

	if( iValue1 < m_iNewAlchemicMinTotalCnt )
		return ART_NOT_ENOUGH_CNT;

	// rare check
	NewAlchemicInfoMap::iterator iter = m_NewAlchemicSoldierInfoMap.find( iClassType );
	if( iter != m_NewAlchemicSoldierInfoMap.end() )
	{
		if( (iter->second.m_iPieceNum != iPiece1) || (iter->second.m_iAdditiveNum != iAdditive) )
			return ART_NOT_MACH_CODE;
	}
	else
	{
		if( (iClassType != iPiece1) || (m_iSoldierAdditive != iAdditive) )
			return ART_NOT_MACH_CODE;
	}

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	// 아이템, 첨가물 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 <= 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iAdditive );
	if( iAddValue <= 0 || iCurCount < iAddValue )
		return ART_NOT_MACH_VALUE;

	int iUseCnt1 = iValue1;
	int iUseCnt2 = iAddValue;

	int iPeriodValue = GetSoldierPeriodTime( iValue1 );
	if( iPeriodValue == -1 )
		return ART_PERIOD_ERROR;

	// 소모처리
	pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
	pAlchemicInven->UseAlchemicItem( iAdditive, iAddValue );

	// 선물 발송
	g_PresentHelper.SendPresentByAlchemicSoldier( pUser, iClassType, iPeriodValue );

	// 결과 전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_NEW_SOLDIER_SUCC) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iValue1) );
	PACKET_GUARD_INT( kReturn.Write(iAdditive) );
	PACKET_GUARD_INT( kReturn.Write(iAddValue) );
	PACKET_GUARD_INT( kReturn.Write(iClassType) );
	PACKET_GUARD_INT( kReturn.Write(iPeriodValue) );

	pUser->SendMessage( kReturn );

	// 성공로그
	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_SOLDIER, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RECIPE_SUCC,
										iUseCnt1, iUseCnt2, iClassType, 0 );

	g_LogDBClient.OnInsertChar( pUser, iClassType, kAlchemicFunc.m_iPeriodValue, 0, "", LogDBClient::CT_ALCHEMIC_SOLDIER );

	return ART_SUCCESS;
}

int ioAlchemicMgr::NewAlchemicItemFunc( User *pUser,
										int iCode,
										int iItemCode,
										int iPiece1, int iValue1,
										int iAdditive, int iAddValue )
{
	AlchemicFuncInfo kAlchemicFunc;

	if( !FindAlchemicFunc( iCode, kAlchemicFunc ) )
		return ART_NOT_FIND_FUNC;

	if( kAlchemicFunc.m_AlchemicType != ALT_NEW_ITEM )
		return ART_NOT_MACH_FUNC;

	if( !kAlchemicFunc.m_bActive )
		return ART_NOT_FIND_FUNC;

	// alchemic table check
	if( !CheckAlchemicTable( kAlchemicFunc.m_iNewAlchemicListTable, iItemCode ) )
		return ART_TABLE_ERROR;

	const ItemInfo *pItemInfo = g_ItemInfoMgr.GetItemInfo( iItemCode );
	if( !pItemInfo )
		return ART_EXCEPTION;

	ioAlchemicInventory *pAlchemicInven = pUser->GetAlchemicInventory();
	if( !pAlchemicInven )
		return ART_EXCEPTION;

	if( iPiece1 <= 0 || iValue1 <= 0 )
		return ART_EXCEPTION;

	if( iAdditive <= 0 || iAddValue <= 0 )
		return ART_EXCEPTION;

	if( iValue1 < m_iNewAlchemicMinTotalCnt )
		return ART_NOT_ENOUGH_CNT;

	// rare check
	NewAlchemicInfoMap::iterator iter = m_NewAlchemicItemInfoMap.find( iItemCode );
	if( iter != m_NewAlchemicItemInfoMap.end() )
	{
		if( (iter->second.m_iPieceNum != iPiece1) || (iter->second.m_iAdditiveNum != iAdditive) )
			return ART_NOT_MACH_CODE;
	}
	else
	{
		int iClassType = iItemCode % 1000;
		int iSlot = iItemCode / 100000;
		int iAlchemicAdditive = 0;

		switch( iSlot )
		{
		case 0:
			iAlchemicAdditive = m_iWeaponAdditive;
			break;
		case 1:
			iAlchemicAdditive = m_iArmorAdditive;
			break;
		case 2:
			iAlchemicAdditive = m_iHelmetAdditive;
			break;
		case 3:
			iAlchemicAdditive = m_iCloakAdditive;
			break;
		}

		if( (iClassType != iPiece1) || (iAlchemicAdditive != iAdditive) )
			return ART_NOT_MACH_CODE;
	}

	// 아이템, 첨가물 갯수 유효성 체크
	int iCurCount = pAlchemicInven->GetAlchemicItemCnt( iPiece1 );
	if( iValue1 <= 0 || iCurCount < iValue1 )
		return ART_NOT_MACH_VALUE;

	iCurCount = pAlchemicInven->GetAlchemicItemCnt( iAdditive );
	if( iAddValue <= 0 || iCurCount < iAddValue )
		return ART_NOT_MACH_VALUE;

	//
	int iUseCnt1 = iValue1;
	int iUseCnt2 = iAddValue;

	// 소모처리
	pAlchemicInven->UseAlchemicItem( iPiece1, iValue1 );
	pAlchemicInven->UseAlchemicItem( iAdditive, iAddValue );

	// 강화값
	int iReinforce = CheckReinforceFunc( kAlchemicFunc.m_iReinforceNum );

	// 거래타입
	int iTradeType = CheckTradeTypeFunc( kAlchemicFunc.m_iTradeTypeNum );

	// 기간값
	int iPeriodValue = GetItemPeriodTime( iValue1 );
	if( iPeriodValue == -1 )
		return ART_PERIOD_ERROR;

	// 선물 발송
	g_PresentHelper.SendPresentByAlchemicItem( pUser, iItemCode, iPeriodValue, iReinforce );

	// 결과 전송
	SP2Packet kReturn( STPK_ALCHEMIC_RESULT );
	PACKET_GUARD_INT( kReturn.Write(ALCHEMIC_NEW_ITEM_SUCC) );
	PACKET_GUARD_INT( kReturn.Write(iPiece1) );
	PACKET_GUARD_INT( kReturn.Write(iValue1) );
	PACKET_GUARD_INT( kReturn.Write(iAdditive) );
	PACKET_GUARD_INT( kReturn.Write(iAddValue) );
	PACKET_GUARD_INT( kReturn.Write(iItemCode) );
	PACKET_GUARD_INT( kReturn.Write(iPeriodValue) );

	pUser->SendMessage( kReturn );

	// 성공로그
	g_LogDBClient.OnInsertAlchemicFunc( pUser, LogDBClient::AFT_EXTRAITEM, kAlchemicFunc.m_iCode, LogDBClient::AFRT_RECIPE_SUCC,
										iUseCnt1, iUseCnt2, iItemCode, 0 );

	int iPeriodType = ioUserExtraItem::EPT_TIME;
	if( iPeriodValue == 0 ) // 무제한
	{
		iPeriodType = ioUserExtraItem::EPT_MORTMAIN;
	}

	g_LogDBClient.OnInsertExtraItem( pUser, iItemCode, iReinforce, 0, iPeriodValue, 0, iPeriodType, 0, 0, "NONE", LogDBClient::ERT_ALCHEMIC_ITEM );

	return ART_SUCCESS;
}

int ioAlchemicMgr::GetSoldierPeriodTime( int iPieceCnt )
{
	int iPeriod = -1;
	int iCurCnt = 0;

	int iListCnt = m_SoldierPeriodInfoList.size();
	for( int i=0; i < iListCnt; ++i )
	{
		if( COMPARE( iPieceCnt, iCurCnt, int(m_SoldierPeriodInfoList[i].m_dwLimitCnt+1) ) )
		{
			int iTable = m_SoldierPeriodInfoList[i].m_dwTableNum;

			NewAlchemicPeriodTableMap::iterator iter = m_SoldierPeriodTabelMap.find( iTable );
			if( iter != m_SoldierPeriodTabelMap.end() )
			{
				NewAlchemicPeriodTable kResultInfo = iter->second;

				int iRand = m_PeriodRandom.Random( kResultInfo.m_dwTotalRate );
				int iCurPosition = 0;
				int iItemCnt = kResultInfo.m_vList.size();

				for( int j=0; j < iItemCnt; ++j )
				{
					int iCurRate = kResultInfo.m_vList[j].m_iRandomRate;
					if( COMPARE( iRand, iCurPosition, iCurPosition+iCurRate ) )
					{
						iPeriod = kResultInfo.m_vList[j].m_iValue;
						break;
					}

					iCurPosition += iCurRate;
				}
			}

			break;
		}
		else
		{
			iCurCnt = m_SoldierPeriodInfoList[i].m_dwLimitCnt;
		}
	}

	return iPeriod;
}

int ioAlchemicMgr::GetItemPeriodTime( int iPieceCnt )
{
	int iPeriod = -1;
	int iCurCnt = 0;

	int iListCnt = m_ItemPeriodInfoList.size();
	for( int i=0; i < iListCnt; ++i )
	{
		if( COMPARE( iPieceCnt, iCurCnt, m_ItemPeriodInfoList[i].m_dwLimitCnt+1 ) )
		{
			int iTable = m_ItemPeriodInfoList[i].m_dwTableNum;
			NewAlchemicPeriodTableMap::iterator iter = m_ItemPeriodTabelMap.find( iTable );
			if( iter != m_ItemPeriodTabelMap.end() )
			{
				NewAlchemicPeriodTable kResultInfo = iter->second;

				int iRand = m_PeriodRandom.Random( kResultInfo.m_dwTotalRate );
				int iCurPosition = 0;
				int iItemCnt = kResultInfo.m_vList.size();

				for( int j=0; j < iItemCnt; ++j )
				{
					int iCurRate = kResultInfo.m_vList[j].m_iRandomRate;
					if( COMPARE( iRand, iCurPosition, iCurPosition+iCurRate ) )
					{
						iPeriod = kResultInfo.m_vList[j].m_iValue;
						break;
					}

					iCurPosition += iCurRate;
				}
			}

			break;
		}
		else
		{
			iCurCnt = m_ItemPeriodInfoList[i].m_dwLimitCnt;
		}
	}

	return iPeriod;
}

int ioAlchemicMgr::GetSouleStoneGainCnt()
{
	int iRandom = m_DisassembleRandom.Random(m_iTotalSoulStoneRandom);
	int iCurPosition = 0;

	for( int i = 0; i < m_vSoultoneRandomInfo.size(); i++ )
	{
		int iCurRate = m_vSoultoneRandomInfo[i];
		if( COMPARE(iRandom, iCurPosition, iCurPosition+iCurRate) )
		{
			return i+1;
		}

		iCurPosition += iCurRate;
	}

	return -1;
}