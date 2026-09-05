#include "stdafx.h"

#include "ioBurnBuff.h"


#include "ioNpcChar.h"

ioBurnBuff::ioBurnBuff()
{
}

ioBurnBuff::ioBurnBuff( const ioBurnBuff &rhs )
: ioBuff( rhs ),
 m_AddEntity( rhs.m_AddEntity ),
 m_dwMinTime( rhs.m_dwMinTime ),
 m_dwMaxTime( rhs.m_dwMaxTime ),
 m_dwMinTimeByNpc( rhs.m_dwMinTimeByNpc ),
 m_dwMaxTimeByNpc( rhs.m_dwMaxTimeByNpc ),
 m_dwMinTimeByBossNpc( rhs.m_dwMinTimeByBossNpc ),
 m_dwMaxTimeByBossNpc( rhs.m_dwMaxTimeByBossNpc ),
 m_dwTeamMinTime( rhs.m_dwTeamMinTime ),
 m_dwTeamMaxTime( rhs.m_dwTeamMaxTime ),
 m_bEnableTimeEnd( rhs.m_bEnableTimeEnd )
{
}

ioBurnBuff::~ioBurnBuff()
{
}

void ioBurnBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "add_mesh", "", szBuf, MAX_PATH );
	m_AddEntity = szBuf;

	m_dwMaxTime = rkLoader.LoadInt_e( "max_time", 0 );
	m_dwMinTime = rkLoader.LoadInt_e( "min_time", 0 );
	m_dwMaxTimeByNpc = rkLoader.LoadInt_e( "max_time_npc", m_dwMaxTime );
	m_dwMinTimeByNpc = rkLoader.LoadInt_e( "min_time_npc", m_dwMinTime );
	m_dwMaxTimeByBossNpc = rkLoader.LoadInt_e( "max_time_boss_npc", m_dwMaxTimeByNpc );
	m_dwMinTimeByBossNpc = rkLoader.LoadInt_e( "min_time_boss_npc", m_dwMinTimeByNpc );

	m_dwTeamMaxTime = rkLoader.LoadInt_e( "team_max_time", 0 );
	m_dwTeamMinTime = rkLoader.LoadInt_e( "team_min_time", 0 );

	m_bEnableTimeEnd = rkLoader.LoadBool_e( "enable_time_end", false );
}

DWORD ioBurnBuff::GetMaxTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwMaxTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwMaxTimeByBossNpc;
	return m_dwMaxTimeByNpc;
}

DWORD ioBurnBuff::GetMinTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwMinTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwMinTimeByBossNpc;

	return m_dwMinTimeByNpc;
}

ioBuff* ioBurnBuff::Clone()
{
	return new ioBurnBuff( *this );
}

void ioBurnBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	if( !pOwner ) return;

	if( !m_AddEntity.IsEmpty() )
	{
		pOwner->AddEntity( m_AddEntity );
		pOwner->AddMaterialModifyExcept( m_AddEntity );
	}

	bool bEnableTeamAttack = pOwner->IsEnableTeamAttack();

	if( !bEnableTeamAttack && m_bTeamAttack )
		pOwner->SetFlameStateValue( m_dwTeamMinTime, m_dwTeamMaxTime );
	else
		pOwner->SetFlameStateValue( GetMinTime(), GetMaxTime() );	
}

bool ioBurnBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	if( !m_AddEntity.IsEmpty() )
	{
		pOwner->AddEntity( m_AddEntity );
		pOwner->AddMaterialModifyExcept( m_AddEntity );
	}

	pOwner->SetFlameStateValue( GetMinTime(), GetMaxTime() );

	return true;
}

void ioBurnBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_BLOW_WOUNDED && m_pOwner->GetState() != CS_BOUND_BLOW_WOUNDED )
	{
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
	
	if( m_bEnableTimeEnd )
	{
		if( m_dwBuffEndTime <= FRAMEGETTIME() )
		{
			m_pOwner->ResetFlameStateValue();
			SetReserveEndBuff();
			return;
		}
	}	
}

void ioBurnBuff::EndBuff()
{
	if( !m_AddEntity.IsEmpty() )
	{
		m_pOwner->DestroyEntity( m_AddEntity );
		m_pOwner->RemoveMaterialModifyExcept( m_AddEntity );
	}

	ioBuff::EndBuff();
}
