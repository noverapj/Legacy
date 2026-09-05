

#include "stdafx.h"

#include "ioTimer.h"
#include "ioFrameTimer.h"

ioTimer::ioTimer()
{
	m_fTotalTime = 0.0f;
	m_fPresetTime = 0.0f;
}

ioTimer::~ioTimer()
{
}

void ioTimer::InitTimer()
{
	m_fTotalTime = 0.0f;
	m_fPresetTime = 0.0f;
}

bool ioTimer::IsTimePast( DWORD dwTime )
{
	m_fTotalTime += g_FrameTimer.GetCurFrameGap();

	if( m_fTotalTime > dwTime )
	{
		m_fTotalTime = 0.0f;
		return true;
	}

	return false;
}

bool ioTimer::IsPresetTimePast( DWORD dwTime )
{
	m_fTotalTime += g_FrameTimer.GetCurFrameGap();

	if( m_fTotalTime + m_fPresetTime > dwTime )
	{
		m_fPresetTime = m_fTotalTime + m_fPresetTime - dwTime;
		m_fTotalTime = 0.0f;
		return true;
	}

	return false;
}