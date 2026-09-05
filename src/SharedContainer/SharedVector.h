#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/exception/all.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
 

 
class SharedVector
{
private:
	typedef boost::interprocess::allocator<int,boost::interprocess::managed_shared_memory::segment_manager> UserAllocator;
	typedef boost::interprocess::vector<int,UserAllocator> UserVector;

public:
	SharedVector()
	{
		m_state = false;
		m_sharedMemoryManager = NULL;
	}
	SharedVector(TCHAR* name,unsigned int count,int size,bool processMutexState = false)
	{
		m_sharedMemoryManager = NULL;
		m_size = size; 
		Set(name,count,size);	
		
	};
	bool CanCreate( unsigned int size )
	{
		int state = 0;
		try
		{
			state = 1;
			auto shared = boost::interprocess::managed_shared_memory(boost::interprocess::open_only,m_sharedmemoryName.c_str());//벡터는 어느정도 모자르면 2배씩잡기에 더 많이 잡아줌 
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
	
	virtual bool Set(TCHAR* name,unsigned int count,int size,bool processMutexState = false)
	{
		try
		{
			m_size = size;
			m_sharedmemoryName = name;
			m_maxCount = count;	
			m_state = true;
			m_containerName = name;
			m_containerName += _T("vector");
			m_processMutexName = name;
			m_processMutexName += _T("processmutex");
			m_processMutexState = processMutexState;
			m_maxMemorySize = (((size*count*3)+(count*sizeof(int)*2*3)+1024));
			 if(CanCreate(m_maxMemorySize) == false)
					throw "Vitual memory is small";
			 			
			m_sharedMemoryManager = 
				new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,name,m_maxMemorySize);//벡터는 어느정도 모자르면 2배씩잡기에 더 많이 잡아줌 
			m_allocator = new UserAllocator(m_sharedMemoryManager->get_segment_manager());
			m_lists = m_sharedMemoryManager->find_or_construct<UserVector>(_T(m_containerName.c_str()))(*m_allocator);
			m_lock = m_sharedMemoryManager->find_or_construct<boost::interprocess::interprocess_mutex>(_T(m_processMutexName.c_str()))(); 
		}
		catch(char* errstring)
		{
			m_lastError = errstring;
			return false;
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
			return false;
		}
		return true;
	}
	bool CanPush()
	{
		try
		{
			if(m_sharedMemoryManager == NULL)
				return false;
			int freesize = m_sharedMemoryManager->get_free_memory();
			if(freesize <= m_size+200)
			{
				bool rtval = false;
				  m_sharedMemoryManager->grow(_T(m_sharedmemoryName.c_str()),m_size * (m_maxCount/10));
				delete m_sharedMemoryManager;
				delete m_allocator;

				m_sharedMemoryManager = 
					new boost::interprocess::managed_shared_memory(boost::interprocess::open_only,_T(m_sharedmemoryName.c_str()));//최소 기본크기
				m_allocator = new UserAllocator(m_sharedMemoryManager->get_segment_manager());
				m_lists = m_sharedMemoryManager->find_or_construct<UserVector>(_T(m_containerName.c_str()))(*m_allocator);
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
	UserVector::const_iterator end()
	{
		UserVector::const_iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->end();
	}
	UserVector::iterator begin()
	{
		UserVector::iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->begin();
	}
	UserVector::reverse_iterator rend()
	{
		UserVector::reverse_iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->rend();
	}
	UserVector::reverse_iterator rbegin()
	{
		UserVector::reverse_iterator iter;
		if(!m_state) return iter;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->rbegin();
	}
	UserVector::iterator erase(UserVector::iterator iter)
	{
		UserVector::iterator riter;
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
	int GetBuf(int size,int index)  
	{
		TCHAR stmp[30];
		sprintf_s(stmp,"%s%d",m_containerName.c_str(),index);
		if(!CanPush())
			return NULL;
		if(!m_state) return NULL;
		int rtval  = reinterpret_cast<int>(m_sharedMemoryManager->find_or_construct<char>(stmp)[size+1](0));
		return rtval;
	}
	void push_back(int& x)
	{
		try
		{
			if(!m_state) return;
			if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
			if(!CanPush())
			{
				return;
			}
			m_lists->push_back(x);
		}
		catch(boost::exception_detail::bad_alloc_ &e)
		{
			m_lastError = e.what();
		}
	}
	int GetAt(unsigned int index)
	{
		if(index >= m_lists->size())
			return NULL;
	 
		int rtval = m_lists->at(index);
		return rtval;
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
	UserVector::reference front()
	{
		int rtval = NULL;
		if(!m_state) return rtval;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->front();

	}
	UserVector::reference back()
	{
		int rtval = NULL;
		if(!m_state) return rtval;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		return m_lists->back();
	}
 
	void pop_back()
	{
		if(!m_state) return;
		if(m_processMutexState) boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*m_lock); 
		m_lists->pop_back();
	}
	bool flush()
	{
		Remove();
		m_sharedMemoryManager = 
			new boost::interprocess::managed_shared_memory(
					boost::interprocess::open_or_create,
					m_sharedmemoryName.c_str(),
					m_maxMemorySize);//최소 기본크기
		m_allocator = new UserAllocator(m_sharedMemoryManager->get_segment_manager());
		m_lists = m_sharedMemoryManager->find_or_construct<UserVector>(_T(m_containerName.c_str()))(*m_allocator);
		m_lock = m_sharedMemoryManager->find_or_construct<boost::interprocess::interprocess_mutex>(_T(m_processMutexName.c_str()))(); 
		return true;
	}

	bool Remove(TCHAR* name)
	{
		return boost::interprocess::shared_memory_object::remove(m_sharedmemoryName.c_str());
	}

	bool Remove()
	{
		m_sharedMemoryManager->destroy<UserVector>(m_containerName.c_str());
		boost::interprocess::named_mutex::remove(m_processMutexName.c_str());
		delete m_allocator;
		bool rtval = boost::interprocess::shared_memory_object::remove(m_sharedmemoryName.c_str());
		delete m_sharedMemoryManager;
		return rtval;
	}

	bool SetVectorAddress() //뒤바뀐 주소 초기화 해줌 
	{
		TCHAR stmp[30];
		TCHAR nametmp[20];
		sprintf_s(nametmp,"%s",m_containerName.c_str());
		
		for(unsigned int i=0;i<m_lists->size();++i)
		{
			
			sprintf_s(stmp,"%s%d",nametmp,i);
			auto pairdata = m_sharedMemoryManager->find<int>(stmp);
			(*m_lists)[i] = reinterpret_cast<int>(pairdata.first);
		}
		return true;
	}

	int GetFreeMemorySize()
	{
		return m_sharedMemoryManager->get_free_memory();
	}
 

protected:
	UserAllocator *m_allocator;
	UserVector* m_lists;
	boost::interprocess::managed_shared_memory * m_sharedMemoryManager;
	boost::interprocess::interprocess_mutex *m_lock;
	bool m_state;
	bool m_processMutexState;
	tstring m_lastError;
	tstring m_processMutexName;
	tstring m_containerName;
	tstring m_sharedmemoryName;
	int m_maxCount;
	int m_size;
	unsigned int m_maxMemorySize;
};