

#ifndef _ioSubEntity_h_
#define _ioSubEntity_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioRenderable.h"

class ioEntity;
class ioSubMesh;
class ioMaterial;
class ioRenderOperation;
class ioCamera;

class __EX ioSubEntity : public ioRenderable
{
	friend class ioEntity;
protected:
	ioEntity  *m_pParent;
	ioSubMesh *m_pSubMesh;

	ioHashString m_DefaultMaterialName;

	ioHashString m_MaterialName;
	ioMaterial *m_pMaterial;

public:
	void SetMaterialName( const ioHashString &name );
	const ioHashString& GetMaterialName() const;

	void ChangeMaterial( const ioHashString &name, bool Soft );
	void RestoreMaterial( bool bSoft );

	void ConvertShaderStaticToVertexColor();
	void ConvertShaderAnimateToStatic();
	void ConvertShaderStaticToAnimate();

public:
	bool IsTransparent() const;
	bool IsMaterialLoadDone();

public:
	int GetMinIndex() const;
	int GetVertexCount() const;
	int GetFaceCount() const;
	int GetIndexStart() const;

// Overide ioRenderable
public:
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRendOp );
	virtual const D3DCOLORVALUE& GetDiffuseRate() const;
	virtual DWORD GetMeshHash() const;

	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;
	virtual const D3DXVECTOR4* GetBipedList( int &iBipedListCnt ) const;
	virtual int GetWeightedBipedCnt() const;
	virtual const ioLightList& GetLights() const;
	virtual int GetAlphaRate() const;

	virtual bool IsShadowRecvRender() const;

protected:
	ioSubEntity( ioEntity *pParent, ioSubMesh *pSubMesh );
	virtual ~ioSubEntity();
};

#endif
