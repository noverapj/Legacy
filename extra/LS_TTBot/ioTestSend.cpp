 
#include "StdAfx.h"
#include "Protocol.h"
#include "network/SP2Packet.h"
#include "ioTestSend.h"

ioTestSend::ioTestSend(void)
{
	Init();

}


ioTestSend::~ioTestSend(void)
{
	Destroy();
}

void ioTestSend::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_SEND);
	AddState(TESTSTATE_SEND, TESTSTATE_SEND);	
}

void ioTestSend::Destroy()
{
	LOG("disconnected, %s", m_IP);
}

void ioTestSend::Run()
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
	case TESTSTATE_SEND:
		{
			SP2Packet kPacket( DTPK_QUERY );
			m_socketTCP.SendPacket( kPacket );
		}
		break;

	case TESTSTATE_DISCONNECT :
		if(DoDisconnect())
			LOG("disconnected, %s", m_IP);
		else
			LOG("disconnected failed");
		break;

	default:
		return;
	}


	m_current = newState;
}

ValueType ioTestSend::GetValueType(VariableType nType,int len)
{
	ValueType vt;
	vt.type = nType;
	vt.size = len;
	return vt;
}
