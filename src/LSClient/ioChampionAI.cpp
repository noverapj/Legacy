#include "StdAfx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"
#include "ioChampionAI.h"
#include "ioTowerDefMode.h"

#include "ioAISymbolTable.h"
#include "ioAIDatabase.h"
#include "ioAIBaseExecElem.h"

ioChampionAI::ioChampionAI( ioNpcChar *pCreator, ioPlayStage *pPlayStage )
	: ioAI( pCreator, pPlayStage ), m_iPrevDir( -1 )
{
	m_pBrain = (ioAIBrain*)0;
	m_pCurTarget = NULL;
}

ioChampionAI::~ioChampionAI()
{
	if( m_pCurTarget )
		m_pCurTarget->ReleaseAIRef( this );
	SAFEDELETE( m_pBrain );
}

AIType ioChampionAI::GetAIType() const
{
	return AI_CHAMPION;
}

//---------------------------------------------------------------------
// 
// Methods that called from terminal nodes
//
void	ioChampionAI::StopCurrentAction()
{
	if( !m_ExecTimer.IsRunning() )
	{
		if( IsDebugging( ioAIBrain::AIDEBUG_TIMER ) )
			LOG.PrintTimeAndLog( 0, "[%u] Execution timer stopped by Idle operation", FRAMEGETTIME() );
		m_ExecTimer.StopTimer();
	}

	m_vKeySeq.clear();
}

void	ioChampionAI::StopAllActions()
{
	StopCurrentAction();
	if( m_pBrain )
		m_pBrain->ResetCurrentBvT();
}

ioNpcChar*	ioChampionAI::GetMyself()
{
	return m_pCreator;
}

float	ioChampionAI::GetDistance( ioBaseChar* pChar )		
{ 
	return GetTargetRange( m_pCreator->GetWorldPosition(), pChar->GetWorldPosition() );
}

ioBaseChar*	ioChampionAI::FindClosestTarget( const float& fInitialSearchRange )
{
	BaseCharList	vTargetList;
	if( FetchTargetableCharList( vTargetList ) )
	{
		return ChooseClosestTarget( fInitialSearchRange, vTargetList );
	}

	return (ioBaseChar*)0;
}

void	ioChampionAI::ClearCurKeySeq()
{
	m_vKeySeq.clear();
}

void	ioChampionAI::AddCurKeySeq(	ioAIKeyBehavior& ka, 
									int inputTypeOverride, 
									int stateTypeOverride, 
									unsigned int durationOverride )
{
	if( inputTypeOverride != -100 )	
		ka.m_inputType = (AIKeyInputType)inputTypeOverride;
	if( stateTypeOverride != -100 )	
		ka.m_stateType = (AIKeyStateType)stateTypeOverride;
	if( durationOverride != 0 )	
		ka.m_duration = (unsigned int)durationOverride;
	
	m_vKeySeq.push_back( ka );
}

void	ioChampionAI::AppendCurKeySeq( const std::string& szKeySeqName,
										int	inputTypeOverride,
										int	stateTypeOverride,
										unsigned int durationOverride )
{
	KeySeqMap::iterator	it = m_mKeySeqMap.find( szKeySeqName );
	if( it == m_mKeySeqMap.end() )
	{
		g_AIDB.CopyKeySequence( szKeySeqName, m_mKeySeqMap[szKeySeqName] );
//		LOG.PrintTimeAndLog( 0, "Key Sequence [%s] has been loaded from DB", szKeySeqName.c_str() ); 
	}

	for( KeySeq::iterator 
			it = m_mKeySeqMap[szKeySeqName].begin(); 
			it != m_mKeySeqMap[szKeySeqName].end(); 
			it++ )
	{
		ioAIKeyBehavior ka = *it;
		AddCurKeySeq( ka, inputTypeOverride, stateTypeOverride, durationOverride );
	}
}

void	ioChampionAI::CopyCurKeySeq( const std::string& szKeySeqName,
										int	inputTypeOverride,
										int	stateTypeOverride,
										unsigned int durationOverride )
{
	// copy key sequence from key sequence map
	ClearCurKeySeq();
	AppendCurKeySeq( szKeySeqName, inputTypeOverride, stateTypeOverride, durationOverride );
}

void	ioChampionAI::ExecuteCurKeySeq()
{
	if( m_ExecTimer.HasTimeoutOccured() )
	{
		m_ExecTimer.StopTimer();
	}

	if( !m_ExecTimer.IsRunning() && m_vKeySeq.size() > 0 )
	{
		m_vKeySeq.at(0).DoKeyAction( m_pBrain->GetCurTarget(), m_pCreator, this );
		m_ExecTimer.StartTimer( m_vKeySeq.at(0).m_duration );
		m_vKeySeq.erase( m_vKeySeq.begin() );
	}
}

ioAITimer*		ioChampionAI::GetExecTimer()
{
	return &m_ExecTimer;
}

ioAITimer*		ioChampionAI::GetIdleTimer()
{
	return &m_IdleTimer;
}

ioAITimer*		ioChampionAI::GetPollingTimer()
{
	return &m_PollingTimer;
}

void		ioChampionAI::WaitForCombo( unsigned int frontMargin, unsigned int tailMargin )
{
	if( frontMargin > 0 )	m_nComboFrontMargin	= frontMargin;
	if( tailMargin > 0 )	m_nComboTailMargin	= tailMargin;

	m_ComboTimer.StopTimer();
	m_ComboTimer.StartTimer( 500 );	// 500 is default timer for combo
}

void	ioChampionAI::SetPollingUnit( ioAIBaseExecElem* unit )
{
	m_pCurPollingElem = unit;
}

void	ioChampionAI::PushCallStack( std::vector<ioAIBvTNode*>::iterator it )
{
	m_vCallStack.push_back( it );
}

void	ioChampionAI::ResetCallStack()
{
	m_vCallStack.clear();
}

int		ioChampionAI::GetCallStackCount()
{
	return m_vCallStack.size();
}

std::vector<ioAIBvTNode*>::iterator	ioChampionAI::PopCallStack()
{
	//LOG.PrintTimeAndLog( 0, "Poping callstack... : number of elements %d ", m_vCallStack.size() );
	std::vector<ioAIBvTNode*>::iterator it = m_vCallStack.back();
	m_vCallStack.pop_back();

	return it;
}

float	ioChampionAI::GetTargetAngle( ioBaseChar* pCurTarget )
{
	if( pCurTarget != (ioBaseChar*)0 )
	{
		float angle = ioAI::GetTargetAngle( pCurTarget->GetWorldPosition() );

		return angle;
	} else return 0.0f;
}

bool	ioChampionAI::IsCallStackEmpty()
{
	return m_vCallStack.empty();
}

int ioChampionAI::GetCounter( const std::string& name )
{
	std::map<std::string,int>::iterator	 it = m_mCounter.find( name );
	if( it != m_mCounter.end() )
	{
		return m_mCounter[name];
	}
	else return 0;
}

bool ioChampionAI::SetCounter( const std::string& name, const int& val )
{
	m_mCounter[name] = val;
	std::map<std::string,int>::iterator	 it = m_mCounter.find( name );
	if( it != m_mCounter.end() )
	{
		return true;
	}
	else return false;
}

//---------------------------------------------------------------------
void	ioChampionAI::ProcessAI()
{
	std::string		pos;

	if( m_PollingTimer.IsRunning() )
	{
		pos = "1 : polling";
		ioBaseChar*	pTarget = m_pBrain->GetCurTarget();
		bool		bResult = false;
		if( m_pCurPollingElem != (ioAIBaseExecElem*)0 )
		{
			bResult = m_pCurPollingElem->Execute( pTarget, this );
		}
		else m_ComboTimer.StopTimer();	// stop timer when execution unit is missing

		if( m_PollingTimer.HasTimeoutOccured() || bResult )
		{
			m_PollingTimer.StopTimer();
			m_pCurPollingElem = (ioAIBaseExecElem*)0;
		}

	}
	else if( m_IdleTimer.IsRunning() )
	{
		pos = "2 : idle";
		if( m_IdleTimer.HasTimeoutOccured() )
		{
			m_IdleTimer.StopTimer();
		}
	}
	else if( m_ExecTimer.IsRunning() || m_vKeySeq.size() > 0 )
	{
		pos = "3 : exec";
		ExecuteCurKeySeq();
	}
	else if( m_ComboTimer.IsRunning() )
	{
		pos = "4 : waiting combo";
		if( GetMyself()->GetComboAttackTimeline( m_nComboFrontMargin, m_nComboTailMargin ) >= 0 || 
			m_ComboTimer.HasTimeoutOccured() )
		{
			m_ComboTimer.StopTimer();
		}
	}
	else
	{
		pos = "5 : process brain";
		if( m_pBrain != (ioAIBrain*)0 )
			m_pBrain->ProcessAI( this );
	}

	//LOG.PrintTimeAndLog( 0, "%s", pos.c_str() );
}

void ioChampionAI::LoadAI( ioXMLDocument &xmlFile )
{
	ioXMLElement*	cur = &(xmlFile.GetRootElement());

	if( !cur->IsEmpty() )
	{
		if( g_AISymbolTable.GetDefElemType( cur->GetTagName() ) == AI_DE_NPCAI_DEF )
		{
			cur = &(cur->FirstChild());
			while( !cur->IsEmpty() )
			{
				switch( g_AISymbolTable.GetDefElemType( cur->GetTagName() ) )
				{
				case AI_DE_INFO:	m_pCreator->LoadMonsterInfo( cur );		break;
				case AI_DE_MENT:	m_pCreator->LoadMonsterMent( cur );		break;
				case AI_DE_BRAIN:	m_pBrain = new ioAIBrain( cur );		break;
				}
				cur = &(cur->NextSibling());
			}
		}
	}
}

bool	ioChampionAI::FetchTargetableCharList( BaseCharList& vTargetList )
{
	D3DXVECTOR3 vPos = m_pCreator->GetMidPositionByRate( FLOAT08 ) + ( m_pCreator->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) );
	m_pCreator->GetNpcTargetCharListByCylinder( vPos, m_fSearchRangeMax, m_fTargetSearchAngle, 50000.0f, 50000.0f, vTargetList );
	return ( vTargetList.size() > 0 );
}

ioBaseChar*	ioChampionAI::ChooseClosestTarget( const float& fInitialSearchRange, BaseCharList& vTargetList )
{
	ioBaseChar*	pRetChar = (ioBaseChar*)0;

	for( BaseCharList::const_iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);
		if( !IsTargetIgnoreState( pChar ) ) 
		{
			// update current winner : only closest one will survive
			float fDistSq = GetTargetRange( pChar->GetWorldPosition() );
			if( fInitialSearchRange < 0.0f )
			{
				fDistSq = fInitialSearchRange;
				pRetChar = pChar;
			}
			else if( fDistSq < fInitialSearchRange )
			{
				pRetChar = pChar;
			}
		}
	}

	SetCurTarget( pRetChar );

	return pRetChar;
}

bool ioChampionAI::IsDebugging( ioAIBrain::AIDebugType type )
{
	if( m_pBrain != (ioAIBrain*)0 )
		return m_pBrain->IsDebugging( type );
	else return false;
}

ioBaseChar* ioChampionAI::GetRandomTarget()
{
	BaseCharList	vTargetList;
	if( FetchTargetableCharList( vTargetList ) )
	{
		std::random_shuffle( vTargetList.begin(), vTargetList.end() );

		ioBaseChar* pRetChar = (ioBaseChar*)0;
		for( BaseCharList::const_iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
		{
			ioBaseChar *pChar = (*iter);
			if( !ioAI::IsTargetIgnoreState( pChar ) && !ioAI::IsTargetIgnoreState( pChar ) ) 
			{
				pRetChar = pChar;
				SetCurTarget( pRetChar );
				return pRetChar;
			}
		}
	}

	return (ioBaseChar*)0;
}

ioBaseChar*	ioChampionAI::FindTargetByHP( const AICompareType& eCompare, const float& fHPRatio )
{
	BaseCharList	vTargetList;
	if( FetchTargetableCharList( vTargetList ) )
	{
		ioBaseChar* pRetChar = (ioBaseChar*)0;

		float fCurHP = 0.0f;
		float fMaxHP = 0.0f;

		for( BaseCharList::const_iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
		{
			ioBaseChar *pChar = (*iter);
			// pChar Null check -> IsTargetIgnoreState inside.. 
			if( !IsTargetIgnoreState( pChar ) ) 
			{
				fCurHP = pChar->GetHP().m_fCurValue;
				fMaxHP = pChar->GetHP().m_fMaxValue;

				switch( eCompare )
				{
				case AI_CMP_BIGGER:
					{
						if( fCurHP > fMaxHP * fHPRatio )
						{
							pRetChar = pChar;
							SetCurTarget( pRetChar );
							return pRetChar;
						}
					}
					break;
				case AI_CMP_SMALLER:
					{
						if( fCurHP < fMaxHP * fHPRatio )
						{
							pRetChar = pChar;
							SetCurTarget( pRetChar );
							return pRetChar;
						}
					}
					break;
				case AI_CMP_EQUAL:
					{
						if( fCurHP == fMaxHP * fHPRatio )
						{
							pRetChar = pChar;
							SetCurTarget( pRetChar );
							return pRetChar;
						}
					}
					break;
				}
			}
		}
	}

	return (ioBaseChar*)0;
}

void ioChampionAI::SetCurTarget( ioBaseChar* pTarget )
{
	if( m_pCurTarget != pTarget )
	{
		if( m_pCurTarget )
			m_pCurTarget->ReleaseAIRef( this );

		m_pCurTarget = pTarget;

		if( m_pCurTarget )
			m_pCurTarget->AddAIRef( this );
	}
}

void ioChampionAI::ChangeTarget()
{
	m_pCurTarget = FindClosestTarget();
}

ioBaseChar* ioChampionAI::FindMostAggroTarget()
{
	BaseCharList	vTargetList;
	if( FetchTargetableCharList( vTargetList ) )
	{
		return ChooseAggroTarget( vTargetList );
	}

	return (ioBaseChar*)0;
}

ioBaseChar* ioChampionAI::ChooseAggroTarget( BaseCharList& vTargetList )
{
	// Aggro 추가
	// 레코드 테이블에서 Damage 수치를 기록해서 그걸 기준으로..
	// 조건1. 세팅 거리 안에 있으면 그 중에서 가장 가까운 캐릭터
	// 조건2. 세팅 거리 밖에 있으면 그 중에서 가장 어그로 높은 캐릭터

	ioBaseChar*	pRetChar = (ioBaseChar*)0;
	bool bChange = false;
	bool bNearSearch = false;
	static float sfNearDistance = 20000.f;
	float fNearDistance = sfNearDistance;
	int iNearAggro = -1;

	for( BaseCharList::const_iterator iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pChar = (*iter);
		if( !IsTargetIgnoreState( pChar ) ) 
		{
			// update current winner : only closest one will survive
			float fDistSq = GetTargetRange( pChar->GetWorldPosition() );

			if( fDistSq < fNearDistance )
			{
				if( m_pCurTarget != pChar )
					bChange = true;

				pRetChar = pChar;
				fNearDistance = fDistSq;
				bNearSearch = true;
			}
			else if ( !bNearSearch )
			{	
				int iAggro = m_pCreator->GetAggro( pChar->GetCharName() );
				if( iAggro > iNearAggro )
				{
					if( m_pCurTarget != pChar )
						bChange = true;

					iNearAggro = iAggro;
					pRetChar = pChar;
				}
			}
		}
	}

	if( pRetChar )
		SetCurTarget( pRetChar );

	return pRetChar;
}

float ioChampionAI::GetTargetPosAngle( D3DXVECTOR3 vTargetPos )
{
	return ioAI::GetTargetAngle( vTargetPos );
}