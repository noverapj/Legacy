

#ifndef _ioRcAnimateFX_h_
#define _ioRcAnimateFX_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioAxisAlignBox.h"
#include "ioTimeRateFactor.h"

class ioMeshData;
class ioRcAniTrack;
class ioMaterial;

class __EX ioRcAnimatePlane
{
protected:
	ioMeshData   *m_pMeshData;
	ioRcAniTrack *m_pTrack;
	ioMaterial	 *m_pMtl;

	float	m_fStartTime;
	float	m_fDuration;

	D3DXMATRIX   m_InvWorldTM;
	ioHashString m_TextureName;
	ioHashString m_LinkBipedName;

	FXRenderType m_RenderType;
	FXTransformType m_TransformType;

	ioTimeRateFactor m_AlphaRateMap;

public:
	ioRcAniTrack* CreateTrack();
	bool GetCurFrame( float fCurTime, D3DXVECTOR3 &vTrans, D3DXQUATERNION &qtRot );

public:
	void AddAlphaRate( float fTimeRate, float fAlphaRate );
	void SetAlphaRate( int iIdx, float fAlphaRate );

	float GetAlphaRateCnt() const;

	float GetPlaneAlphaRate( float fTimeRate );
	float GetTimeRate( int iIdx );
	float GetAlphaRate( int iIdx );

	void RemoveAlphaRate( int iIdx );
	void UpdateAlphaRateByDuration( float fNewDuration );

public:
	void SetWorldTMInverse( const D3DXMATRIX &matInv );
	void SetTextureName( const char *szName );
	void SetLinkBipedName( const char *szName );
	void SetRenderType( FXRenderType eType );
	void SetTransformType( FXTransformType eType );
	void SetStartTime( float fStartTime );
	void SetDuration( float fDuration );

	inline const D3DXMATRIX& GetWorldTMInverse() const { return m_InvWorldTM; }
	inline const ioHashString& GetTextureName() const { return m_TextureName; }
	inline const ioHashString& GetLinkBipedName() const { return m_LinkBipedName; }
	inline FXRenderType GetRenderType() const { return m_RenderType; }
	inline FXTransformType GetTransformType() const { return m_TransformType; }
	inline float GetStartTime() const { return m_fStartTime; }
	inline float GetDuration() const { return m_fDuration; }
	inline float GetEndTime() const { return m_fStartTime + m_fDuration; }

	inline ioMeshData* GetMeshData() const { return m_pMeshData; }
	inline ioMaterial* GetMaterial() const { return m_pMtl; }

protected:
	void ApplyRenderTypeToMaterial( FXRenderType eType );

public:
	inline ioRcAniTrack* GetTrack() const { return m_pTrack; }
	inline bool IsAnimate() const
	{
		return ( m_pTrack != NULL );
	}

public:
	ioRcAnimatePlane();
	~ioRcAnimatePlane();
};

class __EX ioRcAnimateFX : public ioResource
{
protected:
	typedef std::vector< ioRcAnimatePlane* > RcAnimatePlaneList;
	RcAnimatePlaneList m_AnimatePlaneList;

	ioAxisAlignBox m_BoundBox;
	float m_fBoundRadius;

public:
	ioRcAnimatePlane* CreatePlane();
	ioRcAnimatePlane* GetPlane( int i );
	int GetPlaneCnt() const;

public:
	void SetBoundBox( const D3DXVECTOR3 &vMinPos, const D3DXVECTOR3 &vMaxPos );
	void SetBoundRadius( float fRadius );

	inline const ioAxisAlignBox& GetBoundBox() const { return m_BoundBox; }
	inline float GetBoundRadius() const { return m_fBoundRadius; }

public:
	ioRcAnimateFX( const ioHashString &name );
	virtual ~ioRcAnimateFX();
};

#endif