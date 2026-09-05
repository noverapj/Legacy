

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioAttackCountBuff.h"

ioAttackCountBuff::ioAttackCountBuff()
{

}

ioAttackCountBuff::ioAttackCountBuff( const ioAttackCountBuff &rhs )
	: ioBuff( rhs ),
	m_bUseWeaponEquipMesh( rhs.m_bUseWeaponEquipMesh ),
	m_stCheckBuff( rhs.m_stCheckBuff ),
	m_stCallEndBuff( rhs.m_stCallEndBuff )
{
}

ioAttackCountBuff::~ioAttackCountBuff()
{
}

void ioAttackCountBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "check_buff_name", "", szBuf, MAX_PATH );
	m_stCheckBuff = szBuf;

	rkLoader.LoadString_e( "call_buff_name", "", szBuf, MAX_PATH );
	m_stCallEndBuff = szBuf;

	m_bUseWeaponEquipMesh = rkLoader.LoadBool_e( "use_weapon_equip_mesh", false );
}

ioBuff* ioAttackCountBuff::Clone()
{
	return new ioAttackCountBuff( *this );
}

void ioAttackCountBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}
}

bool ioAttackCountBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( m_bUseWeaponEquipMesh && pOwner )
	{
		pOwner->SetChangeEquipMesh( m_ItemName, true );
	}

	return true;
}

void ioAttackCountBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) && m_pOwner->GetState() != CS_OBJECT_RUNNING_ATTACK )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}



void ioAttackCountBuff::EndBuff()
{
	if( m_bUseWeaponEquipMesh && m_pOwner )
	{
		m_pOwner->SetChangeEquipMesh( m_ItemName, false );
	}

	ioBuff::EndBuff();
}


void ioAttackCountBuff::AttackWound( ioBaseChar* pTarget, ioWeapon* pWeapon )
{
	if( !pTarget || !pWeapon )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar* pAttacker = pWeapon->GetOwner();

	ioBuff* pCheckBuf = pTarget->GetBuff( m_stCheckBuff );
	if( pCheckBuf )
	{
		if( pAttacker )
			CallEndBuff( pTarget, pAttacker->GetCharName() );
		
		SetReserveEndBuff();
		return;
	}
	else
	{
		if( pAttacker )
		{
			pTarget->AddNewBuff( m_stCheckBuff, pAttacker->GetCharName(), "", NULL );
			SendPacketBuff( pTarget, pAttacker, m_stCheckBuff );
		}
	}
}


void ioAttackCountBuff::CallEndBuff( ioBaseChar* pTarget, const ioHashString& stCreator )
{
	pTarget->AddNewBuff( m_stCallEndBuff, stCreator, "", NULL );

	ioBaseChar* pCreator = pTarget->GetBaseChar( stCreator );
	if( pTarget && pCreator )
		SendPacketBuff( pTarget, pCreator, m_stCallEndBuff );
}


void ioAttackCountBuff::SendPacketBuff( ioBaseChar* pTarget, ioBaseChar* pCreator, const ioHashString& stBuffName )
{
	SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
	kPacket << pTarget->GetCharName();
	kPacket << pCreator->GetCharName();
	kPacket << "";
	kPacket << pTarget->GetRandomSeed();
	kPacket << false;		// 타격횟수 체크
	kPacket << 0.0f;
	kPacket << 0.0f;
	kPacket << 0.0f;
	kPacket << COWT_NONE;				
	kPacket << 1;
	kPacket << stBuffName;
	kPacket << false;					// Use Force

	P2PNetwork::SendToAllPlayingUser( kPacket );
}