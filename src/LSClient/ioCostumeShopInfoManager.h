#pragma once

#include "CostumeEnumType.h"

//코스튬 상점 정보
class CostumeShopInfo
{
protected:
	int m_nDefaultType;

	int m_nCostumeShopCode;
	int m_nGradeType;
	int m_nGroup;
	int m_nNeedCash;
	int m_nBonusPeso;
	int m_nSellPeso;
	int m_nNeedLevel;
	int m_nPriority;				
	int m_nMainManual;
	int m_nRecommandedShopOrder;
	int m_nShopMarkType;
	int m_nLimitDate;
	int m_nSubscriptionType;
	bool m_bPackage;
	bool m_bActive;

	ioHashString m_szName;
	ioHashString m_szIconName;

	IntVec m_vNeedPeso;
	IntVec m_vPeriod;
	CostumeShopSubInfoList m_CostumeShopSubInfoList;

	bool m_bSetPreview;

public:
	void Init();
	void LoadCostumeShopBasicInfo( ioINILoader &rkLoader );
	void LoadCostumeShopSubInfo( ioINILoader &rkLoader );

public:
	int GetNeedPeso( int nArray ) const;
	int GetPeriod( int nArray ) const;
	int GetNeedPesoSize() { return m_vNeedPeso.size(); }
	void SetNeedPeso( int nNeedPeso, int nArray );
	bool GetCostumeSubInfo( int nArray, CostumeShopSubInfo &sCostumeShopSubInfo );
	int GetCostumeSubInfoCnt();

public:
	void SetActive( bool bActive ) { m_bActive = bActive; }
	void SetNeedCash( int nNeedCash ) { m_nNeedCash = nNeedCash; }
	void SetPriority( int nPriority ) { m_nPriority = nPriority; }
	void SetRecommandedShopOrder( int nRecommandedShopOrder ) { m_nRecommandedShopOrder = nRecommandedShopOrder; }
	void SetShopMarkType( int nShopMarkType ) { m_nShopMarkType = nShopMarkType; }
	void SetLimitDate( int nLimitDate ){ m_nLimitDate = nLimitDate; }

public:
	inline int GetCostumeDefault() const { return m_nDefaultType; }
	inline int GetCostumeShopCode() const { return m_nCostumeShopCode; }
	inline int GetGradeType() const { return m_nGradeType; }
	inline int GetGroup() const { return m_nGroup; }

	inline ioHashString &GetName() { return m_szName; }
	inline ioHashString &GetIconName() { return m_szIconName; }

	inline int GetNeedCash() const { return m_nNeedCash; }
	inline int GetNeedLevel() const { return m_nNeedLevel; }
	inline int GetBonusPeso() const { return m_nBonusPeso; }
	inline int GetSellPeso() const { return m_nSellPeso; }
	inline int GetPriority() const { return m_nPriority; }
	inline int GetMainManual() const { return m_nMainManual; }
	inline int GetRecommandedShopOrder() const { return m_nRecommandedShopOrder; }
	inline int GetShopMarkType() const { return m_nShopMarkType; }	
	inline int GetLimitDate() const { return m_nLimitDate; }
	inline bool IsPackage() const { return m_bPackage; }
	inline bool IsActive() const { return m_bActive; }
	inline int GetSubscriptionType() const { return m_nSubscriptionType; }
	inline bool IsPreview() const { return m_bSetPreview; }
	
public:
	void SetDefaultData( const CostumeShopInfo* pInfo );

public:
	CostumeShopInfo();
	virtual ~CostumeShopInfo();
};

typedef std::vector<CostumeShopInfo*> CostumeShopInfoList;

////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioCostumeShopInfoManager : public Singleton< ioCostumeShopInfoManager >
{
protected:
	CostumeShopInfoList m_DefaultCostumeShopInfoList;
	CostumeShopInfoList m_CostumeShopInfoList;

	float m_fMortmainItemSellPrice;
	float m_fTimeItemSellPrice;

public:
	void LoadCostumeMachineInfo();

protected:
	void ClearAllInfo();
	int CalculateOrder( int nCostumeShopCode );

public:
	CostumeShopInfo* GetCostumeMachineInfoByArray( int nArray );
	CostumeShopInfo* GetCostumeMachineInfo( int nCode );
	CostumeShopInfo* GetDefaultCostumeMachineInfo( int nDefaultType );

	bool GetCostumeItemList( int nCostumeShopCode, CostumeShopSubInfoList &rkList );

	int GetCostumeMachineCnt();	
	int GetCostumeRecommandedCnt();

	float GetResellMortmainCostumePeso();
	float GetResellTimeCostumeTimePeso();

public:
	static ioCostumeShopInfoManager& GetSingleton();

public:
	ioCostumeShopInfoManager();
	virtual ~ioCostumeShopInfoManager();
};

#define g_CostumeShopInfoMgr ioCostumeShopInfoManager::GetSingleton()