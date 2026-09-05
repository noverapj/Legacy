// ThreadSync.h: interface for the ThreadSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADSYNC_H__5C2AF66B_BD08_4550_98D4_FE66B4F74737__INCLUDED_)
#define AFX_THREADSYNC_H__5C2AF66B_BD08_4550_98D4_FE66B4F74737__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SuperParent;
class ThreadSync  
{
	private:
	SuperParent *m_pSuperParent;

	public:
	ThreadSync(SuperParent *pParent);
	virtual ~ThreadSync();
};

#endif // !defined(AFX_THREADSYNC_H__5C2AF66B_BD08_4550_98D4_FE66B4F74737__INCLUDED_)
