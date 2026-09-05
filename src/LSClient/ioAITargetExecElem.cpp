#include "stdafx.h"

#include <regex>

#include "ioAIBaseExecElem.h"
#include "ioAITargetExecElem.h"
#include "ioBaseChar.h"
#include "ioChampionAI.h"
#include "ioAISymbolTable.h"

#include "AniEventHelpStruct.h"

#include "stringext.h"

ioAIExec_Target_IsHP::ioAIExec_Target_IsHP( AIExecOperatorType op, const std::string& args )
	: ioAIIsHP( AI_EXEC_TARGET_IsHP, op, args )
{
//	SetArgs( args );
}

void ioAIExec_Target_IsHP::SetArgs( const std::string& args )
{
//	ioAIIsHP::SetArgs( args );
}

bool ioAIExec_Target_IsHP::Execute(	ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	if( pTarget )	
		return CompareHP( pTarget );
	else			
		return false;
}

std::string ioAIExec_Target_IsHP::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "AI_EXEC_TARGET_IsHP {m_fRate=%f,m_Compare=%d}", m_fRate, m_Compare );
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Target_IsCurrentState::ioAIExec_Target_IsCurrentState(AIExecOperatorType op, const std::string& args)
	:	ioAIIsCurrentState( AI_EXEC_TARGET_IsCurrentState, op, args ), 
		m_Check(AI_CS_CHECK_NORMAL), m_Compare( AI_CMP_ONEOF ), m_nDuration(0)
{
}

void ioAIExec_Target_IsCurrentState::SetArgs( const std::string& args )
{
}

bool ioAIExec_Target_IsCurrentState::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	if( pTarget )	return CompareState( pTarget, pCAI );
	else			return false;
}

std::string ioAIExec_Target_IsCurrentState::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Target_IsCurrentState {%d", m_vCharState.size()  );
	retStr = buff;

	for( std::vector<CharState>::iterator it = m_vCharState.begin(); it != m_vCharState.end(); it++ )
	{
		sprintf( buff, "%d ", *it );
		retStr += buff;
	}

	return retStr+"}";
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Target_IsTargetValid::ioAIExec_Target_IsTargetValid(AIExecOperatorType op)		
	: ioAIBaseExecElem( AI_EXEC_TARGET_IsTargetValid, op )
{
}

bool ioAIExec_Target_IsTargetValid::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = ( pTarget != (ioBaseChar*)0 );
	
	return m_bLastResult;
}


std::string ioAIExec_Target_IsTargetValid::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Target_IsTargetValid }");
	retStr = buff;

	return retStr;
}

///////////////////////////////////////////////////////////////////////
ioAIExec_Target_IsDistance::ioAIExec_Target_IsDistance(AIExecOperatorType op, const std::string& args)
	: ioAIBaseExecElem( AI_EXEC_TARGET_IsDistance, op )
{
	SetArgs( args );
}

void ioAIExec_Target_IsDistance::SetArgs( const std::string& args )
{
	std::regex	reg(	g_AISymbolTable.GetSymbolString("rangeCompare")	+ "\\{" +
						g_AISymbolTable.GetSymbolString("floatRange")	+ "\\}" );
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		m_Compare = g_AISymbolTable.GetCompareType( sm[1] );
		m_fMin = ConvertToFloat( std::string(sm[2])+std::string(sm[3]) );
		m_fMax = ConvertToFloat( std::string(sm[4])+std::string(sm[5]) );
		if( m_fMin > m_fMax )
		{
			LOG.PrintTimeAndLog( 0, "Target_IsDistanceInRange : 정의 된 Min(%f)값이 Max(%f)값 보다 큽니다.", m_fMin, m_fMax );
			m_fMin = m_fMax = 0.0f;
		}
	}
	else	LOG.PrintTimeAndLog( 0, "Target_IsDistanceInRange : 주어진 arguement가 형식에 맞지 않습니다. : %s", args.c_str() );

}

bool ioAIExec_Target_IsDistance::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	m_bLastResult = false;
	if( pTarget != (ioBaseChar*)0 )
	{
		float	fDistSq = pCAI->GetDistance( pTarget );
	
		switch( m_Compare )
		{
		case AI_CMP_BETWEEN:	m_bLastResult = ((m_fMin*m_fMin) <= fDistSq &&  (m_fMax*m_fMax) >= fDistSq );	break;
		case AI_CMP_NOTIN:		m_bLastResult = ((m_fMin*m_fMin) >  fDistSq &&  (m_fMax*m_fMax) <  fDistSq );	break;
		}
	}
	else
		LOG.PrintTimeAndLog( 0, "ioAIExec_Target_IsDistance : Target[%x] is null",
								pTarget );

	return m_bLastResult;
}

std::string ioAIExec_Target_IsDistance::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIExec_Target_IsDistance {m_fMin=%f,m_fMax=%f}", m_fMin, m_fMax );
	retStr = buff;

	return retStr;
}

