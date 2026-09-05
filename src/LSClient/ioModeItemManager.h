#pragma once

class ioPlayStage;
class ioINILoader;
class ioModeItem;

class ioModeItemManager : public Singleton< ioModeItemManager >
{
protected:
	typedef std::map< int, ioModeItem* > ModeItemMap;
	typedef ModeItemMap::iterator        ModeItemMap_iter;

protected:
	ioPlayStage *m_pPlayStage;
	ModeItemMap  m_ModeItemMap;

public:
	void ClearModeItemList();
	void LoadItemList();
	ioModeItem *CreateModeItemTemplete( ioINILoader &rkLoader );
	ioModeItem *CreateModeItem( int iType );

public:
	inline void SetPlayStage( ioPlayStage *pStage ) { m_pPlayStage = pStage; }
	inline ioPlayStage* GetPlayStage() { return m_pPlayStage; }

public:
	static ioModeItemManager& GetSingeton();

public:
	ioModeItemManager();
	virtual ~ioModeItemManager();
};

#define g_ModeItemManager ioModeItemManager::GetSingleton()