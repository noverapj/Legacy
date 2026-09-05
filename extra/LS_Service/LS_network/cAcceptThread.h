// cAcceptThread.h: interface for the cAcceptThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACCEPTTHREAD_H__91DFC159_6F46_446D_BF50_61F70967AB34__INCLUDED_)
#define AFX_CACCEPTTHREAD_H__91DFC159_6F46_446D_BF50_61F70967AB34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cBaseThread.h"


class cAcceptThread : public cBaseThread  
{
public:
	cAcceptThread();
	virtual ~cAcceptThread();

	void	Init();
	void	Destroy();

protected:
	virtual void Process();
};



#endif // !defined(AFX_CACCEPTTHREAD_H__91DFC159_6F46_446D_BF50_61F70967AB34__INCLUDED_)
