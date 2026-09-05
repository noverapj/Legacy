

#include "stdafx.h"

#include "ioStatePriority.h"

ioStatePriority::ioStatePriority()
{
	for( int i=0 ; i<MAX_SPT_TYPE ; i++ )
	{
		m_Priority[i] = -1;
	}
}

ioStatePriority::~ioStatePriority()
{
}

void ioStatePriority::LoadPriority( ioINILoader &rkLoader )
{
	m_Priority[ SPT_DEFENSE ]		= rkLoader.LoadInt_e( "priority_defense", -1 );
	m_Priority[ SPT_DASH ]			= rkLoader.LoadInt_e( "priority_dash", -1 );
	m_Priority[ SPT_JUMP ]			= rkLoader.LoadInt_e( "priority_jump", -1 );
	m_Priority[ SPT_GETUP ]			= rkLoader.LoadInt_e( "priority_getup", -1 );
	m_Priority[ SPT_WOUNDED ]		= rkLoader.LoadInt_e( "priority_wounded", -1 );
	m_Priority[ SPT_ATTACK ]		= rkLoader.LoadInt_e( "priority_attack", -1 );
	m_Priority[ SPT_EXTEND_ATTACK ] = rkLoader.LoadInt_e( "priority_extend_attack", -1 );
	m_Priority[ SPT_DELAY ]			= rkLoader.LoadInt_e( "priority_delay", -1 );
	m_Priority[ SPT_RUN ]			= rkLoader.LoadInt_e( "priority_run", -1 );
}

int ioStatePriority::GetPriority( StatePriorityType eType )
{
	if( COMPARE( eType, SPT_DEFENSE, MAX_SPT_TYPE ) )
		return m_Priority[ eType ];

	return -1;
}

void ioStatePriority::SetPriority( StatePriorityType eType , int iValue)
{
	if( COMPARE( eType, SPT_DEFENSE, MAX_SPT_TYPE ) )
	{
		m_Priority[ eType ] = iValue;
	}
}