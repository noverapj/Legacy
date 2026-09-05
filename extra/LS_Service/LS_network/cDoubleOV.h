#pragma once

#include "cIocpOv.h"


class cDoubleOV
{
public:
	cDoubleOV(void);
	~cDoubleOV(void);

	void Init();
	void Destroy();

public:
	void Reset();

	uint32 GetDataLength();
	uint32 GetBufferLength();
	BOOL IsSending();

	void Alloc(const uint32 bufferLength);
	void CopyBuffer(const uint8* buffer, const uint32 bufferLength);
	BOOL AppendBuffer(const uint8* buffer, const uint32 bufferLength);
	BOOL Appendable(const uint32 bufferLength);

	cIocpOv* Pop();
	void Push(cIocpOv* iocpOvl);

protected:
	uint32 GetIterator();

protected:
	cIocpOv m_iocpOV[2];
	uint32 m_iterator;
	volatile uint32 m_sending;
};

