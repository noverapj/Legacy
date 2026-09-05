// ThreadSync.h: interface for the ThreadSync class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class SuperParent;

class IOCP_SOCKET_API ThreadSync  
{
	private:
	SuperParent *m_pSuperParent;

	public:
	ThreadSync(SuperParent *pParent);
	virtual ~ThreadSync();
};
