

#include "stdafx.h"

#include "ioEffect.h"
#include "ioParticleSystem.h"

#include "ioMeshData.h"
#include "ioRenderQueue.h"
#include "ioMaterial.h"

#include "ioCPU.h"
#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"
#include "ioRenderSystem.h"

#include "ioParticleRenderable.h"
#include "ioEffectBufferManager.h"
#include "HelpFunc.h"
#include "ErrorReport.h"


#define PARTICLE_VERTEX_SIZE 32768
#define PARTICLE_INDEX_SIZE  65536
#define PARTICLE_FVF ( IOFVF_POSITION | IOFVF_COLOR0 | IOFVF_UV0 )

template<> ioEffectBufferManager* Singleton<ioEffectBufferManager>::ms_Singleton = 0;

ioEffectBufferManager::ioEffectBufferManager()
{
	m_pParticleMeshData = NULL;

	m_iNeedParticleQuad = 0;
	m_iMaxNeedParticleQuad = 0;
	m_iStartParticleVertex = 0;
}

ioEffectBufferManager::~ioEffectBufferManager()
{
	if( m_pParticleMeshData )
	{
		m_pParticleMeshData->DeleteAllBindingBuffers();
		m_pParticleMeshData->m_pIndexBufferInstance = NULL;
		SAFEDELETE( m_pParticleMeshData );
	}

	ClearRenderableLists();
}

void ioEffectBufferManager::NotifyParticleSystemList( ParticleSystemList &rkSystemList, float fDepthSq )
{
	ioParticleSystem *pPreSys = NULL;
	ioParticleRenderable *pCurRend = NULL;

	ParticleSystemList::iterator iter;
	for( iter=rkSystemList.begin() ; iter!=rkSystemList.end() ; ++iter )
	{
		ioParticleSystem *pCurSys = *iter;

		if( !pCurSys->IsEnable() ) continue;
		if( pCurSys->IsEachCulled() ) continue;
		if( pCurSys->GetNumVisibleParticles() <= 0 )
			continue;

		if( !pPreSys ||
			pPreSys->GetRenderQueueGroup() != pCurSys->GetRenderQueueGroup() ||
			!pPreSys->HasSameMaterialProperty( pCurSys ) )
		{
			AddParticleRenderable( pCurRend, fDepthSq );

			pCurRend = new ioParticleRenderable( this );
			pPreSys = pCurSys;
		}

		pCurRend->AddSystem( pCurSys );
	}

	AddParticleRenderable( pCurRend, fDepthSq );
}

void ioEffectBufferManager::UpdateRenderQueue( ioRenderQueue &queue )
{
	if( !UpdateEffectBuffers() )
		return;

	ParticleRenderableList::iterator iter=m_ParticleRenderables.begin();
	for( ; iter!=m_ParticleRenderables.end() ; ++iter )
	{
		ioParticleRenderable *pRenderable = *iter;

		queue.AddRenderable( pRenderable, pRenderable->GetRenderQueueGroup() );
	}
}

void ioEffectBufferManager::AddParticleRenderable( ioParticleRenderable *pRend, float fDepthSq )
{
	if( !pRend )	return;

	pRend->SetSquaredViewDepth( fDepthSq );
	m_ParticleRenderables.push_back( pRend );

	int iNumVisibleParticles = pRend->GetNumVisibleParticles();
	m_iMaxNeedParticleQuad = max( m_iMaxNeedParticleQuad, iNumVisibleParticles );
	m_iNeedParticleQuad   += iNumVisibleParticles;
}

void ioEffectBufferManager::CheckParticleBuffers()
{
	if( !m_pParticleMeshData )
	{
		m_pParticleMeshData = new ioMeshData;
		m_pParticleMeshData->InsertStreamSource( 0, PARTICLE_FVF );

		ioVertexBufferInstance *pVtxInstance = NULL;
		pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC,
															PARTICLE_FVF,
															PARTICLE_VERTEX_SIZE,
															ioRenderSystem::m_NoneAutoBufferUsage );

		m_pParticleMeshData->SetBinding( 0, pVtxInstance );
		m_pParticleMeshData->m_iVertexCount = PARTICLE_VERTEX_SIZE;
	}

	int iNeedVtxCnt = m_iNeedParticleQuad * 4;
	ioVertexBufferInstance *pVtxBuf = m_pParticleMeshData->GetVtxInstance( 0 );
	if( pVtxBuf->GetVertexCount() < iNeedVtxCnt )
	{
		g_VtxBufMgr.DestroyInstance( pVtxBuf );

		pVtxBuf = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC,
													   PARTICLE_FVF,
													   iNeedVtxCnt,
													   ioRenderSystem::m_NoneAutoBufferUsage );

		m_pParticleMeshData->SetBinding( 0, pVtxBuf );
		m_pParticleMeshData->m_iVertexCount = iNeedVtxCnt;

		LOG.PrintTimeAndLog( 0, "ioEffectBufferManager::CheckParticleBuffers - Remake Buffer(%d)", iNeedVtxCnt);
	}

	m_pParticleMeshData->m_pIndexBufferInstance = g_IdxBufMgr.NotifyQuadSharedBuffer( m_iMaxNeedParticleQuad );
	m_pParticleMeshData->m_iIndexCount = m_iMaxNeedParticleQuad * 6;
}

bool ioEffectBufferManager::UpdateEffectBuffers()
{
	if( m_ParticleRenderables.empty() )
		return false;

	CheckParticleBuffers();

	int iNeedVtx = m_iNeedParticleQuad * 4;
	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;

	ioVertexBufferInstance *pVtxBuf = m_pParticleMeshData->GetVtxInstance( 0 );
	if( m_iStartParticleVertex + iNeedVtx > pVtxBuf->GetVertexCount() )
	{
		m_iStartParticleVertex = 0;
		dwLockFlags = D3DLOCK_DISCARD;
	}

	int iLockSizeBytes  = iNeedVtx * sizeof(ioVtxColorTex);
	int iLockStartBytes = m_iStartParticleVertex * sizeof(ioVtxColorTex);

	if( !pVtxBuf->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
		return false;

	bool bUpdateOK = true;
	__try
	{
		int iBaseVertexIndex = m_iStartParticleVertex;
		ioVtxColorTex *pCursor = (ioVtxColorTex*)pVtxBuf->GetBuffer();

		int iCurFilledVertexCnt = 0;
		for( size_t i=0 ; i<m_ParticleRenderables.size() ; i++ )
		{
			ioParticleRenderable *pRend = m_ParticleRenderables[i];
			iCurFilledVertexCnt = pRend->FillBuffer( pCursor, iBaseVertexIndex );

			iBaseVertexIndex += iCurFilledVertexCnt;
			pCursor += iCurFilledVertexCnt;
		}
	}
	__except( ErrorReport::DynamicBufferFilter( GetExceptionCode() ) )
	{
		bUpdateOK = false;
	}

	pVtxBuf->UnLock();
	m_iStartParticleVertex += iNeedVtx;

	return bUpdateOK;
}

void ioEffectBufferManager::ClearRenderableLists()
{
	if( !m_ParticleRenderables.empty() )
	{
		ParticleRenderableList::iterator iter;
		for( iter=m_ParticleRenderables.begin() ; iter!=m_ParticleRenderables.end() ; ++iter )
		{
			delete *iter;
		}

		m_ParticleRenderables.erase( m_ParticleRenderables.begin(), m_ParticleRenderables.end() );
	}

	m_iMaxNeedParticleQuad = 0;
	m_iNeedParticleQuad = 0;
}

ioEffectBufferManager& ioEffectBufferManager::GetSingleton()
{
	return Singleton<ioEffectBufferManager>::GetSingleton();
}


