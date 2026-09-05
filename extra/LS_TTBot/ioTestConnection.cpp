#include "StdAfx.h"
#include "iotestconnection.h"

ioTestConnection::ioTestConnection(void)
{
	Init();
}

ioTestConnection::~ioTestConnection(void)
{
	Destroy();
}

void ioTestConnection::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_DISCONNECT);
	AddState(TESTSTATE_DISCONNECT, TESTSTATE_CONNECT);
}

void ioTestConnection::Destroy()
{
	LOG("disconnected, %s", m_IP);
}

void ioTestConnection::Run()
{
	int oldState = GetCurrentState();
	int newState = GetNextState();
	switch(newState)
	{
	case TESTSTATE_CONNECT :
		if(DoConnect(FALSE))
		{
			SYSTEMTIME systime;
			GetLocalTime(&systime);
			LOG("[%02d:%02d:%02d]connected, %s", systime.wHour, systime.wMinute, systime.wSecond, m_IP);
		}
		else
		{
			LOG("connect failed");
		}
		break;

	case TESTSTATE_DISCONNECT :
		if(DoDisconnect())
			LOG("disconnected, %s", m_IP);
		else
		{
			LOG("disconnected failed");
		}
		break;

	default:
		return;
	}

	m_current = newState;
}
