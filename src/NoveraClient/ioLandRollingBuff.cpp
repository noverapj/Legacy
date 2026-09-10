

#include "stdafx.h"



#include "ioBaseChar.h"
#include "ioLandRollingBuff.h"

ioLandRollingBuff::ioLandRollingBuff()
{
}

ioLandRollingBuff::ioLandRollingBuff( const ioLandRollingBuff &rhs )
: ioBuff( rhs ),
m_fEnableTimeRate( rhs.m_fEnableTimeRate ),
m_fExtendEnableTimeRate( rhs.m_fExtendEnableTimeRate ),
m_bInitialGauge( rhs.m_bInitialGauge ),
m_RollingAction( rhs.m_RollingAction ),
m_fRollingActionRate( rhs.m_fRollingActionRate ),
m_GetUpAction( rhs.m_GetUpAction ),
m_fGetUpActionRate( rhs.m_fGetUpActionRate ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_LandRollType( rhs.m_LandRollType )
{
}

ioLandRollingBuff::~ioLandRollingBuff()
{
}

void ioLandRollingBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_LandRollType = (LandRollType)rkLoader.LoadInt_e( "land_roll_type", LRT_NORMAL );

	m_bInitialGauge = rkLoader.LoadBool_e( "initial_gauge", false );
	m_fEnableTimeRate = rkLoader.LoadFloat_e( "enable_check_time_rate", 0.0f );
	m_fExtendEnableTimeRate = rkLoader.LoadFloat_e( "enable_extend_check_time_rate", 0.0f );

	rkLoader.LoadString_e( "rolling_action", "", szBuf, MAX_PATH );
	m_RollingAction = szBuf;
	rkLoader.LoadString_e( "getup_action", "", szBuf, MAX_PATH );
	m_GetUpAction = szBuf;

	m_fRollingActionRate = rkLoader.LoadFloat_e( "rolling_action_rate", FLOAT1 );
	m_fGetUpActionRate = rkLoader.LoadFloat_e( "getup_action_rate", FLOAT1 );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "rolling_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "rolling_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "rolling_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vForceInfoList.push_back( kInfo );
	}
}

ioBuff* ioLandRollingBuff::Clone()
{
	return new ioLandRollingBuff( *this );
}

void ioLandRollingBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioLandRollingBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGlobalTime ) )
		return false;

	return true;
}

void ioLandRollingBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		if( m_pOwner->GetState() == CS_JUMP )
		{
			CheckLandRolling();
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		if( m_pOwner->GetState() == CS_JUMP )
		{
			CheckLandRolling();
		}

		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		if( m_pOwner->GetState() == CS_JUMP )
		{
			CheckLandRolling();
		}

		CheckCallingAreaWeapon();
		break;
	}
}

void ioLandRollingBuff::CheckLandRolling()
{
	if( !m_pOwner ) return;
	if( !m_pOwner->IsNeedProcess() ) return;
	if( m_pOwner->IsPrisonerMode() ) return;
	if( m_pOwner->IsCatchMode() ) return;

	if( m_pOwner->HasDisableSkillFlameBuff() )
		return;
	
	if( !m_pOwner->IsDirKeyDoubleClick() )
		return;

	if( !m_pOwner->IsCanLandRollingState( m_fEnableTimeRate, m_fExtendEnableTimeRate ) )
		return;

	m_pOwner->ClearDirDoubleClick();

	ioItem *pOwnerItem = m_pOwner->GetEquipedItem( m_ItemName );

	/*if( m_pOwnerItem && m_bInitialGauge )
	{
		m_pOwnerItem->SetCurSkillGauge( 0.0f );
	}*/

	if( pOwnerItem && m_bInitialGauge )
	{
		pOwnerItem->SetCurSkillGauge( 0.0f );
	}

	switch( m_LandRollType )
	{
	case LRT_NORMAL:
		m_pOwner->SetDownRollingState( m_RollingAction, m_fRollingActionRate,
									   m_GetUpAction, m_fGetUpActionRate,
									   m_vForceInfoList );
		return;
	case LRT_DASH:
		m_pOwner->SetTargetRotToDirKey( m_pOwner->GetDirKey() );
		m_pOwner->SetDashState( false );
		m_pOwner->SendDashState( false );
		return;
	}
}

void ioLandRollingBuff::EndBuff()
{
	ioBuff::EndBuff();
}
