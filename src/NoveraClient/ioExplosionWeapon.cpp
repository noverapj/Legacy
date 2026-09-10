

#include "stdafx.h"

#include "ioExplosionWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioExplosionWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioExplosionWeapon::m_DieActionStopType = AST_NONE;

ioExplosionWeapon::ioExplosionWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;
	m_State = ES_DELAY;

	m_dwDuration = 0;
	
	m_bResistance = false;
}

ioExplosionWeapon::~ioExplosionWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioExplosionWeapon::~ioExplosionWeapon - UnReleased Resource" );
	}
}

bool ioExplosionWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return SetFireMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioExplosionWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	DWORD dwDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );
	m_dwDuration = FRAMEGETTIME() + dwDuration;
	
	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "explosion_boom_effect", "", szBuf, MAX_PATH );	
	m_BoomEffect = szBuf;

	m_bSetForcePos = rkLoader.LoadBool_e( "set_force_pos", false );
}

bool ioExplosionWeapon::SetFireMesh( const ioHashString &szFileName )
{
	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )
		return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();
	
	AddEffectEvent( m_pFireMesh );
	return true;
}

void ioExplosionWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_State )
	{
	case ES_DELAY:
		OnDelayState( fTimePerSec, pStage );
		break;
	case ES_EXPLOSION:
		SetWeaponDead();
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioExplosionWeapon::OnDelayState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		Explosion( pStage );
		return;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioExplosionWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioExplosionWeapon::GetType() const
{
	return WT_EXPLOSION;
}

ActionStopType ioExplosionWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioExplosionWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioExplosionWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioExplosionWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioExplosionWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_State == ES_EXPLOSION )
		return;

	m_State = ES_EXPLOSION;

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();
		vPos.y += FLOAT1;

		pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		ioWeapon::PlayExplosionSound();
	}

	DestroyResource();
}

bool ioExplosionWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	D3DXVECTOR3 vPos = GetPosition();

	ioBaseChar* pChr = ToBaseChar( pEntity );
	if( pChr )
	{
		vPos = pChr->GetWorldPosition();
	}
	if( m_State != ES_EXPLOSION ) 
		return false;

	float fTargetDist = -FLOAT1;
	if( !pEntity->CheckSphereDistance( GetPosition(), m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

void ioExplosionWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( !pStage || m_bSetForcePos)
	{
		SetPosition( vPos );
		return;
	}

	D3DXVECTOR3 vNewPos = vPos;
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y + FLOAT1 );

	if( fMapHeight <= FLOAT1 )
	{
		DestroyResource();
		SetWeaponDead();
		return;
	}

	vNewPos.y = fMapHeight + FLOAT1;
	SetPosition( vNewPos );
}

bool ioExplosionWeapon::IsExplosion() const
{
	if( m_State == ES_EXPLOSION )
		return true;

	return false;
}

bool ioExplosionWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

DWORD ioExplosionWeapon::GetCollisionEnableTime() const
{
	if( m_State == ES_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}