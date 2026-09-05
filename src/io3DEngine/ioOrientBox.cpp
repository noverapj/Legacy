

#include "stdafx.h"

#include "ioMath.h"
#include "HelpFunc.h"
#include "ioAxisAlignBox.h"

#include "ioOrientBox.h"

ioOrientBox::ioOrientBox()
{
	m_vCenter  = ioMath::VEC3_ZERO;
	m_vAxis[0] = ioMath::UNIT_X;
	m_vAxis[1] = ioMath::UNIT_Y;
	m_vAxis[2] = ioMath::UNIT_Z;

	m_fExtent[0] = 0.0f;
	m_fExtent[1] = 0.0f;
	m_fExtent[2] = 0.0f;

	m_fBoxRadius = 0.0f;

	m_bNull = true;
	m_bNeedUpdateRadius = true;
}

ioOrientBox::~ioOrientBox()
{
}

void ioOrientBox::Transform( const D3DXMATRIX *pXForm )
{
	if( !IsNull() )
	{
		float fLength;
		D3DXVECTOR3 vAxis;
		for( int i=0 ; i<3 ; i++ )
		{
			D3DXVec3TransformNormal( &vAxis, &m_vAxis[i], pXForm );

			fLength = D3DXVec3Length( &vAxis );

			m_vAxis[i] = vAxis / fLength;
			m_fExtent[i] *= fLength;
		}

		D3DXVec3TransformNormal( &m_vCenter, &m_vCenter, pXForm );
		m_vCenter.x += pXForm->_41;
		m_vCenter.y += pXForm->_42;
		m_vCenter.z += pXForm->_43;

		NeedUpdateBoxRadiuse();
	}
}

void ioOrientBox::Transform( const D3DXVECTOR3 &vTranslate,
							 const D3DXVECTOR3 &vScale,
							 const D3DXQUATERNION &qtRotate )
{
	if( !IsNull() )
	{
		m_vCenter = qtRotate * ( m_vCenter * vScale ) + vTranslate;

		m_vAxis[0] = qtRotate * m_vAxis[0];
		m_vAxis[1] = qtRotate * m_vAxis[1];
		m_vAxis[2] = qtRotate * m_vAxis[2];

		m_fExtent[0] *= vScale.x;
		m_fExtent[1] *= vScale.y;
		m_fExtent[2] *= vScale.z;

		NeedUpdateBoxRadiuse();
	}
}

void ioOrientBox::SetNull()
{
	m_bNull = true;
}

void ioOrientBox::SetCenter( const D3DXVECTOR3 &vCenter )
{
	m_vCenter = vCenter;
	m_bNull = false;
}

void ioOrientBox::SetAxis( int i, const D3DXVECTOR3 &vAxis )
{
	m_vAxis[i] = vAxis;
	m_bNull = false;
}

void ioOrientBox::SetExtents( int i, float fExtent )
{
	m_fExtent[i] = fExtent;

	m_bNull = false;
	NeedUpdateBoxRadiuse();
}

void ioOrientBox::SetByAxisBox( const ioAxisAlignBox &rkAABB )
{
	SetBoxByMinMax( rkAABB.GetMinPos(), rkAABB.GetMaxPos() );
}

void ioOrientBox::SetBoxByMinMax( const D3DXVECTOR3 &vMin,
								  const D3DXVECTOR3 &vMax )
{
	m_vCenter = ( vMin + vMax ) * FLOAT05;

	m_vAxis[0] = ioMath::UNIT_X;
	m_vAxis[1] = ioMath::UNIT_Y;
	m_vAxis[2] = ioMath::UNIT_Z;

	m_fExtent[0] = ( vMax.x - vMin.x ) * FLOAT05;
	m_fExtent[1] = ( vMax.y - vMin.y ) * FLOAT05;
	m_fExtent[2] = ( vMax.z - vMin.z ) * FLOAT05;

	m_bNull = false;
	NeedUpdateBoxRadiuse();
}

void ioOrientBox::SetBoxMinMaxForAxis( const D3DXVECTOR3 &vMin,
									   const D3DXVECTOR3 &vMax )
{
	m_vCenter = ( vMin + vMax ) * FLOAT05;

	D3DXVECTOR3 vDiff = vMax - m_vCenter;
	m_fExtent[0] = fabs( D3DXVec3Dot( &m_vAxis[0], &vDiff ) );
	m_fExtent[1] = fabs( D3DXVec3Dot( &m_vAxis[1], &vDiff ) );
	m_fExtent[2] = fabs( D3DXVec3Dot( &m_vAxis[2], &vDiff ) );

	m_bNull = false;
	NeedUpdateBoxRadiuse();
}

void ioOrientBox::GetVertices( D3DXVECTOR3 *pArray ) const
{
	D3DXVECTOR3	vXAxis, vYAxis, vZAxis;
	vXAxis = m_vAxis[0] * m_fExtent[0];
	vYAxis = m_vAxis[1] * m_fExtent[1];
	vZAxis = m_vAxis[2] * m_fExtent[2];

	//UJ 130912, 최적화: 벡터 연산을 줄인다(3x8=24회 -> 12회)
	//pArray[0] = m_vCenter - vXAxis - vYAxis - vZAxis;
	//pArray[1] = m_vCenter + vXAxis - vYAxis - vZAxis;
	//pArray[2] = m_vCenter + vXAxis - vYAxis + vZAxis;
	//pArray[3] = m_vCenter - vXAxis - vYAxis + vZAxis;
	//pArray[4] = m_vCenter - vXAxis + vYAxis - vZAxis;
	//pArray[5] = m_vCenter + vXAxis + vYAxis - vZAxis;
	//pArray[6] = m_vCenter + vXAxis + vYAxis + vZAxis;
	//pArray[7] = m_vCenter - vXAxis + vYAxis + vZAxis;
	D3DXVECTOR3 vXYadd = vXAxis + vYAxis;
	D3DXVECTOR3 vXYsub = vXAxis - vYAxis;
	D3DXVECTOR3 vZCadd = m_vCenter + vZAxis;
	D3DXVECTOR3 vZCsub = m_vCenter - vZAxis;
	pArray[2] = vZCadd + vXYsub;
	pArray[3] = vZCadd - vXYadd;
	pArray[6] = vZCadd + vXYadd;
	pArray[7] = vZCadd - vXYsub;
	pArray[0] = vZCsub - vXYadd;
	pArray[1] = vZCsub + vXYsub;
	pArray[4] = vZCsub - vXYsub;
	pArray[5] = vZCsub + vXYadd;
}

D3DXMATRIX ioOrientBox::GetTransform() const
{
	D3DXMATRIX matXform;

	matXform._11 = m_vAxis[0].x;
	matXform._12 = m_vAxis[0].y;
	matXform._13 = m_vAxis[0].z;
	matXform._14 = 0.0f;

	matXform._21 = m_vAxis[1].x;
	matXform._22 = m_vAxis[1].y;
	matXform._23 = m_vAxis[1].z;
	matXform._24 = 0.0f;

	matXform._31 = m_vAxis[2].x;
	matXform._32 = m_vAxis[2].y;
	matXform._33 = m_vAxis[2].z;
	matXform._34 = 0.0f;

	matXform._41 = m_vCenter.x;
	matXform._42 = m_vCenter.y;
	matXform._43 = m_vCenter.z;
	matXform._44 = 1.0f;

	return matXform;
}

float ioOrientBox::GetBoxRadius() const
{
	if( m_bNeedUpdateRadius )
	{
		float fRadiusSq = m_fExtent[0] * m_fExtent[0] +
					  m_fExtent[1] * m_fExtent[1] + 
					  m_fExtent[2] * m_fExtent[2];

		m_fBoxRadius = sqrt( fRadiusSq );
		m_bNeedUpdateRadius = false;
	}

	return m_fBoxRadius;	
}

bool ioOrientBox::operator==( const ioOrientBox &rhs ) const
{
	if( !ioMath::IsEqual( m_vCenter, rhs.m_vCenter ) )
		return false;

	for( int i=0 ; i<3 ; i++ )
	{
		if( !ioMath::IsEqual( m_vAxis[i], rhs.m_vAxis[i] ) )
			return false;

		if( m_fExtent[i] != rhs.m_fExtent[i] )
			return false;
	}

	return true;
}

bool ioOrientBox::operator!=( const ioOrientBox &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}


