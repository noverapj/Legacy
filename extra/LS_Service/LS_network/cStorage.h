#pragma once

#include "cBuffer.h"

class cStorage
{
public:
	cStorage(void);
	~cStorage(void);

	void Init();
	void Destroy();

public:
	BOOL CreateStorage( uint32 length )					{ return m_storage.Create(length);			}
	void DestroyStorage()	{ m_storage.Destroy(); }

	BOOL StoreBuffer( uint8* buffer, uint32 length )	{ return m_storage.Copy( buffer, length );	}
	void CleanStorage()		{ m_storage.Erase(); }
	
	uint8* GetStorageBuffer()	{ return m_storage.GetBuffer(); }
	uint32 GetStorageLength()	{ return m_storage.GetLength(); }

protected:
	cBuffer	m_storage;
};
