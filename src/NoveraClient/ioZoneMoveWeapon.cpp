

#include "stdafx.h"

#include "ioZoneMoveWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioZoneMoveWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioZoneMoveWeapon::m_DieActionStopType = AST_NONE;

ioZoneMoveWeapon::ioZoneMoveWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	ClearData();	
}

ioZoneMoveWeapon::~ioZoneMoveWeapon()
{
}

void ioZoneMoveWeapon::ClearData()
{
	m_fZoneRange		= 0.0f;
	m_fZoneInnerRange	= 0.0f;
	m_fZoneAngle		= 0.0f;

	m_fCurMoveRange = 0.0f;
	m_fGoMaxRange   = 0.0f;
	m_fBackMaxRange = 0.0f;

	m_dwCurCharCallTime = 0;

	m_vPosition		 = ioMath::VEC3_ZERO;
	m_vMoveDir		 = ioMath::VEC3_ZERO;
	m_vCollisionDir  = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );

	m_State = WS_INIT;
	m_bCharWallCol = false;
}

void ioZoneMoveWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "zone_live_time", 0 );

	m_fZoneRange		= rkLoader.LoadFloat_e( "zone_range", 0.0f );
	m_fZoneMinRange		= rkLoader.LoadFloat_e( "zone_min_range", 0.0f );
	m_fZoneMaxRange		= rkLoader.LoadFloat_e( "zone_max_range", 0.0f );
	m_fZoneInnerRange	= rkLoader.LoadFloat_e( "zone_inner_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "zone_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "zone_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "zone_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "zone_width", 0.0f );
	m_fMaxWidth		= rkLoader.LoadFloat_e( "zone_max_width", 0.0f );

	m_dwCharCallTime = rkLoader.LoadInt_e( "enable_char_col_time", 0 );
	
	m_fStartSpeed = rkLoader.LoadFloat_e( "start_speed", 0.0f );
	m_fMaxSpeed   = rkLoader.LoadFloat_e( "max_speed", 0.0f );
	m_fAccelSpeed = rkLoader.LoadFloat_e( "accel_speed", 0.0f );

	m_fGoMaxRange   = rkLoader.LoadInt_e( "go_move_range", 0 );
	m_fBackMaxRange = rkLoader.LoadInt_e( "back_move_range", 0 );

	m_fCurveRange   = rkLoader.LoadInt_e( "curve_range", 0 );

	rkLoader.LoadString_e( "wall_effect", "", szBuf, MAX_PATH );
	m_szWallEffect = szBuf;

	rkLoader.LoadString_e( "wall_coll_effect", "", szBuf, MAX_PATH );
	m_szWallCollEffect = szBuf;
	//////////////////////////////////////////////////////////////////////////
	m_eWeaponRetrunType = (WeaponRetrunType)rkLoader.LoadInt_e( "return_type", WRT_NONE );

	m_fRotateY = rkLoader.LoadFloat_e( "rotate_y", 0.f );
	m_fRotateX = rkLoader.LoadFloat_e( "rotate_x", 0.f );

	rkLoader.LoadString_e( "weapon_effect", "", szBuf, MAX_PATH );
	m_szWeaponEffect = szBuf;

	m_bRotationBox = rkLoader.LoadBool( "rotation_collision_box", false );
}

void ioZoneMoveWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	SetPosition( vPos );
	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );
}

ioWeapon::WeaponType ioZoneMoveWeapon::GetType() const
{
	return WT_ZONE_MOVE_WEAPON;
}

ActionStopType ioZoneMoveWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioZoneMoveWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioZoneMoveWeapon::CreateWall( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos = GetPosition();
	vPos += m_vMoveDir * m_fGoMaxRange;

	if( pStage )
	{
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szWallEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		if( pMapEffect )
		{
			pMapEffect->SetWorldOrientation( GetOrientation() );
			m_WallEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioZoneMoveWeapon::CreateWallColl( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bCharWallCol  )
		return;

	if( pStage )
	{
		ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_szWallCollEffect, GetPosition(), D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
		if( pMapEffect )
		{
			pMapEffect->SetWorldOrientation( GetOrientation() );
			m_WallCollEfffectID = pMapEffect->GetUniqueID();
		}
	}
}

float ioZoneMoveWeapon::CheckCurSpeed( float fTimePerSec )
{
	float fCurSpeed = GetAttribute()->m_fMoveSpeed;

	if( m_fStartSpeed != m_fMaxSpeed || m_fAccelSpeed > 0.0f )
	{
		float fGapTime = (float)(FRAMEGETTIME() - m_dwCreatedTime);
		float fDTime = fGapTime / FLOAT1000;
		float fCurAccel = m_fAccelSpeed * fDTime * fDTime;

		fCurSpeed += fCurAccel;
		fCurSpeed = min( fCurSpeed, m_fMaxSpeed );
	}
		
	return fCurSpeed;
}

void ioZoneMoveWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case WS_INIT:
		{			
			m_State = WS_MOVE;
			m_dwCurCharCallTime = dwCurTime;

			CreateWall( fTimePerSec, pStage );

			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szWeaponEffect, GetPosition(), ioMath::UNIT_ALL );
			if( pMapEffect )
			{
				pMapEffect->SetWorldOrientation( GetOrientation() );
				m_dwWeaponEffectID = pMapEffect->GetUniqueID();
			}
		}
		break;
	case WS_MOVE:
	case WS_RETURN:
		{
			ProcessMove( fTimePerSec, pStage );
		}
		break;
	default:
		{
			SetWeaponDead();
		}
		break;
	}
}

void ioZoneMoveWeapon::ProcessMove( float fTimePerSec, ioPlayStage *pStage )
{
	if( fTimePerSec == 0.0f )
		fTimePerSec = 0.001f;
	
	
	float fMoveDist = CheckCurSpeed( fTimePerSec ) * fTimePerSec;
	D3DXVECTOR3 vMove = GetMoveDir() * fMoveDist;	
	m_fCurMoveRange += fMoveDist;
	
	switch( m_State )
	{
	case WS_MOVE:
		if( m_fCurMoveRange < m_fGoMaxRange )
		{			
			GetParentSceneNode()->Translate( vMove );
		}
		else
		{
			if( m_eWeaponRetrunType == WRT_ROTATE )
			{
				float fAngleH = ioMath::ArrangeHead( m_fRotateY );
				float fAngleV = ioMath::ArrangeHead( m_fRotateX );

				D3DXQUATERNION qtRot;
				D3DXVECTOR3 vAxis;
				D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
				// vertical
				D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngleV) );
				m_vMoveDir = qtRot * m_vMoveDir;
				// horizon
				D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngleH) );
				m_vMoveDir = qtRot * m_vMoveDir;
			}

			m_fCurMoveRange = 0;
			m_State = WS_RETURN;
			CreateWallColl( fTimePerSec, pStage );
		}
		break;
	case WS_RETURN:
		if( m_fCurMoveRange < m_fBackMaxRange )
		{
			if( m_eWeaponRetrunType == WRT_NONE )
				GetParentSceneNode()->Translate( -vMove );
			else 
				GetParentSceneNode()->Translate( vMove );
		}
		else
		{
			m_fCurMoveRange = 0;
			m_State = WS_END;
		}
		break;
	}

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWeaponEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( GetPosition() );
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioZoneMoveWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{	
	if( m_State == WS_INIT )
		return false;

	if( !pEntity )
		return false;

	if( !CheckCollisionBox( pEntity ) )
		return false;

	if( pEntity->GetSubType() == ioPlayEntity::PST_CHAR && m_dwCurCharCallTime + m_dwCharCallTime < FRAMEGETTIME() )
		return false;

	if( !CheckCollisionLine( pEntity, pEntity->GetWorldPosition() ) )
		return false;

	m_bCharWallCol = true;
	return true;
}

bool ioZoneMoveWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	DWORD dwCurTime = FRAMEGETTIME();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	vCenter += m_vCollisionDir * m_fZoneRange * FLOAT05;
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
	if( m_bRotationBox )
	{
		D3DXVECTOR3 vDir = GetParentSceneNode()->GetDerivedOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		vDir.y = 0.0f;
		D3DXVec3Normalize( &vDir, &vDir );
		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );

		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, qtRot );
	}
	else
	{
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
	}

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

void ioZoneMoveWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		m_fCurRange = 0.0f;
		m_fCurWidth = 0.0f;
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
		if( m_bRotationBox )
		{
			D3DXVECTOR3 vDir = GetParentSceneNode()->GetDerivedOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
			vDir.y = 0.0f;
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );

			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, qtRot );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, qtRot );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
		}
	}
}

const ioOrientBox& ioZoneMoveWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioZoneMoveWeapon::SetWeaponDead()
{
	m_bLive = false;
	ClearData();
}

bool ioZoneMoveWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	D3DXVECTOR3 vPos = GetPosition();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vPos, vColPoint, true );

	return false;
}

void ioZoneMoveWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

D3DXVECTOR3 ioZoneMoveWeapon::GetMoveDir()
{
	return m_vMoveDir;
}

D3DXVECTOR3 ioZoneMoveWeapon::GetAttchPosition()
{
	m_vPosition = ioWeapon::GetPosition();
	m_vPosition.y -= m_fUnderHeight;

	return m_vPosition;
}

void ioZoneMoveWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( pStage )
	{
		ioMapEffect* pWallffect = pStage->FindMapEffect( m_WallEffectID );
		if( pWallffect )
			pWallffect->EndEffectGrace();
		
		ioMapEffect* pWallColEffect = pStage->FindMapEffect( m_WallCollEfffectID );
		if( pWallColEffect )
			pWallColEffect->EndEffectGrace();

		ioMapEffect* pWeaponEffect = pStage->FindMapEffect( m_dwWeaponEffectID );
		if( pWeaponEffect )
			pWeaponEffect->EndEffectGrace();
	}
}

bool ioZoneMoveWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	if ( m_bIgnoreMapCollision )
		return false;

	return true;
}
