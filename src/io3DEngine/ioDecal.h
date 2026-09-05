

#ifndef _ioDecal_h_
#define _ioDecal_h_

#include "io3DCommon.h"
#include "ioRenderable.h"
#include "ioAxisAlignBox.h"

class ioMaterial;
class ioCamera;
class ioRenderOperation;
class ioEntityGroup;
class ioMeshData;
class ioDecalMaker;

class __EX ioDecal : public ioRenderable
{
	friend class ioDecalMaker;

protected:
	ioHashString m_Name;
	ioEntityGroup *m_pParent;
	ioMeshData *m_pMeshData;
	ioMaterial *m_pMaterial;

	ioAxisAlignBox m_BoundBox;
	bool m_bVisible;

protected:
	bool Build( const Vector3Vec &rkVtxList,
				const Vector2Vec &rkUVList,
				const WordVec &rkIdxList );

	void BuildBoundBox( const Vector3Vec &rkVtxList );
	void DestroyResource();
	
	void BuildMaterial();

public:
	void SetVisible( bool bVisible );
	void NotifyAttached( ioEntityGroup *pParent );
	void NotifyCurrentCamera( ioCamera *pCamera );

public:
	inline bool IsVisible() const { return m_bVisible; }
	inline const ioHashString& GetName() const { return m_Name; }

public:	// overide ioRenderable
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;

protected:
	ioDecal( const ioHashString &rkName );

public:
	virtual ~ioDecal();
};

#endif
