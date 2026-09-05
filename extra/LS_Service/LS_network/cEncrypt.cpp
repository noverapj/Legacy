#include "stdafx.h"
#include "cEncrypt.h"

// cEncrypt

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cEncrypt::cEncrypt() : m_arrayLen(4028)
{
	m_arrayBuffer = new uint8[m_arrayLen];
}

cEncrypt::~cEncrypt()
{
	if(m_arrayBuffer)
	{
		delete []m_arrayBuffer;
		m_arrayBuffer = NULL;
	}
}

uint32  cEncrypt::Compress(const uint8 *source, uint32 sourceLen, uint8 *destination)
{
	uint32 index = 0;
	uint32 zeroCount = 0;
	uint32 zeroBit = 0;
	uint32 bitArrayCount = 0;
	uint32 usedBytes = 0;
	uint8  bit = 0;

	ZeroMemory(m_arrayBuffer, m_arrayLen);

	for(uint32 i = 0; i < sourceLen; ++i)
	{
		if(0 == source[i])
		{
			++zeroCount;
			zeroBit		= i % 8;
			bitArrayCount	= i >> 3;
			bit			= 1;
			bit			= bit << zeroBit;

			m_arrayBuffer[bitArrayCount] = m_arrayBuffer[bitArrayCount] | bit;
			if(usedBytes < bitArrayCount) usedBytes = bitArrayCount;
		}
		else
		{
			// 1바이트씩 복사
			destination[index++] = source[i];
		}
	}

	if(zeroCount > 0) ++usedBytes;

	memmove(destination+sizeof(uint16)+usedBytes, destination, sourceLen-zeroCount);
	CopyMemory(destination, &usedBytes, sizeof(uint16));		// + 널값정보에 이용되는 버퍼 길이 2Byte
	CopyMemory(destination+sizeof(uint16), m_arrayBuffer, usedBytes);

	return (sizeof(uint16) + usedBytes + sourceLen - zeroCount);
}

uint32  cEncrypt::Decompress(const uint8 *source, uint32 sourceLen, uint8 *destination)
{
	uint16 bitArrayCount = 0;
	uint32 originalLen = 0;
	uint32 bufferCount = 0;
	charBit *tempByte;
	
	CopyMemory(&bitArrayCount, source, sizeof(unsigned short));
	originalLen	= sourceLen-sizeof(uint16);
	bufferCount	= originalLen = originalLen - bitArrayCount;

	ZeroMemory(m_arrayBuffer, m_arrayLen);
	CopyMemory(m_arrayBuffer, source+sizeof(uint16), bitArrayCount);
	
	const uint8* lpData = (source + sizeof(uint16) + bitArrayCount);

	uint32 indexSrc = 0, indexDst = 0;
	for(uint32 i=0; i < bitArrayCount; ++i)
	{
		tempByte = reinterpret_cast<charBit*>(&m_arrayBuffer[i]);

		if(tempByte->v1==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v2==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v3==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v4==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v5==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v6==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v7==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}

		if(tempByte->v8==0)
		{
			if(indexSrc < bufferCount)		
				destination[indexDst++] = lpData[indexSrc++];
		}
		else
		{
			destination[indexDst++] = 0;
			++originalLen;
		}
	}

	// 아직 데이타가 남아있으면 넣어준다
	// (m_arrayBuffer 처리 이후에도 값이 남아있는경우는 (byte단위로)NULL값 없이 연속으로 데이터가 있는경우이다)
	for( ; indexSrc < bufferCount;)
	{
		destination[indexDst++] = lpData[indexSrc++];
	}

	return originalLen;
}

