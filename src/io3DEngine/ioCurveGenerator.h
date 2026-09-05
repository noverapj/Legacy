

#ifndef _ioCurveGenerator_h_
#define _ioCurveGenerator_h_

#include "io3DCommon.h"

// GPG4 2.4 RNS( rounded noneuniform spline )
class __EX ioNoneUniformSpline
{
private:
	struct SplineData
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vVelocity;
		float fDistance;
	};

	typedef std::vector< SplineData > SplineDataList;
	SplineDataList m_SplineList;

	float m_fMaxDistance;

public:
	void InitBuffer( int iNeedSize );

public:
	void AddPoint( const D3DXVECTOR3 &vPos );
	void BuildSpline();

private:	
	D3DXVECTOR3 GetStartVelocity( int iIndex );
	D3DXVECTOR3 GetEndVelocity( int iIndex );

public:
	D3DXVECTOR3 GetPosition( float fTime ) const;

public:
	ioNoneUniformSpline();
	~ioNoneUniformSpline();
};

#endif
