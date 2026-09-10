

#include "stdafx.h"

#include "ioTickTimer.h"

ioTickTimer::ioTickTimer()
{
	m_dwPreTick = 0;
	m_dwTickGap = 0;

	m_dwStartFPSTick = 0;
	m_iFrameCnt = 0;
	m_fFPS = 0.0f;
}

ioTickTimer::~ioTickTimer()
{
}

void ioTickTimer::UpdateTick()
{
	DWORD dwCurTick = timeGetTime();

	if( m_dwPreTick > 0 )
	{
		m_dwTickGap = dwCurTick - m_dwPreTick;

		if( dwCurTick - m_dwStartFPSTick > 1000 )
		{
			m_fFPS = (float)m_iFrameCnt / (float)( dwCurTick - m_dwStartFPSTick );
			m_fFPS *= FLOAT1000;
			
			m_dwStartFPSTick = dwCurTick;
			m_iFrameCnt = 0;
		}
	}

	m_iFrameCnt++;
	m_dwPreTick = dwCurTick;		 
}
