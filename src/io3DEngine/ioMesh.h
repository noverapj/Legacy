

#ifndef _ioMesh_h_
#define _ioMesh_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioResource.h"
#include "ioAxisAlignBox.h"
#include "ioVectorIterator.h"
#include "ioMeshControlPoint.h"

class ioSubMesh;
class ioEntity;
class ioVertexBufferInstance;
class ioIndexBufferInstance;
class ioMeshManager;

struct BlendWeight;
struct BlendWeightIndex;

class __EX ioMesh : public ioResource
{
public:
	typedef std::vector< ioSubMesh* > SubMeshList;
	typedef std::vector< ioVertexBufferInstance* > VertexBufferList;

protected:
	MeshType m_MeshType;
	VertexBufferList m_VertexBufferList;
	ioIndexBufferInstance *m_pIndexInstance;

	int m_iVertexCount;
	int m_iIndexCount;

protected:
	typedef std::map< VertexComponent, void* > SoftVertexBufferList;
	SoftVertexBufferList m_SoftVertexBufferList;
	WORD* m_pIndexBuf;

	ioVtxNrm4   *m_pPosNrm;
	JointWeight *m_pJointWeight;

protected:
	SubMeshList	m_SubMeshList;
	ioAxisAlignBox m_LocalBox;
	float m_fBoundRadius;

	ioHashStringVec m_BipedNameList;
	int m_iWeightedBipedCnt;

protected:
	CtrlPointSet m_ControlPointSet;

public:
	ioSubMesh* CreateSubMesh();
	ioSubMesh* GetSubMesh( int iIndex ) const;
	int GetNumSubMesh() const;

	typedef ioVectorIterator< SubMeshList > SubMeshIterator;
	SubMeshIterator GetSubMeshIterator();

public:
	bool AddControlPoint( ioMeshControlPoint *pPoint );
	inline int  GetControlPointCnt() const { return m_ControlPointSet.size(); }

	const ioMeshControlPoint* GetControlPoint( const ioHashString &szType, int iIndex ) const;
	const ioMeshControlPoint* GetControlPoint( int iArray ) const;
	ConstCtrlPointList GetControlPointList( const ioHashString &szType );
	void ClearAllControlPoints();

public:
	void AddVertexBuffer( ioVertexBufferInstance *pVtxBuf );
	void SetIndexBufferInstance( ioIndexBufferInstance *pInstance );
	void SetBipedNameList( const ioHashStringVec &rkNameList );

	void DestroyHardwareBuffers();

public:
	void AddSoftVertexBuffer( VertexComponent eComponent, void *pBuffer );
	void SetSoftBlendWeight( const BlendWeight *pWeights, int iVtxCount, int iJointCount );
	void SetSoftIndexBuffer( WORD *pBuf );
	void BuildSoftSkinningBuffers( int iVertexCount );
	bool CheckSoftNormalBuffer();

public:
	const void* GetSoftVertexBuffer( VertexComponent eComponent ) const;

	inline const WORD* GetSoftIndexBuffer() const { return m_pIndexBuf; }
	inline const ioVtxNrm4* GetSoftPosNrm() const { return m_pPosNrm; }
	inline const JointWeight* GetSoftJointWeights() const { return m_pJointWeight; }

public:
	ioVertexBufferInstance* GetVertexBufferInstance( int iIndex );
	ioVertexBufferInstance* GetVertexBufferInstanceByMask( DWORD dwMask );
	
	inline ioIndexBufferInstance* GetIndexBufferInstance() { return m_pIndexInstance; }
	inline int GetVertexBufferInstanceCnt() const { return m_VertexBufferList.size(); }

public:
	void SetMeshType( MeshType eType ) { m_MeshType = eType; }
	void SetBoundRadius( float fRadius ) { m_fBoundRadius = fRadius; }
	void SetBoundBox( const ioAxisAlignBox &rkAABox ) { m_LocalBox = rkAABox; }
	void SetWeightBipedCnt( int iWeightCnt ) { m_iWeightedBipedCnt = iWeightCnt; }
	void SetVertexCount( int iCount ) { m_iVertexCount = iCount; }
	void SetIndexCount( int iCount ) { m_iIndexCount = iCount; }

public:
	inline MeshType GetMeshType() const { return m_MeshType; }
	inline const ioAxisAlignBox& GetBoundBox() const { return m_LocalBox; }
	inline float GetBoundRadius() const { return m_fBoundRadius; }

	inline const ioHashStringVec& GetBipedNameList() const { return m_BipedNameList; }
	inline int GetWeightedBipedCnt() const { return m_iWeightedBipedCnt; }

public:
	inline int GetVertexCount() const { return m_iVertexCount; }
	inline int GetIndexCount() const { return m_iIndexCount; }
	inline int GetFaceCount() const { return m_iIndexCount / 3; }

private:
	friend class ioMeshManager;

protected:
	ioMesh( const ioHashString &name );
	virtual ~ioMesh();
};

#endif