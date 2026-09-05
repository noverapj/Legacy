
#ifndef _ioUserGrowthLevel_h_
#define _ioUserGrowthLevel_h_

#define MAX_CHAR_GROWTH 4
#define MAX_ITEM_GROWTH 4

class User;
class CQueryResultData;

enum
{
	TIG_NONE	= 0,
	TIG_WEAPON	= 1,
	TIG_ARMOR	= 2,
	TIG_HELMET	= 3,
	TIG_CLOAK	= 4,
	TIG_ATTACK	= 5,
	TIG_DEFENCE	= 6,
	TIG_MOVE	= 7,
	TIG_DROP	= 8,
};

////////////////////////////////////////////////////////////////////////////

struct LevelInfo
{
	int m_iClassType;
	BYTE m_CharLevel[MAX_CHAR_GROWTH];
	BYTE m_ItemLevel[MAX_ITEM_GROWTH];

	int m_iTimeGrowthSlot;
	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	LevelInfo()
	{
		m_iClassType = 0;
		memset( m_CharLevel, 0, sizeof(m_CharLevel) );
		memset( m_ItemLevel, 0, sizeof(m_ItemLevel) );

		m_iTimeGrowthSlot = 0;
		m_iValue1 = 0;
		m_iValue2 = 0;
	}

	void Init()
	{
		memset( m_CharLevel, 0, sizeof(m_CharLevel) );
		memset( m_ItemLevel, 0, sizeof(m_ItemLevel) );

		m_iTimeGrowthSlot = 0;
		m_iValue1 = 0;
		m_iValue2 = 0;
	}

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
		return m_iValue2 / 10000;           //  [21]23   ( 21시 23분 )
	}
	SHORT GetMinute()
	{
		return ( m_iValue2/100 ) % 100;           //  21[23]
	}
	SHORT GetSec()
	{
		return m_iValue2 % 100;
	}
	void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute, int iSec )
	{
		m_iValue1 = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
		m_iValue2 = ( iHour * 10000 ) + ( iMinute * 100 ) + iSec;
	}
	void GetDate( SYSTEMTIME &sysTime )
	{
		if(m_iTimeGrowthSlot == 0)
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
			sysTime.wSecond			= GetSec();
			sysTime.wMilliseconds	= 0;
		}
	}
};

//////////////////////////////////////////////////////////////////////////////

struct TimeGrowthInfo
{
	int m_iClassType;
	int m_iTimeSlot;	// 어느 것이 강화중인지... 무, 갑, 투, 망, 공, 방, 이, 드 순서

	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	bool m_bConfirm;

	TimeGrowthInfo()
	{
		m_iClassType = 0;
		m_iTimeSlot = 0;
		
		m_iValue1 = 0;
		m_iValue2 = 0;

		m_bConfirm = false;
	}

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
		return m_iValue2 / 10000;           //  [21]23   ( 21시 23분 )
	}
	SHORT GetMinute()
	{
		return ( m_iValue2/100 ) % 100;           //  21[23]
	}
	SHORT GetSec()
	{
		return m_iValue2 % 100;
	}
	void SetDate( int iYear , int iMonth, int iDay, int iHour, int iMinute, int iSec )
	{
		m_iValue1 = ( iYear * 10000 ) + ( iMonth * 100 ) + iDay;
		m_iValue2 = ( iHour * 10000 ) + ( iMinute * 100 ) + iSec;
	}
};
typedef std::vector<TimeGrowthInfo> TimeGrowthInfoList;

////////////////////////////////////////////////////////////////////////////

class ioUserGrowthLevel
{
public:
	enum
	{
		MAX_SLOT	= 5,
		NEW_INDEX	= 0xffffffff,
	};

protected:
	struct GrowthDB
	{
		DWORD m_dwIndex;
		bool m_bChange;
		LevelInfo m_LevelInfo[MAX_SLOT];

		GrowthDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
		}
	};
	typedef std::vector< GrowthDB > vGrowthDB;

protected:
	User *m_pUser;

protected:
	typedef std::map< int, int > GrowthPointMap;
	GrowthPointMap m_GrowthPointList;

	vGrowthDB m_GrowthDBList;

	
// DB Load & Save
public:
	void DBtoRecordData( CQueryResultData *query_data );
	void SaveGrowthLevel();

public:
	void Initialize( User *pUser );
	bool DBtoNewIndex( DWORD dwIndex );

public:
	void CharGrowthLevelUp( int iClassType, int iIndex, int iUpLevel );
	void ItemGrowthLevelUp( int iClassType, int iIndex, int iUpLevel );

	void CharGrowthLevelDown( int iClassType, int iSlot, int iDownLevel );
	void ItemGrowthLevelDown( int iClassType, int iSlot, int iDownLevel );
	
	void SetCharGrowthPoint( int iClassType, int iPoint );

	BYTE GetCharGrowthLevel( int iClassType, int iSlot, bool bOriginal );       // 오직 Return용이다 이 값을 참조해서 DB에 넣거나하면 난리남
	BYTE GetItemGrowthLevel( int iClassType, int iSlot, bool bOriginal );		// 오직 Return용이다 이 값을 참조해서 DB에 넣거나하면 난리남
	int GetCharGrowthPoint( int iClassType );

	void CheckCharGrowthPoint();
	void CharGrowthLevelInit( int iClassType, int iPoint );

	void AddCharGrowthPointByDecoWoman( int iDecoType, int iDecoCode );

protected:
	void ClearList();

	LevelInfo* FindGrowthDB( int iClassType );
	void SetChangeGrowthDB( int iClassType );
	
	void AddGrowthDB( int iClassType );
	void InsertDBGrowth( GrowthDB &kGrowthDB );

	int GetUsePoint( const LevelInfo &rLevelInfo );

	void FillGrowthLevelData( SP2Packet &rkPacket );

public:
	bool FillGrowthLevelDataByClassType( int iClassType, SP2Packet &rkPacket );
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false );

	//
	void CheckTimeGrowth( OUT TimeGrowthInfoList &rkList );

	bool AddTimeGrowth( IN int iClassType, IN int iSlot, OUT int &iValue1, OUT int &iValue2 );
	bool RemoveTimeGrowth( int iClassType, int iSlot );

	int GetCurTimeGrowthCntInChar( int iClassType );
	int GetCurTimeGrowthTotalCnt();

	bool CheckEnableTimeGrowthLevel( int iClassType, int iSlot );
	bool HasTimeGrowthValue( int iClassType, int iSlot );
	//

public:
	void FillClassTypeGrowthInfo( SP2Packet &rkPacket, int iClassType );

public:
	ioUserGrowthLevel();
	virtual ~ioUserGrowthLevel();
};

#endif