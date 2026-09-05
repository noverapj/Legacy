
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioNotMoveProtectBuff.h"


ioNotMoveProtectBuff::ioNotMoveProtectBuff()
{
	m_bSetState = false;
}

ioNotMoveProtectBuff::ioNotMoveProtectBuff( const ioNotMoveProtectBuff &rhs )
: ioBuff( rhs ),
 m_ChangeMaterial( rhs.m_ChangeMaterial ),
 m_LoopAnimation( rhs.m_LoopAnimation ),
 m_LoopEffect( rhs.m_LoopEffect )
{
	m_bSetState = false;
}

ioNotMoveProtectBuff::~ioNotMoveProtectBuff()
{
}

void ioNotMoveProtectBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "change_material", "", szBuf, MAX_PATH );
	m_ChangeMaterial = szBuf;
	
	rkLoader.LoadString_e( "not_move_animation", "", szBuf, MAX_PATH );
	m_LoopAnimation = szBuf;
	rkLoader.LoadString_e( "not_move_effect", "", szBuf, MAX_PATH );
	m_LoopEffect = szBuf;
}

ioBuff* ioNotMoveProtectBuff::Clone()
{
	return new ioNotMoveProtectBuff( *this );
}

void ioNotMoveProtectBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_bSetState = false;
}

bool ioNotMoveProtectBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_bSetState = false;
	return true;
}

void ioNotMoveProtectBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if( m_bSetState )
	{
		if( m_pOwner->GetState() != CS_NOT_MOVE_PROTECT )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}
	}
	else if( m_pOwner->GetState() == CS_NOT_MOVE_PROTECT )
	{
		m_bSetState = true;
		m_pOwner->AttachEffect( m_LoopEffect, NULL );

		ioEntityGroup *pGrp = m_pOwner->GetGroup();

		if( !m_LoopAnimation.IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );

			m_pOwner->ClearAllEquipEffect();

			if( !m_ChangeMaterial.IsEmpty() )
			{
				Help::ChangeMaterial( m_pOwner->GetGroup(), m_ChangeMaterial, Setting::SoftSkinning() );
			}
		}
	}
	else if( m_pOwner->GetState() != CS_USING_SKILL )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
}

void ioNotMoveProtectBuff::EndBuff()
{
	if( m_pOwner->GetState() == CS_NOT_MOVE_PROTECT )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}

	if( !m_ChangeMaterial.IsEmpty() )
	{
		Help::RestoreMaterial( m_pOwner->GetGroup(), Setting::SoftSkinning() );
		m_pOwner->ApplyModifiedMaterial();
	}

	m_pOwner->RestoreAllEquipEffect();
	m_pOwner->RestroeAllEquipItemCustomize();


	m_pOwner->EndEffect( m_LoopEffect );

	ioBuff::EndBuff();
}
