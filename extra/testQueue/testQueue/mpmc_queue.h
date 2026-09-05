/*  Multi-producer/multi-consumer queue
 *  2009, Dmitriy V'yukov
 *  Distributed under the terms of the GNU General Public License
 *  as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *  See: http://www.gnu.org/licenses
 */
#include <assert.h>
#pragma once
 


class mpmc_queue
{
public:
    struct node_t
    {
	
		node_t* volatile        next_;
		 
    };

    mpmc_queue()
    {
        head_.ptr_ = 0;
        head_.cnt_ = 0;
        tail_ = &head_.ptr_;
		m_size = 0;
    }

    ~mpmc_queue()
    {
        assert(head_.ptr_ == 0);
        assert(tail_ == &head_.ptr_);
    }
 
	long size()
	{
		return m_size;
	}
	bool empty()
	{
		if(m_size == 0)
			return true;
		return false;
	}
    void enqueue(node_t* node)
    {
		
        assert(node);
        node->next_ = 0;
        node_t** prev = (node_t**)
            _InterlockedExchange((long*)&tail_, (long)node);
        assert(prev);
        // <--- the window of inconsistency is HERE (***)
        prev[0] = node;
		InterlockedIncrement(&m_size);
    }

    node_t* dequeue()
    {
        unsigned retry_count = 0;
		InterlockedDecrement(&m_size);
        retry:
        __try
        {
            head_t h;
            h.ptr_= head_.ptr_;
            h.cnt_ = head_.cnt_;
            for (;;)
            {
                node_t* n = h.ptr_;
                if (n == 0)
                    return 0;
                if (n->next_)
                {
                    head_t xchg = {n->next_, h.cnt_ + 1};
                    __int64 prev_raw = 
                        _InterlockedCompareExchange64
                            (&head_.whole_, xchg.whole_, h.whole_);
                    head_t prev = *(head_t*)&prev_raw;
                    if (*(__int64*)&prev == *(__int64*)&h)
                        return n;
                    h.ptr_ = prev.ptr_;
                    h.cnt_ = prev.cnt_;
                }
                else
                {
                    node_t* t = (node_t*)tail_;
                    if (n != t)
                    {
                        // spinning here may only be caused
                        // by producer preempted in (***)
                        SwitchToThread();
                        h.ptr_= head_.ptr_;
                        h.cnt_ = head_.cnt_;
                        continue;
                    }
                    head_t xchg = {0, h.cnt_ + 1};
                    head_t prev;
                    prev.whole_ = _InterlockedCompareExchange64
                        (&head_.whole_, xchg.whole_, h.whole_);
                    if (prev.whole_ == h.whole_)
                    {
                        node_t* prev_tail = (node_t*)
                            _InterlockedCompareExchange
                            ((long*)&tail_, (long)&head_.ptr_, (long)n);
                        if (prev_tail == n)
                            return n;
                        // spinning here may only be caused
                        // by producer preempted in (***)
                        while (n->next_ == 0)
                            SwitchToThread();
                        head_.ptr_ = n->next_;
                        return n;
                    }
                    h.ptr_ = prev.ptr_;
                    h.cnt_ = prev.cnt_;
                }
            }
        }
        __except ((GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
                && ++retry_count < 64*1024) ?
                EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
        {
            goto retry;
        }
    }

private:
    union head_t
    {
        struct
        {
            node_t*             ptr_;
            unsigned            cnt_;
        };
        __int64                 whole_;
    };

    head_t volatile             head_;
    char                        pad_ [64];
    node_t* volatile* volatile  tail_;

    mpmc_queue(mpmc_queue const&);
    mpmc_queue& operator = (mpmc_queue const&);
	long m_size;
};

