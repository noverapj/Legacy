#pragma once

// encrypt & compress
/*		
 *	[압축] 
 *          - 버퍼 구조 : 3개의 층으로 되어있다.
 *
 *				-------------------------------------------------------------
 *			1)	| 널 값을 가지는 위치(BYTE)정보를 가지는 버퍼의 크기(2Byte) |
 *				-------------------------------------------------------------
 *			2)	| 널 값의 위치 정보를 가지고 있는 버퍼. 크기는 가변(nByte)  |
 *				------------------------------------------------------------
 *			3)	| 널 값을 없앤 데이타.                                      |
 *				-------------------------------------------------------------
 */
class cEncrypt
{
public:
	cEncrypt();
	virtual ~cEncrypt();

public:
	struct charBit
	{
		uint8 v1:1;
		uint8 v2:1;
		uint8 v3:1;
		uint8 v4:1;
		uint8 v5:1;
		uint8 v6:1;
		uint8 v7:1;
		uint8 v8:1;
	};
	// 압축
	uint32	Compress(const uint8 *lpSource, uint32 nSourceLen, uint8 *lpDestination);
	// 압축해제
	uint32	Decompress(const uint8 *lpSource, uint32 nSourceLen, uint8 *lpDestination);

protected:
	uint32	m_nArrayLen;
	uint8*  m_pArrayBuffer;
};
