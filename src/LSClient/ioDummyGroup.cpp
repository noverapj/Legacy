#include "stdafx.h"

#include "ioDummyGroup.h"
#include "ioDummyCharManager.h"

#include "ioPlayStage.h"
#include "ioItem.h"
#include "ioBaseChar.h"

ioDummyGroup::ioDummyGroup() : m_pStage( NULL )
{

}

ioDummyGroup::ioDummyGroup( ioPlayStage* pStage, const ioHashString& szGroupOwner, const ioHashString& szIdentityName )
	: m_pStage( pStage ), m_GroupOwner( szGroupOwner ), m_IdentityName( szIdentityName )
{

}

ioDummyGroup::~ioDummyGroup()
{

}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ioDummyGroup::CheckDieDummy()
{
	if( !m_pStage )
		return;

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return;

	IntVec::iterator iter = m_DummyList.begin();
	while( iter != m_DummyList.end() )
	{
		int iCurrIndex = *iter;		
		ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurrIndex );
		if( !pDummy || pDummy->GetState() == DCS_DIE )
		{
			iter = m_DummyList.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

ioDummyChar* ioDummyGroup::FirstDummyDie( ioDummyChar* pExceptionDummy )
{
	if( !m_pStage )
		return NULL;

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return NULL;

	if( m_DummyList.empty() )
		return  NULL;

	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{	
		ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if( pDummy && pDummy->GetState() != DCS_DIE && pDummy != pExceptionDummy )
		{
			pDummy->SetDieState( true );
			return pDummy;
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ioDummyGroup::IsLiveDummy( int iDummyIndex )
{
	ioDummyChar* pDummy = GetDummy( iDummyIndex );
	if( pDummy && pDummy->GetState() != DCS_DIE )
		return true;

	return false;
}

bool ioDummyGroup::HasDummy( int iDummyIndex )
{	
	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{
		int iCurrIndex = *iter;
		if( iCurrIndex == iDummyIndex )
				return true;
	}	

	return  false;
}


void ioDummyGroup::AddDummy( int iDummyIndex )
{
	if( !GetDummy( iDummyIndex ) )
	{
		m_DummyList.push_back( iDummyIndex );
	}
}

ioDummyChar* ioDummyGroup::GetDummy( int iDummyIndex )
{
	ioBaseChar*	pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return NULL;

	IntVec::iterator iter = m_DummyList.begin();
	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{
		int iCurrIndex = *iter;
		if( iDummyIndex == iCurrIndex )
		{
			ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurrIndex );
			return pDummy;
		}
	}

	return NULL;
}

void ioDummyGroup::GetDummyByArea( IN float fMaxRange, OUT DummyCharList& List )
{
	List.clear();

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return;
	
	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{	
		ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if( pDummy && pDummy->GetState() != DCS_DIE )
		{	
			D3DXVECTOR3 vDiff = pDummy->GetWorldPosition() - pOwner->GetWorldPosition();
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );
			if( 0.0f <= fCurDistSq && fMaxRange*fMaxRange >= fCurDistSq )
			{
				List.push_back( pDummy );
			}
		}
	}
}

void ioDummyGroup::GetDummyAll( OUT DummyCharList& List )
{
	List.clear();

	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return;

	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{	
		ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if( pDummy && pDummy->GetState() != DCS_DIE )
		{
			List.push_back( pDummy );
		}
	}
}

int ioDummyGroup::GetDummyCount( ioHashString szName )
{
	ioBaseChar* pOwner = m_pStage->GetBaseChar( m_GroupOwner );
	if( !pOwner )
		return 0;

	DummyCharList List;
	for( IntVec::iterator iter = m_DummyList.begin(); iter != m_DummyList.end(); ++iter )
	{	
		ioDummyChar* pDummy = g_DummyCharMgr.FindDummyChar( pOwner, *iter );
		if( pDummy && pDummy->GetState() != DCS_DIE && pDummy->GetDummyCharName() == szName )
		{
			List.push_back( pDummy );
		}
	}

	return List.size();
}