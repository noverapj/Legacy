#pragma once

class CustomMedal
{
public:
	enum
	{
		MEDAL_STAT	 = 8,
	};


	int m_iPresentIndex;		// 선물함 인덱스
	int m_iPresentSlotIndex;	// 선물함 슬롯인덱스
	int m_iItemIndex;	// 인벤토리 고유 인덱스

	int	 m_iGrowth[8];	// 스탯
	bool m_bGrowth[8];	// 스탯

	int m_iItemCode;   // 1부터 증가하며 증복되지 않는다. 0는 아이템이 없다.
	int m_iEquipClass; // class type, 0이면 미장착  
	int m_iPeriodType; // PT_TIME, PT_MORTMAIN
	int m_iLimitDate;  // 년월일을 나타냄 20090715(2009년 7월 15일 )  , 클라이언트와 같이쓰기 위해서 CTime을 사용하지 않음.
	int m_iLimitTime;  // 시간을 나타냄 1232 (12시32분)

	bool m_bStatSelectDone;

	void Init();

	CustomMedal(void);
	~CustomMedal(void);

	void ApplyMoveData( SP2Packet &rkPacket );
	void FillMoveData( SP2Packet &rkPacket );

	SHORT GetYear()
	{
		return m_iLimitDate/10000;           // [2009]0715
	}
	SHORT GetMonth()
	{
		return ( m_iLimitDate/100 ) % 100;   //  2009[07]15
	}
	SHORT GetDay()
	{
		return m_iLimitDate % 100;           //  200907[15]
	}
	SHORT GetHour()
	{
		return m_iLimitTime / 100;           //  [21]23   ( 21시 23분 )
	}
	SHORT GetMinute()
	{
		return m_iLimitTime % 100;           //  21[23]
	}
	void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute )
	{
		m_iLimitDate = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
		m_iLimitTime = ( iHour * 100 ) + iMinute;
	}
	void GetDate( SYSTEMTIME &sysTime )
	{
		if(m_iItemCode == 0)
		{
			sysTime.wYear			= 1900;
			sysTime.wMonth			= 01;
			sysTime.wDay			= 01;
			sysTime.wHour			= 0;
			sysTime.wMinute			= 0;
			sysTime.wSecond			= 0;
			sysTime.wMilliseconds	= 0;
		}
		else
		{
			sysTime.wYear			= GetYear();
			sysTime.wMonth			= GetMonth();
			sysTime.wDay			= GetDay();
			sysTime.wHour			= GetHour();
			sysTime.wMinute			= GetMinute();
			sysTime.wSecond			= 0;
			sysTime.wMilliseconds	= 0;
		}
	}


	//// 날짜용
	//SHORT GetYear()
	//{
	//	return m_iLimitDate/10000;           // [2009]0715
	//}
	//SHORT GetMonth()
	//{
	//	return ( m_iLimitDate/100 ) % 100;   //  2009[07]15
	//}
	//SHORT GetDay()
	//{
	//	return m_iLimitDate % 100;           //  200907[15]
	//}
	//SHORT GetHour()
	//{
	//	return m_iLimitTime / 100;           //  [21]23   ( 21시 23분 )
	//}
	//SHORT GetMinute()
	//{
	//	return m_iLimitTime % 100;           //  21[23]
	//}
	//void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute )
	//{
	//	m_iLimitDate = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
	//	m_iLimitTime = ( iHour * 100 ) + iMinute;
	//}
	//void GetDate( SYSTEMTIME &sysTime )
	//{
	//	if(m_iItemType == 0)
	//	{
	//		sysTime.wYear			= 1900;
	//		sysTime.wMonth			= 01;
	//		sysTime.wDay			= 01;
	//		sysTime.wHour			= 0;
	//		sysTime.wMinute			= 0;
	//		sysTime.wSecond			= 0;
	//		sysTime.wMilliseconds	= 0;
	//	}
	//	else
	//	{
	//		sysTime.wYear			= GetYear();
	//		sysTime.wMonth			= GetMonth();
	//		sysTime.wDay			= GetDay();
	//		sysTime.wHour			= GetHour();
	//		sysTime.wMinute			= GetMinute();
	//		sysTime.wSecond			= 0;
	//		sysTime.wMilliseconds	= 0;
	//	}
	//}
};