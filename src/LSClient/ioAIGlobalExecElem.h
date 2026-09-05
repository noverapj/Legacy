#pragma once

#include "ioAIDatabase.h"
#include "ioAIBaseExecElem.h"

class	ioAIExec_Global_GetTarget : public ioAIBaseExecElem 
{
public:
	ioAIExec_Global_GetTarget( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Global_GetTarget() {}

	enum	TargetingType
	{
		AI_TARGETING_ERROR,
		AI_TARGETING_CLOSEST,
		AI_TARGETING_FIXED,
		AI_TARGETING_MOSTDAMAGE,
		AI_TARGETING_MOSTAGGRO,
		AI_TARGETING_MOSTAGGRO_RANDOM_CLOSE,
		AI_TARGETING_RANDOM,
	};

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	TargetingType	m_Targeting;
};

//------------------------------------------------------------------------
class	ioAIExec_Global_GetTargetByHP : public ioAIBaseExecElem 
{
public:
	ioAIExec_Global_GetTargetByHP( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Global_GetTargetByHP() {}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

protected:
	float			m_fRate;
	AICompareType	m_Compare;
};

//------------------------------------------------------------------------
class	ioAIExec_Global_IsTarget : public ioAIBaseExecElem 
{
public:
	ioAIExec_Global_IsTarget( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Global_IsTarget() {}

	enum	TargetingType
	{
		AI_TARGETING_ERROR,
		AI_TARGETING_CLOSEST,
		AI_TARGETING_FIXED,
		AI_TARGETING_MOSTDAMAGE,
		AI_TARGETING_MOSTAGGRO,
		AI_TARGETING_MOSTAGGRO_RANDOM_CLOSE,
		AI_TARGETING_RANDOM,
	};

	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	TargetingType	m_Targeting;
};

//------------------------------------------------------------------------
class	ioAIExec_Global_Counter : public ioAIBaseExecElem 
{
public:
	ioAIExec_Global_Counter( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Global_Counter() {}

	enum	CounterActionType
	{
		AI_COUNTER_ERROR,
		AI_COUNTER_INCREASE,
		AI_COUNTER_DECREASE,
		AI_COUNTER_SETTO,
		AI_COUNTER_ISBIGGER,
		AI_COUNTER_ISSMALLER,
		AI_COUNTER_ISEQUAL,
	};

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	std::string			m_szCounterName;
	CounterActionType	m_CounterType;
	int					m_nStep;
};