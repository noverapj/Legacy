#pragma once


class LSPacketQueue;
class OpMemPool;

class LSTimerMgr
{
public:
	LSTimerMgr(void)
	{
		m_timers.reserve(100);
		m_queue = NULL;
	};

	virtual ~LSTimerMgr(void)
	{
	};

	void SetQueue(LSPacketQueue* queue)
	{
		m_queue = queue;
	}

	void SetPool(OpMemPool* pool)
	{
		m_pool = pool;
	}

	template<typename T>
	void AddTimer(bool repeatState,int millSecond,T* buffer,DWORD repeatCount = INFINITE)
	{
		if(m_pool == NULL && m_queue == NULL)
		{
			LOG.PrintTimeAndLog(0,"Timer Error MemoryPool");
			return;
		}
 
		new LSTimer<T>(repeatState,millSecond,buffer,repeatCount,m_queue,m_pool); //lhf쓰면 상관없어짐 
	}

	template<typename T>
	void DelTimer(LSTimer<T>* timer) //이게 필요없음
	{
		for(int i=0; i<m_timers.size(); ++i)
		{
			if(timer == m_timers[i])
			{
				m_timers.erase(m_timers.begin()+i);
				break;
			}
		}
	}

protected:
	LSPacketQueue* m_queue;
	OpMemPool* m_pool;
	std::vector<void*> m_timers;
};

