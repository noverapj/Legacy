

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"
#include "ioRenderable.h"
#include "ioParticle.h"

#include "ioParticleIterator.h"

#include "ioParticleAffector.h"
#include "ioScaleAffector.h"

#include "ioMovableObject.h"
#include "ioParticleSystem.h"

ioScaleAffector::CmdScaleRate ioScaleAffector::m_ScaleRateCmd;

ioScaleAffector::ioScaleAffector()
{
	if( CreateParamDictionary( "ScaleAffector" ) )
	{
		AddBaseParameters();

		ioParamDictionary *pDict = GetParamDictionary();
		pDict->AddParameter( ioParameterDef( "scale_rate", PT_VECTOR_LIST ),
							 &m_ScaleRateCmd );
	}

	m_ScaleFactorList.push_back( ScaleFactor( 0.0f, 1.0f, 1.0f ) );
}

ioScaleAffector::~ioScaleAffector()
{
}

void ioScaleAffector::AffectParticle( ioParticleSystem *pSys, float fTimePerSec )
{
	if( !CheckWaitTime(fTimePerSec) )
		return;

	D3DXVECTOR2 vScale;
	ioParticle *pParticle;

	ioParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pParticle = iter.GetNext();

		vScale = GenScaleRate( pParticle->TimeRate() );

		pParticle->SetSize( pParticle->m_fBeginWidth * vScale.x,
							pParticle->m_fBeginHeight * vScale.y );
	}
}

bool ioScaleAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( !ioParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioScaleAffector *pScale = dynamic_cast< const ioScaleAffector* >( pRhs );
	
	if( m_ScaleFactorList.size() != pScale->m_ScaleFactorList.size() )
		return false;

	int iScaleFactorCnt = m_ScaleFactorList.size();
	for( int i=0 ; i<iScaleFactorCnt ; i++ )
	{
		const ScaleFactor &rkLeft  = m_ScaleFactorList[i];
		const ScaleFactor &rkRight = pScale->m_ScaleFactorList[i];

		if( rkLeft.fTimeRate != rkRight.fTimeRate ||
			rkLeft.fScaleX != rkRight.fScaleX ||
			rkLeft.fScaleY != rkRight.fScaleY )
			return false;
	}

	return true;
}

void ioScaleAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	ioParticleAffector::ParametersCopy( pSrc );

	const ioScaleAffector *pScale = dynamic_cast< const ioScaleAffector* >( pSrc );

	m_ScaleFactorList.clear();
	m_ScaleFactorList = pScale->m_ScaleFactorList;
}

const ioHashString& ioScaleAffector::GetType() const
{
	static ioHashString szType( "Scale" );
	return szType;
}

void ioScaleAffector::AddScaleRate( float fTimeRate, float fScaleX, float fScaleY )
{
	ScaleFactorList::iterator iter;
	for( iter=m_ScaleFactorList.begin() ; iter!=m_ScaleFactorList.end() ; ++iter )
	{
		if( iter->fTimeRate == fTimeRate )
		{
			iter->fScaleX = fScaleX;
			iter->fScaleY = fScaleY;
			return;
		}
	}

	m_ScaleFactorList.push_back( ScaleFactor( fTimeRate, fScaleX, fScaleY ) );

	if( m_ScaleFactorList.size() > 1 )
	{
		std::sort( m_ScaleFactorList.begin(), m_ScaleFactorList.end(), FactorSort() );
	}
}

void ioScaleAffector::AddScaleRate( float fTimeRate, const D3DXVECTOR2 &vScale )
{
	AddScaleRate( fTimeRate, vScale.x, vScale.y );
}

void ioScaleAffector::SetScaleRateList( const Vector3Vec &rkList )
{
	m_ScaleFactorList.clear();
	m_ScaleFactorList.reserve( rkList.size() );

	Vector3Vec::const_iterator iter;
	for( iter=rkList.begin() ; iter!=rkList.end() ; ++iter )
	{
		m_ScaleFactorList.push_back( ScaleFactor( iter->x, iter->y, iter->z ) );
	}
}

D3DXVECTOR2 ioScaleAffector::GenScaleRate( float fTimeRate )
{
	ScaleFactorList::iterator iterBefore, iterAfter;
	for( iterAfter=m_ScaleFactorList.begin() ; iterAfter!=m_ScaleFactorList.end() ; ++iterAfter )
	{
		if( iterAfter->fTimeRate > fTimeRate )
			break;
	}

	D3DXVECTOR2 vScale;
	if( iterAfter == m_ScaleFactorList.end() )
	{
		--iterAfter;
		vScale.x = iterAfter->fScaleX;
		vScale.y = iterAfter->fScaleY;
	}
	else if( iterAfter == m_ScaleFactorList.begin() )
	{
		vScale.x = iterAfter->fScaleX;
		vScale.y = iterAfter->fScaleY;
	}
	else
	{
		iterBefore = iterAfter;
		--iterBefore;

		float fBlend = ( fTimeRate - iterBefore->fTimeRate ) /
					   ( iterAfter->fTimeRate - iterBefore->fTimeRate );

		vScale.x = iterBefore->fScaleX * ( 1.0f - fBlend ) +
				   iterAfter->fScaleX * fBlend;

		vScale.y = iterBefore->fScaleY * ( 1.0f - fBlend ) +
				   iterAfter->fScaleY * fBlend;
	}

	return vScale;
}

float ioScaleAffector::GetScaleTimeRate( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
		return m_ScaleFactorList[iIdx].fTimeRate;

	return 0.0f;
}

D3DXVECTOR2 ioScaleAffector::GetScaleRate( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
	{
		D3DXVECTOR2 vScale;
		vScale.x = m_ScaleFactorList[iIdx].fScaleX;
		vScale.y = m_ScaleFactorList[iIdx].fScaleY;
		return vScale;
	}

	return D3DXVECTOR2( 1.0f, 1.0f );
}

void ioScaleAffector::RemoveScaleRate( int iIdx )
{
	if( COMPARE( iIdx, 0, GetScaleRateCnt() ) )
	{
		m_ScaleFactorList.erase( m_ScaleFactorList.begin() + iIdx );
	}
}

void ioScaleAffector::RemoveAllScaleRate()
{
	m_ScaleFactorList.clear();
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioScaleAffector::CmdScaleRate::Get( const void *pTarget ) const
{
	const ioScaleAffector *pScale = static_cast< const ioScaleAffector* >(pTarget);

	int iRateCnt = pScale->GetScaleRateCnt();

	std::vector< D3DXVECTOR3 > vScaleList;
	vScaleList.reserve( iRateCnt );

	D3DXVECTOR3 vScaleRate;
	D3DXVECTOR2 vScale;
	for( int i=0 ; i<iRateCnt ; i++ )
	{
		vScale = pScale->GetScaleRate( i );
		vScaleRate.x = pScale->GetScaleTimeRate( i );
		vScaleRate.y = vScale.x;
		vScaleRate.z = vScale.y;

		vScaleList.push_back( vScaleRate );
	}

	return ioStringConverter::toString( vScaleList );
}

void ioScaleAffector::CmdScaleRate::Set( void *pTarget, const std::string &kVal )
{
	ioScaleAffector *pScale = static_cast< ioScaleAffector* >(pTarget);

	Vector3Vec vScaleList;
	ioStringConverter::ParseVector3Vec( kVal.c_str(), vScaleList );

	pScale->SetScaleRateList( vScaleList );
}