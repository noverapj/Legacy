//
// CFrameTimer
// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//
#include "FrameTimerDLL.h"

//////////////////////////////////////////////////////////////////////////////
// CFrameTimer implementation //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

__EX CFrameTimer FrameTimer;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


CFrameTimer::CFrameTimer()
{
    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &m_qwTicksPerSec))
        m_qwTicksPerSec = 1000;

    m_fTicksPerSec  = (float) (__int64) m_qwTicksPerSec;

    m_fFramesPerSec = 0.0f;
    m_fSecsPerFrame = 0.0f;
    m_qwTicksStart = 0;
    m_qwTicks = 0;
    m_qwTicksPerFrame = 0;
}

void 
CFrameTimer::Start(float fFramesPerSec)
{
    m_fFramesPerSec = fFramesPerSec;
    m_fSecsPerFrame = 1.0f / fFramesPerSec;
    m_qwTicksPerFrame = m_qwTicksPerSec / (__int64) fFramesPerSec;
    m_qwTicksStart = m_qwTicks = GetTicks();
}

void 
CFrameTimer::SetFrame()
{
    UINT64 qw;
    qw = GetTicks();

    if(qw != m_qwTicks)
    {
        m_qwTicksPerFrame = qw - m_qwTicks;
        m_qwTicks = qw;

        m_fFramesPerSec = 0.0f * m_fFramesPerSec +
            1.0f * (m_fTicksPerSec / (float) (__int64) m_qwTicksPerFrame);

        if(m_fFramesPerSec < 1.0f)
            m_fFramesPerSec = 1.0f;

        m_fSecsPerFrame = 1.0f / m_fFramesPerSec;
    }
	else
	{
		m_fSecsPerFrame = 1.0f;
		m_fFramesPerSec = 1.0f;
	}
		
}

UINT64
CFrameTimer::GetTicks()
{
	UINT64 qw;
 	if(QueryPerformanceCounter((LARGE_INTEGER *) &qw))
    {
        while(!qw)
			QueryPerformanceCounter((LARGE_INTEGER *) &qw);
    }
	else	
		qw = (UINT64) timeGetTime();		

	return qw;
}

DWORD  CFrameTimer::GetTotalSec()
{
	UINT64 qw = GetTicks();

	return (DWORD)(1000*(__int64)(qw - m_qwTicksStart)/m_qwTicksPerSec);
	
}
