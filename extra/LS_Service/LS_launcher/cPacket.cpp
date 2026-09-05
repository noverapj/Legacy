// cPacket.cpp: implementation of the cPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "cPacket.h"


//#define _ENCODE	1




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cPacket::cPacket() : m_nLength(0)
{
	m_Buffer = new uint8[MAX_TRANSFER_SIZE];
	ZeroMemory(m_Buffer, MAX_TRANSFER_SIZE);

	m_Compress = new uint8[MAX_TRANSFER_SIZE];
	ZeroMemory(m_Compress, MAX_TRANSFER_SIZE);

	m_Decompress = new uint8[MAX_TRANSFER_SIZE];
	ZeroMemory(m_Decompress, MAX_TRANSFER_SIZE);
}

cPacket::~cPacket()
{
	if(m_Buffer)
	{
		delete []m_Buffer;
		m_Buffer = NULL;
	}
	if(m_Compress)
	{
		delete []m_Compress;
		m_Compress = NULL;
	}
	if(m_Decompress)
	{
		delete []m_Decompress;
		m_Decompress = NULL;
	}
}

BOOL	cPacket::Make(const uint16 nCommand, const uint8 *lpData, const uint16 nLength, const uint8 nSequence)
{
	if(0 == nSequence)
	{
		return Pack(nCommand, lpData, nLength, nSequence);
	}

	return Pack(nCommand, lpData, nLength, nSequence);
	//switch(ENCRYPT_OFF)
	//{
	//case ENCRYPT_OFF: // 비압축
	//	{
	//		return Pack(nCommand, lpData, nLength, nSequence);
	//	}
	//	break;

	//case ENCRYPT_ON: // 압축
	//	{	
	//		uint32 nResult = cEncrypt::Compress(lpData, nLength, m_Compress);
	//		return Pack(nCommand, m_Compress, static_cast<uint16>(nResult), nSequence);
	//	}
	//	break;
	//}
	return FALSE;
}

BOOL	cPacket::Pack(const uint16 nCommand, const uint8 *lpData, const uint16 nLength, const uint8 nSequence)
{
	// 총 길이
	m_nLength = nLength + sizeof(cHeader);
	if(m_nLength <= 60000)
	{
		// Header 설정
		m_Header.SetHeader(nLength, nCommand, nSequence);
		CopyMemory(m_Buffer, reinterpret_cast<uint8*>(&m_Header), sizeof(cHeader));

		// Payload 복사
		CopyMemory(m_Buffer+sizeof(cHeader), lpData, nLength);
		return TRUE;
	}

	m_nLength = 0;
	return FALSE;
}

BOOL	cPacket::Decode(uint8 *lpBuffer, const uint32 nLength, const uint8 nSequence, uint32& nResult)
{
	if(0 == nSequence)
		return FALSE;

	return FALSE;
	//switch(g_Config.GetEncryptType())
	//{
	//case ENCRYPT_OFF: // 비압축
	//	{
	//		return FALSE;
	//	}
	//	break;

	//case ENCRYPT_ON: // 압축해제
	//	{	
	//		nResult = Decompress(lpBuffer, nLength, m_Decompress);
	//		return TRUE;
	//	}
	//	break;
	//}
	return FALSE;
}