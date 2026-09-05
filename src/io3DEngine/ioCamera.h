

#ifndef _ioCamera_h_
#define _ioCamera_h_

#include "io3DCommon.h"
#include "ioPlane.h"

class ioCameraInput;
class ioCameraController;

class ioSphere;
class ioAxisAlignBox;
class ioOrientBox;
class ioCylinder;

class __EX ioCamera
{
private:
	ioCameraController *m_pController;

	mutable D3DXMATRIX m_matView;
	mutable D3DXMATRIX m_matInvView;

	D3DXMATRIX m_matProj;
	float	m_fNearP;
	float	m_fFarP;
	float	m_fFov;
	float	m_fAspectRatio;

	float	m_fCoEffLeft[2];
	float	m_fCoEffRight[2];
	float	m_fCoEffBottom[2];
	float	m_fCoEffTop[2];

	float	m_fDistAlphaStart;
	float	m_fDistAlphaEnd;
	D3DXVECTOR2 m_vDistAlphaFactor;

	enum
	{
		CAM_LEFT_PLANE	 = 0,
        CAM_RIGHT_PLANE  = 1,
        CAM_BOTTOM_PLANE = 2,
        CAM_TOP_PLANE	 = 3,
        CAM_FAR_PLANE	 = 4,
        CAM_NEAR_PLANE	 = 5,
        CAM_FRUSTUM_PLANES = 6,
	};

	// Normal이 Frustum 안쪽으로 향하는 평면으로 구성됨
	ioPlane	m_kFrustumPlane[CAM_FRUSTUM_PLANES];

	bool	m_bRecalcFrustum;
	mutable bool m_bNeedReCalcMatrix;

public:
	void SetController( ioCameraController *pControl );
	void SetPosition( const D3DXVECTOR3 &vPos );
	void SetDirection( float fHead, float fPitch, float fRoll );
	void SetCameraRoll( float fRoll );

public:
	void SetProjection( float fNear, float fFar, float fFov, float fAspect );
	void SetNearPlaneDist( float fNearP );
	void SetFarPlaneDist( float fFarP );
	void SetFov( float fFovDeg );
	void SetAspectRatio( float fAspect );

	void SetDistAlphaFactor( float fStart, float fEnd );

public:
	inline float GetNearP() const { return m_fNearP; }
	inline float GetFarP() const { return m_fFarP; }
	inline float GetNearFarGap() const { return m_fFarP - m_fNearP; }

	inline float GetFovY() const { return m_fFov; }
	inline float GetFovX() const { return m_fFov * m_fAspectRatio; }

	float GetHead() const;
	float GetPitch() const;
	float GetRoll() const;

	void GetViewVolumnWorldVtx( Vector3Vec &vVtxList );
	void GetViewVolumnPlaneList( std::vector< ioPlane > &rkList );

	inline float GetDistAlphaStart() const { return m_fDistAlphaStart; }
	inline float GetDistAlphaEnd() const { return m_fDistAlphaEnd; }
	inline const D3DXVECTOR2& GetDistAlphaFactor() const {return m_vDistAlphaFactor; }

	inline ioCameraController* GetController() const { return m_pController; }

public:
	void Update( ioCameraInput *pInput );

private:
	void UpdateFrustum();
	void UpdateCullPlane();

public:
	bool IsVisible( const ioSphere &kSphere ) const;
	bool IsVisible( const ioAxisAlignBox &rkAABB ) const;
	bool IsVisible( const ioCylinder &kCylinder ) const;

public:
	const D3DXMATRIX* GetViewMatrix() const;
	const D3DXMATRIX* GetViewInverseMatrix() const;
	const D3DXMATRIX* GetProjMatrix() const;

	D3DXVECTOR3 GetPosition() const;
	D3DXVECTOR3 GetDirection() const;

public:
	void GetExtraInfoText( char *szBuf, int iLen );
	const char* GetControllerName() const;

public:
	ioCamera();
	~ioCamera();
};

#endif