#pragma once

class ioTimeGate
{
public:
	ioTimeGate();
	~ioTimeGate();

	void Init();
	void Destroy();

public:
	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket );

};