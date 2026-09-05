#include "StdAfx.h"
#include "ioDateHelp.h"

namespace DateHelp
{
	int GetTodayOfYear()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );

		return 1900 + tmday->tm_year;
	}

	int GetTodayOfMon()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );
		return tmday->tm_mon+1;
	}

	int GetTodayOfWeek()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );
		return tmday->tm_wday;
	}

	int GetTodayOfDay()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );

		return tmday->tm_mday;
	}

	int GetTodayOfHour()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );

		return tmday->tm_hour;
	}

	int GetTodayOfMin()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );

		return tmday->tm_min;
	}

	int GetTodayOfSec()
	{
		time_t today;
		time( &today );
		struct tm* tmday;
		tmday = localtime( &today );

		return tmday->tm_sec;
	}
	
	const CTime& GetCurrTime()
	{
		static CTime kCurrTime;
		if( P2PNetwork::IsNetworkPlaying() )
		{
			SYSTEMTIME rkServerTime;
			g_MyInfo.GetServerTime( rkServerTime );	
			kCurrTime = CTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, rkServerTime.wSecond );
		}
		else
		{
			kCurrTime = CTime( GetTodayOfYear(), GetTodayOfMon(), GetTodayOfDay(), GetTodayOfHour(), GetTodayOfMin(), GetTodayOfSec() );
		}

		return kCurrTime;
	}

	int GetDateYear( DWORD dwDate )
	{
		return 2000 + ( dwDate / 100000000 );
	}

	int GetDateMonth( DWORD dwDate )
	{
		return ( dwDate % 100000000 ) / 1000000;
	}

	int GetDateDay( DWORD dwDate )
	{
		return ( dwDate % 1000000 ) / 10000;
	}

	int GetDateHour( DWORD dwDate )
	{
		return ( dwDate % 10000 ) / 100;
	}

	int GetDateMinute( DWORD dwDate )
	{
		return ( dwDate % 100 );
	}

	DWORD ConvertDwDate( int iYear, int iMonth, int iDay, int iHour, int iMin )
	{
		DWORD dwDate = 0;

		dwDate += ( iYear - 2000 ) * 100000000;
		dwDate += iMonth * 1000000;
		dwDate += iDay * 10000;
		dwDate += iHour * 100;
		dwDate += iMin;

		return dwDate;
	}
	
	void ConvertCTime( DWORD dwDate, OUT CTime& Time )
	{
		Time = CTime( GetDateYear( dwDate ), GetDateMonth( dwDate ), GetDateDay( dwDate ), GetDateHour( dwDate ), GetDateMinute( dwDate ), 0 );
	}

	bool isLeapYear( int iYear )
	{
		if( ( iYear - 2000 ) % 4  == 0 )
			return true;

		return false;
	}

	bool isOddMonth( int iMonth )
	{
		int nm[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 };

		if( nm[iMonth-1] == 31 )
			return true;
		else
			return false;
	}

	void IncreaseMonth( int& iYear, int& iMonth, int& iDay )
	{
		if( 12 == iMonth )
		{
			iMonth = 1;
			iYear++;
		}
		else
		{
			iMonth++;
		}

		iDay = 1;	
	}

	void DecreaseMonth( int& iYear, int& iMonth, int& iDay )
	{
		if( 1 == iMonth )
		{
			iMonth = 12;
			iYear--;
		}
		else
		{
			iMonth--;
		}

		iDay = 1;
	}


	void IncreaseDay( int& iYear, int& iMonth, int& iDay )
	{
		int nm[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 };
		if( isLeapYear(iYear) )
			nm[1] = 29;

		if( iDay < nm[iMonth-1] )
		{
			iDay++;
		}
		else
		{
			if( iMonth == 12 )
			{
				iYear++;
				iMonth = 1;
				iDay = 1;
			}
			else
			{
				iMonth++;
				iDay = 1;
			}
		}
	}

	void DecreaseDay( int& iYear, int& iMonth, int& iDay )
	{
		int nm[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 };
		if( isLeapYear(iYear) )
			nm[1] = 29;

		if( 1 < iDay )
		{
			iDay--;
		}
		else
		{
			if( iMonth == 1 )
			{
				iYear--;
				iMonth = 12;
				iDay = nm[11];
			}
			else
			{
				iMonth--;
				iDay = nm[iMonth-1];
			}
		}
	}

	void IncreaseMin( int& iYear, int& iMonth, int& iDay, int& iHour, int& iMin, int iIncraseMin )
	{
		int nm[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 };
		if( isLeapYear(iYear) )
			nm[1] = 29;

		int iCurDay = 0;

		if( 60 <= iIncraseMin + iMin )
		{
			iHour += (iIncraseMin + iMin) / 60;
			iMin = (iIncraseMin + iMin) % 60;

			if( 24 <= iHour )
			{
				iCurDay = iDay + (iHour / 24);
				iHour   = iHour % 24;

				int iRestDay = iCurDay - iDay;
				for( int i = 0; i < iRestDay; ++i )
					IncreaseDay( iYear, iMonth, iDay );

			}
		}
		else
		{
			iMin += iIncraseMin;
		}

	}

	int GetDatePeriod( int iStartYear,
		int iStartMonth,
		int iStartDay,
		int iStartHour,
		int iStartMin,
		int iEndYear,
		int iEndMonth,
		int iEndDay,
		int iEndHour,
		int iEndMin,
		PeriodType eType )
	{

		//difftime 함수를 이용해서 두 날짜의 차이값이 허용 범위를 초과 하게 되면 0이되므로 
		//최소 날짜를 2000, 1, 1일로 설정
		time_t rawtime;
		time ( &rawtime );
		tm StartTime;
		StartTime = (*localtime( &rawtime ));
		StartTime.tm_year = max( 100, iStartYear - 1900 );
		StartTime.tm_mon  = max( 0, iStartMonth - 1 );
		StartTime.tm_mday = max( 1, iStartDay );
		StartTime.tm_hour = iStartHour;
		StartTime.tm_min  = iStartMin;

		tm EndTime;
		EndTime = (*localtime( &rawtime ));
		EndTime.tm_year = max( 100, iEndYear - 1900 );
		EndTime.tm_mon  = max( 0, iEndMonth - 1 );
		EndTime.tm_mday = max( 1, iEndDay );
		EndTime.tm_hour = iEndHour;
		EndTime.tm_min  = iEndMin;

		time_t tStart = mktime( &StartTime );
		time_t tEnd = mktime( &EndTime );

		if( eType == PT_DAY )
			return difftime( tEnd, tStart ) / ( 60 * 60 * 24 );
		else if( eType == PT_HOUR )
			return difftime( tEnd, tStart ) / ( 60 * 60 );
		else
			return difftime( tEnd, tStart ) / 60;
	}

	int GetMonthDayPeriod( int num, int mon )
	{ 
		int nm[12]={ 31,28,31,30,31,30,31,31,30,31,30,31 }, i = 0;
		if(mon  > 0 && mon < 13)
		{  
			if(mon == 2)
			{
				// 윤달인지 체크 한다
				nm[1] = (num%4) ? 28 : (num%100) ? 29 : (num%400) ? 28 : 29;
			}
			i = nm[mon-1];
		}
		return  i;
	}

	int GetWeekDay( int iYear, int iMonth, int iDay )
	{	
		if( iMonth == 1 || iMonth == 2 )
			iYear -= 1;
		iMonth = ( iMonth + 9 ) % 12 + 1;
		int iY = iYear % 100;
		int iCentury = iYear / 100;
		int iWeek = ( ( 13 * iMonth - 1 ) / 5 + iDay + iY + iY/4 + iCentury/4 - 2*iCentury) % 7;
		if( iWeek < 0 )
			iWeek = ( iWeek + 7 ) % 7;
		return max( 0, min( 6, iWeek ) );
	}

	char* GetWeekDayString( int index )
	{
		char a[7][3]={"일", "월", "화", "수", "목", "금", "토"};
		static char b[3]={"일"};

		if( index >= 7 )  // 예외 처리
			return b;

		b[0] = a[index][0]; b[1] = a[index][1];
		return b;
	}

	char* GetWeekDayString( DWORD dwDate )
	{
		int Y = GetDateYear( dwDate );
		int M = GetDateMonth( dwDate );
		int D = GetDateDay( dwDate );

		return GetWeekDayString( Y, M, D );
	}

	char* GetWeekDayString( int num, int mon, int day )
	{
		int iy=num-1, nal = iy * 365;

		 // 윤달을 더 해주어서 입력한 앞달까지의 총 일수를 구한다.
		nal += iy/4-iy/100+iy/400;

		for(iy=1; iy<mon; iy++) 
		{
			nal +=  GetMonthDayPeriod( num, iy);
		}

		return GetWeekDayString( ( nal+day ) % 7);
	}
	
	time_t ConvertSecondTime( int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond )
	{
		struct tm base_time;
		base_time.tm_year = iYear - 1900;
		base_time.tm_mon  = iMonth - 1;
		base_time.tm_mday = iDay;	
		base_time.tm_hour = iHour;
		base_time.tm_min  = iMinute;		
		base_time.tm_sec  = iSecond;
		base_time.tm_isdst= 0;

		return mktime( &base_time );
	}
}