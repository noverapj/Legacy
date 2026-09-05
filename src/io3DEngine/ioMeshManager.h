

#ifndef _ioMeshManager_h_
#define _ioMeshManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioResourceManager.h"

class ioResource;
class ioMesh;
class ioVertexBufferBinder;
class ioVertexBufferInstance;
class ioIndexBufferInstance;
class ioBinaryStream;

struct BlendWeight;

class __EX ioMeshManager : public ioResourceManager, public Singleton< ioMeshManager >
{
protected:
	struct NotifiedRequest
	{
		ioBinaryStream *m_pStream;
		ioMesh *m_pMesh;
		DWORD m_dwLoadState;
	};

	typedef std::vector< NotifiedRequest > NotifiedRequestList;
	NotifiedRequestList m_NotifiedRequestList;
	CRITICAL_SECTION m_NotifiedListSection;

	float m_fAnimateMeshLODRate;
	bool  m_bDividedStaticVertexStream;

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	ioMesh* CreateMesh( const ioHashString &name, bool bThread = false );
	ioMesh* GetMeshByName( const ioHashString &name );

public:
	void SetAnimateMeshLODRate( float fRate );
	void UpdateNotifiedResources();

protected:
	bool IsMeshFile( const char *szToken );

	bool LoadFile( ioMesh *pMesh, bool bThread );
	bool LoadFileInternal( ioMesh *pMesh, ioBinaryStream *pStream );

public:
	inline void SetDivideStaticVertexStream( bool bDivide ) { m_bDividedStaticVertexStream = bDivide; }
	inline bool IsDivideStaticVertexSteram() const { return m_bDividedStaticVertexStream; }

public:
	static ioMeshManager& GetSingleton();

// ------- Mesh Loading Helper -------------

protected:
	D3DXVECTOR3	*m_pPos;
	D3DXVECTOR3	*m_pNormal;
	D3DXVECTOR3	*m_pTangent;
	D3DXVECTOR3	*m_pBiNormal;
	D3DXVECTOR2	*m_pTexUV;
	D3DXVECTOR2	*m_pLightTexUV;
	BlendWeight	*m_pWeight;
	DWORD		*m_pVtxColor;
	D3DXVECTOR3	*m_pBillCenter;
	WORD		*m_pFace;

	int			m_iCurJointWeightCnt;
	int			m_iCurMaxWeightCnt;

protected:
	void LoadPosition( ioBinaryStream *pStream, int iVtxCnt );
	void LoadNormal( ioBinaryStream *pStream, int iVtxCnt );
	void LoadTexUV( ioBinaryStream *pStream, int iVtxCnt );
	void LoadLightTexUV( ioBinaryStream *pStream, int iVtxCnt );
	void LoadTangentList( ioBinaryStream *pStream, int iVtxCnt );
	void LoadBiNormalList( ioBinaryStream *pStream, int iVtxCnt );
	void LoadBlendWeight( ioBinaryStream *pStream, int iVtxCnt );
	void LoadVertexColor( ioBinaryStream *pStream, int iVtxCnt );
	void LoadBillboardCenter( ioBinaryStream *pStream, int iVtxCnt );
	void LoadFace( ioBinaryStream *pStream, int iFaceCnt );

	void BuildAnimateLODMesh( ioMesh *pMesh, int &iVertexCount, int &iFaceCount );

protected:
	bool CreateVertexBuffers( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh );

	bool CreateDivideStream( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh );
	bool CreateOneStream( BufferType bType, int iVtxCnt, DWORD dwMask, ioMesh *pMesh );

	ioVertexBufferInstance* CreatePosition( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreatePosition2( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateWeights( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateIndices( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateNormal( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateColor0( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateUV0( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateUV1( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateTangent( BufferType bType, int iVtxCnt );
	ioVertexBufferInstance* CreateBinormal( BufferType bType, int iVtxCnt );

protected:
	bool CreateIndexBufferInstance( BufferType bType, int iFaceCnt, ioMesh *pMesh );
	void ApplySoftBuffers( ioMesh *pMesh, MeshType eType, int iVtxCnt, int iFaceCnt );
	void ClearTempBuffers();

public:
	void PrintStatics();

public:
	ioMeshManager();
	virtual ~ioMeshManager();
};

#define g_MeshMgr ioMeshManager::GetSingleton()

#endif
