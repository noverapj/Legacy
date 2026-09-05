// cPacket.cpp: implementation of the cPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cPacket.h"
#include "../include/Constant.h"



uint32 cPacket::m_maxLength = 1024 * 16;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cPacket::cPacket(const uint32 encoding) : m_compressor(NULL), m_decompressor(NULL), m_length(0), m_encoding(encoding)
{
	m_buffer = new uint8[m_maxLength];
	ZeroMemory(m_buffer, m_maxLength);
}

cPacket::~cPacket()
{
	if(m_buffer)
	{
		delete []m_buffer;
		m_buffer = NULL;
	}
	if(m_compressor)
	{
		delete []m_compressor;
		m_compressor = NULL;
	}
	if(m_decompressor)
	{
		delete []m_decompressor;
		m_decompressor = NULL;
	}
}

BOOL cPacket::Make(const uint16 command, const uint8 *data, const uint16 length, const uint8 sequence)
{
	if(0 == sequence)
	{
		return Pack(command, data, length, sequence);
	}

	switch(m_encoding)
	{
	case ENCRYPT_OFF: // 비압축
		{
			return Pack(command, data, length, sequence);
		}
		break;

	case ENCRYPT_ON: // 압축
		{	
			CreateCompressor();

			uint32 result = cEncrypt::Compress(data, length, m_compressor);
			return Pack(command, m_compressor, static_cast<uint16>(result), sequence);
		}
		break;
	}
	return FALSE;
}

BOOL cPacket::Pack(const uint16 command, const uint8 *data, const uint16 length, const uint8 sequence)
{
	// 총 길이
	m_length = length + sizeof(cHeader);
	if(m_length < m_maxLength)
	{
		// Header 설정
		m_header.SetHeader(length, command, sequence);
		CopyMemory(m_buffer, reinterpret_cast<uint8*>(&m_header), sizeof(cHeader));

		// Payload 복사
		CopyMemory(m_buffer+sizeof(cHeader), data, length);
		return TRUE;
	}

	m_length = 0;
	return FALSE;
}

BOOL cPacket::Decode(uint8 *buffer, const uint32 length, const uint8 sequence, uint32& result)
{
	if(0 == sequence)
		return FALSE;

	switch(m_encoding)
	{
	case ENCRYPT_OFF: // 비압축
		{
			return FALSE;
		}
		break;

	case ENCRYPT_ON: // 압축해제
		{	
			CreateDecompressor();
			result = Decompress(buffer, length, m_decompressor);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void cPacket::CreateCompressor()
{
	if(NULL == m_compressor)
	{
		static uint32 length = 1024*16;

		m_compressor = new uint8[length];
		ZeroMemory(m_compressor, length);
	}
}

void cPacket::CreateDecompressor()
{
	if(NULL == m_decompressor)
	{
		static uint32 length = 1024*16;

		m_decompressor = new uint8[length];
		ZeroMemory(m_decompressor, length);
	}
}
