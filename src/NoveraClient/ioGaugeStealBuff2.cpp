

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioGaugeStealBuff2.h"

ioGaugeStealBuff2::ioGaugeStealBuff2()
{
	m_dwStealCheckTime = 0;
}

ioGaugeStealBuff2::ioGaugeStealBuff2( const ioGaugeStealBuff2 &rhs )
: ioBuff( rhs ),
 m_fRecoveryRate( rhs.m_fRecoveryRate ),
 m_dwStealTic( rhs.m_dwStealTic ),
 m_fStealValue( rhs.m_fStealValue ),
 m_bDisbaleWeapon( rhs.m_bDisbaleWeapon ),
 m_bDisbaleArmor( rhs.m_bDisbaleArmor ),
 m_bDisbaleHelmet( rhs.m_bDisbaleHelmet ),
 m_bDisbaleCloak( rhs.m_bDisbaleCloak )
{
	m_dwStealCheckTime = 0;
}

ioGaugeStealBuff2::~ioGaugeStealBuff2()
{
}

void ioGaugeStealBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fRecoveryRate = rkLoader.LoadFloat_e( "steal_recovery_rate", 0.0f );

	m_dwStealTic = (DWORD)rkLoader.LoadInt_e( "steal_tic", 0 );
	m_fStealValue = rkLoader.LoadFloat_e( "steal_value", 0.0f );

	m_bDisbaleWeapon = rkLoader.LoadBool_e( "disable_weapon", false );
	m_bDisbaleArmor = rkLoader.LoadBool_e( "disable_armor", false );
	m_bDisbaleHelmet = rkLoader.LoadBool_e( "disable_helmet", false );
	m_bDisbaleCloak = rkLoader.LoadBool_e( "disable_cloak", false );
}

ioBuff* ioGaugeStealBuff2::Clone()
{
	return new ioGaugeStealBuff2( *this );
}

void ioGaugeStealBuff2::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_dwStealCheckTime = FRAMEGETTIME();
}

bool ioGaugeStealBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_dwStealCheckTime = FRAMEGETTIME();
	return true;
}

void ioGaugeStealBuff2::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwStealCheckTime > 0 && m_dwStealCheckTime+m_dwStealTic < dwCurTime )
	{
		m_dwStealCheckTime = FRAMEGETTIME();
		ioItem *pOwnerItem = NULL;
		if( m_pOwner )
			pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );

		for( int iSlot=ES_WEAPON; iSlot < ES_OBJECT; ++iSlot )
		{
			ioItem *pItem = NULL;
			ioBaseChar *pCreator = GetCreator();
			if( pCreator )
				pItem = pCreator->GetEquipedItem( iSlot );

			if( !pItem ) continue;
			//if( pItem == m_pOwnerItem ) continue;
			if( pItem == pOwnerItem ) continue;

			if( m_bDisbaleWeapon && pItem->GetType() == ioItem::IT_WEAPON )
				continue;
			if( m_bDisbaleArmor && pItem->GetType() == ioItem::IT_ARMOR )
				continue;
			if( m_bDisbaleHelmet && pItem->GetType() == ioItem::IT_HELMET )
				continue;
			if( m_bDisbaleCloak && pItem->GetType() == ioItem::IT_CLOAK )
				continue;

			float fCurGauge = pItem->GetCurSkillGuage() + m_fStealValue;
			pItem->SetCurSkillGauge( fCurGauge );
		}
	}
}

void ioGaugeStealBuff2::EndBuff()
{
	ioBuff::EndBuff();
}


