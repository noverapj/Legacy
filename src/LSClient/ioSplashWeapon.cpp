

#include "stdafx.h"

#include "ioSplashWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioSplashWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioSplashWeapon::m_DieActionStopType = AST_NONE;

ioSplashWeapon::ioSplashWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_fSplashScale_w = FLOAT1;
	m_fSplashScale_h = FLOAT1;
	m_bSplash = false;
}

ioSplashWeapon::~ioSplashWeapon()
{
}

void ioSplashWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_fSplashScale_w = rkLoader.LoadFloat_e( "splash_scale_w", FLOAT1 );
	m_fSplashScale_h = rkLoader.LoadFloat_e( "splash_scale_h", FLOAT1 );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "splash_effect", "", szBuf, MAX_PATH );
	m_SplashEffect = szBuf;
}

void ioSplashWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_bSplash )
	{
		SetWeaponDead();
		return;
	}

	if( !m_SplashEffect.IsEmpty() )
	{
		pStage->CreateMapEffect( m_SplashEffect,
								 GetPosition(),
								 GetScale() );

		ioWeapon::PlayExplosionSound();
	}

	UpdateWorldAttackBox( fTimePerSec, false );
	m_bSplash = true;
}

ioWeapon::WeaponType ioSplashWeapon::GetType() const
{
	return WT_SPLASH;
}

ActionStopType ioSplashWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioSplashWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioSplashWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	ioBaseChar *pOwner = GetOwner();

	if( bSetNull || !pOwner )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
	}
	else
	{
		m_WorldAttackBox = pOwner->GetAttackCollisionBox( m_CollisionType );
		m_WorldAttackBox.SetExtents( 0, m_WorldAttackBox.GetExtents(0) * m_fSplashScale_h );
		m_WorldAttackBox.SetExtents( 1, m_WorldAttackBox.GetExtents(1) * m_fSplashScale_w );

		m_WorldAttackBoxForMap = m_WorldAttackBox;
	}
}
