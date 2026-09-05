
#include "stdafx.h"
#include "ioHashString.h"

ioHashString::ioHashString()
{
	m_iCapacity = DEFAULT_CAPACITY;
	m_pString = new char[ m_iCapacity ];

	Clear();

	m_dwHashCode = CalcHashCode( m_pString );
}

ioHashString::ioHashString( const char* str ) : m_pString(NULL)
{
	m_iLength = strlen( str );

	if( m_iLength <= MAX_CAPACITY )
	{
		int iCapacity = max( m_iLength + 1, DEFAULT_CAPACITY );
		ReAllocCapacity( iCapacity );

		strcpy( m_pString, str );
		m_dwHashCode = CalcHashCode( str );
	}
	else
	{
		Clear();
	}
}

ioHashString::ioHashString( const ioHashString &rhs ) : m_pString(NULL)
{
	m_iLength = rhs.Length();

	if( m_iLength <= MAX_CAPACITY )
	{
		ReAllocCapacity( rhs.m_iCapacity );

		strcpy( m_pString, rhs.m_pString );
		m_dwHashCode = rhs.m_dwHashCode;
	}
	else
	{
		Clear();
	}
}

ioHashString::~ioHashString()
{
	SAFEDELETEARRAY( m_pString );
}

void ioHashString::Clear()
{
	memset( m_pString, 0, m_iCapacity );
	m_iLength = 0;
}

void ioHashString::ReAllocCapacity( int iNewCapacity )
{
	SAFEDELETEARRAY( m_pString );

	m_iCapacity = iNewCapacity;
	m_pString = new char[iNewCapacity];
}

DWORD ioHashString::CalcHashCode( const char *str )
{
	DWORD dwLen, dwCh, dwResult;

	dwLen    = strlen( str );
	dwResult = HASH_CONST;

	for( DWORD i=0; i<dwLen ; i++ )
	{
		dwCh     = (DWORD)str[i];
		dwResult = ((dwResult<< 5) + dwResult) + dwCh; // hash * 33 + ch
	}

	return dwResult;
}

char ioHashString::At( int i ) const
{
	if( m_iLength < i || !m_pString )
	{
		assert( false );
		return 0;
	}

	return m_pString[i];
}

ioHashString& ioHashString::operator=( const ioHashString &rhs )
{
	m_iLength = rhs.Length();

	if( m_iLength <= MAX_CAPACITY )
	{
		if( m_iCapacity < m_iLength + 1 )
		{
			ReAllocCapacity( m_iLength + 1 );
		}

		strcpy(m_pString, rhs.m_pString );
		m_dwHashCode = rhs.m_dwHashCode;
	}
	else
	{
		Clear();
	}

	return *this;
}

ioHashString& ioHashString::operator=( const char *szText )
{
	m_iLength = strlen( szText );

	if( m_iLength <= MAX_CAPACITY )
	{
		if( m_iCapacity < m_iLength + 1 )
		{
			ReAllocCapacity( m_iLength + 1 );
		}

		strcpy( m_pString, szText );
		m_dwHashCode = CalcHashCode( szText );
	}
	else
	{
		Clear();
	}

	return *this;
}
ioHashString& ioHashString::operator+( const ioHashString &rhs )
{
	int iLength = m_iLength;
	m_iLength += rhs.Length();

	if( m_iLength <= MAX_CAPACITY )
	{
		char* temp = m_pString;
		if( m_iCapacity < m_iLength + 1 )
		{
			m_iCapacity = m_iLength+1;
			m_pString = new char[m_iCapacity];
			if( iLength > 0)
				strcpy( m_pString, temp );

			SAFEDELETEARRAY(temp);
		}
		strcpy(m_pString+iLength, rhs.m_pString );

		m_dwHashCode = CalcHashCode( m_pString );
	}
	else
	{
		Clear();
	}

	return *this;
}

ioHashString& ioHashString::operator+=( const ioHashString &rhs )
{
	if( m_iLength + rhs.Length() > MAX_CAPACITY )
	{
		Clear();

		return *this;
	}

	return *this + rhs;
}
