#pragma once
#pragma once
//템플릿 전용 이경우에는 부스트라이브러리가 있는 pc에서만 컴파일됨 
 

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/list.hpp>
#include <boost/exception/all.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

template <typename T>
class TemplateSharedList
{
private:
	typedef boost::interprocess::allocator<T,boost::interprocess::managed_shared_memory::segment_manager> UserAllocator;
	typedef boost::interprocess::list<T,UserAllocator> UserList;

public:
	TemplateSharedList()
	{
		m_state = false;
	}
	TemplateSharedList(TCHAR* name,unsigned int ncount = 10,bool processMutexState = false)
	{
		Set(name,ncount);	

	};
	virtual void Set(TCHAR* name,unsigned int ncount = 10,bool processMutexState = false)
	{
		try
		{
			m_sharedmemoryName = name;
			m_maxCount = ncount;	
			m_state = true;
			m_containerName = name;
			m_containerName += _T("list");
			m_processMutexName = name;
			m_processMutexName += _T("processmutex");
			m_processMutexState = processMutexState;
			m_maxMemorySize = sizeof(T)*ncount+1024;
			if(!CanCreate())
				throw "VM is too small";
			m_sharedMemoryManager = 
				new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,name,m_maxMemorySize);//최소 기본크기
			m_allocator = new UserAllocator(m_sharedMemoryManager->get_segment_manager());

			m_lists = m_sharedMemoryManager->find_or_construct<UserList>(_T(m_containerName.c_str()))(*m_allocator);
			m_lock = m_sharedMemoryManager->find_or_construct<boost::interprocess::interprocess_mutex>(_T(m_processMutexName.c_str()))(); 
		}
		catch(tstring e)
		{
			m_lastError = e;
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}
	}
	bool CanCreate( unsigned int size )
	{
		int state = 0;
		try
		{
			state = 1;
			auto shared = boost::interprocess::managed_shared_memory(boost::interprocess::open_only,m_sharedmemoryName.c_str()); 
		}
		catch(std::exception &e)
		{
			state = -1; //생성된거 없을때 
			m_lastError = e.what();
		}

		if(state == -1)
		{
			MEMORYSTATUSEX stmem;
			ZeroMemory(&stmem,sizeof(MEMORYSTATUSEX));
			stmem.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&stmem);
			unsigned int freevirsize  = stmem.ullAvailVirtual;
			freevirsize /= 2;
			if(size > freevirsize)
				return false;
			return true;
		}
		return true;
	}
	bool CanPush()
	{
		try
		{
			int freesize = m_sharedMemoryManager->get_free_memory();
			if(freesize <= sizeof(T))
			{
				bool rtval =
					m_sharedMemoryManager->grow(m_sharedmemoryName.c_str(),max(sizeof(T)*(m_maxCount/10),1024));
				delete m_sharedMemoryManager;
				delete m_allocator;

				m_sharedMemoryManager = 
					new boost::interprocess::managed_shared_memory(boost::interprocess::open_only,m_sharedmemoryName.c_str());//최소 기본크기
				m_allocator = new UserAllocator(m_sharedMemoryManager->get_segment_manager());
				m_lists = m_sharedMemoryManager->find_or_construct<UserList>(_T(m_containerName.c_str()))(*m_allocator);
				m_lock = m_sharedMemoryManager->find_or_construct<boost::interprocess::interprocess_mutex>(_T(m_processMutexName.c_str()))(); 
			}
			return true;
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}

		return false;
	}
	void clear()
	{
		if(!m_state) return;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		m_lists->clear();
	}
	typename UserList::const_iterator end()
	{
		UserList::const_iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->end();
	}
	typename UserList::iterator begin()
	{
		UserList::iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->begin();
	}
	typename UserList::const_iterator rend()
	{
		UserList::const_iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->rend();
	}
	typename UserList::iterator rbegin()
	{
		UserList::iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_list->rbegin();
	}
	typename UserList::iterator erase(typename UserList::iterator iter)
	{
		UserList::iterator riter;
		if(!m_state) return riter;
		try
		{
			if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
			return m_lists->erase(iter);
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}
		return riter;
	}
	void push_back(T& x)
	{
		try
		{
			if(!m_state) return;
			if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
			if(!CanPush())
			{
				tstring errorString = "Error grow opertaion fail";
				throw errorString;
 				return;
			}
			int b = m_sharedMemoryManager->get_free_memory();
			m_lists->push_back(x);
		}
		catch(tstring& e)
		{
			m_lastError = e;
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}
	}
	void push_front(T& x)
	{
		try
		{
			if(!m_state) return;
			if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
			if(!CanPush())
				return;
			m_lists->push_front(x);
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}
	}
	unsigned int size()
	{
		if(!m_state) return NULL;
		return m_lists->size();
	}
	bool empty()
	{
		if(!m_state) return false;
		return m_lists->empty();	
	}
	typename UserList::reference front()
	{
		if(!m_state) return NULL;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->front();

	}
	typename UserList::reference back()
	{
		if(!m_state) return NULL;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->back();
	}
	void sort()
	{
		if(!m_state) return;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		m_lists->sort();
	}
	void pop_back()
	{
		if(!m_state) return;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		m_lists->pop_back();
	}
	void pop_front()
	{
		if(!m_state) return;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		m_lists->pop_front();
	}
	bool Remove(TCHAR* name)
	{
		return boost::interprocess::shared_memory_object::remove(m_sharedmemoryName.c_str());
	}
	bool Remove()
	{
		m_sharedMemoryManager->destroy<UserList>(m_containerName.c_str());
		boost::interprocess::named_mutex::remove(m_processMutexName.c_str());
		return boost::interprocess::shared_memory_object::remove(m_sharedmemoryName.c_str());
	}
protected:

protected:
	UserAllocator *m_allocator;
	UserList* m_lists;
	boost::interprocess::managed_shared_memory * m_sharedMemoryManager;
	bool m_state;
	bool m_processMutexState;
	tstring m_lastError;
	tstring m_processMutexName;
	tstring m_containerName;
	tstring m_sharedmemoryName;
	boost::interprocess::interprocess_mutex *m_lock;
	int m_maxCount;
	int m_maxMemorySize;
};