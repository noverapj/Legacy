

#include "stdafx.h"

#include "ioParticle.h"

#include "ioStringInterface.h"
#include "ioMovableObject.h"
#include "ioRenderable.h"

#include "ioParticleSystem.h"

ioParticle::ioParticle()
{
	m_vPos.x = m_vPos.y = m_vPos.z = 0.0f;
	m_vDir.x = m_vDir.y = m_vDir.z = 0.0f;
	m_fMoveSpeed = 0.0f;
	m_dwColor	= 0xffffffff;

	m_fCurTime  = 0.0f;
	m_fLiveTime = 0.0f;

	m_fWidth	= 0.0f;
	m_fHeight	= 0.0f;

	m_fBeginWidth  = 100.0f;
	m_fBeginHeight = 100.0f;

	m_fTexRotation = 0.0f;
	m_fTexRotationSpeed = 0.0f;

	m_fRotationSpeed = 0.0f;
}

ioParticle::~ioParticle()
{
}

void ioParticle::SetBeginSize( float fWidth, float fHeight )
{
	m_fBeginWidth  = m_fWidth  = fWidth;
	m_fBeginHeight = m_fHeight = fHeight;
}

void ioParticle::SetSize( float fWidth, float fHeight )
{
	m_fWidth  = fWidth;
	m_fHeight = fHeight;
}

void ioParticle::SetTexRotation( float fRotateDeg )
{
	m_fTexRotation = DEGtoRAD( fRotateDeg );
}

void ioParticle::SetTexRotationSpeed( float fSpeed )
{
	m_fTexRotationSpeed = fSpeed;
}

void ioParticle::UpdateTexRotation( float fTimePerSec )
{
	m_fTexRotation += m_fTexRotationSpeed * fTimePerSec;
}

void ioParticle::SetRotateSpeed( float fSpeed )
{
	m_fRotationSpeed = fSpeed;
}

void ioParticle::SetRotate( const D3DXVECTOR3 &vAxis, float fAngle )
{
	D3DXQuaternionRotationAxis( &m_qtRotate, &vAxis, fAngle );
}

void ioParticle::Rotate( const D3DXVECTOR3 &vAxis, float fAngle )
{
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, fAngle );

	D3DXQuaternionMultiply( &m_qtRotate, &m_qtRotate, &qtRot );
}
