#include "stdafx.h"
#include "ioAIBrain.h"
#include "ioAISymbolTable.h"
#include "ioAIDatabase.h"
#include "ioChampionAI.h"

#include <regex>

#include "stringext.h"

/////////////////////////////////////////////////////////////////////////////////
ioAIExecUnit::ioAIExecUnit( AIExecOperatorType op, AIExecVariableType var, AIExecMethodType m, std::string args )
	: m_bHold( false )//, m_bLastResult( false )
{
	args = trim(args);
	m_pElem = NULL;
	switch( var )
	{
	case AI_BvT_AVT_MYSELF:
		switch( m )
		{
		case AI_BvT_AMT_Move:			m_pElem = new ioAIExec_Myself_Move( op, args );				break;
		case AI_BvT_AMT_DoKeySeq:		m_pElem = new ioAIExec_Myself_DoKeySeq( op, args );			break;
		case AI_BvT_AMT_StopAllActions:	m_pElem = new ioAIExec_Myself_StopAllActions( op );			break;
		case AI_BvT_AMT_IsHP:			m_pElem = new ioAIExec_Myself_IsHP( op, args );				break;
		case AI_BvT_AMT_IsCurrentState:	m_pElem = new ioAIExec_Myself_IsCurrentState( op, args );	break;
		case AI_BvT_AMT_ShowChat:		m_pElem = new ioAIExec_Myself_ShowChat( op, args );			break;
		case AI_BvT_AMT_Turn:			m_pElem = new ioAIExec_Myself_Turn( op, args );				break;
		case AI_BvT_AMT_Idle:			m_pElem = new ioAIExec_Myself_Idle( op, args );				break;
		case AI_BvT_AMT_IsCombo:		m_pElem = new ioAIExec_Myself_IsCombo( op, args );			break;
		case AI_BvT_AMT_CanDoCombo:		m_pElem = new ioAIExec_Myself_CanDoCombo( op, args );		break;
		case AI_BvT_AMT_WaitForCombo:	m_pElem = new ioAIExec_Myself_WaitForCombo( op, args );		break;
		}
		break;
	case AI_BvT_AVT_TARGET:
		switch( m )
		{
		case AI_BvT_AMT_IsTargetValid:	m_pElem = new ioAIExec_Target_IsTargetValid( op );			break;
		case AI_BvT_AMT_IsDistance:		m_pElem = new ioAIExec_Target_IsDistance( op, args );		break;
		case AI_BvT_AMT_IsHP:			m_pElem = new ioAIExec_Target_IsHP( op, args );				break;
		case AI_BvT_AMT_IsCurrentState:	m_pElem = new ioAIExec_Target_IsCurrentState( op, args );	break;
		}
		break;
	case AI_BvT_AVT_GLOBAL:
		switch( m )
		{
		case AI_BvT_AMT_GetTarget:		m_pElem = new ioAIExec_Global_GetTarget( op, args );		break;
		case AI_BvT_AMT_GetTargetByHP:	m_pElem = new ioAIExec_Global_GetTargetByHP( op, args );	break;
		case AI_BvT_AMT_IsTarget:		m_pElem = new ioAIExec_Global_IsTarget( op, args );			break;
		}
		break;
	}
}

ioAIExecUnit::~ioAIExecUnit()
{
	SAFEDELETE(m_pElem); 
}

bool	ioAIExecUnit::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	bool bResult = false;
	if( m_pElem )
	{
		bResult = m_pElem->Execute( pTarget, pCAI );

		// set holding 
		switch( m_pElem->GetType() )
		{
		default:
			m_bHold = false;
			break;
		case AI_EXEC_TARGET_IsCurrentState:
			m_bHold = pCAI->GetPollingTimer()->IsRunning();
			break;
		case AI_EXEC_MYSELF_DoKeySeq:			
		case AI_EXEC_MYSELF_Move:	
		case AI_EXEC_MYSELF_Idle:	
		case AI_EXEC_MYSELF_WaitForCombo:
			m_bHold = true;
			break;
		}
	}

	return bResult;
}

bool	ioAIExecUnit::GetLastResult()
{
	if( m_pElem != (ioAIBaseExecElem*)0 )
	{
		return m_pElem->GetLastResult();
	}
	else return false;
}

std::string	ioAIExecUnit::GetDebugString()
{
	std::string szDebugStr;
	if( m_pElem )
	{
		szDebugStr = m_pElem->GetDebugString();
	}

	return szDebugStr;
}
