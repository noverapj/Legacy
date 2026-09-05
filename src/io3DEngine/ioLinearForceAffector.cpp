

#include "stdafx.h"

#include "ioStringConverter.h"
#include "ioStringInterface.h"

#include "ioRenderable.h"
#include "ioMovableObject.h"
#include "ioParticle.h"

#include "ioParticleIterator.h"

#include "ioParticleAffector.h"
#include "ioLinearForceAffector.h"

#include "ioParticleSystem.h"
#include "HelpFunc.h"
#include "ioMath.h"

ioLinearForceAffector::CmdForceVector ioLinearForceAffector::m_ForceVectorCmd;
ioLinearForceAffector::CmdForceMethod ioLinearForceAffector::m_ForceMethodCmd;
ioLinearForceAffector::CmdForceRate	  ioLinearForceAffector::m_ForceRateCmd;

ioLinearForceAffector::ioLinearForceAffector() 
: m_vForceDir( 0.0f, -100.0f, 0.0f ), m_eForceMethod( FORCE_ADD )
{
	if( CreateParamDictionary( "LinearForceAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();
		pDict->AddParameter( ioParameterDef("force_vector", PT_VECTOR3),
							 &m_ForceVectorCmd );

		pDict->AddParameter( ioParameterDef("force_method", PT_UNSIGNED_INT),
							 &m_ForceMethodCmd );

		pDict->AddParameter( ioParameterDef("force_rate", PT_STRING),
							 &m_ForceRateCmd );
	}
}

ioLinearForceAffector::~ioLinearForceAffector()
{
}

void ioLinearForceAffector::AffectParticle(	ioParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime(fTimePerSec) )
		return;

	D3DXVECTOR3 vAddDir;
	D3DXVECTOR3 vCurForce = m_vForceDir * fTimePerSec;

	ioParticle *pParticle;
	ioParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		vAddDir = pParticle->m_vDir * pParticle->m_fMoveSpeed +
				  vCurForce * GetCurForceRate( pParticle->TimeRate() );

		D3DXVec3Normalize( &pParticle->m_vDir, &vAddDir );
		pParticle->m_fMoveSpeed = D3DXVec3Length( &vAddDir );

		if( m_eForceMethod == FORCE_AVERAGE )
			pParticle->m_fMoveSpeed *= FLOAT05;
	}
}

void ioLinearForceAffector::ParentSystemScaled( float fScaleRate )
{
	m_vForceDir *= fScaleRate;
}

void ioLinearForceAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
	float fForce = D3DXVec3Length( &m_vForceDir );

	m_vForceDir = qtRotate * m_vForceDir;
	D3DXVec3Normalize( &m_vForceDir, &m_vForceDir );
	m_vForceDir *= fForce;
}

bool ioLinearForceAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioLinearForceAffector *pLinear = dynamic_cast< const ioLinearForceAffector * >( pRhs );

	if( !ioMath::IsEqual( m_vForceDir, pLinear->m_vForceDir ) )
		return false;

	if( m_eForceMethod != pLinear->m_eForceMethod )
		return false;

	if( m_ForceRateMap != pLinear->m_ForceRateMap )
		return false;

	return true;
}

void ioLinearForceAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioLinearForceAffector *pLinear = dynamic_cast< const ioLinearForceAffector* >( pSrc );
	m_vForceDir = pLinear->m_vForceDir;
	m_eForceMethod = pLinear->m_eForceMethod;
	m_ForceRateMap = pLinear->m_ForceRateMap;
}

const ioHashString& ioLinearForceAffector::GetType() const
{
	static ioHashString szType( "Linear" );
	return szType;
}

void ioLinearForceAffector::SetForceDir( const D3DXVECTOR3 &vForceDir )
{
	m_vForceDir = vForceDir;
}

void ioLinearForceAffector::SetForceDir( float fForceX, float fForceY, float fForceZ )
{
	m_vForceDir.x = fForceX;
	m_vForceDir.y = fForceY;
	m_vForceDir.z = fForceZ;
}

void ioLinearForceAffector::SetForceMethod( ForceMethod eMethod )
{
	m_eForceMethod = eMethod;
}

void ioLinearForceAffector::AddForceRate( float fTime, float fRate )
{
	m_ForceRateMap.AddTimeFactor( fTime, fRate );
}

void ioLinearForceAffector::SetForceRateList( const Vector2Vec &rkForceList )
{
	m_ForceRateMap.SetTimeFactorList( rkForceList );
}

void ioLinearForceAffector::RemoveForceRate( int iIdx )
{
	m_ForceRateMap.RemoveFactor( iIdx );
}

void ioLinearForceAffector::RemoveAllForceRate()
{
	m_ForceRateMap.RemoveAllFactor();
}

int ioLinearForceAffector::GetForceRateCnt() const
{
	return m_ForceRateMap.GetFactorCnt();
}

float ioLinearForceAffector::GetForceRateTime( int iIdx ) const
{
	return m_ForceRateMap.GetTimeRate( iIdx );
}

float ioLinearForceAffector::GetForceRate( int iIdx ) const
{
	return m_ForceRateMap.GetFactor( iIdx );
}

float ioLinearForceAffector::GetCurForceRate( float fTimeRate )
{
	return m_ForceRateMap.GetCurFactor( fTimeRate );
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioLinearForceAffector::CmdForceVector::Get( const void *pTarget ) const
{
	const ioLinearForceAffector *pLinear = static_cast< const ioLinearForceAffector* >(pTarget);

	return ioStringConverter::toString( pLinear->GetForceDir() );
}

void ioLinearForceAffector::CmdForceVector::Set( void *pTarget, const std::string &kVal )
{
	ioLinearForceAffector *pLinear = static_cast< ioLinearForceAffector* >(pTarget);

	pLinear->SetForceDir( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioLinearForceAffector::CmdForceMethod::Get( const void *pTarget ) const
{
	const ioLinearForceAffector *pLinear = static_cast< const ioLinearForceAffector* >(pTarget);

	switch( pLinear->GetForceMethod() )
	{
	case FORCE_ADD:
		return "add";
		break;
	case FORCE_AVERAGE:
		return "average";
		break;
	}

	return "";
}

void ioLinearForceAffector::CmdForceMethod::Set( void *pTarget, const std::string &kVal )
{
	if( kVal == "add" )
	{
		static_cast<ioLinearForceAffector*>(pTarget)->SetForceMethod( FORCE_ADD );
	}
	else if( kVal == "average" )
	{
		static_cast<ioLinearForceAffector*>(pTarget)->SetForceMethod( FORCE_AVERAGE );
	}
}

std::string ioLinearForceAffector::CmdForceRate::Get( const void *pTarget ) const
{
	const ioLinearForceAffector *pLinear = static_cast< const ioLinearForceAffector* >(pTarget);

	int iForceRateCnt = pLinear->GetForceRateCnt();

	D3DXVECTOR2 vForceRate;
	Vector2Vec vForceRateList;
	vForceRateList.reserve( iForceRateCnt );

	for( int i=0 ; i<iForceRateCnt ; i++ )
	{
		vForceRate.x = pLinear->GetForceRateTime(i);
		vForceRate.y = pLinear->GetForceRate(i);

		vForceRateList.push_back( vForceRate );
	}

	return ioStringConverter::toString( vForceRateList );
}

void ioLinearForceAffector::CmdForceRate::Set( void *pTarget, const std::string &kVal )
{
	ioLinearForceAffector *pLinear = static_cast< ioLinearForceAffector* >(pTarget);

	Vector2Vec vForceRateList;
	ioStringConverter::ParseVector2Vec( kVal.c_str(), vForceRateList );
	pLinear->SetForceRateList( vForceRateList );
}

