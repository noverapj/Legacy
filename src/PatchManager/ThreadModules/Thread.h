// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__A7D2C074_CC00_4B9B_8B1F_4B332E6B33D3__INCLUDED_)
#define AFX_THREAD_H__A7D2C074_CC00_4B9B_8B1F_4B332E6B33D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SuperParent.h"
class Thread : public SuperParent 
{
	private:
	static DWORD WINAPI HandleRunner(LPVOID parameter);
	DWORD m_threadID;

	protected:
	HANDLE m_hThred;
	
	public:
	virtual void Run() = 0;            //상속 받은 클래스가 구현한다.

	public:
	DWORD GetThread(){ return m_threadID; }
	void  Begin();

	public:
	Thread();
	virtual ~Thread();
};

#endif // !defined(AFX_THREAD_H__A7D2C074_CC00_4B9B_8B1F_4B332E6B33D3__INCLUDED_)
