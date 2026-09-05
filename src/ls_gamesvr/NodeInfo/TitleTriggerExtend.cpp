#include "stdafx.h"
#include "UserTitleInfo.h"
#include "TitleTriggerExtend.h"

BOOL TitleTriggerAccumulateEvent::DoTrigger(UserTitleInfo* pData, const __int64  iValue)
{
	if( !pData )
		return FALSE;

	pData->AddValue(iValue);

	return TRUE;
}


BOOL TitleTriggerNoAction::DoTrigger(UserTitleInfo* pData, const __int64  iValue)
{
	if( !pData )
		return FALSE;

	return TRUE;
}