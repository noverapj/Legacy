#pragma once

struct MedalSyncInfo
{
	int m_iItemType;
	int m_iCustomIndex;
	int m_iStat[8];

	MedalSyncInfo()
	{
		Init();
	}

	void Init()
	{
		m_iItemType = 0;
		m_iCustomIndex = 0;
		for( int i = 0; i < 8; i++ )
			m_iStat[i] = 0;
	}
};
typedef std::vector<MedalSyncInfo> MedalSyncInfoVec;

class ioUserMedalItem 
{
public:
	enum
	{
		PT_TIME		= 0,
		PT_MORTMAIN	= 1,
	};

	enum
	{
		MAX_SLOT_CNT = 1000,  // 1000개 제한
	};

	struct MEDALITEMSLOT
	{
		CEncrypt<int> m_iItemType;   // 1부터 증가하며 증복되지 않는다. 0는 아이템이 없다.
		int m_iCustomIndex;
		int m_iEquipClass; // class type, 0이면 미장착  
		int m_iPeriodType; // PT_TIME, PT_MORTMAIN
		int m_iLimitDate;  // 년월일을 나타냄 20090715(2009년 7월 15일 )  , 클라이언트와 같이쓰기 위해서 CTime을 사용하지 않음.
		int m_iLimitTime;  // 시간을 나타냄 1232 (12시32분)
		CEncrypt<int> m_iStat[8];

		MEDALITEMSLOT()
		{
			Init();
		}

		void Init()
		{
			m_iItemType  = 0;
			m_iCustomIndex = 0;
			m_iEquipClass = 0;
			m_iPeriodType = PT_TIME;
			m_iLimitDate = 0;
			m_iLimitTime = 0;
			for( int i = 0; i < 8; i++ )
				m_iStat[i] = 0;
		}

		// 날짜용
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

		bool operator=( const MEDALITEMSLOT &rhs )
		{
			m_iItemType = rhs.m_iItemType;
			m_iCustomIndex = rhs.m_iCustomIndex;
			m_iEquipClass = rhs.m_iEquipClass;
			m_iPeriodType = rhs.m_iPeriodType;
			m_iLimitDate = rhs.m_iLimitDate;
			m_iLimitTime = rhs.m_iLimitTime;
			for( int i = 0; i < 8; i++ )
				m_iStat[i] = rhs.m_iStat[i];

			return true;
		}
	};

protected:
	typedef std::vector< MEDALITEMSLOT > vMedalItemVec;
	vMedalItemVec m_vMedalItemVec;

public:
	void Initialize();
	void ApplyMedalItemData( SP2Packet &rkPacket );
	void ApplyCustomMedalItemData( SP2Packet &rkPacket );

	void AddMedalItemData( const MEDALITEMSLOT &rkNewSlot );
	void AddCustomMedalItemData( SP2Packet &rkPacket );
	bool DeleteMedalItemData( int iItemType, int iCustomIndex );

	void SetMedalItem( const MEDALITEMSLOT &rkMedalItem, bool bFindNotEquipMedal = false, int nClassType = 0 );

	bool GetMedalItem( IN int iItemType, int iCustomIndex, OUT MEDALITEMSLOT &rkMedalItem, bool FindNotEquipMedal = false, int nClassType = 0 );
	bool GetMedalItemArray( IN int iArray, OUT MEDALITEMSLOT &rkMedalItem );

	int GetMedalItemCnt();

	bool GetMedalItemTypeVec( OUT MedalSyncInfoVec &rvItemTypeVec, IN int iClassType );

	void GetUseInfo( IN int iItemType, int iCustomIndex, OUT ioHashString &rsInfo );

	void ReleaseEquipMedal( int iClassType );

	int  GetExtraItemCnt();
	bool IsSlotFull();

public:
	ioUserMedalItem(void);
	virtual ~ioUserMedalItem(void);
};


