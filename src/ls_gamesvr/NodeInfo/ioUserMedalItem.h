#ifndef __ioUserMedalItem_h__
#define __ioUserMedalItem_h__

#include "ioDBDataController.h"

class cSerialize;

class ioUserMedalItem : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT     = 10,
	};

	enum
	{
		PT_TIME		= 0,
		PT_MORTMAIN	= 1,
	};

	struct MEDALITEMSLOT
	{
		int m_iItemType;   // 1부터 증가하며 증복되지 않는다. 0는 아이템이 없다.
		int m_iEquipClass; // class type, 0이면 미장착  
		int m_iPeriodType; // PT_TIME, PT_MORTMAIN
		int m_iLimitDate;  // 년월일을 나타냄 20090715(2009년 7월 15일 )  , 클라이언트와 같이쓰기 위해서 CTime을 사용하지 않음.
		int m_iLimitTime;  // 시간을 나타냄 1232 (12시32분)

		MEDALITEMSLOT()
		{
			Init();
		}

		void Init()
		{
			m_iItemType  = 0;
			m_iEquipClass = 0;
			m_iPeriodType = PT_TIME;
			m_iLimitDate = 0;
			m_iLimitTime = 0;
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
		void GetDate( SYSTEMTIME &sysTime )
		{
			if(m_iItemType == 0)
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
	struct MEDALITEMDB
	{
		bool     m_bChange;
		DWORD    m_dwIndex;
		MEDALITEMSLOT m_kMedalItem[MAX_SLOT];		

		MEDALITEMDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_kMedalItem, 0, sizeof( m_kMedalItem ) );
		}

		int GetEquipClass( const int iSlotNum )
		{
			if( iSlotNum >= MAX_SLOT ||  iSlotNum < 0 )
				return -1;

			return m_kMedalItem[iSlotNum].m_iEquipClass;
		}

		int GetEquipMedalType( const int iSlotNum )
		{
			if( iSlotNum >= MAX_SLOT ||  iSlotNum < 0 )
				return -1;

			return m_kMedalItem[iSlotNum].m_iItemType;
		}
	};
	typedef std::vector< MEDALITEMDB > vMEDALITEMDB;
	vMEDALITEMDB m_vMedalItemList;

protected:
	void InsertDBMedalItem( MEDALITEMDB &kMedalItemDB, int iLogType, int iLimitTime );

	void GetQueryArgument( IN MEDALITEMSLOT &rkMedalItem, cSerialize& v_FT );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	bool AddMedalItem( IN const MEDALITEMSLOT &rkNewSlot, IN int iLogType, IN int iLimitTime, OUT DWORD &rdwIndex, OUT int &riArray );
	bool GetMedalItem( IN int iItemType, OUT MEDALITEMSLOT &rkMedalItem );
	bool GetRowMedalItem( IN DWORD dwIndex, OUT MEDALITEMSLOT kMedalItem[MAX_SLOT] );
	void SetMedalItem( const MEDALITEMSLOT &rkMedalItem );
	

	bool DeleteMedalItem( int iItemType );
	bool DeleteMedalItem( int iItemType, const int iListArrayIndex );

	void DeleteMedalItemPassedDate( OUT IntVec &rvTypeVec );

	int ReleaseEquipMedal( int iClassType );

	bool GetMedalItemIndex( IN int iItemType, OUT DWORD &rdwIndex, OUT int &iFieldCnt );
	int  GetEquipNum( int iClassType );
	bool GetMedalItemTypeVec( OUT IntVec &rvItemTypeVec, IN int iClassType );

	void FillEquipClass( IN int iClassType, IN int iMaxSlotNum, OUT SP2Packet &rkPacket );

public:
	bool IsEquipMedalByChar( const int iClassType, const int iItemType );

	bool GetMedalItem( const int iClassType, const int iItemType, const  bool bEquip, MEDALITEMSLOT &rkMedalItem );

	bool GetNotEquipMedalItem( const int iItemType, MEDALITEMSLOT &rkMedalItem, int &iListArrayIndex );
	bool GetNotEquipMedalItem( const int iItemType, MEDALITEMSLOT &rkMedalItem );

	bool GetEquipMedalItem( const int iClassType, const int iItemType, MEDALITEMSLOT &rkMedalItem );

	void SetMedalItemDueToEquip( const int iItemType, MEDALITEMSLOT &rkMedalItem );
	void SetMedalItemDueToTakeOff( const int iClassType, const int iItemType, MEDALITEMSLOT &rkMedalItem );

	int GetNotEquipMedalItemCount( const int iItemType );

public:
	ioUserMedalItem(void);
	virtual ~ioUserMedalItem(void);
};

#endif // __ioUserMedalItem_h__


