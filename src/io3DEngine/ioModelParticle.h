

#ifndef _ioModelParticle_h_
#define _ioModelParticle_h_

#include "io3DCommon.h"
#include "ioEntityParent.h"
#include "ioMemoryPool.h"

class ioEntity;
class ioCamera;
class ioAxisAlignBox;
class ioRenderQueue;
class ioModelParticleSystem;

class __EX ioModelParticle : public ioEntityParent, public ioPoolObject< ioModelParticle >
{
public:
	D3DXQUATERNION m_qtOrientation;
	D3DXQUATERNION m_qtStartParentOrientation;
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vScale;
	D3DXVECTOR3 m_vDir;

	float	m_fCurTime;
	float	m_fLiveTime;
	float	m_fMoveSpeed;
	float	m_fRotSpeed;

protected:
	D3DXMATRIX m_WorldTransform;
	ioModelParticleSystem *m_pParent;
	ioEntity *m_pEntity;

public:
	void SetParent( ioModelParticleSystem *pParent );
	void SetEntity( const ioHashString &kMeshName );
	void SetRotateSpeed( float fAngle );
	void SetAlphaRate( int iAlphaRate );

public:
	void Rotate( const D3DXVECTOR3 &vAxis, float fAngle );
	void Rotate( const D3DXQUATERNION &qtRot );

public:
	void UpdateWorldTransform();
	void UpdateRenderQueue( ioRenderQueue &queue, RenderGroupID eGroupID );
	void BuildResource();

public:
	bool  GetTextureMatrix( int iSubIndex, D3DXMATRIX *pMatTex );
	DWORD GetParticleColor() const;

	bool GetWorldBoundPos( D3DXVECTOR3 &vMin, D3DXVECTOR3 &vMax ) const;
	ioAxisAlignBox GetWorldBoundBox() const;

	ioEntity *GetEntity(){ return m_pEntity; }
// overide ioEntityParent
public:
	virtual void  GetEntityTransform( D3DXMATRIX *pMatWorld ) const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );

	virtual bool IsShadowRecvRender() const { return false; }
	virtual bool HasSkeleton() const { return false; }

public:
	inline float GetRotateSpeed() const { return m_fRotSpeed; }

	inline bool IsLive() const
	{
		if( m_fCurTime < m_fLiveTime )
			return true;

		return false;
	}

	inline float TimeRate() const
	{
		return m_fCurTime / m_fLiveTime;
	}

public:
	ioModelParticle();
	virtual ~ioModelParticle();
};

#endif
