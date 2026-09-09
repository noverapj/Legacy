#pragma once

#include <windows.h>
#include <vector>
#include <list>
#include <mutex>
#include "PacketDecoder.h"

#define STORE_MAX_PACKETS 200000

class PacketStore
{
public:
	PacketStore() = default;
	~PacketStore() = default;

	void PushDecoded(const DecodedPacket& dp)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.size() >= STORE_MAX_PACKETS)
			m_queue.pop_front();
		m_queue.push_back(dp);
	}

	void Drain(std::vector<DecodedPacket>& vOut)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_queue.empty())
			return;
		size_t nOld = vOut.size();
		vOut.resize(nOld + m_queue.size());
		size_t i = nOld;
		for (auto it = m_queue.begin(); it != m_queue.end(); ++it)
			vOut[i++] = *it;
		m_queue.clear();
	}

	static void AssignSeq(std::vector<DecodedPacket>& vAll)
	{
		for (size_t i = 0; i < vAll.size(); i++)
			vAll[i].dwSeq = (DWORD)(i + 1);
	}

	void Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.clear();
	}

	size_t PendingCount()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_queue.size();
	}

private:
	std::list<DecodedPacket> m_queue;
	std::mutex m_mutex;
};
