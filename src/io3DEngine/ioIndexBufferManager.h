

#ifndef _ioIndexBufferManager_h_
#define _ioIndexBufferManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioIndexBufferInstance;
class ioIndexBufferHeap;

class __EX ioIndexBufferManager : public Singleton< ioIndexBufferManager >
{
protected:
	typedef std::list< ioIndexBufferHeap* > IndexBufferHeapList;
	IndexBufferHeapList m_IndexBufferHeapList;

	IDirect3DDevice9 *m_pD3DDevice;

	ioIndexBufferInstance *m_pQuadSharedIdxBuf;
	ioIndexBufferInstance *m_pStripSharedIdxBuf;

	static BufferUsage m_AutoBufferUsage;
	
public:
	void ReleaseDefaultPoolResources();
	void ReCreateDefaultPoolResources();

public:
	ioIndexBufferInstance* CreateInstance( BufferType eType,
										   int iIdxCnt,
										   BufferUsage eUsage = BU_AUTO );

	ioIndexBufferInstance* CreateInstance( BufferType eType,
										   int iIdxCnt,
										   const void *pBuf,
										   DWORD dwLockFlag = 0,
										   BufferUsage eUsage = BU_AUTO );

	ioIndexBufferInstance* CreateExclusiveInstance( BufferType eType,
												    int iIdxCnt,
													BufferUsage eUsage = BU_AUTO );

	void DestroyInstance( ioIndexBufferInstance *pInstance );

public:
	ioIndexBufferInstance* NotifyQuadSharedBuffer( int iQuadCnt );
	ioIndexBufferInstance* NotifyStripSharedBuffer( int iStripCnt );

protected:
	ioIndexBufferHeap* FindAllocEnableHeap( BufferType eType,
											BufferUsage eUsage,
											int iIdxCnt );

	ioIndexBufferHeap* CreateNewHeap( BufferType eType, BufferUsage eUsage, int iIndexCnt );

	void DestroyAllIndexHeap();

public:
	int GetTotalHeapCount() const;
	int GetTotalHeapMemory() const;
	int GetTotalHeapUsedMemory() const;
	int GetTotalHeapFreeMemory() const;

	int GetHeapTotalMemory( int iIndex ) const;
	int GetHeapUsedMemory( int iIndex ) const;
	int GetHeapFreeMemory( int iIndex ) const;

public:
	static ioIndexBufferManager& GetSingleton();
	static void SetAutoBufferUsage( BufferUsage eUsage );
	static BufferUsage GetAutoBufferUsage() { return m_AutoBufferUsage; }
	
public:
	ioIndexBufferManager( IDirect3DDevice9 *pDevice );
	virtual ~ioIndexBufferManager();
};

#define g_IdxBufMgr ioIndexBufferManager::GetSingleton()

#endif
