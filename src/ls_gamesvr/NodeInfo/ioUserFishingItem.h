
#ifndef __ioUserFishingItem_h__
#define __ioUserFishingItem_h__

#include "ioDBDataController.h"

class Room; 

class ioUserFishingItem : public ioDBDataController
{
public:
	enum
	{
		MAX_SLOT			= 20,
		DEFAULT_FISH_INVEN	= 10,
	};

	struct FishItemInfo
	{
		BYTE m_iType;
		int m_iArray;

		FishItemInfo()
		{
			Init();
		}

		void Init()
		{
			m_iType = 0;
			m_iArray = 0;
		}
	};

protected:
	struct FISHITEMDB
	{
		bool     m_bChange;
		DWORD    m_dwIndex;
		FishItemInfo m_FishItem[MAX_SLOT];		
		
		FISHITEMDB()
		{
			m_bChange = false;
			m_dwIndex = NEW_INDEX;
			memset( m_FishItem, 0, sizeof( m_FishItem ) );
		}
	};
	typedef std::vector< FISHITEMDB > vFISHITEMDB;
	vFISHITEMDB m_vFishItemList;

	bool m_bLoadDB;
	int m_iCurMaxArray;
	int m_iCurMaxInventory;

protected:
	void InsertDBFishItem( FISHITEMDB &kFishItemDB );

	int GetCurActivityItemCnt();

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );	
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	inline bool IsDataLoad() const { return m_bLoadDB; }

	int AddFishItem( BYTE iType );
	BYTE DeleteFishItem( int iArray );

	void CheckCurMaxInventory();
	int GetCurMaxInventory();

	bool IsFullInventory();


public:
	ioUserFishingItem(void);
	virtual ~ioUserFishingItem(void);
};

#endif // __ioUserFishingItem_h__


