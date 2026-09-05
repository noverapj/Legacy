

#ifndef _ioOrientBox_h_
#define _ioOrientBox_h_

class ioAxisAlignBox;

class __EX ioOrientBox
{
private:
	D3DXVECTOR3	m_vCenter;
	D3DXVECTOR3 m_vAxis[3];
	
	float	m_fExtent[3];
	mutable float m_fBoxRadius;

	bool	m_bNull;
	mutable bool m_bNeedUpdateRadius;

public:
	void Transform( const D3DXMATRIX *pXForm );
	void Transform( const D3DXVECTOR3 &vTranslate,
					const D3DXVECTOR3 &vScale,
					const D3DXQUATERNION &qtRotate );

public:
	void SetNull();
	void SetCenter( const D3DXVECTOR3 &vCenter );
	void SetAxis( int i, const D3DXVECTOR3 &vAxis );
	void SetExtents( int i, float fExtent );

	void SetByAxisBox( const ioAxisAlignBox &rkAABB );
	void SetBoxByMinMax( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );
	void SetBoxMinMaxForAxis( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );

protected:
	inline void NeedUpdateBoxRadiuse() { m_bNeedUpdateRadius = true; }

public:
	inline bool IsNull() const { return m_bNull; }

	inline const D3DXVECTOR3& GetCenter() const { return m_vCenter; }
	inline const D3DXVECTOR3& GetAxis( int i ) const { return m_vAxis[i]; }
	inline const D3DXVECTOR3* GetAxis() const { return m_vAxis; }
	inline float GetExtents( int i ) const { return m_fExtent[i]; }
	inline const float* GetExtents() const { return m_fExtent; }

	float GetBoxRadius() const;

public:
	void GetVertices( D3DXVECTOR3 *pArray ) const;
	D3DXMATRIX GetTransform() const;

public:
	bool operator==( const ioOrientBox &rhs ) const;
	bool operator!=( const ioOrientBox &rhs ) const;

public:
	ioOrientBox();
	~ioOrientBox();
};

#endif
