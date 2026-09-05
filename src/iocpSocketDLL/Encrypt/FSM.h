

#ifndef _FSM_h_
#define _FSM_h_

#define MAGIC_TOKEN_FSM		-1

class IOCP_SOCKET_API FSM
{
protected:
	int		m_iState;

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

#endif