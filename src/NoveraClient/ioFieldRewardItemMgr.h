#pragma once


class ioFieldRewardItemMgr : public Singleton< ioFieldRewardItemMgr >
{
protected:
	struct RewardData
	{
		ioHashString m_MeshName;
		ioHashString m_IconName;
	};
	typedef std::map< DWORD, RewardData *> RewardDataMap;
	RewardDataMap m_RewardDataMap;

public:
	void LoadINIData();

public:
	const ioHashString &GetMeshName( DWORD dwIndex );
	const ioHashString &GetIconName( DWORD dwIndex );

public:
	static ioFieldRewardItemMgr& GetSingleton();

public:   
	ioFieldRewardItemMgr();
	virtual ~ioFieldRewardItemMgr();
};
#define g_FieldRewardItemMgr ioFieldRewardItemMgr::GetSingleton()
