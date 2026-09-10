#include "StdAfx.h"
#include "ioCircleTraceDummy.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioCircleTraceDummy::ioCircleTraceDummy( ioEntityGroup *pGrp, ioPlayMode *pMode ):
ioDummyChar( pGrp, pMode )
{
	m_CircleTraceDummyState = CTDS_START;
	m_CirclePositionList.clear();
	m_WeaponFiredTargetList.clear();
	m_vTargetCirclePos = ioMath::VEC3_ZERO;
	m_vTargetCircleDir = ioMath::VEC3_ZERO;
	m_fCurMoveRange = 0.0f;
	m_fMaxMoveRange = 0.0f;

	m_dwCircleLastPositionEffectID = -1;
}

ioCircleTraceDummy::~ioCircleTraceDummy(void)
{
}

void ioCircleTraceDummy::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	ioDummyChar::LoadProperty( rkLoader );

	m_dwAttackTicTime = (DWORD)rkLoader.LoadInt_e( "attack_tic_time", 0 );
	m_DummyAttack.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_type", NO_WEAPON_ATTRIBUTE );
	m_DummyAttack.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "attack_type_wounded", "", szBuf, MAX_PATH );
	m_DummyAttack.m_WoundedAnimation = szBuf;
	m_DummyAttack.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_type_wounded_time", 0 );
	m_DummyAttack.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_type_wounded_loop_ani", false );
	m_fTargetCircleHeightOffset = rkLoader.LoadFloat_e( "target_circle_height_offset", 0.0f );

	m_DummyAttackOffset.x = rkLoader.LoadFloat_e( "attack_type_offset_x", 0.0f );
	m_DummyAttackOffset.y = rkLoader.LoadFloat_e( "attack_type_offset_y", 0.0f );
	m_DummyAttackOffset.z = rkLoader.LoadFloat_e( "attack_type_offset_z", 0.0f );

	m_fCircleTraceSpeed = rkLoader.LoadFloat_e( "circle_trace_speed", 0.0f );

	rkLoader.LoadString_e( "circle_last_position_effect", "", szBuf, MAX_PATH );
	m_szCircleLastPositionEffect = szBuf;

	m_fCircleLastEffectOffsetX = rkLoader.LoadFloat_e( "circle_last_position_effect_offset_x", 0.0f );
	m_fCircleLastEffectOffsetY = rkLoader.LoadFloat_e( "circle_last_position_effect_offset_y", 0.0f );
}

void ioCircleTraceDummy::AddCirclePosition( D3DXVECTOR3 vPos )
{
	if ( vPos == ioMath::VEC3_ZERO )
		return;

	vPos.y += m_fTargetCircleHeightOffset;

	m_CirclePositionList.push_back( vPos );
}

void ioCircleTraceDummy::ProcessState( float fTimerPerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
	{
		SetDieState( true );
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
	{
		SetDieState( true );
		return;
	}

	switch( m_CircleTraceDummyState )
	{
	case CTDS_START:
		ProcessStart( pOwner, pStage );
		break;
	case CTDS_MOVE:
		ProcessMoveState( pOwner, pStage, fTimerPerSec );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );
}

void ioCircleTraceDummy::ProcessStart( ioBaseChar *pOwner, ioPlayStage *pStage )
{
	if ( !pOwner || !pStage )
		return;

	if( m_CirclePositionList.empty() )
	{
		SetDieState( true );
		return;
	}

	if( !m_bMoveEffectCreated && !m_DummyEffect.IsEmpty() )
	{
		m_pGroup->AttachEffect( m_DummyEffect, NULL );
		m_bMoveEffectCreated = true;
	}

	if ( !m_szCircleLastPositionEffect.IsEmpty() && m_dwCircleLastPositionEffectID == -1 )
	{
		D3DXVECTOR3 vLastCirclePos = m_CirclePositionList.back();
		vLastCirclePos.y -= m_fTargetCircleHeightOffset;
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

		vLastCirclePos.x += m_fCircleLastEffectOffsetX;
		vLastCirclePos.y += m_fCircleLastEffectOffsetY;

		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szCircleLastPositionEffect, vLastCirclePos, vScale );
		if( pMapEffect )
			m_dwCircleLastPositionEffectID = pMapEffect->GetUniqueID();
	}

	SetState( CTDS_MOVE );
}

void ioCircleTraceDummy::SetState( CircleTraceDummyState eState )
{
	m_CircleTraceDummyState = eState;

	switch( eState )
	{
	case CTDS_MOVE:
		SetNextTargetCircle( false );
		break;
	}
}

void ioCircleTraceDummy::ProcessMoveState( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimerPerSec )
{
	float fMoveDist = m_fCircleTraceSpeed * fTimerPerSec;

	if ( pOwner->IsNeedProcess() )
	{
		m_fCurMoveRange += fMoveDist;
		if( m_fCurMoveRange > m_fMaxMoveRange )
		{
			if ( m_CirclePositionList.empty() )
			{
				SendDieState( true );
				return;
			}
			else
				SetNextTargetCircle( true );
		}
	}

	D3DXVECTOR3 vPos = GetWorldPosition();

	vPos += m_vTargetCircleDir * fMoveDist;

	SetWorldPosition( vPos );

	ProcessSensingAttack( pOwner, pStage, fTimerPerSec );
}

void ioCircleTraceDummy::SetNextTargetCircle( bool bSend )
{
	if ( m_CirclePositionList.empty() )
		return;

	m_fCurMoveRange = 0.0f;
	m_vTargetCirclePos = m_CirclePositionList.front();
	m_CirclePositionList.erase( m_CirclePositionList.begin() );

	D3DXVECTOR3 vDiff = m_vTargetCirclePos - GetWorldPosition();
	m_fMaxMoveRange = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );
	m_vTargetCircleDir = vDiff;
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
	SetWorldOrientation( qtRot );

	if( bSend && GetOwner() && GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DUMMY_CHAR_SYNC );
		kPacket << m_OwnerName;
		kPacket << m_iCreateIndex;
		kPacket << DSS_CHANGE_TARGET_CIRCLE;
		kPacket << GetWorldPosition();
		kPacket << GetWorldOrientation();
		kPacket << m_vTargetCirclePos;
		kPacket << m_vTargetCircleDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleTraceDummy::ProcessSensingAttack( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimerPerSec )
{
	if ( !pOwner || !pStage )
		return;

	ioHashStringVec vTargetList;
	SearchTarget( pOwner, pStage, vTargetList );
	CheckTargetWeaponFire( pOwner, pStage, vTargetList );
}

void ioCircleTraceDummy::SearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashStringVec& vTargetList )
{
	if ( !pOwner || !pStage )
		return;

	vTargetList.clear();

	float fSensingRangeSq = 0.0f;
	fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	if( fSensingRangeSq == 0.0f )
		return;

	WeaponFiredTargetList::iterator Targetiter = m_WeaponFiredTargetList.begin();
	for ( ; Targetiter != m_WeaponFiredTargetList.end() ; )
	{
		if( (*Targetiter).dwFireTime + m_dwAttackTicTime < FRAMEGETTIME() )
			Targetiter = m_WeaponFiredTargetList.erase( Targetiter );
		else
			Targetiter++;
	}

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;

		if( !pChar ) 
			continue;

		if ( CheckTargetState( pOwner, pChar ) )
			continue;
		
		if ( CheckSkipBuff( pChar ) )
			continue;

		bool bPass = false;
		for ( int i=0 ; i<(int)m_WeaponFiredTargetList.size() ; ++i )
		{
			//대상이 이미 존재할 경우
			if ( m_WeaponFiredTargetList[i].szTargetName == pChar->GetCharName() )
			{
				bPass = true;
				break;
			}
		}

		if ( bPass )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition();
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
		
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );
		if( fDiffRangeSq >= fSensingRangeSq )
			continue;

		vTargetList.push_back( pChar->GetCharName() );
	}
}

bool ioCircleTraceDummy::CheckTargetState( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if ( !pOwner || !pTarget )
		return true;
	if( pTarget->GetTeam() == pOwner->GetTeam() ) 
		return true;
	if( pTarget->GetCharName() == GetOwnerName() ) 
		return true;
	if( pTarget->GetExperienceMode() == EMS_EXPERIENCE ) 
		return true;
	if( !IsEnableTargetState( pTarget ) )
		return true;

	if( pTarget->IsCanNoWound() )
		return true;

	if( m_bCheckWounedState && !IsTargetWoundedState( pTarget ) )
		return true;

	return false;
}

bool ioCircleTraceDummy::CheckSkipBuff( ioBaseChar *pChar )
{
	if ( !pChar )
		return false;
	auto SkipBuffIter = m_vSkipBuffList.begin();
	for ( ; SkipBuffIter != m_vSkipBuffList.end(); ++SkipBuffIter )
	{
		ioHashString& szBuffName = *SkipBuffIter;
		if ( pChar->HasBuff( szBuffName ) )
		{
			return true;
		}
	}
	return false;
}

void ioCircleTraceDummy::CheckTargetWeaponFire( ioBaseChar *pOwner, ioPlayStage *pStage, const ioHashStringVec& vTargetList )
{
	if ( !pOwner )
		return;

	for ( int i=0; i<(int)vTargetList.size() ; ++i )
	{
		WeaponFiredTarget WeaponTarget;
		WeaponTarget.dwFireTime = FRAMEGETTIME();;
		WeaponTarget.szTargetName = vTargetList[i];
		m_WeaponFiredTargetList.push_back( WeaponTarget );
		//공격 발사
		CreateWeapon( pOwner, pStage, vTargetList[i] );
	}
}

void ioCircleTraceDummy::CreateWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioHashString szTargetName )
{
	if ( !pOwner || !pStage || szTargetName.IsEmpty() )
		return;
	if ( m_DummyAttack.m_iWeaponIdx == NO_WEAPON_ATTRIBUTE )
		return;

	ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );
	if ( !pTarget )
		return;

	DWORD dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.iAttributeIdx = m_DummyAttack.m_iWeaponIdx;
	kFireTime.iResistanceIdx = m_DummyAttack.m_iResistanceIdx;
	kFireTime.szWoundedAni  = m_DummyAttack.m_WoundedAnimation;
	kFireTime.dwWoundedAniDuration = m_DummyAttack.m_dwWoundedDuration;
	kFireTime.bLoopWoundedAni = m_DummyAttack.m_bWoundedLoopAni;
	kFireTime.dwWeaponIdx = dwWeaponBaseIndex++;
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;
	D3DXVECTOR3 vCurDir = pTarget->GetWorldPosition() - GetWorldPosition();
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vPosOffset = GetWorldOrientation() * m_DummyAttackOffset;
	D3DXVECTOR3 vPos = GetWorldPosition() + vPosOffset;

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vCurDir, "" );
}

void ioCircleTraceDummy::ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pStage ) 
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case DSS_CHANGE_TARGET_CIRCLE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			SetWorldPosition( vPos );
			SetWorldOrientation( qtRot );

			rkPacket >> m_vTargetCirclePos;
			rkPacket >> m_vTargetCircleDir;
		}
		break;
	default:
		ioDummyChar::ApplyDummyCharSyncByType( iType, rkPacket, pStage );
	}
}

void ioCircleTraceDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );

	ioBaseChar *pOwner = GetOwner();
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	if ( m_dwCircleLastPositionEffectID != - 1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwCircleLastPositionEffectID );
		if( pMapEffect )
		{
			pMapEffect->EndEffectForce();
			m_dwCircleLastPositionEffectID = -1;
		}
	}
}
