

#include "stdafx.h"

#include "ioMath.h"
#include "ioSphere.h"
#include "ioAxisAlignBox.h"
#include "ioRay.h"
#include "ErrorReport.h"

#include "ioCameraController.h"
#include "ioFPSCameraController.h"
#include "ioLookAtCameraController.h"

#include "ioCamera.h"

ioCamera::ioCamera()
{
	m_pController = NULL;

	m_fNearP = 100.0f;
	m_fFarP  = 10000.0f;
	m_fFov	 = 45.0f;
	m_fAspectRatio = 1.33333f;
	
	SetDistAlphaFactor( 1000000.0f, 1000001.0f );

	m_bRecalcFrustum = true;
	m_bNeedReCalcMatrix = true;
}

ioCamera::~ioCamera()
{
	SAFEDELETE( m_pController );
}

void ioCamera::SetProjection( float fNear, float fFar, float fFov, float fAspect )
{
	m_fNearP = fNear;
	m_fFarP	 = fFar;
	m_fFov	 = fFov;
	m_fAspectRatio = fAspect;

	m_bRecalcFrustum = true;
	UpdateFrustum();
}

void ioCamera::SetNearPlaneDist( float fNearP )
{
	m_fNearP = fNearP;

	m_bRecalcFrustum = true;
}

void ioCamera::SetFarPlaneDist( float fFarP )
{
	m_fFarP = fFarP;

	m_bRecalcFrustum = true;
}

void ioCamera::SetFov( float fFovDeg )
{
	m_fFov = fFovDeg;

	m_bRecalcFrustum = true;
}

void ioCamera::SetAspectRatio( float fAspect )
{
	m_fAspectRatio = fAspect;

	m_bRecalcFrustum = true;
}

void ioCamera::SetPosition( const D3DXVECTOR3 &vPos )
{
	if( m_pController )
	{
		m_pController->SetPosition( vPos );
		m_bNeedReCalcMatrix = true;
	}
}

void ioCamera::SetDirection( float fHead, float fPitch, float fRoll )
{
	if( m_pController )
	{
		m_pController->SetDirection( fHead, fPitch, fRoll );
		m_bNeedReCalcMatrix = true;
	}
}

void ioCamera::SetCameraRoll( float fRoll )
{
	if( m_pController )
	{
		m_pController->SetRollValue( fRoll );
		m_bNeedReCalcMatrix = true;
	}
}

void ioCamera::SetController( ioCameraController *pControl )
{
	SAFEDELETE( m_pController );

	m_pController = pControl;

	m_bRecalcFrustum = true;
	m_bNeedReCalcMatrix = true;
}

void ioCamera::Update( ioCameraInput *pInput )
{
	if( m_pController )
	{
		m_pController->Update( pInput );
		m_bNeedReCalcMatrix = true;
	}

	UpdateFrustum();
	UpdateCullPlane();
}

void ioCamera::UpdateFrustum()
{
	if( !m_bRecalcFrustum )	return;

	D3DXMatrixPerspectiveFovLH( &m_matProj,
								 DEGtoRAD( m_fFov ),
								 m_fAspectRatio,
								 m_fNearP,
								 m_fFarP );

	float fThetaY = DEGtoRAD( m_fFov / FLOAT05 );
	float fTanThetaY = tan( fThetaY );
	float fTanThetaX = fTanThetaY * m_fAspectRatio;

	float vpTop    = fTanThetaY * m_fNearP;
	float vpRight  = fTanThetaX * m_fNearP;
	float vpBottom = -vpTop;
	float vpLeft   = -vpRight;

	float fNSqr = m_fNearP * m_fNearP;
	float fLSqr = vpRight * vpRight;
	float fRSqr = fLSqr;
	float fTSqr = vpTop * vpTop;
	float fBSqr = fTSqr;

	float fInvLength = 1.0f / sqrt( fNSqr + fLSqr );
	m_fCoEffLeft[0] = m_fNearP * fInvLength;
	m_fCoEffLeft[1] = -vpLeft * fInvLength;

	fInvLength = 1.0f / sqrt( fNSqr + fRSqr );
	m_fCoEffRight[0] = -m_fNearP * fInvLength;
	m_fCoEffRight[1] = vpRight * fInvLength;

	fInvLength = 1.0f / sqrt( fNSqr + fBSqr );
	m_fCoEffBottom[0] = m_fNearP * fInvLength;
	m_fCoEffBottom[1] = -vpBottom * fInvLength;

	fInvLength = 1.0f / sqrt( fNSqr + fTSqr );
	m_fCoEffTop[0] = -m_fNearP * fInvLength;
	m_fCoEffTop[1] = vpTop * fInvLength;

	m_bRecalcFrustum = false;
}

void ioCamera::UpdateCullPlane()
{
	D3DXMATRIX matInvViewProj;
	D3DXMatrixMultiply( &matInvViewProj, GetViewMatrix(), GetProjMatrix() );
	D3DXMatrixInverse( &matInvViewProj, NULL, &matInvViewProj );

	D3DXVECTOR3 vFrustum[8];
	vFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
    vFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
    vFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
    vFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
    vFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
    vFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
    vFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
    vFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

	D3DXVec3TransformCoordArray( vFrustum,
								 sizeof(D3DXVECTOR3),
								 vFrustum,
								 sizeof(D3DXVECTOR3),
								 &matInvViewProj,
								 8 );

	// Left
	m_kFrustumPlane[CAM_LEFT_PLANE].SetPlane( vFrustum[2], vFrustum[6], vFrustum[4] );
	
	// Right
	m_kFrustumPlane[CAM_RIGHT_PLANE].SetPlane( vFrustum[7], vFrustum[3], vFrustum[5] );

	// Top
	m_kFrustumPlane[CAM_TOP_PLANE].SetPlane( vFrustum[2], vFrustum[3], vFrustum[6] );

	// Bottom
	m_kFrustumPlane[CAM_BOTTOM_PLANE].SetPlane( vFrustum[1], vFrustum[0], vFrustum[4] );

	// Near
	m_kFrustumPlane[CAM_NEAR_PLANE].SetPlane( vFrustum[0], vFrustum[1], vFrustum[2] );

	// Far
	m_kFrustumPlane[CAM_FAR_PLANE].SetPlane( vFrustum[6], vFrustum[7], vFrustum[5] );
}

bool ioCamera::IsVisible( const ioSphere &kSphere ) const
{
	for( int i=0 ; i<CAM_FRUSTUM_PLANES ; i++ )
	{
		if( m_kFrustumPlane[i].GetDistance( kSphere.GetCenter() ) < -kSphere.GetRadius() )
			return false;
	}

	return true;
}

bool ioCamera::IsVisible( const ioAxisAlignBox &rkAABB ) const
{
	if( rkAABB.IsNull() )	return false;

	D3DXVECTOR3 vMin, vMax;
	vMin = rkAABB.GetMinPos();
	vMax = rkAABB.GetMaxPos();

	for( int i=0 ; i<CAM_FRUSTUM_PLANES ; i++ )
	{
		if( m_kFrustumPlane[i].GetSide( vMin, vMax ) == ioPlane::NEGATIVE_SIDE )
			return false;
	}

	return true;
}

bool ioCamera::IsVisible( const ioCylinder &kCylinder ) const
{
	for( int i=0 ; i<CAM_FRUSTUM_PLANES ; i++ )
	{
		if( ioMath::IsCulled( m_kFrustumPlane[i], kCylinder ) )
			return false;
	}

	return true;
}

const D3DXMATRIX* ioCamera::GetViewMatrix() const
{
	if( m_bNeedReCalcMatrix && m_pController )
	{
		m_pController->CreateViewMatrix( &m_matView );
		D3DXMatrixInverse( &m_matInvView, NULL, &m_matView );

		m_bNeedReCalcMatrix = false;
	}

	return &m_matView;
}

const D3DXMATRIX* ioCamera::GetViewInverseMatrix() const
{
	if( m_bNeedReCalcMatrix )
	{
		GetViewMatrix();
	}
	
	return &m_matInvView;
}

const D3DXMATRIX* ioCamera::GetProjMatrix() const
{
	return &m_matProj;
}

D3DXVECTOR3 ioCamera::GetPosition() const
{
	if( m_pController )
		return m_pController->GetPosition();

	return ioMath::VEC3_ZERO;
}

D3DXVECTOR3 ioCamera::GetDirection() const
{
	if( m_pController )
		return m_pController->GetDirection();

	return ioMath::UNIT_Z;
}

const char* ioCamera::GetControllerName() const
{
	if( m_pController )
		return m_pController->GetName();

	return "Not Initialized";
}

void ioCamera::GetExtraInfoText( char *szBuf, int iLen )
{
	if( m_pController )
	{
		m_pController->GetExtraInfoText( szBuf, iLen );
	}
}

float ioCamera::GetHead() const
{
	if( m_pController )
		return m_pController->GetHead();

	return 0.0f;
}

float ioCamera::GetPitch() const
{
	if( m_pController )
		return m_pController->GetPitch();

	return 0.0f;
}

float ioCamera::GetRoll() const
{
	if( m_pController )
		return m_pController->GetRoll();

	return 0.0f;
}

void ioCamera::GetViewVolumnWorldVtx( Vector3Vec &vVtxList )
{
	float fFactX = (float)tan( DEGtoRAD( GetFovX() ) / 2.0f );
	float fFactY = (float)tan( DEGtoRAD( GetFovY() ) / 2.0f );

	D3DXVECTOR3 vVertex;
	const D3DXMATRIX *pViewInv = GetViewInverseMatrix();

	// Near Left Down
	vVertex.x = -m_fNearP * fFactX;
	vVertex.y = -m_fNearP * fFactY;
	vVertex.z = m_fNearP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Near Right Down
	vVertex.x = m_fNearP * fFactX;
	vVertex.y = -m_fNearP * fFactY;
	vVertex.z = m_fNearP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Near Right Up
	vVertex.x = m_fNearP * fFactX;
	vVertex.y = m_fNearP * fFactY;
	vVertex.z = m_fNearP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Near Left Up
	vVertex.x = -m_fNearP * fFactX;
	vVertex.y = m_fNearP * fFactY;
	vVertex.z = m_fNearP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Far Left Down
	vVertex.x = -m_fFarP * fFactX;
	vVertex.y = -m_fFarP * fFactY;
	vVertex.z = m_fFarP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Far Right Down
	vVertex.x = m_fFarP * fFactX;
	vVertex.y = -m_fFarP * fFactY;
	vVertex.z = m_fFarP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Far Right Up
	vVertex.x = m_fFarP * fFactX;
	vVertex.y = m_fFarP * fFactY;
	vVertex.z = m_fFarP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );

	// Far Left Up
	vVertex.x = -m_fFarP * fFactX;
	vVertex.y = m_fFarP * fFactY;
	vVertex.z = m_fFarP;
	D3DXVec3TransformCoord( &vVertex, &vVertex, pViewInv );
	vVtxList.push_back( vVertex );
}

void ioCamera::GetViewVolumnPlaneList( std::vector< ioPlane > &rkList )
{
	rkList.reserve( CAM_FRUSTUM_PLANES );

	for( int i=0 ; i<CAM_FRUSTUM_PLANES ; i++ )
	{
		rkList.push_back( m_kFrustumPlane[i] );
	}
}

void ioCamera::SetDistAlphaFactor( float fStart, float fEnd )
{
	m_fDistAlphaStart = max( 0.0f, min( fStart, fEnd-1.0f ) );
	m_fDistAlphaEnd   = fEnd;

	m_vDistAlphaFactor.x = m_fDistAlphaEnd;
	m_vDistAlphaFactor.y = 1.0f / ( m_fDistAlphaEnd - m_fDistAlphaStart );
}

