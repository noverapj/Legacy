

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioGroundTraceWeapon.h"

ActionStopType ioGroundTraceWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioGroundTraceWeapon::m_DieActionStopType = AST_NONE;

ioGroundTraceWeapon::ioGroundTraceWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER | WP_MINE_EXPLOSION | WP_WOUND_NO_SOUND | WP_DEF_NO_SOUND );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_dwExplosionStart = 0;
	m_dwExplosionTime = 0;
	m_fStartHeight = 0.0f;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;
	m_dwSensingStartTime = 0;

	m_dwTraceEndTime = 0;

	m_bResistance = false;

	m_iTest = 0;
}

ioGroundTraceWeapon::~ioGroundTraceWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioGroundTraceWeapon::~ioGroundTraceWeapon - UnReleased Resource" );
	}
}

void ioGroundTraceWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_bMapCheck = rkLoader.LoadBool_e( "trace_map_check", false );

	m_fEnableMoveHeight = rkLoader.LoadFloat_e( "enable_move_height", 0.0f );

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

bool ioGroundTraceWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioGroundTraceWeapon::CreateFireBall( const ioHashString &szFileName )
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

void ioGroundTraceWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioGroundTraceWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );
	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );
}

bool ioGroundTraceWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION )
		return true;

	return false;
}

void ioGroundTraceWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간 넘긴 경우 폭발
	if( m_dwCreatedTime + m_dwLiveTime < FRAMEGETTIME() )
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

void ioGroundTraceWeapon::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwSensingEnableTime < dwCurTime )
	{
		m_State = TS_SENSING;
		m_dwSensingStartTime = dwCurTime;
		return;
	}

	CheckChangeDir();

	// 이동처리
	D3DXVECTOR3 vPos = GetPosition();
	float fMoveDist = GetMoveSpeed() * fTimePerSec;

	ProcessMoving( pStage, fMoveDist, fTimePerSec );
}

void ioGroundTraceWeapon::OnSensing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !m_TargetName.IsEmpty() )
		m_TargetName.Clear();

	DWORD dwCurTime = FRAMEGETTIME();
	// Sensing 시간동안 타겟 잡지 못하면 이동만 한다
	if( m_dwSensingStartTime + m_dwSensingDuration < dwCurTime )
	{
		float fMoveDist = m_fTraceSpeed * fTimePerSec;
		ProcessMoving( pStage, fMoveDist, fTimePerSec );
		return;
	}

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	ProcessMoving( pStage, fMoveDist, fTimePerSec );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vCurPos = GetPosition();
	float fSensingRangeSq = m_fSensingRange * m_fSensingRange;

	float fSensingHalfAnigle = m_fSensingAngle * FLOAT05;
	fSensingHalfAnigle = ioMath::ArrangeHead(fSensingHalfAnigle);
	float fHalfValue = cosf( DEGtoRAD(fSensingHalfAnigle) );

	// Char Check
	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
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
			D3DXQUATERNION qtRot = GetOwner()->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			D3DXVECTOR3 vDiff2 = vTargetPos - GetOwner()->GetMidPositionByRate();
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

void ioGroundTraceWeapon::OnTracing( ioPlayStage *pStage, float fTimePerSec )
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
				m_vMoveDir = vDiff;
				m_vMoveDir.y = 0.0f;

				D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
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
	ProcessMoving( pStage, fMoveDist, fTimePerSec );
}

void ioGroundTraceWeapon::OnWaitExplosion( ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwExplosionTime < dwCurTime )
		SetExplosionState( pStage );
}

void ioGroundTraceWeapon::OnExplosion( float fTimePerSec )
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

void ioGroundTraceWeapon::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioGroundTraceWeapon::SetTraceState( ioPlayStage *pStage, bool bSendNetwork )
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

void ioGroundTraceWeapon::SetExplosionWaitState( ioPlayStage *pStage, bool bSendNetwork )
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

void ioGroundTraceWeapon::SetExplosionState( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION ) return;

	m_dwExplosionStart = FRAMEGETTIME();

	UpdateWorldAttackBox( 0.0f, true );

	if( !m_BoomEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_BoomEffect,
								 GetPosition(),
								 GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	m_State = TS_EXPLOSION;
}

bool ioGroundTraceWeapon::IsCanMineExplosion()
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

void ioGroundTraceWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioGroundTraceWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioGroundTraceWeapon::GetType() const
{
	return WT_GROUND_TRACE;
}

ActionStopType ioGroundTraceWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioGroundTraceWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioGroundTraceWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_State == TS_WAIT_EXPLOSION || m_State == TS_EXPLOSION ) return;

	bool bCollision = false;
	ioBaseChar *pChar = ToBaseChar(pTarget);
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( pChar && pChar->IsNeedProcess() )
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

	if( GetPiercingType() == CPT_NONE )
	{
		SetExplosionWaitState( pStage, true );
		return;
	}
}

bool ioGroundTraceWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

DWORD ioGroundTraceWeapon::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

bool ioGroundTraceWeapon::IsEnableTargetState( ioBaseChar *pChar )
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

void ioGroundTraceWeapon::CheckChangeDir()
{
	if( m_ChangeDirInfoList.empty() ) return;

	float fChangeRange = m_ChangeDirInfoList[0].m_fChangeRange;

	if( m_fCurMoveRange >= fChangeRange )
	{
		if( m_ChangeDirInfoList[0].m_fChangeSpeed > 0.0f )
			m_fCurMoveSpeed = m_ChangeDirInfoList[0].m_fChangeSpeed;

		RotateMoveDir( m_ChangeDirInfoList[0].m_fChangeAngleH, m_ChangeDirInfoList[0].m_fChangeAngleV );
		m_ChangeDirInfoList.erase( m_ChangeDirInfoList.begin() );
	}
}

void ioGroundTraceWeapon::RotateMoveDir( float fAngleH, float fAngleV )
{
	fAngleH = ioMath::ArrangeHead( fAngleH );
	fAngleV = ioMath::ArrangeHead( fAngleV );

	D3DXQUATERNION qtRot;

	// horizon
	if( fAngleH != 0.0f )
	{
		D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fAngleH );
	}

	// vertical
	if( fAngleV != 0.0f )
	{
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
		D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
		m_vMoveDir = qtRot * m_vMoveDir;

		GetParentSceneNode()->Rotate( vAxis, fAngleV );
	}

	/*
	D3DXVECTOR3 vNewDir = ioMath::VEC3_ZERO;
	vNewDir -= m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, vNewDir );
	SetOrientation( qtRot );
	*/
}

float ioGroundTraceWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioGroundTraceWeapon::ProcessMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec )
{
	D3DXVECTOR3 vPos = GetPosition();
	m_fCurMoveRange += fMoveAmt;

	m_vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	vPos += m_vMoveDir * fMoveAmt;

	float fMapHeight = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+m_pFireBall->GetBoundBox().GetExtents(1) );
	fMapHeight += m_fStartHeight;

	if( m_fEnableMoveHeight < fabs(vPos.y - fMapHeight) )
	{
		bool bTest = false;
		float fNewMapHeight = 0.0f;
		D3DXVECTOR3 vDiff = m_vPrePos;

		if( fabs(m_vMoveDir.x) >= fabs(m_vMoveDir.z) )
		{
			if( m_vMoveDir.x >= 0.0f )
				vDiff += D3DXVECTOR3( FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
			else
				vDiff += D3DXVECTOR3( -FLOAT1, 0.0f, 0.0f ) * fMoveAmt;
		}
		else
		{
			if( m_vMoveDir.z >= 0.0f )
				vDiff += D3DXVECTOR3( 0.0f, 0.0f, FLOAT1 ) * fMoveAmt;
			else
				vDiff += D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * fMoveAmt;
		}

		fNewMapHeight = pStage->GetMapHeightNoneEntity( vDiff.x, vDiff.z, vDiff.y+m_pFireBall->GetBoundBox().GetExtents(1) );
		fNewMapHeight += m_fStartHeight;

		if( m_fEnableMoveHeight < fabs(vDiff.y - fNewMapHeight) )
			return;

		vPos = vDiff;
		fMapHeight = fNewMapHeight;
	}

	vPos.y = fMapHeight;
	SetPosition( vPos );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );

	UpdateWorldAttackBox( fTimePerSec, false );
}

