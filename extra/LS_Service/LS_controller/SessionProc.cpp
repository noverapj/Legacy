#include "StdAfx.h"
#include "SessionProc.h"
#include "SessionProc.h"
#include "../include/IIocpContext.h"
#include "../include/INetwork.h"
#include "../include/cSingleton.h"


void SessionProc::SendPacket(	
							 IIocpContext* context,
							const uint32 command, 
							const uint8* packet, 
							const uint32 length)
{
	if(context)
	{
		cSingleton<INetwork>::GetInstance()->Send(
			context,
			static_cast<uint16>(command),
			packet,
			static_cast<uint16>(length),
			FALSE);
	}
}

void SessionProc::Broadcast(	
							const uint32 command, 
							const uint8* packet, 
							const uint32 length)
{
	cSession* session = SessionProc::GetFirstSession();
	while(session)
	{
		SendPacket(
			session->GetContext(),
			command,
			packet,
			length);
		session = SessionProc::GetNextSession();
	}
}

BOOL SessionProc::AddSession(IIocpContext* context)
{
	cSession* session = g_sessionTable.Get(context->GetEntity());
	if(session) return FALSE;

	// 세션정보 추가
	return g_sessionTable.Insert(context->GetEntity(), context);
}


BOOL SessionProc::DeleteSession(IIocpContext* context)
{
	cSession* session = g_sessionTable.Get(context->GetEntity());
	if(session)
	{
		// 세션 종료시의 처리
		session->OnDisconnect();

		// 세션 정보 삭제
		g_sessionTable.Remove(context->GetEntity());
	}

	// 접속종료
	context->Disconnect();
	return TRUE;
}

uint32 SessionProc::GetSessionCount()
{
	return g_sessionTable.GetCount();
}

cSession* SessionProc::GetSession(const uint32 entity)
{
	return g_sessionTable.Get(entity);
}

cSession* SessionProc::GetFirstSession()
{
	return g_sessionTable.GetFirst();
}

cSession* SessionProc::GetNextSession()
{
	return g_sessionTable.GetNext();
}
