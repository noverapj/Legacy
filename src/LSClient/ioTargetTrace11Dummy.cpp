#include "StdAfx.h"
#include "ioTargetTrace11Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTrace11Dummy::ioTargetTrace11Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_iAreaWeaponIndex = -1;
	m_vAreaWeaponPos = ioMath::VEC3_ZERO;
}


ioTargetTrace11Dummy::~ioTargetTrace11Dummy(void)
{
}

void ioTargetTrace11Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
	m_bAttachAreaWeapon = rkLoader.LoadBool_e( "attach_area_weapon", false );
	m_bDestroyAfterEat = rkLoader.LoadBool_e( "destroy_after_eat", true );
}

void ioTargetTrace11Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		OnProcessStartState();
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case DCS_NONE_DELAY:
		ProcessNoneDelayState( fTimerPerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimerPerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	case DCS_CONTROL_ATTACK:
		ProcessChargeOwnerControl( fTimerPerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimerPerSec );
		break;
	case DCS_TRACE:
		OnProcessTraceState( fTimerPerSec, dwPreTime );
		break;
	case DCS_TRACE_TARGET_ATTACK:
		ProcessTraceTargetAttack( fTimerPerSec, dwPreTime );
		break;
	case DCS_LOOP_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessLoopAttackState( fTimerPerSec, dwPreTime );
		break;
	case DCS_GROUND_PIERCING_ATTACK:		
		ProcessPiercingState( fTimerPerSec );		
		break;
	}


	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimerPerSec );
	ProcessEffectUpdate( fTimerPerSec );
	ProcessAreaWeapon( fTimerPerSec );
}

void ioTargetTrace11Dummy::OnProcessStartState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );

		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
			return;

		if ( !m_AreaWeaponName.IsEmpty() )
		{
			ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(),
				"",
				"",
				m_AreaWeaponName,
				GetWorldPosition(),
				GetWorldOrientation(),
				ioAreaWeapon::CT_NORMAL );

			if ( pArea )
			{
				m_iAreaWeaponIndex = pArea->GetAreaWeaponIndex();
				m_vAreaWeaponPos = pArea->GetStartPos();
			}
		}
	}
}

void ioTargetTrace11Dummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;

		if( m_nCanEatCnt && m_nCanEatCnt <= (int)m_vTargetedChar.size() )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner && pOwner->IsNeedProcess() )
			{
				if( m_bDestroyAfterEat )
					SendDieState( true );
				return;
			}

			SetDelayState( false );
			return;
		}

		if ( !m_bFindTargetAttack )
			SetTraceState( DCTS_SENSING );
		else
			SetTraceState( DCTS_TARGET_JUMP );
	}
}

void ioTargetTrace11Dummy::OnProcessTraceState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		OnProcessTraceSensing( fTimePerSec );
		break;
	case DCTS_TRACING:
		OnProcessTraceTracing( fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		OnProcessTraceAttack( fTimePerSec, dwPreTime );
		break;
	case DCTS_TRACING_WAIT:
		ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		break;
	case DCTS_TARGET_JUMP:
		OnProcessTraceJumpAttack( pStage, fTimePerSec );
		break;
	}

	CheckTraceAttack();

	if( m_nCanEatCnt && m_nCanEatCnt <= (int)m_vTargetedChar.size() )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->IsNeedProcess() )
		{
			if( m_bDestroyAfterEat )
				SendDieState( true );
		}

		SetDelayState( false );
	}
}

void ioTargetTrace11Dummy::OnProcessTraceSensing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	ioHashString szTarget;
	szTarget = GetSearchTarget( pStage, false );

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTracingState( pStage, true );
		}
	}
}

void ioTargetTrace11Dummy::OnProcessTraceTracing( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTraceTarget( pStage );
		FindTraceTarget( pStage );

		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );		
		if( !IsTargetWoundedState( pTarget ) )
		{
			SetTraceWaitState();
			return;
		}

		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				D3DXVECTOR3 vTargetDir = vDiff;
				vTargetDir.y = 0;
				D3DXVec3Normalize( &vTargetDir, &vTargetDir );

				if( m_dwRotateSpeed > 0 )
				{
					D3DXQUATERNION	qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -vTargetDir );

					float fAngle = D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir );

					float fAngleGap = RADtoDEG( acosf(fAngle) );
					float fRate = (fTimePerSec*1000) / m_dwRotateSpeed;
					float fRotateAngle = 360.0f * fRate;

					if( fAngleGap < fRotateAngle )
					{
						m_vTracingMoveDir = vTargetDir;
					}
					else
					{
						D3DXQUATERNION qtRotate;
						ioMath::CalcDirectionQuaternion( qtRotate, -m_vTracingMoveDir );

						bool bLeftRot = false;
						if( D3DXVec3Dot( &vTargetDir, &m_vTracingMoveDir ) > -FLOAT1 )
						{
							float fCurYaw, fTargetYaw;
							fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );
							fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRot ) );

							fCurYaw = ioMath::ArrangeHead( fCurYaw );
							fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

							float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
							if( fRotAngle < 0.0f )
								bLeftRot = true;
						}

						if( bLeftRot )
							fRotateAngle *= -1;

						fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

						D3DXQUATERNION	qtNewRot;
						D3DXQuaternionRotationYawPitchRoll( &qtNewRot, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );
						m_vTracingMoveDir = qtNewRot * m_vTracingMoveDir;
					}
				}
				else
				{
					m_vTracingMoveDir = vTargetDir;
				}
			}
		}
		else
		{
			m_TargetTraceName.Clear();
		}
	}
	else
	{
		SetTraceState( DCTS_SENSING );
		return;
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
}

void ioTargetTrace11Dummy::OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

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
				ioWeapon* pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
				ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
				if( pZone )
				{
					pZone->SetCollisionDir( vCurDir );
					pZone->SetZoneValue( true, false );
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		m_LastAttacker = m_OwnerName;
		SetDelayState( false );
		return;
	}
}

void ioTargetTrace11Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_iAreaWeaponIndex != -1 )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_iAreaWeaponIndex );
		if ( pAreaWeapon && pAreaWeapon->IsLive() )
			pAreaWeapon->DestroyAreaWeapon( pStage );
		m_iAreaWeaponIndex = -1;
	}
	
	
	
	m_vAreaWeaponPos = ioMath::VEC3_ZERO;

	pStage->RemoveSummonDummyInfo( m_iCreateIndex, false );

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

ioHashString ioTargetTrace11Dummy::GetSearchTarget( ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return szTarget;
	if( !pStage )
		return szTarget;
	if( m_nCanEatCnt <= (int)m_vTargetedChar.size() )
		return szTarget;

	float fSensingRangeSq = 0.0f;
	if( bSecond )
		fSensingRangeSq = m_fSensingRange2 * m_fSensingRange2;
	else
		fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	if( fSensingRangeSq == 0.0f )
		return szTarget;

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		if( pChar->IsCanNoWound() )
			continue;

		if( m_bCheckWounedState && !IsTargetWoundedState( pChar ) )
			continue;

		bool bPass = false;

		auto SkipBuffIter = m_vSkipBuffList.begin();
		for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
		{
			ioHashString& szBuffName = *SkipBuffIter;
			if ( pChar->HasBuff( szBuffName ) )
			{				
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		bPass = false;
		for( size_t i = 0; i < m_vTargetedChar.size(); i++ )
		{
			const ioHashString& stTargetedName = m_vTargetedChar[i];
			if( pChar->GetCharName() == stTargetedName )
			{
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fSensingRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pChar->GetCharName();
			}
		}
	}

	return szTarget;
}

void ioTargetTrace11Dummy::TranslateByConveyer( const D3DXVECTOR3 &vMove )
{

}

void ioTargetTrace11Dummy::OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pStage )
		return;
	m_fSensingRange = JUMP_RANGE * (float)m_fJumpAttackRate;
	ioHashString szTargetName = GetSearchTarget( pStage, false );
	FindAttackTheTarget( szTargetName );
	SetJumpAttackState();
}

void ioTargetTrace11Dummy::ProcessAreaWeapon( float fTimePerSec )
{
	if ( !m_bAttachAreaWeapon )
		return;

	if ( m_AreaWeaponName.IsEmpty() || m_iAreaWeaponIndex == -1 )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_vAreaWeaponPos != GetWorldPosition() )
	{
		m_vAreaWeaponPos = GetWorldPosition();
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_iAreaWeaponIndex );
		if ( pAreaWeapon && pAreaWeapon->IsLive() )
		{
			pAreaWeapon->SetAreaPosition( m_vAreaWeaponPos, pStage );
		}
	}
}
