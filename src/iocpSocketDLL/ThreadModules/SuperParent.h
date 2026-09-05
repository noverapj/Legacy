// SuperParent.h: interface for the SuperParent class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ThreadSync.h"

class CPacket;

//동기화 & 메모리 풀을 필요로 하는 객체는 이 클래스를 상속 받아야 한다.
class IOCP_SOCKET_API SuperParent  
{
	friend ThreadSync;
	CRITICAL_SECTION m_critical_section;

	public:
	bool equals(SuperParent *pParent) { return (this == pParent); }
	
	public:
	SuperParent();
	virtual ~SuperParent();
};

//네트웍 클래스는 이 클래스를 상속받아야함
class IOCP_SOCKET_API NetworkParent
{
	public:
	virtual bool SendMessage( CPacket &rkPacket, const BOOL bImmediatelySend = FALSE ) = 0;

	public:
	NetworkParent();
	virtual ~NetworkParent();
};
