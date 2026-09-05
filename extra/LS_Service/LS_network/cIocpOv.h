// cIocpOv.h: interface for the cIocpOv class.
//
// date : 2006.03.20,  신영욱
// note : IOCP operation에 필요한 WSAOVERLAPPED 객체와 
//        WSABUF 객체를 포함하고 있는 클래스
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "cOverlapped.h"


class cIocpOv : public cOverlapped
{
public:
	cIocpOv(E_IO_OPERATION iocpOP=E_IO_NONE);

public:
	void Init(const uint32 bufferLength, E_IO_OPERATION iocpOP);
	void Destroy();
	void Clean();

public:
	uint32 GetDataLength()		{ return m_dataLength; }
	uint32 GetBufferLength()	{ return m_bufferLength; }

	void Alloc(const uint32 bufferLength);
	void CopyBuffer(const uint8* buffer, const uint32 bufferLength);
	void AppendBuffer(const uint8* buffer, const uint32 bufferLength);

public:
	WSABUF m_wsaBuf;

	uint8* m_buffer;
	uint32 m_dataLength;
	uint32 m_bufferLength;
};
