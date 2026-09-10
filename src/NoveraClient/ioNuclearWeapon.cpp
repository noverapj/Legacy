

#include "stdafx.h"

#include "ioNuclearWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioNuclearWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioNuclearWeapon::m_DieActionStopType = AST_NONE;

ioNuclearWeapon::ioNuclearWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_SensorID = -1;
	m_pNuclear = NULL;
	m_NuclearState = NS_FALL;

	m_bResistance = false;
}

ioNuclearWeapon::~ioNuclearWeapon()
{
	if( m_pNuclear )
	{
		LOG.PrintTimeAndLog( 0, "ioNuclearWeapon::~ioNuclearWeapon - UnReleased Resource" );
	}
}

bool ioNuclearWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyNuclear();
		return CreateNuclear( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioNuclearWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_sensor", "", szBuf, MAX_PATH );
	m_RedSensor = szBuf;
	rkLoader.LoadString_e( "blue_sensor", "", szBuf, MAX_PATH );
	m_BlueSensor = szBuf;
	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );
	m_fStartHeight = rkLoader.LoadFloat_e( "start_height", 0.0f );
	m_bEffectOnlyTeam = rkLoader.LoadBool_e( "effect_only_team", false );
}

bool ioNuclearWeapon::CreateNuclear( const ioHashString &szFileName )
{
	m_pNuclear = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pNuclear )
		return false;

	GetParentSceneNode()->AttachObject( m_pNuclear );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	AddEffectEvent( m_pNuclear );
	return true;
}

void ioNuclearWeapon::DestroyNuclear()
{
	if( m_pNuclear )
	{
		g_EffectFactory.DestroyEffect( m_pNuclear );
		m_pNuclear = NULL;
	}
}

void ioNuclearWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( !pStage )
	{
		m_vStartPos = vPos;
		SetPosition( vPos );
		return;
	}

	m_vStartPos.x = vPos.x;
	m_vStartPos.y = pStage->GetMapHeight( vPos.x, vPos.z/*, GetOwner()*/ );
	m_vStartPos.z =	vPos.z;

	SetPosition( m_vStartPos );

	if( m_pNuclear )
	{
		D3DXVECTOR3 vNewPos = m_vStartPos;
		vNewPos.y += m_fStartHeight;
		m_pNuclear->GetParentSceneNode()->SetPosition( vNewPos );
	}

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_SensorID != -1 )
	{
		ioMapEffect *pEffect = pStage->FindMapEffect( m_SensorID );
		if( pEffect )
		{
			pStage->DestroyMapEffect( m_SensorID );
			m_SensorID = -1;
		}
	}

	if ( m_bEffectOnlyTeam )
	{
		ioBaseChar *pPlayer = pStage->GetOwnerChar();
		if ( pOwner->GetTeam() != pPlayer->GetTeam() )
			return;
	}

	// Set Sensor Effect
	ioHashString szCurSensor;

	if( GetTeam() == TEAM_BLUE )
		szCurSensor = m_BlueSensor;
	else if( GetTeam() == TEAM_RED )
		szCurSensor = m_RedSensor;
	else //°³ÀÎÀü
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			szCurSensor = m_BlueSensor;
		else
			szCurSensor = m_RedSensor;
	}

	D3DXVECTOR3 vScale(1.0f, FLOAT1, FLOAT1 );
	ioMapEffect *pEffect = pStage->CreateMapEffect( szCurSensor, m_vStartPos, vScale );
	if( pEffect )
	{
		m_SensorID = pEffect->GetUniqueID();
	}
}

void ioNuclearWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	switch( m_NuclearState )
	{
	case NS_FALL:
		OnFalling( fTimePerSec, pStage );
		break;
	case NS_EXPLOSION:
		SetWeaponDead();
		break;
	}
}

void ioNuclearWeapon::OnFalling( float fTimePerSec, ioPlayStage *pStage )
{
	float fMoveDist = GetMoveSpeed() * fTimePerSec;
	GetParentSceneNode()->Translate( D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist );

	if( m_pNuclear )
	{
		m_pNuclear->Update( fTimePerSec * FLOAT1000 );
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

bool ioNuclearWeapon::IsFalling() const
{
	if( m_NuclearState == NS_FALL )
		return true;

	return false;
}

void ioNuclearWeapon::Explosion( ioPlayStage *pStage )
{
	if( m_NuclearState == NS_EXPLOSION )
		return;

	m_NuclearState = NS_EXPLOSION;

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

void ioNuclearWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_SensorID != -1 )
	{
		if( pStage )
		{
			pStage->DestroyMapEffect( m_SensorID );
			m_SensorID = -1;
		}
	}

	DestroyNuclear();
}

const ioOrientBox& ioNuclearWeapon::GetLocalAttackBox() const
{
	if( m_pNuclear )
	{
		if( IsFalling() )
			return m_pNuclear->GetCollisionBox();
		else
			return m_LocalAttackBox;
	}

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioNuclearWeapon::GetType() const
{
	return WT_NUCLEAR;
}

ActionStopType ioNuclearWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioNuclearWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioNuclearWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

void ioNuclearWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioNuclearWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_NuclearState != NS_EXPLOSION )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fExplosionRange;
	vBottom.y -= m_fExplosionRange;
	ioCylinder kCylinder( vTop, vBottom, m_fExplosionRange );
	vCenter = kCylinder.GetCenter();

	D3DXVECTOR3 vPoint;
	if( pEntity->CheckCylinderPoint( kCylinder, vCenter, vPoint ) )
	{
		D3DXVECTOR3 vDiff = vCenter - vPoint;
		vDiff.y = 0.0f;

		float fDistanceGap = D3DXVec3Length( &vDiff );
		float fHeightGap = fabs( vCenter.y - vPoint.y );

		if( fHeightGap <= m_fExplosionRange && fDistanceGap <= m_fExplosionRange )
			return true;
	}

	return false;
}

void ioNuclearWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_NuclearState != NS_FALL )
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

DWORD ioNuclearWeapon::GetCollisionEnableTime() const
{
	if( m_NuclearState == NS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}