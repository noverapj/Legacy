#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWeaponAttachBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;

	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

	ioHashStringVec m_vMapCollisionBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void SetAction();
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();
	bool CheckWeaponAttach();

public:
	ioWeaponAttachBuff();
	ioWeaponAttachBuff( const ioWeaponAttachBuff &rhs );
	virtual ~ioWeaponAttachBuff();
};


inline ioWeaponAttachBuff* ToWeaponAttachBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WEAPON_ATTACH )
		return NULL;

	return dynamic_cast< ioWeaponAttachBuff* >( pBuff );
}


