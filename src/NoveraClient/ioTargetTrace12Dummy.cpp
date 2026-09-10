#include "StdAfx.h"
#include "ioTargetTrace12Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTrace12Dummy::ioTargetTrace12Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_dwNextFireTime = FRAMEGETTIME();
	m_dwCreateFireTime = FRAMEGETTIME();
	m_nAttackAttributeIndex = 0;
}


ioTargetTrace12Dummy::~ioTargetTrace12Dummy(void)
{
}

void ioTargetTrace12Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_fAddOffSetHeight = rkLoader.LoadFloat_e( "add_offset_height", FLOAT100 );

	//////////////////////////////////////////////////////////////////////////
	char szBuf[MAX_PATH] = "", szKey[MAX_PATH];
	int nMaxAttackAttributeCnt = rkLoader.LoadInt("step_max_attribute_cnt", 0 );
	for( int i = 0; i < nMaxAttackAttributeCnt; i++ )
	{
		WeaponInfo Info;
		wsprintf_e( szKey, "step%d_weapon_attribute_index", i+1 );
		Info.m_iWeaponIdx		= rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "step%d_weapon_attribute_resist_index", i+1 );
		Info.m_iResistanceIdx	= rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "step%d_weapon_wounded_animation", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info.m_WoundedAnimation	= szBuf;

		wsprintf_e( szKey, "step%d_weapon_wounded_duration", i+1 );
		Info.m_dwWoundedDuration	= rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "step%d_weapon_wounded_loop_ani", i+1 );
		Info.m_bWoundedLoopAni		= rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "step%d_weapon_duration", i+1 );
		Info.m_dwDuration		= (DWORD)rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "step%d_fire_gap", i+1 );
		Info.m_dwCallTime		= (DWORD)rkLoader.LoadInt( szKey, 0 );

		m_vAttackInfo.push_back( Info );
	}
	//////////////////////////////////////////////////////////////////////////
}


void ioTargetTrace12Dummy::InitDummyCharInfo(	int iIndex, DWORD dwCreateGapTime, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, float fStartAngle, bool bNetWork )
{
	__super::InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle, bNetWork );

	D3DXVECTOR3 vWorldPos = GetWorldPosition();
	vWorldPos.y += m_fAddOffSetHeight;

	SetWorldPosition( vWorldPos );
}


void ioTargetTrace12Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
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

	//ProcessPos( fTimerPerSec );
	ProcessEffectUpdate( fTimerPerSec );

	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_State != DCS_START )
	{
		DWORD dwCurtime = FRAMEGETTIME();
		if(  pOwner->IsNeedProcess() && dwCurtime > m_dwNextFireTime )
			SetNextAttack();

		/*if( dwCurtime > m_dwCreateFireTime )
		{
			CreateWeapon( pOwner );
			int nAttributeIndex = m_nAttackAttributeIndex - 1;
			nAttributeIndex = max( 0, nAttributeIndex );
			nAttributeIndex = min( nAttributeIndex, (int)m_vAttackInfo.size()-1 );
			DWORD dwCallTime = m_vAttackInfo[nAttributeIndex].m_dwCallTime;
			m_dwCreateFireTime += dwCallTime;
		}*/
	}

}


void ioTargetTrace12Dummy::CreateWeapon( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( (int)m_vAttackInfo.size() <= m_nAttackAttributeIndex )
		return;
	DWORD dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_vAttackInfo[m_nAttackAttributeIndex].m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_vAttackInfo[m_nAttackAttributeIndex].m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_vAttackInfo[m_nAttackAttributeIndex].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_vAttackInfo[m_nAttackAttributeIndex].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_vAttackInfo[m_nAttackAttributeIndex].m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	//D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	//D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vPos = GetMidPositionByRate();
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.f, -FLOAT1, 0.f );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
		pWeapon->SetPosition( vPos );
}


void ioTargetTrace12Dummy::OnProcessStartState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );

		ioBaseChar *pOwner = GetOwner();
		if ( !pOwner )
			return;
	}
}

void ioTargetTrace12Dummy::ProcessDelayState( float fTimePerSec )
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
				SendDieState( true );
				return;
			}
		}

		if ( !m_bFindTargetAttack )
		{
			SetTraceState( DCTS_SENSING );
		}
		else
			SetTraceState( DCTS_TARGET_JUMP );
	}
}

void ioTargetTrace12Dummy::OnProcessTraceState( float fTimePerSec, DWORD dwPreTime )
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

	//CheckTraceAttack();
}

void ioTargetTrace12Dummy::OnProcessTraceSensing( float fTimePerSec )
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


void ioTargetTrace12Dummy::CheckTarget( ioPlayStage *pStage )
{
	if( !pStage ) return;
	if( m_TargetTraceName.IsEmpty() ) return;

	ioHashString szTarget = m_TargetTraceName;
	bool bLost = false;
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		CharState eState = pTarget->GetState();
		switch( eState )
		{
		case CS_DIE:
		case CS_DROP_ZONE_DOWN:
		case CS_READY:
		case CS_VIEW:
		case CS_OBSERVER:
		case CS_LOADING:
		case CS_FISHING:
		case CS_STARTMOTION:
		case CS_ENDMOTION:
		case CS_WARP_STATE:
		case CS_EXCAVATING:
		case CS_ROULETTE:
			m_TargetTraceName.Clear();
			break;
		}

		if( !pTarget->IsNeedProcess() )
			return;

		if( !m_TargetTraceName.IsEmpty() && pTarget->HasHideBuff() )
			m_TargetTraceName.Clear();

		if( !m_TargetTraceName.IsEmpty() && pTarget->IsChatModeState( false ) )
			m_TargetTraceName.Clear();

		if( !m_TargetTraceName.IsEmpty() )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			vPrePos.y = 0.f;

			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate();
			vDiff.y = 0.f;
			vDiff = vDiff - vPrePos;

			//float fRangeSq = D3DXVec3Length( & vDiff );
			float fRange = D3DXVec3Length( & vDiff );
			if( fRange >= m_fTraceRange )
				m_TargetTraceName.Clear();
		}
	}
	else
	{
		m_TargetTraceName.Clear();
	}

	if( m_TargetTraceName.IsEmpty() )
		bLost = true;

	if( bLost )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
			kPacket << m_OwnerName;
			kPacket << m_iCreateIndex;
			kPacket << DUMMY_CHAR_TRACE;
			kPacket << m_TargetTraceName;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetTrace12Dummy::OnProcessTraceTracing( float fTimePerSec )
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
	ioBaseChar *pTarget = NULL;
	if( m_dwTracingEndTime > dwCurTime )
	{
		CheckTarget( pStage );
		FindTraceTarget( pStage );

		pTarget = pStage->GetBaseChar( m_TargetTraceName );		
		if( !IsTargetWoundedState( pTarget ) )
		{
			SetTraceWaitState();
			return;
		}

		if( pTarget )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			vPrePos.y = 0;
			D3DXVECTOR3 vDiff;
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y = 0.f;

			vDiff = vTargetPos - vPrePos;

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
	//ProcessTraceMoving( pStage, fMoveDist, fTimePerSec );
	if( pTarget )
	{
		float fLength = 0.f;
		D3DXVECTOR3 vSrc = GetWorldPosition();
		D3DXVECTOR3 vDest = pTarget->GetWorldPosition();
		
		vSrc.y = 0.f;
		vDest.y = 0.f;

		D3DXVECTOR3 vDiffLength = vDest - vSrc;
		fLength = D3DXVec3Length( &vDiffLength );
		if( fLength < fMoveDist )
		{
			vDest.y = GetWorldPosition().y;
			SetWorldPosition( vDest );
		}
		else
			ProcessTraceMovingNoneTerrain( pStage, fMoveDist, fTimePerSec );
	}
	
}

void ioTargetTrace12Dummy::OnProcessTraceAttack( float fTimePerSec, DWORD dwPreTime )
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

void ioTargetTrace12Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pStage->RemoveSummonDummyInfo( m_iCreateIndex, false );

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

ioHashString ioTargetTrace12Dummy::GetSearchTarget( ioPlayStage *pStage, bool bSecond )
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
	vCurPos.y = 0.f;

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
		vTargetPos.y = 0.f;

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


void ioTargetTrace12Dummy::OnProcessTraceJumpAttack( ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pStage )
		return;
	m_fSensingRange = JUMP_RANGE * (float)m_fJumpAttackRate;
	ioHashString szTargetName = GetSearchTarget( pStage, false );
	FindAttackTheTarget( szTargetName );
	SetJumpAttackState();
}



void ioTargetTrace12Dummy::SetNextAttack()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	if( (int)m_vAttackInfo.size() <= m_nAttackAttributeIndex )
		return;

	CheckChargeWeaponDead();

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	//
	FireTime kFireTime;
	kFireTime.dwStart = dwCurTime;
	kFireTime.iAttributeIdx  = m_vAttackInfo[m_nAttackAttributeIndex].m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_vAttackInfo[m_nAttackAttributeIndex].m_iResistanceIdx;
	kFireTime.szWoundedAni   = m_vAttackInfo[m_nAttackAttributeIndex].m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration  = m_vAttackInfo[m_nAttackAttributeIndex].m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_vAttackInfo[m_nAttackAttributeIndex].m_bWoundedLoopAni;

	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;

	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;
	kFireTime.m_iExtraValue = m_iCreateIndex;

	m_iChargeWeaponIndex = 0;

	m_dwNextFireTime = dwCurTime + m_vAttackInfo[m_nAttackAttributeIndex].m_dwDuration;
	if( kFireTime.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
	{
		ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vCurPos, vCurDir, "" );
		if( pWeapon )
		{
			m_iChargeWeaponIndex = m_dwBaseWeaponIndex;

	//		// 처음 한 번은 여기서 위치를 조정해 줘야한다.
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
					pZone->SetLiveTime( m_vAttackInfo[m_nAttackAttributeIndex].m_dwDuration );
				}
			}
		}
	}

	m_AttackSoundID = PlaySound( pOwner, m_szControlAttackSound, m_bSoundLoop );
	//SendPack
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket	<< (int)DUMMY_CHAR_STRIDER_SYNC;
		kPacket << m_nAttackAttributeIndex;
		kPacket << GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//
	m_nAttackAttributeIndex++;
}


void ioTargetTrace12Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;	

	int eType;
	rkPacket >> eType;

	if( eType == DUMMY_CHAR_STRIDER_SYNC )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> m_nAttackAttributeIndex;
		rkPacket >> vPos;
		SetWorldPosition( vPos );
		SetNextAttack();
	}
	else
		ApplyDummyCharSyncByType( eType, rkPacket, pStage );
}