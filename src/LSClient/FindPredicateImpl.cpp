

#include "stdafx.h"

#include "FindPredicateImpl.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

FD_CloseAliveEnemy::FD_CloseAliveEnemy( const ioBaseChar *pOwner )
: FindPredicate( pOwner )
{
}

FD_CloseAliveEnemy::~FD_CloseAliveEnemy()
{
}

ioBaseChar* FD_CloseAliveEnemy::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	MakeMostCloseSortList( rkCandidateList, std::numeric_limits<float>::infinity() );

	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		if( pChar->GetState() != CS_DIE
			&& pChar->GetState() != CS_VIEW
			&& pChar->GetState() != CS_OBSERVER
			&& pChar->GetState() != CS_LOADING
			&& pChar->GetState() != CS_CHANGE_WAIT
			&& m_pOwner->GetTeam() != pChar->GetTeam()
			&& !pChar->IsPrisonerMode() 
			&& pChar->GetExperienceMode() != EMS_EXPERIENCE )
			return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------------

FD_AimTarget::FD_AimTarget( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bDownAim, bool bOwnerTeam )
: FindPredicate( pOwner )
{
	m_fAimAngle = fAngle;
	m_fAimRange = fRange;
	m_fMinRange = fMinRange;
	m_bOwnerTeam = bOwnerTeam;
	m_bDownAim = bDownAim;
}

FD_AimTarget::~FD_AimTarget()
{
}

ioBaseChar* FD_AimTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fMinRange*m_fMinRange, m_fAimRange*m_fAimRange );

	//D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();
	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vCharDir  = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;

		CharState eState = pChar->GetState();
		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;
		if( eState == CS_FROZEN && pChar->GetBlowFrozenState() && !m_bDownAim )
			continue;

		if( eState == CS_BLOW_WOUNDED && !pChar->IsFloatingState() && !m_bDownAim )
			continue;

		if( eState == CS_BOUND_BLOW_WOUNDED && !pChar->IsFloatingState() && !m_bDownAim )
			continue;

		if( eState == CS_ETC_STATE && pChar->IsApplyDownState(false) && !m_bDownAim )
			continue;

		if( eState == CS_USING_SKILL && pChar->IsApplyDownState(false) && !m_bDownAim )
			continue;
		if( eState == CS_STOP_MOTION && pChar->GetBlowStopMotionState() && !m_bDownAim )
			continue;

		if( eState == CS_ICE_STATE && pChar->GetBlowStopMotionState() )
			continue;

		if( pChar->HasChangeTeamBuff(true) )
			continue;
		
		//개발자 K는 팀에 구애받지 않고 힐이 가능하다.
		if( pChar->GetCharName() != Help::GetDeveloperKCharName() )        
		{
			if( pChar->IsChatModeState( false ) )
				continue;

			if( m_bOwnerTeam )
			{
				if( pChar->HasHideBuff() )
					continue;

				if( pChar->GetTeam() == m_pOwner->GetTeam() )
					continue;
			}
			else if(!m_bOwnerTeam && pChar->GetTeam() != m_pOwner->GetTeam() )
				continue;
		}		

		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - vOwnerPos;
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------------

FD_AimTarget_Staff::FD_AimTarget_Staff( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bOwnerTeam )
: FindPredicate( pOwner )
{
	m_fAimAngle = fAngle;
	m_fAimRange = fRange;
	m_fMinRange = fMinRange;
	m_bOwnerTeam = bOwnerTeam;
}

FD_AimTarget_Staff::~FD_AimTarget_Staff()
{
}

ioBaseChar* FD_AimTarget_Staff::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fMinRange*m_fMinRange, m_fAimRange*m_fAimRange );

	//D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();
	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vCharDir  = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();		
		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;

		if( eState == CS_FROZEN && pChar->GetBlowFrozenState() )
			continue;

		if( eState == CS_STOP_MOTION && pChar->GetBlowStopMotionState() )
			continue;

		if( eState == CS_ICE_STATE && pChar->GetBlowStopMotionState() )
			continue;

		if( pChar->IsPrisonerMode() )
			continue;
		if( pChar->HasChangeTeamBuff(true) )
			continue;
		
		//개발자 K는 팀에 구애받지 않고 힐이 가능하다.
		if( pChar->GetCharName() != Help::GetDeveloperKCharName() )     
		{
			if( pChar->IsChatModeState( false ) )
				continue;

			if( m_bOwnerTeam && pChar->GetTeam() != m_pOwner->GetTeam() )
				continue;

			if( pChar->HasHideBuff() )
			{
				if( pChar->GetTeam() != m_pOwner->GetTeam() )
					continue;
			}
		}		

		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - vOwnerPos;
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------------

FD_AimTarget_Bow::FD_AimTarget_Bow( const ioBaseChar *pOwner, float fAngle, float fMinRange, float fRange, bool bDownAim, bool bOwnerTeam )
: FindPredicate( pOwner )
{
	m_fAimAngle = fAngle;
	m_fAimRange = fRange;
	m_fMinRange = fMinRange;
	m_bOwnerTeam = bOwnerTeam;
	m_bDownAim = bDownAim;
}

FD_AimTarget_Bow::~FD_AimTarget_Bow()
{
}

ioBaseChar* FD_AimTarget_Bow::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fMinRange*m_fMinRange, m_fAimRange*m_fAimRange );

	D3DXQUATERNION qtRot = m_pOwner->GetTargetRot();
	D3DXVECTOR3 vCharDir  = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle ) * FLOAT05 );

	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;

		CharState eState = pChar->GetState();
		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;


		if( eState == CS_FROZEN && pChar->GetBlowFrozenState() ) 
			continue;

		if( eState == CS_BLOW_WOUNDED && !pChar->IsFloatingState() && !m_bDownAim )
			continue;

		if( eState == CS_BOUND_BLOW_WOUNDED && !pChar->IsFloatingState() && !m_bDownAim )
			continue;

		if( eState == CS_ETC_STATE && pChar->IsApplyDownState(false) && !m_bDownAim )
			continue;

		if( eState == CS_USING_SKILL && pChar->IsApplyDownState(false) && !m_bDownAim )
			continue;

		if( eState == CS_STOP_MOTION && pChar->GetBlowStopMotionState() ) 
			continue;

		if( eState == CS_ICE_STATE && pChar->GetBlowStopMotionState() ) 
			continue;

		if( pChar->HasChangeTeamBuff(true) )
			continue;

		if( m_bOwnerTeam && pChar->GetTeam() == m_pOwner->GetTeam() )
			continue;
		else if(!m_bOwnerTeam && pChar->GetTeam() != m_pOwner->GetTeam() )
			continue;

		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - vOwnerPos;
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------

FD_SightTarget::SightTargetSort::SightTargetSort( TeamType eOwnerTeam )
{
	m_OwnerTeam = eOwnerTeam;
}

bool FD_SightTarget::SightTargetSort::operator()( const CharDistanceSq &lhs, const CharDistanceSq &rhs ) const
{
	TeamType eLhsTeam = lhs.pChar->GetTeam();
	TeamType eRhsTeam = rhs.pChar->GetTeam();

	if( eLhsTeam == eRhsTeam )
	{
		if( lhs.fDistanceSq < rhs.fDistanceSq )
			return true;

		return false;
	}

// 	if( eLhsTeam != m_OwnerTeam)
// 		return true;

	if( (eLhsTeam < m_OwnerTeam) && (eLhsTeam > m_OwnerTeam) )
		return true;

	return false;
}

FD_SightTarget::FD_SightTarget( const ioBaseChar *pOwner, float fAngle, float fRange )
: FindPredicate( pOwner )
{
	m_fSightAngle = fAngle;
	m_fSightRange = fRange;
}

FD_SightTarget::~FD_SightTarget()
{
}

ioBaseChar* FD_SightTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fSightRange * m_fSightRange );
	std::sort( m_vDistanceSortList.begin(), m_vDistanceSortList.end(), SightTargetSort( m_pOwner->GetTeam() ) );

	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = m_pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	float fHalfCosine = cosf( DEGtoRAD( m_fSightAngle ) * FLOAT05 );

	D3DXVECTOR3 vDiff;
	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();

		if( eState == CS_DIE ) continue;
		if( eState == CS_VIEW ) continue;
		if( eState == CS_OBSERVER ) continue;
		if( eState == CS_READY ) continue;
		if( eState == CS_LOADING ) continue;
		if( eState == CS_DROP_ZONE_DOWN ) continue;
		if( pChar->GetExperienceMode() == EMS_EXPERIENCE ) continue;

		if( pChar->HasHideBuff() ) continue;

		vDiff = pChar->GetWorldPosition() - vOwnerPos;
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			return pChar;
	}

	return NULL;
}

bool FD_SightTarget::CheckUser( ioBaseChar* pTarget )
{
	if(pTarget == NULL) return false;

	D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	float fHalfCosine = cosf( DEGtoRAD( m_fSightAngle ) * FLOAT05 );

	D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - vOwnerPos;
	vDiff.y = 0.0f;
	D3DXVec3Normalize( &vDiff, &vDiff );
	
	if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
		return true;
	
	return false;
}

//----------------------------------------------------------------------------

FD_AutoTarget::FD_AutoTarget( const ioBaseChar *pOwner,
							  float fMinAngle,
							  float fMaxAngle,
							  float fRange )
: FindPredicate( pOwner )
{
	m_fMinAngle = fMinAngle;
	m_fMaxAngle = fMaxAngle;
	m_fRangeSq = fRange * fRange;
}

FD_AutoTarget::~FD_AutoTarget()
{
}

ioBaseChar* FD_AutoTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	float fCurAngle, fMaxHalfCosine, fHalfCosine, fCurAngleRate;

	fMaxHalfCosine = cosf( DEGtoRAD( m_fMaxAngle ) * FLOAT05  );
	MakeSmallestAngleSortList( rkCandidateList, m_fRangeSq, fMaxHalfCosine );

	D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();
	//D3DXVECTOR3 vCharDir  = m_pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vDiff;

	CharAngleSortList::iterator iter=m_vAngleSortList.begin();
	for( ; iter!=m_vAngleSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();
		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;

		if( eState == CS_USING_SKILL && pChar->IsApplyDownState(false) )
			continue;

		if( eState == CS_BLOW_WOUNDED && !pChar->IsFloatingState() ) 
			continue;

		if( eState == CS_BOUND_BLOW_WOUNDED && !pChar->IsFloatingState() )
			continue;

		if( eState == CS_ETC_STATE && pChar->IsApplyDownState(false) )
			continue;
		
		if( pChar->GetTeam() == m_pOwner->GetTeam() )
		{
			if( !pChar->IsPrisonerMode() )
				continue;
		}

		vDiff = pChar->GetWorldPosition() - vOwnerPos;

		fCurAngleRate = D3DXVec3LengthSq( &vDiff ) / m_fRangeSq;
		fCurAngle = m_fMinAngle * fCurAngleRate + m_fMaxAngle * (1.0f - fCurAngleRate );
		fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );

		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
			return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------

FD_CloseChainTarget::FD_CloseChainTarget( const ioBaseChar *pOwner, ioWeapon *pWeapon, float fRange, float fAngle )
: FindPredicate( pOwner )
{
	m_fRangeSq = fRange * fRange;
	m_fHalfAngle = fAngle * FLOAT05;
	m_pWeapon = pWeapon;
}

FD_CloseChainTarget::~FD_CloseChainTarget()
{
}

ioBaseChar* FD_CloseChainTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	if( !m_pWeapon ) return NULL;
	ioBaseChar *pAttacker = m_pWeapon->GetOwner();
	if( !pAttacker ) return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fRangeSq );

	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();

		if( !pChar )
			continue;

		if( pChar == m_pOwner )
			continue;

		if( pChar == pAttacker )
			continue;
		
		if( m_pWeapon->IsAlreadyCollisioned( pChar ) )
			continue;

		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;


		if( pChar->GetTeam() == pAttacker->GetTeam() )
			continue;

		ioChainWeapon *pChain = ToChainWeapon( m_pWeapon );
		if( pChain && pChain->IsDisableDownTarget() )
		{
			if( eState == CS_BLOW_WOUNDED && !pChar->IsFloatingState() )
				continue;
			if( eState == CS_BOUND_BLOW_WOUNDED && !pChar->IsFloatingState() )
				continue;
			if( eState == CS_ETC_STATE && pChar->IsApplyDownState(false) )
				continue;
			if( eState == CS_USING_SKILL && pChar->IsApplyDownState(false) )
				continue;
		}

		D3DXVECTOR3 vDir = m_pWeapon->GetMoveDir();
		vDir.y = 0.0f;
		D3DXVec3Normalize( &vDir, &vDir );

		D3DXVECTOR3 vDiff = pChar->GetMidPositionByRate() - m_pWeapon->GetPosition();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_fHalfAngle > 0.0f )
		{
			float fDotValue = D3DXVec3Dot( &vDiff, &vDir );
			float fGapAngle = RADtoDEG( acosf( fDotValue ) );
            
			if( fGapAngle >= m_fHalfAngle )
				continue;
		}

		return pChar;
	}

	return NULL;
}

//----------------------------------------------------------------------------

FD_GrapplingAutoTarget::FD_GrapplingAutoTarget( const ioBaseChar *pOwner, float fMinAngle, float fMaxAngle, float fRange, bool bDownTarget /*= true */ ) 
	: FindPredicate( pOwner )
{
	m_fMinAngle		= fMinAngle;
	m_fMaxAngle		= fMaxAngle;
	m_fRangeSq		= fRange * fRange;
	m_bDownTarget	= bDownTarget;
}

FD_GrapplingAutoTarget::~FD_GrapplingAutoTarget()
{
}

ioBaseChar* FD_GrapplingAutoTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() ) return NULL;

	float fCurAngle, fMaxHalfCosine, fHalfCosine, fCurAngleRate;

	fMaxHalfCosine = cosf( DEGtoRAD( m_fMaxAngle ) * FLOAT05  );
	MakeSmallestAngleSortList( rkCandidateList, m_fRangeSq, fMaxHalfCosine );

	D3DXVECTOR3 vCharDir  = m_pOwner->GetMoveDir();
	D3DXVECTOR3 vOwnerPos = m_pOwner->GetWorldPosition();
	D3DXVECTOR3 vDiff;

	ioBaseChar *pTarget = NULL;
	float fCheckAngle = -FLOAT1;
	CharAngleSortList::iterator iter=m_vAngleSortList.begin();
	for( ; iter!=m_vAngleSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();

		if( pChar == m_pOwner )
			continue;

		if( pChar->IsSystemState() )
			continue;
		
		if( !pChar->IsEnableTarget( false ) ) 
			continue;

		if( pChar->GetTeam() == m_pOwner->GetTeam() )
			continue;

		switch( pChar->GetState() )
		{
		case CS_BLOW_WOUNDED:
		case CS_BOUND_BLOW_WOUNDED:
			if( !pChar->IsFloatingState() && !m_bDownTarget )
				continue;
			break;
		case CS_ETC_STATE:
		case CS_USING_SKILL:
			if( pChar->IsApplyDownState(false) && !m_bDownTarget )
				continue;
			break;
		}

		vDiff = pChar->GetWorldPosition() - vOwnerPos;

		fCurAngleRate = D3DXVec3LengthSq( &vDiff ) / m_fRangeSq;
		fCurAngle = m_fMinAngle * fCurAngleRate + m_fMaxAngle * (1.0f - fCurAngleRate );
		fHalfCosine = cosf( DEGtoRAD( fCurAngle ) * FLOAT05 );

		float fLengthSq = D3DXVec3LengthSq( &vDiff );
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
		{
			if( fCheckAngle == -FLOAT1 )
			{
				fCheckAngle = fHalfCosine;
				pTarget = pChar;
			}
			else if( fHalfCosine < fCheckAngle )
			{
				fCheckAngle = fHalfCosine;
				pTarget = pChar;
			}
		}
	}

	return pTarget;
}

//----------------------------------------------------------------------------------

FD_RangeTarget::FD_RangeTarget( const ioBaseChar *pOwner, float fMaxRange, float fMinRange, bool bDownAim, bool bOwnerTeam )
	: FindPredicate( pOwner )
{
	m_fMaxRange		= fMaxRange;
	m_fMinRange		= fMinRange;
	m_bOwnerTeam	= bOwnerTeam;
	m_bDownAim		= bDownAim;
}

FD_RangeTarget::~FD_RangeTarget()
{
}

ioBaseChar* FD_RangeTarget::GetFindChar( const BaseCharList &rkCandidateList )
{
	if( IsExperienceOwner() )
		return NULL;

	MakeMostCloseSortList( rkCandidateList, m_fMinRange*m_fMinRange, m_fMaxRange*m_fMaxRange );
	
	CharDistanceList::iterator iter;
	for( iter=m_vDistanceSortList.begin() ; iter!=m_vDistanceSortList.end() ; ++iter )
	{
		ioBaseChar *pChar = iter->pChar;
		CharState eState = pChar->GetState();

		if( !FindPredicateUtil::CheckTargetState( pChar, m_bDownAim ) )
			continue;

		if( m_bOwnerTeam && pChar->GetTeam() == m_pOwner->GetTeam() )
			continue;

		else if(!m_bOwnerTeam && pChar->GetTeam() != m_pOwner->GetTeam() )
			continue;

		if( !CheckFindUser( pChar ) )
			continue;

		return pChar;
	}

	return NULL;
}

bool FD_RangeTarget::CheckFindUser( ioBaseChar* pChar )
{
	return true;
}

//----------------------------------------------------------------------------

FD_BuffHasTarget::FD_BuffHasTarget( const ioBaseChar *pOwner, const ioHashString& szTargetBuff, float fMaxRange, float fMinRange, bool bDownAim, bool bOwnerTeam )
	: FD_RangeTarget( pOwner, fMaxRange, fMinRange, bDownAim, bOwnerTeam ), m_szTargetBuff( szTargetBuff )
{

}

FD_BuffHasTarget::~FD_BuffHasTarget()
{

}

bool FD_BuffHasTarget::CheckFindUser( ioBaseChar* pTarget )
{
	if( m_szTargetBuff.IsEmpty() )
		return true;

	if( !pTarget )
		return false;
	
	if( !pTarget->HasBuff( m_szTargetBuff ) )
		return false;

	return true;
}

//----------------------------------------------------------------------------

FD_RangeMultipleTarget::FD_RangeMultipleTarget( const ioBaseChar *pOwner, float fMaxRange, float fMinRange, float fMaxAngle, bool bDownAim, bool bOwnerTeam /*= true */, bool bHideTargetFind /*= true*/ )
	: FD_RangeTarget( pOwner, fMaxRange, fMinRange, bDownAim, bOwnerTeam ), m_fMaxAngle( fMaxAngle )
{
	m_bHideTargetFind = bHideTargetFind;
}

FD_RangeMultipleTarget::~FD_RangeMultipleTarget()
{
}

void FD_RangeMultipleTarget::GetFindCharList( IN const BaseCharList &rkCandidateList, OUT BaseCharList& rkTargetedCharList, OUT BaseCharList& rkNotTargetedCharList, bool bTeleport, IN D3DXVECTOR3 vOwnerPos )
{
	if( IsExperienceOwner() )
		return;

	rkTargetedCharList.clear();
	rkNotTargetedCharList.clear();

	if( !bTeleport )
		vOwnerPos = m_pOwner->GetWorldPosition();

	BaseCharList::const_iterator iter;
	for( iter = rkCandidateList.begin() ; iter != rkCandidateList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( !pChar )
			continue;

		CharState eState = pChar->GetState();		
		if( pChar == m_pOwner )
			continue;
		
		D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );
		if( m_fMaxRange * m_fMaxRange < fCurDistSq || m_fMinRange * m_fMinRange > fCurDistSq )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}
		
		float fMaxHalfCosine  = cosf( DEGtoRAD( m_fMaxAngle ) * FLOAT05  );
		D3DXVECTOR3 vOwnerDir = m_pOwner->GetMoveDir();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		float fHalfCosine = D3DXVec3Dot( &vOwnerDir, &vDiff );
		if( fHalfCosine < fMaxHalfCosine )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}
		
		if( !FindPredicateUtil::CheckTargetState( pChar, m_bDownAim, m_bHideTargetFind ) )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}

		if( m_bOwnerTeam && pChar->GetTeam() == m_pOwner->GetTeam() )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}
		else if(!m_bOwnerTeam && pChar->GetTeam() != m_pOwner->GetTeam() )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}

		if( !CheckFindUser( pChar ) )
		{
			rkNotTargetedCharList.push_back( pChar );
			continue;
		}

		rkTargetedCharList.push_back( pChar );
	}	
}

bool FD_RangeMultipleTarget::CheckFindUser( ioBaseChar* pChar )
{
	return true;
}