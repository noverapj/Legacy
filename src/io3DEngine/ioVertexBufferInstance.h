

#ifndef _ioVertexBufferInstance_h_
#define _ioVertexBufferInstance_h_

#include "io3DCommon.h"

class ioVtxBuffer;
class ioVertexBufferHeap;

class __EX ioVertexBufferInstance
{
	friend class ioVertexBufferHeap;

private:
	ioVertexBufferHeap *m_pParentHeap;

	ioVtxBuffer *m_pVtxBuf;
	DWORD	m_dwMask;
	int m_iOffsetBytes;
	int m_iVertexStride;
	int m_iVertexCount;

	int m_iExtraOffset;

public:
	bool Lock( DWORD dwAddFlags = 0, int iStartBytes = 0, int iLockSizeBytes = 0 );
	bool UnLock();

	void* GetBuffer();

public:
	void SetExtraOffset( int iExtra ) { m_iExtraOffset = iExtra; }

public:
	IDirect3DVertexBuffer9* GetD3DBuffer() const;

	inline DWORD GetVertexMask() const { return m_dwMask; }
	inline int GetVertexCount() const { return m_iVertexCount; }
	inline int GetVertexStride() const { return m_iVertexStride; }
	
	inline int GetOffsetBytes() const { return m_iOffsetBytes; }
	inline int GetExtraOffsetBytes() const { return m_iExtraOffset; }
	inline int GetFullOffsetBytes() const { return m_iOffsetBytes + m_iExtraOffset; }

	inline ioVertexBufferHeap* GetParentHeap() const { return m_pParentHeap; }

private:
	ioVertexBufferInstance( ioVertexBufferHeap *pHeap,
							ioVtxBuffer *pVtxBuffer,
							DWORD dwMask,
							int iOffsetBytes,
							int iVtxStride,
							int iVtxCnt );

public:
	~ioVertexBufferInstance();
};

#endif

