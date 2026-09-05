
#include "StdAfx.h"

#include "ioRcBiped.h"
#include "ioRcSkeleton.h"

#include "ioResourceManager.h"
#include "ioSkeletonManager.h"

ioRcSkeleton::ioRcSkeleton( const ioHashString& name ) : ioResource( name )
{
	m_pLinkedSkeleton = NULL;
}

ioRcSkeleton::~ioRcSkeleton()
{
	ReleaseLinkedSkeleton();
	ClearBipedList();

	ioSkeletonManager::GetSingleton().RemoveMem( GetName() );
}

void ioRcSkeleton::AllocBipedList( int iCount )
{
	m_vRcBipedList.resize( iCount );
}

void ioRcSkeleton::ClearBipedList()
{
	m_vRcBipedList.erase( m_vRcBipedList.begin(), m_vRcBipedList.end() );
}

ioRcBiped* ioRcSkeleton::GetRcBiped( const ioHashString &name )
{
	ioRcBipedVec::iterator iter;
	for( iter=m_vRcBipedList.begin() ; iter!=m_vRcBipedList.end() ; ++iter )
	{
		if( iter->GetName() == name )
			return &(*iter);
	}

	LOG.PrintTimeAndLog( 0, "ioRcSkeleton::GetRcBiped  %s:%s Not Exist",
							m_Name.c_str(), name.c_str() );

	return NULL;
}

ioRcBiped* ioRcSkeleton::GetRcBiped( int iBipedID )
{
	if( COMPARE(iBipedID, 0, GetRcBipedCnt() ) )
		return &m_vRcBipedList[iBipedID];

	LOG.PrintTimeAndLog( 0, "ioRcSkeleton::GetRcBiped - %s over flow (%d/%d)", m_Name.c_str(),
																			   iBipedID,
																			   GetRcBipedCnt() );
	return NULL;
}

void ioRcSkeleton::SetLinkedSkeleton( const ioHashString &rkName )
{
	ReleaseLinkedSkeleton();

	m_pLinkedSkeleton = ioSkeletonManager::GetSingleton().CreateSkeleton( rkName );
	if( !m_pLinkedSkeleton )
	{
		LOG.PrintTimeAndLog( 0, "ioRcSkeleton::SetLinkedSkeleton - %s Not Exist", rkName.c_str() );
		return;
	}

	if( GetRcBipedCnt() != m_pLinkedSkeleton->GetRcBipedCnt() )
	{
		LOG.PrintTimeAndLog( 0, "ioRcSkeleton::SetLinkedSkeleton - Not Same Biped Cnt(%d/%d)",
								GetRcBipedCnt(), m_pLinkedSkeleton->GetRcBipedCnt() );

		ReleaseLinkedSkeleton();
		return;
	}

	ioRcBiped *pLinked, *pOwner;
	int iBipedCnt = GetRcBipedCnt();
	for( int i=0 ; i<iBipedCnt ; i++ )
	{
		pLinked = m_pLinkedSkeleton->GetRcBiped( i );
		pOwner = GetRcBiped( i );

		pOwner->MakeOffsetTM( pLinked );
	}
}

void ioRcSkeleton::ReleaseLinkedSkeleton()
{
	if( m_pLinkedSkeleton )
	{
		ioSkeletonManager::GetSingleton().Destroy( m_pLinkedSkeleton );
		m_pLinkedSkeleton = NULL;
	}
}
