#include "StdAfx.h"
#include "ioTargetTrace17Dummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioZoneEffectWeapon.h"

ioTargetTrace17Dummy::ioTargetTrace17Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	Init();
}


ioTargetTrace17Dummy::~ioTargetTrace17Dummy(void)
{
}

void ioTargetTrace17Dummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
		//, szKey[MAX_PATH];
	ioDummyChar::LoadProperty( rkLoader );

	//Pre
	rkLoader.LoadString_e( "pre_delay_animation", "", szBuf, MAX_PATH );
	m_PreDelayAnimation = szBuf;
	m_fPreDelayAniRate = rkLoader.LoadFloat_e( "pre_delay_ani_rate", FLOAT1 );
	m_dwPreDelayDuration = (DWORD)rkLoader.LoadInt_e( "pre_delay_duration", 0 );

	//Delay
	m_dwDelayDurationTime = (DWORD)rkLoader.LoadInt_e( "delay_duration", 0 );

	//Move
	m_fDummyMoveSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_dwMoveDuration = (DWORD)rkLoader.LoadInt_e( "move_duration", 0 );
	m_fMaxMoveRange = rkLoader.LoadFloat_e( "max_move_range", 0.0f );

	//Attack
	m_dwAttackCode = (DWORD)rkLoader.LoadInt_e( "attack_code", 0 );
	m_iMaxMoveCount = rkLoader.LoadInt_e( "max_attack_count", 0 );

	m_vTargetBuffList.clear();
	int iSize = rkLoader.LoadInt_e( "max_target_buff_count", 0 );
	for( int i=0; i<iSize; ++i )
	{
		ioHashString szBuffName;
		wsprintf_e( szKey, "target_buff_name%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szBuffName = szBuf;
		if ( !szBuffName.IsEmpty() )
			m_vTargetBuffList.push_back( szBuffName );
	}
}

void ioTargetTrace17Dummy::Init()
{
	m_dwStateEndTime = 0;
	m_fCurMoveRange = 0;
	m_vMoveDir = ioMath::VEC3_ZERO;
	m_TraceDummyState = TDS_START;
	m_iCurMoveCount = 0;
}

void ioTargetTrace17Dummy::SetLoopAni( ioBaseChar *pOwner, const ioHashString& szName, float fAniRate )
{
	if ( !pOwner || szName.IsEmpty() )
		return;

	int iAniID = m_pGroup->GetAnimationIdx( szName );
	if( iAniID == -1 )
		return;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
}

void ioTargetTrace17Dummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
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
		if ( m_TraceDummyState != TDS_ATTACK )
		{
			SendDieState( true );
			return;
		}
	}

	switch( m_TraceDummyState )
	{
	case TDS_START:
		ProcessStartState( pOwner );
		break;
	case TDS_PRE_DELAY:
		ProcessPreDelayState( pOwner );
		break;
	case TDS_MOVE:
		ProcessMoveState( pOwner );
		break;
	case TDS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimerPerSec, dwPreTime );
		break;
	case TDS_DELAY:
		ProcessDelayState( pOwner );
		break;
	}

	ProcessPos( fTimerPerSec );
}

void ioTargetTrace17Dummy::ProcessPos( float fTimePerSec )
{
	switch( m_TraceDummyState )
	{
	case TDS_MOVE:
		{
			ioBaseChar *pOwner = GetOwner();
			if ( !pOwner )
				return;
			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;
			if ( !CheckMove( pOwner, pStage ) )
			{
				if ( pOwner->IsNeedProcess() )
				{
					SendDieState( true );
					return;
				}
			}
		}
		break;
	}
}

void ioTargetTrace17Dummy::ProcessStartState( ioBaseChar *pOwner )
{
	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetPreDelayState( pOwner );
		return;
	}
}

void ioTargetTrace17Dummy::SetPreDelayState( ioBaseChar *pOwner )
{
	m_iReserveAttackCode = 0;
	m_dwStateEndTime = m_dwPreDelayDuration + FRAMEGETTIME();
	m_TraceDummyState = TDS_PRE_DELAY;
	SetLoopAni( pOwner, m_PreDelayAnimation, m_fPreDelayAniRate );
}

void ioTargetTrace17Dummy::ProcessPreDelayState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if( m_dwStateEndTime > 0 && m_dwStateEndTime < FRAMEGETTIME() )
	{
		m_dwStateEndTime = 0;
		ioPlayStage *pStage = pOwner->GetCreator();
		ioHashString szTarget = GetTraceTarget( pStage, false );
		SetMoveState( pOwner, szTarget, true );
	}
}

void ioTargetTrace17Dummy::SetMoveState( ioBaseChar *pOwner, const ioHashString& szTarget, bool bUseNoneTargetMove )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
		if ( !pTarget && !bUseNoneTargetMove )
		{
			SendDieState( true );
			return;
		}
		m_vMoveDir = GetWorldOrientation()  * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		if ( pTarget )
		{
			m_vMoveDir = pTarget->GetMidPositionByRate() - GetWorldPosition();	
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

			D3DXVECTOR3 vXZDir( m_vMoveDir.x, 0.0f, m_vMoveDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );
			D3DXQUATERNION qtRot;
			ioMath::CalcDirectionQuaternion( qtRot, -vXZDir );
			SetWorldOrientation( qtRot );
		}
	}

	m_iReserveAttackCode = 0;
	m_fCurMoveRange = 0;
	m_dwStateEndTime = m_dwMoveDuration + FRAMEGETTIME();
	m_TraceDummyState = TDS_MOVE;
	SetLoopAni( pOwner, m_MoveAnimation, m_fMoveAniRate );

	DestroyAttachWeaponIndexList( pOwner );
	SetAttachWeapon();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_MOVE;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		kPacket << szTarget;
		kPacket << bUseNoneTargetMove;
		kPacket << m_vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurMoveCount++;
}

void ioTargetTrace17Dummy::ProcessMoveState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	//矫埃眉农
	if( m_dwStateEndTime > 0 && m_dwStateEndTime < FRAMEGETTIME() )
	{
		m_dwStateEndTime = 0;
		SetDelayState( pOwner );
		return;
	}

	//芭府 眉农
	if ( m_fCurMoveRange >= m_fMaxMoveRange )
	{
		SetDelayState( pOwner );
		return;
	}
}

bool ioTargetTrace17Dummy::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage ) 
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fDummyMoveSpeed > 0.0f && m_vMoveDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fDummyMoveSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = m_vMoveDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveRange += fMoveDist;
		}
		if ( bCol )
			return false;
	}

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	pStage->CheckAxisLimit( &vCurPos );
	SetWorldPosition( vCurPos );
	return true;
}

void ioTargetTrace17Dummy::CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if ( m_TraceDummyState != TDS_MOVE )
		return;

	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if ( !pTarget || pTarget->GetTeam() == GetTeam() )
		return;

	auto SkipBuffIter = m_vSkipBuffList.begin();
	for ( int i=0; i < (int)m_vSkipBuffList.size() ; ++i )
	{
		ioHashString& szBuffName = m_vSkipBuffList[i];
		if ( pTarget->HasBuff( pOwner->GetCharName(), szBuffName ) )
			return;
	}

	SetAttackState( pOwner, pTarget->GetCharName() );
}

void ioTargetTrace17Dummy::SetDelayState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	m_TraceDummyState = TDS_DELAY;
	m_iReserveAttackCode = 0;
	m_dwExtraAniJumpTime = 0;
	m_fExtraAniJumpPower = 0.0f;
	m_iCurExtraAniJump = 0;
	m_vExtraAniJumpList.clear();
	m_dwStateEndTime = m_dwDelayDurationTime + FRAMEGETTIME();
	SetLoopAni( pOwner, m_DelayAnimation, m_fDelayAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_DELAY;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace17Dummy::DestroyAttachWeaponIndexList( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	if ( m_vAttachWeaponIndexList.empty() )
		return;

	int iCnt = m_vAttachWeaponIndexList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_vAttachWeaponIndexList[i] );
		if( pWeapon )
			pWeapon->SetWeaponDead();
	}

	m_vAttachWeaponIndexList.clear();
}

void ioTargetTrace17Dummy::SetAttackState( ioBaseChar *pOwner, ioHashString szTargetName )
{
	if( !pOwner )
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
	if ( !pTarget )
		return;

	m_TraceDummyState = TDS_ATTACK;

	m_iReserveAttackCode = m_dwAttackCode;
	SetAttackAttribute( pOwner );
	AddTargetBuff( pOwner, pTarget );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DCST_ATTACK;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		kPacket << szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTrace17Dummy::SetAttackAttribute( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

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

		// fire 贸府
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
		// 扁鸥 贸府

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
		//
	}

	DestroyAttachWeaponIndexList( pOwner );
}

void ioTargetTrace17Dummy::AddTargetBuff( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if ( !pOwner || !pTarget || m_vTargetBuffList.empty() )
		return;

	const ioHashString& szCharName = pOwner->GetCharName();
	for ( int i=0 ; i<(int)m_vTargetBuffList.size() ; ++i )
		pTarget->AddNewBuff( m_vTargetBuffList[i], szCharName, "", NULL );
}

void ioTargetTrace17Dummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
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
				ioWeapon *pWeapon = NULL;
				if( m_DummyCharType == DCT_OWNER_ATTACH3 )
					pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, m_vAttackDir, "" );
				else
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
				}
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
			++iter;
	}

	//
	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime < dwCurTime )
	{
		SetDelayState( pOwner );
		return;
	}
}

void ioTargetTrace17Dummy::ProcessDelayState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	if( m_dwStateEndTime > 0 && m_dwStateEndTime < FRAMEGETTIME() )
	{
		m_dwStateEndTime = 0;

		if ( m_iMaxMoveCount <= m_iCurMoveCount )
		{
			SendDieState( true );
			return;
		}
		else
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			ioHashString szTarget = GetTraceTarget( pStage, false );
			SetMoveState( pOwner, szTarget, false );
			return;
		}
	}
}

void ioTargetTrace17Dummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner || !pStage )
		return;
	int iType;
	rkPacket >> iType;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	SetWorldPosition( vPos );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	SetWorldOrientation( qtRot );

	switch( iType )
	{
	case DCST_MOVE:
		{
			ioHashString szTarget;
			rkPacket >> szTarget;
			bool bNoneTargetMove;
			rkPacket >> bNoneTargetMove;
			rkPacket >> m_vMoveDir;
			SetMoveState( pOwner, szTarget, bNoneTargetMove );
		}
		break;
	case DCST_ATTACK:
		{
			ioHashString szTarget;
			rkPacket >> szTarget;
			SetAttackState( pOwner, szTarget );
		}
		break;
	case DCST_DELAY:
		SetDelayState( pOwner );
		break;
	default:
		ApplyDummyCharSyncByType( iType, rkPacket, pStage );
		break;
	}
}
