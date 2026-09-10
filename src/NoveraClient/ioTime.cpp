#pragma once
#include "StdAfx.h"
#include "ioTime.h"

namespace ioTime
{
	DWORD GetYear( DWORD dwDate )
	{
		return ioTime::DEFAULT_YEAR + ( dwDate / ioTime::DATE_YEAR_VALUE );
	}

	DWORD GetMonth( DWORD dwDate )
	{
		return ( dwDate % ioTime::DATE_YEAR_VALUE ) / ioTime::DATE_MONTH_VALUE;
	}

	DWORD GetDay( DWORD dwDate )
	{
		return ( dwDate % ioTime::DATE_MONTH_VALUE ) / ioTime::DATE_DAY_VALUE;
	}

	DWORD GetHour( DWORD dwDate )
	{
		return ( dwDate % ioTime::DATE_DAY_VALUE ) / ioTime::DATE_HOUR_VALUE;
	}

	DWORD GetMinute( DWORD dwDate )
	{
		return ( dwDate % ioTime::DATE_HOUR_VALUE );
	}
};