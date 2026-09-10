
#include "stdafx.h"

#include "ioTargetMoveAttackSkill3.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioTargetMoveAttackSkill3::ioTargetMoveAttackSkill3()
{
	m_dwMotionEndTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_fJumpStartHeight = 0.0f;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_vFallMoveDir = ioMath::VEC3_ZERO;
	m_fFallDuration = 0.0f;
	m_fFallMoveSpeed = 0.0f;
	m_dwCheckGaugeTime = 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_bSetDelay	= false;
}

ioTargetMoveAttackSkill3::ioTargetMoveAttackSkill3( const ioTargetMoveAttackSkill3 &rhs )
	: ioNormalSkill( rhs ),
	  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	  m_bDisableGrowth( rhs.m_bDisableGrowth ),
	  m_fGaugePerTic( rhs.m_fGaugePerTic ),
	  m_dwTicTime( rhs.m_dwTicTime ),
	  m_JumpInfo( rhs.m_JumpInfo ),
	  m_FallAttackInfo( rhs.m_FallAttackInfo ),
	  m_FallLandAttackInfo( rhs.m_FallLandAttackInfo ),
	  m_SpecialAttackInfo( rhs.m_SpecialAttackInfo ),
	  m_ExtendAttackInfo( rhs.m_ExtendAttackInfo ),
	  m_fJumpStopHeight( rhs.m_fJumpStopHeight ),
	  m_szMoveLoopAni( rhs.m_szMoveLoopAni ),
	  m_fMoveLoopAniRate( rhs.m_fMoveLoopAniRate ),
	  m_szDelayLoopAni( rhs.m_szDelayLoopAni ),
	  m_fDelayLoopAniRate( rhs.m_fDelayLoopAniRate ),
	  m_fFallJumpPower( rhs.m_fFallJumpPower ),
	  m_fFallGravityRate( rhs.m_fFallGravityRate ),
	  m_ReadyCircle( rhs.m_ReadyCircle ),
	  m_fCircleRange( rhs.m_fCircleRange ),
	  m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
	  m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
	  m_fCircleOffSet( rhs.m_fCircleOffSet ),
	  m_fUseExtendAttackGauge( rhs.m_fUseExtendAttackGauge )
{
	m_dwMotionEndTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_fJumpStartHeight = 0.0f;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_vFallMoveDir = ioMath::VEC3_ZERO;
	m_fFallDuration = 0.0f;
	m_fFallMoveSpeed = 0.0f;
	m_dwCheckGaugeTime = 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_bSetDelay	= false;
}

ioTargetMoveAttackSkill3::~ioTargetMoveAttackSkill3()
{
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetMoveAttackSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGaugePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwSkillProtectTime = rkLoader.LoadInt_e( "skill_protect_time", 0 );

	LoadAttackAttribute_e( "jump_info", m_JumpInfo, rkLoader );
	LoadAttackAttribute_e( "fall_attack_info", m_FallAttackInfo, rkLoader );
	LoadAttackAttribute_e( "fall_land_attack_info", m_FallLandAttackInfo, rkLoader );
	LoadAttackAttribute_e( "special_attack_info", m_SpecialAttackInfo, rkLoader );
	LoadAttackAttribute_e( "extend_attack_info", m_ExtendAttackInfo, rkLoader );

	m_fJumpStopHeight = rkLoader.LoadFloat_e( "jump_stop_height", 0.0f );
	
	rkLoader.LoadString_e( "delay_loop_ani", "", szBuf, MAX_PATH );
	m_szDelayLoopAni = szBuf;
	m_fDelayLoopAniRate = rkLoader.LoadFloat_e( "delay_loop_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_loop_ani", "", szBuf, MAX_PATH );
	m_szMoveLoopAni = szBuf;
	m_fMoveLoopAniRate = rkLoader.LoadFloat_e( "move_loop_ani_rate", FLOAT1 );

	//마법진 설정
	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	m_fCircleRange = rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	m_fFallJumpPower = rkLoader.LoadFloat_e( "fall_jump_power", 0.0f );
	m_fFallGravityRate = rkLoader.LoadFloat_e( "fall_gravity_rate", FLOAT1 );

	m_fUseExtendAttackGauge = rkLoader.LoadFloat_e( "use_extend_attack_gauge", 0.0f );
}

ioSkill* ioTargetMoveAttackSkill3::Clone()
{
	return new ioTargetMoveAttackSkill3( *this );
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackSkill3::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioNormalSkill::OnSkillStart(pChar) )
		return false;

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( false );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_dwMotionEndTime = 0;
	m_dwCurSkillProtectTime = 0;
	m_SkillState = SS_NONE;
	m_fJumpStartHeight = 0.0f;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_vFallMoveDir = ioMath::VEC3_ZERO;
	m_fFallDuration = 0.0f;
	m_fFallMoveSpeed = 0.0f;
	m_dwCheckGaugeTime = 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_bSetDelay	= false;

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetCameraBuff( pChar );

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;
			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue( pStage );

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue( pStage );			
		}
	}
	
	ioBaseChar *pPlayer = pStage->GetOwnerChar();
	if ( !pPlayer )
		return false;

	if ( pChar->GetTeam() == pPlayer->GetTeam() )
		pChar->SetBottomCircleVisible( true );
	else
		pChar->SetBottomCircleVisible( false );

	return true;
}

void ioTargetMoveAttackSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGaugePerTic / m_fTicGaugeRateByGrowth;
	int nCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetJumpState( pChar );
			}
		}
		break;
	case SS_JUMP:
		{
			if ( pChar->IsNeedProcess() )
			{
				if( m_dwCheckGaugeTime + nCurTicTime <= dwCurTime )
				{
					m_dwCheckGaugeTime = dwCurTime;

					if( fCurTicGauge != 0.0f )
					{
						if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
						{
							if ( m_dwMotionEndTime < dwCurTime )
								SetFallAttackState( pChar, pStage );
							return;
						}
					}
				}
			}

			CheckExtraAniJump( pChar );

			if ( CheckHeight( pChar, pStage ) )
			{
				pChar->SetCurJumpPowerAmt( 0.0f );
				pChar->SetForcePowerAmt( 0.0f );
				pChar->SetGravityAmt( 0.0f );
				m_SkillState = SS_MOVE;

				if ( pChar->IsNeedProcess() )
					SetMoveState( pChar );
				return;
			}
		}
		break;
	case SS_MOVE:
		{
			if ( pChar->IsNeedProcess() )
			{
				if( m_dwCheckGaugeTime + nCurTicTime <= dwCurTime )
				{
					m_dwCheckGaugeTime = dwCurTime;

					if( fCurTicGauge != 0.0f )
					{
						if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
						{
							if ( m_dwMotionEndTime < dwCurTime )
								SetFallAttackState( pChar, pStage );
							return;
						}
					}
				}
			}

			CheckMoveAniState( pChar, false );
			CheckInputKey( pChar, pStage );
		}
		break;
	case SS_FALL_ATTACK:
		{
			if( IsFloatState( pChar ) )
			{
				UpdateMagicCircle( pChar );
				CheckMove( pChar, pStage );
			}
			else
			{
				if ( pChar->IsNeedProcess() )
					SetFallLandAttackState( pChar );
			}
		}
		break;
	case SS_FALL_LAND_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
				m_SkillState = SS_END;
		}
		break;
	case SS_SPECIAL_ATTACK:
		{
			if ( pChar->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
				SetFallAttackState( pChar, pStage );
		}
		break;
	case SS_EXTEND_ATTACK:
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				if ( pChar->IsNeedProcess() )
				{
					ioItem *pOwnerItem = GetOwnerItem();
					if ( pOwnerItem && pOwnerItem->GetCurSkillGuage() <= 0.0f )
						SetFallAttackState( pChar, pStage );
					else
						SetMoveState( pChar );
				}
			}
		}
		break;
	}
}

void ioTargetMoveAttackSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetBottomCircleVisible( true );
}

void ioTargetMoveAttackSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}
}

void ioTargetMoveAttackSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar || !pStage )
		return;
	
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pChar->SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRotAndMoveDirChange( qtRot );

	int nState;
	rkPacket >> nState;

	switch ( (SkillState)nState )
	{
	case SS_MOVE:
		SetMoveState( pChar );
		break;
	case SS_FALL_ATTACK:
		SetFallAttackState(	pChar, pStage );
		break;
	case SS_FALL_LAND_ATTACK:
		SetFallLandAttackState( pChar );
		break;
	case SS_SPECIAL_ATTACK:
		SetSpecialAttackState( pChar );
		break;
	case SS_EXTEND_ATTACK:
		SetExtendAttackState( pChar );
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState == SS_END )
		return true;
	
	return false;
}

bool ioTargetMoveAttackSkill3::IsProtected( int iDefenseBreakType ) const
{
	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurSkillProtectTime > dwCurTime )
		return true;

	return false;
}

bool ioTargetMoveAttackSkill3::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;
	return false;
}

bool ioTargetMoveAttackSkill3::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_MOVE:
	case SS_SPECIAL_ATTACK:
	case SS_EXTEND_ATTACK:
		return true;
	}

	return false;
}

float ioTargetMoveAttackSkill3::GetSkillGravityRate()
{
	switch( m_SkillState )
	{
	case SS_FALL_ATTACK:
		return m_fFallGravityRate;
	}

	return ioSkill::GetSkillGravityRate();
}

bool ioTargetMoveAttackSkill3::IsInvisibleBottomCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if ( !pStage )
		return false;

	ioBaseChar *pPlayer = pStage->GetOwnerChar();
	if ( !pPlayer )
		return false;

	switch( m_SkillState )
	{
	case SS_FALL_ATTACK:
	case SS_FALL_LAND_ATTACK:
	case SS_END:
		return false;
	default:
		{
			if ( pChar->GetTeam() != pPlayer->GetTeam() )
				return true;

			return false;
		}
	}
}

bool ioTargetMoveAttackSkill3::IsCanSwitchSkill() const
{
	switch( m_SkillState )
	{
	case SS_FALL_ATTACK:
	case SS_FALL_LAND_ATTACK:
		return false;
	}

	return ioNormalSkill::IsCanSwitchSkill();
}

bool ioTargetMoveAttackSkill3::IsCanSemiSwitchSkill() const
{
	switch( m_SkillState )
	{
	case SS_FALL_ATTACK:
	case SS_FALL_LAND_ATTACK:
		return false;
	}

	return ioNormalSkill::IsCanSemiSwitchSkill();
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetMoveAttackSkill3::SetJumpState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;
	
	int iAniID = pGrp->GetAnimationIdx( m_JumpInfo.m_AttackAnimation );
	float fTimeRate = m_JumpInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_JumpInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeList( iAniID, m_JumpInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetReservedSliding( m_JumpInfo.m_vForceInfoList, iAniID, fTimeRate, 0 );

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge( pChar->GetCharName() ) );
	m_bReduceGauge = true;

	m_fJumpStartHeight = pChar->GetWorldPosition().y;
	m_SkillState = SS_JUMP;
	pChar->SetSkillMoveEnable( true, true );
	m_dwCheckGaugeTime = FRAMEGETTIME();
}

void ioTargetMoveAttackSkill3::SetMoveState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	pChar->ChangeDirectionExplicitIfDirChanged( true );
	m_bSetDelay	= false;	
	m_dwCheckGaugeTime = FRAMEGETTIME();
	m_SkillState = SS_MOVE;
	pChar->SetSkillMoveEnable( true, true );

	CheckMoveAniState( pChar, true );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << SS_MOVE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill3::SetFallAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_FallAttackInfo.m_AttackAnimation );
	float fTimeRate = m_FallAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_FallAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeList( iAniID, m_FallAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_FallAttackInfo.m_vForceInfoList, iAniID, fTimeRate, 0 );
	
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	CreateMagicCircle( pChar );

	m_SkillState = SS_FALL_ATTACK;
	m_fFallDuration = GetFallDuration( pChar, pStage, pChar->GetCharGravity() );
	m_fFallDuration += FRAMEGETTIME();

	pChar->SetJumpPower( m_fFallJumpPower );
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetBottomCircleVisible( true );

	ioItem *pOwnerItem = GetOwnerItem();
	if ( pOwnerItem )
		pChar->SetCurItemSkillGauge( pOwnerItem->GetName(), 0.0f );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << SS_FALL_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill3::SetFallLandAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_FallLandAttackInfo.m_AttackAnimation );
	float fTimeRate = m_FallLandAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_FallLandAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeList( iAniID, m_FallLandAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );
	
	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_FallLandAttackInfo.m_vForceInfoList, iAniID, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	m_SkillState = SS_FALL_LAND_ATTACK;

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << SS_FALL_LAND_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill3::SetSpecialAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_SpecialAttackInfo.m_AttackAnimation );
	float fTimeRate = m_SpecialAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_SpecialAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeList( iAniID, m_SpecialAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_SpecialAttackInfo.m_vForceInfoList, iAniID, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_SkillState = SS_SPECIAL_ATTACK;
	pChar->SetSkillMoveEnable( false, false );

	ioItem *pOwnerItem = GetOwnerItem();
	if ( pOwnerItem )
		pChar->SetCurItemSkillGauge( pOwnerItem->GetName(), 0.0f );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << SS_SPECIAL_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMoveAttackSkill3::SetExtendAttackState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return;

	int iAniID = pGrp->GetAnimationIdx( m_ExtendAttackInfo.m_AttackAnimation );
	float fTimeRate = m_ExtendAttackInfo.m_fAttackAniRate;
	DWORD dwPreDelay = m_ExtendAttackInfo.m_dwPreDelay;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->RefreshFireTimeList( iAniID, m_ExtendAttackInfo.m_vWeaponInfoList, FTT_SKILL_ATTACK, fTimeRate, dwPreDelay );

	pChar->ClearReservedSliding();
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetReservedSliding( m_ExtendAttackInfo.m_vForceInfoList, iAniID, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + dwPreDelay;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_SkillState = SS_EXTEND_ATTACK;
	pChar->SetSkillMoveEnable( false, false );
	pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), m_fUseExtendAttackGauge );

	if ( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetWorldOrientation();
		kPacket << SS_EXTEND_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

//////////////////////////////////////////////////////////////////////////////////////

bool ioTargetMoveAttackSkill3::CheckHeight( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage )
		return false;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if ( vPos.y >= m_fJumpStartHeight + m_fJumpStopHeight )
		return true;
	return false;
}

void ioTargetMoveAttackSkill3::CheckMoveAniState( ioBaseChar *pChar, bool bFirst )
{
	if ( !pChar )
		return;
	
	if( pChar->IsSettedDirection() )
	{
		bool bChangeDir = pChar->IsChangeDirection();
		if( bChangeDir || bFirst )
		{
			ioEntityGroup *pGrp = pChar->GetGroup();
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );			
			pGrp->SetLoopAni( m_szMoveLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fMoveLoopAniRate );
			m_bSetDelay = false;
		}
	}
	else
	{
		if( !m_bSetDelay )
		{
			ioEntityGroup *pGrp = pChar->GetGroup();
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szDelayLoopAni, FLOAT100, FLOAT1, FLOAT1/m_fDelayLoopAniRate );
			m_bSetDelay = true;
		}
	}
}

void ioTargetMoveAttackSkill3::CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage )
		return;	

	if( pChar->IsAttackKey() )
		SetSpecialAttackState( pChar );
	else if ( pChar->IsDefenseKey() )
		SetExtendAttackState( pChar );
	else if ( pChar->IsJumpKey() )
		SetFallAttackState( pChar, pStage );
}

bool ioTargetMoveAttackSkill3::IsFloatState( ioBaseChar *pChar )
{
	if( !pChar ) 
		return false;

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
		return true;

	return false;
}

void ioTargetMoveAttackSkill3::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		float fGapTime = m_fFallDuration - dwCurTime;

		float fNewSpeed = 0.0f;
		if( fGapTime > 0 )
			fNewSpeed = fRange / (float)( fGapTime / FLOAT1000 );

		m_fFallMoveSpeed = fNewSpeed;
	}

	if( m_fFallMoveSpeed <= 0.0f )
		return;

	D3DXVec3Normalize( &m_vFallMoveDir, &vDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vFallMoveDir * m_fFallMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
			pChar->Translate( vTotalMove );
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	pChar->SetTargetRotToDir( m_vFallMoveDir, true );
}

float ioTargetMoveAttackSkill3::GetFallDuration( ioBaseChar *pChar, ioPlayStage *pStage, float fCurGravity )
{
	if ( !pChar || !pStage )
		return 0.0f;

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vPos.x, vPos.z );

	float fTopTime = 0.0f;
	float fCurTime = 0.0f;

	float fNewGravity = fCurGravity * GetSkillGravityRate();
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fFallJumpPower / fNewGravity;
		fCurTime = fabs( vPos.y - fMapHeight ) / fNewGravity;
	}

	return ( 2 * fTopTime + fCurTime ) * FLOAT1000;
}

//////////////////////////////////////////////////////////////////////////////////////

void ioTargetMoveAttackSkill3::CreateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( !pChar->CheckMagicCircle() )
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, "", m_fCircleOffSet );
}

void ioTargetMoveAttackSkill3::UpdateMagicCircle( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckChangeDirectionForMagicCircle();
		pChar->UpdateMagicCirclePosAll( m_fCircleMoveSpeed, m_fCircleRange, m_fCircleHeightGap, false );

		m_vCirclePos = pChar->GetMagicCirclePos( m_fCircleOffSet, false );
	}
}