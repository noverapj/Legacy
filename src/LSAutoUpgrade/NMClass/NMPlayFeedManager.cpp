#include "NMManager.h"
#include "NMPlayFeedManager.h"


BOOL NMPlayFeedManager::WritePlayLog(
	IN UINT32		uFeedCategory,
	IN UINT32		uFeedTypeNo,
	IN LPCTSTR		szPlayLog
)
{
	CNMPlayFeedWritePlayLogFunc func( uFeedCategory, uFeedTypeNo, szPlayLog );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMPlayFeedManager::PublishPlayFeed
(
	IN UINT32		uLogType,
	IN UINT32		uFeedCategory,
	IN UINT32		uFeedTypeNo,
	IN LPCTSTR		szPlayLog
)
{
	CNMPlayFeedPublishPlayFeedFunc func( uLogType, uFeedCategory, uFeedTypeNo, szPlayLog );
	return CNMManager::GetInstance().CallNMFunc( &func );
}
