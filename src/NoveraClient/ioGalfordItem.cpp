#include "stdafx.h"

#include "ioGalfordItem.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioZoneEffectWeapon.h"
#include "ioPappyDummy.h"

ioGalfordItem::ioGalfordItem()
{
	m_ChargeState			= CS_NONE;
	m_iCurCombo				= 0;
	m_iEquipDummyCharIndex = -1;
	m_dwJumpKeyCounterAttackEffectID = -1;
	m_fCurBullet = 0.0f;
	m_dwShadowCopyStartEffectID = -1;
	Init();
	ClearCmdInfo();
	m_CurGalfordCmdInfo.Init();
}

ioGalfordItem::ioGalfordItem( const ioGalfordItem &rhs )
:ioWeaponItem( rhs ),
//Dummy
m_EquipDummyCharName( rhs.m_EquipDummyCharName ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
//Charge Attack
m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
//Cmd Input
m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
m_dwInputGapTime( rhs.m_dwInputGapTime ),
m_GalfordCmdInfoList( rhs.m_GalfordCmdInfoList ),
//Counter Attack
m_JumpKeyCounterAttack( rhs.m_JumpKeyCounterAttack ),
m_szJumpKeyCounterAttackEffect( rhs.m_szJumpKeyCounterAttackEffect ),
m_vJumpKeyCANoneTargetTeleportOffset( rhs.m_vJumpKeyCANoneTargetTeleportOffset ),
m_vJumpKeyCATargetTeleportOffset( rhs.m_vJumpKeyCATargetTeleportOffset ),
m_fJumpKeyCATargetRange( rhs.m_fJumpKeyCATargetRange ),
m_fJumpKeyCATargetAngle( rhs.m_fJumpKeyCATargetAngle ),
m_JumpKeyCALandingAttack( rhs.m_JumpKeyCALandingAttack ),
m_fJumpKeyCAMaxMoveRange( rhs.m_fJumpKeyCAMaxMoveRange ),
m_fJumpKeyCAEndJumpPower( rhs.m_fJumpKeyCAEndJumpPower ),
m_fJumpKeyCAGravityRate( rhs.m_fJumpKeyCAGravityRate ),
m_fPappyAttackEndJumpPower( rhs.m_fPappyAttackEndJumpPower ),
m_fPappyAttackEndLandingRate( rhs.m_fPappyAttackEndLandingRate ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreaseGaugeTic( rhs.m_fIncreaseGaugeTic ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fJumpKeyCADecreaseBullet( rhs.m_fJumpKeyCADecreaseBullet )
{
	m_ChargeState			= CS_NONE;

	m_iCurCombo				= 0;
	m_iEquipDummyCharIndex = -1;
	m_dwJumpKeyCounterAttackEffectID = -1;
	m_fCurBullet = 0.0f;
	m_dwShadowCopyStartEffectID = -1;

	Init();
	ClearCmdInfo();
	m_CurGalfordCmdInfo.Init();
}

ioGalfordItem::~ioGalfordItem()
{
}

void ioGalfordItem::Init()
{	
	m_dwAttackStartTime		= 0;
	m_dwExtraAniJumpTime = 0;
	m_fExtraJumpPower = 0.0f;
	m_fExtraJumpAngle = 0.0f;
	m_fReplicaCurMoveRange = 0.0f;
	m_fReplicaBeforeHeight = 0.0f;
	m_fJumpKeyCACurMoveRange = 0.0f;
	m_fJumpKeyCABeforeHeight = 0.0f;
	m_iGalfordCmdIndex = -1;
	m_iCurCmdCombo = 0;
	m_iCurCmdMaxCombo = 0;
}

void ioGalfordItem::ClearCmdInfo()
{
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_iCurCmdIndex = 0;
	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_dwInputStartTime = 0;
	m_dwInputCheckTime = 0;
	
}

void ioGalfordItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	rkLoader.LoadString_e( "equip_dummy_char_name", "", szBuf, MAX_PATH );
	m_EquipDummyCharName = szBuf;

	// ready ani
	int iMaxCombo = GetMaxCombo();
	m_vExtendAttributeList.clear();
	if( iMaxCombo > 0 )
		m_vExtendAttributeList.reserve( iMaxCombo );

	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szKey) );

		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	//cmd
	LoadCommand( rkLoader );

	//Jump key Counter Attack
	LoadJumpKeyCounterAttack( rkLoader );

	m_fPappyAttackEndJumpPower = rkLoader.LoadFloat_e( "pappy_attack_end_jump_power", 0.0f );
	m_fPappyAttackEndLandingRate = rkLoader.LoadFloat_e( "pappy_attack_end_landing_rate", FLOAT1 );

	m_fIncreaseGaugeTic = rkLoader.LoadFloat_e( "increase_gauge_tic", 0.0f );
	m_fMaxBullet = rkLoader.LoadFloat_e( "max_gauge", 0.0f );

	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge",0.0f );

	m_fJumpKeyCADecreaseBullet = rkLoader.LoadFloat_e( "jump_key_counter_attack_decrease_gauge", 0.0f );
}

void ioGalfordItem::LoadCommand( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_dwInputMaxTime = (DWORD)rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputGapTime = (DWORD)rkLoader.LoadInt_e( "input_gap_time", 0 );
	m_dwInputStartGapTime = (DWORD)rkLoader.LoadInt_e( "input_start_gap_time", 0 );

	int iCmdMaxInfo = rkLoader.LoadInt_e( "max_cmd_info_cnt", 0 );
	for( int i=0; i < iCmdMaxInfo; ++i )
	{
		GalfordCmdInfo kInfo;
		kInfo.Init();

		wsprintf_e( szBuf, "cmd_info%d", i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		kInfo.m_Cmd = szKey;

		wsprintf_e( szBuf, "cmd_info%d_type", i+1 );
		kInfo.m_GalfordCmdState = (GalfordCmdState)rkLoader.LoadInt( szBuf, GCS_NONE );

		wsprintf_e( szBuf, "cmd_info%d", i+1 );
		LoadCMDState( rkLoader, kInfo, kInfo.m_GalfordCmdState, szBuf );

		wsprintf_e( szBuf, "cmd_info%d_skill_slot", i+1 );
		kInfo.m_iSkillSlot = rkLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "cmd_info%d_gravity_rate", i+1 );
		kInfo.m_fGravityRate = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "cmd_info%d_decrease_gauge", i+1 );
		kInfo.m_fDecreseGauge = rkLoader.LoadFloat( szBuf, 0.0f );

		m_GalfordCmdInfoList.push_back( kInfo );
	}
}

void ioGalfordItem::LoadCMDState( ioINILoader &rkLoader, GalfordCmdInfo& rkInfo, int CmdState, const char *szInfo )
{
	switch( CmdState )
	{
	case GCS_REPLICA_ATTACK:
		LoadReplicaAttack( rkLoader, rkInfo.m_ReplicaInfo, szInfo );
		break;
	case GCS_SHADOW_COPY:
		LoadShadowCopy( rkLoader, rkInfo.m_ShadowCopyInfo, szInfo );
		break;
	case GCS_STRIKE_HEADS:
		LoadStrikerHeads( rkLoader, rkInfo.m_StrikeHeadsInfo, szInfo );
		break;
	case GCS_PAPPY:
		LoadPappyAttack( rkLoader, rkInfo, szInfo );
		break;
	}
}

void ioGalfordItem::LoadReplicaAttack( ioINILoader &rkLoader, ReplicaInfo& rkInfo, const char *szInfo )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	wsprintf_e( szBuf, "%s_replica_loop_ani", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_szReplicaLoopAni = szKey;

	wsprintf_e( szBuf, "%s_replica_loop_ani_rate", szInfo );
	rkInfo.m_fReplicaLoopAniRate = rkLoader.LoadInt( szBuf, 0 );

	wsprintf_e( szBuf, "%s_replica_attack_type_count", szInfo );
	int iMaxCount = rkLoader.LoadInt( szBuf, 0 );
	for ( int i=0; i<iMaxCount ; ++i )
	{
		WeaponInfo kWeaponInfo;

		wsprintf_e( szKey, "%s_replica_attack_type%d", szInfo, i+1 );
		kWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_replica_attack_type%d_resist", szInfo, i+1 );
		kWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "%s_replica_attack_type%d_wounded", szInfo, i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kWeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "%s_replica_attack_type%d_wounded_time", szInfo, i+1 );
		kWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_replica_attack_type%d_wounded_loop_ani", szInfo, i+1 );
		kWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		rkInfo.m_ReplicaWeaponInfoList.push_back( kWeaponInfo );
	}

	wsprintf_e( szBuf, "%s_replica_end_attack", szInfo );
	LoadAttackAttribute( szBuf, rkInfo.m_ReplicaEndAttack, rkLoader );


	wsprintf_e( szBuf, "%s_replica_target_range", szInfo );
	rkInfo.m_fReplicaTargetRange = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_replica_target_angle", szInfo );
	rkInfo.m_fReplicaTargetAngle = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_replica_target_offset_x", szInfo );
	rkInfo.m_vReplicaTargetOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_replica_target_offset_y", szInfo );
	rkInfo.m_vReplicaTargetOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_replica_target_offset_z", szInfo );
	rkInfo.m_vReplicaTargetOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_replica_none_target_offset_x", szInfo );
	rkInfo.m_vReplicaNoneTargetOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_replica_none_target_offset_y", szInfo );
	rkInfo.m_vReplicaNoneTargetOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_replica_none_target_offset_z", szInfo );
	rkInfo.m_vReplicaNoneTargetOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_replica_max_move_range", szInfo );
	rkInfo.m_fReplicaMaxMoveRange = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_replica_end_jump_power", szInfo );
	rkInfo.m_fReplicaEndJumpPower= rkLoader.LoadFloat( szBuf, 0.0f );
	
}

void ioGalfordItem::LoadShadowCopy( ioINILoader &rkLoader, ShadowCopyInfo& rkInfo, const char *szInfo )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	//Dummy
	wsprintf_e( szBuf, "%s_shadow_dummy_name", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_szShadowDummyName = szKey;

	wsprintf_e( szBuf, "%s_owner_dummy_name", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_szOwnerDummyName = szKey;
	
	//Offset
	wsprintf_e( szBuf, "%s_shadow_dummy_offset_x", szInfo );
	rkInfo.m_vShadowDummyOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_shadow_dummy_offset_y", szInfo );
	rkInfo.m_vShadowDummyOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_shadow_dummy_offset_z", szInfo );
	rkInfo.m_vShadowDummyOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_owner_dummy_offset_x", szInfo );
	rkInfo.m_vOwnerDummyOffset.x = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_owner_dummy_offset_y", szInfo );
	rkInfo.m_vOwnerDummyOffset.y = rkLoader.LoadFloat( szBuf, 0.0f );
	wsprintf_e( szBuf, "%s_owner_dummy_offset_z", szInfo );
	rkInfo.m_vOwnerDummyOffset.z = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_shadow_copy_duration", szInfo );
	rkInfo.m_dwShadowCopyDuration = (DWORD)rkLoader.LoadInt( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_shadow_copy_enable_key_time", szInfo );
	rkInfo.m_dwShadowCopyEnableKeyTime = (DWORD)rkLoader.LoadInt( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_shadow_copy_end_jump_power", szInfo );
	rkInfo.m_fShadowCopyEndJumpPower = rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_shadow_copy_start_effect", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_fShadowCopyStartEffect = szKey;

	wsprintf_e( szBuf, "%s_shadow_copy_max_buff_count", szInfo );
	int iMaxBuffCount = rkLoader.LoadInt( szBuf, 0 );
	for( int i=0; i < iMaxBuffCount; ++i )
	{
		ioHashString szName;
		wsprintf_e( szBuf, "%s_shadow_copy_buff%d", szInfo, i+1 );
		rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
		szName = szKey;
		rkInfo.m_vShadowCopyBuffList.push_back( szName );
	}
}

void ioGalfordItem::LoadStrikerHeads( ioINILoader &rkLoader, StrikeHeadsInfo& rkInfo, const char *szInfo )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	wsprintf_e( szBuf, "%s_strike_heads_attack", szInfo );
	LoadAttackAttribute( szBuf, rkInfo.m_StrikeHeadsAttack, rkLoader );

	wsprintf_e( szBuf, "%s_strike_heads_loop_motion", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_GrapplingLoopMotion = szKey;

	wsprintf_e( szBuf, "%s_strike_heads_loop_motion_rate", szInfo );
	rkInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat( szBuf, FLOAT1 );

	wsprintf_e( szBuf, "%s_strike_heads_land_attack", szInfo );
	LoadAttackAttribute( szBuf, rkInfo.m_StrikeLandAttack, rkLoader );
	
	wsprintf_e( szBuf, "%s_strike_heads_jump_power", szInfo );
	rkInfo.m_fJumpPower = rkLoader.LoadFloat( szBuf, 0.0f );

	rkInfo.m_TargetGrapplingInfo.Init();
	wsprintf_e( szBuf, "%s_grappling_target_reverse_rotate", szInfo );
	rkInfo.m_TargetGrapplingInfo.m_bReverseTargetRot	= rkLoader.LoadBool( szBuf, false );

	wsprintf_e( szBuf, "%s_grappling_target_enable_col", szInfo );
	rkInfo.m_TargetGrapplingInfo.m_GrapplingColType	= (GrapplingColType)rkLoader.LoadInt( szBuf, GCT_ALL );

	wsprintf_e( szBuf, "%s_grappling_target_loop_motion", szInfo );
	rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
	rkInfo.m_TargetGrapplingInfo.m_GrapplingLoopMotion		= szKey;
	rkInfo.m_TargetGrapplingInfo.m_GrapplingLoopMotion2		= szKey;

	wsprintf_e( szBuf, "%s_grappling_target_loop_motion_rate", szInfo );
	rkInfo.m_TargetGrapplingInfo.m_fGrapplingLoopMotionRate	= rkLoader.LoadFloat( szBuf, FLOAT1 );

	wsprintf_e( szBuf, "%s_grappling_target_gap_range", szInfo );
	rkInfo.m_fGrapplingOffsetGapRange			= rkLoader.LoadFloat( szBuf, 0.0f );

	wsprintf_e( szBuf, "%s_grappling_target_height_rate", szInfo );
	rkInfo.m_fGrapplingOffsetHeightRate		= rkLoader.LoadFloat( szBuf, FLOAT05 );
}

void ioGalfordItem::LoadPappyAttack( ioINILoader &rkLoader, GalfordCmdInfo& rkInfo, const char *szInfo )
{
	char szBuf[MAX_PATH];
	wsprintf_e( szBuf, "%s_attack", szInfo );
	LoadAttackAttribute( szBuf, rkInfo.m_Attack, rkLoader );

	wsprintf_e( szBuf, "%s_max_attack_count", szInfo );
	rkInfo.m_iMaxAttackCount = rkLoader.LoadInt( szBuf, 0 );
}

void ioGalfordItem::LoadJumpKeyCounterAttack( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	LoadAttackAttribute_e( "jump_key_count_attack", m_JumpKeyCounterAttack, rkLoader );

	rkLoader.LoadString_e( "jump_key_count_attack_effect", "", szBuf, MAX_PATH );
	m_szJumpKeyCounterAttackEffect = szBuf;

	m_vJumpKeyCANoneTargetTeleportOffset.x = rkLoader.LoadFloat_e( "jump_key_counter_attack_none_target_offset_x", 0.0f );
	m_vJumpKeyCANoneTargetTeleportOffset.y = rkLoader.LoadFloat_e( "jump_key_counter_attack_none_target_offset_y", 0.0f );
	m_vJumpKeyCANoneTargetTeleportOffset.z = rkLoader.LoadFloat_e( "jump_key_counter_attack_none_target_offset_z", 0.0f );

	m_vJumpKeyCATargetTeleportOffset.x = rkLoader.LoadFloat_e( "jump_key_counter_attack_target_offset_x", 0.0f );
	m_vJumpKeyCATargetTeleportOffset.y = rkLoader.LoadFloat_e( "jump_key_counter_attack_target_offset_y", 0.0f );
	m_vJumpKeyCATargetTeleportOffset.z = rkLoader.LoadFloat_e( "jump_key_counter_attack_target_offset_z", 0.0f );

	m_fJumpKeyCAMaxMoveRange = rkLoader.LoadFloat_e( "jump_key_counter_attack_max_move_range", 0.0f );
	m_fJumpKeyCAEndJumpPower = rkLoader.LoadFloat_e( "jump_key_counter_attack_end_jump_power", 0.0f );

	m_fJumpKeyCATargetRange = rkLoader.LoadFloat_e( "jump_key_counter_attack_target_range", 0.0f );
	m_fJumpKeyCATargetAngle = rkLoader.LoadFloat_e( "jump_key_counter_attack_target_angle", 0.0f );

	LoadAttackAttribute_e( "jump_key_count_landing_attack", m_JumpKeyCALandingAttack, rkLoader );

	m_fJumpKeyCAGravityRate = rkLoader.LoadFloat_e( "jump_key_counter_attack_gravity_rate", FLOAT1 );
}

ioItem* ioGalfordItem::Clone()
{
	return new ioGalfordItem( *this );
}

ioWeaponItem::WeaponSubType ioGalfordItem::GetSubType() const
{
	return WST_GALFORD_ITEM;
}

void ioGalfordItem::OnEquiped( ioBaseChar *pOwner )
{
	ioAttackableItem::OnEquiped( pOwner );
	CreateEquipDummyChar( pOwner );
}

void ioGalfordItem::OnReleased( ioBaseChar *pOwner )
{
	ioAttackableItem::OnReleased( pOwner );
	DestroyEquipDummyChar( pOwner );
}

int ioGalfordItem::CreateDummyChar( ioBaseChar *pOwner, const ioHashString& szDummyName, D3DXVECTOR3 vPos )
{
	if ( !pOwner )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();

	ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyName, iCurIndex,
		pOwner->GetCharName(),
		vPos,
		0.0f,
		0, true );

	if ( pDummy )
		return iCurIndex;
	else
		return -1;
}

void ioGalfordItem::DestroyDummyChar( ioBaseChar *pOwner, const ioHashString& szDummyName )
{
	g_DummyCharMgr.DestroyDummyCharByName( pOwner, szDummyName );
}

void ioGalfordItem::CreateEquipDummyChar( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if ( pOwner->IsSystemState() )
		return;

	DestroyEquipDummyChar( pOwner );
	m_iEquipDummyCharIndex = CreateDummyChar( pOwner, m_EquipDummyCharName, pOwner->GetWorldPosition() );
}

void ioGalfordItem::DestroyEquipDummyChar( ioBaseChar *pOwner )
{
	DestroyDummyChar( pOwner, m_EquipDummyCharName );
	m_iEquipDummyCharIndex = -1;
}

//Normal Attack
void ioGalfordItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	Init();
	ClearCmdInfo();

	InitAniRotate();

	DWORD dwCurTime = FRAMEGETTIME();
	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = 0;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = dwCurTime;
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = dwCurTime;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioGalfordItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		OnNormalAttack( pOwner );
		break;
	case CS_ATTACK_FIRE:
		OnAttackFire( pOwner );
		break;
	}
}

void ioGalfordItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToAttackFire( pOwner );
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioGalfordItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_NORMAL;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	int iExtendCnt = m_vExtendAttributeList.size();
	if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
	{
		m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
		m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
		float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		SetAniRotate( pOwner, iAniID, fTimeRate );
	}

	if( iExtendCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_ATTACK_FIRE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::OnNormalAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner->IsNeedProcess() )
	{
		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
		
		if( CheckCmdInputState( pOwner ) )
			return;
		/*
		if( CheckSkillInputState( pOwner ) )
			return;
			*/
	}
	ioWeaponItem::CheckNormalAttack( pOwner );
}

void ioGalfordItem::OnAttackFire( ioBaseChar *pOwner )
{
	pOwner->CheckExtraAniJump();

	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->CheckExtendAttackEndJump() )
			return;

		CheckCancelReserve( pOwner );

		if( ProcessCancel(pOwner) )
			return;
		
		if( CheckCmdInputState( pOwner ) )
			return;
		/*
		if( CheckSkillInputState( pOwner ) )
			return;
			*/
	}

	CheckAniRotate( pOwner );
	ioWeaponItem::CheckNormalAttack( pOwner );
}

//CMD
bool ioGalfordItem::CheckCmdInputState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	if( !pOwner->IsNeedProcess() )
		return false;

	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return false;

	if( CheckCmdInputTime() )
	{
		return ChangeToCmdSet( pOwner );
	}

	CheckCmdInput( pOwner );

	return false;
}

bool ioGalfordItem::CheckCmdInputTime()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwInputStartTime+m_dwInputMaxTime < dwCurTime )
	{
		return true;
	}

	if( m_dwInputCheckTime < dwCurTime )
	{
		return true;
	}

	return false;
}

bool ioGalfordItem::CheckCmdInput( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	int iCurMaxCmd = MAX_COMMAND_CNT - 1;
	iCurMaxCmd = max( 0, iCurMaxCmd );
	DWORD dwCurTime = FRAMEGETTIME();

	ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
	if( m_PreDirKey == eDirKey )
		return true;

	char szChar = '\0';

	switch( eDirKey )
	{
	case ioUserKeyInput::DKI_NONE:
		m_PreDirKey = eDirKey;
		return true;
	case ioUserKeyInput::DKI_RIGHTUP:
	case ioUserKeyInput::DKI_RIGHTDOWN:
	case ioUserKeyInput::DKI_LEFTDOWN:
	case ioUserKeyInput::DKI_LEFTUP:
		return true;
	case ioUserKeyInput::DKI_UP:
		szChar = '1';
		break;
	case ioUserKeyInput::DKI_RIGHT:
		szChar = '2';
		break;
	case ioUserKeyInput::DKI_DOWN:
		szChar = '3';
		break;
	case ioUserKeyInput::DKI_LEFT:
		szChar = '4';
		break;
	}

	if( !COMPARE( m_iCurCmdIndex, 0, iCurMaxCmd ) )
		return false;

	m_CurCmdInfo[m_iCurCmdIndex] = szChar;

	m_PreDirKey = eDirKey;
	m_iCurCmdIndex++;
	m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

	return true;
}

bool ioGalfordItem::ChangeToCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if( m_GalfordCmdInfoList.empty() )
	{
		m_iCurCmdCombo = 0;
		ClearCmdInfo();
		m_CurGalfordCmdInfo.Init();
		return false;
	}

	if( m_CurGalfordCmdInfo.m_Cmd.IsEmpty() )
		return ChangeToNewCmdSet( pOwner );

	return ChangeToComboCmdSet( pOwner );
}

bool ioGalfordItem::ChangeToNewCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	for ( int i=0; i<(int)m_GalfordCmdInfoList.size() ; ++i )
	{
		GalfordCmdInfo& rkInfo = m_GalfordCmdInfoList[i];
		if( strcmp(rkInfo.m_Cmd.c_str(), m_CurCmdInfo) == 0 )
		{
			if( rkInfo.m_GalfordCmdState == GCS_SKILL )
			{
				if( !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(rkInfo.m_iSkillSlot, SUT_NORMAL))
				{
					m_ChargeState = CS_NONE;

					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_ChargeState;
						kPacket << m_iCurCombo;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					return false;
				}

				m_iGalfordCmdIndex = -1;
				ClearCmdInfo();

				return true;
			}
			else if ( CheckGalfordCmdState( rkInfo.m_GalfordCmdState ) && (int)m_fCurBullet > 0 )
			{
				m_iCurCmdCombo = 0;
				m_iGalfordCmdIndex = i;
				m_CurGalfordCmdInfo = rkInfo;
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;
				SetGalfordCmdAttack( pOwner, m_CurGalfordCmdInfo );

				ClearCmdInfo();
				return true;
			}
			else if ( CheckPappyCmd( rkInfo.m_GalfordCmdState ) && (int)m_fCurBullet > 0 )
			{
				if( !CheckEnablePappyCmd( pOwner, rkInfo.m_GalfordCmdState ) )
				{
					ClearCmdInfo();

					return false;
				}

				m_iCurCmdCombo = 0;
				m_iGalfordCmdIndex = i;
				m_CurGalfordCmdInfo = rkInfo;
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;

				SetPappyCmdAttack( pOwner );
				ClearCmdInfo();
				return true;
			}
		}
	}

	ClearCmdInfo();
	m_iCurCmdCombo = 0;
	m_CurGalfordCmdInfo.Init();

	return false;
}

bool ioGalfordItem::CheckGalfordCmdState( GalfordCmdState eUseType )
{
	switch( eUseType )
	{
	case GCS_REPLICA_ATTACK:
	case GCS_SHADOW_COPY:
	case GCS_STRIKE_HEADS:
		return true;
	}

	return false;
}

bool ioGalfordItem::ChangeToComboCmdSet( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	if ( m_CurGalfordCmdInfo.m_Cmd.IsEmpty() )
		return ChangeToNewCmdSet( pOwner );

	if( strcmp(m_CurGalfordCmdInfo.m_Cmd.c_str(), m_CurCmdInfo) != 0 )
	{
		return ChangeToNewCmdSet( pOwner );
	}

	if( !COMPARE( m_iCurCmdCombo, 0, m_iCurCmdMaxCombo ) )
	{
		return false;
	}

	if ( CheckGalfordCmdState( m_CurGalfordCmdInfo.m_GalfordCmdState ) && (int)m_fCurBullet > 0 )
	{
		SetGalfordCmdAttack( pOwner, m_CurGalfordCmdInfo );
		ClearCmdInfo();
		return true;
	}
	else if ( CheckPappyCmd( m_CurGalfordCmdInfo.m_GalfordCmdState ) && (int)m_fCurBullet > 0 )
	{
		if( !CheckEnablePappyCmd( pOwner, m_CurGalfordCmdInfo.m_GalfordCmdState ) )
		{
			m_iCurCmdIndex = 0;
			ClearCmdInfo();
			return false;
		}

		SetPappyCmdAttack( pOwner );
		
		ClearCmdInfo();
		return true;
	}

	return true;
}

bool ioGalfordItem::CheckPappyCmd( GalfordCmdState eUseType )
{
	if ( eUseType == GCS_PAPPY )
		return true;
	return false;
}

bool ioGalfordItem::CheckEnablePappyCmd( ioBaseChar *pOwner, GalfordCmdState eUseType )
{
	ioPappyDummy *pPappy = ToPappyDummy( g_DummyCharMgr.FindDummyChar( pOwner, m_iEquipDummyCharIndex ) );
	if ( pPappy )
		return pPappy->CheckEnableUseCmd();

	return false;
}

void ioGalfordItem::SetActionAni( ioBaseChar *pOwner, const AttackAttribute& rkAttri, bool bRefreshFireTimeList /* = true */ )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "is not exist ani motion" );
		return;
	}

	float fTimeRate = rkAttri.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = rkAttri.m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( bRefreshFireTimeList && !rkAttri.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeList( iAniID,
			rkAttri.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + rkAttri.m_dwEndDelay;

	// Col Skip
	pOwner->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay + dwCurTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( rkAttri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	SetCurAttackAniForInputJump( rkAttri.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	AniEventConstPtrList vExtraAniJumpList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );
	if( !vExtraAniJumpList.empty() )
	{
		AniEventConstPtrList::iterator iter = vExtraAniJumpList.begin();
		m_dwExtraAniJumpTime = (*iter)->fEventTime * fTimeRate;
		m_dwExtraAniJumpTime += FRAMEGETTIME();
		m_dwExtraAniJumpTime += dwPreDelay;

		if( !(*iter)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
			if( vParam.size() == 2 )
			{
				m_fExtraJumpPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				m_fExtraJumpAngle = ioStringConverter::ParseFloat( vParam[1].c_str() );
			}
		}

	}
	
}

void ioGalfordItem::SetGalfordCmdAttack( ioBaseChar *pOwner, GalfordCmdInfo kinfo )
{
	switch( kinfo.m_GalfordCmdState )
	{
	case GCS_REPLICA_ATTACK:
		SetReplicaAttack( pOwner );
		break;
	case GCS_SHADOW_COPY:
		SetShadowCopy( pOwner );
		break;
	case GCS_STRIKE_HEADS:
		SetStrikeHeadsAttack( pOwner );
		break;
	}
	if ( pOwner->IsNeedProcess() )
	{
		m_iCurCmdCombo++;
		DecreaseGauge( m_CurGalfordCmdInfo.m_fDecreseGauge );
	}
}

void ioGalfordItem::SetReplicaAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CurGalfordCmdInfo.m_ReplicaInfo.m_szReplicaLoopAni );
	if ( iAniID == -1 )
		return;
	float fTimeRate  = m_CurGalfordCmdInfo.m_ReplicaInfo.m_fReplicaLoopAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;
	
	ClearCancelInfo();
	pOwner->ClearReservedSliding();
	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		pOwner->SetState( CS_GALFORD_CMD_STATE );
	m_SpecialState = SS_REPLICA_ATTACK;
	
	m_fReplicaCurMoveRange = 0.0f;
	
	if ( pOwner->IsNeedProcess() )
		SetReplicaPosition( pOwner );

	m_fReplicaBeforeHeight = pOwner->GetWorldPosition().y;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pOwner->RefreshFireTimeList( iAniID, m_CurGalfordCmdInfo.m_ReplicaInfo.m_ReplicaWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_REPLICA;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iGalfordCmdIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::SetReplicaPosition( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioHashString szTarget = FindAttTarget( pOwner, m_CurGalfordCmdInfo.m_ReplicaInfo.m_fReplicaTargetAngle, m_CurGalfordCmdInfo.m_ReplicaInfo.m_fReplicaTargetRange );
	ioBaseChar* pTarget = pOwner->GetBaseChar( szTarget );
	if ( pTarget )
	{
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vNewPos = pTarget->GetWorldOrientation() * m_CurGalfordCmdInfo.m_ReplicaInfo.m_vReplicaTargetOffset;
		vNewPos += vTargetPos;

		pOwner->SetWorldPosition( vNewPos );

		D3DXVECTOR3 vDir = vTargetPos - vNewPos;
		D3DXVec3Normalize( &vDir, &vDir );
		pOwner->SetTargetRotToDir( vDir, true );
	}
	else
	{
		D3DXVECTOR3 vNewPos = pOwner->GetWorldOrientation() * m_CurGalfordCmdInfo.m_ReplicaInfo.m_vReplicaNoneTargetOffset;
		vNewPos += pOwner->GetWorldPosition();
		pOwner->SetWorldPosition( vNewPos );
	}
}

ioHashString ioGalfordItem::FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	ioHashString szTarget;
	if( !pOwner )
		return szTarget;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return szTarget;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = fRange * fRange;
	float fCurGap = -FLOAT1;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pOwner )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() ) 
			continue;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	return szTarget;
}

void ioGalfordItem::ProcessGalfordCmdSpecialState( ioBaseChar *pOwner )
{
	switch( m_SpecialState )
	{
	case SS_REPLICA_ATTACK:
		OnReplicaAttack( pOwner );
		break;
	case SS_REPLICA_END_ATTACK:
		OnReplicaEndAttack( pOwner );
		break;
	case SS_SHADOW_COPY:
		OnShadowCopy( pOwner );
		break;
	case SS_STRIKE_HEADS:
		OnStrikeHeadsAttack( pOwner );
		break;
	case SS_STRIKE_HEADS_LOOP:
		OnStrikeHeadsLoopAttack( pOwner );
		break;
	case SS_STRIKE_HEADS_LAND_ATTACK:
		OnStrikeHeadsLandAttack( pOwner );
		break;
	case SS_PAPPY_ATTACK:
		OnPappyAttack( pOwner );
		break;
	case SS_JUMP_KEY_COUNTER_ATTACK:
		OnJumpKeyCounterAttack( pOwner );
		break;
	case SS_JUMP_KEY_COUNTER_LAND_ATTACK:
		OnJumpKeyCounterLandAttack( pOwner );
		break;
	}

	CheckExtraAniJump( pOwner );
}

void ioGalfordItem::OnReplicaAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( CheckLanding( pOwner ) )
	{
		SetReplicaEndAttack( pOwner );
		return;
	}

	float fCurHeight = pOwner->GetWorldPosition().y;
	float fGap = fabs( m_fReplicaBeforeHeight - fCurHeight );
	m_fReplicaCurMoveRange += fGap;
	m_fReplicaBeforeHeight = fCurHeight;
	if( m_CurGalfordCmdInfo.m_ReplicaInfo.m_fReplicaMaxMoveRange < m_fReplicaCurMoveRange )
	{
		pOwner->SetCurMoveSpeed(0.0f);
		pOwner->SetJumpPower(0.0f);
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetCurGravityGapAmt( 0.0f );
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_CurGalfordCmdInfo.m_ReplicaInfo.m_fReplicaEndJumpPower, FLOAT1, false, true, false, true );
		else
			pOwner->SetExtendAttackEndJump( 0.0f, FLOAT1 );
		return;
	}
}

bool ioGalfordItem::CheckLanding( ioBaseChar *pOwner )
{
	if( !pOwner )
		return true;

	float fHeightGap = 0.0f;
	if( pOwner->CheckGhostState() )
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetPreHeightForGhostState();
	else
		fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

	if( fHeightGap > 0.0f || !pOwner->IsDownState() )
		return false;

	return true;
}

void ioGalfordItem::SetReplicaEndAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	pOwner->ClearReservedSliding();

	m_SpecialState = SS_REPLICA_END_ATTACK;

	pOwner->ClearAttackFireTimeAndSkill();

	SetActionAni( pOwner, m_CurGalfordCmdInfo.m_ReplicaInfo.m_ReplicaEndAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_REPLICA_LAND;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::OnReplicaEndAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( CheckCmdInputState( pOwner ) )
		return;
}

void ioGalfordItem::SetCmdInputStart( int iType, ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( pOwner->IsChatModeState( false ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ClearCmdInfo();

	m_dwInputStartTime = dwCurTime;
	m_dwInputCheckTime = dwCurTime+m_dwInputStartGapTime;
}

void ioGalfordItem::SetCmdInputEnd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	if( !pOwner->IsNeedProcess() )
		return;
	if( m_dwInputStartTime == 0 || m_dwInputCheckTime == 0 )
		return;

	ChangeToCmdSet( pOwner );
}

void ioGalfordItem::ClearGalfordCmdSpecialStateInfo( ioBaseChar *pOwner )
{
	switch ( m_SpecialState )
	{
	case SS_SHADOW_COPY:
		{
			pOwner->SetInvisibleState( false, true );
			pOwner->SetDisableGauge( false );
			pOwner->SetBottomCircleVisible( true );
			DeleteShadowCopyBuff( pOwner );
			ioPlayStage *pStage = pOwner->GetCreator();
			if ( pStage )
			{
				DestroyDummyChar( pOwner, m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_szOwnerDummyName );
				DestroyDummyChar( pOwner, m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_szShadowDummyName );
				DestroyEffect( pStage, m_dwShadowCopyStartEffectID );
			}
		}
		break;
	case SS_JUMP_KEY_COUNTER_ATTACK:
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			if ( pStage )
				DestroyEffect( pStage, m_dwJumpKeyCounterAttackEffectID );
		}
		break;
	}

	Init();
	ClearCmdInfo();
	m_CurGalfordCmdInfo.Init();
	m_SpecialState = SS_NONE;
}

void ioGalfordItem::SetShadowCopy( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ClearCancelInfo();
	pOwner->ClearReservedSliding();
	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		pOwner->SetState( CS_GALFORD_CMD_STATE );

	AddShadowCopyBuff( pOwner );

	m_SpecialState = SS_SHADOW_COPY;

	const ShadowCopyInfo& CurInfo = m_CurGalfordCmdInfo.m_ShadowCopyInfo;

	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vScale = pOwner->GetWorldScale();
	D3DXQUATERNION qtOwnerRot = pOwner->GetTargetRot();

	//Start Effect
	DestroyEffect( pStage, m_dwShadowCopyStartEffectID );
	if( !CurInfo.m_fShadowCopyStartEffect.IsEmpty() )
		CreateEffect( pStage, m_dwShadowCopyStartEffectID, CurInfo.m_fShadowCopyStartEffect, vOwnerPos, vScale, qtOwnerRot );


	D3DXVECTOR3 vShadowDummyPos = vOwnerPos + qtOwnerRot * CurInfo.m_vShadowDummyOffset;
	 float fMapHeight = pStage->GetMapHeight( vShadowDummyPos.x, vShadowDummyPos.z );
	 if ( fMapHeight != 0.0f )
		vShadowDummyPos.y = fMapHeight;
	D3DXVECTOR3 vOwnerDummyPos = vOwnerPos + qtOwnerRot * CurInfo.m_vOwnerDummyOffset;
	fMapHeight = pStage->GetMapHeight( vOwnerDummyPos.x, vOwnerDummyPos.z );
	if ( fMapHeight != 0.0f )
		vOwnerDummyPos.y = fMapHeight;
	

	DestroyDummyChar( pOwner, CurInfo.m_szOwnerDummyName );
	DestroyDummyChar( pOwner, CurInfo.m_szShadowDummyName );

	CreateDummyChar( pOwner, CurInfo.m_szOwnerDummyName, vOwnerDummyPos );
	CreateDummyChar( pOwner, CurInfo.m_szShadowDummyName, vShadowDummyPos );

	m_dwShadowCopyEndTime = m_dwShadowCopyEnableKeyStartTime = FRAMEGETTIME();
	m_dwShadowCopyEndTime += CurInfo.m_dwShadowCopyDuration;
	m_dwShadowCopyEnableKeyStartTime += CurInfo.m_dwShadowCopyEnableKeyTime;

	pOwner->SetInvisibleState( true, true, true );
	pOwner->SetDisableGauge( true );
	pOwner->SetBottomCircleVisible( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_SHADOW;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iGalfordCmdIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::CreateEffect( ioPlayStage *pStage, DWORD& dwEffectID, const ioHashString& szEffectName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vScale, const D3DXQUATERNION qtRot )
{
	ioMapEffect* pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
	if( pMapEffect )
	{	
		dwEffectID = pMapEffect->GetUniqueID();			
		pMapEffect->SetWorldOrientation( qtRot );
	}
}

void ioGalfordItem::DestroyEffect( ioPlayStage* pStage, DWORD& dwEffectID )
{
	if( !pStage || dwEffectID == -1 )
		return;

	ioMapEffect* pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )	
		pStage->DestroyMapEffect(dwEffectID);

	dwEffectID = -1;
}

void ioGalfordItem::OnShadowCopy( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwShadowCopyEndTime < dwCurTime )
	{
		SetShadowCopyEndState( pOwner );
		return;
	}

	if ( m_dwShadowCopyEnableKeyStartTime < dwCurTime )
	{
		CheckReserve( pOwner );
		if ( pOwner->IsSettedDirection() || 
			pOwner->IsAttackKey() ||
			pOwner->IsDefenseKey() ||
			pOwner->IsJumpKey() )
		{
			SetShadowCopyEndState( pOwner );
			return;
		}
	}

	pOwner->SetBottomCircleVisible( false );
}

void ioGalfordItem::SetShadowCopyEndState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	pOwner->SetInvisibleState( false, true );
	pOwner->SetDisableGauge( false );
	pOwner->SetBottomCircleVisible( true );
	DeleteShadowCopyBuff( pOwner );
	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( pOwner, m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_szOwnerDummyName );
	if ( !pDummy )
	{
		pOwner->SetExtendAttackEndJump( 0.0f, FLOAT1 );
		return;
	}

	D3DXVECTOR3 vNewPos = pDummy->GetWorldPosition();
	pOwner->SetWorldPosition( vNewPos );
	float fPreHeight = pOwner->GetBottomHeight();
	if ( fPreHeight <= 0.0f )
		pOwner->SetExtendAttackEndJump( m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_fShadowCopyEndJumpPower, FLOAT1 );
	else
		ProcessReserveKeyInput( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_SHADOW_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::SetStrikeHeadsAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	ClearCancelInfo();
	pOwner->ClearReservedSliding();

	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		pOwner->SetState( CS_GALFORD_CMD_STATE );
	m_SpecialState = SS_STRIKE_HEADS;

	SetActionAni( pOwner, m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_StrikeHeadsAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_STRIKE;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_iGalfordCmdIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioGalfordItem::SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !pOwner )
		return false;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		return false;

	if ( m_SpecialState != SS_STRIKE_HEADS )
		return false;

	const StrikeHeadsInfo& rkInfo = m_CurGalfordCmdInfo.m_StrikeHeadsInfo;
	int iAniID = pGrp->GetAnimationIdx( rkInfo.m_GrapplingLoopMotion );
	if ( iAniID == -1 )
		return false;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
	if( !pTarget )
		return false;

	m_SpecialState = SS_STRIKE_HEADS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( rkInfo.m_fGrapplingLoopMotionRate > 0.0f )
		fAniRate = rkInfo.m_fGrapplingLoopMotionRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingTargetName = szTarget;

	SetAniRotate( pOwner, iAniID, fAniRate );

	if( rkInfo.m_fJumpPower > 0.0f )
		pOwner->SetJumpPower( rkInfo.m_fJumpPower );

	pTarget->SetItemGrapplingState( pOwner->GetCharName(), rkInfo.m_TargetGrapplingInfo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_STRIKE_LOOP;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << szTarget;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioGalfordItem::OnStrikeHeadsAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioGalfordItem::OnStrikeHeadsLoopAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( CheckLanding( pOwner ) )
	{
		SetStrikeHeadsLandAttack( pOwner );
		return;
	}
}

void ioGalfordItem::SetStrikeHeadsLandAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_SpecialState = SS_STRIKE_HEADS_LAND_ATTACK;
	SetActionAni( pOwner, m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_StrikeLandAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_STRIKE_LAND;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::OnStrikeHeadsLandAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( CheckCmdInputState( pOwner ) )
		return;
}

D3DXVECTOR3 ioGalfordItem::GetGrapplingTargetPos( ioBaseChar *pOwner )
{
	if( m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_fGrapplingOffsetGapRange > 0.0f )
	{
		D3DXVECTOR3 vPos = ioMath::VEC3_ZERO;

		vPos = pOwner->GetMidPositionByRate( m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_fGrapplingOffsetHeightRate );		
		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_fGrapplingOffsetGapRange > 0.0f )
			vPos += m_CurGalfordCmdInfo.m_StrikeHeadsInfo.m_fGrapplingOffsetGapRange * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioGalfordItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioGalfordItem::SetPappyCmdAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();
	pOwner->ClearReservedSliding();

	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		pOwner->SetState( CS_GALFORD_CMD_STATE );
	m_SpecialState = SS_PAPPY_ATTACK;

	SetActionAni( pOwner, m_CurGalfordCmdInfo.m_Attack );

	if ( pOwner->IsNeedProcess() )
	{
		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << GST_PAPPY;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << m_iGalfordCmdIndex;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		DecreaseGauge( m_CurGalfordCmdInfo.m_fDecreseGauge );
		m_iCurCmdCombo++;
	}
	
}

void ioGalfordItem::OnPappyAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
		{
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetExtendAttackEndJump( m_fPappyAttackEndJumpPower, m_fPappyAttackEndLandingRate, false, true, false, true );
		}
		else
			ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( CheckCmdInputState( pOwner ) )
		return;
}

bool ioGalfordItem::IsEnableJumpCounterAttack()
{
	if ( (int)m_fCurBullet > 0 )
		return true;
	else
		return false;
}

void ioGalfordItem::SetJumpCountAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ClearCancelInfo();
	pOwner->ClearReservedSliding();

	if ( pOwner->GetState() != CS_GALFORD_CMD_STATE )
		pOwner->SetState( CS_GALFORD_CMD_STATE );
	m_SpecialState = SS_JUMP_KEY_COUNTER_ATTACK;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	CreateEffect( pStage, m_dwJumpKeyCounterAttackEffectID,m_szJumpKeyCounterAttackEffect, vPos, pOwner->GetWorldScale(), qtRot );

	m_fJumpKeyCACurMoveRange = 0.0f;

	if ( pOwner->IsNeedProcess() )
		SetJumpCounterAttackPosition( pOwner );

	m_fJumpKeyCABeforeHeight = pOwner->GetWorldPosition().y;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_JumpKeyCounterAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;
	float fTimeRate  = m_JumpKeyCounterAttack.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->RefreshFireTimeList( iAniID, m_JumpKeyCounterAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_JUMP_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fJumpKeyCADecreaseBullet );
}

void ioGalfordItem::SetJumpCounterAttackPosition( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	ioHashString szTarget = FindAttTarget( pOwner, m_fJumpKeyCATargetAngle, m_fJumpKeyCATargetRange );
	ioBaseChar* pTarget = pOwner->GetBaseChar( szTarget );
	if ( pTarget )
	{
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vNewPos = pTarget->GetWorldOrientation() * m_vJumpKeyCATargetTeleportOffset;
		vNewPos += vTargetPos;

		pOwner->SetWorldPosition( vNewPos );

		D3DXVECTOR3 vDir = vTargetPos - vNewPos;
		D3DXVec3Normalize( &vDir, &vDir );
		pOwner->SetTargetRotToDir( vDir, true );

	}
	else
	{
		D3DXVECTOR3 vNewPos = pOwner->GetWorldOrientation() * m_vJumpKeyCANoneTargetTeleportOffset;
		vNewPos += pOwner->GetWorldPosition();
		pOwner->SetWorldPosition( vNewPos );
	}
}

void ioGalfordItem::OnJumpKeyCounterAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( CheckLanding( pOwner ) )
	{
		SetJumpCountLandAttackState( pOwner );
		return;
	}
	float fCurHeight = pOwner->GetWorldPosition().y;
	float fGap = fabs( m_fJumpKeyCABeforeHeight - fCurHeight );
	m_fJumpKeyCACurMoveRange += fGap;
	m_fJumpKeyCABeforeHeight = fCurHeight;
	if( m_fJumpKeyCAMaxMoveRange < m_fJumpKeyCACurMoveRange )
	{
		pOwner->SetCurMoveSpeed(0.0f);
		pOwner->SetJumpPower(0.0f);
		pOwner->SetGravityAmt( 0.0f );
		pOwner->SetCurGravityGapAmt( 0.0f );
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if ( fHeightGap >= FLOAT1 )
			pOwner->SetExtendAttackEndJump( m_fJumpKeyCAEndJumpPower, FLOAT1, false, true, false, true );
		else
			pOwner->SetExtendAttackEndJump( 0.0f, FLOAT1 );
		return;
	}
}

void ioGalfordItem::CheckExtraAniJump( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_fExtraJumpPower == 0.0f )
		return;

	if ( m_dwExtraAniJumpTime > 0 && m_dwExtraAniJumpTime < FRAMEGETTIME() )
	{
		m_dwExtraAniJumpTime = 0;

		D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		float fJumpAngle = m_fExtraJumpAngle;

		float fAngleV = ioMath::ArrangeHead( fJumpAngle );

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vTargetDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

		// vertical
		D3DXQUATERNION qtRot;
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		vTargetDir = qtRot * vTargetDir;

		pOwner->SetForcePower( vTargetDir, fabs(m_fExtraJumpPower), FLOAT1, true );
	}
}

void ioGalfordItem::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	if ( m_SpecialState == SS_JUMP_KEY_COUNTER_ATTACK )
		fCurGravity *= m_fJumpKeyCAGravityRate;
	else
		fCurGravity *= m_CurGalfordCmdInfo.m_fGravityRate;
}

void ioGalfordItem::SetJumpCountLandAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ClearCancelInfo();
	pOwner->ClearReservedSliding();

	m_SpecialState = SS_JUMP_KEY_COUNTER_LAND_ATTACK;
	pOwner->ClearAttackFireTimeAndSkill();
	SetActionAni( pOwner, m_JumpKeyCALandingAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << GST_JUMP_LAND_ATTACK;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGalfordItem::OnJumpKeyCounterLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );

	if( CheckCmdInputState( pOwner ) )
		return;
}

void ioGalfordItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

void ioGalfordItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		if( m_fCurBullet < m_fMaxBullet )
		{
			float fGauge = m_fIncreaseGaugeTic * fTimePerSec;

			m_fCurBullet += fGauge;
			m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
		}
		break;
	}
}

void ioGalfordItem::DecreaseGauge( float fNeedGauge )
{
	m_fCurBullet -= fNeedGauge;
	m_fCurBullet = (float)max( 0, min( m_fCurBullet, m_fMaxBullet ) );
}

int ioGalfordItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

void ioGalfordItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if ( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	switch( iState )
	{
	case GST_NORMAL:
		{
			rkPacket >> m_iCurCombo;
			ChangeToNormalAttack( pOwner );
		}
		break;
	case GST_ATTACK_FIRE:
		{
			rkPacket >> m_iCurCombo;
			ChangeToAttackFire( pOwner );
		}
		break;
	case GST_REPLICA:
		{
			rkPacket >> m_iGalfordCmdIndex;
			if ( COMPARE( m_iGalfordCmdIndex, 0, (int)m_GalfordCmdInfoList.size() ) )
			{
				m_CurGalfordCmdInfo = m_GalfordCmdInfoList[m_iGalfordCmdIndex];
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;
				SetReplicaAttack( pOwner );
			}
		}
		
		break;
	case GST_REPLICA_LAND:
		SetReplicaEndAttack( pOwner );
		break;
	case GST_SHADOW:
		{
			rkPacket >> m_iGalfordCmdIndex;
			if ( COMPARE( m_iGalfordCmdIndex, 0, (int)m_GalfordCmdInfoList.size() ) )
			{
				m_CurGalfordCmdInfo = m_GalfordCmdInfoList[m_iGalfordCmdIndex];
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;
				SetShadowCopy( pOwner );
			}
		}
		break;
	case GST_STRIKE:
		{
			rkPacket >> m_iGalfordCmdIndex;
			if ( COMPARE( m_iGalfordCmdIndex, 0, (int)m_GalfordCmdInfoList.size() ) )
			{
				m_CurGalfordCmdInfo = m_GalfordCmdInfoList[m_iGalfordCmdIndex];
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;
				SetStrikeHeadsAttack( pOwner );
			}
		}
		break;
	case GST_STRIKE_LOOP:
		{
			rkPacket >> m_GrapplingTargetName;
			SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		}
		break;
	case GST_STRIKE_LAND:
		SetStrikeHeadsLandAttack( pOwner );
		break;
	case GST_PAPPY:
		{
			rkPacket >> m_iGalfordCmdIndex;
			if ( COMPARE( m_iGalfordCmdIndex, 0, (int)m_GalfordCmdInfoList.size() ) )
			{
				m_CurGalfordCmdInfo = m_GalfordCmdInfoList[m_iGalfordCmdIndex];
				m_iCurCmdMaxCombo = m_CurGalfordCmdInfo.m_iMaxAttackCount;
				SetPappyCmdAttack( pOwner );
			}
			break;
		}
	case GST_JUMP_ATTACK:
		SetJumpCountAttackState( pOwner );
		break;
	case GST_JUMP_LAND_ATTACK:
		SetJumpCountLandAttackState( pOwner );
		break;
	}
}

void ioGalfordItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case GST_SHADOW_END:
		pOwner->SetInvisibleState( false, true );
		pOwner->SetDisableGauge( false );
		pOwner->SetBottomCircleVisible( true );
		break;
	}
}

bool ioGalfordItem::IsCollisionAvailableWeaponBySpecialState()
{
	if ( m_SpecialState == SS_SHADOW_COPY )
		return false;

	return true;
}

bool ioGalfordItem::IsEntityCollisionSkipStateBySpecialState()
{
	if ( m_SpecialState == SS_SHADOW_COPY )
		return true;

	return true;
}

void ioGalfordItem::AddShadowCopyBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	const ioHashStringVec& rkBuffList = m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_vShadowCopyBuffList;
	for ( int i=0; i<(int)rkBuffList.size() ; ++i )
	{
		if ( !rkBuffList[i].IsEmpty() )
			pOwner->AddNewBuff( rkBuffList[i], pOwner->GetCharName(), this->GetName(), NULL );
	}
}

void ioGalfordItem::DeleteShadowCopyBuff( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	const ioHashStringVec& rkBuffList = m_CurGalfordCmdInfo.m_ShadowCopyInfo.m_vShadowCopyBuffList;
	for ( int i=0; i<(int)rkBuffList.size() ; ++i )
	{
		if ( !rkBuffList[i].IsEmpty() )
			pOwner->RemoveBuff( rkBuffList[i] );
	}
}

bool ioGalfordItem::IsNoWoundedBySpecialState( ioBaseChar *pOwner )
{
	if( pOwner->HasBuff( BT_NOWOUND ) )
		return true;
	return false;
}

bool ioGalfordItem::IsEnableBlowWounded()
{
	switch( m_SpecialState )
	{
	case SS_JUMP_KEY_COUNTER_ATTACK:
	case SS_REPLICA_ATTACK:
	case SS_STRIKE_HEADS_LOOP:
		return true;
	case SS_PAPPY_ATTACK:
		{
			if ( m_fExtraJumpPower > 0.0f )
				return true;
		}
		break;
	}

	return false;
}

void ioGalfordItem::ProcessReserveDummyCreate( ioBaseChar *pOwner, const ioPlayMode* const pMode )
{
	if ( m_iEquipDummyCharIndex != -1 )
		return;

	if( pMode && !pMode->GetDummyTypeCreateValue() )
		return;

	if ( !m_EquipDummyCharName.IsEmpty() )
		CreateEquipDummyChar( pOwner );
}

bool ioGalfordItem::IsFloatingStateBySpecialState() const
{
	switch( m_SpecialState )
	{
	case SS_JUMP_KEY_COUNTER_ATTACK:
	case SS_REPLICA_ATTACK:
	case SS_STRIKE_HEADS_LOOP:
		return true;
	case SS_PAPPY_ATTACK:
		{
			if ( m_fExtraJumpPower > 0.0f )
				return true;
		}
		break;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------------------
ioGalfordCmdSpecialState::ioGalfordCmdSpecialState()
{

}

ioGalfordCmdSpecialState::~ioGalfordCmdSpecialState()
{

}

void ioGalfordCmdSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGalfordCmdSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford )
		pGalford->ClearGalfordCmdSpecialStateInfo( pOwner );
}

void ioGalfordCmdSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		pGalford->ProcessGalfordCmdSpecialState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioGalfordCmdSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioGalfordCmdSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		pGalford->GetGravity( pOwner, fCurGravity );
}

bool ioGalfordCmdSpecialState::IsNoDropState( ioBaseChar* pOwner )
{
	return true;
}

bool ioGalfordCmdSpecialState::IsCollisionAvailableWeapon( ioBaseChar* pOwner, ioWeapon *pWeapon )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsCollisionAvailableWeaponBySpecialState();
	else
		return true;
}

bool ioGalfordCmdSpecialState::IsEntityCollisionSkipState( const ioBaseChar *const pOwner, ioGameEntity* pColEntity )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsEntityCollisionSkipStateBySpecialState();
	else
		return false;
}

bool ioGalfordCmdSpecialState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsEntityCollisionSkipStateBySpecialState();
	else
		return false;
}

bool ioGalfordCmdSpecialState::IsCanNoWound( ioBaseChar* pOwner )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsNoWoundedBySpecialState( pOwner );
	else
		return false;
}

bool ioGalfordCmdSpecialState::IsEnableBlowWounded( ioBaseChar *pOwner )
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsEnableBlowWounded();
	else
		return true;
}

bool ioGalfordCmdSpecialState::IsFloatingState( const ioBaseChar * const pOwner ) const
{
	ioGalfordItem *pGalford = ToGalfordItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGalford  )
		return pGalford->IsFloatingStateBySpecialState();
	else
		return false;
}
