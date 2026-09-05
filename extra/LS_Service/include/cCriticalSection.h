// cCriticalSection.h: interface for the cCriticalSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCRITICALSECTION_H__DB4999B0_D591_4B20_8D30_B6C442AC1C4B__INCLUDED_)
#define AFX_CCRITICALSECTION_H__DB4999B0_D591_4B20_8D30_B6C442AC1C4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000




class cCriticalSection  
{
public:
	cCriticalSection()	
	{
		InitializeCriticalSection( &m_CriticalSection );
	}

	~cCriticalSection()	{	DeleteCriticalSection( &m_CriticalSection );		}

public:
	void	Lock()		{	EnterCriticalSection( &m_CriticalSection );			}
	void	Unlock()	{	LeaveCriticalSection( &m_CriticalSection );			}

private:
	CRITICAL_SECTION	m_CriticalSection;
};




#endif // !defined(AFX_CCRITICALSECTION_H__DB4999B0_D591_4B20_8D30_B6C442AC1C4B__INCLUDED_)
