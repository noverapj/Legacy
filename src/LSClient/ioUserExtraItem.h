#pragma once



class ioBaseChar;


enum
{
	EPT_TIME		= 0,     // 시간제 장비
	EPT_MORTMAIN	= 1,     // 영구 장비
	EPT_GROW_TIME   = 2,     // 시간제 장비이면서 성장 가능한 이벤트 장비
};

enum
{
	EET_DISABLE		= 0,
	EET_NORMAL		= 1,
	EET_ENABLE		= 2,
	EET_REGISTER	= 3,
};

struct EXTRAITEMSLOT
{
	int m_iItemCode;
	int m_iReinforce;
	int m_iIndex;

	int m_PeriodType;
	int m_iValue1;     // 년월일을 나타냄 20090715(2009년 7월 15일 )
	int m_iValue2;     // 시간을 나타냄 1232 (12시32분)

	int m_iTradeState;

	DWORD m_dwMaleCustom;	// 남성 텍스쳐 커스텀 
	DWORD m_dwFemaleCustom; // 여성 텍스쳐 커스텀
	WORD m_failExp; 
	bool m_bCharEquip;

	ioHashString m_Name;
	ioHashString m_IconName;

	EXTRAITEMSLOT()
	{
		Init();
	}

	void Init()
	{
		m_iItemCode = 0;
		m_iReinforce = 0;
		m_iIndex = 0;

		m_PeriodType = EPT_TIME;
		m_iValue1 = 0;
		m_iValue2 = 0;

		m_iTradeState = EET_DISABLE;
	
		m_dwMaleCustom = 0;
		m_dwFemaleCustom = 0;
		m_failExp = 0;

		m_bCharEquip = false;
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
};
typedef std::vector<EXTRAITEMSLOT> ExtraItemList;


class ExtraItemSort : public std::binary_function< const EXTRAITEMSLOT&, const EXTRAITEMSLOT&, bool >
{
public:
	bool operator()( const EXTRAITEMSLOT &lhs , const EXTRAITEMSLOT &rhs ) const
	{
		if( lhs.m_iIndex < rhs.m_iIndex )
		{
			return true;
		}
		return false;
	}
};

class ioUserExtraItem
{
public:
	enum
	{
		MAX_SLOT_CNT = 500,  // 500개 제한
	};

protected:
	ExtraItemList m_vExtraItemList;

public:
	void Initialize();
	void ApplyExtraItemData( SP2Packet &rkPacket );

	void AddExtraItemData( const EXTRAITEMSLOT &rkNewSlot );
	bool DeleteExtraItemData( int iSlotIndex );

	bool SetCharEquip( int iSlotIndex, bool bEquip );
	void ClearCharEquip();

	void SetExtraItem( const EXTRAITEMSLOT &rkExtraItem );

	bool GetExtraItem( IN int iSlotIndex, OUT EXTRAITEMSLOT &rkExtraItem );
	bool GetExtraItemArray( IN int iArray, OUT EXTRAITEMSLOT &rkExtraItem );
	bool GetExtraItemByItemCode( IN int ItemCode, OUT EXTRAITEMSLOT &rkExtraItem );
	
	int GetExtraItemCnt();
	int GetExtraItemMaxSlotCnt();

	void GetUseInfo( IN int iSlotIndex, OUT ioHashString &rsInfo );

	bool HasTradeWaitItem();
	bool IsSlotFull();

public:
	void EquipItem( CHARACTERDATA &rkCharData, bool bSetEquip, bool bBulletInit = true );

public:
	void FindAllItem( IN int iItemCode, OUT ExtraItemList& rkExtraItemList );

public:
	ioUserExtraItem();
	virtual ~ioUserExtraItem();
};

