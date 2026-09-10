#pragma once

#include "ManualWnd.h"

class ioAnnouncementManualMgr : public ManualMgr , public Singleton< ioAnnouncementManualMgr >
{

public:
	static ioAnnouncementManualMgr& GetSingleton();
	static ioAnnouncementManualMgr *GetSingletonPtr();
public:
	ioAnnouncementManualMgr();
	virtual ~ioAnnouncementManualMgr();
};

#define g_AnnouncementManualMgr ioAnnouncementManualMgr::GetSingleton()
