// cBuffer.cpp: implementation of the cBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cBuffer.h"
#include <iostream>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cBuffer::cBuffer()
{
	Init();
}

cBuffer::cBuffer( uint32 buffLen )
{
	Init();
	Create( buffLen );
}

cBuffer::~cBuffer()
{
	Destroy();
}

void cBuffer::Init()
{
	m_pDT		= NULL;
	m_length		= 0;
	m_max		= 0;
}

void cBuffer::Destroy()
{
	if( m_pDT )
		delete []m_pDT;

	m_pDT		= NULL;
	m_length		= 0;
	m_max		= 0;
}

//////////////////////////////////////////////////////////////////////
// Create/Erase
//////////////////////////////////////////////////////////////////////

bool cBuffer::Create( uint32 buffLen )
{
	if( m_pDT )
		Destroy();

	m_pDT = new uint8[ buffLen ];
	if( m_pDT )
	{
		m_length	= 0;
		m_max	= buffLen;

		Erase();
		return true;
	}

	return false;
}

bool cBuffer::Erase()
{
	if( m_pDT )
	{
		ZeroMemory( m_pDT, m_length );
		m_length = 0;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// Copy/Append
//////////////////////////////////////////////////////////////////////

bool cBuffer::Copy( const TCHAR* pDT )
{
	if( m_pDT )
	{
		uint32 length = (_tcslen( pDT ) + 1) * sizeof(TCHAR);
		if( length <= m_max )
		{
			Erase();

			m_length	= length;
			CopyMemory( m_pDT, pDT, m_length );
			return true;
		}
	}
	return false;
}

bool cBuffer::Copy( uint32 unDst, const TCHAR* pDT )
{
	if( m_pDT )
	{
		uint32 length = (_tcslen( pDT ) + 1) * sizeof(TCHAR);
		if( (unDst+length) <= m_max )
		{
			CopyMemory( m_pDT+unDst, pDT, length );
			m_length	= unDst + length;
			return true;
		}
	}
	return false;
}

bool cBuffer::Copy( const uint8* pDT, uint32 length )
{
	if( m_pDT )
	{
		if( length <= m_max )
		{
			Erase();

			m_length	= length;
			CopyMemory( m_pDT, pDT, m_length );
			return true;
		}
	}
	return false;
}

bool cBuffer::Copy( uint32 unDst, const uint8* pDT, uint32 length )
{
	if( m_pDT )
	{
		if( (unDst+length) <= m_max )
		{
			CopyMemory( m_pDT + unDst, pDT, length );
			m_length	= unDst + length;
			return true;
		}
	}
	return false;
}


bool cBuffer::Append( const char pDT )
{
	if( m_pDT )
	{
		if( (sizeof(char)+m_length) <= m_max )
		{
			m_pDT[m_length] = pDT;
			m_length++;
			return true;
		}
	}
	return false;
}

bool cBuffer::Append( const char* pDT )
{
	if( m_pDT )
	{
		uint32 length = strlen( pDT ) + 1;
		if( (length+m_length) <= m_max )
		{
			CopyMemory( m_pDT+m_length, pDT, length );
			m_length	+= length;
			return true;
		}
	}
	return false;
}


bool cBuffer::Append( const uint8* pDT, uint32 length )
{
	if( m_pDT )
	{
		if( (length+m_length) <= m_max )
		{
			CopyMemory( m_pDT + m_length, pDT, length );
			m_length	+= length;
			return true;
		}
	}
	return false;
}

bool cBuffer::Append( const char* pDT, uint32 length )
{
	if( m_pDT )
	{
		if( (length+m_length) <= m_max )
		{
			CopyMemory( m_pDT + m_length, pDT, length );
			m_length	+= length;
			return true;
		}
	}
	return false;
}