

#ifndef _ioSegment_h_
#define _ioSegment_h_

// Segment is S(t) = P+t*D for 0 <= t <= 1.  D is not necessarily unit length
// The end points are P and P+D.

class __EX ioSegment
{
private:
	D3DXVECTOR3 m_vOrigin;		// P
	D3DXVECTOR3 m_vDirection;	// D

public:
	void SetOrigin( const D3DXVECTOR3 &vOrig ) { m_vOrigin = vOrig; }
	void SetDirection( const D3DXVECTOR3 &vDir ) { m_vDirection = vDir; }

	const D3DXVECTOR3& GetOrigin() const { return m_vOrigin; }
	const D3DXVECTOR3& GetDirection() const { return m_vDirection; }

public:
	ioSegment();
	ioSegment( const ioSegment &rkSegment );
	ioSegment( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd );

	~ioSegment();
};

#endif
