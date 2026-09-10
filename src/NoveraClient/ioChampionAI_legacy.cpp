#include "StdAfx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioChampionAI.h"
#include "ioTowerDefMode.h"

#include "ioAISymbolTable.h"

bool	ioChampionAI::SearchFixedTarget( ioHashString& szFirstTarget )
{
	szFirstTarget = m_szFixingName;

	return ( !szFirstTarget.IsEmpty() );
}

bool	ioChampionAI::SearchTargetByDamage( ioHashString& szFirstTarget )
{
	// 데미지에 따른 우선 순위
	switch( m_dwSearchType )
	{
	// actual default value since m_dwSearchType will be set to 0 as default when loaded
	// and SEARCH_TOP_DAMAGE has value 0
	case SEARCH_TOP_DAMAGE:		
		m_pCreator->GetDamagedTopChar( szFirstTarget );
		return true;
	case SEARCH_LOWEST_DAMAGE:
		m_pCreator->GetDamagedLowestChar( szFirstTarget );
		return true;
	default:
		return false;
	}
}

bool	ioChampionAI::SetAsTarget( ioBaseChar* pChar )
{
	if( pChar != (ioBaseChar*)0 )
	{
		m_TargetData.SetTargetName( pChar->GetCharName() );
		m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
		m_TargetData.SetTargetLevel( pChar->GetLevel() );
		
		m_TargetData.SetTargetType( TARGET_BASECHAR );

		return true;
	}
	return false;
}

bool	ioChampionAI::CheckWaypointObstacles( float& fSearchRange )
{
	bool	bRet = false;

	if( m_TargetData.GetTargetType() == TARGET_WAYPOINT )
	{
		const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
		for( BaseCharList::const_iterator iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{				
			ioBaseChar *pChar = (*iter);
			if( IsTargetIgnoreState( pChar ) ) continue;

			float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
			if( fDistSq < fSearchRange )	// Enemy
			{
				float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
				float fAngle2   = GetAngle( pChar->GetWorldPosition(), m_pCreator->GetWorldPosition() );
				float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
				if( fGapAngle <= m_fObjectSearchAngle )
				{
					fSearchRange = fDistSq;
					m_TargetData.SetTargetName( pChar->GetCharName() );
					m_TargetData.SetTargetPos( pChar->GetWorldPosition() );
					m_TargetData.SetTargetType( TARGET_BASECHAR );

					bRet = true;
				}					
			}
		}
	}

	return bRet;
}

bool	ioChampionAI::CheckTargetObstacles( float& fSearchRange )
{
	const GameEntityList &rkObjectList = m_pPlayStage->GetGameEntityList();
	GameEntityList::const_iterator iter;
	for( iter = rkObjectList.begin() ; iter!=rkObjectList.end() ; ++iter )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( !pPush ) continue;
		if( !pPush->GetGroup()->IsVisible() ) continue;		
		if( pPush->IsEnableClick() ) continue;

		float fDistSq = GetTargetRange( pPush->GetWorldPosition() );
		if( fDistSq < fSearchRange )	
		{
			float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
			float fAngle2   = GetAngle( pPush->GetWorldPosition(), m_pCreator->GetWorldPosition() );
			float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
			if( fGapAngle <= m_fObjectSearchAngle )
			{
				m_TargetData.SetTargetPos( pPush->GetWorldPosition() );
				m_TargetData.SetTargetType( TARGET_OBJECT );

				return true;
			}
		}
	}

	return false;
}

bool	ioChampionAI::IsNPCBlocking( ioBaseChar* pNPC, float& fSearchRange )
{
	if( m_pCreator->GetCharName() == pNPC->GetCharName() ) 
		return false;

	if( pNPC->GetState() == CS_READY ||
		pNPC->GetState() == CS_VIEW  ||
		pNPC->GetState() == CS_DIE   ||
		pNPC->GetState() == CS_OBSERVER ||
		pNPC->GetState() == CS_LOADING ||
		pNPC->GetState() == CS_DROP_ZONE_DOWN ||
		pNPC->GetState() == CS_FALL || 
		pNPC->GetState() == CS_BLOW_WOUNDED ||
		pNPC->GetState() == CS_BOUND_BLOW_WOUNDED ||
		pNPC->GetState() == CS_BLOW_EXTEND_WOUND )
	{
		return false;
	}

	float fDistSq = GetTargetRange( pNPC->GetWorldPosition() );
	if( fDistSq < fSearchRange )	
	{
		float fAngle1   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() );
		float fAngle2   = GetAngle( pNPC->GetWorldPosition(), m_pCreator->GetWorldPosition() );
		float fGapAngle = fabs( ioMath::AngleGap( fAngle1, fAngle2 ) );
		float fCheckAngle = m_fObjectSearchAngle * ( FLOAT1 - ( fDistSq / fSearchRange ) );
		if( fGapAngle <= fCheckAngle )
		{
			D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
			if( !CheckLeftRightPos( pNPC->GetWorldOrientation(), pNPC->GetWorldPosition(), FLOAT100, m_TargetData.GetTargetPos(), vTargetPos ) )
				return false;

			m_TargetData.SetTargetPos( vTargetPos );
			m_TargetData.SetTargetType( TARGET_POSITION );
			
			return true;
		}
	}
	return false;
}

bool	ioChampionAI::CheckTargetMonsterBlocking( float& fSearchRange )
{
	if( fSearchRange > FLOAT1 ) 
	{
		const BaseCharList &rkTargetList = m_pPlayStage->GetBaseCharList();
		BaseCharList::const_iterator iter;
		for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
		{
			ioBaseChar *pChar = (*iter);
			
			if( !pChar ) continue;
			if( !ToNpcChar( pChar ) ) continue;

			if( IsNPCBlocking( pChar, fSearchRange ) )
				return true;
		}
	}

	return false;
}

bool	ioChampionAI::CheckTargetBlocking( float& fSearchRange )
{
	// 타겟 선택 후 처리
	if( !m_TargetData.GetTargetName().IsEmpty() )
	{
		// 웨이 포인트 사이에 오브젝트가 있는지 검색
		CheckWaypointObstacles( fSearchRange );

		// 타겟 사이에 오브젝트가 있는지 검색
		CheckTargetObstacles( fSearchRange );

		// 타겟 사이에 몬스터가 있는지 검색
		CheckTargetMonsterBlocking( fSearchRange );
	}

	return false;
}

bool	ioChampionAI::CheckFlee( float& fSearchRange )
{
	if( !m_TargetData.GetTargetName().IsEmpty() )
	{
		// 대상이 계급 높을 때 도망 
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			if( m_TargetData.GetTargetLevel() >= m_pCreator->GetLevel() )
			{
				float fDistSq = GetTargetRange( m_TargetData.GetTargetPos() );
				if( fDistSq < m_fEscapeRange * m_fEscapeRange )
				{
					float fAngle   = GetAngle( m_TargetData.GetTargetPos(), m_pCreator->GetWorldPosition() ) + 180.0f;
					fAngle = ioMath::ArrangeHead( fAngle );
					D3DXQUATERNION qtAngle;
					D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );

					D3DXVECTOR3 kBackPos = m_TargetData.GetTargetPos();
					D3DXVECTOR3 vCurDir  = qtAngle * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					m_TargetData.SetTargetPos( m_pCreator->GetWorldPosition() + 200.0f * vCurDir );
					m_TargetData.SetTargetType( TARGET_ESCAPE );
					m_TargetData.SetTargetName( m_pCreator->GetCharName() );
					fSearchRange = 200.0f * 200.0f;

					return true;
				}
				else if( m_fEscapeRange > 0.0f )
				{
					// 거리가 멀면 그냥 중지
					m_TargetData.Init();
				}
			}
		}
	}

	return false;
}

bool	ioChampionAI::CanSearchTimeBeUpdated()
{
	float fSearchTime = FRAMEGETTIME() - m_fSearchTargetTime;
	if( fSearchTime < m_fCurSearchTime )
	{
		return false;
	}

	return true;
}

bool	ioChampionAI::FindTargetChar()
{
	if( CanSearchTimeBeUpdated() )
	{
	    m_fCurSearchTime = FRAMEGETTIME();	
		m_TargetData.Init();

		CheckGlobalTarget();
		CheckFixingTarget();
		CheckRandomAggroTarget();
		CheckWayPoint(); 

		float fSearchRange = m_fSearchRangeCurrent * m_fSearchRangeCurrent;	
		if( m_TargetData.GetTargetType() != TARGET_WAYPOINT )
		{
			ioHashString szFirstTarget;
		
			if( !SearchFixedTarget( szFirstTarget ) )
			{
				SearchTargetByDamage( szFirstTarget );
			}

			ioBaseChar *pFirstTarget = m_pCreator->GetBaseChar( szFirstTarget );
			if( !IsTargetIgnoreState( pFirstTarget ) )
			{
				SetAsTarget( pFirstTarget );
				fSearchRange = GetTargetRange( pFirstTarget->GetWorldPosition() );
			}
			else
			{
				if( IsOffensive() )
				{
					SetAsTarget( FindClosestTarget( fSearchRange ) );
				}
				SetToRoamingMonster( fSearchRange );
			}		
		}

		CheckFlee( fSearchRange );
		CheckTargetBlocking( fSearchRange );

		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
			m_fSearchRangeCurrent = m_fSearchRangeMax;

		return true;
	}

	return false;
}

bool	ioChampionAI::SetToRoamingMonster( float& fSearchRange )
{
	// 타겟이 없으면 로밍
	if( m_TargetData.GetTargetType() == TARGET_NONE )
	{
		// processed check death monster romaing
		if( m_bRoaming )
		{
			fSearchRange = 200.0f * 200.0f;
			m_TargetData.SetTargetPos( GetRoamingPoint() );
			m_TargetData.SetTargetType( TARGET_ROAMING );
			m_TargetData.SetTargetName( m_pCreator->GetCharName() );

			return true;
		}
	}

	return false;
}

bool	ioChampionAI::HasValidTarget()
{
	return !m_TargetData.GetTargetName().IsEmpty();
}

void	ioChampionAI::OldProcessAI()
{
	// 타겟 검색
	if( !IsRoaming() )
	{
		FindTargetChar();		// m_TargetData will be updated
	}

	// 타겟이 없으면 모든 키입력 중단.
	if( !HasValidTarget() )
	{
		OnClearAction();
	}
	else
	{
		// 실시간 타겟 위치 변경
		ioBaseChar *pTargetChar = NULL;
		if( m_TargetData.GetTargetType() == TARGET_BASECHAR )
		{
			pTargetChar = m_pPlayStage->GetBaseChar( m_TargetData.GetTargetName() );
			if( pTargetChar )
			{
				m_TargetData.SetTargetPos( pTargetChar->GetWorldPosition() );
			}
		}

		{
			// 타겟에 대한 행동
			OnDefense();
			if( m_DefenseController.IsDefense() )
			{
				m_DefenseController.OnDefenseAction( m_pCreator, pTargetChar );
			}
			else
			{
				m_ActionController.OnAttackAction( m_pCreator, pTargetChar );			
				if( !m_ActionController.IsAction() )
				{
					m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
												   ioAI::GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );
					m_JumpController.OnJumpAction( m_pCreator, pTargetChar );				
				}
				else if( IsActionToMove() )
				{
					m_MoveController.OnMoveAction( m_pCreator, pTargetChar, m_TargetData.GetTargetPos(), 
												   ioAI::GetTargetAngle( m_TargetData.GetTargetPos() ), GetTargetRange( m_TargetData.GetTargetPos() ) );
				}
			}
		}

		{   // 타겟에 대한 행동 결정
			if( !OnAction() )
			{				
				if( !OnMove() && !m_DefenseController.IsDefense() )
				{
					OnClearAction();
				}
				OnJump();				
			}
		}
	}
}

