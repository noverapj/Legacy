#pragma once


#include "../io3DEngine/ioSphere.h"
#include "../io3DEngine/ioOrientBox.h"

class ioSkeleton;
class ioCylinder;

struct WoundedCollisionBox
{
	ioOrientBox m_LocalBox;
	ioOrientBox m_WorldBox;

	ioHashString m_LinkBone;
	int m_iLinkBipedIndex;
};

class ioWoundedCollisionBoxGrp
{
private:
	typedef std::vector< WoundedCollisionBox* > WoundedCollisionBoxList;
	WoundedCollisionBoxList m_WoundedCollisionBoxList;
	ioSphere m_WorldBoundingSphere;

public:
	bool InitBoxList( const char *szFileName );

private:
	bool IsWoundedCollisionFile( const char *szToken, int iVersion );
	WoundedCollisionBox* CreateNewBox();
	void ClearBoxList();

public:
	void UpdateWorldBoxList( const ioSkeleton *pSkeleton, const D3DXMATRIX *pWorldXform );
	void UpdateBipedIndex( const ioSkeleton *pSkeleton );

public:
	bool IsCollision( const ioOrientBox &rkAttackBox, D3DXVECTOR3 *pColPoint = NULL ) const;

	bool CheckSphereDistance( const D3DXVECTOR3 &vStart, float fRange, float &fDistance ) const;
	bool CheckSpherePoint( const D3DXVECTOR3 &vStart, float fRange, D3DXVECTOR3 &vPoint ) const;
	bool CheckCylinderPoint( const ioCylinder &rkCylinder,
							 const D3DXVECTOR3 &vStart,
							 D3DXVECTOR3 &vPoint ) const;

public:
	bool GetLocalBox( const ioHashString &rkBiped, ioOrientBox &rkLocalBox ) const;

	bool GetBoxVertexArray( int iIndex, D3DXVECTOR3 pArray[8] ) const;
	int GetBoxCount() const;

public:
	ioWoundedCollisionBoxGrp* Clone();

public:
	ioWoundedCollisionBoxGrp();
	ioWoundedCollisionBoxGrp( const ioWoundedCollisionBoxGrp &rhs );
	~ioWoundedCollisionBoxGrp();
};

