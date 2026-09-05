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

cBuffer::cBuffer( uint32 uBuffLen )
{
	Init();
	Create( uBuffLen );
}

cBuffer::~cBuffer()
{
	Destroy();
}

void	cBuffer::Init()
{
	m_pDT		= NULL;
	m_unLen		= 0;
	m_unMax		= 0;
}

void	cBuffer::Destroy()
{
	if( m_pDT )
		delete []m_pDT;

	m_pDT		= NULL;
	m_unLen		= 0;
	m_unMax		= 0;
}

//////////////////////////////////////////////////////////////////////
// Create/Erase
//////////////////////////////////////////////////////////////////////

bool	cBuffer::Create( uint32 uBuffLen )
{
	if( m_pDT )
		Destroy();

	m_pDT = new uint8[ uBuffLen ];
	if( m_pDT )
	{
		m_unLen	= 0;
		m_unMax	= uBuffLen;

		Erase();
		return true;
	}

	return false;
}

bool	cBuffer::Erase()
{
	if( m_pDT )
	{
		ZeroMemory( m_pDT, m_unLen );
		m_unLen = 0;
		return true;
	}
	return false;
}

void	cBuffer::Resize( uint32 uBuffLen )
{
	if(m_unMax < uBuffLen)
	{
		Create(uBuffLen);
	}
	else
	{
		Erase();
	}
}

//////////////////////////////////////////////////////////////////////
// Copy/Append
//////////////////////////////////////////////////////////////////////

bool	cBuffer::Copy( const char* pDT )
{
	if( m_pDT )
	{
		uint32 unLen = strlen( pDT ) + 1;
		if( unLen <= m_unMax )
		{
			Erase();

			m_unLen	= unLen;
			CopyMemory( m_pDT, pDT, m_unLen );
			return true;
		}
	}
	return false;
}

bool	cBuffer::Copy( uint32 unDst, const char* pDT )
{
	if( m_pDT )
	{
		uint32 unLen = strlen( pDT ) + 1;
		if( (unDst+unLen) <= m_unMax )
		{
			CopyMemory( m_pDT+unDst, pDT, unLen );
			m_unLen	= unDst + unLen;
			return true;
		}
	}
	return false;
}

bool	cBuffer::Copy( const uint8* pDT, uint32 unLen )
{
	if( m_pDT )
	{
		if( unLen <= m_unMax )
		{
			Erase();

			m_unLen	= unLen;
			CopyMemory( m_pDT, pDT, m_unLen );
			return true;
		}
	}
	return false;
}

bool	cBuffer::Copy( uint32 unDst, const uint8* pDT, uint32 unLen )
{
	if( m_pDT )
	{
		if( (unDst+unLen) <= m_unMax )
		{
			CopyMemory( m_pDT + unDst, pDT, unLen );
			m_unLen	= unDst + unLen;
			return true;
		}
	}
	return false;
}


bool	cBuffer::Append( const char pDT )
{
	if( m_pDT )
	{
		if( (sizeof(char)+m_unLen) <= m_unMax )
		{
			m_pDT[m_unLen] = pDT;
			m_unLen++;
			return true;
		}
	}
	return false;
}

bool	cBuffer::Append( const char* pDT )
{
	if( m_pDT )
	{
		uint32 unLen = strlen( pDT ) + 1;
		if( (unLen+m_unLen) <= m_unMax )
		{
			CopyMemory( m_pDT+m_unLen, pDT, unLen );
			m_unLen	+= unLen;
			return true;
		}
	}
	return false;
}


bool	cBuffer::Append( const uint8* pDT, uint32 unLen )
{
	if( m_pDT )
	{
		if( (unLen+m_unLen) <= m_unMax )
		{
			CopyMemory( m_pDT + m_unLen, pDT, unLen );
			m_unLen	+= unLen;
			return true;
		}
	}
	return false;
}

