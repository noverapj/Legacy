
#ifndef _CustomTournamentEtcItemManager_h_
#define _CustomTournamentEtcItemManager_h_

class ioEtcItem;
struct TournamentRewardItem
{
	DWORD m_dwRewardItemPirce;
	ioHashString m_szRewardItemName;
	ioUIRenderImage* m_pIcon;
	ioUIRenderImage* m_pSubIcon;

	TournamentRewardItem()
	{
		m_dwRewardItemPirce = 0;

		m_pIcon = NULL;
		m_pSubIcon = NULL;
	}
};
typedef std::map<DWORD, TournamentRewardItem*> TournamentRewardItemMap;

class TournamentRewardManager : public Singleton< TournamentRewardManager >
{
protected:
	TournamentRewardItemMap m_TournamentRewardItemMap;

public:
	void Initialize();
	
public:
	static TournamentRewardManager& GetSingleton();

public:
	inline const TournamentRewardItemMap& GetTournamentRewardItemMap() const { return m_TournamentRewardItemMap; }

	const TournamentRewardItem* FindRewardItem( DWORD dwType ) const ;
	void DestroyRewardItem();

public:
	TournamentRewardManager();
	virtual ~TournamentRewardManager();
};

#define g_TourRewardItemManager TournamentRewardManager::GetSingleton()

#endif
