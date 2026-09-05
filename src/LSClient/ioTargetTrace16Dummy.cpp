#include "StdAfx.h"
#include "ioTargetTrace16Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTrace16Dummy::ioTargetTrace16Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_TraceDummyState = TDS_START;
	m_dwJumpStartTime = 0;
	m_iCurAttackCount = 0;
	m_vWeaponIndexList.clear();
	m_dwReturnEndTime = 0;
}

ioTargetTrace16Dummy::~ioTargetTrace16Dummy(void)
{
}

void ioTargetTrace16Dummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	ioDummyChar::LoadProperty( rkLoader );

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );

	m_iLandingAttackCode = rkLoader.LoadInt_e( "landing_attack", 0 );

	rkLoader.LoadString_e( "jumpping_animation", "", szBuf, MAX_PATH );
	m_szJumppingAnimation = szBuf;
	m_fJumppingAniRate = rkLoader.LoadFloat_e( "jumpping_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "return_animation", "", szBuf, MAX_PATH );
	m_szReturnAnimation = szBuf;
	m_fReturnAniRate = rkLoader.LoadFloat_e( "return_ani_rate", FLOAT1 );
	m_fReturnMoveSpeed = rkLoader.LoadFloat_e( "return_move_speed", 0.f );
	m_dwReturnDuration = (DWORD)rkLoader.LoadInt_e( "return_duration", 0 );
	m_fReturnOwnerGap  = rkLoader.LoadFloat_e( "return_owner_gap", 0.0f );
}

void ioTargetTrace16Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SendDieState( true );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if ( pOwner->IsNeedProcess() && m_dwLifeTime && m_dwCreateTime + m_dwLifeTime < dwCurTime )
	{
		if ( m_TraceDummyState != TDS_ATTACK && m_TraceDummyState != TDS_LANDING_ATTACK &&
			m_TraceDummyState != TDS_JUMP )
		{
			SendDieState( true );
			return;
		}
	}

	switch( m_TraceDummyState )
	{
	case TDS_START:
		ProcessStartState( fTimerPerSec );
		break;
	case TDS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case TDS_SENSING:
		ProcessSensingState( pOwner );
		break;
	case TDS_TRACING:
		ProcessTracingState( pOwner, fTimerPerSec );
		CheckTraceAttack( pOwner );
		break;
	case TDS_TRACE_WAIT:
		ProcessTraceTracingWait( fTimerPerSec, dwPreTime );
		break;
	case TDS_ATTACK:
	case TDS_LANDING_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( pOwner, fTimerPerSec, dwPreTime );
		break;
	case TDS_JUMP:
		ProcessJumpState( pOwner );
		break;
	case TDS_RETURN:
		ProcessReturnState( pOwner );
		break;
	}

	ProcessPos( fTimerPerSec );
}

void ioTargetTrace16Dummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;

		if( m_nCanEatCnt && m_nCanEatCnt <= m_iCurAttackCount )
		{
			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;
			if( pOwner->IsNeedProcess() )
			{
				SetReturnState( pOwner );
				return;
			}
		}

		SetSensingState();
	}
}

void ioTargetTrace16Dummy::SetDelayState( bool bNet )
{
	m_TraceDummyState = TDS_DELAY;
	m_iReserveAttackCode = 0;

	m_dwDelayStartTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bSetUpKey = false;
	m_bSetDownKey = false;

	CheckChargeWeaponDead();
	ClearWeaponIndexList();
	if( !m_DelayAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );
		if( iAniID != -1 )
		{
			float fAniRate = FLOAT1;
			if( m_fDelayAniRate > 0.0f )
				fAniRate = m_fDelayAniRate;

			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );

			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
		}
	}

	ioBaseChar *pOwner = GetOwner();
	if( bNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::SetSensingState()
{
	m_TraceDummyState = TDS_SENSING;

	m_dwSensingStartTime = FRAMEGETTIME();

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_SensingAni );
	if( 0 <= iAniID )
	{
		float fRate = FLOAT1;
		if( m_fSensingAniRate > 0.0f )
			fRate = m_fSensingAniRate;

		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
	}
}

void ioTargetTrace16Dummy::ProcessSensingState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		return;

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	ioHashString szTarget;
	szTarget = GetSearchTarget( pStage, false );

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pOwner->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTracingState( pOwner );
		}
	}
}

ioHashString ioTargetTrace16Dummy::GetSearchTarget( ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return szTarget;
	if( !pStage )
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

void ioTargetTrace16Dummy::SetTracingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	m_TraceDummyState = TDS_TRACING;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwTracingEndTime = dwCurTime + m_dwTraceDuration;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_TracingAni );
	float fRate = FLOAT1;
	if( m_fTracingAniRate > 0.0f )
		fRate = m_fTracingAniRate;

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_TRACING;
		kPacket << m_TargetTraceName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::ProcessTracingState( ioBaseChar *pOwner, float fTimerPerSec )
{
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
			SetTraceWaitState( pOwner );
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
					float fRate = (fTimerPerSec*1000) / m_dwRotateSpeed;
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
		SetSensingState();
		return;
	}

	float fMoveDist = m_fTraceSpeed * fTimerPerSec;
	ProcessTraceMoving( pOwner, pStage, m_vTracingMoveDir, fMoveDist );
}

void ioTargetTrace16Dummy::CheckTraceAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_TargetTraceName.IsEmpty() )
		return;

	if( m_TraceDummyState != TDS_TRACING )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;

		// 각도 체크
		bool bEnableAngle = false;
		float fBoomHalfAnigle = m_fTraceAttackAngle * FLOAT05;
		fBoomHalfAnigle = ioMath::ArrangeHead(fBoomHalfAnigle);
		float fHalfValue = cosf( DEGtoRAD(fBoomHalfAnigle) );

		if( m_fTraceAttackAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2;
			D3DXVec3Normalize( &vDiff2, &vDiff );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fHalfValue <= fCurValue )
				bEnableAngle = true;
		}
		else
			bEnableAngle = true;

		float fRangeSq = D3DXVec3LengthSq( & vDiff );
		if( bEnableAngle && fRangeSq < m_fTraceAttackRange * m_fTraceAttackRange )
			SetTracingAttack( pOwner );
	}
}

void ioTargetTrace16Dummy::SetTracingAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_TraceDummyState = TDS_ATTACK;
	m_iCurAttackCount++;
	SetAttackAni( pOwner, m_iTraceNormalAttack );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_TRACING_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::ProcessAttackState(ioBaseChar *pOwner, float fTimerPerSec, DWORD dwPreTime )
{
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

	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
		{
			m_LastAttacker = m_OwnerName;
			SetDelayState( true );
			return;
		}
	}
}

void ioTargetTrace16Dummy::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_TraceDummyState != TDS_ATTACK )
		return;

	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( pTarget && pTarget->GetTeam() != GetTeam() )
	{
		SetJumpState( );
		return;
	}
}

void ioTargetTrace16Dummy::SetJumpState()
{
	m_TraceDummyState = TDS_JUMP;
	m_fCurJumpPowerAmt = m_fJumpPower;
	m_dwJumpStartTime = FRAMEGETTIME();

	int iAniID = m_pGroup->GetAnimationIdx( m_szJumppingAnimation );
	if( iAniID != -1 )
	{
		float fAniRate = FLOAT1;
		if( m_fJumppingAniRate > 0.0f )
			fAniRate = m_fJumppingAniRate;

		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	}
	
	if( GetOwner() && GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_JUMP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::ProcessJumpState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwJumpStartTime + 100 < FRAMEGETTIME() && CheckLanding( pOwner ) )
	{
		SetLandingAttack( pOwner );
		return;
	}
	
}

bool ioTargetTrace16Dummy::CheckLanding( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return false;
	D3DXVECTOR3 vPos = GetWorldPosition();
	float fHeightGap = vPos.y -pStage->GetMapHeight( vPos.x,vPos.z );
	if( fHeightGap > 0.0f )
		return false;
	return true;
}

void ioTargetTrace16Dummy::SetLandingAttack( ioBaseChar *pOwner )
{
	m_TraceDummyState = TDS_LANDING_ATTACK;
	m_fCurJumpPowerAmt = 0.0f;
	SetAttackAni( pOwner, m_iLandingAttackCode );
	ClearWeaponIndexList();

	if( GetOwner() && GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_LAND_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::SetAttackAni( ioBaseChar *pOwner, int iAttackCode )
{
	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;
	m_iReserveAttackCode = iAttackCode;
	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwAttackAniEndTime = dwCurTime;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 처리
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return;

		m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
		DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;

		int i=0;
		AniEventConstPtrList::iterator iter;

		std::list< float >::iterator iterEndTime = vFireEndList.begin();
		for( iter=vFireStartList.begin() ; iter!=vFireStartList.end() ; ++iter, i++ )
		{
			float fFireStart = (*iter)->fEventTime;

			FireTime kFireTime;
			kFireTime.dwStart = dwStartTime + fFireStart * fAniRate;

			if( iterEndTime != vFireEndList.end() )
			{
				kFireTime.dwDuration = ( *iterEndTime - fFireStart ) * fAniRate;
				++iterEndTime;
			}

			kFireTime.iAttributeIdx  = vInfoList[i].m_iWeaponIdx;
			kFireTime.iResistanceIdx = vInfoList[i].m_iResistanceIdx;
			kFireTime.szWoundedAni   = vInfoList[i].m_WoundedAnimation;
			kFireTime.dwWoundedAniDuration  = vInfoList[i].m_dwWoundedDuration;
			kFireTime.bLoopWoundedAni = vInfoList[i].m_bWoundedLoopAni;

			kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex + i;
			m_vWeaponIndexList.push_back( kFireTime.dwWeaponIdx );

			if( !(*iter)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter)->szSubInfo.c_str(), " " );
				if( vParam.size() >= 1 )
				{
					ioHashString szTypeName = vParam[0].c_str();
					kFireTime.eCollisionType = GetAniCollisionType( szTypeName );

					if( vParam.size() == 2 && kFireTime.eCollisionType == ACT_SPECIAL )
					{
						kFireTime.szSpecialCollisionBox = vParam[1].c_str();
					}
				}
			}

			kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
			kFireTime.m_iExtraValue = m_iCreateIndex;

			m_vAttackFireTimeList.push_back( kFireTime );
		}

		SetReservedSliding( rkCurAttribute.m_vForceInfoList, iAniID, fAniRate, rkCurAttribute.m_dwPreDelay );
		// 기타 처리

		m_dwExtraAniJumpTime = 0;
		m_fExtraAniJumpPower = 0.0f;
		m_vExtraAniJumpList.clear();

		AniEventConstPtrList vExtraAniJumpList;
		m_pGroup->GetAnimationEventConstPtrList( iAniID, "extra_ani_jump", vExtraAniJumpList );

		AniEventConstPtrList::iterator iter_event = vExtraAniJumpList.begin();
		while( iter_event != vExtraAniJumpList.end() )
		{
			ExtraAniJump kInfo;

			kInfo.m_dwTime = (*iter_event)->fEventTime * fAniRate;
			kInfo.m_dwTime += dwCurTime;

			if( !(*iter_event)->szSubInfo.IsEmpty() )
			{
				const StringVector &vParam = ioStringConverter::Split( (*iter_event)->szSubInfo.c_str(), " " );
				if( vParam.size() == 1 )
				{
					kInfo.m_fPower = ioStringConverter::ParseFloat( vParam[0].c_str() );
				}
			}

			m_vExtraAniJumpList.push_back(kInfo);

			++iter_event;
		}

		if( !m_vExtraAniJumpList.empty() )
		{
			m_iCurExtraAniJump = 0;
			m_dwExtraAniJumpTime = m_vExtraAniJumpList[m_iCurExtraAniJump].m_dwTime;
			m_fExtraAniJumpPower = m_vExtraAniJumpList[m_iCurExtraAniJump].m_fPower;
		}
	}
}

void ioTargetTrace16Dummy::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_TraceDummyState == TDS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	switch( m_TraceDummyState )
	{
	case TDS_RETURN:
		{
			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;

			D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition();
			D3DXVECTOR3 vDir = vNewPos - GetWorldPosition();
			float fGap = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			if( fGap <= m_fReturnOwnerGap && pOwner->IsNeedProcess() )
			{
				SendDieState( true );
				return;
			}

			D3DXVECTOR3 vMove;
			float fMoveSpeed = m_fReturnMoveSpeed * fTimePerSec;
			vMove = fMoveSpeed * vDir;
			Translate( vMove );
		}
		break;
	default:
		ProcessGravityMove( fTimePerSec );
		break;
	}
	

	m_dwProcessTime = dwCurTime;
}

void ioTargetTrace16Dummy::ProcessGravityMove( float fTimePerSec )
{
	if( !m_bNoGravityMove )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vCurPos = vPrePos;
		float fMapHeight = GetBottomHeight();

		DWORD dwGapTime = FRAMEGETTIME() - m_dwProcessTime;

		do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( m_fCurJumpPowerAmt > 0.0f )
			{
				vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
			}

			if( vCurPos.y > fMapHeight )
			{
				float fCurRate = m_fGravityRate;
				if( fCurRate <= 0.0f )
					fCurRate = FLOAT1;

				m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate )* fNewTimePerSec;
				vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

				if( vCurPos.y <= fMapHeight )
				{
					m_fGravityAmt = 0.0f;
					m_fCurJumpPowerAmt = 0.0f;
				}
			}
		} while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			m_fCurJumpPowerAmt = 0.0f;
		}

		SetWorldPosition( vCurPos );
	}
}

void ioTargetTrace16Dummy::ClearWeaponIndexList()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORDVec::iterator iter = m_vWeaponIndexList.begin();
	while( iter != m_vWeaponIndexList.end() )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, (*iter) );
		if( pWeapon )
		{
			pWeapon->SetWeaponDead();
		}

		iter = m_vWeaponIndexList.erase( iter );
	}
	m_vWeaponIndexList.clear();
}

void ioTargetTrace16Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DCST_TRACING:
		{
			rkPacket >> m_TargetTraceName;
			SetTracingState( pOwner );
		}
		break;
	case DCST_TRACING_ATTACK:
		SetTracingAttack( pOwner );
		break;
	case DCST_JUMP:
		SetJumpState();
		break;
	case DCST_LAND_ATTACK:
		SetLandingAttack( pOwner );
		break;
	case DCST_RETURN:
		SetReturnState( pOwner );
		break;
	default:
		ApplyDummyCharSyncByType( iType, rkPacket, pStage );
		break;
	}
	
}

void ioTargetTrace16Dummy::SetTraceWaitState( ioBaseChar *pOwner )
{
	if( m_TraceDummyState == TDS_TRACE_WAIT )
		return;

	m_TraceDummyState = TDS_TRACE_WAIT;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 );
}

void ioTargetTrace16Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ClearWeaponIndexList();
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioTargetTrace16Dummy::ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt )
{
	if ( !pOwner || !pStage )
		return;

	if ( fMoveAmt == 0.0f )
		return;

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	vPos += vMoveDir * fMoveAmt;

	float fTopHeight = m_fEnableTraceMoveHeight * 4.0f;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+fTopHeight );
	float fGapHeight = fMapHeight - vPos.y;

	if( m_fEnableTraceMoveHeight < fabs(fGapHeight) )
	{
		if ( fMapHeight <= 0.0f )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -m_vTracingMoveDir );
			SetWorldOrientation( qtRot );

			if( m_bCharColOnlyAttack )
			{
				m_LastAttacker = m_OwnerName;
				SendDieState( true );
			}
			else
			{
				SetTracingAttack( pOwner );
			}
		}
		else
		{
			float fNewMapHeight = 0.0f;
			D3DXVECTOR3 vDiff = vPrePos;

			if( fabs(vMoveDir.x) >= fabs(vMoveDir.z) )
			{
				if( vMoveDir.x >= 0.0f )
					vDiff += D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
			}
			else
			{
				if( vMoveDir.z >= 0.0f )
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * fMoveAmt;
			}

			fNewMapHeight = pStage->GetMapHeightNoneEntity( vDiff.x, vDiff.z, vDiff.y+fTopHeight );

			if( m_fEnableTraceMoveHeight < fabs(vDiff.y - fNewMapHeight) )
				return;

			vPos = vDiff;
			fMapHeight = fNewMapHeight;
		}
	}

	vPos.y = fMapHeight;
	SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
	SetWorldOrientation( qtRot );
}

void ioTargetTrace16Dummy::DropZoneCheck()
{
	if( m_dwDropZoneLiveTime > 0 )
	{
		bool bCheckDropZone = false;
		if( m_dwDropZoneCheckStartTime <= 0 )
			m_dwDropZoneCheckStartTime = FRAMEGETTIME();

		D3DXVECTOR3 vCurPos = GetWorldPosition();

		if( GetOwner() &&
			GetOwner()->GetCreator() &&
			GetOwner()->GetCreator()->GetMapHeightNoneEntity( vCurPos.x, vCurPos.z, vCurPos.y+3.0f ) <= 0.0f )
			bCheckDropZone = true;

		if( !bCheckDropZone )
			m_dwDropZoneCheckStartTime = 0;

		if( bCheckDropZone && m_dwDropZoneCheckStartTime > 0 && m_dwDropZoneCheckStartTime + m_dwDropZoneLiveTime <= FRAMEGETTIME() )
		{
			m_HP.m_fCurValue = 0.0f;
			m_LastAttacker = m_OwnerName;

			SetDieState( true );
		}
	}

	if ( m_TraceDummyState == TDS_ATTACK )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();

	if( m_HP.m_fCurValue > 0.0f && vPos.y <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		m_LastAttacker = m_OwnerName;

		SendDieState( true );
	}
}

void ioTargetTrace16Dummy::SetReturnState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_TraceDummyState = TDS_RETURN;
	m_dwReturnEndTime = FRAMEGETTIME() + m_dwReturnDuration;
	if ( !m_szReturnAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_szReturnAnimation );
		if( iAniID == -1 )
			return;
		float fAniRate = FLOAT1;
		if( m_fReturnAniRate > 0.0f )
			fAniRate = m_fReturnAniRate;

		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_RETURN;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace16Dummy::ProcessReturnState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_dwReturnEndTime && m_dwReturnEndTime < FRAMEGETTIME() )
	{
		m_dwReturnEndTime = 0;
		SendDieState( true );
		return;
	}
}
