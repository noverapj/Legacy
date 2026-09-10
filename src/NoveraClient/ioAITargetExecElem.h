#pragma once
#include <string>

#include "ioAIDatabase.h"
#include "ioAIBaseExecElem.h"

//------------------------------------------------------------------------
class	ioAIExec_Target_IsHP : public ioAIIsHP
{
public:
	ioAIExec_Target_IsHP( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Target_IsHP()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();
};

//------------------------------------------------------------------------
class	ioAIExec_Target_IsCurrentState : public ioAIIsCurrentState
{
public:
	ioAIExec_Target_IsCurrentState(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Target_IsCurrentState()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	CharStateCheckType		m_Check;
	int						m_nDuration;
	AICompareType			m_Compare;
	std::vector<CharState>	m_vCharState;
};

//------------------------------------------------------------------------
class	ioAIExec_Target_IsTargetValid : public ioAIBaseExecElem 
{
public:
	ioAIExec_Target_IsTargetValid(AIExecOperatorType op);
	virtual ~ioAIExec_Target_IsTargetValid()	{}

	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();
};

//------------------------------------------------------------------------
class	ioAIExec_Target_IsDistance : public ioAIBaseExecElem 
{
public:
	ioAIExec_Target_IsDistance(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Target_IsDistance()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	float			m_fMin, m_fMax;
	AICompareType	m_Compare;
};
