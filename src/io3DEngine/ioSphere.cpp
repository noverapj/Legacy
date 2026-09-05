

#include "stdafx.h"

#include "ioSphere.h"
#include "HelpFunc.h"

ioSphere::ioSphere() : m_vCenter( 0.0f, 0.0f, 0.0f )
{
	m_fRadius = 1.0f;
}

ioSphere::ioSphere( const D3DXVECTOR3 &vCenter, float fRadius ) : m_vCenter(vCenter)
{
	m_fRadius = fRadius;
}

ioSphere::~ioSphere()
{
}

void ioSphere::SetRadius( float fRadius )
{
	m_fRadius = fRadius;
}

void ioSphere::SetCenter( const D3DXVECTOR3 &vCenter )
{
	m_vCenter = vCenter;
}

void ioSphere::Transform( const D3DXVECTOR3 &vTranslate,
						  const D3DXVECTOR3 &vScale,
						  const D3DXQUATERNION &qtRotate )
{
	m_vCenter = qtRotate * ( m_vCenter * vScale ) + vTranslate;
	m_fRadius *= max( vScale.x, max( vScale.y, vScale.z ) );
}
