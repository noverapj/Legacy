#ifndef __NMPLAYFEEDMANAGER_H_51417972_D476_4D1F_9302_DD39F91613B4__
#define __NMPLAYFEEDMANAGER_H_51417972_D476_4D1F_9302_DD39F91613B4__


#include "NMGeneral.h"


namespace NMPlayFeedManager
{
	BOOL WritePlayLog
	(
		IN UINT32		uFeedCategory,
		IN UINT32		uFeedTypeNo,
		IN LPCTSTR		szPlayLog
	);

	BOOL PublishPlayFeed
	(
		IN UINT32		uLogType,
		IN UINT32		uFeedCategory,
		IN UINT32		uFeedTypeNo,
		IN LPCTSTR		szPlayLog
	);
};

#endif	//	#ifndef __NMPLAYFEEDMANAGER_H_51417972_D476_4D1F_9302_DD39F91613B4__