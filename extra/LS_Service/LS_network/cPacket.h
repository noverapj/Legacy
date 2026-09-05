#pragma once

#include "cHeader.h"
#include "cEncrypt.h"


// network packet
class cPacket : public cEncrypt
{
public:
	cPacket(const uint32 encoding=0);
	~cPacket(void);

public:
	// 패킷 정보
	uint8* GetBuffer()	{ return m_buffer; }
	uint32 GetLength()	{ return m_length; }

	// 패킷 생성
	BOOL Make(	const uint16 command, 
				const uint8 *data, 
				const uint16 length,
				const uint8 sequence = 0 );

	BOOL Pack(	const uint16 command, 
				const uint8 *data, 
				const uint16 length,
				const uint8 sequence = 0 );

public:
	BOOL Decode(	uint8 *buffer,  
					const uint32 length,
					const uint8 sequence,
					uint32& result );

	const uint8* GetOutput()	{ return m_decompressor; }

private:
	void CreateCompressor();
	void CreateDecompressor();

private:
	cHeader m_header;

	uint8  *m_buffer;
	uint8  *m_compressor, *m_decompressor;
	uint32 m_length;
	uint32 m_encoding;

public:
	static uint32 m_maxLength;
};
