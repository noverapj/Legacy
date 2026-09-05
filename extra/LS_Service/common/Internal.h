#pragma once

#include "Protocol.h"

// internal protocol
enum InternalProtocol
{
	BEGIN_INTERNAL_COMMAND = 0x0000,

	I_CONNECT,
	I_CLOSE,
	I_PING,
	I_FLUSH,


	C_COLLABORATE = 0x0080,		S_COLLABORATE,
	C_USERCOUNT,

	END_INTERNAL_COMMAND
};



#pragma pack()
