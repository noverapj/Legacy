#include "stdafx.h"
#include "cEncrypt.h"

// cEncrypt

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cEncrypt::cEncrypt() : m_nArrayLen(4028)
{
	m_pArrayBuffer = new uint8[m_nArrayLen];
}

cEncrypt::~cEncrypt()
{
	if(m_pArrayBuffer)
	{
		delete []m_pArrayBuffer;
		m_pArrayBuffer = NULL;
	}
}

uint32  cEncrypt::Compress(const uint8 *lpSource, uint32 nSourceLen, uint8 *lpDestination)
{
	uint32 nIndex = 0;
	uint32 nZeroCount = 0;
	uint32 nZeroBit = 0;
	uint32 nBitArrayCount = 0;
	uint32 nUsedBytes = 0;
	uint8  bBit  = 0;

	ZeroMemory(m_pArrayBuffer, m_nArrayLen);

	for(uint32 i = 0; i < nSourceLen; ++i)
	{
		if(0 == lpSource[i])
		{
			++nZeroCount;
			nZeroBit		= i % 8;
			nBitArrayCount	= i >> 3;
			bBit			= 1;
			bBit			= bBit << nZeroBit;

			m_pArrayBuffer[nBitArrayCount] = m_pArrayBuffer[nBitArrayCount] | bBit;
			if(nUsedBytes < nBitArrayCount) nUsedBytes = nBitArrayCount;
		}
		else
		{
			// 1바이트씩 복사
			lpDestination[nIndex++] = lpSource[i];
		}
	}

	if(nZeroCount > 0) ++nUsedBytes;

	memmove(lpDestination+sizeof(uint16)+nUsedBytes, lpDestination, nSourceLen-nZeroCount);
	CopyMemory(lpDestination, &nUsedBytes, sizeof(uint16));		// + 널값정보에 이용되는 버퍼 길이 2Byte
	CopyMemory(lpDestination+sizeof(uint16), m_pArrayBuffer, nUsedBytes);

	return (sizeof(uint16) + nUsedBytes + nSourceLen - nZeroCount);
}

uint32  cEncrypt::Decompress(const uint8 *lpSource, uint32 nSourceLen, uint8 *lpDestination)
{
	uint16 nBitArrayCount = 0;
	uint32 nOriginalLen = 0;
	uint32 nBufferCount = 0;
	charBit *pTempByte;
	
	CopyMemory(&nBitArrayCount, lpSource, sizeof(unsigned short));
	nOriginalLen	= nSourceLen-sizeof(uint16);
	nBufferCount	= nOriginalLen = nOriginalLen - nBitArrayCount;

	ZeroMemory(m_pArrayBuffer, m_nArrayLen);
	CopyMemory(m_pArrayBuffer, lpSource+sizeof(uint16), nBitArrayCount);
	
	const uint8* lpData = (lpSource + sizeof(uint16) + nBitArrayCount);

	uint32 nSource = 0, nDestination = 0;
	for(uint32 i=0; i < nBitArrayCount; ++i)
	{
		pTempByte = reinterpret_cast<charBit*>(&m_pArrayBuffer[i]);

		if(pTempByte->v1==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v2==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v3==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v4==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v5==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v6==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v7==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}

		if(pTempByte->v8==0)
		{
			if(nSource < nBufferCount)		
				lpDestination[nDestination++] = lpData[nSource++];
		}
		else
		{
			lpDestination[nDestination++] = 0;
			++nOriginalLen;
		}
	}

	// 아직 데이타가 남아있으면 넣어준다
	// (m_pArrayBuffer 처리 이후에도 값이 남아있는경우는 (byte단위로)NULL값 없이 연속으로 데이터가 있는경우이다)
	for( ; nSource < nBufferCount;)
	{
		lpDestination[nDestination++] = lpData[nSource++];
	}

	return nOriginalLen;
}

