

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioBipedTrailAffector.h"

#include "ioRenderable.h"

#include "ioParticle.h"
#include "ioParticleSystem.h"
#include "ioParticleIterator.h"

ioBipedTrailAffector::CmdTrailSpeed ioBipedTrailAffector::m_TrailSpeedCmd;
ioBipedTrailAffector::CmdTrailBiped ioBipedTrailAffector::m_TrailBipedCmd;
ioBipedTrailAffector::CmdForceRate  ioBipedTrailAffector::m_TrailForceCmd;

ioBipedTrailAffector::ioBipedTrailAffector()
{
	m_fTrailSpeed = 0.0f;

	if( CreateParamDictionary( "BipedTrailAffector" ) )
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

ioBipedTrailAffector::~ioBipedTrailAffector()
{
}

void ioBipedTrailAffector::AffectParticle( ioParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos = pSys->GetTrailBipedPosition( m_TrailBiped );
	float fSpeed = m_fTrailSpeed * fTimePerSec;

	ioParticle *pParticle;
	ioParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		vDir = vPos - pParticle->m_vPos;
		D3DXVec3Normalize( &vDir, &vDir );
		pParticle->m_vPos += vDir * fSpeed * GetCurForceRate( pParticle->TimeRate() );
	}
}

void ioBipedTrailAffector::ParentSystemScaled( float fScaleRate )
{
	m_fTrailSpeed *= fScaleRate;
}

bool ioBipedTrailAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioBipedTrailAffector *pBiped = dynamic_cast< const ioBipedTrailAffector* >( pRhs );
	if( m_TrailBiped != pBiped->m_TrailBiped )
		return false;

	if( m_fTrailSpeed != pBiped->m_fTrailSpeed )
		return false;

	if( m_ForceRateMap != pBiped->m_ForceRateMap )
		return false;

	return true;
}

void ioBipedTrailAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioBipedTrailAffector *pBiped = dynamic_cast< const ioBipedTrailAffector* >( pSrc );
	m_ForceRateMap = pBiped->m_ForceRateMap;
	m_TrailBiped  = pBiped->m_TrailBiped;
	m_fTrailSpeed = pBiped->m_fTrailSpeed;
}

const ioHashString& ioBipedTrailAffector::GetType() const
{
	static ioHashString szType( "Trail" );
	return szType;
}

void ioBipedTrailAffector::SetTrailSpeed( float fSpeed )
{
	m_fTrailSpeed = fSpeed;
}

void ioBipedTrailAffector::SetTrailBiped( const ioHashString &rkName )
{
	m_TrailBiped = rkName;
}

void ioBipedTrailAffector::AddForceRate( float fTime, float fRate )
{
	m_ForceRateMap.AddTimeFactor( fTime, fRate );
}

void ioBipedTrailAffector::SetForceRateList( const Vector2Vec &rkForceList )
{
	m_ForceRateMap.SetTimeFactorList( rkForceList );
}

void ioBipedTrailAffector::RemoveForceRate( int iIdx )
{
	m_ForceRateMap.RemoveFactor( iIdx );
}

void ioBipedTrailAffector::RemoveAllForceRate()
{
	m_ForceRateMap.RemoveAllFactor();
}

int ioBipedTrailAffector::GetForceRateCnt() const
{
	return m_ForceRateMap.GetFactorCnt();
}

float ioBipedTrailAffector::GetForceRateTime( int iIdx ) const
{
	return m_ForceRateMap.GetTimeRate( iIdx );
}

float ioBipedTrailAffector::GetForceRate( int iIdx ) const
{
	return m_ForceRateMap.GetFactor( iIdx );
}

float ioBipedTrailAffector::GetCurForceRate( float fTimeRate )
{
	return m_ForceRateMap.GetCurFactor( fTimeRate );
}

//---------------------------------------------------------------------------------

std::string ioBipedTrailAffector::CmdTrailSpeed::Get( const void *pTarget ) const
{
	const ioBipedTrailAffector *pTrail = static_cast<const ioBipedTrailAffector*>(pTarget);

	return ioStringConverter::toString( pTrail->GetTrailSpeed() );
}

void ioBipedTrailAffector::CmdTrailSpeed::Set( void *pTarget, const std::string &kVal )
{
	ioBipedTrailAffector *pTrail = static_cast<ioBipedTrailAffector*>(pTarget);

	pTrail->SetTrailSpeed( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioBipedTrailAffector::CmdTrailBiped::Get( const void *pTarget ) const
{
	const ioBipedTrailAffector *pTrail = static_cast<const ioBipedTrailAffector*>(pTarget);

	return pTrail->GetTrailBiped().c_str();
}

void ioBipedTrailAffector::CmdTrailBiped::Set( void *pTarget, const std::string &kVal )
{
	ioBipedTrailAffector *pTrail = static_cast<ioBipedTrailAffector*>(pTarget);
	pTrail->SetTrailBiped( kVal.c_str() );
}

std::string ioBipedTrailAffector::CmdForceRate::Get( const void *pTarget ) const
{
	const ioBipedTrailAffector *pTrail = static_cast< const ioBipedTrailAffector* >(pTarget);

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

void ioBipedTrailAffector::CmdForceRate::Set( void *pTarget, const std::string &kVal )
{
	ioBipedTrailAffector *pTrail = static_cast< ioBipedTrailAffector* >(pTarget);

	Vector2Vec vForceRateList;
	ioStringConverter::ParseVector2Vec( kVal.c_str(), vForceRateList );
	pTrail->SetForceRateList( vForceRateList );
}
