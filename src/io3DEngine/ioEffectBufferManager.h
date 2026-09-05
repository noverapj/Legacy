

#ifndef _ioEffectBufferManager_h_
#define _ioEffectBufferManager_h_

class ioMeshData;
class ioEffect;
class ioRenderQueue;
class ioParticleRenderable;

#include "ioParticleSystem.h"

class ioEffectBufferManager : public Singleton< ioEffectBufferManager >
{
protected:
	ioMeshData *m_pParticleMeshData;

	typedef std::vector< ioParticleRenderable* > ParticleRenderableList;
	ParticleRenderableList m_ParticleRenderables;
	int m_iNeedParticleQuad;
	int m_iMaxNeedParticleQuad;
	int m_iStartParticleVertex;

public:
	void NotifyParticleSystemList( ParticleSystemList &rkSystemList, float fDepthSq );

public:
	void UpdateRenderQueue( ioRenderQueue &queue );
	void ClearRenderableLists();

protected:
	void AddParticleRenderable( ioParticleRenderable *pRend, float fDepthSq );
	void UpdateParticleBuffers();
	void CheckParticleBuffers();

protected:
	bool UpdateEffectBuffers();

public:
	inline ioMeshData* GetParticleMeshData() const { return m_pParticleMeshData; }

public:
	static ioEffectBufferManager& GetSingleton();

public:
	ioEffectBufferManager();
	virtual ~ioEffectBufferManager();
};

#define g_EffectBuffer ioEffectBufferManager::GetSingleton()

#endif