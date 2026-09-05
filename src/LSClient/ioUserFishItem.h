#pragma once

class ioBaseChar;

#define DEFAULT_FISH_INVEN 10

struct FishItem
{
	BYTE m_iType;
	int m_iArray;
	DWORD m_dwEtcItemCode;		// (낚은 결과물이면 0, 그 외 낚시용품은 EtcItemCode)
	bool m_bOwn;	// 소유 여부

	FishItem()
	{
		m_iType = 0;
		m_iArray = 0;
		m_dwEtcItemCode = 0;
		m_bOwn = false;
	}
};
typedef std::vector<FishItem> FishItemList;

class FishItemSort : public std::binary_function< const FishItem&, const FishItem&, bool >
{
public:
	bool operator()( const FishItem &lhs , const FishItem &rhs ) const
	{
		if( lhs.m_iArray < rhs.m_iArray )
		{
			return true;
		}
		return false;
	}
};

class FishingGearItemSort : public std::binary_function< const FishItem&, const FishItem&, bool >
{
public:
	bool operator()( const FishItem &lhs , const FishItem &rhs ) const
	{
		if( ( lhs.m_bOwn && rhs.m_bOwn ) || ( !lhs.m_bOwn && !rhs.m_bOwn ) )
		{
			if( lhs.m_iArray < rhs.m_iArray )
			{
				return true;
			}
			return false;
		}
		
		if( lhs.m_bOwn && !rhs.m_bOwn )
			return true;

		return false;
	}
};

class ioUserFishItem
{
protected:
	CEncrypt<int> m_iCurMaxInventory;
	FishItemList m_vFishItemList;

	// 낚시대
	FishItemList m_vFishingRodItemList;
	// 미끼
	FishItemList m_vFishingBaitItemList;
	// 낚시터
	FishItemList m_vFishingGroundItemList;

public:
	void Initialize();
	void ApplyFishItemData( SP2Packet &rkPacket );

	void AddFishItemData( BYTE iType, int iIndexArray );
	void DeleteFishItemData( int iIndexArray );

	int GetFishItemType( int iListArray );
	int GetFishItemArray( int iListArray );

	int GetFishingRodItemType( int iListArray );
	int GetFishingBaitItemType( int iListArray );
	int GetFishingGroundItemType( int iListArray );

	void ChangeFishingGroundItemOwn( int iListArray, bool bOwn );

	void AlignFishingGearByOwn();

	bool GetFishingGearOwnByTabType( int iListArray, FishingGearType eFishingGearType );
	bool GetFishingGearOwnByEtcCode( int iEtcCode, FishingGearType eFishingGearType );
	DWORD GetFishingGearEtcCodeByTabType( int iListArray, FishingGearType eFishingGearType );

	DWORD GetAscendingOrderFishingBaitEtcCodeByOwn();

	int GetFishingGearCountByTabType( FishingGearType eFishingGearType );

	// 가지고 있는 낚싯대, 미끼들 중 제일 작은 값의 순번(Array)의 아이템 EtcCode 반환
	DWORD GetFishingRodByFirstOrder();
	DWORD GetFishingBaitByFirstOrder();

	int GetCurFishItemCnt();

	void CheckCurMaxInventory();
	int GetCurMaxInventory();

public:
	ioUserFishItem();
	virtual ~ioUserFishItem();
};

