#pragma once

class ioItemRechargeManager : public Singleton< ioItemRechargeManager >
{
protected:
	struct PromotionInfo
	{
		int m_iWeapon;
		int m_iArmor;
		int m_iHelmet;
		int m_iCloak;
	};

	typedef std::map< int, int > mapExceptionInfo;

protected:
	PromotionInfo    m_NormalItem;
	PromotionInfo    m_ExtraItem;
	PromotionInfo    m_RareItem;
	mapExceptionInfo m_ExceptionInfoList;

public:
	void LoadInIData();
	void LoadPromotionInfo( ioINILoader &rkLoader, PromotionInfo &rkInfo );

public:
	inline const int GetNormalWeaponPromotionTime() const { return m_NormalItem.m_iWeapon; }
	inline const int GetNormalArmorPromotionTime()  const { return m_NormalItem.m_iArmor; }
	inline const int GetNormalHelmetPromotionTime() const { return m_NormalItem.m_iHelmet; }
	inline const int GetNormalCloakPromotionTime()  const { return m_NormalItem.m_iCloak; }

	inline const int GetExtralWeaponPromotionTime() const { return m_ExtraItem.m_iWeapon; }
	inline const int GetExtralArmorPromotionTime()  const { return m_ExtraItem.m_iArmor; }
	inline const int GetExtralHelmetPromotionTime() const { return m_ExtraItem.m_iHelmet; }
	inline const int GetExtralCloakPromotionTime()  const { return m_ExtraItem.m_iCloak; }

	inline const int GetRareWeaponPromotionTime() const { return m_RareItem.m_iWeapon; }
	inline const int GetRareArmorPromotionTime()  const { return m_RareItem.m_iArmor; }
	inline const int GetRareHelmetPromotionTime() const { return m_RareItem.m_iHelmet; }
	inline const int GetRareCloakPromotionTime()  const { return m_RareItem.m_iCloak; }

	int GetPromotionTime( int iItemCode );

	bool SetRechargeExtraItem( User *pUser, int iSlotIndex, int iItemCode, int iRechargeTime, DWORD dwEtcItemCode );

public:
	static ioItemRechargeManager& GetSingleton();

public:   
	ioItemRechargeManager();
	virtual ~ioItemRechargeManager();
};
#define g_ItemRechargeMgr ioItemRechargeManager::GetSingleton()



class ioAccRechargeManager : public Singleton< ioAccRechargeManager >
{
protected:
	struct PromotionInfo
	{
		int nRing;
		int nNecklace;
		int nBracelet;
	};

	typedef std::map< int, int > mapExceptionInfo;

protected:
	PromotionInfo    m_NormalItem;
	mapExceptionInfo m_ExceptionInfoList;

public:
	void LoadInIData();
	void LoadPromotionInfo( ioINILoader &rkLoader, PromotionInfo &rkInfo );

public:
	int GetPromotionTime( int iItemCode );
	bool SetRechargeAccessory( User *pUser, int iSlotIndex, int iItemCode, int iRechargeTime, DWORD dwEtcItemCode );

public:
	static ioAccRechargeManager& GetSingleton();

public:   
	ioAccRechargeManager();
	virtual ~ioAccRechargeManager();
};
#define g_AccRechargeMgr ioAccRechargeManager::GetSingleton()


