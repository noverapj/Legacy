

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioKingScaleBuff.h"

ioKingScaleBuff::ioKingScaleBuff()
{
	m_fCurScaleRate = FLOAT1;
	m_bRemoveBuffByDrop = false;
}

ioKingScaleBuff::ioKingScaleBuff( const ioKingScaleBuff &rhs )
: ioBuff( rhs ),
  m_ChangeSkeletonHm( rhs.m_ChangeSkeletonHm ),
  m_ChangeSkeletonHw( rhs.m_ChangeSkeletonHw ),
  m_ChangeSkeletonEm( rhs.m_ChangeSkeletonEm ),
  m_ChangeSkeletonEw( rhs.m_ChangeSkeletonEw ),
  m_ChangeSkeletonDm( rhs.m_ChangeSkeletonDm ),
  m_ChangeSkeletonDw( rhs.m_ChangeSkeletonDw ),
  m_bDisableDownState( rhs.m_bDisableDownState )
{
	m_fScaleRate = rhs.m_fScaleRate;
	m_bRemoveBuffByDrop = rhs.m_bRemoveBuffByDrop;

	m_fCurScaleRate = FLOAT1;
}

ioKingScaleBuff::~ioKingScaleBuff()
{
}

void ioKingScaleBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szSkeleton[MAX_PATH];
	rkLoader.LoadString_e( "scale_skeleton_hm", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonHm = szSkeleton;
	rkLoader.LoadString_e( "scale_skeleton_hw", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonHw = szSkeleton;
	rkLoader.LoadString_e( "scale_skeleton_em", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonEm = szSkeleton;
	rkLoader.LoadString_e( "scale_skeleton_ew", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonEw = szSkeleton;
	rkLoader.LoadString_e( "scale_skeleton_dm", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonDm = szSkeleton;
	rkLoader.LoadString_e( "scale_skeleton_dw", "", szSkeleton, MAX_PATH );
	m_ChangeSkeletonDw = szSkeleton;

	m_fScaleRate = rkLoader.LoadFloat_e( "scale_rate", FLOAT1 );
	m_fScaleRate = max( 0.0f, m_fScaleRate );

	m_bRemoveBuffByDrop = rkLoader.LoadBool_e( "remove_buff_drop", false );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioKingScaleBuff::Clone()
{
	return new ioKingScaleBuff( *this );
}

void ioKingScaleBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_fCurScaleRate = m_fScaleRate;
	pOwner->SetKingScaleRate( m_fCurScaleRate );

	int iRaceType = pOwner->GetRaceDetailType();

	switch( iRaceType )
	{
	case RDT_HUMAN_MAN:
		m_CurSkeleton = m_ChangeSkeletonHm;
		break;
	case RDT_HUMAN_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonHw;
		break;
	case RDT_ELF_MAN:
		m_CurSkeleton = m_ChangeSkeletonEm;
		break;
	case RDT_ELF_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonEw;
		break;
	case RDT_DWARF_MAN:
		m_CurSkeleton = m_ChangeSkeletonDm;
		break;
	case RDT_DWARF_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonDw;
		break;
	}

	pOwner->SetKingScaleSkeleton( m_CurSkeleton );
}

bool ioKingScaleBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_fCurScaleRate = m_fScaleRate;
	pOwner->SetKingScaleRate( m_fCurScaleRate );

	int iRaceType = pOwner->GetRaceDetailType();

	switch( iRaceType )
	{
	case RDT_HUMAN_MAN:
		m_CurSkeleton = m_ChangeSkeletonHm;
		break;
	case RDT_HUMAN_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonHw;
		break;
	case RDT_ELF_MAN:
		m_CurSkeleton = m_ChangeSkeletonEm;
		break;
	case RDT_ELF_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonEw;
		break;
	case RDT_DWARF_MAN:
		m_CurSkeleton = m_ChangeSkeletonDm;
		break;
	case RDT_DWARF_WOMAN:
		m_CurSkeleton = m_ChangeSkeletonDw;
		break;
	}

	pOwner->SetKingScaleSkeleton( m_CurSkeleton );

	return true;
}

void ioKingScaleBuff::ProcessBuff( float fTimePerSec )
{
	if( m_bRemoveBuffByDrop && m_pOwner->GetState() == CS_DROP_ZONE_DOWN  )
	{
		SetReserveEndBuff();
		return;
	}

	CheckOwnerStateCheck();

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
}

void ioKingScaleBuff::EndBuff()
{
	m_pOwner->RemoveKingScaleSkeleton( m_CurSkeleton );
	m_pOwner->RemoveKingScaleRate( m_fCurScaleRate );

	ioBuff::EndBuff();
}

void ioKingScaleBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}
