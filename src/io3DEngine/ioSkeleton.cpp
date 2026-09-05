
#include "StdAfx.h"

#include "ioResource.h"
#include "ioRcSkeleton.h"
#include "ioRcBiped.h"
#include "ioBiped.h"

#include "ioSkeleton.h"
#include "ioResourceManager.h"
#include "ioSkeletonManager.h"
#include "ErrorReport.h"

#include "ioMath.h"
#include "ioRopeSpringCurve.h"

using namespace std;

ioSkeleton::ioSkeleton( ioRcSkeleton *pRc )
{
	m_pRcSkeleton = pRc;

	GenerateBipedList();
}

ioSkeleton::~ioSkeleton()
{
	ClearAllRopeSpringCurve();

	if( m_pRcSkeleton )
	{
		ioSkeletonManager::GetSingleton().Destroy( m_pRcSkeleton );
		m_pRcSkeleton = NULL;
	}

	ClearBipedList();
}

ioBiped* ioSkeleton::CreateBiped()
{
	ioBiped *pBiped = new ioBiped( this );
	if( pBiped )
	{
		m_vBipedList.push_back( pBiped );
	}

	return pBiped;
}

ioBiped* ioSkeleton::CreateBiped( const ioHashString &kName )
{
	ioBiped *pBiped = CreateBiped();
	if( pBiped )
	{
		pBiped->SetName( kName );
	}

	return pBiped;
}

void ioSkeleton::GenerateBipedList()
{
	ClearBipedList();

	int i=0;
	int iBipedCnt = m_pRcSkeleton->GetRcBipedCnt();
	m_vBipedList.reserve( iBipedCnt );

	for( i=0 ; i<iBipedCnt ; i++ )
	{
		ioRcBiped* pRcBiped = m_pRcSkeleton->GetRcBiped(i);
		if( !pRcBiped )	continue;

		ioBiped *pBiped = CreateBiped( pRcBiped->GetName() );
		if( pBiped )
		{
			pBiped->InitializeByRcBiped( pRcBiped );
			m_BipedSearchMap.insert( BipedMap::value_type( pBiped->GetName(), pBiped ) );
		}
	}

	for( i=0 ; i<iBipedCnt ; i++ )
	{
		ioRcBiped *pRcBiped = m_pRcSkeleton->GetRcBiped(i);
		if( !pRcBiped )	continue;

		ioBiped *pBiped = GetBiped( pRcBiped->GetName() );
		if( !pBiped ) continue;

		int iChildCnt = pRcBiped->GetChildCnt();
		for( int j=0 ; j<iChildCnt ; j++ )
		{
			ioBiped *pChild = GetBiped( pRcBiped->GetChildName(j) );
			if( pChild )
			{
				pBiped->AddChild( pChild );
			}
		}
	}

	UpdateSkeleton( 0.0f );

	ioBipedVec::iterator iter = m_vBipedList.begin();
	for( ; iter!=m_vBipedList.end() ; ++iter )
	{
		ioBiped *pBiped = *iter;
		if( pBiped )
		{
			pBiped->SaveDefDerivedXform();
		}
	}
}

void ioSkeleton::ClearBipedList()
{
	ioBipedVec::iterator iter = m_vBipedList.begin();
	for( ; iter!=m_vBipedList.end() ; ++iter )
	{
		delete *iter;
	}
	
	m_vBipedList.clear();
	m_BipedSearchMap.clear();
}

void ioSkeleton::ClearBlend()
{
	ioBipedVec::iterator iter = m_vBipedList.begin();
	for( ; iter!=m_vBipedList.end() ; ++iter )
	{
		ioBiped *pBiped = *iter;
		if( pBiped )
		{
			pBiped->ClearBlend();
		}
	}
}

void ioSkeleton::BlendBiped( int iBipedID,
							 float fWeight, 
							 const D3DXVECTOR3 &vTrans,
							 const D3DXQUATERNION &qtRot )
{
	m_vBipedList[iBipedID]->Blend( fWeight, vTrans, qtRot );
}

void ioSkeleton::LockBlend()
{
	ioBipedVec::iterator iter = m_vBipedList.begin();
	for( ; iter!=m_vBipedList.end() ; ++iter )
	{
		ioBiped *pBiped = *iter;
		if( pBiped )
		{
			pBiped->LockBlend();
		}
	}
}

void ioSkeleton::UpdateSkeleton( float fTime )
{
	if( m_vBipedList.empty() )
		return;

	UpdateRopeSpringCurve( fTime );

	if( IsLinkedSkeleton() )
	{
		ioBipedVec::iterator iter = m_vBipedList.begin();
		for( ; iter!=m_vBipedList.end() ; ++iter )
		{
			ioBiped *pBiped = *iter;
			if( pBiped )
			{
				pBiped->ApplyObjectOffsetTM();
			}
		}
	}

	ioBiped *pRoot = m_vBipedList.front();
	if( pRoot )
	{
		pRoot->Update( true, false );
	}
}

void ioSkeleton::SetExtraRotate( const ioHashString &kBipedName,
								 const D3DXQUATERNION &qtRot,
								 bool bLocal )
{
	ioBiped *pBiped = GetBiped( kBipedName );
	if( pBiped )
	{
		pBiped->SetExtraRotation( qtRot, bLocal );
	}
}

void ioSkeleton::ReleaseExtraRotate( const ioHashString &kBipedName )
{
	ioBiped *pBiped = GetBiped( kBipedName );
	if( pBiped )
	{
		pBiped->ReleaseExtraRotation();
	}
}

void ioSkeleton::ReleaseAllExtraRotate()
{
	ioBipedVec::iterator iter = m_vBipedList.begin();
	for( ; iter!=m_vBipedList.end() ; ++iter )
	{
		ioBiped *pBiped = *iter;
		if( pBiped )
		{
			pBiped->ReleaseExtraRotation();
		}
	}
}

bool ioSkeleton::HasRopeSpring() const
{
	return !m_SpringCurveList.empty();
}

bool ioSkeleton::AddRopeSpringCurve( const ioHashString &rkBiped, const RopeSpringParam *pParam )
{
	ioBiped *pRoot = GetBiped( rkBiped );
	if( !pRoot )
	{
		LOG.PrintTimeAndLog( 0, "ioSkeleton::AddRopeSpringCurve - %s Not Exist", rkBiped.c_str() );
		return false;
	}

	// 기존에 같은게 있으면 무시
	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter != m_SpringCurveList.end() ; ++iter )
	{
		ioRopeSpringCurve *pCurve = *iter;
		if( pCurve->IsRoot( rkBiped ) )
		{
			return false;
		}
	}

	ioBipedVec vBipedList;
	vBipedList.reserve( 8 );
	vBipedList.push_back( pRoot );

	ioBiped *pCursor = pRoot;
	while( pCursor )
	{
		int iNumChildren = pCursor->GetNumChildren();
		for( int i=0 ; i<iNumChildren ; i++ )
		{
			ioBiped *pChild = dynamic_cast<ioBiped*>( pCursor->GetChildByIndex(i) );
			if( !pChild ) continue;

			ioBipedVec::iterator iter = std::find( m_vBipedList.begin(),
												   m_vBipedList.end(),
												   pChild );

			if( iter != m_vBipedList.end() )
			{
				vBipedList.push_back( pChild );
				pCursor = pChild;
				break;
			}
		}

		if( i >= iNumChildren )	// No Child
		{
			pCursor = NULL;
		}
	}

	if( vBipedList.size() < 2 )
	{
		LOG.PrintTimeAndLog( 0, "ioSkeleton::AddRopeSpringCurve - %s Child Not Enough - %d",
								rkBiped.c_str(),
								vBipedList.size() );
		return false;
	}

	ioRopeSpringCurve *pCurve = new ioRopeSpringCurve;
	if( !pCurve->GenerateRopes( vBipedList, pParam ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSkeleton::AddRopeSpringCurve - %s Failed", rkBiped.c_str() );
		SAFEDELETE( pCurve );
		return false;
	}

	m_SpringCurveList.push_back( pCurve );

	return true;
}

bool ioSkeleton::RemoveRopeSpringCurve( const ioHashString &rkBiped )
{
	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter != m_SpringCurveList.end() ; ++iter )
	{
		ioRopeSpringCurve *pCurve = *iter;
		if( pCurve->IsRoot( rkBiped ) )
		{
			m_SpringCurveList.erase( iter );

			delete pCurve;
			return true;
		}
	}

	return false;
}

void ioSkeleton::ClearAllRopeSpringCurve()
{
	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter != m_SpringCurveList.end() ; ++iter )
	{
		delete *iter;
	}
	m_SpringCurveList.clear();
}

void ioSkeleton::AddWorldForce( const D3DXVECTOR3 &vForce )
{
	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter != m_SpringCurveList.end() ; ++iter )
	{
		(*iter)->AddWorldForce( vForce );
	}
}

void ioSkeleton::UpdateRopeSpringCurve( float fTime )
{
	if( m_SpringCurveList.empty() || fTime <= 0.0f )
		return;

	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter != m_SpringCurveList.end() ; ++iter )
	{
		(*iter)->Update( fTime );
	}
}

ioBiped* ioSkeleton::GetBiped( int id ) const
{
	if( COMPARE( id, 0, GetBipedCnt() ) )
		return m_vBipedList[id];

// 	LOG.PrintTimeAndLog( 0, "ioSkeleton::GetBiped %s:%d Not Exist",
// 							GetName().c_str(), id );

	return NULL;
}

ioBiped* ioSkeleton::GetBiped( const ioHashString &name ) const
{
	BipedMap::const_iterator iter = m_BipedSearchMap.find( name );
	if( iter != m_BipedSearchMap.end() )
		return iter->second;

// 	LOG.PrintTimeAndLog( 0, "ioSkeleton::GetBiped  %s:%s Not Exist",
// 							GetName().c_str(), name.c_str() );

	return NULL;
}

int ioSkeleton::GetBipedIndex( const ioHashString &name ) const
{
	ioBipedVec::const_iterator iter=m_vBipedList.begin();
	for( int i=0; iter!=m_vBipedList.end() ; ++iter, ++i )
	{
		if( (*iter)->GetName() == name )
			return i;
	}

// 	LOG.PrintTimeAndLog( 0, "ioSkeleton::GetBipedIndex %s:%s Not Exist",
// 							GetName().c_str(), name.c_str() );

	return 0;
}

const ioHashString& ioSkeleton::GetName() const
{
	return m_pRcSkeleton->GetName();
}

bool ioSkeleton::IsLinkedSkeleton() const
{
	if( m_pRcSkeleton )
		return m_pRcSkeleton->IsLinkedSkeleton();

	return false;
}

void ioSkeleton::DebugRopeCollisionBox( std::vector< ioOrientBox > &vBoxList )
{
	RopeSpringCurveList::iterator iter = m_SpringCurveList.begin();
	for( ; iter!=m_SpringCurveList.end() ; ++iter )
	{
		(*iter)->DebugGetBoxList( vBoxList );
	}
}
