#include "StdAfx.h"
#include "LS_TTBot.h"
#include "LS_TTBotDlg.h"
#include "iotestcase.h"

ioTestCase::ioTestCase(void) : m_sequence(0), m_current(TESTSTATE_NONE)
{
	Init();
}

ioTestCase::~ioTestCase(void)
{
	Destroy();
}

void ioTestCase::Init()
{
}

void ioTestCase::Destroy()
{
}

void ioTestCase::Run()
{
}

void ioTestCase::LOG(const TCHAR *format, ...)
{
	TCHAR buffer[2048]; 
	va_list marker; 

	va_start( marker, format ); 
	_vsnprintf( buffer, sizeof(buffer), format, marker ); 
	va_end( marker );

	((CLS_TTBotDlg*)AfxGetMainWnd())->LOG("%s", buffer);
}

void ioTestCase::AddState(int oldState, int newState)
{
	m_testCases[oldState] = newState;
}

int ioTestCase::GetNextState()
{
	++m_sequence;
	TEST_CASES::iterator it =  m_testCases.find(m_current);
	if(it != m_testCases.end())
	{
		int newState = it->second;
		return newState;
	}
	return TESTSTATE_NONE;
}

