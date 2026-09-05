

#ifndef _ioRay_h_
#define _ioRay_h_

// Ray is R(t) = P+t*D for t >= 0.  D is not necessarily unit length.

class __EX ioRay
{
private:
	D3DXVECTOR3	m_vOrigin;
	D3DXVECTOR3 m_vDirection;

public:
	void SetOrigin( const D3DXVECTOR3 &vOrig );
	void SetDirection( const D3DXVECTOR3 &vDir );

	void SetXAxisDirection( bool bInverse );
	void SetYAxisDirection( bool bInverse );
	void SetZAxisDirection( bool bInverse );

	const D3DXVECTOR3& GetOrigin() const;
	const D3DXVECTOR3& GetDirection() const;

public:
	ioRay();
	ioRay( const D3DXVECTOR3 &vOrig, const D3DXVECTOR3 &vDir );
	ioRay( const ioRay &rhs, const D3DXMATRIX *pMat );
	~ioRay();
};

#endif