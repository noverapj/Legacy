

#include "stdafx.h"

#include "ioMath.h"
#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioRenderable.h"

#include "ioParticleEmitter.h"
#include "ioParticleAffector.h"
#include "ioEmitAffector.h"

#include "ioParticleIterator.h"

#include "ioMovableObject.h"

#include "ioParticle.h"
#include "ioParticleSystem.h"

ioEmitAffector::CmdEmitPower ioEmitAffector::m_EmitPowerCmd;
ioEmitAffector::CmdForceRate ioEmitAffector::m_ForceRateCmd;
ioEmitAffector::CmdDirectionChange ioEmitAffector::m_DirectionChangeCmd;

ioEmitAffector::ioEmitAffector()
{
	m_fPower = 0.0f;
	m_bDirectionChange = false;

	if( CreateParamDictionary( "EmitAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			pDict->AddParameter( ioParameterDef("power", PT_FLOAT),
								 &m_EmitPowerCmd );

			pDict->AddParameter( ioParameterDef("force_rate", PT_STRING),
								 &m_ForceRateCmd );

			pDict->AddParameter( ioParameterDef("change_direction", PT_BOOL),
								 &m_DirectionChangeCmd );
		}
	}
}

ioEmitAffector::~ioEmitAffector()
{
}

void ioEmitAffector::InitParticle( ioParticleSystem *pSys, ioParticle *pParticle )
{
	if( m_bDirectionChange )
	{
		D3DXVECTOR3 vDir = pParticle->m_vPos - pSys->GetEmitPosition();
		D3DXVec3Normalize( &pParticle->m_vDir, &vDir );
	}
}

void ioEmitAffector::AffectParticle( ioParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos = pSys->GetEmitPosition();
	float fPower = m_fPower * fTimePerSec;

	ioParticle *pParticle;
	ioParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		vDir = pParticle->m_vPos - vPos;
		D3DXVec3Normalize( &vDir, &vDir );
		
		pParticle->m_vPos += vDir * fPower * GetCurForceRate( pParticle->TimeRate() );
	}
}

void ioEmitAffector::ParentSystemScaled( float fScaleRate )
{
	m_fPower *= fScaleRate;
}

bool ioEmitAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioEmitAffector *pEmit = dynamic_cast< const ioEmitAffector *>( pRhs );

	if( m_ForceRateMap != pEmit->m_ForceRateMap )
		return false;

	if( m_fPower != pEmit->m_fPower )
		return false;

	if( m_bDirectionChange != pEmit->m_bDirectionChange )
		return false;

	return true;
}

void ioEmitAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioEmitAffector *pEmit = dynamic_cast< const ioEmitAffector* >( pSrc );
	m_ForceRateMap = pEmit->m_ForceRateMap;
	m_fPower = pEmit->m_fPower;

	m_bDirectionChange = pEmit->m_bDirectionChange;
}

const ioHashString& ioEmitAffector::GetType() const
{
	static ioHashString szType( "Emit" );
	return szType;
}

void ioEmitAffector::AddForceRate( float fTime, float fRate )
{
	m_ForceRateMap.AddTimeFactor( fTime, fRate );
}

void ioEmitAffector::SetForceRateList( const Vector2Vec &rkForceList )
{
	m_ForceRateMap.SetTimeFactorList( rkForceList );
}

void ioEmitAffector::RemoveForceRate( int iIdx )
{
	m_ForceRateMap.RemoveFactor( iIdx );
}

void ioEmitAffector::RemoveAllForceRate()
{
	m_ForceRateMap.RemoveAllFactor();
}

int ioEmitAffector::GetForceRateCnt() const
{
	return m_ForceRateMap.GetFactorCnt();
}

float ioEmitAffector::GetForceRateTime( int iIdx ) const
{
	return m_ForceRateMap.GetTimeRate( iIdx );
}

float ioEmitAffector::GetForceRate( int iIdx ) const
{
	return m_ForceRateMap.GetFactor( iIdx );
}

float ioEmitAffector::GetCurForceRate( float fTimeRate )
{
	return m_ForceRateMap.GetCurFactor( fTimeRate );
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioEmitAffector::CmdEmitPower::Get( const void *pTarget ) const
{
	const ioEmitAffector* pEmit = static_cast<const ioEmitAffector*>(pTarget);

	return ioStringConverter::toString( pEmit->GetPower() );
}

void ioEmitAffector::CmdEmitPower::Set( void *pTarget, const std::string &kVal )
{
	ioEmitAffector* pEmit = static_cast<ioEmitAffector*>(pTarget);

	pEmit->SetPower( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

std::string ioEmitAffector::CmdForceRate::Get( const void *pTarget ) const
{
	const ioEmitAffector *pEmit = static_cast< const ioEmitAffector* >(pTarget);

	int iForceRateCnt = pEmit->GetForceRateCnt();

	D3DXVECTOR2 vForceRate;
	Vector2Vec vForceRateList;
	vForceRateList.reserve( iForceRateCnt );

	for( int i=0 ; i<iForceRateCnt ; i++ )
	{
		vForceRate.x = pEmit->GetForceRateTime(i);
		vForceRate.y = pEmit->GetForceRate(i);

		vForceRateList.push_back( vForceRate );
	}

	return ioStringConverter::toString( vForceRateList );
}

void ioEmitAffector::CmdForceRate::Set( void *pTarget, const std::string &kVal )
{
	ioEmitAffector *pEmit = static_cast< ioEmitAffector* >(pTarget);

	Vector2Vec vForceRateList;
	ioStringConverter::ParseVector2Vec( kVal.c_str(), vForceRateList );
	pEmit->SetForceRateList( vForceRateList );
}

std::string ioEmitAffector::CmdDirectionChange::Get( const void *pTarget ) const
{
	const ioEmitAffector* pEmit = static_cast<const ioEmitAffector*>(pTarget);

	return ioStringConverter::toString( pEmit->IsDirectionChange() );
}

void ioEmitAffector::CmdDirectionChange::Set( void *pTarget, const std::string &kVal )
{
	ioEmitAffector* pEmit = static_cast<ioEmitAffector*>(pTarget);

	pEmit->SetDirectionChange( ioStringConverter::ParseBool( kVal.c_str() ) );
}
