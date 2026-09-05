#pragma once


class SP2Packet;
// 권한 , 펫, 기타 아이템
class ioUserEtcItem 
{
public:
	enum
	{
		MAX_SLOT   = 20,	
		ITEM_USE_ON  = 1,
		ITEM_USE_OFF = 0,
	};

	struct ETCITEMSLOT
	{
		int m_iType;       // AABBBBB // AA: 1:횟수용, 2:시간용, 3:즉시사용 // BBBBB: 종류 1~9999 : 권한 , 10000 ~ 19999 : 펫(미확정), 20000 ~ 29999 : 기타(미확정) - 만단위씩 구분함 
		int m_iValue1;     // Type에 따라서 쓰임새가 다른 변수 , 유저효과음이면 남은 횟수, 기타 시간소모형이면 남은시간
		int m_iValue2;     // 사용여부 // 0 : 미사용, 1:사용

		ETCITEMSLOT()
		{
			m_iType   = 0;
			m_iValue1 = 0;
			m_iValue2 = 0;
		}

		void AddUse( int iUse )
		{
			m_iValue1 += iUse;
		}
		int GetUse()
		{
			return m_iValue1;
		}
		bool IsUse()
		{
			if( m_iValue2 == ITEM_USE_ON )
				return true;
			
			return false;
		}
		void SetUse( bool bUse )
		{
			if( bUse )
				m_iValue2 = ITEM_USE_ON;
			else
				m_iValue2 = ITEM_USE_OFF;
		}
		// UT_DATE: 날짜용
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
		// Value2에서 날짜는 하위 4자리만 사용한다. 상위 자리는 특별 아이템에서 사용 -
		SHORT GetHour()
		{
			return (m_iValue2 % 10000) / 100;           //  [21]23   ( 21시 23분 )
		}
		SHORT GetMinute()
		{
			return (m_iValue2 % 10000) % 100;           //  21[23]
		}
		void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute )
		{
			m_iValue1 = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
			m_iValue2 = ((m_iValue2/10000)*10000) + ( ( iHour * 100 ) + iMinute );
		}

		int GetDateExcludeValue2()
		{
			return (m_iValue2 / 10000);
		}

		void SetDateExcludeValue2( int iValue2 )
		{
			int iDate = (m_iValue2 % 10000);
			m_iValue2 = (iValue2 * 10000) + iDate;
		}

		int GetDateExcludeValue3Time()
		{
			int iValue = GetDateExcludeValue2();
			return iValue % 1000;
		}

		int GetDateExcludeValue3State()
		{
			int iValue = GetDateExcludeValue2();
			return (iValue / 10000)-1;
		}

		void SetDateExcludeValue3( int iValue, int iState )
		{
			int iDate = ( m_iValue2 % 10000 );

			//999분 이상 세팅 불가
			iValue = min( 999, iValue );

			//State계산
			iState = min( 9, iState );
			iState = (iState+1) * 10000;
			m_iValue2 = ( (iState + iValue) * 10000) + iDate;
		}
	};

protected:
	struct ETCITEMDB
	{
		ETCITEMSLOT m_EtcItem[MAX_SLOT];		
		ETCITEMDB()
		{
			memset( m_EtcItem, 0, sizeof( m_EtcItem ) );
		}
	};
	typedef std::vector< ETCITEMDB > vETCITEMDB;
	vETCITEMDB m_vEtcItemList;

	typedef std::map< int, DWORD > StartTimeMap; // int : iType, DWORD : StartTime
	StartTimeMap m_StartTimeMap;

protected:
	void InsertStartTimeMap( int iType );
	void DeleteStartTimeMap( int iType );
	void SetStartTimeMap( int iType, DWORD dwStartTime );

public:
	void Initialize();
	void ApplyData( SP2Packet &rkPacket );

public:
	int  GetUse( int iType );
	bool AddEtcItem( const ETCITEMSLOT &rkNewSlot );
	bool GetEtcItem( IN int iType, OUT ETCITEMSLOT &rkEtcItem );
	bool GetEtcItemByArray( IN int iArray, OUT ETCITEMSLOT &rkEtcItem );
	bool GetEtcItemRange( IN int iStarType, IN int iEndType, OUT ETCITEMSLOT &rkEtcItem );
	int  GetEtcItemCurrentSlot();
	void SetEtcItem( const ETCITEMSLOT &rkEtcItem );
	void DeleteEtcItem( int iType );

	void ApplyTimeData( SP2Packet &rkPacket, bool bClearStartTime = false );
	void DeleteEtcItemZeroTime();
	int  GetValueByCalcTime( int iType );

	bool IsCanUse( int iType );
	void SendUse( int iType );

	bool CheckDurationOfFishingGear( int iType );

	void GetUseInfo( IN int iType, OUT ioHashString &rsInfo1, OUT ioHashString &rsInfo2, OUT DWORD &rInfo1Color, OUT DWORD &rInfo2Color, bool bExpireText = false );

	void UpdateGashaponTime( int iMinute );

private:
	void UpdateUI( DWORD dwItemType );

public:
	ioUserEtcItem(void);
	virtual ~ioUserEtcItem(void);
};



