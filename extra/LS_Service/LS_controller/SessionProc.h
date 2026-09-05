#pragma once

#include "cSessionTable.h"

class IIocpContext;

class SessionProc
{
public:
	static void SendPacket(
		IIocpContext* context,
		const uint32 command, 
		const uint8* packet, 
		const uint32 length);
							
	static void Broadcast(	
		const uint32 command, 
		const uint8* packet, 
		const uint32 length);

public:
	static BOOL AddSession(IIocpContext* context);
	static BOOL DeleteSession(IIocpContext* context);

public:
	static uint32 GetSessionCount();

	static cSession* GetSession(const uint32 entity);

	static cSession* GetFirstSession();
	static cSession* GetNextSession();
};
