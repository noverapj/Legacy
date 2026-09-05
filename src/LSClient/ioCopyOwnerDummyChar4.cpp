#include "StdAfx.h"
#include "ioCopyOwnerDummyChar4.h"
#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

#include "ioZoneEffectWeapon.h"
#include "ioSuperDimensionItem.h"

#include "ioGuildMarkMgr.h"
#include "ioLadderTeamMgr.h"

ioCopyOwnerDummyChar4::ioCopyOwnerDummyChar4( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_iCurAttackCount = 0;

	m_pAniCollisionBoxGrp = NULL;
	
	m_dwFirstSensingMoveTime = 0;
	m_fCurTraceSpeed = 0.0f;
	m_bFirstSensingMove = false;
}


ioCopyOwnerDummyChar4::~ioCopyOwnerDummyChar4(void)
{
	SAFEDELETE( m_pAniCollisionBoxGrp );
}

void ioCopyOwnerDummyChar4::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );
	char szBuf[MAX_PATH] = "";
	/////////////////////////////////////////////
	m_vAttackCode.clear();
	for( int i=0 ; i<100 ; i++ )
	{
		wsprintf_e( szBuf, "dummy_trace_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );
		if ( iAttackCode == 0 )
			break;
		m_vAttackCode.push_back( iAttackCode );
	}

	ZeroMemory( szBuf, sizeof(szBuf) );
	rkLoader.LoadString_e( "check_buff_name", "", szBuf, MAX_PATH );
	m_szCheckBuffName = szBuf;

	rkLoader.LoadString_e( "explosion_effect", "", szBuf, MAX_PATH );
	m_ExplosionEffect = szBuf;

	rkLoader.LoadString_e( "explosion_sound", "", szBuf, MAX_PATH );
	m_ExplosionSound = szBuf;

	rkLoader.LoadString_e( "default_collision_mesh", "", szBuf, MAX_PATH );

	SAFEDELETE( m_pAniCollisionBoxGrp );
	m_pAniCollisionBoxGrp = g_CollisionBoxGrpMgr.CreateAniCollisionGrp( szBuf );

	m_bNoDropZoneMove = rkLoader.LoadBool_e( "no_drop_zone_move", false );

	m_bDisableWeapon = rkLoader.LoadBool_e( "disable_weapon", false );

	m_dwFirstSensingMoveDuration = (int)rkLoader.LoadInt_e("first_sensing_move_duration", 0 );
	m_dwFirstSensingMoveSpeed = rkLoader.LoadFloat_e( "first_sensing_move_speed", 0.0f );

	m_bShowOwnerName = rkLoader.LoadBool_e( "show_owner_name", false );
}

void ioCopyOwnerDummyChar4::SetCopyOwnerInfo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	ioBaseChar *pMyChar = NULL;
	if( !pStage )
		return;

	pMyChar = pStage->GetOwnerChar();
	if( !pMyChar )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ioEntityGroup *pDummyGrp = GetGroup();

	//m_DelayAnimation = pGrp->GetAnimationName( pOwner->GetDelayAnimationIdx() );
	//m_TracingAni = pGrp->GetAnimationName( pOwner->GetRunAnimationIdx() );

	// 팀셋팅
	TeamType eCurTeamType = pOwner->GetTeam();
	TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

	switch( eCurTeamType )
	{
	case TEAM_BLUE:
	case TEAM_RED:
		m_pGauge->SetTeam( eCurTeamType );
		break;
	default:
		m_pGauge->SetTeam( eCurSingleTeamType );
		break;
	}

	m_ArmorType = pOwner->GetArmorType();
}

void ioCopyOwnerDummyChar4::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( pOwner, fTimePerSec );
		break;
	case DCS_TRACE:
		ProcessTraceState( pOwner, pStage, fTimePerSec, dwPreTime );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimePerSec );
}

void ioCopyOwnerDummyChar4::ProcessDelayState( ioBaseChar* pOwner, float fTimePerSec )
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

		SetTraceState();
	}
}

void ioCopyOwnerDummyChar4::SetTraceState()
{
	m_State = DCS_TRACE;
	m_TraceState = DCTS_SENSING;

	m_dwSensingStartTime = FRAMEGETTIME();
	m_fCurTraceSpeed = 0.0f;
	if ( m_dwFirstSensingMoveDuration > 0 && !m_bFirstSensingMove )
	{
		m_dwFirstSensingMoveTime = FRAMEGETTIME() + m_dwFirstSensingMoveDuration;
		D3DXQUATERNION qtRot = GetWorldOrientation();
		m_vTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		m_fCurTraceSpeed = m_dwFirstSensingMoveSpeed;

		m_pGroup->ClearAllLoopAni( FLOAT100 );
		m_pGroup->ClearAllActionAni( FLOAT100 );

		int iAniID = m_pGroup->GetAnimationIdx( m_TracingAni );
		float fRate = FLOAT1;
		if( m_fTracingAniRate > 0.0f )
			fRate = m_fTracingAniRate;

		if( iAniID != -1 )
			m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fRate );

		m_bFirstSensingMove = true;
	}
}

void ioCopyOwnerDummyChar4::ProcessTraceState( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec, DWORD dwPreTime )
{
	if ( !pOwner || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_TraceState )
	{
	case DCTS_NONE:
		break;
	case DCTS_SENSING:
		ProcessTraceSensing( pOwner, pStage, fTimePerSec );
		break;
	case DCTS_TRACING:
		ProcessTraceTracing( pOwner, pStage, fTimePerSec );
		break;
	case DCTS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessTraceAttack( pOwner, fTimePerSec, dwPreTime );
		break;
	case DCTS_TRACING_WAIT:
		ProcessTraceTracingWait( fTimePerSec, dwPreTime );
		break;
	}

	float fMoveDist = m_fCurTraceSpeed * fTimePerSec;
	ProcessTraceMoving( pOwner, pStage, m_vTracingMoveDir, fMoveDist, m_bNoDropZoneMove );
	CheckTargetTraceAttack( pOwner, pStage );
}

void ioCopyOwnerDummyChar4::ProcessTraceSensing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec )
{
	if ( !pOwner || !pStage )
		return;

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
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
			SetTracingState( pStage, true );
			m_fCurTraceSpeed = m_fTraceSpeed;
		}
	}
	else if( m_dwFirstSensingMoveTime != 0 && m_dwFirstSensingMoveTime < FRAMEGETTIME() )
	{
		m_fCurTraceSpeed = 0.0f;
		m_dwFirstSensingMoveTime = 0;
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

ioHashString ioCopyOwnerDummyChar4::GetSearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSecond )
{
	ioHashString szTarget;
	szTarget.Clear();

	if ( !pOwner || !pStage )
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

void ioCopyOwnerDummyChar4::ProcessTraceTracing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec )
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
			SetTraceWaitState();
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
		SetTraceState();
		return;
	}	
}

void ioCopyOwnerDummyChar4::ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt, bool bNoDropZoneMove )
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

void ioCopyOwnerDummyChar4::CheckTargetTraceAttack( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;
	if( m_TargetTraceName.IsEmpty() )
		return;

	if( m_State != DCS_TRACE )
		return;
	if( m_TraceState != DCTS_TRACING )
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

void ioCopyOwnerDummyChar4::SetTraceAttack( ioBaseChar *pOwner )
{
	m_fCurTraceSpeed = 0.0f;
	if ( m_iCurAttackCount > (int)m_vAttackCode.size() )
		return;

	m_TraceState = DCTS_ATTACK;


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

void ioCopyOwnerDummyChar4::ProcessTraceAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
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

void ioCopyOwnerDummyChar4::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if( m_pModeParent && m_pModeParent->GetCreator() && !bEffect )
	{
		ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_ExplosionEffect, GetWorldPosition(), ioMath::UNIT_ALL );
		if( pEffect )
		{
			pEffect->SetWorldOrientation( GetWorldOrientation() );
			g_SoundMgr.PlaySound( m_ExplosionSound, m_pGroup->GetParentSceneNode(), PSM_THREAD );
		}
	}

	if( !m_szCheckBuffName.IsEmpty() && m_iCurAttackCount >= (int)m_vAttackCode.size() )
	{
		if( pOwner->GetState() != CS_DROP_ZONE_DOWN && !pOwner->HasBuff( pOwner->GetCharName(), m_szCheckBuffName ) )
		{
			ioBuff* pBuff = pOwner->AddNewBuff( m_szCheckBuffName, pOwner->GetCharName(), "", NULL );
			//버프가 잘 생성 되었을 경우만 이동시켜준다.
			if ( pBuff )
				pOwner->SetWorldPosition( GetWorldPosition() );
		}
		
	}

	ioDummyChar::SetDieState( bEffect, bWoundedDie );
}

void ioCopyOwnerDummyChar4::DropZoneCheck()
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

ioOrientBox ioCopyOwnerDummyChar4::GetAttackCollisionBox( AniCollisionType eType /*= ACT_NONE*/, bool bNoScale/*=false */ )
{
	AniCollisionBox *pColBox = NULL;
	float fBoxScaleRate = FLOAT1;
	float fBoxMoveOffset = 0.0f;

	ioWeaponItem *pWeapon = NULL;
	if( GetOwner()->GetEquipSlot() )
	{
		if( GetOwner()->GetObject() )
			pWeapon = GetOwner()->GetObject();
		else
			pWeapon = GetOwner()->GetEquipSlot()->GetWeapon();
	}

	if( pWeapon && eType != ACT_SPECIAL )
	{
		ioAniCollisionBoxGrp *pBoxGrp = pWeapon->GetAniCollisionBoxGrp();
		if( pBoxGrp && pBoxGrp->HasCollisionBox( eType ) )
		{
			pColBox = pBoxGrp->GetCollisionBox( eType );
			fBoxScaleRate  = pWeapon->GetCollisionBoxScale();
			fBoxMoveOffset = pWeapon->GetCollisionBoxOffset();
		}
	}

	if( !pColBox && eType == ACT_SPECIAL )
	{
		if( m_pSpecialBox && m_pSpecialBox->HasCollisionBox( eType ) )
		{
			pColBox = m_pSpecialBox->GetCollisionBox( eType );
			fBoxMoveOffset = 0.0f;
		}
	}

	if( !pColBox && m_pAniCollisionBoxGrp && m_pAniCollisionBoxGrp->HasCollisionBox( eType ) )
	{
		pColBox = m_pAniCollisionBoxGrp->GetCollisionBox( eType );
		fBoxMoveOffset = 0.0f;
	}

	ioOrientBox kBox;
	if( pColBox )
	{
		ioNode *pNode = m_pGroup->GetParentNode();
		ioBiped *pBiped = m_pGroup->GetBiped( pColBox->m_LinkBone );
		if( pNode && pBiped )
		{
			kBox = pColBox->m_Box;

			if( fBoxMoveOffset != 0.0f )
			{
				kBox.SetCenter( kBox.GetCenter() - kBox.GetAxis(2) * fBoxMoveOffset );
			}

			D3DXMATRIX matWorld;
			D3DXMatrixMultiply( &matWorld, pBiped->GetBipedSpaceFullXform(), pNode->GetFullTransform() );
			kBox.Transform( &matWorld );
		}
	}
	else
	{
		kBox = GetWorldCollisionBox();

		float fExtentsX = kBox.GetExtents( 0 );
		float fExtentsY = kBox.GetExtents( 1 );
		float fExtentsZ = kBox.GetExtents( 2 );

		float fZLength = 3.0f;

		D3DXVECTOR3 vDiff = kBox.GetAxis(0) * fExtentsX * ( fZLength + FLOAT1 );
		vDiff += kBox.GetAxis(1) * fExtentsY * FLOAT05;
		kBox.SetCenter( kBox.GetCenter() + vDiff );

		kBox.SetExtents( 0, fExtentsX * 2.0f );
		kBox.SetExtents( 1, fExtentsY * FLOAT05 );
		kBox.SetExtents( 2, fExtentsZ );
	}

	return kBox;
}

void ioCopyOwnerDummyChar4::UpdateBound() const
{
	UpdateWorldAxisBox();
}

void ioCopyOwnerDummyChar4::UpdateWorldAxisBox() const
{
	ioBiped *pBiped = m_pGroup->GetBiped( "Bip01" );
	if( !pBiped )
	{
		ioPlayEntity::UpdateBound();
		return;
	}

	m_WorldColBox = GetCollisionBox();
	m_WorldColBox.SetCenter( ioMath::VEC3_ZERO );

	D3DXVECTOR3 vTranslate;
	vTranslate = GetWorldOrientation() * pBiped->GetDerivedPosition() + GetWorldPosition();

	D3DXQUATERNION qtRotate;
	D3DXQuaternionMultiply( &qtRotate,
		&pBiped->GetDerivedOrientation(),
		&GetWorldOrientation() );

	D3DXVECTOR3 vScale = GetWorldScale();
	m_WorldColBox.Transform( vTranslate, vScale, qtRotate );
	m_WorldAxisBox.SetByOrientBox( m_WorldColBox );

	D3DXVECTOR3 vMin, vMax;
	vMin = m_WorldAxisBox.GetMinPos();
	vMax = m_WorldAxisBox.GetMaxPos();

	float m_fAxisCollisionBoxRate = FLOAT08;
	D3DXVECTOR3 vDiff = vMax - vMin;
	vDiff.y = 0.0f;
	vDiff *= ( FLOAT1 - m_fAxisCollisionBoxRate ) * FLOAT05;
	vMax -= vDiff;
	vMin += vDiff;
	m_WorldAxisBox.SetMinMaxPos( vMin, vMax );

	ioOrientBox kDefColBox = GetCollisionBox();
	kDefColBox.Transform( GetWorldPosition(),
		GetWorldScale(),
		GetWorldOrientation() );

	ioAxisAlignBox kDefAxisBox;
	kDefAxisBox.SetByOrientBox( kDefColBox );
	m_WorldAxisBox.Merge( kDefAxisBox );
}

void ioCopyOwnerDummyChar4::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DUMMY_CHAR_TRACE:
		{
			bool bFirst;
			rkPacket >> m_TargetTraceName;
			rkPacket >> bFirst;

			if( bFirst )
			{
				m_State = DCS_TRACE;
				SetTracingState( pStage, false );
			}
			m_fCurTraceSpeed = m_fTraceSpeed;
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioCopyOwnerDummyChar4::RenderGauge()
{
	if( m_bShowOwnerName )
	{
		if( !m_pGauge || !m_bGaugeVisible || m_bDisableHPVisible ) return;

		bool bCheckMode = g_GUIMgr.IsNoneUIMode();
		ioPlayStage *pStage = m_pModeParent->GetCreator();
		if( pStage && !bCheckMode )
		{
			if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
				bCheckMode = true;
		}

		if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

		D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		int iXPos = 0, iYPos = 0;
		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			RenderGaugeOwnerInfo( iXPos, iYPos );
		}
	}
	else
	{
		ioDummyChar::RenderGauge();
	}
}

void ioCopyOwnerDummyChar4::RenderGaugeOwnerInfo( int iXPos, int iYPos )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	if( m_pGauge )
	{
		g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		TeamType eCurTeamType = pOwner->GetTeam();
		TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

		switch( eCurTeamType )
		{
		case TEAM_BLUE:
			g_FontMgr.SetTextColor( 0, 150, 255 );
			break;
		case TEAM_RED:
			g_FontMgr.SetTextColor( 255, 60, 0 );
			break;
		default: //개인전
			{
				if( eCurSingleTeamType == TEAM_BLUE )
					g_FontMgr.SetTextColor( 0, 150, 255 );
				else
					g_FontMgr.SetTextColor( 255, 60, 0 );
			}				
			break;
		}

		enum { UPPER_Y_OFFSET = -12, GUILD_Y_OFFSET = -11, LADDER_TITLE_Y_OFFSET = -16, };
		if( pStage->GetModeType() == MT_MYROOM && !pStage->IsNetworkMode() )
		{
			if( pOwner->IsOwnerChar() )
			{
				g_FontMgr.SetAlignType( TAT_LEFT );

				DWORD dwGuildIndex, dwGuildMark;
				g_UserInfoMgr.GetGuildInfo( g_MyInfo.GetPublicID(), dwGuildIndex, dwGuildMark );
				int iNameWidth = g_FontMgr.GetTextWidth( g_MyInfo.GetPublicID().c_str(), TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
				int iHalfWidth = ( iNameWidth + 20 + g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

				g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
				g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, g_MyInfo.GetPublicID().c_str() );

				g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

				// 래더전 타이틀
				if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
				}
			}
		}
		else
		{
			g_FontMgr.SetAlignType( TAT_LEFT );

			char szCharName[MAX_PATH] = "";
			sprintf( szCharName, "%s", pOwner->GetCharName().c_str() );
			DWORD dwGuildIndex, dwGuildMark;
			g_UserInfoMgr.GetGuildInfo( pOwner->GetCharName(), dwGuildIndex, dwGuildMark );
			int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
			int iHalfWidth = ( iNameWidth + 20+ g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
				break;
			case TEAM_RED:
				g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
				break;
			default: //개인전
				if( eCurSingleTeamType == TEAM_BLUE )
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
				else
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
				break;
			}

			g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, szCharName );
			g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

			// 래더전 타이틀
			if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
			{
				g_FontMgr.SetAlignType( TAT_CENTER );
				g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
			}
		}

		enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
		switch( eCurTeamType )
		{
		case TEAM_BLUE:
		case TEAM_RED:
			if(Help::IsMonsterDungeonMode(pStage->GetModeType()))
			{
				g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetGrowthInstantLevel()+1, eCurTeamType, pOwner->IsOwnerChar() );
			}
			else
			{
				g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), eCurTeamType, pOwner->IsOwnerChar() );
			}
			break;
		default: //개인전
			if( eCurSingleTeamType == TEAM_BLUE )
				g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_BLUE, pOwner->IsOwnerChar() );
			else
				g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_RED, pOwner->IsOwnerChar() );
			break;
		}

		enum { GAUGE_Y_OFFSET = 6, };
		m_pGauge->RenderGauge( iXPos, iYPos + GAUGE_Y_OFFSET, false );
	}
}