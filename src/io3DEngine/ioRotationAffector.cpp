

#include "stdafx.h"

#include "ioMath.h"
#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioRenderable.h"
#include "ioMovableObject.h"

#include "ioParticle.h"
#include "ioParticleIterator.h"
#include "ioParticleAffector.h"
#include "ioParticleSystem.h"
#include "ioRotationAffector.h"
#include "HelpFunc.h"

ioRotationAffector::CmdRotAxis	ioRotationAffector::m_RotAxisCmd;
ioRotationAffector::CmdRotSpeed ioRotationAffector::m_RotSpeedCmd;

ioRotationAffector::CmdStartRotAxis	ioRotationAffector::m_StartRotAxisCmd;
ioRotationAffector::CmdStartRotRange ioRotationAffector::m_StartRotRangeCmd;
ioRotationAffector::CmdSetStartAngle ioRotationAffector::m_SetStartAngleCmd;

ioRotationAffector::ioRotationAffector()
{
	m_vRotAxis = ioMath::UNIT_Y;
	m_fMinRotSpeed = 0.0f;
	m_fMaxRotSpeed = 0.0f;

	m_vStartRotAxis = ioMath::UNIT_Y;
	m_fRotRangeStart = 0.0f;
	m_fRotRangeEnd   = 360.0f;

	m_bSetStartAngle = false;

	if( CreateParamDictionary( "RotationAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "rot_axis", PT_VECTOR3 ),
							 &m_RotAxisCmd );

		pDict->AddParameter( ioParameterDef( "rot_speed", PT_VECTOR2 ),
							 &m_RotSpeedCmd );

		pDict->AddParameter( ioParameterDef( "start_rot_axis", PT_VECTOR3 ),
							 &m_StartRotAxisCmd );

		pDict->AddParameter( ioParameterDef( "start_rot_range", PT_VECTOR2 ),
							 &m_StartRotRangeCmd );

		pDict->AddParameter( ioParameterDef( "set_start_angle", PT_BOOL ),
							 &m_SetStartAngleCmd );
	}
}

ioRotationAffector::~ioRotationAffector()
{
}

void ioRotationAffector::InitParticle( ioParticleSystem *pSys, ioParticle *pParticle )
{
	float fSpeed = ioMath::RangeRandom( m_fMinRotSpeed, m_fMaxRotSpeed );
	pParticle->SetRotateSpeed( DEGtoRAD( fSpeed ) );

	if( m_bSetStartAngle )
	{
		float fRotStart = ioMath::RangeRandom( m_fRotRangeStart, m_fRotRangeEnd );
		pParticle->SetRotate( m_vStartRotAxis, fRotStart );
	}
}

void ioRotationAffector::AffectParticle( ioParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	ioParticleIterator iter = pSys->GetIterator();

	float fRotate;
	ioParticle *pParticle = NULL;
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		fRotate = fTimePerSec * pParticle->m_fRotationSpeed;
		pParticle->Rotate( m_vRotAxis, fRotate );
	}
}

void ioRotationAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
	m_vRotAxis = qtRotate * m_vRotAxis;
	D3DXVec3Normalize( &m_vRotAxis, &m_vRotAxis );

	m_vStartRotAxis = qtRotate * m_vStartRotAxis;
	D3DXVec3Normalize( &m_vStartRotAxis, &m_vStartRotAxis );
}

bool ioRotationAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioRotationAffector *pRotate = dynamic_cast< const ioRotationAffector* >( pRhs );

	if( !ioMath::IsEqual( m_vRotAxis, pRotate->m_vRotAxis ) )
		return false;

	if( m_fMinRotSpeed != pRotate->m_fMinRotSpeed )
		return false;

	if( m_fMaxRotSpeed != pRotate->m_fMaxRotSpeed )
		return false;

	if( !ioMath::IsEqual( m_vStartRotAxis, pRotate->m_vStartRotAxis ) )
		return false;

	if( m_fRotRangeStart != pRotate->m_fRotRangeStart )
		return false;

	if( m_fRotRangeEnd != pRotate->m_fRotRangeEnd )
		return false;

	if( m_bSetStartAngle != pRotate->m_bSetStartAngle )
		return false;

	return true;
}

void ioRotationAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioRotationAffector *pRotate = dynamic_cast< const ioRotationAffector* >( pSrc );
	
	m_vRotAxis = pRotate->m_vRotAxis;
	m_fMinRotSpeed = pRotate->m_fMinRotSpeed;
	m_fMaxRotSpeed = pRotate->m_fMaxRotSpeed;

	m_vStartRotAxis = pRotate->m_vStartRotAxis;
	m_fRotRangeStart = pRotate->m_fRotRangeStart;
	m_fRotRangeEnd   = pRotate->m_fRotRangeEnd;
	m_bSetStartAngle = pRotate->m_bSetStartAngle;
}

const ioHashString& ioRotationAffector::GetType() const
{
	static ioHashString szType( "Rotation" );
	return szType;
}

void ioRotationAffector::SetRotAxis( const D3DXVECTOR3 &vAxis )
{
	m_vRotAxis = vAxis;
}

void ioRotationAffector::SetRotSpeed( float fMinSpeed, float fMaxSpeed )
{
	m_fMinRotSpeed = fMinSpeed;
	m_fMaxRotSpeed = fMaxSpeed;
}

void ioRotationAffector::SetStartRotAxis( const D3DXVECTOR3 &vAxis )
{
	m_vStartRotAxis = vAxis;
}

void ioRotationAffector::SetStartRotRange( float fStart, float fEnd )
{
	m_fRotRangeStart = fStart;
	m_fRotRangeEnd   = fEnd;
}

void ioRotationAffector::SetStartAngle( bool bSet )
{
	m_bSetStartAngle = bSet;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioRotationAffector::CmdRotAxis::Get( const void *pTarget ) const
{
	const ioRotationAffector *pRotate = static_cast< const ioRotationAffector* >(pTarget);

	return ioStringConverter::toString( pRotate->GetRotAxis() );
}

void ioRotationAffector::CmdRotAxis::Set( void *pTarget, const std::string &kVal )
{
	ioRotationAffector *pRotate = static_cast< ioRotationAffector* >(pTarget);

	pRotate->SetRotAxis( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioRotationAffector::CmdRotSpeed::Get( const void *pTarget ) const
{
	const ioRotationAffector *pRotate = static_cast< const ioRotationAffector* >(pTarget);

	D3DXVECTOR2 vSpeed;
	vSpeed.x = pRotate->GetMinRotSpeed();
	vSpeed.y = pRotate->GetMaxRotSpeed();

	return ioStringConverter::toString( vSpeed );
}

void ioRotationAffector::CmdRotSpeed::Set( void *pTarget, const std::string &kVal )
{
	ioRotationAffector *pRotate = static_cast< ioRotationAffector* >(pTarget);

	D3DXVECTOR2 vSpeed = ioStringConverter::ParseVector2( kVal.c_str() );
	pRotate->SetRotSpeed( vSpeed.x, vSpeed.y );
}

std::string ioRotationAffector::CmdStartRotAxis::Get( const void *pTarget ) const
{
	const ioRotationAffector *pRotate = static_cast< const ioRotationAffector* >(pTarget);

	return ioStringConverter::toString( pRotate->GetStartRotAxis() );
}

void ioRotationAffector::CmdStartRotAxis::Set( void *pTarget, const std::string &kVal )
{
	ioRotationAffector *pRotate = static_cast< ioRotationAffector* >(pTarget);

	pRotate->SetStartRotAxis( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioRotationAffector::CmdStartRotRange::Get( const void *pTarget ) const
{
	const ioRotationAffector *pRotate = static_cast< const ioRotationAffector* >(pTarget);

	D3DXVECTOR2 vRange;
	vRange.x = pRotate->GetRotRangeStart();
	vRange.y = pRotate->GetRotRangeEnd();

	return ioStringConverter::toString( vRange );
}

void ioRotationAffector::CmdStartRotRange::Set( void *pTarget, const std::string &kVal )
{
	ioRotationAffector *pRotate = static_cast< ioRotationAffector* >(pTarget);

	D3DXVECTOR2 vRange = ioStringConverter::ParseVector2( kVal.c_str() );
	pRotate->SetStartRotRange( vRange.x, vRange.y );
}

std::string ioRotationAffector::CmdSetStartAngle::Get( const void *pTarget ) const
{
	const ioRotationAffector *pRotate = static_cast< const ioRotationAffector* >(pTarget);

	return ioStringConverter::toString( pRotate->IsSetStartAngle() );
}

void ioRotationAffector::CmdSetStartAngle::Set( void *pTarget, const std::string &kVal )
{
	ioRotationAffector *pRotate = static_cast< ioRotationAffector* >(pTarget);

	bool bSet = ioStringConverter::ParseBool( kVal.c_str() );
	pRotate->SetStartAngle( bSet );
}