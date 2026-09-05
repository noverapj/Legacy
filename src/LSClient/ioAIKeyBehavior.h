#pragma once

#include "ioAIDefinitions.h"

class ioXMLElement;
class ioAIKeyBehavior
{
public:
	ioAIKeyBehavior() 
		: m_inputType(AI_BvT_KIT_MOVE_CENTER), m_stateType(AI_BvT_KST_NONE), m_duration(0) 
	{
	}
	ioAIKeyBehavior( const ioAIKeyBehavior& k ) 
		: m_inputType( k.m_inputType ), m_stateType( k.m_stateType ), m_duration( k.m_duration ) 
	{
	}
	ioAIKeyBehavior( ioXMLElement* node );
	~ioAIKeyBehavior();

	ioAIKeyBehavior& operator=( const ioAIKeyBehavior& k )
	{
		m_inputType	= k.m_inputType;
		m_stateType	= k.m_stateType;
		m_duration	= k.m_duration;
		
		return *this;
	}

	bool	DoKeyAction( ioBaseChar* target, ioNpcChar* myself, ioChampionAI* pCAI );

	void	DecreaseDuration( unsigned int dec )	{	m_duration -= dec;	}

public:
	AIKeyInputType		m_inputType;
	AIKeyStateType		m_stateType;
	unsigned int		m_duration;
};

