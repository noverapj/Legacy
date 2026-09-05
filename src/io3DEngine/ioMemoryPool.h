

#ifndef _ioMemoryPool_h_
#define _ioMemoryPool_h_

#include <vector>
#include <algorithm>
#include <assert.h>

template<typename T, int PRESERVE_SIZE=0>
class ioMemoryPool
{
protected:
	std::vector<T*> m_vec_pkData;
	std::vector<T*> m_vec_pkFree;

	unsigned int m_uUsedPreserve;
	unsigned int m_uMaxPreserve;
    unsigned int m_uInitCapacity;
    unsigned int m_uUsedCapacity;
    int m_iRestAllocCount;

	char* m_akPreserve;

    bool m_bEnableStrictFreeCheck;

public:
	void SetStrictFreeCheck( bool bEnable )
	{
		m_bEnableStrictFreeCheck = bEnable;
	}

	void Destroy()
	{
		assert( 0 == m_iRestAllocCount );

		if( m_akPreserve )
		{
			delete [] m_akPreserve;
			m_akPreserve = NULL;

			m_uUsedCapacity = 0;
		}

		std::for_each( m_vec_pkData.begin(), m_vec_pkData.end(), __DeleteData );
		m_vec_pkData.clear();
		m_vec_pkFree.clear();
	}

	void InitCapacity( unsigned int uCapacity )
	{
		m_uInitCapacity = uCapacity;

		m_vec_pkData.reserve( uCapacity );
		m_vec_pkFree.reserve( uCapacity );
	}

	void InitPreserve( unsigned int uPreserve )
	{
		m_uMaxPreserve  = uPreserve;
		m_uUsedPreserve = 0;

		if( m_uMaxPreserve > 0 )
		{
			if( m_akPreserve )
			{
				delete [] m_akPreserve;
			}

			m_akPreserve = new char[ sizeof(T) * m_uMaxPreserve ];
		}
	}

	T* Alloc()
	{
#ifdef _DEBUG
		++m_iRestAllocCount;
#endif

		if( m_vec_pkFree.empty() )
		{
			T* pkNewData = NULL;

			if( m_uUsedPreserve < m_uMaxPreserve )
			{
				pkNewData = ((T*)m_akPreserve) + (m_uUsedPreserve++);
			}
			else
			{
				pkNewData = __NewData();
				m_vec_pkData.push_back( pkNewData );
			}

			++m_uUsedCapacity;
			return pkNewData;
		}

		T* pkFreeData = m_vec_pkFree.back();
		m_vec_pkFree.pop_back();
		return pkFreeData;
	}

	void Free( T* pkData )
	{
#ifdef _DEBUG
		--m_iRestAllocCount;

		if( m_bEnableStrictFreeCheck )
		{
			assert( __IsValidData(pkData) );
			assert( !__IsFreeData(pkData) );
		}
#endif
		m_vec_pkFree.push_back(pkData);
	}

	DWORD GetAllocCount()
	{
		return m_vec_pkData.size() + m_uUsedPreserve;
	}

protected:
	bool __IsValidData( T* pkData )
	{
		if (pkData >= ((T*)m_akPreserve) && pkData < ((T*)m_akPreserve) + m_uMaxPreserve)
			return true;

		if (m_vec_pkData.end() == std::find(m_vec_pkData.begin(), m_vec_pkData.end(), pkData))
			return false;

		return true;
	}

	bool __IsFreeData(T* pkData)
	{
		if (m_vec_pkFree.end() == std::find(m_vec_pkFree.begin(), m_vec_pkFree.end(), pkData))
			return false;

		return true;
	}

	static T* __NewData()
	{
		return (T*)::operator new(sizeof(T));
	}

	static void __DeleteData(T* pkData)
	{
		::operator delete(pkData);
	}

public:
	ioMemoryPool()
	{
		m_uUsedPreserve = 0;
		m_uMaxPreserve  = 0;
		m_uInitCapacity = 0;
		m_uUsedCapacity = 0;

		m_iRestAllocCount = 0;

		m_akPreserve = NULL;

		if( PRESERVE_SIZE > 0 )
		{
			InitPreserve( PRESERVE_SIZE );
		}

		m_bEnableStrictFreeCheck = false;
	}

	~ioMemoryPool()
	{
		Destroy();
	}
};

template < class T, int PRESERVE_SIZE=0 >
class ioPoolObject
{
public:
	typedef ioMemoryPool<T, PRESERVE_SIZE> SELF_TYPE;

protected:
	static SELF_TYPE m_sPool;

public:
	void* operator new( unsigned int mem_size )
	{
		return m_sPool.Alloc();
	}

	void operator delete( void *pT )
	{
		m_sPool.Free( (T*)pT );
	}

	static SELF_TYPE& GetSharedPool()
	{
		return m_sPool;
	}

public:
	ioPoolObject()
	{
	}

	virtual ~ioPoolObject()
	{
	}
};

template< class T, int PRESERVE_SIZE >
ioMemoryPool<T, PRESERVE_SIZE> ioPoolObject<T, PRESERVE_SIZE >::m_sPool;

#endif