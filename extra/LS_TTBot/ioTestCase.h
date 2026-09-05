#pragma once

#include <map>
#include "ioTestBot.h"

enum TestStates
{
	TESTSTATE_NONE = 0,
	TESTSTATE_CONNECT,
	TESTSTATE_DISCONNECT,
	TESTSTATE_LOGIN,
	TESTSTATE_ECHO,
	TESTSTATE_SEND,

	TESTSTATE_END
};

class ioTestCase : public ioTestBot
{
public:
	ioTestCase(void);
	~ioTestCase(void);

	void Init();
	void Destroy();

public:
	virtual void Run();

	void LOG(const TCHAR *format, ...);

public:
	void AddState(int oldState, int newState);
	int GetNextState();
	int GetCurrentState() { return m_current; }


	uint32 Current() const { return m_recvstate; }
	void Current(uint32 val) { m_recvstate = val; }
protected:
	typedef std::map<int,int> TEST_CASES;

	uint32 m_sequence;
	uint32 m_current;
	uint32 m_recvstate;
	TEST_CASES m_testCases;

};
