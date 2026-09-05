

#include "stdafx.h"

#include "HackCheck.h"
#include "../ioINILoader/ioINILoader.h"

namespace HackCheck
{
	static DWORD m_dwSpeedHackLessTime;
	static DWORD m_dwSpeedHackOverTime;

	static int m_iSpeedHackLessCount;
	static int m_iSpeedHackOverCount;
	static int m_iSpeedHackLessOverCount;
	static int m_iSpeedHackTotalCount;

	struct QuizCondition
	{
		int m_iMaxAnswerChance;
		DWORD m_dwMacroCount;
		DWORD m_dwCheckTime;
		DWORD m_dwClientAnswerTime;
		DWORD m_dwServerAnswerTime;

		int m_iFirstMaxOperandSize;
		int m_iSecondMaxOperandSize;
		int m_iMaxOperatorType;
	};

	static QuizCondition m_Quiz[HT_MAX];

	void LoadHackCheckValues()
	{
		ioINILoader kLoader;
		kLoader.ReloadFile( "ls_config_game.ini" );

		kLoader.SetTitle( "HACK" );
		m_dwSpeedHackLessTime = kLoader.LoadInt( "check_min_time", 8000 );
		m_dwSpeedHackOverTime = kLoader.LoadInt( "check_max_time", 12000 );

		m_iSpeedHackLessCount = kLoader.LoadInt( "check_less_count", 3 );
		m_iSpeedHackOverCount = kLoader.LoadInt( "check_over_count", 5 );
		m_iSpeedHackLessOverCount = kLoader.LoadInt( "check_less_over_count", 8 );
		m_iSpeedHackTotalCount = kLoader.LoadInt( "check_total_count", 20 );

		kLoader.SetTitle( "SpeedHackQuiz" );
		m_Quiz[HT_SPEED].m_iMaxAnswerChance = max( 1, kLoader.LoadInt( "max_answer_chance", 1 ) );
		m_Quiz[HT_SPEED].m_dwMacroCount = kLoader.LoadInt( "macro_count", 2 );
		m_Quiz[HT_SPEED].m_dwCheckTime = kLoader.LoadInt( "check_time", 10000 );
		m_Quiz[HT_SPEED].m_dwClientAnswerTime = kLoader.LoadInt( "client_answer_time", 10000 );
		m_Quiz[HT_SPEED].m_dwServerAnswerTime = kLoader.LoadInt( "server_answer_time", 2000 );

		m_Quiz[HT_SPEED].m_iFirstMaxOperandSize  = max( 1, kLoader.LoadInt( "first_max_operand_size", 1 ) );
		m_Quiz[HT_SPEED].m_iSecondMaxOperandSize = max( 1, kLoader.LoadInt( "second_max_operand_size", 1 ) );
		m_Quiz[HT_SPEED].m_iMaxOperatorType = max( 1, kLoader.LoadInt( "max_operator_type", 2 ) );

		kLoader.SetTitle( "AbuseQuiz" );
		m_Quiz[HT_ABUSE].m_iMaxAnswerChance = max( 1, kLoader.LoadInt( "max_answer_chance", 1 ) );
		m_Quiz[HT_ABUSE].m_dwMacroCount = kLoader.LoadInt( "macro_count", 2 );
		m_Quiz[HT_ABUSE].m_dwCheckTime = kLoader.LoadInt( "check_time", 10000 );
		m_Quiz[HT_ABUSE].m_dwClientAnswerTime = kLoader.LoadInt( "client_answer_time", 10000 );
		m_Quiz[HT_ABUSE].m_dwServerAnswerTime = kLoader.LoadInt( "server_answer_time", 2000 );

		m_Quiz[HT_ABUSE].m_iFirstMaxOperandSize  = max( 1, kLoader.LoadInt( "first_max_operand_size", 1 ) );
		m_Quiz[HT_ABUSE].m_iSecondMaxOperandSize = max( 1, kLoader.LoadInt( "second_max_operand_size", 1 ) );
		m_Quiz[HT_ABUSE].m_iMaxOperatorType = max( 1, kLoader.LoadInt( "max_operator_type", 2 ) );

		kLoader.SetTitle( "MacroQuiz" );
		m_Quiz[HT_MACRO].m_iMaxAnswerChance = max( 1, kLoader.LoadInt( "max_answer_chance", 1 ) );
		m_Quiz[HT_MACRO].m_dwMacroCount = kLoader.LoadInt( "macro_count", 2 );
		m_Quiz[HT_MACRO].m_dwCheckTime = kLoader.LoadInt( "check_time", 10000 );
		m_Quiz[HT_MACRO].m_dwClientAnswerTime = kLoader.LoadInt( "client_answer_time", 10000 );
		m_Quiz[HT_MACRO].m_dwServerAnswerTime = kLoader.LoadInt( "server_answer_time", 2000 );

		m_Quiz[HT_MACRO].m_iFirstMaxOperandSize  = max( 1, kLoader.LoadInt( "first_max_operand_size", 1 ) );
		m_Quiz[HT_MACRO].m_iSecondMaxOperandSize = max( 1, kLoader.LoadInt( "second_max_operand_size", 1 ) );
		m_Quiz[HT_MACRO].m_iMaxOperatorType = max( 1, kLoader.LoadInt( "max_operator_type", 2 ) );
	}

	DWORD SH_LessCheckTime() { return m_dwSpeedHackLessTime; }
	DWORD SH_OverCheckTime() { return m_dwSpeedHackOverTime; }

	int SH_LessCount() { return m_iSpeedHackLessCount; }
	int SH_OverCount() { return m_iSpeedHackOverCount; }
	int SH_LessOverCount() { return m_iSpeedHackLessOverCount; }
	int SH_TotalCount() { return m_iSpeedHackTotalCount; }

	int MaxAnswerChance( HackType eType )
	{
		return m_Quiz[eType].m_iMaxAnswerChance; 
	}
	
	DWORD MacroCount( HackType eType )
	{
		return m_Quiz[eType].m_dwMacroCount;
	}

	DWORD CheckTime( HackType eType )
	{
		return m_Quiz[eType].m_dwCheckTime;
	}

	DWORD ClientAnswerTime( HackType eType )
	{
		return m_Quiz[eType].m_dwClientAnswerTime;
	}

	DWORD ServerAnswerTime( HackType eType )
	{
		return m_Quiz[eType].m_dwServerAnswerTime;
	}

	int FirstMaxOperandSize( HackType eType )
	{
		return m_Quiz[eType].m_iFirstMaxOperandSize;
	}

	int SecondMaxOperandSize( HackType eType )
	{
		return m_Quiz[eType].m_iSecondMaxOperandSize;
	}

	int MaxOperatorType( HackType eType )
	{
		return m_Quiz[eType].m_iMaxOperatorType; 
	}

	CheckProblem GenerateProblem( HackType eType )
	{
		CheckProblem kProblem;
		const QuizCondition &rkQuiz = m_Quiz[eType];

		// FirstOperand
		int iMaxValue = 1;
		for( int i=0 ; i<rkQuiz.m_iFirstMaxOperandSize ; i++ )
		{
			iMaxValue *= 10;
		}
		iMaxValue = max( 2, iMaxValue ) - 1;
		kProblem.m_iFirstOperand  = rand() % ( iMaxValue ) + 1;

		if( rkQuiz.m_iFirstMaxOperandSize > 1 )
		{
			iMaxValue = 1;
			for( int i=0 ; i < (rkQuiz.m_iFirstMaxOperandSize-1) ; i++ )
			{
				iMaxValue *= 10;
			}

			if( kProblem.m_iFirstOperand < iMaxValue )
				kProblem.m_iFirstOperand += iMaxValue;
		}

		// SecondOperand
		iMaxValue = 1;
		for( int i=0 ; i<rkQuiz.m_iSecondMaxOperandSize ; i++ )
		{
			iMaxValue *= 10;
		}
		iMaxValue = max( 2, iMaxValue ) - 1;
		kProblem.m_iSecondOperand = rand() % ( iMaxValue ) + 1;

		if( rkQuiz.m_iSecondMaxOperandSize > 1 )
		{
			iMaxValue = 1;
			for( int i=0 ; i < (rkQuiz.m_iSecondMaxOperandSize-1) ; i++ )
			{
				iMaxValue *= 10;
			}

			if( kProblem.m_iSecondOperand < iMaxValue )
				kProblem.m_iSecondOperand += iMaxValue;
		}

		// Operator
		kProblem.m_Operator = (QuizOperator)( rand() % rkQuiz.m_iMaxOperatorType );

		return kProblem;
	}

	int SolveProblem( const CheckProblem &rkProblem )
	{
		int iResult = 0;

		switch( rkProblem.m_Operator )
		{
		case QUIZ_ADD:
			iResult = rkProblem.m_iFirstOperand + rkProblem.m_iSecondOperand;
			break;
		case QUIZ_MINUS:
			iResult = rkProblem.m_iFirstOperand - rkProblem.m_iSecondOperand;
			break;
		case QUIZ_MULTIPLY:
			iResult = rkProblem.m_iFirstOperand * rkProblem.m_iSecondOperand;
			break;
		case QUIZ_DIVIDE:
			iResult = rkProblem.m_iFirstOperand / rkProblem.m_iSecondOperand;
			break;
		}

		return iResult;
	}
}
