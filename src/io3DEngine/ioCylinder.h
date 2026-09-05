

#ifndef _ioCylinder_h_
#define _ioCylinder_h_

#include "ioSegment.h"

// Cylinder line segment has end points C-(H/2)*D and C+(H/2)*D where
// D is a unit-length vector.  H is infinity for infinite cylinder.

class __EX ioCylinder
{
private:
	D3DXVECTOR3 m_vCenter;
	D3DXVECTOR3 m_vDirection;
	float	m_fHeight;
	float	m_fRadius;

public:
	inline void SetCenter( const D3DXVECTOR3 &vCenter ) { m_vCenter = vCenter; }
	inline const D3DXVECTOR3& GetCenter() const { return m_vCenter; }

	inline void SetDirection( const D3DXVECTOR3 &vDir ) { m_vDirection = vDir; }
	inline const D3DXVECTOR3& GetDirection() const { return m_vDirection; }

	inline void SetHeight( float fHeight ) { m_fHeight = fHeight; }
	inline float GetHeight() const { return m_fHeight; }

	inline void SetRadius( float fRadius ) { m_fRadius = fRadius; }
	inline float GetRadius() const { return m_fRadius; }

public:
	ioSegment GetSegment() const;

public:
	ioCylinder();
	ioCylinder( const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2, float fRadius );
	ioCylinder( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vUnitDir, float fHeight, float fRadius );
	~ioCylinder();
};

#endif
