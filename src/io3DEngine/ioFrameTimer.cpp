

#include "stdafx.h"

#include "ioFrameTimer.h"

template<> ioFrameTimer* Singleton< ioFrameTimer >::ms_Singleton = 0;

static bool s_bWin2kModule = true;

void ioFrameTimer::SetWindowsModule( bool bWin2k )
{
	s_bWin2kModule = bWin2k;
}

inline unsigned __int64 getMisteryCount() 
{ 
   __asm 
   { 
      mov         edx,dword ptr ds:[7FFE000Ch] 
      mov         eax,dword ptr ds:[7FFE0008h] 
   } 
}

DWORD GetSafeTime()
{
	if( s_bWin2kModule )
		return static_cast< DWORD >( getMisteryCount()/10000 ); 

	return timeGetTime();
}

DWORD GetPerfCheckTime()
{
	if( s_bWin2kModule )
		return static_cast< DWORD >( getMisteryCount()/1000 );

	return timeGetTime() * 10;
}

ioFrameTimer::ioFrameTimer()
{
    m_fSecsPerFrame = 1.0f / 60.0f;
	m_fCurFrameGap  =FLOAT1000 / 60.0f;

	m_dwTicksStart = m_dwTicks = GetSafeTime();

	m_dwLoopGetTime  = 0;
	m_dwFrameCounter = 1;
	m_fTimeRate = 1.0f;
	
	m_bTimerPause = false;
}

ioFrameTimer::~ioFrameTimer()
{
}

void ioFrameTimer::Start( float fFramesPerSec )
{
    m_fSecsPerFrame = 1.0f / fFramesPerSec;
	m_fCurFrameGap  = m_fSecsPerFrame *FLOAT1000;
	m_dwTicksStart = m_dwTicks = GetSafeTime();

#ifndef SHIPPING
	if (m_fTimeRate != 1.0f)
		m_dwTicksStart = m_dwTicks = GetSafeTime() * m_fTimeRate;
#endif
}

DWORD ioFrameTimer::Frame()
{
	DWORD dwTick = GetSafeTime();
#ifndef SHIPPING
	if (m_fTimeRate != 1.0f)
		dwTick = GetSafeTime() * m_fTimeRate;
#endif

	if( dwTick > m_dwTicks )
	{
		if( !m_bTimerPause )
		{
			float fTickPerFrame = (float)(dwTick - m_dwTicks);
			m_fSecsPerFrame = fTickPerFrame /FLOAT1000;
			m_fCurFrameGap  = fTickPerFrame;
		}
		else
		{
			m_fSecsPerFrame = 0.0f;
			m_fCurFrameGap  = 0.0f;
		}
	}
	else
	{
		m_fSecsPerFrame = 0.0f;
		m_fCurFrameGap  = 0.0f;
	}

	m_dwTicks = dwTick;
	m_dwLoopGetTime = GetTotalSec();
	m_dwFrameCounter++;

	return m_dwLoopGetTime;
}

void ioFrameTimer::SetTimeRate( float fRate )
{
	m_fTimeRate = max( 0.0f, fRate );
}

void ioFrameTimer::SetTimerPause( bool bPause )
{
	m_bTimerPause = bPause;
}

void ioFrameTimer::SetCurFrameGapForce( float fFrameGap )
{
	m_fSecsPerFrame = fFrameGap /FLOAT1000;
	m_fCurFrameGap  = fFrameGap;
}

void ioFrameTimer::SetLoopGetTime( DWORD dwLoopTime )
{
	m_dwLoopGetTime = dwLoopTime;
}

DWORD ioFrameTimer::GetTotalSec()
{
#ifndef SHIPPING
	if (m_fTimeRate != 1.0f)
		return GetSafeTime() * m_fTimeRate - m_dwTicksStart;
#endif
	return GetSafeTime() - m_dwTicksStart;
}

DWORD ioFrameTimer::GetPerfTotalSec()
{
#ifndef SHIPPING
	if (m_fTimeRate != 1.0f)
		return GetPerfCheckTime() * m_fTimeRate - m_dwTicksStart;
#endif
	return GetPerfCheckTime() - m_dwTicksStart;
}

ioFrameTimer& ioFrameTimer::GetSingleton()
{
	return Singleton<ioFrameTimer>::GetSingleton();
}
