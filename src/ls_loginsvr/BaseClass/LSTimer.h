#pragma once


#include "UserDefine.h"

class LSPacketQueue;

template<typename T>
class LSTimer
{
public:
	LSTimer() : m_timer(*g_IoServices())
	{
	}

	LSTimer(bool repeatState,int millSecond,T* buffer,DWORD repeatCount,LSPacketQueue* queue,OpMemPool* pool) : m_timer(*S_IO::instance()) //넘겨줄떄 메모리풀로 
	{
		m_repeatCount = repeatCount;
		m_queue = queue;
		m_repeatState = repeatState;
		m_pool = pool;
		m_millSecond = millSecond;
		m_buffer = buffer;

	 	m_timer.expires_from_now(boost::posix_time::millisec(m_millSecond));
		m_timer.async_wait(boost::bind(&LSTimer::TimeOut,this));
	}

	virtual ~LSTimer(void)
	{
		m_timer.cancel();
	}

	void SetQueue(LSPacketQueue* queue)
	{
		m_queue = queue;
	}

	void TimeOut()	
	{
		try
		{	
			CountTime countTime;
			SP2Packet st(EPROTOCOL::ITPK_OPERATIONTYPE);

			countTime.Start();
			st << *m_buffer;

			m_queue->InsertQueue(NULL,st,(PacketQueueTypes)PK_QUEUE_INTERNAL);

			if(m_repeatState)
			{	 
				m_timer.cancel();
				if(m_repeatCount != INFINITE)
				{
					m_repeatCount --;
					if(m_repeatCount <= 0 )
					{
						OnDestroy();
						return;
					}
				}

				int nTime = countTime.End();//시간 보정 

				m_timer.expires_at(m_timer.expires_at() + boost::posix_time::millisec(m_millSecond - nTime));
				m_timer.async_wait(boost::bind(&LSTimer::TimeOut,this));
				
			}
			else
			{
				OnDestroy();
			}	
		}
		catch(boost::system::error_code& e)
		{
			LOG.PrintTimeAndLog(0,"LSTimer Error :%s",e.message().c_str());
		}
	}

	void OnDestroy()
	{
		m_pool->Push(m_buffer);
		m_buffer = NULL;

		delete this;
	}

protected:
	boost::asio::deadline_timer m_timer;
	T* m_buffer;
	bool m_repeatState;
	int m_millSecond;
	LSPacketQueue* m_queue;
	OpMemPool* m_pool;
	DWORD m_repeatCount; 
};

