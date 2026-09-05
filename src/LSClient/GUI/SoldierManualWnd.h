#pragma once

#include "ManualWnd.h"

class ioSoldierManualMgr : public ManualMgr , public Singleton< ioSoldierManualMgr >
{

public:
	static ioSoldierManualMgr& GetSingleton();
	static ioSoldierManualMgr *GetSingletonPtr();
public:
	ioSoldierManualMgr();
	virtual ~ioSoldierManualMgr();
};

#define g_SoldierManualMgr ioSoldierManualMgr::GetSingleton()


