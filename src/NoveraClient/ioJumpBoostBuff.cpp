

#include "stdafx.h"

#include "ioJumpBoostBuff.h"

#include "ioBaseChar.h"

ioJumpBoostBuff::ioJumpBoostBuff()
{
	m_dwPreCheckTime = 0;
	m_dwFlyBoostEndTime = 0;
	m_bSetFlyState = false;

	m_fPreMoveSpeed = 0.0f;

	m_dwCurEffectID = -1;

	m_fOwnerCharBottomHeight = 0.0;
}

ioJumpBoostBuff::ioJumpBoostBuff( const ioJumpBoostBuff &rhs )
: ioBuff( rhs ),
 m_fFlyStartJumpPower( rhs.m_fFlyStartJumpPower ),
 m_fFlyJumpPower( rhs.m_fFlyJumpPower ),
 m_fFlyMoveSpeed( rhs.m_fFlyMoveSpeed ),
 m_fFlyFrictionAmt( rhs.m_fFlyFrictionAmt ),
 m_fFlyGravityAmt( rhs.m_fFlyGravityAmt ),
 m_fFlyOffHeight( rhs.m_fFlyOffHeight ),
 m_dwFlyOffTime( rhs.m_dwFlyOffTime ),
 m_FlyDelayAnimation( rhs.m_FlyDelayAnimation ),
 m_FlyDelayEffect( rhs.m_FlyDelayEffect ),
 m_FlyMoveAnimation( rhs.m_FlyMoveAnimation ),
 m_FlyMoveEffect( rhs.m_FlyMoveEffect ),
 m_FlyBoostAnimation( rhs.m_FlyBoostAnimation ),
 m_FlyBoostEffect( rhs.m_FlyBoostEffect ),
 m_dwFlyBoostDuration( rhs.m_dwFlyBoostDuration ),
 m_FlyMoveType( rhs.m_FlyMoveType ),
 m_dwFlyRotateTime( rhs.m_dwFlyRotateTime ),
 m_FlyExtraEffect( rhs.m_FlyExtraEffect ),
 m_bEnableEndJump( rhs.m_bEnableEndJump ),
 m_fEndJumpPower( rhs.m_fEndJumpPower ),
 m_bMustEndJump( rhs.m_bMustEndJump ),
 m_bInitCurSpeed( rhs.m_bInitCurSpeed ),
 m_bIsCanStopByKeyInput( rhs.m_bIsCanStopByKeyInput ),
 m_bUseNoSkillState( rhs.m_bUseNoSkillState ),
 m_bEscapeCatchState( rhs.m_bEscapeCatchState ),
 m_bUseFireAttack( rhs.m_bUseFireAttack ),
 m_stAreaWeaponName( rhs.m_stAreaWeaponName ),
 m_fAreaWeaponOffset( rhs.m_fAreaWeaponOffset ),
 m_stAttackAni( rhs.m_stAttackAni ),
 m_fAttackAniRate( rhs.m_fAttackAniRate ),
 m_fMaxHeight( rhs.m_fMaxHeight ),
 m_bEnableBoostAttack( rhs.m_bEnableBoostAttack ),
 m_BoostAttackAttribute( rhs.m_BoostAttackAttribute )
{
	m_dwPreCheckTime = 0;
	m_dwFlyBoostEndTime = 0;
	m_bSetFlyState = false;

	m_fPreMoveSpeed = 0.0f;

	m_dwCurEffectID = -1;

	m_dwAttackMotionEndTime = 0;

	m_fOwnerCharBottomHeight = 0.0f;
}

ioJumpBoostBuff::~ioJumpBoostBuff()
{
}

void ioJumpBoostBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fFlyStartJumpPower = rkLoader.LoadFloat_e( "fly_start_jump_power", 0.0f );
	m_fFlyJumpPower = rkLoader.LoadFloat_e( "fly_jump_power", 0.0f );
	m_fFlyFrictionAmt = rkLoader.LoadFloat_e( "fly_friction_amt", 0.0f );
	m_fFlyMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_fFlyOffHeight = rkLoader.LoadFloat_e( "fly_off_height", 0.0f );
	m_fFlyGravityAmt = rkLoader.LoadFloat_e( "fly_gravity_amt", 0.0f );
	m_dwFlyOffTime = rkLoader.LoadInt_e( "fly_off_time", 0 );

	m_bEnableEndJump = rkLoader.LoadBool_e( "enable_end_jump", false );
	m_fEndJumpPower = rkLoader.LoadFloat_e( "end_jump_power", 0.1f );
	m_bMustEndJump = rkLoader.LoadBool_e( "end_jump_must", false );
	m_bInitCurSpeed = rkLoader.LoadBool_e( "end_speed_init", false );
	m_bIsCanStopByKeyInput = rkLoader.LoadBool_e( "end_by_key_input", false );

	char szBuf[MAX_PATH];
	
	rkLoader.LoadString_e( "fly_delay_animation", "", szBuf, MAX_PATH );
	m_FlyDelayAnimation = szBuf;
	rkLoader.LoadString_e( "fly_delay_effect", "", szBuf, MAX_PATH );
	m_FlyDelayEffect = szBuf;

	rkLoader.LoadString_e( "fly_move_animation", "", szBuf, MAX_PATH );
	m_FlyMoveAnimation = szBuf;
	rkLoader.LoadString_e( "fly_move_effect", "", szBuf, MAX_PATH );
	m_FlyMoveEffect = szBuf;

	rkLoader.LoadString_e( "fly_boost_animation", "", szBuf, MAX_PATH );
	m_FlyBoostAnimation = szBuf;
	rkLoader.LoadString_e( "fly_boost_effect", "", szBuf, MAX_PATH );
	m_FlyBoostEffect = szBuf;

	rkLoader.LoadString_e( "fly_extra_effect", "", szBuf, MAX_PATH );
	m_FlyExtraEffect = szBuf;

	m_dwFlyBoostDuration = rkLoader.LoadInt_e( "fly_boost_duration", 0 );

	m_FlyMoveType = (FlyMoveType)rkLoader.LoadInt_e( "fly_move_type", FMT_NORMAL );

	m_dwFlyRotateTime = rkLoader.LoadInt_e( "fly_rotate_speed", 0 );

	m_bUseNoSkillState = rkLoader.LoadBool_e( "use_no_skill_state", false );

	m_bEscapeCatchState = rkLoader.LoadBool_e( "escape_catch_state", false );

	//////////////////////////////////////////////////////////////////////////
	m_bUseFireAttack = rkLoader.LoadBool_e( "use_fire_attack", false );
	
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_stAreaWeaponName = szBuf;
	m_fAreaWeaponOffset = rkLoader.LoadFloat_e( "area_weapon_offset", 0.f );

	rkLoader.LoadString_e( "attack_motion_ani", "", szBuf, MAX_PATH );
	m_stAttackAni = szBuf;
	m_fAttackAniRate = rkLoader.LoadFloat_e( "attack_motion_ani_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////

	m_fMaxHeight	= rkLoader.LoadFloat_e( "owner_control_max_height", 0.0f );

	m_bEnableBoostAttack = rkLoader.LoadBool_e( "enable_boost_attack", false );
	wsprintf_e( szBuf, "boost_attack" );
	LoadAttackAttribute( szBuf, m_BoostAttackAttribute, rkLoader );
}

ioBuff* ioJumpBoostBuff::Clone()
{
	return new ioJumpBoostBuff( *this );
}

void ioJumpBoostBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	m_bSetFlyState = false;
	m_dwFlyBoostEndTime = 0;
	m_pOwner->SetFlyStateValue( m_Name,
								m_fFlyStartJumpPower, m_fFlyMoveSpeed,
								m_fFlyFrictionAmt, m_fFlyGravityAmt,
								m_dwFlyRotateTime,
								m_fFlyOffHeight,
								m_dwFlyOffTime,
								m_FlyMoveType );

	if( m_FlyMoveType != FMT_BIRD )
	{
		m_pOwner->AttachEffect( m_FlyBoostEffect, NULL );
	}

	m_dwPreCheckTime = 0;
	m_fPreMoveSpeed = 0.0f;
}

bool ioJumpBoostBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bEscapeCatchState && pOwner && pOwner->IsCatchMode() )
	{
		pOwner->SetEscapeCatchState();
	}

	m_bSetFlyState = false;
	m_dwFlyBoostEndTime = 0;
	m_pOwner->SetFlyStateValue( m_Name,
								m_fFlyStartJumpPower, m_fFlyMoveSpeed,
								m_fFlyFrictionAmt, m_fFlyGravityAmt,
								m_dwFlyRotateTime,
								m_fFlyOffHeight,
								m_dwFlyOffTime,
								m_FlyMoveType );

	if( m_FlyMoveType != FMT_BIRD )
	{
		m_pOwner->AttachEffect( m_FlyBoostEffect, NULL );
	}
	
	m_dwPreCheckTime = 0;
	m_fPreMoveSpeed = 0.0f;
	return true;
}

void ioJumpBoostBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}



	if( m_bSetFlyState )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		if( m_pOwner->GetState() != CS_JUMP )
		{
			if( !IsUnlimitedPassiveType() )
			{
				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}

		if( m_bIsCanStopByKeyInput && m_pOwner->IsNeedProcess() && m_pOwner->IsAttackKey() )
		{
			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << m_pOwner->GetCharName();
			kPacket << GetName();
			kPacket << (int)(BS_DIE);
			P2PNetwork::SendToAllPlayingUser( kPacket );

			SetReserveEndBuff();
			return;
		}

		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		if( !pGrp )
			return;
		//¨ù¡©CIAC¡ío¢Ò¡× A©¬¡Æ¢®
		if( m_bUseFireAttack && m_pOwner->IsNeedProcess() && m_pOwner->IsAttackKey() && m_dwAttackMotionEndTime < dwCurTime )
		{
			AreaWeaponFire();

			SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
			kPacket << m_pOwner->GetCharName();
			kPacket << GetName();
			kPacket << (int)(BS_ATTACK);
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
		else if( m_bUseFireAttack && m_dwAttackMotionEndTime && m_dwAttackMotionEndTime < dwCurTime )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_FlyMoveAnimation, FLOAT100 );

			m_dwAttackMotionEndTime = 0;
		}

		if( m_dwFlyBoostEndTime != 0 && m_dwFlyBoostEndTime < FRAMEGETTIME() )
		{
			switch( m_FlyMoveType )
			{
			case FMT_NORMAL:
				{
					float fHeightGap = m_pOwner->GetWorldPosition().y - m_pOwner->GetBottomHeight();

					if( fHeightGap > 0.1f )
						SetFlyDelayState();
				}
				break;
			case FMT_BIRD:
			case FMT_DIRECT:
			case FMT_ROTATE:
				{
					if( m_pOwner->GetCurSpeed() > 0.0f )
					{
						SetFlyMoveState();
					}
					else
					{
						SetFlyDelayState();
					}
				}
				break;
			}
		}
		else if( m_pOwner->IsSetBoost() )
		{
			bool bBoostPowerForce = true;
			float fHeightGap = 0.0f;
			if( m_pOwner->GetBottomHeight() > 0.0f)
				m_fOwnerCharBottomHeight = m_pOwner->GetBottomHeight();
			fHeightGap = m_pOwner->GetWorldPosition().y - m_fOwnerCharBottomHeight;

			if( fHeightGap >= m_fMaxHeight )
				bBoostPowerForce = false;

			switch( m_FlyMoveType )
			{
			case FMT_DIRECT:
			case FMT_ROTATE:
				{
					m_pOwner->SetCurJumpPowerAmt( 0.0f );
					m_pOwner->SetGravityAmt( 0.0f );

					if(bBoostPowerForce)
						m_pOwner->SetJumpPower( m_fFlyJumpPower );
					else
						m_pOwner->SetJumpPower( 0.0f );
				}
				break;
			default:
				{
					if(bBoostPowerForce)
						m_pOwner->SetJumpPower( m_fFlyJumpPower );
					else
						m_pOwner->SetJumpPower( 0.0f );
				}
				break;
			}

			SetFlyBoostState();
		}
		else if( m_fPreMoveSpeed == 0.0f && m_pOwner->GetCurSpeed() > 0.0f )
		{
			SetFlyMoveState();
		}

		if( m_pOwner->CheckStandOnGround() )
			m_pOwner->SetJumpBoostState(false);
	}
	else if( m_pOwner->GetState() == CS_JUMP )
	{
		m_bSetFlyState = true;
		if( m_FlyMoveType == FMT_BIRD )
		{
			m_pOwner->SetInvisibleState( true );
			m_pOwner->AttachEffect( m_FlyExtraEffect, NULL, false );
			m_pOwner->SetCurMoveSpeed( m_pOwner->GetMaxSpeed() );
		}

		SetFlyDelayState();
	}
	else if ( m_bUseNoSkillState && m_pOwner->GetState() != CS_JUMP )
	{
		m_bSetFlyState = true;
		m_pOwner->SetState( CS_JUMP );
		if( m_FlyMoveType == FMT_BIRD )
		{
			m_pOwner->SetInvisibleState( true );
			m_pOwner->AttachEffect( m_FlyExtraEffect, NULL, false );
			m_pOwner->SetCurMoveSpeed( m_pOwner->GetMaxSpeed() );
		}
		
		SetFlyDelayState();

	}
}


void ioJumpBoostBuff::AreaWeaponFire()
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vLookVec = m_pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	int iAniID = pGrp->GetAnimationIdx( m_stAttackAni );
	if ( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fAttackAniRate );

	m_dwAttackMotionEndTime = dwCurTime;
	m_dwAttackMotionEndTime += pGrp->GetAnimationFullTime(iAniID) * m_fAttackAniRate;

	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( m_pOwner->GetCharName(),
		"",
		"",
		m_stAreaWeaponName,
		m_pOwner->GetWorldPosition() + vLookVec*m_fAreaWeaponOffset,
		m_pOwner->GetTargetRot(),
		ioAreaWeapon::CT_NORMAL );
}



void ioJumpBoostBuff::EndBuff()
{
	if( m_pOwner->GetState() == CS_JUMP )
	{
		if( m_bEnableEndJump )
		{
			if( m_bInitCurSpeed )
				m_pOwner->SetCurMoveSpeed( 0.0f );

			m_pOwner->SetSKillEndJumpState( m_fEndJumpPower, false, false, false, m_bMustEndJump );
		}
		else
		{
			m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			m_pOwner->SetFallState( true );
		}
	}

	if( m_FlyMoveType == FMT_BIRD )
		m_pOwner->SetInvisibleState( false );

	bool bGrace = false;
	if( m_FlyMoveType == FMT_NORMAL )
		bGrace = true;

	m_pOwner->EndEffect( m_FlyDelayEffect, bGrace );
	m_pOwner->EndEffect( m_FlyMoveEffect, bGrace );
	m_pOwner->EndEffect( m_FlyBoostEffect, bGrace );
	m_pOwner->EndEffect( m_FlyExtraEffect, bGrace );

	ioBuff::EndBuff();
}

void ioJumpBoostBuff::SetFlyBoostState()
{
	ioEntityGroup *pGrp = m_pOwner->GetGroup();

	if( m_dwFlyBoostEndTime == 0 )
	{
		if( !m_FlyBoostAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetLoopAni( m_FlyBoostAnimation, FLOAT100 );
		}

		bool bGrace = false;
		if( m_FlyMoveType == FMT_NORMAL )
			bGrace = true;

		m_pOwner->EndEffect( m_FlyDelayEffect, bGrace );
		m_pOwner->EndEffect( m_FlyMoveEffect, bGrace );

		bool bDependence = true;
		if( m_FlyMoveType == FMT_BIRD )
			bDependence = false;

		m_pOwner->AttachEffect( m_FlyBoostEffect, NULL, bDependence );
	}

	if( m_bEnableBoostAttack )
	{
		int iAniID	     = pGrp->GetAnimationIdx( m_BoostAttackAttribute.m_AttackAnimation );
		float fTimeRate  = m_BoostAttackAttribute.m_fAttackAniRate;
		DWORD dwPreDelay = m_BoostAttackAttribute.m_dwPreDelay;
		DWORD dwEndDelay = m_BoostAttackAttribute.m_dwEndDelay;

		m_pOwner->RefreshFireTimeList( iAniID, m_BoostAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
		m_pOwner->IncreaseWeaponIndexBase();
	}

	m_pOwner->SetBoostState( false );

	m_dwFlyBoostEndTime = FRAMEGETTIME() + m_dwFlyBoostDuration;
}

void ioJumpBoostBuff::SetFlyDelayState()
{
	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( m_FlyDelayAnimation.IsEmpty() ) return;

	m_dwFlyBoostEndTime = 0;
	m_fPreMoveSpeed = 0.0f;

	bool bGrace = false;
	if( m_FlyMoveType == FMT_NORMAL )
		bGrace = true;

	m_pOwner->EndEffect( m_FlyBoostEffect, bGrace );
	m_pOwner->EndEffect( m_FlyMoveEffect, bGrace );

	bool bDependence = true;
	if( m_FlyMoveType == FMT_BIRD )
		bDependence = false;
	
	m_pOwner->AttachEffect( m_FlyDelayEffect, NULL, bDependence );
}

void ioJumpBoostBuff::SetFlyMoveState()
{
	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( m_FlyMoveAnimation.IsEmpty() ) return;

	if( m_dwFlyBoostEndTime == 0 || !m_FlyBoostAnimation.IsEmpty() )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( m_FlyMoveAnimation, FLOAT100 );
	}

	m_dwFlyBoostEndTime = 0;
	m_fPreMoveSpeed = m_pOwner->GetCurSpeed();

	bool bGrace = false;
	if( m_FlyMoveType == FMT_NORMAL )
		bGrace = true;

	m_pOwner->EndEffect( m_FlyBoostEffect, bGrace );
	m_pOwner->EndEffect( m_FlyDelayEffect, bGrace );

	bool bDependence = true;
	if( m_FlyMoveType == FMT_BIRD )
		bDependence = false;

	m_pOwner->AttachEffect( m_FlyMoveEffect, NULL, bDependence );
}


void ioJumpBoostBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioBaseChar *pCreator = pOwner->GetBaseChar( m_CreateChar );
	if( !pCreator )
		return;

	int iState;
	rkPacket >> iState;
	
	if( (BuffState)iState == BS_DIE )
		SetReserveEndBuff();

	if( (BuffState)iState == BS_ATTACK )
		AreaWeaponFire();
}

bool ioJumpBoostBuff::CheckAxisLimitByBuff( ioBaseChar* pChar )
{
	if( m_pOwner->GetBottomHeight() > 0.0f)
		m_fOwnerCharBottomHeight = m_pOwner->GetBottomHeight();
	float fHeightGap = m_pOwner->GetWorldPosition().y - m_fOwnerCharBottomHeight;
	if( fHeightGap >= m_fMaxHeight )
		return true;
	return false;
}