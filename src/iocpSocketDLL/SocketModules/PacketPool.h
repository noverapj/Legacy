#pragma once

#include <vector>
#include "PacketQueue.h"

class IOCP_SOCKET_API PacketPool
{
public:
	PacketPool(void);
	~PacketPool(void);

	void Init();
	void Destroy();

public:
	void Create(const uint32 seed);

	PacketQueue* Pop(const uint32 size, const BOOL accept=FALSE);
	void Push(PacketQueue* queueElem);

	int GetAcceptorCount() 
	{
		return (m_poolerAccept.GetTotalCount() - m_poolerAccept.GetCount());
	}
	void GetPoolCount(int *usingCount, int *remainCount) 
	{ 
		usingCount[0] = m_pooler64.GetTotalCount() - m_pooler64.GetCount();
		usingCount[1] = m_pooler256.GetTotalCount() - m_pooler256.GetCount();
		usingCount[2] = m_pooler1024.GetTotalCount() - m_pooler1024.GetCount();
		usingCount[3] = m_poolerBig.GetTotalCount() - m_poolerBig.GetCount();

		remainCount[0] = m_pooler64.GetCount();
		remainCount[1] = m_pooler256.GetCount();
		remainCount[2] = m_pooler1024.GetCount();
		remainCount[3] = m_poolerBig.GetCount();
	}

protected:
	MemPooler<PacketQueueType64> m_pooler64;
	MemPooler<PacketQueueType256> m_pooler256;
	MemPooler<PacketQueueType1024> m_pooler1024;
	MemPooler<PacketQueueTypeBig> m_poolerBig;
	MemPooler<PacketQueueTypeAccept> m_poolerAccept;


};
