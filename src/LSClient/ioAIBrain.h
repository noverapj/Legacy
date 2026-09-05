#pragma once

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include <map>
#include <vector>

#include "ioAIDefinitions.h"
#include "ioAIBehaviorTree.h"
#include "ioAIFSM.h"

#include <vector>

class	ioChampionAI;
class	ioAIBrain
{
public:
	enum	AIDebugType
	{
		AIDEBUG_NONE			= 0x0000,
		AIDEBUG_SHOW_COMMENT	= 0x0001,
		AIDEBUG_SHOW_NAME		= 0x0002,
		AIDEBUG_SHOW_BVT		= 0x0004,
		AIDEBUG_KEYSEQ			= 0x0008,
		AIDEBUG_IDLE			= 0x0010,
		AIDEBUG_TIMER			= 0x0020,
		AIDEBUG_MOVESEQ			= 0x0040,
		AIDEBUG_CHECK_STATE		= 0x0080,
	};

	ioAIBrain( ioXMLElement* node );
	~ioAIBrain();

	bool	BuildBrain( ioXMLElement* node );

	bool	DecideAndAct();
	bool	ProcessAI( ioChampionAI* pCAI );
	void	ResetCurrentBvT();

	// helper functions
	long	GetDebuggingMode()	{	return m_iCurDebug;	}
	bool	IsDebugging(AIDebugType type);

	ioBaseChar*	GetCurTarget()	{ return m_pTarget; }
	void	ChangeTarget( ioChampionAI* pCAI );

protected:
	ioAIStateMachine*	m_pFSM;
	ioBaseChar*			m_pTarget;
	long				m_iCurDebug;
};