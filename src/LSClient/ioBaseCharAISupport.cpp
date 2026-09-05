#include "stdafx.h"

#include "ioBaseChar.h"

SkillUseType	ioBaseChar::GetAvailableSkillUseType()
{
	// using AI : which type of skill can I use now?
	if( IsJumpingState() )					return SUT_JUMP;
	else if( IsApplyDownState( false ) )	return SUT_DOWN;
	else									return SUT_NORMAL;
}

bool	ioBaseChar::IsCharacterWounded()
{
	int	x = (int)m_CharState;

	return ( m_CharState == CS_WOUNDED ||
			 m_CharState == CS_BLOW_WOUNDED ||
			 m_CharState == CS_BOUND_BLOW_WOUNDED ||
			 m_CharState == CS_BLOW_EXTEND_WOUND );
}
