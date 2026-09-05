#include "StdAfx.h"
#include "network/SecurityTCP.h"
#include "network/SecurityUDP.h"
#include "iotestclient.h"

ioTestClient::ioTestClient(void) : m_port(0)
{
	Init();
}

ioTestClient::~ioTestClient(void)
{
	Destroy();
}

void ioTestClient::Init()
{
}

void ioTestClient::Destroy()
{
}

void ioTestClient::Startup(const char* IP, const uint32 port)
{
	m_socketTCP.Startup();

	m_socketUDP.SetBindPort(10515);
	m_socketUDP.SetNS(new SecurityUDP);

	if(NULL != IP)
	{
		m_IP = IP;
	}
	if(0 != port)
	{
		m_port = port;
	}
}

BOOL ioTestClient::DoConnect(const BOOL thread)
{
	DoDisconnect();

	//if(m_socketTCP.Connect(m_IP, m_port, 10, thread))
	if(m_socketTCP.Connect(m_IP, m_port ))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL ioTestClient::DoDisconnect()
{
	if(m_socketTCP.IsConnected())
	{
		m_socketTCP.Disconnect();
		return TRUE;
	}
	return FALSE;
}

void ioTestClient::SendTCP(SP2Packet& packet)
{
	m_socketTCP.SendPacket(packet);
}

void ioTestClient::SendUDP(SP2Packet& packet)
{
	m_socketUDP.SendPacket(packet);
}