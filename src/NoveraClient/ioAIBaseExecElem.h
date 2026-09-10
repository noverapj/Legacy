#pragma once

#include <string>
#include "GameEnumType.h"
#include "ioAIDefinitions.h"

class	ioBaseChar;
class	ioAI;
class	ioAIBaseExecElem
{
public:
	ioAIBaseExecElem();
	ioAIBaseExecElem( AIExecElemType type, AIExecOperatorType op );
	virtual ~ioAIBaseExecElem() {}

	AIExecElemType	GetType()			{	return m_Type;			}
	AIExecOperatorType	
					GetOperator()		{	return m_Operation;		}
	bool			GetLastResult()		{	return m_bLastResult;	}


	virtual	void SetArgs( const std::string& args ) {}			// no arg
	virtual	bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI ); // do not execute
	virtual std::string GetDebugString();

	// utility functions that will be used in various exec units
	// mainly converter
	int			ConvertToInt( const std::string& arg );
	float		ConvertToFloat( const std::string& arg );
	CharState	ConvertToCharState( const std::string& arg );
	std::string	LoadRegExString( const std::string& regname );

	bool		ApplyOperation( bool bInitialResult, bool store = false );

protected:
	bool				m_bLastResult;

private:
	AIExecOperatorType	m_Operation; 
	AIExecElemType		m_Type;
};

/////////////////////////////////////////////////////////////////////////
class	ioAIIsHP : public ioAIBaseExecElem
{
public:
	ioAIIsHP( AIExecElemType type, AIExecOperatorType op, const std::string& args );
	virtual ~ioAIIsHP() {}

	virtual	void SetArgs( const std::string& args );
	virtual	bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

	bool	CompareHP( ioBaseChar* character );
	
protected:
	float			m_fRate;
	AICompareType	m_Compare;
};

/////////////////////////////////////////////////////////////////////////
class	ioAIIsCurrentState : public ioAIBaseExecElem
{
public:
	ioAIIsCurrentState( AIExecElemType type, AIExecOperatorType op, const std::string& args );
	virtual ~ioAIIsCurrentState() {}

	virtual	void SetArgs( const std::string& args );
	virtual	bool Execute( ioBaseChar*& pTarget, ioChampionAI* pCAI );
	virtual std::string GetDebugString();

	bool	CompareState( ioBaseChar* character, ioChampionAI* pCAI );
	
protected:
	CharStateCheckType		m_Check;
	int						m_nDuration;
	AICompareType			m_Compare;
	std::vector<CharState>	m_vCharState;
};