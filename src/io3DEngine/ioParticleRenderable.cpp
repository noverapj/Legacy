

#include "stdafx.h"

#include "ioIndexBufferManager.h"
#include "ioMeshData.h"

#include "ioRenderOperation.h"
#include "ioEffectBufferManager.h"

#include "ioCPU.h"
#include "ioParticleRenderable.h"

ioParticleRenderable::ioParticleRenderable( ioEffectBufferManager *pParent )
{
	m_pParent = pParent;
	m_iNumVisibleParticles = 0;
	m_iBaseVertexIndex = 0;

	m_fSquaredViewDepth = 0.0f;
}

ioParticleRenderable::~ioParticleRenderable()
{
}

void ioParticleRenderable::AddSystem( ioParticleSystem *pSys )
{
	m_ParticleSystems.push_back( pSys );
	m_iNumVisibleParticles += pSys->GetNumVisibleParticles();
}

int ioParticleRenderable::FillBuffer( ioVtxColorTex *pLockBuf, int iBaseVertexIndex )
{
	m_iBaseVertexIndex = iBaseVertexIndex;

	ParticleSystemList::iterator iter;
	for( iter=m_ParticleSystems.begin() ; iter!=m_ParticleSystems.end() ; ++iter )
	{
		pLockBuf += (*iter)->FillParticleBuffer( pLockBuf );
	}

	return m_iNumVisibleParticles * 4;
}

ioMaterial* ioParticleRenderable::GetMaterial() const
{
	return m_ParticleSystems.front()->GetMaterial();
}

float ioParticleRenderable::GetSquaredViewDepth( const ioCamera *pCamera )
{
	return m_fSquaredViewDepth;
}

void ioParticleRenderable::GetRenderOperation( ioRenderOperation &rkRenderOp )
{
	rkRenderOp.m_pMeshData	   = m_pParent->GetParticleMeshData();
	rkRenderOp.m_pMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyQuadSharedBuffer( m_iNumVisibleParticles );

	rkRenderOp.m_PrimitiveType = D3DPT_TRIANGLELIST;

	rkRenderOp.m_iMinIndex	  = 0;
	rkRenderOp.m_iVertexCount = m_iNumVisibleParticles * 4;
	rkRenderOp.m_iBaseVertexIndex = m_iBaseVertexIndex;

	rkRenderOp.m_iIndexStart  = 0;
	rkRenderOp.m_iFaceCount   = m_iNumVisibleParticles * 2;
	rkRenderOp.m_bUseIndex	  = true;
}

void ioParticleRenderable::GetWorldTransform( D3DXMATRIX *pMatWorld ) const
{
	D3DXMatrixIdentity( pMatWorld );
}

RenderGroupID ioParticleRenderable::GetRenderQueueGroup() const
{
	return m_ParticleSystems.front()->GetRenderQueueGroup();
}
