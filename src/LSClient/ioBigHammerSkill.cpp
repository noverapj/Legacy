

#include "stdafx.h"

#include "ioBigHammerSkill.h"

#include "ioBaseChar.h"

ioBigHammerSkill::ioBigHammerSkill()
: m_vScale( FLOAT1, FLOAT1, FLOAT1 )
{
}

ioBigHammerSkill::ioBigHammerSkill( const ioBigHammerSkill &rhs )
: ioAttackSkill( rhs ),
  m_vScale( rhs.m_vScale )
{
}

ioBigHammerSkill::~ioBigHammerSkill()
{
}

void ioBigHammerSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_vScale.x = rkLoader.LoadFloat_e( "scale_x", FLOAT1 );
	m_vScale.y = rkLoader.LoadFloat_e( "scale_y", FLOAT1 );
	m_vScale.z = rkLoader.LoadFloat_e( "scale_z", FLOAT1 );
}

ioSkill* ioBigHammerSkill::Clone()
{
	return new ioBigHammerSkill( *this );
}

bool ioBigHammerSkill::OnSkillStart( ioBaseChar *pChar )
{
	bool bStart = ioAttackSkill::OnSkillStart( pChar );
	if( bStart )
	{
		ioBiped *pBiped = pChar->GetGroup()->GetBiped( "Bip01 R Hand" );
		if( pBiped )
		{
			pBiped->Scale( m_vScale );
		}
	}

	return bStart;
}

void ioBigHammerSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioBiped *pBiped = pChar->GetGroup()->GetBiped( "Bip01 R Hand" );
	if( pBiped )
	{
		D3DXVECTOR3 vInvScale( FLOAT1/m_vScale.x, FLOAT1/m_vScale.y, FLOAT1/m_vScale.z );
		pBiped->Scale( vInvScale );
	}

	ioAttackSkill::OnSkillEnd( pChar );
}

