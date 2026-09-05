#pragma once

#include "ResultInfo.h"

class ioPlayStage;

class ioObserverWndManager : public Singleton< ioObserverWndManager >
{	
protected:
	ioPlayStage *m_pPlayStage;

	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_RoundInfoList;

public:
	void ObserverWndShow();
	void ObserverWndHide();
	void ObserverInfoShow( bool bSpecial );

	void ClearInfo();
	void SetRoundInfo( RoundInfo &rkInfo );
	void UpdateInfo( ModeType eModeType );

public:
	static ioObserverWndManager &GetSingleton();

public:
	ioObserverWndManager( ioPlayStage *pStage );
	virtual ~ioObserverWndManager();
};

#define g_ObserverWndMgr ioObserverWndManager::GetSingleton()

