#include "StdAfx.h"

#include "ioTargetTrace18Dummy.h"
#include "ioZoneEffectWeapon.h"
#include "ioRotateTargetWeapon.h"

ioTargetTrace18Dummy::ioTargetTrace18Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_DummyState = DS_START;
	m_iCurAttackCount = 0;
}

ioTargetTrace18Dummy::~ioTargetTrace18Dummy(void)
{
}

void ioTargetTrace18Dummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	ioDummyChar::LoadProperty( rkLoader );
	rkLoader.LoadString_e( "owner_trace_ani", "", szBuf, MAX_PATH );
	m_OwnerTraceAni = szBuf;
	m_fOwnerTraceAniRate = rkLoader.LoadFloat_e( "owner_trace_ani_rate", 0.0f );
	m_fOwnerTraceCheckRange = rkLoader.LoadFloat_e( "owner_trace_check_range", 0.0f );
	m_fOwnerTraceRange = rkLoader.LoadFloat_e( "owner_trace_range", 0.0f );
	m_fOwnerTraceSpeed = rkLoader.LoadFloat_e( "owner_trace_speed", 0.0f );

	rkLoader.LoadString_e( "dying_ani", "", szBuf, MAX_PATH );
	m_szDyingAnimation = szBuf;
	m_fDyingAniRate = rkLoader.LoadFloat_e( "dying_ani_rate", 0.0f );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
	m_bAttachAreaWeapon = rkLoader.LoadBool_e( "attach_area_weapon", false );
	rkLoader.LoadString_e( "hp_absorb_buff", "", szBuf, MAX_PATH );
	m_szHPAbsorbBuff = szBuf;

	m_fAbsorbGauge = rkLoader.LoadFloat_e( "absorb_gauge", 0.0f );

	rkLoader.LoadString_e( "owner_trace_range_effect", "", szBuf, MAX_PATH );
	m_szOwnerTraceRangeEffect = szBuf;

	m_bNoDropZoneMove = rkLoader.LoadBool_e( "no_drop_zone_move", false );
	m_bTargetRotAttack = rkLoader.LoadBool_e( "target_rot_attack", false );

	m_vAttackCode.clear();
	for( int i=0 ; i<100 ; i++ )
	{
		wsprintf_e( szBuf, "dummy_trace_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );
		if ( iAttackCode == 0 )
			break;
		m_vAttackCode.push_back( iAttackCode );
	}
}

void ioTargetTrace18Dummy::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	m_pGroup->Update( fSecPerTime );

	if( m_bMountEnable )
	{
		UpdateMountedEntityList();
		UpdateMountedWeaponList();
	}

	ProcessState( fTimePerSec, dwPreTime );

	CheckDyingByHp();
	DropZoneCheck();
	CheckDestroyRange();

	if( CheckOwnerDestroyState() )
	{
		SendDieState( false );
	}
}

void ioTargetTrace18Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( false );
		return;
	}

	if( m_dwLifeTime && m_dwStartTime + m_dwLifeTime < FRAMEGETTIME() )
	{
		if ( m_DummyState != DS_TARGET_ATTACK && m_DummyState != DS_DYING && m_DummyState != DS_DIE )
		{
			SetDyingState( pOwner, false );
			return;
		}
	}

	switch( m_DummyState )
	{
	case DS_START:
		ProcessStartState();
		break;
	case DS_DELAY:
		ProcessDelayState( fTimerPerSec );
		break;
	case DS_OWNER_TRACE:
		ProcessOwnerTraceState( pOwner, fTimerPerSec );
		break;
	case DS_TARGET_SENSING:
		ProcessSensingState( pOwner, fTimerPerSec );
		break;
	case DS_TARGET_TRACE:
		ProcessTargetTraceState( pOwner, fTimerPerSec );
		CheckTraceAttack( pOwner );
		break;
	case DS_TARGET_ATTACK:
		CheckReservedSliding();
		ProcessTracingAttack( pOwner, fTimerPerSec, dwPreTime );
		break;
	case DS_DYING:
		ProcessDyingState( pOwner );
		break;
	}

	ProcessAreaWeapon();
	ProcessPos( fTimerPerSec );
}

void ioTargetTrace18Dummy::SetDelayState( bool bNet )
{
	//m_State가 Start일 경우 맵 충돌, DropZone체크를 안하기 때문에 Delay로 바꿔준다.
	m_State = DCS_DELAY;
	m_DummyState = DS_DELAY;
	SetLoopAni( m_DelayAnimation, m_fDelayAniRate );

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

void ioTargetTrace18Dummy::SetLoopAni( ioHashString szAniName, float fRate /*= 1.0f */ )
{
	if ( szAniName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szAniName );
	if( iAniID == -1 )
		return;
	if ( fRate <= 0.0f )
		return;

	m_pGroup->ClearAllActionAni( FLOAT100, true );
	m_pGroup->ClearAllLoopAni( FLOAT100, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fRate );
}

void ioTargetTrace18Dummy::ProcessStartState()
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

		if ( pOwner->IsNeedProcess() && !m_szOwnerTraceRangeEffect.IsEmpty() )
			m_pGroup->AttachEffect( m_szOwnerTraceRangeEffect, NULL );
	}
}

void ioTargetTrace18Dummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	//Check Owner
	if ( CheckOwnerTrace( pOwner ) )
	{
		SetOwnerTraceState( pOwner );
		return;
	}

	SetSensingState( pOwner, true );
}

bool ioTargetTrace18Dummy::CheckOwnerTrace( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vDiff = pOwner->GetWorldPosition() - GetWorldPosition();
	float fRangesq = D3DXVec3LengthSq( &vDiff );
	if ( fRangesq > m_fOwnerTraceCheckRange * m_fOwnerTraceCheckRange )
		return true;

	return false;
}

void ioTargetTrace18Dummy::SetOwnerTraceState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	m_DummyState = DS_OWNER_TRACE;
	SetLoopAni( m_OwnerTraceAni, m_fOwnerTraceAniRate );

	D3DXQUATERNION qtRot = GetWorldOrientation();
	m_vOwnerTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TTS_OWNER_TRACE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace18Dummy::ProcessOwnerTraceState( ioBaseChar *pOwner, float fTimerPerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vDiff = pOwner->GetWorldPosition() - GetWorldPosition();
	if ( pOwner->IsNeedProcess() )
	{
		float fRangesq = D3DXVec3LengthSq( &vDiff );
		if ( fRangesq <= m_fOwnerTraceRange * m_fOwnerTraceRange )
		{
			SetDelayState( true );
			return;
		}
	}
	
	CheckTracingDir( fTimerPerSec, m_vOwnerTracingMoveDir, vDiff );
	float fMoveDist = m_fOwnerTraceSpeed * fTimerPerSec;
	ProcessTraceMoving( pOwner, m_vOwnerTracingMoveDir, fMoveDist, m_bNoDropZoneMove );
}

void ioTargetTrace18Dummy::CheckTracingDir( float fTimerPerSec, D3DXVECTOR3& vTracingMoveDir, D3DXVECTOR3 vDiff )
{
	vDiff.y=0;
	D3DXVec3Normalize( &vDiff, &vDiff );

	if( m_dwRotateSpeed > 0 )
	{
		D3DXQUATERNION	qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDiff );

		float fAngle = D3DXVec3Dot( &vDiff, &vTracingMoveDir );

		float fAngleGap = RADtoDEG( acosf(fAngle) );
		float fRate = (fTimerPerSec*1000) / m_dwRotateSpeed;
		float fRotateAngle = 360.0f * fRate;

		if( fAngleGap < fRotateAngle )
			vTracingMoveDir = vDiff;
		else
		{
			D3DXQUATERNION qtRotate;
			ioMath::CalcDirectionQuaternion( qtRotate, -vTracingMoveDir );

			bool bLeftRot = false;
			if( D3DXVec3Dot( &vDiff, &vTracingMoveDir ) > -FLOAT1 )
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
			vTracingMoveDir = qtNewRot * vTracingMoveDir;
		}
	}
	else
		vTracingMoveDir = vDiff;
}

void ioTargetTrace18Dummy::ProcessTraceMoving( ioBaseChar *pOwner, D3DXVECTOR3& vDir, float fMoveAmt, bool bNoDropZoneMove )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	vPos += vDir * fMoveAmt;

	float fTopHeight = m_fEnableTraceMoveHeight * 4.0f;
	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+fTopHeight );
	float fGapHeight = fMapHeight - vPos.y;

	if ( fMapHeight <= 0.0f && bNoDropZoneMove )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		SetWorldOrientation( qtRot );
		return;
	}

	if( m_fEnableTraceMoveHeight < fabs(fGapHeight) )
	{
		if( fGapHeight <= 0.0f )
		{
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDir );
			SetWorldOrientation( qtRot );

			if( m_bCharColOnlyAttack )
			{
				m_LastAttacker = m_OwnerName;
				SendDieState( true );
			}
			else
			{
				SetTracingAttack( pOwner, false );
			}

			return;
		}
		else
		{
			float fNewMapHeight = 0.0f;
			D3DXVECTOR3 vDiff = vPrePos;

			if( fabs(vDir.x) >= fabs(vDir.z) )
			{
				if( vDir.x >= 0.0f )
					vDiff += D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
				else
					vDiff += D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
			}
			else
			{
				if( vDir.z >= 0.0f )
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
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetWorldOrientation( qtRot );
}


void ioTargetTrace18Dummy::SetSensingState( ioBaseChar *pOwner, bool bNet )
{
	m_DummyState = DS_TARGET_SENSING;
	m_TargetTraceName.Clear();
	SetLoopAni( m_SensingAni, m_fSensingAniRate );
	m_iCurAttackCount = 0;

	D3DXQUATERNION qtRot = GetWorldOrientation();
	m_vTracingMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if ( bNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TTS_SENSING;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace18Dummy::ProcessSensingState( ioBaseChar *pOwner, float fTimerPerSec )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( !m_szOwnerAttackTargetName.IsEmpty() )
	{
		if( IsTraceTarget( m_szOwnerAttackTargetName, pStage ) )
		{
			m_TargetTraceName = m_szOwnerAttackTargetName;
			SetTargetTraceState( pOwner );
			m_szOwnerAttackTargetName.Clear();
			return;
		}
	}

	//Check Owner
	if ( CheckOwnerTrace( pOwner ) )
	{
		SetOwnerTraceState( pOwner );
		return;
	}

	//Check Target
	ioHashString szTargetName = GetTraceTarget( pStage, false );
	ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
	if ( pTarget )
	{
		m_TargetTraceName = szTargetName;
		SetTargetTraceState( pOwner );
		return;
	}

}
void ioTargetTrace18Dummy::SetTargetTraceState( ioBaseChar *pOwner )
{
	m_DummyState = DS_TARGET_TRACE;
	m_dwTracingEndTime = FRAMEGETTIME() + m_dwTraceDuration;
	SetLoopAni( m_TracingAni, m_fTracingAniRate );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TTS_TARGET_TRACE;
		kPacket << m_TargetTraceName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace18Dummy::ProcessTargetTraceState( ioBaseChar *pOwner, float fTimerPerSec )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if( m_dwTracingEndTime < FRAMEGETTIME() )
		{
			SetSensingState( pOwner, true );
			return;
		}
		//Check Target
		if ( !IsTraceTarget( m_TargetTraceName, pStage ) )
		{
			SetSensingState( pOwner, true );
			return;
		}
	}
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetTraceName );
	if( pTarget )
	{
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
		CheckTracingDir( fTimerPerSec, m_vTracingMoveDir, vDiff );
		float fMoveDist = m_fTraceSpeed * fTimerPerSec;
		ProcessTraceMoving( pOwner, m_vTracingMoveDir, fMoveDist, m_bNoDropZoneMove );
	}
}

bool ioTargetTrace18Dummy::IsTraceTarget( ioHashString szTargetName, ioPlayStage *pStage )
{
	if( !pStage )
		return false;
	if( szTargetName.IsEmpty() )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( szTargetName );
	if( pTarget )
	{
		if( !szTargetName.IsEmpty() && pTarget->IsSystemState() )
			return false;

		if( !szTargetName.IsEmpty() && !pTarget->IsEnableTarget() )
			return false;

		if( !szTargetName.IsEmpty() )
		{
			D3DXVECTOR3 vPrePos = GetWorldPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			float fRange = D3DXVec3Length( & vDiff );
			if( fRangeSq >= m_fTraceRange * m_fTraceRange )
				return false;
		}
	}
	else
		return false;

	return true;
}

void ioTargetTrace18Dummy::CheckTraceAttack( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_TargetTraceName.IsEmpty() )
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
		{
			SetTracingAttack( pOwner, true );
		}
	}
}

void ioTargetTrace18Dummy::SetTracingAttack( ioBaseChar *pOwner, bool bNet )
{
	if ( !pOwner )
		return;

	if ( m_vAttackCode.empty() )
		return;

	if ( m_iCurAttackCount > (int)m_vAttackCode.size() )
		return;

	m_DummyState = DS_TARGET_ATTACK;
	m_iReserveAttackCode = m_vAttackCode[m_iCurAttackCount];

	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;

	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vPos = vPrePos;

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

	if ( m_bTargetRotAttack )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetTraceName );
		if ( pTarget )
		{
			D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
			SetWorldOrientation( qtRot );
		}
	}

	if( bNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TTS_TRACE_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace18Dummy::ProcessTracingAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
{
	if ( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwAttackAniEndTime && m_dwAttackAniEndTime < dwCurTime )
	{
		m_dwAttackAniEndTime = 0;
		if ( m_iCurAttackCount >= (int)m_vAttackCode.size() )
			SetSensingState( pOwner, false );
		else
			SetTracingAttack( pOwner, false );
		return;
	}

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
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
			++iter;
	}
}

ApplyWeaponExp ioTargetTrace18Dummy::ApplyWeapon( ioWeapon *pWeapon )
{
	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioDummyChar::ApplyWeapon - %s Not Exist", pWeapon->GetOwnerName().c_str() );
		return AWE_EXPLOSION;
	}

	if( pWeapon->IsNoWoundedWeapon() )
		return AWE_NO_EXPLOSION;

	if( P2PNetwork::IsNetworkPlaying() &&
		m_pModeParent->IsNetworkMode() &&
		!pAttacker->IsNeedProcess() )
		return AWE_EXPLOSION;

	m_LastAttacker = pWeapon->GetOwnerName();

	float fPreCurHP = m_HP.m_fCurValue;
	float fDamage = 0.0f;
	if( 0.0f < m_fAbsoluteDamage )
	{
		fDamage = m_fAbsoluteDamage;
	}
	else
	{
		fDamage = pWeapon->GetDamage( this ) * m_fDamageRate;
	}

	float fPushPower = pWeapon->GetDefensePushPower( fDamage );

	if( !m_bNoDamage && !m_bPiercingProtected )
		m_HP.m_fCurValue -= fDamage;
	AddHeadDamageInfo( (int)fDamage );

	bool bContactType = false;
	if( pWeapon->GetType() == ioWeapon::WT_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_RUN_CONTACT ||
		pWeapon->GetType() == ioWeapon::WT_CONTACT2 )
		bContactType = true;

	bool bStructWeapon = false;
	if( pWeapon->CheckWeaponByStruct() || pWeapon->CheckWeaponByDummyChar() )
		bStructWeapon = true;

	bool bOnlyEmoticon = true;
	D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir(this);
	if( bContactType && !bStructWeapon && !pWeapon->CheckWeaponBySkill() && !m_BlockBuff.IsEmpty() )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pWeapon->IsChangeJumpping( false, false )
			&& !pAttacker->IgnoreBlock() )
		{
			pAttacker->AddNewBuff( m_BlockBuff, "", "", NULL );
			pAttacker->ReverseReflectPush( vAttackDir, fPushPower * FLOAT05 );

			bOnlyEmoticon = false;
		}
	}

	if( !m_BlockBuff.IsEmpty() )
		pAttacker->SetDisableAttackEmoticon();

	int iShakeCamera = pWeapon->GetShakeCamera();
	ActionStopType eActionStop = pWeapon->GetActionStopType();

	if( eActionStop == AST_BOTH || eActionStop == AST_ATTACKER )
	{
		pAttacker->SetActionStopDelay( fDamage, false, iShakeCamera );
		pWeapon->SetActionStopDuration( ioBaseChar::CalcActionStopDelay( fDamage, false ),
			ioBaseChar::GetActionStopLooseRate( false ) );
	}

	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		SetDyingState( GetOwner(),  true ); 
	}
	else if( P2PNetwork::IsNetworkPlaying() && m_pModeParent->IsNetworkMode() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DAMAGE;
		kPacket << pWeapon->GetOwnerName();
		kPacket << m_HP.m_fCurValue;
		kPacket << m_HP.m_fMaxValue;
		kPacket << bOnlyEmoticon;
		kPacket << vAttackDir;
		kPacket << fDamage;
		kPacket << fPushPower;
		kPacket << (int)eActionStop;
		kPacket << iShakeCamera;
		kPacket << pWeapon->GetWeaponIndex();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}


	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, this, iDummyIndex, false );
	}

	return AWE_EXPLOSION;
}

bool ioTargetTrace18Dummy::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	if ( m_DummyState == DS_DYING || m_DummyState == DS_DIE )
		return false;

	return ioDummyChar::IsCollisionAvailableWeapon( pWeapon );
}

void ioTargetTrace18Dummy::SetDyingState( ioBaseChar *pOwner, bool bNet )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_DummyState == DS_DYING || m_DummyState == DS_DIE )
		return;

	m_DummyState = DS_DYING;

	int iAniID = m_pGroup->GetAnimationIdx( m_szDyingAnimation );
	if( iAniID == -1 )
		return;
	float fAniRate = FLOAT1;
	if( m_fStartAniRate > 0.0f )
		fAniRate = m_fDyingAniRate;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate );

	m_dwDyingEndTime = FRAMEGETTIME();
	m_dwDyingEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

	if ( m_iAreaWeaponIndex != -1 )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_iAreaWeaponIndex );
		if ( pAreaWeapon && pAreaWeapon->IsLive() )
			pAreaWeapon->DestroyAreaWeapon( pStage );
		m_iAreaWeaponIndex = -1;
	}
	m_vAreaWeaponPos = ioMath::VEC3_ZERO;

	if( bNet )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << TTS_DYING;
		kPacket << m_HP.m_fCurValue;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace18Dummy::ProcessDyingState( ioBaseChar *pOwner )
{
	if ( m_dwDyingEndTime < FRAMEGETTIME() )
	{
		SetDieState( true );
	}
}

void ioTargetTrace18Dummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	if( m_State == DCS_DIE )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		pOwner->CheckDummyCharDestroy( m_iCreateIndex );

	StopSound( m_szBGM, m_BGMID );

	CheckChargeWeaponDead();

	m_State = DCS_DIE;
	m_DummyState = DS_DIE;
	m_LastAttacker.Clear();

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	if( m_dwTeamEffect != -1 )
	{
		if( m_pModeParent && m_pModeParent->GetCreator() )
		{
			ioMapEffect *pMapEffect = m_pModeParent->GetCreator()->FindMapEffect( m_dwTeamEffect );
			if( pMapEffect )
			{
				pMapEffect->EndEffectForce();
			}
		}

		m_dwTeamEffect = -1;
	}

	if( m_bMoveEffectCreated )
	{
		m_pGroup->EndEffect( m_DummyEffect, true );
		m_bMoveEffectCreated = false;
	}

	if( bEffect )
	{
		ioMapEffect *pEffect = m_pModeParent->GetCreator()->CreateMapEffect( m_DestroyEffect,
			GetWorldPosition(),
			ioMath::UNIT_ALL );

		if( pEffect )
		{
			pEffect->SetWorldOrientation( GetWorldOrientation() );
		}
	}

	SetExtendDieState( m_pModeParent->GetCreator(), pOwner );

	int iCnt = m_vAttachWeaponIndexList.size();

	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_vAttachWeaponIndexList[i] );
		ioRotateTargetWeapon *pRotate = ToRotateTargetWeapon( pWeapon );
		if( pRotate )
			continue;

		if( pWeapon )
			pWeapon->SetWeaponDead();
	}

	if ( m_bDummyDeadDestoryWeapon && pOwner )
	{
		g_WeaponMgr.DestroyDummyCharWeapon( pOwner->GetCharName(), GetDummyCharIdx() );
	}

	DummyDieClearMountEntity();
	m_bCurMountEnable = false;
	if ( bEffect )
		CallEndWeapon( bWoundedDie );

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( m_iAreaWeaponIndex != -1 )
	{
		ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( m_iAreaWeaponIndex );
		if ( pAreaWeapon && pAreaWeapon->IsLive() )
			pAreaWeapon->DestroyAreaWeapon( pStage );
		m_iAreaWeaponIndex = -1;
	}
	m_vAreaWeaponPos = ioMath::VEC3_ZERO;
}

void ioTargetTrace18Dummy::ProcessAreaWeapon()
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

bool ioTargetTrace18Dummy::IsEnableTelepoteState()
{
	switch( m_DummyState )
	{
	case DS_DYING:
	case DS_DIE:
		return false;
	}

	return true;
}

void ioTargetTrace18Dummy::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	ioBaseChar *pChar = ToBaseChar( pEntity );
	if ( !pChar )
		return;

	m_HP.m_fCurValue += m_fAbsorbGauge;
	if ( m_HP.m_fCurValue > m_HP.m_fMaxValue )
		m_HP.m_fCurValue = m_HP.m_fMaxValue;
}

void ioTargetTrace18Dummy::TraceOwnerAttackTarget( ioBaseChar *pOwner, ioHashString szTargetName )
{
	if ( !pOwner )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if ( !IsTraceTarget( szTargetName, pStage ) )
		return;

	switch( m_DummyState )
	{
	case DS_OWNER_TRACE:
	case DS_TARGET_SENSING:
	case DS_TARGET_TRACE:
		{
			m_TargetTraceName = szTargetName;
			SetTargetTraceState( pOwner );
		}
		break;
	case DS_TARGET_ATTACK:
		m_szOwnerAttackTargetName = szTargetName;
		break;
	}
}

void ioTargetTrace18Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage )
		return;

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case TTS_OWNER_TRACE:
		SetOwnerTraceState( pOwner );
		break;
	case TTS_SENSING:
		SetSensingState( pOwner, false );
		break;
	case TTS_TARGET_TRACE:
		rkPacket >> m_TargetTraceName;
		SetTargetTraceState( pOwner );
		break;
	case TTS_TRACE_ATTACK:
		SetTracingAttack( pOwner, false );
		break;
	case TTS_DYING:
		{
			rkPacket >> m_HP.m_fCurValue;
			SetDyingState( pOwner, false );
		}
		break;
	case TTS_OWNER_ATTACK_TARGET:
		rkPacket >> m_szOwnerAttackTargetName;
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioTargetTrace18Dummy::CheckDyingByHp()
{
	if( m_State == DCS_DIE )
		return;

	DWORD dwTickTime = 0;
	if( m_pModeParent && m_pModeParent->GetModeType() == MT_TRAINING && 0 < m_dwTicTimeByTraining )
	{
		dwTickTime = m_dwTicTimeByTraining;
	}
	else
	{
		dwTickTime = m_dwTicTime;
	}

	if( dwTickTime <= 0 || m_fDecreaseHP <= 0.0f )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwCheckHPTime );
	if( iTickGap >= (int)dwTickTime )
	{
		m_dwCheckHPTime = dwCurTime - ( iTickGap - dwTickTime );
		m_HP.m_fCurValue -= m_fDecreaseHP;
	}

	bool bDie = false;
	if( m_HP.m_fCurValue <= 0.0f )
	{
		m_HP.m_fCurValue = 0.0f;
		bDie = true;
	}

	if( bDie )
	{
		m_LastAttacker = m_OwnerName;
		SetDyingState( GetOwner(), true );
	}
}