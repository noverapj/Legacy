
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioSkillGaugeCheckBuff.h"

#include "BuffDefine.h"

ioSkillGaugeCheckBuff::ioSkillGaugeCheckBuff()
{
}

ioSkillGaugeCheckBuff::ioSkillGaugeCheckBuff( const ioSkillGaugeCheckBuff &rhs )
	: ioBuff( rhs ),
	m_nSuccessWeapon( rhs.m_nSuccessWeapon ),
	m_nFailWeapon( rhs.m_nFailWeapon )
{
}

ioSkillGaugeCheckBuff::~ioSkillGaugeCheckBuff()
{
}

void ioSkillGaugeCheckBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	m_nSuccessWeapon	= rkLoader.LoadInt_e( "success_weapon_idx", 0 );
	m_nFailWeapon		= rkLoader.LoadInt_e( "fail_weapon_idx", 0 );
}

ioBuff* ioSkillGaugeCheckBuff::Clone()
{
	return new ioSkillGaugeCheckBuff( *this );
}

void ioSkillGaugeCheckBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

}

bool ioSkillGaugeCheckBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioSkillGaugeCheckBuff::ProcessBuff( float fTimePerSec )
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

	ioPlayStage *pStage = GetOwner()->GetCreator();
	if( pStage == NULL )
	{
		SetReserveEndBuff();
		return;
	}
	
	if( pStage->IsNetworkMode() )
	{
		if( GetOwner()->IsNeedProcess() )
		{
			if( GetOwner()->ClearEquipItemGauge( ioGaugeClearBuff::GCT_FULL_HIGHT ) )
				CreateSuccusWeapon( true );
			else
				CreateFailWeapon( true );

			SetReserveEndBuff();
			return;
		}
	}
	else
	{
		if( GetOwner()->ClearEquipItemGauge( ioGaugeClearBuff::GCT_FULL_HIGHT ) )
			CreateSuccusWeapon( false );
		else
			CreateFailWeapon( false );

		SetReserveEndBuff();
		return;
	}

	if( m_dwBuffEndTime < FRAMEGETTIME() )
		SetReserveEndBuff();
}

void ioSkillGaugeCheckBuff::EndBuff()
{
	ioBuff::EndBuff();
}


void ioSkillGaugeCheckBuff::CreateSuccusWeapon( bool bSend )
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

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nSuccessWeapon;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pCreator->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = GetOwner()->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pCreator->GetWorldPosition() - GetOwner()->GetWorldPosition();
	pCreator->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if( bSend )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << true;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioSkillGaugeCheckBuff::CreateFailWeapon( bool bSend )
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

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = m_nFailWeapon;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;

	kFireTime.dwWeaponIdx = pCreator->IncreaseWeaponIndexBase();
	kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

	D3DXVECTOR3 vPos = GetOwner()->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pCreator->GetWorldPosition() - GetOwner()->GetWorldPosition();
	pCreator->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );

	if( bSend )
	{
		SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
		kPacket << m_pOwner->GetCharName();
		kPacket << GetName();
		kPacket << false;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioSkillGaugeCheckBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
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

	SetReserveEndBuff();
}
