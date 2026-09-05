

#ifndef _ioAxisAlignBox_h_
#define _ioAxisAlignBox_h_

#include "io3DCommon.h"

/*
                 o-----o ( m_vMaxPos )
	            /|    /|
               / |   / |
              o-----o  |
              |  o--|--o
              | /   | /
              |/    |/
( m_vMinPos ) o-----o
*/

class ioOrientBox;

class __EX ioAxisAlignBox
{
protected:
	D3DXVECTOR3	m_vMinPos;
	D3DXVECTOR3	m_vMaxPos;
	D3DXVECTOR3	m_vCenter;
	float m_fHalfDiagnal;
	bool m_bNull;

public:
	void Translate( const D3DXVECTOR3 &vMove );

	void Transform( const D3DXMATRIX *pMat );
	void Transform( const D3DXVECTOR3 &vPosition,
					const D3DXVECTOR3 &vScale,
					const D3DXQUATERNION &qtOrientation );

public:
	void Merge( const ioAxisAlignBox &rhs );
	void Merge( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );

public:
	void SetMinPos( const D3DXVECTOR3 &vMin );
	void SetMaxPos( const D3DXVECTOR3 &vMax );
	void SetMinMaxPos( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );
	void SetMinMaxPos( const Vector3Vec &vVtxList, const D3DXMATRIX *pXform );
	void SetByOrientBox( const ioOrientBox &rkOBB );

	void AddPoint( const D3DXVECTOR3 &vPoint );

protected:
	void UpdateCenter();

public:
	inline void SetNull(){ m_bNull = true; }
	inline bool IsNull() const { return m_bNull; }

	inline float GetHalfDiagonal() const { return m_fHalfDiagnal; }
	inline const D3DXVECTOR3& GetMinPos() const { return m_vMinPos; }
	inline const D3DXVECTOR3& GetMaxPos() const { return m_vMaxPos; }
	inline const D3DXVECTOR3& GetCenter() const { return m_vCenter; }
	inline D3DXVECTOR3 GetSize() const { return m_vMaxPos - m_vMinPos; }

	void GetVertices( D3DXVECTOR3 vPos[8] ) const;
	float GetExtents( int i ) const;
	float GetVolume() const;

public:
	ioAxisAlignBox();
	explicit ioAxisAlignBox( const ioOrientBox &rkOBB );
	ioAxisAlignBox( const D3DXVECTOR3 &vMin, const D3DXVECTOR3 &vMax );
	virtual ~ioAxisAlignBox();
};

#endif