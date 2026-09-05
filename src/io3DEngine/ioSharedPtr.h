

#ifndef _ioSharedPtr_h_
#define _ioSharedPtr_h_

#include "ioPrerequisites.h"

template< class T > class ioSharedPtr
{
protected:
	T* pRep;
	DWORD *pUseCount;

public:
	inline T& operator*() const { assert(pRep); return *pRep; }
	inline T* operator->() const { assert(pRep); return pRep; }
	inline T* GetPointer() const { return pRep; }

	// Binds rep to the ioSharedPtr
	// Assumes that the SharedPtr is uninitialised!
	void Bind( T *rep )
	{
		assert( !pRep && !pUseCount );

		pUseCount = new DWORD(1);
		pRep = rep;
	}

	inline bool Unique() const { assert( pUseCount ); return *pUseCount == 1; }
	inline DWORD UseCount() const { assert( pUseCount ); return *pUseCount; }
	inline DWORD* UseCountPointer() const { return pUseCount; }

	inline bool IsNull() const { return pRep == NULL; }
	inline void SetNull()
	{
		if( pRep )
		{
			Release();
			pRep = NULL;
			pUseCount = NULL;
		}
	}

public:
	ioSharedPtr& operator=( const ioSharedPtr &rhs )
	{
		if( pRep == rhs.pRep )
			return *this;

		Release();

		pRep = rhs.pRep;
		pUseCount = rhs.pUseCount;
		if( pUseCount )
		{
			++(*pUseCount);
		}

		return *this;
	}

protected:
	inline void Release()
	{
		if( pUseCount )
		{
			if( --(*pUseCount) == 0 )
			{
				Destroy();
			}
		}
	}

	virtual void Destroy()
	{
		delete pRep;
		delete pUseCount;
	}

public:
	/* Constructor, does not initialise the SharedPtr.
	   Dangerous! You have to call bind() before using the SharedPtr. */
	ioSharedPtr() : pRep(NULL), pUseCount(NULL){}
	explicit ioSharedPtr( T *rep ) : pRep( rep ), pUseCount( new DWORD(1) ){}

	ioSharedPtr( const ioSharedPtr &rhs )
	{
		pRep = rhs.pRep;
		pUseCount = rhs.pUseCount;
		if( pUseCount )
		{
			++( *pUseCount );
		}
	}

	virtual ~ioSharedPtr()
	{
		Release();
	}
};

template< class T, class U >
inline bool operator==( ioSharedPtr<T> const &a, ioSharedPtr<U> const &b )
{
	return a.GetPointer() == b.GetPointer();
}

template< class T, class U >
inline bool operator !=( ioSharedPtr<T> const &a, ioSharedPtr<U> const &b )
{
	return a.GetPointer() != b.GetPointer();
}

#endif
