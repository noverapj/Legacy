

#include "stdafx.h"

#include "ioBiped.h"
#include "ioSkeleton.h"
#include "ioBulletHelper.h"
#include "ioRopeSpringCurve.h"

#include "ErrorReport.h"

namespace btAllocHelp
{
	
static btDefaultCollisionConfiguration *s_Configuration = NULL;

void InitializeAlloc( int iAlgorithmPoolSize, 
					  int iManifoldPoolSize,
					  int iStackSize )
{
	DestroyAlloc();

	btDefaultCollisionConstructionInfo kInfo;
	kInfo.m_defaultMaxCollisionAlgorithmPoolSize = iAlgorithmPoolSize;
	kInfo.m_defaultMaxPersistentManifoldPoolSize = iManifoldPoolSize;
	kInfo.m_defaultStackAllocatorSize = iStackSize;

	s_Configuration = new btDefaultCollisionConfiguration( kInfo );
}

void DestroyAlloc()
{
	SAFEDELETE( s_Configuration );
}

btDefaultCollisionConfiguration* GetPool()
{
	return s_Configuration;
}

}

ioBulletWorld::ioBulletWorld( btDispatcher* dispatcher,
							  btBroadphaseInterface* pairCache,
							  btConstraintSolver* constraintSolver,
							  btCollisionConfiguration* collisionConfiguration,
							  ioRopeSpringCurve *pRope )
: btDiscreteDynamicsWorld( dispatcher, pairCache, constraintSolver, collisionConfiguration )
{
	m_pRope = pRope;
}

ioBulletWorld::~ioBulletWorld()
{
}

void ioBulletWorld::applyGravity()
{
	btDiscreteDynamicsWorld::applyGravity();

	m_pRope->ApplyAccumulatedWorldForce();
}

void ioBulletWorld::internalSingleStepSimulation( btScalar timeStep )
{
	///apply gravity, predict motion
	predictUnconstraintMotion(timeStep);

	btDispatcherInfo& dispatchInfo = getDispatchInfo();
	dispatchInfo.m_timeStep  = timeStep;
	dispatchInfo.m_stepCount = 0;
	dispatchInfo.m_debugDraw = getDebugDrawer();

	calculateSimulationIslands();

	getSolverInfo().m_timeStep = timeStep;

	///solve contact and other joint constraints
	solveConstraints( getSolverInfo() );

	///integrate transforms
	integrateTransforms(timeStep);

	updateActivationState( timeStep );

	m_pRope->ForceLimitAngles( timeStep );
}


