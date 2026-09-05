

#include "stdafx.h"

#include "ioMath.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioRenderable.h"
#include "ioMovableObject.h"

#include "ioParticle.h"
#include "ioParticleIterator.h"
#include "ioParticleAffector.h"
#include "ioTexRotationAffector.h"
#include "ioParticleSystem.h"

ioTexRotationAffector::CmdRotSpeed ioTexRotationAffector::m_RotSpeedCmd;
ioTexRotationAffector::CmdRotRange ioTexRotationAffector::m_RotRangeCmd;

ioTexRotationAffector::ioTexRotationAffector()
{
	m_fMinRotSpeed = 0.0f;
	m_fMaxRotSpeed = 0.0f;
	m_fRotRangeStart = 0.0f;
	m_fRotRangeEnd	 = 0.0f;

	if( CreateParamDictionary( "TexRotationAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();
		pDict->AddParameter( ioParameterDef("rot_range", PT_VECTOR2),
							 &m_RotRangeCmd );
		pDict->AddParameter( ioParameterDef("rot_speed", PT_VECTOR2),
							 &m_RotSpeedCmd );
	}
}

ioTexRotationAffector::~ioTexRotationAffector()
{
}

void ioTexRotationAffector::InitParticle( ioParticleSystem *pSys, ioParticle *pParticle )
{
	pParticle->SetTexRotation( ioMath::RangeRandom( m_fRotRangeStart, m_fRotRangeEnd ) );
	pParticle->SetTexRotationSpeed( ioMath::RangeRandom( m_fMinRotSpeed, m_fMaxRotSpeed ) );
}

void ioTexRotationAffector::AffectParticle( ioParticleSystem *pSys,
										    float fTimePerSec )
{
	if( !CheckWaitTime(fTimePerSec) )
		return;

	ioParticleIterator iter = pSys->GetIterator();

	float fNewRotation = 0.0f;
	ioParticle *pParticle;
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		pParticle->UpdateTexRotation( fTimePerSec );
	}
}

bool ioTexRotationAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioTexRotationAffector *pTexRot = dynamic_cast< const ioTexRotationAffector* >( pRhs );

	if( m_fMinRotSpeed != pTexRot->m_fMinRotSpeed )
		return false;

	if( m_fMaxRotSpeed != pTexRot->m_fMaxRotSpeed )
		return false;

	if( m_fRotRangeStart != pTexRot->m_fRotRangeStart )
		return false;

	if( m_fRotRangeEnd != pTexRot->m_fRotRangeEnd )
		return false;

	return true;
}

void ioTexRotationAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioTexRotationAffector *pTex = dynamic_cast< const ioTexRotationAffector* >( pSrc );

	m_fMinRotSpeed = pTex->m_fMinRotSpeed;
	m_fMaxRotSpeed = pTex->m_fMaxRotSpeed;

	m_fRotRangeStart = pTex->m_fRotRangeStart;
	m_fRotRangeEnd   = pTex->m_fRotRangeEnd;
}

const ioHashString& ioTexRotationAffector::GetType() const
{
	static ioHashString szType( "TexRotation" );
	return szType;
}

void ioTexRotationAffector::SetRotateSpeed( float fMin, float fMax )
{
	m_fMinRotSpeed = fMin;
	m_fMaxRotSpeed = fMax;
}

void ioTexRotationAffector::SetRotateRange( float fStart, float fEnd )
{
	m_fRotRangeStart = fStart;
	m_fRotRangeEnd   = fEnd;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioTexRotationAffector::CmdRotRange::Get( const void *pTarget ) const
{
	const ioTexRotationAffector *pTexRot = static_cast< const ioTexRotationAffector* >(pTarget);

	D3DXVECTOR2 vRange;
	vRange.x = pTexRot->GetRotateRangeStart();
	vRange.y = pTexRot->GetRotateRangeEnd();

	return ioStringConverter::toString( vRange );
}

void ioTexRotationAffector::CmdRotRange::Set( void *pTarget, const std::string &kVal )
{
	ioTexRotationAffector *pTexRot = static_cast< ioTexRotationAffector* >(pTarget);

	D3DXVECTOR2 vRange = ioStringConverter::ParseVector2( kVal.c_str() );
	pTexRot->SetRotateRange( vRange.x, vRange.y );
}

std::string ioTexRotationAffector::CmdRotSpeed::Get( const void *pTarget ) const
{
	const ioTexRotationAffector *pTexRot = static_cast< const ioTexRotationAffector* >(pTarget);

	D3DXVECTOR2 vSpeed;
	vSpeed.x = pTexRot->GetRotateMinSpeed();
	vSpeed.y = pTexRot->GetRotateMaxSpeed();

	return ioStringConverter::toString( vSpeed );
}

void ioTexRotationAffector::CmdRotSpeed::Set( void *pTarget, const std::string &kVal )
{
	ioTexRotationAffector *pTexRot = static_cast< ioTexRotationAffector* >(pTarget);

	D3DXVECTOR2 vSpeed = ioStringConverter::ParseVector2( kVal.c_str() );
	pTexRot->SetRotateSpeed( vSpeed.x, vSpeed.y );
}
