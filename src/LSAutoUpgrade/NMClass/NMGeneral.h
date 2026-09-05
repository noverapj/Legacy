#ifndef __NMGENERAL_H_2EFA9787_0C6D_48e0_A447_FAC5A9D35A11__
#define __NMGENERAL_H_2EFA9787_0C6D_48e0_A447_FAC5A9D35A11__

// singleton class

#ifndef _USE_SINGLETON_
#define _USE_SINGLETON_

template< class T_ >
class CSingleton
{
public:
	typedef T_ singletonClass;

protected:
	CSingleton( void ) {};
	virtual ~CSingleton( void ) {};

public:
	static void Init( void )		{ CSingleton< T_ >::GetInstance(); }
	static void Release( void )		{ CSingleton< T_ >::GetInstance( TRUE ); }

	static T_& GetInstance( BOOL bDestory = FALSE ) 
	{
		static volatile LONG	staticStatus = 0;
		static volatile T_*		staticSingletonClassPtr = NULL;
		static BYTE				staticSingletonClassBuf[ sizeof( T_ ) ];

		if ( bDestory )
		{
			while ( staticSingletonClassPtr != NULL )
			{
				if ( ::InterlockedCompareExchange( &staticStatus, 0, 1 ) == 1 )
				{
					T_* pPrevInstance = (T_*) staticSingletonClassPtr;
					staticSingletonClassPtr = NULL;
					delete pPrevInstance;
				}
				else
				{
					::Sleep( 0 );
				}
			}
		}
		else
		{
			while ( staticSingletonClassPtr == NULL )
			{
				if ( ::InterlockedCompareExchange( &staticStatus, 1, 0 ) == 0 )
				{
					::ZeroMemory( staticSingletonClassBuf, sizeof( T_ ) );
					staticSingletonClassPtr = new( staticSingletonClassBuf ) T_;
					return *((T_*) staticSingletonClassPtr);
				}
				else
				{
					::Sleep( 0 );
				}
			}
		}

		return *((T_*) staticSingletonClassPtr);
	}

	// operators
	void*	operator new( size_t sz, void* p )		{ return p; }
	void	operator delete( void* p )				{ ::ZeroMemory( p, sizeof( T_ ) ); }
	void	operator delete( void* c, void* p )		{ ::ZeroMemory( p, sizeof( T_ ) ); }
};

#endif


// simple lock implementation

class CNMCSHolder
{
protected:
	LPCRITICAL_SECTION		m_pCriticalSection;

public:
	CNMCSHolder( LPCRITICAL_SECTION pCriticalSection )
	{
		this->m_pCriticalSection = pCriticalSection;
		::EnterCriticalSection( this->m_pCriticalSection );
	}

	~CNMCSHolder()
	{
		::LeaveCriticalSection( this->m_pCriticalSection );
	}

	operator bool()	{ return true; }
};
#define NMCSLOCK( x )		if ( CNMCSHolder	csholder = CNMCSHolder( &x ) )

// utility macro

#define NMSAFE_STRCOPY( dst, src, len )		\
	{ ::_tcsncpy( dst, src, len );			\
	dst[ len - 1 ] = NULL; }

#define NMSAFE_STRCOPYW( dst, src, len )	\
	{ ::wcsncpy( dst, src, len );			\
	dst[ len - 1 ] = NULL; }

#define NMSAFE_STRCOPYA( dst, src, len )	\
	{ ::strncpy( dst, src, len );			\
	dst[ len - 1 ] = NULL; }

#endif	//	#ifndef __NMGENERAL_H_2EFA9787_0C6D_48e0_A447_FAC5A9D35A11__