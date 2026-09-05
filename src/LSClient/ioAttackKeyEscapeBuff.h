#pragma once
#include "iobuff.h"

class ioINILoader;
class ioBaseChar;

class ioAttackKeyEscapeBuff : public ioBuff
{
protected:
	DWORD m_dwTotalAttackKeyCount;
	DWORD m_dwReduceTime;
	float m_fDamage;

	DWORD m_dwCurrentAttackKeyCount;

	ioHashString m_DummyCharName;
	ioHashStringVec m_vDestroyDummyList;

	ioHashString m_szDummyActionAni;
	float m_fDummyActionAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void ProcessKeyInput();
	void EscapeKeyAction( ioBaseChar* pOwner );

public:
	ioAttackKeyEscapeBuff(void);
	ioAttackKeyEscapeBuff( const ioAttackKeyEscapeBuff &rhs );
	virtual ~ioAttackKeyEscapeBuff(void);
};

inline ioAttackKeyEscapeBuff* ToAttackKeyEscapeBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACK_KEY_ESCAPE_BUFF )
		return NULL;

	return dynamic_cast< ioAttackKeyEscapeBuff* >( pBuff );
}