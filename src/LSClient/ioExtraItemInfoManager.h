#pragma once

#include "EncryptValue.h"

///////////////////////////////////////////////////////////////////////////
struct ItemListInfo
{
	int		m_iItemCode;
	int		m_iShopMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,

	ItemListInfo()
	{
		m_iItemCode		= 0;
		m_iShopMarkType	= 0;         // SHOP_MARK_TYPE_NONE
	}
};
typedef std::vector< ItemListInfo > ItemListInfoVector;

struct ItemListInfoSort
{
	int m_iItemCode;
	int m_iShopMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	int m_iOrder;

	ItemListInfoSort()
	{
		m_iItemCode     = 0;
		m_iShopMarkType = 0;
		m_iOrder		= 0;
	}
};
typedef std::vector< ItemListInfoSort > ItemListInfoSortVector;

///////////////////////////////////////////////////////////////////////////

class RandomMachineInfo
{
public:
	enum //MachineCode
	{
		MC_BASIC    = 1,
		MC_MORTMAIN,
		MC_MORTMAIN_WEAPON,
		MC_MORTMAIN_ARMOR,
		MC_MORTMAIN_HELM,
		MC_MORTMAIN_CLOAK,
		MC_BASIC_WEAPON,
		MC_BASIC_ARMOR,
		MC_BASIC_HELM,
		MC_BASIC_CLOAK,
	};

	enum MachineType  // array 순서 변경 금지 , 범위 체크하는 부분 있음
	{
		MT_NONE        = 0,
		MT_WEAPON      = 1,
		MT_ARMOR       = 2,
		MT_HELMET      = 3,
		MT_CLOAK       = 4,
		MT_ALL         = 5,
		MT_RARE_WEAPON = 6,
		MT_RARE_ARMOR  = 7,
		MT_RARE_HELMET = 8,
		MT_RARE_CLOAK  = 9,
		MT_RARE_ALL    = 10,
		MT_HIGH_EXTRA  = 11,
		MT_LIMITED_EXTRA = 12,
	};

public:
	friend class ioExtraItemInfoManager;

protected:
	MachineType m_eDefaultType;

	ioHashString m_Name;
	ioHashString m_IconName;

	int m_iMachineCode;
	int m_iGroup;		// 보급, 무기, 갑옷, 투구, 망토 구분.
	
	IntVec m_vNeedPeso;
	IntVec m_vPeriod;
	int m_iNeedCash;
	int m_iBonusPeso;
	int m_iSellPeso;
	int m_iNeedLevel;
	int m_iPriority;				// ORDER_DEFAULT
	int m_iMainManual;
	int m_iRecommandedShopOrder;
	int m_iShopMarkType;			// SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	bool m_bPackage;
	bool m_bActive;

	int m_iSubscriptionType;
	int m_iLimitDate;

	ItemListInfoVector m_vItemList;
	int m_nGradeType;

public:
	int GetNeedPeso( int iArray ) const;
	int GetPeriod( int iArray ) const;
	int GetNeedPesoSize() const { return m_vNeedPeso.size(); }
	void SetNeedPeso( int iNeedPeso, int iArray );

	inline int GetMachineCode() const { return m_iMachineCode; }
	inline int GetGroup() const { return m_iGroup; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetIconName() const { return m_IconName; }

	inline int GetNeedCash() const { return m_iNeedCash; }
	inline int GetNeedLevel() const { return m_iNeedLevel; }
	inline int GetBonusPeso() const { return m_iBonusPeso; }
	inline int GetSellPeso() const { return m_iSellPeso; }
	inline int GetPriority() const { return m_iPriority; }
	inline int GetMainManual() const { return m_iMainManual; }
	inline int GetRecommandedShopOrder() const { return m_iRecommandedShopOrder; }
	inline int GetShopMarkType() const { return m_iShopMarkType; }	
	inline int GetLimitDate() const { return m_iLimitDate; }
	inline bool IsPackage() const { return m_bPackage; }
	inline bool IsActive() const { return m_bActive; }
	inline int GetSubscriptionType() const { return m_iSubscriptionType; }
	inline int GetGradeType() const { return m_nGradeType; }
	

	void SetActive( bool bActive ) { m_bActive = bActive; }
	void SetNeedCash( int iNeedCash ) { m_iNeedCash = iNeedCash; }
	void SetPriority( int iPriority ) { m_iPriority = iPriority; }
	void SetRecommandedShopOrder( int iRecommandedShopOrder ) { m_iRecommandedShopOrder = iRecommandedShopOrder; }
	void SetShopMarkType( int iShopMarkType ) { m_iShopMarkType = iShopMarkType; }

	int GetItemCode( int i )
	{
		if ( i < 0 ) return -1;
		if ( i >= static_cast<int>(m_vItemList.size()) ) return -1;
		return m_vItemList[ i ].m_iItemCode;
	}

	void LoadMachineBasicInfo( ioINILoader &rkLoader );
	void LoadMachineRandomInfo( ioINILoader &rkLoader, bool bItemLoadAll );

public:
	RandomMachineInfo();
	virtual ~RandomMachineInfo();
};
typedef std::vector< RandomMachineInfo* > RandomMachineInfoVector;

///////////////////////////////////////////////////////////////////////////

class ioExtraItemInfoManager : public Singleton< ioExtraItemInfoManager >
{
protected:
	CEncrypt<int> m_iLevelLimitConst;
	CEncrypt<float> m_fMortmainItemSell;
	CEncrypt<float> m_fTimeItemSell;
	CEncrypt<float> m_fMortmainItemSellRate;
	CEncrypt<float> m_fMortmainItemSellMax;
	CEncrypt<float> m_fTimeItemSellRate;
	CEncrypt<float> m_fTimeItemSellMax;

	RandomMachineInfoVector m_MachineInfoList;
	RandomMachineInfoVector m_DefaultMachineInfoList;

	typedef std::map< int, RandomMachineInfo* > MachineInfoMap;
	MachineInfoMap m_InfoMap;

	bool m_bTestRandomItem;

public:
	void LoadRandomMachineInfo();

protected:
	void ClearAllInfo();
	bool IsRightExtraItem( RandomMachineInfo::MachineType eType, int iItemCode );
	void CopyDefaultMachineBasicInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo );
	bool CopyDefaultMachineRandomInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo );
	void SetAutoMachineBasicInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo );
	int CalculateOrder( int iItemCode );

public:
	int GetRadomMachineCnt();

	RandomMachineInfo* GetRandomMachineInfoByArray( int iArray );
	RandomMachineInfo* GetRandomMachineInfo( int iCode );

	bool GetRandomItemList( int iMachineCode, ItemListInfoSortVector &rkList );

	inline int GetLevelLimitConst() const { return m_iLevelLimitConst; }
	inline float GetMortmainItemSellPeso() const { return m_fMortmainItemSell; }
	inline float GetTimeItemSellPeso() const { return m_fTimeItemSell; }
	inline bool IsTestLOG() const { return m_bTestRandomItem; }

	int GetExtraItemRecommandedCnt();
	float GetMortmainItemSellPesoReinforce( int nReinforce );
	float GetTimeItemSellPesoReinforce( DWORD dwTime, int nReinforce );

public:
	static ioExtraItemInfoManager& GetSingleton();

public:
	ioExtraItemInfoManager();
	virtual ~ioExtraItemInfoManager();
};

#define g_ExtraItemInfoMgr ioExtraItemInfoManager::GetSingleton()

