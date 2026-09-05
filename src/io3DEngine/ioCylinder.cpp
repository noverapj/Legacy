

#include "stdafx.h"
#include "ioCylinder.h"

ioCylinder::ioCylinder()
{
}

ioCylinder::ioCylinder( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fRadius )
{
	m_vCenter = ( v1 + v2 ) * FLOAT05;

	D3DXVECTOR3 vDiff = v2 - v1;
	m_fHeight = D3DXVec3Length( &vDiff );

	m_vDirection.x = vDiff.x / m_fHeight;
	m_vDirection.y = vDiff.y / m_fHeight;
	m_vDirection.z = vDiff.z / m_fHeight;

	m_fRadius = fRadius;
}

ioCylinder::ioCylinder( const D3DXVECTOR3 &vStart,
					    const D3DXVECTOR3 &vUnitDir,
					    float fHeight,
						float fRadius )
{
	m_vCenter = vStart + vUnitDir * ( FLOAT05 * fHeight );
	m_vDirection = vUnitDir;

	m_fHeight = fHeight;
	m_fRadius = fRadius;
}

ioCylinder::~ioCylinder()
{
}

ioSegment ioCylinder::GetSegment() const
{
	ioSegment kSegment;
	kSegment.SetDirection( m_fHeight * m_vDirection );
	kSegment.SetOrigin( m_vCenter - kSegment.GetDirection() * FLOAT05 );

	return kSegment;
}
