
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioSkillAttackBuff.h"

#include "BuffDefine.h"

ioSkillAttackBuff::ioSkillAttackBuff()
{
}

ioSkillAttackBuff::ioSkillAttackBuff( const ioSkillAttackBuff &rhs )
	: ioBuff( rhs ),
	m_nCallWeapon( rhs.m_nCallWeapon ),
	m_stCreatorAddBuff( rhs.m_stCreatorAddBuff )
{
}

ioSkillAttackBuff::~ioSkillAttackBuff()
{
}

void ioSkillAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_nCallWeapon	= rkLoader.LoadInt_e( "call_weapon_idx", 0 );
	rkLoader.LoadString( "call_creator_buff_name", "", szBuf, MAX_PATH );
	m_stCreatorAddBuff = szBuf;
}

ioBuff* ioSkillAttackBuff::Clone()
{
	return new ioSkillAttackBuff( *this );
}

void ioSkillAttackBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

}

bool ioSkillAttackBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioSkillAttackBuff::ProcessBuff( float fTimePerSec )
{
	if( GetOwner() == NULL )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar* pOwner = GetOwner();

	ioBaseChar *pCreator = GetOwner()->GetBaseChar( m_CreateChar );
	if( pCreator == NULL )
	{
		SetReserveEndBuff();
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage == NULL )
	{
		SetReserveEndBuff();
		return;
	}

	if( pOwner->GetState() == CS_USING_SKILL )
	{
		ioSkill* pActiveSkill = pOwner->GetCurActiveSkill();
		if( pActiveSkill )
		{
			for( int iSlot = 0; iSlot < 4; iSlot++ )
			{
				ioItem *pItem = pOwner->GetEquipedItem( iSlot );
				ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
				if( !pItem || !pSkill )
					continue;

				if( pSkill == pActiveSkill )
				{
					pItem->SetCurSkillGauge( 0.f );
					break;
				}

			}

			//pCreator->ReserveAddNewBuff( m_stCreatorAddBuff, pCreator->GetCharName(), "", NULL );
			pCreator->AddNewBuff( m_stCreatorAddBuff, pCreator->GetCharName(), "", NULL );
			CreateWeapon(false);
		}
		
		SetReserveEndBuff();
		return;
	}

	if( m_dwBuffEndTime < FRAMEGETTIME() )
		SetReserveEndBuff();
}

void ioSkillAttackBuff::EndBuff()
{
	ioBuff::EndBuff();
}


void ioSkillAttackBuff::CreateWeapon( bool bSend )
{
	if( GetOwner() == NULL )
	{
		SetReserveEndBuff();
		return;
	}

	ioBaseChar *pCreator = GetOwner()->GetBaseChar( m_CreateChar );
	if( pCreator == NULL )
	{
		SetReserveEndBuff();
		return;
	}
	pCreator->IncreaseWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nCallWeapon;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pCreator->GetWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = GetOwner()->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pCreator->GetWorldPosition() - GetOwner()->GetWorldPosition();
	pCreator->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	/*if( bSend )
	{
	SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
	kPacket << m_pOwner->GetCharName();
	kPacket << GetName();
	kPacket << true;

	P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}



void ioSkillAttackBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	/*ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	bool bSuccess;
	rkPacket >> bSuccess;

	if( bSuccess )
		CreateSuccusWeapon(false);
	else
		CreateFailWeapon(false);

	SetReserveEndBuff();*/
}
