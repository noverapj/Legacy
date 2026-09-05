

#include "stdafx.h"

#include "ioMath.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioMovableObject.h"

#include "ioModelParticle.h"
#include "ioModelParticleAffector.h"

#include "ioModelRotateAffector.h"
#include "ioModelParticleIterator.h"

#include "ioModelParticleSystem.h"

#include "HelpFunc.h"

ioModelRotateAffector::CmdRotAxis  ioModelRotateAffector::m_RotAxisCmd;
ioModelRotateAffector::CmdRotSpeed ioModelRotateAffector::m_RotSpeedCmd;

ioModelRotateAffector::ioModelRotateAffector() : m_vAxis( 0.0f, 1.0f, 0.0f )
{
	m_fMinRotSpeed = 0.0f;
	m_fMaxRotSpeed = 0.0f;

	if( CreateParamDictionary( "ModelRotateAffector" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "rot_axis", PT_VECTOR3 ),
							 &m_RotAxisCmd );

		pDict->AddParameter( ioParameterDef( "rot_speed", PT_VECTOR2 ),
							 &m_RotSpeedCmd );
	}
}

ioModelRotateAffector::~ioModelRotateAffector()
{
}

void ioModelRotateAffector::SetRotSpeed( float fMinSpeed, float fMaxSpeed )
{
	m_fMinRotSpeed = fMinSpeed;
	m_fMaxRotSpeed = fMaxSpeed;
}

void ioModelRotateAffector::InitModel( ioModelParticle *pMdl )
{
	float fSpeed = ioMath::RangeRandom( m_fMinRotSpeed, m_fMaxRotSpeed );
	pMdl->SetRotateSpeed( fSpeed );

	float fRot = ioMath::RangeRandom( 0.0f, 360.f );
	pMdl->Rotate( m_vAxis, fRot );
}

void ioModelRotateAffector::AffectModel( ioModelParticleSystem *pSys,
									     float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	ioModelParticleIterator iter = pSys->GetIterator();

	float fRotate;
	ioModelParticle *pMdl;
	while( !iter.end() )
	{
		pMdl = iter.GetNext();

		fRotate = fTimePerSec * pMdl->GetRotateSpeed();
		pMdl->Rotate( m_vAxis, fRotate );
	}
}

void ioModelRotateAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
	m_vAxis = qtRotate * m_vAxis;
	D3DXVec3Normalize( &m_vAxis, &m_vAxis );
}

bool ioModelRotateAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( !ioModelParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioModelRotateAffector *pRotate = dynamic_cast< const ioModelRotateAffector* >( pRhs );
	if( !ioMath::IsEqual( m_vAxis, pRotate->m_vAxis ) )
		return false;

	if( m_fMinRotSpeed != pRotate->m_fMinRotSpeed )
		return false;

	if( m_fMaxRotSpeed != pRotate->m_fMaxRotSpeed )
		return false;

	return true;
}

void ioModelRotateAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	ioModelParticleAffector::ParametersCopy( pSrc );

	const ioModelRotateAffector *pRotate = dynamic_cast< const ioModelRotateAffector* >( pSrc );

	m_vAxis = pRotate->m_vAxis;
	m_fMinRotSpeed = pRotate->m_fMinRotSpeed;
	m_fMaxRotSpeed = pRotate->m_fMaxRotSpeed;
}

const ioHashString& ioModelRotateAffector::GetType() const
{
	static ioHashString szType( "Rotation" );
	return szType;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelRotateAffector::CmdRotAxis::Get( const void *pTarget ) const
{
	const ioModelRotateAffector *pRotate = static_cast< const ioModelRotateAffector* >(pTarget);

	return ioStringConverter::toString( pRotate->GetRotAxis() );
}

void ioModelRotateAffector::CmdRotAxis::Set( void *pTarget, const std::string &kVal )
{
	ioModelRotateAffector *pRotate = static_cast< ioModelRotateAffector* >(pTarget);

	pRotate->SetRotAxis( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioModelRotateAffector::CmdRotSpeed::Get( const void *pTarget ) const
{
	const ioModelRotateAffector *pRotate = static_cast< const ioModelRotateAffector* >(pTarget);

	D3DXVECTOR2 vSpeed;
	vSpeed.x = pRotate->GetMinRotSpeed();
	vSpeed.y = pRotate->GetMaxRotSpeed();

	return ioStringConverter::toString( vSpeed );
}

void ioModelRotateAffector::CmdRotSpeed::Set( void *pTarget, const std::string &kVal )
{
	ioModelRotateAffector *pRotate = static_cast< ioModelRotateAffector* >(pTarget);

	D3DXVECTOR2 vSpeed = ioStringConverter::ParseVector2( kVal.c_str() );

	pRotate->SetRotSpeed( vSpeed.x, vSpeed.y );
}
