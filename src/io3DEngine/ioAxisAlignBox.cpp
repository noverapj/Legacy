

#include "stdafx.h"

#include "HelpFunc.h"
#include "ioAxisAlignBox.h"
#include "ioOrientBox.h"
#include "ioMath.h"

ioAxisAlignBox::ioAxisAlignBox()
{
	m_vMinPos = m_vCenter = m_vMaxPos = ioMath::VEC3_ZERO;
	m_bNull = true;
}

ioAxisAlignBox::ioAxisAlignBox( const ioOrientBox &rkOBB )
{
	SetByOrientBox( rkOBB );
}

ioAxisAlignBox::ioAxisAlignBox( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax )
{
	SetMinMaxPos( vMin, vMax );
}

ioAxisAlignBox::~ioAxisAlignBox()
{
}

void ioAxisAlignBox::SetMinPos( const D3DXVECTOR3 &vMin )
{
	m_bNull = false;
	m_vMinPos = vMin;

	UpdateCenter();
}

void ioAxisAlignBox::SetMaxPos( const D3DXVECTOR3 &vMax )
{
	m_bNull = false;
	m_vMaxPos = vMax;

	UpdateCenter();
}

void ioAxisAlignBox::SetMinMaxPos( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax )
{
	m_bNull = false;
	m_vMinPos = vMin;
	m_vMaxPos = vMax;

	UpdateCenter();
}

void ioAxisAlignBox::SetMinMaxPos( const Vector3Vec &vVtxList, const D3DXMATRIX *pXform )
{
	if( vVtxList.empty() )	return;

	D3DXVECTOR3 vXformed;
	D3DXVec3TransformCoord( &vXformed, &vVtxList[0], pXform );

	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vXformed;

	int iVtxCnt = vVtxList.size();
	for( int i=1 ; i<iVtxCnt ; i++ )
	{
		D3DXVec3TransformCoord( &vXformed, &vVtxList[i], pXform );

		for( int j=0 ; j<3 ; j++ )
		{
			if( vMin[j] > vXformed[j] )
				vMin[j] = vXformed[j];

			if( vMax[j] < vXformed[j] )
				vMax[j] = vXformed[j];
		}
	}

	SetMinMaxPos( vMin, vMax );
}

void ioAxisAlignBox::SetByOrientBox( const ioOrientBox &rkOBB )
{
	D3DXVECTOR3 vVtxArray[8];
	rkOBB.GetVertices( vVtxArray );

	D3DXVECTOR3 vMin, vMax;
	ioMath::CalcMinMaxPoint( vMin, vMax, vVtxArray, 8 );

	SetMinMaxPos( vMin, vMax );
}

void ioAxisAlignBox::AddPoint( const D3DXVECTOR3 &vPoint )
{
	if( IsNull() )
	{
		m_bNull = false;
		m_vMinPos = vPoint;
		m_vMaxPos = vPoint;
	}
	else
	{
		m_vMinPos.x = min( m_vMinPos.x, vPoint.x );
		m_vMinPos.y = min( m_vMinPos.y, vPoint.y );
		m_vMinPos.z = min( m_vMinPos.z, vPoint.z );

		m_vMaxPos.x = max( m_vMaxPos.x, vPoint.x );
		m_vMaxPos.y = max( m_vMaxPos.y, vPoint.y );
		m_vMaxPos.z = max( m_vMaxPos.z, vPoint.z );
	}

	UpdateCenter();
}

void ioAxisAlignBox::UpdateCenter()
{
	m_vCenter = ( m_vMinPos + m_vMaxPos ) * FLOAT05;
	m_fHalfDiagnal = D3DXVec3Length( &(m_vMaxPos - m_vMinPos) ) * FLOAT05;
}

void ioAxisAlignBox::Merge( const ioAxisAlignBox &rhs )
{
	if( !rhs.IsNull() )
	{
		Merge( rhs.GetMinPos(), rhs.GetMaxPos() );
	}
}

void ioAxisAlignBox::Merge( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax )
{
	if( !IsNull() )
	{
		m_vMinPos.x = min( m_vMinPos.x, vMin.x );
		m_vMinPos.y = min( m_vMinPos.y, vMin.y );
		m_vMinPos.z = min( m_vMinPos.z, vMin.z );

		m_vMaxPos.x = max( m_vMaxPos.x, vMax.x );
		m_vMaxPos.y = max( m_vMaxPos.y, vMax.y );
		m_vMaxPos.z = max( m_vMaxPos.z, vMax.z );
		UpdateCenter();
	}
	else
	{
		SetMinMaxPos( vMin, vMax );
	}
}

void ioAxisAlignBox::Translate( const D3DXVECTOR3 &vMove )
{
	SetMinMaxPos( m_vMinPos + vMove, m_vMaxPos + vMove );
}

void ioAxisAlignBox::Transform( const D3DXMATRIX *pMat )
{
	D3DXVECTOR3 vVtxArray[8];
	GetVertices( vVtxArray );

	D3DXVECTOR3 vMin( 3.3e33f, 3.3e33f, 3.3e33f );
	D3DXVECTOR3 vMax( -3.3e33f, -3.3e33f, -3.3e33f );

	D3DXVECTOR3 vTemp;
	for( int i=0 ; i<8 ; i++ )
	{
		D3DXVec3TransformCoord( &vTemp, &vVtxArray[i], pMat );

		vMin.x = min( vMin.x, vTemp.x );
		vMin.y = min( vMin.y, vTemp.y );
		vMin.z = min( vMin.z, vTemp.z );

		vMax.x = max( vMax.x, vTemp.x );
		vMax.y = max( vMax.y, vTemp.y );
		vMax.z = max( vMax.z, vTemp.z );
	}

	SetMinMaxPos( vMin, vMax );
}

void ioAxisAlignBox::Transform( const D3DXVECTOR3 &vPosition,
								const D3DXVECTOR3 &vScale,
								const D3DXQUATERNION &qtOrientation )
{
	m_vCenter = qtOrientation * ( m_vCenter * vScale );
	m_vCenter += vPosition;

	D3DXVECTOR3 vDiff = ( m_vMaxPos - m_vMinPos ) * FLOAT05;
	vDiff = vDiff * vScale;

	SetMinMaxPos( m_vCenter - vDiff, m_vCenter + vDiff );
}

void ioAxisAlignBox::GetVertices( D3DXVECTOR3 vPos[8] ) const
{
	vPos[0].x = m_vMinPos.x;	vPos[0].y = m_vMinPos.y;	vPos[0].z = m_vMinPos.z;
	vPos[1].x = m_vMaxPos.x;	vPos[1].y = m_vMinPos.y;	vPos[1].z = m_vMinPos.z;
	vPos[2].x = m_vMaxPos.x;	vPos[2].y = m_vMinPos.y;	vPos[2].z = m_vMaxPos.z;
	vPos[3].x = m_vMinPos.x;	vPos[3].y = m_vMinPos.y;	vPos[3].z = m_vMaxPos.z;
	vPos[4].x = m_vMinPos.x;	vPos[4].y = m_vMaxPos.y;	vPos[4].z = m_vMinPos.z;
	vPos[5].x = m_vMaxPos.x;	vPos[5].y = m_vMaxPos.y;	vPos[5].z = m_vMinPos.z;
	vPos[6].x = m_vMaxPos.x;	vPos[6].y = m_vMaxPos.y;	vPos[6].z = m_vMaxPos.z;
	vPos[7].x = m_vMinPos.x;	vPos[7].y = m_vMaxPos.y;	vPos[7].z = m_vMaxPos.z;
}

float ioAxisAlignBox::GetExtents( int i ) const
{
	if( COMPARE( i, 0, 3 ) )
	{
		return ( m_vMaxPos[i] - m_vMinPos[i] ) * FLOAT05;
	}

	return 0.0f;
}

float ioAxisAlignBox::GetVolume() const
{
	D3DXVECTOR3 vSize = GetSize();

	return vSize.x * vSize.y * vSize.z;
}