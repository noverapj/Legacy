#include "StdAfx.h"
#include "OpMemPool.h"
 

OpMemPool::OpMemPool(void)
{
}

OpMemPool::~OpMemPool(void)
{
	m_block32::release_memory();
	m_block64::release_memory();
	m_block128::release_memory();
	m_block256::release_memory();
	m_block512::release_memory();
	m_block1024::release_memory();
	m_block2048::release_memory();
	m_block4096::release_memory();
}

void OpMemPool::Init()
{
	 
}

void OpMemPool::Increment()
{
	g_State()->IncrementOPPool();
}

void OpMemPool::Decrement()
{
	g_State()->DecrementOPPool();
}