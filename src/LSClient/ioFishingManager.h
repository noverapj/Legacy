#pragma once

struct FishingItem
{
	ioHashString m_Name;
	ioHashString m_IconName;
	ioHashString m_Desc;

	int m_iItemNum;
	__int64 m_iPeso;

	DWORD m_dwEtcItemCode;

	FishingGearType m_GearType;

	int m_iOrder;

	FishingItem()
	{
		m_iItemNum = 0;
		m_iPeso = 0;
		m_dwEtcItemCode = 0;
		m_GearType = FGT_NONE;
		m_iOrder = 0;
	}
};
typedef std::vector<FishingItem> FishingItemList;

struct FishingItemGrade
{
	int m_iGradeNum;
	ioHashString m_Name;
	ioHashString m_Desc;
	float m_fValue;

	FishingItemGrade()
	{
		m_iGradeNum = 0;
		m_fValue = FLOAT1;
	}
};
typedef std::vector<FishingItemGrade> FishingItemGradeList;

struct FishingTypeInfo
{
	ioHashString m_FishingRod;
	ioHashString m_FishingReadyEffect;
	ioHashString m_FishingWaitEffect;
	ioHashString m_FishingPullEffect;
};
typedef std::vector<FishingTypeInfo> FishingTypeInfoList;

class ioFishingManager : public Singleton< ioFishingManager >
{
protected:
	FishingItemList m_vFishingItemList;
	FishingItemList m_vFishingGearItemList;
	FishingItemGradeList m_vFishingItemGradeList;
	FishingTypeInfoList m_vFishingTypeInfoList;

public:
	void LoadFishingInfo();
	void LoadGuildFishingInfo();

	void LoadFishItemList( ioINILoader& rkLoader );
	void LoadFishGradeList( ioINILoader& rkLoader );
	void LoadFishTypeList( ioINILoader& rkLoader );

	void LoadFishingRodItemList( ioINILoader& rkLoader );
	void LoadFishingBaitItemList( ioINILoader& rkLoader );
	void LoadFishingGroundItemList( ioINILoader& rkLoader );
 
protected:
	void ClearAllInfo();

public:
	ioHashString GetFishingItemName( int iItemType, FishingGearType GearType = FGT_NONE );
	ioHashString GetFishingItemDesc( int iItemType, FishingGearType GearType = FGT_NONE );
	ioHashString GetFishingItemIconName( int iItemType, FishingGearType GearType = FGT_NONE );
	ioHashString GetFishingItemGradeName( int iItemGrade );
	ioHashString GetFishingItemGradeDesc( int iItemGrade );
	ioHashString GetFishingPresentItemName( short iPresentType, int iValue1, int iValue2 );

	ioHashString GetFishingRod( int iType );
	ioHashString GetFishingReadyEffect( int iType );
	ioHashString GetFishingWaitEffect( int iType );
	ioHashString GetFishingPullEffect( int iType );

	void MoveFishingRodData( FishItemList& rkFishItemList, FishingGearType GearType );

	int GetFishingGearNumByItemCode( DWORD dwItemCode );

	DWORD GetFishingGearCodeByTypeNum( FishingGearType GearType, int iItemNum );

public:
	inline int GetFishingItemGradeSize(){ return m_vFishingItemGradeList.size(); }

public:
	static ioFishingManager& GetSingleton();

public:
	ioFishingManager();
	virtual ~ioFishingManager();
};

#define g_FishingMgr ioFishingManager::GetSingleton()

