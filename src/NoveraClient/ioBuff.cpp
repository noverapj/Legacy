
#include "stdafx.h"

#include "ioBuff.h"
#include "ioGrowthLevel.h"
#include "ioNpcChar.h"

ioBuff::ioBuff()
{
	m_pOwner = NULL;
	//m_pOwnerItem = NULL;

	m_dwCheckTime = 0;
	m_dwBuffStartTime = 0;
	m_dwBuffEndTime = 0;

	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_dwExtraDurationByGrowth = 0;

	m_iChangeColorIndex = -1;

	m_bBuffStart = false;
	m_bLive = false;
	m_bPassive = false;
	m_bTeamAttack = false;
	m_bLoopSoundPlayed = false;

	m_CreateChar.Clear();
	m_dwCreateWeaponItemCode = 0;
	m_bDescEqualDuration     = false;

	m_dwAreaWeaponIndex = 0;
	m_dwCreateWeaponIndex = 0;
	m_dwCreateAreawaponIndex = 0;

	m_vWeaponIndexList.clear();

	m_LoopSoundID = 0;
	
	m_bRemoveEquipSkillBuff = false;
	m_fCurAccumSpeedRate = 0.0f;

	m_dwInvisibleWeaponID = 0;
	m_dwInvisibleHelmetID = 0;
	m_dwInvisibleCloakID = 0;

	m_dwStopPassiveSkillID = 0;

	m_bCallErased = false;
	m_bNoEndEffect = false;
	m_bChangeWeaponDirToTargetRot = false;

	m_dwReserveGetupEndTime = 0;

	m_iBuffCategory = BC_NORMAL;

	m_dwPassiveBuffEffectStartTime = 0;
	m_bPassiveActivated = false;
}

ioBuff::ioBuff( const ioBuff &rhs )
: m_Name( rhs.m_Name ),
  m_EndEmoticon( rhs.m_EndEmoticon ),
  m_KillingList( rhs.m_KillingList ),
  m_SuicideList( rhs.m_SuicideList ),
  m_RemoveByTypeList( rhs.m_RemoveByTypeList ),
  m_BuffEffect( rhs.m_BuffEffect ),
  m_bVisibleBuffEffectOnlyOwner( rhs.m_bVisibleBuffEffectOnlyOwner ),
  m_bVisibleBuffEffectIndependence( rhs.m_bVisibleBuffEffectIndependence ),
  m_BuffEndEffect( rhs.m_BuffEndEffect ),
  m_PassiveBuffEffect( rhs.m_PassiveBuffEffect ),
  m_dwPassiveBuffIdleTime( rhs.m_dwPassiveBuffIdleTime ),
  m_LoopSound( rhs.m_LoopSound ),
  m_dwBuffDuration( rhs.m_dwBuffDuration ),
  m_dwBuffDurationByNpc( rhs.m_dwBuffDurationByNpc ),
  m_dwBuffDurationByBossNpc( rhs.m_dwBuffDurationByBossNpc ),
  m_dwNPCModeBuffDuration( rhs.m_dwNPCModeBuffDuration ),
  m_dwTrainingModeBuffDuration( rhs.m_dwTrainingModeBuffDuration ),
  m_dwFCModeBuffDuration( rhs.m_dwFCModeBuffDuration ),
  m_dwFCModeTickTime( rhs.m_dwFCModeTickTime ),
  m_dwTDModeTickTime( rhs.m_dwTDModeTickTime ),
  m_fGaugePerTick( rhs.m_fGaugePerTick ),
  m_dwTickTime( rhs.m_dwTickTime ),
  m_dwDelayDuration( rhs.m_dwDelayDuration ),
  m_CallBuffList( rhs.m_CallBuffList ),
  m_ExceptTargetBuff( rhs.m_ExceptTargetBuff ),
  m_ExceptCallBuffList( rhs.m_ExceptCallBuffList ),
  m_CallAttributeList( rhs.m_CallAttributeList ),
  m_CallAreaWeapon( rhs.m_CallAreaWeapon ),
  m_bCreatorLook( rhs.m_bCreatorLook ),
  m_CreatorLookType( rhs.m_CreatorLookType ),
  m_bInvisibleState( rhs.m_bInvisibleState ),
  m_bInvisibleWeapon( rhs.m_bInvisibleWeapon ),
  m_bInvisibleCloak( rhs.m_bInvisibleCloak ),
  m_bInvisibleHelmet( rhs.m_bInvisibleHelmet ),
  m_bInvisibleHead( rhs.m_bInvisibleHead ),
  m_Desc( rhs.m_Desc ),
  m_DescName( rhs.m_DescName ),
  m_DescIcon( rhs.m_DescIcon ),
  m_bDescEqualDuration( rhs.m_bDescEqualDuration ),
  m_bChangeColor( rhs.m_bChangeColor ),
  m_ChangeColor( rhs.m_ChangeColor ),
  m_bDisableGrowth( rhs.m_bDisableGrowth ),
  m_bCallExistAttacker( rhs.m_bCallExistAttacker ),
  m_OtherBuff( rhs.m_OtherBuff ),
  m_BuffEndBuff( rhs.m_BuffEndBuff ),
  m_BuffEndBuff2( rhs.m_BuffEndBuff2 ),
  m_bIgnoreDefenseBreakBuff( rhs.m_bIgnoreDefenseBreakBuff ),
  m_bIgnoreProtectBuff( rhs.m_bIgnoreProtectBuff ),
  m_bCharCollisionSkip( rhs.m_bCharCollisionSkip ),
  m_fCallAttributeEnableHeight( rhs.m_fCallAttributeEnableHeight ),
  m_bEnableSkillState( rhs.m_bEnableSkillState ),
  m_bUseCallAttWeaponCol( rhs.m_bUseCallAttWeaponCol ),
  m_bEnableOverlapCnt( rhs.m_bEnableOverlapCnt ),
  m_bOverlapCreatorCompare( rhs.m_bOverlapCreatorCompare ),
  m_bEnableWoundCancel( rhs.m_bEnableWoundCancel ),
  m_iMaxActiveCount( rhs.m_iMaxActiveCount ),
  m_bCancelBuff( rhs.m_bCancelBuff ),
  m_bEnablePossession( rhs.m_bEnablePossession ),
  m_fMaxAccumSpeedRate( rhs.m_fMaxAccumSpeedRate ),
  m_fMinAccumSpeedRate( rhs.m_fMinAccumSpeedRate ),
  m_GroupName( rhs.m_GroupName ),
  m_TitleName( rhs.m_TitleName ),
  m_bPassiveSkillGauageReduce( rhs.m_bPassiveSkillGauageReduce ),
  m_GoodBadType( rhs.m_GoodBadType ),
  m_bReduceAllGaugeOnBuffEnd( rhs.m_bReduceAllGaugeOnBuffEnd ),
  m_bPassEndBuffCall( rhs.m_bPassEndBuffCall ),
  m_bNotTargetNpc( rhs.m_bNotTargetNpc ),
  m_bNoEndEffect( rhs.m_bNoEndEffect ),
  m_bSendDelayNetWork( rhs.m_bSendDelayNetWork ),
  m_bPetForceHide( rhs.m_bPetForceHide ),
  m_bCurBuffAddEnable( rhs.m_bCurBuffAddEnable ),
  m_bWaitSelfRemove( rhs.m_bWaitSelfRemove ),
  m_bGetupStateForceEnd( rhs.m_bGetupStateForceEnd ),
  m_iBuffCategory( rhs.m_iBuffCategory ),
  m_bChangeWeaponDirToTargetRot( rhs.m_bChangeWeaponDirToTargetRot ),
  m_bEndAfterAction( rhs.m_bEndAfterAction ),
  m_bEnableEquipItemGaugeUpdate( rhs.m_bEnableEquipItemGaugeUpdate ),
  m_bBuffDuration( rhs.m_bBuffDuration ),	// 2020-08-19
  m_bIsDuplicateBuff( rhs.m_bIsDuplicateBuff )	// 2020-11-02
{
	m_pOwner = NULL;
	//m_pOwnerItem = NULL;

	m_dwCheckTime = 0;
	m_dwBuffStartTime = 0;
 	m_dwBuffEndTime = 0;

	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_dwExtraDurationByGrowth = 0;

	m_iChangeColorIndex = -1;

	m_iBuffType = rhs.m_iBuffType;
	m_iOperationType = rhs.m_iOperationType;
	m_iCameraType = rhs.m_iCameraType;

	m_bEnableDelayState = rhs.m_bEnableDelayState;
	m_bDisableChange = rhs.m_bDisableChange;
	m_bBadStateBuff = rhs.m_bBadStateBuff;
	m_bDisableBoss = rhs.m_bDisableBoss;

	m_bBuffStart = false;
	m_bLive = false;
	m_bPassive = false;
	m_bTeamAttack = false;
	m_bLoopSoundPlayed = false;

	m_CreateChar.Clear();
	m_dwCreateWeaponItemCode = 0;
	m_dwCreateWeaponIndex = 0;

	m_dwAreaWeaponIndex = 0;
	m_dwCreateAreawaponIndex = 0;

	m_vWeaponIndexList.clear();

	m_LoopSoundID = 0;

	m_bRemoveEquipSkillBuff = false;
	m_fCurAccumSpeedRate = 0.0f;

	m_dwInvisibleWeaponID = 0;
	m_dwInvisibleHelmetID = 0;
	m_dwInvisibleCloakID = 0;

	m_dwStopPassiveSkillID = 0;

	m_bCallErased = false;

	m_dwReserveGetupEndTime = 0;

	m_dwPassiveBuffEffectStartTime = 0;
	m_bPassiveActivated = false;
}

ioBuff::~ioBuff()
{
}

void ioBuff::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_Name = szBuf;

	rkLoader.LoadString_e( "buff_effect", "", szBuf, MAX_PATH );
	m_BuffEffect = szBuf;
	m_bVisibleBuffEffectOnlyOwner = rkLoader.LoadBool_e( "visible_buff_effect_only_Owner", false );
	m_bVisibleBuffEffectIndependence = rkLoader.LoadBool_e( "visible_buff_effect_independence", false );

	rkLoader.LoadString_e( "buff_end_effect", "", szBuf, MAX_PATH );
	m_BuffEndEffect = szBuf;

	rkLoader.LoadString_e( "passive_buff_effect", "", szBuf, MAX_PATH );
	m_PassiveBuffEffect = szBuf;
	m_dwPassiveBuffIdleTime = rkLoader.LoadInt_e( "passive_buff_idle_time", 0 );

	m_GoodBadType = (GoodBadType)rkLoader.LoadInt_e( "good_bad_type", GBT_NONE );

	rkLoader.LoadString_e( "loop_sound", "", szBuf, MAX_PATH );
	m_LoopSound = szBuf;

	rkLoader.LoadString_e( "buff_end_emoticon", "", szBuf, MAX_PATH );
	m_EndEmoticon = szBuf;

	rkLoader.LoadString_e( "buff_desc_name", "", szBuf, MAX_PATH );
	m_DescName = szBuf;
	rkLoader.LoadString_e( "buff_desc", "", szBuf, MAX_PATH );
	m_Desc = szBuf;
	rkLoader.LoadString_e( "buff_desc_icon", "", szBuf, MAX_PATH );
	m_DescIcon = szBuf;
	m_bDescEqualDuration = rkLoader.LoadBool_e( "buff_emoticon_equal_duration", false );

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	
	m_dwDelayDuration = rkLoader.LoadInt_e( "delay_duration", 0 );
	m_dwBuffDuration = (DWORD)rkLoader.LoadInt_e( "duration", 0 );
	m_dwBuffDurationByNpc = (DWORD)rkLoader.LoadInt_e( "duration_npc", m_dwBuffDuration );
	m_dwBuffDurationByBossNpc = (DWORD)rkLoader.LoadInt_e( "duration_boss_npc", m_dwBuffDurationByNpc );
	m_dwNPCModeBuffDuration = (DWORD)rkLoader.LoadInt_e( "npc_mode_duration", 0 );

	m_dwTrainingModeBuffDuration = (DWORD)rkLoader.LoadInt_e( "training_mode_duration", 0 );

	m_dwFCModeBuffDuration = (DWORD)rkLoader.LoadInt_e( "FC_mode_duration", 0 );
	m_dwFCModeTickTime = (DWORD)rkLoader.LoadInt_e( "FC_tic_time", 0 );

	m_dwTDModeTickTime = (DWORD)rkLoader.LoadInt_e("npc_mode_ticktime", 0);

	m_dwTickTime = rkLoader.LoadInt_e( "tick_time", 0 );
	m_fGaugePerTick = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );

	m_iOperationType = rkLoader.LoadInt_e( "operation_type", 1 );
	m_iCameraType = rkLoader.LoadInt_e( "camera_type", 0 );

	m_bChangeColor = rkLoader.LoadBool_e( "change_color", false );

	m_ChangeColor.r = rkLoader.LoadFloat_e( "change_color_red", 255.0f ) / 255.0f;
	m_ChangeColor.g = rkLoader.LoadFloat_e( "change_color_green", 255.0f ) / 255.0f;
	m_ChangeColor.b = rkLoader.LoadFloat_e( "change_color_blue", 255.0f ) / 255.0f;
	m_ChangeColor.a = rkLoader.LoadFloat_e( "change_color_alpha", 255.0f ) / 255.0f;

	m_bEnableDelayState = rkLoader.LoadBool_e( "enable_delay_state", false );
	m_bDisableChange = rkLoader.LoadBool_e( "disable_change_char", false );
	m_bBadStateBuff = rkLoader.LoadBool_e( "bad_state_buff", false );
	m_bDisableBoss = rkLoader.LoadBool_e( "disable_boss", false );
	m_bCreatorLook = rkLoader.LoadBool_e( "creator_look", false );

	m_CreatorLookType = (CreatorLookType)rkLoader.LoadInt_e( "creator_look", 0 );

	m_bInvisibleState = rkLoader.LoadBool_e( "invisible_state", false );
	
	m_bInvisibleWeapon = rkLoader.LoadBool_e( "invisible_weapon", false );
	m_bInvisibleCloak = rkLoader.LoadBool_e( "invisible_cloak", false );
	m_bInvisibleHelmet = rkLoader.LoadBool_e( "invisible_helmet", false );
	m_bInvisibleHead = rkLoader.LoadBool_e( "invisible_head", false );

	m_bCallExistAttacker = rkLoader.LoadBool_e( "call_exist_attacker", false );
	m_bUseCallAttWeaponCol = rkLoader.LoadBool_e( "call_use_weapon_col", false );
	m_fCallAttributeEnableHeight = rkLoader.LoadFloat_e( "call_attribute_enable_height", 0.0f );

	m_bIgnoreDefenseBreakBuff = rkLoader.LoadBool_e( "ignore_defense_break_buff", false );
	m_bIgnoreProtectBuff = rkLoader.LoadBool_e( "ignore_protect_buff", false );
	m_bCharCollisionSkip = rkLoader.LoadBool_e( "char_collision_skip", false );
	m_bCancelBuff = rkLoader.LoadBool_e( "cancel_by_escape_skill", false );

	m_bEnableSkillState = rkLoader.LoadBool_e( "enable_skill_state", false );
	m_bEnableOverlapCnt = rkLoader.LoadBool_e( "enable_overlap_cnt", false );
	m_bOverlapCreatorCompare = rkLoader.LoadBool_e( "overlap_creator_compare", false );
	m_bEnableWoundCancel = rkLoader.LoadBool_e( "enable_wound_cancel", false );
	m_bEnablePossession = rkLoader.LoadBool_e( "enable_possession", false );

	m_bPassiveSkillGauageReduce = rkLoader.LoadBool_e( "enable_passive_skill_reduce_gauge", false );

	rkLoader.LoadString_e( "other_buff", "", szBuf, MAX_PATH );
	m_OtherBuff = szBuf;

	rkLoader.LoadString_e( "buff_end_buff", "", szBuf, MAX_PATH );
	m_BuffEndBuff = szBuf;

	rkLoader.LoadString_e( "buff_end_buff2", "", szBuf, MAX_PATH );
	m_BuffEndBuff2 = szBuf;

	m_bPassEndBuffCall = rkLoader.LoadBool_e( "pass_buff_end_buff", false );

	m_iMaxActiveCount = rkLoader.LoadInt_e( "max_active_count", 0 );

	m_fMinAccumSpeedRate = rkLoader.LoadFloat_e( "min_accumlate_speed_rate", -2.0f );
	m_fMaxAccumSpeedRate = rkLoader.LoadFloat_e( "max_accumlate_speed_rate", 2.0f );

	m_bReduceAllGaugeOnBuffEnd = rkLoader.LoadBool_e( "reduce_all_gauge_on_buff_end", false );

	m_bNotTargetNpc = rkLoader.LoadBool_e( "not_target_npc", false );

	m_bSendDelayNetWork = rkLoader.LoadBool_e( "if_delay_state_send_net", true );

	LoadCallBuffList( rkLoader );
	LoadCallAttributeList( rkLoader );
	LoadCallAreaWeapon( rkLoader );
	LoadBuffExceptList( rkLoader );

	//악령도둑때 추가
	m_bNoEndEffect = rkLoader.LoadBool_e( "not_multy_end_effect", false );

	m_bPetForceHide = rkLoader.LoadBool_e( "pet_force_hide", false );

	m_bCurBuffAddEnable = rkLoader.LoadBool_e( "cur_buff_add_enable", false );

	m_bWaitSelfRemove = rkLoader.LoadBool_e( "wait_self_remove", false );

	m_bGetupStateForceEnd = rkLoader.LoadBool_e( "get_up_state_force_end", false );
	m_bChangeWeaponDirToTargetRot = rkLoader.LoadBool_e( "change_weapon_dir_to_target_rot", false );
	
	m_bEndAfterAction = rkLoader.LoadBool_e( "end_after_action", false );

	// 장착한 아이템 게이지 업데이트 여부 플래그
	m_bEnableEquipItemGaugeUpdate = rkLoader.LoadBool_e( "enable_equip_item_gauge_update", true );

	// 2020-08-19
	m_bBuffDuration = rkLoader.LoadBool_e( "check_duration", false );

	// AreaWeapon 아군 이브(자신 제외)가 생성한 다른 에어리어웨폰에 영향X 2020-11-02
	m_bIsDuplicateBuff = rkLoader.LoadBool_e( "duplicate_buff", false );
}

bool ioBuff::IsEanableCurBuffAdd()
{
	return m_bCurBuffAddEnable;
}

void ioBuff::SetGroupName( const ioHashString& szGroupName )
{
	m_GroupName = szGroupName;
}

void ioBuff::SetTitleName( const ioHashString& szTitleName )
{
	m_TitleName = szTitleName;
}

void ioBuff::ReLoadProperty()
{
	// HARDCODE :: 치트관련 1단계에서는 미적용
	/*
	ioINILoader kSetItemLoader( m_GroupName.c_str() );
	kSetItemLoader.SetTitle( m_TitleName.c_str() );
	
	LoadProperty( kSetItemLoader );
	*/
}

ioBuff* ioBuff::Clone()
{
	return new ioBuff( *this );
}

void  ioBuff::SetBuffDuration( DWORD dwBuffDuration )
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		m_dwBuffDuration = dwBuffDuration;
	else
	{
		if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
			m_dwBuffDurationByBossNpc = dwBuffDuration;
		else
			m_dwBuffDurationByNpc = dwBuffDuration;
	}
}

DWORD ioBuff::GetBuffDuration()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );

	if( m_dwTrainingModeBuffDuration > 0 &&
		m_pOwner->GetCreator() &&
		((m_pOwner->GetCreator()->GetModeType() == MT_TRAINING ) ) )
		return m_dwTrainingModeBuffDuration;

	if( m_dwNPCModeBuffDuration > 0 &&
		m_pOwner->GetCreator() &&
		((m_pOwner->GetCreator()->GetModeType() == MT_MONSTER_SURVIVAL) || (m_pOwner->GetCreator()->GetModeType() == MT_RAID) ||
		(Help::IsMonsterDungeonMode(m_pOwner->GetCreator()->GetModeType())) ) )
		return m_dwNPCModeBuffDuration;

	if( m_dwFCModeBuffDuration > 0 &&
		m_pOwner->GetCreator() &&
		m_pOwner->GetCreator()->GetModeType() == MT_FIGHT_CLUB )
		return m_dwFCModeBuffDuration;

	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwBuffDuration;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwBuffDurationByBossNpc;
	
	return m_dwBuffDurationByNpc;
}

void  ioBuff::SetBuffDurationByRate( float fRate )
{
	DWORD dwBuffDuration = GetBuffDuration();
	dwBuffDuration *= fRate;
	SetBuffDuration( dwBuffDuration );

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
}

void  ioBuff::SetBuffDurationAndEndTime( DWORD dwBuffDuration )
{
	SetBuffDuration( dwBuffDuration );

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
}

void ioBuff::StartBuff( ioBaseChar *pOwner )
{
	#ifndef SHIPPING 
		if( Setting::ShowExtraInfo() )
		{
			if( m_iBuffType != BT_CAMERA_BUFF )
				LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
		}
	#endif

	if( !pOwner )
		return;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckInvisibleState( false );
	
	CheckChangeColor( true );
	CheckChangeSkill( true );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwBuffStartTime = dwCurTime;
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;
	m_bRemoveEquipSkillBuff = false;
	m_bLive = true;

	CheckBuffDesc(); // buff Time 값 계산 이후에 호출할 것 버프에 따라서 Time 값이 필요하다.

	if( m_bVisibleBuffEffectOnlyOwner && m_pOwner->IsNeedProcess() )
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner(), !m_bVisibleBuffEffectIndependence );
	
	if( !m_bVisibleBuffEffectOnlyOwner )
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner(), !m_bVisibleBuffEffectIndependence );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	CalcRandAttributeTime( pOwner );
	
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pOwner );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	// 이 버프와 같이 시작될 버프
	StartOtherBuff();
}


void ioBuff::CalcRandAttributeTime( ioBaseChar *pOwner )
{
	if( !m_CreateChar.IsEmpty() )
	{
		ioBaseChar *pCreater = pOwner->GetBaseChar( m_CreateChar );
		if( pCreater )
		{
			IORandom random;
			DWORD dwSeed = pCreater->GetRandomSeed();

			for( int i = 0; i < (int)m_CallAttributeList.size(); i++ )
			{
				random.SetRandomSeed( dwSeed+i );

				int nMin = m_CallAttributeList[i].m_iRandomMinTime;
				int nMax = m_CallAttributeList[i].m_iRandomMaxTime;
				if( nMin >= 0 && nMax >= 0 )
					m_CallAttributeList[i].m_dwRandomTime = random.Random( nMax - nMin ) + nMin;
			}
		}
	}
}



bool ioBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );
	if( m_iOperationType != OT_SWITCH )
	{
		if( GetBuffDuration() > 0 && dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bRemoveEquipSkillBuff = false;
	m_bLive = true;

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	if( m_bVisibleBuffEffectOnlyOwner && m_pOwner->IsNeedProcess() )
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner(), !m_bVisibleBuffEffectIndependence );

	if( !m_bVisibleBuffEffectOnlyOwner )
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner(), !m_bVisibleBuffEffectIndependence );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckInvisibleState( false );

	CheckChangeSkill( true );
	CheckChangeColor( true );
	
	CheckBuffDesc();

	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( m_pOwner );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	return true;
}

void ioBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	StartBuff( pOwner );
}

void ioBuff::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_bGetupStateForceEnd )
	{
		if ( m_dwReserveGetupEndTime <= 0 && m_pOwner->GetBlowWoundedState() == BW_GETUP )
			m_dwReserveGetupEndTime = m_pOwner->GetGetupDurationTime() -10;

		if ( m_dwReserveGetupEndTime > 0 && m_dwReserveGetupEndTime < dwCurTime)
		{
			if ( m_pOwner->GetBlowWoundedState() == BW_GETUP )
			{
				SetReserveEndBuff();
				return;
			}
			else
				m_dwReserveGetupEndTime = 0;
		}
	}

	if( CheckEnableDelayState() )
		return;

	CheckCallingProcess();

	if( GetBuffDuration() == 0 )
	{		
		return;
	}

	if( m_dwBuffEndTime <= dwCurTime )
	{		
		if( !m_bEndAfterAction )
			SetReserveEndBuff();
		else if(m_pOwner->GetState() != CS_BUFF_ACTION )
			SetReserveEndBuff();
		return;
	}
}

void ioBuff::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( m_pOwner )
	{
		for( int iSlot = ES_WEAPON ; iSlot <= ES_CLOAK; ++iSlot )
		{			
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( m_pOwner->GetEquipedSkill( iSlot ) );
			if( pPassiveSkill && pPassiveSkill->GetName() == m_SkillName && pPassiveSkill->IsBuffEndGaugeClear( GetName() ) )
				pPassiveSkill->SetCurPassiveGauge( 0.0f );
		}
	}
	
	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );

	CheckInvisibleState( true );

	CheckChangeSkill( false );
	CheckChangeColor( false );

	//악령 도둑때 추가됨
	if( m_pOwner && (!m_bNoEndEffect || !m_bCallErased ) )
	{
		m_pOwner->EndEffect( m_BuffEffect, false );
		m_pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );
	}

	if( m_pOwner && m_bDescEqualDuration )
		m_pOwner->EndEmoticonByName( m_DescName );

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	if( IsBadStateBuff() )
	{
		if( m_pOwner && !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
	}
	else
	{
		if( m_pOwner && !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
	}

	if( m_pOwner && m_bReduceAllGaugeOnBuffEnd )
	{
		ioItem *pItem = GetOwnerItem();
		if( pItem && pItem->HasSkillGauge() )
			pItem->ReduceNeedGauge( pItem->GetMaxSkillGauge( m_pOwner->GetCharName() ), m_pOwner );
	}

	DestroyAreaWeapon();

	if( !m_bPassEndBuffCall )
		CheckBuffEndBuff();

	CheckCallWeaponEnd();
	
	if ( m_pOwner )
	{
		if ( m_bPetForceHide )
		{
			ioPet *pPet = g_PetMgr.FindPet( m_pOwner );
			if ( pPet )
				pPet->SetForceHide( false );
		}
	}

	// 다른 연관 버프도 끝내기
	EndOtherBuff();
}

void ioBuff::CancelBuff()
{
	if( m_bCancelBuff )
		SetReserveEndBuff();
}

bool ioBuff::IsLiveButStopped() const
{
	return false;
}

void ioBuff::SetReserveEndBuff()
{
	m_bLive = false;
}

void ioBuff::SetTypeID( int iType )
{
	m_iBuffType = iType;
}

bool ioBuff::SetPassiveBuff( bool bPassive )
{
	if( m_iOperationType != OT_PASSIVE )
		return false;

	m_bPassive = bPassive;
	return true;
}

void ioBuff::LoadCallBuffList( ioINILoader &rkLoader )
{
	m_CallBuffList.clear();
	m_ExceptCallBuffList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_buff_cnt", 0 );
	if( iCnt > 0 )
	{
		m_CallBuffList.reserve( iCnt );

		for( int i=0; i<iCnt; ++i )
		{
			CallBuff kCallBuff;

			wsprintf_e( szKey, "call_buff%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kCallBuff.m_szBuffName = szBuf;

			wsprintf_e( szKey, "call_buff%d_time", i+1 );
			kCallBuff.m_dwBuffCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

			m_CallBuffList.push_back( kCallBuff );
		}

		if( !m_CallBuffList.empty() )
			std::sort( m_CallBuffList.begin(), m_CallBuffList.end(), CallBuffSort() );
	}

	//
	rkLoader.LoadString_e( "except_target_buff", "", szBuf, MAX_PATH );
	m_ExceptTargetBuff = szBuf;

	int iExceptCnt = rkLoader.LoadInt_e( "except_call_buff_cnt", 0 );
	if( iExceptCnt > 0 )
	{
		m_ExceptCallBuffList.reserve( iExceptCnt );

		for( int i=0; i < iExceptCnt; ++i )
		{
			wsprintf_e( szKey, "except_call_buff%d_name", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_ExceptCallBuffList.push_back( szBuf );
		}
	}
}

void ioBuff::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallAttributeList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallAttributeList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		CallAttribute kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_dwAttributeCallTime = (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_randomtime_min", i+1 );
		kCallAttribute.m_iRandomMinTime = rkLoader.LoadInt( szKey, -1);

		wsprintf_e( szKey, "call_attribute%d_randomtime_max", i+1 );
		kCallAttribute.m_iRandomMaxTime = rkLoader.LoadInt( szKey, -1);

		kCallAttribute.m_iRandomMaxTime = max( kCallAttribute.m_iRandomMinTime, kCallAttribute.m_iRandomMaxTime);

		wsprintf_e( szKey, "call_attribute%d_angle", i+1 );
		kCallAttribute.m_fAttributeCallAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "call_attribute%d_range", i+1 );
		kCallAttribute.m_fAttributeCallRange = rkLoader.LoadFloat( szKey, 0.0f );

		m_CallAttributeList.push_back( kCallAttribute );
	}

	std::sort( m_CallAttributeList.begin(), m_CallAttributeList.end(), CallAttributeSort() );
}

void ioBuff::LoadCallAreaWeapon( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_CallAreaWeapon.m_dwAreaWeaponCallTime = rkLoader.LoadInt_e( "call_area_weapon_time", 0 );

	rkLoader.LoadString_e( "call_area_weapon_name", "", szBuf, MAX_PATH );
	m_CallAreaWeapon.m_AreaWeaponName = szBuf;
}

void ioBuff::LoadBuffExceptList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "killing_exception_cnt", 0 );
	m_KillingList.clear();

	for( int i=0; i < iCnt; i++ )
	{
		wsprintf_e( szKey, "killing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szName = szBuf;
		
		if( !szName.IsEmpty() )
			m_KillingList.push_back( szName );
	}

	iCnt = rkLoader.LoadInt_e( "suicide_exception_cnt", 0 );
	m_SuicideList.clear();

	for( int j=0; j < iCnt; j++ )
	{
		wsprintf_e( szKey, "suicide_exception_buff%d", j+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szName = szBuf;
		
		if( !szName.IsEmpty() )
			m_SuicideList.push_back( szName );
	}

	iCnt = rkLoader.LoadInt_e( "remove_by_type_cnt", 0 );
	m_RemoveByTypeList.clear();

	for( int j=0; j < iCnt; j++ )
	{
		wsprintf_e( szKey, "remove_by_type_buff%d", j+1 );
		int iType = rkLoader.LoadInt( szKey, BT_NONE );

		m_RemoveByTypeList.push_back( iType );
	}
}

void ioBuff::SetOwnerItemName( const ioHashString &szName )
{
	m_ItemName = szName;
}

void ioBuff::SetOwnerSkill( ioSkill *pSkill )
{
	m_SkillName.Clear();
	//m_pOwnerItem = NULL;

	if( pSkill )
	{
		m_SkillName = pSkill->GetName();
		//m_pOwnerItem = pSkill->GetOwnerItem();

		//if( m_pOwnerItem )
		if( pSkill->GetOwnerItem() )
		{
			m_ItemName = pSkill->GetOwnerItem()->GetName();
		}
	}
}

void ioBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	rkPacket << m_CreateChar;
	rkPacket << GetName();
	rkPacket << m_ItemName;
	rkPacket << m_SkillName;

	// 공통 GrowthInfo
	rkPacket << m_fTicGaugeRateByGrowth;
	rkPacket << m_dwTicTimeByGrowth;
	rkPacket << m_dwExtraDurationByGrowth;

	DWORD dwGlobalTime = g_GlobalTimer.GetGlobalTime();
	rkPacket << dwGlobalTime;
	if( m_dwBuffEndTime < FRAMEGETTIME() )
	{
		rkPacket << 0;
	}
	else
	{
		DWORD dwGapTime = m_dwBuffEndTime-FRAMEGETTIME();
		rkPacket << dwGapTime;
	}
}

void ioBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
}

CharBuffInfo ioBuff::GetBuffInfoByChangeChar()
{
	CharBuffInfo eBuffInfo;
	eBuffInfo.m_BuffName = GetName();
	eBuffInfo.m_SkillName = m_SkillName;
	eBuffInfo.m_GlobalTime = g_GlobalTimer.GetGlobalTime();

	if( m_dwBuffEndTime < FRAMEGETTIME() )
		eBuffInfo.m_RemainTime = 0;
	else
		eBuffInfo.m_RemainTime = m_dwBuffEndTime-FRAMEGETTIME();

	return eBuffInfo;
}

bool ioBuff::CheckEnableDelayState()
{
	if( m_bEnableDelayState && m_pOwner->GetState() != CS_DELAY )
	{
		SetReserveEndBuff();
		return true;
	}

	return false;
}

bool ioBuff::IsSwitchType()
{
	if( m_iOperationType == OT_SWITCH )
		return true;

	return false;
}

bool ioBuff::IsPassiveType()
{
	if( m_iOperationType == OT_PASSIVE || m_iOperationType == OT_UNLIMITED_PASSIVE )
		return true;

	return false;
}

bool ioBuff::IsUnlimitedPassiveType()
{
	if( m_iOperationType == OT_UNLIMITED_PASSIVE )
		return true;

	return false;
}

void ioBuff::SetCreator( const ioHashString &szCreator )
{
	if( szCreator.IsEmpty() ) return;

	m_CreateChar = szCreator;
}

void ioBuff::SetItemCode( DWORD dwCreateWeaponItemCode )
{
	m_dwCreateWeaponItemCode = dwCreateWeaponItemCode;
}

void ioBuff::SetWeaponIndex( DWORD dwCreateWeaponIndex )
{
	m_dwCreateWeaponIndex = dwCreateWeaponIndex;
}

void ioBuff::SetAreaWeaponIndex( DWORD dwCreateAreaweaponIndex )
{
	m_dwCreateAreawaponIndex = dwCreateAreaweaponIndex;
}

void ioBuff::SetAction()
{
}

void ioBuff::ProcessDelay( float fTimePerSec )
{
	CheckCallingProcess();
}

void ioBuff::CheckCallingBuff()
{
	if( m_CallBuffList.empty() ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwTopTime = m_CallBuffList[0].m_dwBuffCallTime;
	dwTopTime += m_dwBuffStartTime;

	if( dwCurTime >= dwTopTime )
	{
		if( !pOwner->IsProtectState() )
		{
			pOwner->ReserveAddNewBuff( m_CallBuffList[0].m_szBuffName, m_CreateChar, m_ItemName, NULL );
		}
		else if( CheckIgnoreProtectCallingBuff( m_CallBuffList[0].m_szBuffName ) )
		{
			pOwner->ReserveAddNewBuff( m_CallBuffList[0].m_szBuffName, m_CreateChar, m_ItemName, NULL );
		}

		m_CallBuffList.erase( m_CallBuffList.begin() );
	}
}

void ioBuff::CheckExceptCallingBuff()
{
	if( m_ExceptTargetBuff.IsEmpty() ) return;
	if( m_ExceptCallBuffList.empty() ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->HasBuff( m_ExceptTargetBuff ) )
		return;

	int iTotalCnt = m_ExceptCallBuffList.size();
	for( int i=0; i < iTotalCnt; ++i )
	{
		if( !pOwner->IsProtectState() )
		{
			pOwner->ReserveAddNewBuff( m_ExceptCallBuffList[0], m_CreateChar, m_ItemName, NULL );
		}
		else if( CheckIgnoreProtectCallingBuff( m_ExceptCallBuffList[0] ) )
		{
			pOwner->ReserveAddNewBuff( m_ExceptCallBuffList[0], m_CreateChar, m_ItemName, NULL );
		}
	}

	pOwner->RemoveBuff( m_ExceptTargetBuff );
	SetReserveEndBuff();
}

void ioBuff::CheckCallingAttribute()
{
	if( m_CallAttributeList.empty() ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	DWORD dwTopTime = m_CallAttributeList[0].m_dwAttributeCallTime;
	DWORD dwRandomTime = m_CallAttributeList[0].m_dwRandomTime;
	dwTopTime += m_dwBuffStartTime;

	if( dwRandomTime == 0 )
	{
		if( dwCurTime >= dwTopTime )
			CallingAttribute();
	}
	else
	{
		if( dwCurTime >= (dwRandomTime+m_dwBuffStartTime) )
			CallingAttribute();
	}
}


void ioBuff::CallingAttribute()
{
	float fGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetBottomHeight();
	if( m_fCallAttributeEnableHeight > 0.0f && m_fCallAttributeEnableHeight < fGap )
	{
		m_CallAttributeList.erase( m_CallAttributeList.begin() );
		return;
	}

	if( m_bCallExistAttacker )
	{
		ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
		//if( pCreator && pCreator->IsNeedProcess() )
		if( pCreator )
		{
			float fAngle = m_CallAttributeList[0].m_fAttributeCallAngle;
			float fRange = m_CallAttributeList[0].m_fAttributeCallRange;

			D3DXVECTOR3 vPos;
			if( fAngle > 0.0f && fRange > 0.0f )
			{
				vPos = m_pOwner->GetMidPositionByRate();
				D3DXVECTOR3 vDir = m_pOwner->GetMoveDir();
				D3DXVec3Normalize( &vDir, &vDir );
				D3DXQUATERNION	qtNewRot;
				ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );

				D3DXVECTOR3 vAxis = ioMath::UNIT_Y;
				D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fAngle) );
				vDir = qtNewRot * vDir;
				D3DXVec3Normalize( &vDir, &vDir );
				vPos += vDir * fRange;
			}
			else
			{
				vPos = m_pOwner->GetMidPositionByRate();
			}

			DWORD dwWeaponIndex = pCreator->IncreaseWeaponIndexBase() + 1;

			m_vWeaponIndexList.push_back( dwWeaponIndex );

			pCreator->DummyExplicit( m_CallAttributeList[0].m_WeaponInfo,
				vPos,
				dwWeaponIndex,
				false,
				m_OwnerName,
				m_Name,
				m_bUseCallAttWeaponCol, m_bChangeWeaponDirToTargetRot );
		}
	}
	else
	{
		if( !m_pOwner->IsProtectState() )
			m_pOwner->BuffFireExplicit( m_CallAttributeList[0].m_WeaponInfo, m_Name );
		else if( IsIgnoreProtectBuff() )
			m_pOwner->BuffFireExplicit( m_CallAttributeList[0].m_WeaponInfo, m_Name );
	}

	m_CallAttributeList.erase( m_CallAttributeList.begin() );
}


void ioBuff::CheckCallingAreaWeapon()
{
	DWORD dwTopTime = m_CallAreaWeapon.m_dwAreaWeaponCallTime;

	if( dwTopTime == 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	dwTopTime += m_dwBuffStartTime;

	if( dwCurTime >= dwTopTime )
	{
		if( !m_CreateChar.IsEmpty() )
		{
			D3DXQUATERNION qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
			ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.CreateAreaWeapon( m_CreateChar,
																		m_pOwner->GetCharName(),
																		"",
																		m_CallAreaWeapon.m_AreaWeaponName,
																		D3DXVECTOR3(0.0f, 0.0f, 0.0f),
																		qtRot,
																		ioAreaWeapon::CT_TARGET_ATTACHED );

			if( pAreaWeapon )
			{
				m_dwAreaWeaponIndex = pAreaWeapon->GetAreaWeaponIndex();
			}
		}

		m_CallAreaWeapon.m_dwAreaWeaponCallTime = 0;
	}
}

void ioBuff::DestroyAreaWeapon()
{
	if( m_dwAreaWeaponIndex == 0 )
		return;

	g_AreaWeaponMgr.DestroyAreaWeapon( m_dwAreaWeaponIndex );
	m_dwAreaWeaponIndex = 0;
}

bool ioBuff::HasAreaWeapon() const
{
	if( !m_CallAreaWeapon.m_AreaWeaponName.IsEmpty() )
		return true;

	return false;
}

void ioBuff::CheckInvisibleState( bool bVisible )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_bInvisibleState )
		pOwner->SetInvisibleState( !bVisible );
}

void ioBuff::CheckInvisibleWeapon( bool bVisible )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_bInvisibleWeapon )
	{
		if( bVisible )
		{
			pOwner->VisibleEquipItemMesh( ES_WEAPON, m_dwInvisibleWeaponID );
		}
		else
		{
			pOwner->InvisibleEquipItemMesh( ES_WEAPON, m_dwInvisibleWeaponID );
		}
	}
}

void ioBuff::CheckInvisibleCloak( bool bVisible )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_bInvisibleCloak )
	{
		if( bVisible )
			pOwner->VisibleEquipItemMesh( ES_CLOAK, m_dwInvisibleCloakID );
		else
			pOwner->InvisibleEquipItemMesh( ES_CLOAK, m_dwInvisibleCloakID );
	}
}

void ioBuff::CheckInvisibleHelmet( bool bVisible )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_bInvisibleHead && !pOwner->IsObejctEquipState( OES_WEREWOLF ) )
	{
		pOwner->ShowHeadMesh( bVisible );
		return;
	}

	if( m_bInvisibleHelmet )
	{
		if( bVisible )
			pOwner->VisibleEquipItemMesh( ES_HELMET, m_dwInvisibleHelmetID );
		else
			pOwner->InvisibleEquipItemMesh( ES_HELMET, m_dwInvisibleHelmetID );
	}
}

void ioBuff::CheckBuffDesc()
{
	if( Setting::IsHideSkillBuffEmoticon() )
		return;

	if( m_DescName.IsEmpty() ) return;
	if( ioBaseChar::IsOnlyMyEmoticon() && !m_pOwner->IsOwnerChar() ) return;

	if( m_pOwner )
	{
		const vHelpText *pHelpList = g_ModeHelpMgr.GetHelpText( ioModeHelpManager::HT_BUFF );

		if( !pHelpList )
		{
			LOG.PrintTimeAndLog( 0, "ioBuff::CheckBuffDesc() Emoticon is Not Exist" );
			return;
		}

		if( pHelpList->empty() ) return;

		const HelpText &rkText = (*pHelpList->begin());
		DWORD dwColor = rkText.m_dwColor;

		DWORD dwDuration = 0; // Default
		if( m_bDescEqualDuration )
		{
			dwDuration = m_dwBuffEndTime - m_dwBuffStartTime;
			if( dwDuration > GetBuffDuration() + m_dwExtraDurationByGrowth )
			{
				LOG.PrintTimeAndLog(0, "ioBuff::CheckBuffDesc : Big Duration To Default: %d:%d:%d", dwDuration, (DWORD)m_dwBuffEndTime, (DWORD) m_dwBuffStartTime );
				dwDuration = 0; // Default
			}
		}	
		
		m_pOwner->SetNamedEmoticon( m_DescIcon, m_DescName, dwColor );
	}
}

void ioBuff::CheckChangeColor( bool bChange )
{
	if( !m_bChangeColor ) return;

	if( bChange && m_iChangeColorIndex == -1 )
	{
		m_iChangeColorIndex = m_pOwner->ModifyMaterialColor( m_ChangeColor );
	}
	else if( !bChange && m_iChangeColorIndex != -1 )
	{
		m_pOwner->RestoreMaterialColor( m_iChangeColorIndex );
		m_iChangeColorIndex = -1;
	}
}

void ioBuff::SetTicGaugeRateByGrowth( float fRate )
{
	if( !m_bDisableGrowth )
	{
		m_fTicGaugeRateByGrowth = fRate;
	}
}

void ioBuff::SetTicTimeByGrowth( float fTicTime )
{
	if( !m_bDisableGrowth )
		m_dwTicTimeByGrowth = (DWORD)fTicTime;
}

void ioBuff::SetExtraDurationByGrowth( float fDuration )
{
	m_dwExtraDurationByGrowth = 0;

	if( m_bDisableGrowth )
		return;

	if( GetBuffDuration() == 0 ) return;
	if( fDuration == 0.0f ) return;

	m_dwExtraDurationByGrowth = (DWORD)fDuration;
	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration() + m_dwExtraDurationByGrowth;
}

ioGrowthUpInfo* ioBuff::GetGrowthUpInfoForItem( int iType )
{
	if( m_bDisableGrowth )
		return NULL;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pChar = NULL;
	ioItem *pItem = NULL;
	
	if( pStage )
		pChar = pStage->GetBaseChar( m_CreateChar );

	if( pChar )
		pItem = pChar->GetEquipedItem( m_ItemName );

	if( pItem )
	{
		int iGrowthNum = 0;
		int iGrowthInfoNum = pItem->GetGrowthInfoNumByType( iType, iGrowthNum );

		int iLevel = 0;
		int iClassType = 0;
		ioGrowthLevel *pLevel = NULL;


		if( pStage )
		{
			pLevel = pStage->GetGrowthLevel( m_CreateChar );

			if( pChar )
				iClassType = pChar->GetCharacterInfo().m_class_type;
		}

		if( pLevel )
		{
			int iSlot = pItem->GetType() - 1;
			iLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, false );

			if( pItem )
			{
				int nReinforce = pItem->GetItemReinforce();
				//강화레벨에 따른 추가 스탯
				if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
					iLevel += nReinforce;
				else
					iLevel += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );
			}

			if( pChar )
			{
				pChar->GetMedalItemGrowth( iLevel, false, iSlot );

				//펫 성장
				pChar->GetPetGrowth( iLevel, false, iSlot );

				//각성
				pChar->GetAwakeCharGrowth( iLevel, iSlot );
			}
		}

		return g_GrowthInfoMgr.GetGrowthUpInfo( iGrowthInfoNum, iLevel );
	}

	return NULL;
}

bool ioBuff::IsRightOwnerItem()
{
	if( !m_pOwner )
		return false;

	if( m_ItemName.IsEmpty() )
		return false;

	for (int i = 0; i < ES_OBJECT; i++)
	{
		if( !m_pOwner->GetEquipedItem(i) )
			continue;

		if( m_ItemName == m_pOwner->GetEquipedItem( i )->GetName() )
			return true;
	}

	return false;
}

bool ioBuff::IsCanJumpAttack() const
{
	return false;
}

void ioBuff::AccumulateBuff()
{
}

void ioBuff::StartOtherBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && !m_OtherBuff.IsEmpty() )
	{
		pOwner->AddNewBuff( m_OtherBuff, m_CreateChar, m_ItemName, NULL );
	}	
}

void ioBuff::EndOtherBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && !m_OtherBuff.IsEmpty() )
	{
		pOwner->RemoveBuff( m_OtherBuff );
	}	
}

void ioBuff::CheckBuffEndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && !m_BuffEndBuff.IsEmpty() )
	{
		pOwner->ReserveAddNewBuff( m_BuffEndBuff, m_CreateChar, m_ItemName, NULL );
	}	
}


void ioBuff::CheckBuffEndBuff2()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner && !m_BuffEndBuff2.IsEmpty() )
	{
		pOwner->ReserveAddNewBuff( m_BuffEndBuff2, m_CreateChar, m_ItemName, NULL );
	}	
}

bool ioBuff::CheckIgnoreProtectCallingBuff( const ioHashString &szBuffName )
{
	if( !szBuffName.IsEmpty() )
	{
		ioBuff *pBuff = g_BuffMaker.CreateBuff( szBuffName );
		if( !pBuff )
			return false;

		if( pBuff->IsIgnoreProtectBuff() )
		{
			delete pBuff;
			return true;
		}

		delete pBuff;
	}

	return false;
}

ioBaseChar *ioBuff::GetEffectEventOwner()
{
	if( m_CreateChar.IsEmpty() )
		return m_pOwner;

    if( m_pOwner && m_pOwner->GetCreator() )
		return m_pOwner->GetCreator()->GetBaseChar( m_CreateChar );

	return NULL;
}

void ioBuff::CheckChangeSkill( bool bChange )
{
	if( !m_pOwner ) return;

	for( int i=ES_WEAPON; i < MAX_EQUIP_SLOT; ++i )
	{
		ioItem *pItem = m_pOwner->GetEquipedItem( i );
		if( !pItem ) continue;

		pItem->ReserveChangeSkillByBuff( m_Name, bChange );
	}
}

void ioBuff::CheckCallWeaponEnd()
{
	if( m_vWeaponIndexList.empty() )
		return;

	if( !m_bUseCallAttWeaponCol )
		return;

	int iSize = m_vWeaponIndexList.size();
	for( int i=0; i < iSize; ++i )
	{
		int iWeaponIndex = m_vWeaponIndexList[i];
		
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( m_pOwner, iWeaponIndex );
		if( pWeapon && pWeapon->IsLive() )
		{
			pWeapon->SetWeaponDead();
		}
	}
}

bool ioBuff::OnReduceGauge() const
{
	return false;
}

bool ioBuff::UseActiveCount() const
{
	if( m_iMaxActiveCount > 0 )
		return true;

	return false;
}

void ioBuff::CheckActiveCount()
{
	return;
}

void ioBuff::CheckCallingProcess()
{
	CheckCallingBuff();
	CheckExceptCallingBuff();
	CheckCallingAttribute();
	CheckCallingAreaWeapon();
}

void ioBuff::CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff )
{
	m_bRemoveEquipSkillBuff = bRemoveEquipSkillBuff;
}

ioBaseChar* ioBuff::GetOwner()
{
	if( m_OwnerName.IsEmpty() )
		return NULL;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return NULL;

	ioBaseChar *pOwner = pStage->GetBaseChar( m_OwnerName );
	if( pOwner )
		return pOwner;

	return NULL;
}

ioBaseChar* ioBuff::GetCreator()
{
	if( m_CreateChar.IsEmpty() )
		return NULL;

	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( !pStage )
		return NULL;

	ioBaseChar *pCreateChar = pStage->GetBaseChar( m_CreateChar );
	if( pCreateChar )
		return pCreateChar;

	return NULL;
}

ioItem* ioBuff::GetOwnerItem()
{
	if( m_ItemName.IsEmpty() )
		return NULL;

	ioBaseChar *pCreateChar = GetCreator();
	if( pCreateChar )
		return pCreateChar->GetEquipedItem( m_ItemName );

	return NULL;
}

bool ioBuff::CheckSwitchType( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = GetCurTicGauge();
	int iTickGap = GetCurTicGap();
	int iCurTicTime = GetCurTicTime();

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		ioObjectItem *pObject = pOwner->GetObject();
		if( pObject && !pObject->EnableReduceGaugeOnSwitchBuff() && pObject->GetItemCode() == m_dwCreateWeaponItemCode )
			return true;

		if( fCurTicGauge != 0.0f )
		{
			if( !IsRightOwnerItem() )
				return false;

			if( pOwner->ReduceCurSkillGaugeBySkill( m_ItemName, fCurTicGauge ) <= 0.0f )
				return false;
		}
	}

	return true;
}

float ioBuff::GetCurTicGauge()
{
	return m_fGaugePerTick / m_fTicGaugeRateByGrowth;
}

int ioBuff::GetCurTicGap()
{
	DWORD dwCurTime = FRAMEGETTIME();
	return (int)( dwCurTime - m_dwCheckTime );
}

int ioBuff::GetCurTicTime()
{
	ioBaseChar *pOwner = GetOwner();
	ioPlayStage *pStage = NULL;
	if( pOwner )
		pStage = pOwner->GetCreator();

	if( m_dwFCModeTickTime > 0 && pStage && pStage->GetModeType() == MT_FIGHT_CLUB )
		return (m_dwFCModeTickTime + m_dwTicTimeByGrowth);

	if( m_dwTDModeTickTime > 0 && pStage && ((pStage->GetModeType() == MT_MONSTER_SURVIVAL) || (pStage->GetModeType() == MT_RAID) ||
		(Help::IsMonsterDungeonMode(pStage->GetModeType())) )) 
		return (m_dwTDModeTickTime + m_dwTicTimeByGrowth);

	return (m_dwTickTime + m_dwTicTimeByGrowth);
}

void ioBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
}

bool ioBuff::IsCharCollisionSkip() const
{
	return m_bCharCollisionSkip;
}

bool ioBuff::IsCollisionAvailableWeapon() const
{
	return true;
}

bool ioBuff::HasExceptSuicideBuff( ioBaseChar* pChar ) const
{
	for( ioHashStringVec::const_iterator iter = m_SuicideList.begin(); iter != m_SuicideList.end(); ++iter )
	{
		if( pChar && pChar->HasBuff( *iter ) )
		{
			return true;
		}
	}

	return false;
}

void ioBuff::ThisCallAndErase( bool b )
{
	m_bCallErased = b;
}

void ioBuff::PlayPassiveBuffEffect()
{
	if( m_pOwner && !m_PassiveBuffEffect.IsEmpty() )
	{
		ioEffect* pEffect = m_pOwner->GetEffectByFileName( m_PassiveBuffEffect );

		DWORD dwCurrentTime = FRAMEGETTIME();
		if( !pEffect && m_dwPassiveBuffEffectStartTime + m_dwPassiveBuffIdleTime < dwCurrentTime )
		{
			m_pOwner->AttachEffect( m_PassiveBuffEffect, GetEffectEventOwner() );
			m_dwPassiveBuffEffectStartTime = dwCurrentTime;
		}
	}
}

bool ioBuff::SetBuffGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	return false;
}

D3DXVECTOR3 ioBuff::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	return ioMath::VEC3_ZERO;
}

bool ioBuff::IsProtected() const
{
	return false;
}

//Accessory
void ioBuff::SetBuffCategory( int iCategory )
{
	m_iBuffCategory = iCategory;
}

// 2020-11-02
bool ioBuff::IsDuplicateBuff( ioBuff *pBuff )
{
	if( !pBuff ) return true;

	if( m_bIsDuplicateBuff )
	{
		// 현재 걸린 버프 
		// pBuff 새로운 버프
		// buff를 생성한 캐릭터 와 같은지 비교 해서 같으면, 삭제
		if( m_CreateChar != pBuff->GetCreatorName() )
		{
			LOG.PrintTimeAndLog( 0, "ioBuff::IsDuplicateBuff - %s != %s",m_CreateChar.c_str(), pBuff->GetCreatorName().c_str());
			return false;
		}
	}

	return true;
}