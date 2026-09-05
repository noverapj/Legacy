// cPacket.h: interface for the cPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CPACKET_H__4B0D147C_BA0F_4DD5_9E0F_DB797F29E846__INCLUDED_)
#define AFX_CPACKET_H__4B0D147C_BA0F_4DD5_9E0F_DB797F29E846__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "cHeader.h"
#include "cEncrypt.h"


// network packet
class cPacket : public cEncrypt
{
public:
	cPacket();
	~cPacket();

public:
	// 패킷 정보
	uint8*	GetBuffer()	{	return m_Buffer;		}
	uint32	GetLength()	{	return m_nLength;		}

	// 패킷 생성
	BOOL	Make(	const uint16 nCommand, 
					const uint8 *lpData, 
					const uint16 nLength,
					const uint8 nSequence = 0 );

	BOOL	Pack(	const uint16 nCommand, 
					const uint8 *lpData, 
					const uint16 nLength,
					const uint8 nSequence = 0 );

public:
	BOOL	Decode(	uint8 *lpBuffer,  
					const uint32 nLength,
					const uint8 nSequence,
					uint32& nResult );

	const uint8* GetOutput()	{ return m_Decompress; }

private:
	cHeader m_Header;

	uint8	*m_Buffer;
	uint8	*m_Compress, *m_Decompress;
	uint32	m_nLength;
};


#endif // !defined(AFX_CPACKET_H__4B0D147C_BA0F_4DD5_9E0F_DB797F29E846__INCLUDED_)
