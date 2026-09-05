

#include "stdafx.h"

#include "ioTraceWeapon5.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioTraceWeapon5::m_ActionStopType = AST_NONE;
ActionStopType ioTraceWeapon5::m_DieActionStopType = AST_NONE;

ioTraceWeapon5::ioTraceWeapon5( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
	m_State = TS_ROTATE;
	
	m_fCurMoveSpeed = 0.0f;
	m_fCurMoveRange = 0.0f;

	m_dwTraceEndTime = 0;

	m_bResistance = false;
	m_vMoveDir = ioMath::VEC3_ZERO;
}

ioTraceWeapon5::~ioTraceWeapon5()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioTraceWeapon5::~ioTraceWeapon5 - UnReleased Resource" );
	}
}

void ioTraceWeapon5::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "fire_sound", "", szBuf, MAX_PATH );
	m_FireSound = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	// Rotate
	m_fRotateSpeed = rkLoader.LoadFloat_e( "rotate_speed", 0.0f );

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

	m_bSetDeadArriveTarget = rkLoader.LoadBool_e( "set_dead_arrive_target", false );

	m_bExplosionWeapon = rkLoader.LoadBool_e( "is_explosion_weapon", false );
	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
}

bool ioTraceWeapon5::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioTraceWeapon5::CreateFireBall( const ioHashString &szFileName )
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

void ioTraceWeapon5::SetLiveTime( DWORD dwTime )
{
}

void ioTraceWeapon5::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	if( m_vMoveDir != ioMath::VEC3_ZERO )
		return;

	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioTraceWeapon5::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
	{
		m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );
	D3DXVECTOR3 vDiff = vPos - GetOwner()->GetMidPositionByRate();
	m_fRotateRange = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );
	m_vMoveDir = vDiff;
	//m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	//SetOrientation( GetOwner()->GetTargetRot() );
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDiff );
	SetOrientation( qtRot );
}

void ioTraceWeapon5::Process( float fTimePerSec, ioPlayStage *pStage )
{
	// 생존시간을 넘길 경우 Weapon을 소멸시킨다.
	if( m_State != TS_ROTATE && m_dwCreatedTime + GetLiveTime() < FRAMEGETTIME() )
	{
		UpdateWorldAttackBox( fTimePerSec, false );

		if( m_pFireBall )
			m_pFireBall->Update( fTimePerSec * FLOAT1000 );

		SetWeaponDead();
		return;
	}

	switch( m_State )
	{
	case TS_ROTATE:
		OnRotate( pStage, fTimePerSec );
		if( GetOwner() && !GetOwner()->HasBuff( BT_CONTROL_WEAPON ) )
		{
			UpdateWorldAttackBox( fTimePerSec, false );

			if( m_pFireBall )
				m_pFireBall->Update( fTimePerSec * FLOAT1000 );

			SetWeaponDead();
			return;
		}
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

void ioTraceWeapon5::OnRotate( ioPlayStage *pStage, float fTimePerSec )
{
	if( !pStage )	return;
	if( m_fRotateSpeed <= 0.0f )	return;

	if( !m_TargetName.IsEmpty() )
		SetTraceState( pStage );
	
	DWORD dwCurTime = FRAMEGETTIME();

	// Rotate Weapon.
	float fRate = (fTimePerSec*1000) / m_fRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	/*if( m_bReverseLoopRotate )
		vAxis = -ioMath::UNIT_Y;*/
	ioMath::CalcDirectionQuaternion( qtNewRot, -m_vMoveDir );

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	m_vMoveDir = qtNewRot * m_vMoveDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXVECTOR3 vNewPos = GetOwner()->GetMidPositionByRate();
	vNewPos += m_vMoveDir * m_fRotateRange;

	SetPosition( vNewPos );
	GetParentSceneNode()->Rotate( ioMath::UNIT_Y, fRotateAngle );
	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioTraceWeapon5::OnTracing( ioPlayStage *pStage, float fTimePerSec )
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

void ioTraceWeapon5::SetTraceTarget( ioHashString &szName )
{
	m_TargetName = szName;
	m_dwCreatedTime = FRAMEGETTIME();

	/*if( GetOwner()->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );			
		kPacket << GetOwnerName();
		kPacket << m_iWeaponIdx;
		kPacket << szName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}

void ioTraceWeapon5::SetTraceState( ioPlayStage *pStage )
{
	m_State = TS_TRACING;
	m_dwTraceEndTime = FRAMEGETTIME() + m_dwTraceDuration;

	g_SoundMgr.PlaySound( m_FireSound, GetParentSceneNode() );
}

void ioTraceWeapon5::SetExplosionState( ioPlayStage *pStage, bool bSend )
{
	if( m_State == TS_EXPLOSION )
		return;

	m_State = TS_EXPLOSION;

	if( bSend )
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

void ioTraceWeapon5::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

const ioOrientBox& ioTraceWeapon5::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioTraceWeapon5::GetType() const
{
	return WT_TRACE5;
}

ActionStopType ioTraceWeapon5::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioTraceWeapon5::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioTraceWeapon5::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( !m_bExplosionWeapon )
		return;

	if( m_State == TS_ROTATE )
		return;

	if( m_State == TS_EXPLOSION )
		return;

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
		SetExplosionState( pStage, true );
		return;
	}
}

bool ioTraceWeapon5::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State == TS_ROTATE )
		return false;
	
	bool bCollision = false;

	if( m_bExplosionWeapon )
	{
		if( m_State != TS_EXPLOSION )
			return false;

		if( m_bOnlyCharCollision )
		{
			ioBaseChar *pTarget = ToBaseChar( pEntity );
			if( !pTarget )
				return false;
		}

		float fTargetDist = -FLOAT1;
		D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

		float fCurExplosionRange = m_fExplosionRange;
		if( m_fOwnerChargeExplosionRate > FLOAT1 )
		{
			fCurExplosionRange = m_fExplosionRange * m_fOwnerChargeExplosionRate;
		}

		if( !pEntity->CheckSphereDistance( vCenter, fCurExplosionRange, fTargetDist ) )
			return false;

		if( COMPARE( fTargetDist, 0.0f, fCurExplosionRange ) )
		{
			return true;
		}
	}
	else
	{
		if( m_bOnlyCharCollision )
		{
			ioBaseChar *pTarget = ToBaseChar( pEntity );
			if( !pTarget )
				return false;
		}

		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
			bCollision = true;

		CheckPiercing( bCollision );

		//초능력자때 추가(타겟된 대상에 도착했으면 무기 삭제)
		if ( m_bSetDeadArriveTarget )
		{
			ioBaseChar *pTarget = ToBaseChar( pEntity );
			//충돌되었으면서 타겟 이름 확인
			if ( bCollision && pTarget && pTarget->GetCharName() == m_TargetName )
			{
				SetWeaponDead();
			}
		}
	}
	
	return bCollision;
}

float ioTraceWeapon5::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed;
}

void ioTraceWeapon5::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
}

void ioTraceWeapon5::SetOwnerChargeScaleRate( float fRate )
{
	m_fOwnerChargeScaleRate = fRate;

	if( m_fOwnerChargeScaleRate > FLOAT1 )
	{
		D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
		GetParentSceneNode()->SetScale( vScale );
	}
}

void ioTraceWeapon5::SetWeaponDead()
{
	if( m_State == TS_ROTATE && GetOwner() && GetOwner()->GetCreator() )
	{
		GetOwner()->GetCreator()->CreateMapEffect( GetExplosionEffectName(), GetWorldAttackBox().GetCenter(), GetScale() );
		PlayExplosionSound();
	}

	StopLoopSound();
	m_bLive = false;
}

void ioTraceWeapon5::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioTraceWeapon5::CheckDownTargetState( ioBaseChar *pTarget )
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

bool ioTraceWeapon5::IsIgnoreMapCollision() const
{
	if( m_State == TS_ROTATE )
		return true;

	return false;
}

bool ioTraceWeapon5::IsIgnoreTestMapCollision() const
{
	if( m_State == TS_ROTATE )
		return true;

	return false;
}