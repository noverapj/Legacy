

#ifndef _ioPlane_h_
#define _ioPlane_h_

#include "ioMath.h"

class __EX ioPlane
{
public:
	enum Side
	{
		NO_SIDE,
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
	};

private:
	D3DXPLANE	m_Plane;

public:
	void SetPlane( float a, float b, float c, float d );
	void SetPlane( const D3DXVECTOR3 &vNrm, float fDist );
	void SetPlane( const D3DXVECTOR3 &vNrm, const D3DXVECTOR3 &vPoint );
	void SetPlane( const D3DXVECTOR3 &vPoint1,
				   const D3DXVECTOR3 &vPoint2,
				   const D3DXVECTOR3 &vPoint3 );

	void Transform( const D3DXMATRIX &mat );

public:
	inline float GetPlaneDotNormal( const D3DXVECTOR3 &vDir ) const
	{
		return D3DXPlaneDotNormal( &m_Plane, &vDir );
	}

	inline float GetDistance( const D3DXVECTOR3 &vPoint ) const
	{
		return D3DXPlaneDotCoord( &m_Plane, &vPoint );
	}

	Side GetSide( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax ) const;
	Side GetSide( const D3DXVECTOR3 &vPoint ) const;
	
	inline bool PlaneNegativeSide( const D3DXVECTOR3 &vPt, float fEpsilon = ALMOST_ZERO ) const
	{
		return GetDistance( vPt ) < fEpsilon;
	}

	inline bool PlanePositiveSide( const D3DXVECTOR3 &vPt, float fEpsilon = ALMOST_ZERO ) const
	{
		return GetDistance( vPt ) > fEpsilon;
	}

public:
	inline const D3DXPLANE* GetPlane() const { return &m_Plane; }
	inline float GetD() const { return m_Plane.d; }
	
	inline D3DXVECTOR3 GetNormal() const
	{
		return D3DXVECTOR3( m_Plane.a, m_Plane.b, m_Plane.c );
	}

public:
	ioPlane();
	ioPlane( const ioPlane &rhs );
	ioPlane( const D3DXVECTOR3 &vNormal, float fConstant );
	ioPlane( const D3DXVECTOR3 &vNormal, const D3DXVECTOR3 &vPoint );
	ioPlane( const D3DXVECTOR3 &vPoint1,
			 const D3DXVECTOR3 &vPoint2,
			 const D3DXVECTOR3 &vPoint3 );
	
	~ioPlane();
};

#endif
