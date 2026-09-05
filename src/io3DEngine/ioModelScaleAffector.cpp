

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioMovableObject.h"
#include "ioModelParticle.h"

#include "ioModelParticleAffector.h"
#include "ioModelScaleAffector.h"

#include "ioModelParticleIterator.h"
#include "ioModelParticleSystem.h"

#include "ioMath.h"

ioModelScaleAffector::CmdScaleRate ioModelScaleAffector::m_ScaleRateCmd;

ioModelScaleAffector::ioModelScaleAffector()
{
	if( CreateParamDictionary( "ModelScaleAffector" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();
		if( pDict )
		{
			pDict->AddParameter( ioParameterDef("scale_rate", PT_VECTOR_LIST),
								 &m_ScaleRateCmd );
		}
	}
	
	AddScaleRate( 0.0f, 1.0f, 1.0f, 1.0f );
}

ioModelScaleAffector::~ioModelScaleAffector()
{
}

void ioModelScaleAffector::InitModel( ioModelParticle *pMdl )
{
	pMdl->m_vScale = GetScaleRate( 0.0f );
}

void ioModelScaleAffector::AffectModel(	ioModelParticleSystem *pSys,
										float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	ioModelParticleIterator iter = pSys->GetIterator();

	ioModelParticle *pMdl;
	while( !iter.end() )
	{
		pMdl = iter.GetNext();

		pMdl->m_vScale = GetScaleRate( pMdl->TimeRate() );
	}
}

void ioModelScaleAffector::ParentSystemScaled( float fScaleRate )
{
	ModelScaleRateMap::iterator iter;
	for( iter=m_ScaleMap.begin() ; iter!=m_ScaleMap.end() ; ++iter )
	{
		iter->second *= fScaleRate;
	}
}

bool ioModelScaleAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( !ioModelParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioModelScaleAffector *pScale = dynamic_cast< const ioModelScaleAffector * >( pRhs );
	if( m_ScaleMap.size() != pScale->m_ScaleMap.size() )
		return false;

	ModelScaleRateMap::const_iterator iter, iRhs;
	iter = m_ScaleMap.begin();
	iRhs = pScale->m_ScaleMap.begin();
	for( ; iter!=m_ScaleMap.end() ; ++iter, ++iRhs )
	{
		if( iter->first != iRhs->first )
			return false;

		if( !ioMath::IsEqual( iter->second, iRhs->second ) )
			return false;
	}

	return true;
}

void ioModelScaleAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	ioModelParticleAffector::ParametersCopy( pSrc );

	const ioModelScaleAffector *pScale = dynamic_cast< const ioModelScaleAffector* >( pSrc );

	m_ScaleMap.clear();
	m_ScaleMap = pScale->m_ScaleMap;
}

const ioHashString& ioModelScaleAffector::GetType() const
{
	static ioHashString szType( "Scale" );
	return szType;
}

void ioModelScaleAffector::AddScaleRate( float fTime, const D3DXVECTOR3 &vScale )
{
	ModelScaleRateMap::iterator iter = m_ScaleMap.find( fTime );
	if( iter == m_ScaleMap.end() )
	{
		m_ScaleMap.insert( ModelScaleRateMap::value_type(fTime, vScale) );
	}
	else
	{
		iter->second = vScale;
	}
}

void ioModelScaleAffector::AddScaleRate( float fTime,
										 float xScale,
										 float yScale,
										 float zScale )
{
	AddScaleRate( fTime, D3DXVECTOR3( xScale, yScale, zScale ) );	
}

void ioModelScaleAffector::RemoveScaleRate( int iIdx )
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
	{
		ModelScaleRateMap::iterator iter = m_ScaleMap.begin();
		std::advance( iter, iIdx );
		m_ScaleMap.erase( iter );
	}

	LOG.PrintTimeAndLog( 0, "ioModelScaleAffector::RemoveScaleRate - over %d/%d",
							 iIdx, GetScaleRateCnt() );
}

void ioModelScaleAffector::RemoveAllScaleRate()
{
	m_ScaleMap.clear();
}

D3DXVECTOR3 ioModelScaleAffector::GetScaleRate( float fTime ) const
{
	ModelScaleRateMap::const_iterator iterBefore, iterAfter;

	iterAfter = m_ScaleMap.upper_bound( fTime );

	if( iterAfter == m_ScaleMap.begin() )
	{
		return iterAfter->second;
	}

	if( iterAfter == m_ScaleMap.end() )
	{
		--iterAfter;
		return iterAfter->second;
	}

	iterBefore = iterAfter;
	iterBefore--;

	float fBlend;
	fBlend = fTime - iterBefore->first;
	fBlend /= iterAfter->first - iterBefore->first;

	return ioMath::Lerp( fBlend, iterBefore->second, iterAfter->second );
}

D3DXVECTOR3 ioModelScaleAffector::GetScaleRateByIdx( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
	{
		ModelScaleRateMap::const_iterator iter = m_ScaleMap.begin();
		std::advance( iter, iIdx );
		return iter->second;
	}

	LOG.PrintTimeAndLog( 0, "ioModelScaleAffector::GetScaleRate - over %d/%d",
							 iIdx, GetScaleRateCnt() );

	return ioMath::VEC3_ZERO;
}

float ioModelScaleAffector::GetScaleRateTime( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
	{
		ModelScaleRateMap::const_iterator iter = m_ScaleMap.begin();
		std::advance( iter, iIdx );
		return iter->first;
	}

	return 0.0f;
}

int ioModelScaleAffector::GetScaleRateCnt() const
{
	return m_ScaleMap.size();
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelScaleAffector::CmdScaleRate::Get( const void *pTarget ) const
{
	const ioModelScaleAffector *pScale = static_cast< const ioModelScaleAffector* >(pTarget);

	int iScaleCnt = pScale->GetScaleRateCnt();

	std::vector< D3DXVECTOR4 > vScaleList;
	vScaleList.reserve( iScaleCnt );

	D3DXVECTOR4 vScale;
	D3DXVECTOR3 vScaleRate;
	for( int i=0 ; i<iScaleCnt ; i++ )
	{
		vScaleRate = pScale->GetScaleRateByIdx( i );

		vScale.x = pScale->GetScaleRateTime( i );
		vScale.y = vScaleRate.x;
		vScale.z = vScaleRate.y;
		vScale.w = vScaleRate.z;

		vScaleList.push_back( vScale );
	}

	return ioStringConverter::toString( vScaleList );
}

void ioModelScaleAffector::CmdScaleRate::Set( void *pTarget, const std::string &kVal )
{
	ioModelScaleAffector *pScale = static_cast< ioModelScaleAffector* >(pTarget);

	std::vector< D3DXVECTOR4 > vScaleList;
	ioStringConverter::ParseVector4Vec( kVal.c_str(), vScaleList );

	pScale->RemoveAllScaleRate();

	D3DXVECTOR4 vScale;

	int iScaleCnt = vScaleList.size();

	for( int i=0 ; i<iScaleCnt ; i++ )
	{
		vScale = vScaleList[i];

		pScale->AddScaleRate( vScale.x, vScale.y, vScale.z, vScale.w );
	}
}
