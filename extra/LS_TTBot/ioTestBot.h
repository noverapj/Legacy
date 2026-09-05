#pragma once

#include "ioTestClient.h"


class ioTestBot : public ioTestClient
{
public:
	ioTestBot(void);
	~ioTestBot(void);

	void Init();
	void Destroy();

public:
	void PacketProcess(SP2Packet& packet);

protected:
	int m_index;
};
