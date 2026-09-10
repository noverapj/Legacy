#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAttackCountBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bUseWeaponEquipMesh;

	//해당 버프가 없으면 생성 있으면 죽으면서 CallEndBuff생성
	ioHashString m_stCheckBuff;
	ioHashString m_stCallEndBuff;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	void AttackWound( ioBaseChar* pTarget, ioWeapon* pWeapon );

private:
	void CallEndBuff( ioBaseChar* pTarget, const ioHashString& stCreator );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

private:
	void SendPacketBuff( ioBaseChar* pTarget, ioBaseChar* pCreator, const ioHashString& stBuffName );

public:
	ioAttackCountBuff();
	ioAttackCountBuff( const ioAttackCountBuff &rhs );
	virtual ~ioAttackCountBuff();
};


inline ioAttackCountBuff* ToAttackCountBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACK_COUNT_BUFF )
		return NULL;

	return dynamic_cast< ioAttackCountBuff* >( pBuff );
}


