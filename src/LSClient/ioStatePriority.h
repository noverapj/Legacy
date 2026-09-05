#pragma once

class ioINILoader;

enum StatePriorityType
{
	SPT_DEFENSE,
	SPT_DASH,
	SPT_JUMP,
	SPT_GETUP,
	SPT_WOUNDED,
	SPT_ATTACK,
	SPT_EXTEND_ATTACK,
	SPT_DELAY,
	SPT_RUN,
	MAX_SPT_TYPE
};

class ioStatePriority
{
private:
	int m_Priority[MAX_SPT_TYPE];

public:
	void LoadPriority( ioINILoader &rkLoader );
	int GetPriority( StatePriorityType eType );
	void SetPriority( StatePriorityType eType , int iValue);

public:
	ioStatePriority();
	~ioStatePriority();
};

