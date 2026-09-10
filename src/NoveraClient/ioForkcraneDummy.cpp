#include "StdAfx.h"
#include "ioForkcraneDummy.h"
#include "ioZoneEffectWeapon.h"
#include "ioExcavationManager.h"
#include "ioMapCollisionHelper.h"


ioForkcraneDummy::ioForkcraneDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
}


ioForkcraneDummy::~ioForkcraneDummy(void)
{
}

void ioForkcraneDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
}

void ioForkcraneDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
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

void ioForkcraneDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
	}

	D3DXVECTOR3 vTotalMove = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vOwnerMove = ioMath::VEC3_ZERO;

	vTotalMove = pOwner->GetWorldPosition() - GetWorldPosition();
	vOwnerMove = vTotalMove;

	bool bCol = false;

	if( ReCalculateMoveVector( this, &vTotalMove, bCol, m_vMountEntityList ) )
	{
		if( vOwnerMove != vTotalMove )
		{
			pOwner->SetWorldPosition( GetWorldPosition() );
			pOwner->Translate( vTotalMove );
		}
	}

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdatePosMountedEntityList();
	}

	SetWorldPosition( pOwner->GetWorldPosition() );
	SetWorldOrientation( pOwner->GetTargetRot() );

	m_dwProcessTime = dwCurTime;
}

void ioForkcraneDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckDieState( pOwner ) )
	{
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

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioForkcraneDummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckDieState( pOwner ) )
	{
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

	if( CheckEnableAttackState() )
	{
		SetAttackState();
		return;
	}
}

void ioForkcraneDummy::ProcessDashState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckDieState( pOwner ) )
	{
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

void ioForkcraneDummy::SetAttackState()
{
	ioDummyChar::SetAttackState();
}

void ioForkcraneDummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( CheckDieState( pOwner ) )
	{
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

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
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
							D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
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
		SetDelayState( false );
		return;
	}
}

bool ioForkcraneDummy::CheckDieState( ioBaseChar* pOwner )
{
	if( pOwner == NULL )
		return false;

	if( pOwner->IsExcavating() )
		return false;

	if( pOwner->IsWereWolfState() )
		return false;

	SetDieState( true );
	return true;
}

void ioForkcraneDummy::SetMoveState()
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
			m_pGroup->ClearAllActionAni( 100.0f, true );
			m_pGroup->ClearAllLoopAni( 100.0f, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = 1.0f;

			m_pGroup->SetLoopAni( iAniID, 100.0f, 1.0f, fAniRate );
		}
	}

	if( m_DummyCharType == DCT_RUSH )
		SetAttachWeapon();
}

void ioForkcraneDummy::SetMoveStateMid()
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
			m_pGroup->ClearAllActionAni( 100.0f, true );
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, 100.0f, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = 1.0f;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = m_pGroup->GetLoopInPlay( iAniID );
			if( pAni ) 
				dwPlayTime = pAni->GetCurTime();

			m_pGroup->SetLoopAniInMid( iAniID, dwPlayTime, 100.0f, 1.0f, fAniRate );
		}
	}
}

void ioForkcraneDummy::SetDashState()
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
			m_pGroup->ClearAllActionAni( 100.0f, true );
			m_pGroup->ClearAllLoopAni( 100.0f, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = 1.0f;

			m_pGroup->SetLoopAni( iAniID, 100.0f, 1.0f, fAniRate );
		}
	}

	if( m_DummyCharType == DCT_RUSH )
		SetAttachWeapon();
}

void ioForkcraneDummy::SetDashStateMid()
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
			m_pGroup->ClearAllActionAni( 100.0f, true );
			m_pGroup->ClearAllLoopAniExceptOne( iAniID, 100.0f, true );

			float fAniRate = pOwner->GetRunDashAniRate();
			if( fAniRate <= 0.0f )
				fAniRate = 1.0f;

			DWORD dwPlayTime = 0;
			ioAnimation *pAni = m_pGroup->GetLoopInPlay( iAniID );
			if( pAni ) 
				dwPlayTime = pAni->GetCurTime();

			m_pGroup->SetLoopAniInMid( iAniID, dwPlayTime, 100.0f, 1.0f, fAniRate );
		}
	}
}

void ioForkcraneDummy::UpdatePosMountedEntityList()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( !m_bMountEnable )
		return;

	int iCount = m_vMountEntityList.size();
	for( int i = 0; i < iCount; ++i )
	{
		ioGameEntity* pEntity = m_vMountEntityList[i];
		if( pEntity == NULL )
			continue;

		D3DXVECTOR3 vEntityPos = pEntity->GetWorldPosition();
		D3DXVECTOR3 vDummyPos = GetWorldPosition();
		D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

		// 더미 위치에 따른 엔티티 위치 변화
		D3DXVECTOR3 vDiff = vDummyPos - vEntityPos;
		vEntityPos = vOwnerPos - vDiff;

		pEntity->SetWorldPosition( vEntityPos );

		// 더미 회전에 따른 엔티티 회전 위치 변화
		D3DXVECTOR3 vDummyDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVec3Normalize( &vDummyDir, &vDummyDir );

		D3DXVECTOR3 vOwnerDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXVec3Normalize( &vOwnerDir, &vOwnerDir );

		float fAddDotValue = D3DXVec3Dot( &vDummyDir, &vOwnerDir );
		float fAddGapAngle = RADtoDEG( acosf(fAddDotValue) );

		D3DXVECTOR3 vCross;
		D3DXVec3Cross( &vCross, &vDummyDir, &vOwnerDir );
		D3DXVec3Normalize( &vCross, &vCross );
		if( vCross.y < 0 )
		{
			fAddGapAngle = 360.f - fAddGapAngle;
		}

		if( fAddGapAngle > 0.1f )
		{
			D3DXVECTOR3 vEntityToDummyDir = (vEntityPos - vDummyPos);
			vEntityToDummyDir.y = 0.f;
			D3DXVec3Normalize( &vEntityToDummyDir, &vEntityToDummyDir );

			D3DXVECTOR3 vTemp = vEntityPos;
			vTemp.y = vDummyPos.y;
			D3DXVECTOR3 vGap = vDummyPos - vTemp;
			float fDist = D3DXVec3Length( &vGap );
			float fDotValue = D3DXVec3Dot( &vEntityToDummyDir, &ioMath::UNIT_Z );
			float fGapAngle = RADtoDEG( acosf(fDotValue) );

			D3DXVec3Cross( &vCross, &ioMath::UNIT_Z, &vEntityToDummyDir );
			D3DXVec3Normalize( &vCross, &vCross );
			if( vCross.y < 0 )
			{
				fGapAngle = 360.f - fGapAngle;
			}

			D3DXVECTOR3 vPos = D3DXVECTOR3( vDummyPos.x + fDist * sinf(DEGtoRAD(fGapAngle + fAddGapAngle)),
				vEntityPos.y,
				vDummyPos.z + fDist * cosf(DEGtoRAD(fGapAngle + fAddGapAngle)) );

			pEntity->SetWorldPosition( vPos );
		}
	}
}

bool ioForkcraneDummy::ReCalculateMoveVector( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol, const MountedEntityList& rkMount )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return false;

	ioPlayStage* pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	bCol = false;

	ioAxisAlignBox kAxisBox = pDummy->GetWorldAxisBox();

	D3DXVECTOR3 vUpMinPos = kAxisBox.GetMinPos();
	if( !pDummy->IsDisableRecalculateByterrain() )
		vUpMinPos.y += ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
	kAxisBox.SetMinPos( vUpMinPos );

	ioAxisAlignBox kAxisMoveBox = kAxisBox;
	kAxisMoveBox.Translate( *pMove );
	kAxisMoveBox.Merge( kAxisBox );

	D3DXVECTOR3 vMoveDir, vCenterDir;
	D3DXVec3Normalize( &vMoveDir, pMove );

	bool bForceMove = false;
	if( pDummy->GetForceAmt() > 0.0f )
		bForceMove = true;

	
	GameEntityList::const_iterator iEntity;
	for( iEntity = pStage->GetGameEntityList().begin(); iEntity != pStage->GetGameEntityList().end(); ++iEntity )
	{
		ioGameEntity *pOtherEntity = *iEntity;
		ioBaseChar *pOtherChar = ToBaseChar( pOtherEntity );
		ioDummyChar *pOtherDummy = ToDummyChar( pOtherEntity );
		ioPushStruct *pOtherPushStruct = ToPushStruct( pOtherEntity );

		bool bContinute = false;
		MountedEntityList::const_iterator iter = rkMount.begin();
		for( ; iter != rkMount.end(); ++iter )
		{
			ioGameEntity* pMountedEntity = *iter;
			if( pMountedEntity == pOtherEntity )
			{
				bContinute = true;
				break;
			}
		}

		if( bContinute )
			continue;

		if( pDummy == pOtherDummy )
			continue;

		if( pOtherPushStruct && pOtherPushStruct->IsEntityCollisionSkipState( NULL ) )
			continue;

		if( pOtherChar && pOtherChar->IsEntityCollisionSkipState( pDummy ) )
			continue;

		if( pOtherChar && pDummy->IsDummyCharCollisionSkipState( pOtherChar ) )
			continue;

		if( pOtherDummy && pDummy->IsDummyVsDummyCollisionSkipState( pOtherDummy ) )
			continue;

		if( pOtherChar && pDummy->IsCharCollisionSkipState(pOtherChar->GetTeam()) )
			continue;

		if( !ioMapCollisionHelper::IsCollisionCheckRange( pDummy->GetWorldPosition(), pOtherEntity->GetWorldPosition() ) )
			continue;

		const ioAxisAlignBox &rkMountBox = pOtherEntity->GetWorldAxisBox();

		if( !ioMath::TestIntersection( kAxisMoveBox, rkMountBox ) )
			continue;

		vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
		vCenterDir.y = 0.0f;

		if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// ??
			continue;

		if( ioMapCollisionHelper::ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, bForceMove, pMove ) )
		{
			kAxisMoveBox = kAxisBox;
			kAxisMoveBox.Translate( *pMove );
		}
		else
		{
			return false;
		}
	}

	return true;
}