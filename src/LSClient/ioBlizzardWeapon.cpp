
#include "stdafx.h"

#include "ioBlizzardWeapon.h"

#include "ioBaseChar.h"

ActionStopType ioBlizzardWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioBlizzardWeapon::m_DieActionStopType = AST_NONE;

ioBlizzardWeapon::ioBlizzardWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON );

	m_pBlizzard = NULL;
}

ioBlizzardWeapon::~ioBlizzardWeapon()
{
	if( m_pBlizzard )
	{
		LOG.PrintTimeAndLog( 0, "ioBlizzardWeapon::~ioBlizzardWeapon - UnReleased Resouce" );
	}
}

bool ioBlizzardWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		return CreateBlizzard( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBlizzardWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
}

void ioBlizzardWeapon::SetLiveTime( DWORD dwTime )
{
}

bool ioBlizzardWeapon::CreateBlizzard( const ioHashString &szFileName )
{
	m_pBlizzard = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBlizzard )	return false;

	GetParentSceneNode()->AttachObject( m_pBlizzard );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		pOwner->ReduceCurSkillGaugeBySkill( NULL, m_pConstAttr->m_fNeedGauge );
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioBlizzardWeapon::CreateBlizzard - Not Has Owner" );
	AddEffectEvent( m_pBlizzard );
	return false;	
}

void ioBlizzardWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();
	}

	if( m_dwLiveTime > 0 && m_dwCreatedTime+m_dwLiveTime+dwLooseTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
	}
	else
	{
		float fMoveDist = GetMoveSpeed() * fTimePerSec;
		GetParentSceneNode()->Translate( D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) * fMoveDist );

		if( m_pBlizzard )
		{
			m_pBlizzard->Update( fTimePerSec * FLOAT1000 );
		}

		UpdateWorldAttackBox( fTimePerSec, false );
	}
}

void ioBlizzardWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBlizzard )
	{
		g_EffectFactory.DestroyEffect( m_pBlizzard );
		m_pBlizzard = NULL;
	}
}

ioWeapon::WeaponType ioBlizzardWeapon::GetType() const
{
	return WT_BLIZZARD;
}

ActionStopType ioBlizzardWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBlizzardWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioBlizzardWeapon::GetMoveSpeed()
{
	return GetAttribute()->m_fMoveSpeed;
}

const ioOrientBox& ioBlizzardWeapon::GetLocalAttackBox() const
{
	if( m_pBlizzard )
		return m_pBlizzard->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

void ioBlizzardWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioBlizzardWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision) );

	CheckPiercing( bCollision );

	return bCollision;
}

bool ioBlizzardWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

