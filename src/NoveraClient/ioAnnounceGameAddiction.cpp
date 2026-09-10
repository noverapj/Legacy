#include "stdafx.h"
#include "ioAnnounceGameAddiction.h"


ioAnnounceGameAddiction::ioAnnounceGameAddiction(void)
{
	m_dwPreAnnouceTime = 0;
	m_bAnnounce        = false;
	m_dwPassedHour     = 0;
}


ioAnnounceGameAddiction::~ioAnnounceGameAddiction(void)
{
}

void ioAnnounceGameAddiction::SetTime( DWORD dwRealGetTime )
{
	m_dwPreAnnouceTime = dwRealGetTime;
}

void ioAnnounceGameAddiction::ProcessTime( DWORD dwCurrentRealGetTime )
{
	m_bAnnounce = false;
	if( dwCurrentRealGetTime - m_dwPreAnnouceTime < ANNOUNCE_INTERVAL_TIME )
		return;
	m_bAnnounce = true;
	m_dwPreAnnouceTime = dwCurrentRealGetTime;
	m_dwPassedHour++;
}

bool ioAnnounceGameAddiction::IsAnnounce()
{
	return m_bAnnounce;
}

int ioAnnounceGameAddiction::GetPassedHour()
{
	return m_dwPassedHour;
}