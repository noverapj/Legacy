
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioGaugeStealBuff.h"

ioGaugeStealBuff::ioGaugeStealBuff()
{
}

ioGaugeStealBuff::ioGaugeStealBuff( const ioGaugeStealBuff &rhs )
: ioBuff( rhs ),
 m_fStealRate( rhs.m_fStealRate )
{
}

ioGaugeStealBuff::~ioGaugeStealBuff()
{
}

void ioGaugeStealBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fStealRate = rkLoader.LoadFloat_e( "steal_rate", FLOAT1 );
}

ioBuff* ioGaugeStealBuff::Clone()
{
	return new ioGaugeStealBuff( *this );
}

void ioGaugeStealBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	SetBuffDuration( 0 );

	int iSlot = 0;
	float fCurBest = 0.0f;
	ioItem *pTargetItem = NULL;
	for( iSlot=0; iSlot < 4; ++iSlot )
	{
		ioItem *pItem = m_pOwner->GetEquipedItem( iSlot );
		if( !pItem ) continue;

		float fCurGauge = pItem->GetCurSkillGuage();
		if( fCurGauge > fCurBest )
		{
			fCurBest = fCurGauge;
			pTargetItem = pItem;
		}
	}

	if( pTargetItem )
		pTargetItem->SetCurSkillGauge( 0.0f );

	float fCurWorst = std::numeric_limits<float>::infinity();
	ioItem *pGoalItem = NULL;

	ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );
	for( iSlot=0; iSlot < 4; ++iSlot )
	{
		ioItem *pItem = NULL;
		ioBaseChar *pCreator = m_pOwner->GetBaseChar( m_CreateChar );
		if( pCreator )
			pItem = pCreator->GetEquipedItem( iSlot );

		if( !pItem ) continue;
		//if( pItem == m_pOwnerItem ) continue;
		if( pItem == pOwnerItem ) continue;

		float fCurGauge = pItem->GetCurSkillGuage();
		if( fCurGauge < fCurWorst )
		{
			fCurWorst = fCurGauge;
			pGoalItem = pItem;
		}
	}

	if( pGoalItem )
	{
		float fStealGauge = fCurBest * m_fStealRate;
		pGoalItem->SetCurSkillGauge( fCurWorst + fStealGauge );
	}
}

bool ioGaugeStealBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	SetBuffDuration( 0 );
	return true;
}

void ioGaugeStealBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_DELAY:
	case OT_SWITCH:
	case OT_ACTIVE:
	case OT_PASSIVE:
		SetReserveEndBuff();
		break;
	}
}

void ioGaugeStealBuff::EndBuff()
{
	ioBuff::EndBuff();
}


