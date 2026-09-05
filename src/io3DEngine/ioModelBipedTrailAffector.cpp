

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioModelBipedTrailAffector.h"

#include "ioRenderable.h"

#include "ioMovableObject.h"
#include "ioModelParticle.h"
#include "ioModelParticleSystem.h"
#include "ioModelParticleIterator.h"

ioModelBipedTrailAffector::CmdTrailSpeed ioModelBipedTrailAffector::m_TrailSpeedCmd;
ioModelBipedTrailAffector::CmdTrailBiped ioModelBipedTrailAffector::m_TrailBipedCmd;
ioModelBipedTrailAffector::CmdForceRate  ioModelBipedTrailAffector::m_TrailForceCmd;

ioModelBipedTrailAffector::ioModelBipedTrailAffector()
{
	m_fTrailSpeed = 0.0f;

	if( CreateParamDictionary( "ModelBipedTrailAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			pDict->AddParameter( ioParameterDef( "trail_speed", PT_FLOAT ),
								 &m_TrailSpeedCmd );
			pDict->AddParameter( ioParameterDef( "trail_biped", PT_STRING ),
								 &m_TrailBipedCmd );
			pDict->AddParameter( ioParameterDef("force_rate", PT_STRING),
								 &m_TrailForceCmd );
		}
	}
}

ioModelBipedTrailAffector::~ioModelBipedTrailAffector()
{
}

void ioModelBipedTrailAffector::AffectModel( ioModelParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos = pSys->GetTrailBipedPosition( m_TrailBiped );
	float fSpeed = m_fTrailSpeed * fTimePerSec;

	ioModelParticle *pMdl;
	ioModelParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pMdl = iter.GetNext();

		vDir = vPos - pMdl->m_vPosition;
		D3DXVec3Normalize( &vDir, &vDir );
		pMdl->m_vPosition += vDir * fSpeed * GetCurForceRate( pMdl->TimeRate() );
	}
}

void ioModelBipedTrailAffector::ParentSystemScaled( float fScaleRate )
{
	m_fTrailSpeed *= fScaleRate;
}

bool ioModelBipedTrailAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( !ioModelParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioModelBipedTrailAffector *pBiped = dynamic_cast< const ioModelBipedTrailAffector* >( pRhs );

	if( m_ForceRateMap != pBiped->m_ForceRateMap )
		return false;

	if( m_TrailBiped != pBiped->m_TrailBiped )
		return false;

	if( m_fTrailSpeed != pBiped->m_fTrailSpeed )
		return false;

	return true;
}

void ioModelBipedTrailAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	ioModelParticleAffector::ParametersCopy( pSrc );

	const ioModelBipedTrailAffector *pBiped = dynamic_cast< const ioModelBipedTrailAffector* >( pSrc );

	m_ForceRateMap = pBiped->m_ForceRateMap;
	m_TrailBiped  = pBiped->m_TrailBiped;
	m_fTrailSpeed = pBiped->m_fTrailSpeed;
}

const ioHashString& ioModelBipedTrailAffector::GetType() const
{
	static ioHashString szType( "Trail" );
	return szType;
}

void ioModelBipedTrailAffector::SetTrailSpeed( float fSpeed )
{
	m_fTrailSpeed = fSpeed;
}

void ioModelBipedTrailAffector::SetTrailBiped( const ioHashString &rkName )
{
	m_TrailBiped = rkName;
}

void ioModelBipedTrailAffector::AddForceRate( float fTime, float fRate )
{
	m_ForceRateMap.AddTimeFactor( fTime, fRate );
}

void ioModelBipedTrailAffector::SetForceRateList( const Vector2Vec &rkForceList )
{
	m_ForceRateMap.SetTimeFactorList( rkForceList );
}

void ioModelBipedTrailAffector::RemoveForceRate( int iIdx )
{
	m_ForceRateMap.RemoveFactor( iIdx );
}

void ioModelBipedTrailAffector::RemoveAllForceRate()
{
	m_ForceRateMap.RemoveAllFactor();
}

int ioModelBipedTrailAffector::GetForceRateCnt() const
{
	return m_ForceRateMap.GetFactorCnt();
}

float ioModelBipedTrailAffector::GetForceRateTime( int iIdx ) const
{
	return m_ForceRateMap.GetTimeRate( iIdx );
}

float ioModelBipedTrailAffector::GetForceRate( int iIdx ) const
{
	return m_ForceRateMap.GetFactor( iIdx );
}

float ioModelBipedTrailAffector::GetCurForceRate( float fTimeRate )
{
	return m_ForceRateMap.GetCurFactor( fTimeRate );
}

//---------------------------------------------------------------------------------

std::string ioModelBipedTrailAffector::CmdTrailSpeed::Get( const void *pTarget ) const
{
	const ioModelBipedTrailAffector *pTrail = static_cast<const ioModelBipedTrailAffector*>(pTarget);

	return ioStringConverter::toString( pTrail->GetTrailSpeed() );
}

void ioModelBipedTrailAffector::CmdTrailSpeed::Set( void *pTarget, const std::string &kVal )
{
	ioModelBipedTrailAffector *pTrail = static_cast<ioModelBipedTrailAffector*>(pTarget);

	pTrail->SetTrailSpeed( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioModelBipedTrailAffector::CmdTrailBiped::Get( const void *pTarget ) const
{
	const ioModelBipedTrailAffector *pTrail = static_cast<const ioModelBipedTrailAffector*>(pTarget);

	return pTrail->GetTrailBiped().c_str();
}

void ioModelBipedTrailAffector::CmdTrailBiped::Set( void *pTarget, const std::string &kVal )
{
	ioModelBipedTrailAffector *pTrail = static_cast<ioModelBipedTrailAffector*>(pTarget);
	pTrail->SetTrailBiped( kVal.c_str() );
}

std::string ioModelBipedTrailAffector::CmdForceRate::Get( const void *pTarget ) const
{
	const ioModelBipedTrailAffector *pTrail = static_cast< const ioModelBipedTrailAffector* >(pTarget);

	int iForceRateCnt = pTrail->GetForceRateCnt();

	D3DXVECTOR2 vForceRate;
	Vector2Vec vForceRateList;
	vForceRateList.reserve( iForceRateCnt );

	for( int i=0 ; i<iForceRateCnt ; i++ )
	{
		vForceRate.x = pTrail->GetForceRateTime(i);
		vForceRate.y = pTrail->GetForceRate(i);

		vForceRateList.push_back( vForceRate );
	}

	return ioStringConverter::toString( vForceRateList );
}

void ioModelBipedTrailAffector::CmdForceRate::Set( void *pTarget, const std::string &kVal )
{
	ioModelBipedTrailAffector *pTrail = static_cast< ioModelBipedTrailAffector* >(pTarget);

	Vector2Vec vForceRateList;
	ioStringConverter::ParseVector2Vec( kVal.c_str(), vForceRateList );
	pTrail->SetForceRateList( vForceRateList );
}

