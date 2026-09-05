

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioTraceWeapon7.h"

#include "SkillDefine.h"
#include "ioNanginItem.h"

ActionStopType ioTraceWeapon7::m_ActionStopType = AST_NONE;
ActionStopType ioTraceWeapon7::m_DieActionStopType = AST_NONE;

ioTraceWeapon7::ioTraceWeapon7( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_MOVE;

	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_dwTraceEndTime = 0;

	m_bResistance = false;

	m_dwExplosionStart = 0;
}

ioTraceWeapon7::~ioTraceWeapon7()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceWeapon7::~ioTraceWeapon7 - UnReleased Resource" );
	}
}

void ioTraceWeapon7::LoadProperty( ioINILoader &rkLoader )
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

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	rkLoader.LoadString_e( "trace_map_effect", "", szBuf, MAX_PATH );
	m_szTraceMapEffect = szBuf;

	m_bNoChangeOwnerPos = rkLoader.LoadBool_e( "no_change_owner_position", false );
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

bool ioTraceWeapon7::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceWeapon7::CreateFireBall( const ioHashString &szFileName )
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

void ioTraceWeapon7::SetLiveTime( DWORD dwTime )
{
}

void ioTraceWeapon7::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioTraceWeapon7::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
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

void ioTraceWeapon7::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간을 넘길 경우 Weapon을 소멸시킨다.
	DWORD dwLive = GetLiveTime();
	if( m_dwCreatedTime + dwLive < FRAMEGETTIME() )
	{
		if( m_State != TS_EXPLOSION )
		{
			UpdateWorldAttackBox( fTimePerSec, false );

			if( m_pFireBall )
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );

			SetExplosionState( pStage );
			SetPositionOwner( GetPosition() );
			return;
		}
	}

	switch( m_State )
	{
	case TS_MOVE:
		OnMove( pStage, fTimePerSec );
		break;
	case TS_TRACING:
		OnTracing( pStage, fTimePerSec );
		break;
	case TS_EXPLOSION:
		OnExplosion();
		break;
	}

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioTraceWeapon7::OnMove( ioPlayStage *pStage, float fTimePerSec )
{
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime + m_dwTraceEnableTime < dwCurTime )
	{
		if( !m_TargetName.IsEmpty() )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
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

void ioTraceWeapon7::OnTracing( ioPlayStage *pStage, float fTimePerSec )
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

					float fPosRate = m_fTargetTracePosRate;
					if( CheckDownTargetState( pTarget ) )
						fPosRate = m_fDownTargetTracePosRate;

					vDiff = pTarget->GetMidPositionByRate(fPosRate) - GetPosition();
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

		if( !m_TargetName.IsEmpty() && m_fTraceEndRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();
			D3DXVECTOR3 vLength = vPos - GetPosition();
			float fLength = D3DXVec3Length( &vLength );
			if( fLength <= m_fTraceEndRange )
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

void ioTraceWeapon7::OnExplosion()
{
	if( FRAMEGETTIME() - m_dwExplosionStart > 100 )
	{
		SetWeaponDead();
	}
}

void ioTraceWeapon7::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
}

void ioTraceWeapon7::SetTraceState( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	m_State = TS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;

	if( !m_szTraceMapEffect.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
			D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
			vPos.y = pTarget->GetBottomHeight();
			pStage->CreateMapEffect( m_szTraceMapEffect, vPos, vScale );
		}
	}
}

void ioTraceWeapon7::SetExplosionState( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_State == TS_EXPLOSION )
		return;

	m_State = TS_EXPLOSION;

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

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

	m_dwExplosionStart = FRAMEGETTIME();
}

void ioTraceWeapon7::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceWeapon7::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceWeapon7::GetType() const
{
	return WT_TRACE7;
}

ActionStopType ioTraceWeapon7::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceWeapon7::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioTraceWeapon7::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	SetExplosionState( pStage );
	SetPositionOwner( vPos );
}

void ioTraceWeapon7::CheckPreCollision( ioPlayEntity *pEntity, ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION ) return;

	bool bCollision = false;
	ioBaseChar *pTarget = ToBaseChar(pEntity);
	if( pEntity->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( !m_bOnlyCharCollision &&
			( ToSymbolStruct(pEntity) || ToPushStruct(pEntity) || ToBallStruct(pEntity) ||
			  ToDummyChar(pEntity) || ToWallZone(pEntity) ) )
		{
			bCollision = true;
		}
		else if( pTarget && pTarget->IsNeedProcess() )
		{
			if( GetTeam() != pTarget->GetTeam() )
			{
				bCollision = true;
			}
			else if( GetOwner() != pTarget )
			{
				if( pTarget->IsEnableTeamCollision( this ) )
				{
					bCollision = true;
				}
			}
		}
	}

	if( bCollision )
	{
		SetExplosionState( pStage );
		SetPositionOwner( GetPosition() );

		if( pTarget && pTarget->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << GetPosition();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioTraceWeapon7::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
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
	{
		return true;
	}

	return false;
}

DWORD ioTraceWeapon7::GetCollisionEnableTime() const
{
	if( m_State != TS_MOVE )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioTraceWeapon7::CheckChangeDir()
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

void ioTraceWeapon7::RotateMoveDir( float fAngleH, float fAngleV )
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

float ioTraceWeapon7::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceWeapon7::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceWeapon7::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
}

void ioTraceWeapon7::SetMapCollision()
{
	if( m_bIgnoreMapCollision )
		return;

	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( pStage )
		SetExplosionState( pStage );
}

void ioTraceWeapon7::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioTraceWeapon7::CheckDownTargetState( ioBaseChar *pTarget )
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

void ioTraceWeapon7::SetPositionOwner( D3DXVECTOR3 vPos )
{
	if( m_bNoChangeOwnerPos )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioSkill *pSkill = pOwner->GetCurActiveSkill();
		ioTargetShotOwnerSkill *pTargetShotOwnerSkill = ToTargetShotOwnerSkill( pSkill );
		if( pTargetShotOwnerSkill )
		{
			pTargetShotOwnerSkill->SetPositionOwner( pOwner, vPos );
		}

		ioNanginItem* pNangin = ToNanginItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pNangin && pNangin->GetSpecialState() == ioNanginItem::SS_TRACE_ATTACK )
		{
			pNangin->SetPositionOwner( pOwner, vPos );
		}
	}
}