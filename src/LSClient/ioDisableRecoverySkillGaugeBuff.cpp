

#include "stdafx.h"

#include "ioDisableRecoverySkillGaugeBuff.h"
#include "ioBaseChar.h"

ioDisableRecoverySkillGaugeBuff::ioDisableRecoverySkillGaugeBuff()
{
}

ioDisableRecoverySkillGaugeBuff::ioDisableRecoverySkillGaugeBuff( const ioDisableRecoverySkillGaugeBuff &rhs )
	: ioBuff( rhs ),
	m_DisableType( rhs.m_DisableType ),
	m_DummyCharName( rhs.m_DummyCharName ),
	m_iArmorType( rhs.m_iArmorType )
{
}

ioDisableRecoverySkillGaugeBuff::~ioDisableRecoverySkillGaugeBuff()
{
}

void ioDisableRecoverySkillGaugeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_DisableType = (DisableRecoverySkillGaugeType)rkLoader.LoadInt_e("disable_type", DRST_DUMMY );

	rkLoader.LoadString_e( "dummy_char_name", "", szBuf, MAX_PATH );
	m_DummyCharName = szBuf;

	m_iArmorType = rkLoader.LoadInt_e( "equip_armor_type", 0 );
}

ioBuff* ioDisableRecoverySkillGaugeBuff::Clone()
{
	return new ioDisableRecoverySkillGaugeBuff( *this );
}

void ioDisableRecoverySkillGaugeBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( pOwner )
		pOwner->AddDisableRecoverySkillGaugeBuff( GetName() );
}

void ioDisableRecoverySkillGaugeBuff::StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime )
{
	ioBuff::StartBuff( pOwner );
	m_dwBuffEndTime = FRAMEGETTIME() + GetBuffDuration();

	if( pOwner )
		pOwner->AddDisableRecoverySkillGaugeBuff( GetName() );
}

bool ioDisableRecoverySkillGaugeBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( pOwner )
		pOwner->AddDisableRecoverySkillGaugeBuff( GetName() );

	return true;
}

void ioDisableRecoverySkillGaugeBuff::ProcessBuff( float fTimePerSec )
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

	CheckDisableBuff();
}

void ioDisableRecoverySkillGaugeBuff::CheckDisableBuff()
{
	switch( m_DisableType )
	{
	case DRST_DUMMY: 
		{
			if( m_DummyCharName.IsEmpty() )
			{
				SetReserveEndBuff();
			}

			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyCharToName( GetOwner(), m_DummyCharName );
			if( !pDummy )
			{
				SetReserveEndBuff();
			}
		}
		break;
	case DRST_DURATION:
		{
			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case DRST_OBJECTITEM:
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				ioObjectItem* pItem = pOwner->GetObject();
				if( !pItem )
					SetReserveEndBuff();
			}
		}
		break;
	}
}

void ioDisableRecoverySkillGaugeBuff::EndBuff()
{
	if( m_pOwner )
		m_pOwner->RemoveDisableRecoverySkillGaugeBuff( GetName() );

	ioBuff::EndBuff();
}