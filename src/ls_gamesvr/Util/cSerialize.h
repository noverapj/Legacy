#pragma once

#include <list>
#include <vector>
#include "cBuffer.h"


// 최대버퍼길이
const uint32 G_MAXSTORAGELENGTH = 1024 * 16;

class cSerialize
{
public:
	cSerialize(void)
	{
		m_storage.Create(G_MAXSTORAGELENGTH);

		m_temporary = NULL;
		m_offset = 0;
		m_maxLength = 0;
	}

public:
	void Reset()
	{
		m_storage.Erase();
	}

	uint8* GetBuffer()	{ return m_storage.GetBuffer(); }
	uint32 GetLength()	{ return m_storage.GetLength(); }

public:
	void operator<<(const uint8 value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }
	void operator<<(const uint16 value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }
	void operator<<(const uint32 value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }
	void operator<<(const uint64 value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }
	void operator<<(const float value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }
	void operator<<(const double value)	{ m_storage.Append(reinterpret_cast<const uint8*>(&value), sizeof(value)); }

public:
	void Write( const uint8 value )	{ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const uint16 value ){ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const uint32 value ){ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const uint64 value ){ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const short value ) { m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const float value ) { m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const double value ){ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) );}
	void Write( const int value )	{ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	void Write( const int64 value )	{ m_storage.Append( reinterpret_cast<const uint8*>(&value), sizeof(value) ); }
	
	void Write( const uint8* value, uint32 length, BOOL saveLength = FALSE )
	{
		if( saveLength )
			Write( static_cast<uint16>(length) );
		m_storage.Append( value, length );
	}
	void Write( const char* value, uint32 length, BOOL saveLength = FALSE )
	{
		if( saveLength )
			Write( static_cast<uint16>(length) );
		m_storage.Append( value, length );
	}
	template<typename TD>
	void Write( std::vector<TD>& vcList, BOOL saveLength = TRUE )
	{
		if( saveLength )
			Write( static_cast<uint32>(vcList.size()) );

		for( std::vector<TD>::iterator it = vcList.begin() ; it != vcList.end() ; ++it )
		{
			Write( *it );
		}
	}
	template<typename TD>
	void Write( std::list<TD>& vcList, BOOL saveLength = TRUE )
	{
		if( saveLength )
			Write( static_cast<uint32>(vcList.size()) );

		for( std::list<TD>::iterator it = vcList.begin() ; it != vcList.end() ; ++it )
		{
			Write( *it );
		}
	}
	template<typename TD>
	void Read( const uint8* pBuffer, std::vector<TD>& vcList )
	{
		uint32 nLength = 0;
		CopyMemory( &nLength, pBuffer, sizeof(nLength) );

		vcList.clear();
		vcList.reserve( nLength );

		TD data;
		pBuffer += sizeof(nLength);
		for( uint32 i = 0 ; i < nLength ; i++ )
		{
			CopyMemory( &data, pBuffer+sizeof(TD)*i, sizeof(TD) );
			vcList.push_back( data );
		}
	}

public:
	void SetBuffer( uint8* pBuffer, uint32 maxLength )
	{
		m_temporary = pBuffer;
		m_maxLength = maxLength;
		m_offset = 0;
	}
	BOOL GetStringLength(uint16& length)
	{
		CopyMemory( &length, m_temporary+m_offset, sizeof(length) );
		m_offset += sizeof(uint16);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetUInt(uint8& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetUInt(uint16& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetUInt(uint32& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetUInt(uint64& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}	
	BOOL GetFloat(float& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetDouble(double& data)
	{
		CopyMemory( &data, m_temporary+m_offset, sizeof(data) );
		m_offset += sizeof(data);
		return (m_offset <= m_maxLength) ? TRUE : FALSE;
	}
	BOOL GetString(const uint32 length, TCHAR* buffer, const uint32 size)
	{
		if( length < size )
		{
			CopyMemory( buffer, m_temporary+m_offset, length );
			buffer[length/sizeof(TCHAR)] = NULL;			
			m_offset += length;
			return (m_offset <= m_maxLength) ? TRUE : FALSE;
		}
		return FALSE;
	}
	BOOL GetString(TCHAR* buffer, const uint32 size)
	{
		uint16 length;
		if(!GetStringLength(length)) return FALSE;

		if( length < size )
		{
			CopyMemory( buffer, m_temporary+m_offset, length );
			buffer[length/sizeof(TCHAR)] = NULL;			
			m_offset += length;
			return (m_offset <= m_maxLength) ? TRUE : FALSE;
		}
		return FALSE;
	}

	const uint8* GetExtra()			{ return (m_offset <= m_maxLength) ? (m_temporary + m_offset) : NULL; }
	const uint32 GetExtraLength()	{ return (m_offset <= m_maxLength) ? (m_maxLength - m_offset) : 0;  }

private:
	uint8* m_temporary;
	uint32 m_offset, m_maxLength;

	cBuffer m_storage;
};

