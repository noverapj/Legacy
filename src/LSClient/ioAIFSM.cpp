#include "stdafx.h"
#include "ioAISymbolTable.h"
#include "ioAIFSM.h"

///////////////////////////////////////////////////////////////////////
ioAIFSMTransition::ioAIFSMTransition( const std::string& destination, const std::string& condition )
	:	m_szLinkedStateName( destination ), m_szCondition( condition ), m_pLinkedState( (ioAIFSMState*)0 )
{
	// currently do nothing for transition
}

ioAIFSMTransition::~ioAIFSMTransition()
{
	//SAFEDELETE( m_pLinkedState );	this is just a link : so do not delete
}

///////////////////////////////////////////////////////////////////////
ioAIFSMState::ioAIFSMState( ioXMLElement* node )
	:m_pBvT( (ioAIBvT*)0 )
{
	BuildState( node );
}

ioAIFSMState::~ioAIFSMState() 
{
//	m_pBvT->DetachDBElements();
	SAFEDELETE( m_pBvT );

	for( std::map<std::string, ioAIFSMTransition*>::iterator 
				it = m_mTransitions.begin();
				it != m_mTransitions.end();
				it++ )
	{
		SAFEDELETE( it->second );
	}
}

bool	ioAIFSMState::BuildState( ioXMLElement* node )
{
	if( node != (ioXMLElement*)0 && !node->IsEmpty() )
	{
		m_szName = node->GetStringAttribute_e( "name" );
		ioXMLElement*	cur = &(node->FirstChild());
		while( !cur->IsEmpty() )
		{
			std::string	tagName = cur->GetTagName();
			switch( g_AISymbolTable.GetBrainElemType( tagName ) )
			{
			case AI_BE_BEHAVIOR:	
				m_pBvT = new ioAIBvT( &(cur->FirstChild()), 0 );	
				break;
			case AI_BE_TRANS_LIST:	BuildTransitionList( cur );		break;
			}

			cur = &(cur->NextSibling());
		}

		return true;
	} 
	return false;
}

bool	ioAIFSMState::BuildTransitionList( ioXMLElement* node )
{
	if( node != (ioXMLElement*)0 && !node->IsEmpty() )
	{
		ioXMLElement*	cur = &(node->FirstChild());
		while( cur->IsEmpty() )
		{
			std::string	tagName = cur->GetTagName();
			switch( g_AISymbolTable.GetBrainElemType( tagName ) )
			{
			case AI_BE_TRANS:
				m_mTransitions[cur->GetStringAttribute_e("destination")] = 
					new ioAIFSMTransition(
						cur->GetStringAttribute_e("destination"),	cur->GetStringAttribute_e("condition")
					);
				break;
			}
			cur = &(node->NextSibling());
		}

		return true;
	}

	return false;
}

void	ioAIFSMState::VerifyState()
{
	if( m_pBvT != (ioAIBvT*)0 )
		m_pBvT->VerifyBvT();
}

bool	ioAIFSMState::CheckAndBuildTransition( ioAIFSMState* state )
{
	for( std::map<std::string, ioAIFSMTransition*>::iterator	
			it = m_mTransitions.begin();
			it != m_mTransitions.end();
			it++ )
	{
		if( state->GetName() == it->second->GetLinkedStateName() )
		{
			it->second->SetLinkedState( state );
		}
	}

	return true;
}

bool	ioAIFSMState::ProcessAI( ioBaseChar*& pCurTarget, ioChampionAI* pCAI )
{
	if( m_pBvT )
		return m_pBvT->ProcessBehavior( pCurTarget, pCAI );
	else return false;
}

void	ioAIFSMState::ResetCurrentBvT()
{
	if( m_pBvT )
		m_pBvT->Reset();
}

//////////////////////////////////////////////////////////////////////////
ioAIStateMachine::ioAIStateMachine( ioXMLElement* node )
	: m_pInitState( (ioAIFSMState*)0 ), m_pCurState( (ioAIFSMState*)0 )
{
	if( BuildFiniteStateMachine( node ) )
	{
		ioAIFSMState* state = m_mStateMap[node->GetStringAttribute_e( "initial_state" )];

		if( state == (ioAIFSMState*)0 )
		{
 			LOG.PrintTimeAndLog( 0, "Initialization failed : no initial sate name %s did not found on this FSM.",  
										node->GetStringAttribute_e( "initial_state" ) );
		}
		else
		{
			// see again when making state transition
			m_pInitState = m_pCurState = state;
		}
	}
	else LOG.PrintTimeAndLog( 0, "FSM Building failed." );
}

ioAIStateMachine::~ioAIStateMachine() 
{
	for( std::map<std::string, ioAIFSMState*>::iterator 
				it = m_mStateMap.begin();
				it != m_mStateMap.end();
				it++ )
	{
		SAFEDELETE( it->second );
	}
}

bool	ioAIStateMachine::BuildFiniteStateMachine( ioXMLElement* node )
{
	if( g_AISymbolTable.GetBrainElemType( node->GetTagName() ) )
	{
		ioXMLElement*	cur = &(node->FirstChild());
		while( !cur->IsEmpty() )
		{
			switch( g_AISymbolTable.GetBrainElemType( cur->GetTagName() ) )
			{
			case AI_BE_STATE:	AddState( new ioAIFSMState( cur ) );	break;
			}
			cur = &(node->NextSibling());
		}
		RebuildTransitionMap();

		return true;
	}

	return false;
}

bool	ioAIStateMachine::BuildTransition( ioAIFSMState* state )
{
	for( std::map<std::string, ioAIFSMState*>::iterator 
				it = m_mStateMap.begin();
				it != m_mStateMap.end();
				it++ )
	{
		// make new transition : fix it in future
		it->second->CheckAndBuildTransition( state );
	}

	return true;
}

bool	ioAIStateMachine::RebuildTransitionMap( ioAIFSMState* state )
{
	if( state == (ioAIFSMState*)0 )
	{
		for( std::map<std::string, ioAIFSMState*>::iterator it = m_mStateMap.begin();
								it != m_mStateMap.end(); it++ )
		{
			BuildTransition( it->second );
		}
		return true;
	}
	else	return BuildTransition( state );
}

void	ioAIStateMachine::AddState( ioAIFSMState* state )
{
	m_mStateMap[state->GetName()] = state;
	
	RebuildTransitionMap( state );
}

bool	ioAIStateMachine::ProcessAI( ioBaseChar*& pCurTarget, ioChampionAI* pCAI )
{
	// change state if need : later

	// process new AI according to curstate
	if( m_pCurState )
		return m_pCurState->ProcessAI( pCurTarget, pCAI );
	else return false;
}

void	ioAIStateMachine::VerifyStateMachine()
{
	for( std::map<std::string, ioAIFSMState*>::iterator  
					it = m_mStateMap.begin(); 
					it != m_mStateMap.end(); 
					it++ )
	{
		it->second->VerifyState();
	}
}

void	ioAIStateMachine::ResetCurrentBvT()
{
	if( m_pCurState )
		m_pCurState->ResetCurrentBvT();
}
