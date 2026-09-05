#pragma once

#include <string>

#include "ioAIDefinitions.h"

#include "ioAIMyselfExecElem.h"
#include "ioAITargetExecElem.h"
#include "ioAIGlobalExecElem.h"

class	ioAIExecUnit
{
public:
	ioAIExecUnit( AIExecOperatorType op, AIExecVariableType var, AIExecMethodType m, std::string args );
	~ioAIExecUnit();

	bool	IsHolding()	{	return m_bHold;	}
	void	SetHolding( bool holding )	{ m_bHold = holding; }

	bool	GetLastResult();
	std::string	GetDebugString();
	
//	bool	ApplyOperation( bool initialResult );

	virtual bool	Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	//virtual bool	ApplyOperation( bool initialResult );

	ioAIBaseExecElem*	GetElem()	{	return m_pElem;	}

private:
	ioAIBaseExecElem*	m_pElem;
	bool				m_bHold;
};
