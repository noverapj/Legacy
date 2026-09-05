#ifndef __ioAlchemicInventory_h__
#define __ioAlchemicInventory_h__

#include "ioDBDataController.h"

class Room; 

class ioAlchemicInventory : public ioDBDataController
{
public:
	enum
	{
		MAX_DB_SLOT		= 10,
		MAX_INVEN		= 30,
		MAX_SLOT_CNT	= 2000000000,
	};

	enum AlchemicItemType
	{
		AIT_NONE,
		AIT_PIECE,
		AIT_ADDITIVE,
	};

	struct AlchemicItem
	{
		int m_iCode;
		int m_iCount;

		int m_iValue1;
		int m_iValue2;

		AlchemicItem()
		{
			Init();
		}

		void Init()
		{
			m_iCode = 0;
			m_iCount = 0;
			m_iValue1 = 0;
			m_iValue2 = 0;
		}
	};
	typedef std::vector< AlchemicItem > vAlchemicItem;

	struct ALCHEMICDB
	{
		bool     m_bChange;
		DWORD    m_dwIndex;
		AlchemicItem m_AlchemicItem[MAX_DB_SLOT];
		
		ALCHEMICDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_AlchemicItem, 0, sizeof(m_AlchemicItem) );
		}
	};
	typedef std::vector< ALCHEMICDB > vALCHEMICDB;


protected:
	vALCHEMICDB m_vAlchemicPageList;

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	

	virtual void SaveData();

	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

	// 나중을 위한것
	void InsertPage();

protected:
	void InsertDBAlchemic( ALCHEMICDB &kAlchemicDB );

	bool AddAlchemicItem( int iCode, int iCount );
	bool DeleteAlchemicItem( int iCode );

	bool ChangeAlchemicItem( int iCode, const AlchemicItem &rkAlchemicItem );

public:
	// 획득
	bool GainAlchemicItem( int iCode, int iCount );

	// 소모
	bool UseAlchemicItem( int iCode, int iCount );

	// 빈칸있는지 체크
	bool CheckEmptySlot();

	// 현재갯수 체크
	int GetAlchemicItemCnt( int iCode );

	bool FindAlchemicItem( int iCode, AlchemicItem &rkAlchemicItem );

	// 소지 중인지 체크
	bool CheckHaveAlchemicItem( int iCode );

public:
	ioAlchemicInventory(void);
	virtual ~ioAlchemicInventory(void);
};

#endif // __ioAlchemicInventory_h__


