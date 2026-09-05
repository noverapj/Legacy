

#include "stdafx.h"

#include "ioWoundPassiveSkill.h"

#include "ioBaseChar.h"

ioWoundPassiveSkill::ioWoundPassiveSkill()
{
}

ioWoundPassiveSkill::ioWoundPassiveSkill( const ioWoundPassiveSkill &rhs )
: ioPassiveSkill( rhs )
{
}

ioWoundPassiveSkill::~ioWoundPassiveSkill()
{
}

ioSkill* ioWoundPassiveSkill::Clone()
{
	return new ioWoundPassiveSkill( *this );
}

void ioWoundPassiveSkill::CheckWoundProcess( ioBaseChar *pOwner, ioBaseChar *pAttacker )
{
	ioPassiveSkill::CheckWoundProcess( pOwner, pAttacker );

	if( !pAttacker ) return;

	int iState = pAttacker->GetState();

	switch( iState )
	{
	case CS_ESCAPE_PRISONER:
	case CS_BLOW_WOUNDED:
	case CS_BOUND_BLOW_WOUNDED:
	case CS_BLOW_EXTEND_WOUND:
	case CS_ESCAPE_CATCH:
	case CS_CREATEMOTION:
	case CS_COIN_REBIRTH:
	case CS_USING_SKILL:
	case CS_STRUCT_PUSH:
	case CS_CHANGE_WAIT:
	case CS_STARTMOTION:
	case CS_STOP_MOTION:
	case CS_PROTECTION:
	case CS_WARP_STATE:
	case CS_CLOSEORDER:
	case CS_ETC_STATE:
	case CS_ENDMOTION:
	case CS_PICK_ITEM:
	case CS_CREEPING:
	case CS_WOUNDED:
	case CS_LOADING:
	case CS_FISHING:
	case CS_FROZEN:
	case CS_READY:
	case CS_DRINK:
	case CS_FALL:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_DIE:
	case CS_FLY:
	case CS_EXCAVATING:
	case CS_CONSTRUCT_HOUSE:
	case CS_ROULETTE:
	case CS_CHANGE_GANGSI:
	case CS_GRADEUP_MOTION:
	case CS_ICE_STATE:
	case CS_PANIC_MOVE:
		return;
	}

	AddTargetBuff( pAttacker );
}