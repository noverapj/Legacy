#include "StdAfx.h"
#include "Protocol.h"
#include "network/SP2Packet.h"
#include "ioTestEcho.h"

ioTestEcho::ioTestEcho(void)
{
	Init();
}

ioTestEcho::~ioTestEcho(void)
{
	Destroy();
}

void ioTestEcho::Init()
{
	AddState(TESTSTATE_NONE, TESTSTATE_CONNECT);
	AddState(TESTSTATE_CONNECT, TESTSTATE_ECHO);
	AddState(TESTSTATE_ECHO, TESTSTATE_ECHO);
}

void ioTestEcho::Destroy()
{
}

static int COUNT = 0;

void ioTestEcho::Run()
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

	case TESTSTATE_ECHO :
		{
			//LOG("[%lu]try echo", m_sequence);
			char temp[512];
			sprintf_s(temp, sizeof(temp), "@%dæ»≥Á«œººø‰!æ»≥Á«œººø‰!æ»≥Á«œººø‰!æ»≥Á«œººø‰!æ»≥Á«œººø‰!æ»≥Á«œººø‰!æ»≥Á«œººø‰!", ++COUNT);
			for(int i = 0 ; i < 20 ; i++)
			{
				SP2Packet kPacket( CTPK_ABSTRACT );
				kPacket << 2930;
				kPacket << "≈◊Ω∫∆Æ∫ø";
				kPacket << temp;
				//SP2Packet kPacket( CTPK_CONNECT );
				//kPacket << "e68a849fdd2d7662b96e91786a2486787080ffaa87c638" << "172.20.20.108" << 8652 << 119483910;
				SendTCP(kPacket);
			}
		}
		break;

	default:
		return;
	}

	m_current = newState;
}
