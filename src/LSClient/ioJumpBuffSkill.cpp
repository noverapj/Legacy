
#include "stdafx.h"
#include "ioJumpBuffSkill.h"

ioJumpBuffSkill::ioJumpBuffSkill()
{
}

ioJumpBuffSkill::ioJumpBuffSkill( const ioJumpBuffSkill &rhs )
	: ioBuffSkill( rhs ),
	m_fEndJumpAmt( rhs.m_fEndJumpAmt )
{
}

ioJumpBuffSkill::~ioJumpBuffSkill()
{
}

void ioJumpBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	m_fEndJumpAmt = rkLoader.LoadFloat_e( "end_jump_amt", 1000.0f );
}

ioSkill* ioJumpBuffSkill::Clone()
{
	return new ioJumpBuffSkill( *this );
}

void ioJumpBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioBuffSkill::BuffSkillSet( pChar );

	if( m_SkillUseType == SUT_JUMP )
		pChar->SetSkillNextJumpAmt( m_fEndJumpAmt );
}