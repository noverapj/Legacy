

#include "stdafx.h"

#include "ioFlameWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioFlameWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioFlameWeapon::m_DieActionStopType = AST_NONE;

ioFlameWeapon::ioFlameWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION );

	m_pMeteor = NULL;
	m_FlameState = FS_FALL;

	m_bResistance = false;

	m_szBlueEffect.Clear();
	m_szRedEffect.Clear();

	m_CircleMapEffectID = -1;
}

ioFlameWeapon::~ioFlameWeapon()
{
	if( m_pMeteor )
	{
		LOG.PrintTimeAndLog( 0, "ioNuclearWeapon::~ioNuclearWeapon - UnReleased Resource" );
	}
}

bool ioFlameWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateMeteor( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFlameWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_szRedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_szBlueEffect = szBuf;
}

void ioFlameWeapon::SetExplosionRange( float fRange )
{
	m_fExplosionRange = fRange;
}

bool ioFlameWeapon::CreateMeteor( const ioHashString &szFileName )
{
	m_pMeteor = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pMeteor )
		return false;

	GetParentSceneNode()->AttachObject( m_pMeteor );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	AddEffectEvent( m_pMeteor );
	return true;
}

void ioFlameWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	m_vStartPos = vPos;
	SetPosition( m_vStartPos );

	if( m_pMeteor )
	{
		D3DXVECTOR3 vNewPos( vPos.x, vPos.y+m_fStartHeight, vPos.z );
		m_pMeteor->GetParentSceneNode()->SetPosition( vNewPos );
	}
}

void ioFlameWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_FlameState )
	{
	case FS_FALL:
		OnFalling( fTimePerSec, pStage );
		break;
	case FS_EXPLOSION:
		SetWeaponDead();
		break;
	}
}

void ioFlameWeapon::OnFalling( float fTimePerSec, ioPlayStage *pStage )
{
	CreateCircleMapEffect();
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist );

	if( m_pMeteor )
	{
		m_pMeteor->Update( fTimePerSec * FLOAT1000 );
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	D3DXVECTOR3 vPos = GetPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		SetPosition( vPos );
		SetOrientation( D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );

		Explosion( pStage );
	}
}

void ioFlameWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_FlameState == FS_EXPLOSION )
		return;

	m_FlameState = FS_EXPLOSION;

	float fHalfRange = m_fExplosionRange;

	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );
	
	if( !m_BoomEffect.IsEmpty() && pStage )
	{
		pStage->CreateMapEffect( m_BoomEffect,
								 GetPosition(),
								 GetScale() );

		ioWeapon::PlayExplosionSound();
	}
}

bool ioFlameWeapon::IsFalling() const
{
	if( m_FlameState == FS_FALL )
		return true;

	return false;
}

void ioFlameWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pMeteor )
	{
		g_EffectFactory.DestroyEffect( m_pMeteor );
		m_pMeteor = NULL;
	}
}

const ioOrientBox& ioFlameWeapon::GetLocalAttackBox() const
{
	if( m_pMeteor )
	{
		if( IsFalling() )
			return m_pMeteor->GetCollisionBox();
		else
			return m_LocalAttackBox;
	}

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioFlameWeapon::GetType() const
{
	return WT_FLAME;
}

ActionStopType ioFlameWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFlameWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioFlameWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioFlameWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
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

bool ioFlameWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_FlameState != FS_EXPLOSION )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
		return true;

	return false;
}

void ioFlameWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_FlameState != FS_FALL )
		return;

	bool bCollision = false;
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		ioBaseChar *pChar = ToBaseChar( pTarget );
		if( pChar )
		{
			if( GetTeam() != pTarget->GetTeam() )
				bCollision = true;
			else if( pChar->GetCharName() == GetOwnerName() )
				bCollision = true;
		}
		else
			bCollision = true;
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

DWORD ioFlameWeapon::GetCollisionEnableTime() const
{
	if( m_FlameState == FS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioFlameWeapon::CreateCircleMapEffect()
{
	if ( m_CircleMapEffectID != -1 )
		return;

	if ( m_szBlueEffect.IsEmpty() || m_szRedEffect.IsEmpty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	D3DXVECTOR3 vCirclePos = m_vStartPos;
	float fHeight = pStage->GetMapHeight( vCirclePos.x, vCirclePos.z );

	if ( fHeight <= 0.0f )
		return;

	vCirclePos.y = fHeight;

	ioMapEffect *pMapEffect = NULL;
	if( pOwner->GetTeam() == TEAM_RED )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_szRedEffect, vCirclePos );
	else if( pOwner->GetTeam() == TEAM_BLUE )
		pMapEffect = pOwner->CreateMapEffectBySkill( m_szBlueEffect, vCirclePos );
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pOwner->CreateMapEffectBySkill( m_szBlueEffect, vCirclePos );
		else
			pMapEffect = pOwner->CreateMapEffectBySkill( m_szRedEffect, vCirclePos );
	}

	if( pMapEffect )
		m_CircleMapEffectID = pMapEffect->GetUniqueID();
}

void ioFlameWeapon::DeleteCircleMapEffect()
{
	if ( m_CircleMapEffectID == -1 )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) 
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_CircleMapEffectID );
	if( pMapEffect )
	{
		pMapEffect->EndEffectForce();
		m_CircleMapEffectID = -1;
	}
}

void ioFlameWeapon::SetWeaponDead()
{
	ioWeapon::SetWeaponDead();
	DeleteCircleMapEffect();
}
