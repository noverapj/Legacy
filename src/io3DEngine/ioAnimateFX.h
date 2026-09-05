

#ifndef _ioAnimateFX_h_
#define _ioAnimateFX_h_

class ioRcAnimatePlane;
class ioRcAnimateFX;

class ioCamera;
class ioRenderQueue;
class ioAnimateFX;
class ioAnimation;
class ioSkeleton;
class ioMaterial;

class __EX ioAnimatePlane : public ioRenderable
{
	friend class ioAnimateFX;

protected:
	ioAnimateFX *m_pParent;
	ioRcAnimatePlane *m_pRcPlane;

	D3DXMATRIX	m_FullLocalTransform;
	D3DXMATRIX  m_ObjectOffsetTM;
	float	m_fLinkOffsetRate;
	int		m_iCurAlphaRate;

	D3DXMATRIX	m_OwnerTransform;

	bool	m_bSetLinked;
	bool	m_bSetOwnerTransform;

public:
	void SetLinkInfo( const D3DXMATRIX &rkObjectOffsetTM, float fLinkOffsetRate );
	bool UpdateFrame( float fCurTimePast );

protected:
	void SetOwnerTransform();

public:
	const ioHashString& GetLinkBipedName() const;
	FXTransformType GetTransformType() const;

public:	// overide ioRenderable
	virtual ioMaterial* GetMaterial() const;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera );
	virtual void GetRenderOperation( ioRenderOperation &rkRenderOp );
	
	virtual void GetWorldTransform( D3DXMATRIX *pMatWorld ) const;
	virtual int GetAlphaRate() const;

protected:
	ioAnimatePlane( ioAnimateFX *pParent, ioRcAnimatePlane *pRcPlane );
	virtual ~ioAnimatePlane();
};

class ioAnimateFX : public ioMovableObject
{
protected:
	typedef std::vector< ioAnimatePlane* > AnimatePlaneList;
	AnimatePlaneList m_AnimatePlaneList;
	ioRcAnimateFX *m_pRcFX;

	float m_fCurTimePast;
	float m_fTimeFactor;
	bool  m_bPlaneGenerated;

public:
	void Initialize( float fTimeFactor, ioSkeleton *pOwnerSkel );
	bool UpdateFX( float fCurTimeRate, ioSkeleton *pOwnerSkel );

protected:
	bool GeneratePlanes( ioSkeleton *pOwnerSkel );
	void DestroyAllPlanes();

public:	// overide ioMovableObject
	virtual void NotifyCurrentCamera( ioCamera *pCamera );
	virtual void UpdateRenderQueue( ioRenderQueue &queue );

	virtual const ioAxisAlignBox& GetBoundBox() const;
	virtual float GetBoundRadius() const;

public:
	ioAnimateFX( ioRcAnimateFX *pRcFX );
	virtual ~ioAnimateFX();
};

#endif