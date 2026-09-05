

#ifndef _ioVtxBuffer_h_
#define _ioVtxBuffer_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class __EX ioVtxBuffer
{
private:
	IDirect3DVertexBuffer9	*m_pVtxBuffer;
	void *m_pBuffer;

private:
	BufferType m_BufType;
	BufferUsage m_BufUsage;

	DWORD	m_dwLockFlags;
	int		m_iBufSize;

public:
	bool Lock( DWORD dwAddFlags = 0, int iStartBytes = 0, int iLockSizeBytes = 0 );
	bool UnLock();

public:
	bool CreateVertexBuffer( IDirect3DDevice9 *pDevice,
							 BufferType eType,
							 BufferUsage eUsage,
							 int iBufferSize );
	
	void ReleaseIfDefaultPool();
	void ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice );

public:
	inline IDirect3DVertexBuffer9* GetVtxBuffer(){ return m_pVtxBuffer; }
	inline void* GetBuffer(){ return m_pBuffer; }
	inline int GetBufSize() const{ return m_iBufSize; }

public:
	ioVtxBuffer();
	~ioVtxBuffer();
};

#endif