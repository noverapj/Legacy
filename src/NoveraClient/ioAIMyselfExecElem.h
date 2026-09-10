#pragma once
#include <string>

#include "ioAIBaseExecElem.h"

//------------------------------------------------------------------------
class	ioAIExec_Myself_IsHP : public ioAIIsHP 
{
public:
	ioAIExec_Myself_IsHP( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Myself_IsHP()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	AICompareType	m_Compare;
	float			m_fRate;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_IsCurrentState : public ioAIIsCurrentState
{
public:
	ioAIExec_Myself_IsCurrentState(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_IsCurrentState()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

};

//------------------------------------------------------------------------
class	ioAIExec_Myself_StopAllActions : public ioAIBaseExecElem
{
public:
	ioAIExec_Myself_StopAllActions(AIExecOperatorType op);
	virtual ~ioAIExec_Myself_StopAllActions()	{}

	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );

	virtual std::string GetDebugString();
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_ShowChat : public ioAIBaseExecElem
{
public:
	ioAIExec_Myself_ShowChat(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_ShowChat()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	std::string		m_szChatMsg;
	int				m_iDuration;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_DoKeySeq : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_DoKeySeq(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_DoKeySeq()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	std::string		m_szKeySeqName;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_Move : public ioAIBaseExecElem 
{
public:
	enum MoveDirType
	{
		AI_MOVE_STRICT_CENTER = -1,
		AI_MOVE_STRICT_DOWN,
		AI_MOVE_STRICT_DN_LT,
		AI_MOVE_STRICT_LEFT,
		AI_MOVE_STRICT_UP_LT,
		AI_MOVE_STRICT_UP,
		AI_MOVE_STRICT_UP_RT,
		AI_MOVE_STRICT_RIGHT,
		AI_MOVE_STRICT_DN_RT,

		AI_MOVE_TOWARD_TARGET,
		AI_MOVE_TURNBACK_TARGET,
		AI_MOVE_TURNLEFT_TARGET,
		AI_MOVE_TURNRIGHT_TARGET,
		AI_MOVE_START_POS,
	};

	ioAIExec_Myself_Move( AIExecOperatorType op, const std::string& args );
	virtual ~ioAIExec_Myself_Move()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	MoveDirType		m_DirType;
	std::string		m_szMoveSeqName;
	unsigned int	m_nDuration;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_Turn : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_Turn(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_Turn()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	ioAIExec_Myself_Move::MoveDirType		m_DirType;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_Idle : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_Idle(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_Idle()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	int	m_nIdleTime;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_IsSkillAvailable : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_IsSkillAvailable(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_IsSkillAvailable()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	EquipSlot		m_EquipSlot;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_IsCombo : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_IsCombo(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_IsCombo()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	int		m_iAttackCombo;
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_CanDoCombo : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_CanDoCombo(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_CanDoCombo()	{}

	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();
};

//------------------------------------------------------------------------
class	ioAIExec_Myself_WaitForCombo : public ioAIBaseExecElem 
{
public:
	ioAIExec_Myself_WaitForCombo(AIExecOperatorType op, const std::string& args);
	virtual ~ioAIExec_Myself_WaitForCombo()	{}

	virtual void SetArgs( const std::string& args );
	virtual bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

private:
	unsigned int m_nFrontMargin, m_nTailMargin;
};

