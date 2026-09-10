

#include "stdafx.h"

#include "ioLimitAttachWeapon.h"

#include "ioBaseChar.h"

ActionStopType ioLimitAttachWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioLimitAttachWeapon::m_DieActionStopType = AST_NONE;

ioLimitAttachWeapon::ioLimitAttachWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DIVIDE_ACTION );

	m_pFireBall = NULL;
}

ioLimitAttachWeapon::~ioLimitAttachWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioLimitAttachWeapon::~ioMissileWeapon - UnReleased Resouce" );
	}
}

bool ioLimitAttachWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioLimitAttachWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
}

bool ioLimitAttachWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );
	UpdateCurPosition( pOwner );

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioLimitAttachWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );
}

void ioLimitAttachWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		UpdateWorldAttackBox( fTimePerSec, true );
		SetWeaponDead();
	}

	CheckOnlyDownAttack( pOwner );

	if( !m_CreateBuffName.IsEmpty() )
	{
		if( !pOwner->HasBuff( m_CreateBuffName ) )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			SetWeaponDead();
		}
	}
	else
	{
		if( m_dwCreatedTime + m_dwLiveTime < FRAMEGETTIME() )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			SetWeaponDead();
		}
	}

	UpdateCurPosition( pOwner );

	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioLimitAttachWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioLimitAttachWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return ioWeapon::GetAttackDir(pEntity);
}

const ioOrientBox& ioLimitAttachWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioLimitAttachWeapon::GetType() const
{
	return WT_LIMIT_ATTACH;
}

ActionStopType ioLimitAttachWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioLimitAttachWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioLimitAttachWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
									GetScale(),
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  GetScale(),
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

const WeaponAttribute* ioLimitAttachWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

bool ioLimitAttachWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = true;

	CheckPiercing( bCollision );

	return bCollision;
}

void ioLimitAttachWeapon::UpdateCurPosition( ioBaseChar *pOwner )
{
	if( !pOwner || !m_pFireBall ) return;

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
	else
	{
		SetPosition( pOwner->GetWorldPosition() );
	}
}


