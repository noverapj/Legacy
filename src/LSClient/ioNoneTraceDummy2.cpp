#include "StdAfx.h"
#include "ioNoneTraceDummy2.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"
#include "ioFloatThrowWeapon.h"

ioNoneTraceDummy2::ioNoneTraceDummy2( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_vCirclePos = ioMath::VEC3_ZERO;
}

ioNoneTraceDummy2::~ioNoneTraceDummy2(void)
{
}

void ioNoneTraceDummy2::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	// Weapon
	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fWeaponGapDistance = rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fRevisionRate = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );

	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );

	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );

	m_fFireHightOffset	= rkLoader.LoadFloat_e( "fire_height_offset", 0.0f );
	m_fFireFrontOffset	= rkLoader.LoadFloat_e( "fire_front_offset", 0.0f );

	m_iNormalAttackCode = rkLoader.LoadInt_e( "normal_attack_code", 0 );
}

void ioNoneTraceDummy2::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimerPerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimerPerSec );
}

void ioNoneTraceDummy2::ProcessDelayState( float fTimePerSec )
{
	if( CheckEnableAttackState() )
	{
		SetAttackState();
	}
}

void ioNoneTraceDummy2::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	vCurPos.y += m_fFireHightOffset;
	vCurPos += ( vCurDir * m_fFireFrontOffset );

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon *pWeapon = NULL;
				pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );

				if( pWeapon )
				{
					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						pZone->SetPosition( GetMidPositionByRate() );
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
							pZone->SetCollisionDir( vDir );
							pZone->SetZoneValue( true, false );
						}
					}
					else
					{
						CheckWeaponFloatInfo();
						if ( m_vWeaponMoveInfo.m_vMoveDir != ioMath::VEC3_ZERO )
						{
							pWeapon->SetMoveDir( m_vWeaponMoveInfo.m_vMoveDir );
							pWeapon->SetMoveSpeed( m_vWeaponMoveInfo.m_fMoveSpeed );
							pWeapon->SetFloatPower( m_vWeaponMoveInfo.m_fFloatPower );
						}

						ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
						if( pFloatWeapon )
						{
							pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
						}
					}
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//
	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioNoneTraceDummy2::CheckWeaponFloatInfo()
{
	m_vWeaponMoveInfo.m_vMoveDir = ioMath::VEC3_ZERO;
	m_vWeaponMoveInfo.m_fMoveSpeed = 0.0f;
	m_vWeaponMoveInfo.m_fFloatPower = 0.0f;

	if ( m_vCirclePos == ioMath::VEC3_ZERO )
		return;
	// CharDir
	D3DXVECTOR3 vdummyPos = GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vdummyPos;
	vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	D3DXVec3Normalize( &vCharDir, &vCharDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCharDir );
	SetWorldOrientation( qtRot );

	D3DXVECTOR3 vTargetPos = m_vCirclePos;

	float fGapDistance = m_fWeaponGapDistance;
	D3DXVECTOR3 vPos = vTargetPos + (vCharDir * fGapDistance);
	D3DXVECTOR3 vDir = vPos - vdummyPos;
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( fRange > 0.0f )
	{
		fRate = fRange / (m_fCircleRange+fGapDistance);

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = GetMidHeightByRate(m_fRevisionRate) - vdummyPos.y;
		float fGapHeight2 = vdummyPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}


		m_vWeaponMoveInfo.m_vMoveDir = vDir;
		m_vWeaponMoveInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
		m_vWeaponMoveInfo.m_fFloatPower = fCurWeaponFloatPower;
	}
}

float ioNoneTraceDummy2::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioNoneTraceDummy2::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioNoneTraceDummy2::SetAttackState()
{
	m_iReserveAttackCode = m_iNormalAttackCode;
	ioDummyChar::SetAttackState();
}

void ioNoneTraceDummy2::SetAttackState( int iCode )
{
	m_iReserveAttackCode = iCode;
	ioDummyChar::SetAttackState();
}
