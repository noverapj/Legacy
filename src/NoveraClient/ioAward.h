#pragma once

class SP2Packet;
class ioAward
{
protected:
	int      m_iAwardLevel;
	int      m_iAwardExp;

public:
	void Initialize();

public:
	void ApplyAwardData( SP2Packet &rkPacket );	

public:
	bool AddAwardExp( int iExp );
	int GetAwardLevel();
	int GetAwardExp();

public:
	ioAward();
	virtual ~ioAward();
};

