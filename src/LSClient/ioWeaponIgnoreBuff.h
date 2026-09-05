#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWeaponIgnoreBuff : public ioBuff
{
protected:
	float m_fOutLineR;
	float m_fOutLineG;
	float m_fOutLineB;
	float m_fOutLineAlpha;
	float m_fOutLineThickness;
	bool m_bEnableOutLine;
	bool m_bDisableDownState;

	DWORDVec m_vWeaponList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void SetExtraDurationByGrowth( float fDuration );

	bool CheckWeaponIndex( DWORD dwWeaponIndex );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );
	void CheckOwnerStateCheck();

public:
	ioWeaponIgnoreBuff();
	ioWeaponIgnoreBuff( const ioWeaponIgnoreBuff &rhs );
	virtual ~ioWeaponIgnoreBuff();

};

inline ioWeaponIgnoreBuff* ToWeaponIgnoreBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WEAPON_IGNORE_BUFF )
		return NULL;

	return dynamic_cast< ioWeaponIgnoreBuff* >( pBuff );
}