

#include "stdafx.h"

#include "ioGraffitiWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioGraffitiWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioGraffitiWeapon::m_DieActionStopType = AST_NONE;

ioGraffitiWeapon::ioGraffitiWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;

	m_EffectID = -1;

	m_dwExplosionStart = 0;

	m_State = TS_NONE;
	m_bResistance = false;
}

ioGraffitiWeapon::~ioGraffitiWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioGraffitiWeapon::~ioGraffitiWeapon - UnReleased Resource" );
	}
}

bool ioGraffitiWeapon::InitResource()
{
	return true;
}

void ioGraffitiWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	rkLoader.LoadString_e( "red_team_effect", "", szBuf, MAX_PATH );
	m_RedTeamEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_effect", "", szBuf, MAX_PATH );
	m_BlueTeamEffect = szBuf;

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_dwExplosionDuration = (DWORD)rkLoader.LoadInt_e( "explosion_duration", 0 );

	m_TargetColType		= (TargetColType)rkLoader.LoadInt_e( "collision_type", 0 );
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "wound_type", 0 );

	m_fZoneRange		= rkLoader.LoadFloat_e( "collision_range", 0.0f );
	m_fZoneMinRange		= rkLoader.LoadFloat_e( "collision_min_range", 0.0f );
	m_fZoneMaxRange		= rkLoader.LoadFloat_e( "collision_max_range", 0.0f );
	m_fZoneInnerRange	= rkLoader.LoadFloat_e( "collision_inner_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "collision_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "collision_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "collision_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "collision_width", 0.0f );
	m_fOffSet		= rkLoader.LoadFloat_e( "collision_offset", 0.0f );
}

void ioGraffitiWeapon::SetTeam( ioPlayStage *pStage, TeamType eType )
{
	if( !pStage ) return;

	ioWeapon::SetTeam( pStage, eType );

	DestroyResource();

	ioHashString szCurWeaponEffect;

	if( eType == TEAM_BLUE )
	{
		szCurWeaponEffect = m_BlueTeamEffect;
	}
	else if( eType == TEAM_RED )
	{
		szCurWeaponEffect = m_RedTeamEffect;
	}
	else //개인전
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
		{
			szCurWeaponEffect = m_BlueTeamEffect;
		}
		else
		{
			szCurWeaponEffect = m_RedTeamEffect;
		}
	}

	if( !SetFireMesh( szCurWeaponEffect ) )
		DestroyResource();
}

bool ioGraffitiWeapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	D3DXVECTOR3 vNewPos = GetPosition();
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();

	if( m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;
		vNewPos += vOffSet;

		SetPosition( vNewPos );
	}

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();

	SetOrientation( qtRot );

	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );
	m_vMoveDir = vDir;

	AddEffectEvent( m_pFireMesh );
	return true;
}

void ioGraffitiWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case TS_NONE:
		if( m_dwCreatedTime+m_dwLiveTime < dwCurTime )
			Explosion( pStage );
		else
			OnNoneState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioGraffitiWeapon::OnNoneState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioGraffitiWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionDuration == 0 )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwExplosionStart+m_dwExplosionDuration < dwCurTime )
	{
		SetWeaponDead();
		return;
	}

	if( m_EffectID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
		if( pEffect )
		{
			pEffect->SetWorldPosition( GetPosition() );
		}
	}
}

void ioGraffitiWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}

	if( !IsLive() )
	{
		if( m_EffectID != -1 )
		{
			ioMapEffect *pEffect = pStage->FindMapEffect( m_EffectID );
			if( pEffect )
			{
				pEffect->EndEffectGrace();
				m_EffectID = -1;
			}
		}
	}
}

ioWeapon::WeaponType ioGraffitiWeapon::GetType() const
{
	return WT_GRAFFITI;
}

ActionStopType ioGraffitiWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioGraffitiWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioGraffitiWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioGraffitiWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(),
									ioMath::UNIT_ALL,
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  ioMath::UNIT_ALL,
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioGraffitiWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION )
		return;

	m_State = TS_EXPLOSION;
	m_dwExplosionStart = FRAMEGETTIME();

	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() && pStage )
	{
		D3DXVECTOR3 vPos = GetPosition();
		ioMapEffect *pEffect = pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		if( pEffect )
		{
			m_EffectID = pEffect->GetUniqueID();
			pEffect->SetWorldOrientation( GetOrientation() );
		}

		ioWeapon::PlayExplosionSound();
	}

	DestroyResource( pStage );
}

bool ioGraffitiWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State != TS_EXPLOSION ) 
		return false;

	bool bCollision = false;

	if( m_TargetColType == TCT_SPHERE )
	{
		bCollision = CheckCollisionSphere( pEntity );
	}
	else if( m_TargetColType == TCT_CYLINDER )
	{
		bCollision = CheckCollisionCylinder( pEntity );
	}
	else if( m_TargetColType == TCT_BOX )
	{
		bCollision = CheckCollisionBox( pEntity );
	}
	else if( m_TargetColType == TCT_DONUT )
	{
		bCollision = CheckCollisionDonut( pEntity );
	}

	ioBaseChar *pChar = ToBaseChar(pEntity);
	if( bCollision && pEntity->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( !pChar ) return false;

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( pChar->IsApplyDownState() ||
				( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
				pChar->GetState() == CS_GRAPPLING_PUSHED ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_ITEM ||
				pChar->GetState() == CS_GRAPPLING_WOUNDED_BUFF ) )
			{
				return false;
			}
			else if( pChar->IsApplyDownState() )
			{
				return false;
			}
		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pChar->IsFloating() )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( pChar->IsApplyDownState() || !pChar->IsFloating() )
				return false;
		}
	}

	return bCollision;
}

bool ioGraffitiWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

DWORD ioGraffitiWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioGraffitiWeapon::TranslateByConveyer( const D3DXVECTOR3 &vMoveAmt )
{
	if( m_State == TS_EXPLOSION )
	{
		SetPosition( GetPosition() + vMoveAmt );
	}
}

bool ioGraffitiWeapon::CheckCollisionSphere( ioPlayEntity *pEntity )
{
	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();
	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

#ifndef SHIPPING
	// 사각형 생성
	D3DXVECTOR3 vTestMin, vTestMax;

	vTestMin.x = vPos.x - fZoneRange;
	vTestMin.y = vPos.y - fZoneRange;
	vTestMin.z = vPos.z - fZoneRange;

	vTestMax.x = vPos.x + fZoneRange;
	vTestMax.y = vPos.y + fZoneRange;
	vTestMax.z = vPos.z + fZoneRange;

	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vTestMin, vTestMax );

	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vDiff = vPos - pEntity->GetMidPositionByRate();
	float fLength = D3DXVec3Length( &vDiff );
	if( fLength <= m_fZoneMinRange )
		return false;

	D3DXVECTOR3 vPoint = ioMath::VEC3_ZERO;
	if( !pEntity->CheckSpherePoint( vPos, fZoneRange, vPoint ) )
		return false;

	D3DXVECTOR3 vDir = vPoint - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	if( D3DXVec3Dot( &m_vMoveDir, &vDir ) >= fHalfCosine )
		return true;

	return false;
}

bool ioGraffitiWeapon::CheckCollisionCylinder( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, fZoneRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += fZoneRange;
	vMax.z -= fZoneRange;

	vMin.x -= fZoneRange;
	vMin.z += fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > fZoneRange * fZoneRange )
		return false;

	if( fTargetDist < m_fZoneMinRange * m_fZoneMinRange )
		return false;

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vMoveDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

bool ioGraffitiWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	vCenter += m_vMoveDir * m_fZoneRange * FLOAT05; 
	D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
	vCenter -= vOffset;

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );
	// 유저 방향으로 사각형 회전
	kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pOwnerChar->GetWorldOrientation() );
	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
	kBuffBox.SetCenter( vCenter );
#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	if( !pEntity )
		return false;

	if( !pEntity->IsWoundedCollision( kBuffBox ) )
		return false;

	return true;
}

bool ioGraffitiWeapon::CheckCollisionDonut( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fZoneRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += m_fZoneRange;
	vMax.z -= m_fZoneRange;

	vMin.x -= m_fZoneRange;
	vMin.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > m_fZoneRange * m_fZoneRange )
	{
		return false;
	}

	float fInnerRange = m_fZoneRange - m_fZoneInnerRange;
	fInnerRange = max( 0, fInnerRange );

	if( fTargetDist < fInnerRange * fInnerRange )
	{
		return false;
	}

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vMoveDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
	{
		return true;
	}

	return false;
}

void ioGraffitiWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	D3DXVECTOR3 vNewPos = vPos;
	D3DXQUATERNION qtRot = pOwner->GetTargetRot();

	if( m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;
		vNewPos += vOffSet;
	}

	SetPosition( vNewPos );
}

void ioGraffitiWeapon::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
	{
		m_dwLiveTime = dwTime;
	}
}
