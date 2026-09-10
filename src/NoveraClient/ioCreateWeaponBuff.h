#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateWeaponBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_DELAY,
		BS_FIRE,
	};

protected:
	BuffState m_BuffState;

	CEncrypt<bool> m_bVisibleOnlyEffect;
	bool m_bDisableDownState;

	CEncrypt<DWORD> m_dwFireGapTime;
	DWORD m_dwCheckFireTime;

	ioHashString m_szDelayEffect;
	DWORD m_dwDelayEffectID;
	ioHashString m_szFireEffect;

	DWORD m_dwCheckGavityTime;
	CEncrypt<DWORD> m_dwGravityTime; 
	CEncrypt<float> m_fGravityRate;

	WeaponInfo m_WeaponInfo;

	ioHashString m_szFireSound;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();
	void ProcessKeyInput();
	void ProcessState();
	void CreateWeapon();

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );
	bool IsHideState()	{ return m_bVisibleOnlyEffect; }

public:
	ioCreateWeaponBuff();
	ioCreateWeaponBuff( const ioCreateWeaponBuff &rhs );
	virtual ~ioCreateWeaponBuff();
};


inline ioCreateWeaponBuff* ToCreateWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_WEAPON )
		return NULL;

	return dynamic_cast< ioCreateWeaponBuff* >( pBuff );
}