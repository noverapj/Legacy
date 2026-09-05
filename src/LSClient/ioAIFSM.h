#pragma once

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include <map>
#include <vector>
#include <string>

#include "ioAIDefinitions.h"
#include "ioAIBehaviorTree.h"

class	ioAIFSMState;
class	ioAIFSMTransition
{
public:
	ioAIFSMTransition( const std::string& destination, const std::string& condition );
	~ioAIFSMTransition();

	std::string	GetLinkedStateName()	{	return m_szLinkedStateName;	}
	ioAIFSMState*	GetLinkedState()		{	return m_pLinkedState;		}
	std::string	GetCondition()			{	return m_szCondition;		}

	void	SetLinkedState( ioAIFSMState* state )	{	m_pLinkedState = state;	}
	void	SetCondition( std::string& cond )		{	m_szCondition = cond;	}
	void	SetLink( ioAIFSMState* state, std::string& cond )
	{
		SetLinkedState( state );
		SetCondition( cond );
	}

protected:
	std::string		m_szLinkedStateName;
	ioAIFSMState*	m_pLinkedState;
	std::string		m_szCondition;
};

class	ioAIFSMState
{
public:
	ioAIFSMState( ioXMLElement* node );
	~ioAIFSMState();

	std::string&	GetName()	{	return m_szName;	}
	
	bool	BuildState( ioXMLElement* node );
	bool	BuildTransitionList( ioXMLElement* node );
	bool	CheckAndBuildTransition( ioAIFSMState* state );
	void	VerifyState();

	bool	ProcessAI(	ioBaseChar*& pCurTarget, ioChampionAI* pCAI );
	
	void	ResetCurrentBvT();

protected:
	std::string				m_szName;
	ioAIBvT*				m_pBvT;

	std::map<std::string, ioAIFSMTransition*>	m_mTransitions;
};

class	ioAIStateMachine
{
public:
	ioAIStateMachine( ioXMLElement* node );
	~ioAIStateMachine();

	bool	BuildFiniteStateMachine( ioXMLElement* node );
	bool	RebuildTransitionMap( ioAIFSMState* state = (ioAIFSMState*)0 );
	bool	BuildTransition( ioAIFSMState* state );

	void	AddState( ioAIFSMState* state );
	void	AddState( ioAIFSMState* state, ioAIFSMTransition* tran );

	bool	ProcessAI( ioBaseChar*& pCurTarget, ioChampionAI* pCAI );

	void	VerifyStateMachine();
	void	ResetCurrentBvT();

protected:
	std::map<std::string, ioAIFSMState*>	m_mStateMap;
	
	ioAIFSMState*	m_pInitState;
	ioAIFSMState*	m_pCurState;
};
