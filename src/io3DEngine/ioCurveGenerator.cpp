

#include "stdafx.h"

#include "ioCurveGenerator.h"

static D3DXMATRIX matHermite( 2.f,-2.f, 1.f, 1.f,
							 -3.f, 3.f,-2.f,-1.f,
							  0.f, 0.f, 1.f, 0.f,
							  1.f, 0.f, 0.f, 0.f );

D3DXVECTOR3 GetPositionOnCubic( const D3DXVECTOR3 &vStart,
							    const D3DXVECTOR3 &vStartVel,
								const D3DXVECTOR3 &vEnd,
								const D3DXVECTOR3 &vEndVel,
								float fTime )
{
	D3DXMATRIX m;

	m._11 = vStart.x,	 m._12 = vStart.y,	  m._13 = vStart.z,		m._14 = 1.0f;
	m._21 = vEnd.x,		 m._22 = vEnd.y,	  m._23 = vEnd.z,		m._24 = 1.0f;
	m._31 = vStartVel.x, m._32 = vStartVel.y, m._33 = vStartVel.z,	m._34 = 1.0f;
	m._41 = vEndVel.x,	 m._42 = vEndVel.y,	  m._43 = vEndVel.z,	m._44 = 1.0f;

	D3DXVECTOR4 vPower( fTime * fTime * fTime, fTime * fTime, fTime, 1.0f );
	D3DXVec4Transform( &vPower, &vPower, &matHermite );
	D3DXVec4Transform( &vPower, &vPower, &m );

	return D3DXVECTOR3( vPower.x, vPower.y, vPower.z );
}

ioNoneUniformSpline::ioNoneUniformSpline()
{
	m_fMaxDistance = 0.0f;
}

ioNoneUniformSpline::~ioNoneUniformSpline()
{
}

void ioNoneUniformSpline::InitBuffer( int iNeedSize )
{
	m_SplineList.erase( m_SplineList.begin(), m_SplineList.end() );
	m_SplineList.reserve( iNeedSize );
	m_fMaxDistance = 0.0f;
}

void ioNoneUniformSpline::BuildSpline()
{
	int iEndCount = m_SplineList.size() - 1;

	D3DXVECTOR3 vDiff1, vDiff2, vVelocity;
	for( int i=1 ; i<iEndCount ; i++ )
	{
		vDiff1 = m_SplineList[i+1].vPos - m_SplineList[i].vPos;
		vDiff2 = m_SplineList[i-1].vPos - m_SplineList[i].vPos;

		D3DXVec3Normalize( &vDiff1, &vDiff1 );
		D3DXVec3Normalize( &vDiff2, &vDiff2 );

		vVelocity = vDiff1 - vDiff2;
		D3DXVec3Normalize( &m_SplineList[i].vVelocity, &vVelocity );
	}

	m_SplineList[0].vVelocity = GetStartVelocity(0);
	m_SplineList.back().vVelocity = GetEndVelocity( iEndCount );
}

void ioNoneUniformSpline::AddPoint( const D3DXVECTOR3 &vPos )
{
	if( m_SplineList.empty() )
	{
		m_fMaxDistance = 0.0f;
	}
	else
	{
		float fCurDistance = D3DXVec3Length( &( m_SplineList.back().vPos - vPos ) );

		m_SplineList.back().fDistance = fCurDistance;
		m_fMaxDistance += fCurDistance;
	}

	SplineData kData;
	kData.vPos = vPos;

	m_SplineList.push_back( kData );
}

D3DXVECTOR3 ioNoneUniformSpline::GetStartVelocity( int iIndex )
{
	D3DXVECTOR3 vTemp( 0.0f, 0.0f, 0.0f );

	if( m_SplineList[iIndex].fDistance > 0.0f )
	{
		vTemp = m_SplineList[iIndex+1].vPos - m_SplineList[iIndex].vPos;
		vTemp *= 3.0f / m_SplineList[iIndex].fDistance;
	}

	return ( vTemp - m_SplineList[iIndex+1].vVelocity ) * FLOAT05;
}

D3DXVECTOR3 ioNoneUniformSpline::GetEndVelocity( int iIndex )
{
	D3DXVECTOR3 vTemp( 0.0f, 0.0f, 0.0f );

	if( m_SplineList[iIndex-1].fDistance > 0.0f )
	{
		vTemp = m_SplineList[iIndex].vPos - m_SplineList[iIndex-1].vPos;
		vTemp *= 3.0f / m_SplineList[iIndex-1].fDistance;
	}

	return ( vTemp - m_SplineList[iIndex-1].vVelocity ) * FLOAT05;
}

D3DXVECTOR3 ioNoneUniformSpline::GetPosition( float fTime ) const
{
	if( fTime <= 0.0f )
		return m_SplineList[0].vPos;
	else if( fTime >= 1.0f )
		return m_SplineList.back().vPos;

	float fCurDistance = 0.0f;
	float fDistance = fTime * m_fMaxDistance;

	int iNodeCount = m_SplineList.size() - 1;
	for( int i=0 ; i<iNodeCount; i++ )
	{
		if( fCurDistance + m_SplineList[i].fDistance < fDistance )
		{
			fCurDistance += m_SplineList[i].fDistance;
		}
		else
			break;
	}

	float fT = ( fDistance - fCurDistance ) / m_SplineList[i].fDistance;
	D3DXVECTOR3 vStart = m_SplineList[i].vVelocity * m_SplineList[i].fDistance;
	D3DXVECTOR3 vEnd = m_SplineList[i+1].vVelocity * m_SplineList[i].fDistance;

	return GetPositionOnCubic( m_SplineList[i].vPos, vStart, 
							   m_SplineList[i+1].vPos, vEnd, fT );
}

