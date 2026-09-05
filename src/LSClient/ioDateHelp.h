#pragma once

namespace DateHelp
{
	//오늘 날짜 구하기
	int GetTodayOfYear();
	int GetTodayOfMon();
	int GetTodayOfWeek();
	int GetTodayOfDay();
	int GetTodayOfHour();
	int GetTodayOfMin();
	int GetTodayOfSec();

	const CTime& GetCurrTime();

	//DWORD -> DATE 변환
	int GetDateYear( DWORD dwDate );
	int GetDateMonth( DWORD dwDate );
	int GetDateDay( DWORD dwDate );
	int GetDateHour( DWORD dwDate );
	int GetDateMinute( DWORD dwDate );

	//DATE -> DWORD 변환
	DWORD ConvertDwDate( int iYear, int iMonth, int iDay, int iHour, int iMin );
	
	// DWORD -> Ctime 변환
	void ConvertCTime( IN DWORD dwDate, OUT CTime& Time );

	//윤년/윤달 계산
	bool isLeapYear( int iYear );
	bool isOddMonth( int iMonth );

	//날짜 증감
	void IncreaseMonth( int& iYear, int& iMonth, int& iDay );
	void DecreaseMonth( int& iYear, int& iMonth, int& iDay );

	void IncreaseDay( int& iYear, int& iMonth, int& iDay );
	void DecreaseDay( int& iYear, int& iMonth, int& iDay );

	void IncreaseMin( int& iYear, int& iMonth, int& iDay, int& iHour, int& iMin, int iIncraseMin );

	//날짜 간격 구하기
	enum PeriodType
	{
		PT_DAY,
		PT_HOUR,
		PT_MIN,
	};
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
		PeriodType eType=PT_DAY);


	//togetday
	int GetMonthDayPeriod( int num, int mon );

	// 입력한 날짜의 요일 구하기 : 0(Sunday) ~
	int GetWeekDay( int iYear, int iMonth, int iDay );
	
	//요일 문자를 가져오기(0 ~ 6)
	char* GetWeekDayString( int index );
	char* GetWeekDayString( DWORD dwDate );
	char* GetWeekDayString( int num, int mon, int day );
	
	time_t ConvertSecondTime( int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond );
}