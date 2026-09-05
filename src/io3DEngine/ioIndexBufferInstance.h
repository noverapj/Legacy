

#ifndef _ioIndexBufferInstance_h_
#define _ioIndexBufferInstance_h_

class ioIdxBuffer;
class ioIndexBufferHeap;

class __EX ioIndexBufferInstance
{
	friend class ioIndexBufferHeap;

private:
	ioIndexBufferHeap *m_pParentHeap;

	ioIdxBuffer *m_pIdxBuf;
	int		m_iOffsetIndex;
	int		m_iIndexCount;

public:
	bool Lock( DWORD dwAddFlags = 0, int iStartBytes = 0, int iLockSizeBytes = 0 );
	bool UnLock();

	void* GetBuffer();

public:
	IDirect3DIndexBuffer9* GetD3DBuffer() const;
	inline int GetOffsetIndex() const { return m_iOffsetIndex; }
	inline int GetIndexCount() const { return m_iIndexCount; }

	inline ioIndexBufferHeap* GetParentHeap() const { return m_pParentHeap; }

private:	// ioIndexBufferHeap 만이 생성할수 있다.
	ioIndexBufferInstance( ioIndexBufferHeap *pHeap,
						   ioIdxBuffer *pIdxBuffer,
						   int iOffsetIndex,
						   int iIndexCnt );

public:	
	~ioIndexBufferInstance();
};

#endif
