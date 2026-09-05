#pragma once

#include "../include/MemPooler.h"

class cIocpContext;

class cPooler
{
public:
	cPooler(void);
	~cPooler(void);

	void Init();
	void Destory();

public:
	void Create( int32 maximum );

	int32 GetCount()	{ return m_contextPool.GetCount(); }

	cIocpContext* Pop();
	void Push( cIocpContext* context );

private:
	MemPooler<cIocpContext>	m_contextPool;
};
