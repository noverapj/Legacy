#include "stdafx.h"
#include "cIocpContext.h"
#include "cPooler.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cPooler::cPooler(void)
{
	Init();
}

cPooler::~cPooler(void)
{
	Destory();
}

void cPooler::Init()
{
}

void cPooler::Destory()
{
	m_contextPool.DestroyPool();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

void cPooler::Create( int32 maximum )
{
	m_contextPool.CreatePool( maximum, maximum );
}

cIocpContext* cPooler::Pop()
{
	return m_contextPool.Remove();
}

void cPooler::Push( cIocpContext* context )
{
	m_contextPool.Push( context );
}

