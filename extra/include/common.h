#pragma once

#ifndef _LS_BASIC_TYPES_
#define _LS_BASIC_TYPES_
	typedef unsigned int		uint;  
	typedef unsigned char		uchar;
	typedef unsigned short		ushort;
	typedef unsigned long		ulong;
	typedef unsigned long		uint32;
	typedef unsigned short		uint16;
	typedef unsigned char		uint8;
	typedef unsigned __int64	uint64;
	typedef char				int8;
	typedef short				int16;
	typedef long				int32;
	typedef	__int64				int64;
	
	typedef	uint64				uniq;
	typedef	uint32				unit;
#endif

	
	namespace EPROTOCOL
	{
		enum PROTOCOL
		{
			TEST_SEND = 0xd001,
			MNSTPK_STATUS_REQUEST = 0x10601,
			MNSTPK_STATUS_RESULT  =	0x10602,
			MNSTPK_CHANGE_REQUEST = 0x10603,
			MNSTPK_CHANGE_RESULT  = 0x10604,
			MNSTPK_CLOSE		  = 0x10605,
			LSTPK_STATUS_REQUEST = 0x10680,
			LSTPK_CONNECT_CLIENT = 0x10681,
			LSTPK_SVR_STATUS_RESPONSE = 0x10682,
			LSPTK_LS_LOGINSVR_CONNECT = 0x10683,
			LSPTK_TICKET_REQUEST = 0x10684,
			LSPTK_TICKET_RESPONSE = 0x10685,
			LSPTK_TIMEOUT_CLOSE_REQUEST = 0x10686,
			LSPTK_ERROR = 0x10687,
			LSPTK_CONNECT_MONITOR = 0x10688,
			LSPTK_CONTROL_SERVER = 0x10689,
			LSPTK_SERVER_RESPONSE = 0x10690,
			LSPTK_PING = 0x10691,
			LSPTK_SERVER_FILLINFO = 0x10692,


		};
	}