
#include "stdafx.h"

#include "ItemDefine.h"
#include "WeaponDefine.h"
#include "BuffDefine.h"

ioAreaWeapon::ioAreaWeapon()
{
	m_TeamType = TEAM_NONE;
	
	m_dwAreaWeaponEffect = -1;
	m_dwCheckProcessTime = 0;
	m_dwGaugeCheckTime = 0;
	m_bLive = true;

	m_bUsedAreaWeapon = false;

	m_vCurTargetList.clear();
	m_vNewTargetList.clear();
	m_vCurBlockList.clear();

	m_dwAreaWeaponIndex = 0;

	m_fExtraBuffDuration = 0.0f;

	m_AreaWeaponState = AWS_READY;

	m_dwWeaponBaseIndex = 0;
	m_dwChangeCheckTime = 0;

	// Move
	m_AreaMoveType = AMT_NORMAL;

	// Move : Trace
	m_bUsedTrace = false;
	m_TraceTarget.Clear();

	// Move : LiveArea
	m_dwLiveAreaEndTime = 0;

	m_dwPreFrameTime = 0;
	m_dwLostTraceTime = 0;

	m_dwCreateWeaponIndex = 0;
	m_AreaweaponSoundID = 0;

	// random attack
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwGenerateSeed = 0;

	m_SubAreaType = ST_NONE;

	// target att
	m_dwCheckTargetAttTime = 0;
	m_dwTargetAttCurCnt = 0;
	m_dwCurTargetAttEff = -1;

	m_dwEndWeaponDuration = 0;
	m_dwCheckCreateAreaAttackTime = 0;

	m_bCallNextWeaponAttribute = false;
}

ioAreaWeapon::ioAreaWeapon( const ioAreaWeapon &rhs )
: m_AreaWeaponName( rhs.m_AreaWeaponName ),
m_AreaType( rhs.m_AreaType ),
m_AreaWeaponEffectBlue( rhs.m_AreaWeaponEffectBlue ),
m_AreaWeaponEffectRed( rhs.m_AreaWeaponEffectRed ),
m_AreaWeaponReadyEffectBlue( rhs.m_AreaWeaponReadyEffectBlue ),
m_AreaWeaponReadyEffectRed( rhs.m_AreaWeaponReadyEffectRed ),
m_AreaWeaponEndEffectBlue( rhs.m_AreaWeaponEndEffectBlue ),
m_AreaWeaponEndEffectRed( rhs.m_AreaWeaponEndEffectRed ),
m_bForceEnd( rhs.m_bForceEnd ),
m_TargetColType( rhs.m_TargetColType ),
m_TargetWoundType( rhs.m_TargetWoundType ),
m_fTargetEnableHeight( rhs.m_fTargetEnableHeight ),
m_dwLiveTime( rhs.m_dwLiveTime ),
m_dwLiveTimeByNPC( rhs.m_dwLiveTimeByNPC ),
m_dwLiveTimeByFC( rhs.m_dwLiveTimeByFC ),
m_dwReadyTime( rhs.m_dwReadyTime ),
m_dwCheckTicTime( rhs.m_dwCheckTicTime ),
m_fCollisionRange( rhs.m_fCollisionRange ),
m_fCollisionInRange( rhs.m_fCollisionInRange ),
m_fAreaAngle( rhs.m_fAreaAngle ),
m_fUpHeight( rhs.m_fUpHeight ),
m_fUnderHeight( rhs.m_fUnderHeight ),
m_fWidth( rhs.m_fWidth ),
m_vOwnerBuffList( rhs.m_vOwnerBuffList ),
m_vTeamBuffList( rhs.m_vTeamBuffList ),
m_vEnemyBuffList( rhs.m_vEnemyBuffList ),
m_vTeamBuffOnCreateWeaponList( rhs.m_vTeamBuffOnCreateWeaponList ),
m_WeaponEndBuffList( rhs.m_WeaponEndBuffList ),
m_bEnableAttacker( rhs.m_bEnableAttacker ),
m_bEnableAttackCnt( rhs.m_bEnableAttackCnt ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_dwGaugeTicTime( rhs.m_dwGaugeTicTime ),
m_fScaleRate( rhs.m_fScaleRate ),
m_dwScaleSlerpTime( rhs.m_dwScaleSlerpTime ),
m_fAreaForceAmt( rhs.m_fAreaForceAmt ),
m_fAreaForceFriction( rhs.m_fAreaForceFriction ),
m_fRotationTime( rhs.m_fRotationTime ),
m_fAreaMoveSpeed( rhs.m_fAreaMoveSpeed ),
m_bOwnerMoveDir( rhs.m_bOwnerMoveDir ),
m_AreaweaponSound( rhs.m_AreaweaponSound ),
m_WeaponInfo( rhs.m_WeaponInfo ),
m_dwWeaponDuration( rhs.m_dwWeaponDuration ),
m_ChangeMoveInfoList( rhs.m_ChangeMoveInfoList ),
m_TargetMapEffect( rhs.m_TargetMapEffect ),
m_AreaMoveType( rhs.m_AreaMoveType ),
m_fTargetSearchRange( rhs.m_fTargetSearchRange ),
m_fTargetTraceRange( rhs.m_fTargetTraceRange ),
m_bOneTimeUse( rhs.m_bOneTimeUse ),
m_bUseLiveArea( rhs.m_bUseLiveArea ),
m_vMinLivePos( rhs.m_vMinLivePos ),
m_vMaxLivePos( rhs.m_vMaxLivePos ),
m_dwLiveAreaTime( rhs.m_dwLiveAreaTime ),
m_dwSencingWaitTime( rhs.m_dwSencingWaitTime ),
m_dwNotFindWaitTime( rhs.m_dwNotFindWaitTime ),
m_dwRotateMaxTime( rhs.m_dwRotateMaxTime ),
m_bUseRandomAttack( rhs.m_bUseRandomAttack ),
m_bUseRandomAttackFixedStartPos( rhs.m_bUseRandomAttackFixedStartPos ),
m_dwMinGenerateTime( rhs.m_dwMinGenerateTime ),
m_dwMaxGenerateTime( rhs.m_dwMaxGenerateTime ),
m_fGenerateHeight( rhs.m_fGenerateHeight ),
m_VertDirType( rhs.m_VertDirType ),
m_RandomGenerateFieldType( rhs.m_RandomGenerateFieldType ),
m_fDoubleInRange( rhs.m_fDoubleInRange ),
m_vOwnerBuffList2( rhs.m_vOwnerBuffList2 ),
m_vTeamBuffList2( rhs.m_vTeamBuffList2 ),
m_vEnemyBuffList2( rhs.m_vEnemyBuffList2 ),
m_fEffectOffset( rhs.m_fEffectOffset ),
m_fEffectHeightOffset( rhs.m_fEffectHeightOffset ),
m_fDropHeight( rhs.m_fDropHeight ),
m_fDropEnableRange( rhs.m_fDropEnableRange ),
m_dwPointToPointDelayProcessTime( rhs.m_dwPointToPointDelayProcessTime ),
m_fEnablePositionRange( rhs.m_fEnablePositionRange ),
m_fEnableMaxPositionRange( rhs.m_fEnableMaxPositionRange ),
szTargetIdentifyBuffName( rhs.szTargetIdentifyBuffName ),
m_dwTargetAttGap( rhs.m_dwTargetAttGap ),
m_TargetAttEffList( rhs.m_TargetAttEffList ),
m_fInvisibleRange( rhs.m_fInvisibleRange ),
m_dwRefreshDuration( rhs.m_dwRefreshDuration ),
m_bUseLengthKill( rhs.m_bUseLengthKill ),
m_fKillLength( rhs.m_fKillLength ),
m_EndWeaponInfo( rhs.m_EndWeaponInfo ),
m_dwEndWeaponDuration( rhs.m_dwEndWeaponDuration ),
m_AreaweaponEndSound( rhs.m_AreaweaponEndSound ),
m_OverlapWeaponInfo( rhs. m_OverlapWeaponInfo ),
m_dwOverlapWeaponDuration( rhs.m_dwOverlapWeaponDuration ),
m_bDelayCreateAreaAttack( rhs.m_bDelayCreateAreaAttack),
m_dwDelayCreateAreaAttackTime( rhs.m_dwDelayCreateAreaAttackTime ),
m_bUseOffSet( rhs.m_bUseOffSet ),
m_fOffSet( rhs.m_fOffSet ),
m_bSetRepeatAttack( rhs.m_bSetRepeatAttack ),
m_bTraceTargetCheckTeam( rhs.m_bTraceTargetCheckTeam ),
m_bCreateOnlyOne( rhs.m_bCreateOnlyOne ),
m_vPassDummyList( rhs.m_vPassDummyList ),
m_vDestroyWeaponList( rhs.m_vDestroyWeaponList ),
m_vNextAttackAttributeByTeamBuff( rhs.m_vNextAttackAttributeByTeamBuff ),
m_CheckOnceCallAttAttrMap( rhs.m_CheckOnceCallAttAttrMap ),
m_vOverlapAreaWeapon( rhs.m_vOverlapAreaWeapon ),
m_OverlapTargetType( rhs.m_OverlapTargetType ),
m_bHideEffect( rhs.m_bHideEffect )
{
	m_TeamType = TEAM_NONE;
	
	m_dwAreaWeaponEffect = -1;
	m_dwCheckProcessTime = 0;
	m_dwGaugeCheckTime = 0;

	m_bLive = true;
	m_bUsedAreaWeapon = false;

	m_vCurTargetList.clear();
	m_vNewTargetList.clear();
	m_vCurBlockList.clear();

	m_fExtraBuffDuration = 0.0f;

	m_AreaWeaponState = AWS_READY;

	m_dwWeaponBaseIndex = 0;
	m_dwChangeCheckTime = 0;

	m_bUsedTrace = false;
	m_TraceTarget.Clear();

	m_dwLiveAreaEndTime = 0;
	m_dwPreFrameTime = 0;
	m_dwLostTraceTime = 0;

	m_dwCreateWeaponIndex = 0;
	m_AreaweaponSoundID = 0;

	// random attack
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwGenerateSeed = 0;

	// Point to Point
	m_dwFirstEffect = -1;
	m_dwSecondEffect  = -1;
	m_vFirstPointPos = ioMath::VEC3_ZERO;
	m_vSecondPointPos = ioMath::VEC3_ZERO;
	m_dwPointToPointStartProcessTime = 0;
	m_fPointToPointTotalMoveAmount = 0;

	m_SubAreaType = ST_NONE;

	// target att
	m_dwCheckTargetAttTime = 0;
	m_dwTargetAttCurCnt = 0;
	m_dwCurTargetAttEff = -1;
	m_dwCheckCreateAreaAttackTime = 0;

	m_bCallNextWeaponAttribute = false;
}

ioAreaWeapon::~ioAreaWeapon()
{
	m_vGenerateList.clear();
}

ioAreaWeapon* ioAreaWeapon::Clone()
{
	return new ioAreaWeapon( *this );
}

void ioAreaWeapon::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;

	rkLoader.LoadString_e( "area_weapon_effect_blue", "", szBuf, MAX_PATH );
	m_AreaWeaponEffectBlue = szBuf;
	rkLoader.LoadString_e( "area_weapon_effect_red", "", szBuf, MAX_PATH );
	m_AreaWeaponEffectRed = szBuf;

	rkLoader.LoadString_e( "area_weapon_ready_effect_blue", "", szBuf, MAX_PATH );
	m_AreaWeaponReadyEffectBlue = szBuf;
	rkLoader.LoadString_e( "area_weapon_ready_effect_red", "", szBuf, MAX_PATH );
	m_AreaWeaponReadyEffectRed = szBuf;

	rkLoader.LoadString_e( "area_weapon_end_effect_blue", "", szBuf, MAX_PATH );
	m_AreaWeaponEndEffectBlue = szBuf;
	rkLoader.LoadString_e( "area_weapon_end_effect_red", "", szBuf, MAX_PATH );
	m_AreaWeaponEndEffectRed = szBuf;

	m_bForceEnd = rkLoader.LoadBool_e( "area_weapon_effect_force_end", false );

	rkLoader.LoadString_e( "area_weapon_target_map_effect", "", szBuf, MAX_PATH );
	m_TargetMapEffect = szBuf;

	rkLoader.LoadString_e( "area_weapon_sound", "", szBuf, MAX_PATH );
	m_AreaweaponSound = szBuf;

	m_bEnableAttacker = rkLoader.LoadBool_e( "enable_attacker", false );
	m_bEnableAttackCnt = rkLoader.LoadBool_e( "enable_attack_cnt", false );
	m_bOneTimeUse = rkLoader.LoadBool_e( "one_time_use", false );

	m_AreaType = (AreaType)rkLoader.LoadInt_e( "area_type", AT_TIC );

	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_col_type", TCT_CYLINDER );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetEnableHeight = rkLoader.LoadFloat_e( "target_enable_height_gap", 0.0f );

	m_dwLiveTime      = rkLoader.LoadInt_e( "area_weapon_live_time", 0 );
	m_dwLiveTimeByNPC = rkLoader.LoadInt_e( "area_weapon_live_time_npc", 0 );
	m_dwLiveTimeByFC  = rkLoader.LoadInt_e( "area_weapon_live_time_FC", 0 );
	m_dwReadyTime = rkLoader.LoadInt_e( "area_weapon_ready_time", 0 );
	m_dwCheckTicTime = rkLoader.LoadInt_e( "area_weapon_tic_time", 0 );

	m_fCollisionRange = rkLoader.LoadFloat_e( "area_weapon_range", 0.0f );
	m_fCollisionInRange = rkLoader.LoadFloat_e( "area_weapon_in_range", 0.0f );
	m_fCollisionInRange = min( max(0.0f, m_fCollisionInRange), m_fCollisionRange );

	m_fAreaAngle = rkLoader.LoadFloat_e( "area_weapon_angle", 360.0f );
	m_fUpHeight = rkLoader.LoadFloat_e( "area_weapon_up_height", 0.0f );
	m_fUnderHeight = rkLoader.LoadFloat_e( "area_weapon_under_height", 0.0f );
	m_fWidth = rkLoader.LoadFloat_e( "area_weapon_width", 0.0f );

	m_fNeedGauge = rkLoader.LoadFloat_e( "area_weapon_need_gauge", 0.0f );
	m_dwGaugeTicTime = rkLoader.LoadInt_e( "area_weapon_need_gauge_tic", 0 );

	m_fScaleRate = rkLoader.LoadFloat_e( "area_weapon_scale_rate", FLOAT1 );
	m_dwScaleSlerpTime = rkLoader.LoadInt_e( "area_weapon_scale_time", 0 );

	m_fAreaForceAmt = rkLoader.LoadFloat_e( "area_weapon_force_amt", 0.0f );
	m_fAreaForceFriction = rkLoader.LoadFloat_e( "area_weapon_force_friction", 0.0f );

	m_fRotationTime = rkLoader.LoadFloat_e( "rotation_time", 0.0f );

	// AreaWeapon 자신과 해당 적에게만 보이도록 할때 사용 2020-10-28
	m_bHideEffect = rkLoader.LoadBool_e( "hide_effect", false );

	LoadOwnerBuffList( rkLoader );
	LoadTeamBuffList( rkLoader );
	LoadEnemyBuffList( rkLoader );

	LoadAttributeInfo( rkLoader );
	LoadEndAttributeInfo( rkLoader );
	LoadOverlapAttributeInfo( rkLoader );
	LoadRandomAttack( rkLoader );
	LoadNextCallAttributeList( rkLoader );

	// Move
	m_AreaMoveType = (AreaMoveType)rkLoader.LoadInt_e( "area_weapon_move_type", AMT_NORMAL );
	m_bOwnerMoveDir = rkLoader.LoadBool_e( "area_weapon_owner_move_dir", false );
	m_fAreaMoveSpeed = rkLoader.LoadFloat_e( "area_weapon_move_speed", 0.0f );

	// Move : Change
	LoadChangeMoveInfoList( rkLoader );

	// Move : Trace
	m_fTargetSearchRange = rkLoader.LoadFloat_e( "area_weapon_search_range", 0.0f );
	m_fTargetTraceRange = rkLoader.LoadFloat_e( "area_weapon_trace_range", 0.0f );
	m_dwSencingWaitTime = (DWORD)rkLoader.LoadInt_e( "area_weapon_search_wait_time", 0 );
	m_dwNotFindWaitTime = (DWORD)rkLoader.LoadInt_e( "area_weapon_not_find_wait_time", 0 );

	// Move : LiveArea
	m_bUseLiveArea = rkLoader.LoadBool_e( "use_live_area", false );

	m_vMinLivePos.x = rkLoader.LoadFloat_e( "live_area_min_x", 0.0f );
	m_vMinLivePos.y = rkLoader.LoadFloat_e( "live_area_min_y", 0.0f );
	m_vMinLivePos.z = rkLoader.LoadFloat_e( "live_area_min_z", 0.0f );

	m_vMaxLivePos.x = rkLoader.LoadFloat_e( "live_area_max_x", 0.0f );
	m_vMaxLivePos.y = rkLoader.LoadFloat_e( "live_area_max_y", 0.0f );
	m_vMaxLivePos.z = rkLoader.LoadFloat_e( "live_area_max_z", 0.0f );

	m_dwLiveAreaTime      = (DWORD)rkLoader.LoadInt_e( "live_area_time", 0 );

	// Move : Owner Rot
	m_dwRotateMaxTime = (DWORD)rkLoader.LoadInt_e( "rotate_max_time", 0 );

	// Double
	m_fDoubleInRange = rkLoader.LoadFloat_e( "double_in_range", 0.0f );

	// Point to Point
	m_fEffectOffset = rkLoader.LoadFloat_e( "area_weapon_effect_offset", 0.0f );
	m_fEffectHeightOffset = rkLoader.LoadFloat_e( "area_weapon_effect_height", 0.0f );
	m_fDropHeight = rkLoader.LoadFloat_e( "char_drop_height", 0.0f );
	m_fDropEnableRange = rkLoader.LoadFloat_e( "char_teleport_ranage", 0.0f );

	m_dwPointToPointDelayProcessTime = rkLoader.LoadInt_e( "area_weapon_pre_delay", 0 );

	m_fEnablePositionRange = rkLoader.LoadFloat_e( "enable_drop_postion_range", 0.0f );
	m_fEnableMaxPositionRange = rkLoader.LoadFloat_e( "enable_drop_postion_max_range", 0.0f );

	rkLoader.LoadString_e( "identify_buff", "", szBuf, MAX_PATH );
	szTargetIdentifyBuffName = szBuf;

	// target att
	m_dwTargetAttGap = (DWORD)rkLoader.LoadInt_e( "target_att_gap", 0 );

	m_TargetAttEffList.clear();
	int iEffCnt = rkLoader.LoadInt_e( "target_att_eff_cnt", 0 );
	if( iEffCnt > 0 )
		m_TargetAttEffList.reserve( iEffCnt );

	for( int i=0; i < iEffCnt; ++i )
	{
		wsprintf_e( szKey, "target_att_eff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_TargetAttEffList.push_back( szBuf );
	}

	m_fInvisibleRange = rkLoader.LoadFloat_e( "invisible_to_enemy_range", 0.0f );
	m_dwRefreshDuration = rkLoader.LoadInt_e( "collision_list_refresh_time", 0 );

	m_bUseLengthKill	= rkLoader.LoadBool_e( "use_length_kill", false );
	m_fKillLength		= rkLoader.LoadFloat_e( "kill_length", 0.f );

	rkLoader.LoadString_e( "area_weapon_end_sound", "", szBuf, MAX_PATH );
	m_AreaweaponEndSound = szBuf;

	m_bDelayCreateAreaAttack = rkLoader.LoadBool( "delay_create_attack", false );
	m_dwDelayCreateAreaAttackTime = rkLoader.LoadInt_e( "delay_create_attack_time", 0 );

	//////////////////////////////////////////////////////////////////////////
	m_bUseOffSet = rkLoader.LoadBool_e( "use_area_offset", false );
	m_fOffSet = rkLoader.LoadFloat_e( "area_offset_look", 0.f );

	m_bSetRepeatAttack = rkLoader.LoadBool_e( "set_repeat_attack", false );

	m_bTraceTargetCheckTeam = rkLoader.LoadBool_e( "trace_target_check_team", false );
	m_bCreateOnlyOne = rkLoader.LoadBool_e( "create_only_one", false );

	int nPassDummyCnt = rkLoader.LoadInt_e( "pass_dummy_cnt", 0 );
	m_vPassDummyList.clear();
	m_vPassDummyList.reserve( nPassDummyCnt );

	for ( int i = 0; i < nPassDummyCnt; i++ )
	{
		wsprintf_e( szKey, "pass_dummy_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vPassDummyList.push_back( szBuf );
	}

	int nDestroyWeaponCnt = rkLoader.LoadInt_e( "destroy_weapon_cnt", 0 );
	m_vDestroyWeaponList.clear();
	m_vDestroyWeaponList.reserve( nDestroyWeaponCnt );

	for( int i = 0; i < nDestroyWeaponCnt; i++ )
	{
		wsprintf_e( szKey, "destroy_weapon_type%d", i+1 );
		ioWeapon::WeaponType weaponType = (ioWeapon::WeaponType)rkLoader.LoadInt( szKey, 0 );
		m_vDestroyWeaponList.push_back( weaponType );
	}

	int iWeaponCnt = rkLoader.LoadInt_e( "overlap_area_weapon_cnt", 0 );
	m_vOverlapAreaWeapon.clear();
	m_vOverlapAreaWeapon.reserve( iWeaponCnt );
	
	for( int i = 0; i < iWeaponCnt; i++ )
	{
		wsprintf_e( szKey, "overlap_area_weapon%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_vOverlapAreaWeapon.push_back( szBuf );
	}

	m_OverlapTargetType = (TargetType)rkLoader.LoadInt_e( "overlap_target_type", TT_NONE );
}

void ioAreaWeapon::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	ioHashString szBuffName;

	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_vOwnerBuffList.clear();

	if( iCnt > 0 )
	{
		m_vOwnerBuffList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szTitle, "owner_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vOwnerBuffList.push_back( szBuf );
		}
	}

	iCnt = rkLoader.LoadInt_e( "double_owner_buff_cnt", 0 );
	m_vOwnerBuffList2.clear();

	if( iCnt > 0 )
	{
		m_vOwnerBuffList2.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szTitle, "double_owner_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vOwnerBuffList2.push_back( szBuf );
		}
	}

	iCnt = rkLoader.LoadInt_e( "weapon_end_buff_cnt", 0 );
	m_WeaponEndBuffList.clear();

	if( iCnt > 0 )
	{
		m_WeaponEndBuffList.reserve( iCnt );
		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szTitle, "weapon_end_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_WeaponEndBuffList.push_back( szBuf );
		}
	}
}

void ioAreaWeapon::LoadTeamBuffList( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	ioHashString szBuffName;

	int iCnt = rkLoader.LoadInt_e( "team_buff_cnt", 0 );
	m_vTeamBuffList.clear();

	if( iCnt > 0 )
	{
		m_vTeamBuffList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			ioBuffInfo tBuffInfo;

			wsprintf_e( szTitle, "team_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

			szBuffName = szBuf;

			wsprintf_e( szTitle, "team_buff%d_enable_call_next_weapon", i+1 );
			tBuffInfo.m_bCallNextWeapon = rkLoader.LoadBool( szTitle, false );

			tBuffInfo.m_BuffName = szBuffName;

			m_vTeamBuffList.push_back( tBuffInfo );
		}
	}

	iCnt = rkLoader.LoadInt_e( "double_team_buff_cnt", 0 );
	m_vTeamBuffList2.clear();

	if( iCnt > 0 )
	{
		m_vTeamBuffList2.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szTitle, "double_team_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vTeamBuffList2.push_back( szBuffName );
		}
	}

	iCnt = rkLoader.LoadInt_e( "team_buff_on_create_weapon_cnt", 0 );
	m_vTeamBuffOnCreateWeaponList.clear();

	if( iCnt > 0 )
	{
		m_vTeamBuffOnCreateWeaponList.reserve( iCnt );

		for( int i=0; i<iCnt; ++i )
		{
			wsprintf_e( szTitle, "team_buff_on_create_weapon%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vTeamBuffOnCreateWeaponList.push_back( szBuffName );
		}
	}
}

void ioAreaWeapon::LoadEnemyBuffList( ioINILoader &rkLoader )
{
	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	ioHashString szBuffName;

	int iCnt = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	m_vEnemyBuffList.clear();

	if( iCnt > 0 )
	{
		m_vEnemyBuffList.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			ioBuffInfo tBuffInfo;

			wsprintf_e( szTitle, "enemy_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;
			tBuffInfo.m_BuffName = szBuffName;

			wsprintf_e( szTitle, "enemy_buff%d_enable_call_next_weapon", i+1 );
			tBuffInfo.m_bCallNextWeapon = rkLoader.LoadBool( szTitle, false );

			m_vEnemyBuffList.push_back( tBuffInfo );
		}
	}

	iCnt = rkLoader.LoadInt_e( "double_enemy_buff_cnt", 0 );
	m_vEnemyBuffList2.clear();

	if( iCnt > 0 )
	{
		m_vEnemyBuffList2.reserve( iCnt );

		for( int i=0; i < iCnt; ++i )
		{
			wsprintf_e( szTitle, "double_enemy_buff%d", i+1 );
			rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
			szBuffName = szBuf;

			m_vEnemyBuffList2.push_back( szBuffName );
		}
	}
}

void ioAreaWeapon::LoadAttributeInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "attack_type" );
	m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "attack_type_resist" );
	m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_WeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "attack_type_wounded_time" );
	m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "attack_type_wounded_loop_ani" );
	m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_dwWeaponDuration = (DWORD)rkLoader.LoadInt_e( "attack_collision_duration", 0 );
}

void ioAreaWeapon::LoadEndAttributeInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "end_attack_type" );
	m_EndWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "end_attack_type_resist" );
	m_EndWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "end_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_EndWeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "end_attack_type_wounded_time" );
	m_EndWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "end_attack_type_wounded_loop_ani" );
	m_EndWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_dwEndWeaponDuration = (DWORD)rkLoader.LoadInt_e( "end_attack_collision_duration", 0 );
}

void ioAreaWeapon::LoadOverlapAttributeInfo( ioINILoader &rkLoader )
{
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	wsprintf_e( szKey, "overlap_attack_type" );
	m_OverlapWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "overlap_attack_type_resist" );
	m_OverlapWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "overlap_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_OverlapWeaponInfo.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "overlap_attack_type_wounded_time" );
	m_OverlapWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "overlap_attack_type_wounded_loop_ani" );
	m_OverlapWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_dwOverlapWeaponDuration = (DWORD)rkLoader.LoadInt_e( "overlap_attack_collision_duration", 0 );
}

void ioAreaWeapon::LoadChangeMoveInfoList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "change_move_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_ChangeMoveInfoList.clear();
	m_ChangeMoveInfoList.reserve( iCnt );

	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iCnt; ++i )
	{
		ChangeMoveInfo kInfo;

		wsprintf_e( szBuf, "change_time%d", i+1 );
		kInfo.m_dwChangeTime = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "change_speed%d", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_h%d", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_v%d", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_ChangeMoveInfoList.push_back( kInfo );
	}

	std::sort( m_ChangeMoveInfoList.begin(), m_ChangeMoveInfoList.end(), ChangeMoveSort() );
}

void ioAreaWeapon::LoadRandomAttack( ioINILoader &rkLoader )
{
	m_bUseRandomAttack = rkLoader.LoadBool_e( "use_random_attack", false );
	m_bUseRandomAttackFixedStartPos = rkLoader.LoadBool_e( "use_random_attack_fixed_start_pos", true );

	m_dwMinGenerateTime = (DWORD)rkLoader.LoadInt_e( "min_generate_time", 300 );
	m_dwMaxGenerateTime = (DWORD)rkLoader.LoadInt_e( "max_generate_time", 400 );

	m_fGenerateHeight = rkLoader.LoadFloat_e( "generate_height", 0.0f );

	m_VertDirType = (VertDirType)rkLoader.LoadInt_e( "vert_direction", VDT_NONE );
	m_RandomGenerateFieldType = (RandomGenerateFieldType)rkLoader.LoadInt_e( "random_generate_field_type", RFT_CIRCLE );
}

void ioAreaWeapon::LoadNextCallAttributeList( ioINILoader &rkLoader )
{
	m_vNextAttackAttributeByTeamBuff.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_vNextAttackAttributeByTeamBuff.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponInfo kCallAttribute;

		wsprintf_e( szKey, "call_next_attribute%d_index", i+1 );
		kCallAttribute.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_next_attribute%d_resistance", i+1 );
		kCallAttribute.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_next_attribute%d_time", i+1 );
		kCallAttribute.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_next_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_next_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_next_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vNextAttackAttributeByTeamBuff.push_back( kCallAttribute );

		wsprintf_e( szKey, "call_next_attribute%d_once", i+1 );
		bool bCheckOnceCallWeapon = rkLoader.LoadBool( szKey, false );
		if( bCheckOnceCallWeapon )
			m_CheckOnceCallAttAttrMap.insert( CheckOnceCallAttAttrMap::value_type( kCallAttribute.m_iWeaponIdx, false ) );
	}
}

void ioAreaWeapon::SetInitInfo( const ioHashString& szOwner,
							    const ioHashString& szTarget,
							    const ioHashString& szSkill,
								ioPlayStage *pStage,
								const D3DXVECTOR3 &vStartPos,
								const D3DXQUATERNION &qtRot,
								DWORD dwIndex,
								int eCreateType )
{
	m_RefreshCollisionList.clear();

	m_qtFirstRotate = qtRot;
	m_qtRotate = qtRot;
	m_dwCreatedTime = m_dwCheckProcessTime = m_dwGaugeCheckTime = FRAMEGETTIME();
	m_dwChangeStartTime = m_dwCreatedTime;
	m_dwSencingStartTime = m_dwCreatedTime;

	ioBaseChar *pOwner = pStage->GetBaseChar( szOwner );	// buff를 생성한 캐릭터
	ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );	// buff가 걸린 캐릭터 

	// 자신과 해당 적에게만 이펙트를 보이게 한다. 2020-10-28
	if( m_bHideEffect )
	{
		ioBaseChar* pMyChar = pStage->GetOwnerChar();
		if( pMyChar->GetCharName() != pOwner->GetCharName() && pMyChar->GetCharName() != pTarget->GetCharName() )
		{
			return;
		}
	}


	m_bFirstMove = true;
	m_OwnerName = szOwner;
	m_TargetName = szTarget;

	m_fExtraBuffDuration = 0.0f;
	m_AreaweaponSoundID = 0;

	m_vTargetAttPos = ioMath::VEC3_ZERO;

	// random attack
	m_dwGenerateStartTime = 0;
	m_dwNextGenerateTime = 0;
	m_dwGenerateSeed = 0;

	if( pOwner )
	{
		m_TeamType = pOwner->GetTeam();
		
		if( !szSkill.IsEmpty() )
		{
			for( int i=0; i < ES_OBJECT; ++i )
			{
				ioSkill *pSkill = pOwner->GetEquipedSkill( i );
				if( pSkill && pSkill->GetName() == szSkill )
				{
					m_OwnerSkill = pSkill->GetName();
					ioItem *pItem = pSkill->GetOwnerItem();
					if( pItem )
					{
						m_OwnerItem = pItem->GetName();
					}

					ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(pSkill->GetGrowthUpInfoByType(GT_BUFF_DURATION, pOwner->GetCharName()));
					if( pUpInfo )
					{
						float fValue = pUpInfo->GetValue(pStage);
						m_fExtraBuffDuration = fValue;
					}
				}
			}
		}
	}
	else
	{
		m_TeamType = TEAM_NONE;
	}

	m_dwAreaWeaponIndex = dwIndex;
	m_CreateType = (CreateType)eCreateType;

	m_vFirstPos = vStartPos;
	m_vStartPos = vStartPos;

	if( 0.0f < m_fEffectOffset )
	{
		D3DXVECTOR3 vDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vDir *= m_fEffectOffset;
		m_vFirstPos += vDir;
		m_vStartPos += vDir;
	}

	if( 0.0f < m_fEffectHeightOffset )
	{
		m_vFirstPos.y += m_fEffectHeightOffset;
		m_vStartPos.y += m_fEffectHeightOffset;
	}

	if( pTarget && (m_CreateType == CT_ATTACHED || m_CreateType == CT_TARGET_ATTACHED) )
	{
		m_vStartPos = pTarget->GetMidPositionByRate();
		if( m_bUseOffSet )
		{
			D3DXVECTOR3 vLook = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			m_vStartPos = m_vStartPos + (vLook * m_fOffSet);
		}
	}

	m_vPreStartPos = m_vStartPos;

	m_vNewTargetList.clear();
	m_vCurTargetList.clear();
	m_vCurBlockList.clear();

	m_dwAreaWeaponMapIndex = 0;

	m_fCurRange = m_fCollisionRange;
	m_fCurInRange = m_fCurRange - m_fCollisionInRange;

	// 우선적으로 체크
	if( m_CreateType == CT_SKILL_DEPENDENCY && pOwner->GetState() != CS_USING_SKILL )
	{
		DestroyAreaWeapon( pStage );
		return;
	}

	ioHashString szCurRedEffect = m_AreaWeaponEffectRed;
	ioHashString szCurBlueEffect = m_AreaWeaponEffectBlue;
	if( m_dwReadyTime > 0 )
	{
		m_AreaWeaponState = AWS_READY;
		szCurRedEffect = m_AreaWeaponReadyEffectRed;
		szCurBlueEffect = m_AreaWeaponReadyEffectBlue;
	}
	else
	{
		m_AreaWeaponState = AWS_ACTION;
	}

	ioMapEffect *pMapEffect = NULL;
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	if( m_CreateType == CT_MAP )
	{
		pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
	}
	else if( m_TeamType == TEAM_RED )
	{
		pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
	}
	else if( m_TeamType == TEAM_BLUE )
	{
		pMapEffect = pStage->CreateMapEffect( szCurBlueEffect, m_vStartPos, vScale );
	}
	else
	{
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( szCurBlueEffect, m_vStartPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
	}

	if( pMapEffect )
	{
		m_dwAreaWeaponEffect = pMapEffect->GetUniqueID();
		pMapEffect->SetWorldOrientation( m_qtRotate );

		// Sound
		if( m_AreaWeaponState != AWS_READY )
		{
			ioEffect *pEffect = pMapEffect->GetEffect();
			if( pEffect && !m_AreaweaponSound.IsEmpty() )
			{
				m_AreaweaponSoundID = g_SoundMgr.PlaySound( m_AreaweaponSound, pEffect->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
			}
		}
	}

	m_vAreaMoveDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( !m_bDelayCreateAreaAttack )
		CreateAreaAttack( pStage );
	else
		m_dwCheckCreateAreaAttackTime = FRAMEGETTIME() + m_dwDelayCreateAreaAttackTime;

	CheckNeedGauge( pStage );

	if( m_AreaWeaponState == AWS_READY )
		return;

	CheckAreaWeaponTarget( pStage );

	if( m_AreaType == AT_DOUBLE )
		CheckAreaWeaponBuffByDouble( pStage );
	else
		CheckAreaWeaponBuff( pStage );

	CheckAreaAttack( pStage, true );
	CheckAreaWeaponBuffOnCreateWeapon( pStage );

	GenerateRandomAttack( pOwner );

	if( m_AreaType == AT_OVERLAP && CheckOverlapAreaWeapon( pStage ) )
		CreateOverlapWeapon( pStage );
}

void ioAreaWeapon::SetPointToPointInfo( ioPlayStage* pStage,
										D3DXVECTOR3 vFirstPos, 
										D3DXVECTOR3 vSecondPos,
										ioHashString szFirstEffectName,
										ioHashString szSecondEffectName )
{	

	if( !pStage )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pFirstMapEffect = pStage->CreateMapEffect( szFirstEffectName, vFirstPos, vScale );
	

	if( pFirstMapEffect )
		m_dwFirstEffect = pFirstMapEffect->GetUniqueID();
	
	ioBaseChar* pOwner = pStage->GetBaseChar( GetOwnerName() );
	ioBaseChar* pTarget = pStage->GetCameraTargetChar();
	if( pTarget && pOwner )
	{
		if( pOwner->GetTeam() == pTarget->GetTeam() )
		{
			ioMapEffect *pSecondMapEffect = pStage->CreateMapEffect( szSecondEffectName, vSecondPos, vScale );

			if( pSecondMapEffect )
				m_dwSecondEffect = pSecondMapEffect->GetUniqueID();
		}
	}
	
	m_vFirstPointPos = vFirstPos;
	m_vSecondPointPos = vSecondPos;

	m_vAreaMoveDir = vSecondPos - vFirstPos;
	m_fPointToPointMoveRanage = fabs( D3DXVec3Length( &m_vAreaMoveDir ) );
	D3DXVec3Normalize( &m_vAreaMoveDir, &m_vAreaMoveDir );
	
	m_SubAreaType = ST_POINT_TO_POINT;

	m_dwPointToPointStartProcessTime = FRAMEGETTIME();
	m_dwPointToPointStartProcessTime += m_dwPointToPointDelayProcessTime;
}

void ioAreaWeapon::SetAreaPosition( const D3DXVECTOR3 &vNewPos, ioPlayStage *pStage )
{
	m_vPreStartPos = m_vStartPos;
	m_vStartPos = vNewPos;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
	if( pEff )
	{
		pEff->SetWorldPosition( m_vStartPos );
		pEff->SetWorldOrientation( m_qtRotate );
	}

	CheckTargetPosition();
}

void ioAreaWeapon::SetAreaRotation( const D3DXQUATERNION &qtTargetRot )
{
	m_qtRotate = qtTargetRot;
}

void ioAreaWeapon::SetAreaDirection( const D3DXVECTOR3& vDir )
{
	m_vAreaMoveDir = vDir;
}

void ioAreaWeapon::ProcessState( ioPlayStage *pStage )
{
	if( !m_bLive ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;

	switch( m_SubAreaType )
	{
	case ST_POINT_TO_POINT:
		{
			if( 0 < m_dwPointToPointStartProcessTime && m_dwPointToPointStartProcessTime < dwCurTime )
			{				
				// AreaWeapon Move
				ProcessAreaWeaponMove( pStage );

				if( m_fPointToPointMoveRanage < m_fPointToPointTotalMoveAmount )
				{
					m_bLive = false;
					if( m_dwFirstEffect != -1 )
					{	
						pStage->DestroyMapEffect( m_dwFirstEffect );	
						m_dwFirstEffect = -1;
					}
					if( m_dwSecondEffect != -1 )
					{
						pStage->DestroyMapEffect( m_dwSecondEffect );
						m_dwSecondEffect = -1;
					}
					return;
				}
			}
		}
		break;

	case ST_NONE:
		{
			// 일반 모드
			DWORD dwLiveTime = GetLiveTime();
			if( dwLiveTime > 0 && m_dwCreatedTime+m_dwReadyTime+dwLiveTime < dwCurTime )
			{
				m_bLive = false;
				return;
			}

			if( m_dwLiveAreaEndTime > 0 && m_dwLiveAreaEndTime < dwCurTime )
			{
				m_bLive = false;
				return;
			}	

			if( m_bOneTimeUse && m_bUsedAreaWeapon )
			{
				SetReposition( pStage );
				return;
			}

			// AreaWeapon Move
			ProcessAreaWeaponMove( pStage );
		}
		break;
	}

	if( m_AreaWeaponState == AWS_READY )
	{
		if( m_dwCreatedTime+m_dwReadyTime > dwCurTime )
			return;

		ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
		if( !pOwner ) return;

		m_AreaWeaponState = AWS_ACTION;

		m_dwCheckProcessTime = dwCurTime;
		CheckAreaWeaponTarget( pStage );

		if( m_AreaType == AT_DOUBLE )
			CheckAreaWeaponBuffByDouble( pStage );
		else
			CheckAreaWeaponBuff( pStage );

		CheckAreaAttack( pStage, true );

		GenerateRandomAttack( pOwner );

		if( m_dwAreaWeaponEffect != -1 )
		{
			ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
			if( pEff )
			{	
				if( m_SubAreaType == ST_NONE )
				{
					if( m_CreateType == CT_ATTACHED )
						pEff->EndEffectForce();
					else
						pEff->EndEffectGrace();
				}
				else if( m_SubAreaType == ST_POINT_TO_POINT )
				{
					pStage->DestroyMapEffect( m_dwAreaWeaponEffect );
				}

				m_dwAreaWeaponEffect = -1;
			}

			g_SoundMgr.StopSound( m_AreaweaponSound, m_AreaweaponSoundID );
			//g_SoundMgr.StopSound( m_AreaweaponSound, 0 );
		}

		ioHashString szCurRedEffect = m_AreaWeaponEffectRed;
		ioHashString szCurBlueEffect = m_AreaWeaponEffectBlue;

		ioMapEffect *pMapEffect = NULL;
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		if( m_CreateType == CT_MAP )
		{
			pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
		}
		else if( m_TeamType == TEAM_RED )
		{
			pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
		}
		else if( m_TeamType == TEAM_BLUE )
		{
			pMapEffect = pStage->CreateMapEffect( szCurBlueEffect, m_vStartPos, vScale );
		}
		else
		{
			if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( szCurBlueEffect, m_vStartPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( szCurRedEffect, m_vStartPos, vScale );
		}

		if( pMapEffect )
		{
			m_dwAreaWeaponEffect = pMapEffect->GetUniqueID();
			pMapEffect->SetWorldOrientation( m_qtRotate );

			ioEffect *pEffect = pMapEffect->GetEffect();
			if( pEffect && !m_AreaweaponSound.IsEmpty() )
			{
				m_AreaweaponSoundID = g_SoundMgr.PlaySound( m_AreaweaponSound, pEffect->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
			}
		}
	}

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( pTarget && (m_CreateType == CT_ATTACHED || m_CreateType == CT_TARGET_ATTACHED) )
	{
		m_vPreStartPos = m_vStartPos;
		m_vStartPos = pTarget->GetMidPositionByRate();
		m_qtRotate = pTarget->GetTargetRot();

		if( m_bUseOffSet )
		{
			D3DXVECTOR3 vLook = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			m_vStartPos = m_vStartPos + (vLook * m_fOffSet);
		}

		ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
		if( pEff )
		{
			pEff->SetWorldPosition( m_vStartPos );
			pEff->SetWorldOrientation( m_qtRotate );
		}
	}

	// 우선적으로 체크
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( pOwner && m_CreateType == CT_SKILL_DEPENDENCY && pOwner->GetState() != CS_USING_SKILL )
	{
		DestroyAreaWeapon( pStage );
		return;
	}

	//거리에 따른 체크
	if( m_bUseLengthKill && pOwner )
	{
		D3DXVECTOR3 vSub = GetStartPos() - pOwner->GetWorldPosition();
		float fLenth = fabs( D3DXVec3Length( &vSub ) );
		if( fLenth > m_fKillLength )
		{
			DestroyAreaWeapon( pStage );
			return;
		}
	}

	// Check State
	if( m_dwCreatedTime+m_dwReadyTime > dwCurTime )
		return;

	CheckNeedGauge( pStage );
	// 게이지 처리시 종료될수 있음.
	if( !m_bLive ) return;

	// 영역 범위 업데이트
	UpdateCollisionRange();

	// 대상 검색/처리
	CheckAreaWeaponTarget( pStage );

	// attack
	if( pOwner && m_AreaWeaponState == AWS_ACTION )
	{
		ProcessRandomAttack( pOwner );

		if( m_AreaType == AT_ATTACK )
			ProcessTargetAttack( pOwner );
	}

	switch( m_AreaType )
	{
	case AT_TIC:
		if( m_dwCheckProcessTime + m_dwCheckTicTime < dwCurTime )
		{
			m_dwCheckProcessTime = dwCurTime;
			CheckAreaWeaponBuff( pStage );
			CheckAreaAttack( pStage, true );

			if (m_bSetRepeatAttack)
				CreateAreaAttack( pStage );
		}
		break;
	case AT_INOUT:
		UpdateTargetListByInOut( pStage );
		CheckAreaWeaponBuff( pStage );
		break;
	case AT_SCALE:
		UpdateTargetListByScale( pStage );
		CheckAreaWeaponBuff( pStage );
		CheckAreaAttack( pStage, false );

		if( 0 < m_dwPointToPointStartProcessTime && m_dwPointToPointStartProcessTime < dwCurTime )
		{
			switch( m_SubAreaType )
			{
				case ST_POINT_TO_POINT:
				{				
					CheckAreaTargetTeleport( pStage );
				}
				break;
			}
		}

		break;
	case AT_DOUBLE:
		UpdateTargetListByDouble( pStage );
		CheckAreaWeaponBuffByDouble( pStage );
		CheckAreaAttack( pStage, false );
		break;
	case AT_DESTROY:
		CheckDestroy( pStage );
		break;
	}

	CheckTargetRotation();
	CheckVisible( pStage );
	m_dwPreFrameTime = dwCurTime;

	if ( m_bDelayCreateAreaAttack && m_dwCheckCreateAreaAttackTime > 0 && m_dwCheckCreateAreaAttackTime < dwCurTime )
	{
		CreateAreaAttack( pStage );
		m_dwCheckCreateAreaAttackTime = 0;
	}
}

void ioAreaWeapon::DestroyAreaWeapon( ioPlayStage *pStage, bool bActive /*= true */ )
{
	m_bLive = false;

	bool bWeaponDeadType = true;
	switch( m_AreaType )
	{
	case AT_ATTACK:
		bWeaponDeadType = false;
		break;
	}

	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( pOwner && m_dwWeaponBaseIndex > 0 && bWeaponDeadType )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon(pOwner, m_dwWeaponBaseIndex);
		if( pWeapon )
		{
			pWeapon->SetWeaponDead();
			m_dwWeaponBaseIndex = 0;
		}
	}

	if( m_AreaType == AT_INOUT )
	{
		ioHashStringVec::iterator iter = m_vCurTargetList.begin();
		while( iter != m_vCurTargetList.end() )
		{
			ioHashString szName;
			szName = (*iter);

			ioBaseChar *pChar = pStage->GetBaseChar( szName );
			if( !pChar )
			{
				iter = m_vCurTargetList.erase( iter );
				continue;
			}

			TargetType eType = TT_OWNER;

			if( m_CreateType == CT_MAP )
				eType = TT_ENEMY;
			else if( pChar->GetCharName() == m_OwnerName && m_TeamType == pChar->GetTeam() )
				eType = TT_OWNER;
			else if( m_TeamType == pChar->GetTeam() )
				eType = TT_TEAM;
			else if( m_TeamType != pChar->GetTeam() )
				eType = TT_ENEMY;


			if( pChar->IsNeedProcess() )
				RemoveTargetBuff( pChar, TT_NONE, true );

			iter = m_vCurTargetList.erase( iter );
		}
	}

	m_vCurTargetList.clear();
	m_vCurBlockList.clear();
	m_TargetAttEffList.clear();

	if( m_dwAreaWeaponEffect != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
		if( pEff )
		{
			if( m_CreateType == CT_ATTACHED )
				pEff->EndEffectForce();
			else
			{
				if( !m_bForceEnd && !m_bCreateOnlyOne )
					pEff->EndEffectGrace();
				else
					pEff->EndEffectForce();
			}
		}

		g_SoundMgr.StopSound( m_AreaweaponSound, m_AreaweaponSoundID );
		//g_SoundMgr.StopSound( m_AreaweaponSound, 0 );
	}

	if( m_dwCurTargetAttEff != -1 )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwCurTargetAttEff );
		if( pEff )
		{
			pEff->EndEffectGrace();
		}
	}

	if( m_SubAreaType == ST_POINT_TO_POINT )
	{
		if( m_dwFirstEffect != -1 )
		{
			pStage->DestroyMapEffect( m_dwFirstEffect );	
			m_dwFirstEffect = -1;
		}
		if( m_dwSecondEffect != -1 )
		{
			pStage->DestroyMapEffect( m_dwSecondEffect );
			m_dwSecondEffect = -1;
		}
	}

	if ( !bActive )
		return;
	ioMapEffect *pMapEndEffect = NULL;
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	if( m_TeamType == TEAM_RED )
	{
		pMapEndEffect = pStage->CreateMapEffect( m_AreaWeaponEndEffectRed, m_vStartPos, vScale );
	}
	else if( m_TeamType == TEAM_BLUE )
	{
		pMapEndEffect = pStage->CreateMapEffect( m_AreaWeaponEndEffectBlue, m_vStartPos, vScale );
	}
	else
	{
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEndEffect = pStage->CreateMapEffect( m_AreaWeaponEndEffectBlue, m_vStartPos, vScale );
		else
			pMapEndEffect = pStage->CreateMapEffect( m_AreaWeaponEndEffectRed, m_vStartPos, vScale );
	}

	if ( pMapEndEffect )
	{
		pMapEndEffect->SetWorldOrientation( m_qtRotate );

		if( !m_AreaweaponEndSound.IsEmpty() )
		{
			ioEffect *pEffect = pMapEndEffect->GetEffect();
			g_SoundMgr.PlaySound( m_AreaweaponEndSound, pEffect->GetParentSceneNode() );
		}
	}

	//추가 yongjun 
	CreateEndAreaAttack( pStage );
}

void ioAreaWeapon::SetLiveTime( DWORD dwLiveTime )
{
	m_dwLiveTime = dwLiveTime;
}

void ioAreaWeapon::UpdateTargetListByInOut( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	ioHashStringVec::iterator iter = m_vCurTargetList.begin();
	while( iter != m_vCurTargetList.end() )
	{
		ioHashString szName;
		szName = (*iter);

		ioBaseChar *pChar = pStage->GetBaseChar( szName );
		if( !pChar )
		{
			iter = m_vCurTargetList.erase( iter );
			continue;
		}

		int i = 0;
		bool bExist = false;
		int iNewCnt = m_vNewTargetList.size();
		for( i=0; i < iNewCnt; ++i )
		{
			if( !m_vNewTargetList[i] )
				continue;

			if( szName == m_vNewTargetList[i]->GetCharName() )
			{
				bExist = true;
				break;
			}
		}

		TargetType eType = TT_OWNER;
		if( m_CreateType == CT_MAP )
			eType = TT_ENEMY;
		else if( pChar->GetCharName() == m_OwnerName && m_TeamType == pChar->GetTeam() )
			eType = TT_OWNER;
		else if( m_TeamType == pChar->GetTeam() )
			eType = TT_TEAM;
		else if( m_TeamType != pChar->GetTeam() )
			eType = TT_ENEMY;

		if( pChar->IsNeedProcess() )
		{
			if( !bExist )
			{
				// 해당 타겟의 버프 해제
				RemoveTargetBuff( pChar, TT_NONE, true );
				iter = m_vCurTargetList.erase( iter );
				continue;
			}
			else if( !IsCollisioned(pChar, TT_NONE) )
			{
				// 해당 타겟의 버프 해제
				RemoveTargetBuff( pChar, TT_NONE, true );
				iter = m_vCurTargetList.erase( iter );
				continue;
			}
		}

		++iter;
	}
}

void ioAreaWeapon::UpdateTargetListByScale( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	BaseCharList::iterator iter = m_vNewTargetList.begin();

	while( iter != m_vNewTargetList.end() )
	{
		ioBaseChar *pChar = (*iter);
		if( !pChar )
		{
			iter = m_vNewTargetList.erase(iter);
			continue;
		}

		if( FindTarget( pChar->GetCharName() ) )
		{
			if( !CheckRefreshCollisionList( pChar->GetCharName(), pStage ) )
			{
				iter = m_vNewTargetList.erase(iter);
				continue;
			}
		}

		iter++;
	}
}

void ioAreaWeapon::UpdateCollisionRange()
{
	if( m_AreaType != AT_SCALE )
		return;

	if( m_dwScaleSlerpTime == 0 )
	{
		m_fCurRange = m_fCollisionRange * m_fScaleRate;
		return;
	}


	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwCreatedTime;
	dwGapTime = min( m_dwScaleSlerpTime, dwGapTime );

	float fRate = (float)dwGapTime / (float)m_dwScaleSlerpTime;
	float fTotalRate = FLOAT1 + fRate * (m_fScaleRate - FLOAT1);
	
	m_fCurRange = m_fCollisionRange * fTotalRate;
	m_fCurInRange = m_fCurRange - m_fCollisionInRange;
}

void ioAreaWeapon::CheckNeedGauge( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( !pOwner ) return;

	if( m_fNeedGauge == 0.0f ) return;
	if( m_dwGaugeTicTime == 0 ) return;


	if( m_CreateType == CT_STRUCT || m_CreateType == CT_TARGET_ATTACHED )
		return;

	if( m_CreateType == CT_NORMAL && pOwner->GetState() != CS_USING_SKILL )
		return;

	ioItem *pOwnerItem = NULL;
	for( int i=0; i < ES_OBJECT; ++i )
	{
		pOwnerItem = pOwner->GetEquipedItem( i );
		if( pOwnerItem && pOwnerItem->GetName() == m_OwnerName )
			break;

		pOwnerItem = NULL;
	}

	if( !pOwnerItem )
	{
		DestroyAreaWeapon( pStage );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGaugeCheckTime;

	float fCurGauge = FLOAT100;
	while( dwGapTime > m_dwGaugeTicTime )
	{
		fCurGauge = pOwner->ReduceCurSkillGaugeBySkill(  pOwnerItem, m_fNeedGauge );

		dwGapTime -= m_dwGaugeTicTime;

		m_dwGaugeCheckTime = dwCurTime - dwGapTime;
	}

	if( m_CreateType == CT_ATTACHED && fCurGauge <= 0.0f )
	{
		DestroyAreaWeapon( pStage );
	}
}

void ioAreaWeapon::CheckAreaWeaponTarget( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	m_vNewTargetList.clear();

	switch( m_AreaType )
	{
	case AT_ATTACK:
		return;
	}

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		GetTargetCharListBySphere( pStage, m_vNewTargetList );
		break;
	case TCT_CYLINDER:
		GetTargetCharListByCylinder( pStage, m_vNewTargetList );
		break;
	case TCT_BOX:
		GetTargetCharListByBox( pStage, m_vNewTargetList );
		break;
	}

	// CT_MAP의 경우 pOwner가 NULL인 상황 유념 필요.
	if( pOwner && pOwner->GetExperienceMode() == EMS_EXPERIENCE )
	{
		m_vNewTargetList.clear();
	}

	bool bCallNextWeapon = false;
	for( int i = 0; i < (int)m_vTeamBuffList.size(); i++ )
	{
		if( m_vTeamBuffList[ i ].m_bCallNextWeapon )
			bCallNextWeapon = true;
	}

	BaseCharList::iterator iter = m_vNewTargetList.begin();
	while( iter != m_vNewTargetList.end() )
	{
		ioBaseChar *pChar = (*iter);

		if( bCallNextWeapon )
		{
			// 같은 팀일때 추가 웨폰 발동(이후 다른 팀일때도 하려면 아래 조건 추가
			if( m_TeamType == pChar->GetTeam() && GetOwnerName() != pChar->GetCharName() )
			{
				if( !m_bCallNextWeaponAttribute )
					CallNextWeaponAttribute( pOwner );
			}
		}

		if( !pChar || 
			!pChar->IsNeedProcess() || 
			pChar->GetState() == CS_SKIP_STUN ||
			Help::CheckTargetNoCollision( pChar->GetState() ) ||
			!pChar->HasCollisionAvailableBuff() ||
			pChar->IsSkipState() ||
			!pStage->IsCollisionCheckByMode() || 
			pChar->GetExperienceMode() == EMS_EXPERIENCE )
		{
			iter = m_vNewTargetList.erase( iter );
			continue;
		}

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pChar->IsApplyDownState(false) )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( pChar->IsApplyDownState() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}
		else if( m_TargetWoundType == TWT_ALL_WOUNDED )
		{
			if( pChar->IsFloating() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
			/*
			if( !pChar->CheckTargetAllWoundedState() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
			*/
		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			float fGapHeight = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
			if( !pChar->IsFloating() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
			else if( fGapHeight < m_fTargetEnableHeight )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( !pChar->IsApplyDownState() || pChar->IsFloating() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}
		else if ( m_TargetWoundType == TWT_WOUNDED )
		{
			if ( !pChar->IsWoundedState() )
			{
				iter = m_vNewTargetList.erase( iter );
				continue;
			}
		}

		++iter;
	}

	m_bCallNextWeaponAttribute = true;
}

void ioAreaWeapon::CheckAreaWeaponBuff( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	switch( m_AreaType )
	{
	case AT_ATTACK:
		return;
	}

	int iNewCnt = m_vNewTargetList.size();
	int iTeamUserCnt = 0;
	for( int i=0; i < iNewCnt; ++i )
	{
		ioBaseChar *pChar = m_vNewTargetList[i];
		if( !pChar ) continue;
		if( !pChar->IsNeedProcess() ) continue;

		if( m_AreaType == AT_INOUT && FindTarget( pChar->GetCharName() ) )
			continue;

		if( m_CreateType != CT_MAP && pChar->GetCharName() == m_OwnerName && m_TeamType == pChar->GetTeam() )
		{
			if( !IsCollisioned(pChar, TT_OWNER) && SetOwnerBuffList( pChar ) )
			{
				if( m_AreaType == AT_INOUT )
					m_vCurTargetList.push_back( pChar->GetCharName() );
				else if( m_AreaType == AT_SCALE )
					m_vCurTargetList.push_back( pChar->GetCharName() );
			}
		}
		else
		{
			bool bEnableTeamAttack = pChar->IsEnableTeamAttack();
			iTeamUserCnt++;
			if( m_CreateType != CT_MAP && m_TeamType == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar, iTeamUserCnt ) )
			{
				if( pOwner && m_bEnableAttackCnt )
					pOwner->CheckTeamBuffAttackCnt( pChar );

				if( m_AreaType == AT_INOUT )
					m_vCurTargetList.push_back( pChar->GetCharName() );
				else if( m_AreaType == AT_SCALE )
					m_vCurTargetList.push_back( pChar->GetCharName() );
			}

			if( (m_CreateType == CT_MAP || m_TeamType != pChar->GetTeam() || bEnableTeamAttack) && SetEnemyBuffList( pOwner, pChar, pStage ) )
			{
				if( pOwner && m_bEnableAttackCnt )
				{
					//pOwner->CheckRealComboHit( pChar->GetCharName() );
					pOwner->CheckComboSkillHit();
					pOwner->CheckAttackCnt( pChar );
				}

				if( m_AreaType == AT_INOUT )
				{
					m_vCurTargetList.push_back( pChar->GetCharName() );
				}
				else if( m_AreaType == AT_SCALE )
					m_vCurTargetList.push_back( pChar->GetCharName() );
			}
		}
	}
}

bool ioAreaWeapon::IsCollisioned( ioBaseChar *pTarget, TargetType eType )
{
	bool bResult = false;
	int iCnt = 0;
	ioHashString szBuffName;

	if( eType == TT_NONE || eType == TT_OWNER )
	{
		iCnt = m_vOwnerBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vOwnerBuffList[i];

			if( pTarget->HasBuff( szBuffName ) )
			{
				bResult = true;
				break;
			}
		}
	}

	if( eType == TT_NONE || eType == TT_TEAM )
	{
		iCnt = m_vTeamBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vTeamBuffList[i].m_BuffName;

			if( pTarget->HasBuff( szBuffName ) )
			{
				bResult = true;
				break;
			}
		}
	}
	
	if( eType == TT_NONE || eType == TT_ENEMY )
	{
		iCnt = m_vEnemyBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vEnemyBuffList[i].m_BuffName;

			if( pTarget->HasBuff( szBuffName ) )
			{
				bResult = true;
				break;
			}
		}
	}

	return bResult;
}

bool ioAreaWeapon::IsBlocked( ioBaseChar *pChar )
{
	ioHashStringVec::iterator iter = std::find( m_vCurBlockList.begin(),
												m_vCurBlockList.end(),
												pChar->GetCharName() );

	if( iter != m_vCurBlockList.end() )
		return true;

	return false;
}

void ioAreaWeapon::GetTargetCharListBySphere( ioPlayStage *pStage, BaseCharList &rkCharList )
{
	rkCharList.clear();

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	float fCurAngle = m_fAreaAngle;

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		D3DXVECTOR3 vPoint;
		if( pChar->CheckSpherePoint( m_vStartPos, m_fCurRange, vPoint ) )
		{

			D3DXVECTOR3 vDir = vPoint - m_vStartPos;
			D3DXVec3Normalize( &vDir, &vDir );

			float fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );
			if( D3DXVec3Dot( &m_vAreaMoveDir, &vDir ) >= fHalfCosine )
				rkCharList.push_back( pChar );
		}
	}
}

void ioAreaWeapon::GetTargetCharListByCylinder( ioPlayStage *pStage, BaseCharList &rkCharList )
{
	rkCharList.clear();
	
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return;

	D3DXVECTOR3 vTop, vBottom, vCenter;
	vTop = vBottom = m_vStartPos;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fCurRange );
	vCenter = kCylinder.GetCenter();

	float fRangeSq = m_fCurRange * m_fCurRange;
	float fInRangeSq = m_fCurInRange * m_fCurInRange;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	float fCurAngle = m_fAreaAngle;

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		D3DXVECTOR3 vCollisionPoint;
		if( !pChar->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
			continue;

		// 수평 거리 체크
		D3DXVECTOR3 vDiff = vCollisionPoint - vCenter;
		vDiff.y = 0.0f;

		float fTargetDist = D3DXVec3LengthSq( &vDiff );
		if( fTargetDist > fRangeSq )
			continue;

		if( m_fCollisionInRange > 0.0f && fTargetDist < fInRangeSq )
			continue;


		// 수평 각 체크
		D3DXVECTOR3 vDiffDir;
		float fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );
		D3DXVec3Normalize( &vDiffDir, &vDiff );

		if( D3DXVec3Dot( &m_vAreaMoveDir, &vDiffDir ) < fHalfCosine )
			continue;

		// 높이 체크
		if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
			rkCharList.push_back( pChar );
	}
}

void ioAreaWeapon::GetTargetCharListByBox( ioPlayStage *pStage, BaseCharList &rkCharList )
{
	rkCharList.clear();

	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return;

	if( m_fWidth == 0.0f )
		return;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = m_vStartPos;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fCurRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

	// 사각형 위치를 중심으로 변경하면서 정확한 높이 조정을 위해서 offset 적용.
	D3DXVECTOR3 vCenterPos = m_vStartPos;
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenterPos -= vOffset;
	kBuffBox.SetCenter( vCenterPos );

	// 방향전환
	kBuffBox.Transform( vCenterPos, ioMath::UNIT_ALL, m_qtRotate );
	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
	kBuffBox.SetCenter( vCenterPos );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	if( !pStage )
		return;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		D3DXVECTOR3 vCollisionPoint;
		if( !pChar->IsWoundedCollision( kBuffBox ) )
			continue;

		rkCharList.push_back( pChar );
	}
}

void ioAreaWeapon::RemoveTargetBuff( ioBaseChar *pTarget, TargetType eType, bool bSend )
{
	ioHashStringVec vBuffList;
	bool bResult = false;
	int iCnt, i;
	iCnt = i = 0;

	if( eType == TT_NONE || eType == TT_OWNER )
	{
		iCnt = m_vOwnerBuffList.size();
		for( i=0; i < iCnt; ++i )
		{
			ioBuff *pBuff = pTarget->GetBuff( m_vOwnerBuffList[i] );
			if( !pBuff || !pBuff->IsRemoveEnable() )
				continue;

			ioHashString szBuffName = m_vOwnerBuffList[i];
			if( pTarget->RemoveBuff( szBuffName ) )
			{
				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( eType == TT_NONE || eType == TT_TEAM )
	{
		iCnt = m_vTeamBuffList.size();
		for( i=0; i < iCnt; ++i )
		{
			ioBuff *pBuff = pTarget->GetBuff( m_vTeamBuffList[i].m_BuffName );
			if( !pBuff || !pBuff->IsRemoveEnable() )
				continue;

			ioHashString szBuffName = m_vTeamBuffList[i].m_BuffName;
			if( pTarget->RemoveBuff( szBuffName ) )
			{
				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}
	
	if( eType == TT_NONE || eType == TT_ENEMY )
	{
		iCnt = m_vEnemyBuffList.size();
		for( i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vEnemyBuffList[i].m_BuffName;
			if( pTarget->RemoveBuff( szBuffName ) )
			{
				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	int iSize = vBuffList.size();
	if( iSize == 0 ) return;

	if( pTarget->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_REMOVE_BUFF );
		kPacket << pTarget->GetCharName();
		kPacket << iSize;

		for( i=0; i < iSize; i++ )
		{
			kPacket << vBuffList[i];
		}

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

DWORD ioAreaWeapon::GetCurLiveTime()
{
	return (FRAMEGETTIME() - m_dwCreatedTime);
}

void ioAreaWeapon::SetCurLiveTime( DWORD dwCurGapTime )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( dwCurTime < dwCurGapTime ) return;

	m_dwCreatedTime = dwCurTime - dwCurGapTime;
}

void ioAreaWeapon::SetAreaReadyTime( DWORD dwReadyTime )
{
	m_dwReadyTime = dwReadyTime;
}

bool ioAreaWeapon::FindTarget( const ioHashString &szTarget )
{
	ioHashStringVec::iterator iter;
	for( iter = m_vCurTargetList.begin(); iter != m_vCurTargetList.end(); ++iter )
	{
		if( (*iter) == szTarget )
			return true;
	}

	return false;
}

void ioAreaWeapon::FillCurTargetList( SP2Packet &rkPacket )
{
	int iSize = m_vCurTargetList.size();

	rkPacket << iSize;

	for( int i=0; i < iSize; ++i )
	{
		rkPacket << m_vCurTargetList[i];
	}
}

void ioAreaWeapon::ApplyCurTargetList( ioHashStringVec vTargetList )
{
	int iSize = vTargetList.size();
	if( iSize == 0 ) return;

	m_vCurTargetList.clear();

	for( int i=0; i < iSize; ++i )
	{
		m_vCurTargetList.push_back( vTargetList[i] );
	}
}

bool ioAreaWeapon::SetOwnerBuffList( ioBaseChar *pChar )
{
	if( !pChar->IsNeedProcess() )
		return false;
	if( m_vOwnerBuffList.empty() )
		return false;

	if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;
	
	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	int iCnt = m_vOwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_vOwnerBuffList[i];

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pChar->GetCharName(), m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioAreaWeapon::SetTeamBuffList( ioBaseChar *pOwner, ioBaseChar *pChar, int iTeamUserCnt )
{
	if( m_vTeamBuffList.empty() )
		return false;

	if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;
	
	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;
	
	ioHashString szCreator;
	if( pOwner )
		szCreator = pOwner->GetCharName();

	int iCnt = m_vTeamBuffList.size();
	for( int i = 0 ; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vTeamBuffList[i].m_BuffName;

		//오브젝트 아이템 장착 버프가 AreaWeapon을 통해서 생성되는 경우에 먼저 현재 장착되어있는 오브젝트 아이템에 대해서 체크
		if( !CheckTargetEquipObjectBuff( szBuffName, pChar ) )
			return false;
	}

	for( int i = 0 ; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vTeamBuffList[i].m_BuffName;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( szCreator, GetOwnerSkillName() );
				}

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}
	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << szCreator;
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioAreaWeapon::SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bOneTimeUse && m_bUsedAreaWeapon )
		return false;

	if( m_vEnemyBuffList.empty() )
		return false;

	if( pChar->IsCanNoWound() && !CheckIgnoreProtectBuff() )
		return false;

	if( pChar->IsProtectState() && !CheckIgnoreProtectBuff() )
	{
		int iType = pChar->CheckBlockEmoticonType();
		if( iType >= 0 && !IsBlocked( pChar ) )
		{
			pChar->SetProtectWound( iType );
			m_vCurBlockList.push_back( pChar->GetCharName().c_str() );
		}

		return false;
	}

	//
	DWORD dwSeed = pChar->GetRandomSeed();
	DWORD dwNewSeed = timeGetTime();

	pChar->SetRandomSeed( dwNewSeed );
	//

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szCreator;
	if( pOwner )
		szCreator = pOwner->GetCharName();

	int iCnt = m_vEnemyBuffList.size();
	for( int i = 0 ; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vEnemyBuffList[i].m_BuffName;

		// 2020-08-19
		ioBuff *pBuff = pChar->GetBuff( szBuffName );
		if( pBuff && pBuff->IsLive() && pBuff->IsBuffDuration() )
		{

			return false;
		}

		//오브젝트 아이템 장착 버프가 AreaWeapon을 통해서 생성되는 경우에 먼저 현재 장착되어있는 오브젝트 아이템에 대해서 체크
		if( !CheckTargetEquipObjectBuff( szBuffName, pChar ) )
			return false;
	}

	for( int i = 0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vEnemyBuffList[i].m_BuffName;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				//생성한 버프에 Area 무기 인덱스 넣기
				pBuff->SetAreaWeaponIndex( m_dwAreaWeaponIndex );

				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( szCreator, GetOwnerSkillName() );
				}

				vBuffList.push_back( szBuffName );
				bResult = true;

				ioCloseOrderBuff *pCloseOrder = ToCloseOrderBuff(pBuff);
				if( pCloseOrder && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE )
				{
					pCloseOrder->SetTargetAreaWeapon( m_dwAreaWeaponIndex );
				}

				ioPullBuff *pPullBuff = ToPullBuff(pBuff);
				if( pPullBuff/* && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE*/ )
				{
					pPullBuff->SetTargetAreaWeapon( m_dwAreaWeaponIndex );
				}
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( m_bOneTimeUse )
		{
			m_bUsedAreaWeapon = true;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_AREAWEAPON );
				kPacket << AREAWEAPON_MOVE_INFO;
				kPacket << m_dwAreaWeaponMapIndex;
				kPacket << m_dwAreaWeaponIndex;
				kPacket << AUT_USED;
				kPacket << m_vStartPos;
				kPacket << m_vAreaMoveDir;
				kPacket << m_qtRotate;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		D3DXVECTOR3 vDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		if( m_fAreaForceAmt > 0.0f )
		{
			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, vDir );
			pChar->SetTargetRot( qtCharRot );
			pChar->SetForcePower( vDir, m_fAreaForceAmt, m_fAreaForceFriction, true );
		}

		// TargetMapEffect
		D3DXVECTOR3 vEffectPos = pChar->GetWorldPosition();
		if( pStage && !m_TargetMapEffect.IsEmpty() )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			pStage->CreateMapEffect( m_TargetMapEffect, vEffectPos, vScale );
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << szCreator;
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
		
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			
			kPacket << COWT_AREAWEAPON;			// Check CloseOrderBuff(12.07.13 수정)
			kPacket << GetOwnerName();
			kPacket << GetAreaWeaponName();
			
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << true;
			kPacket << m_fAreaForceAmt;
			kPacket << m_fAreaForceFriction;
			kPacket << vDir;

			kPacket << vEffectPos;
			kPacket << m_TargetMapEffect;

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		// 복구
		pChar->SetRandomSeed( dwSeed );
	}

	return bResult;
}

bool ioAreaWeapon::SetTeamBuffOnCreateWeaponList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( m_vTeamBuffOnCreateWeaponList.empty() )
		return false;

	if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szCreator;
	if( pOwner )
		szCreator = pOwner->GetCharName();

	int iCnt = m_vTeamBuffOnCreateWeaponList.size();
	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vTeamBuffOnCreateWeaponList[i];

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( szCreator, GetOwnerSkillName() );
				}

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << szCreator;
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioAreaWeapon::IsCreateByStruct()
{
	if( m_CreateType == CT_STRUCT )
		return true;

	return false;
}

void ioAreaWeapon::CheckTargetRotation()
{
	if( m_fRotationTime <= 0.0f )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	int iNewCnt = m_vNewTargetList.size();
	for( int i=0; i < iNewCnt; ++i )
	{
		ioBaseChar *pChar = m_vNewTargetList[i];
		if( !pChar ) continue;

		bool bContinue = true;

		// Areaweapon에 걸리수 있는 캐릭터인지 체크
		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();
		if( (m_CreateType == CT_MAP || m_TeamType != pChar->GetTeam() || bEnableTeamAttack) )
			bContinue = false;

		if( bContinue ) continue;

		// EnemyBuff가 걸려있는 캐릭터인지 체크
		bContinue = true;
		int iCnt = m_vEnemyBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vEnemyBuffList[i].m_BuffName;

			if( pChar->HasBuff( szBuffName ) )
			{
				bContinue = false;
				break;
			}
		}

		if( bContinue ) continue;


		// 회전처리
		float fGapTime = fTimePerSec * FLOAT1000;
		float fRotationRate = fGapTime / m_fRotationTime;
		float fAngle = 360.0f * fRotationRate;

		fAngle = ioMath::ArrangeHead( fAngle );

		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

		D3DXVECTOR3 vAreaPos = m_vStartPos;
		D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();

		vAreaPos.y = vCharPos.y;

		D3DXVECTOR3 vDiffPos = vCharPos - vAreaPos;
		vDiffPos = qtAngle * vDiffPos;

		D3DXVECTOR3 vNewPos = vAreaPos + vDiffPos;

		pChar->SetWorldPosition( vNewPos );
	}
}

void ioAreaWeapon::CheckTargetPosition()
{
	if( m_fRotationTime <= 0.0f )
		return;

	D3DXVECTOR3 vDiffPos = m_vStartPos - m_vPreStartPos;

	int iNewCnt = m_vNewTargetList.size();
	for( int i=0; i < iNewCnt; ++i )
	{
		ioBaseChar *pChar = m_vNewTargetList[i];
		if( !pChar ) continue;

		bool bContinue = true;

		// Areaweapon에 걸리수 있는 캐릭터인지 체크
		bool bEnableTeamAttack = pChar->IsEnableTeamAttack();
		if( (m_CreateType == CT_MAP || m_TeamType != pChar->GetTeam() || bEnableTeamAttack) )
			bContinue = false;

		if( bContinue ) continue;

		// EnemyBuff가 걸려있는 캐릭터인지 체크
		bContinue = true;
		int iCnt = m_vEnemyBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_vEnemyBuffList[i].m_BuffName;

			if( pChar->HasBuff( szBuffName ) )
			{
				bContinue = false;
				break;
			}
		}

		if( bContinue ) continue;

		D3DXVECTOR3 vNewPos = pChar->GetWorldPosition() + vDiffPos;
		pChar->SetWorldPosition( vNewPos );
	}
}

void ioAreaWeapon::ProcessAreaWeaponMove( ioPlayStage *pStage )
{
	if( m_AreaWeaponState == AWS_READY )
		return;

	if( !pStage ) return;
	if( m_CreateType != CT_NORMAL && m_CreateType != CT_MAP && m_CreateType != CT_STRUCT )
		return;
	if( m_fAreaMoveSpeed <= 0.0f ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bUseLiveArea && !CheckLiveAreaInAreaWeapon( pStage ) )
	{
		if( m_dwLiveAreaEndTime == 0 )
			m_dwLiveAreaEndTime = dwCurTime + m_dwLiveAreaTime;
	}

	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurMoveSpeed = m_fAreaMoveSpeed * fTimePerSec;

	switch( m_AreaMoveType )
	{
	case AMT_NORMAL:
		{
			CheckChangeMove( pStage );
		}
		break;
	case AMT_OWNER:
		if( pOwner )
		{
			m_qtRotate = pOwner->GetTargetRot();
			m_vAreaMoveDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
		break;
	case AMT_TRACE:
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		if( !m_bUsedTrace && m_TraceTarget.IsEmpty() )
			CheckChangeMove( pStage );
		break;
	case AMT_OWNER_ROT:
		CheckOwnerRot( pOwner, pStage, fTimePerSec );
		break;
	}

	if( m_bUsedTrace && m_TraceTarget.IsEmpty() &&
		(m_dwLostTraceTime+m_dwNotFindWaitTime) < dwCurTime )
	{
		SetReposition( pStage );
		return;
	}

	D3DXVECTOR3 vMoveAmt = m_vAreaMoveDir * fCurMoveSpeed;
	m_fPointToPointTotalMoveAmount += D3DXVec3Length(&vMoveAmt);

	m_vPreStartPos = m_vStartPos;
	m_vStartPos += vMoveAmt;

	ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
	if( pEff )
	{
		pEff->SetWorldPosition( m_vStartPos );
		pEff->SetWorldOrientation( m_qtRotate );
	}

	if( pOwner && m_dwWeaponBaseIndex > 0 )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
		ioAreaEffectWeapon *pAreaEffectWeapon = ToAreaEffectWeapon(pWeapon);
		if( pAreaEffectWeapon )
		{
			pAreaEffectWeapon->SetPosition( m_vStartPos );
			pAreaEffectWeapon->SetCollisionDir( m_vAreaMoveDir );
		}
	}

	CheckTargetPosition();
}

void ioAreaWeapon::CreateAreaAttack( ioPlayStage *pStage )
{
	if( m_bUseRandomAttack )
		return;

	switch( m_AreaType )
	{
	case AT_INOUT:
	case AT_ATTACK:
		return;
	}

	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( !pOwner )
	{
		m_dwWeaponBaseIndex = 0;
		return;
	}

	if( m_WeaponInfo.m_iWeaponIdx == 0 )
	{
		m_dwWeaponBaseIndex = 0;
		return;
	}

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	D3DXVECTOR3	vMoveDir = m_vAreaMoveDir;
	ioWeapon *pWeapon = pOwner->AreaFireExplicit( m_WeaponInfo, m_dwWeaponBaseIndex, m_vStartPos, vMoveDir, m_dwWeaponDuration );
	ioAreaEffectWeapon *pAreaEffectWeapon = ToAreaEffectWeapon(pWeapon);
	if( pAreaEffectWeapon )
	{
		pAreaEffectWeapon->SetPosition( m_vStartPos );
		pAreaEffectWeapon->SetCollisionDir( m_vAreaMoveDir );
	}
	else
	{
		m_dwWeaponBaseIndex = 0;
		return;
	}
}

void ioAreaWeapon::CheckAreaAttack( ioPlayStage *pStage, bool bRefresh )
{
	if( m_AreaType == AT_INOUT )
		return;

	switch( m_AreaType )
	{
	case AT_INOUT:
	case AT_ATTACK:
		return;
	}

	if( m_AreaWeaponState != AWS_ACTION )
		return;

	if( m_dwWeaponBaseIndex == 0 )
		return;

	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( pOwner )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponBaseIndex );
		ioAreaEffectWeapon *pAreaEffectWeapon = ToAreaEffectWeapon(pWeapon);
		if( pAreaEffectWeapon )
		{
			m_qtRotate = pOwner->GetTargetRot();
			m_vAreaMoveDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			pAreaEffectWeapon->SetPosition( m_vStartPos );
			pAreaEffectWeapon->UpdateCollisionList( m_vNewTargetList, bRefresh );
			pAreaEffectWeapon->SetCollisionDir( m_vAreaMoveDir );
		}
	}
}

void ioAreaWeapon::CheckAreaTargetTeleport( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	if( m_vCurTargetList.empty() )
		return;
	
	std::vector<D3DXVECTOR3> vTargetTeleportPosList;
	ioHashStringVec szTargetList;
	
	int iCurTargetCnt = 0;
	szTargetList.clear();
	vTargetTeleportPosList.clear();

	ioHashStringVec::iterator iter = m_vCurTargetList.begin();
	float fCurAngle = m_fAreaAngle;

	while( iter != m_vCurTargetList.end() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar((*iter));
		if( pTarget )
		{
			D3DXVECTOR3 vPoint;
			if( pTarget->CheckSpherePoint( m_vStartPos, m_fDropEnableRange, vPoint ) )
			{
				D3DXVECTOR3 vDir = vPoint - m_vStartPos;
				D3DXVec3Normalize( &vDir, &vDir );

				float fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );
				if( D3DXVec3Dot( &m_vAreaMoveDir, &vDir ) >= fHalfCosine )
				{					
					if( !szTargetIdentifyBuffName.IsEmpty() && pTarget->HasBuff( szTargetIdentifyBuffName ) )
					{
						D3DXVECTOR3 vPos = m_vStartPos;
						vPos.y += m_fDropHeight;
						pTarget->SetWorldPosition( vPos );

						D3DXVECTOR3 vWarpPos = pTarget->GetEnableWarpPosition( pTarget, 
							360,
							m_fEnablePositionRange,
							m_fEnableMaxPositionRange,
							true,
							false,
							0.0f );
						pTarget->SetWorldPosition( vWarpPos );

						szTargetList.push_back( pTarget->GetCharName() );
						vTargetTeleportPosList.push_back( vWarpPos );
						iCurTargetCnt++;

						ioHashStringVec vBuffList;
						ioHashStringVec::iterator iter = m_WeaponEndBuffList.begin();
						for( ; iter != m_WeaponEndBuffList.end(); ++iter )
						{
							ioHashString szBuff = (*iter);
							if( !szBuff.IsEmpty() )
							{
								ioBuff* pBuff = pTarget->AddNewBuff(szBuff, pOwner->GetCharName(), m_OwnerItem, NULL );
								if( pBuff )
								{
									vBuffList.push_back( szBuff );
								}
							}
						}

						int iCurBuffCnt = vBuffList.size();

						if( pTarget->IsNeedSendNetwork() && iCurBuffCnt > 0 )
						{
							SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
							kPacket << pTarget->GetCharName();
							kPacket << pOwner->GetCharName();
							kPacket << m_OwnerItem;
							kPacket << pTarget->GetRandomSeed();
							kPacket << false;					// 타격횟수 체크
							kPacket << 0.0f;
							kPacket << 0.0f;
							kPacket << 0.0f;
							kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
							kPacket << iCurBuffCnt;

							for( int i=0; i < iCurBuffCnt; i++ )
							{				
								kPacket << vBuffList[i];
							}

							kPacket << false;					// Use Force

							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
					}
				}
			}
		}
		iter++;
	}
	
	if( pOwner->IsNeedSendNetwork() && 0 < iCurTargetCnt )
	{
		SP2Packet kPacket( CUPK_AREAWEAPON );
		kPacket << AREAWEAPON_SUB_INFO;
		kPacket << m_dwAreaWeaponMapIndex;
		kPacket << m_dwAreaWeaponIndex;
		kPacket << ASST_POINT_TO_POINT;
		kPacket << iCurTargetCnt;
		for( int i = 0; i < iCurTargetCnt; ++i )
		{
			kPacket << szTargetList[i];
			kPacket << vTargetTeleportPosList[i];
		}

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAreaWeapon::CheckChangeMove( ioPlayStage *pStage )
{
	if( m_ChangeMoveInfoList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ChangeMoveInfoList::iterator iter = m_ChangeMoveInfoList.begin();

	int iChangeCnt = m_ChangeMoveInfoList.size();

	int iCnt = 0;
	bool bOver = false;
	while( iter != m_ChangeMoveInfoList.end() )
	{
		DWORD dwChangeTime = (*iter).m_dwChangeTime + m_dwChangeStartTime;
		if( COMPARE( dwChangeTime, m_dwChangeCheckTime, dwCurTime ) )
		{
			m_dwChangeCheckTime = dwCurTime;
			RotateMoveDir( (*iter).m_fChangeAngleH, (*iter).m_fChangeAngleV, pStage );
			return;
		}

		++iter;
		++iCnt;

		if( m_dwChangeCheckTime > 0 && iCnt == iChangeCnt && dwChangeTime < m_dwChangeCheckTime )
			bOver = true;
	}

	if( m_AreaMoveType == AMT_TRACE && bOver && m_bOneTimeUse )
	{
		m_dwChangeStartTime = dwCurTime;
		m_dwChangeCheckTime = 0;
	}

	return;
}

void ioAreaWeapon::CheckChangeMove( ioPlayStage *pStage, DWORD dwGapTime )
{
	if( m_ChangeMoveInfoList.empty() ) return;

	DWORD dwCurTime = m_dwPreFrameTime + dwGapTime;
	ChangeMoveInfoList::iterator iter = m_ChangeMoveInfoList.begin();

	int iChangeCnt = m_ChangeMoveInfoList.size();

	int iCnt = 0;
	bool bOver = false;
	while( iter != m_ChangeMoveInfoList.end() )
	{
		DWORD dwChangeTime = (*iter).m_dwChangeTime + m_dwChangeStartTime;
		if( COMPARE( dwChangeTime, m_dwChangeCheckTime, dwCurTime ) )
		{
			m_dwChangeCheckTime = dwCurTime;
			RotateMoveDir( (*iter).m_fChangeAngleH, (*iter).m_fChangeAngleV, pStage );
			return;
		}

		++iter;
		++iCnt;

		if( m_dwChangeCheckTime > 0 && iCnt == iChangeCnt && dwChangeTime < m_dwChangeCheckTime )
			bOver = true;
	}

	if( m_AreaMoveType == AMT_TRACE && bOver && m_bOneTimeUse )
	{
		m_dwChangeStartTime = dwCurTime;
		m_dwChangeCheckTime = 0;
	}

	return;
}

void ioAreaWeapon::RotateMoveDir( float fAngleH, float fAngleV, ioPlayStage *pStage )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vPreDir = m_vAreaMoveDir;

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vAreaMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	m_vAreaMoveDir = qtRot * m_vAreaMoveDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	m_vAreaMoveDir = qtRot * m_vAreaMoveDir;

	// Test용
	/*
	D3DXVec3Cross( &vAxis, &vPreDir, &m_vAreaMoveDir );
	D3DXVec3Normalize( &vAxis, &-vAxis );

	if( vAxis != ioMath::VEC3_ZERO )
	{
		float fCos = D3DXVec3Dot( &vPreDir, &m_vAreaMoveDir );
		float fGapAngle = (float)acos( fCos );
		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, fGapAngle );

		D3DXQuaternionMultiply( &m_qtRotate, &m_qtRotate, &qtRot );
	}
	*/

	ioMath::CalcDirectionQuaternion( qtRot, -m_vAreaMoveDir );
	m_qtRotate = qtRot;
}

bool ioAreaWeapon::CheckIgnoreProtectBuff()
{
	int iCnt = m_vEnemyBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vEnemyBuffList[i].m_BuffName;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = g_BuffMaker.CreateBuff( szBuffName );
			if( !pBuff )
				continue;

			if( pBuff->IsIgnoreProtectBuff() )
			{
				delete pBuff;
				return true;
			}

			delete pBuff;
		}
	}

	return false;
}

void ioAreaWeapon::FindTraceTarget( ioPlayStage *pStage )
{
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( !m_bUsedTrace && m_dwSencingStartTime + m_dwSencingWaitTime >= dwCurTime )
		return;
	
	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( !pOwner )
		return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	// Search 범위에 타겟 가능한 대상 검색
	ioBaseChar *pTarget = NULL;
	float fCurTargeRange = std::numeric_limits<float>::infinity();
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();

	ioHashString szTarget;
	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar )
			continue;

		if ( m_bTraceTargetCheckTeam )
		{
			if ( pChar->GetTeam() == pOwner->GetTeam() )
				continue;
		}

		if( pChar->HasHideBuff() )
			continue;

		if( pChar->GetExperienceMode() == EMS_EXPERIENCE )
			continue;
				
		if( pChar->IsSystemState() )		
			continue;

		if( m_bUseLiveArea && !CheckLiveAreaInChar( pChar, pStage ) )
			continue;

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
				continue;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pChar->IsApplyDownState(false) )
				continue;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( pChar->IsApplyDownState() )
				continue;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUNDED )
		{
			if( pChar->IsFloating() )
				continue;

			/*
			if( !pChar->CheckTargetAllWoundedState() )
				continue;
			*/
		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pChar->IsFloating() )
				continue;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( !pChar->IsApplyDownState() || pChar->IsFloating() )
				continue;
		}
		else if ( m_TargetWoundType == TWT_WOUNDED ) 
		{
			if ( !pChar->IsWoundedState() )
				continue;
		}

		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		D3DXVECTOR3 vDiff = vPos - m_vStartPos;

		float fCurRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fCurRangeSq < m_fTargetSearchRange*m_fTargetSearchRange )
		{
			if( fCurRangeSq < fCurTargeRange )
			{
				fCurTargeRange = fCurRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	if( !szTarget.IsEmpty() && szTarget != m_TraceTarget )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetName = szTarget;

			m_bUsedTrace = true;
			m_TraceTarget = pTarget->GetCharName();

			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDiff = vPos - m_vStartPos;

			D3DXVec3Normalize( &vDiff, &vDiff );
			D3DXVECTOR3 vPreDir = m_vAreaMoveDir;
			m_vAreaMoveDir = vDiff;

			// Test용
			/*
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vPreDir, &m_vAreaMoveDir );
			D3DXVec3Normalize( &vAxis, &-vAxis );
			
			if( vAxis != ioMath::VEC3_ZERO )
			{
				float fCos = D3DXVec3Dot( &vPreDir, &m_vAreaMoveDir );
				float fGapAngle = (float)acos( fCos );
				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, fGapAngle );

				D3DXQuaternionMultiply( &m_qtRotate, &m_qtRotate, &qtRot );
			}
			*/

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vAreaMoveDir );

			m_qtRotate = qtRot;

			if( pTarget->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_AREAWEAPON );
				kPacket << AREAWEAPON_MOVE_INFO;
				kPacket << m_dwAreaWeaponMapIndex;
				kPacket << m_dwAreaWeaponIndex;
				kPacket << AUT_NONE;
				kPacket << true;				// Target
				kPacket << m_TraceTarget;
				kPacket << m_vStartPos;
				kPacket << m_vAreaMoveDir;
				kPacket << m_qtRotate;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

bool ioAreaWeapon::CheckTraceTarget( ioPlayStage *pStage )
{
	if( !pStage ) return false;
	if( m_TraceTarget.IsEmpty() ) return false;

	bool bLost = false;
	ioBaseChar *pTraceTarget = pStage->GetBaseChar( m_TraceTarget );
	if( pTraceTarget )
	{
		if( pTraceTarget->IsNeedProcess() )
		{
			if( pTraceTarget->IsSystemState() )
				pTraceTarget = NULL;

			if( pTraceTarget && pTraceTarget->HasHideBuff() )
				pTraceTarget = NULL;

			// 타겟 상태 체크
			if( pTraceTarget )
			{
				if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
				{
					if( pTraceTarget->IsApplyDownState() && !pTraceTarget->IsApplyDownState(false) )
						pTraceTarget = NULL;
				}
				else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
				{
					if( pTraceTarget->IsApplyDownState(false) )
						pTraceTarget = NULL;
				}
				else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
				{
					if( pTraceTarget->IsApplyDownState() )
						pTraceTarget = NULL;
				}
				else if( m_TargetWoundType == TWT_ALL_WOUNDED )
				{
					if( pTraceTarget->IsFloating() )
						pTraceTarget = NULL;

					/*
					if( !pTraceTarget->CheckTargetAllWoundedState() )
						pTraceTarget = NULL;
					*/
				}
				else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
				{
					if( !pTraceTarget->IsFloating() )
						pTraceTarget = NULL;
				}
				else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
				{
					if( !pTraceTarget->IsApplyDownState() || pTraceTarget->IsFloating() )
						pTraceTarget = NULL;
				}
				else if ( m_TargetWoundType == TWT_WOUNDED )
				{
					if ( !pTraceTarget->IsWoundedState() )
						pTraceTarget = NULL;
				}
			}

			// 타겟 거리 체크
			if( pTraceTarget )
			{
				D3DXVECTOR3 vPos = pTraceTarget->GetMidPositionByRate();
				D3DXVECTOR3 vDiff = vPos - m_vStartPos;

				float fCurRangeSq = D3DXVec3LengthSq( &vDiff );
				if( fCurRangeSq > m_fTargetTraceRange*m_fTargetTraceRange )
					pTraceTarget = NULL;
			}

			// 유효하면 중단
			if( pTraceTarget )
			{
				D3DXVECTOR3 vPos = pTraceTarget->GetMidPositionByRate();
				D3DXVECTOR3 vDiff = vPos - m_vStartPos;

				D3DXVec3Normalize( &vDiff, &vDiff );
				D3DXVECTOR3 vPreDir = m_vAreaMoveDir;
				m_vAreaMoveDir = vDiff;

				// Test용
				/*
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &vPreDir, &m_vAreaMoveDir );
				D3DXVec3Normalize( &vAxis, &-vAxis );

				if( vAxis != ioMath::VEC3_ZERO )
				{
					float fCos = D3DXVec3Dot( &vPreDir, &m_vAreaMoveDir );
					float fGapAngle = (float)acos( fCos );
					D3DXQUATERNION qtRot;
					D3DXQuaternionRotationAxis( &qtRot, &vAxis, fGapAngle );

					D3DXQuaternionMultiply( &m_qtRotate, &m_qtRotate, &qtRot );
				}
				*/

				D3DXQUATERNION qtRot;
				ioMath::CalcDirectionQuaternion( qtRot, -m_vAreaMoveDir );

				m_qtRotate = qtRot;

				return true;
			}

			m_TraceTarget.Clear();
			bLost = true;
		}
		else
		{
			D3DXVECTOR3 vPos = pTraceTarget->GetMidPositionByRate();
			D3DXVECTOR3 vDiff = vPos - m_vStartPos;

			D3DXVec3Normalize( &vDiff, &vDiff );
			D3DXVECTOR3 vPreDir = m_vAreaMoveDir;
			m_vAreaMoveDir = vDiff;

			// Test용
			/*
			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vPreDir, &m_vAreaMoveDir );
			D3DXVec3Normalize( &vAxis, &-vAxis );

			if( vAxis != ioMath::VEC3_ZERO )
			{
				float fCos = D3DXVec3Dot( &vPreDir, &m_vAreaMoveDir );
				float fGapAngle = (float)acos( fCos );
				D3DXQUATERNION qtRot;
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, fGapAngle );

				D3DXQuaternionMultiply( &m_qtRotate, &m_qtRotate, &qtRot );
			}
			*/

			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vAreaMoveDir );

			m_qtRotate = qtRot;

			return true;
		}
	}
	else
	{
		m_TraceTarget.Clear();
	}

	if( bLost )
	{
		m_dwLostTraceTime = FRAMEGETTIME();

		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_AREAWEAPON );
			kPacket << AREAWEAPON_MOVE_INFO;
			kPacket << m_dwAreaWeaponMapIndex;
			kPacket << m_dwAreaWeaponIndex;
			kPacket << AUT_NONE;
			kPacket << false;				// Target
			kPacket << m_vStartPos;
			kPacket << m_vAreaMoveDir;
			kPacket << m_qtRotate;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return false;
}

void ioAreaWeapon::ApplyMoveInfo( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int eUsedType;
	rkPacket >> eUsedType;

	switch( eUsedType )
	{
	case AUT_NONE:
		{
			bool bTarget;
			rkPacket >> bTarget;

			if( bTarget )
			{
				rkPacket >> m_TraceTarget;

				m_bUsedTrace = true;
			}
			else
			{
				m_TraceTarget.Clear();
				m_dwLostTraceTime = FRAMEGETTIME();
			}
		}
		break;
	case AUT_USED:
		m_bUsedAreaWeapon = true;
		break;
	}

	rkPacket >> m_vStartPos;
	rkPacket >> m_vAreaMoveDir;
	rkPacket >> m_qtRotate;

	if( pStage )
	{
		ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
		if( pEff )
		{
			pEff->SetWorldPosition( m_vStartPos );
			pEff->SetWorldOrientation( m_qtRotate );
		}
	}
}

void ioAreaWeapon::ApplySubInfo( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iSubType;
	rkPacket >> iSubType;

	switch( iSubType )
	{
	case ASST_POINT_TO_POINT:
		{		
			int iCurTargetCnt;
			rkPacket >> iCurTargetCnt;

			for( int i = 0; i < iCurTargetCnt; ++i )
			{
				ioHashString szTargetName;
				rkPacket << szTargetName;
				if( !szTargetName.IsEmpty() )
				{
					ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
					if( pTarget )
					{	
						D3DXVECTOR3 vPos;
						rkPacket >> vPos;
						pTarget->SetWorldPosition(vPos);
					}
				}				
			}
		}
	}
}

bool ioAreaWeapon::CheckLiveAreaInChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return false;
	if( !pStage ) return false;

	D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( m_vMinLivePos, m_vMaxLivePos );

	D3DXVECTOR3 vCollisionPoint;
	if( pChar->IsWoundedCollision( kBuffBox ) )
		return true;

	return false;
}

bool ioAreaWeapon::CheckLiveAreaInAreaWeapon( ioPlayStage *pStage )
{
	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( m_vMinLivePos, m_vMaxLivePos );

	// 수평 체크
	if( !COMPARE( m_vStartPos.x, m_vMinLivePos.x, m_vMaxLivePos.z ) )
		return false;

	// 수직 체크
	if( !COMPARE( m_vStartPos.y, m_vMinLivePos.y, m_vMaxLivePos.y ) )
		return false;

	// 깊이 체크
	if( !COMPARE( m_vStartPos.z, m_vMinLivePos.z, m_vMaxLivePos.z ) )
		return false;

	return true;
}

void ioAreaWeapon::SetUsedAreaWeapon( bool bUsed)
{
	m_bUsedAreaWeapon = bUsed;
}

void ioAreaWeapon::SetUsedTrace( bool bUsed )
{
	m_bUsedTrace = bUsed;
}

void ioAreaWeapon::SetAreaWeaponMapIndex( DWORD dwIndex )
{
	m_dwAreaWeaponMapIndex = dwIndex;
}

void ioAreaWeapon::SetTraceTarget( const ioHashString &szName )
{
	m_TraceTarget = szName;
}

void ioAreaWeapon::SetReposition( ioPlayStage *pStage )
{
	SetAreaPosition( m_vFirstPos, pStage );
	SetAreaRotation( m_qtFirstRotate );

	m_vAreaMoveDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_bUsedAreaWeapon = false;
	m_bUsedTrace = false;

	DWORD dwCurTime = FRAMEGETTIME();
	m_TraceTarget.Clear();
	m_dwChangeStartTime = dwCurTime;
	m_dwSencingStartTime = dwCurTime;
	m_dwChangeCheckTime = 0;

	m_vCurTargetList.clear();
	m_vNewTargetList.clear();
	m_vCurBlockList.clear();
}

void ioAreaWeapon::SetCreateWeaponIndex( DWORD dwIndex )
{
	m_dwCreateWeaponIndex = dwIndex;
}

void ioAreaWeapon::CheckOwnerRot( ioBaseChar *pChar, ioPlayStage *pStage, float fTimePerSec )
{
	if( m_dwRotateMaxTime == 0 ) return;

	if( m_bFirstMove )
	{
		m_bFirstMove = false;

		D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		m_vAreaMoveDir = vTargetDir;
		
		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_qtRotate = qtTargetRot;
		return;
	}

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateMaxTime;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vCurMoveDir = m_vAreaMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	D3DXQUATERNION	qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vTargetDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = (float)D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_vAreaMoveDir = qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &m_vAreaMoveDir, &m_vAreaMoveDir );

		m_qtRotate = qtTargetRot;
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		D3DXVECTOR3 vTmpAxis;

		if( vCurMoveDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_Y, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}
		else
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_X, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}

		D3DXVec3Cross( &vAxis, &vTmpAxis, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vCurMoveDir, &vTargetDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}

	float fCurRotateAngle = fRotateAngle;
	if( fGapAngle < fRotateAngle )
		fCurRotateAngle = fGapAngle;

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vAreaMoveDir = vNewDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	m_qtRotate = qtNewRot;
}

void ioAreaWeapon::GenerateRandomAttack( ioBaseChar *pChar )
{
	if( !m_bUseRandomAttack )
		return;

	if( !pChar )
		return;

	m_vGenerateList.clear();
	m_dwGenerateStartTime = FRAMEGETTIME();
	m_dwNextGenerateTime = m_dwGenerateStartTime;

	DWORD dwNextTime = m_dwGenerateStartTime;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	m_dwGenerateSeed = pChar->GetRandomSeed();

	IORandom random;
	random.SetRandomSeed( m_dwGenerateSeed );
	
	m_GeneratePos = m_vStartPos;

	DWORD dwEndTime = m_dwCreatedTime+m_dwReadyTime+GetLiveTime();
	int iCurCnt = 0;

	while ( dwNextTime <= dwEndTime )
	{
		//Generate Pos
		GenerateInfo eGenerateInfo;
		D3DXVECTOR3 vPos = m_GeneratePos;

		if( m_RandomGenerateFieldType == RFT_CIRCLE )
		{
			int iAngle = random.Random( 361 );
			int iRange = (int)(m_fCollisionRange);
			iRange = random.Random(iRange);

			int iXPos = cos((double) iAngle ) * iRange;
			int iZPos = sin((double) iAngle ) * iRange;

			vPos.x += iXPos;
			vPos.z += iZPos;
		}
		else if( m_RandomGenerateFieldType == RFT_RECTANGLE )
		{
			int iValue = m_fCollisionRange * 2;
			int iWidth1 = random.Random( iValue );
			int iWidth2 = random.Random( iValue );
			iWidth1 -= static_cast<int>( m_fCollisionRange );
			iWidth2 -= static_cast<int>( m_fCollisionRange );
			vPos.x += iWidth1;
			vPos.z += iWidth2;
		}
		vPos.y += m_fGenerateHeight;

		//Generate NextTime
		DWORD dwGapTime = 0;
		dwGapTime = m_dwMinGenerateTime;
		dwGapTime += random.Random(m_dwMaxGenerateTime - m_dwMinGenerateTime);

		dwNextTime += dwGapTime;

		eGenerateInfo.vPos = vPos;
		eGenerateInfo.dwGapTime = dwGapTime;
		eGenerateInfo.iAttributeIndex = iCurCnt;

		iCurCnt++;

		m_vGenerateList.push_back( eGenerateInfo );
	}
}

void ioAreaWeapon::ProcessRandomAttack( ioBaseChar *pChar )
{
	if( !m_bUseRandomAttack )
		return;
	if( m_vGenerateList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	GenerateList::iterator iter = m_vGenerateList.begin();
	while ( iter != m_vGenerateList.end() )
	{
		if( m_dwNextGenerateTime > 0 && m_dwNextGenerateTime <= dwCurTime )
		{
			DWORD dwWeaponIndex = m_dwWeaponBaseIndex + (*iter).iAttributeIndex;
			D3DXVECTOR3 vPos;
			if( !m_bUseRandomAttackFixedStartPos )
				vPos = m_vStartPos - m_GeneratePos + (*iter).vPos;
			else
				vPos = (*iter).vPos;
			
			DWORD dwGapTime = (*iter).dwGapTime;

			D3DXVECTOR3	vMoveDir = m_vAreaMoveDir;
			switch( m_AreaType )
			{
			case AT_ATTACK:
				vMoveDir = m_vTargetAttPos - vPos;
				D3DXVec3Normalize( &vMoveDir, &vMoveDir );
				break;
			}

			switch( m_VertDirType )
			{
			case VDT_UP:
				vMoveDir = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
				D3DXVec3Normalize( &vMoveDir, &vMoveDir );
				break;
			case VDT_DOWN:
				vMoveDir = D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f );
				D3DXVec3Normalize( &vMoveDir, &vMoveDir );
				break;
			}

			ioWeapon *pWeapon = pChar->AreaFireExplicit( m_WeaponInfo, dwWeaponIndex, vPos, vMoveDir, m_dwWeaponDuration );
			
			iter = m_vGenerateList.erase( iter );

			if( iter != m_vGenerateList.end() )
			{
				m_dwNextGenerateTime += dwGapTime;
			}
		}
		else
			break;
	}
}

void ioAreaWeapon::UpdateTargetListByDouble( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	BaseCharList::iterator iter = m_vNewTargetList.begin();

	while( iter != m_vNewTargetList.end() )
	{
		ioBaseChar *pChar = (*iter);
		if( !pChar )
		{
			iter = m_vNewTargetList.erase(iter);
			continue;
		}

		if( FindTarget( pChar->GetCharName() ) )
		{
			iter = m_vNewTargetList.erase(iter);
			continue;
		}

		iter++;
	}
}

void ioAreaWeapon::CheckAreaWeaponBuffByDouble( ioPlayStage *pStage )
{
	if( m_AreaType != AT_DOUBLE )
		return;

	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	int iNewCnt = m_vNewTargetList.size();
	int iTeamUserCnt = 0;
	for( int i=0; i < iNewCnt; ++i )
	{
		ioBaseChar *pChar = m_vNewTargetList[i];
		if( !pChar ) continue;
		if( !pChar->IsNeedProcess() ) continue;

		bool bInRange = CheckDoubleRange( pChar );

		if( m_CreateType != CT_MAP && pChar->GetCharName() == m_OwnerName && m_TeamType == pChar->GetTeam() )
		{
			if( bInRange )
			{
				if( !IsCollisioned(pChar, TT_OWNER) && SetOwnerBuffList( pChar ) )
					m_vCurTargetList.push_back( pChar->GetCharName() );
			}
			else
			{
				if( !IsCollisioned(pChar, TT_OWNER) && SetOwnerBuffList2( pChar ) )
					m_vCurTargetList.push_back( pChar->GetCharName() );
			}
		}
		else
		{
			bool bEnableTeamAttack = pChar->IsEnableTeamAttack();

			if( bInRange )
			{
				iTeamUserCnt++;
				if( m_CreateType != CT_MAP && m_TeamType == pChar->GetTeam() && SetTeamBuffList( pOwner, pChar, iTeamUserCnt ) )
				{
					if( pOwner && m_bEnableAttackCnt )
						pOwner->CheckTeamBuffAttackCnt( pChar );

					m_vCurTargetList.push_back( pChar->GetCharName() );
				}

				if( (m_CreateType == CT_MAP || m_TeamType != pChar->GetTeam() || bEnableTeamAttack) && SetEnemyBuffList( pOwner, pChar, pStage ) )
				{
					if( pOwner && m_bEnableAttackCnt )
					{
						//pOwner->CheckRealComboHit( pChar->GetCharName() );
						pOwner->CheckComboSkillHit();
						pOwner->CheckAttackCnt( pChar );
					}

					m_vCurTargetList.push_back( pChar->GetCharName() );
				}
			}
			else
			{
				if( m_CreateType != CT_MAP && m_TeamType == pChar->GetTeam() && SetTeamBuffList2( pOwner, pChar ) )
				{
					if( pOwner && m_bEnableAttackCnt )
						pOwner->CheckTeamBuffAttackCnt( pChar );

					m_vCurTargetList.push_back( pChar->GetCharName() );
				}

				if( (m_CreateType == CT_MAP || m_TeamType != pChar->GetTeam() || bEnableTeamAttack) && SetEnemyBuffList2( pOwner, pChar, pStage ) )
				{
					if( pOwner && m_bEnableAttackCnt )
					{
						//pOwner->CheckRealComboHit( pChar->GetCharName() );
						pOwner->CheckComboSkillHit();
						pOwner->CheckAttackCnt( pChar );
					}

					m_vCurTargetList.push_back( pChar->GetCharName() );
				}
			}
		}
	}
}

void ioAreaWeapon::CheckAreaWeaponBuffOnCreateWeapon( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( m_CreateType != CT_MAP && !m_OwnerName.IsEmpty() && !pOwner ) return;

	switch( m_AreaType )
	{
	case AT_ATTACK:
		return;
	}

	int iNewCnt = m_vNewTargetList.size();
	for( int i=0; i < iNewCnt; ++i )
	{
		ioBaseChar *pChar = m_vNewTargetList[i];
		if( !pChar ) continue;
		if( !pChar->IsNeedProcess() ) continue;

		if( m_TeamType == pChar->GetTeam() )
		{
			SetTeamBuffOnCreateWeaponList( pOwner, pChar );
		}
	}
}

void ioAreaWeapon::FindAndDestroyDummy( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioBaseChar *pChar = pStage->GetBaseChar( m_OwnerName );
	if( !pChar )
		return;

	TargetDummyList vTargetDummyList;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	float fColRange = m_fCollisionRange;
	float fAngle = m_fAreaAngle;
	pChar->GetTargetDummyListBySphere( vPos, fColRange, fAngle, vTargetDummyList );

	if ( vTargetDummyList.empty() )
		return;

	TargetDummyList::iterator iter = vTargetDummyList.begin();
	for ( ; iter != vTargetDummyList.end() ; iter++ )
	{
		ioDummyChar *pDummy = *iter;

		ioBaseChar *pDummyOwner = pDummy->GetOwner();

		if ( !pDummyOwner )
			continue;

		//같은 팀이면 패스
		if ( pChar->GetTeam() == pDummyOwner->GetTeam() )
			continue;

		if ( !IsPassDummy( pDummy ) )
		{
			pDummy->SetDieState( true );
		}
	}
}

bool ioAreaWeapon::IsPassDummy( ioDummyChar* pDummy )
{
	int nPassDummyCnt = (int)m_vPassDummyList.size();
	for ( int i=0; i<nPassDummyCnt; i++ )
	{
		if ( m_vPassDummyList[i] == pDummy->GetDummyCharName() )
			return true;
	}
	return false;
}

void ioAreaWeapon::FindAndWeapon( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioBaseChar *pChar = pStage->GetBaseChar( m_OwnerName );
	if( !pChar )
		return;

	TargetWeaponList vTargetWeaponList;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();

	float fColRange = m_fCollisionRange;
	float fAngle = m_fAreaAngle;
	pChar->GetTargetWeaponListBySphere( vPos, fColRange, fAngle, vTargetWeaponList );

	if ( vTargetWeaponList.empty() )
		return;

	TargetWeaponList::iterator iter = vTargetWeaponList.begin();
	for ( ; iter != vTargetWeaponList.end() ; iter++ )
	{
		ioWeapon *pWeapon = *iter;

		ioBaseChar *pWeaponOwner = pWeapon->GetOwner();
		if ( !pWeaponOwner )
			continue;

		if ( pChar->GetTeam() == pWeaponOwner->GetTeam() )
			continue;

		if( IsCorrectWeapon( pWeapon ) )
			pWeapon->SetWeaponDead();
	}
}

bool ioAreaWeapon::IsCorrectWeapon( ioWeapon *pWeapon )
{
	int nDestroyWeapon = (int)m_vDestroyWeaponList.size();
	for ( int i=0; i<nDestroyWeapon; i++ )
	{
		if ( m_vDestroyWeaponList[i] == pWeapon->GetType() )
			return true;
	}
	return false;
}

void ioAreaWeapon::CheckDestroy( ioPlayStage *pStage )
{
	if( m_AreaType != AT_DESTROY )
		return;

	if( m_AreaWeaponState != AWS_ACTION )
		return;

	FindAndDestroyDummy( pStage );
	FindAndWeapon( pStage );
}

bool ioAreaWeapon::CheckDoubleRange( ioBaseChar *pChar )
{
	if( !pChar )
		return true;

	float fCurInRange = m_fDoubleInRange;
	fCurInRange = max( 0.0f, min(fCurInRange, m_fCollisionRange) );

	D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - GetStartPos();

	float fInRangeSq = fCurInRange * fCurInRange;
	float fDistanceSq = 0.0f;

	switch( m_TargetColType )
	{
	case TCT_SPHERE:
		fDistanceSq = D3DXVec3LengthSq( &vDiff );
		break;
	case TCT_CYLINDER:
		{
			D3DXVECTOR3 vNewDiff = vDiff;
			vNewDiff.y = 0.0f;

			fDistanceSq = D3DXVec3LengthSq( &vNewDiff );
		}
		break;
	}

	if( fDistanceSq > fInRangeSq )
		return false;

	return true;
}

bool ioAreaWeapon::SetOwnerBuffList2( ioBaseChar *pChar )
{
	if( !pChar->IsNeedProcess() )
		return false;
	if( m_vOwnerBuffList2.empty() )
		return false;

	if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	int iCnt = m_vOwnerBuffList2.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_vOwnerBuffList2[i];

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pChar->GetCharName(), m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioAreaWeapon::SetTeamBuffList2( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( m_vTeamBuffList2.empty() )
		return false;

	if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szCreator;
	if( pOwner )
		szCreator = pOwner->GetCharName();

	int iCnt = m_vTeamBuffList2.size();
	for( int i = 0 ; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vTeamBuffList2[i];

		//오브젝트 아이템 장착 버프가 AreaWeapon을 통해서 생성되는 경우에 먼저 현재 장착되어있는 오브젝트 아이템에 대해서 체크
		if( !CheckTargetEquipObjectBuff( szBuffName, pChar ) )
			return false;
	}

	for( int i = 0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vTeamBuffList2[i];

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( szCreator, GetOwnerSkillName() );
				}

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << szCreator;
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();
			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;
			kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << false;					// Use Force

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	return bResult;
}

bool ioAreaWeapon::SetEnemyBuffList2( ioBaseChar *pOwner, ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bOneTimeUse && m_bUsedAreaWeapon )
		return false;

	if( m_vEnemyBuffList2.empty() )
		return false;

	if( pChar->IsCanNoWound() )
		return false;

	if( pChar->IsProtectState() && !CheckIgnoreProtectBuff() )
	{
		int iType = pChar->CheckBlockEmoticonType();
		if( iType >= 0 && !IsBlocked( pChar ) )
		{
			pChar->SetProtectWound( iType );
			m_vCurBlockList.push_back( pChar->GetCharName().c_str() );
		}

		return false;
	}

	//
	DWORD dwSeed = pChar->GetRandomSeed();
	DWORD dwNewSeed = timeGetTime();

	pChar->SetRandomSeed( dwNewSeed );
	//

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szCreator;
	if( pOwner )
		szCreator = pOwner->GetCharName();

	int iCnt = m_vEnemyBuffList2.size();
	for( int i = 0 ; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vEnemyBuffList2[i];

		//오브젝트 아이템 장착 버프가 AreaWeapon을 통해서 생성되는 경우에 먼저 현재 장착되어있는 오브젝트 아이템에 대해서 체크
		if( !CheckTargetEquipObjectBuff( szBuffName, pChar ) )
			return false;
	}

	for( int i = 0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_vEnemyBuffList2[i];

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, m_OwnerItem, NULL, 0 );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( m_fExtraBuffDuration );

				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( szCreator, GetOwnerSkillName() );
				}

				vBuffList.push_back( szBuffName );
				bResult = true;

				ioCloseOrderBuff *pCloseOrder = ToCloseOrderBuff(pBuff);
				if( pCloseOrder && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE )
				{
					pCloseOrder->SetTargetAreaWeapon( m_dwAreaWeaponIndex );
				}

				ioPullBuff *pPullBuff = ToPullBuff(pBuff);
				if( pPullBuff )
				{
					pPullBuff->SetTargetAreaWeapon( m_dwAreaWeaponIndex );
				}
			}
		}
	}

	int iCurBuffCnt = vBuffList.size();
	if(iCurBuffCnt > 0 && bResult )
	{
		if( m_bOneTimeUse )
		{
			m_bUsedAreaWeapon = true;

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_AREAWEAPON );
				kPacket << AREAWEAPON_MOVE_INFO;
				kPacket << m_dwAreaWeaponMapIndex;
				kPacket << m_dwAreaWeaponIndex;
				kPacket << AUT_USED;
				kPacket << m_vStartPos;
				kPacket << m_vAreaMoveDir;
				kPacket << m_qtRotate;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		D3DXVECTOR3 vDir = m_qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		if( m_fAreaForceAmt > 0.0f )
		{
			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, vDir );
			pChar->SetTargetRot( qtCharRot );
			pChar->SetForcePower( vDir, m_fAreaForceAmt, m_fAreaForceFriction, true );
		}

		// TargetMapEffect
		D3DXVECTOR3 vEffectPos = pChar->GetWorldPosition();
		if( pStage && !m_TargetMapEffect.IsEmpty() )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			pStage->CreateMapEffect( m_TargetMapEffect, vEffectPos, vScale );
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << szCreator;
			kPacket << m_OwnerItem;
			kPacket << pChar->GetRandomSeed();

			kPacket << m_bEnableAttackCnt;		// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << m_fExtraBuffDuration;
			kPacket << 0.0f;

			kPacket << COWT_AREAWEAPON;			// Check CloseOrderBuff(12.07.13 수정)
			kPacket << GetOwnerName();
			kPacket << GetAreaWeaponName();

			kPacket << iCurBuffCnt;

			for( i=0; i < iCurBuffCnt; i++ )
			{
				kPacket << vBuffList[i];
			}

			kPacket << true;
			kPacket << m_fAreaForceAmt;
			kPacket << m_fAreaForceFriction;
			kPacket << vDir;

			kPacket << vEffectPos;
			kPacket << m_TargetMapEffect;

			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		// 복구
		pChar->SetRandomSeed( dwSeed );
	}

	return bResult;
}

void ioAreaWeapon::SetTargetAttInfo( const D3DXVECTOR3 &vTargetPos, DWORD dwMaxCnt )
{
	m_vTargetAttPos = vTargetPos;
	m_dwTargetAttMaxCnt = dwMaxCnt;
	m_dwTargetAttCurCnt = 0;
	m_dwCurTargetAttEff = -1;

	ioPlayStage *pStage = g_AreaWeaponMgr.GetPlayStage();
	if( !pStage )
		return;

	ioMapEffect *pMapEffect = NULL;
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szCurEffect;
	int iIndex = m_dwTargetAttMaxCnt - 1;
	if( COMPARE( iIndex, 0, (int)m_TargetAttEffList.size() ) )
		szCurEffect = m_TargetAttEffList[iIndex];
	
	pMapEffect = pStage->CreateMapEffect( szCurEffect, m_vStartPos, vScale );

	if( pMapEffect )
	{
		m_dwCurTargetAttEff = pMapEffect->GetUniqueID();
		pMapEffect->SetWorldOrientation( m_qtRotate );
	}
}

void ioAreaWeapon::ProcessTargetAttack( ioBaseChar *pChar )
{
	if( m_dwTargetAttCurCnt >= m_dwTargetAttMaxCnt )
	{
		m_bLive = false;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	// areaweapon 생성과 동시에 공격이 발생 안하게 했음..
	if( m_dwCheckTargetAttTime == 0 )
	{
		m_dwCheckTargetAttTime = dwCurTime + m_dwTargetAttGap;
		return;
	}

	if( m_dwCheckTargetAttTime > dwCurTime )
		return;

	m_dwCheckTargetAttTime = dwCurTime + m_dwTargetAttGap;
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	D3DXVECTOR3	vMoveDir = m_vTargetAttPos - m_vStartPos;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	if( vMoveDir == ioMath::VEC3_ZERO )
		vMoveDir = m_vAreaMoveDir;

	ioWeapon *pWeapon = pChar->AreaFireExplicit( m_WeaponInfo, m_dwWeaponBaseIndex, m_vStartPos, vMoveDir, m_dwWeaponDuration );
	ioAreaEffectWeapon *pAreaEffectWeapon = ToAreaEffectWeapon(pWeapon);
	if( pAreaEffectWeapon )
	{
		pAreaEffectWeapon->SetPosition( m_vStartPos );
		pAreaEffectWeapon->SetCollisionDir( vMoveDir );
	}

	m_dwTargetAttCurCnt++;
}

DWORD ioAreaWeapon::GetLiveTime()
{
	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( !pStage )
		return 0;
	
	if( (m_dwLiveTimeByNPC > 0) &&		
		(pStage->GetModeType() == MT_MONSTER_SURVIVAL || pStage->GetModeType() == MT_RAID ||
		Help::IsMonsterDungeonMode(pStage->GetModeType())) )	// NPC모드
	{
		return m_dwLiveTimeByNPC;
	}
	
	if( m_dwLiveTimeByFC > 0 &&
		pStage->GetModeType() == MT_FIGHT_CLUB )	// 챔피언모드
	{
		return m_dwLiveTimeByFC;
	}

	return m_dwLiveTime;
}

void ioAreaWeapon::CheckVisible( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_fInvisibleRange <= 0.0f )
		return;

	ioBaseChar *pOwner = pStage->GetBaseChar( GetOwnerName() );
	ioBaseChar *pTarget = pStage->GetCameraTargetChar();
	if( pOwner && pTarget )
	{
		if( pOwner->GetTeam() != pTarget->GetTeam() )
		{
			float fVisibleRangeSq = m_fInvisibleRange * m_fInvisibleRange;
			
			D3DXVECTOR3 vDiff = m_vStartPos - pTarget->GetWorldPosition();
			vDiff.y = 0.0f;

			ioMapEffect *pEff = pStage->FindMapEffect( m_dwAreaWeaponEffect );
			if( pEff )
			{
				if( D3DXVec3LengthSq( &vDiff ) < fVisibleRangeSq )
					pEff->SetVisible( true );
				else
					pEff->SetVisible( false );
			}
		}
	}
}

bool ioAreaWeapon::CheckRefreshCollisionList( const ioHashString &szName, ioPlayStage *pStage )
{
	if( !pStage )
		return False;

	if( m_dwRefreshDuration <= 0 )
		return false;

	bool bAlready = false;
	int iSize = m_RefreshCollisionList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szName );
		if( pTarget )
		{
			if( IsCollisioned( pTarget, TT_NONE ) )
				return false;
		}
		RefreshCollisionInfo &kInfo = m_RefreshCollisionList[i];
		if( kInfo.m_CharName == szName )
		{
			if( kInfo.m_dwCollisionTime < FRAMEGETTIME() )
			{
				kInfo.m_dwCollisionTime = m_dwRefreshDuration + FRAMEGETTIME();
				return true;
			}
			bAlready = true;
		}
	}

	if( !bAlready )
	{
		RefreshCollisionInfo kInfo( szName, m_dwRefreshDuration + FRAMEGETTIME() );
		m_RefreshCollisionList.push_back( kInfo );
		return true;
	}

	return false;
}

void ioAreaWeapon::SetAreaEffectScale( ioPlayStage *pStage, D3DXVECTOR3 vScale )
{

	ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwAreaWeaponEffect );
	
	if ( pMapEffect )
	{
		ioEffect* pEffect= pMapEffect->GetEffect();
		if ( pEffect )
		{
			pEffect->GetParentNode()->SetScale( vScale );
		}
	}	
}

void ioAreaWeapon::CreateEndAreaAttack( ioPlayStage *pStage )
{
	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( !pOwner )
		return;

	if( m_EndWeaponInfo.m_iWeaponIdx == 0 )
		return;

	CheckAreaWeaponTarget( pStage );

	D3DXVECTOR3	vMoveDir = m_vAreaMoveDir;
	ioWeapon *pWeapon = pOwner->AreaFireExplicit( m_EndWeaponInfo, 0, m_vStartPos, vMoveDir, m_dwEndWeaponDuration );
	ioAreaEffectWeapon *pAreaEffectWeapon = ToAreaEffectWeapon(pWeapon);
	if( pAreaEffectWeapon )
	{
		pAreaEffectWeapon->SetPosition( m_vStartPos );
		pAreaEffectWeapon->SetCollisionDir( m_vAreaMoveDir );
		pAreaEffectWeapon->UpdateCollisionList( m_vNewTargetList, false );
	}
}


bool ioAreaWeapon::CheckTargetEquipObjectBuff( const ioHashString& szBuffName, ioBaseChar* pTarget )
{
	//오브젝트 아이템 장착 버프가 AreaWeapon을 통해서 생성되는 경우에 먼저 현재 장착되어있는 오브젝트 아이템에 대한 처리 로직
	const ioBuff* pTempleteBuff = g_BuffMaker.GetBuffConst( szBuffName );
	if( !pTempleteBuff )
		return false;

	if( pTempleteBuff->GetTypeID() == BT_TARGET_EQUIP_OBJECTITEM || pTempleteBuff->GetTypeID() == BT_CREATE_OBJECT_ITEM )
	{
		if( pTempleteBuff->HasExceptSuicideBuff( pTarget ) )
			return false;

		ioObjectItem* pObject = pTarget->GetObject();
		if( pObject )
		{
			if( ToSmartBoomObjectItem( pObject ) )
			{
				//어택 어트리뷰트를 발생 시킨후 GenerateWeaponDetail을 통해서 ReleaseObjectItem()을 호출
				pTarget->CheckDropSmartBomb( pTarget->GetMoveDir() );
			}
			else if( ToThrowBombItem( pObject ) )
			{
				//어택 어트리뷰트를 발생 시킨후 GenerateWeaponDetail을 통해서 ReleaseObjectItem()을 호출
				pTarget->CheckDropThrowBomb( pTarget->GetMoveDir(), false );
			}
			else if( ToPoweredObjectItem( pObject ) )
			{
				//어택 어트리뷰트를 발생 시킨후 GenerateWeaponDetail을 통해서 ReleaseObjectItem()을 호출
				pTarget->CheckPowerArmorWounded();
			}
			else if( ToBladeChargeObjItem( pObject ) )
			{
				pTarget->CheckDropBladeChargeObject( pTarget->GetMoveDir() );
			}
			else if( ToSpiralChargeObjectItem( pObject ) )
			{
				pTarget->ReleaseObjectItem( __FUNCTION__ );
			}
			else if( ToJackFrostObjectItem( pObject ) )
			{
				pTarget->ReleaseObjectItem( __FUNCTION__ );
			}
			else if( pObject->IsOneTimeWeaponItem() )
			{
				pTarget->CheckObjectItemDrop();
			}
			else
			{
				switch( pObject->GetObejctCreateType() )
				{
				case ioObjectItem::OCT_SOILDER:
				case ioObjectItem::OCT_EQUIP_SKILL:
					{
						pTarget->ReleaseObjectItem( __FUNCTION__ );
					}
					break;
				case ioObjectItem::OCT_EQUIP_BUFF1:
					{
						ioTargetEquipObjectItemBuff* pEquipBuff = ToTargetEquipObjectItemBuff( pTarget->GetBuff( BT_TARGET_EQUIP_OBJECTITEM ) );
						if( pEquipBuff )						
							return pEquipBuff->ReleaseObject( pTarget->GetCreator(), pTarget );
					}
					break;
				case ioObjectItem::OCT_EQUIP_BUFF2:
					{
						ioCreateObjectItemBuff* pCreateBuff = ToCreateObjectItemBuff( pTarget->GetBuff( BT_CREATE_OBJECT_ITEM ) );
						if( pCreateBuff )
							return pCreateBuff->SetReleaseObjectItem( pTarget );
					}
					break;
				default:
					{
					}
					break;
				}
			}
		}
	}

	return true;
}

void ioAreaWeapon::CallNextWeaponAttribute( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	for( int i = 0; i < (int)m_vNextAttackAttributeByTeamBuff.size(); i++ )
	{
		CheckOnceCallAttAttrMap::iterator iter = m_CheckOnceCallAttAttrMap.find( m_vNextAttackAttributeByTeamBuff[ i ].m_iWeaponIdx );
		if( iter != m_CheckOnceCallAttAttrMap.end() && iter->second == true )
			continue;

		pOwner->NormalAttackFireExplicit( m_vNextAttackAttributeByTeamBuff[ i ],
			m_vAreaMoveDir,
			m_vStartPos );

		if( iter != m_CheckOnceCallAttAttrMap.end() && iter->second == false )
			iter->second = true;
	}
}

bool ioAreaWeapon::CheckOverlapAreaWeapon( ioPlayStage* pStage )
{
	if( m_vOverlapAreaWeapon.empty() )
		return false;

	if( m_OverlapTargetType == TT_NONE )
		return false;

	ioBaseChar* pOwner = pStage->GetBaseChar( GetOwnerName() );
	if( !pOwner )
		return false;

	std::vector< std::vector< ioAreaWeapon* > > vOverlapList;
	vOverlapList.clear();

	for( int i = 0; i < (int)m_vOverlapAreaWeapon.size(); i++ )
	{
		std::vector< ioAreaWeapon* > vAreaWeaponList;
		vAreaWeaponList.clear();
		g_AreaWeaponMgr.FindAllAreaWeaponByName( m_vOverlapAreaWeapon[i], vAreaWeaponList );

		for( std::vector< ioAreaWeapon* >::iterator iter = vAreaWeaponList.begin(); iter != vAreaWeaponList.end(); iter++ )
		{
			std::vector< ioAreaWeapon* >::iterator	iterTemp	= iter;
			ioAreaWeapon* pAreaWeapon = (*iter);
			ioBaseChar* pOther = pStage->GetBaseChar( pAreaWeapon->GetOwnerName() );
			if( !pOther )
				continue;

			if( m_OverlapTargetType == TT_OWNER )
			{
				if( pAreaWeapon->GetOwnerName() != GetOwnerName() )
				{
					iter--;
					vAreaWeaponList.erase( iterTemp );
				}
			}
			else if( m_OverlapTargetType == TT_TEAM )
			{
				if( pOwner->GetTeam() != pOther->GetTeam() )
				{
					iter--;
					vAreaWeaponList.erase( iterTemp );
				}
			}
			else if( m_OverlapTargetType == TT_ENEMY )
			{
				if( pOwner->GetTeam() == pOther->GetTeam() )
				{
					iter--;
					vAreaWeaponList.erase( iterTemp );
				}
			}
		}

		vOverlapList.push_back( vAreaWeaponList );
	}

	if( vOverlapList.size() < m_vOverlapAreaWeapon.size() )
		return false;

	for( int i = 0; i < (int)vOverlapList.size() - 1; i++ )
	{
		for( int j = 0; j < (int)vOverlapList[i].size(); j++ )
		{
			if( fabsf( m_vStartPos.x - vOverlapList[i][j]->GetStartPos().x ) <= 10.0f && fabsf( m_vStartPos.z == vOverlapList[i][j]->GetStartPos().z ) <= 10.0f )
			{
				for( int k = 0; k < (int)vOverlapList[i + 1].size(); k++ )
				{
					if( fabsf( m_vStartPos.x - vOverlapList[i + 1][k]->GetStartPos().x ) <= 10.0f && fabsf( m_vStartPos.z == vOverlapList[i + 1][k]->GetStartPos().z ) <= 10.0f )
						return true;
				}
			}
		}
	}

	std::vector< std::vector< CrossEquationInfo > > vCrossEquationList;
	for( int i = 0; i < (int)vOverlapList.size(); i++ )
	{
		std::vector< CrossEquationInfo > vCrossEquation;
		for( int j = 0; j < (int)vOverlapList[i].size(); j++ )
		{
			CrossEquationInfo crossEquation;
			crossEquation.m_fXCoefficient = ( vOverlapList[i][j]->GetStartPos().x * 2.0f ) - ( m_vStartPos.x * 2.0f );
			crossEquation.m_fYCoefficient = ( vOverlapList[i][j]->GetStartPos().z * 2.0f ) - ( m_vStartPos.z * 2.0f );
			crossEquation.m_fConstant = ( pow( vOverlapList[i][j]->GetStartPos().x, 2.0f ) + pow( vOverlapList[i][j]->GetStartPos().z, 2.0f ) - pow( vOverlapList[i][j]->GetAreaRange(), 2.0f ) ) 
				- ( pow( m_vStartPos.x, 2.0f ) + pow( m_vStartPos.z, 2.0f ) - pow( (float)m_fCollisionRange, 2.0f ) );
			
			vCrossEquation.push_back( crossEquation );

			if( (int)vOverlapList.size() <= i + 1 )
				continue;

			for( int k = 0; k < (int)vOverlapList[i + 1].size(); k++ )
			{
				float fD, fT1, fT2, fX1, fY1, fX2, fY2;
				fD = sqrt( pow( ( vOverlapList[i][j]->GetStartPos().x - m_vStartPos.x ), 2.0f ) + pow( ( vOverlapList[i][j]->GetStartPos().z - m_vStartPos.z ), 2.0f ) );
				fT1 = acosf( ( pow( (float)m_fCollisionRange, 2.0f ) - pow( vOverlapList[i][j]->GetAreaRange(), 2.0f ) + pow( fD, 2.0f ) ) / ( 2 * (float)m_fCollisionRange * fD ) );
				fT2 = atanf( ( vOverlapList[i][j]->GetStartPos().z - m_vStartPos.z ) / ( vOverlapList[i][j]->GetStartPos().x - m_vStartPos.x ) );

				if( fT2 != 0 )
				{
					fX1 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 + fT1 );
					fY1 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 + fT1 );
					fX2 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 - fT1 );
					fY2 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 - fT1 );

					if( pow( vOverlapList[i + 1][k]->GetStartPos().x - fX1, 2.0f ) + pow( vOverlapList[i + 1][k]->GetStartPos().z - fY1, 2.0f ) < pow( vOverlapList[i + 1][k]->GetAreaRange(), 2.0f )
						&& pow( vOverlapList[i + 1][k]->GetStartPos().x - fX2, 2.0f ) + pow( vOverlapList[i + 1][k]->GetStartPos().z - fY2, 2.0f ) < pow( vOverlapList[i + 1][k]->GetAreaRange(), 2.0f ) )
						return true;
				}

				fD = sqrt( pow( ( vOverlapList[i + 1][k]->GetStartPos().x - m_vStartPos.x ), 2.0f ) + pow( ( vOverlapList[i + 1][k]->GetStartPos().z - m_vStartPos.z ), 2.0f ) );
				fT1 = acosf( ( pow( (float)m_fCollisionRange, 2.0f ) - pow( vOverlapList[i + 1][k]->GetAreaRange(), 2.0f ) + pow( fD, 2.0f ) ) / ( 2 * (float)m_fCollisionRange * fD ) );
				fT2 = atanf( ( vOverlapList[i + 1][k]->GetStartPos().z - m_vStartPos.z ) / ( vOverlapList[i + 1][k]->GetStartPos().x - m_vStartPos.x ) );

				if( fT2 != 0 )
				{
					fX1 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 + fT1 );
					fY1 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 + fT1 );
					fX2 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 - fT1 );
					fY2 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 - fT1 );

					if( pow( vOverlapList[i][j]->GetStartPos().x - fX1, 2.0f ) + pow( vOverlapList[i][j]->GetStartPos().z - fY1, 2.0f ) < pow( vOverlapList[i][j]->GetAreaRange(), 2.0f )
						&& pow( vOverlapList[i][j]->GetStartPos().x - fX2, 2.0f ) + pow( vOverlapList[i][j]->GetStartPos().z - fY2, 2.0f ) < pow( vOverlapList[i][j]->GetAreaRange(), 2.0f ) )
						return true;
				}

				fD = sqrt( pow( ( vOverlapList[i + 1][k]->GetStartPos().x - vOverlapList[i][j]->GetStartPos().x ), 2.0f ) + pow( ( vOverlapList[i + 1][k]->GetStartPos().z - vOverlapList[i][j]->GetStartPos().z ), 2.0f ) );
				fT1 = acosf( ( pow( vOverlapList[i][j]->GetAreaRange(), 2.0f ) - pow( vOverlapList[i + 1][k]->GetAreaRange(), 2.0f ) + pow( fD, 2.0f ) ) / ( 2 * vOverlapList[i][j]->GetAreaRange() * fD ) );
				fT2 = atanf( ( vOverlapList[i + 1][k]->GetStartPos().z - vOverlapList[i][j]->GetStartPos().z ) / ( vOverlapList[i + 1][k]->GetStartPos().x - vOverlapList[i][j]->GetStartPos().x ) );

				if( fT2 != 0 )
				{
					fX1 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 + fT1 );
					fY1 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 + fT1 );
					fX2 = m_vStartPos.x + (float)m_fCollisionRange * cosf( fT2 - fT1 );
					fY2 = m_vStartPos.z + (float)m_fCollisionRange * sinf( fT2 - fT1 );

					if( pow( m_vStartPos.x - fX1, 2.0f ) + pow( m_vStartPos.z - fY1, 2.0f ) < pow( (float)m_fCollisionRange, 2.0f )
						&& pow( m_vStartPos.x - fX2, 2.0f ) + pow( m_vStartPos.z - fY2, 2.0f ) < pow( (float)m_fCollisionRange, 2.0f ) )
						return true;
				}
			}
		}

		vCrossEquationList.push_back( vCrossEquation );
	}

	std::vector< std::vector< POINT > > vCrossPoint;
	for( int i = 0; i < (int)vCrossEquationList.size() - 1; i++ )
	{
		std::vector< POINT > tempCrossPoint;
		for( int j = 0; j < (int)vCrossEquationList[i].size(); j++ )
		{
			for( int k = 0; k < (int)vCrossEquationList[i + 1].size(); k++ )
			{
				POINT crossPoint;
				crossPoint = GetIntersection( vCrossEquationList[i][j], vCrossEquationList[i + 1][k] );

				if( m_fCollisionRange * m_fCollisionRange < pow( crossPoint.x - m_vStartPos.x, 2 ) + pow( crossPoint.y - m_vStartPos.z, 2 ) )
					continue;

				tempCrossPoint.push_back( crossPoint );
			}
		}

		if( tempCrossPoint.size() > 0 )
			vCrossPoint.push_back( tempCrossPoint );
	}

	if( vCrossPoint.size() <= 0 )
		return false;

	if( vCrossPoint.size() == 1 )
		return true;

	for( int i = 0; i < (int)vCrossPoint[0].size(); i++ )
	{
		bool bIntersection = false;
		POINT p1 = vCrossPoint[0][i];

		for( int j = 1; j < (int)vCrossPoint.size() - 1; j++ )
		{
			bIntersection = false;
			for( int k = 0; k < (int)vCrossPoint[j].size(); k++ )
			{
				POINT p2 = vCrossPoint[j][k];

				if( p1.x == p2.x && p1.y == p2.y )
				{
					bIntersection = true;
					break;
				}
			}

			if( !bIntersection )
				break;
		}

		if( bIntersection )
			return true;
	}

	return false;
}

POINT ioAreaWeapon::GetIntersection( CrossEquationInfo crossInfo1, CrossEquationInfo crossInfo2 )
{
	POINT crossPoint;
	crossPoint.x = (int)((( crossInfo1.m_fYCoefficient * crossInfo2.m_fConstant ) - ( crossInfo2.m_fYCoefficient * crossInfo1.m_fConstant )) / (( crossInfo1.m_fXCoefficient * crossInfo2.m_fYCoefficient ) - ( crossInfo2.m_fXCoefficient * crossInfo1.m_fYCoefficient )));
	crossPoint.y = (int)((-1.0f * crossInfo1.m_fXCoefficient / crossInfo1.m_fYCoefficient) * 
		( ( ( crossInfo1.m_fYCoefficient * crossInfo2.m_fConstant ) - ( crossInfo2.m_fYCoefficient * crossInfo1.m_fConstant ) ) / ( ( crossInfo1.m_fXCoefficient * crossInfo2.m_fYCoefficient ) - ( crossInfo2.m_fXCoefficient * crossInfo1.m_fYCoefficient ) ) ) - 
		( crossInfo1.m_fConstant / crossInfo1.m_fYCoefficient ));
	
	crossPoint.x *= -1;
	crossPoint.y *= -1;

	return crossPoint;
}

void ioAreaWeapon::CreateOverlapWeapon( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	ioBaseChar *pOwner = pStage->GetBaseChar(m_OwnerName);
	if( !pOwner )
		return;

	D3DXVECTOR3	vMoveDir = m_vTargetAttPos - m_vStartPos;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );
	if( vMoveDir == ioMath::VEC3_ZERO )
		vMoveDir = m_vAreaMoveDir;

	pOwner->AreaFireExplicit( m_OverlapWeaponInfo, pOwner->IncreaseWeaponIndexBase(), m_vStartPos, vMoveDir, m_dwOverlapWeaponDuration );
}