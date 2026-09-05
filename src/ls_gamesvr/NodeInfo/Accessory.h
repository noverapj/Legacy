#pragma once

enum AccessoryType
{
	ACST_NONE		= 0,
	ACST_RING		= 1,
	ACST_NECKLACE	= 2,
	ACST_BRACELET	= 3,
};

enum AccessoryPeriodType
{
	ASPT_TIME		= 0,
	ASPT_MORTMAIN	= 1,
};

class Accessory
{
public:
	Accessory();
	~Accessory();

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
	void GetAccessoryLimitDate(SYSTEMTIME& sysTime);

	inline int GetAccessoryIndex() { return m_dwIndex; }
	inline void SetAccessoryIndex(DWORD dwIndex) { m_dwIndex = dwIndex; }

	inline int GetAccessoryCode() { return m_iAccessoryCode; }
	inline void SetAccessoryCode(int iCode) { m_iAccessoryCode = iCode; }

	inline int GetPeriodType()	{ return m_iPeriodType; }
	inline void SetPeriodType(int iType) { m_iPeriodType= iType; }

	inline int GetWearingClass() { return m_iWearingClassType; }
	inline void SetWearingClass( int iClassType ) { m_iWearingClassType = iClassType; }

	inline int GetYearMonthDayValue() { return m_iValue1; }
	inline int GetHourMinute() { return m_iValue2; }

	inline int GetAccessoryValue() { return m_iAccessoryValue; }
	void SetAccessoryValue( int iValue );

	inline int GetComposeCode() { return m_iComposeCode; }
	inline void SetComposeCode(int iCode) { m_iComposeCode = iCode; }

	inline int GetComposeValue() { return m_iComposeValue; }
	inline void SetComposeValue(int iValue) { m_iComposeValue = iValue; }

protected:
	DWORD m_dwIndex;
	int m_iAccessoryCode;
	int m_iPeriodType;
	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	int m_iWearingClassType;
	int m_iAccessoryValue;

	// 합성
	int m_iComposeCode;
	int m_iComposeValue;
};