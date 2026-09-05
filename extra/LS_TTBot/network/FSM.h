#pragma once

#define MAGIC_TOKEN_FSM		-1

class FSM
{
protected:
	int m_iState;

public:
	void InitState();
	void UpdateState();

public:
	bool IsFirst();	
	int GetState();
	void SetState(int iState);

public:
	FSM();
};
