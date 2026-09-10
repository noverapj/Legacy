
#include "stdafx.h"
#include "ioTargetControlBuff.h"
#include "ioWeaponControlSkill.h"

ioTargetControlBuff::ioTargetControlBuff()
{
}

ioTargetControlBuff::ioTargetControlBuff( const ioTargetControlBuff &rhs )
	: ioBuff( rhs ),
	m_bDropZoneCheck( rhs.m_bDropZoneCheck ),
	m_fControlOutRange( rhs.m_fControlOutRange ),
	m_bDisableKeyInputCheck( rhs.m_bDisableKeyInputCheck ),
	m_dwDropZoneCheckTime( rhs.m_dwDropZoneCheckTime ),
	m_fChangeMoveSpeed( rhs.m_fChangeMoveSpeed )
{
	m_bNotTargetNpc = true;
	m_iCurUseCount  = 0;
	m_bControlSkill = false;
	m_dwDropZoneStartTime = 0;
}

ioTargetControlBuff::~ioTargetControlBuff()
{
}

void ioTargetControlBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );
	
	m_bNotTargetNpc = true;

	m_bDropZoneCheck		= rkLoader.LoadBool_e( "drop_zone_check", false );
	m_fControlOutRange		= rkLoader.LoadFloat_e( "control_out_range", 0.0f );
	m_bDisableKeyInputCheck = rkLoader.LoadBool_e( "disable_key_input_check", false );
	m_dwDropZoneCheckTime = rkLoader.LoadInt_e( "drop_zone_check_time", 0 );
	m_fChangeMoveSpeed = rkLoader.LoadFloat_e( "change_move_speed", 0.0f );
}

ioBuff* ioTargetControlBuff::Clone()
{
	return new ioTargetControlBuff( *this );
}

void ioTargetControlBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	if( pOwner )
	{
		if ( m_bDisableKeyInputCheck )
		{
			pOwner->EnableKeyInputCheck( false );
		}		
		pOwner->EnableKeyInputMust( false );

		ioBaseChar *pCreater = pOwner->GetBaseChar( m_CreateChar );
		ioSkill *pOwnerSkill = pCreater->GetEquipedSkillByItemName( m_ItemName );
		ioWeaponControlSkill *pControl = ToWeaponControlSkill( pOwnerSkill );
		if( pControl )
		{
			pOwner->SetState( CS_DELAY );
			pOwner->ClearDirKey();

			m_bControlSkill = pControl->SetControlBuffInfo( pCreater, m_OwnerName, m_Name );
			if( !m_bControlSkill )
				SetReserveEndBuff();
		}
	}

	m_iCurUseCount = 0;
}

bool ioTargetControlBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_iCurUseCount = 0;

	return true;
}

void ioTargetControlBuff::RemoveOldTargetControlBuff( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetReserveEndBuff( BT_TARGET_CONTROL, this );
}

void ioTargetControlBuff::ProcessStartCreatorSync()
{
	ioBaseChar* pCreator = GetCreator();
	ioBaseChar* pOwner   = GetOwner();
	if( !pCreator || !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( pOwner->GetState() != CS_DELAY && pOwner->GetState() != CS_ATTACK && pOwner->GetState() != CS_RUN )
		return;

	if( 0 < m_iCurUseCount )
		return;

	switch( pCreator->GetState() )
	{
	case CS_RUN:
		{
			pCreator->SetTargetControlState( CS_RUN );
			m_iCurUseCount++;
		}
		break;
	case CS_ATTACK:
		{
			pCreator->SetTargetControlState( CS_ATTACK );
			m_iCurUseCount++;
		}
		break;
	case CS_DELAY:
		{
			pCreator->SetTargetControlState( CS_DELAY );
			m_iCurUseCount++;
		}
		break;
	}
}

void ioTargetControlBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			ProcessStartCreatorSync();
			ProcessRun( fTimePerSec );
			ioBuff::ProcessBuff( fTimePerSec );
			CheckDropZone();
			CheckOutRange();
		}
		break;
	case OT_PASSIVE:
		{
			CheckCallingAreaWeapon();
		}
		break;
	}
}

void ioTargetControlBuff::ProcessRun( float fTimePerSec )
{
	ioBaseChar* pCreator = GetCreator();
	ioBaseChar* pOwner   = GetOwner();
	if( !pCreator || !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_bControlSkill )
		return;

	switch( pOwner->GetState() )
	{
	case CS_ATTACK:
	case CS_RUN:
		if( IsLive() )
		{
			D3DXVECTOR3 vDir = pCreator->GetMoveDir();
			pOwner->SetTargetRotToDir( vDir, true, false );
		}
		break;
	}
}

void ioTargetControlBuff::EndBuff()
{
	ioBuff::EndBuff();

	ioBaseChar* pOwner = GetOwner();
	if( pOwner )
	{
		if ( m_bDisableKeyInputCheck )
		{
			pOwner->EnableKeyInputCheck( true );
		}	
		pOwner->EnableKeyInputMust( true );
		switch( pOwner->GetState() )
		{
		case CS_RUN:
		case CS_DELAY:
			pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			break;
		}

		ioBaseChar *pCreater = pOwner->GetBaseChar( m_CreateChar );
		if( pCreater )
		{
			ioSkill *pOwnerSkill = pCreater->GetEquipedSkillByItemName( m_ItemName );
			ioWeaponControlSkill *pControl = ToWeaponControlSkill( pOwnerSkill );
			if( pControl )
				pControl->ClearControlBuffInfo( pCreater );
		}
	}
	m_bControlSkill = false;
	m_dwDropZoneStartTime = 0;
}

void ioTargetControlBuff::CheckDropZone()
{
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( pOwner->IsCharDropZonePos() )
	{
		if( m_bDropZoneCheck )
		{
			SetReserveEndBuff();
			return;
		}
		else if( m_dwDropZoneCheckTime != 0 )
		{
			DWORD dwCurTime = FRAMEGETTIME();
			if( m_dwDropZoneStartTime == 0 )
				m_dwDropZoneStartTime = dwCurTime;

			if( dwCurTime > m_dwDropZoneStartTime + m_dwDropZoneCheckTime )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else if( m_dwDropZoneStartTime != 0 )
		m_dwDropZoneStartTime = 0;
}

void ioTargetControlBuff::CheckOutRange()
{
	if ( m_fControlOutRange == 0.0f )
		return;

	ioBaseChar* pCreator = GetCreator();
	ioBaseChar* pOwner = GetOwner();
	if( !pCreator || !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	D3DXVECTOR3 vDiff = pOwner->GetWorldPosition() - pCreator->GetWorldPosition();
	float fDistanceSq = D3DXVec3LengthSq( &vDiff );

	if( m_fControlOutRange * m_fControlOutRange < fDistanceSq )
	{
		SetReserveEndBuff();
		return;
	}
}

