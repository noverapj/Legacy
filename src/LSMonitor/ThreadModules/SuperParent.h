// SuperParent.h: interface for the SuperParent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUPERPARENT_H__B172B135_64D2_4DAE_9AB8_668B8D68A2E7__INCLUDED_)
#define AFX_SUPERPARENT_H__B172B135_64D2_4DAE_9AB8_668B8D68A2E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThreadSync.h"

//동기화를 필요로 하는 객체는 이 클래스를 상속 받아야 한다.
class SuperParent  
{
	friend ThreadSync;
	CRITICAL_SECTION m_critical_section;

	public:
	bool equals(SuperParent *pParent) { return (this == pParent); }
	
	public:
	SuperParent();
	virtual ~SuperParent();
};

#endif // !defined(AFX_SUPERPARENT_H__B172B135_64D2_4DAE_9AB8_668B8D68A2E7__INCLUDED_)
