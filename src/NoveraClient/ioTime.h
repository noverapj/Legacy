#pragma once
#include "StdAfx.h"

namespace ioTime
{
	enum
	{
		DEFAULT_YEAR    = 2010,			// 2010년은 DB에 저장하지 않는다. 즉 DateData의 년도가 0이면 2010이란 뜻이다. 1이면 2011년
		DATE_YEAR_VALUE = 100000000,    // 년까지 나눈다.
		DATE_MONTH_VALUE= 1000000,      // 월까지 나눈다.
		DATE_DAY_VALUE =  10000,        // 일까지 나눈다.
		DATE_HOUR_VALUE = 100,          // 시까지 나눈다.
	};

	DWORD GetYear( DWORD dwDate );
	DWORD GetMonth( DWORD dwDate );
	DWORD GetDay( DWORD dwDate );
	DWORD GetHour( DWORD dwDate );
	DWORD GetMinute( DWORD dwDate );
}