#include "stdafx.h"
#include "ioAITimer.h"

void	ioAITimer::StartTimer( unsigned int newDuration )
{
	m_nStartTimeStamp = FRAMEGETTIME();
	
	if( newDuration != 0 )
		m_nDuration = newDuration;

#ifdef _DEBUG_AI_TIMER_
	LOG.PrintTimeAndLog( 0, "Timer started at %u with duration %u", m_nStartTimeStamp, m_nDuration );
#endif
}

void	ioAITimer::StartTimerOnce( unsigned int newDuration )
{
	if( newDuration != 0 )
		m_nDuration = newDuration;

	if( m_nDuration > 0 && m_nStartTimeStamp == 0 )
	{
		StartTimer();
	}
}

bool	ioAITimer::HasTimeoutOccured()
{
	bool bRet = (	( m_nDuration > 0 ) &&
				( m_nStartTimeStamp != 0 ) &&
				( FRAMEGETTIME() - m_nStartTimeStamp > m_nDuration ));

#ifdef _DEBUG_AI_TIMER_
	if( bRet )
		LOG.PrintTimeAndLog( 0, "Time over detected at %u, started at %u with duration %u", 
									FRAMEGETTIME(), m_nStartTimeStamp, m_nDuration );
#endif

	return bRet;
}

void	ioAITimer::StopTimer()
{
#ifdef _DEBUG_AI_TIMER_
	LOG.PrintTimeAndLog( 0, "Timer stopped at %u", FRAMEGETTIME() );
#endif

	m_nStartTimeStamp = 0;
	m_nDuration = 0;
}
