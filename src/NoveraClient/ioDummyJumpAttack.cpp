#include "StdAfx.h"
#include "ioDummyJumpAttack.h"
#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioZoneEffectWeapon.h"


ioDummyJumpAttack::ioDummyJumpAttack( ioEntityGroup *pGrp, ioPlayMode *pMode )
: ioDummyChar( pGrp, pMode )
{
	m_fFlyCurRange = 0;

	m_szTargetName.Clear();

	m_JumpAttackState = DJS_START;

	m_dwLandAttackAniEndTime = 0;
	m_iCurAttackCount = 0;
	m_dwMoveAttackStartTime = 0;
}

ioDummyJumpAttack::~ioDummyJumpAttack(void)
{
}

void ioDummyJumpAttack::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	ioDummyChar::LoadProperty( rkLoader );

	m_fStartMaxRange = rkLoader.LoadFloat_e( "start_fly_range", 0.0f );
	m_dwStartDuration = (DWORD)rkLoader.LoadInt( "start_fly_duration", 0 );
	m_fStartFlyAngle = rkLoader.LoadFloat_e( "start_fly_angle", 0.0f );

	m_iMoveAttackCode = rkLoader.LoadInt_e( "move_attack_code", 0 );
	m_iLandAttackCode = rkLoader.LoadInt_e( "land_attack_code", 0 );

	rkLoader.LoadString_e( "jump_animation", "", szBuf, MAX_PATH );
	m_JumpAni = szBuf;
	m_fJumpAniRate = rkLoader.LoadFloat_e( "jump_ani_rate", FLOAT1 );
	m_iMaxAttackCount = rkLoader.LoadInt_e( "max_attack_count", 0 );

	m_vMoveAttackOffset.x = rkLoader.LoadFloat_e( "move_attack_offset_x", 0.0f );
	m_vMoveAttackOffset.y = rkLoader.LoadFloat_e( "move_attack_offset_y", 0.0f );
	m_vMoveAttackOffset.z = rkLoader.LoadFloat_e( "move_attack_offset_z", 0.0f );

	m_vLandAttackOffset.x = rkLoader.LoadFloat_e( "land_attack_offset_x", 0.0f );
	m_vLandAttackOffset.y = rkLoader.LoadFloat_e( "land_attack_offset_y", 0.0f );
	m_vLandAttackOffset.z = rkLoader.LoadFloat_e( "land_attack_offset_z", 0.0f );

	m_dwMoveAttackTime = (DWORD)rkLoader.LoadInt_e( "move_attack_time", 0 );
}

bool ioDummyJumpAttack::ProcessJump( ioBaseChar *pOwner, float fTimerPerSec )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vTargetDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fStartFlyAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_fStartMaxRange / (float)m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimerPerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;
		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
		Translate( vMoveAmt );

	return bRangeLimit;
}

void ioDummyJumpAttack::CheckLive()
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCreateTime + m_dwLifeTime > dwCurTime )
		return;

	m_JumpAttackState = DJS_END;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DJS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyJumpAttack::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar* pOwner = GetOwner();
	if ( !pOwner )
		return;

	CheckLive();

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_JumpAttackState )
	{
	case DJS_START:
		{
			if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
			{	
				SetJumpState();
				return;
			}
		}
		break;
	case DJS_JUMP:
		{
			if( !ProcessJump( pOwner, fTimerPerSec ) )
			{
				SetSensingState();
				return;
			}
		}
		break;
	case DJS_SENSING:
		{
			SensingTarget( pOwner );
			if ( !m_szCurTargetName.IsEmpty() )
			{
				SetMoveAttackState();
				return;
			}
		}
		break;
	case DJS_MOVE_ATTACK:
		{
			if( !ProcessMoveAttack( pOwner, fTimerPerSec, dwPreTime ) )
			{
				SetLandAttack();
				return;
			}
			if ( m_dwMoveAttackTime > 0 && m_dwMoveAttackStartTime + m_dwMoveAttackTime < dwCurTime )
			{
				if ( m_iMaxAttackCount > 0 && m_iCurAttackCount >= m_iMaxAttackCount )
				{
					m_JumpAttackState = DJS_END;
					return;
				} 
				SetSensingState();
				return;
			}
			CheckExtraAniJump();
		}
		break;
	case DJS_LAND_ATTACK:
		{
			if( m_dwLandAttackAniEndTime == 0 || m_dwLandAttackAniEndTime < dwCurTime )
			{	
				if ( m_iMaxAttackCount > 0 && m_iCurAttackCount >= m_iMaxAttackCount )
				{
					m_JumpAttackState = DJS_END;
					return;
				}
				SetJumpState();
				return;
			}
			CheckAttackTime( pOwner, dwPreTime, m_vLandAttackOffset );
		}
		break;
	case DJS_END:
		SetDieState( true );
		break;
	}

}

void ioDummyJumpAttack::SetMoveAttackState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage ) return;
	
	if( !SetAttackAttribute( pOwner, m_iMoveAttackCode, m_dwAttackAniEndTime ) )
		return;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_szCurTargetName );
	if ( !pTarget )
		return;

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vPrePos;
	D3DXVec3Normalize( &vDiff, &vDiff );
	m_vTracingMoveDir = vDiff;
	m_JumpAttackState = DJS_MOVE_ATTACK;
	m_iCurAttackCount++;
	m_dwMoveAttackStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DJS_MOVE_ATTACK;
		kPacket << m_szCurTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioDummyJumpAttack::SetSensingState()
{
	int iAniID = m_pGroup->GetAnimationIdx( m_SensingAni );
	if ( iAniID == - 1 )
		return;

	m_pGroup->ClearAllLoopAni( FLOAT100 );
	m_pGroup->ClearAllActionAni( FLOAT100 );

	float fAniRate = FLOAT1;
	if( m_fSensingAniRate > 0.0f )
		fAniRate = m_fStartAniRate;

	m_JumpAttackState = DJS_SENSING;
	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
	m_szCurTargetName.Clear();
}

void ioDummyJumpAttack::SensingTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	if( !pOwner->IsNeedProcess() )
		return;

	ioBaseChar *pTargetChar = pOwner;
	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	TargetDistInfoList SensingTargetList;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )	continue;

		if( !pStage->IsCollisionCheckByMode() )
			continue;

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;


		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 	
			continue;


		if( CheckTargetWoundType( pTarget ) )
			continue;

		bool bPass = false;
		auto SkipBuffIter = m_vSkipBuffList.begin();
		for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
		{
			ioHashString& szBuffName = *SkipBuffIter;
			if ( pTarget->HasBuff( szBuffName ) )
			{				
				bPass = true;
				break;
			}
		}

		if( bPass )
			continue;

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		// 阿档 眉农
		float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
		fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
		float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );
		if( m_fSensingAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2;
			D3DXVec3Normalize( &vDiff2, &vDiff );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fHalfValue > fCurValue )
				continue;
		}

		//芭府 眉农
		float fEnableDistSq = 0.0f;
		fEnableDistSq = m_fSensingRange * m_fSensingRange;
		if( fDistSq > fEnableDistSq )	continue;

		TargetDistInfo	kInfo;
		kInfo.m_TargetName = pTarget->GetCharName();
		kInfo.m_fLenth = fDistSq;

		SensingTargetList.push_back(kInfo);
	}

	if( !SensingTargetList.empty() )
	{
		std::sort( SensingTargetList.begin(), SensingTargetList.end(), DistSort() );
		m_szCurTargetName = SensingTargetList[0].m_TargetName;
	}
}

bool ioDummyJumpAttack::ProcessMoveAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime )
{
	if ( !pOwner )
		return false;

	D3DXVECTOR3 vPos = GetWorldPosition();
	DWORD dwCurTime = FRAMEGETTIME();
	float fMoveValue = fTimePerSec * m_fTraceMoveSpeed;
	vPos += m_vTracingMoveDir * fMoveValue;
	float fPreHeight = GetBottomHeight();
	if( vPos.y < fPreHeight )
	{
		vPos.y = fPreHeight;
		return false;
	}

	SetWorldPosition( vPos );
	D3DXVECTOR3 vCurDir = m_vTracingMoveDir;
	vCurDir.y = 0.0f;
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCurDir );
	SetWorldOrientation( qtRot );

	CheckAttackTime( pOwner, dwPreTime, m_vMoveAttackOffset );

	return true;
}

void ioDummyJumpAttack::SetJumpState()
{
	int iAniID = m_pGroup->GetAnimationIdx( m_JumpAni );
	if ( iAniID == -1 )
		return;

	m_fFlyCurRange = 0;
	m_JumpAttackState = DJS_JUMP;

	m_pGroup->ClearAllActionAni( FLOAT10, true );
	m_pGroup->ClearAllLoopAni( FLOAT10, true );

	float fAniRate = FLOAT1;
	if( m_fJumpAniRate > 0.0f )
		fAniRate = m_fJumpAniRate;

	m_pGroup->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );
}

void ioDummyJumpAttack::SetLandAttack()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage ) return;

	if( !SetAttackAttribute( pOwner, m_iLandAttackCode, m_dwLandAttackAniEndTime ) )
		return;

	m_JumpAttackState = DJS_LAND_ATTACK;
}

void ioDummyJumpAttack::CheckAttackTime( ioBaseChar *pOwner, DWORD dwPreTime, D3DXVECTOR3 vOffset/* = ioMath::VEC3_ZERO */ )
{
	if ( !pOwner || m_vAttackFireTimeList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vCurPos = GetMidPositionByRate();
	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );
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
				if ( vOffset != ioMath::VEC3_ZERO )
				{
					D3DXVECTOR3 vDummyOffset = GetWorldOrientation() * vOffset;
					vCurPos += vDummyOffset;
				}
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
							pZone->SetCollisionDir( vCurDir );
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

bool ioDummyJumpAttack::SetAttackAttribute( ioBaseChar *pOwner, int iAttackCode, DWORD& dwEndTime )
{
	if ( !pOwner )
		return false;
	m_iReserveAttackCode = iAttackCode;
	DummyAttackMap::iterator iter = m_DummyAttackMap.find( m_iReserveAttackCode );
	if( iter == m_DummyAttackMap.end() )
	{
		m_iReserveAttackCode = 0;
		return false;
	}

	m_vAttackFireTimeList.clear();
	AttackAttribute &rkCurAttribute = iter->second.m_AttackAttribute;
	DWORD dwCurTime = FRAMEGETTIME();
	dwEndTime = dwCurTime;
	int iAniID = m_pGroup->GetAnimationIdx( rkCurAttribute.m_AttackAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );

		float fAniRate = rkCurAttribute.m_fAttackAniRate;
		if( fAniRate <= 0.0f )
			fAniRate = FLOAT1;

		m_pGroup->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fAniRate, 0.0f, 0.0f, true );
		dwEndTime += m_pGroup->GetAnimationFullTime( iAniID ) * fAniRate;

		// fire 贸府
		AniEventConstPtrList vFireStartList;
		m_pGroup->GetAnimationEventConstPtrList_e( iAniID, "fire_s", vFireStartList );

		std::list<float> vFireEndList;
		m_pGroup->GetAnimationEventTimeList_e( iAniID, "fire_e", vFireEndList );

		const WeaponInfoList &vInfoList = rkCurAttribute.m_vWeaponInfoList;
		if( vFireStartList.size() != vInfoList.size() )
			return false;

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

	return true;
}

void ioDummyJumpAttack::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DJS_MOVE_ATTACK:
		{
			rkPacket >> m_szCurTargetName;
			if ( !m_szCurTargetName.IsEmpty() )
				SetMoveAttackState();
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}
