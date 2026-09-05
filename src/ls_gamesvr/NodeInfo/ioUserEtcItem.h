#ifndef __ioUserEtcItem_h__
#define __ioUserEtcItem_h__

#include "ioDBDataController.h"

class Room; 

// 권한 , 펫, 기타 아이템
class ioUserEtcItem : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT     = 20,
		ITEM_USE_ON  = 1,
		ITEM_USE_OFF = 0,
		USE_TYPE_CUT_VALUE= 1000000,
	};

	struct ETCITEMSLOT
	{
		int m_iType;       // AABBBBBB // AA: 1:횟수용, 2:시간용, 3:즉시사용 , 4:영구사용 , 5:날짜제한 // BBBBBB: 종류 1~99999 : 권한 , 100000 ~ 199999 : 클래스별아이템(XYYYZZ -> X:타입,YYY:클래스타입,ZZ:아이템종류) , 200000 ~ 299999 : 펫(미확정) - 십만단위씩 구분함 
		int m_iValue1;     // Type에 따라서 쓰임새가 다른 변수 , 유저효과음이면 남은 횟수, 기타 시간소모형이면 남은시간, 날짜아이템은 년월일을 나타냄 20090715(2009년 7월 15일 )
		int m_iValue2;     // Type에 따라서 쓰임새가 다른 변수 : 보통은 사용여부 // 0 : 미사용, 1:사용  | 날짜아이템은 시간을 나타냄 1232 (12시32분)

		ETCITEMSLOT()
		{
			m_iType   = 0;
			m_iValue1 = 0;
			m_iValue2 = 0;
		}
		
		int GetUseType()
		{
			return m_iType / USE_TYPE_CUT_VALUE;
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
		bool     m_bChange;
		DWORD    m_dwIndex;
		ETCITEMSLOT m_EtcItem[MAX_SLOT];		
		ETCITEMDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_EtcItem, 0, sizeof( m_EtcItem ) );
		}
	};
	typedef std::vector< ETCITEMDB > vETCITEMDB;
	vETCITEMDB m_vEtcItemList;

	typedef std::map< int, DWORD > StartTimeMap; // int : iType, DWORD : StartTime
	StartTimeMap m_StartTimeMap;

protected:
	void InsertDBEtcItem( ETCITEMDB &kEtcItemDB, bool bBuyCash, int iBuyPrice );
	void InsertStartTimeMap( int iType );
	void DeleteStartTimeMap( int iType );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	void GainSpendTypeEtcItem( int iGainCode, int iGainCount );
	bool AddEtcItem( IN const ETCITEMSLOT &rkNewSlot, IN bool bBuyCash, IN int iBuyPrice, OUT DWORD &rdwIndex, OUT int &riArray );
	bool GetEtcItem( IN int iType, OUT ETCITEMSLOT &rkEtcItem );
	bool GetEtcItemByArray( IN int iArray, OUT ETCITEMSLOT &rkEtcItem );
	bool GetRowEtcItem( IN DWORD dwIndex,  OUT ETCITEMSLOT kEtcItem[MAX_SLOT] );
	int  GetEtcItemCurrentSlot();
	void SetEtcItem( const ETCITEMSLOT &rkEtcItem, int iLogType = 0 );
	void ioUserEtcItem::SetEtcItem( const ETCITEMSLOT &rkEtcItem, DWORD& dwIndex, int& iArray, int iLogType = 0 );
	bool DeleteEtcItem( int iType, int iDelLogType );
	void DeleteEtcItemZeroTime();
	void DeleteEtcItemPassedDate( OUT IntVec &rvTypeVec );

	bool SetStartTimeMap( Room *pRoom, const char *szFunction, int iType = 0/*ioEtcItem::EIT_NONE*/ ); // ioEtcItem.h 추가하지 않기 위해서
	void FillTimeData( SP2Packet &rkPacket, int iType = 0/*ioEtcItem::EIT_NONE*/ );
	bool UpdateTimeData( OUT IntVec &rvType, IN Room *pRoom, IN const char *szFunction, IN int iType = 0/*ioEtcItem::EIT_NONE*/ );
	void LeaveRoomTimeItem( OUT IntVec &rvType ); 
	
	bool GetEtcItemIndex( IN int iType, OUT DWORD &rdwIndex, OUT int &iFieldCnt );

	// 시간마다 지급하는 가챠폰
	void SendGashaponRealTime( int iMinute );
	void UpdateGashaponTime( int iMinute );

	BOOL HaveAThisItem(DWORD dwType);

	// 2018-01-15 by bckim, 탐사 확장
	void SetRechargeTime();							//	DWORD m_dwRechargeStartTime;	// 탐사 키트 충전 시작 시간.
	void CheckLoginExcavationInfo();				//	로그인시 시간 확인 >> 재산정 및 보유 탐사장비 확인 지급 
	void LoginRecharge(ETCITEMSLOT &rkFindItem);	// 시간 확인 재산정 및 충전 
	void LogoutRecharge();
	void TimeRechargeExcavationKit();
	int GetExcavationKitCount();
	
	void NotifyChanges( int iType, bool bIsDiscount = false);
	DWORD GetPassedRechargeTimeByCurTime();
	BOOL FillExcavationKit(int iCount, ETCITEMSLOT &rkFindItem);
	
	// 2018-02-20 by bkcim, 탐사 서버 이동시 리차징 타임 전송
	DWORD GetRechargeStartTime();
	void SetRechargeStartTime( DWORD i_dwRechargeStartTime);
	// end. 2018-02-20 by bkcim, 탐사 서버 이동시 리차징 타임 전송

protected:
	DWORD m_dwRechargeStartTime;	// 탐사 키트 충전 시작 시간.
	// End. 2018-01-15 by bckim, 탐사 확장


public:
	ioUserEtcItem(void);
	virtual ~ioUserEtcItem(void);
};

#endif // __ioUserEtcItem_h__


