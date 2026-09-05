#include "StdAfx.h"
#include "cEnvironment.h"
#include "cSessionTable.h"
#include "SessionProc.h"
#include "InternalCommand.h"
#include "../include/IIocpContext.h"
#include "../include/INetwork.h"
#include "../include/cSingleton.h"
#include "../common/Internal.h"

void IConnect::Run(IIocpContext* context)
{
	Debug(_T("Connected\n"));
	// 세션추가
	if(!SessionProc::AddSession(context))
	{
		// 접속 중이던 세션이므로 종료가 필요함
		SessionProc::DeleteSession(context);
	}
}

void IClose::Run(IIocpContext* context)
{
	Debug(_T("Closed\n"));
	// 세션삭제
	SessionProc::DeleteSession(context);
}

void IPing::Run(const uint32 serverId)
{
	// 접속된 모든 클라이언트 소켓들에게 PING을 보낸다
	//Debug("@Ping\n");

	SessionProc::Broadcast(S_PING, NULL, 0);
}

void IFlush::Run(const uint32 serverId)
{
	//Debug("@Flush\n");

	cSession* session = SessionProc::GetFirstSession();
	while(session)
	{
		cSingleton<INetwork>::GetInstance()->Flush(session->GetContext());
		session = SessionProc::GetNextSession();
	}
}