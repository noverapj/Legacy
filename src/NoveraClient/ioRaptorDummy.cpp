
#include "stdafx.h"

#include "ioRaptorDummy.h"

#include "ioVoiceChat/Fourier.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "SkillDefine.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

ioRaptorDummy::ioRaptorDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
}

ioRaptorDummy::~ioRaptorDummy()
{
}

void ioRaptorDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
}

void ioRaptorDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case DCS_DASH:
		ProcessDashState( fTimePerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioRaptorDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
	}

	SetWorldPosition( pOwner->GetWorldPosition() );
	SetWorldOrientation( pOwner->GetTargetRot() );

	m_dwProcessTime = dwCurTime;
}

void ioRaptorDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsObejctEquipState() )
	{
		SetDieState( true );
		return;
	}

	if( pOwner->GetState() == CS_RUN )
	{
		SetMoveState();
		return;
	}
	else if( pOwner->GetState() == CS_DASH )
	{
		SetDashState();
		return;
	}
	else if( pOwner->GetState() == CS_DELAY && pOwner->GetWeaponItemCode() == UMT_DR_SPIDER )
		return;

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioRaptorDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsObejctEquipState() )
	{
		SetDieState( true );
		return;
	}

	if( pOwner->GetState() == CS_DELAY )
	{
		SetDelayState( false );
		return;
	}
	else if( pOwner->GetState() == CS_DASH )
	{
		SetDashState();
		return;
	}
	
	if( pOwner->GetWeaponItemCode() == UMT_DR_SPIDER && ( pOwner->GetState() == CS_DELAY || pOwner->GetState() == CS_RUN ) )
		return;

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioRaptorDummy::ProcessDashState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !pOwner->IsObejctEquipState() )
	{
		SetDieState( true );
		return;
	}

	if( pOwner->GetState() == CS_DELAY )
	{
		SetDelayState( false );
		return;
	}

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioRaptorDummy::SetAttackState()
{
	ioDummyChar::SetAttackState();
}

void ioRaptorDummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsObejctEquipState() )
	{
		SetDieState( true );
		return;
	}

	if( pOwner->GetState() == CS_DELAY || pOwner->GetState() == CS_DROP_ZONE_DOWN )
	{
		SetDelayState( false );
		return;
	}

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		m_iReserveAttackCode = 0;
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

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
				ioWeapon *pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				if( pWeapon )
				{
					pWeapon->SetPosition( GetMidPositionByRate() );

					ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
					if( pZone )
					{
						ioBaseChar *pChar = pZone->GetOwner();
						if( pChar )
						{
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
							pZone->SetCollisionDir( vDir );
							pZone->SetZoneValue( true, false );
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
		DWORD dwUseWeaponItemCode = pOwner->GetWeaponItemCode();
		float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
		if( fHeightGap > 0.0f && dwUseWeaponItemCode == UMT_DR_SPIDER)
			return;
		SetDelayState( false );
		return;
	}
}

void ioRaptorDummy::SetMoveState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_MOVE;

	CheckChargeWeaponDead();

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = FLOAT1;

			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, fAniRate );
		}
	}

	if( m_DummyCharType == DCT_RUSH )
		SetAttachWeapon();
}

void ioRaptorDummy::SetMoveStateMid()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_State != DCS_MOVE )
		return;

	if( !m_MoveAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_MoveAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = FLOAT1;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = m_pGroup->GetLoopInPlay( iAniID );
			if( pAni ) 
				dwPlayTime = pAni->GetCurTime();

			m_pGroup->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, fAniRate );
		}
	}
}

void ioRaptorDummy::SetDashState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_State = DCS_DASH;

	CheckChargeWeaponDead();

	if( !m_DashAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DashAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAni( FLOAT100, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = FLOAT1;

			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, fAniRate );
		}
	}

	if( m_DummyCharType == DCT_RUSH )
		SetAttachWeapon();
}

void ioRaptorDummy::SetDashStateMid()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_State != DCS_DASH )
		return;

	if( !m_DashAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DashAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT100, true );
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = FLOAT1;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = m_pGroup->GetLoopInPlay( iAniID );
			if( pAni ) 
				dwPlayTime = pAni->GetCurTime();

			m_pGroup->SetLoopAniInMid( iAniID, dwPlayTime, FLOAT100, FLOAT1, fAniRate );
		}
	}
}






