

#ifndef _ioRopeSpringCurve_h_
#define _ioRopeSpringCurve_h_

class btBroadphaseInterface;
class btCollisionDispatcher;
class btConstraintSolver;

class btRigidBody;
class btTypedConstraint;
class btCollisionShape;

class ioBiped;
class ioBulletWorld;

#include "LinearMath\btTransform.h"

class ioRopeSpringCurve
{
protected:
	btBroadphaseInterface *m_pBroadphase;
	btCollisionDispatcher *m_pDispatcher;
	btConstraintSolver*	m_pSolver;
	ioBulletWorld *m_pWorld;

protected:
	typedef std::vector< ioBiped* > JointBipedList;
	JointBipedList m_JointBipedList;
	SwingParamList m_SwingParamList;

	struct BodyData
	{
		btRigidBody *m_pBody;
		btTransform m_InitXform;
	};

	typedef std::vector< BodyData* > RigidList;
	RigidList m_RigidList;

	struct JointData
	{
		btTypedConstraint *m_pJoint;
		float m_fLength;
	};

	typedef std::vector< JointData > JointDataList;
	JointDataList m_JointDataList;

protected:
	D3DXVECTOR3 m_vAccumulatedWorldForce;
	float m_fLengthFixRate;
	float m_fBoxWeights;

protected:
	void InitPhysics();
	void ExitPhysics();

public:
	bool IsRoot( const ioHashString &rkBiped ) const;
	void AddWorldForce( const D3DXVECTOR3 &vForce );
	void ApplyAccumulatedWorldForce();
	void ForceLimitAngles( float fTimePerSec );

public:
	void Update( float fTime );

protected:
	void UpdateSkeletonChange();
	void ReArrangeOverChanged();
	void ApplyRigidChangeToBiped();

public:
	bool GenerateRopes( const std::vector< ioBiped* > &rkBipedList,
						const RopeSpringParam *pParam );

protected:
	void SetGravity( float fX, float fY, float fZ );
	void SetBipedManualBlending( bool bManual );

	void GenerateBodies( float fDamping, float fAngDamping );
	void GenerateJoints();

	btRigidBody* CreateRigidBody( float fMass,
								  const D3DXVECTOR3 &vPos,
								  const D3DXQUATERNION &qtRot,
								  float fShapeRadius,
								  bool bKinematic );

public:
	void DebugGetBoxList( std::vector< ioOrientBox > &rkBoxList );

public:
	ioRopeSpringCurve();
	~ioRopeSpringCurve();
};

#endif
