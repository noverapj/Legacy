

#ifndef _HackCheck_h_
#define _HackCheck_h_

namespace HackCheck
{
	enum HackType
	{
		HT_SPEED,
		HT_ABUSE,
		HT_MACRO,
		HT_MAX,
	};

	enum QuizOperator
	{
		QUIZ_ADD,
		QUIZ_MINUS,
		QUIZ_MULTIPLY,
		QUIZ_DIVIDE,
	};

	struct CheckProblem
	{
		int m_iFirstOperand;
		int m_iSecondOperand;
		QuizOperator m_Operator;
	};

//----- COMMON ------------------------
	void LoadHackCheckValues();
	int  SolveProblem( const CheckProblem &rkProblem );

//----- SPEED HACK ------------------------
	DWORD SH_LessCheckTime();
	DWORD SH_OverCheckTime();
	int SH_LessCount();
	int SH_OverCount();
	int SH_LessOverCount();
	int SH_TotalCount();

//----- TYPE ------------------------
	int MaxAnswerChance( HackType eType );
	DWORD MacroCount( HackType eType );
	DWORD CheckTime( HackType eType );
	DWORD ClientAnswerTime( HackType eType );
	DWORD ServerAnswerTime( HackType eType );

	int FirstMaxOperandSize( HackType eType );
	int SecondMaxOperandSize( HackType eType );
	int MaxOperatorType( HackType eType );
	CheckProblem GenerateProblem( HackType eType );
};

#endif
