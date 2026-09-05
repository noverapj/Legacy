#include "stdafx.h"

#include "ioUserExtraItem.h"

#include "ioRoomOptionManager.h"

#include "GUI/PartyVersusWnd.h"

template<> ioRoomOptionManager* Singleton< ioRoomOptionManager >::ms_Singleton = 0;

ioRoomOptionManager::ioRoomOptionManager()
{
	ClearAllInfo();
}

ioRoomOptionManager::~ioRoomOptionManager()
{
	ClearAllInfo();
}

void ioRoomOptionManager::LoadRoomOptionInfo()
{
	ClearAllInfo();

	ioINILoader_e kLoader( "config/sp2_room_option_info.ini" );

	int i = 0;
	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	// EnableLevel
	kLoader.SetTitle_e( "common" );
	m_iEnableLevel = kLoader.LoadInt_e( "enable_level", 0 );

	// EnableMode	
	kLoader.SetTitle_e( "EnableMode" );

	int iModeCnt = kLoader.LoadInt_e( "enable_mode_cnt", 0 );
	for( i=0; i < iModeCnt; ++i )
	{
		wsprintf_e( szKey, "enable_mode_index%d", i+1 );

		int iModeIndex = kLoader.LoadInt( szKey, 0 );
		m_EnableOptionModeList.push_back( iModeIndex );
	}

	// no challenger
	kLoader.SetTitle_e( "NoChallengerMode" );

	int iNoChallengerModeCnt = kLoader.LoadInt_e( "no_challenger_mode_cnt", 0 );
	for( i=0; i < iNoChallengerModeCnt; ++i )
	{
		wsprintf_e( szKey, "no_challenger_mode_index%d", i+1 );

		int iModeIndex = kLoader.LoadInt( szKey, 0 );
		m_NoChallengerModeList.push_back( iModeIndex );
	}

	m_iNoChallengerGrade = kLoader.LoadInt_e( "no_challenger_grade", 0 );

	// Enable RoundOptionMode	
	int iOptionModeCnt = kLoader.LoadInt_e( "enable_round_option_cnt", 0 );
	for( i=0; i < iOptionModeCnt; ++i )
	{
		wsprintf_e( szKey, "enable_round_option_index%d", i+1 );

		int iModeIndex = kLoader.LoadInt( szKey, 0 );
		m_EnableRoundOptionModeList.push_back( iModeIndex );
	}


	// ChangeChar
	kLoader.SetTitle_e( "ChangeChar" );

	int iTypeCnt = kLoader.LoadInt_e( "change_char_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "change_char_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "change_char_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "change_char_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomChangeCharList.push_back( pInfo );
	}


	// CoolTimeOption
	kLoader.SetTitle_e( "CoolTimeOption" );

	iTypeCnt = kLoader.LoadInt_e( "cooltime_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "cooltime_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "cooltime_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "cooltime_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomCoolTimeList.push_back( pInfo );
	}

	// RedHPOption
	kLoader.SetTitle_e( "RedHPOption" );

	iTypeCnt = kLoader.LoadInt_e( "hp_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "hp_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "hp_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "hp_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRedHPList.push_back( pInfo );
	}

	// BlueHPOption
	kLoader.SetTitle_e( "BlueHPOption" );

	iTypeCnt = kLoader.LoadInt_e( "hp_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "hp_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "hp_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "hp_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomBlueHPList.push_back( pInfo );
	}


	// DropDamage
	kLoader.SetTitle_e( "DropDamage" );

	iTypeCnt = kLoader.LoadInt_e( "dropdamge_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "dropdamage_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "dropdamage_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "dropdamage_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomDropDamageList.push_back( pInfo );
	}


	// Gravity
	kLoader.SetTitle_e( "Gravity" );

	iTypeCnt = kLoader.LoadInt_e( "gravity_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "gravity_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "gravity_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "gravity_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomGravityList.push_back( pInfo );
	}


	// TeamAttack
	kLoader.SetTitle_e( "TeamAttack" );

	iTypeCnt = kLoader.LoadInt_e( "team_attack_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "team_attack_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "team_attack_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "team_attack_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomTeamAttackList.push_back( pInfo );
	}


	// Getup
	kLoader.SetTitle_e( "GetUp" );

	iTypeCnt = kLoader.LoadInt_e( "get_up_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "get_up_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "get_up_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "get_up_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomGetUpList.push_back( pInfo );
	}


	// MoveSpeed
	kLoader.SetTitle_e( "RedMoveSpeed" );

	iTypeCnt = kLoader.LoadInt_e( "move_speed_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "move_speed_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "move_speed_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "move_speed_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRedMoveSpeedList.push_back( pInfo );
	}

	kLoader.SetTitle_e( "BlueMoveSpeed" );

	iTypeCnt = kLoader.LoadInt_e( "move_speed_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "move_speed_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "move_speed_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "move_speed_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomBlueMoveSpeedList.push_back( pInfo );
	}


	// KO Type
	kLoader.SetTitle_e( "KOType" );

	iTypeCnt = kLoader.LoadInt_e( "ko_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "ko_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "ko_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "ko_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomKOList.push_back( pInfo );
	}


	// KO Effect Type
	kLoader.SetTitle_e( "KOEffectType" );

	iTypeCnt = kLoader.LoadInt_e( "ko_effect_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "ko_effect_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "ko_effect_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "ko_effect_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomKOEffectList.push_back( pInfo );
	}


	// Red Blow
	kLoader.SetTitle_e( "RedBlowType" );

	iTypeCnt = kLoader.LoadInt_e( "blow_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "blow_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "blow_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "blow_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRedBlowList.push_back( pInfo );
	}

	// Blue Blow
	kLoader.SetTitle_e( "BlueBlowType" );

	iTypeCnt = kLoader.LoadInt_e( "blow_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "blow_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "blow_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "blow_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomBlueBlowList.push_back( pInfo );
	}


	// EquipType
	kLoader.SetTitle_e( "RedEquipType" );

	iTypeCnt = kLoader.LoadInt_e( "equip_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "equip_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "equip_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "equip_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRedEquipList.push_back( pInfo );
	}

	kLoader.SetTitle_e( "BlueEquipType" );

	iTypeCnt = kLoader.LoadInt_e( "equip_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "equip_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "equip_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "equip_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomBlueEquipList.push_back( pInfo );
	}

	// Round Type
	kLoader.SetTitle_e( "CatchRoundType" );

	iTypeCnt = kLoader.LoadInt_e( "round_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "round_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "round_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "round_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRoundTypeList.push_back( pInfo );
	}

	// Round Time Type
	kLoader.SetTitle_e( "CatchRoundTimeType" );

	iTypeCnt = kLoader.LoadInt_e( "round_time_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "round_time_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "round_time_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "round_time_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomRoundTimeTypeList.push_back( pInfo );
	}

	// GrowthUse
	kLoader.SetTitle_e( "GrowthUse" );

	iTypeCnt = kLoader.LoadInt_e( "growth_use_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "growth_use_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "growth_use_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "growth_use_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomGrowthUseList.push_back( pInfo );
	}

	// ExtraItemUse
	kLoader.SetTitle_e( "ExtraItemUse" );

	iTypeCnt = kLoader.LoadInt_e( "extraitem_use_type_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		RoomOptionInfo *pInfo = new RoomOptionInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "extraitem_use_rate%d", i+1 );
		pInfo->m_fValue = kLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "extraitem_use_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "extraitem_use_ui_desc%d", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_UIDesc = szBuf;

		m_RoomExtraItemUseList.push_back( pInfo );
	}


	// Default
	kLoader.SetTitle_e( "DefaultSet" );

	m_DefaultInfo.m_iChangeCharType = kLoader.LoadInt_e( "default_charnge_char", 0 );
	m_DefaultInfo.m_iTeamAttackType = kLoader.LoadInt_e( "default_team_attack", 0 );
	m_DefaultInfo.m_iCoolTimeType = kLoader.LoadInt_e( "default_cooltime", 0 );
	m_DefaultInfo.m_iRedHPType = kLoader.LoadInt_e( "default_red_hp", 0 );
	m_DefaultInfo.m_iBlueHPType = kLoader.LoadInt_e( "default_blue_hp", 0 );
	m_DefaultInfo.m_iDropDamageType = kLoader.LoadInt_e( "default_dropdamage", 0 );
	m_DefaultInfo.m_iGravityType = kLoader.LoadInt_e( "default_gravity", 0 );
	m_DefaultInfo.m_iGetUpType = kLoader.LoadInt_e( "default_getup", 0 );
	m_DefaultInfo.m_iKOType = kLoader.LoadInt_e( "default_ko_type", 0 );
	m_DefaultInfo.m_iKOEffectType = kLoader.LoadInt_e( "default_ko_effect_type", 0 );
	m_DefaultInfo.m_iRedBlowType = kLoader.LoadInt_e( "default_red_blow_type", 0 );
	m_DefaultInfo.m_iBlueBlowType = kLoader.LoadInt_e( "default_blue_blow_type", 0 );
	m_DefaultInfo.m_iRedMoveSpeedType = kLoader.LoadInt_e( "default_red_move_speed", 0 );
	m_DefaultInfo.m_iBlueMoveSpeedType = kLoader.LoadInt_e( "default_blue_move_speed", 0 );
	m_DefaultInfo.m_iRedEquipType = kLoader.LoadInt_e( "default_red_equip", 0 );
	m_DefaultInfo.m_iBlueEquipType = kLoader.LoadInt_e( "default_blue_equip", 0 );

	m_DefaultInfo.m_iCatchModeRoundType = kLoader.LoadInt_e( "default_round_type", 0 );
	m_DefaultInfo.m_iCatchModeRoundTimeType = kLoader.LoadInt_e( "default_round_time_type", 0 );

	m_DefaultInfo.m_iGrowthUseType = kLoader.LoadInt_e( "default_growth_use", 0 );
	m_DefaultInfo.m_iExtraItemUseType = kLoader.LoadInt_e( "default_extraitem_use", 0 );
	//


	// PreSet
	kLoader.SetTitle_e( "PreSet" );
	iTypeCnt = kLoader.LoadInt_e( "preset_cnt", 0 );
	for( i=0; i < iTypeCnt; ++i )
	{
		PreSetInfo *pInfo = new PreSetInfo;
		if( !pInfo )
			continue;

		wsprintf_e( szKey, "preset%d_desc", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		pInfo->m_Desc = szBuf;

		wsprintf_e( szKey, "preset%d_charnge_char", i+1 );
		pInfo->m_iChangeCharType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_team_attack", i+1 );
		pInfo->m_iTeamAttackType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_cooltime", i+1 );
		pInfo->m_iCoolTimeType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_red_hp", i+1 );
		pInfo->m_iRedHPType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_blue_hp", i+1 );
		pInfo->m_iBlueHPType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_dropdamage", i+1 );
		pInfo->m_iDropDamageType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_gravity", i+1 );
		pInfo->m_iGravityType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_getup", i+1 );
		pInfo->m_iGetUpType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_red_move_speed", i+1 );
		pInfo->m_iRedMoveSpeedType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_blue_move_speed", i+1 );
		pInfo->m_iBlueMoveSpeedType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_ko_type", i+1 );
		pInfo->m_iKOType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_ko_effect_type", i+1 );
		pInfo->m_iKOEffectType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_red_blow_type", i+1 );
		pInfo->m_iRedBlowType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_blue_blow_type", i+1 );
		pInfo->m_iBlueBlowType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_red_equip", i+1 );
		pInfo->m_iRedEquipType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_blue_equip", i+1 );
		pInfo->m_iBlueEquipType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_round_type", i+1 );
		pInfo->m_iCatchModeRoundType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_round_time_type", i+1 );
		pInfo->m_iCatchModeRoundTimeType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_growth_use", i+1 );
		pInfo->m_iGrowthUseType = kLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "preset%d_extraitem_use", i+1 );
		pInfo->m_iExtraItemUseType = kLoader.LoadInt( szKey, 0 );

		m_PreSetList.push_back( pInfo );
	}

	// PreSetList에 수동 추가.
	PreSetInfo *pInfo = new PreSetInfo;
	if( !pInfo )
		return;
	pInfo->m_Desc = STR(1);

	m_PreSetList.push_back( pInfo );

}

void ioRoomOptionManager::ClearAllInfo()
{
	m_EnableOptionModeList.clear();
	m_EnableRoundOptionModeList.clear();
	m_NoChallengerModeList.clear();

	for(RoomOptionInfoList::iterator iter = m_RoomChangeCharList.begin(); iter != m_RoomChangeCharList.end(); ++iter)
	    SAFEDELETE( *iter );
	m_RoomChangeCharList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRedHPList.begin(); iter != m_RoomRedHPList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRedHPList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomBlueHPList.begin(); iter != m_RoomBlueHPList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomBlueHPList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomCoolTimeList.begin(); iter != m_RoomCoolTimeList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomCoolTimeList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomDropDamageList.begin(); iter != m_RoomDropDamageList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomDropDamageList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomGravityList.begin(); iter != m_RoomGravityList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomGravityList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomTeamAttackList.begin(); iter != m_RoomTeamAttackList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomTeamAttackList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomGetUpList.begin(); iter != m_RoomGetUpList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomGetUpList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomKOList.begin(); iter != m_RoomKOList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomKOList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomKOEffectList.begin(); iter != m_RoomKOEffectList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomKOEffectList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRedBlowList.begin(); iter != m_RoomRedBlowList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRedBlowList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomBlueBlowList.begin(); iter != m_RoomBlueBlowList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomBlueBlowList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRedMoveSpeedList.begin(); iter != m_RoomRedMoveSpeedList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRedMoveSpeedList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomBlueMoveSpeedList.begin(); iter != m_RoomBlueMoveSpeedList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomBlueMoveSpeedList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRedEquipList.begin(); iter != m_RoomRedEquipList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRedEquipList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomBlueEquipList.begin(); iter != m_RoomBlueEquipList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomBlueEquipList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRoundTypeList.begin(); iter != m_RoomRoundTypeList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRoundTypeList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomRoundTimeTypeList.begin(); iter != m_RoomRoundTimeTypeList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomRoundTimeTypeList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomGrowthUseList.begin(); iter != m_RoomGrowthUseList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomGrowthUseList.clear();
	for(RoomOptionInfoList::iterator iter = m_RoomExtraItemUseList.begin(); iter != m_RoomExtraItemUseList.end(); ++iter)
		SAFEDELETE( *iter );
	m_RoomExtraItemUseList.clear();

	for(PreSetInfoList::iterator iter = m_PreSetList.begin(); iter != m_PreSetList.end(); ++iter)
		SAFEDELETE( *iter );
	m_PreSetList.clear();
}

bool ioRoomOptionManager::IsEnableOptionMode( int iModeIndex )
{
	int iSize = m_EnableOptionModeList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_EnableOptionModeList[i] == iModeIndex )
			return true;
	}

	return false;
}

bool ioRoomOptionManager::IsNoChallengerMode( int iModeIndex )
{
	int iSize = m_NoChallengerModeList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_NoChallengerModeList[i] == iModeIndex )
			return true;
	}

	return false;
}

bool ioRoomOptionManager::IsEnableRoundOptionMode( int iModeIndex )
{
	int iSize = m_EnableRoundOptionModeList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_EnableRoundOptionModeList[i] == iModeIndex )
			return true;
	}

	return false;
}

bool ioRoomOptionManager::CheckChangeCharType( int iIndex )
{
	int iSize = m_RoomChangeCharList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		float fValue = m_RoomChangeCharList[iIndex]->m_fValue;
		if( fValue >= 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioRoomOptionManager::CheckEnableTeamAttack( int iIndex )
{
	int iSize = m_RoomTeamAttackList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		float fValue = m_RoomTeamAttackList[iIndex]->m_fValue;
		if( fValue >= 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioRoomOptionManager::CheckEnableTeamDamage( int iIndex )
{
	int iSize = m_RoomTeamAttackList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		float fValue = m_RoomTeamAttackList[iIndex]->m_fValue;
		if( fValue > 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioRoomOptionManager::CheckEnableGrowthUse( int iIndex )
{
	int iSize = m_RoomGrowthUseList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		float fValue = m_RoomGrowthUseList[iIndex]->m_fValue;
		if( fValue > 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

bool ioRoomOptionManager::CheckEnableExtraItemUse( int iIndex )
{
	int iSize = m_RoomExtraItemUseList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		float fValue = m_RoomExtraItemUseList[iIndex]->m_fValue;
		if( fValue > 0.0f )
			return true;
		else
			return false;
	}

	return true;
}

float ioRoomOptionManager::GetRedHPRate( int iIndex )
{
	int iSize = m_RoomRedHPList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomRedHPList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetBlueHPRate( int iIndex )
{
	int iSize = m_RoomBlueHPList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomBlueHPList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetCoolTimeRate( int iIndex )
{
	int iSize = m_RoomCoolTimeList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomCoolTimeList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetDropDamageRate( int iIndex )
{
	int iSize = m_RoomDropDamageList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomDropDamageList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetGravityRate( int iIndex )
{
	int iSize = m_RoomGravityList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomGravityList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetGetUpRate( int iIndex )
{
	int iSize = m_RoomGetUpList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomGetUpList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetRedMoveSpeedRate( int iIndex )
{
	int iSize = m_RoomRedMoveSpeedList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomRedMoveSpeedList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetBlueMoveSpeedRate( int iIndex )
{
	int iSize = m_RoomBlueMoveSpeedList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomBlueMoveSpeedList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetKORate( int iIndex )
{
	int iSize = m_RoomKOList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomKOList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetKOEffectRate( int iIndex )
{
	int iSize = m_RoomKOEffectList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomKOEffectList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetRedBlowRate( int iIndex )
{
	int iSize = m_RoomRedBlowList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomRedBlowList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetBlueBlowRate( int iIndex )
{
	int iSize = m_RoomBlueBlowList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomBlueBlowList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetRedEquipRate( int iIndex )
{
	int iSize = m_RoomRedEquipList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomRedEquipList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

float ioRoomOptionManager::GetBlueEquipRate( int iIndex )
{
	int iSize = m_RoomBlueEquipList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_RoomBlueEquipList[iIndex]->m_fValue;
	}

	return FLOAT1;
}

ioHashString ioRoomOptionManager::GetPreSetDesc( int iIndex )
{
	int iSize = m_PreSetList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		return m_PreSetList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetChangeCharDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomChangeCharList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomChangeCharList[iIndex]->m_UIDesc;
		else
			return m_RoomChangeCharList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRedHPDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRedHPList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRedHPList[iIndex]->m_UIDesc;
		else
			return m_RoomRedHPList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetBlueHPDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomBlueHPList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomBlueHPList[iIndex]->m_UIDesc;
		else
			return m_RoomBlueHPList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetCoolTimeDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomCoolTimeList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomCoolTimeList[iIndex]->m_UIDesc;
		else
			return m_RoomCoolTimeList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetDropDamageDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomDropDamageList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomDropDamageList[iIndex]->m_UIDesc;
		else
			return m_RoomDropDamageList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetGravityDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomGravityList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomGravityList[iIndex]->m_UIDesc;
		else
			return m_RoomGravityList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetTeamAttackDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomTeamAttackList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomTeamAttackList[iIndex]->m_UIDesc;
		else
			return m_RoomTeamAttackList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetGetUpDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomGetUpList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomGetUpList[iIndex]->m_UIDesc;
		else
			return m_RoomGetUpList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRedMoveSpeedDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRedMoveSpeedList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRedMoveSpeedList[iIndex]->m_UIDesc;
		else
			return m_RoomRedMoveSpeedList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetBlueMoveSpeedDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomBlueMoveSpeedList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomBlueMoveSpeedList[iIndex]->m_UIDesc;
		else
			return m_RoomBlueMoveSpeedList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetKODesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomKOList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomKOList[iIndex]->m_UIDesc;
		else
			return m_RoomKOList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetKOEffectDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomKOEffectList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomKOEffectList[iIndex]->m_UIDesc;
		else
			return m_RoomKOEffectList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRedBlowDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRedBlowList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRedBlowList[iIndex]->m_UIDesc;
		else
			return m_RoomRedBlowList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetBlueBlowDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomBlueBlowList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomBlueBlowList[iIndex]->m_UIDesc;
		else
			return m_RoomBlueBlowList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRedEquipDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRedEquipList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRedEquipList[iIndex]->m_UIDesc;
		else
			return m_RoomRedEquipList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetBlueEquipDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomBlueEquipList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomBlueEquipList[iIndex]->m_UIDesc;
		else
			return m_RoomBlueEquipList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRoundTypeDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRoundTypeList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRoundTypeList[iIndex]->m_UIDesc;
		else
			return m_RoomRoundTypeList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetRoundTimeTypeDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomRoundTimeTypeList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomRoundTimeTypeList[iIndex]->m_UIDesc;
		else
			return m_RoomRoundTimeTypeList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetGrowthUseDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomGrowthUseList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomGrowthUseList[iIndex]->m_UIDesc;
		else
			return m_RoomGrowthUseList[iIndex]->m_Desc;
	}

	return ioHashString();
}

ioHashString ioRoomOptionManager::GetExtraItemUseDesc( int iIndex, bool bUiDesc /* = false  */ )
{
	int iSize = m_RoomExtraItemUseList.size();

	if( COMPARE( iIndex, 0, iSize ) )
	{
		if( bUiDesc )
			return m_RoomExtraItemUseList[iIndex]->m_UIDesc;
		else
			return m_RoomExtraItemUseList[iIndex]->m_Desc;
	}

	return ioHashString();
}

int ioRoomOptionManager::GetOptionTypeCnt( int iOptionType )
{
	switch( iOptionType )
	{
	case PartyOptionWnd::ID_PRESET_BTN:
		return m_PreSetList.size();
	case PartyOptionWnd::ID_TEAM_ATTACK_TYPE:
		return m_RoomTeamAttackList.size();
	case PartyOptionWnd::ID_CHANGE_CHAR_TYPE:
		return m_RoomChangeCharList.size();
	case PartyOptionWnd::ID_COOLTIME_TYPE:
		return m_RoomCoolTimeList.size();
	case PartyOptionWnd::ID_RED_HP_TYPE:
		return m_RoomRedHPList.size();
	case PartyOptionWnd::ID_BLUE_HP_TYPE:
		return m_RoomBlueHPList.size();
	case PartyOptionWnd::ID_DROP_DAMAGE_TYPE:
		return m_RoomDropDamageList.size();
	case PartyOptionWnd::ID_GRAVITY_TYPE:
		return m_RoomGravityList.size();
	case PartyOptionWnd::ID_GETUP_TYPE:
		return m_RoomGetUpList.size();
	case PartyOptionWnd::ID_KO_TYPE:
		return m_RoomKOList.size();
	case PartyOptionWnd::ID_KO_EFFECT_TYPE:
		return m_RoomKOEffectList.size();
	case PartyOptionWnd::ID_RED_BLOW_TYPE:
		return m_RoomRedBlowList.size();
	case PartyOptionWnd::ID_BLUE_BLOW_TYPE:
		return m_RoomBlueBlowList.size();
	case PartyOptionWnd::ID_RED_MOVE_SPEED_TYPE:
		return m_RoomRedMoveSpeedList.size();
	case PartyOptionWnd::ID_BLUE_MOVE_SPEED_TYPE:
		return m_RoomBlueMoveSpeedList.size();
	case PartyOptionWnd::ID_RED_EQUIP_TYPE:
		return m_RoomRedEquipList.size();
	case PartyOptionWnd::ID_BLUE_EQUIP_TYPE:
		return m_RoomBlueEquipList.size();
	case PartyOptionWnd::ID_ROUND_TYPE:
		return m_RoomRoundTypeList.size();
	case PartyOptionWnd::ID_ROUND_TIME_TYPE:
		return m_RoomRoundTimeTypeList.size();
	case PartyOptionWnd::ID_GROWTH_USE_TYPE:
		return m_RoomGrowthUseList.size();
	case PartyOptionWnd::ID_EXTRAITEM_USE_TYPE:
		return m_RoomExtraItemUseList.size();
	}

	return 0;
}

bool ioRoomOptionManager::GetPreSetInfo( IN int iPreSetNum, OUT PreSetInfo &rkPreSetInfo )
{
	int iSize = m_PreSetList.size();

	// 수동인지 체크
	if( iPreSetNum == iSize-1 )
		return false;

	rkPreSetInfo.Init();

	if( COMPARE( iPreSetNum, 0, iSize ) )
	{
		// -1 을 해줘야 값이 같아진다.
		rkPreSetInfo.m_iTeamAttackType = max( 0, m_PreSetList[iPreSetNum]->m_iTeamAttackType-1 );
		rkPreSetInfo.m_iChangeCharType = max( 0, m_PreSetList[iPreSetNum]->m_iChangeCharType-1 );
		rkPreSetInfo.m_iCoolTimeType = max( 0, m_PreSetList[iPreSetNum]->m_iCoolTimeType-1 );
		rkPreSetInfo.m_iRedHPType = max( 0, m_PreSetList[iPreSetNum]->m_iRedHPType-1 );
		rkPreSetInfo.m_iBlueHPType = max( 0, m_PreSetList[iPreSetNum]->m_iBlueHPType-1 );
		rkPreSetInfo.m_iDropDamageType = max( 0, m_PreSetList[iPreSetNum]->m_iDropDamageType-1 );
		rkPreSetInfo.m_iGravityType = max( 0, m_PreSetList[iPreSetNum]->m_iGravityType-1 );
		rkPreSetInfo.m_iGetUpType = max( 0, m_PreSetList[iPreSetNum]->m_iGetUpType-1 );
		rkPreSetInfo.m_iKOType = max( 0, m_PreSetList[iPreSetNum]->m_iKOType-1 );
		rkPreSetInfo.m_iKOEffectType = max( 0, m_PreSetList[iPreSetNum]->m_iKOEffectType-1 );
		rkPreSetInfo.m_iRedBlowType = max( 0, m_PreSetList[iPreSetNum]->m_iRedBlowType-1 );
		rkPreSetInfo.m_iBlueBlowType = max( 0, m_PreSetList[iPreSetNum]->m_iBlueBlowType-1 );
		rkPreSetInfo.m_iRedMoveSpeedType = max( 0, m_PreSetList[iPreSetNum]->m_iRedMoveSpeedType-1 );
		rkPreSetInfo.m_iBlueMoveSpeedType = max( 0, m_PreSetList[iPreSetNum]->m_iBlueMoveSpeedType-1 );
		rkPreSetInfo.m_iRedEquipType = max( 0, m_PreSetList[iPreSetNum]->m_iRedEquipType-1 );
		rkPreSetInfo.m_iBlueEquipType = max( 0, m_PreSetList[iPreSetNum]->m_iBlueEquipType-1 );

		rkPreSetInfo.m_iCatchModeRoundType = max( 0, m_PreSetList[iPreSetNum]->m_iCatchModeRoundType-1 );
		rkPreSetInfo.m_iCatchModeRoundTimeType = max( 0, m_PreSetList[iPreSetNum]->m_iCatchModeRoundTimeType-1 );

		rkPreSetInfo.m_iGrowthUseType = max( 0, m_PreSetList[iPreSetNum]->m_iGrowthUseType-1 );
		rkPreSetInfo.m_iExtraItemUseType = max( 0, m_PreSetList[iPreSetNum]->m_iExtraItemUseType-1 );

		return true;
	}

	return false;
}

bool ioRoomOptionManager::GetDefaultInfo( OUT PreSetInfo &rkPreSetInfo )
{
	rkPreSetInfo.Init();

	// -1 을 해줘야 값이 같아진다.
	rkPreSetInfo.m_iTeamAttackType = max( 0, m_DefaultInfo.m_iTeamAttackType-1 );
	rkPreSetInfo.m_iChangeCharType = max( 0, m_DefaultInfo.m_iChangeCharType-1 );
	rkPreSetInfo.m_iCoolTimeType = max( 0, m_DefaultInfo.m_iCoolTimeType-1 );
	rkPreSetInfo.m_iRedHPType = max( 0, m_DefaultInfo.m_iRedHPType-1 );
	rkPreSetInfo.m_iBlueHPType = max( 0, m_DefaultInfo.m_iBlueHPType-1 );
	rkPreSetInfo.m_iDropDamageType = max( 0, m_DefaultInfo.m_iDropDamageType-1 );
	rkPreSetInfo.m_iGravityType = max( 0, m_DefaultInfo.m_iGravityType-1 );
	rkPreSetInfo.m_iGetUpType = max( 0, m_DefaultInfo.m_iGetUpType-1 );
	rkPreSetInfo.m_iKOType = max( 0, m_DefaultInfo.m_iKOType-1 );
	rkPreSetInfo.m_iKOEffectType = max( 0, m_DefaultInfo.m_iKOEffectType-1 );
	rkPreSetInfo.m_iRedBlowType = max( 0, m_DefaultInfo.m_iRedBlowType-1 );
	rkPreSetInfo.m_iBlueBlowType = max( 0, m_DefaultInfo.m_iBlueBlowType-1 );
	rkPreSetInfo.m_iRedMoveSpeedType = max( 0, m_DefaultInfo.m_iRedMoveSpeedType-1 );
	rkPreSetInfo.m_iBlueMoveSpeedType = max( 0, m_DefaultInfo.m_iBlueMoveSpeedType-1 );
	rkPreSetInfo.m_iRedEquipType = max( 0, m_DefaultInfo.m_iRedEquipType-1 );
	rkPreSetInfo.m_iBlueEquipType = max( 0, m_DefaultInfo.m_iBlueEquipType-1 );

	rkPreSetInfo.m_iCatchModeRoundType = max( 0, m_DefaultInfo.m_iCatchModeRoundType-1 );
	rkPreSetInfo.m_iCatchModeRoundTimeType = max( 0, m_DefaultInfo.m_iCatchModeRoundTimeType-1 );

	rkPreSetInfo.m_iGrowthUseType = max( 0, m_DefaultInfo.m_iGrowthUseType-1 );
	rkPreSetInfo.m_iExtraItemUseType = max( 0, m_DefaultInfo.m_iExtraItemUseType-1 );

	return true;
}

ioRoomOptionManager& ioRoomOptionManager::GetSingleton()
{
	return Singleton< ioRoomOptionManager >::GetSingleton();
}

