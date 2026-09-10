#include "StdAfx.h"
#include "ioTargetTrace15Dummy.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"


ioTargetTrace15Dummy::ioTargetTrace15Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode ) :
ioDummyChar( pGrp, pMode )
{
	m_TargetTraceState = TS_START;
	m_dwFireStartTime = 0;
	m_fCurTraceSpeed = 0.0f;
	m_iCurAttackCount = 0;
	m_dwFirstSensingMoveEndTime = 0;
}


ioTargetTrace15Dummy::~ioTargetTrace15Dummy(void)
{
}

void ioTargetTrace15Dummy::InitDummyCharInfo( int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, float fStartAngle/*=0.0f*/, bool bNetWork /*= false */ )
{
	ioDummyChar::InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle, bNetWork );

	if ( !m_StartAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( m_StartAnimation );
		m_dwFireStartTime = m_pGroup->GetAnimationEventTime_e( iAniID, "fire_s" );
		if ( m_dwFireStartTime != 0 )
			m_dwFireStartTime += FRAMEGETTIME();
	}
}

void ioTargetTrace15Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	
	//Start Ani Attack
	m_StartWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "start_ani_attack_type", NO_WEAPON_ATTRIBUTE );
	m_StartWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( "start_ani_attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "start_ani_attack_type_wounded", "", szBuf, MAX_PATH );
	m_StartWeaponInfo.m_WoundedAnimation = szBuf;
	m_StartWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( "start_ani_attack_type_wounded_time", 0 );
	m_StartWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( "start_ani_attack_type_wounded_loop_ani", false );

	m_vAttackCode.clear();
	for( int i=0 ; i<100 ; i++ )
	{
		wsprintf_e( szBuf, "dummy_trace_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );
		if ( iAttackCode == 0 )
			break;
		m_vAttackCode.push_back( iAttackCode );
	}

	m_fFirstSensingMoveSpeed = rkLoader.LoadFloat_e( "first_sensing_move_speed", 0.0f );
	m_dwFirstSensingMoveDuration = (DWORD)rkLoader.LoadInt_e( "first_sensing_move_duration", 0 );

	m_bNoDropZoneMove = rkLoader.LoadBool_e( "no_drop_zone_move", false );

}

void ioTargetTrace15Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	switch( m_TargetTraceState )
	{
	case TS_START:
		ProcessStartState( pOwner );
		break;
	case TS_DELAY:
		ProcessDelayState( pOwner, fTimePerSec );
		break;
	case TS_SENSING:
		ProcessTraceSensing( pOwner, pStage, fTimePerSec );
		break;
	case TS_TARGET_TRACE:
		ProcessTraceTracing( pOwner, pStage, fTimePerSec );
		break;
	case TS_TARGET_TRACE_WAIT:
		ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		break;
	case TS_TARGET_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessTraceAttack( pOwner, fTimePerSec, dwPreTime );
		break;
	}

	float fMoveDist = m_fCurTraceSpeed * fTimePerSec;
	ProcessTraceMoving( pOwner, pStage, m_vTracingMoveDir, fMoveDist, m_bNoDropZoneMove );
	CheckTargetTraceAttack( pOwner, pStage );
}

void ioTargetTrace15Dummy::ProcessStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_StartWeaponInfo.m_iWeaponIdx != NO_WEAPON_ATTRIBUTE && 
		m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		m_dwFireStartTime = 0;

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = m_StartWeaponInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = m_StartWeaponInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = m_StartWeaponInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = m_StartWeaponInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = m_StartWeaponInfo.m_bWoundedLoopAni;
		kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase();
		kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
		kFireTime.m_iExtraValue = m_iCreateIndex;

		D3DXVECTOR3 vCurPos = GetMidPositionByRate();
		D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		ioWeapon* pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
		ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
		if( pZone )
		{
			pZone->SetCollisionDir( vCurDir );
			pZone->SetZoneValue( true, false );
		}
	}

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetSensingState( m_fFirstSensingMoveSpeed );
		return;
	}
}

void ioTargetTrace15Dummy::SetDelayState( bool bNet )
{
	m_TargetTraceState = TS_DELAY;
	m_iReserveAttackCode = 0;

	m_dwDelayStartTime = FRAMEGETTIME();

	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;

	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();

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

void ioTargetTrace15Dummy::ProcessDelayState( ioBaseChar* pOwner, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime+m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;

		if( pOwner->IsNeedProcess() )
		{
			if( m_iCurAttackCount >= (int)m_vAttackCode.size() )
			{
				SendDieState( true );
				return;
			}
		}

		SetSensingState( 0 );
	}
}

void ioTargetTrace15Dummy::SetSensingState( float fMoveSpeed )
{
	m_TargetTraceState = TS_SENSING;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwSensingStartTime = dwCurTime;
	m_dwFirstSensingMoveEndTime = 0;
	if ( fMoveSpeed != 0 )
	{
		m_fCurTraceSpeed = fMoveSpeed;
		int iAniID = m_pGroup->GetAnimationIdx( m_TracingAni );
		if ( iAniID == -1 )
			return;

		if ( m_dwFirstSensingMoveDuration != 0 )
			m_dwFirstSensingMoveEndTime = dwCurTime + m_dwFirstSensingMoveDuration;
		D3DXQUATERNION qtRot = GetWorldOrientation();
		m_vTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		m_pGroup->ClearAllLoopAni( FLOAT100 );
		m_pGroup->ClearAllActionAni( FLOAT100 );

		float fRate = FLOAT1;
		if( m_fTracingAniRate > 0.0f )
			fRate = m_fTracingAniRate;

		if( iAniID != -1 )
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );
	}
	
}

void ioTargetTrace15Dummy::ProcessTraceSensing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pOwner || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_TargetTraceName.IsEmpty() )
		m_TargetTraceName.Clear();

	ioHashString szTarget;
	szTarget = GetSearchTarget( pOwner, pStage, false );

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetTraceName = szTarget;
			SetTargetTracingState( pStage, true );
		}
	}
	else if ( m_dwFirstSensingMoveEndTime > 0 && m_dwFirstSensingMoveEndTime < dwCurTime )
	{
		m_dwFirstSensingMoveEndTime = 0;
		m_fCurTraceSpeed = 0.0f;
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
}

ioHashString ioTargetTrace15Dummy::GetSearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;
	szTarget.Clear();

	if ( !pOwner || !pStage )
		return szTarget;

	float fSensingRangeSq = 0.0f;
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

		if( !pChar ) 
			continue;
		if( pChar->GetTeam() == pOwner->GetTeam() ) 
			continue;
		if( pChar->GetCharName() == GetOwnerName() ) 
			continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) 
			continue;
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

void ioTargetTrace15Dummy::SetTargetTracingState( ioPlayStage *pStage, bool bSendNetwork )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	m_TargetTraceState = TS_TARGET_TRACE;
	DWORD dwCurTime = FRAMEGETTIME();

	m_dwTracingEndTime = dwCurTime + m_dwTraceDuration;

	m_fCurTraceSpeed = m_fTraceSpeed;
	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket << m_TargetTraceName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_TracingAni );
	float fRate = FLOAT1;
	if( m_fTracingAniRate > 0.0f )
		fRate = m_fTracingAniRate;

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

	SetAttachWeapon();
}

void ioTargetTrace15Dummy::ProcessTraceTracing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pOwner || !pStage )
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
			SetTargetTraceWaitState();
			m_fCurTraceSpeed = 0.0f;
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
		SetSensingState( 0 );
		return;
	}
}

void ioTargetTrace15Dummy::SetTargetTraceWaitState()
{
	if( m_TargetTraceState == TS_TARGET_TRACE_WAIT )
		return;

	m_TargetTraceState = TS_TARGET_TRACE_WAIT;
	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	int iAniID = m_pGroup->GetAnimationIdx( m_DelayAnimation );

	if( iAniID != -1 )
		m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 );
}

void ioTargetTrace15Dummy::ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt, bool bNoDropZoneMove )
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

	if ( fMapHeight <= 0.0f && bNoDropZoneMove )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
		SetWorldOrientation( qtRot );
		return;
	}

	if( m_fEnableTraceMoveHeight < fabs(fGapHeight) )
	{
		if( fGapHeight <= 0.0f )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vMoveDir );
			SetWorldOrientation( qtRot );

			if( m_bCharColOnlyAttack )
			{
				m_LastAttacker = m_OwnerName;
				SendDieState( true );
			}
			else
			{
				SetTraceAttack( pOwner );
			}

			return;
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

void ioTargetTrace15Dummy::SetTraceAttack( ioBaseChar *pOwner )
{
	m_fCurTraceSpeed = 0.0f;
	if ( m_iCurAttackCount > (int)m_vAttackCode.size() )
		return;

	m_TargetTraceState = TS_TARGET_ATTACK;


	m_iReserveAttackCode = m_vAttackCode[m_iCurAttackCount];

	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

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

	m_iCurAttackCount++;
}

void ioTargetTrace15Dummy::ProcessTraceAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
{
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

void ioTargetTrace15Dummy::CheckTargetTraceAttack( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	if( m_TargetTraceName.IsEmpty() )
		return;

	if( m_TargetTraceState != TS_TARGET_TRACE )
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
		{
			bEnableAngle = true;
		}

		float fRangeSq = D3DXVec3LengthSq( & vDiff );
		if( bEnableAngle && fRangeSq < m_fTraceAttackRange * m_fTraceAttackRange )
		{
			SetTraceAttack( pOwner );
		}
	}
}

void ioTargetTrace15Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DUMMY_CHAR_TRACE:
		{
			rkPacket >> m_TargetTraceName;
			SetTargetTracingState( pStage, false );
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioTargetTrace15Dummy::DropZoneCheck()
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
}
