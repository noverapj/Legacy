
#include "stdafx.h"

#include "ioTraceMineStateBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponAttribute.h"


#include "WeaponDefine.h"

ioTraceMineStateBuff::ioTraceMineStateBuff()
{
}

ioTraceMineStateBuff::ioTraceMineStateBuff( const ioTraceMineStateBuff &rhs )
: ioBuff( rhs ),
m_iMaxMineCount( rhs.m_iMaxMineCount ),
m_iArmorType( rhs.m_iArmorType ),
m_iCurMineCnt( rhs.m_iCurMineCnt )
{
}

ioTraceMineStateBuff::~ioTraceMineStateBuff()
{
}

void ioTraceMineStateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_iMaxMineCount = rkLoader.LoadInt_e( "mine_max_cnt", 1 );
	m_iCurMineCnt = (int)m_iMaxMineCount;
	m_iArmorType = rkLoader.LoadInt_e( "equip_armor_type", 0 );
}

ioBuff* ioTraceMineStateBuff::Clone()
{
	return new ioTraceMineStateBuff( *this );
}

void ioTraceMineStateBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	
	if( pOwner )
		pOwner->SetMineTraceBuff( GetName() );
}

bool ioTraceMineStateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( pOwner )
		pOwner->SetMineTraceBuff( GetName() );

	return true;
}

void ioTraceMineStateBuff::ProcessBuff( float fTimePerSec )
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
}

int ioTraceMineStateBuff::GetCurMineCnt()
{
	int iCurCnt = min( m_iMaxMineCount, g_WeaponMgr.GetCurTrackingMineCnt( GetOwner() ) );
	return m_iMaxMineCount - iCurCnt;
}
int ioTraceMineStateBuff::GetMaxMineCnt()
{ 
	return m_iMaxMineCount;
}

ioItem* ioTraceMineStateBuff::GetBuffOwnerItem()
{
	return ioBuff::GetOwnerItem();
}

void ioTraceMineStateBuff::EndBuff()
{
	ioBuff::EndBuff();
}
