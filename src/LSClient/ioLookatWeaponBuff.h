#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioLookatWeaponBuff : public ioBuff
{
protected:
	ioHashStringVec m_RemoveBuffList;

	bool m_bLookWeapon;
	bool m_bLookAreaWeapon;

	bool m_bLookInit;

	float m_fMoveSpeed;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );	
	
protected:
	void LookAtWeaponPos( D3DXVECTOR3 vPos );

public:
	ioLookatWeaponBuff();
	ioLookatWeaponBuff( const ioLookatWeaponBuff &rhs );
	virtual ~ioLookatWeaponBuff();
};

