

#ifndef _ioParticle_h_
#define _ioParticle_h_

#include "ioMemoryPool.h"

class __EX ioParticle : public ioPoolObject< ioParticle >
{
public:
	D3DXVECTOR3	m_vPos;
	D3DXVECTOR3	m_vDir;
	float	m_fMoveSpeed;
	DWORD	m_dwColor;

	float	m_fCurTime;
	float	m_fLiveTime;

	float	m_fWidth;
	float	m_fHeight;

	float   m_fBeginWidth;
	float   m_fBeginHeight;

	// Radian
	float	m_fTexRotation;
	float	m_fTexRotationSpeed;

	D3DXQUATERNION m_qtRotate;
	float	m_fRotationSpeed;

public:
	void SetBeginSize( float fWidth, float fHeight );
	void SetSize( float fWidth, float fHeight );

	void SetTexRotation( float fRotateDeg );
	void SetTexRotationSpeed( float fSpeed );
	void UpdateTexRotation( float fTimePerSec );

	void SetRotate( const D3DXVECTOR3 &vAxis, float fAngle );
	void Rotate( const D3DXVECTOR3 &vAxis, float fAngle );

	void SetRotateSpeed( float fSpeed );

public:
	inline bool IsLive() const
	{
		if( m_fCurTime < m_fLiveTime )
			return true;

		return false;
	}

	inline float TimeRate() const
	{
		return m_fCurTime / m_fLiveTime;
	}

public:
	ioParticle();
	virtual ~ioParticle();
};

#endif
