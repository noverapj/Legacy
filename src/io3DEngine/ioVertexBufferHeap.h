

#ifndef _ioVertexBufferHeap_h_
#define _ioVertexBufferHeap_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioVertexBufferInstance;
class ioVtxBuffer;

class ioVertexBufferHeap
{
private:
	struct VChunk
	{
		int iStart;
		int iEnd;
		VChunk *pPrev;
		VChunk *pNext;
		BOOL bUsed;

		inline int AllocSize() const { return iEnd - iStart; }
	};

	class FindRealChunk : public std::unary_function< VChunk*, bool >
	{
	private:
		int m_iStartOffset;

	public:
		bool operator()( const VChunk *pChunk ) const
		{
			if( pChunk->iStart == m_iStartOffset )
				return true;

			return false;
		}
		
		FindRealChunk( int iStartOffset ) : m_iStartOffset( iStartOffset )
		{
		}
	};

private:
	ioVtxBuffer	*m_pVtxBuf;
	BufferType	m_BufferType;
	BufferUsage m_BufferUsage;

	int		m_iHeapBufSize;
	int		m_iRefCount;

	typedef std::list< VChunk* > UsedVChunkList;
	UsedVChunkList	m_UsedChunkList;

	typedef std::list< VChunk* > EmptyChunkList;
	EmptyChunkList	m_EmptyChunkList;

public:
	bool IsCanAlloc( BufferType eType,
					 BufferUsage eUsage,
					 int iVtxStride,
					 int iVtxCnt );

public:
	ioVertexBufferInstance* CreateInstance( DWORD dwMask, int iVtxStride, int iVtxCnt );
	int DestroyInstance( ioVertexBufferInstance *pInstance );
	
public:
	void ReleaseIfDefaultPool();
	void ReCreateIfDefaultPool( IDirect3DDevice9 *pDevice );

private:
	VChunk* FindBestChunk( int iVtxStride, int iVtxCnt );
	VChunk* FindUsedChunk( int iStartOffset );

	bool AllocNewChunk( int iVtxStride, int iVtxCnt, int *pStartOffset );
	void DeAllocChunk( int iStartOffset );

public:
	inline int GetRefCount() const { return m_iRefCount; }
	inline BufferType GetBufferType() const { return m_BufferType; }
	inline BufferUsage GetBufferUsage() const { return m_BufferUsage; }

// For Debug
public:
	int GetTotalMemoryBytes() const;
	int GetUsedMemoryBytes() const;
	int GetFreeMemoryBytes() const;

public:
	ioVertexBufferHeap( ioVtxBuffer *pBuf, BufferType eType, BufferUsage eUsage );
	~ioVertexBufferHeap();
};

#endif