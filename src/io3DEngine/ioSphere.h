

#ifndef _ioSphere_h_
#define _ioSphere_h_

class __EX ioSphere
{
private:
	D3DXVECTOR3	m_vCenter;
	float	m_fRadius;

public:
	void SetRadius( float fRadius );
	void SetCenter( const D3DXVECTOR3 &vCenter );

	void Transform( const D3DXVECTOR3 &vTranslate,
					const D3DXVECTOR3 &vScale,
					const D3DXQUATERNION &qtRotate );

public:
	inline float GetRadius() const { return m_fRadius; }
	inline float GetSqrRadius() const { return m_fRadius * m_fRadius; }
	inline const D3DXVECTOR3& GetCenter() const { return m_vCenter; }

public:
	ioSphere();
	ioSphere( const D3DXVECTOR3 &vCenter, float fRadius );
	~ioSphere();
};

#endif