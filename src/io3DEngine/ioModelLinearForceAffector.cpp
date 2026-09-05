

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioMovableObject.h"

#include "ioModelParticle.h"
#include "ioModelParticleAffector.h"
#include "ioModelLinearForceAffector.h"

#include "ioModelParticleIterator.h"
#include "ioModelParticleSystem.h"

#include "HelpFunc.h"
#include "ioMath.h"

ioModelLinearForceAffector::CmdForceVector ioModelLinearForceAffector::m_ForceVectorCmd;
ioModelLinearForceAffector::CmdForceRate   ioModelLinearForceAffector::m_ForceRateCmd;

ioModelLinearForceAffector::ioModelLinearForceAffector() : m_vForceDir(0.0f,-100.0f,0.0f)
{
	if( CreateParamDictionary( "ModelLinearForceAffector" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			pDict->AddParameter( ioParameterDef( "force_dir", PT_VECTOR3 ),
								 &m_ForceVectorCmd );
			
			pDict->AddParameter( ioParameterDef( "force_rate", PT_STRING ),
								 &m_ForceRateCmd );
		}
	}
}

ioModelLinearForceAffector::~ioModelLinearForceAffector()
{
}

void ioModelLinearForceAffector::AffectModel( ioModelParticleSystem *pSys,
											  float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	
	D3DXVECTOR3 vAddDir;
	D3DXVECTOR3 vForceDir = m_vForceDir * fTimePerSec;

	ioModelParticle *pMdl;
	ioModelParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pMdl = iter.GetNext();

		vAddDir = pMdl->m_vDir * pMdl->m_fMoveSpeed + 
				  vForceDir * GetCurForceRate( pMdl->TimeRate() );

		D3DXVec3Normalize( &pMdl->m_vDir, &vAddDir );
		pMdl->m_fMoveSpeed = D3DXVec3Length( &vAddDir );
	}
}

void ioModelLinearForceAffector::ParentSystemScaled( float fScaleRate )
{
	m_vForceDir *= fScaleRate;
}

void ioModelLinearForceAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
	float fForce = D3DXVec3Length( &m_vForceDir );

	m_vForceDir = qtRotate * m_vForceDir;
	D3DXVec3Normalize( &m_vForceDir, &m_vForceDir );
	m_vForceDir *= fForce;
}

bool ioModelLinearForceAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( !ioModelParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioModelLinearForceAffector *pLinear = dynamic_cast< const ioModelLinearForceAffector* >( pRhs );
	if( m_ForceRateMap != pLinear->m_ForceRateMap )
		return false;

	if( !ioMath::IsEqual( m_vForceDir, pLinear->m_vForceDir ) )
		return false;

	return true;
}

void ioModelLinearForceAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	ioModelParticleAffector::ParametersCopy( pSrc );

	const ioModelLinearForceAffector *pLinear = dynamic_cast< const ioModelLinearForceAffector* >( pSrc );

	m_ForceRateMap = pLinear->m_ForceRateMap;
	m_vForceDir = pLinear->m_vForceDir;
}

const ioHashString& ioModelLinearForceAffector::GetType() const
{
	static ioHashString szType( "Linear" );
	return szType;
}

void ioModelLinearForceAffector::AddForceRate( float fTime, float fRate )
{
	m_ForceRateMap.AddTimeFactor( fTime, fRate );
}

void ioModelLinearForceAffector::SetForceRateList( const Vector2Vec &rkForceList )
{
	m_ForceRateMap.SetTimeFactorList( rkForceList );
}

void ioModelLinearForceAffector::RemoveForceRate( int iIdx )
{
	m_ForceRateMap.RemoveFactor( iIdx );
}

void ioModelLinearForceAffector::RemoveAllForceRate()
{
	m_ForceRateMap.RemoveAllFactor();
}

int ioModelLinearForceAffector::GetForceRateCnt() const
{
	return m_ForceRateMap.GetFactorCnt();
}

float ioModelLinearForceAffector::GetForceRateTime( int iIdx ) const
{
	return m_ForceRateMap.GetTimeRate( iIdx );
}

float ioModelLinearForceAffector::GetForceRate( int iIdx ) const
{
	return m_ForceRateMap.GetFactor( iIdx );
}

float ioModelLinearForceAffector::GetCurForceRate( float fTimeRate )
{
	return m_ForceRateMap.GetCurFactor( fTimeRate );
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelLinearForceAffector::CmdForceVector::Get( const void *pTarget ) const
{
	const ioModelLinearForceAffector* pLinear = static_cast<const ioModelLinearForceAffector*>(pTarget);

	return ioStringConverter::toString( pLinear->GetForceDir() );
}

void ioModelLinearForceAffector::CmdForceVector::Set( void *pTarget,
													  const std::string &kVal )
{
	ioModelLinearForceAffector* pLinear = static_cast<ioModelLinearForceAffector*>(pTarget);

	pLinear->SetForceDir( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

std::string ioModelLinearForceAffector::CmdForceRate::Get( const void *pTarget ) const
{
	const ioModelLinearForceAffector *pLinear = static_cast< const ioModelLinearForceAffector* >(pTarget);

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

void ioModelLinearForceAffector::CmdForceRate::Set( void *pTarget, const std::string &kVal )
{
	ioModelLinearForceAffector *pLinear = static_cast< ioModelLinearForceAffector* >(pTarget);

	Vector2Vec vForceRateList;
	ioStringConverter::ParseVector2Vec( kVal.c_str(), vForceRateList );
	pLinear->SetForceRateList( vForceRateList );
}
