

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTraceOwnerWeapon.h"

ActionStopType ioTraceOwnerWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioTraceOwnerWeapon::m_DieActionStopType = AST_NONE;

ioTraceOwnerWeapon::ioTraceOwnerWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_dwTraceEndTime = 0;
}

ioTraceOwnerWeapon::~ioTraceOwnerWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceOwnerWeapon::~ioTraceOwnerWeapon - UnReleased Resource" );
	}
}

void ioTraceOwnerWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	// Tracing
	m_dwTraceEnableTime = rkLoader.LoadInt_e( "trace_enable_time", 0 );

	m_fTraceSpeed = rkLoader.LoadFloat_e( "trace_speed", 0.0f );
	m_fTraceRange = rkLoader.LoadFloat_e( "trace_range", 0.0f );
	m_fTraceRotSpeed = rkLoader.LoadFloat_e( "trace_rotate_speed", 0.0f );
	m_dwTraceDuration = (DWORD)rkLoader.LoadInt_e( "trace_duration", 0 );
	m_fTraceEndRange = rkLoader.LoadFloat_e( "trace_end_range", 0.0f );

	m_fTargetTracePosRate = rkLoader.LoadFloat_e( "target_trace_pos_rate", FLOAT05 );
	m_fDownTargetTracePosRate = rkLoader.LoadFloat_e( "down_target_trace_pos_rate", 0.0f );

	m_bOnlyCharCollision	= rkLoader.LoadBool_e( "only_char_collision", false );

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

bool ioTraceOwnerWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioTraceOwnerWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

void ioTraceOwnerWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioTraceOwnerWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioTraceOwnerWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
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

void ioTraceOwnerWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간을 넘길 경우 Weapon을 소멸시킨다.
	if( m_dwCreatedTime + GetLiveTime() < FRAMEGETTIME() )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pFireBall )
			m_pFireBall->Update( fTimePerSec * FLOAT1000 );

		if( pStage )
			pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
		SetWeaponDead();
		return;
	}

	switch( m_State )
	{
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		OnTracing( pStage, fTimePerSec );
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

const ioOrientBox& ioTraceOwnerWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

bool ioTraceOwnerWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	ioBaseChar *pTarget = ToBaseChar( pEntity );
	if( !pTarget && m_bOnlyCharCollision )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}

DWORD ioTraceOwnerWeapon::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

ioWeapon::WeaponType ioTraceOwnerWeapon::GetType() const
{
	return WT_TRACE_OWNER;
}

ActionStopType ioTraceOwnerWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceOwnerWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioTraceOwnerWeapon::CreateFireBall( const ioHashString &szFileName )
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

float ioTraceOwnerWeapon::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceOwnerWeapon::CheckChangeDir()
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

void ioTraceOwnerWeapon::RotateMoveDir( float fAngleH, float fAngleV )
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
}

void ioTraceOwnerWeapon::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwTraceEnableTime < dwCurTime )
	{
		if( GetOwner() )
		{
			SetTraceState( pStage );
			return;
		}
	}

	CheckChangeDir();

	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceOwnerWeapon::OnTracing( ioPlayStage *pStage, float fTimePerSec )
{
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar *pOwner = GetOwner();

	bool bTracing = true;
	if( pOwner )
	{
		if( pOwner->IsSystemState() )
		{
			bTracing = false;
		}
		
		if( !pOwner->IsEnableTarget() )
		{
			bTracing = false;
		}
				
		if( bTracing && m_dwTraceEndTime < dwCurTime )
		{
			bTracing = false;
		}

		if( bTracing && m_fTraceEndRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vLength = vPos - GetPosition();
			float fLength = D3DXVec3Length( &vLength );
			if( fLength <= m_fTraceEndRange )
			{
				pStage->CreateMapEffect( GetWeaponEndEffect(), GetPosition(), GetScale() );
				SetWeaponDead();
				return;
			}
		}

		if( bTracing )
		{
			m_vPrePos = GetPosition();
			D3DXVECTOR3 vDiff = pOwner->GetMidPositionByRate() - m_vPrePos;

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

					float fPosRate = m_fTargetTracePosRate;
					if( CheckDownTargetState( pOwner ) )
						fPosRate = m_fDownTargetTracePosRate;

					vDiff = pOwner->GetMidPositionByRate(fPosRate) - GetPosition();
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
				}
			}
		}
	}

	float fMoveDist = m_fTraceSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceOwnerWeapon::SetTraceState( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	m_State = TS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;
}

bool ioTraceOwnerWeapon::CheckDownTargetState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() )
		return true;

	return false;
}