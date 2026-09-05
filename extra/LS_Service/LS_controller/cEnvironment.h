// cEnvironment.h: interface for the cEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CENVIRONMENT_H__299EC96C_7762_41C7_997D_35C16F4E94A9__INCLUDED_)
#define AFX_CENVIRONMENT_H__299EC96C_7762_41C7_997D_35C16F4E94A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



const int PATH_MAXSIZE	= 250;

class cEnvironment
{
public :
	cEnvironment();

public:
	const uint32 GetMaxSession()			{	return m_maxSession;		}
	const uint32 GetFlushTime()				{	return m_flushTime;			}

	void SetMaxSession( uint32 sessionCount )		{	m_maxSession = sessionCount;	}
	void SetFlushTime( uint32 flushTime )			{	m_flushTime = flushTime;		}

private:
	uint32	m_maxSession;
	uint32  m_flushTime;
};

extern cEnvironment g_environment;

#endif // !defined(AFX_CENVIRONMENT_H__299EC96C_7762_41C7_997D_35C16F4E94A9__INCLUDED_)
