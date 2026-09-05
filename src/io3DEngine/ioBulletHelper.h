

#ifndef _ioBullet_Helper_h_
#define _ioBullet_Helper_h_

#include "btBulletDynamicsCommon.h"

//---------------------------------------------------------------------------------------------

inline btVector3 DXtoBT( const D3DXVECTOR3 &vVector )
{
	return btVector3( vVector.x, vVector.y, vVector.z );
}

inline D3DXVECTOR3 BTtoDX( const btVector3 &vVector )
{
	return D3DXVECTOR3( vVector.x(), vVector.y(), vVector.z() );
}

inline btQuaternion DXtoBT( const D3DXQUATERNION &qtRot )
{
	return btQuaternion( qtRot.x, qtRot.y, qtRot.z, qtRot.w );
}

inline D3DXQUATERNION BTtoDX( const btQuaternion &qtRot )
{
	return D3DXQUATERNION( qtRot.x(), qtRot.y(), qtRot.z(), qtRot.w() );
}

//---------------------------------------------------------------------------------------------

namespace btAllocHelp
{
	void InitializeAlloc( int iAlgorithmPoolSize, 
						  int iManifoldPoolSize,
						  int iStackSize );

	void DestroyAlloc();

	btDefaultCollisionConfiguration* GetPool();
}

//---------------------------------------------------------------------------------------------

class ioRopeSpringCurve;

class ioBulletWorld : public btDiscreteDynamicsWorld
{
protected:
	ioRopeSpringCurve *m_pRope;

public:
	virtual void applyGravity();
	virtual void internalSingleStepSimulation( btScalar timeStep );

public:
	ioBulletWorld( btDispatcher* dispatcher,
				   btBroadphaseInterface* pairCache,
				   btConstraintSolver* constraintSolver,
				   btCollisionConfiguration* collisionConfiguration,
				   ioRopeSpringCurve *pRope );

	virtual ~ioBulletWorld();
};

#endif
