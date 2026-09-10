

#include "stdafx.h"


#include "FindPredicate.h"
#include "ioBaseChar.h"

FindPredicate::CharDistanceList FindPredicate::m_vDistanceSortList;
FindPredicate::CharAngleSortList FindPredicate::m_vAngleSortList;

FindPredicate::FindPredicate( const ioBaseChar *pOwner )
: m_pOwner( pOwner )
{
}

FindPredicate::~FindPredicate()
{
}

bool FindPredicate::IsExperienceOwner()
{
	if( m_pOwner == NULL )
		return false;

	return ( m_pOwner->GetExperienceMode() == EMS_EXPERIENCE );
}

void FindPredicate::MakeMostCloseSortList( const BaseCharList &rkCandidateList, float fLimitDistSq )
{
	if( rkCandidateList.empty() )
		return;

	m_vDistanceSortList.erase( m_vDistanceSortList.begin(), m_vDistanceSortList.end() );
	m_vDistanceSortList.reserve( rkCandidateList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=rkCandidateList.begin() ; iter!=rkCandidateList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != m_pOwner )
		{
			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			if( fLimitDistSq >= fCurDistSq )
			{
				CharDistanceSq kCharDist;
				kCharDist.pChar = pCurChar;
				kCharDist.fDistanceSq = fCurDistSq;

				m_vDistanceSortList.push_back( kCharDist );
			}
		}
	}

	std::sort( m_vDistanceSortList.begin(), m_vDistanceSortList.end() );
}

void FindPredicate::MakeMostCloseSortList( const BaseCharList &rkCandidateList, float fMinDistSq, float fLimitDistSq )
{
	if( rkCandidateList.empty() )
		return;

	m_vDistanceSortList.erase( m_vDistanceSortList.begin(), m_vDistanceSortList.end() );
	m_vDistanceSortList.reserve( rkCandidateList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter=rkCandidateList.begin() ; iter!=rkCandidateList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != m_pOwner )
		{
			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
			{
				CharDistanceSq kCharDist;
				kCharDist.pChar = pCurChar;
				kCharDist.fDistanceSq = fCurDistSq;
				m_vDistanceSortList.push_back( kCharDist );
			}
		}
	}

	std::sort( m_vDistanceSortList.begin(), m_vDistanceSortList.end() );
}

void FindPredicate::MakeSmallestAngleSortList( const BaseCharList &rkCandidateList, float fLimitDistSq, float fMaxHalfCosine )
{
	if( rkCandidateList.empty() )
		return;

	m_vAngleSortList.erase( m_vAngleSortList.begin(), m_vAngleSortList.end() );
	m_vAngleSortList.reserve( rkCandidateList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();

	BaseCharList::const_iterator iter;
	for( iter=rkCandidateList.begin() ; iter!=rkCandidateList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar != m_pOwner )
		{
			vDiff = pCurChar->GetWorldPosition() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );
			if( fLimitDistSq < fCurDistSq ) continue;

			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );
			float fHalfCosine = D3DXVec3Dot( &vCharDir, &vDiff );

			if( fHalfCosine >= fMaxHalfCosine )
			{
				CharAngle kCharAngle;
				kCharAngle.pChar = pCurChar;
				kCharAngle.fHalfCosine = fHalfCosine * -1;
				m_vAngleSortList.push_back( kCharAngle );
			}
		}
	}

	std::sort( m_vAngleSortList.begin(), m_vAngleSortList.end() );
}


//--------------------------------------------------------------------------------------------

FindListPredicate::FindListPredicate()
{
}

FindListPredicate::~FindListPredicate()
{
}

//--------------------------------------------------------------------------------------------

bool FindPredicateUtil::CheckTargetState( ioBaseChar* pTarget, bool bDownTargetFind, bool bHideTargetFind /*= true */ )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	switch( pTarget->GetState() )
	{
	case CS_FROZEN:
		if( pTarget->GetBlowFrozenState() )
			return false;
		break;
	case CS_BLOW_WOUNDED:
	case CS_BOUND_BLOW_WOUNDED:
		if( !pTarget->IsFloatingState() && !bDownTargetFind )
			return false;
		break;
	case CS_ETC_STATE:
	case CS_USING_SKILL:
		if( pTarget->IsApplyDownState(false) && !bDownTargetFind )
			return false;
		break;		
	case CS_STOP_MOTION:
		if( pTarget->GetBlowStopMotionState() )
			return false;
		break;
	case CS_ICE_STATE:
		if( pTarget->GetBlowStopMotionState() )
			return false;
		break;
	}

	if( bHideTargetFind && pTarget->HasHideBuff() )
		return false;

	if( pTarget->IsChatModeState( false ) )
		return false;

	if( pTarget->IsExcavating() )
		return false;

	if( pTarget->GetExperienceMode() == EMS_EXPERIENCE )
		return false;

	if( pTarget->HasChangeTeamBuff(true) )
		return false;

	return true;
}


