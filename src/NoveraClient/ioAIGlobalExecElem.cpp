#include "stdafx.h"

#include <regex>

#include "ioAIMyselfExecElem.h"
#include "ioBaseChar.h"

#include "ioChampionAI.h"
#include "ioAISymbolTable.h"

///////////////////////////////////////////////////////////////////////
ioAIExec_Global_GetTarget::ioAIExec_Global_GetTarget(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_GLOBAL_GetTarget, op )
{
	SetArgs( args );
}

void ioAIExec_Global_GetTarget::SetArgs( const std::string& args )
{
	std::regex	reg( g_AISymbolTable.GetSymbolString("targeting") );	// more strong?
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		// extract to symbol table?
		m_Targeting = (TargetingType)g_AISymbolTable.GetSymbolValue( "TargetingType" ,std::string(sm[1]) );
	}
	else	
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Global_GetTarget with : %s", args.c_str() );
}

bool ioAIExec_Global_GetTarget::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	ioBaseChar* pBase = NULL;
	if( pCAI )
	{
		switch( m_Targeting )
		{
		// this is not good design 
		// since this routing will find the target that is closest to owner of pCAI
		// but we'll still place this routine here
		case AI_TARGETING_CLOSEST:	
			pTarget = pCAI->FindClosestTarget();		
			m_bLastResult = ( pTarget != NULL );		
			break;
		case AI_TARGETING_MOSTAGGRO:
			pBase = pCAI->FindMostAggroTarget();
			if( pBase )
			{
				m_bLastResult = ( pBase != pTarget );
				pTarget = pBase;
			}
			break;
		case AI_TARGETING_MOSTAGGRO_RANDOM_CLOSE:
			{
				IORandom eRandom;
				eRandom.SetRandomSeed( timeGetTime() );

				int iRand = eRandom.Random( 100 );
				if( iRand < 80 )
				{
					pBase = pCAI->FindMostAggroTarget();
					if( pBase )
					{
						m_bLastResult = ( pBase != pTarget );
						pTarget = pBase;
						break;
					}
				}

				pTarget = pCAI->FindClosestTarget();	
				m_bLastResult = ( pTarget != NULL );		
			}
			break;
		case AI_TARGETING_RANDOM:
			{
				pTarget = pCAI->GetRandomTarget();	
				m_bLastResult = ( pTarget != NULL );		
			}
			break;
		}
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Myself_IsCombo : pCAI[%x] is null", pCAI );

	return m_bLastResult;
}

std::string ioAIExec_Global_GetTarget::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Global_GetTarget {m_Targeting=%d}", m_Targeting );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Global_GetTargetByHP::ioAIExec_Global_GetTargetByHP(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_GLOBAL_GetTargetByHP, op )
{
	SetArgs( args );
}

void ioAIExec_Global_GetTargetByHP::SetArgs( const std::string& args )
{
	std::regex	reg( g_AISymbolTable.GetSymbolString("isHP") );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		m_Compare = g_AISymbolTable.GetCompareType( sm[1] );
		if( m_Compare != AI_CMP_ERROR )
		{
			m_fRate = ConvertToFloat( sm[2] ) / FLOAT100;
		}
		else m_fRate = 0.0f;
	}
	else m_fRate = 0.0f;
}

bool ioAIExec_Global_GetTargetByHP::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	ioBaseChar* pBase = NULL;
	if( pCAI )
	{
		pBase = pCAI->FindTargetByHP( m_Compare, m_fRate );
		if( pBase )
		{
			m_bLastResult = ( pBase != pTarget );
			pTarget = pBase;
		}
	}

	return m_bLastResult;
}

std::string ioAIExec_Global_GetTargetByHP::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf_e(buff, "ioAIExec_Global_GetTargetByHP");
	retStr = buff;

	return retStr;
}

ioAIExec_Global_IsTarget::ioAIExec_Global_IsTarget( AIExecOperatorType op, const std::string& args )
	: ioAIBaseExecElem( AI_EXEC_GLOBAL_IsTarget, op )
{
	SetArgs( args );
}
bool ioAIExec_Global_IsTarget::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	ioBaseChar* pBase = NULL;
	if( pCAI && pTarget )
		m_bLastResult = true;

	return m_bLastResult;
}

std::string ioAIExec_Global_IsTarget::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf_e(buff, "ioAIExec_Global_IsTarget {m_Targeting=%d}", m_Targeting );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Global_Counter::ioAIExec_Global_Counter(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_GLOBAL_GetTarget, op )
{
	SetArgs( args );
}

void ioAIExec_Global_Counter::SetArgs( const std::string& args )
{
	std::regex	reg( g_AISymbolTable.GetSymbolString("counter") );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		// extract to symbol table?
		if( sm[2] == "INCREASE" )		m_CounterType = AI_COUNTER_INCREASE;
		else if( sm[2] == "DECREASE" )	m_CounterType = AI_COUNTER_DECREASE;
		else if( sm[2] == "SETTO" )		m_CounterType = AI_COUNTER_SETTO;
		else if( sm[2] == "BIGGER" )	m_CounterType = AI_COUNTER_ISBIGGER;
		else if( sm[2] == "SMALLER" )	m_CounterType = AI_COUNTER_ISSMALLER;
		else if( sm[2] == "EQUAL" )		m_CounterType = AI_COUNTER_ISEQUAL;
	}
	else	
		LOG.PrintTimeAndLog( 0, "failed to parse the given arg in ioAIExec_Global_Counter with : %s", args.c_str() );

	m_nStep = ConvertToInt( sm[3] );
}

bool ioAIExec_Global_Counter::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;

	int		count = pCAI->GetCounter( m_szCounterName );
	switch( m_CounterType )
	{
	case AI_COUNTER_INCREASE:	count += m_nStep;	m_bLastResult = pCAI->SetCounter( m_szCounterName, count );	break;
	case AI_COUNTER_DECREASE:	count += m_nStep;	m_bLastResult = pCAI->SetCounter( m_szCounterName, count );	break;
	case AI_COUNTER_SETTO:		count = m_nStep;	m_bLastResult = pCAI->SetCounter( m_szCounterName, count );	break;
	case AI_COUNTER_ISBIGGER:	m_bLastResult = ( count > m_nStep );		break;
	case AI_COUNTER_ISSMALLER:	m_bLastResult = ( count < m_nStep );		break;
	case AI_COUNTER_ISEQUAL:	m_bLastResult = ( count == m_nStep );		break;
	}

	return m_bLastResult;
}

std::string ioAIExec_Global_Counter::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Global_Counter {m_szCounterName=\"%s\"m_CounterType=%d,m_nStep=%d}", 
						m_szCounterName.c_str(), m_CounterType, m_nStep );
	retStr = buff;

	return retStr;
}
