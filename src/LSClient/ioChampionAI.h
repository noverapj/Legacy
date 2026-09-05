#pragma once
#include "ioAI.h"
#include "ioAIBrain.h"
#include "ioAITimer.h"

class	ioAIExecElem;
class	ioChampionAI : public ioAI
{
public:
	ioChampionAI( ioNpcChar *pCreator, ioPlayStage *pPlayStage );
	ioChampionAI::~ioChampionAI();

public:
	void	ProcessAI();
	void	OldProcessAI();

	bool	FindTargetChar();

	void	LoadAI( ioXMLDocument &xmlFile );
	AIType	ioChampionAI::GetAIType() const;

public:
	/////////////////////////////////////////////////////////////////////////
	//
	// these functions will be used from ioAIExecUnit class directly
	//
	
	// object retreival operations : get objects from previosly built ioAI object
	ioNpcChar*	GetMyself();

	// target character operations
	float		GetTargetAngle( ioBaseChar* pCurTarget );
	float		GetTargetPosAngle( D3DXVECTOR3 vTargetPos  );
	ioBaseChar*	GetRandomTarget( );
	ioBaseChar*	FindTargetByHP( const AICompareType& eCompare, const float& fHPRatio );
	ioBaseChar*	FindMostAggroTarget();

	ioBaseChar*	GetCurTarget()	{ return m_pCurTarget; }
	void		SetCurTarget( ioBaseChar* pTarget );//{ m_pCurTarget = pTarget; }
	void		ChangeTarget();
	ioBaseChar*	ChooseAggroTarget( BaseCharList& vTargetList );

	ioBaseChar*	FindClosestTarget( const float& fInitialSearchRange = -1.0 );
	float		GetDistance( ioBaseChar* pChar );

	// action control operations
	void		StopCurrentAction();
	void		StopAllActions();
	void		ClearCurKeySeq();
	void		AddCurKeySeq(	ioAIKeyBehavior& ka, 
								int inputTypeOverride, 
								int stateTypeOverride, 
								unsigned int durationOverride );
	void		AppendCurKeySeq(	const std::string& szKeySeqName, 
									int	inputTypeOverride = -100,
									int	stateTypeOverride = -100,
									unsigned int durationOverride = 0 );
	void		CopyCurKeySeq(	const std::string& szKeySeqName, 
								int	inputTypeOverride = -100,
								int	stateTypeOverride = -100,
								unsigned int durationOverride = 0 );
	void		SetPrevDirection( int dir )	{	m_iPrevDir = dir;	}
	int			GetPrevDirection()			{	return m_iPrevDir;	}

	// time related operations
	ioAITimer*	GetExecTimer();
	ioAITimer*	GetIdleTimer();
	ioAITimer*	GetPollingTimer();
	void		WaitForCombo( unsigned int frontMargin = 0, unsigned int tailMargin = 0 );
	void		SetPollingUnit( ioAIBaseExecElem* unit );

	// call stack operations : to manage call stack of BvT
	void		ResetCallStack();
	int			GetCallStackCount();
	void		PushCallStack( std::vector<ioAIBvTNode*>::iterator it );
	std::vector<ioAIBvTNode*>::iterator		
				PopCallStack();
	bool		IsCallStackEmpty();

	// global variable control
	int			GetCounter( const std::string& name );
	bool		SetCounter( const std::string& name, const int& val );

	// helper function
	bool		IsDebugging( ioAIBrain::AIDebugType type );

protected:
	bool	SearchFixedTarget( ioHashString& szFirstTarget );
	bool	SearchTargetByDamage( ioHashString& szFirstTarget );

	bool	FetchTargetableCharList( BaseCharList& vTargetList );
	bool	CanSearchTimeBeUpdated();

	bool	CheckTargetBlocking( float& fSearchRange );
	bool	CheckFlee( float& fSearchRange );
	
	bool	CheckWaypointObstacles( float& fSearchRange );
	bool	CheckTargetObstacles( float& fSearchRange );
	bool	CheckTargetMonsterBlocking( float& fSearchRange );

	bool	IsNPCBlocking( ioBaseChar* pChar, float& fSearchRange );

	bool	SetToRoamingMonster( float& fSearchRange );

	bool	HasValidTarget();
	void	ExecuteCurKeySeq();

private:
	bool		SetAsTarget( ioBaseChar* pChar );
	ioBaseChar*	ChooseClosestTarget( const float& fInitialSearchRange, BaseCharList& vTargetList );

protected:
	ioAIBrain*	m_pBrain;
	ioBaseChar*	m_pCurTarget;

	// timers
	ioAITimer	m_IdleTimer;
	ioAITimer	m_ExecTimer;
	ioAITimer	m_ComboTimer;
	ioAITimer	m_PollingTimer;

	// timing control objects
	ioAIBaseExecElem*	m_pCurPollingElem;
	unsigned int		m_nComboFrontMargin, m_nComboTailMargin;
	int					m_iPrevDir;

	typedef	std::vector<ioAIKeyBehavior>	KeySeq;
	typedef std::map<std::string,KeySeq>	KeySeqMap;

	KeySeq			m_vKeySeq;
	KeySeqMap		m_mKeySeqMap;

	DWORD	m_dwCurrentActionTimestamp;
	DWORD	m_dwLastIdleTimestamp;
	DWORD	m_dwCurrentIdleTimeDuration;
	
	typedef std::vector<std::vector<ioAIBvTNode*>::iterator>	CallStack;

	CallStack	m_vCallStack;

	// global variables
	std::map<std::string,int>	m_mCounter;
};

inline ioChampionAI* ToChampionAI( ioAI *pAIObj )
{
	if( !pAIObj || pAIObj->GetAIType() != AI_CHAMPION )
		return NULL;

	return dynamic_cast< ioChampionAI* >(pAIObj);
}