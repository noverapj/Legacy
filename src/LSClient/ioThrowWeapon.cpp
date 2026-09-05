

#include "stdafx.h"

#include "ioThrowWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioThrowWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioThrowWeapon::m_DieActionStopType = AST_NONE;

ioThrowWeapon::ioThrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE );

	m_pFireMesh = NULL;

	m_fCurMoveRange = 0.0f;
	m_fDownSpeed = 0.0f;

	m_fMeshHalfHeight = 0.0f;
	m_fPingPongPower = 400.0f;
	m_State = TS_MOVE;
}

ioThrowWeapon::~ioThrowWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioThrowWeapon::~ioThrowWeapon - UnReleased Resource" );
	}
}

bool ioThrowWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioThrowWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXVec3Cross( &m_vRollAxis,
				   &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
				   &m_vMoveDir );
}

bool ioThrowWeapon::SetFireMesh( const ioHashString &rkMeshName )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->GetSceneMgr()->DestroyEntityGrp( m_pFireMesh );
		m_pFireMesh = NULL;
	}

	m_pFireMesh = GetParentSceneNode()->GetSceneMgr()->CreateEntityGrp();
	m_pFireMesh->AddEntity( rkMeshName );
	m_pFireMesh->UpdateBounds( true );
	
	GetParentSceneNode()->AttachObject( m_pFireMesh );

	const ioAxisAlignBox &rkCollisionBox = m_pFireMesh->GetBoundBox();
	D3DXVECTOR3 vMaxPos = rkCollisionBox.GetMaxPos();
	m_fMeshHalfHeight = vMaxPos.y;

	m_LocalAttackBox.SetExtents( 0, vMaxPos.x );
	m_LocalAttackBox.SetExtents( 1, vMaxPos.y );
	m_LocalAttackBox.SetExtents( 2, vMaxPos.z );

	return true;
}

void ioThrowWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	D3DXVECTOR3 vReflect = m_vMoveDir - 2.0f * D3DXVec3Dot( &m_vMoveDir, &vFaceNrm ) * vFaceNrm;
	D3DXVec3Normalize( &m_vMoveDir, &vReflect );

	SetState( TS_PINGPONG );
}

void ioThrowWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_State )
	{
	case TS_MOVE:
		OnMoveState( fTimePerSec );
		UpdateWorldAttackBox( fTimePerSec, false );
		break;
	case TS_DOWN:
		OnDownState( fTimePerSec, pStage );
		UpdateWorldAttackBox( fTimePerSec, false );
		break;
	case TS_PINGPONG:
		OnPingPongState( fTimePerSec, pStage );
		break;
	}

	RollWeapon( fTimePerSec );
}

bool ioThrowWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State == TS_PINGPONG || m_State == TS_DOWN )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	SetWallCollision( vNormal );

	pStage->CreateMapEffect( GetExplosionEffectName(), GetPosition(), GetScale() );
	PlayWoundedSound( NULL );
	return true;
}

void ioThrowWeapon::SetState( ThrowState eState )
{
	m_State = eState;

	switch( eState )
	{
	case TS_DOWN:
		m_fDownSpeed = 0.0f;
		break;
	case TS_PINGPONG:
		m_LocalAttackBox.SetNull();
		break;
	}
}

void ioThrowWeapon::OnMoveState( float fTimePerSec )
{
	float fMoveDist = GetAttribute()->m_fMoveSpeed * fTimePerSec;
	GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	m_fCurMoveRange += fMoveDist;

	//float fMaxRange = GetAttribute()->m_fMoveRange + m_fGrowthMoveRange;
	if( m_fCurMoveRange >= GetAttribute()->m_fMoveRange )
	{
		SetState( TS_DOWN );
	}
}

void ioThrowWeapon::OnDownState( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos = GetPosition();
	vPos += m_vMoveDir * ( GetAttribute()->m_fMoveSpeed * fTimePerSec );

	m_fDownSpeed += pStage->GetRoomGravity() * fTimePerSec;
	vPos.y -= m_fDownSpeed * fTimePerSec;


	float fExtends = m_pFireMesh->GetBoundBox().GetExtents(1);
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+fExtends );
	if( vPos.y < fMapHeight+fExtends )
	{
		vPos.y = fMapHeight;
		vPos.y += fExtends;

		SetPosition( vPos );

		pStage->CreateMapEffect( GetExplosionEffectName(), GetPosition(), GetScale() );
		PlayWoundedSound( NULL );
		SetWeaponDead();
	}
	else
	{
		SetPosition( vPos );
	}
}

void ioThrowWeapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
{
	D3DXVECTOR3 vPos = GetPosition();
	vPos += m_vMoveDir * ( GetAttribute()->m_fMoveSpeed * fTimePerSec * 0.3f );

	vPos.y += m_fPingPongPower * fTimePerSec;
	m_fPingPongPower -= pStage->GetRoomGravity() * fTimePerSec;

	SetPosition( vPos );

	float fRate = ( 400.0f + m_fPingPongPower ) / 800.0f;
	int iAlphaRate = MAX_ALPHA_RATE * fRate;
	m_pFireMesh->SetAlphaRate( iAlphaRate );

	if( iAlphaRate <= 0 )
	{
		SetWeaponDead();
	}
}

void ioThrowWeapon::RollWeapon( float fTimePerSec )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->Rotate( m_vRollAxis, 1600.0f * fTimePerSec );
	}
}

void ioThrowWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		GetParentSceneNode()->GetSceneMgr()->DestroyEntityGrp( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioThrowWeapon::GetType() const
{
	return WT_THROW;
}

ActionStopType ioThrowWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioThrowWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

D3DXVECTOR3 ioThrowWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioThrowWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioThrowWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}
