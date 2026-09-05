

#ifndef _ioParticleRenderable_h_
#define _ioParticleRenderable_h_

class ioMaterial;
class ioCamera;
class ioRenderOperation;
class ioEffectBufferManager;

#include "ioMemoryPool.h"
#include "ioRenderable.h"
#include "ioParticleSystem.h"

class ioParticleRenderable : public ioRenderable, public ioPoolObject< ioParticleRenderable, 1000 >
{
protected:
	friend class ioEffectBufferManager;

protected:
	ioEffectBufferManager *m_pParent;

	ParticleSystemList m_ParticleSystems;
	int m_iNumVisibleParticles;
	int m_iBaseVertexIndex;

	float m_fSquaredViewDepth;

public:
	void AddSystem( ioParticleSystem *pSys );
	void SetSquaredViewDepth( float fDepthSq ) { m_fSquaredViewDepth = fDepthSq; }
	int  FillBuffer( ioVtxColorTex *pLockBuf, int iBaseVertexIndex );

public:
	inline int GetNumSystems() const { return m_ParticleSystems.size(); }
	inline int GetNumVisibleParticles() const { return m_iNumVisibleParticles; }

public:
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;

public:
	RenderGroupID GetRenderQueueGroup() const;

protected:
	ioParticleRenderable( ioEffectBufferManager *pParent );
	virtual ~ioParticleRenderable();
};

#endif