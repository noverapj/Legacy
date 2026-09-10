

#include "stdafx.h"

#include "DamageTable.h"

DamageTable::DamageTable()
{
}

DamageTable::~DamageTable()
{
}

void DamageTable::AddDamage( const ioHashString &rkAttacker, int iDamage )
{
	if( rkAttacker.IsEmpty() || iDamage <= 0 )
		return;

	DamageTableMap::iterator iter = m_DamageTable.find( rkAttacker );
	if( iter != m_DamageTable.end() )
	{
		DamageData &rkData = iter->second;
		rkData.m_iCurDamage += iDamage;
	}
	else
	{
		DamageData kData;
		kData.m_iCurDamage = iDamage;
		m_DamageTable.insert( DamageTableMap::value_type( rkAttacker, kData ) );
	}
}

void DamageTable::ClearTable()
{
	m_DamageTable.clear();
}

int DamageTable::GetDamageTableCnt() const
{
	return m_DamageTable.size();
}

bool DamageTable::GetCurDamage( int iIdx, ioHashString &rkName, int &iDamage )
{
	if( !COMPARE( iIdx, 0, GetDamageTableCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "DamageTable::GetDamage - overflow(%d/%d)", iIdx, GetDamageTableCnt() );
		return false;
	}

	DamageTableMap::iterator iter = m_DamageTable.begin();
	std::advance( iter, iIdx );

	rkName  = iter->first;
	
	DamageData &rkData = iter->second;	
	iDamage = rkData.m_iCurDamage;

	return true;
}

bool DamageTable::GetAllDamage( int iIdx, ioHashString &rkName, int &iDamage )
{
	if( !COMPARE( iIdx, 0, GetDamageTableCnt() ) )
	{
		LOG.PrintTimeAndLog( 0, "DamageTable::GetDamage - overflow(%d/%d)", iIdx, GetDamageTableCnt() );
		return false;
	}

	DamageTableMap::iterator iter = m_DamageTable.begin();
	std::advance( iter, iIdx );

	rkName  = iter->first;

	DamageData &rkData = iter->second;	
	iDamage = rkData.m_iCurDamage + rkData.m_iPrevDamage;

	return true;
}

void DamageTable::AttachAllDamage()
{
	DamageTableMap::iterator iCreate;
	for( iCreate = m_DamageTable.begin() ; iCreate != m_DamageTable.end() ; ++iCreate )
	{
		DamageData &rkData = iCreate->second;
		rkData.m_iPrevDamage += rkData.m_iCurDamage;
		rkData.m_iCurDamage   = 0;
	}
}

int DamageTable::GetAllDamage( const ioHashString &rkAttacker )
{
	DamageTableMap::iterator iter = m_DamageTable.find( rkAttacker );
	if( iter == m_DamageTable.end() )
		return 0;

	DamageData &rkData = iter->second;	
	return rkData.m_iCurDamage + rkData.m_iPrevDamage;
}

void DamageTable::AttachAllDamage( const ioHashString &rkName )
{
	DamageTableMap::iterator iter = m_DamageTable.find( rkName );
	if( iter != m_DamageTable.end() )
	{
		DamageData &rkData = iter->second;
		rkData.m_iPrevDamage += rkData.m_iCurDamage;
		rkData.m_iCurDamage   = 0;
	}
}

