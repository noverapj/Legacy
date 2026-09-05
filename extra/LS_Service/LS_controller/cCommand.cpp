#include "StdAfx.h"
#include "../include/IIocpContext.h"
#include "cCommand.h"

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL cCommand::Run(IIocpContext* context, const uint8* buffer, const uint32 payload)
{
	// 자신의 정보를 출력
	//Self();
	
	if(Verify(context, const_cast<uint8*>(buffer), payload))
	{
		// 명령처리
		Run(context);
		return TRUE;
	}
	return FALSE;
}

BOOL cCommand::Run(const uint32 serverId, const uint8* buffer, const uint32 payload)
{
	// 자신의 정보를 출력
	//Self();

	if(Verify(const_cast<uint8*>(buffer), payload))
	{
		// 명령처리
		Run(serverId);
		return TRUE;
	}
	return FALSE;
}
