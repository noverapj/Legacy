

#include "stdafx.h"

#include "ioMagneticFlyBuff.h"

#include "ioBaseChar.h"

ioMagneticFlyBuff::ioMagneticFlyBuff()
{
	m_dwPreCheckTime = 0;
	m_dwFlyBoostEndTime = 0;
	m_bSetFlyState = false;

	m_fPreMoveSpeed = 0.0f;

	m_dwCurEffectID = -1;
}

ioMagneticFlyBuff::ioMagneticFlyBuff( const ioMagneticFlyBuff &rhs )
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
 m_fAttackAniRate( rhs.m_fAttackAniRate )
{
	m_dwPreCheckTime = 0;
	m_dwFlyBoostEndTime = 0;
	m_bSetFlyState = false;

	m_fPreMoveSpeed = 0.0f;

	m_dwCurEffectID = -1;

	m_dwAttackMotionEndTime = 0;
}

ioMagneticFlyBuff::~ioMagneticFlyBuff()
{
}

void ioMagneticFlyBuff::LoadProperty( ioINILoader &rkLoader )
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
}

ioBuff* ioMagneticFlyBuff::Clone()
{
	return new ioMagneticFlyBuff( *this );
}

void ioMagneticFlyBuff::StartBuff( ioBaseChar *pOwner )
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

bool ioMagneticFlyBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioMagneticFlyBuff::ProcessBuff( float fTimePerSec )
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

		if( m_pOwner->GetState() != CS_FLY )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
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
				SetFlyDelayState();
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
			switch( m_FlyMoveType )
			{
			case FMT_DIRECT:
			case FMT_ROTATE:
				m_pOwner->SetCurJumpPowerAmt( 0.0f );
				m_pOwner->SetGravityAmt( 0.0f );

				m_pOwner->SetJumpPower( m_fFlyJumpPower );
				break;
			default:
				m_pOwner->SetJumpPower( m_fFlyJumpPower );
				break;
			}

			SetFlyBoostState();
		}
		else if( m_fPreMoveSpeed == 0.0f && m_pOwner->GetCurSpeed() > 0.0f )
		{
			SetFlyMoveState();
		}
	}
	else if( m_pOwner->GetState() == CS_FLY )
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
	else if ( m_bUseNoSkillState && m_pOwner->GetState() != CS_FLY )
	{
		m_bSetFlyState = true;
		m_pOwner->SetState( CS_FLY );
		if( m_FlyMoveType == FMT_BIRD )
		{
			m_pOwner->SetInvisibleState( true );
			m_pOwner->AttachEffect( m_FlyExtraEffect, NULL, false );
			m_pOwner->SetCurMoveSpeed( m_pOwner->GetMaxSpeed() );
		}
		
		SetFlyDelayState();

	}
	else if( m_pOwner->GetState() != CS_USING_SKILL )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
}


void ioMagneticFlyBuff::AreaWeaponFire()
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



void ioMagneticFlyBuff::EndBuff()
{
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

void ioMagneticFlyBuff::SetFlyBoostState()
{
	if( m_dwFlyBoostEndTime == 0 )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
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

	m_pOwner->SetBoostState( false );

	m_dwFlyBoostEndTime = FRAMEGETTIME() + m_dwFlyBoostDuration;
}

void ioMagneticFlyBuff::SetFlyDelayState()
{
	ioEntityGroup *pGrp = m_pOwner->GetGroup();
	if( m_FlyDelayAnimation.IsEmpty() ) return;

	if( m_dwFlyBoostEndTime == 0 || !m_FlyBoostAnimation.IsEmpty() )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->SetLoopAni( m_FlyDelayAnimation, FLOAT100 );
	}

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

void ioMagneticFlyBuff::SetFlyMoveState()
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


void ioMagneticFlyBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
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
