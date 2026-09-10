
#include "stdafx.h"

#include "ioRangeSkill.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ItemDefine.h"

ioRangeSkill::ioRangeSkill()
{
	m_RangeState = RS_NONE;

	m_dwPreDelayStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	m_vCollisionedList.clear();
	m_vBlockedList.clear();
}

ioRangeSkill::ioRangeSkill( const ioRangeSkill &rhs )
: ioSkill( rhs ),
 m_fSkillRange( rhs.m_fSkillRange ),
 m_fSkillAngle( rhs.m_fSkillAngle ),
 m_fOffSet( rhs.m_fOffSet ),
 m_dwDuration( rhs.m_dwDuration ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_EndAnimation( rhs.m_EndAnimation ),
 m_fEndAniRate( rhs.m_fEndAniRate ),
 m_RangeEffect( rhs.m_RangeEffect ),
 m_StartHandMesh( rhs.m_StartHandMesh ),
 m_LoopHandMesh( rhs.m_LoopHandMesh ),
 m_EndHandMesh( rhs.m_EndHandMesh ),
 m_TargetColType( rhs.m_TargetColType ),
 m_TargetWoundType( rhs.m_TargetWoundType ),
 m_fUpHeight( rhs.m_fUpHeight ),
 m_fUnderHeight( rhs.m_fUnderHeight ),
 m_fWidth( rhs.m_fWidth ),
 m_EnemyBuffType( rhs.m_EnemyBuffType ),
 m_OwnerBuffList( rhs.m_OwnerBuffList ),
 m_TeamBuffList( rhs.m_TeamBuffList ),
 m_EnemyBuffList( rhs.m_EnemyBuffList ),
 m_iEnemyRandomBuffCount( rhs.m_iEnemyRandomBuffCount ),
 m_EnemyRandomBuffList( rhs.m_EnemyRandomBuffList ),
 m_bEnableAttacker( rhs.m_bEnableAttacker ),
 m_bEnableAttackCnt( rhs.m_bEnableAttackCnt ),
 m_dwLoopProtectDuration( rhs.m_dwLoopProtectDuration ),
 m_bCheckCollisionLine( rhs.m_bCheckCollisionLine ),
 m_bIgnoreProtect( rhs.m_bIgnoreProtect ),
 m_bStateEscapeOnStand( rhs.m_bStateEscapeOnStand ),
 m_bStateEscapeOnAir( rhs.m_bStateEscapeOnAir ),
 m_bStateEscapeOnDown( rhs.m_bStateEscapeOnDown ),
 m_vEndAttackWeaponList( rhs.m_vEndAttackWeaponList ),
 m_vEndAttackForceList( rhs.m_vEndAttackForceList ),
 m_bNotUseKeyReserve( rhs.m_bNotUseKeyReserve ),
 m_DirectionBuffFrontList( rhs.m_DirectionBuffFrontList ),
 m_DirectionBuffBackList( rhs.m_DirectionBuffBackList )
{
	m_RangeState = RS_NONE;

	m_dwPreDelayStartTime = 0;
	m_dwLoopStartTime = 0;
	m_dwEndAniStartTime = 0;
	m_dwEndAniEndTime = 0;
	m_bSetHandMesh = false;

	m_vCollisionedList.clear();
	m_vBlockedList.clear();
}

ioRangeSkill::~ioRangeSkill()
{
	m_vCollisionedList.clear();
	m_vBlockedList.clear();
}

void ioRangeSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bEnableAttacker = rkLoader.LoadBool_e( "enable_attacker", false );
	m_bEnableAttackCnt = rkLoader.LoadBool_e( "enable_attack_cnt", true );
	m_bCheckCollisionLine = rkLoader.LoadBool_e( "check_collision_line", false );

	m_TargetColType = (TargetColType)rkLoader.LoadInt_e( "target_collision_type", TCT_SPHERE );
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_fSkillRange = rkLoader.LoadFloat_e( "apply_range", 0.0f );
	m_fSkillAngle = rkLoader.LoadFloat_e( "apply_angle", 0.0f );
	m_fOffSet = rkLoader.LoadFloat_e( "apply_position_offset", 0.0f );

	m_fUpHeight = rkLoader.LoadFloat_e( "apply_up_height", 0.0f );
	m_fUpHeight = max( FLOAT1, m_fUpHeight );
	m_fUnderHeight = rkLoader.LoadFloat_e( "apply_under_height", 0.0f );
	m_fUnderHeight = max( FLOAT1, m_fUnderHeight );
	m_fWidth = rkLoader.LoadFloat_e( "apply_width", 0.0f );

	m_dwDuration = rkLoader.LoadInt_e( "range_duration", 0 );
	m_dwLoopProtectDuration = rkLoader.LoadInt_e( "loop_protect_duration", 0 );

	rkLoader.LoadString_e( "loop_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	m_dwLoopTime = rkLoader.LoadInt_e( "loop_duration", 0 );

	rkLoader.LoadString_e( "end_animation", "", szBuf, MAX_PATH );
	m_EndAnimation = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "start_hand_mesh", "", szBuf, MAX_PATH );
	m_StartHandMesh = szBuf;
	rkLoader.LoadString_e( "loop_hand_mesh", "", szBuf, MAX_PATH );
	m_LoopHandMesh = szBuf;
	rkLoader.LoadString_e( "end_hand_mesh", "", szBuf, MAX_PATH );
	m_EndHandMesh = szBuf;

	rkLoader.LoadString_e( "range_effect", "", szBuf, MAX_PATH );
	m_RangeEffect = szBuf;

	m_EnemyBuffType = (EnemyBuffType)rkLoader.LoadInt_e( "enemy_buff_type", EBT_NORMAL );

	LoadOwnerBuffList( rkLoader );
	LoadTeamBuffList( rkLoader );
	LoadEnemyBuffList( rkLoader );
	LoadEnemyRandomBuffList( rkLoader );
	LoadDirectionBuffList( rkLoader );

	m_bIgnoreProtect = rkLoader.LoadBool_e( "ignore_protect", false );

	m_bStateEscapeOnStand = rkLoader.LoadBool_e( "state_escape_on_stand", false );
	m_bStateEscapeOnAir = rkLoader.LoadBool_e( "state_escape_on_air", false );
	m_bStateEscapeOnDown = rkLoader.LoadBool_e( "state_escape_on_down", false );

	LoadEndAttackList( rkLoader );

	m_bNotUseKeyReserve = rkLoader.LoadBool_e( "not_use_key_reserve", false );
	
}

void ioRangeSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioRangeSkill::LoadTeamBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "team_buff_cnt", 0 );
	m_TeamBuffList.clear();

	if( iCnt <= 0 ) return;

	m_TeamBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "team_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
        tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_TeamBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioRangeSkill::LoadEnemyBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "enemy_buff_cnt", 0 );
	m_EnemyBuffList.clear();

	if( iCnt <= 0 ) return;

	m_EnemyBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "enemy_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
        tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_EnemyBuffList.push_back( tBuffInfo );
			}
		}
	}
}

void ioRangeSkill::LoadEnemyRandomBuffList( ioINILoader &rkLoader )
{
	m_iEnemyRandomBuffCount = rkLoader.LoadInt_e( "enemy_random_buff_cnt", 0 );
	m_EnemyRandomBuffList.clear();

	if( m_iEnemyRandomBuffCount <= 0 ) return;

	m_EnemyRandomBuffList.reserve( m_iEnemyRandomBuffCount );

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < m_iEnemyRandomBuffCount; ++i )
	{
		BuffInfoList kList;
		wsprintf_e( szKey, "enemy_random%d_buff_cnt", i+1 );
		int iCnt = rkLoader.LoadInt( szKey, 0 );
		kList.reserve( iCnt );

		for( int j=0; j<iCnt; ++j )
		{
			wsprintf_e( szKey, "enemy_random%d_buff%d", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			ioBuffInfo tBuffInfo;
			tBuffInfo.m_BuffName = szBuf;

			if( !tBuffInfo.m_BuffName.IsEmpty() )
			{
				tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
				if( tBuffInfo.m_iBuffType != BT_NONE )
				{
					kList.push_back( tBuffInfo );
				}
			}
		}
		m_EnemyRandomBuffList.push_back( kList );
	}
}

void ioRangeSkill::LoadDirectionBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "direction_front_buff_cnt", 0 );
	m_DirectionBuffFrontList.clear();
	
	if( iCnt > 0 )
		m_DirectionBuffFrontList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "direction_front_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_DirectionBuffFrontList.push_back( tBuffInfo );
			}
		}
	}

	iCnt = rkLoader.LoadInt_e( "direction_back_buff_cnt", 0 );
	m_DirectionBuffBackList.clear();

	if( iCnt > 0 )
		m_DirectionBuffBackList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "direction_back_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_DirectionBuffBackList.push_back( tBuffInfo );
			}
		}
	}
}

void ioRangeSkill::LoadEndAttackList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	m_vEndAttackWeaponList.clear();
	int iEndAttackCount = rkLoader.LoadInt_e( "end_attack_type_count", 0 );
	if ( iEndAttackCount > 0 )
	{
		m_vEndAttackWeaponList.reserve( iEndAttackCount );
		for ( int i=0 ; i < iEndAttackCount ; ++i )
		{
			WeaponInfo kInfo;
			wsprintf_e( szBuf, "end_attack_type%d", i+1 );
			kInfo.m_iWeaponIdx = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szBuf, "end_attack_type%d_resist", i+1 );
			kInfo.m_iResistanceIdx = rkLoader.LoadInt( szBuf, NO_WEAPON_ATTRIBUTE );

			wsprintf_e( szBuf, "end_attack_type%d_wounded", i+1 );
			rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
			kInfo.m_WoundedAnimation = szBuf;

			wsprintf_e( szBuf, "end_attack_type%d_wounded_time", i+1 );
			kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szBuf, 0 );

			wsprintf_e( szBuf, "end_attack_type%d_wounded_loop_ani", i+1 );
			kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szBuf, false );

			m_vEndAttackWeaponList.push_back( kInfo );
		}
	}

	int iEndAttackForceCount = rkLoader.LoadInt_e( "end_attack_force_cnt", 0 );
	if( iEndAttackForceCount > 0 )
		m_vEndAttackForceList.reserve( iEndAttackForceCount );

	for( int i=0; i < iEndAttackForceCount; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "end_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "end_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "end_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vEndAttackForceList.push_back( kInfo );
	}
}

ioSkill* ioRangeSkill::Clone()
{
	return new ioRangeSkill( *this );
}

int ioRangeSkill::GetTypeID() const
{
	return ST_RANGE;
}

bool ioRangeSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	if( m_RangeState == RS_ACTION )
		return true;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_RangeState == RS_LOOP )
	{
		if( m_dwLoopProtectDuration == 0 )
			return true;

		if( m_dwProtectTime > dwCurTime )
			return true;
	}
	
	if( m_RangeState == RS_END )
		return false;

	return false;
}

bool ioRangeSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioRangeSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_RangeState != RS_END )
		return false;

	if( m_dwEndAniStartTime > 0 && m_dwEndAniEndTime < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioRangeSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	if( !m_bNoCheckSkillmotion && !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_vCollisionedList.clear();
	m_vBlockedList.clear();

	pChar->SetSkillProtection();

	m_RangeState = RS_NONE;
	m_dwProtectTime = 0;
	m_bSetHandMesh = false;
	m_bReduceGauge = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelayTime = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelayTime;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	m_vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );
	
	return true;
}

void ioRangeSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < FRAMEGETTIME() )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( m_dwLoopStartTime == 0 )
		{
			SetEndState( pChar );
		}
		break;
	}
}

void ioRangeSkill::OnSkillEnd( ioBaseChar *pOwner )
{
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pOwner->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false; 

	if( !m_StartHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );

	if( !m_LoopHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );

	if( !m_EndHandMesh.IsEmpty() )
		pOwner->RestoreSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );


	EndPreDelayEffect( pOwner );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->ClearTarget();
		GetOwnerItem()->ClearCurChargeRateForSkill();
	}

	pOwner->EndEffect( m_SkillEffectName );

	m_vCollisionedList.clear();
	m_vBlockedList.clear();

	m_bReduceGauge = false;
	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	pOwner->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pOwner->AttachEffect( m_SkillEndEffectName );

	ioMechanicsItem *pMechanics = ToMechanicsItem( GetOwnerItem() );
	ioPhantomItem *pPhantom = ToPhantomItem( GetOwnerItem() );
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetOwnerItem() );

	if( pMechanics )
		pMechanics->ClearMechanicsTargetSkill();
	else if( pPhantom )
		pPhantom->ClearPhantomTargetSkill();
	else if( pDestroyer )
		pDestroyer->ClearDestroyerTargetSkill();
	
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pOwner );
		if ( pPet )
			pPet->SetForceHide( false );
	}
}

bool ioRangeSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );
	
	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );
	
	return true;
}

void ioRangeSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeSkill::SetActionState() - %s, Not Exist ActionAnimation", GetName().c_str() );
	}
	else
	{
		float fAniRate = GetSkillMotionRate();

		float fTimeRate = fAniRate * m_fExtraAniRate;

		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	}

	m_RangeState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioRangeSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iAniID == -1 )
	{
		if ( !m_bNotUseKeyReserve )
			m_dwEnableReserveTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeSkill::SetLoopState() - %s, Not Exist LoopAnimation", GetName().c_str() );
	}
	else
	{
		pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );
		if ( !m_bNotUseKeyReserve )
		{
			m_dwEnableReserveTime = FRAMEGETTIME();
			m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
		}
	}

	m_dwLoopStartTime = FRAMEGETTIME();
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;
}

void ioRangeSkill::SetEndState( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_EndAnimation );
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwEndAniStartTime = dwCurTime;
		m_dwEndAniEndTime = m_dwEndAniStartTime;
	}
	else
	{
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fEndAniRate );
		m_dwEndAniStartTime = dwCurTime;
		m_dwEndAniEndTime = m_dwEndAniStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*m_fEndAniRate;
	}

	m_RangeState = RS_END;

	if( m_bSetHandMesh )
	{
		if( !m_LoopHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		}
		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_EndHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_EndHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if ( !m_vEndAttackWeaponList.empty() )
	{
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
			iCurSkillAniID,
			m_vEndAttackWeaponList,
			FTT_SKILL_ATTACK,
			m_fEndAniRate,
			0 );
	}
	if ( !m_vEndAttackForceList.empty() )
		pChar->SetReservedSliding( m_vEndAttackForceList, iCurSkillAniID, m_fEndAniRate, 0 );
}

bool ioRangeSkill::SetOwnerBuffList( ioBaseChar *pChar )
{
	if( !pChar->IsNeedProcess() ) return false;
	if( m_OwnerBuffList.empty() ) return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	if( pChar && !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;


		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pChar->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				pBuff->SetExtraDurationByGrowth( fValue );

				bResult = true;
				vBuffList.push_back( szBuffName );
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff10 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
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

bool ioRangeSkill::SetTeamBuffList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( m_TeamBuffList.empty() )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	int iCnt = m_TeamBuffList.size();
	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_TeamBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
			continue;


		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				pBuff->SetExtraDurationByGrowth( fValue );

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff11 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
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

bool ioRangeSkill::SetEnemyBuffList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( pChar->IsProtectState() )
	{
		bool bBreakProtcet = false;
		if( m_bIgnoreProtect )
		{
			if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
			{
				bBreakProtcet = true;
			}
		}
		if( !bBreakProtcet )
		{
			int iType = pChar->CheckBlockEmoticonType();
			if( iType >= 0 && !IsBlocked( pChar ) )
			{
				m_vBlockedList.push_back( pChar->GetCharName() );
				pChar->SetProtectWound( iType );
			}

			return false;
		}
	}

	if( m_EnemyBuffList.empty() )
		return false;

	pChar->SetRandomSeed( 0 );

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	int iCnt = m_EnemyBuffList.size();

	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = m_EnemyBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
		{
			if( !m_bIgnoreProtect )
			{
				continue;
			}
		}
		else if( pChar->IsCanNoWound() )
			continue;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				pBuff->SetExtraDurationByGrowth( fValue );

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff12 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
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

bool ioRangeSkill::SetEnemyRandomBuffList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( pChar->IsProtectState() )
	{
		bool bBreakProtcet = false;
		if( m_bIgnoreProtect )
		{
			if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
			{
				bBreakProtcet = true;
			}
		}
		if( !bBreakProtcet )
		{
			int iType = pChar->CheckBlockEmoticonType();
			if( iType >= 0 && !IsBlocked( pChar ) )
			{
				m_vBlockedList.push_back( pChar->GetCharName() );
				pChar->SetProtectWound( iType );
			}

			return false;
		}
	}

	if( m_EnemyRandomBuffList.empty() )
		return false;

	pChar->SetRandomSeed( 0 );

	IORandom random;
	DWORD dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed ); 
	
	int iBuffIndex = random.Random( m_iEnemyRandomBuffCount );
	if( !COMPARE( iBuffIndex, 0, m_iEnemyRandomBuffCount ) )
		return false;

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	BuffInfoList kBuffList = m_EnemyRandomBuffList[iBuffIndex];
	int iCnt = kBuffList.size();

	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = kBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
		{
			if( !m_bIgnoreProtect )
			{
				continue;
			}
		}
		else if( pChar->IsCanNoWound() )
			continue;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				pBuff->SetExtraDurationByGrowth( fValue );

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff13 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
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

bool ioRangeSkill::SetEnemyDirectionBuffList( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	if( pChar->IsProtectState() )
	{
		bool bBreakProtcet = false;
		if( m_bIgnoreProtect )
		{
			if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
			{
				bBreakProtcet = true;
			}
		}
		if( !bBreakProtcet )
		{
			int iType = pChar->CheckBlockEmoticonType();
			if( iType >= 0 && !IsBlocked( pChar ) )
			{
				m_vBlockedList.push_back( pChar->GetCharName() );
				pChar->SetProtectWound( iType );
			}

			return false;
		}
	}

	BuffInfoList pBuffList;
	if( IsFrontSide( pOwner, pChar ) )
		pBuffList = m_DirectionBuffFrontList;
	else
		pBuffList = m_DirectionBuffBackList;

	if( pBuffList.empty() )
		return false;

	bool bResult = false;
	ioHashStringVec vBuffList;

	ioHashString szItemName;
	ioItem *pItem = GetOwnerItem();
	if( pItem )
	{
		szItemName = pItem->GetName();
	}

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpinfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
	if( pUpinfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpinfo->GetValue(pStage);
	}

	int iCnt = pBuffList.size();

	for( int i=0; i < iCnt; ++i )
	{
		const ioHashString& szBuffName = pBuffList[i].m_BuffName;

		if( pChar->IsHasCrown() && m_bDisableKingSkill )
		{
			continue;
		}
		else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) || pChar->HasBuff(BT_NOT_MOVE_PROTECT) )
		{
			if( !m_bIgnoreProtect )
			{
				continue;
			}
		}
		else if( pChar->IsCanNoWound() )
			continue;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pOwner->GetCharName(), szItemName, this, GetSkillMotionTime() );
			if( pBuff )
			{
				if( m_bEnableAttacker )
				{
					pChar->SetLastAttackerName( pOwner->GetCharName(), GetName() );
				}

				pBuff->SetExtraDurationByGrowth( fValue );

				vBuffList.push_back( szBuffName );
				bResult = true;
			}
		}
	}

	if( bResult )
	{
		m_vCollisionedList.push_back( pChar->GetCharName() );

		int iCurBuffCnt = vBuffList.size();
		if( pChar->IsNeedSendNetwork() )
		{
			if( Help::CheckEnableTimeGapUser() )
			{
				LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff12 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
			}
			SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
			kPacket << pChar->GetCharName();
			kPacket << pOwner->GetCharName();
			kPacket << szItemName;
			kPacket << pChar->GetRandomSeed();
			kPacket << true;					// 타격횟수 체크
			kPacket << 0.0f;
			kPacket << fValue;
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

bool ioRangeSkill::IsCollisioned( ioBaseChar *pChar )
{
	ioHashStringVec::iterator iter = std::find( m_vCollisionedList.begin(),
												m_vCollisionedList.end(),
												pChar->GetCharName() );

	if( iter != m_vCollisionedList.end() )
		return true;

	return false;
}

bool ioRangeSkill::IsBlocked( ioBaseChar *pChar )
{
	ioHashStringVec::iterator iter = std::find( m_vBlockedList.begin(),
												m_vBlockedList.end(),
												pChar->GetCharName() );

	if( iter != m_vBlockedList.end() )
		return true;

	return false;
}

bool ioRangeSkill::IsFrontSide( ioBaseChar *pOwner, ioBaseChar *pChar )
{
	bool bRtnValue = false;

	/*D3DXVECTOR3 vDefenseDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize(&vMoveDir,&vMoveDir);
	D3DXVECTOR3 vInvAttack = -vMoveDir;

	float fAttackedCosine = D3DXVec3Dot( &vDefenseDir, &vInvAttack );

	float fFrontAngle = DEGtoRAD(FLOAT90 );
	float fFrontSideAngleCosine = cosf( fFrontAngle );

	if( fAttackedCosine >= fFrontSideAngleCosine )
		bRtnValue = true;
	else
		bRtnValue = false;*/
	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vDir = pChar->GetWorldPosition() - pOwner->GetWorldPosition();

	float fAttackedCosine = D3DXVec3Dot( &vTargetDir, &vDir );

	float fFrontAngle = DEGtoRAD(FLOAT90 );
	float fFrontSideAngleCosine = cosf( fFrontAngle );

	if( fAttackedCosine >= fFrontSideAngleCosine )
		bRtnValue = false;
	else
		bRtnValue = true;

	return bRtnValue;
}

bool ioRangeSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioRangeSkill::IsCanStateEscape() const
{
	if( m_bStateEscapeOnStand || m_bStateEscapeOnAir || m_bStateEscapeOnDown )
		return true;

	return false;
}

bool ioRangeSkill::IsCanStateEscapeOnStand( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;

	if( m_bStateEscapeOnStand &&
		!pChar->IsFloatingState() &&
		!pChar->IsApplyDownState(false) &&
		!pChar->IsBlowDownState() &&
		!pChar->IsFloating() )
		return true;

	return false;
}

bool ioRangeSkill::IsCanStateEscapeOnAir( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;
	if( !m_bEnableJumpingSkill )	return false;

	if( m_bStateEscapeOnAir && pChar->IsFloating() )
		return true;

	return false;
}

bool ioRangeSkill::IsCanStateEscapeOnDown( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;
	if( !m_bEnableDownSkill )	return false;

	if( m_bStateEscapeOnDown && !pChar->IsFloating() &&
		( pChar->IsApplyDownState( false ) || pChar->IsBlowDownState() ) )
		return true;

	return false;
}


