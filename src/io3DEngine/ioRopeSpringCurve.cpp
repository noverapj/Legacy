	

#include "stdafx.h"

#include "ioMath.h"
#include "HelpFunc.h"

#include "ioOrientBox.h"
#include "ioBiped.h"
#include "ioSkeleton.h"

#include "ioRopeSpringCurve.h"
#include "ioBulletHelper.h"
#include "ErrorReport.h"

ioRopeSpringCurve::ioRopeSpringCurve()
{
	m_pBroadphase = NULL;
	m_pDispatcher = NULL;
	m_pSolver = NULL;
	m_pWorld = NULL;

	InitPhysics();

	m_vAccumulatedWorldForce = ioMath::VEC3_ZERO;
	m_fLengthFixRate = FLOAT05;
	m_fBoxWeights    = 1.0f;
}

ioRopeSpringCurve::~ioRopeSpringCurve()
{
	SetBipedManualBlending( false );
	ExitPhysics();
}

void ioRopeSpringCurve::InitPhysics()
{
	m_pDispatcher = new	btCollisionDispatcher( btAllocHelp::GetPool() );

	btVector3 btWorldMin( -1000.0f, -1000.0f, -1000.0f );
	btVector3 btWorldMax( 1000.0f, 1000.0f, 1000.0f );

	m_pBroadphase = new btAxisSweep3( btWorldMin, btWorldMax, 128 );
	m_pSolver = new btSequentialImpulseConstraintSolver;
	m_pWorld = new ioBulletWorld( m_pDispatcher,
								  m_pBroadphase,
								  m_pSolver,
								  btAllocHelp::GetPool(),
								  this );

	SetGravity( 0.0f, -10.0f, 0.0f );
}

void ioRopeSpringCurve::ExitPhysics()
{
	int i;

	int iJointDataCount = m_JointDataList.size();
	for( i=0 ; i<iJointDataCount ; i++ )
	{
		JointData &rkData = m_JointDataList[i];

		m_pWorld->removeConstraint( rkData.m_pJoint );
		delete rkData.m_pJoint;
	}
	m_JointDataList.clear();

	for( i = m_pWorld->getNumCollisionObjects()-1 ; i>=0 ; i-- )
	{
		btCollisionObject* obj = m_pWorld->getCollisionObjectArray()[i];

		btRigidBody* body = btRigidBody::upcast( obj );
		if( body )
		{
			if( body->getMotionState() )
			{
				delete body->getMotionState();
			}
			
			if( body->getCollisionShape())
			{
				delete body->getCollisionShape();
			}
		}

		m_pWorld->removeCollisionObject( obj );
		delete obj;
	}

	SAFEDELETE( m_pWorld );
	SAFEDELETE( m_pSolver );
	SAFEDELETE( m_pBroadphase );
	SAFEDELETE( m_pDispatcher );
}

bool ioRopeSpringCurve::IsRoot( const ioHashString &rkBiped ) const
{
	if( m_JointBipedList.empty() )
		return false;

	ioBiped *pRoot = m_JointBipedList.front();
	if( pRoot->GetName() == rkBiped )
		return true;

	return false;
}

void ioRopeSpringCurve::SetGravity( float fX, float fY, float fZ )
{
	if( m_pWorld )
	{
		m_pWorld->setGravity( btVector3( fX, fY, fZ ) );
	}
}

void ioRopeSpringCurve::AddWorldForce( const D3DXVECTOR3 &vForce )
{
	m_vAccumulatedWorldForce += vForce;
}

void ioRopeSpringCurve::ApplyAccumulatedWorldForce()
{
	if( D3DXVec3LengthSq( &m_vAccumulatedWorldForce ) > 0.0f )
	{
		btRigidBody *pRoot = m_RigidList[0]->m_pBody;
		btVector3 btForce = DXtoBT( -m_vAccumulatedWorldForce ) / pRoot->getInvMass();

		RigidList::iterator iter = m_RigidList.begin() + 2;
		for( ; iter!=m_RigidList.end() ; ++iter )
		{
			btRigidBody *pBody = (*iter)->m_pBody;
			pBody->applyCentralImpulse( btForce );
		}
	}

	m_vAccumulatedWorldForce = ioMath::VEC3_ZERO;
}

void ioRopeSpringCurve::SetBipedManualBlending( bool bManual )
{
	JointBipedList::iterator iter = m_JointBipedList.begin();
	for( ; iter!=m_JointBipedList.end() ; ++iter )
	{
		(*iter)->SetManualBlending( bManual );
	}
}

bool ioRopeSpringCurve::GenerateRopes( const std::vector< ioBiped* > &rkBipedList,
									   const RopeSpringParam *pParam )
{
	if( rkBipedList.size() < 2 || pParam->m_SwingList.empty() )
		return false;

	m_JointBipedList = rkBipedList;
	m_fLengthFixRate = pParam->m_fLengthFixRate;
	m_fBoxWeights    = pParam->m_fBoxWeights;

	m_SwingParamList.clear();
	m_SwingParamList.reserve( m_JointBipedList.size() - 1 );
	m_SwingParamList = pParam->m_SwingList;

	// Constraint 갯수만큼의 Parameter가 없다.
	if( m_SwingParamList.size() < m_JointBipedList.size() - 1 )
	{
		SwingParam kCopyParam = m_SwingParamList.back();

		int iNeedCount = m_JointBipedList.size() - m_SwingParamList.size() - 1;
		for( int i=0 ; i<iNeedCount ; i++ )
		{
			m_SwingParamList.push_back( kCopyParam );
		}
	}

	SetGravity( 0.0f, -pParam->m_fGravity, 0.0f );

	GenerateBodies( pParam->m_fDamping, pParam->m_fAngDamping );
	GenerateJoints();

	SetBipedManualBlending( true );

	return true;
}

void ioRopeSpringCurve::GenerateBodies( float fDamping, float fAngDamping )
{
	m_RigidList.clear();
	m_RigidList.reserve( m_JointBipedList.size() );

	ioBiped *pRoot = m_JointBipedList.front();
	
	D3DXVECTOR3 vParentPos = pRoot->GetInitDerivedPosition();
	D3DXQUATERNION qtParentRot = pRoot->GetInitDerivedOrientation();

	btRigidBody *pBody = CreateRigidBody( m_fBoxWeights,
										  vParentPos,
										  qtParentRot,
										  1.0f,
										  true );

	BodyData *pBodyData = new BodyData;
	pBodyData->m_pBody = pBody;
	pBodyData->m_InitXform = pBody->getCenterOfMassTransform();
	m_RigidList.push_back( pBodyData );

	D3DXVECTOR3 vOffsetDir = qtParentRot * ioMath::UNIT_Y;
	D3DXVec3Normalize( &vOffsetDir, &vOffsetDir );

	int iBipedCount = m_JointBipedList.size();
	for( int i=1 ; i<iBipedCount ; i++ )
	{
		ioBiped *pParent = m_JointBipedList[i-1];
		ioBiped *pChild  = m_JointBipedList[i];

		D3DXVECTOR3 vDiff = pChild->GetInitDerivedPosition() - pParent->GetInitDerivedPosition();
		D3DXVECTOR3 vCurPos = vParentPos + vOffsetDir * D3DXVec3Length( &vDiff );

		btRigidBody *pBody = CreateRigidBody( m_fBoxWeights,
											  vCurPos,
											  qtParentRot,
											  2.0f,
											  false );

		pBody->setDamping( fDamping, fAngDamping );

		BodyData *pBodyData = new BodyData;
		pBodyData->m_pBody = pBody;
		pBodyData->m_InitXform = pBody->getCenterOfMassTransform();
		m_RigidList.push_back( pBodyData );

		vParentPos = vCurPos;
	}
}

void ioRopeSpringCurve::GenerateJoints()
{
	m_JointDataList.reserve( m_SwingParamList.size() );

	int iBipedCount = m_JointBipedList.size();
	for( int i=1 ; i<iBipedCount ; i++ )
	{
		btRigidBody *pParent = m_RigidList[i-1]->m_pBody;
		btRigidBody *pChild  = m_RigidList[i]->m_pBody;

		const btTransform &xParent = pParent->getCenterOfMassTransform();
		const btTransform &xChild  = pChild->getCenterOfMassTransform();

		const SwingParam &rkSwing = m_SwingParamList[i-1];
		float fHSwingGap = rkSwing.m_fHSwingUP - rkSwing.m_fHSwingDOWN;
		float fVSwingGap = rkSwing.m_fVSwingUP - rkSwing.m_fVSwingDOWN;

		float fHRotR = DEGtoRAD( rkSwing.m_fHSwingUP - fHSwingGap * FLOAT05 );
		float fVRotR = DEGtoRAD( rkSwing.m_fVSwingUP - fVSwingGap * FLOAT05 );

		btQuaternion qtConstraint( -fVRotR, fHRotR, D3DX_PI*FLOAT05 );

		btTransform xConstraint;
		xConstraint.setRotation( xParent.getRotation() * qtConstraint );
		xConstraint.setOrigin( xParent.getOrigin() );

		btConeTwistConstraint *pCone = new btConeTwistConstraint( *pParent,
																  *pChild,
																  xParent.inverse() * xConstraint,
																  xChild.inverse() * xConstraint );

		// swing1 is along z, and swing2 is along y, twist is along x
		pCone->setLimit( DEGtoRAD( fHSwingGap ) * FLOAT05,
						 DEGtoRAD( fVSwingGap ) * FLOAT05,
						 0.0f,
						 0.4f,
						 0.15f );

		m_pWorld->addConstraint( pCone, true );

		btVector3 vDiff = xChild.getOrigin() - xParent.getOrigin();

		JointData kJointData;
		kJointData.m_pJoint  = pCone;
		kJointData.m_fLength = vDiff.length();
		m_JointDataList.push_back( kJointData );
	}
}

void ioRopeSpringCurve::UpdateSkeletonChange()
{
	if( m_JointBipedList.empty() || m_RigidList.empty() )
		return;

	ioBiped *pRoot = m_JointBipedList[0];
	btTransform btXform( DXtoBT( pRoot->GetDerivedOrientation() ),
						 DXtoBT( pRoot->GetDerivedPosition() ) );

	btRigidBody *pBody = m_RigidList[0]->m_pBody;
	if( pBody )
	{
		btMotionState *pMotion = pBody->getMotionState();
		if( pMotion )
		{
			pMotion->setWorldTransform( btXform );
		}
	}
}

void ioRopeSpringCurve::Update( float fTime )
{
	UpdateSkeletonChange();

	if( m_pWorld )
	{
		//UJ 131022, CHECK: 사용법이 잘못된것 같음, 두 번째 인자를 추가해야지 그렇지않으면 "losing time"
		// http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_the_World
		// timeStep < maxSubSteps * fixedTimeStep이고 우리의 fixedTimeStep은 1/60이다
		m_pWorld->stepSimulation( fTime * 0.001f, max(1, fTime / (1.0f/60.0f)) );
	}

	ReArrangeOverChanged();
	ApplyRigidChangeToBiped();
}

void ioRopeSpringCurve::ReArrangeOverChanged()
{
	if( m_RigidList.empty() || m_JointBipedList.empty() )
		return;

	bool bNeedModify = false;
	JointDataList::iterator iter = m_JointDataList.begin() + 1;
	for( ; iter!=m_JointDataList.end() ; ++iter )
	{
		const JointData &rkData = *iter;

		btTypedConstraint *pJoint = rkData.m_pJoint;

		btVector3 btGap = pJoint->getRigidBodyA().getCenterOfMassPosition() -
						  pJoint->getRigidBodyB().getCenterOfMassPosition();

		if( rkData.m_fLength * 5.0f < btGap.length() )
		{
			bNeedModify = true;
			break;
		}
	}

	if( bNeedModify )
	{
		RigidList::iterator iter = m_RigidList.begin();
		for( ; iter!=m_RigidList.end() ; ++iter )
		{
			BodyData* pData = (*iter);
			btRigidBody *pBody = pData->m_pBody;

			pBody->setLinearVelocity( btVector3(0,0,0) );
			pBody->setAngularVelocity( btVector3(0,0,0) );

			int iFlags = pBody->getCollisionFlags();
			pBody->setCollisionFlags( 0 );
			pBody->setCenterOfMassTransform( pData->m_InitXform );

			btMotionState *pMotion = pBody->getMotionState();
			if( pMotion )
			{
				pMotion->setWorldTransform( pData->m_InitXform );
			}

			pBody->setCollisionFlags( iFlags );
		}

		int iJointDataCount = m_JointDataList.size();
		for( int i=0 ; i<iJointDataCount ; i++ )
		{
			JointData &rkData = m_JointDataList[i];

			if ( m_pWorld )
				m_pWorld->removeConstraint( rkData.m_pJoint );

			delete rkData.m_pJoint;
		}
		m_JointDataList.clear();

		GenerateJoints();
	}
}

void ioRopeSpringCurve::ApplyRigidChangeToBiped()
{
	int iBipedCount = m_JointBipedList.size();
	for( int i=1 ; i<iBipedCount ; i++ )
	{
		ioBiped *pCurrent = m_JointBipedList[i];
		ioBiped *pParent  = pCurrent->GetParentBiped();

		if( pCurrent && pParent )
		{
			D3DXMATRIX matInvParent;
			D3DXMatrixInverse( &matInvParent, NULL, pParent->GetFullTransform() );

			btRigidBody *pBody = m_RigidList[i]->m_pBody;
			if( pBody )
			{
				D3DXVECTOR3 vNewPos = BTtoDX( pBody->getCenterOfMassPosition() );
				D3DXVec3TransformCoord( &vNewPos, &vNewPos, &matInvParent );
				pCurrent->SetPosition( vNewPos );
			}
		}
	}
}

void ioRopeSpringCurve::ForceLimitAngles( float fTimePerSec )
{
	int iJointDataCount = m_JointDataList.size();
	for( int i=0 ; i<iJointDataCount; i++ )
	{
		btRigidBody *pParent = m_RigidList[i]->m_pBody;
		btRigidBody *pChild  = m_RigidList[i+1]->m_pBody;

		const btTransform &btParent = pParent->getCenterOfMassTransform();
		const btTransform &btChild  = pChild->getCenterOfMassTransform();

		btVector3 vToChild = btChild.getOrigin() - btParent.getOrigin();

		float fSwingX = atan2f( vToChild.dot( btParent.getBasis().getColumn(2) ),
								vToChild.dot( btParent.getBasis().getColumn(1) ) );

		float fSwingZ = atan2f( vToChild.dot( btParent.getBasis().getColumn(0) ),
								vToChild.dot( btParent.getBasis().getColumn(1) ) );

		float fSwingXCopy = fSwingX;
		float fSwingZCopy = fSwingZ;

		const SwingParam &rkSwing = m_SwingParamList[i];
		float fZMin = DEGtoRAD( rkSwing.m_fHSwingDOWN );
		float fZMax = DEGtoRAD( rkSwing.m_fHSwingUP );
		float fXMin = DEGtoRAD( rkSwing.m_fVSwingDOWN );
		float fXMax = DEGtoRAD( rkSwing.m_fVSwingUP );

		fSwingZ = max( fZMin, min( fSwingZ, fZMax ) );
		fSwingX = max( fXMin, min( fSwingX, fXMax ) );

		float fDefLength = m_JointDataList[i].m_fLength;

		if( fSwingXCopy != fSwingX || fSwingZCopy != fSwingZ )
		{
			D3DXQUATERNION qtGap;
			D3DXQuaternionRotationYawPitchRoll( &qtGap, 0.0f, fSwingX, fSwingZ );

			D3DXQUATERNION qtFinal = qtGap * BTtoDX( btParent.getRotation() );
			D3DXVECTOR3 vNewDir = qtFinal * D3DXVECTOR3( 0.0f, fDefLength, 0.0f );

			btVector3 vCurVel  = pChild->getLinearVelocity();
			btVector3 vImpulse = btParent.getOrigin() + DXtoBT( vNewDir ) - btChild.getOrigin();

			if( vImpulse.dot( vCurVel ) < 0.0f )
			{
				pChild->setLinearVelocity( vCurVel * 0.2f );
			}

			pChild->applyCentralImpulse( vImpulse );
		}

		btVector3 vVelocity = pChild->getLinearVelocity();
		if( vVelocity.length() > 130.0f )
		{
			btScalar btCurVelAmount = min( vVelocity.length() * 0.3f, 80.0f );
			pChild->setLinearVelocity( vVelocity.normalized() * btCurVelAmount );
		}

		if( m_fLengthFixRate > 0.0f && fTimePerSec > 0.0f )
		{
			btTransform btNewParent, btNewChild;
			pParent->predictIntegratedTransform( fTimePerSec, btNewParent );
			pChild->predictIntegratedTransform( fTimePerSec, btNewChild );

			btVector3 vNewGap = btNewChild.getOrigin() - btNewParent.getOrigin();
			float fNewGapLength = (float)vNewGap.length();

			if( !COMPARE( fNewGapLength, fDefLength * 0.95f, fDefLength * 1.05f ) )
			{
				float fLengthGap = ( fDefLength - fNewGapLength ) / fTimePerSec * m_fLengthFixRate;
				btVector3 vNewImpulse = vNewGap.normalized() * ( fLengthGap / pChild->getInvMass() );
				pChild->applyCentralImpulse( vNewImpulse );
			}
		}
	}
}

btRigidBody* ioRopeSpringCurve::CreateRigidBody( float fMass,
												 const D3DXVECTOR3 &vPos,
												 const D3DXQUATERNION &qtRot,
												 float fShapeRadius,
												 bool bKinematic )
{
	btVector3 vInertia(0,0,0);
	btSphereShape *pShape = new btSphereShape( fShapeRadius );

	if( fMass != 0.0f )
	{
		pShape->calculateLocalInertia( fMass, vInertia );
	}

	btTransform btXform( DXtoBT( qtRot ), DXtoBT( vPos ) );
	btDefaultMotionState *pMotion = new btDefaultMotionState( btXform );

	btRigidBody *pBody = new btRigidBody( fMass, pMotion, pShape, vInertia );
	if( bKinematic )
	{
		pBody->setCollisionFlags( pBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
	}

	pBody->setActivationState( DISABLE_DEACTIVATION );

	m_pWorld->addRigidBody( pBody );

	return pBody;
}

void ioRopeSpringCurve::DebugGetBoxList( std::vector< ioOrientBox > &rkBoxList )
{
	RigidList::iterator iter = m_RigidList.begin();
	for( ; iter!=m_RigidList.end() ; ++iter )
	{
		btRigidBody *pBody = (*iter)->m_pBody;

		btCollisionShape *pShape = pBody->getCollisionShape();
		if( !pShape )	continue;
		if( pShape->getShapeType() != BOX_SHAPE_PROXYTYPE ||
			pShape->getShapeType() != SPHERE_SHAPE_PROXYTYPE )
			continue;

		btVector3 vHarfSize;
		if( pShape->getShapeType() == BOX_SHAPE_PROXYTYPE )
		{
			btBoxShape *pBox = dynamic_cast< btBoxShape* >( pShape );
			vHarfSize = pBox->getHalfExtentsWithMargin();
		}
		else	// SPHERE_SHAPE_PROXYTYPE
		{
			btSphereShape *pSphere = dynamic_cast< btSphereShape* >( pShape );
			
			btScalar btRadius = pSphere->getRadius();
			vHarfSize = btVector3( btRadius, btRadius, btRadius );
		}

		ioOrientBox kBox;
		kBox.SetExtents( 0, vHarfSize.x() );
		kBox.SetExtents( 1, vHarfSize.y() );
		kBox.SetExtents( 2, vHarfSize.z() );

		btTransform btXform;
		pBody->getMotionState()->getWorldTransform( btXform );

		D3DXVECTOR3 vPos = BTtoDX( btXform.getOrigin() );
		D3DXQUATERNION qtRot = BTtoDX( btXform.getRotation() );
		kBox.Transform( vPos, ioMath::UNIT_ALL, qtRot );

		rkBoxList.push_back( kBox );
	}
}



