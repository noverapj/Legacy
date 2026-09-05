
#include "stdafx.h"

#include "ioCallRandomBuff.h"

#include "ioBaseChar.h"

ioCallRandomBuff::ioCallRandomBuff()
{
}

ioCallRandomBuff::ioCallRandomBuff( const ioCallRandomBuff &rhs )
: ioBuff( rhs ),
  m_bDisableDownState( rhs.m_bDisableDownState ),
  m_dwCallRandomBuffTime( rhs.m_dwCallRandomBuffTime ),
  m_vCallRandomBuff( rhs.m_vCallRandomBuff )
{
}

ioCallRandomBuff::~ioCallRandomBuff()
{
}

void ioCallRandomBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	m_dwCallRandomBuffTime = (DWORD)rkLoader.LoadInt_e( "call_random_buff_time", 0 );

	int iSize = rkLoader.LoadInt_e( "call_random_buff_cnt", 0 );
	
	m_vCallRandomBuff.clear();
	m_vCallRandomBuff.reserve( iSize );
	
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "call_random%d_buff_cnt", i+1 );
		int iBuffSize = rkLoader.LoadInt( szKey, 0 );
		
		ioHashStringVec kList;
		kList.clear();
		kList.reserve( iBuffSize );
		
		for( int j=0; j<iBuffSize; ++j )
		{
			wsprintf_e( szKey, "call_random%d_buff%d", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioHashString szBuffName = szBuf;
			kList.push_back( szBuf );
		}

		m_vCallRandomBuff.push_back( kList );
	}
}

ioBuff* ioCallRandomBuff::Clone()
{
	return new ioCallRandomBuff( *this );
}

void ioCallRandomBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioCallRandomBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioCallRandomBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	CheckCallingRandomBuff();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

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
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

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

bool ioCallRandomBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioCallRandomBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioCallRandomBuff::CheckCallingRandomBuff()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( m_dwBuffStartTime + m_dwCallRandomBuffTime < FRAMEGETTIME() )
	{
		IORandom random;
		random.SetRandomSeed( pOwner->GetRandomSeed() );
		int iArray = random.Random( m_vCallRandomBuff.size() );

		int iSize = m_vCallRandomBuff[iArray].size();
		for( int i=0; i<iSize; ++i )
		{
			//pOwner->AddNewBuff( m_vCallRandomBuff[iArray][i], m_CreateChar, "", NULL );
			pOwner->ReserveAddNewBuff( m_vCallRandomBuff[iArray][i], m_CreateChar, "", NULL );
		}

		SetReserveEndBuff();
	}
}