

#include "stdafx.h"

#include "ioTimeRateFactor.h"

ioTimeRateFactor::ioTimeRateFactor()
{
	m_TimeFactorList.reserve( 10 );

	AddTimeFactor( 0.0f, 1.0f );
}

ioTimeRateFactor::~ioTimeRateFactor()
{
}

void ioTimeRateFactor::AddTimeFactor( float fTimeRate, float fFactor )
{
	fTimeRate = max( 0.0f, min( fTimeRate, 1.0f ) );
	
	TimeFactorList::iterator iter;
	for( iter=m_TimeFactorList.begin() ; iter!=m_TimeFactorList.end() ; ++iter )
	{
		if( iter->fTimeRate == fTimeRate )
		{
			iter->fFactor = fFactor;
			return;
		}
	}

	m_TimeFactorList.push_back( TimeFactor( fTimeRate, fFactor ) );

	if( m_TimeFactorList.size() > 1 )
	{
		std::sort( m_TimeFactorList.begin(), m_TimeFactorList.end(), FactorSort() );
	}
}

void ioTimeRateFactor::SetFactor( int iIdx, float fFactor )
{
	if( COMPARE( iIdx, 0, GetFactorCnt() ) )
	{
		m_TimeFactorList[iIdx].fFactor = fFactor;
	}
}

void ioTimeRateFactor::RemoveFactor( int iIdx )
{
	if( COMPARE( iIdx, 0, GetFactorCnt() ) )
	{
		m_TimeFactorList.erase( m_TimeFactorList.begin() + iIdx );
	}
}

void ioTimeRateFactor::RemoveAllFactor()
{
	m_TimeFactorList.clear();
}

int ioTimeRateFactor::GetFactorCnt() const
{
	return m_TimeFactorList.size();
}

float ioTimeRateFactor::GetFactor( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetFactorCnt() ) )
		return m_TimeFactorList[iIdx].fFactor;

	return 0.0f;
}

float ioTimeRateFactor::GetTimeRate( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetFactorCnt() ) )
		return m_TimeFactorList[iIdx].fTimeRate;

	return 0.0f;
}

float ioTimeRateFactor::GetCurFactor( float fTimeRate ) const
{
	if( m_TimeFactorList.empty() )
		return 1.0f;

	float fFactor = 1.0f;

	TimeFactorList::const_iterator iterBefore, iterAfter;
	for( iterAfter=m_TimeFactorList.begin() ; iterAfter!=m_TimeFactorList.end() ; ++iterAfter )
	{
		if( iterAfter->fTimeRate > fTimeRate )
			break;
	}

	if( iterAfter == m_TimeFactorList.end() )
	{
		--iterAfter;
		fFactor = iterAfter->fFactor;
	}
	else if( iterAfter == m_TimeFactorList.begin() )
	{
		fFactor = iterAfter->fFactor;
	}
	else
	{
		iterBefore = iterAfter;
		--iterBefore;

		float fBlend;
		fBlend = fTimeRate - iterBefore->fTimeRate;
		fBlend /= iterAfter->fTimeRate - iterBefore->fTimeRate;

		fFactor = iterBefore->fFactor * ( 1.0f - fBlend ) +
				  iterAfter->fFactor * fBlend;
	}

	return fFactor;
}

ioTimeRateFactor& ioTimeRateFactor::operator=( const ioTimeRateFactor &rhs )
{
	m_TimeFactorList.erase( m_TimeFactorList.begin(), m_TimeFactorList.end() );
	m_TimeFactorList = rhs.m_TimeFactorList;

	return *this;
}

bool ioTimeRateFactor::operator==( const ioTimeRateFactor &rhs ) const
{
	if( m_TimeFactorList.size() != rhs.m_TimeFactorList.size() )
		return false;

	int iFactorCnt = m_TimeFactorList.size();
	for( int i=0 ; i<iFactorCnt ; i++ )
	{
		if( m_TimeFactorList[i].fTimeRate != rhs.m_TimeFactorList[i].fTimeRate )
			return false;

		if( m_TimeFactorList[i].fFactor != rhs.m_TimeFactorList[i].fFactor )
			return false;
	}

	return true;
}

bool ioTimeRateFactor::operator!=( const ioTimeRateFactor &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

void ioTimeRateFactor::SetTimeFactorList( const Vector2Vec &rkList )
{
	m_TimeFactorList.erase( m_TimeFactorList.begin(), m_TimeFactorList.end() );
	m_TimeFactorList.reserve( rkList.size() );

	Vector2Vec::const_iterator iter;
	for( iter=rkList.begin() ; iter!=rkList.end() ; ++iter )
	{
		m_TimeFactorList.push_back( TimeFactor( iter->x, iter->y ) );
	}
}

void ioTimeRateFactor::GetTimeFactorList( Vector2Vec &rkList ) const
{
	rkList.clear();
	rkList.reserve( m_TimeFactorList.size() );

	TimeFactorList::const_iterator iter;
	for( iter=m_TimeFactorList.begin() ; iter!=m_TimeFactorList.end() ; ++iter )
	{
		rkList.push_back( D3DXVECTOR2( iter->fTimeRate, iter->fFactor ) );
	}
}
