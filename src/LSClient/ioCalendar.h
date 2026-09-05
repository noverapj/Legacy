#pragma once

enum DateType
{
	DT_LAST_DAY   = 0,
	DT_LAST_HOUR  = 1,
	DT_LAST_MIN	  = 2,
};

class ioCalendarSet
{
protected:
	DateType m_eDateType;

public:
	int m_iYear;
	int m_iMonth;
	int m_iDay;
	int m_iDayOfWeek;
	int m_iHour;
	int m_iMin;

public:
	ioCalendarSet();
	ioCalendarSet( DWORD dwDate );
	ioCalendarSet( DateType eType );
	ioCalendarSet( DWORD dwDate, DateType eType );
	ioCalendarSet( const ioCalendarSet& rhs );
	ioCalendarSet( const ioCalendarSet& rhs, DateType eType );
	~ioCalendarSet();

public:
	void SetClear();
	void SetTodayLocalDate();
	void SetTodayServerDate();
	void SetCalendarCompareType( DateType eType );
	void SetTournamentDate( DWORD dwDate );

	DWORD GetDate();

public:
	bool isEmpty();

public:
	bool operator==( const ioCalendarSet &rhs );
	bool operator<=( const ioCalendarSet &rhs );
	bool operator>=( const ioCalendarSet &rhs );
	bool operator>( const ioCalendarSet &rhs );
	bool operator<( const ioCalendarSet &rhs );

	ioCalendarSet operator+( int iMin );
	int operator-( const ioCalendarSet& rhs );
};
