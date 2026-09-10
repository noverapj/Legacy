#include "StdAfx.h"
#include "ioPappyDummy.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioZoneEffectWeapon.h"


ioPappyDummy::ioPappyDummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_PappyState = PS_START;
	m_PappyAttackState = PAS_NONE;
	m_vPappyMoveDir = ioMath::VEC3_ZERO;
	m_vPappyTargetPos = ioMath::VEC3_ZERO;
	m_fCurMoveMaxRange = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_vWeaponIndexList.clear();
	m_dwHideEndTime = 0;
	m_dwAttackStartAniEndTime = 0;
}

ioPappyDummy::~ioPappyDummy(void)
{
}

void ioPappyDummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	ioDummyChar::LoadProperty( rkLoader );

	rkLoader.LoadString_e( "jumpping_animation", "", szBuf, MAX_PATH );
	m_szJumppingAni = szBuf;
	m_fJumppingAniRate = rkLoader.LoadFloat_e( "jumpping_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "run_animation", "", szBuf, MAX_PATH );
	m_RunAnimation = szBuf;
	m_fRunAniRate = rkLoader.LoadFloat_e( "run_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_landing_animation", "", szBuf, MAX_PATH );
	m_szJumpLandingAni = szBuf;
	m_fJumpLandingAniRate = rkLoader.LoadFloat_e( "jump_landing_ani_rate", FLOAT1 );

	m_vBaseOffset.x = rkLoader.LoadFloat_e( "base_offset_x", 0.0f );
	m_vBaseOffset.y = rkLoader.LoadFloat_e( "base_offset_y", 0.0f );
	m_vBaseOffset.z = rkLoader.LoadFloat_e( "base_offset_z", 0.0f );

	m_fPappyAttackMoveSpeed = rkLoader.LoadFloat_e( "pappy_attack_move_speed", 0 );
	m_fPappyAttackAngleH = rkLoader.LoadFloat_e( "pappy_attack_angle_h", 0 );
	m_fPappyAttackAngleV = rkLoader.LoadFloat_e( "pappy_attack_angle_v", 0 );
	m_fPappyAttackDuration = rkLoader.LoadFloat_e( "pappy_attack_duration", 0 );

	//Return
	rkLoader.LoadString_e( "return_animation", "", szBuf, MAX_PATH );
	m_szReturnAnimation = szBuf;
	m_fReturnAniRate = rkLoader.LoadFloat_e( "return_ani_rate", FLOAT1 );
	m_fReturnLimitRange = rkLoader.LoadFloat_e( "return_limit_range", 0.f );
	m_fReturnMoveSpeed = rkLoader.LoadFloat_e( "return_move_speed", 0.f );

	rkLoader.LoadString_e( "bite_buff_name", "", szBuf, MAX_PATH );
	m_szBiteBuffName = szBuf;

	m_fReplicaTargetAngle = rkLoader.LoadFloat_e( "replica_target_angle", 0.0f );
	m_fReplicaTargetRange = rkLoader.LoadFloat_e( "replica_target_range", 0.0f );

	m_vReplicaTargetOffset.x = rkLoader.LoadFloat_e( "replica_target_offset_x", 0.0f );
	m_vReplicaTargetOffset.y = rkLoader.LoadFloat_e( "replica_target_offset_y", 0.0f );
	m_vReplicaTargetOffset.z = rkLoader.LoadFloat_e( "replica_target_offset_z", 0.0f );

	m_vReplicaNoneTargetOffset.x = rkLoader.LoadFloat_e( "replica_none_target_offset_x", 0.0f );
	m_vReplicaNoneTargetOffset.y = rkLoader.LoadFloat_e( "replica_none_target_offset_y", 0.0f );
	m_vReplicaNoneTargetOffset.z = rkLoader.LoadFloat_e( "replica_none_target_offset_z", 0.0f );

	m_fMaxReplicaAttackRange = rkLoader.LoadFloat_e( "max_replica_attack_range", 0.0f );
	m_fLimitReplicaAttackOwnerRange = rkLoader.LoadFloat_e( "limit_replica_attack_owner_range", 0.0f );

	m_dwHideDuration = rkLoader.LoadInt_e( "hide_duration", 0 );


	rkLoader.LoadString_e( "rush_start_animation", "", szBuf, MAX_PATH );
	m_szRushAttackStartAni = szBuf;
	m_fRushAttackStartAniRate = rkLoader.LoadFloat_e( "rush_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "rush2_start_animation", "", szBuf, MAX_PATH );
	m_szRush2AttackStartAni = szBuf;
	m_fRush2AttackStartAniRate = rkLoader.LoadFloat_e( "rush2_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_start_animation", "", szBuf, MAX_PATH );
	m_szTeleportAttackStartAni = szBuf;
	m_fTeleportAttackStartAniRate = rkLoader.LoadFloat_e( "teleport_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "hide_start_animation", "", szBuf, MAX_PATH );
	m_szHideAttackStartAni = szBuf;
	m_fHideAttackStartAniRate = rkLoader.LoadFloat_e( "hide_start_ani_rate", FLOAT1 );

	m_vHidePappySkillNameList.clear();
	int iMaxSkillCount = rkLoader.LoadInt_e( "hide_pappy_skill_name_max_count", 0 );
	if ( iMaxSkillCount > 0 )
	{
		m_vHidePappySkillNameList.reserve( iMaxSkillCount );
		for ( int i=0; i<iMaxSkillCount ; ++i )
		{
			ioHashString szSkillName;
			wsprintf_e( szBuf, "hide_pappy_skill_name%d", i+1 );
			rkLoader.LoadString( szBuf, "", szKey, MAX_PATH );
			szSkillName = szKey;
			m_vHidePappySkillNameList.push_back( szSkillName );
		}
	}
}

void ioPappyDummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	switch( m_PappyState )
	{
	case PS_START:
		ProcessStartState( pOwner );
		break;
	case PS_DELAY:
		ProcessDelayState( pOwner, fTimerPerSec );
		break;
	case PS_RUN:
		ProcessRunState( pOwner, fTimerPerSec );
		break;
	case PS_MOVE:
		ProcessMoveState( pOwner, fTimerPerSec );
		break;
	case PS_JUMP:
		ProcessJumpState( pOwner, fTimerPerSec );
		break;
	case PS_ATTACK:
		ProcessPappyAttackState( pOwner, fTimerPerSec, dwPreTime );
		break;
	case PS_RETURN:
		{
			if( CheckEnableAttackState() )
			{
				SetAttackStartState( pOwner );
				return;
			}
		}
		break;
	case PS_DIE:
		ProcessDieState( fTimerPerSec );
		break;
	}

	CheckPappyVisible( pOwner );
	ProcessPos( pOwner, fTimerPerSec );
}

void ioPappyDummy::ProcessStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetDelayState( pOwner, false );
		return;
	}
}

void ioPappyDummy::SetDelayState( ioBaseChar *pOwner, bool bNet )
{
	m_PappyState = PS_DELAY;
	m_iReserveAttackCode = 0;

	SetLoopAni( m_DelayAnimation, m_fDelayAniRate );

	if( bNet && pOwner && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DUMMY_CHAR_DELAY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPappyDummy::ProcessDelayState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	if( pOwner->GetState() == CS_DASH )
	{
		SetMoveState();
		return;
	}
	else if( pOwner->GetState() == CS_RUN )
	{
		SetRunState();
		return;
	}
	else if ( pOwner->GetState() == CS_JUMP )
	{
		const JumpState& rkJumpState = pOwner->GetJumpState();
		SetJumpState( rkJumpState );
		return;
	}

	if( CheckEnableAttackState() )
	{
		SetAttackStartState( pOwner );
		return;
	}
}

void ioPappyDummy::SetRunState()
{
	m_PappyState = PS_RUN;

	SetLoopAni( m_RunAnimation, m_fRunAniRate );
}

void ioPappyDummy::ProcessRunState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	if( pOwner->GetState() != CS_RUN )
	{
		SetDelayState( pOwner, false );
		return;
	}
}
void ioPappyDummy::SetMoveState()
{
	m_PappyState = PS_MOVE;

	SetLoopAni( m_MoveAnimation, m_fMoveAniRate );
}

void ioPappyDummy::ProcessMoveState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	if( pOwner->GetState() != CS_DASH )
	{
		SetDelayState( pOwner, false );
		return;
	}
}

void ioPappyDummy::SetJumpState( JumpState OwnerJumpState )
{
	m_PappyState = PS_JUMP;
	m_OwnerJumpState = OwnerJumpState;

	m_iReserveAttackCode = 0;
	if ( m_OwnerJumpState != JS_LANDING )
		SetActionAni( m_szJumppingAni, m_fJumppingAniRate );
	else
		SetLoopAni( m_szJumpLandingAni, m_fJumpLandingAniRate );
}

void ioPappyDummy::ProcessJumpState( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;

	if ( pOwner->GetState() != CS_JUMP )
	{
		SetDelayState( pOwner, false );
		return;
	}

	const JumpState& rkJumpState = pOwner->GetJumpState();
	if ( m_OwnerJumpState == JS_LANDING && rkJumpState != JS_LANDING )
	{
		SetJumpState( rkJumpState );
		return;
	}
	else if ( m_OwnerJumpState != JS_LANDING && rkJumpState == JS_LANDING )
	{
		SetJumpState( rkJumpState );
		return;
	}
}

void ioPappyDummy::ProcessPos( ioBaseChar *pOwner, float fTimePerSec )
{
	if ( !pOwner )
		return;


	DWORD dwCurTime = FRAMEGETTIME();
	if( m_PappyState == PS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	switch( m_PappyState )
	{
	case PS_START:
	case PS_DELAY:
	case PS_MOVE:
	case PS_RUN:
	case PS_JUMP:
		{
			D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + pOwner->GetTargetRot() * m_vBaseOffset;

			SetWorldPosition( vNewPos );
			SetWorldOrientation( pOwner->GetTargetRot() );
		}
		return;
	case PS_ATTACK:
		{
			switch( m_PappyAttackState )
			{
			case PAS_RUSH:
			case PAS_RUSH2:
				{
					bool bEnd = false;
					D3DXVECTOR3 vMove;
					float fMoveSpeed = m_fPappyAttackMoveSpeed * fTimePerSec;

					if( m_fCurMoveRange + fMoveSpeed < m_fCurMoveMaxRange )
						m_fCurMoveRange += fMoveSpeed;
					else
					{
						fMoveSpeed = m_fCurMoveMaxRange - m_fCurMoveRange;
						m_fCurMoveRange = m_fCurMoveMaxRange;
						bEnd = true;
					}

					vMove = fMoveSpeed * m_vPappyMoveDir;
					Translate( vMove );

					if( bEnd )
						SetReturnState( pOwner );
				}
				break;
			case PAS_TELEPOLT:
				{
					D3DXVECTOR3 vPrePos = GetWorldPosition();
					D3DXVECTOR3 vCurPos = vPrePos;
					float fMapHeight = GetBottomHeight();

					DWORD dwGapTime = dwCurTime - m_dwProcessTime;

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
							vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;

						float fCurRate = m_fGravityRate;
						if( fCurRate <= 0.0f )
							fCurRate = FLOAT1;

						m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate )* fNewTimePerSec;
						vCurPos.y -= m_fGravityAmt * fNewTimePerSec;
					} while( dwGapTime > 0 );

					m_fCurMoveRange += abs(vCurPos.y - vPrePos.y);
					SetWorldPosition( vCurPos );
					m_dwProcessTime = dwCurTime;
				}
				break;
			}
		}
		return;
	case PS_RETURN:
		{
			D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition()+ pOwner->GetTargetRot() * m_vBaseOffset;
			D3DXVECTOR3 vDir = vNewPos - GetWorldPosition();
			float fGap = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			if( fGap <= 15.0f )
			{
				SetWorldPosition( vNewPos );
				SetWorldOrientation( pOwner->GetTargetRot() );

				SetDelayState( pOwner, false );
			}
			else if( fGap > m_fReturnLimitRange )
			{
				SetWorldPosition( vNewPos );
				SetWorldOrientation( pOwner->GetTargetRot() );

				SetDelayState( pOwner, false );
			}
			else
			{
				D3DXVECTOR3 vMove;

				float fMoveSpeed = m_fReturnMoveSpeed * fTimePerSec;
				vMove = fMoveSpeed * vDir;

				Translate( vMove );
			}	
		}
		return;
	}
}

void ioPappyDummy::SetAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_vAttackFireTimeList.clear();
	m_dwAttackKeyCheckTime = 0;
	m_dwAttackStartAniEndTime = 0;
	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;
	int iSubType = iter->second.m_iSubUseType;
	
	switch( iSubType )
	{
	//러시 독
	case DSUT_EXTRA_ATTACK1:
		SetRushState( pOwner, rkCurAttribute );
		break;
	//머신건 독
	case DSUT_EXTRA_ATTACK2:
		SetRush2State( pOwner, rkCurAttribute );
		break;
	//레프리카 독
	case DSUT_EXTRA_ATTACK3:
		SetTeleportAttackState( pOwner, rkCurAttribute );
		break;
	//오버 헤드 크러쉬
	case DSUT_EXTRA_ATTACK4:
		SetHideState( pOwner );
		break;
	}
	m_iReserveAttackCode = 0;
	m_PappyState = PS_ATTACK;
}

void ioPappyDummy::SetRushState( ioBaseChar *pOwner, const AttackAttribute& rkAttack )
{
	if( !pOwner )
		return;

	m_fCurMoveMaxRange = m_fPappyAttackMoveSpeed * m_fPappyAttackDuration / FLOAT1000;
	m_vPappyMoveDir = RotateMoveDir( GetWorldOrientation(), m_fPappyAttackAngleH, m_fPappyAttackAngleV );
	m_vPappyTargetPos = GetWorldPosition() + (m_fCurMoveMaxRange * m_vPappyMoveDir);
	m_fCurMoveRange = 0.0f;

	m_PappyAttackState = PAS_RUSH;
	
	D3DXVECTOR3 vNewDir = m_vPappyMoveDir;
	vNewDir.y = 0.0f;
	D3DXVec3Normalize( &vNewDir, &vNewDir );
	SetWorldOrientation( Help::ConvertDirToQuaternion( vNewDir ) );

	SetAttackAni( pOwner, rkAttack, false );
}

D3DXVECTOR3 ioPappyDummy::RotateMoveDir( const D3DXQUATERNION& qtOri, float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXVECTOR3 vCurDir = qtOri * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vCurDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	// vertical
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
	vCurDir = qtRot * vCurDir;

	// horizon
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
	vCurDir = qtRot * vCurDir;
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	return vCurDir;
}

void ioPappyDummy::SetAttackAni( ioBaseChar *pOwner, const AttackAttribute& rkCurAttribute, bool bAction )
{
	if ( !pOwner )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwStartTime = dwCurTime + rkCurAttribute.m_dwPreDelay;
	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	float fAniRate = rkCurAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	if ( bAction )
	{
		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		m_dwAttackAniEndTime = dwStartTime + m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;
	}
	else
		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

	// fire 처리
	AniEventConstPtrList vFireStartList;
	m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

	std::list<float> vFireEndList;
	m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

	const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
	if( vFireStartList.size() != vInfoList.size() )
		return;

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	

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

void ioPappyDummy::ProcessPappyAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
{
	switch( m_PappyAttackState )
	{
	case PAS_START:
		ProcessAttackStartState( pOwner );
		break;
	case PAS_BITE:
		ProcessBiteState( pOwner );
		break;
	case PAS_TELEPOLT:
		ProcessTeleprotAttackState( pOwner, fTimePerSec, dwPreTime );
		break;
	case PAS_HIDE:
		ProcessHideState( pOwner );
		break;
	default:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( pOwner, fTimePerSec, dwPreTime );
		break;
	}
}

void ioPappyDummy::ProcessAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
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
}

void ioPappyDummy::SetReturnState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( m_PappyState == PS_ATTACK )
	{
		switch( m_PappyAttackState )
		{
		case PAS_RUSH:
			ClearWeaponIndexList();
			break;
		case PAS_TELEPOLT:
		case PAS_HIDE:
			{
				ClearWeaponIndexList();
				D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + pOwner->GetTargetRot() * m_vBaseOffset;
				SetWorldPosition( vNewPos );
				SetWorldOrientation( pOwner->GetTargetRot() );
				SetDelayState( pOwner, false );
				return;
			}
		}
	}

	m_vWeaponIndexList.clear();
	m_PappyState = PS_RETURN;

	SetLoopAni( m_szReturnAnimation, m_fReturnAniRate );
}

bool ioPappyDummy::CheckEnableUseCmd()
{
	if ( m_PappyState == PS_ATTACK )
		return false;

	return true;
}

void ioPappyDummy::ClearWeaponIndexList()
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

bool ioPappyDummy::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	if ( m_PappyState != PS_ATTACK )
		return true;

	if ( m_PappyAttackState == PAS_START )
		return true;

	ioOrientBox kEntityBox;
	kEntityBox.SetByAxisBox( GetWorldAxisBox() );

	if( !pShape->FindIntersection( pShapeWorldMat, kEntityBox, NULL ) )
		return false;

	SetReturnState( GetOwner() );

	return true;
}

void ioPappyDummy::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	if ( m_PappyState != PS_ATTACK )
		return;

	ioBaseChar *pTarget = ToBaseChar(pEntity);

	switch( m_PappyAttackState )
	{
	case PAS_RUSH2:
		if( pTarget && pTarget->GetTeam() != GetTeam() )
		{
			SetBiteState( pTarget->GetCharName() );
			return;
		}
		break;
	}

	SetReturnState( GetOwner() );
}

void ioPappyDummy::SetRush2State( ioBaseChar *pOwner, const AttackAttribute& rkAttack )
{
	if( !pOwner )
		return;

	m_fCurMoveMaxRange = m_fPappyAttackMoveSpeed * m_fPappyAttackDuration / FLOAT1000;
	m_vPappyMoveDir = RotateMoveDir( GetWorldOrientation(), m_fPappyAttackAngleH, m_fPappyAttackAngleV );
	m_vPappyTargetPos = GetWorldPosition() + (m_fCurMoveMaxRange * m_vPappyMoveDir);
	m_fCurMoveRange = 0.0f;

	m_PappyAttackState = PAS_RUSH2;

	D3DXVECTOR3 vNewDir = m_vPappyMoveDir;
	vNewDir.y = 0.0f;
	D3DXVec3Normalize( &vNewDir, &vNewDir );
	SetWorldOrientation( Help::ConvertDirToQuaternion( vNewDir ) );

	SetAttackAni( pOwner, rkAttack, true );
}

void ioPappyDummy::SetBiteState( ioHashString szTargetName )
{
	m_PappyAttackState = PAS_BITE;
	m_szBiteTargetName = szTargetName;
}

void ioPappyDummy::CheckPappyVisible( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	bool bCharVisible = CheckPappyOwnerVisible( pOwner );
	bool bDummyVisible = m_pGroup->IsVisible();

	switch( m_PappyState )
	{
	case PS_START:
	case PS_DELAY:
	case PS_MOVE:
	case PS_JUMP:
	case PS_RUN:
		if( bCharVisible && !bDummyVisible )
			m_pGroup->SetVisible( true );
		else if( !bCharVisible && bDummyVisible )
			m_pGroup->SetVisible( false );
		break;
	case PS_ATTACK:
		{
			if ( m_PappyAttackState == PAS_BITE || m_PappyAttackState == PAS_HIDE )
			{
				if( bDummyVisible )
					m_pGroup->SetVisible( false );
			}
			else if( !bDummyVisible )
				m_pGroup->SetVisible( true );
		}
		break;
	default:
		if( !bDummyVisible )
			m_pGroup->SetVisible( true );
		break;
	}
}

bool ioPappyDummy::CheckPappyOwnerVisible( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	bool bCharVisible = pOwner->IsVisibleChar();

	if( pOwner->IsCanInvisible() )
		bCharVisible = false;

	if( pOwner->HasHideBuff() )
		bCharVisible = false;

	if ( pOwner->GetState() == CS_USING_SKILL && pOwner->GetCurrActiveSkill() )
	{
		ioHashString szSkillName = pOwner->GetCurrActiveSkill()->GetName();
		for ( int i=0; i<(int)m_vHidePappySkillNameList.size() ; ++i )
		{
			if ( szSkillName == m_vHidePappySkillNameList[i] )
			{
				bCharVisible = false;
				break;
			}
		}
	}

	return bCharVisible;
}

void ioPappyDummy::ProcessBiteState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_szBiteBuffName.IsEmpty() )
	{
		SetReturnState( pOwner );
		return;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_szBiteTargetName );
	if( !pTarget || !pTarget->HasBuff( m_szBiteBuffName ) )
	{
		SetReturnState( pOwner );
		return;
	}
}

void ioPappyDummy::SetTeleportAttackState( ioBaseChar *pOwner, const AttackAttribute& rkAttack )
{
	m_PappyAttackState = PAS_TELEPOLT;

	ioHashString szTarget = FindAttTarget( pOwner, m_fReplicaTargetAngle, m_fReplicaTargetRange );
	ioBaseChar* pTarget = pOwner->GetBaseChar( szTarget );
	if ( pTarget )
	{
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		D3DXVECTOR3 vNewPos = pTarget->GetWorldOrientation() * m_vReplicaTargetOffset;
		vNewPos += vTargetPos;

		SetWorldPosition( vNewPos );

		D3DXVECTOR3 vDir = vTargetPos - vNewPos;
		D3DXVec3Normalize( &vDir, &vDir );
		vDir.y = 0.0f;
		SetWorldOrientation( Help::ConvertDirToQuaternion( vDir ) );
	}
	else
	{
		D3DXVECTOR3 vNewPos = GetWorldOrientation() * m_vReplicaNoneTargetOffset;
		vNewPos += GetWorldPosition();
		SetWorldPosition( vNewPos );
	}

	m_dwProcessTime = FRAMEGETTIME();
	
	m_fGravityAmt = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_fCurMoveMaxRange = m_fMaxReplicaAttackRange;
	SetAttackAni( pOwner, rkAttack, true );
}

ioHashString ioPappyDummy::FindAttTarget( ioBaseChar *pOwner, float fAngle, float fRange )
{
	ioHashString szTarget;
	if( !pOwner )
		return szTarget;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return szTarget;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( fAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = fRange * fRange;
	float fCurGap = -FLOAT1;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);
		if( pTarget == pOwner )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;

		if( !pTarget->IsEnableTarget() ) 
			continue;

		if( pTarget->GetExperienceMode() != EMS_NONE )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}

		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}

	return szTarget;
}

void ioPappyDummy::ProcessTeleprotAttackState( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
{
	if ( !pOwner )
		return;

	if ( m_fCurMoveMaxRange > 0.0f && m_fCurMoveRange >= m_fCurMoveMaxRange )
	{
		SetReturnState( pOwner );
		return;
	}

	if ( m_fLimitReplicaAttackOwnerRange > 0 )
	{
		D3DXVECTOR3 vGap = pOwner->GetWorldPosition()-GetWorldPosition();
		float fLengesq = D3DXVec3LengthSq( &vGap );

		if ( m_fLimitReplicaAttackOwnerRange * m_fLimitReplicaAttackOwnerRange < fLengesq )
		{
			SetReturnState( pOwner );
			return;
		}
	}
	ProcessAttackState( pOwner, fTimePerSec, dwPreTime );
}

void ioPappyDummy::SetHideState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	m_PappyAttackState = PAS_HIDE;
	m_dwHideEndTime = FRAMEGETTIME() + m_dwHideDuration;
}

void ioPappyDummy::ProcessHideState( ioBaseChar *pOwner )
{
	if ( m_dwHideEndTime < FRAMEGETTIME() )
	{
		SetReturnState( pOwner );
		return;
	}
}

void ioPappyDummy::OnPoisonZoneDamage()
{
}

bool ioPappyDummy::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

bool ioPappyDummy::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}

bool ioPappyDummy::IsNeedCheckMapCollision() const
{
	return false;
}

void ioPappyDummy::DropZoneCheck()
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

void ioPappyDummy::ProcessAttackStartState( ioBaseChar *pOwner )
{
	if ( m_dwAttackStartAniEndTime <= FRAMEGETTIME() )
	{
		SetAttackState( pOwner );
		return;
	}
}

void ioPappyDummy::SetAttackStartState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_dwAttackStartAniEndTime = 0;
	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return;
	}

	int iSubType = iter->second.m_iSubUseType;

	switch( iSubType )
	{
		//러시 독
	case DSUT_EXTRA_ATTACK1:
		SetStartAni( m_szRushAttackStartAni, m_fRushAttackStartAniRate );
		break;
		//머신건 독
	case DSUT_EXTRA_ATTACK2:
		SetStartAni( m_szRush2AttackStartAni, m_fRush2AttackStartAniRate );
		break;
		//레프리카 독
	case DSUT_EXTRA_ATTACK3:
		SetStartAni( m_szTeleportAttackStartAni, m_fTeleportAttackStartAniRate );
		break;
		//오버 헤드 크러쉬
	case DSUT_EXTRA_ATTACK4:
		SetStartAni( m_szHideAttackStartAni, m_fHideAttackStartAniRate );
		break;
	}
	m_PappyState = PS_ATTACK;
	m_PappyAttackState = PAS_START;
}

void ioPappyDummy::SetLoopAni( const ioHashString& szName, const float& fRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 )
		return;
	float fAniRate = FLOAT1;
	if( fRate > 0.0f )
		fAniRate = fRate;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
}

void ioPappyDummy::SetStartAni( const ioHashString& szName, const float& fRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 )
		return;
	float fAniRate = FLOAT1;
	if( fRate > 0.0f )
		fAniRate = fRate;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );

	m_dwAttackStartAniEndTime = FRAMEGETTIME() + m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;
}

void ioPappyDummy::SetActionAni( const ioHashString& szName, const float& fRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 )
		return;
	float fAniRate = FLOAT1;
	if( fRate > 0.0f )
		fAniRate = fRate;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
}
