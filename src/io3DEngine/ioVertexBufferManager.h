

#ifndef _ioVtxBufferManager_h_
#define _ioVtxBufferManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioVertexDeclaration.h"

class ioVertexBufferInstance;
class ioVertexBufferHeap;
class ioMeshData;

class __EX ioVertexBufferManager : public Singleton< ioVertexBufferManager >
{
protected:
	typedef std::list< ioVertexDeclaration > VertexDeclarationList;
	VertexDeclarationList m_VertexDeclarationList;

	typedef std::list< ioVertexBufferHeap* > VertexBufferHeapList;
	VertexBufferHeapList  m_VertexBufferHeapList;

	IDirect3DDevice9 *m_pD3DDevice;

protected:
	ioMeshData *m_pShareVtxTexData;
	ioMeshData *m_pShareXZPlaneData;

protected:
	static BufferUsage m_AutoBufferUsage;

public:
	void ReleaseDefaultPoolResources();
	void ReCreateDefaultPoolResources();

public:
	ioVertexBufferInstance* CreateInstance( BufferType bType,
											DWORD dwMask,
											int iVtxCnt,
											BufferUsage eUsage = BU_AUTO );

	ioVertexBufferInstance* CreateInstance( BufferType bType,
											DWORD dwMask,
											int iVtxCnt,
											void *pBuf,
											DWORD dwLockFlags = 0,
											BufferUsage eUsage = BU_AUTO );

	ioVertexBufferInstance* CreateExclusiveInstance( BufferType bType,
													 DWORD dwMask,
													 int iVtxCnt,
													 BufferUsage eUsage = BU_AUTO );

	ioVertexBufferInstance* CreateExclusiveInstance( BufferType bType,
													 DWORD dwMask,
													 int iVtxCnt,
													 void *pBuf,
													 DWORD dwLockFlags = 0,
													 BufferUsage eUsage = BU_AUTO );

	void DestroyInstance( ioVertexBufferInstance *pInstance );

	IDirect3DVertexDeclaration9* GetD3DVertexDeclaration( const ioVertexDeclaration &kDecl );

protected:
	ioVertexBufferHeap* CreateNewHeap( BufferType eType,
									   BufferUsage eUsage,
									   int iBufferSize );

	ioVertexBufferHeap* FindAllocEnableHeap( BufferType bType,
											 BufferUsage eUsage,
											 int iVtxStride,
											 int iVtxCnt );

	void DestroyAllVertexBufferHeap();

	IDirect3DVertexDeclaration9* CreateNewVertexDeclaration( const ioVertexDeclaration &kDecl );
	void DestroyAllVertexDeclaration();

public:
	ioMeshData* GetShareVtxTexData();
	ioMeshData* GetShareXZPlaneData();

public:
	int GetTotalHeapCount() const;
	int GetTotalHeapMemory() const;
	int GetTotalHeapUsedMemory() const;
	int GetTotalHeapFreeMemory() const;

	int GetTotalHeapCount( BufferType eType ) const;
	int GetTotalHeapMemory( BufferType eType ) const;
	int GetTotalHeapUsedMemory( BufferType eType ) const;
	int GetTotalHeapFreeMemory( BufferType eType ) const;

	int GetHeapTotalMemory( int iIndex ) const;
	int GetHeapUsedMemory( int iIndex ) const;
	int GetHeapFreeMemory( int iIndex ) const;

public:
	static ioVertexBufferManager& GetSingleton();
	
	static void SetAutoBufferUsage( BufferUsage eUsage );
	static BufferUsage GetAutoBufferUsage() { return m_AutoBufferUsage; }

public:
	ioVertexBufferManager( IDirect3DDevice9 *pDevice );
	virtual ~ioVertexBufferManager();
};

#define g_VtxBufMgr ioVertexBufferManager::GetSingleton()

#endif
