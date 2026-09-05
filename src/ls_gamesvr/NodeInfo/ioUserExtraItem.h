#ifndef __ioUserExtraItem_h__
#define __ioUserExtraItem_h__

#include "ioDBDataController.h"

class Room; 

// 권한 , 펫, 기타 아이템
class ioUserExtraItem : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT     = 10,
	};

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
		short m_dwFailExp;
		BOOL  m_bEquip;

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

			m_dwMaleCustom = m_dwFemaleCustom = 0;
			m_dwFailExp = 0;
			m_bEquip = FALSE;
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
	};

protected:
	struct EXTRAITEMDB
	{
		bool     m_bChange;
		DWORD    m_dwIndex;
		EXTRAITEMSLOT m_ExtraItem[MAX_SLOT];		
		
		EXTRAITEMDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_ExtraItem, 0, sizeof( m_ExtraItem ) );
		}
	};
	typedef std::vector< EXTRAITEMDB > vEXTRAITEMDB;
	vEXTRAITEMDB m_vExtraItemList;

	typedef std::map< int, DWORD > StartTimeMap; // int : iType, DWORD : StartTime
	StartTimeMap m_StartTimeMap;

	int m_iCurMaxIndex;

	int m_iCurPossessionCount;
	int m_iMaxPossessionCount;

protected:
	void InsertDBExtraItem( EXTRAITEMDB &kEtcItemDB, bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime );
	
	void SetCurPossessionCount( int iVal ) {  m_iCurPossessionCount = iVal; }
	
	void AddCurPossessionCount( int iVal ) { m_iCurPossessionCount += iVal; }
	void DecreasePossessionCount( int iVal );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	int AddExtraItem( IN const EXTRAITEMSLOT &rkNewSlot, IN bool bBuyCash, IN int iBuyPrice, IN int iLogType, IN int iMachineCode, IN int iPeriodTime, OUT DWORD &rdwIndex, OUT int &riArray );
	bool GetExtraItem( IN int iSlotIndex, OUT EXTRAITEMSLOT &rkExtraItem );
	bool GetExtraItemByItemCode( IN int iItemCode, IN int iItemReinforceLevel );
	bool GetRowExtraItem( IN DWORD dwIndex,  OUT EXTRAITEMSLOT kExtraItem[MAX_SLOT] );
	void SetExtraItem( const EXTRAITEMSLOT &rkExtraItem );

	bool DeleteExtraItem( int iSlotIndex );
	void DeleteExtraItemPassedDate( OUT IntVec &rvTypeVec );

	bool GetExtraItemIndex( IN int iSlotIndex, OUT DWORD &rdwIndex, OUT int &iFieldCnt );
	
	inline int GetRowCount()	{ return m_vExtraItemList.size(); }
	void SendAllExtraItemInfo();

	void FillMoveDataWithStartRow( SP2Packet &rkPacket, int iStartRow );
	void ApplyMoveDataWithRow(  SP2Packet &rkPacket, bool bDummyNode = false );

	void DBtoData( CQueryResultData *query_data, int& iLastIndex );

public:
	inline int GetCurPossessionCount() { return m_iCurPossessionCount; }
	inline int GetMaxPossesionCount() { return m_iMaxPossessionCount; }

	void SetMaxPossessionCount();
	bool IsSlotFull();

public:
	ioUserExtraItem(void);
	virtual ~ioUserExtraItem(void);
};

#endif // __ioUserExtraItem_h__


