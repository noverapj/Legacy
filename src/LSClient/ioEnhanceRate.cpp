

#include "stdafx.h"
#include "ioEnhanceRate.h"

ioEnhanceRate::ioEnhanceRate()
{
	m_dwCurID = 0;
	m_EnhanceList.clear();
}

ioEnhanceRate::~ioEnhanceRate()
{
	m_EnhanceList.clear();
}

DWORD ioEnhanceRate::AddEnhanceRate( EnhanceOwnerType eType, float fRate )
{
	m_dwCurID++;

	EnhanceRate kRate;
	kRate.m_dwID = m_dwCurID;
	kRate.m_Type = eType;
	kRate.m_fEnhanceRate = fRate;

	m_EnhanceList.push_back( kRate );

	return m_dwCurID;
}

void ioEnhanceRate::RemoveEnhanceRate( EnhanceOwnerType eType, DWORD dwID )
{
	if( dwID == 0 ) return;

	EnhanceRateList::iterator iter = m_EnhanceList.begin();
	while( iter != m_EnhanceList.end() )
	{
		if( iter->m_dwID == dwID )
			iter = m_EnhanceList.erase( iter );
		else
			++iter;
	}
}

void ioEnhanceRate::ResetEnhanceRateList()
{
	m_EnhanceList.erase( m_EnhanceList.begin(), m_EnhanceList.end() );
	m_dwCurID = 1;
}

bool ioEnhanceRate::HasEnhanceRate( EnhanceOwnerType eType ) const
{
	EnhanceRateList::const_iterator iter;
	for( iter=m_EnhanceList.begin() ; iter!=m_EnhanceList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			return true;
	}

	return false;
}

float ioEnhanceRate::GetEnhanceRate( EnhanceOwnerType eType ) const
{
	float fRate = FLOAT1;
	EnhanceRateList::const_iterator iter;
	for( iter=m_EnhanceList.begin() ; iter!=m_EnhanceList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			fRate = iter->m_fEnhanceRate;
	}

	return fRate;
}

float ioEnhanceRate::GetTotalEnhanceRate( EnhanceOwnerType eType ) const
{
	float fRate = FLOAT1;
	EnhanceRateList::const_iterator iter;
	for( iter=m_EnhanceList.begin() ; iter!=m_EnhanceList.end() ; ++iter )
	{
		if( iter->m_Type == eType )
			fRate *= iter->m_fEnhanceRate;
	}

	return fRate;
}
