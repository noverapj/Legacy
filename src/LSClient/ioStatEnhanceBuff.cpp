

#include "stdafx.h"

#include "ioStatEnhanceBuff.h"

#include "ioBaseChar.h"

ioStatEnhanceBuff::ioStatEnhanceBuff()
{
}

ioStatEnhanceBuff::ioStatEnhanceBuff( const ioStatEnhanceBuff &rhs )
: ioBuff( rhs )
{
	m_EnhanceStat = rhs.m_EnhanceStat;
}

ioStatEnhanceBuff::~ioStatEnhanceBuff()
{
}

void ioStatEnhanceBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_EnhanceStat.m_fStrength	  = rkLoader.LoadFloat_e( "strength", 0.0f );
	m_EnhanceStat.m_fDexterity	  = rkLoader.LoadFloat_e( "dexterity", 0.0f );
	m_EnhanceStat.m_fIntelligence = rkLoader.LoadFloat_e( "intelligence", 0.0f );
	m_EnhanceStat.m_fVitality	  = rkLoader.LoadFloat_e( "vitality", 0.0f );
}

ioBuff* ioStatEnhanceBuff::Clone()
{
	return new ioStatEnhanceBuff( *this );
}

void ioStatEnhanceBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->ModifyExtraStat( m_EnhanceStat, true );
}

bool ioStatEnhanceBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->ModifyExtraStat( m_EnhanceStat, true );
	return true;
}

void ioStatEnhanceBuff::EndBuff()
{
	m_pOwner->ModifyExtraStat( m_EnhanceStat, false );
	
	ioBuff::EndBuff();
}
