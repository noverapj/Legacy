#include "StdAfx.h"

#include "ioCalendar.h"
#include "ioDateHelp.h"

ioCalendarSet::ioCalendarSet()
{
	SetClear();
	m_eDateType   = DT_LAST_DAY;
}

ioCalendarSet::ioCalendarSet( DateType eType )
{
	SetClear();
	m_eDateType = eType;
}

ioCalendarSet::ioCalendarSet( DWORD dwDate )
{
	SetClear();
	if( dwDate != 0 )
		SetTournamentDate( dwDate );
}

ioCalendarSet::ioCalendarSet( DWORD dwDate, DateType eType )
{
	SetClear();
	if( dwDate != 0 )
		SetTournamentDate( dwDate );

	m_eDateType = eType;
}

ioCalendarSet::ioCalendarSet( const ioCalendarSet& rhs )
{
	m_iYear		 = rhs.m_iYear;
	m_iMonth	 = rhs.m_iMonth;
	m_iDay		 = rhs.m_iDay;
	m_iDayOfWeek = rhs.m_iDayOfWeek;
	m_iHour		 = rhs.m_iHour;
	m_iMin		 = rhs.m_iMin;
}

ioCalendarSet::ioCalendarSet( const ioCalendarSet& rhs, DateType eType )
{
	m_iYear		 = rhs.m_iYear;
	m_iMonth	 = rhs.m_iMonth;
	m_iDay		 = rhs.m_iDay;
	m_iDayOfWeek = rhs.m_iDayOfWeek;
	m_iHour		 = rhs.m_iHour;
	m_iMin		 = rhs.m_iMin;
	m_eDateType  = eType;
}

ioCalendarSet::~ioCalendarSet()
{

}

void ioCalendarSet::SetClear()
{
	m_iYear		 = 0;
	m_iMonth	 = 0;
	m_iDay		 = 0;
	m_iDayOfWeek = 0;
	m_iHour		 = 0;
	m_iMin		 = 0;
}

void ioCalendarSet::SetTodayLocalDate()
{
	m_iYear		 = DateHelp::GetTodayOfYear();
	m_iMonth	 = DateHelp::GetTodayOfMon();
	m_iDay		 = DateHelp::GetTodayOfDay();
	m_iDayOfWeek = DateHelp::GetTodayOfWeek();
	m_iHour		 = DateHelp::GetTodayOfHour();
	m_iMin		 = DateHelp::GetTodayOfMin();
}

void ioCalendarSet::SetTodayServerDate()
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );
	m_iYear		 = rkServerTime.wYear;
	m_iMonth	 = rkServerTime.wMonth;
	m_iDay		 = rkServerTime.wDay;
	m_iDayOfWeek = rkServerTime.wDayOfWeek;
	m_iHour		 = rkServerTime.wHour;
	m_iMin		 = rkServerTime.wMinute;
}

void ioCalendarSet::SetCalendarCompareType( DateType eType )
{
	m_eDateType = eType;
}

void ioCalendarSet::SetTournamentDate( DWORD dwDate )
{	
	m_iYear  = DateHelp::GetDateYear( dwDate );
	m_iMonth = DateHelp::GetDateMonth( dwDate );
	m_iDay   = DateHelp::GetDateDay( dwDate );
	m_iHour  = DateHelp::GetDateHour( dwDate );
	m_iMin   = DateHelp::GetDateMinute( dwDate );
}

DWORD ioCalendarSet::GetDate()
{
	if( isEmpty() )
		return 0;
	else
		return DateHelp::ConvertDwDate( m_iYear, m_iMonth, m_iDay, m_iHour, m_iMin );
}

bool ioCalendarSet::isEmpty()
{
	if( m_eDateType == DT_LAST_HOUR )
	{
		if( m_iYear == 0 && m_iMonth == 0 && m_iDay == 0 && m_iHour == 0 )
			return true;
	}
	else if( m_eDateType == DT_LAST_HOUR )
	{
		if( m_iYear == 0 && m_iMonth == 0 && m_iDay == 0 && m_iHour == 0 && m_iMin == 0 )
			return true;
	}
	else
	{
		if( m_iYear == 0 && m_iMonth == 0 && m_iDay == 0 )
			return true;
	}

	return false;
}

bool ioCalendarSet::operator==( const ioCalendarSet &rhs )
{
	if( m_eDateType == DT_LAST_HOUR )
	{
		if( m_iYear == rhs.m_iYear && m_iMonth == rhs.m_iMonth && m_iDay == rhs.m_iDay && m_iHour == rhs.m_iHour )
			return true;
	}
	else if( m_eDateType == DT_LAST_MIN )
	{
		if( m_iYear == rhs.m_iYear && m_iMonth == rhs.m_iMonth && m_iDay == rhs.m_iDay && m_iHour == rhs.m_iHour && m_iMin == rhs.m_iMin )
			return true;
	}
	else
	{
		if( m_iYear == rhs.m_iYear && m_iMonth == rhs.m_iMonth && m_iDay == rhs.m_iDay )
			return true;
	}

	return false;
}

bool ioCalendarSet::operator>( const ioCalendarSet &rhs )
{

	if( m_eDateType == DT_LAST_HOUR )
	{
		if( DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, m_iHour, 0, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, 0, DateHelp::PT_MIN ) < 0 )
			return true;
	}
	else if( m_eDateType == DT_LAST_MIN )
	{
		if( DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, m_iHour, m_iMin, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, rhs.m_iMin, DateHelp::PT_HOUR ) < 0 )
			return true;
	}
	else
	{
		if( DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, 0, 0, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, 0, 0 ) < 0 )
			return true;
	}

	return false;
}

bool ioCalendarSet::operator<( const ioCalendarSet &rhs )
{
	if( m_eDateType == DT_LAST_HOUR )
	{
		if( 0 < DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, m_iHour, 0, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, 0, DateHelp::PT_HOUR ) )
			return true;
	}
	else if( m_eDateType == DT_LAST_MIN )
	{
		if( 0 < DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, m_iHour, m_iMin, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, rhs.m_iMin, DateHelp::PT_MIN ) )
			return true;
	}
	else
	{
		if( 0 < DateHelp::GetDatePeriod( m_iYear, m_iMonth, m_iDay, 0, 0, rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, 0, 0 ) )
			return true;
	}

	return false;
}

bool ioCalendarSet::operator<=( const ioCalendarSet &rhs )
{
	if( operator==(rhs) )
		return true;
	else if( operator<(rhs) )
		return true;

	return false;
}

bool ioCalendarSet::operator>=( const ioCalendarSet &rhs )
{
	if( operator==(rhs) )
		return true;
	else if( operator>(rhs) )
		return true;

	return false;
}

ioCalendarSet ioCalendarSet::operator+( int iMin )
{
	ioCalendarSet kSet = (*this);
	DateHelp::IncreaseMin( kSet.m_iYear, kSet.m_iMonth, kSet.m_iDay, kSet.m_iHour, kSet.m_iMin, iMin );

	return kSet;
}

int ioCalendarSet::operator-( const ioCalendarSet& rhs )
{
	if( m_eDateType == DT_LAST_HOUR )
	{
		return DateHelp::GetDatePeriod( rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, 0, m_iYear, m_iMonth, m_iDay, m_iHour, 0, DateHelp::PT_HOUR );
	}
	else if( m_eDateType == DT_LAST_MIN )
	{
		return DateHelp::GetDatePeriod( rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, rhs.m_iHour, rhs.m_iMin, m_iYear, m_iMonth, m_iDay, m_iHour, m_iMin, DateHelp::PT_MIN );
	}
	else
	{
		return DateHelp::GetDatePeriod( rhs.m_iYear, rhs.m_iMonth, rhs.m_iDay, 0, 0, m_iYear, m_iMonth, m_iDay, 0, 0, DateHelp::PT_DAY );
	}
}