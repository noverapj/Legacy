#include "StdAfx.h"

#include "AnnouncementWnd.h"

template<> ioAnnouncementManualMgr* Singleton< ioAnnouncementManualMgr >::ms_Singleton = 0;

ioAnnouncementManualMgr::ioAnnouncementManualMgr()
{

}

ioAnnouncementManualMgr::~ioAnnouncementManualMgr()
{

}

ioAnnouncementManualMgr& ioAnnouncementManualMgr::GetSingleton()
{
	return Singleton< ioAnnouncementManualMgr >::GetSingleton();
}

ioAnnouncementManualMgr *ioAnnouncementManualMgr::GetSingletonPtr()
{
	return Singleton< ioAnnouncementManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
