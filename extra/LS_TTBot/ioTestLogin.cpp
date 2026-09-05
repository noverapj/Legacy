#include "StdAfx.h"
#include "Protocol.h"
#include "network/SP2Packet.h"
#include "ioTestLogin.h"

ioTestLogin::ioTestLogin(void)
{
	Init();
}

ioTestLogin::~ioTestLogin(void)
{
	Destroy();
}

void ioTestLogin::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_LOGIN);
	AddState(TESTSTATE_LOGIN, TESTSTATE_DISCONNECT);
	AddState(TESTSTATE_DISCONNECT, TESTSTATE_CONNECT);
}

void ioTestLogin::Destroy()
{
}

void ioTestLogin::Run()
{
	int newState = GetNextState();
	switch(newState)
	{
	case TESTSTATE_CONNECT :
		if(DoConnect())
			LOG("connected, %s", m_IP);
		else
			LOG("connect failed");
		break;

	case TESTSTATE_DISCONNECT :
		if(DoDisconnect())
			LOG("disconnected, %s", m_IP);
		else
			LOG("disconnected failed");
		break;

	case TESTSTATE_LOGIN :
		{
			LOG("try login");

			SP2Packet kPacket( CTPK_CONNECT );
			kPacket << "e68a849fdd2d7662b96e91786a2486787080ffaa87c638" << "172.20.20.108" << 8652 << 119483910;

			SendTCP(kPacket);
		}
		break;

	default:
		return;
	}

	m_current = newState;
}
