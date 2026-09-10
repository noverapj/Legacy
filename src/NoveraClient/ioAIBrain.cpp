#include "stdafx.h"
#include "ioAIBrain.h"
#include "ioAISymbolTable.h"
#include "ioChampionAI.h"

////////////////////////////////////////////////////////////////////////////////////////////
ioAIBrain::ioAIBrain( ioXMLElement* node )
	: m_pFSM(( ioAIStateMachine*)0 ), m_iCurDebug( 0 )
{
	BuildBrain( node );
}
ioAIBrain::~ioAIBrain() 
{
	SAFEDELETE( m_pFSM );
}

bool ioAIBrain::BuildBrain( ioXMLElement* node )
{
	if( node != (ioXMLElement*)0 )
	{
		if( !node->IsEmpty() )
		{
			if( g_AISymbolTable.GetDefElemType( node->GetTagName() ) == AI_DE_BRAIN )
			{
				// set debugging info
				m_iCurDebug = node->GetIntAttribute_e( "debug" );

				//LOG.PrintTimeAndLog( 0, "DEBUG = %d", m_iCurDebug );

				// only 1 FSM for now
				ioXMLElement* child = &(node->FirstChild( "FSM" ));
				if( !child->IsEmpty() )
				{
					m_pFSM = new ioAIStateMachine( child );
				}

				//m_pAIDirector = new ioAIDirector( node );

				if( IsDebugging( ioAIBrain::AIDEBUG_SHOW_BVT ) )
					m_pFSM->VerifyStateMachine();

				return true;
			}
		}
	}

	return false;
}

bool ioAIBrain::ProcessAI( ioChampionAI* pCAI )
{
	m_pTarget = pCAI->GetCurTarget();
	if( m_pFSM )
		return m_pFSM->ProcessAI( m_pTarget, pCAI );
	else
		return false;
}

void ioAIBrain::ResetCurrentBvT()
{
	if( m_pFSM )
		m_pFSM->ResetCurrentBvT();
}

bool ioAIBrain::IsDebugging( AIDebugType type )
{
	long t = m_iCurDebug & type;

	//if( t != 0 ) LOG.PrintTimeAndLog( 0, "DEBUGGING = %d", t );

	return ( t != 0 );
}
void ioAIBrain::ChangeTarget( ioChampionAI* pCAI )
{
	ioBaseChar* pNewTarget = pCAI->FindClosestTarget();
	// NULL 이라도 들어가야 됨
	pCAI->SetCurTarget( pNewTarget );
}
