

#include "stdafx.h"

#include "ioTraceWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioRocketlauncherItem.h"

ActionStopType ioTraceWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioTraceWeapon::m_DieActionStopType = AST_NONE;

ioTraceWeapon::ioTraceWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION | WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_dwExplosionStart = 0;
	m_dwExplosionTime = 0;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwSensingStartTime = 0;

	m_dwTraceEndTime = 0;

	m_bResistance = false;

	m_iTest = 0;

	m_bUseChangeDir = true;

	m_bLastWeapon = false;
}

ioTraceWeapon::~ioTraceWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceWeapon::~ioTraceWeapon - UnReleased Resource" );
	}
}

void ioTraceWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_bMapCheck = rkLoader.LoadBool_e( "trace_map_check", false );

	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );

	// Sensing
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );

	m_dwSensingEnableTime = rkLoader.LoadInt_e( "sensing_enable_time", 0 );
	m_dwSensingDuration = rkLoader.LoadInt_e( "sensing_duration", 0 );
	m_fSensingRange = rkLoader.LoadFloat_e( "sensing_range", 0.0f );
	m_fSensingAngle = rkLoader.LoadFloat_e( "sensing_angle", 0.0f );

	// Tracing
	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );

	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );

	// Explosion Wait
	m_dwExplosionWaitTime = rkLoader.LoadInt_e( "explosion_wait_time", 0 );

	// Explosion
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );

	//Offset
	m_fOffSet_X		= rkLoader.LoadFloat_e("weapon_offset_x", 0.0f);
	m_fOffSet_Y		= rkLoader.LoadFloat_e("weapon_offset_y", 0.0f);
	m_fOffSet_Z		= rkLoader.LoadFloat_e("weapon_offset_z", 0.0f);

	m_bSetForceTrace = rkLoader.LoadBool_e( "set_force_trace", false );
	m_dwTraceEnableTime = (DWORD)rkLoader.LoadInt_e( "trace_enable_time", 0 );
	m_OrientationState = (Orientation_State)rkLoader.LoadInt_e( "orientation_state", OS_NONE );
	m_bSetAttackMoveDirByTraceWeapon = rkLoader.LoadBool_e( "set_attack_move_dir_by_trace_weapon", false );

	// Move
	int iCnt = rkLoader.LoadInt_e( "change_dir_cnt", 0 );
	if( iCnt <= 0 ) return;

	m_ChangeDirInfoList.clear();
	m_ChangeDirInfoList.reserve( iCnt );

	for( int i=0; i < iCnt; ++i )
	{
		ChangeDirInfo kInfo;

		wsprintf_e( szBuf, "change_range%d", i+1 );
		kInfo.m_fChangeRange = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_speed%d", i+1 );
		kInfo.m_fChangeSpeed = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_h%d", i+1 );
		kInfo.m_fChangeAngleH = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "change_angle_v%d", i+1 );
		kInfo.m_fChangeAngleV = rkLoader.LoadFloat( szBuf, 0.0f );

		m_ChangeDirInfoList.push_back( kInfo );
	}

	std::sort( m_ChangeDirInfoList.begin(), m_ChangeDirInfoList.end(), ChangeDirSort() );
}

bool ioTraceWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioTraceWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioTraceWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	D3DXQUATERNION qtRot = ioMath::QUAT_IDENTITY;
	D3DXVECTOR3 vNewPos  = ioMath::VEC3_ZERO;
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		if( 0 < m_iDummyIndex )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
			if( pDummy )
			{				
				qtRot = pDummy->GetWorldOrientation();
			}
		}
		else if( GetOwner() )
		{
			qtRot = GetOwner()->GetTargetRot();
			m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
			SetOrientation( qtRot );
		}
		
		//Offset
		vNewPos.y = m_fStartHeight;
		vNewPos.x = m_fOffSet_X;
		vNewPos.z = m_fOffSet_Z;
		vNewPos.y += m_fOffSet_Y;
		vNewPos = qtRot * vNewPos;

		SetPosition( vPos + vNewPos );
		return;
	}

	if( 0 < m_iDummyIndex )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
		if( pDummy )
		{				
			qtRot = pDummy->GetWorldOrientation();
		}
	}
	else if( GetOwner() )
	{
		qtRot = GetOwner()->GetTargetRot();
		m_vMoveDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
		SetOrientation( qtRot );
	}

	//Offset
	vNewPos.y = m_fStartHeight;
	vNewPos.x = m_fOffSet_X;
	vNewPos.z = m_fOffSet_Z;
	vNewPos.y += m_fOffSet_Y;		
	vNewPos = qtRot * vNewPos;

	SetPosition( vPos + vNewPos );
}

bool ioTraceWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return true;

	bool bSend = false;
	if( !m_TargetName.IsEmpty() )
	{
		ioBaseChar *pOwner = pStage->GetOwnerChar();
		if( pOwner && pOwner->GetCharName() == m_TargetName )
			bSend = true;
	}

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetExplosionWaitState( pStage, bSend );
		return true;
	}

	return false;
}

void ioTraceWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간 넘긴 경우 폭발
	if( m_dwCreatedTime + GetLiveTime() < FRAMEGETTIME() )
	{
		if( m_State != TS_WAIT_EXPLOSION && m_State != TS_EXPLOSION )
		{
			UpdateWorldAttackBox( fTimePerSec, false );

			if( m_pFireBall )
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );

			SetExplosionWaitState( pStage, false );
			return;
		}
	}

	//
	switch( m_State )
	{
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
		break;
	case TS_SENSING:
		OnSensing( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		OnTracing( pStage, fTimePerSec );
		break;
	case TS_WAIT_EXPLOSION:
		OnWaitExplosion( pStage );
		break;
	case TS_EXPLOSION:
		OnExplosion( fTimePerSec );
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioTraceWeapon::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if ( !m_bSetForceTrace )
	{
		if( m_dwCreatedTime + m_dwSensingEnableTime < dwCurTime )
		{
			m_State = TS_SENSING;
			m_dwSensingStartTime = dwCurTime;
			return;
		}
	}
	else
	{
		if( m_dwCreatedTime + m_dwTraceEnableTime < dwCurTime )
		{
			m_State = TS_TRACING;
			m_dwTraceEndTime = dwCurTime + m_dwTraceDuration;
			return;
		}
	}


	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !m_TargetName.IsEmpty() )
		m_TargetName.Clear();	
	
	DWORD dwCurTime = FRAMEGETTIME();
	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		float fMoveDist = m_fTraceSpeed * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

		UpdateWorldAttackBox( fTimePerSec, false );
		return;
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf(fSensingHalfAnigle);

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar ) continue;

		if( pChar->GetTeam() == GetTeam() ) continue;
		if( pChar->GetCharName() == GetOwnerName() ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;
		if( IsExperienceOwner() ) continue;
		if( !IsEnableTargetState( pChar ) )
			continue;

		D3DXVECTOR3 vTargetPos = pChar->GetMidPositionByRate();
		vDiff = vCurPos - vTargetPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( m_fSensingAngle != 0.0f && GetOwner() )
		{
			/*
			D3DXQUATERNION qtRot = GetOwner()->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetOwner()->GetMidPositionByRate();
			D3DXVec3Normalize( &vDiff2, &vDiff2 );
			*/

			D3DXVECTOR3 vDir = m_vMoveDir;
			D3DXVECTOR3 vDiff2 = vTargetPos - GetPosition();
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

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget && pTarget->IsNeedProcess() )
		{
			m_TargetName = szTarget;
			SetTraceState( pStage, true );
		}
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon::OnTracing( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
	if( pTarget )
	{
		if( !m_TargetName.IsEmpty() && pTarget->IsSystemState() )
		{
			m_TargetName.Clear();
		}
		if( !m_TargetName.IsEmpty() && !pTarget->IsEnableTarget() )
		{
			m_TargetName.Clear();
		}

		if( !m_TargetName.IsEmpty() )
		{
			m_vPrePos = GetPosition();
			D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - m_vPrePos;

			float fRangeSq = D3DXVec3LengthSq( & vDiff );
			if( fRangeSq < m_fTraceRange * m_fTraceRange )
			{
				float fTimeGap = fTimePerSec * FLOAT1000;
				float fCurLoopTimeGap;
				float fCurLoopTimePerSec;
				while( fTimeGap > 0.0f )
				{
					if( fTimeGap > FLOAT10 )
					{
						fCurLoopTimeGap = FLOAT10;
						fCurLoopTimePerSec = FLOAT10 / FLOAT1000;

						fTimeGap -= FLOAT10;
					}
					else
					{
						fCurLoopTimeGap = fTimeGap;
						fCurLoopTimePerSec = fTimeGap / FLOAT1000;

						fTimeGap = 0.0f;
					}

					vDiff = pTarget->GetMidPositionByRate() - GetPosition();
					D3DXVec3Normalize( &vDiff, &vDiff );

					D3DXVECTOR3 vPreDir = m_vMoveDir;
					D3DXVECTOR3 vCurForce = m_vMoveDir * m_fTraceSpeed * fCurLoopTimePerSec;
					m_vMoveDir = vCurForce + (vDiff * m_fTraceRotSpeed * fCurLoopTimePerSec);

					D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

					float fCos = D3DXVec3Dot( &vPreDir, &m_vMoveDir );
					// 0도 일때
					if( fCos >= FLOAT1 )
					{
						// 처리 필요없음
					}
					else if( fCos <= -FLOAT1 ) // 180도 일때
					{
						D3DXVECTOR3 vAxis1, vAxis2;
						if( m_vMoveDir != ioMath::UNIT_Y )
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_Y );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}
						else
						{
							D3DXVec3Cross( &vAxis1, &m_vMoveDir, &ioMath::UNIT_X );
							D3DXVec3Normalize( &vAxis1, &-vAxis1 );
						}

						D3DXVec3Cross( &vAxis2, &m_vMoveDir, &vAxis1 );
						D3DXVec3Normalize( &vAxis2, &-vAxis2 );

						GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
					}
					else // 그외
					{
						float fGapAngle = RADtoDEG( acos( fCos ) );

						D3DXVECTOR3 vAxis;
						D3DXVec3Cross( &vAxis, &vDiff, &m_vMoveDir );
						D3DXVec3Normalize( &vAxis, &-vAxis );

						GetParentSceneNode()->Rotate( vAxis, fGapAngle );
					}

					m_iTest++;
				}
			}
			else
			{
				m_TargetName.Clear();
			}
		}

		if( !m_TargetName.IsEmpty() )
		{
			if( m_dwTraceEndTime < dwCurTime )
			{
				m_TargetName.Clear();
			}
		}
	}
	else
	{
		m_TargetName.Clear();
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon::OnWaitExplosion( ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwExplosionTime < dwCurTime )
		SetExplosionState( pStage );
}

void ioTraceWeapon::OnExplosion( float fTimePerSec )
{
	if( m_dwExplosionDuration == 0 )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwExplosionStart > m_dwExplosionDuration )
	{
		SetWeaponDead();
		return;
	}
}

void ioTraceWeapon::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioTraceWeapon::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return;

	m_State = TS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			kPacket << m_TargetName;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceWeapon::SetExplosionWaitState( ioPlayStage *pStage, bool bSendNetwork )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwExplosionTime = dwCurTime + m_dwExplosionWaitTime;
	m_State = TS_WAIT_EXPLOSION;

	if( bSendNetwork )
	{
		if( P2PNetwork::IsNetworkPlaying() && pStage->IsNetworkMode() )
		{
			SP2Packet kPacket( CUPK_TRACE_WEAPON_WAIT );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTraceWeapon::SetExplosionState( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION ) return;

	m_dwExplosionStart = FRAMEGETTIME();

	UpdateWorldAttackBox( 0.0f, true );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vScale = GetScale();

		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			vScale = D3DXVECTOR3( m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate, m_fOwnerChargeExplosionRate );
		}

		pStage->CreateMapEffect( m_BoomEffect,
								 GetPosition(),
								 vScale );

		ioWeapon::PlayExplosionSound();
	}

	m_State = TS_EXPLOSION;
}

bool ioTraceWeapon::IsCanMineExplosion()
{
	if( !IsLive() )	return false;
	if( !HasPeculiarity( WP_MINE_EXPLOSION ) )
		return false;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( m_State != TS_EXPLOSION )
		return false;

	return true;
}

void ioTraceWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceWeapon::GetType() const
{
	return WT_TRACE;
}

ActionStopType ioTraceWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioTraceWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION ) return;

	bool bCollision = false;
	ioBaseChar *pChar = ToBaseChar(pTarget);
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
			ToDummyChar(pTarget) || ToWallZone(pTarget) )
		{
			bCollision = true;
		}
		else if( pChar && pChar->IsNeedProcess() )
		{
			if( GetTeam() != pTarget->GetTeam() )
			{
				bCollision = true;
			}
			else if( GetOwner() != pTarget )
			{
				if( pChar )
				{
					if( pChar->IsEnableTeamCollision( this ) )
					{
						bCollision = true;
					}
				}
			}
		}
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		SetExplosionWaitState( pStage, true );
		return;
	}
}

bool ioTraceWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	float fCurExplosionRange = m_fExplosionRange;
	if( m_fOwnerChargeExplosionRate > FLOAT1 )
	{
		fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;
	}

	if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

DWORD ioTraceWeapon::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioTraceWeapon::IsEnableTargetState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( pChar->IsSystemState() )
		return false;

	if( !pChar->IsEnableTarget() )
		return false;

	if( !pChar->CheckTargetWoundType(m_TargetWoundType) )
		return false;

	return true;
}

void ioTraceWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) 
		return;

	if( !m_bUseChangeDir )
		return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioTraceWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXQUATERNION qtRot;

	// horizon
	if( fAngleH != 0.0f )
	{
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		m_vMoveDir = qtRot * m_vMoveDir;
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

		GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fAngleH );
	}

	// vertical
	if( fAngleV != 0.0f )
	{
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		m_vMoveDir = qtRot * m_vMoveDir;
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

		GetParentSceneNode()->Rotate( vAxis, fAngleV );
	}

	/*
	D3DXVECTOR3 vNewDir = ioMath::VEC3_ZERO;
	vNewDir -= m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, vNewDir );
	SetOrientation( qtRot );
	*/
}

float ioTraceWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceWeapon::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

void ioTraceWeapon::SetMoveDir( const D3DXVECTOR3 &vMoveDir )
{
	m_vMoveDir = vMoveDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	
	switch( m_OrientationState )
	{
	case OS_ORIENTATION:
		{
			D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( m_vMoveDir );
			SetOrientation( qtRot );
		}
		break;
	}	
}

D3DXVECTOR3 ioTraceWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if ( m_bSetAttackMoveDirByTraceWeapon )
		return m_vMoveDir;

	return ioWeapon::GetAttackDir( pEntity );
}


void ioTraceWeapon::SetUseChangeDir( bool bUse )
{
	m_bUseChangeDir = bUse;
}

void ioTraceWeapon::SetWeaponDead()
{
	// 로켓 런처의 공격 레벨이 웨폰 폭발 할때 1보다 작을 경우 마지막 공격 웨폰이라고 판단하여 타겟 마커 삭제
	ioRocketLauncherItem* pRocketLauncher1 = ToRocketLauncherItem( GetOwner()->GetWeapon() );
	if( pRocketLauncher1 && m_bLastWeapon )
	{
		if( GetOwner()->GetCharName() != g_MyInfo.GetPublicID() && P2PNetwork::IsNetworkPlaying() )			// 다른 유저의 캐릭터이면 상태 초기화 후 패킷 보냄 타겟 표시 지우라고...
			pRocketLauncher1->InitChargeState( GetOwner(), true, true );
	}

	ioWeapon::SetWeaponDead();
}