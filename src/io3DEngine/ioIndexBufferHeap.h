

#ifndef _ioIndexBufferHeap_h_
#define _ioIndexBufferHeap_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioIndexBufferInstance;
class ioIdxBuffer;

class ioIndexBufferHeap
{
private:
	struct IChunk
	{
		int iStart;
		int iEnd;
		IChunk *pPrev;
		IChunk *pNext;
		BOOL bUsed;

		inline int AllocSize() const { return iEnd - iStart; }
	};

	class FindIChunk : public std::unary_function< IChunk*, bool >
	{
	private:
		int m_iStart;
	public:
		bool operator()( const IChunk *pChunk ) const
		{
			if( pChunk->iStart == m_iStart )
				return true;

			return false;
		}

		FindIChunk( int iStart ) : m_iStart(iStart){}
	};

private:
	ioIdxBuffer *m_pIdxBuf;
	BufferType  m_BufferType;
	BufferUsage m_BufferUsage;

	int		m_iHeapIdxSize;
	int		m_iRefCount;

	typedef std::list< IChunk* > UsedIChunkList;
	UsedIChunkList	m_UsedChunkList;

	typedef std::list< IChunk* > EmptyIChunkList;
	EmptyIChunkList m_EmptyChunkList;
	
public:
	void ReleaseIfDefaultPool();
	void ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice );

public:
	ioIndexBufferInstance* CreateInstance( int iIdxCnt );
	int DestroyInstance( ioIndexBufferInstance *pInstance );

private:
	IChunk* FindBestChunk( int iIdxCnt );
	IChunk* FindUsedChunk( int iStart );

	bool AllocNewChunk( int iIdxCnt, int *pStart );
	void DeAllocChunk( int iStart );

public:
	bool IsCanAlloc( BufferType eType, BufferUsage eUsage, int iIdxCnt );
	inline int GetRefCount() const { return m_iRefCount; }

// For Debug
	int GetTotalMemoryBytes() const;
	int GetAllocMemoryBytes() const;
	int GetFreeMemoryBytes() const;

public:
	ioIndexBufferHeap( ioIdxBuffer *pBuf, BufferType eType, BufferUsage eUsage );
	~ioIndexBufferHeap();
};

#endif
