

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioCannonBallWeapon.h"

ActionStopType ioCannonBallWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioCannonBallWeapon::m_DieActionStopType = AST_NONE;

ioCannonBallWeapon::ioCannonBallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pBombMesh = NULL;

	m_dwExplosionTime = 0;

	m_bResistance = false;

	m_BombState = BS_FLOATING;
}

ioCannonBallWeapon::~ioCannonBallWeapon()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioCannonBallWeapon::~ioCannonBallWeapon - UnReleased Resource" );
	}
}

void ioCannonBallWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	DWORD dwDuration = rkLoader.LoadFloat_e( "bomb_drop_duration", 0.0f );
	m_dwExplosionTime = FRAMEGETTIME() + dwDuration;

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fXOffSet = rkLoader.LoadFloat_e( "x_offset", 0.0f );
	m_fYOffSet = rkLoader.LoadFloat_e( "y_offset", 0.0f );

	m_fFloatingPower = GetAttribute()->m_fFloatPower;
}

bool ioCannonBallWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetBombMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

bool ioCannonBallWeapon::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pBombMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pBombMesh->GetCollisionBox();

	AddEffectEvent( m_pBombMesh );
	return true;
}


void ioCannonBallWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioCannonBallWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_BombState )
	{
	case BS_FLOATING:
		OnFloating( fTimePerSec, pStage );
		break;
	case BS_EXPLOSION:
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioCannonBallWeapon::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionTime > FRAMEGETTIME() )
	{
		float fRotateAngle = 900.0f * fTimePerSec;
		GetParentSceneNode()->PitchX( fRotateAngle, ioNode::TS_LOCAL );

		D3DXVECTOR3 vMove = m_vMoveDir * ( GetMoveSpeed() * fTimePerSec );
		vMove.y += m_fFloatingPower * fTimePerSec;
		m_fFloatingPower -= pStage->GetRoomGravity() * fTimePerSec;
		GetParentSceneNode()->Translate( vMove );
		
		if( GetCollisionEnableTime() < FRAMEGETTIME() )
		{
			D3DXVECTOR3 vPos = GetPosition();
			float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
			if( vPos.y <= 0.0f || vPos.y <= fMapHeight )
			{
				if( vPos.y <= 0.0f )
					vPos.y = 0.0f;
				else
					vPos.y = fMapHeight;

				SetPosition( vPos );
				SetOrientation( D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );
				m_fFloatingPower = 0.0f;

				Explosion( pStage );
			}
		}
	}
	else
	{
		Explosion( pStage );
	}

	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

float ioCannonBallWeapon::GetMoveSpeed() const
{
	return GetAttribute()->m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioCannonBallWeapon::SetOwnerChargeFloatRate( float fRate )
{
	m_fOwnerChargeFloatRate = fRate;

	m_fFloatingPower = GetAttribute()->m_fFloatPower * m_fOwnerChargeFloatRate;
}

void ioCannonBallWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_BombState == BS_EXPLOSION )
		return;

	m_BombState = BS_EXPLOSION;
	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );

	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pBombMesh->GetBoundBox().GetExtents(1) );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}
		
		vPos.y += FLOAT1;

		pStage->CreateMapEffect( m_BoomEffect,
								 vPos,
								 GetScale() );

		ioWeapon::PlayExplosionSound();
	}
}

bool ioCannonBallWeapon::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioCannonBallWeapon::IsExplosion() const
{
	if( m_BombState == BS_EXPLOSION )
		return true;

	return false;
}

void ioCannonBallWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXVECTOR3 vNewPos = GetPosition() + m_vMoveDir * m_fXOffSet;
	vNewPos.y += m_fYOffSet;

	SetPosition( vNewPos );
}

bool ioCannonBallWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_BombState != BS_EXPLOSION )
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

void ioCannonBallWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL; 
	}
}

const ioOrientBox& ioCannonBallWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioCannonBallWeapon::GetType() const
{
	return WT_CANNON_BALL;
}

ActionStopType ioCannonBallWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioCannonBallWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

bool ioCannonBallWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioCannonBallWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
	}
}

void ioCannonBallWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_BombState != BS_FLOATING )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToBallStruct(pTarget) || ToDummyChar(pTarget) || ToWallZone(pTarget)  )
		{
			bCollision = true;
		}
		else if( ToPushStruct(pTarget) )
		{
			bCollision = true;

			ioPushStruct *pPush = ToPushStruct( pTarget );
			if( pPush->GetOwnerName() == GetOwnerName() &&
				pPush->CheckCreateWeaponIndex( GetWeaponIndex() ) )
			{
				bCollision = false;
			}
		}
		else
		{
			if( m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay < FRAMEGETTIME() )
			{
				if( GetTeam() != pTarget->GetTeam() )
					bCollision = true;
				else if( ToBaseChar(pTarget) == GetOwner() )
					bCollision = true;
			}
		}
	}

	if( !bCollision )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) ||
		ToDummyChar(pTarget) || ToWallZone(pTarget) ||
		GetPiercingType() == CPT_NONE )
	{
		Explosion( pStage );
		return;
	}

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

DWORD ioCannonBallWeapon::GetCollisionEnableTime() const
{
	if( m_BombState == BS_FLOATING || m_BombState == BS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}


bool ioCannonBallWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	if( GetCollisionEnableTime() >= FRAMEGETTIME() )
		return true;

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		Explosion( pStage );
		return true;
	}

	return false;
}