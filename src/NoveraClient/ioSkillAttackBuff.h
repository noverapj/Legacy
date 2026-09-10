#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSkillAttackBuff : public ioBuff
{
	int m_nCallWeapon;
	ioHashString m_stCreatorAddBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual void ApplyExtraBuffInfo(  SP2Packet &rkPacket  );

protected:
	void CreateWeapon( bool bSend );

public:
	ioSkillAttackBuff();
	ioSkillAttackBuff( const ioSkillAttackBuff &rhs );
	virtual ~ioSkillAttackBuff();
};


inline ioSkillAttackBuff* ToSkillAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_SKILL_ATTACK_BUFF )
		return NULL;

	return dynamic_cast< ioSkillAttackBuff* >( pBuff );
}


