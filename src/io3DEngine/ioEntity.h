

#ifndef _ioEntity_h_
#define _ioEntity_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioMovableObject.h"
#include "ioMeshControlPoint.h"

class ioSceneManager;
class ioSubEntity;
class ioMesh;

class ioCamera;
class ioRenderQueue;
class ioMeshData;
class ioVertexBufferInstance;
class ioIndexBufferInstance;
class ioEntityParent;
class ioSkeleton;
class ioAxisAlignBox;
class ioShaderCustomArg;

class __EX ioEntity
{
protected:
	ioEntityParent *m_pParent;

	ioMesh	*m_pMesh;
	ioMeshData *m_pMeshData;
	ioVertexBufferInstance *m_pExtraBuf;

	typedef std::vector< ioSubEntity* > SubEntityList;
	SubEntityList m_SubEntityList;

	D3DXVECTOR4 *m_aBipedXformList;
	int *m_aBipedIndexList;
	int m_iApplyBipedCount;

	ioLightList m_ApplyLightList;

	D3DCOLORVALUE m_DiffuseRate;
	MeshType	m_MeshType;
	int  m_iAlphaRate;

	int  m_iSoftBufferStartPos;

	bool m_bVisible;	// 외부에서 강제로 출력 여부를 결정
	bool m_bCulled;		// EntityGroup에서 카메라로 컬링되었는지를 저장, SetEachEntityCullEnable시에만
	bool m_bSoftSkinningMeshData;
	bool m_bDoneEntityBuild;
	bool m_bUpdateBufferOK;
	bool m_bVisibleChangePass;	// 외부에서 강제로 출력 여부 변경시 적용 여부(true : 미적용, false : 적용)

public:
	void SetVisible( bool bVisible ) { m_bVisible = bVisible; }
	void SetCulled( bool bCulled ) { m_bCulled = bCulled; }
	void SetAlphaRate( int iAlphaRate ) { m_iAlphaRate = iAlphaRate; }
	void SetDiffuseRate( const D3DCOLORVALUE &rkColor ) { m_DiffuseRate = rkColor; }
	void SetMaterialName( const ioHashString &name );
	void SetCustomParameter( const ioShaderCustomArg &rkArg );
	void SetVisibleChangePass( bool bPass ) { m_bVisibleChangePass = bPass; }

	void ConvertToVertexColor();

public:
	inline const D3DCOLORVALUE& GetDiffuseRate() const { return m_DiffuseRate; }
	inline MeshType GetMeshType() const { return m_MeshType; }
	inline int GetAlphaRate() const { return m_iAlphaRate; }
	inline bool IsVisible() const { return m_bVisible; }
	inline bool IsCulled() const { return m_bCulled; }
	inline bool IsVisibleChangePass() const { return m_bVisibleChangePass; }

public:
	bool HasSkeleton() const;
	bool IsShadowRecvRender() const;
	bool IsLoadDone();

public:
	inline int GetNumSubEntity() const { return m_SubEntityList.size(); }
	ioSubEntity* GetSubEntity( int iIndex );

	inline ioVertexBufferInstance* GetExtraBuffer() const { return m_pExtraBuf; }
	inline ioMeshData* GetMeshData() const { return m_pMeshData; }
	inline ioMesh* GetMesh() const { return m_pMesh; }

	const ioHashString& GetMeshName() const;
	int GetMeshVertexCount() const;
	int GetMeshFaceCount() const;

protected:
	ioVertexBufferInstance* CreateExtraBuffer( BufferType bType, DWORD dwMask );
	void DestroyExtraBuffer();

public:
	inline void SetLightList( const ioLightList &rkList ) { m_ApplyLightList = rkList; }

	void UpdateLightList( const ioLightList &rkList,
						  const D3DXQUATERNION &qtParentRot,
						  const D3DXVECTOR3 &vParentPos,
						  const D3DXVECTOR3 &vParentScale );

	void UpdateRenderQueue( ioRenderQueue &queue, RenderGroupID eGroupID );
	void UpdateGenShadowMapToQueue( ioRenderQueue &queue, RenderGroupID eGroupID );

public:
	bool BuildEntity( const ioSkeleton *pSkeleton, bool bSoftware );
	void BuildSubEntityList();
	void BuildMeshData( bool bSoftSkinning );

	void CacheBipedMatrices( const ioSkeleton *pSkeleton );
	void SoftwareVertexBlending();

protected:
	void BuildBipedIndexList( const ioSkeleton *pSkeleton );

	void BuildHardwareMeshData();
	void BuildSoftSkinningMeshData();
	void DestroyMeshData();

public:
	void ConvertMaterialShaderStaticToAnimate();
	void ConvertMaterialShaderAnimateToStatic();

public:
	void  GetWorldTransform( D3DXMATRIX *pMatWorld ) const;
	float GetSquaredViewDepth( const ioCamera *pCamera );

	const D3DXVECTOR4* GetBipedList( int &iBipedListCnt ) const;
	inline const ioLightList& GetLights() const { return m_ApplyLightList; }

	const ioAxisAlignBox& GetBoundBox() const;
	float GetBoundRadius() const;
	
public:
	const ioMeshControlPoint* GetControlPoint( const ioHashString &szType, int iIndex ) const;
	const ioMeshControlPoint* GetControlPoint( int iArray ) const;
	ConstCtrlPointList GetControlPointList( const ioHashString &szType );
	int  GetControlPointCnt() const;

public:
	inline ioEntityParent* GetEntityParent() const { return m_pParent; }
	inline bool IsBuildDone() const { return m_bDoneEntityBuild; }

public:
	ioEntity( ioEntityParent *pParent, ioMesh *pMesh );
	virtual ~ioEntity();
};

#endif
