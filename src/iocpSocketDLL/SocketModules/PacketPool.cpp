#include "../iocpSocketDLL.h"
#include "PacketPool.h"

PacketPool::PacketPool(void)
{
	Init();
}

PacketPool::~PacketPool(void)
{
	Destroy();
}

void PacketPool::Init()
{
}

void PacketPool::Destroy()
{
}

void PacketPool::Create(const uint32 seed)
{
	m_poolerAccept.CreatePool(100, 1000, TRUE);
	m_pooler64.CreatePool(seed*4, 100000, TRUE);	// 256K ~ 6.1M
	m_pooler256.CreatePool(seed*2, 100000, TRUE);	// 512K ~ 24M
	m_pooler1024.CreatePool(seed, 10000, TRUE);		// 1M ~ 10M
	m_poolerBig.CreatePool(10, 1000, TRUE);			// 320K ~ 30M
}

PacketQueue* PacketPool::Pop(const uint32 size, const BOOL accept)
{
	if(accept)
	{
		return m_poolerAccept.Pop();
	}

	if(size <= 64)
	{
		return m_pooler64.Pop();
	}
	else if(size <= 256)
	{
		return m_pooler256.Pop();
	}
	else if(size <= 1024)
	{
		return m_pooler1024.Pop();
	}
	else
	{
		return m_poolerBig.Pop();
	}
}

void PacketPool::Push(PacketQueue* queueElem)
{
	 
	if(queueElem->GetType() == PK_QUEUE_ACCEPT)
	{
		m_poolerAccept.Push(static_cast<PacketQueueTypeAccept*>(queueElem));
		return;
	}
	
	if(queueElem->GetDefaultSize() <= 64)
	{
		m_pooler64.Push(static_cast<PacketQueueType64*>(queueElem));
	}
	else if(queueElem->GetDefaultSize() <= 256)
	{
		m_pooler256.Push(static_cast<PacketQueueType256*>(queueElem));
	}
	else if(queueElem->GetDefaultSize() <= 1024)
	{
		m_pooler1024.Push(static_cast<PacketQueueType1024*>(queueElem));
	}
	else
	{
		m_poolerBig.Push(static_cast<PacketQueueTypeBig*>(queueElem));
	}
}
