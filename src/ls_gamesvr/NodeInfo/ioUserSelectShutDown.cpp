#include "stdafx.h"
#include "./ioUserSelectShutDown.h"
#include "../Util/ioHashString.h"

#ifndef TEST_SUITE_ON
#include "../Define.h"
#include "../../ioINILoader/ioINILoader.h"
#include "../EtcHelpFunc.h"
#endif

int ioUserSelectShutDown::m_iEarlyShutDownMinutes = 0;

ioUserSelectShutDown::ioUserSelectShutDown( void )
{
	Initialize();
}

ioUserSelectShutDown::~ioUserSelectShutDown( void )
{

}

void ioUserSelectShutDown::LoadINI()
{
#ifndef TEST_SUITE_ON
	ioINILoader kLoader( "config/sp2_select_shut_down.ini" );
	kLoader.SetTitle( "Info" );
	m_iEarlyShutDownMinutes = kLoader.LoadInt( "EarlyShutDownMinutes", 0 );
#endif
}

void ioUserSelectShutDown::SetSelectShutDown( int iShutDown, const DBTIMESTAMP &rkDts )
{
	if( iShutDown == 0 )
		m_bShutDownUser = false;
	else if( iShutDown == 1 )
		m_bShutDownUser = true;
	else
		m_bShutDownUser = false;

	if( !m_bShutDownUser )
		return;

	SYSTEMTIME st;
#ifndef TEST_SUITE_ON
	st = Help::GetSafeValueForCTimeConstructor( rkDts.year, rkDts.month, rkDts.day, rkDts.hour, rkDts.minute, rkDts.second );
#else
	st.wYear  = rkDts.year;
	st.wMonth = rkDts.month;
	st.wDay   = rkDts.day;
	st.wHour  = rkDts.hour;
	st.wMinute= rkDts.minute;
	st.wSecond= rkDts.second;
	st.wMilliseconds = 0;
#endif

	CTime ShutDownTime( st );
	CTimeSpan GapTime( 0, 0, -m_iEarlyShutDownMinutes, 0 );
	m_ShutDownTime = ShutDownTime + GapTime;
}

bool ioUserSelectShutDown::IsShutDown( CTime CurrentTime )
{
	if( !m_bShutDownUser )
		return false;

	CTimeSpan GapTime = m_ShutDownTime - CurrentTime;

	if( GapTime.GetTotalMinutes() > 0 )
		return false;

	return true;
}

void ioUserSelectShutDown::Initialize()
{
	m_bShutDownUser    = false;
	m_ShutDownTime = 0;
}