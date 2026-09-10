

#include "stdafx.h"

#include "ioRunContactWeapon.h"
#include "ioBaseChar.h"

ActionStopType ioRunContactWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioRunContactWeapon::m_DieActionStopType = AST_NONE;

ioRunContactWeapon::ioRunContactWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );
	m_UpdateState = CONTACT_CREATED;
	m_bFirstCheck = true;
}

ioRunContactWeapon::~ioRunContactWeapon()
{
}

void ioRunContactWeapon::SetStartCollisionBox()
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_WorldAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType );
	}
	else
	{
		m_WorldAttackBox.SetNull();
		LOG.PrintTimeAndLog( 0, "ioContactWeapon - %s Not Exist", m_OwnerName.c_str() );
	}

	m_UpdateState = CONTACT_CREATED;
}

void ioRunContactWeapon::GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList )
{
	vBoxList.reserve( 3 );
	vBoxList.push_back( m_PreAttackBox );
	vBoxList.push_back( m_MidAttackBox );
	vBoxList.push_back( m_WorldAttackBox );
}

bool ioRunContactWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );

	if( m_UpdateState == CONTACT_FIRST )
	{
		if( pEntity->IsWoundedCollision( m_PreAttackBox, &vColPoint ) )
			return CheckCollisionLine( pEntity, vColPoint );
	}

	if( pEntity->IsWoundedCollision( m_MidAttackBox, &vColPoint ) )
		return CheckCollisionLine( pEntity, vColPoint );

	if( pEntity->IsWoundedCollision( m_WorldAttackBox, &vColPoint ) )
		return CheckCollisionLine( pEntity, vColPoint );

	return false;
}

bool ioRunContactWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

ioWeapon::WeaponType ioRunContactWeapon::GetType() const
{
	return WT_RUN_CONTACT;
}

ActionStopType ioRunContactWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRunContactWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioRunContactWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	ioBaseChar *pOwner = GetOwner();

	if( bSetNull || !pOwner || !IsLive() )
	{
		m_PreAttackBox.SetNull();
		m_MidAttackBox.SetNull();
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_PreAttackBox = m_WorldAttackBox;

	DWORD dwLooseTime = 0;
	if( pOwner )
	{
		dwLooseTime = pOwner->GetActionStopCurAdd();
	}

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLiveEndTime = m_dwCreatedTime + m_dwLiveTime + dwLooseTime;
	DWORD dwFrameEndTime = dwCurTime + (DWORD)( fTimePerSec * FLOAT1000 );
	D3DXVECTOR3 vAxis;

	if( dwLiveEndTime >= dwFrameEndTime )
	{
		m_WorldAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType );
	}
	else	// 현재 프레임내에서 공격이 끝난다.
	{
		float fLerpRate = 0.0f;
		if( m_dwCreatedTime >= dwCurTime )	// 현재 프레임에서 생성되었다. 1프레임 이내의 공격.
		{
			fLerpRate = (float)(m_dwLiveTime+dwLooseTime) / (float)( dwFrameEndTime - m_dwCreatedTime );
		}
		else	// 이전프레임에서 생성되었다.
		{
			fLerpRate = (float)(dwLiveEndTime - dwCurTime)/(float)( dwFrameEndTime - dwCurTime );
		}

		fLerpRate = max( 0.0f, min( FLOAT1, fLerpRate ) );

		ioOrientBox kCurAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType );

		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &kCurAttackBox.GetAxis(i), fLerpRate );
			D3DXVec3Normalize( &vAxis, &vAxis );
			m_WorldAttackBox.SetAxis( i, vAxis );
		}

		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &kCurAttackBox.GetCenter(), fLerpRate );
		m_WorldAttackBox.SetCenter( vAxis );
	}

	for( int i=0 ; i<3 ; i++ )
	{
		D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetAxis(i), &m_WorldAttackBox.GetAxis(i), FLOAT05 );
		D3DXVec3Normalize( &vAxis, &vAxis );
		m_MidAttackBox.SetAxis( i, vAxis );
		m_MidAttackBox.SetExtents( i, m_WorldAttackBox.GetExtents(i) );
	}

	D3DXVec3Lerp( &vAxis, &m_PreAttackBox.GetCenter(), &m_WorldAttackBox.GetCenter(), FLOAT05 );
	m_MidAttackBox.SetCenter( vAxis );

	if( m_UpdateState == CONTACT_CREATED )
		m_UpdateState = CONTACT_FIRST;
	else
		m_UpdateState = CONTACT_NORMAL;

	m_WorldAttackBoxForMap = m_WorldAttackBox;

	SetPosition( m_WorldAttackBox.GetCenter() );

	if( m_bFirstCheck )
	{
		m_bFirstCheck = false;
	}
}

D3DXVECTOR3 ioRunContactWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( HasFeature( WF_ATTACK_DIRECTION_STRAIGHT ))
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			D3DXVECTOR3	vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize(&vMoveDir,&vMoveDir);
			return vMoveDir;
		}
	}
	else
	{
		if( pEntity )
		{
			D3DXVECTOR3 vMoveDir = pEntity->GetWorldPosition() - m_WorldAttackBox.GetCenter();
			vMoveDir.y = 0.0f;
			D3DXVec3Normalize( &vMoveDir, &vMoveDir );
			return vMoveDir;
		}		
	}

	return D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
}





