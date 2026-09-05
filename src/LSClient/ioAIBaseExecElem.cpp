#include "stdafx.h"

#include <regex>

#include "ioAIBaseExecElem.h"
#include "ioAISymbolTable.h"
#include "ioBaseChar.h"
#include "ioChampionAI.h"

#include "stringext.h"

ioAIBaseExecElem::ioAIBaseExecElem() 
	: m_Type( AI_EXEC_NONE ), m_Operation( AI_BvT_AOT_NOOP )
{
}

ioAIBaseExecElem::ioAIBaseExecElem( AIExecElemType type, AIExecOperatorType op ) 
	: m_Type( type ) , m_Operation( op )
{
}

bool ioAIBaseExecElem::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI ) 
{ 
	LOG.PrintTimeAndLog( 0, "ioAIBaseExecElem : This class should not called directly from script." );
	return false;
}

int ioAIBaseExecElem::ConvertToInt( const std::string& arg )
{
	return ::atoi( arg.c_str() );
}

float ioAIBaseExecElem::ConvertToFloat( const std::string& arg )
{ 
	return ::atof( arg.c_str() );
}

CharState	ioAIBaseExecElem::ConvertToCharState( const std::string& arg )
{
	return g_AISymbolTable.GetCharStateType( arg );
}

std::string	ioAIBaseExecElem::GetDebugString()
{
	return "BaseExecElem{}";
}

bool	ioAIBaseExecElem::ApplyOperation( bool bInitialResult, bool store )
{
	switch( m_Operation )
	{
	default:
	case AI_BvT_AOT_NOOP:		return bInitialResult;
	case AI_BvT_AOT_NOT:		return !bInitialResult;
	}
}

////////////////////////////////////////////////////////////////////
ioAIIsHP::ioAIIsHP( AIExecElemType type, AIExecOperatorType op, const std::string& args )
	: ioAIBaseExecElem( type, op )
{
	SetArgs( args );
}

void	ioAIIsHP::SetArgs( const std::string& args )
{
	//UJ 140120, 보안강화: 인니 AI 치트 방지
	//std::regex	reg( g_AISymbolTable.GetSymbolString("isHP") );
	static char szEncIsHP[5] = {'i','s','H','P',0};
	std::regex	reg( g_AISymbolTable.GetSymbolString(szEncIsHP) );	
	std::smatch sm;

	if( std::regex_match( args, sm, reg ) )
	{
		m_Compare = g_AISymbolTable.GetCompareType( sm[1] );
		if( m_Compare != AI_CMP_ERROR )
		{
			if( sm[3] == "%" )		m_fRate = ConvertToFloat( sm[2] ) / FLOAT100;
			else if( sm[3] == "" )	m_fRate = ConvertToFloat( sm[2] );
			else					m_fRate = ConvertToFloat( std::string(sm[2]) + std::string(sm[3]) );
		}
		else m_fRate = 0.0f;
	}
	else m_fRate = 0.0f;
}

bool ioAIIsHP::CompareHP( ioBaseChar* character )
{
	m_bLastResult = false;

	if( character != (ioBaseChar*)0 )
	{
		float	fCurHP = character->GetHP().m_fCurValue;
		float	fMaxHP = character->GetHP().m_fMaxValue;

		switch( m_Compare )
		{
		case AI_CMP_BIGGER:		m_bLastResult = ( fCurHP > fMaxHP * m_fRate );	break;
		case AI_CMP_SMALLER:	m_bLastResult = ( fCurHP < fMaxHP * m_fRate );	break;	
		case AI_CMP_EQUAL:		m_bLastResult = ( fCurHP == fMaxHP * m_fRate );	break;
		}
	}

	return m_bLastResult;
}

bool ioAIIsHP::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	LOG.PrintTimeAndLog( 0, "ioAIIsHP : This class should not called directly from script." );

	return false;
}

std::string ioAIIsHP::GetDebugString()
{
	char buff[MAX_PATH];
	std::string	retStr;

	sprintf(buff, "ioAIIsHP {m_fRate=%f,m_Compare=%d}", m_fRate, m_Compare );
	retStr = buff;

	return retStr;
}

////////////////////////////////////////////////////////////////////
ioAIIsCurrentState::ioAIIsCurrentState( AIExecElemType type, AIExecOperatorType op, const std::string& args )
	: ioAIBaseExecElem( type, op )
{
	SetArgs( args );
}

void	ioAIIsCurrentState::SetArgs( const std::string& args )
{
	std::string	temp = g_AISymbolTable.GetSymbolString("InState")	+ "[\\s]*,[\\s]*" +
						g_AISymbolTable.GetSymbolString("int") ;
	std::regex	reg1( temp );
	std::smatch sm1;

	if( std::regex_match( args, sm1, reg1 ) )
	{
		m_Check = (CharStateCheckType)g_AISymbolTable.GetSymbolValue( "CharStateCheckType", std::string( sm1[1] ) );

		m_nDuration = ConvertToInt( sm1[2] );
	}
	else
	{
		std::regex	reg2( g_AISymbolTable.GetSymbolString("curStateEx") );
		std::smatch sm2;

		if( std::regex_match( args, sm2, reg2 ) )
		{
			// general case
			m_Check		= AI_CS_CHECK_NORMAL;
			m_Compare	= g_AISymbolTable.GetCompareType( sm2[1] );
			if( m_Compare != AI_CMP_ERROR )
			{
				std::vector<std::string>	argv = explode( std::string(sm2[2]), ",\t " );

				for( std::vector<std::string>::iterator it = argv.begin(); it != argv.end(); it++ )
				{
					m_vCharState.push_back( g_AISymbolTable.GetCharStateType( *it ) );
				}
			}
		}
		else
		{
			std::regex	reg3( g_AISymbolTable.GetSymbolString("curState") );
			std::smatch sm3;

			if( std::regex_match( args, sm3, reg3 ) )
			{
				m_Compare = AI_CMP_ONEOF;
				m_vCharState.push_back( g_AISymbolTable.GetCharStateType( sm3[1] ) );
			}
		}
	}
}

bool ioAIIsCurrentState::CompareState( ioBaseChar* character, ioChampionAI* pCAI )
{
	m_bLastResult	= false;

	if( character != (ioBaseChar*)0 )
	{
		CharState		TargetState = character->GetState();

		switch( m_Check )
		{
		case AI_CS_CHECK_IN_WOUND:
			m_bLastResult = character->IsCharacterWounded();
			if( pCAI->IsDebugging( ioAIBrain::AIDEBUG_CHECK_STATE ) )
				LOG.PrintTimeAndLog( 0, "Checking Tareget's IN_WOUND and current state is : %s ", character->GetStateText() );
			break;
		case AI_CS_CHECK_IN_DOWN:
			m_bLastResult = character->IsApplyDownState( false );
			if( pCAI->IsDebugging( ioAIBrain::AIDEBUG_CHECK_STATE ) )
				LOG.PrintTimeAndLog( 0, "Checking Target's IN_DOWN and current state is : %s ", character->GetStateText() );
			break;
		case AI_CS_CHECK_NORMAL:
		default:
			for( std::vector<CharState>::iterator it = m_vCharState.begin(); it != m_vCharState.end(); it++ )
			{
				if( (*it) == TargetState )
				{
					m_bLastResult = true;
					break;
				}
			}
			break;
		}
	}
	else LOG.PrintTimeAndLog( 0, "No Target found!" );

	if( !pCAI->GetPollingTimer()->IsRunning() && m_nDuration > 0 && !m_bLastResult )
	{
		pCAI->SetPollingUnit( this );
		pCAI->GetPollingTimer()->StartTimer( m_nDuration );
	}

	return m_bLastResult;
}

bool ioAIIsCurrentState::Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI )
{
	LOG.PrintTimeAndLog( 0, "ioAIIsCurrentState : This class should not called directly from script." );

	return false;
}

std::string ioAIIsCurrentState::GetDebugString()
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

