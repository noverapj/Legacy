
#include "stdafx.h"

#include "ioZoneAffector.h"

ioZoneAffector::ioZoneAffector()
{
}

ioZoneAffector::ioZoneAffector( const ioZoneAffector &rhs )
{
}

ioZoneAffector::~ioZoneAffector()
{

}

ioZoneAffector* ioZoneAffector::CreateAffector( ZoneAffectorType eType )
{
	switch( eType )
	{
	case ZAT_NONE:
		return NULL;
	case ZAT_FIRE_ZONE:
		return new ioFireZoneAffector;
	case ZAT_POISON_ZONE:
		return new ioPoisonZoneAffector;
	}

	LOG.PrintTimeAndLog( 0, "ioZoneAffector::CreateAffector - %d Unknown Type", eType );
	return NULL;
}

//----------------------------------------------------------

ioFireZoneAffector::ioFireZoneAffector()
{
	m_fFireDamage = 0.0f;
}

ioFireZoneAffector::ioFireZoneAffector( const ioFireZoneAffector &rhs )
: ioZoneAffector( rhs ),
m_FireBuff( rhs.m_FireBuff ),
m_fFireDamage( rhs.m_fFireDamage )
{
}

ioFireZoneAffector::~ioFireZoneAffector()
{
}

void ioFireZoneAffector::LoadProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	
	rkLoader.LoadString_e( "fire_buff", "", szBuf, MAX_PATH );
	m_FireBuff = szBuf;

	m_fFireDamage = rkLoader.LoadFloat_e( "fire_damage", 10 );
}

ioZoneAffector::ZoneAffectorType ioFireZoneAffector::GetType() const
{
	return ZAT_FIRE_ZONE;
}

void ioFireZoneAffector::ApplyToReceiver( ioWorldEventReceiver *pReceiver )
{
	if( pReceiver )
	{
		pReceiver->OnFireZoneDamage( m_fFireDamage, m_FireBuff );
	}
}

//----------------------------------------------------------

ioPoisonZoneAffector::ioPoisonZoneAffector()
{
}

ioPoisonZoneAffector::ioPoisonZoneAffector( const ioPoisonZoneAffector &rhs )
: ioZoneAffector( rhs )
{
}

ioPoisonZoneAffector::~ioPoisonZoneAffector()
{
}

void ioPoisonZoneAffector::LoadProperty( ioINILoader &rkLoader )
{
}

ioZoneAffector::ZoneAffectorType ioPoisonZoneAffector::GetType() const
{
	return ZAT_POISON_ZONE;
}

void ioPoisonZoneAffector::ApplyToReceiver( ioWorldEventReceiver *pReceiver )
{
	if( pReceiver )
	{
		pReceiver->OnPoisonZoneDamage();
	}
}








