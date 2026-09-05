#pragma once

enum PremiumCostumeSubType
{
	PCST_NONE	= 0,
	PCST_Armor	= 1,
	PCST_Helmet	= 2,
	PCST_Cloak	= 3,
	PCST_MAX	= 4,
};

enum PeriodType
{
	PCPT_TIME		= 0,
	PCPT_MORTMAIN	= 1,
};

class Costume
{
public:
	Costume();
	~Costume();

	void Init();
	void Destroy();

public:
	// 날짜용
	SHORT GetYear()
	{
		return m_iValue1/10000;           // [2009]0715
	}
	SHORT GetMonth()
	{
		return ( m_iValue1/100 ) % 100;   //  2009[07]15
	}
	SHORT GetDay()
	{
		return m_iValue1 % 100;           //  200907[15]
	}
	SHORT GetHour()
	{
		return m_iValue2 / 100;           //  [21]23   ( 21시 23분 )
	}
	SHORT GetMinute()
	{
		return m_iValue2 % 100;           //  21[23]
	}
	void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute )
	{
		m_iValue1 = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
		m_iValue2 = ( iHour * 100 ) + iMinute;
	}
	void SetDate( int iYearMonthDay, int iHourMinute)
	{
		m_iValue1 = iYearMonthDay;
		m_iValue2 = iHourMinute;
	}
public:
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket );

public:
	void GetCostumeLimitDate(SYSTEMTIME& sysTime);

	inline int GetCostumeIndex() { return m_dwIndex; }
	inline void SetCostumeIndex(DWORD dwIndex) { m_dwIndex = dwIndex; }

	inline int GetCostumeCode() { return m_iCostumeCode; }
	inline void SetCostumeCode(int iCode) { m_iCostumeCode = iCode; }

	inline int GetPeriodType()	{ return m_iPeriodType; }
	inline void SetPeriodType(int iType) { m_iPeriodType= iType; }

	inline int GetWearingClass() { return m_iWearingClassType; }
	inline void SetWearingClass( int iClassType ) { m_iWearingClassType = iClassType; }

	inline DWORD GetMaleCustom() { return m_dwMaleCustom; }
	inline void SetMaleCustom(DWORD dwCustom) { m_dwMaleCustom = dwCustom; }

	inline DWORD GetFemaleCustom() { return m_dwFemaleCustom; }
	inline void SetFemaleCustom(DWORD dwCustom) { m_dwFemaleCustom = dwCustom; }

	inline int GetYearMonthDayValue() { return m_iValue1; }
	inline int GetHourMinute() { return m_iValue2; }

protected:
	DWORD m_dwIndex;
	int m_iCostumeCode;
	int m_iPeriodType;
	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	DWORD m_dwMaleCustom;
	DWORD m_dwFemaleCustom;

	int m_iWearingClassType;
};