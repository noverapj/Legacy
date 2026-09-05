
#ifndef _ioInventory_h_
#define _ioInventory_h_

#include "NodeHelpStructDefine.h"
#include "ioDBDataController.h"

class ioInventory : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT         = 20,
		MAX_ROW			 = 500,
		EQUIP_CODE       = 10000,
	};

protected:
	struct ITEMDB
	{
		bool     m_bChange;

		DWORD    m_dwIndex;
		ITEMSLOT m_Slot[MAX_SLOT];		
		ITEMDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_Slot, 0, sizeof( m_Slot ) );
		}
	};
	typedef std::vector< ITEMDB > vITEMDB;
	vITEMDB m_vItemList;

protected:
	void InsertDBInventory( ITEMDB &kItemDB, bool bBuyCash, int iBuyPrice, int iLogType );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

	void DBtoData( CQueryResultData *query_data, int& iLastIndex );

public:
	void DecoFillMoveData( SP2Packet &rkPacket, int iStartIndex );
	void DecoApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );
	int  SendInventory( int iStartArray, int iSendSize );

public:
	int GetCount()	{ return m_vItemList.size(); }

	bool IsSlotItem( ITEMSLOT &kSlot );
	bool IsFull();

	bool AddSlotItem( IN ITEMSLOT &kSlot, IN bool bBuyCash, IN int iBuyPrice, IN int iLogType, OUT DWORD &rdwIndex, OUT int &riArray );
	bool RemoveSlotItem( IN ITEMSLOT &kSlot, OUT DWORD &rdwIndex, OUT int &riArray );
	RaceDetailType GetEquipRaceType( const int iClassType );
	void GetEquipItemCode( CHARACTER &charInfo );
	
	int GetEquipItemCode( const int iItemType );
	void SetEquipItem( const int iItemType, const int iItemCode  );

	bool FindOtherSlotItem( IN ITEMSLOT &kSlot, OUT ITEMSLOT &rkSlot );
	
public:
	bool GetItemInfo( IN DWORD dwIndex, OUT ITEMSLOT kItemSlot[MAX_SLOT] );

public:
	ioInventory();
	virtual ~ioInventory();
};

#endif